#pragma once
#include "../NetworkObjectInterface/NetworkObjectInterface.h"
#include "../../GraphEngine/NodeInterface/NodeInterface.h"
#include <iostream>

//represents a clients connection to the server
class ClientInterface:public NetworkObjectInterface {
	std::thread t;
	int version = 11;
public:
	ClientInterface(b_ip::tcp::socket sock,int id):NetworkObjectInterface(std::move(sock),id) {
		SetIP(socket.remote_endpoint().address().to_string());
		SetPort(socket.remote_endpoint().port());
		SetName("Client " + std::to_string(id));
	}

	void Start() {		
		t = std::thread(&ClientInterface::Loop, this);
	}
	
	void Loop() {
		try {
			while (true) {
				b_beast::flat_buffer buffer;
				b_beast::http::request<b_beast::http::string_body> request;

				b_beast::http::read(socket, buffer, request);

				ProcessMessage(buffer, request);

				if (request.need_eof())
					break;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}

	void ProcessMessage(b_beast::flat_buffer data, b_beast::http::request<b_beast::http::string_body> request) {
		printf("Message: %s\n", request.body().c_str());

		version = request.version();
		b_beast::http::response<b_beast::http::string_body> response{b_beast::http::status::ok, request.version()};
		response.set(b_beast::http::field::server, "Machine-Learning-Labs-Server");
		//convert buffer to string
		std::string message = std::string(request.body().c_str());

		if (message == std::string("GetLibraries")) {
			GetLibraries();
		}
		else if (message == std::string("SyncronizeCore")) {
			SyncronizeCore();
		}
		else if (message == std::string("SyncronizeLibraries")) {
			SyncronizeLibraries();
		}
		else if (message == std::string("SyncronizePlugins")) {
			SyncronizePlugins();
		}
		else if (message == std::string("SyncronizeScripts")) {
			SyncronizeScripts();
		}
		else if (message == std::string("SyncronizeAll")) {
			SyncronizeAll();
		}
		else if (message == std::string("ProcessNode")) {
			
		}
		else {
			response.body() = std::string("Invalid Command");
		}


		response.prepare_payload();
		b_beast::http::write(socket, response);
	}

	virtual ~ClientInterface() {

	};

	virtual int GetID() {
		return ID;
	}

	virtual void Close() {
		socket.close();
		t.join();
	}

	virtual bool IsOpen() {
		return socket.is_open();
	}

	//std::vector<std::string> Get

	virtual void GetLibraries() {
		////get core libraries
		//std::vector<LibraryInterface*> libs = DCEE->GetLibraries();
		////make buffer
		//ws.write(b_net::buffer(std::to_string(libs.size())));
		//for (int i = 0; i < libs.size(); i++)
		//{
		//	ws.write(b_net::buffer(libs[i]->GetPath()));
		//}
	}

	virtual bool SyncronizeCore() {
		return false;
	}

	virtual bool SyncronizeLibraries() {
		
		return false;
	}

	virtual bool SyncronizePlugins() {
		return false;
	}

	virtual bool SyncronizeScripts() {
		return false;
	}

	virtual bool SyncronizeAll() {
		bool result = true;
		result &= SyncronizeLibraries();
		result &= SyncronizeScripts();
		return result;
	}

	virtual void Write(std::string message) {
		b_beast::http::response<b_beast::http::string_body> response{b_beast::http::status::ok, version};
		response.set(b_beast::http::field::server, "Boost Beast Echo Server");
		response.body() = message;
		response.prepare_payload();

		b_beast::http::write(socket, response);
	}

	virtual std::string Read() {
		b_beast::flat_buffer buffer;
		b_beast::http::request<b_beast::http::string_body> request;
		b_beast::http::read(socket, buffer, request);

		return request.body();
	}
};