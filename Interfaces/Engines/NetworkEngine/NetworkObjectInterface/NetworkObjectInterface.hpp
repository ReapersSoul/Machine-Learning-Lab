#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include "../DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"

namespace b_beast = boost::beast;         // from <boost/beast.hpp>
namespace b_http = b_beast::http;           // from <boost/beast/http.hpp>
namespace b_websocket = b_beast::websocket; // from <boost/beast/websocket.hpp>
namespace b_net = boost::asio;            // from <boost/asio.hpp>
namespace b_ip = boost::asio::ip;

//TODO: Implement ServerInterface
class NetworkObjectInterface {
protected:
	int ID;
	b_ip::tcp::socket socket;
	std::string Name;
	std::string IP;
	int Port;
	DynamicCodeExecutionEngineInterface* DCEE;
public:
	NetworkObjectInterface(b_ip::tcp::socket sock, int id) :socket(std::move(sock)) {
		ID = id;
	}

	void SetIP(std::string ip) {
		IP = ip;
	}

	void SetPort(int port) {
		Port = port;
	}

	void SetName(std::string name) {
		Name = name;
	}

	std::string GetName() {
		return Name;
	}

	int GetID() {
		return ID;
	}

	std::string GetIP() {
		return IP;
	}

	int GetPort() {
		return Port;
	}

	void SetDCEE(DynamicCodeExecutionEngineInterface* dce) {
		DCEE = dce;
	}

	virtual void Write(std::string message) = 0;

	virtual std::string Read() = 0;
};