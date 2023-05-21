//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <sstream>
#include <random>

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

std::string ENTITY::getAttribute(const std::string& attribute) {
    std::map<std::string,std::string>::iterator it;
    it = attributes.find(attribute);
    if (it == attributes.end()) return "NULL";
    return attributes.find(attribute)->second;
}

bool ENTITY::isErrored() const {
    return errored;
}

bool ENTITY::isUntyped() const{
    return missingType;
}

bool ENTITY::hasUUID() const{
    return uuid != 0;
}


void ENTITY::setAttribute(const std::string& dblob, const std::string& attribute) {
    attributes.erase(attribute);
    attributes.insert( std::pair<std::string, std::string>(attribute, dblob) );
}

void ENTITY::setAttributes(std::map<std::string, std::string> *attrs){
    this->attributes = std::move(*attrs);
}

void ENTITY::setUUID(unsigned long long newUUID){
    this->uuid = newUUID;
}

std::map<std::string, std::string> ENTITY::getAllAttributes() {
    return this->attributes;
}

std::string ENTITY::serializeEntity(){
    // Ignore the type, just worry about extraneous data and location

    ENTITYLOCATION el = this->getLocation();
    cfloat x(0), y(0), z(0);

    x = el.getX();
    y = el.getY();
    z = el.getZ();

    std::stringstream ss;

    char uid[8] {'\0'};
    ::memcpy(uid, &this->uuid, 8);
    std::string xs = x.serialize(), ys = y.serialize(), zs = z.serialize();

    ss <<
        '{' <<  //0
        xs <<   //1-6
        ys <<   //7-12
        zs <<   //13-18
        uid <<  //19-26
        '{';    //27

    for (std::pair<std::string, std::string> p : this->attributes){
        ss << '[' << p.first << p.second << ']';
    }

    ss << "}}";
    return ss.str();

}

ENTITY ENTITY::deserializeEntity(std::string entityString){

    if (entityString[0] != '{'){
        std::cerr << "Attempted to deserializeEntity a malformed entityString, stuck on first char, expected \'{\', got " << entityString[0] << std::endl;
        return ENTITY();
    }
    if (entityString[27] != '{'){
        std::cerr << "Attempted to deserializeEntity a malformed entityString, stuck on attribute section initializer, expected \'{\', got " << entityString[19] << std::endl;
        return ENTITY();
    }

    unsigned long long len = entityString.length();
    if (entityString[len-1] != '}'){
        std::cerr << "Attempted to deserializeEntity a malformed entityString, stuck on last char escape, expected \'}\', got " << entityString[len-1] << std::endl;
        return ENTITY();
    }
    if (entityString[len-2] != '}'){
        std::cerr << "Attempted to deserializeEntity a malformed entityString, stuck on attribute section escape, expected \'}\', got " << entityString[len-2] << std::endl;
        return ENTITY();
    }


    ENTITYLOCATION el;
    cfloat x(0), y(0), z(0);

    std::string substr = entityString.substr(1,6);
    x = cfloat::deserializeToNewCFloat(substr);

    substr = entityString.substr(7,6);
    y = cfloat::deserializeToNewCFloat(substr);

    substr = entityString.substr(13,6);
    z = cfloat::deserializeToNewCFloat(substr);

    el.setX(x);
    el.setY(y);
    el.setZ(z);

    substr = entityString.substr(19, 8);
    unsigned long long uid = *(unsigned long long*) substr.c_str();

    ENTITY e = ENTITY(el, uid);

    int place = 28; // Start on the first char of the first attribute.
    if (place >= len-2) return e;

    std::map<std::string, std::string> attributes;
    //Loop through until we find a bracket, however if we reach len-2 then we are done

    while (place < len - 1){
        // When we find a bracket, start reading the name into the stringstream
        if (entityString[place] == '['){
            std::stringstream ss;
            place++;
            while(entityString[place] != '{'){
                if (place >= len-2){
                    std::cerr << "WARN: Entity type deserialization failed, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << std::endl;
                    return e;
                }
                ss << entityString[place];
                place++;
            }
            std::string attrName = ss.str();
            ss.str("");
            ss.clear();
            ss << entityString[place];
            place++;
            while (entityString[place] != '}'){
                if (place >= len-2){
                    std::cerr << "WARN: Entity type deserialization failed, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << std::endl;
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
    std::cout << "ENTITY OUTPUT:" << std::endl << "\tTYPE: \"" << this->type << "\"" << std::endl;
    std::cout << "\tERRORED: " << ((this->errored)? "TRUE" : "FALSE") << std::endl;
    std::cout << "\tX: " << this->getLocation().getX().asString()  << std::endl;
    std::cout << "\tY: " << this->getLocation().getY().asString()  << std::endl;
    std::cout << "\tZ: " << this->getLocation().getZ().asString() << std::endl;

    std::cout << "ATTRIBUTES: {" << std::endl;
    for (std::pair<std::string, std::string> p : this->attributes){
        std::cout << "\t\t" << p.first << " : " << p.second << std::endl;
    }
    std::cout << "}\n\rEND ENTITY OUTPUT" << std::endl;

}

bool ENTITY::removeAttribute(const std::string& attribute) {
    return this->attributes.erase(attribute);
}

bool ENTITY::hasAttribute(const std::string& attribute){
    return this->attributes.contains(attribute);
}

unsigned long long ENTITY::generateAndSetNewUUID(){
    std::random_device rd;  // Seed for the random number engine
    std::mt19937_64 engine(rd());  // 64-bit Mersenne Twister random number engine
    std::uniform_int_distribution<unsigned long long> dist(0ULL, std::numeric_limits<unsigned long long>::max());

    // Generate a random number
    this->uuid = dist(engine);
    return uuid;
}
