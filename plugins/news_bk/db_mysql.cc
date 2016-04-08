//  Copyright (c) 2015-2015 The db_mysql.cc Authors. All rights reserved.
//  Created on: 2016年1月9日 Author: paco

#include "news/db_mysql.h"
#include <assert.h>
#include <mysql/mysql.h>
#include <bits/stl_pair.h>
#include <sstream>
#include "base/db/base_db_mysql_auto.h"
namespace news {
DbMysql::DbMysql() {
}

DbMysql::~DbMysql() {
}

void DbMysql::Dest() {
  base_db::MysqlDBPool::Dest();
}

void DbMysql::Init(std::list<base::ConnAddr> *const addrlist) {
  base_db::MysqlDBPool::Init(*addrlist);
}

bool DbMysql::QueryStockHy(StringMap* hy) {
  assert(hy);
  hy->clear();
  bool r = false;
  do {
    base_db::AutoMysqlCommEngine auto_engine;
    base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
    if (engine == NULL) {
      LOG_ERROR("GetConnection Error");
      r = false;
      break;
    }
    std::stringstream os;
    MYSQL_ROW rows;
    int num;;
    os << "call proc_QueryStockHy()";
    std::string sql = os.str();
    r = engine->SQLExec(sql.c_str());
    if (!r) {
      LOG_ERROR("exec sql error");
      break;
    }
    num = engine->RecordCount();
    LOG_DEBUG2("[num:%d]", num);
    if (num > 0) {
      rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      while ( rows != NULL ) {
        hy->insert(std::pair<std::string, std::string>(rows[0], rows[1]));
        rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      }
      r = true;
    }
  } while (0);
  return r;
}

bool DbMysql::QueryStockGn(StringMap* gn) {
  assert(gn);
  gn->clear();
  bool r = false;
  do {
    base_db::AutoMysqlCommEngine auto_engine;
    base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
    if (engine == NULL) {
      LOG_ERROR("GetConnection Error");
      r = false;
      break;
    }
    std::stringstream os;
    MYSQL_ROW rows;
    int num = 0;
    os << "call proc_QueryStockGn()";
    std::string sql = os.str();
    r = engine->SQLExec(sql.c_str());
    if (!r) {
      LOG_ERROR("exec sql error");
      break;
    }
    num = engine->RecordCount();
    LOG_DEBUG2("[num:%d]", num);
    if (num > 0) {
      rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      while ( rows != NULL ) {
        gn->insert(std::pair<std::string, std::string>(rows[0], rows[1]));
        rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      }
      r = true;
    }
  } while (0);
  return r;
}

int32 DbMysql::GetNewsAffect(int64 type, std::string name,
                             StockNewsSend* send_news,
                             std::string starttime,
                             std::string endtime) {
  int32 err = 0;
  bool r = true;
  do {
    base_db::AutoMysqlCommEngine auto_engine;
    base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
    if (engine == NULL) {
      LOG_ERROR("GetConnection Error");
      err = CONNECT_ERROR;
      break;
    }
    std::stringstream os;
    MYSQL_ROW rows;
    int num;;
    os << "call proc_GetNewsAffect(" << type << ",'"
      << name << "','" << starttime << "','" <<endtime<< "')";

    std::string sql = os.str();
    LOG_DEBUG2("%s", sql.c_str());
    r = engine->SQLExec(sql.c_str());
    if (!r) {
      LOG_ERROR("exec sql error");
      err = SQL_EXEC_ERROR;
      break;
    }
    send_news->set_positive_per(0);
    send_news->set_negative_per(0);
    send_news->set_o_per(0);
    num = engine->RecordCount();
    if (num > 0) {
      rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      while ( rows != NULL ) {
        if (rows[0] != NULL)
          send_news->set_positive_per(atof(rows[0]));
        if (rows[1] != NULL)
          send_news->set_negative_per(atof(rows[1]));
        if (rows[2] != NULL)
          send_news->set_o_per(atof(rows[2]));
        rows = *(reinterpret_cast<MYSQL_ROW*>(engine->FetchRows()->proc));
      }
      r = true;
    }
  } while (0);
  return err;
}

}  // namespace news
