//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/3/28 Author: chenxun

#include "userlogin/login.h"

#include <string>
#include <sstream>

#include "logic/logic_unit.h"
#include "db/db_comm.h"
#include "tools/tools.h"
#include "net/typedef.h"
#include "net/user_comm_head.h"
#include "tea/tea.h"
#include "tools/tools.h"

namespace login {
bool Login::GetData() {
  Login::DataInMySql(0, 0);
  return true;
}

bool Login::DataInMySql(time_t start_time, time_t end_time) {
	if (user_type_ == 0) {
		 __sync_fetch_and_add(&count,1);
		 user_id_ = (-1)*count;
	} else {
		db::DbSql sql;
		sql.GetUserInfo(platform_id_, user_name_);
		error_info_ = sql.get_error_info();
		if (0 != error_info_) {
			return false;
		}
		MYSQL_ROW rows = sql.NextRecord();
		if (password_ != rows[2]) {
			error_info_ = USER_PASSWORD_ERROR;
			return false;
		}

		user_id_ = atoll(rows[0]);
		user_level_ = atoll(rows[3]);

		platform_name_ = sql.GetPlatformName(platform_id_);
		error_info_ = sql.get_error_info();
		if (0 != error_info_) {
			return false;
		}
	}
	return true;
}

void Login::SendMessage() {
	std::string token="";
	tools::GetToken(user_id_,token);

	/*int len = token.size();
	for(int i =0; i<len;++i)
	{
		if(token[i]=='=')
		{
			token[i]=-1;
		}
	}*/

	LOG_DEBUG2("-----checktoken:%s", "ffdfdfdfdfdfd");
	tools::CheckToken(user_id_,token);

	netcomm_send::SendLogin qlogin;
	qlogin.set_userinfo_token(token);
	qlogin.set_userinfo_uid(user_id_);
	qlogin.set_userinfo_name(user_name_);
	if (user_type_ != 0) {
		qlogin.set_userinfo_platform(platform_name_);
		qlogin.set_userinfo_level(user_level_);
	}
	int json_type = 0;
	if (jsonp_str_ != "") {
		qlogin.set_jsonp_callback(jsonp_str_);
		json_type = 3;
	}
	send_message(socket_, &qlogin, json_type);

}

}  //  namespace login
