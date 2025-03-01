#include<format>
#include<list>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
constexpr unsigned curl_retry{5};
std::string wiki::view(const std::string &url){
	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(url);
	std::stringstream response;
	request.setOpt<curlpp::options::WriteStream>(&response);
	request.perform();
	return response.str();
}
std::string wiki::view(const std::string &url,const std::list<std::string> &header){
	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(url);
	request.setOpt(new curlpp::options::HttpHeader{header});
	std::stringstream response;
	request.setOpt<curlpp::options::WriteStream>(&response);
	request.perform();
	return response.str();
}
nlohmann::json wiki::view_json(const std::string &url){
	unsigned retry=0;
	std::string str;
	while(str==""){
		if(retry>=curl_retry)
			throw std::invalid_argument(std::format("Failed to view `{}` for {} times! Got empty content.",url,curl_retry));
		retry++;
		str=wiki::view(url);
	}
	return nlohmann::json::parse(str);
}
nlohmann::json wiki::view_json(const std::string &url,const std::list<std::string> &header){
	unsigned retry=0;
	std::string str;
	while(str==""){
		if(retry>=curl_retry)
			throw std::invalid_argument(std::format("Failed to view `{}` for {} times! Got empty content.",url,curl_retry));
		retry++;
		str=wiki::view(url,header);
	}
	return nlohmann::json::parse(str);
}
nlohmann::json wiki::query_all(const std::string &query_prefix,const std::string &continuekey,const std::vector<std::string> &merge_key_series){
	nlohmann::json result,current=wiki::view_json(std::format("{}&format=json",query_prefix));
	while(true){
		const bool ending=current.find("continue")==current.end();
		std::string continuevalue{};
		if(!ending)
			continuevalue=current["continue"][continuekey];
		for(const auto &key:merge_key_series)
			current=current[key];
		for(const auto &item:current)
			result.push_back(item);
		if(ending)
			break;
		current=wiki::view_json(std::format(
				"{}&format=json&{}={}",
				query_prefix,
				continuekey,
				continuevalue
		));
	}
	return result;
}
nlohmann::json wiki::query_all(const std::string &query_prefix,const std::string &continuekey,const std::vector<std::string> &merge_key_series,const std::list<std::string> &header){
	nlohmann::json result,current=wiki::view_json(std::format(
		"{}&format=json",
		query_prefix
	),header);
	while(true){
		const bool ending=current.find("continue")==current.end();
		std::string continuevalue{};
		if(!ending)
			continuevalue=current["continue"][continuekey];
		for(const auto &key:merge_key_series)
			current=current[key];
		for(const auto &item:current)
			result.push_back(item);
		if(ending)
			break;
		current=wiki::view_json(std::format(
				"{}&format=json&{}={}",
				query_prefix,
				continuekey,
				continuevalue
		),header);
	}
	return result;
}
