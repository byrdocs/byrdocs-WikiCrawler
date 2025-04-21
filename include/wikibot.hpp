#pragma once
#include<list>
#include<string>
#include<curlpp/Easy.hpp>
#include<nlohmann/json.hpp>
namespace wiki{
std::string get(curlpp::Easy &request);
std::string raw(const std::string &api,const std::string &title);
std::string raw(const std::string &api,const std::list<std::string> &header,const std::string &title);
void init_request(curlpp::Easy &request,const std::string &url,const std::map<std::string,std::string> &formstr);
void init_request(curlpp::Easy &request,const std::string &url,const std::list<std::string> &header,const std::map<std::string,std::string> &formstr);
nlohmann::json query_all_pages(const std::string &api);
nlohmann::json query_all_pages(const std::string &api,const std::list<std::string> &header);
std::string query_title(const std::string &api,const std::string &pageid);
std::string query_title(const std::string &api,const std::list<std::string> &header,const std::string &pageid);
nlohmann::json query_all_categories(const std::string &api,const std::string &pageid);
nlohmann::json query_all_categories(const std::string &api,const std::list<std::string> &header,const std::string &pageid);
}
