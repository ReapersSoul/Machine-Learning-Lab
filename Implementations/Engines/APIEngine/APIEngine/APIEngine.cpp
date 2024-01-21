#include "../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <Attribute.hpp>
#include <LibraryInterface.hpp>
#include <APIEngineInterface.hpp>

#include <string>

//nlohmann json
#include <nlohmann/json.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

class EndPoint {
	APINodeInterface* InputNode;
	APINodeInterface* OutputNode;
	APINodeInterface* ExpectedNode;
	Graph* graph;
public:
	void SetInputNode(APINodeInterface* InputNode) {
		this->InputNode = InputNode;
		return;
	}

	void SetOutputNode(APINodeInterface* OutputNode) {
		this->OutputNode = OutputNode;
	}

	void SetExpectedNode(APINodeInterface* ExpectedNode) {
		this->ExpectedNode = ExpectedNode;
	}

	void SetGraph(Graph* graph) {
		this->graph = graph;
	}

	std::string Process(nlohmann::json Input) {
		std::string Output;
		bool errors = false;
		if (InputNode == nullptr) {
			Output= "{\"errors\":[\"InputNode is null!\"]}";
			errors = true;
		}
		if (OutputNode == nullptr) {
			Output = "{\"errors\":[\"OutputNode is null!\"]}";
			errors = true;
		}
		if (graph == nullptr) {
			Output = "{\"errors\":[\"Graph is null!\"]}";
			errors = true;
		}

		if (!errors) {
			InputNode->SetShouldProcess(true);
			OutputNode->SetShouldProcess(true);
			ExpectedNode->SetShouldProcess(true);
			if (Input.find("Expected") != Input.end()) {
				if (Input.find("Itterations") != Input.end()) {
					int Itterations = Input["Itterations"];
					for (int i = 0; i < Itterations; i++)
					{
						if (Input.find("Inputs") != Input.end()) {
							InputNode->SetIO(Input);
						}
						ExpectedNode->SetIO(Input);
						graph->Process(true);
						graph->Process(false);
					}
				}
				else {
					ExpectedNode->SetIO(Input);
					if (Input.find("Inputs") != Input.end()) {
						InputNode->SetIO(Input);
					}
					graph->Process(true);
					graph->Process(false);
				}
			}
			else {
				if (Input.find("Inputs") != Input.end()) {
					InputNode->SetIO(Input);
				}
				graph->Process(true);
			}
			InputNode->SetShouldProcess(false);
			OutputNode->SetShouldProcess(false);
			ExpectedNode->SetShouldProcess(false);
			Output = OutputNode->GetIO();
		}

		return Output;
	}
};

class APIEngine : public APIEngineInterface
{
protected:
	std::thread m_thread;
	std::map<std::string, EndPoint> EndPoints;
	bool ShouldRun = true;
public:

	APIEngine() {
		Name = "APIEngine";
	}

	~APIEngine()
	{
		//stop server thread
		ShouldRun = false;
		while (!m_thread.joinable()) {};
		m_thread.join();
	}

	EndPoint* GetEndPoint(std::string Name) {
		if (EndPoints.find(Name) != EndPoints.end()) {
			return &EndPoints[Name];
		}
		return nullptr;
	}

	void Init() override {
		return;
	}

	void Run() override {
	   m_thread= std::thread(
			[&]() {
				auto const address = net::ip::make_address("127.0.0.1");
				unsigned short port = static_cast<unsigned short>(80);

				net::io_context ioc{1};

				tcp::acceptor acceptor{ioc, { address, port }};
				tcp::socket socket{ioc};
				http_server(acceptor, socket);
				try {
					ioc.run();
				}
				catch (std::exception const& e) {
					printf("Error: %s\n", e.what());
				}
			});
	}

	void Start() override {
		return;
	}

	void Stop() override {
		return;
	}

	bool IsStarted() override {
		return m_thread.joinable();
	}

	void AddEndPoint(std::string Name, APINodeInterface* InputNode, APINodeInterface* OutputNode, APINodeInterface* ExpectedNode) override {
		EndPoints[Name] = EndPoint();
		EndPoints[Name].SetInputNode(InputNode);
		EndPoints[Name].SetOutputNode(OutputNode);
		EndPoints[Name].SetExpectedNode(ExpectedNode);
		EndPoints[Name].SetGraph(InputNode->GetParentGraph());
		return;
	}
};

static APIEngine instance;

namespace my_program_state
{
	std::size_t request_count()
	{
		static std::size_t count = 0;
		return ++count;
	}

	std::time_t now()
	{
		return std::time(0);
	}
}

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
	http_connection(tcp::socket socket)
		: socket_(std::move(socket))
	{
	}

	// Initiate the asynchronous operations associated with the connection.
	void start()
	{
		read_request();
		check_deadline();
	}

private:
	// The socket for the currently connected client.
	tcp::socket socket_;

	// The buffer for performing reads.
	beast::flat_buffer buffer_{8192};

	// The request message.
	http::request<http::dynamic_body> request_;

	// The response message.
	http::response<http::dynamic_body> response_;

	// The timer for putting a deadline on connection processing.
	net::steady_timer deadline_{
		socket_.get_executor(), std::chrono::seconds(60)};

	// Asynchronously receive a complete request message.
	void read_request()
	{
		auto self = shared_from_this();

		http::async_read(
			socket_,
			buffer_,
			request_,
			[self](beast::error_code ec,
				std::size_t bytes_transferred)
			{
				boost::ignore_unused(bytes_transferred);
				if (!ec)
					self->process_request();
			});
	}

	// Determine what needs to be done with the request message.
	void process_request()
	{
		response_.version(request_.version());
		response_.keep_alive(false);

		switch (request_.method())
		{
		case http::verb::get:
			response_.result(http::status::ok);
			response_.set(http::field::server, "Machine-Learning-Labs");
			create_response();
			break;

		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			response_.result(http::status::bad_request);
			response_.set(http::field::content_type, "text/plain");
			beast::ostream(response_.body())
				<< "Invalid request-method '"
				<< std::string(request_.method_string())
				<< "'";
			break;
		}

		write_response();
	}

	// Construct a response message based on the program state.
	void create_response()
	{
		if (request_.target() == "/count")
		{
			response_.set(http::field::content_type, "text/html");
			beast::ostream(response_.body())
				<< "<html>\n"
				<< "<head><title>Request count</title></head>\n"
				<< "<body>\n"
				<< "<h1>Request count</h1>\n"
				<< "<p>There have been "
				<< my_program_state::request_count()
				<< " requests so far.</p>\n"
				<< "</body>\n"
				<< "</html>\n";
		}
		else if (request_.target() == "/time")
		{
			response_.set(http::field::content_type, "text/html");
			beast::ostream(response_.body())
				<< "<html>\n"
				<< "<head><title>Current time</title></head>\n"
				<< "<body>\n"
				<< "<h1>Current time</h1>\n"
				<< "<p>The current time is "
				<< my_program_state::now()
				<< " seconds since the epoch.</p>\n"
				<< "</body>\n"
				<< "</html>\n";
		}
		else
		{
			std::string endpointName = request_.target();
			//remove first slash
			endpointName.erase(0, 1);
			EndPoint* endpoint = instance.GetEndPoint(endpointName);
			if (endpoint == nullptr) {
				response_.result(http::status::not_found);
				response_.set(http::field::content_type, "text/plain");
				beast::ostream(response_.body()) << "Invalid Endpoint!\r\n";
			}
			else {
				//return json
				response_.result(http::status::ok);
				response_.set(http::field::content_type, "application/json");
				//body as string
				std::string body = boost::beast::buffers_to_string(request_.body().data());
				if (body == "") {
					body = "{}";
				}
				std::string response = endpoint->Process(nlohmann::json::parse(body));
				beast::ostream(response_.body()) << response;
			}
		}
	}

	// Asynchronously transmit the response message.
	void write_response()
	{
		auto self = shared_from_this();

		response_.content_length(response_.body().size());

		http::async_write(
			socket_,
			response_,
			[self](beast::error_code ec, std::size_t)
			{
				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
				self->deadline_.cancel();
			});
	}

	// Check whether we have spent enough time on this connection.
	void check_deadline()
	{
		auto self = shared_from_this();

		deadline_.async_wait(
			[self](beast::error_code ec)
			{
				if (!ec)
				{
					// Close socket to cancel any outstanding operation.
					self->socket_.close(ec);
				}
			});
	}
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
	acceptor.async_accept(socket,
		[&](beast::error_code ec)
		{
			if (!ec)
				std::make_shared<http_connection>(std::move(socket))->start();
			http_server(acceptor, socket);
		});
}



extern "C" {

	EXPORT void CleanUp() {
		//delete
		instance.~APIEngine();
	}

	EXPORT APIEngineInterface* GetInstance() {
		//check if server thread is running and if not start it
		if (instance.IsStarted() == false) {
			instance.Run();
		}
		return &instance;
	}
}