#include<format>
#include<list>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
constexpr unsigned curl_retry{3};
std::string wiki::get(curlpp::Easy &request){
	std::stringstream response;
	request.setOpt<curlpp::options::WriteStream>(&response);
	request.perform();
	return response.str();
}
std::string wiki::raw(const std::string &api,const std::string &title,const std::list<std::string> &header){
	curlpp::Easy request;
	init_request(
		request,
		api,
		{{"title",title},{"action","raw"}},
		header
	);
	return get(request);
}
void wiki::init_request(curlpp::Easy &request,const std::string &url,const std::map<std::string,std::string> &formstr,const std::list<std::string> &header){
	request.setOpt<curlpp::Options::Url>(url);
	request.setOpt<curlpp::Options::HttpHeader>(header);
	curlpp::Forms form;
	for(const auto &it:formstr)
		form.push_back(new curlpp::FormParts::Content(it.first,it.second));
	request.setOpt<curlpp::Options::HttpPost>(form);
}
nlohmann::json wiki::query_all_pages(const std::string &api,const std::list<std::string> &header){
	nlohmann::json result;
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"list","allpages"}},
		header
	);
	nlohmann::json current=nlohmann::json::parse(wiki::get(request));
	while(true){
		const bool ending=current.find("continue")==current.end();
		for(const auto &item:current["query"]["allpages"])
			result.push_back(item);
		if(ending)
			break;
		curlpp::Easy request;
		init_request(
			request,
			api+"?format=json",
			{{"action","query"},{"list","allpages"},{"apcontinue",current["continue"]["apcontinue"]}},
			header
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
std::string wiki::query_title(const std::string &api,const std::string &pageid,const std::list<std::string> &header){
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"pageids",pageid}},
		header
	);
	nlohmann::json result=nlohmann::json::parse(wiki::get(request));
	return result["query"]["pages"][pageid]["title"];
}
nlohmann::json wiki::query_all_categories(const std::string &api,const std::string &pageid,const std::list<std::string> &header){
	nlohmann::json result;
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"prop","categories"},{"pageids",pageid}},
		header
	);
	nlohmann::json current=nlohmann::json::parse(wiki::get(request));
	while(true){
		const bool ending=current.find("continue")==current.end();
		for(const auto &item:current["query"]["pages"][pageid]["categories"])
			result.push_back(item);
		if(ending)
			break;
		std::string continuevalue{current["continue"]["clcontinue"]};
		curlpp::Easy request;
		init_request(
			request,
			api+"?format=json",
			{{"action","query"},{"prop","categories"},{"pageids",pageid},{"clcontinue",current["continue"]["clcontinue"]}},
			header
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
