#include "../../EngineInterface/EngineInterface.hpp"
#include "../NetworkObjectInterface/NetworkObjectInterface.hpp"
#include "../Client/ClientInterface/ClientInterface.hpp"
#include "../Server/ServerInterface/ServerInterface.hpp"
#include "../LedgerInterface/LedgerInterface.hpp"

#include <string>
#include <vector>
//nlohmann json
#include <nlohmann/json.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace b_beast = boost::beast;         // from <boost/beast.hpp>
namespace b_http = b_beast::http;           // from <boost/beast/http.hpp>
namespace b_websocket = b_beast::websocket; // from <boost/beast/websocket.hpp>
namespace b_net = boost::asio;            // from <boost/asio.hpp>
namespace b_ip = boost::asio::ip;

class NetworkEngineInterface : public EngineInterface
{
protected:
	b_net::io_context io_context;
	std::vector<std::string> DLL_Whitelist;
	bool AllowDLLTransfer = false;
	std::map<int, NetworkObjectInterface*> NetworkObjects;
	LedgerInterface ledger;
	bool isServer;
public:

	virtual bool IsServer() {
		return isServer;
	}

	virtual bool IsClient() {
		return !isServer;
	}

	virtual bool Init(bool IsClient) = 0;
	virtual void Run(int port) = 0;

	virtual int Connect(std::string ip, int port) = 0;

	virtual ServerInterface* GetServer(int id) = 0;
};