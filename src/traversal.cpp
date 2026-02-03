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
#include"page.hpp"
std::mutex mutex;
const std::string API{std::format("{}/api.php",std::getenv("WIKI_SITE_URL"))};
const std::string INDEX{std::format("{}/index.php",std::getenv("WIKI_SITE_URL"))};
void add_page(const nlohmann::json &item,const std::list<std::string> &header,std::size_t &pages_added,std::size_t &pages_total,nlohmann::json &wikijson){
	const std::string pageid=nlohmann::to_string(item["pageid"]);
	const std::string title=wiki::query_title(API,pageid,header);
	mutex.lock();
	std::clog<<std::format("[{}/{}] Processing {}...",pages_added,pages_total,title)<<std::endl;
	mutex.unlock();
	nlohmann::json categories=wiki::query_all_categories(API,pageid,header);
	const std::string page_content=wiki::raw(INDEX,title,header);
	nlohmann::json wikipage=page::gen_metadata(title,categories,page_content);
	mutex.lock();
	if(wikipage.empty()){
		--pages_total;
		std::clog<<std::format("[{}/{}] Passed {}.",pages_added,pages_total,title)<<std::endl;
	}
	else{
		++pages_added;
		wikijson+=wikipage;
		std::clog<<std::format("[{}/{}] Added {}.",pages_added,pages_total,title)<<std::endl;
	}
	mutex.unlock();
}
int main(){
	try{
		std::clog<<"Querying pages info...";
		const std::list<std::string> header{std::format("X-Byrdocs-Token:{}",std::getenv("WIKI_SITE_TOKEN"))};
		nlohmann::json allpages=wiki::query_all_pages(API,header);
		std::clog<<std::format(" Fetched {} pages.",allpages.size())<<std::endl;
		std::clog<<"Started processing pages..."<<std::endl;
		nlohmann::json wikijson;
		std::vector<std::thread> threads;
		std::size_t pages_added{0},pages_total{allpages.size()};
		for(const nlohmann::json &item:allpages)
			threads.emplace_back(add_page,item,header,std::ref(pages_added),std::ref(pages_total),std::ref(wikijson));
		for(std::thread &t:threads)
			t.join();
		std::filesystem::create_directory("./output");
		std::ofstream jsonout{"./output/wiki.json"};
		if(!jsonout.is_open())
			throw std::runtime_error{"Error: Cannot open output/wiki.json!"};
		std::clog<<"Opened output/wiki.json"<<std::endl;
		jsonout<<wikijson;
		std::clog<<std::format("Finished output. {} pages added.",pages_total)<<std::endl;
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
