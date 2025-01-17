#ifndef JSONVALUE_HPP
#define JSONVALUE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <variant>

using namespace std;

class JsonValue;

using JsonArray = vector<JsonValue>;
using JsonObject = map<string, JsonValue>;
using _JsonType = variant<int, double, bool, string, JsonArray, JsonObject>;

enum class JsonType {
    Null, Number, String, Array, Object, Bool
};

class JsonValue {
    JsonType type;
    unique_ptr<_JsonType> value;

public:
    static vector<string> typeNames;
    JsonValue();
    // Constructor for any type in the variant
    template <typename T, typename = enable_if_t<is_constructible_v<_JsonType, T>>>
    JsonValue(const T& val){
        *this = val; // Delegate to assignment operator
    }
    
    JsonValue(const JsonValue& other);
    JsonValue(JsonValue&& other);
    ~JsonValue();

    template <typename T, typename = enable_if_t<is_constructible_v<_JsonType, T>>>
    JsonValue& operator=(const T& val) {
        value = make_unique<_JsonType>(val); 
        setTypeFromVariant(*value);         
        return *this;
    }

    JsonValue& operator=(const JsonValue& other);
    JsonValue& operator[](int i);
    const JsonValue& operator[](int i) const;
    JsonValue& operator[](const string& i);
    const JsonValue& operator[](const string& i) const;

    friend ostream& operator<<(ostream& os, const JsonValue& jsonValue);
    string print(string offset= "", bool pretify = true) const;


    JsonType getType() const;


    JsonArray& Array();
    JsonObject& Object();
    string& String();
    template<typename T,typename =  enable_if_t<is_same<T,int>::value || is_same<T,double>::value >>
    T Number();
    bool& Bool();

private:
    void setTypeFromVariant(const _JsonType& val);
    void is_Array(int i) const;
    void is_Object(int i) const;
    void barcketOperatorError(const char* _type) const;
    void badPtrAccess(const char* _type) const;
};


template<typename T,typename>

T JsonValue::Number(){
    if(type != JsonType::Number){
        badPtrAccess("Number");
    }
    if (holds_alternative<int>(*value)){
        return get<int>(*value);
    }else if((holds_alternative<double>(*value))){
        return get<double>(*value);
    }else {
        throw  runtime_error("\n error : bad access to the  number value \n expected :  int or double");
    }
}

#endif // JSONVALUE_HPP
