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

#include "measurement_exporter.h"
#include <ecal/measurement/hdf5/writer.h>

MeasurementExporter::MeasurementExporter():
  _writer()
{
}

void MeasurementExporter::setPath(const std::string& path, const std::string& base_name, const size_t& max_size_per_file)
{
  _root_output_path = EcalUtils::Filesystem::CleanPath(path);
  _output_path = EcalUtils::Filesystem::CleanPath(_root_output_path + EcalUtils::Filesystem::NativeSeparator(EcalUtils::Filesystem::OsStyle::Current) + eCALMeasCutterUtils::kDefaultFolderOutput, EcalUtils::Filesystem::OsStyle::Current);
  
  eCAL::experimental::measurement::base::WriterOptions options;
  options.file_options.path = _output_path;
  options.file_options.base_name = max_size_per_file;
  options.size_splitting = max_size_per_file;
  options.channel_splitting = eCALMeasCutterUtils::enable_one_file_per_topic;
  
  _writer = eCAL::experimental::measurement::hdf5::CreateWriter(options);

  if (!_writer)
  {
    throw ExporterException("Unable to create HDF5 protobuf output path " + path + ".");
  }
}

MeasurementExporter::~MeasurementExporter()
{
}

void MeasurementExporter::createChannel(const std::string& channel_name, const eCALMeasCutterUtils::ChannelInfo& channel_info)
{
  _current_channel_name = channel_name;
  eCAL::experimental::measurement::base::DataTypeInformation data_type_info;
  if (channel_info.format == eCALMeasCutterUtils::SerializationFormat::PROTOBUF)
  {
    data_type_info.encoding = "proto";
  }
  else
  {
    data_type_info.encoding = "";
  }
  data_type_info.name = channel_info.type;
  data_type_info.descriptor = channel_info.description;
  _writer->SetChannelDataTypeInformation(channel_name, data_type_info);
}

void MeasurementExporter::setData(eCALMeasCutterUtils::Timestamp timestamp, const eCALMeasCutterUtils::MetaData& meta_data, const std::string& payload)
{
  eCALMeasCutterUtils::MetaData::const_iterator iter;

  iter = meta_data.find(eCALMeasCutterUtils::MetaDatumKey::SENDER_TIMESTAMP);
  const auto sender_timestamp = (iter != meta_data.end()) ? iter->second.sender_timestamp : static_cast<eCALMeasCutterUtils::Timestamp>(0);

  iter = meta_data.find(eCALMeasCutterUtils::MetaDatumKey::SENDER_ID);
  const auto sender_id = (iter != meta_data.end()) ? iter->second.sender_id : 0;

  iter = meta_data.find(eCALMeasCutterUtils::MetaDatumKey::SENDER_CLOCK);
  const auto sender_clock = (iter != meta_data.end()) ? iter->second.sender_clock : 0;

  eCAL::experimental::measurement::base::WriterEntry entry{
    (void*)payload.data(), payload.size(), sender_timestamp, timestamp, eCAL::experimental::measurement::base::Channel{ _current_channel_name, sender_id }, sender_clock
  };
  _writer->AddEntryToFile(entry);
}

std::string MeasurementExporter::getOutputPath() const
{
  return _output_path;
}

std::string MeasurementExporter::getRootOutputPath() const
{
  return _root_output_path;
}