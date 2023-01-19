#include "Config.h"
#include <fstream>

void Config::Writter(const std::string filename, const rapidjson::Document& doc)
{
	const std::string l_basepath = "Resources/Config/";
	const std::string l_extension = ".json";

	std::ofstream ofs(l_basepath + filename + l_extension);
	rapidjson::OStreamWrapper osw(ofs);

	//改行書き込み
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	doc.Accept(writer);
}
