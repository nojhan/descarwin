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
        json->unpack( "id" , id );
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
    char temp[1024];
    while (cin)
    {
        cin.getline( temp, 1024, '\n' );
        toParse += temp;
        toParse += '\n';
    }
    cout << Parser::parse( toParse ) << endl;
    */

    ///*

    MyObject o;
    o.integer = 1337;
    o.boolean = false;
    o.str = "KTHXBYE \
             What if you add spaces and new lines ? I guess they are deleted :)";
    
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
    
    parsed->unpack( "integer",  anotherO.integer );
    parsed->unpack( "boolean", anotherO.boolean );
    parsed->unpack( "str", anotherO.str );
    parsed->unpackObject( "sub", anotherO.sub );

    // Another way to do the same thing
    // anotherO.integer = JsonUtils::get<int>( (*parsed)["integer"] );
    // anotherO.boolean = JsonUtils::get<bool>( (*parsed)["boolean"] );
    // anotherO.str = JsonUtils::get<std::string>( (*parsed)["str"] );
    // anotherO.sub = JsonUtils::getObject<MySubObj>( (*parsed)["sub"] );

    const Array* array = static_cast<Array*>( (*parsed)[ "array" ] );
    for (unsigned int i = 0; i < array->size(); ++i)
    {
        int integer;
        array->unpack( i, integer );
        anotherO.array.push_back( integer );
    }

    /*
    // TODO no more doable, see Array.h
    vector<int>* parray = 
    parsed->getCompletedArray<int, vector>( "array", JsonUtils::GetAlgorithm<int, String>()  );
    anotherO.array = *parray;
    delete parray;
    */

    cout    << "\nAnother object...\n"
            << "Integer : " << anotherO.integer << "\n"
            << "Boolean : " << ((anotherO.boolean) ? "true" : "false") << "\n"
            << "String : " << anotherO.str << "\n"
            << "Sub object id : " << anotherO.sub.id << "\n"
            << "2rd value of table : " << anotherO.array[2] << "\n"
            << endl;

    delete parsed;

    //*/
    return 0;
}

