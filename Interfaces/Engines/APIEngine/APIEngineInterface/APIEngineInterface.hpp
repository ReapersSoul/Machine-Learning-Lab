#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <APINodeInterface.h>

//boost
#include <boost/beast.hpp> //boost beast rest api
#include <boost/asio.hpp> //boost beast rest api
#include <boost/asio/ssl.hpp> //boost beast rest api
#include <boost/beast/http.hpp> //boost beast rest api
#include <boost/beast/version.hpp> //boost beast rest api
#include <boost/config.hpp> //boost beast rest api
#include <boost/lexical_cast.hpp> //boost beast rest api

class APIEngineInterface : public EngineInterface
{
public:
	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool IsStarted() = 0;
	virtual void AddEndPoint(std::string Name, APINodeInterface* InputNode, APINodeInterface* OutputNode, APINodeInterface* ExpectedNode) = 0;
};