/* Tests ! */
# include <iostream>
using namespace std;

# include "Json.h"
using namespace eoserial;

struct MySubObj : public Persistent {
    public:

    MySubObj() {}

    int id;
    Object* pack() const
    {
        Object* obj = new Object;
        obj->add( "id", make( id ) );
        return obj;
    }

    void unpack(const Object* json)
    {
        eoserial::unpack( *json, "id" , id );
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

    jo.add( "integer",  make( o.integer ) );
    jo.add( "boolean",  make( o.boolean ) );
    jo.add( "str",      make( o.str ) );
    jo.add( "sub", &so );

    /*
    // Line 81 is equivalent to the following commented lines
    Array* ja = new Array;
    for (unsigned int i = 0; i < o.array.size(); ++i)
    {
        ja->push_back( make( o.array[i] ) );
    }
    */
    Array* ja = makeArray< vector<int>, MakeAlgorithm >( o.array );
    jo.add( "array", ja );


    // Test of JsonObject.print()
    cout << "Encoded object : \n";
    jo.print( cout );
    cout << '\n' << endl;

    // Test of parser
    stringstream ss;
    jo.print( ss );
    Object* parsed = Parser::parse( ss.str() );

    cout << "Parsed object : \n";
    parsed->print( cout );
    cout << endl;

    // Retrieval and reconstruction
    MyObject anotherO;
    unpack( *parsed, "integer", anotherO.integer );
    unpack( *parsed, "boolean", anotherO.boolean );
    unpack( *parsed, "str", anotherO.str );
    unpackObject( *parsed, "sub", anotherO.sub );
    /*
    // Line 115 is equivalent to the following commented lines.
    const Array* array = static_cast<Array*>( (*parsed)[ "array" ] );
    for (unsigned int i = 0; i < array->size(); ++i)
    {
        int integer;
        array->unpack( i, integer );
        anotherO.array.push_back( integer );
    }
    */
    unpackArray< vector<int>, Array::UnpackAlgorithm >( *parsed, "array", anotherO.array );    


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

