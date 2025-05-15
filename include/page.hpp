#include<string>
#include<nlohmann/json.hpp>
#include"wikibot.hpp"
namespace page{
nlohmann::json gen_metadata(const std::string &title,const nlohmann::json &categories,const std::string &page_content);
}
