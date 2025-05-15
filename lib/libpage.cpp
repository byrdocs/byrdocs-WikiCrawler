#include<exception>
#include<format>
#include<mutex>
#include<iostream>
#include<stdexcept>
#include<string>
#include<thread>
#include<vector>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
#include"page.hpp"
extern std::mutex mutex;
extern std::string API;
extern std::string INDEX;
nlohmann::json page::gen_metadata(const std::string &title,const nlohmann::json &categories,const std::string &page_content){
	bool with_source{false};
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
			if(info=="分类:计算机学院")
				wikipage["data"]["college"]+="计算机学院（国家示范性软件学院）";
			else
				wikipage["data"]["college"]+=info.substr(7);
		else if(info=="分类:有完整答案"||info=="分类:有完整可靠答案")
			wikipage["data"]["content"]+="答案";
		else if(info=="分类:本科试卷")
			wikipage["data"]["course"]+={"type","本科"};
		else if(info=="分类:研究生试卷")
			wikipage["data"]["course"]+={"type","研究生"};
		else if(title.find(info.substr(7))!=std::string::npos)
			wikipage["data"]["course"]["name"]=info.substr(7);
		else if(info=="分类:有可靠来源")
			with_source=true;
	}
	if(wikipage["data"]["course"]["name"]=="")
		return nlohmann::json{};
	if(with_source){
		const auto source_idx=page_content.find("|来源=");
		if(source_idx!=std::string::npos)
			wikipage+={"id",page_content.substr(source_idx+8,32)};
	}
	if(wikipage["data"]["college"].empty())
		wikipage["data"].erase("college");
	return wikipage;
}
