/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/tianyiheng_test/runner/northsea/restful/plugins/test/db_hbase.cc
#    Author       : Tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-02-29 18:15
#    Description  : 
=============================================================================*/

#include "db_hbase.h"
#include "logic/logic_comm.h"

DbHbase::DbHbase() {
  is_normal_ = false;
}

bool DbHbase::Init(std::string host, int port) {
  try {
    socket_ptr_.reset(new TSocket(host.c_str(), port));
    transport_ptr_.reset(new TBufferedTransport(socket_ptr_));
    protocol_ptr_.reset(new TBinaryProtocol(transport_ptr_));
    client_ptr_.reset(new HbaseClient(protocol_ptr_));
    transport_ptr_->open();
  }catch(const TException &tx) {
    LOG_DEBUG2("Error:%s", tx.what());
    return false;
  }
  LOG_DEBUG("DbHbase Inited ...");
  is_normal_ = true;
  return true;
}

DbHbase::~DbHbase() {
}

void DbHbase::Close() {
  if (is_normal_) {
    transport_ptr_->close();
    LOG_DEBUG("[DbHbase::Close()]");
    is_normal_ = false;
  }
}

void DbHbase::GetRowData(std::string table_name, std::string row_key,
    RowDataMap & data_map) {
  if (!is_normal_) {
    return;
  }
  std::vector<TRowResult> vec_result;
  std::map<Text, Text>  map_attr;
  client_ptr_->getRow(vec_result, table_name, row_key, map_attr);
  for (size_t i = 0; i < vec_result.size(); ++i) {
    for (std::map<std::string, TCell>::const_iterator
        it = vec_result[i].columns.begin();
      it != vec_result[i].columns.end(); ++it) {
        data_map.insert(make_pair(it->first, it->second.value));
      }
  }
}

void DbHbase::GetColumnData(std::string table_name, std::string row_key,
    std::string column_name,
    std::string &column_data) {
  if (!is_normal_) {
    return;
  }
  std::vector<TRowResult> vec_result;
  std::map<Text, Text>  map_attr;
  std::vector<Text> vec_columns;
  vec_columns.push_back(column_name);
  client_ptr_->getRowWithColumns(vec_result, table_name, row_key,
      vec_columns, map_attr);
  if (vec_result.size() > 0) {
    for (std::map<std::string, TCell>::const_iterator \
        it = vec_result[0].columns.begin();
      it != vec_result[0].columns.end(); ++it) {
        column_data = it->second.value;
        break;
      }
  }
}

// -----------连接队列-------
std::string DbHbase::s_host_;
int DbHbase::s_port_ = 9090;
std::string DbHbase::s_table_ = "wk_detail";
std::queue<DbHbase*> DbHbase::s_db_hbase_connect_queue_;
pthread_mutex_t DbHbase::s_hbase_mutext_ = PTHREAD_MUTEX_INITIALIZER;

void DbHbase::InitHbase(std::string host, int port, std::string table, int num) {
  s_host_ = host;
  s_port_ = port;
  s_table_ = table;
  if (num <= 0) {
    num = 1;
  }
  // init ..
  pthread_mutex_init(&s_hbase_mutext_, NULL);
  for (int i = 0; i < num; ++i) {
    pthread_mutex_lock(&s_hbase_mutext_);
    DbHbase *hbase_ptr = new DbHbase;
    if (NULL != hbase_ptr) {
      hbase_ptr->Init(s_host_, s_port_);
      s_db_hbase_connect_queue_.push(hbase_ptr);
      hbase_ptr = NULL;
    }
    pthread_mutex_unlock(&s_hbase_mutext_);
  }
}

DbHbase* DbHbase::GetDbInstance() {
  DbHbase* ret = NULL;
  pthread_mutex_lock(&s_hbase_mutext_);
  if (0 < s_db_hbase_connect_queue_.size()) {
    ret = s_db_hbase_connect_queue_.front();
    s_db_hbase_connect_queue_.pop();
  } else {
    ret = new DbHbase;
    ret->Init(s_host_, s_port_);
  }
  pthread_mutex_unlock(&s_hbase_mutext_);
  return ret;
}

void DbHbase::PushHbaseConnect(DbHbase* hbase_ptr) {
  pthread_mutex_lock(&s_hbase_mutext_);
  s_db_hbase_connect_queue_.push(hbase_ptr);
  pthread_mutex_unlock(&s_hbase_mutext_);
}

void DbHbase::ReleaseAll() {
  pthread_mutex_lock(&s_hbase_mutext_);
  while (s_db_hbase_connect_queue_.size() > 0) {
    DbHbase* instanse = s_db_hbase_connect_queue_.front();
    if (NULL != instanse) {
      instanse->Close();
      delete instanse;
    }
    s_db_hbase_connect_queue_.pop();
  }
  pthread_mutex_unlock(&s_hbase_mutext_);
}
