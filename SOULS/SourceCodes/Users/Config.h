#pragma once
#include "../../RapidJson/document.h"
#include "../../RapidJson/ostreamwrapper.h"
#include "../../RapidJson/prettywriter.h"
#include <string>

// 真偽値
//doc["true"].GetBool();

// 文字列
//doc["hello"].GetString();

// 数値
//doc["int"].GetInt();
//doc["uint"].GetUint();
//doc["int64"].GetInt64();
//doc["uint64"].GetUint64();
//doc["double"].GetDouble();

// オブジェクト
//doc["object"]["hoge"].IsInt();

// 配列
//for (auto itr = doc["array"].Begin(); itr != doc["array"].End(); ++itr)
//itr->GetInt();

//for (size_t i = 0; i < doc["array"].Size(); ++i)
//doc["array"][i].GetInt();

class Config
{
public:
	//書き込み
	static void Writter(const std::string filename, const rapidjson::Document& doc);
};