#include<iostream>
#include<sstream>
#include<string>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include"wikibot.hpp"
int main(int argc,char *argv[]){
	const std::string expectation{
R"(:'''此页面'''仅供 ''API'' 进行测试，请勿轻易修改其中内容。
<math>\int\frac12\,\mathrm dx</math>

# [[t͡sʓɰǂỹ]]

* [[首页|Main]])"};
	try{
		std::string result{wiki::view("localhost:80/index.php?title=用户:大量更改机器人/沙盒/Api_View&action=raw")};
		std::clog<<result<<std::endl;
		assert(expectation==result);
	}catch(curlpp::RuntimeError &e){
		std::cout<<e.what()<<std::endl;
		assert(false);
	}catch(curlpp::LogicError &e){
		std::cout<<e.what()<<std::endl;
		assert(false);
	}
	return 0;
}
