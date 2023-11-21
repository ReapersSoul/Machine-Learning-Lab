#include <DataBaseEngineInterface.h>

class DataBaseEngine:public DataBaseEngineInterface {

};

static DataBaseEngine instance;

extern "C" {

	__declspec(dllexport) void CleanUp() {
		instance.~DataBaseEngine();
	}

	__declspec(dllexport) DataBaseEngineInterface* GetInstance() {
		return &instance;
	}
}