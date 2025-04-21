#include<cassert>
#include<exception>
#include<format>
#include<iostream>
#include<sstream>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
int main(){
	try{
		std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("WIKITOKEN"))};
		curlpp::Easy request;
		wiki::init_request(
			request,
			"https://wiki.byrdocs.org/api.php?format=json",
			header,
			{{"action","query"},{"pageids","1"}}
		);
		std::string raw{wiki::get(request)};
		std::clog<<raw<<std::endl;
		nlohmann::json j=nlohmann::json::parse(raw);
		nlohmann::json expectation{
			{"batchcomplete",""},
			{"query",{
				{"pages",{
					{"1",{
						{"pageid",1},
						{"ns",0},
						{"title","首页"}
					}}
				}}
			}}
		};
		assert(j==expectation);
	}catch(const curlpp::RuntimeError &e){
		std::cerr<<e.what()<<std::endl;
		assert(false);
	}catch(const curlpp::LogicError &e){
		std::cerr<<e.what()<<std::endl;
		assert(false);
	}
}
