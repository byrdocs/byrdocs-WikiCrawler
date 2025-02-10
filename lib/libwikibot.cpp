#include<format>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
std::string wiki::view(const std::string &url){
	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(url);
	std::stringstream response;
	request.setOpt<curlpp::options::WriteStream>(&response);
	request.perform();
	return response.str();
}
nlohmann::json wiki::query_all(const std::string &query_prefix,const std::string &mergekey,const std::string &continuekey){
	nlohmann::json result,current=nlohmann::json::parse(wiki::view(std::format(
		"{}&format=json&formatrevision=2",
		query_prefix
	)));
	while(true){
		for(auto item:current["query"][mergekey])
			result.push_back(item);
		if(current.find("continue")==current.end())
			break;
		std::string continuevalue=current["continue"][continuekey];
		current=nlohmann::json::parse(wiki::view(std::format(
				"{}&format=json&formatrevision=2&{}={}",
				query_prefix,
				continuekey,
				continuevalue
		)));
	}
	return result;
}
