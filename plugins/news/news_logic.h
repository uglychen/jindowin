#ifndef __NEWS__NEWS__NEWS_LOGIC___
#define __NEWS__NEWS__NEWS_LOGIC___
#include <string>
#include "core/common.h"
#include "net/http_data.h"
#include "news/news_packet.h"
#include "kafka_producer.h"

#define DEFAULT_CONFIG_PATH           "./plugins/news/news_config.xml"
#define NUM_DATE_DIV                  1000000
#define CONTENT_LENGTH_MAX            400
#define NEWS_DATE_LEN                 19
#define DATETIME_STRING_TIME_POS      8

namespace news_logic{

enum QueryNewsType{
  ESTOCK_NEWS = 1,
  EINDUSTRY_NEWS = 2,
  ESECTION_NEWS = 3,
  ESTUDY_NEWS = 4,
  EALL_NEWS = 5
};

enum CommentType{
  ECOMMENT_UNCOMMENT = 0,
  ECOMMENT_UP = 1,
  ECOMMENT_DOWN = 2
};

class NewsLogic{
public:
  NewsLogic();
  virtual ~NewsLogic();
private:
  static NewsLogic  *instance_;
public:
  static NewsLogic *GetInstance();
  static void FreeInstance();
public:
  bool OnNewsConnect(struct server *srv,const int socket);
  bool OnNewsMessage(struct server *srv, const int socket, \
      const void *msg, const int len);
  bool OnNewsClose(struct server *srv,const int socket);
  bool OnBroadcastConnect(struct server *srv,const int socket, \
      const void *data, const int len);
  bool OnBroadcastMessage(struct server *srv, const int socket, \
      const void *msg, const int len);
  bool OnBroadcastClose (struct server *srv, const int socket);
  bool OnIniTimer (struct server *srv);
  bool OnTimeout (struct server *srv, char* id, int opcode, int time);
public:
  bool AppendNewsDetail(NetBase* news_kv);
private:
  bool Init();
  bool GetNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);
  int32 QueryNews(std::string stock_code,
      std::string hy_name,
      int64 news_type,
      int64 news_count,
      StockNewsSend* send_news);
  bool SetNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool GetNewsDetail(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool ShareNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool TransmitNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);
};

}

#endif

