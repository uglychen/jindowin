//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_

#include <string>

#include "net/base.h"
#include "basic/basictypes.h"

static int count = 0;
namespace login {
class Login : public base::Base{
 public:
  Login() \
  : platform_id_(-1), \
  user_id_(-1), \
  user_level_(-1), \
  socket_(-1) {}
  Login(int platform_id, const std::string &user_name, \
                          const std::string &password, \
						  int user_type,\
                         const std::string &jsonp_str, \
                                           int socket) \
      : platform_id_(platform_id), \
      user_name_(user_name), \
      password_(password), \
      user_id_(-1), \
      user_level_(-1), \
	  user_type_(user_type),\
      jsonp_str_(jsonp_str), \
      socket_(socket) {}
  virtual ~Login() {}
  //  ����ӿ�
 public:
  bool GetData();
  void SendMessage();

 private:
  bool DataInMySql(time_t start_time, time_t end_time);

  bool DataInRedis(time_t start_time, time_t end_time) { return true; }

  bool DataBetweenRedisAndMySql(time_t start_time, \
                                  time_t end_time, \
                               time_t middle_time) { return true; }

public:
  int platform_id_;
  std::string user_name_;
  std::string password_;
  std::string platform_name_;
  int user_type_;
  int user_id_;
  int user_level_;
  int socket_;
  std::string jsonp_str_;
};

}  // namespace login

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_
