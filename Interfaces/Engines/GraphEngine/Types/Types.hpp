#pragam once
#include "../../../SerializableInterface/SerializableInterface.hpp"

class IOObject: public SerializableInterface
{
	std::string TypeID;
};

//essentially a vector of IOObjects with some extra functionality
class IOObjectCollection : public IOObject
{

};

class Vector: public IOObject
{

};

class Matrix: public IOObject
{

};

class Tensor: public IOObject
{

};

class Text: public IOObject
{

};

class File: public IOObject
{

};

class SqliteQuery: public IOObject
{

};
