#ifndef STRINGHOLDER_HPP
#define STRINGHOLDER_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "../jsonType/jsonType.hpp"

#define EOS -1

using namespace std;

namespace Parser{
    JsonValue parse(const string& json);
    string stringfy (const JsonValue& json, bool pretify = false);
};


#endif // STRINGHOLDER_HPP
