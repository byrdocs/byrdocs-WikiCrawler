#include<cstdlib>
#include<format>
#include<iostream>
#include<sstream>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include"wikibot.hpp"
int main(){
	const std::string expectation{
R"(:'''此页面'''仅供 ''API'' 进行测试，请勿轻易修改其中内容。
<math>\int\frac12\,\mathrm dx</math>

# [[t͡sʓɰǂỹ]]

* [[首页|Main]])"};
	try{
		std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("WIKI_SITE_TOKEN"))};
		curlpp::Easy request;
		wiki::init_request(
			request,
			std::format("{}/index.php?",std::getenv("WIKI_SITE_URL")),
			{{"title","用户:大量更改机器人/沙盒/Api_View"},{"action","raw"}},
			header
		);
		std::string result{wiki::get(request)};
		std::clog<<result<<std::endl;
		assert(expectation==result);
	}catch(const curlpp::RuntimeError &e){
		std::cout<<e.what()<<std::endl;
		assert(false);
	}catch(const curlpp::LogicError &e){
		std::cout<<e.what()<<std::endl;
		assert(false);
	}catch(const std::exception &e){
		std::cout<<e.what()<<std::endl;
		assert(false);
	}
	return 0;
}
