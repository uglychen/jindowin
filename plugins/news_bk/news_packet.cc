//  Copyright (c) 2015-2015 The news_packet.cc Authors. All rights reserved.
//  Created on: 2016年1月12日 Author: paco
#include "news/news_packet.h"
#include "net/error_comm.h"
#include <algorithm>
using namespace std;

//------------------------- recv -------------------------------
void StockNewsRecv::Init() {
  bool r = false;
  do {
    std::string code;
    r = m_->GetString(L"stock_code", &code);
    base::BasicUtil::UrlDecode(code, stock_code_);
    stock_code_ = stock_code_.substr(0, stock_code_.length() - 1);
    r = m_->GetBigInteger(L"news_type", &news_type_);
    r = m_->GetBigInteger(L"count", &count_);
    std::string name;
    r = m_->GetString(L"hy_name", &name);
    base::BasicUtil::UrlDecode(name, hy_name_);
    r = m_->GetBigInteger(L"page", &page_);
    if (page_ <= -1)
      page_ = 0;
  } while (0);
}

void NewsAffectRecv::Init() {
  bool r = false;
  do {
    // r = m_->GetBigInteger(L"date", &date_);
    std::string date;
    r = m_->GetString(L"date", &date);
    base::BasicUtil::UrlDecode(date, date_);
    r = m_->GetString(L"news_id", &news_id_);
    r = m_->GetBigInteger(L"commit_type", &commit_type_);
  } while (0);
}

void NewsDetailRecv::Init() {
  m_->GetString(L"news_id", &news_id_);
  std::string date;
  m_->GetString(L"date", &date);
  base::BasicUtil::UrlDecode(date, date_);
}

void NewsShareRecv::Init() {
  std::string tem;
  m_->GetString(L"subject", &tem);
  base::BasicUtil::UrlDecode(tem, subject_);

  tem.clear();
  m_->GetString(L"content", &tem);
  base::BasicUtil::UrlDecode(tem, content_);

  tem.clear();
  m_->GetString(L"receiver", &tem);
  base::BasicUtil::UrlDecode(tem, receiver_);

  tem.clear();
  m_->GetString(L"url", &tem);
  base::BasicUtil::UrlDecode(tem, url_);
}

void NewsTransmitRecv::Init() {
  m_->GetString(L"news_id", &news_id_);
  m_->GetString(L"news_date", &news_date_);
}

//--------------------------------------- send ------------------------

std::string StockNewsSend::GetFirstNewsTime() {
  std::string newsdate;
  base_logic::Value *value_news = NULL;
  news_->Get(0, &value_news);
  if (NULL != value_news) {
    base_logic::DictionaryValue* dic_news = \
        dynamic_cast<base_logic::DictionaryValue*>(value_news);
    if (NULL != dic_news) {
      dic_news->GetString(L"time", &newsdate);
    }
  }
  return newsdate;
}

bool StockNewsSend::IsAdded(std::string news_id) {
  std::vector<std::string>::iterator it = \
       find(vec_news_added_.begin(), vec_news_added_.end(), news_id);
  return (it != vec_news_added_.end());
}
