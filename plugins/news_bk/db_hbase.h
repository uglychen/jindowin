/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/tianyiheng_test/runner/northsea/restful/plugins/test/db_hbase.h
#    Author       : tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-02-29 18:13
#    Description  : 
=============================================================================*/

#ifndef __DB_HBASE_H__
#define __DB_HBASE_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <pthread.h>

#include <queue>
#include <iostream>
#include <memory>

#include <boost/lexical_cast.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "Hbase.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace apache::hadoop::hbase::thrift;
using namespace std;


typedef std::map<std::string, std::string> RowDataMap;

class DbHbase{
  public:
    DbHbase();
    ~DbHbase();
    bool Init(std::string host, int port);
    void Close();
  protected:
    boost::shared_ptr<TTransport> socket_ptr_;
    boost::shared_ptr<TTransport> transport_ptr_;
    boost::shared_ptr<TProtocol> protocol_ptr_;
    boost::shared_ptr<HbaseClient> client_ptr_;
  public:
    void GetRowData(std::string table_name, \
        std::string row_key, RowDataMap & data_map);
    void GetColumnData(std::string table_name, std::string row_key, 
        std::string column_name, std::string &column_data);
  
  // 鉴于thirftserver的线程阻塞原因，目前不使用连接池，开放以下
  // 成员变量，每次访问时建立新的连接，访问完毕后断开连接
  public:
    static std::queue<DbHbase*> s_db_hbase_connect_queue_;
    static std::string s_host_;
    static std::string s_table_;
    static int s_port_;
    static void PrintRow(const std::vector<TRowResult> &rowResult);
  public:
    static void InitHbase(std::string host, int port, std::string table, int num = 2);
    static DbHbase* GetDbInstance();
    static void ReleaseAll();
    static void PushHbaseConnect(DbHbase* hbase_ptr);
  protected:
    // mutex lock
    static pthread_mutex_t s_hbase_mutext_;
};

#endif
