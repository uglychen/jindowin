//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing

#include <string>

#include "search/search_logic.h"
#include "search/handle_search.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "net/search_comm.h"
#include "db/db_comm.h"
#include "tools/tools.h"

namespace search_logic {

Searchlogic* Searchlogic::instance_ = NULL;

Searchlogic::Searchlogic() {
  if (!Init()) {
    assert(0);
  }
}

Searchlogic::~Searchlogic() {
  db::DbSql::Dest();
}

bool Searchlogic::Init() {
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);

  db::DbSql::Init(&config->mysql_db_list_);
  return true;
}

bool Searchlogic::OnSearchConnect(struct server *srv, const int socket) {
  return true;
}

bool Searchlogic::OnSearchMessage(struct server *srv, \
                                  const int socket, \
                                   const void *msg, \
                                     const int len) {
  std::string token;
  tools::GetToken(1234556, token);
  tools::CheckToken(1234556, token);
  const char* packet_stream = reinterpret_cast<const char*>(msg);
  std::string http_str(packet_stream, len);
  std::string error_str;
  int error_code = 0;

  scoped_ptr<base_logic::ValueSerializer> serializer(\
    base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP, &http_str));

  NetBase*  value = reinterpret_cast<NetBase*>\
    (serializer.get()->Deserialize(&error_code, &error_str));

  if (value == NULL) {
    error_code = STRUCT_ERROR;
    send_error(error_code, socket);
    return true;
  }

  scoped_ptr<RecvPacketBase> packet(\
    new RecvPacketBase(value));

  int32 type = packet->GetType();
  switch (type) {
    case SEARCH:
    {
      if(!tools::CheckUserIdAndToken(value,socket))
        return false;


      /*if (!tools::check_userid_if_in_sql(value, socket)) {
          return true;
        }
    
      if (!tools::check_id_token(value, socket)) {
          return true;
        }*/
      OnSearch(srv, socket, value);
      break;
    }
    default:  //  end
      return false;
  }
  return true;
}

bool Searchlogic::OnSearchClose(struct server *srv, const int socket) {
  return true;
}

bool Searchlogic::OnBroadcastConnect(struct server *srv, \
                                      const int socket, \
                                       const void *msg, \
                                         const int len) {
  return true;
}

bool Searchlogic::OnBroadcastMessage(struct server *srv, \
                                      const int socket, \
                                       const void *msg, \
                                         const int len) {
  return true;
}

bool Searchlogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool Searchlogic::OnIniTimer(struct server *srv) {
  return true;
}

bool Searchlogic::OnTimeout(struct server *srv, \
                                      char *id, \
                                    int opcode, \
                                      int time) {
  return true;
}

bool Searchlogic::OnSearch(struct server *srv, \
                             const int socket, \
                             NetBase* netbase, \
                              const void* msg, \
                               const int len ) {
  netcomm_recv::RecvSearch search_info(netbase);
  std::string str = search_info.GetJsonp();
  int json_type = str == "" ? 0 : 3;
  int error_code = search_info.GetResult();
  if (0 != error_code) {
    send_error(error_code, socket, json_type, str);
    return false;
  }

  HandleSearch search(search_info.message(), str, socket);
  search.GetData();
  error_code = search.get_error_info();
  if (0 != error_code) {
    send_error(error_code, socket, json_type, str);
    return false;
  } else {
    search.SendMessage();
  }

  return true;
}

}  //  namespace search_logic
