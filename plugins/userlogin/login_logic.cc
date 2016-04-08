
#include <string>
#include "net/user_comm_head.h"
#include "net/error_comm.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "logic/base_values.h"
#include "db/db_comm.h"
#include "base/logic/logic_comm.h"
#include "core/common.h"

#include "userlogin/login.h"
#include "userlogin/login_logic.h"
#include "userlogin/register.h"
#include "tools/tools.h"

namespace login_svc_logic{

Loginlogic* Loginlogic::instance_=NULL;

Loginlogic::Loginlogic(){
   if(!Init())
      assert(0);
}

Loginlogic::~Loginlogic(){
	db::DbSql::Dest();
}

bool Loginlogic::Init(){
	  bool r = false;
	  std::string path = DEFAULT_CONFIG_PATH;
	  config::FileConfig* config = config::FileConfig::GetFileConfig();
	  if (config == NULL) {
	    return false;
	  }
	  r = config->LoadConfig(path);

	  LOG_DEBUG2("user  size: %d", config->redis_list_.size());
	  db::DbSql::Init(&config->mysql_db_list_);
	  return true;
}

Loginlogic* Loginlogic::GetInstance(){

    if(instance_==NULL)
        instance_ = new Loginlogic();

    return instance_;
}

bool Loginlogic::OnLoginConnect(struct server *srv,const int socket){

    return true;
}



bool Loginlogic::OnLoginMessage(struct server *srv, const int socket, const void *msg,const int len)
{
	  const char* packet_stream = reinterpret_cast<const char*>(msg);
	  std::string http_str(packet_stream, len);
	  std::string error_str;
	  int error_code = 0;

	  scoped_ptr<base_logic::ValueSerializer> serializer(\
	    base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP, &http_str));

	  NetBase*  value = (NetBase*)\
	    (serializer.get()->Deserialize(&error_code, &error_str));

	  if (value == NULL) {
	    error_code = STRUCT_ERROR;
	    send_error(error_code, socket);
	    return true;
	  }

	  //if(!tools::check_id_token(value))
	 // {
	  //  error_code = USER_TYPE_IS_VISITOR;
	  //  send_error(error_code, socket);
	  //  return true;
	 // }

	  scoped_ptr<RecvPacketBase> packet(\
	    new RecvPacketBase(value));

	  int32 type = packet->GetType();
	  LOG_DEBUG2("-----type-------%d", type);
	  switch (type)
	  {
	    case USER_LOGIN:
	    	OnLogin(srv, socket, value);
	      break;
	    case USER_REGISTER:
	    	OnRegister(srv, socket, value);
	      break;

	    default:  //  end
	      return false;
	  }

	  return true;
}

bool Loginlogic::OnLoginClose(struct server *srv,const int socket){

    return true;
}



bool Loginlogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Loginlogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Loginlogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Loginlogic::OnIniTimer(struct server *srv){

    return true;
}



bool Loginlogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}

bool Loginlogic::OnLogin(struct server *srv, const int socket, NetBase* netbase,
		const void* msg, const int len)
{
	netcomm_recv::RecvLogin recv_login(netbase);
	std::string jsonp_call = recv_login.GetJsonp();
	int json_type = jsonp_call == "" ? 0 : 3;
	int error_code = recv_login.GetResult();
	if (error_code != 0) {
		send_error(error_code, socket, json_type, jsonp_call);
		return false;
	}

	login::Login login(recv_login.platform_id(),
				recv_login.user_name(),
				recv_login.password(),
				recv_login.user_type(),\
				jsonp_call, socket);
	login.GetData();
	error_code = login.get_error_info();
	if (error_code != 0) {
		send_error(error_code, socket, json_type, jsonp_call);
		return false;
	}
	login.SendMessage();

	return true;
}


bool Loginlogic::OnRegister(struct server *srv, const int socket, NetBase* netbase, const void* msg, const int len)
{
	netcomm_recv::RecvRegister recv_register(netbase);
	std::string jsonp_call = recv_register.GetJsonp();
	int json_type = jsonp_call == "" ? 0 : 3;
	int error_code = recv_register.GetResult();
	if (error_code != 0) {
		send_error(error_code, socket, json_type, jsonp_call);
		return false;
	}

	user_register::Register sign(recv_register.platform_id(),
			recv_register.user_name(),
			recv_register.password(),
				jsonp_call, socket);

	sign.SetData();
	error_code = sign.get_error_info();
	LOG_DEBUG2("--------error_code:%d",error_code);
	if (error_code != 0){
	    send_error(error_code, socket, json_type, jsonp_call);
	    return false;
	  }

	sign.SendMessage();
}


}

