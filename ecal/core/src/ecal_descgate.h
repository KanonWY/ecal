/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
*/

/**
 * @brief  eCAL description gateway class
**/

#pragma once

#include <ecal/ecal_types.h>
#include <ecal/ecal_util.h>

#include "serialization/ecal_struct_sample_registration.h"
#include "util/ecal_expmap.h"

#include <chrono>
#include <map>
#include <mutex>
#include <string>

namespace eCAL
{
  struct STopicIdKey
  {
    std::string   topic_name;
    Util::TopicId topic_id;

    bool operator<(const STopicIdKey& other) const
    {
      if (topic_name != other.topic_name)
        return topic_name < other.topic_name;
      return topic_id < other.topic_id;
    }
  };

  struct SServiceIdKey
  {
    std::string     service_name;
    std::string     method_name;
    Util::ServiceId service_id;

    bool operator<(const SServiceIdKey& other) const
    {
      if (service_name != other.service_name)
        return service_name < other.service_name;
      if (service_id != other.service_id)
        return service_id < other.service_id;
      return method_name < other.method_name;
    }
  };

  using QualityTopicIdMap   = std::map<STopicIdKey,   Util::SQualityTopicInfo>;
  using QualityServiceIdMap = std::map<SServiceIdKey, Util::SQualityServiceInfo>;

  class CDescGate
  {
  public:
    CDescGate(const std::chrono::milliseconds& exp_timeout_);
    ~CDescGate();

    // apply samples to description gate
    void ApplySample(const Registration::Sample& sample_, eTLayerType layer_);

    // get publisher/subscriber maps
    QualityTopicIdMap GetPublishers();
    QualityTopicIdMap GetSubscribers();

    // get service/clients maps
    QualityServiceIdMap GetServices();
    QualityServiceIdMap GetClients();

    // delete copy constructor and copy assignment operator
    CDescGate(const CDescGate&) = delete;
    CDescGate& operator=(const CDescGate&) = delete;

    // delete move constructor and move assignment operator
    CDescGate(CDescGate&&) = delete;
    CDescGate& operator=(CDescGate&&) = delete;

  protected:
    using QualityTopicIdExpMap = eCAL::Util::CExpMap<STopicIdKey, Util::SQualityTopicInfo>;
    struct SQualityTopicIdMap
    {
      explicit SQualityTopicIdMap(const std::chrono::milliseconds& timeout_) : map(timeout_) {};
      mutable std::mutex   mtx;
      QualityTopicIdExpMap map;
    };

    using QualityServiceIdExpMap = eCAL::Util::CExpMap<SServiceIdKey, Util::SQualityServiceInfo>;
    struct SQualityServiceIdMap
    {
      explicit SQualityServiceIdMap(const std::chrono::milliseconds& timeout_) : map(timeout_) {};
      mutable std::mutex     mtx;
      QualityServiceIdExpMap map;
    };

    static QualityTopicIdMap   GetTopics(SQualityTopicIdMap& topic_map_);
    static QualityServiceIdMap GetServices(SQualityServiceIdMap& service_method_info_map_);

    static void ApplyTopicDescription(SQualityTopicIdMap& topic_info_map_,
                                      const std::string& topic_name_,
                                      const Util::TopicId& topic_id_,
                                      const SDataTypeInformation& topic_info_,
                                      Util::DescQualityFlags topic_quality_);

    static void RemTopicDescription(SQualityTopicIdMap& topic_info_map_,
                                    const std::string& topic_name_,
                                    const Util::TopicId& topic_id_);

    static void ApplyServiceDescription(SQualityServiceIdMap& service_method_info_map_,
                                        const std::string& service_name_,
                                        const std::string& method_name_,
                                        const Util::ServiceId& service_id_,
                                        const SDataTypeInformation& request_type_information_,
                                        const SDataTypeInformation& response_type_information_,
                                        Util::DescQualityFlags request_type_quality_,
                                        Util::DescQualityFlags response_type_quality_);

    static void RemServiceDescription(SQualityServiceIdMap& service_method_info_map_,
                                      const std::string& service_name_,
                                      const Util::ServiceId& service_id_);

    // internal quality topic info publisher/subscriber maps
    SQualityTopicIdMap   m_publisher_info_map;
    SQualityTopicIdMap   m_subscriber_info_map;

    // internal quality service info service/client maps
    SQualityServiceIdMap m_service_info_map;
    SQualityServiceIdMap m_client_info_map;
  };
}
