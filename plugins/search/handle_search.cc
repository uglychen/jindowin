// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2016/4/7 Author: jiaoyongqing

#include <regex.h>
#include "search/handle_search.h"
#include "db/db_comm.h"
#include "logic/logic_unit.h"
#include "net/typedef.h"
#include "net/search_comm.h"
#include "global/global.h"
#include "tools/tools.h"
#include "logic/logic_comm.h"

bool HandleSearch::GetData() {
  bool r = DataInMySql(0, 0);
  return r;
}

bool HandleSearch::MatchGn() {
  std::string RegexStr;
  regex_t oRegex;
  size_t num;
  int64 temp;
  bool r;
  MapStrToInt64 CheckIfExist;

  ContainerStr gn = global::GetInfo::GetInstance()->get_gn();
  ContainerStr gn_jp = global::GetInfo::GetInstance()->get_gn_jp();
  ContainerStr gn_qp = global::GetInfo::GetInstance()->get_gn_qp();

  RegexStr = message_ + ".*";
  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }

  num = gn.size();
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, gn[i].c_str(), 0, NULL, 0) == 0) {
      gn_.push_back(gn[i]);
      LOG_DEBUG2("gn : %s", gn[i].c_str());
      CheckIfExist[gn[i]] = 1;
    }
  }
  g_num_ = gn_.size();
  if (g_num_ >= 5) return true;

  //  jp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, gn_jp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
        (CheckIfExist, gn[i], &temp);
      if (r == false) {
        LOG_DEBUG2("gn : %s", gn[i].c_str());
        gn_.push_back(gn[i]);
        CheckIfExist[gn[i]] = 1;
      }
    }
  }
  g_num_ = gn_.size();
  if (g_num_ >= 5) return true;

  //  qp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, gn_qp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, gn[i], &temp);
      if (r == false) {
        LOG_DEBUG2("gn : %s", gn[i].c_str());
        gn_.push_back(gn[i]);
        CheckIfExist[gn[i]] = 1;
      }
    }
  }
  g_num_ = gn_.size();
  if (g_num_ >= 5) return true;

  RegexStr = ".*";
  for (size_t i = 0; i < message_.length(); ++i) {
    RegexStr += message_[i];
    RegexStr += ".*";
  }

  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }

  for (size_t i = 0; i < num; ++i) {
    r = (regexec(&oRegex, gn[i].c_str(), 0, NULL, 0) == 0);
    r = r || (regexec(&oRegex, gn_jp[i].c_str(), 0, NULL, 0) == 0);
    r = r || (regexec(&oRegex, gn_qp[i].c_str(), 0, NULL, 0) == 0);
    if (r) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, gn[i], &temp);
      if (false == r) {
        LOG_DEBUG2("gn : %s", gn[i].c_str());
        gn_.push_back(gn[i]);
      }
    }
  }

  return true;
}

bool HandleSearch::MatchHy() {
  std::string RegexStr;
  regex_t oRegex;
  size_t num;
  int64 temp;
  bool r;
  MapStrToInt64 CheckIfExist;

  ContainerStr hy = global::GetInfo::GetInstance()->get_hy();
  ContainerStr hy_jp = global::GetInfo::GetInstance()->get_hy_jp();
  ContainerStr hy_qp = global::GetInfo::GetInstance()->get_hy_qp();

  RegexStr = message_ + ".*";
  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }

  num = hy.size();
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, hy[i].c_str(), 0, NULL, 0) == 0) {
      hy_.push_back(hy[i]);
      LOG_DEBUG2("hy : %s", hy[i].c_str());
      CheckIfExist[hy[i]] = 1;
    }
  }
  h_num_ = hy_.size();
  if (h_num_ >= 5) return true;

  //  jp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, hy_jp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, hy[i], &temp);
      if (r == false) {
        LOG_DEBUG2("hy : %s", hy[i].c_str());
        hy_.push_back(hy[i]);
        CheckIfExist[hy[i]] = 1;
      }
    }
  }
  h_num_ = hy_.size();
  if (h_num_ >= 5) return true;

  //  qp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, hy_qp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, hy[i], &temp);
      if (r == false) {
        LOG_DEBUG2("hy : %s", hy[i].c_str());
        hy_.push_back(hy[i]);
        CheckIfExist[hy[i]] = 1;
      }
    }
  }
  h_num_ = hy_.size();
  if (h_num_ >= 5) return true;

  RegexStr = ".*";
  for (size_t i = 0; i < message_.length(); ++i) {
    RegexStr += message_[i];
    RegexStr += ".*";
  }

  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }

  for (size_t i = 0; i < num; ++i) {
    r = (regexec(&oRegex, hy[i].c_str(), 0, NULL, 0) == 0);
    r = r || (regexec(&oRegex, hy_jp[i].c_str(), 0, NULL, 0) == 0);
    r = r || (regexec(&oRegex, hy_qp[i].c_str(), 0, NULL, 0) == 0);
    if (r) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, hy[i], &temp);
      if (false == r) {
        LOG_DEBUG2("hy : %s", hy[i].c_str());
        hy_.push_back(hy[i]);
      }
    }
  }

  return true;
}

bool HandleSearch::MatchName() {
  std::string RegexStr;
  regex_t oRegex;
  size_t num;
  int64 temp;
  bool r;
  MapStrToInt64 CheckIfExist;

  ContainerStr name = global::GetInfo::GetInstance()->get_names();
  ContainerStr jp = global::GetInfo::GetInstance()->get_jp();
  ContainerStr qp = global::GetInfo::GetInstance()->get_qp();

  //  匹配股票名称
  RegexStr = message_ + ".*";
  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }

  //  name
  num = name.size();
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, name[i].c_str(), 0, NULL, 0) == 0) {
      LOG_DEBUG2("name : %s", name[i].c_str());
      name_.push_back(name[i]);
      CheckIfExist[name[i]] = 1;
    }
  }
  n_num_ = name_.size();
  if (n_num_ >= 5) return true;

  // jp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, jp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, name[i], &temp);
      if (r == false) {
         LOG_DEBUG2("name : %s", name[i].c_str());
        name_.push_back(name[i]);
        CheckIfExist[name[i]] = 1;
      }
    }
  }
  n_num_ = name_.size();
  if (n_num_ >= 5) return true;

  // qp
  for (size_t i = 0; i < num; ++i) {
    if (regexec(&oRegex, qp[i].c_str(), 0, NULL, 0) == 0) {
      r = tools::MapFind<MapStrToInt64, std::string, int64>\
          (CheckIfExist, name[i], &temp);
      if (r == false) {
         LOG_DEBUG2("name : %s", name[i].c_str());
        name_.push_back(name[i]);
        CheckIfExist[name[i]] = 1;
      }
    }
  }

  n_num_ = name_.size();
  if (n_num_ < 5) {
    RegexStr = ".*";
    for (size_t i = 0; i < message_.length(); ++i) {
      RegexStr += message_[i];
      RegexStr += ".*";
    }

    if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
      LOG_DEBUG2("message %s", "正则表达式编译出错");
      return false;
    }

    for (size_t i = 0; i < num; ++i) {
      r = (regexec(&oRegex, name[i].c_str(), 0, NULL, 0) == 0);
      r = r || (regexec(&oRegex, qp[i].c_str(), 0, NULL, 0) == 0);
      r = r || (regexec(&oRegex, jp[i].c_str(), 0, NULL, 0) == 0);
      if (r) {
        r = tools::MapFind<MapStrToInt64, std::string, int64>\
            (CheckIfExist, name[i], &temp);
        if (false == r) {
           LOG_DEBUG2("name : %s", name[i].c_str());
          name_.push_back(name[i]);
        }
      }
    }
  }

  return true;
}

bool HandleSearch::MatchCode() {
  std::string RegexStr;
  regex_t oRegex;
  size_t num;
  int64 temp;
  bool r;
  MapStrToInt64 CheckIfExist;

  ContainerStr code = global::GetInfo::GetInstance()->get_codes();

  // 匹配股票代码
  RegexStr = message_ + ".*";
  if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
    LOG_DEBUG2("message %s", "正则表达式编译出错");
    return false;
  }
  num = code.size();
  for (size_t i = 0; i < code.size(); ++i) {
    if (regexec(&oRegex, code[i].c_str(), 0, NULL, 0) == 0) {
       LOG_DEBUG2("code : %s", code[i].c_str());
      code_.push_back(code[i]);
      CheckIfExist[code[i]] = 1;
    }
  }

  c_num_ = code_.size();
  if (c_num_ < 5) {
    RegexStr = ".*";
    for (size_t i = 0; i < message_.length(); ++i) {
      RegexStr += message_[i];
      RegexStr += ".*";
    }

    if (regcomp(&oRegex, RegexStr.c_str(), 0) != 0) {
      LOG_DEBUG2("message %s", "正则表达式编译出错");
      return false;
    }

    for (size_t i = 0; i < num; ++i) {
      if (regexec(&oRegex, code[i].c_str(), 0, NULL, 0) == 0) {
        r = tools::MapFind<MapStrToInt64, std::string, int64>\
            (CheckIfExist, code[i], &temp);
        if (false == r) {
          LOG_DEBUG2("code : %s", code[i].c_str());
          code_.push_back(code[i]);
        }
      }
    }
  }

  return true;
}

bool HandleSearch::DataInMySql(time_t start_time, time_t end_time) {
  MatchGn();
  MatchHy();
  MatchName();
  MatchCode();
}

void HandleSearch::SendMessage() {
  netcomm_send::SendSearch return_search;
  MapStrToInt64 IfExist;
  size_t num;
  size_t i;
  int64 temp;
  bool r;
  ContainerStr new_name;
  std::string code_name;

  for (i = 0; i < code_.size(); ++i) {
    code_name = global::GetInfo::GetInstance()->GetNameByCode(code_[i]);
    new_name.push_back(code_name);
    IfExist[code_name] = 1;
  }

  for (i = 0; i < name_.size(); ++i) {
    r = tools::MapFind<MapStrToInt64, std::string, int64>\
        (IfExist, name_[i], &temp);
    if (r == false) {
      code_.push_back(global::GetInfo::GetInstance()->GetCodeByName(name_[i]));
      new_name.push_back(name_[i]);
    }
  }

  int code_num = code_.size();
  int hy_num = hy_.size();
  int gn_num = gn_.size();
  for (i = 0; i < 5 && (i < code_num || i < hy_num || i < gn_num); ++i) {
    if (i < code_num) {
      code_name = new_name[i] + "(" + code_[i] + ")";
      return_search.set_name(base_logic::Value::CreateStringValue(code_name));
    }

    if (i < hy_num) {
      return_search.set_hy(base_logic::Value::CreateStringValue(hy_[i]));
    }

    if (i < gn_num) {
      return_search.set_gn(base_logic::Value::CreateStringValue(gn_[i]));
    }
  }

  if (jsonp_str_ != "")  return_search.set_jsonp_callback(jsonp_str_);
  int json_type = ((jsonp_str_ == "") ? 0 : 3);

  send_message(socket_, &return_search, json_type);
}
