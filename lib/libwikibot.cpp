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
std::string wiki::raw(const std::string &api,const std::string &title){
	curlpp::Easy request;
	init_request(
		request,
		api,
		{{"title",title},{"action","raw"}}
	);
	return get(request);
}
std::string wiki::raw(const std::string &api,const std::list<std::string> &header,const std::string &title){
	curlpp::Easy request;
	init_request(
		request,
		api,
		header,
		{{"title",title},{"action","raw"}}
	);
	return get(request);
}
void wiki::init_request(curlpp::Easy &request,const std::string &url,const std::map<std::string,std::string> &formstr){
	request.setOpt<curlpp::Options::Url>(url);
	curlpp::Forms form;
	for(const auto &it:formstr)
		form.push_back(new curlpp::FormParts::Content(it.first,it.second));
	request.setOpt<curlpp::Options::HttpPost>(form);
}
void wiki::init_request(curlpp::Easy &request,const std::string &url,const std::list<std::string> &header,const std::map<std::string,std::string> &formstr){
	request.setOpt<curlpp::Options::Url>(url);
	request.setOpt<curlpp::Options::HttpHeader>(header);
	curlpp::Forms form;
	for(const auto &it:formstr)
		form.push_back(new curlpp::FormParts::Content(it.first,it.second));
	request.setOpt<curlpp::Options::HttpPost>(form);
}
nlohmann::json wiki::query_all_pages(const std::string &api){
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"list","allpages"}}
	);
	nlohmann::json current=nlohmann::json::parse(wiki::get(request)),result;
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
			{{"action","query"},{"list","allpages"},{"apcontinue",current["continue"]["apcontinue"]}}
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
nlohmann::json wiki::query_all_pages(const std::string &api,const std::list<std::string> &header){
	nlohmann::json result;
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		header,
		{{"action","query"},{"list","allpages"}}
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
			header,
			{{"action","query"},{"list","allpages"},{"apcontinue",current["continue"]["apcontinue"]}}
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
std::string wiki::query_title(const std::string &api,const std::string &pageid){
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"pageids",pageid}}
	);
	nlohmann::json result=nlohmann::json::parse(wiki::get(request));
	return result["query"]["pages"][pageid]["title"];
}
std::string wiki::query_title(const std::string &api,const std::list<std::string> &header,const std::string &pageid){
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		header,
		{{"action","query"},{"pageids",pageid}}
	);
	nlohmann::json result=nlohmann::json::parse(wiki::get(request));
	return result["query"]["pages"][pageid]["title"];
}
nlohmann::json wiki::query_all_categories(const std::string &api,const std::string &pageid){
	nlohmann::json result;
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		{{"action","query"},{"prop","categories"},{"pageids",pageid}}
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
			{{"action","query"},{"prop","categories"},{"pageids",pageid},{"clcontinue",current["continue"]["clcontinue"]}}
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
nlohmann::json wiki::query_all_categories(const std::string &api,const std::list<std::string> &header,const std::string &pageid){
	nlohmann::json result;
	curlpp::Easy request;
	init_request(
		request,
		api+"?format=json",
		header,
		{{"action","query"},{"prop","categories"},{"pageids",pageid}}
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
			header,
			{{"action","query"},{"prop","categories"},{"pageids",pageid},{"clcontinue",current["continue"]["clcontinue"]}}
		);
		current=nlohmann::json::parse(wiki::get(request));
	}
	return result;
}
