#include "jsonParser.hpp"
#include <cctype>
#include <sstream>

using namespace std;

class StringHolder {
    string source; 
    int column;
    int row;
    int position;
public: 
    StringHolder(const string& src);

    char Current() const;
    int skipNewLines();
    char chop();
    char skipWhite();
    void chopBy(int i);
    string sub(int start, int size) const;

    int Column() const;
    int Row() const;
    int Position() const;
};


StringHolder::StringHolder(const string& src) : column(0), row(0), position(0) {
    source = src;
}

char StringHolder::Current() const {
    if (position == source.size()) return EOS;
    return source[position];
}

int StringHolder::skipNewLines() {
    int charCount = 0;
    while (source[position] == '\n') {
        position++;
        column++;
        charCount++;
    }
    return charCount;
}

char StringHolder::chop() {
    if (position == source.size()) return EOS;

    char current = source[position++];

    if (skipNewLines()) {
        row = 0;
    } else {
        row += 1;
    }
    return current;
}

char StringHolder::skipWhite() {
    char current;
    while (isspace(current = Current())) chop();
    return current;
}

void StringHolder::chopBy(int i) {
    while (i--) chop();
}

string StringHolder::sub(int start, int size) const {
    return source.substr(start, size);
}

int StringHolder::Column() const {
    return column;
}

int StringHolder::Row() const {
    return row;
}

int StringHolder::Position() const {
    return position;
}





// ErrorMsgs namespace
string errorPositionHandler(const StringHolder& holder) {
    stringstream msg;
    msg << " at position: " << holder.Position() << " (column:" << holder.Column() << ", row:" << holder.Row() << ")" << endl;
    return msg.str();
}

string errorExcpected(string expected, string got) {
    return "Expected (" + expected + ") but got (" + got + ")" + "\n";
}

string errorParsingType(string type) {
    return "An error occurred while parsing type " + type + "\n";
}

string errorUnknownValue() {
    return "Encountered <Unknown> JSON value\n";
}

void errorObjectkey(const StringHolder& holder) {
    throw runtime_error(
        errorParsingType("Object<key>")
        + errorExcpected(" \" ", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
        + "Object key has to be between \" \n"
        + errorPositionHandler(holder)
    );
}





// Parsers namespace
namespace Parsers {
    bool isDelimiter(char c) {
        return isspace(c) || c == ',' || c == ']' || c == '}';
    }

    string parseString(StringHolder& holder) {
        string res;
        char current;
        holder.chop();
        while ((current = holder.chop()) != '"') {
            if (current == EOS) {
                throw runtime_error(errorParsingType("String")
                                     + errorExcpected("\"", current == EOS ? "end of string" : "new line")
                                     + errorPositionHandler(holder));
            }
            res += current;
        }
        return res;
    }

    JsonValue parseNumber(StringHolder& holder) {
        string res;
        char current, prev = 0;
        int dotcounter = 0;

        //check  for negative number 
        if (holder.Current() == '-') res += prev = holder.chop();
        while ((isdigit(current = holder.Current()) || (current == '.' && isdigit(prev))) && dotcounter < 2) {
            if (current == '.') {
                dotcounter++;
            }
            holder.chop();
            prev = current;
            res += current;
        }
        if (!isDelimiter(current) || prev == '.' || dotcounter > 1 || prev == '-') {
            throw runtime_error(
                errorParsingType("Number")
                + (prev == '.'|| prev == '-' || dotcounter > 1 ?
                   " Invalid Number format: " + res + " \n" :
                   errorExcpected(" delimiter ", current == EOS ? "end of string" : string() + current))
                + errorPositionHandler(holder)
            );
        }
        if (dotcounter) return stod(res);
        else return stoi(res);
    }

    bool parseBool(StringHolder& holder) {
        int initPosition = holder.Position(), returnDecision = -1;
        if (holder.sub(initPosition, 4) == "true") {
            holder.chopBy(4);
            returnDecision = 1;
        } else if (holder.sub(initPosition, 5) == "false") {
            holder.chopBy(5);
            returnDecision = 0;
        }

        char current = holder.Current();
        if (!isDelimiter(current)) {
            returnDecision = -1;
        }

        if (returnDecision == -1) {
            throw runtime_error(
                errorParsingType("Bool")
                + errorExcpected("true or false", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
                + errorPositionHandler(holder)
            );
        }

        return returnDecision;
    }

    void parseNull(StringHolder& holder) {
        int initPosition = holder.Position(), Decision = -1;
        if (holder.sub(initPosition, 4) == "null") {
            holder.chopBy(4);
            Decision = 1;
        }

        char current = holder.Current();
        if (!isDelimiter(current)) {
            Decision = -1;
        }

        if (Decision == -1) {
            throw runtime_error(
                errorParsingType("Null")
                + errorExcpected("null", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
                + errorPositionHandler(holder)
            );
        }
    }

    JsonValue jsonValueParser(StringHolder& holder);

    JsonArray parseArray(StringHolder& holder) {
        holder.chop();
        JsonArray res;
        holder.skipWhite();
        if (holder.Current() == ']') {
            holder.chop();
            return res;
        }
        while (holder.Current() != EOS) {
            res.push_back(jsonValueParser(holder));
            holder.skipWhite();
            if (holder.Current() == ',') {
                holder.chop();
            } else if (holder.Current() == ']') {
                holder.chop();
                break;
            } else {
                throw runtime_error(
                    errorParsingType("Array")
                    + errorExcpected("] or jsonValue", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
                    + errorPositionHandler(holder)
                );
            }
        }
        return res;
    }

    pair<string, JsonValue> parseObjectElement(StringHolder& holder) {
        string key;

        char current = holder.skipWhite();
        if (current != '"') {
            errorObjectkey(holder);
        }
        holder.chop();
        holder.skipWhite();

        while (isalnum(current = holder.Current()) || current == '_') {
            holder.chop();
            key += current;
        }

        current = holder.skipWhite();
        if (current != '"') {
            errorObjectkey(holder);
        }
        holder.chop();

        current = holder.skipWhite();
        if (current != ':') {
            throw runtime_error(
                errorParsingType("Object<key>")
                + errorExcpected(" \"key\" : ", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
                + errorPositionHandler(holder)
            );
        }

        holder.chop();
        return {key, jsonValueParser(holder)};
    }

    JsonObject parseObject(StringHolder& holder) {
        holder.chop();
        JsonObject res;
        holder.skipWhite();
        if (holder.Current() == '}') {
            holder.chop();
            return res;
        }
        while (holder.Current() != EOS) {
            res.insert(parseObjectElement(holder));
            holder.skipWhite();
            if (holder.Current() == ',') {
                holder.chop();
            } else if (holder.Current() == '}') {
                holder.chop();
                break;
            } else {
                throw runtime_error(
                    errorParsingType("Object")
                    + errorExcpected("} or jsonValue", holder.Current() == EOS ? "end of string" : holder.sub(holder.Position(), 6))
                    + errorPositionHandler(holder)
                );
            }
        }

        return res;
    }

    JsonValue jsonValueParser(StringHolder& holder) {
        char current = holder.skipWhite();
        if (current == '"') {
            return JsonValue(parseString(holder));
        } else if (isdigit(current) || current == '-') {
            return parseNumber(holder);
        } else if (current == 'f' || current == 't') {
            return JsonValue(parseBool(holder));
        } else if (current == 'n') {
            parseNull(holder);
            return JsonValue();
        } else if (current == '[') {
            return JsonValue(parseArray(holder));
        } else if (current == '{') {
            return JsonValue(parseObject(holder));
        } else {
            throw runtime_error(
                errorParsingType("JsonValue")
                + errorExcpected("valid json value", holder.sub(holder.Position(), 10))
                + errorUnknownValue()
                + errorPositionHandler(holder)
            );
        }
    }
}


namespace Parser{
    JsonValue parse(const string& json){
        StringHolder holder(json);
        return  Parsers::jsonValueParser(holder);
    }
    string stringfy (const JsonValue& json,bool pretify){
        return json.print("",pretify);
    }
};
