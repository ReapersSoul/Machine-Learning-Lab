#include "../../../Util/Exports.hpp"
#include <NetworkEngineInterface.hpp>
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <AttributeInterface.hpp>
#include <LibraryInterface.hpp>

#include <string>

#include <sqlite3.h>

class NetworkEngine : public NetworkEngineInterface
{
public:

	NetworkEngine() {
		Name = "NetworkEngine";
	}

	~NetworkEngine()
	{

	}

	bool Init(bool IsClient) override {
		isServer = !IsClient;
		return true;
	}

	void Run(int prt) override {
		if (IsServer()) {
			//convert port to 
			b_ip::tcp::acceptor acceptor(io_context, { b_ip::tcp::v4(), (b_ip::port_type)prt });

			while (true) {
				b_ip::tcp::socket socket(io_context);
				acceptor.accept(socket);

				int id = ledger.GetNextID();
				NetworkObjects[id] = new ClientInterface(std::move(socket), id);
				NetworkObjects[id]->SetDCEE(DCEEngine);
				//get clients ip and port
				((ClientInterface*)NetworkObjects[id])->Start();
				printf("Client Connected\n");
			}
		}
	}

	int Connect(std::string ip, int prt) override {
		if (IsClient()) {
			int id = ledger.GetNextID();
			b_ip::tcp::resolver resolver(io_context);
			b_ip::tcp::socket socket(io_context);

			// Resolve the server address and port
			auto endpoints = resolver.resolve(ip, std::to_string(prt));

			// Connect to the server
			b_net::connect(socket, endpoints);

			NetworkObjects[id] = new ServerInterface(std::move(socket), id);
			NetworkObjects[id]->SetDCEE(DCEEngine);

			printf("Connected to Server\n");
			return id;
		}
		else {
			return -1;
		}
	};

	ServerInterface* GetServer(int id) {
		if (IsClient()) {
			return (ServerInterface*)NetworkObjects[id];
		}
		else {
			return nullptr;
		}
	}
};

static NetworkEngine instance;



extern "C" {

	EXPORT void CleanUp() {
		//delete
		instance.~NetworkEngine();
	}

	EXPORT NetworkEngineInterface* GetInstance() {
		return &instance;
	}
}