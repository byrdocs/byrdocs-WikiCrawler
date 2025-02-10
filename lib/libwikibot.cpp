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
nlohmann::json wiki::query_all(const std::string &query_prefix,const std::string &continuekey,const std::vector<std::string> &merge_key_series){
	nlohmann::json result,current=nlohmann::json::parse(wiki::view(std::format(
		"{}&format=json&formatrevision=2",
		query_prefix
	)));
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
		current=nlohmann::json::parse(wiki::view(std::format(
				"{}&format=json&formatrevision=2&{}={}",
				query_prefix,
				continuekey,
				continuevalue
		)));
	}
	return result;
}
