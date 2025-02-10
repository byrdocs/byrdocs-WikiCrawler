#include<format>
#include<fstream>
#include<iostream>
#include<stdexcept>
#include<string>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
int main(){
	try{
		std::ofstream jsonout{"./wiki.json"};
		if(!jsonout.is_open())
			throw std::runtime_error{"Cannot open wiki.json!"};
		nlohmann::json allpages=wiki::query_all("http://wiki.byrdocs.org/api.php?action=query&list=allpages","apcontinue",{"query","allpages"});
		nlohmann::json wikijson;
		for(const auto &item:allpages){
			const std::string pageid=nlohmann::to_string(item["pageid"]);
			const std::string title=nlohmann::json::parse(wiki::view(std::format(
				"http://wiki.byrdocs.org/api.php?format=json&action=query&pageids={}",
				pageid
			)))["query"]["pages"][pageid]["title"];
			nlohmann::json categories=wiki::query_all(std::format(
				"http://wiki.byrdocs.org/api.php?action=query&prop=categories&&pageids={}",
				pageid
			),"clcontinue",{"query","pages",pageid,"categories"});
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
				"http://wiki.byrdocs.org/index.php?title={}&action=raw",
				title
			));
			const auto source_idx=page_content.find("{{来源|");
			if(source_idx!=std::string::npos)
				wikipage+={"id",page_content.substr(source_idx+9,32)};
			wikijson+=wikipage;
		}
		jsonout<<wikijson;
		jsonout.close();
	}catch(const curlpp::RuntimeError &e){
		std::cerr<<e.what()<<std::endl;
	}catch(const curlpp::LogicError &e){
		std::cerr<<e.what()<<std::endl;
	}catch(const std::exception &e){
		std::cerr<<e.what()<<std::endl;
	}
	return 0;
}
