//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/3/28 Author: chenxun

#include "userlogin/register.h"

#include <string>
#include <sstream>

#include "logic/logic_unit.h"
#include "db/db_comm.h"
#include "tools/tools.h"
#include "net/typedef.h"
#include "net/user_comm_head.h"
#include "tea/tea.h"

namespace user_register {
bool Register::SetData() {
	Register::DataInMySql(0, 0);
  return true;
}

bool Register::DataInMySql(time_t start_time, time_t end_time) {

	db::DbSql sql;
	std::string username="";
	username=sql.GetUserNameInfo(user_name_);
	error_info_ = sql.get_error_info();
	if (0 != error_info_) {
		return false;
	}
	LOG_DEBUG2("--------GetUserNameInfo--------------user_name:%s",username.c_str());

	if(username==user_name_)
	{
		error_info_ = USER_NAME_EXIST;
		return false;
	}
 // base::MD5Sum md5;
 // md5.Calculate(password_);
 // password_=md5.GetHash();
 // LOG_DEBUG2("--------md5-----password_:%s",password_.c_str());

	sql.SetUserIfo(platform_id_, user_name_,password_);
	error_info_ = sql.get_error_info();
	if (0 != error_info_) {
		return false;
	}
	return true;
}

void Register::SendMessage() {
	netcomm_send::SendRegister re_register;
	re_register.set_userinfo_name(user_name_);
	re_register.set_userinfo_password(password_);
	int json_type = 0;
	if (jsonp_str_ != "") {
		re_register.set_jsonp_callback(jsonp_str_);
		json_type = 3;
	}
	send_message(socket_, &re_register, json_type);
}

}
