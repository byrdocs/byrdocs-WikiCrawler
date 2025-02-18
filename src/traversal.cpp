#include<filesystem>
#include<format>
#include<fstream>
#include<iostream>
#include<mutex>
#include<stdexcept>
#include<string>
#include<thread>
#include<vector>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
void add_page(const nlohmann::json &item,const std::list<std::string> &header,std::ofstream &jsonout,bool &comma){
	const std::string pageid=nlohmann::to_string(item["pageid"]);
	const std::string title=nlohmann::json::parse(wiki::view(std::format(
		"https://wiki.byrdocs.org/api.php?format=json&action=query&pageids={}",
		pageid
	),header))["query"]["pages"][pageid]["title"];
	std::clog<<std::format("Started processing {}... ",title)<<std::endl;
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
	if(wikipage["data"]["course"]["name"]==""){
		std::clog<<std::format("Passed {}.",title)<<std::endl;
		return;
	}
	const std::string page_content=wiki::view(std::format(
		"https://wiki.byrdocs.org/index.php?title={}&action=raw",
		title
	),header);
	const auto source_idx=page_content.find("{{来源|");
	if(source_idx!=std::string::npos)
		wikipage+={"id",page_content.substr(source_idx+9,32)};
	if(wikipage["data"]["college"].empty())
		wikipage["data"].erase("college");
	if(!comma)
		comma=true;
	else
		jsonout<<',';
	jsonout<<wikipage;
	std::clog<<std::format("Added {}.",title)<<std::endl;
}
int main(){
	try{
		std::filesystem::create_directory("./output");
		std::ofstream jsonout{"./output/wiki.json"};
		if(!jsonout.is_open())
			throw std::runtime_error{"Cannot open output/wiki.json!"};
		std::clog<<"Opened output/wiki.json..."<<std::endl;
		const std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("WIKITOKEN"))};
		nlohmann::json allpages=wiki::query_all("https://wiki.byrdocs.org/api.php?action=query&list=allpages","apcontinue",{"query","allpages"},header);
		jsonout<<"[";
		bool comma=false;
		std::vector<std::thread> threads;
		for(const nlohmann::json &item:allpages)
			threads.emplace_back(add_page,item,header,std::ref(jsonout),std::ref(comma));
		for(std::thread &t:threads)
			t.join();
		jsonout<<"]";
		std::clog<<"Finished output."<<std::endl;
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
