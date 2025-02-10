#pragma once
#include<string>
#include<curlpp/Easy.hpp>
#include<nlohmann/json.hpp>
namespace wiki{
std::string view(const std::string &url);
nlohmann::json query_all(const std::string &query_prefix,const std::string &mergekey,const std::string &continuekey);
}
