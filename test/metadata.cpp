#include<cassert>
#include<exception>
#include<iostream>
#include<string>
#include<nlohmann/json.hpp>
#include"page.hpp"
const std::string title{"23-24-2-高等数学A（下）-期末"};
const nlohmann::json categories{
	{
		{"ns","14"},
		{"title","分类:2023-2024学年第二学期"},
	},{
		{"ns","14"},
		{"title","分类:数字媒体与设计艺术学院"},
	},{
		{"ns","14"},
		{"title","分类:智能工程与自动化学院"},
	},{
		{"ns","14"},
		{"title","分类:有可靠来源"},
	},{
		{"ns","14"},
		{"title","分类:期末"},
	},{
		{"ns","14"},
		{"title","分类:本科试卷"},
	},{
		{"ns","14"},
		{"title","分类:电子工程学院"},
	},{
		{"ns","14"},
		{"title","分类:网络空间安全学院"},
	},{
		{"ns","14"},
		{"title","分类:计算机学院"},
	},{
		{"ns","14"},
		{"title","分类:高等数学A（下）"},
	},{
		{"ns","14"},
		{"title","分类:有完整可靠答案"},
	}
};
const std::string page_content={R"(
{{Infobox
|时间=2023-2024学年第二学期
|阶段=期末
|类型=本科
|科目=高等数学A（下）
|学院=电子工程学院,计算机学院,数字媒体与设计艺术学院,网络空间安全学院,智能工程与自动化学院
|来源=709413e17d8e3a28849c057b656a4db1
|答案完成度=完整可靠
}}

== 一 填空题 (每小题 3 分, 共 30 分) ==
)"};
const std::string expectation={R"(
{
	"id":"709413e17d8e3a28849c057b656a4db1",
	"type":"test",
	"url":"https://wiki.byrdocs.org/w/23-24-2-高等数学A（下）-期末",
	"data":{
		"filetype":"wiki",
		"time":{
			"start":"2023",
			"end":"2024",
			"semester":"Second",
			"stage":"期末"
		},
		"course":{
			"type":"本科",
			"name":"高等数学A（下）"
		},
		"college":[
			"数字媒体与设计艺术学院",
			"智能工程与自动化学院",
			"电子工程学院",
			"网络空间安全学院",
			"计算机学院（国家示范性软件学院）"
		],
		"content":[
			"原题",
			"答案"
		]
	}
}
)"};
int main(){
	try{
		const nlohmann::json wikipage=page::gen_metadata(title,categories,page_content);
		const nlohmann::json expectation_json=nlohmann::json::parse(expectation);
		assert(wikipage==expectation_json);
	}catch(const std::exception &e){
		std::cerr<<e.what()<<std::endl;
		assert(false);
	}
}
