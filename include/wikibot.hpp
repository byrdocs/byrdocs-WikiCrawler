#pragma once
#include<list>
#include<string>
#include<curlpp/Easy.hpp>
#include<nlohmann/json.hpp>
namespace wiki{
std::string view(const std::string &url);
std::string view(const std::string &url,const std::list<std::string> &header);
nlohmann::json view_json(const std::string &url);
nlohmann::json view_json(const std::string &url,const std::list<std::string> &header);
nlohmann::json query_all(const std::string &query_prefix,const std::string &continuekey,const std::vector<std::string> &merge_key_series);
nlohmann::json query_all(const std::string &query_prefix,const std::string &continuekey,const std::vector<std::string> &merge_key_series,const std::list<std::string> &header);
}
