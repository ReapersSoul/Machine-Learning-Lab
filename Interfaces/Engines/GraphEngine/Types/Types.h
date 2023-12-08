#pragam once
#include "../../../SerializableInterface/SerializableInterface.h"

class IOObject: public SerializableInterface
{

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
