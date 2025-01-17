#include "jsonParser/jsonParser.hpp"
#include <sstream>

int main(){

    // JsonArray = vector<JsonValue>
    // JsonObject = map<string, JsonValue>
    
    //-----------------how to initialize  jsonValue object  ------------------

    //from Number <int , double>
    JsonValue NumberValue(4); //int
    JsonValue _NumberValue = 4; //int

    JsonValue NumberValue_d(4.5); //double
    JsonValue _NumberValue_d = 4.5; //double

    //from  string 
    JsonValue StringValue = "hello";
    JsonValue _StringValue("hello");

    //from bool value
    JsonValue BoolValue = true;
    JsonValue _BoolValue(true);

    //from JsonArray 
    JsonValue ArrayValue = JsonArray{ StringValue, NumberValue, JsonValue(false) };
    JsonValue _ArrayValue(JsonArray{ StringValue, NumberValue, JsonValue(false) });

    //from JsonObject 
    JsonValue ObjectValue = JsonObject{ {"string0",StringValue}, {"numValue",NumberValue}, {"bool value",JsonValue(false)} };
    JsonValue _ObjectValue(JsonObject{ {"string0",StringValue}, {"numValue",NumberValue}, {"bool value",JsonValue(false)}});

    //----------------- accessing values  directly ------------------

    NumberValue.Number<int>();  // returns an int 
    NumberValue_d.Number<double>(); //returns a double
    //NOTE : not a reference to the value and if you called  <int> on a double it will cast  it to int and vers versa 

    //all  the following accesses are to  the reference
    ArrayValue.Array(); //reference to the JsonArray
    ObjectValue.Object(); //reference to the JsonObject
    StringValue.String(); //reference to the String
    BoolValue.Bool(); // ....... boolian

    //NOTE : trying to  access the by the wrong function  will throw an exception of wrong type access

    //----------- Accessing JsonValue of Type Array ------------
    int index = 1;
    ArrayValue[index]; //will  returns  a reference to  the JsonValue in corresponding index
    
    //----------- Accessing JsonValue of object Array ------------

    string key = "numValue";
    ObjectValue[key];  //will  returns  a reference to  the JsonValue in corresponding key

    //---------------- overriding a jsonValue -----------------

    NumberValue = "hello"; //will  overridde  the old value with a string value
    NumberValue_d = ArrayValue; // override using another jsonValue 

    //NOTE : you  can override any  type with  any other type

    //----------------- getType -----------
    JsonType type = NumberValue.getType(); // returns  JsonType 
    cout << JsonValue::typeNames[static_cast<int>(type)] ; // prints type name

    //----------------- print jsonValue -----------
    // > using print ()
    string offset = ""; // if u  want to  make an offset : default ""
    bool pretify  = true ; // if u  want a  readable version : default true
    string printed = ObjectValue.print(offset, pretify);  
    // > using << overload 
    cout << ObjectValue ; // will print the jsonValue (pretify : true)

    //--------------------- Json Parser ------

    // example a  json
    string jsonString = R"(
    {
    "user": {
        "id": 12345,
        "name": "John Doe",
        "email": "johndoe@example.com",
        "phone": "+1-800-555-1234",
        "isActive": true,
        "negative": -54.34,
        "roles": ["admin", "editor", "user"]
    }})";
    JsonValue parsed = Parser::parse(jsonString);
    cout << parsed;
    
    // ----------- stringfy -------------
    string jsString = Parser::stringfy(parsed,pretify); // pretify  default : false 
    cout << jsonString;
}