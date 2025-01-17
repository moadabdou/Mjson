# JSON Parser Usage Documentation

```cpp
#include "jsonParser/jsonParser.hpp"  // Include for JSON Parser library
```

## 1. Initialization of `JsonValue` Objects

### From `Number` (int, double):
```cpp
JsonValue NumberValue(4);        // Initializes with an int
JsonValue _NumberValue = 4;      // Initializes with an int

JsonValue NumberValue_d(4.5);    // Initializes with a double
JsonValue _NumberValue_d = 4.5;  // Initializes with a double
```

### From `String`:
```cpp
JsonValue StringValue = "hello";  // Initializes with a string
JsonValue _StringValue("hello");  // Initializes with a string
```

### From `Boolean`:
```cpp
JsonValue BoolValue = true;       // Initializes with a boolean value
JsonValue _BoolValue(true);       // Initializes with a boolean value
```

### From `JsonArray`:
```cpp
JsonValue ArrayValue = JsonArray{ StringValue, NumberValue, JsonValue(false) }; 
JsonValue _ArrayValue(JsonArray{ StringValue, NumberValue, JsonValue(false) });
```

### From `JsonObject`:
```cpp
JsonValue ObjectValue = JsonObject{ {"string0", StringValue}, {"numValue", NumberValue}, {"bool value", JsonValue(false)} };
JsonValue _ObjectValue(JsonObject{ {"string0", StringValue}, {"numValue", NumberValue}, {"bool value", JsonValue(false)}} );
```

## 2. Accessing Values

### Direct Access:
You can access the value of a `JsonValue` using the type-specific functions. 

```cpp
NumberValue.Number<int>();    // Returns an integer value
NumberValue_d.Number<double>(); // Returns a double value
```

Note: These functions return a **copy** of the value. If you try to access an `int` value from a `double`, it will cast the value to an `int`, and vice versa.

### Accessing References:
The following functions provide access to the `JsonValue` references:

```cpp
ArrayValue.Array();   // Returns a reference to the JsonArray
ObjectValue.Object(); // Returns a reference to the JsonObject
StringValue.String(); // Returns a reference to the String
BoolValue.Bool();     // Returns a reference to the Boolean
```

Note: Accessing a value with the wrong function will throw an exception due to incorrect type access.

## 3. Accessing `JsonArray` Elements
You can access elements within a `JsonArray` using an index:

```cpp
int index = 1;
ArrayValue[index];  // Returns a reference to the JsonValue at the specified index
```

## 4. Accessing `JsonObject` Elements
To access elements within a `JsonObject`, use a key:

```cpp
string key = "numValue";
ObjectValue[key];  // Returns a reference to the JsonValue associated with the key
```

## 5. Overriding `JsonValue`
A `JsonValue` can be reassigned to a different type, overriding its previous value:

```cpp
NumberValue = "hello";  // Overrides the previous value with a string
NumberValue_d = ArrayValue;  // Overrides with another JsonValue
```

### Note: 
Any type can be overridden with any other type, which provides flexibility in working with `JsonValue` objects.

## 6. Get the Type of a `JsonValue`
To get the type of a `JsonValue`, use the `getType()` function. This returns a `JsonType`, which can be printed using the `typeNames` array:

```cpp
JsonType type = NumberValue.getType();  // Returns the JsonType
cout << JsonValue::typeNames[static_cast<int>(type)];  // Prints the type name
```

## 7. Printing a `JsonValue`
You can print the `JsonValue` using either the `print()` function or the `<<` overload.

### Using the `print()` Function:
```cpp
string offset = "";    // The indentation offset (default is "")
bool pretty = true;     // Whether to pretty print (default is true)
string printed = ObjectValue.print(offset, pretty);
cout << printed;  // Print the formatted JSON string
```

### Using the `<<` Overload:
```cpp
cout << ObjectValue;  // Prints the JsonValue (pretty print by default)
```

## 8. JSON Parsing

### Example of Parsing a JSON String:
```cpp
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
    }
})";
JsonValue parsed = Parser::parse(jsonString);  // Parse the JSON string
cout << parsed;  // Print the parsed JSON
```

## 9. Stringify a `JsonValue`
To convert a `JsonValue` back to a string, use the `stringfy()` function from the `Parser`:

```cpp
string jsString = Parser::stringfy(parsed, pretty);  // Stringify the parsed JSON
cout << jsString;  // Print the stringified JSON
```

### Note: 
The `pretty` parameter (default: `false`) controls whether the output is pretty-printed.