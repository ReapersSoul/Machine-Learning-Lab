#pragma once
#include <fstream>   // Added for file operations
#include <nlohmann/json.hpp>

struct SerializableInterface {
    virtual ~SerializableInterface() {} // Add a virtual destructor

    virtual nlohmann::json Serialize() = 0;
    virtual void DeSerialize(nlohmann::json, void* DCEE) = 0;

    void SerializeToFile(std::string filepath) {
	    std::ofstream file(filepath);
		if (file.is_open()) {
			file << Serialize().dump(4);
			file.close();
		}
    }
    void DeSerializeFromFile(std::string filepath, void* DCEE) {
        nlohmann::json j;
        std::ifstream file(filepath);
        if (file.is_open()) {
			file >> j;
			file.close();
		}
        DeSerialize(j, DCEE);
    }
};