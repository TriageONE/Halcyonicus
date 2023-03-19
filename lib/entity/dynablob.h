//
// Created by Aron Mantyla on 3/17/23.
//

#ifndef HALCYONICUS_DYNABLOB_H
#define HALCYONICUS_DYNABLOB_H

#include <cstdlib>
#include <cstring>

/**
 * The dynablob class is a special class designed to accomodate the needs of the mapped attributes of the entity class container.
 * a dynablob's point is to assist in wrapping a predefined primitive or datum into a single storable dynamic class. This class
 * can be interpreted as anything, but this also holds the initial immutable primitive that was previously defined. If you call
 *  the interpretation function, it should return a correctly typed datum that represents the wrapped data.
 */


class DYNABLOB{
public:
    enum TYPE{
        BYTE,
        CHAR,
        UBYTE,
        PACK,

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

public:

    DYNABLOB(void * data, int size, TYPE interpretation){
        this->data = malloc(size);
        memcpy(this->data, data, size);
        this->type = interpretation;
    }

    ~DYNABLOB(){
        free(data);
    }

    void* getRaw(){
        return this->data;
    }

    void setRaw(void* d){
        memcpy(this->data, d, sizeof(data));
    }

    TYPE getType(){
        return this->type;
    }
};
#endif //HALCYONICUS_DYNABLOB_H
