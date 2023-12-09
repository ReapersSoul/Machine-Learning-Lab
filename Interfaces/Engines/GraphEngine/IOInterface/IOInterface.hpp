#pragma once
#include "../Types/Types.h"

class IOInterface {
	unsigned int UID;
	std::string TypeID;
	IOObjectCollection Data;
};