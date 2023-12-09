#include "../../../Util/Exports.hpp"
#include <DebugEngineInterface.hpp>
#include <vector>

class DebugEngine : public DebugEngineInterface
{

public:
	

	DebugEngine() {
		Name = "DebugEngine";
	}
};

static DebugEngine instance;



extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT DebugEngine* GetInstance() {
		return &instance;
	}
}