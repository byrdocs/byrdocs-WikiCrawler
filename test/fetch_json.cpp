#include<cassert>
#include<exception>
#include<iostream>
#include<sstream>
#include<string>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
int main(){
	try{
		std::string raw{wiki::view("http://localhost/api.php?format=json&action=query&pageids=1")};
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
