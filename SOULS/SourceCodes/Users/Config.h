#pragma once
#include "../../RapidJson/document.h"
#include "../../RapidJson/ostreamwrapper.h"
#include "../../RapidJson/prettywriter.h"
#include <string>

// �^�U�l
//doc["true"].GetBool();

// ������
//doc["hello"].GetString();

// ���l
//doc["int"].GetInt();
//doc["uint"].GetUint();
//doc["int64"].GetInt64();
//doc["uint64"].GetUint64();
//doc["double"].GetDouble();

// �I�u�W�F�N�g
//doc["object"]["hoge"].IsInt();

// �z��
//for (auto itr = doc["array"].Begin(); itr != doc["array"].End(); ++itr)
//itr->GetInt();

//for (size_t i = 0; i < doc["array"].Size(); ++i)
//doc["array"][i].GetInt();

class Config
{
public:
	//��������
	static void Writter(const std::string filename, const rapidjson::Document& doc);
};