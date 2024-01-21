#pragma once
#include <sqlite3.h>
#include <string>


class DataBaseEngineInterface {

	virtual bool CreateDatabase(std::string Name) = 0;
	virtual bool OpenDatabase(std::string Name) = 0;
	virtual bool DataBaseExists(std::string Name) = 0;

};