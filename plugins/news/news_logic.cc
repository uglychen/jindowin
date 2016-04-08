#include <sstream>
#include "news/news_logic.h"
#include "core/common.h"
#include "base/logic/logic_comm.h"
#include "base/logic/base_values.h"
#include "public/config/config.h"

#include "logic/logic_unit.h"
#include "net/operator_code.h"
#include "news/db_redis.h"
#include "news/db_mysql.h"
#include "news/news_base.h"
#include "news/mem_manager.h"
#include "db_hbase.h"
#include "kafka_producer.h"
#include "kafka_accessor.h"


namespace news_logic {

NewsLogic* NewsLogic::instance_ = NULL;

NewsLogic::NewsLogic() {
  if (!Init())
    assert(0);
}

NewsLogic::~NewsLogic() {
}

bool NewsLogic::Init() {
  bool r = false;
  do {
    std::string path = DEFAULT_CONFIG_PATH;
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if (config == NULL) {
      return false;
      break;
    }
    r = config->LoadConfig(path);
    news::DbRedis::Init(&config->redis_list_);
    news::DbMysql::Init(&config->mysql_db_list_);
    MemManager::GetInstance()->InitHyMap();
    MemManager::GetInstance()->InitGnMap();
    LOG_DEBUG2("[HbaseConfi]host:%s,port:%d, table:%s", \
        config->hbase_db_list_.front().host().c_str(), \
        config->hbase_db_list_.front().port(), \
        config->hbase_db_list_.front().source().c_str());
    
    DbHbase::InitHbase(config->hbase_db_list_.front().host().c_str(),
                      config->hbase_db_list_.front().port(),
                      config->hbase_db_list_.front().source(),
                      1);

    // local kafka config
    LOG_DEBUG2("KafkaConfig]host:%s,port:%d,topic:%s", \
        config->kafka_conf_list_.front().host().c_str(),
        config->kafka_conf_list_.front().port(),
        config->kafka_conf_list_.front().source().c_str());

    if (0 <= news::KafkaAccessor::Init(0, \
          config->kafka_conf_list_.front().host(), \
        config->kafka_conf_list_.front().port(), \
        config->kafka_conf_list_.front().source().c_str())) {
      LOG_DEBUG("Kafka init successful...");
    }
  } while (0);
  return r;
}

NewsLogic* NewsLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new NewsLogic();
  return instance_;
}

void NewsLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
  DbHbase::ReleaseAll();
  news::KafkaAccessor::Release();
  news::DbMysql::Dest();
}

bool NewsLogic::OnNewsConnect(struct server *srv,
    const int socket) {
  return true;
}

bool NewsLogic::OnNewsMessage(struct server *srv,
    const int socket,
    const void *msg,
    const int len) {
  bool r = true;
  do {
    const char* packet = reinterpret_cast<const char*>(msg);
    if (NULL == packet) {
      r = false;
      break;
    }
    std::string http_str_tmp(packet, len);
    std::string http_str;
    // decode
    base::BasicUtil::UrlDecode(http_str_tmp, http_str);

    std::string error_str;
    int error_code = 0;
    scoped_ptr<base_logic::ValueSerializer> serializer(
        base_logic::ValueSerializer::Create(
          base_logic::IMPL_HTTP, &http_str));
    NetBase* value = (NetBase*)(
        serializer.get()->Deserialize(&error_code, &error_str));
    LOG_DEBUG2("OnNewsMessage：%d, http_str:%s", 123, http_str.c_str());
    if (NULL == value) {
      error_code = STRUCT_ERROR;
      send_error(error_code, socket);
      r = true;
      break;
    }
    scoped_ptr<RecvPacketBase> head_packet(new RecvPacketBase(value));
    int32 type = head_packet->GetType();
    switch (type) {
      case GET_NEWS: {
        GetNews(srv, socket, value);
        break;
      }
      case SET_NEWS: {
        SetNews(srv, socket, value);
        break;
      }
      case NEWS_DETAIL: {
        GetNewsDetail(srv, socket, value);
        break;
      }
      case NEWS_SHARE: {
        ShareNews(srv, socket, value);
        break;
      }
      case NEWS_TRANSMIT: {
        TransmitNews(srv, socket, value);
        break;
      }
      default: {
        r = false;
        break;
      }
    }
  } while (0);
  return r;
}

bool NewsLogic::OnNewsClose(struct server *srv,
    const int socket) {
  return true;
}

bool NewsLogic::OnBroadcastConnect(struct server *srv,
    const int socket,
    const void *msg,
    const int len) {
  return true;
}

bool NewsLogic::OnBroadcastMessage(struct server *srv,
    const int socket,
    const void *msg,
    const int len) {
  return true;
}

bool NewsLogic::OnBroadcastClose(struct server *srv,
    const int socket) {
  return true;
}

bool NewsLogic::OnIniTimer(struct server *srv) {
  LOG_DEBUG2("OnIniTimer news:%s", "111");
  srv->add_time_task(srv, "news", QUERY_STOCK_HY, 1, 1);
  srv->add_time_task(srv, "news", QUERY_STOCK_GN, 5, 2);
  srv->add_time_task(srv, "news", QUERY_STOCK_BK, 10, 2);
  return true;
}

bool NewsLogic::OnTimeout(struct server *srv,
    char *id,
    int opcode,
    int time) {
  LOG_DEBUG2("OnTimeout news:%s", "222");
  switch (opcode) {
      case QUERY_STOCK_HY: {
        MemManager::GetInstance()->InitHyMap();
        break;
      }
      case QUERY_STOCK_GN: {
        break;
      }
      case QUERY_STOCK_BK: {
        break;
      }
      default: {
        break;
      }
  }
  return true;
}

bool NewsLogic::GetNews(struct server* srv,
    const int socket, NetBase* netbase,
    const void* msg,
    const int len) {
  bool r = true;
  do {
    scoped_ptr<StockNewsRecv> recv_news(new StockNewsRecv(netbase));
    std::string jsonp_str = recv_news->GetJsonp();
    int32 json_type = ("" == jsonp_str)? \
                      base_logic::IMPL_JSON : base_logic::IMPL_JSONP;
    int32 error_code = recv_news->GetResult();
    if (error_code != 0) {
      send_error(error_code, socket, json_type, jsonp_str);
      r = false;
      break;
    }
    std::string stock_code = recv_news->get_stock_code();
    std::string hy_name = recv_news->get_hy_name();
    int64 news_type = recv_news->get_news_type();
    int64 news_count = recv_news->get_count();
    int64 news_page = recv_news->get_page();
    scoped_ptr<StockNewsSend> send_news(new StockNewsSend());
    if (news_type != EALL_NEWS) {
      error_code = QueryNews(stock_code, hy_name, news_type, news_page, \
                              (StockNewsSend*)send_news.get());
    } else {
      error_code = QueryNews(stock_code, hy_name, news_type, news_page, \
                              (StockNewsSend*)send_news.get());
    }
    if (error_code < 0) {
      send_error(error_code, socket, json_type, jsonp_str);
      break;
    }
    if (json_type == base_logic::IMPL_JSONP)
      send_news->set_jsonp_callback(jsonp_str);
    send_message(socket, (SendPacketBase*)send_news.get(), json_type);
  } while (0);
  return r;
}

int32 NewsLogic::QueryNews(std::string stock_code,
    std::string hy_name,
    int64 news_type,
    int64 news_count,
    StockNewsSend* send_news) {
  int32 err = 0;
  std::string name;
  do {
    if (!stock_code.empty()) {
      if (ESTOCK_NEWS == news_type) {  // 股票-股票新闻
        name = stock_code;
        news::DbRedis::GetStockNews(name, news_count, send_news);
        break;
      } else if (EINDUSTRY_NEWS == news_type) {  // 股票-行业新闻
        name = MemManager::GetInstance()->GetHyByCode(stock_code);
        err = news::DbRedis::GetHyNews(name, news_count, send_news);
        if (err < 0)
          break;
        break;
      } else if (ESECTION_NEWS == news_type) {  // 股票-板块新闻
        name = MemManager::GetInstance()->GetGnByCode(stock_code);
        err = news::DbRedis::GetGnNews(name, news_count, send_news);
        break;
      } else if (ESTUDY_NEWS == news_type) {  // 股票-研报新闻
        err = news::DbRedis::GetYbNews(name, news_count, send_news);
        break;
      } else {
        err = NEW_TYPE_ERROR;
        break;
      }
    } else {
      if (EALL_NEWS == news_type) {
          // err = news::DbRedis::GetAllNews(news_count, send_news);
          err = news::DbRedis::GetAllPageNews(news_count, send_news);
          break;
      }
      if (hy_name.empty()) {
        err = HY_NAME_LACK;
        break;
      }
      err = news::DbRedis::GetHyNews(hy_name, news_count, send_news);
      break;
    }
  } while (0);
  if (err == 0 && news_type > 0 && news_type != EALL_NEWS && \
      send_news->get_news_size() > 0) {
    // get first news Time
    std::string newstime_s = send_news->GetFirstNewsTime();
    std::string newstime  = newstime_s.erase(11, string::npos);
    newstime_s = newstime + "00:00:00";
    std::string newstime_e = newstime + "23:59:59";
    int32 pos = name.find(',');
    if (pos != std::string::npos) {
      name.erase(pos);
    }
    err = news::DbMysql::GetNewsAffect(news_type, name, \
        send_news, newstime_s, newstime_e);
  }
  if (0 == send_news->get_news_size()) {
    send_news->set_positive_per(0.0);
    send_news->set_negative_per(0.0);
    send_news->set_o_per(0.0);
  }
  return err;
}

bool NewsLogic::SetNews(struct server* srv,
    const int socket, NetBase* netbase,
    const void* msg,
    const int len) {
  bool r = true;
  do {
    scoped_ptr<NewsAffectRecv> recv_news(new NewsAffectRecv(netbase));
    std::string jsonp_str = recv_news->GetJsonp();
    int32 json_type = ("" == jsonp_str)? \
                      base_logic::IMPL_JSON : base_logic::IMPL_JSONP;
    int32 error_code = recv_news->GetResult();
    if (error_code != 0) {
      send_error(error_code, socket, json_type, jsonp_str);
      r = false;
      break;
    }
    // int64 date = recv_news->get_date();
    string str_date = recv_news->get_date();
    string str_num_date = news::DbRedis::TimeFormat2TimeNumStr(str_date);
    std::stringstream os;
    os << str_num_date;
    int64 date;
    os >> date;

    std::string news_id = recv_news->get_news_id();
    int64 user_id = recv_news->get_user_id();
    int64 commit_type = recv_news->get_commit_type();
    scoped_ptr<StockNewsSend> send_news(new StockNewsSend());
    if (json_type == base_logic::IMPL_JSONP)
      send_news->set_jsonp_callback(jsonp_str);
    r = news::DbRedis::AterNews(date, news_id, commit_type, \
        (StockNewsSend*)send_news.get(), user_id);
    send_message(socket, (SendPacketBase*)send_news.get(), json_type);
  } while (0);
  return r;
}

bool NewsLogic::GetNewsDetail(struct server* srv,
        const int socket, NetBase* netbase,
        const void* msg,
        const int len) {
  scoped_ptr<NewsDetailRecv> recv_news(new NewsDetailRecv(netbase));
  std::string jsonp_str = recv_news->GetJsonp();
  int32 json_type = (0 == jsonp_str.length())? \
    (base_logic::IMPL_JSON):(base_logic::IMPL_JSONP);
  int32 error_code = recv_news->GetResult();
  if (0 != error_code) {
    send_error(error_code, socket, json_type, jsonp_str);
    return false;
  }
  std::string news_id = recv_news->news_id();
  std::string news_json_content;
  LOG_DEBUG2("[GetNewsDetail, news_id:%s, date:%s]", news_id.c_str(), \
      recv_news->get_date().c_str());

  std::string str_date = recv_news->get_date();
  string str_num_date = news::DbRedis::TimeFormat2TimeNumStr(str_date);
  std::stringstream os;
  os << str_num_date;
  int64 date;
  os >> date;

  std::stringstream ss;
  ss << "News_" << date/NUM_DATE_DIV;
  std::string field = ss.str();
  news::DbRedis::QueryNewsById(field, news_id, &news_json_content);
  if (news_json_content.length() > 0) {
    error_code = 0;
    std::string error_str;
    scoped_ptr<base_logic::ValueSerializer> engine\
      (base_logic::ValueSerializer::Create(\
       base_logic::IMPL_JSON, &news_json_content));
    NetBase* v = (NetBase*)(engine->Deserialize(&error_code, &error_str));
    v->SetString(L"time", news::DbRedis::TimeNum2TimeFormat(date));
    scoped_ptr<StockNewsSend> news_send(new StockNewsSend());
    if (json_type == base_logic::IMPL_JSONP) {
      news_send->set_jsonp_callback(jsonp_str);
    }
    /*
    // get news detail from hbase
    std::string news_detail;
    DbHbase *hbase_ptr = DbHbase::GetDbInstance();
    if (NULL!= hbase_ptr) {
      std::string news_url;
      v->GetString(L"url", &news_url);
      hbase_ptr->GetColumnData(DbHbase::s_table_, news_url, \
          "basic:content", news_detail);
      if (news_detail.length() > 0) {
        v->SetString(L"detail", news_detail);
        LOG_DEBUG2("[根据新闻id获取新闻详情]:%s", news_detail.c_str());
      } else {
        LOG_ERROR2("获取详情失败,news_url:%s", news_url.c_str());
      }
    }
    DbHbase::PushHbaseConnect(hbase_ptr);
    */
    AppendNewsDetail(v);
    news::DbRedis::AppendTestData(v); 
    // end pool
    // 是否评论过
    int64 user_id = recv_news->get_user_id();
    CommentType etype = \
           news::DbRedis::CheckUserComment(user_id, news_id, str_date);
    v->SetBigInteger(L"comment_recored", etype);
    //
    news_send->AddNews(v);
    send_message(socket, (SendPacketBase*)news_send.get(), json_type);
  } else {
    send_error(NULL_DATA, socket, json_type, jsonp_str);
  }
  return true;
}

// share news
bool NewsLogic::ShareNews(struct server* srv,
    const int socket, NetBase* netbase,
    const void* msg,
    const int len) {
    LOG_DEBUG("[ShareNews]");

    scoped_ptr<NewsShareRecv> recv_share(new NewsShareRecv(netbase));
    std::string jsonp_str = recv_share->GetJsonp();
    int32 json_type = (0 == jsonp_str.length())? \
      (base_logic::IMPL_JSON):(base_logic::IMPL_JSONP);
    int32 error_code = recv_share->GetResult();
    if (0 != error_code) {
      send_error(error_code, socket, json_type, jsonp_str);
      return false;
    }
    // 打包成json格式
    std::stringstream ss;
    std::string content = recv_share->content();
    content += "[更多详情，请访问]:";
    content += recv_share->url();

    ss << "{" \
       << "\"subject\":\"" << recv_share->subject() << "\"," \
       << "\"content\":\"" << content <<"\"," \
       << "\"to\":\"" << recv_share->receiver() << "\"" \
       << "}";
    LOG_DEBUG2("share news:\n%s", ss.str().c_str());

    json_type = base_logic::IMPL_JSONP;
    jsonp_str = "jsonp";
    scoped_ptr<NewsShareSend> share_send(new NewsShareSend());
    if (json_type == base_logic::IMPL_JSONP) {
      share_send->set_jsonp_callback(jsonp_str);
    }
    send_message(socket, (SendPacketBase*)share_send.get(), json_type);
    // send data to kafka
    news::KafkaAccessor::PushData(ss.str().c_str(), ss.str().length());
    return true;
}

// transmit News
bool NewsLogic::TransmitNews(struct server* srv,
    const int socket, NetBase* netbase,
    const void* msg,
    const int len) {
  scoped_ptr<NewsTransmitRecv> recv_share(new NewsTransmitRecv(netbase));
  std::string jsonp_str = recv_share->GetJsonp();
  int32 json_type = (0 == jsonp_str.length())? \
    (base_logic::IMPL_JSON):(base_logic::IMPL_JSONP);
  int32 error_code = recv_share->GetResult();
  std::string date = recv_share->news_date();
  if (date.length() < NEWS_DATE_LEN) {
    send_error(STRUCT_ERROR, socket, json_type, jsonp_str);
    return false;
  }
  std::string date_num_str = news::DbRedis::TimeFormat2TimeNumStr(date);
  date_num_str.erase(DATETIME_STRING_TIME_POS, std::string::npos);
  std::string news_json;
  std::stringstream ss;
  ss <<"News_"<< date_num_str;
  int32 ret = news::DbRedis::QueryNewsById(ss.str(), \
      recv_share->news_id(), &news_json);
  if (news_json.length() < 1) {
    send_error(NULL_DATA, socket, json_type, jsonp_str);
    return false;
  }
  // 逆串行化
  std::string error_str;
  scoped_ptr<base_logic::ValueSerializer> engine\
      (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &news_json));
    NetBase* v = (NetBase*)(engine->Deserialize(&error_code, &error_str));

  // 计数增加
  int64 transmit_count = 0;
  v->GetBigInteger(L"transmit_count", &transmit_count);
  v->SetBigInteger(L"transmit_count", ++transmit_count);

  // 串行化
  std::string out_str;
  scoped_ptr<base_logic::ValueSerializer>engine_out \
    (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &out_str));
  engine_out->Serialize(*v);

  // save
  news::DbRedis::SaveNews(ss.str(), recv_share->news_id(), out_str);

  scoped_ptr<StockNewsSend> transmit_send(new StockNewsSend());
  transmit_send->AddNews(v);
  if (json_type == base_logic::IMPL_JSONP) {
    transmit_send->set_jsonp_callback("jsonp");
  }
  send_message(socket, (SendPacketBase*)transmit_send.get(), json_type);
  return true;
}

bool NewsLogic::AppendNewsDetail(NetBase* news_kv) {
  if (NULL == news_kv) {
    return false;
  }
  // get news detail from hbase
  std::string news_detail;
  DbHbase *hbase_ptr = DbHbase::GetDbInstance();
  if (NULL!= hbase_ptr) {
    std::string news_url;
    news_kv->GetString(L"url", &news_url);
    hbase_ptr->GetColumnData(DbHbase::s_table_, news_url, \
        "basic:content", news_detail);
    if (news_detail.length() > 0) {
      news_kv->SetString(L"detail", news_detail);
      LOG_DEBUG2("[根据新闻id获取新闻详情]:%s", news_detail.c_str());
    } else {
      LOG_ERROR2("获取详情失败,news_url:%s", news_url.c_str());
    }
    DbHbase::PushHbaseConnect(hbase_ptr);
  }
  return true;
}

}  // namespace news_logic

