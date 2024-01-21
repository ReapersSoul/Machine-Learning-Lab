#pragma once
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <functional>
#include <stdexcept>

namespace NS_DataObject
{
	class DataObjectInterface
	{
		unsigned int TypeID = 0;

	public:

		virtual void* GetData()=0;

		virtual void SetData(void* Data)=0;

		template <typename T>
		T GetData()
		{
			return *(T*)GetData();
		}

		template <typename T>
		void SetData(T Data)
		{
			SetData((void*)&Data);
		}

		unsigned int GetTypeID()
		{
			return TypeID;
		}
	};
	
	struct ConversionDescriptor
	{
		unsigned int From;
		unsigned int To;
		std::function<DataObjectInterface *(DataObjectInterface *, unsigned int)> ConversionFunction;
		bool operator==(const ConversionDescriptor &other) const
		{
			return From == other.From && To == other.To;
		}
	};
}

template <>
struct std::hash<NS_DataObject::ConversionDescriptor>
{
	std::size_t operator()(const NS_DataObject::ConversionDescriptor &k) const
	{
		return ((std::hash<unsigned int>()(k.From) ^ (std::hash<unsigned int>()(k.To) << 1)) >> 1);
	}
};

namespace NS_DataObject
{
	class Registrar
	{
		std::unordered_map<std::string, unsigned int> TypeIDs = {{"Invalid", 0}};
		std::unordered_map<unsigned int, std::string> TypeIDsReverse = {{0, "Invalid"}};
		std::unordered_map<ConversionDescriptor, std::function<DataObjectInterface *(DataObjectInterface *, unsigned int)>> ConversionFunctions;
		std::unordered_map<unsigned int, std::function<DataObjectInterface *()>> Constructors = {{0, []() { return nullptr; }}};

	public:
		void RegisterType(std::string TypeID)
		{
			if (TypeIDs.find(TypeID) != TypeIDs.end())
				throw std::runtime_error("TypeID already registered");
			TypeIDs[TypeID] = std::hash<std::string>{}(TypeID);
			TypeIDsReverse[std::hash<std::string>{}(TypeID)] = TypeID;
		}
		unsigned int GetTypeID(std::string TypeID)
		{
			return TypeIDs[TypeID];
		}
		std::string GetTypeID(unsigned int TypeID)
		{
			return TypeIDsReverse[TypeID];
		}
		bool TypeIDExists(std::string TypeID)
		{
			return TypeIDs.find(TypeID) != TypeIDs.end();
		}

		void RegisterConversion(ConversionDescriptor CD)
		{
			if (CD.From == 0 || CD.To == 0)
				throw std::runtime_error("Invalid ConversionDescriptor");
			if (ConversionFunctions.find(CD) != ConversionFunctions.end())
				throw std::runtime_error("Conversion already registered");
			ConversionFunctions[CD] = CD.ConversionFunction;
		}
		void RegisterConversion(std::string From, std::string To, std::function<DataObjectInterface *(DataObjectInterface *, unsigned int)> ConversionFunction)
		{
			if (From == "Invalid" || To == "Invalid")
				throw std::runtime_error("Invalid ConversionDescriptor");
			if (ConversionFunctions.find(ConversionDescriptor{GetTypeID(From), GetTypeID(To)}) != ConversionFunctions.end())
				throw std::runtime_error("Conversion already registered");
			ConversionDescriptor CD;
			CD.From = GetTypeID(From);
			CD.To = GetTypeID(To);
			ConversionFunctions[CD] = ConversionFunction;
		}
		DataObjectInterface *Convert(DataObjectInterface *DO, unsigned int To)
		{
			ConversionDescriptor CD;
			CD.From = DO->GetTypeID();
			CD.To = To;
			return ConversionFunctions[CD](DO, To);
		}

		void RegisterConstructor(unsigned int TypeID, std::function<DataObjectInterface *()> Constructor)
		{
			if (Constructors.find(TypeID) != Constructors.end())
				throw std::runtime_error("Constructor already registered");
			Constructors[TypeID] = Constructor;
		}

		void Register(std::string TypeID, std::function<DataObjectInterface *()> Constructor, std::vector<ConversionDescriptor> ConversionFunctions)
		{
			RegisterType(TypeID);
			RegisterConstructor(GetTypeID(TypeID), Constructor);
			for (auto CF : ConversionFunctions)
			{
				RegisterConversion(CF);
			}
		}

		void Register(std::string TypeID, std::function<DataObjectInterface *()> Constructor, std::vector<std::tuple<std::string, std::string, std::function<DataObjectInterface *(DataObjectInterface *, unsigned int)>>> ConversionFunctions)
		{
			RegisterType(TypeID);
			RegisterConstructor(GetTypeID(TypeID), Constructor);
			for (auto CF : ConversionFunctions)
			{
				RegisterConversion(std::get<0>(CF), std::get<1>(CF), std::get<2>(CF));
			}
		}

		DataObjectInterface *Construct(unsigned int TypeID)
		{
			return Constructors[TypeID]();
		}
	};
}