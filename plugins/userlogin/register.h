//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2016/3/28 Author: chenxun

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_REGISTER_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_REGISTER_H_

#include <string>

#include "net/base.h"
#include "basic/basictypes.h"

namespace user_register {
class Register : public base::Base{
 public:
	Register() \
  : platform_id_(-1), \
  user_id_(-1), \
  user_level_(-1), \
  socket_(-1) {}
	Register(int platform_id,const std::string &user_name, const std::string &password, const std::string &jsonp_str, int socket) \
      : platform_id_(platform_id), \
      user_name_(user_name), \
      password_(password), \
      user_id_(-1), \
      user_level_(-1), \
      jsonp_str_(jsonp_str), \
      socket_(socket) {}
  virtual ~Register() {}
  //  对外接口
 public:
  bool SetData();
  void SendMessage();

 private:
  bool DataInMySql(time_t start_time, time_t end_time);

  bool GetData(){};

  bool DataInRedis(time_t start_time, time_t end_time) { return true; }

  bool DataBetweenRedisAndMySql(time_t start_time, \
                                  time_t end_time, \
                               time_t middle_time) { return true; }

public:
  int platform_id_;
  std::string user_name_;
  std::string password_;
  int64 user_id_;
  int user_level_;
  int socket_;
  std::string jsonp_str_;
};

}  // namespace login

#endif
