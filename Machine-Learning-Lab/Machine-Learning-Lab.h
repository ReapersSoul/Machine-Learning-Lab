#pragma once

#include <boost/dll.hpp>
#include <iostream>
#include <string>
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <UIEngineInterface.hpp>
#include <SerializableInterface.hpp>

#include <NetworkEngineInterface.hpp>

#include <filesystem>
#include <thread>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#define SYSERROR()  GetLastError()
#else
#include <errno.h>
#define SYSERROR()  errno
#endif
