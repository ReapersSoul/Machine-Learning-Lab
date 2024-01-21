#pragma once
#include <fstream>   // Added for file operations
#include <nlohmann/json.hpp>

struct SerializableInterface {
    virtual ~SerializableInterface() {} // Add a virtual destructor

    virtual nlohmann::json Serialize() = 0;
    virtual void DeSerialize(nlohmann::json, void* DCEE, void* Regestrar) = 0;

    void SerializeToFile(std::string filepath) {
	    std::ofstream file(filepath);
		if (file.is_open()) {
			file << Serialize().dump(4);
			file.close();
		}
    }
    void DeSerializeFromFile(std::string filepath, void* DCEE, void* Regestrar) {
        nlohmann::json j;
        std::ifstream file(filepath);
        if (file.is_open()) {
			file >> j;
			file.close();
		}
        DeSerialize(j, DCEE, Regestrar);
    }
};

struct SerializationHelper : public SerializableInterface {
    struct SerializableDescriptor {
        int MagicNumber;
        uint64_t offset = 0;
        uint16_t size;
    };

    int MagicNumber = 0;
    std::string TypeID = "SerializationHelper";
    std::vector<SerializableDescriptor> header;
    std::vector<SerializableInterface*> items;

    void AddItem(SerializableInterface* item) {
        items.push_back(item);
    }

    void CalculateHeaderOffsets() {
        uint64_t offset = 0;
        for (auto& item : items) {
            SerializableDescriptor desc;
            desc.offset = offset;
            desc.size = item->Serialize().dump(4).size();
            offset += desc.size;
            header.push_back(desc);
        }
    }

    nlohmann::json Serialize() override {


        return nlohmann::json();
    }
    void DeSerialize(nlohmann::json, void* DCEE, void* Regestrar) override {
    }
};