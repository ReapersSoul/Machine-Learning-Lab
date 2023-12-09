#include "../../../Util/Exports.hpp"
#include <DataBaseEngineInterface.hpp>

class DataBaseEngine:public DataBaseEngineInterface {

};

static DataBaseEngine instance;



extern "C" {

	EXPORT void CleanUp() {
		instance.~DataBaseEngine();
	}

	EXPORT DataBaseEngineInterface* GetInstance() {
		return &instance;
	}
}