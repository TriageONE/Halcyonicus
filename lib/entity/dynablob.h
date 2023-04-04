//
// Created by Aron Mantyla on 3/17/23.
//

#ifndef HALCYONICUS_DYNABLOB_H
#define HALCYONICUS_DYNABLOB_H

#include <cstdlib>
#include <cstring>
#include <bit>
#include "sstream"
#include <algorithm>

/**
 * The dynablob class is a special class designed to accomodate the needs of the mapped attributes of the entity class container.
 * a dynablob's point is to assist in wrapping a predefined primitive or datum into a single storable dynamic class. This class
 * can be interpreted as anything, but this also holds the initial immutable primitive that was previously defined. If you call
 *  the interpretation function, it should return a correctly typed datum that represents the wrapped data.
 */
using namespace std;

class DYNABLOB{
public:

    enum TYPE{
        BYTE,
        UBYTE,
        CHAR,

        SHORT,
        USHORT,

        INT,
        UINT,

        LONG,
        ULONG,

        STR
    };

private:

    TYPE type;
    void * data;
    short size;

public:

    DYNABLOB(void * data, short size, TYPE interpretation){
        this->data = malloc(size);
        memcpy(this->data, data, size);
        this->type = interpretation;
        this->size = size;
    }

    ~DYNABLOB(){
        free(data);
    }

    void* getRaw(){
        return this->data;
    }

    TYPE getType(){
        return this->type;
    }

    short getSize(){
        return this->size;
    }

    void setRaw(void* d){
        memcpy(this->data, d, sizeof(data));
    }




    /**
     * In order to serialize anything at all, we need to determine some conventions
     * We can use the enum TYPE above to decide what we do with it, but we need to serialize that too
     * every dynablob must have a type nomatter what, and if it does not, well you just cant
     * use the class? Whatever
     *
     * TYPE 0-2 = 1 size
     *      3-4 = 2 size
     *      5-6 = 4 size
     *      7-8 = 8 size
     *      9   = variable size, therefore the size comes in 2 bytes shortly after
     *
     * All dynablobs start with a { and end with a }
     * the first byte is the type
     * if the first part is 9 then we interpret as a string where the next 2 bytes are the length
     * the rest of the payload is the data while the last thing is the }
     * @return the serialized output of the dynablob
     */
    string serialize(){
        stringstream ss;
        ss << '{' << (char) this->type;
        string s = *(string*) this->data;
        if (this->type == TYPE::STR){

            if (s.size() > 65535) cerr << "WARN, DYNABLOB SIZE TRUNCATED TO 65535, DATA FIRST 64 CHARS \"" << s.substr(0,64) << "\"..." << endl;
            unsigned short truncSize = s.size() > 65535 ? 65535 : s.size();

            ss << truncSize;
        }
        ss << s << '}';
        return ss.str();

    }

};
#endif //HALCYONICUS_DYNABLOB_H
