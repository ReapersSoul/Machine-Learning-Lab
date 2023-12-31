#pragma once
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <functional>
#include <stdexcept>
namespace NS_DataObject {
	struct ConversionDescriptor {
		unsigned int From;
		unsigned int To;

		bool operator==(const ConversionDescriptor& other) const
		{
			return From == other.From && To == other.To;
		}
	};
}

template <>
struct std::hash<NS_DataObject::ConversionDescriptor>
{
	std::size_t operator()(const NS_DataObject::ConversionDescriptor& k) const
	{
		return ((std::hash<unsigned int>()(k.From) ^ (std::hash<unsigned int>()(k.To) << 1)) >> 1);
	}
};

namespace NS_DataObject {
	static std::unordered_map<std::string, unsigned int> TypeIDs={{"Invalid", 0}};
	static std::unordered_map<unsigned int, std::string> TypeIDsReverse={{0, "Invalid"}};
	static void RegisterType(std::string TypeID) {
		if(TypeIDs.find(TypeID) != TypeIDs.end()) throw std::runtime_error("TypeID already registered");
		TypeIDs[TypeID] = std::hash<std::string>{}(TypeID);
		TypeIDsReverse[std::hash<std::string>{}(TypeID)] = TypeID;
	}
	static unsigned int GetTypeID(std::string TypeID) {
		return TypeIDs[TypeID];
	}
	static std::string GetTypeID(unsigned int TypeID) {
		return TypeIDsReverse[TypeID];
	}
	static bool TypeIDExists(std::string TypeID) {
		return TypeIDs.find(TypeID) != TypeIDs.end();
	}

	class DataObjectInterface {
		enum T_Precision {
			Double = 0,
			Float = 1,
			Int = 2,
			None = 3
		}Precision;
		unsigned int TypeID = 0;

	public:
		template<typename T>
		void SetPrecision() {
			if (typeid(T) == typeid(double)) {
				Precision = T_Precision::Double;
			}
			else if (typeid(T) == typeid(float)) {
				Precision = T_Precision::Float;
			}
			else if (typeid(T) == typeid(int)) {
				Precision = T_Precision::Int;
			}
			else {
				Precision = T_Precision::None;
			}
		}

		void NoPrecision() {
			Precision = T_Precision::None;
		}

		T_Precision GetPrecision() {
			return Precision;
		}

		unsigned int GetTypeID() {
			return TypeID;
		}
	};

	static std::unordered_map<ConversionDescriptor, std::function<DataObjectInterface* (DataObjectInterface*, unsigned int)>> ConversionFunctions;
	static void RegisterConversion(ConversionDescriptor CD, std::function<DataObjectInterface* (DataObjectInterface*, unsigned int)> ConversionFunction) {
		if (CD.From == 0 || CD.To == 0) throw std::runtime_error("Invalid ConversionDescriptor");
		if(ConversionFunctions.find(CD) != ConversionFunctions.end()) throw std::runtime_error("Conversion already registered");
		ConversionFunctions[CD] = ConversionFunction;
	}
	static void RegisterConversion(std::string From, std::string To, std::function<DataObjectInterface* (DataObjectInterface*, unsigned int)> ConversionFunction) {
		if(From == "Invalid" || To == "Invalid") throw std::runtime_error("Invalid ConversionDescriptor");
		if (ConversionFunctions.find(ConversionDescriptor{ GetTypeID(From), GetTypeID(To) }) != ConversionFunctions.end()) throw std::runtime_error("Conversion already registered");
		ConversionDescriptor CD;
		CD.From = GetTypeID(From);
		CD.To = GetTypeID(To);
		ConversionFunctions[CD] = ConversionFunction;
	}
	static DataObjectInterface* Convert(DataObjectInterface* DO, unsigned int To) {
		ConversionDescriptor CD;
		CD.From = DO->GetTypeID();
		CD.To = To;
		return ConversionFunctions[CD](DO, To);
	}

	static std::unordered_map<unsigned int, std::function<DataObjectInterface* ()>> Constructors={{0, []() {return nullptr;}}};
	static void RegisterConstructor(unsigned int TypeID, std::function<DataObjectInterface* ()> Constructor) {
		if (Constructors.find(TypeID) != Constructors.end()) throw std::runtime_error("Constructor already registered");
		Constructors[TypeID] = Constructor;
	}
	static DataObjectInterface* Construct(unsigned int TypeID) {
		return Constructors[TypeID]();
	}
}