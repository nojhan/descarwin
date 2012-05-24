/* Tests ! */
# include <iostream>
using namespace std;

# include "Json.h"
using namespace json;

struct MySubObj : public Serializable {
    public:

    MySubObj() {}

    int id;
    Object* toJson() const
    {
        Object* obj = new Object;
        obj->addPair( "id", String::make( id ) );
        return obj;
    }

    void fromJson(const Object* json)
    {
        id = json->get<int>( "id" );
    }
};

struct MyObject {
    int integer;
    bool boolean;
    string str;
    MySubObj sub;
    vector<int> array;
};

int main (int argc, char **argv)
{
    
    /*
    string toParse;
    while (cin)
    {
        string temp;
        cin >> temp;
        toParse += temp;
    }
    cout << Parser::parse( toParse ) << endl;
    */

    // /*

    MyObject o;
    o.integer = 1337;
    o.boolean = false;
    o.str = "KTHXBYE";
    
    o.array.push_back(1);
    o.array.push_back(9);
    o.array.push_back(8);
    o.array.push_back(9);

    MySubObj so;
    so.id = 42;

    Object jo;

    jo.addPair( "integer",  String::make( o.integer ) );
    jo.addPair( "boolean",  String::make( o.boolean ) );
    jo.addPair( "str",      String::make( o.str ) );
    jo.addPair( "sub", &so );

    Array* ja = new Array;
    for (unsigned int i = 0; i < o.array.size(); ++i)
    {
        ja->push_back( String::make( o.array[i] ) );
    }
    jo.addPair( "array", ja );

    // Test of JsonObject.print()
    cout << "Encoded object : \n" << &jo << '\n' << endl;

    // Test of parser
    stringstream ss;
    ss << &jo;
    Object* parsed = Parser::parse( ss.str() );

    cout << "Parsed object : \n" << parsed << endl;

    // Retrieval and reconstruction
    MyObject anotherO;
    
    anotherO.integer = parsed->get<int>( "integer" );
    anotherO.boolean = parsed->get<bool>( "boolean" );
    anotherO.str = parsed->get<std::string>( "str" );
    anotherO.sub = parsed->getObject<MySubObj>( "sub" );

    // Another way to do the same thing
    // anotherO.integer = JsonUtils::get<int>( (*parsed)["integer"] );
    // anotherO.boolean = JsonUtils::get<bool>( (*parsed)["boolean"] );
    // anotherO.str = JsonUtils::get<std::string>( (*parsed)["str"] );
    // anotherO.sub = JsonUtils::getObject<MySubObj>( (*parsed)["sub"] );

    /*
    // The line 112 is equivalent to the commented following lines
    const Array* array = parsed->getArray( "array" );
    for (unsigned int i = 0; i < array->size(); ++i)
    {
        anotherO.array.push_back( array->get<int>(i) );
    }
    */
    vector<int>* parray = 
    parsed->getCompletedArray<int, vector, JsonUtils::GetAlgorithm >( "array" );
    anotherO.array = *parray;
    delete parray;

    cout    << "\nAnother object...\n"
            << "Integer : " << anotherO.integer << "\n"
            << "Boolean : " << ((anotherO.boolean) ? "true" : "false") << "\n"
            << "String : " << anotherO.str << "\n"
            << "Sub object id : " << anotherO.sub.id << "\n"
            << "3rd value of table : " << anotherO.array[3] << "\n"
            << endl;

    delete parsed;

    // */
    return 0;
}

