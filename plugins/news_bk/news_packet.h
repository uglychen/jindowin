//  Copyright (c) 2015-2015 The news_packet.h Authors. All rights reserved.
//  Created on: 2016年1月12日 Author: paco

#ifndef PLUGINS_NEWS_NEWS_PACKET_H_
#define PLUGINS_NEWS_NEWS_PACKET_H_

#include "net/http_data.h"
#include "base/logic/base_values.h"
// --------------------------------------recv---------------
class StockNewsRecv:public RecvLoginBase {
  public:
    explicit StockNewsRecv(NetBase* m)
      :RecvLoginBase(m) {
        count_ = 5;
        news_type_ = -1;
        page_ = 1;
        Init();
      }

    void Init();

    inline std::string get_stock_code() const { return stock_code_; }
    inline std::string get_hy_name() const { return hy_name_; }
    inline int64 get_count() { return count_; }
    inline int64 get_news_type() { return news_type_; }
    inline int64 get_page() {return page_;}
    inline void set_stock_code(std::string stock_code) \
        { stock_code_ = stock_code; }
    inline void set_hy_name(std::string name) { hy_name_ = name; }
    inline void set_count(int64 count) { count_ = count; }
    inline void set_news_type(int64 type) { news_type_ = type;}
    inline void set_page(int64 page) { page_ = page;}

  private:
    int64 news_type_;
    int64 count_;
    std::string stock_code_;
    std::string hy_name_;
    int64 page_;
};

class NewsAffectRecv:public RecvLoginBase {
  public:
    explicit NewsAffectRecv(NetBase* m)
      :RecvLoginBase(m) {
        commit_type_ = 0;
        Init();
      }
    void Init();
    inline std::string get_date() const { return date_; }
    inline std::string get_news_id() const { return news_id_; }
    inline int64 get_commit_type() const { return commit_type_; }
    inline void set_date(std::string date) { date_ = date; }
    inline void set_news_id(std::string id) { news_id_ = id; }
    inline void set_commit_type(int64 type) { commit_type_ = type; }
    inline int64 get_user_id() {return user_id();}
  private:
    std::string date_;
    std::string news_id_;
    int64 commit_type_;  // 1-up+1, 2-down+1
};

class NewsDetailRecv:public RecvLoginBase {
  public:
    explicit NewsDetailRecv(NetBase *m)
      :RecvLoginBase(m) {
        Init();
    }

    void Init();
    inline std::string news_id() {return news_id_;}
    inline std::string get_date() const {return date_; }
    inline int64 get_user_id() {return user_id();}
   private:
     std::string news_id_;
     std::string date_;
};

class NewsShareRecv:public RecvLoginBase {
  public:
    explicit NewsShareRecv(NetBase *m)
      :RecvLoginBase(m) {
        Init();
      }
      void Init();
      inline const std::string& subject() {return subject_;}
      inline const std::string& content() {return content_;}
      inline const std::string& receiver() {return receiver_;}
      inline const std::string& url() {return url_;}
      inline void set_subject(const std::string & subject) {subject_ = subject;}
      inline void set_content(const std::string & content) {content_ = content;}
      inline void set_reciever(const std::string & email_addr) {\
        receiver_ = email_addr;}
      inline void set_url(const std::string & url) {url_ = url;}
    private:
      std::string subject_;
      std::string content_;
      std::string receiver_;
      std::string url_;
};

// 资讯转发
class NewsTransmitRecv:public RecvLoginBase {
  public:
    explicit NewsTransmitRecv(NetBase *m)
      :RecvLoginBase(m) {
        Init();
      }
    void Init();
    inline std::string news_id() {return news_id_;}
    inline std::string news_date() {return news_date_;}
  private:
    std::string news_id_;
    std::string news_date_;
};

// --------------------------------------send------------------------------
class StockNewsSend:public SendPacketBase {
public:
  StockNewsSend() {news_.reset(new base_logic::ListValue());}
  NetBase* release() {
    this->set_status(1);
    head_->Set("result", news_.release());
    return head_.release();
  }
  inline void AddNews(base_logic::Value* v) {
    news_->Append(v);
  }
  inline void set_positive_per(const double p) {
    head_->SetReal("p_rate", p);
  }

  inline void set_negative_per(const double n) {
    head_->SetReal("n_rate", n);
  }
  inline void set_o_per(const double o) {
    head_->SetReal("o_rate", o);
  }
  inline int32 get_news_size() {
    return news_->GetSize();
  }
  std::string GetFirstNewsTime();
  bool IsAdded(std::string news_id);
  inline void AddNewsId(std::string news_id) {\
    vec_news_added_.push_back(news_id);}
  inline void SetPageCount(int64 count = 0) {
    head_->SetInteger("page_count", count);
  }

private:
  scoped_ptr<base_logic::ListValue> news_;
  std::vector<std::string> vec_news_added_;
};

class NewsShareSend:public SendPacketBase {
public:
  NewsShareSend() { }
  NetBase* release() {
    this->set_status(1);
    return head_.release();
  }
};

#endif  // PLUGINS_NEWS_NEWS_PACKET_H_
