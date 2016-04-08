//  Copyright (c) 2015-2015 The db_mysql.h Authors. All rights reserved.
//  Created on: 2016年1月9日 Author: paco

#ifndef PLUGINS_NEWS_DB_MYSQL_H_
#define PLUGINS_NEWS_DB_MYSQL_H_

#include <list>
#include <string>
#include "public/basic/basic_info.h"

#include "news/news_base.h"
#include "news/news_packet.h"

namespace news {
class DbMysql {
  public:
    DbMysql();
    ~DbMysql();

    static void Init(std::list<base::ConnAddr>* const addrlist);
    static void Dest();

  public:
    static bool QueryStockHy(StringMap* hy);
    static bool QueryStockGn(StringMap* gn);
    static int32 GetNewsAffect(int64 type, \
        std::string name, StockNewsSend* send_news, \
        std::string starttime, std::string endtime);
};

}  // namespace news
#endif  // PLUGINS_NEWS_DB_MYSQL_H_
