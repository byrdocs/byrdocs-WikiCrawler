#include<exception>
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
std::mutex mutex;
const std::string API{"https://wiki.byrdocs.org/api.php"};
void add_page(const nlohmann::json &item,const std::list<std::string> &header,std::size_t &pages_added,const std::size_t &pages_total,nlohmann::json &wikijson){
	const std::string pageid=nlohmann::to_string(item["pageid"]);
	const std::string title=wiki::query_title(API,header,pageid);
	mutex.lock();
	std::clog<<std::format("[{}/{}] Processing {}... ",pages_added,pages_total,title)<<std::endl;
	mutex.unlock();
	nlohmann::json categories=wiki::query_all_categories(API,header,pageid);
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
		mutex.lock();
		pages_added++;
		std::clog<<std::format("[{}/{}] Passed {}.",pages_added,pages_total,title)<<std::endl;
		mutex.unlock();
		return;
	}
	const std::string page_content=wiki::raw(API,header,title);
	const auto source_idx=page_content.find("{{来源|");
	if(source_idx!=std::string::npos)
		wikipage+={"id",page_content.substr(source_idx+9,32)};
	if(wikipage["data"]["college"].empty())
		wikipage["data"].erase("college");
	mutex.lock();
	wikijson+=wikipage;
	pages_added++;
	std::clog<<std::format("[{}/{}] Added {}.",pages_added,pages_total,title)<<std::endl;
	mutex.unlock();
}
int main(){
	try{
		std::clog<<"Querying pages info...";
		const std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("WIKITOKEN"))};
		nlohmann::json allpages=wiki::query_all_pages(API,header);
		std::clog<<std::format(" Fetched {} pages.",allpages.size())<<std::endl;
		std::clog<<"Started processing pages..."<<std::endl;
		nlohmann::json wikijson;
		std::vector<std::thread> threads;
		std::size_t pages_added{0};
		for(const nlohmann::json &item:allpages)
			threads.emplace_back(add_page,item,header,std::ref(pages_added),allpages.size(),std::ref(wikijson));
		for(std::thread &t:threads)
			t.join();
		std::filesystem::create_directory("./output");
		std::ofstream jsonout{"./output/wiki.json"};
		if(!jsonout.is_open())
			throw std::runtime_error{"Error: Cannot open output/wiki.json!"};
		std::clog<<"Opened output/wiki.json"<<std::endl;
		jsonout<<wikijson;
		std::clog<<"Finished output."<<std::endl;
		jsonout.close();
	}catch(const curlpp::RuntimeError &e){
		std::cerr<<e.what()<<std::endl;
		return 1;
	}catch(const curlpp::LogicError &e){
		std::cerr<<e.what()<<std::endl;
		return 1;
	}catch(const std::exception &e){
		std::cerr<<e.what()<<std::endl;
		return 1;
	}
	return 0;
}
