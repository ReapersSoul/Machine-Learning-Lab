#pragma once
#include "../NetworkObjectInterface/NetworkObjectInterface.h"
#include <iostream>

//represents a server's connection to a client
class ServerInterface: public NetworkObjectInterface
{
public:
	ServerInterface(b_ip::tcp::socket sock, int id) :NetworkObjectInterface(std::move(sock), id) {
		SetIP(socket.remote_endpoint().address().to_string());
		SetPort(socket.remote_endpoint().port());
		SetName("Server " + std::to_string(id));
	}

	virtual void Write(std::string message) {
		// Create an HTTP POST request
		b_beast::http::request<b_beast::http::string_body> request(b_beast::http::verb::post, "/", 11);
		request.set(b_beast::http::field::host, "localhost");
		request.set(b_beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		request.set(b_beast::http::field::content_type, "text/plain");
		request.content_length(message.size());
		request.keep_alive(true);

		// Set the request body
		request.body() = std::move(message);

		// Send the HTTP request
		b_beast::http::write(socket, request);
	}

	virtual std::string Read() {
		b_beast::flat_buffer buffer;
		b_beast::http::response<b_beast::http::string_body> response;
		b_beast::http::read(socket, buffer, response);

		std::cout << "Response: " << response.body() << std::endl;

		return response.body();
	}

	virtual ~ServerInterface() {
		printf("Deconstructed!\n");
	};
	
};