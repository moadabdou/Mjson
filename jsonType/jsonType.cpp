#include "jsonType.hpp"

vector<string> JsonValue::typeNames = {"Null", "Number", "String", "Array", "Object", "Bool"};

JsonValue::JsonValue() : type(JsonType::Null) {}



JsonValue::JsonValue(const JsonValue& other) {
    type = other.type;
    if (type != JsonType::Null) {
        value = make_unique<_JsonType>(*other.value);
    } else {
        value.reset();
    }
}

JsonValue::JsonValue(JsonValue&& other) {
    type = other.type;
    value = move(other.value);
    other.type = JsonType::Null;
}

JsonValue::~JsonValue() {}



JsonValue& JsonValue::operator=(const JsonValue& other) {
    type = other.type;
    if (type != JsonType::Null) {
        value = make_unique<_JsonType>(*other.value);
    } else {
        value.reset();
    }
    return *this;
}

JsonValue& JsonValue::operator[](int i) {
    is_Array(0);
    return get<JsonArray>(*value)[i];
}

const JsonValue& JsonValue::operator[](int i) const {
    is_Array(0);
    return get<JsonArray>(*value)[i];
}

JsonValue& JsonValue::operator[](const string& i) {
    is_Object(0);
    return get<JsonObject>(*value)[i];
}

const JsonValue& JsonValue::operator[](const string& i) const {
    is_Object(0);
    return get<JsonObject>(*value)[i];
}

string  JsonValue::print (string offset, bool pretify) const{
    string res ;
    if (type == JsonType::Null) {
        return "null";
    }else if (type == JsonType::Bool){ //auto interpret  bool  as int
        return  (get<bool>(*value) ? "true" : "false");
    }
    visit([&res,&offset,&pretify](auto&& arg) {
        using T = decay_t<decltype(arg)>;
        if constexpr (is_same_v<T, int>) {
            res =  to_string(arg);
        } else if constexpr (is_same_v<T, double>) {
            res =  to_string(arg);
        } else if constexpr (is_same_v<T, string>) {
            res = "\"" + arg +"\"";
        } else if constexpr (is_same_v<T, JsonArray>) {
            res = "[";
            for (int i = 0; i  <  arg.size() ; i++) {
                res += (pretify && i != 0 ? "\n "+ offset : "") + arg[i].print(offset + (pretify ? "  " : ""),pretify);
                if (i < arg.size()-1){
                    res += ", ";
                }
            }
            res +=  "]";
        } else if constexpr (is_same_v<T, JsonObject>) {
            res = "{";
            int i = 0;
            for (const auto& [key, value] : arg) {
                res += (pretify ? "\n " : "")+ offset + "\"" + key + "\": " + value.print(offset+ (pretify ? "  " : ""),pretify);
                if (i < arg.size()-1){
                    res += ", ";
                }
                i++;
            }
            res += (pretify ? "\n " : "") +offset + "}";
        }
    }, *value);
    return res;
}   

ostream& operator<<(ostream& os, const JsonValue& jsonValue) {
    os << jsonValue.print();
    return os;
}


JsonType JsonValue::getType() const {
    return type;
}

JsonArray& JsonValue::Array() {
    is_Array(1);
    return get<JsonArray>(*value);
}

string& JsonValue::String() {
    if (type !=  JsonType::String){
        badPtrAccess("String");
    }
    return get<string>(*value);
}



bool& JsonValue::Bool(){
    if(type != JsonType::Bool){
        badPtrAccess("Bool");
    }
    return get<bool>(*value);
}

JsonObject& JsonValue::Object() {
    is_Object(1);
    return get<JsonObject>(*value);
}

void JsonValue::setTypeFromVariant(const _JsonType& val) {
    if(holds_alternative<bool>(val)){
        type =  JsonType::Bool;
    }else if (holds_alternative<int>(val) || holds_alternative<double>(val)) {
        type = JsonType::Number;
    } else if (holds_alternative<string>(val)) {
        type = JsonType::String;
    } else if (holds_alternative<JsonObject>(val)) {
        type = JsonType::Object;
    } else {
        type = JsonType::Array;
    }
}

void JsonValue::is_Array(int i) const {
    if (type != JsonType::Array) {
        if (i == 0) {
            barcketOperatorError("int");
        } else if (i == 1) {
            badPtrAccess("Array");
        }
    }
}

void JsonValue::is_Object(int i) const {
    if (type != JsonType::Object) {
        if (i == 0) {
            barcketOperatorError("string");
        } else if (i == 1) {
            badPtrAccess("Object");
        }
    }
}

void JsonValue::barcketOperatorError(const char* _type) const {
    throw runtime_error(
        (string)"error:  [" + _type + "] operator is not allowed on a type " 
        + typeNames[static_cast<int>(type)]
    );
}

void JsonValue::badPtrAccess(const char* _type) const {
    throw runtime_error(
        (string)"error: can't access " + _type + " on a type " 
        + typeNames[static_cast<int>(type)]
    );
}
