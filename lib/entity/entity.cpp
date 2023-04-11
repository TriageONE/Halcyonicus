//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <sstream>

ENTITYLOCATION ENTITY::getLocation() {
    return this->location;
}

std::string ENTITY::getType() {
    return this->type;
}


void ENTITY::setLocation(ENTITYLOCATION l) {
    this->location = l;
}

void ENTITY::setType(std::string t) {
    this->type = std::move(t);
}

string ENTITY::getAttribute(const std::string& attribute) {
    std::map<std::string,string>::iterator it;
    it = attributes.find(attribute);
    if (it == attributes.end()) return "NULL";
    return attributes.find(attribute)->second;
}

bool ENTITY::getErrored() const {
    return errored;
}

void ENTITY::setAttribute(const std::string& dblob, const std::string& attribute) {
    cout << "Erasing " << attribute<< endl;
    int erased = attributes.erase(attribute);
    cout << "Erased " << erased << " entity attributes " << endl;
    attributes.insert( pair<string, string>(attribute, dblob) );
}

void ENTITY::setAttributes(std::map<string, string> *attrs){
    this->attributes = std::move(*attrs);
}

std::map<string, string> ENTITY::getAllAttributes() {
    return this->attributes;
}

string ENTITY::serializeEntity(){
    // Ignore the type, just worry about extraneous data and location

    ENTITYLOCATION el = this->getLocation();
    cfloat x(0), y(0), z(0);

    x = el.getX();
    y = el.getY();
    z = el.getZ();

    stringstream ss;

    string xs = x.serialize(), ys = y.serialize(), zs = z.serialize();

    //     0     1-6              7-12             13-18             19
    ss << '{' << xs << ys << zs << '{';

    for (pair<string, string> p : this->attributes){
        ss << '[' << p.first << p.second << ']';
    }

    ss << "}}";
    return ss.str();

}

ENTITY ENTITY::deserializeEntity(string entityString){

    if (entityString[0] != '{'){
        cerr << "Attempted to deserializeEntity a malformed entityString, stuck on first char, expected \'{\', got " << entityString[0] << endl;
        return ENTITY();
    }
    if (entityString[19] != '{'){
        cerr << "Attempted to deserializeEntity a malformed entityString, stuck on attribute section initializer, expected \'{\', got " << entityString[19] << endl;
        return ENTITY();
    }

    unsigned long long len = entityString.length();
    if (entityString[len-1] != '}'){
        cerr << "Attempted to deserializeEntity a malformed entityString, stuck on last char escape, expected \'}\', got " << entityString[len-1] << endl;
        return ENTITY();
    }
    if (entityString[len-2] != '}'){
        cerr << "Attempted to deserializeEntity a malformed entityString, stuck on attribute section escape, expected \'}\', got " << entityString[len-2] << endl;
        return ENTITY();
    }


    ENTITYLOCATION el;
    cfloat x(0), y(0), z(0);

    string substr = entityString.substr(1,6);
    x = cfloat::deserializeToNewCFloat(substr);

    substr = entityString.substr(7,6);
    y = cfloat::deserializeToNewCFloat(substr);

    substr = entityString.substr(13,6);
    z = cfloat::deserializeToNewCFloat(substr);


    el.setX(x);
    el.setY(y);
    el.setZ(z);
    ENTITY e = ENTITY(el);

    int place = 20; // Start on the first char of the first attribute.
    if (place >= len-2) return e;

    std::map<std::string, std::string> attributes;
    //Loop through until we find a bracket, however if we reach len-2 then we are done

    while (place < len - 1){
        // When we find a bracket, start reading the name into the stringstream
        if (entityString[place] == '['){
            stringstream ss;
            place++;
            while(entityString[place] != '{'){
                if (place >= len-2){
                    cerr << "WARN: Entity type deserialization failed, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << endl;
                    return e;
                }
                ss << entityString[place];
                place++;
            }
            string attrName = ss.str();
            ss.str("");
            ss.clear();
            ss << entityString[place];
            place++;
            while (entityString[place] != '}'){
                if (place >= len-2){
                    cerr << "WARN: Entity type deserialization failed, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << endl;
                    return e;
                }
                ss << entityString[place];
                place++;
            }
            ss << entityString[place];
            place++;
            //Should now be at the end of the attribute
            attributes.insert({attrName, ss.str()});
        } else {
            place++;
        }
    }
    e.setAttributes(&attributes);
    return e;
}

void ENTITY::out(){
    cout << "ENTITY OUTPUT:" << endl << "\tTYPE: \"" << this->type << "\"" << endl;
    cout << "\tERRORED: " << ((this->errored)? "TRUE" : "FALSE") << endl;
    cout << "\tX: " << this->getLocation().getX().asString()  << endl;
    cout << "\tY: " << this->getLocation().getY().asString()  << endl;
    cout << "\tZ: " << this->getLocation().getZ().asString() << endl;

    cout << "ATTRIBUTES: {" << endl;
    for (pair<string, string> p : this->attributes){
        cout << "\t\t" << p.first << " : " << p.second << endl;
    }
    cout << "}\n\rEND ENTITY OUTPUT" << endl;

}

bool ENTITY::removeAttribute(const string& attribute) {
    return this->attributes.erase(attribute);
}

bool ENTITY::hasAttribute(const string& attribute){
    return this->attributes.contains(attribute);
}
