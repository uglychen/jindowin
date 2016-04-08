/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/runner/northsea/restful/plugins/news/kafka_accessor.cc
#    Author       : tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-03-12 09:48
#    Description  : 
=============================================================================*/

#include "kafka_accessor.h"
#include <assert.h>

namespace news {

std::list<kafka_producer*> KafkaAccessor::s_producer_list_;
pthread_mutex_t KafkaAccessor::s_kafka_mutext_;
int KafkaAccessor::s_partition;
std::string KafkaAccessor::s_broker;
std::string KafkaAccessor::s_topic;

int KafkaAccessor::Init(int partition, std::string host, \
    int port, std::string topic) {
  s_topic = topic;
  s_partition = partition;
  std::stringstream ss;
  ss << host << ":"<<port;
  s_broker = ss.str();
  pthread_mutex_init(&s_kafka_mutext_, NULL);
  for (int i =0; i < 3; ++i) {
    pthread_mutex_lock(&s_kafka_mutext_);
    kafka_producer *kafka_ptr = new kafka_producer();
    assert(NULL != kafka_ptr);
    if (kafka_ptr->Init(s_partition, s_topic.c_str(), \
          s_broker.c_str(), NULL) < 0) {
       pthread_mutex_unlock(&s_kafka_mutext_);
       return -1;
    }
    s_producer_list_.push_back(kafka_ptr);
    pthread_mutex_unlock(&s_kafka_mutext_);
  }
  return 0;
}

int KafkaAccessor::PushData(const char *content, const int content_length) {
  kafka_producer *producer = NULL;
  pthread_mutex_lock(&s_kafka_mutext_);
  int ret = 0;
  if (0 < s_producer_list_.size()) {
    producer = s_producer_list_.front();
    s_producer_list_.pop_front();
  } else {
    producer = new kafka_producer();
    ret = producer->Init(s_partition, s_topic.c_str(), s_broker.c_str(), NULL);
    if (0 > ret) {
      pthread_mutex_unlock(&s_kafka_mutext_);
      return ret;
    }
  }
  pthread_mutex_unlock(&s_kafka_mutext_);
  ret = producer->PushData(content, content_length);
  assert(NULL != producer);
  pthread_mutex_lock(&s_kafka_mutext_);
  s_producer_list_.push_back(producer);
  pthread_mutex_unlock(&s_kafka_mutext_);
  return ret;
}

void KafkaAccessor::Release() {
  pthread_mutex_lock(&s_kafka_mutext_);
  for (std::list<kafka_producer*>::iterator it = s_producer_list_.begin();
      it != s_producer_list_.end();
      ++it) {
    (*it)->Close();
    delete (*it);
  }

  pthread_mutex_unlock(&s_kafka_mutext_);
}

}  // namespace news end
