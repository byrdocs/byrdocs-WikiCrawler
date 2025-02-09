#include<sstream>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include"wikibot.hpp"
std::string wiki::view(const std::string &url){
	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(url);
	std::stringstream response;
	request.setOpt<curlpp::options::WriteStream>(&response);
	request.perform();
	return response.str();
}
