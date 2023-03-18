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

template <class T>
class DYNABLOB{
public:
    enum TYPE{
        BYTE,
        CHAR,
        UBYTE,
        PACK,

        SHORT,
        USHORT,
        FLOAT11,

        INT,
        UINT,
        FLOAT31,

        LONG,
        ULONG,
        FLOAT71,

        STR
    };

private:
    TYPE type;
    T payload;

public:
    /**
     * When creating a new blob, we must know exactly how we want to interpret it. This means that we cant just pass a primitive in and let it be, we have to first deconstruct the data and copy it into an array we have to dynamically define.
     * @tparam T The type of data coming in
     * @param data The data that should be stored
     * @param interpretation what it should be interpreted as
     */
    DYNABLOB(T data, TYPE interpretation){
        this->payload = data;
        this->type = interpretation;
    }



};
#endif //HALCYONICUS_DYNABLOB_H
