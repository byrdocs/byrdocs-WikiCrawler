#include<format>
#include<fstream>
#include<iostream>
#include<stdexcept>
#include<string>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
int main(){
	std::ofstream jsonout{"./wiki.json"};
	try{
		if(!jsonout.is_open())
			throw std::runtime_error{"Cannot open wiki.json!"};
		const std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("wikitoken"))};
		nlohmann::json allpages=wiki::query_all("https://wiki.byrdocs.org/api.php?action=query&list=allpages","apcontinue",{"query","allpages"},header);
		nlohmann::json wikijson;
		for(const auto &item:allpages){
			const std::string pageid=nlohmann::to_string(item["pageid"]);
			const std::string title=nlohmann::json::parse(wiki::view(std::format(
				"https://wiki.byrdocs.org/api.php?format=json&action=query&pageids={}",
				pageid
			),header))["query"]["pages"][pageid]["title"];
			nlohmann::json categories=wiki::query_all(std::format(
				"https://wiki.byrdocs.org/api.php?action=query&prop=categories&&pageids={}",
				pageid
			),"clcontinue",{"query","pages",pageid,"categories"},header);
			nlohmann::json wikipage{
				{"url",std::format("https://wiki.byrdocs.org/w/{}",title)},
				{"type","test"},
				{"data",{
					{"college",{}},
					{"course",{
						{"name",""}
					}},
					{"time",{
						{"start",""},
						{"end",""},
						{"semester",""},
						{"stage",""}
					}},
					{"filetype","wiki"},
					{"content",{"原题"}},
				}},
			};
			for(const auto &item:categories){
				const std::string info=item["title"];
				if(info.ends_with("学期")){
					wikipage["data"]["time"]["start"]=info.substr(7,4);
					wikipage["data"]["time"]["end"]=info.substr(12,4);
					wikipage["data"]["time"]["semester"]=info.substr(25,3)=="一"?"First":"Second";
				}
				else if(info=="分类:期中")
					wikipage["data"]["time"]["stage"]="期中";
				else if(info=="分类:期末")
					wikipage["data"]["time"]["stage"]="期末";
				else if(info.ends_with("学院"))
					wikipage["data"]["college"]+=info.substr(7);
				else if(info=="分类:有完整答案")
					wikipage["data"]["content"]+="答案";
				else if(info=="分类:本科课程")
					wikipage["data"]["course"]+={"type","本科"};
				else if(info=="分类:研究生课程")
					wikipage["data"]["course"]+={"type","研究生"};
				else if(title.find(info.substr(7))!=std::string::npos)
					wikipage["data"]["course"]["name"]=info.substr(7);
			}
			const std::string page_content=wiki::view(std::format(
				"https://wiki.byrdocs.org/index.php?title={}&action=raw",
				title
			),header);
			const auto source_idx=page_content.find("{{来源|");
			if(source_idx!=std::string::npos)
				wikipage+={"id",page_content.substr(source_idx+9,32)};
			if(wikipage["data"]["course"]["name"]!="")
				wikijson+=wikipage;
		}
		jsonout<<wikijson;
	}catch(const curlpp::RuntimeError &e){
		std::cerr<<e.what()<<std::endl;
	}catch(const curlpp::LogicError &e){
		std::cerr<<e.what()<<std::endl;
	}catch(const std::exception &e){
		std::cerr<<e.what()<<std::endl;
	}
	jsonout.close();
	return 0;
}
