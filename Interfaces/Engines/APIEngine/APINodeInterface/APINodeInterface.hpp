#include "../NodeInterface/NodeInterface.hpp"

class APINodeInterface : public NS_Node::NodeInterface
{
public:
	virtual void SetIO(nlohmann::json IO) = 0;
	virtual bool ShouldProcessForward() = 0;
	virtual void SetShouldProcess(bool ShouldProcess) = 0;
	virtual std::string GetIO() = 0;
};