#pragma once
#include<list>
#include<string>
#include<curlpp/Easy.hpp>
#include<nlohmann/json.hpp>
namespace wiki{
std::string get(curlpp::Easy &request);
std::string raw(const std::string &api,const std::string &title,const std::list<std::string> &header={});
void init_request(curlpp::Easy &request,const std::string &url,const std::map<std::string,std::string> &formstr,const std::list<std::string> &header={});
nlohmann::json query_all_pages(const std::string &api,const std::list<std::string> &header={});
std::string query_title(const std::string &api,const std::string &pageid,const std::list<std::string> &header={});
nlohmann::json query_all_categories(const std::string &api,const std::string &pageid,const std::list<std::string> &header={});
}
