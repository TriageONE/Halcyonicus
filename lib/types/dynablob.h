//
// Created by Aron Mantyla on 3/17/23.
//

#ifndef HALCYONICUS_DYNABLOB_H
#define HALCYONICUS_DYNABLOB_H


#include <cstdlib>
#include <iostream>
#include <sstream>

/**
 * The dynablob class is a special class designed to accomodate the needs of the mapped attributes of the entity class container.
 * a dynablob's point is to assist in wrapping a predefined primitive or datum into a single storable dynamic class. This class
 * can be interpreted as anything, but this also holds the initial immutable primitive that was previously defined.
 */

class DYNABLOB{
public:

    enum TYPE {
        BYTE, CHAR, UCHAR,

        SHORT, USHORT,

        INT, UINT, FLOAT,

        CFLOAT,

        LONG, ULONG, DOUBLE,

        STR
    };

private:

    TYPE type;
    void * data;
    unsigned short size;
    bool err = false;

    DYNABLOB(){
        this->err = true;
    }

public:

    template <class T> DYNABLOB(T data, TYPE interpretation){
        this->data = malloc(sizeof(data));
        std::cout << "Data size is " << sizeof data << std::endl;
        memcpy(this->data, &data, sizeof(data));
        this->type = interpretation;
        this->size = sizeof(data);
        this->err = false;
    }

    DYNABLOB(std::string data){
        this->data = malloc(data.length());

        memcpy(this->data, data.c_str(), data.length());
        std::cout << "DYNBLOB Data: " << this->data << std::endl;
        this->type = DYNABLOB::STR;
        this->size = data.size();
        this->err = false;
    }

    ~DYNABLOB(){
        free(data);
    }

    [[nodiscard]] bool isErrored() const{
        return this->err;
    }

    void* getRaw(){
        return this->data;
    }


    void setRaw(void* d){
        memcpy(this->data, d, sizeof(data));
    }

    /**
     * In order to serialize anything at all, we need to determine some conventions
     * We can use the enum TYPE above to decide what we do with it, but we need to serializeEntity that too
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
    std::string serialize(){
        std::stringstream ss;

        ss << '{' << (char) (this->type + 48);

        if (this->type == TYPE::STR){

            char aChar[2];
            ::memcpy(aChar, &this->size, 2);

            ss << (aChar)[0] << (aChar)[1];
        }

        for (int c = 0; c < this->size; c++){
            char tem = ((char*) this->data)[c];
            ss << tem ;

            std::cout << "Char is " << tem << ", " << ss.str().size() << std::endl;
        }
        ss << "}";

        return ss.str();

    }

    static DYNABLOB deserialize(std::string s){
        if (s.size() < 3) {
            std::cerr << "Dynablob deserialization could not complete due to the string passed being to small, must be at least 3 characters long (initiator, payload type, payload-optional, terminator)! Passed: " << s << std::endl;
            return DYNABLOB();
        }
        if (s[0] != '{') {
            std::cerr << "Attempted to deserialize a bad dynablob, stuck on first char, expected \'{\', got " << s[0] << std::endl;
            return DYNABLOB();
        }
        TYPE t = (TYPE) (int(s[1]) - 48);
        short dLen;

        switch (t) {
            case BYTE:
            case CHAR:
            case UCHAR:
                dLen = 1;
                break;
            case SHORT:
            case USHORT:
                dLen = 2;
                break;
            case INT:
            case UINT:
            case FLOAT:
                dLen = 4;
                break;
            case CFLOAT:
                dLen = 6;
                break;
            case LONG:
            case ULONG:
            case DOUBLE:
                dLen = 8;
                break;
            case STR:
                char size[2];
                size[0] = s[2];
                size[1] = s[3];
                dLen = *(short*) size;
                if (dLen == 0 || s.size() == 5) {
                    std::cerr << "Attempted to deserialize a bad dynablob, string type used, but length was notated as " << dLen << " which can't work because there was no payload, full string was: " << s << std::endl;
                    return DYNABLOB();
                }
                break;

        }
        short readPoint = 2;
        if (t == STR) readPoint = 4;
        char terminator = s[s.size()-1];

        if (terminator != '}') {
            std::cerr << "Attempted to deserialize a bad dynablob, serial terminator '}' not found, got '" << terminator << "\' within "<< s << " at space " << readPoint + dLen << std::endl;
            return DYNABLOB();
        }

        std::stringstream data;

        if (readPoint + 1 != s.length()) {
            for (short tracker = 0; tracker < dLen; readPoint++, tracker++){
                data << s[readPoint];
            }
        }

        return {data.str(), t};

    }

};
/**
 * DYNABLOB CONSISTENCY TEST:

/////////// CODE: ///////////

#include <iostream>
#include <dynablob.h>   // MAKE SURE TO GET THE EXACT REFERENCE
#include <cfloat.h>      // MAKE SURE TO GET THE EXACT REFERENCE

int main() {
    string s = "0123456789abcdef1234567890abcde";
    int i = INT32_MAX;
    char c = '!';
    long double f = i+ 0.19f;
    long long L = (long long) f;
    cfloat cf = 100.18f;


    DYNABLOB blob = DYNABLOB(s, DYNABLOB::STR);
    DYNABLOB blob2 = DYNABLOB(i, DYNABLOB::INT);
    DYNABLOB blob3 = DYNABLOB(c, DYNABLOB::CHAR);
    DYNABLOB blob4 = DYNABLOB(L, DYNABLOB::LONG);
    DYNABLOB blob5 = DYNABLOB(f, DYNABLOB::DOUBLE);
    DYNABLOB blob6 = DYNABLOB(cf, DYNABLOB::CFLOAT);

    string serial1 = blob.serialize();
    string serial2 = blob2.serialize();
    string serial3 = blob3.serialize();
    string serial4 = blob4.serialize();
    string serial5 = blob5.serialize();
    string serial6 = blob6.serialize();


    DYNABLOB b1 = DYNABLOB::deserialize(serial1);
    DYNABLOB b2 = DYNABLOB::deserialize(serial2);
    DYNABLOB b3 = DYNABLOB::deserialize(serial3);
    DYNABLOB b4 = DYNABLOB::deserialize(serial4);
    DYNABLOB b5 = DYNABLOB::deserialize(serial5);
    DYNABLOB b6 = DYNABLOB::deserialize(serial6);

    cout << serial1 << ", " << serial2 << ", " << serial3 << ", " << serial4 << ", " << serial5 << ", " << serial6<< endl;
    cout << (char*) b1.getRaw() << ", " << *(int*) b2.getRaw() << ", " << *(char*) b3.getRaw() << ", " << *(long*) b4.getRaw() << ", " << *(long double*) b5.getRaw() << ", "  << *(cfloat*) b6.getRaw() << endl;

    return 0;
}

/////////// OUTPUT: ///////////

{< 0123456789abcdef1234567890abcde}, {5���}, {1!}, {9   �    }, {;      �A}, {8d   .}
0123456789abcdef1234567890abcde, 2147483647, !, 2147483648, 2.14748e+09, 100.1800

 */
#endif //HALCYONICUS_DYNABLOB_H
