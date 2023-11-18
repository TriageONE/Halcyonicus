//
// Created by Triage on 2/5/2023.
//


#include "entity.h"


COORDINATE::ENTITYCOORD ENTITY::getLocation() const{
    return this->location;
}

std::string ENTITY::getType() {
    return this->type;
}


void ENTITY::setLocation(COORDINATE::ENTITYCOORD  l) {
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
    std::cout << "Setting attribute on entity " << this->uuid << " as attr class \"" << attribute << "\" and data " << dblob << std::endl;
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

void ENTITY::updateTimestamp(){
    this->timeLastSaved = TIMETOOLS::getCurrentEpochTime();
}

void ENTITY::updateTimestamp(long long time){
    this->timeLastSaved = time;
}

/*
 * Serialization is changing, based on how entities were stored.
 * the original idea was that we would organize our entities into layers, then serialize them all at once and store them in a database of layers sorted by chunks instead
 * The new idea is that entities are now part of a lookable database, able to be sorted and found by UUID in a flat manner only holding data regarding their location, ID, and attributes.
 * The type should be another column as a string only and can be whatever you want
 *
 * The new database schema looks like this
 *
 * UUID(Long long 8) -- X(LONG LONG) -- Y(LONG LONG) -- DATA(Blob)
 *
 * Data should include some non-redundant aspects that matter to the entity such as
 * Facing, Layer, last saved time then Attributes
 */
void ENTITY::serializeEntity(std::vector<char> * serialOut){
    // Ignore the type, just worry about extraneous data and location
    serialOut->clear();
    serialOut->reserve(32);

    //Z aspect, type int
    char zc[4] {'\0'};
    ::memcpy(&zc, &this->location.z, 4);

    //float
    char fs[4] {'\0'};
    ::memcpy(fs, &facing, 4);

    //Time saved as now, 8 bytes
    auto time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    char ts[8] {'\0'};
    ::memcpy(fs, &time, 4);

    serialOut->push_back('{');
    for (char c : zc) serialOut->push_back(c);
    for (char c : fs) serialOut->push_back(c);
    for (char c : ts) serialOut->push_back(c);
    serialOut->push_back('{');

    for (std::pair<std::string, std::string> p : this->attributes){
        serialOut->push_back('[');
        for (char c : p.first) serialOut->push_back(c);
        for (char c : p.second) serialOut->push_back(c);
        serialOut->push_back(']');
    }

    serialOut->push_back('}');
    serialOut->push_back('}');
    return;
}

void ENTITY::decodeEntityData(std::string entityString){

    if (entityString[0] != '{'){
        std::cerr << "Attempted to deserializeEntity a malformed entity String, stuck on first char, expected \'{\', got " << entityString[0] << std::endl;
        return;
    }
    if (entityString[17] != '{'){
        std::cerr << "Attempted to deserializeEntity a malformed entity String, stuck on attribute section initializer, expected \'{\', got " << entityString[15] << std::endl;
        return;
    }

    unsigned long long len = entityString.length();
    if (entityString[len-1] != '}'){
        std::cerr << "Attempted to deserializeEntity a malformed entity String, stuck on last char escape, expected \'}\', got " << entityString[len-1] << std::endl;
        return;
    }
    if (entityString[len-2] != '}'){
        std::cerr << "Attempted to deserializeEntity a malformed entity String, stuck on attribute section escape, expected \'}\', got " << entityString[len-2] << std::endl;
        return;
    }

    std::string substr = entityString.substr(1,4);
    ::memcpy(&this->location.z, substr.c_str(), 2);

    substr = entityString.substr(5,4);
    ::memcpy(&this->facing, substr.c_str(), 4);

    substr = entityString.substr(9,8);
    ::memcpy(&this->timeLastSaved, substr.c_str(), 8);

    int place = 17; // Start on the first char of the first attribute.
    if (place >= len-2) return;

    std::map<std::string, std::string> attrs;
    //Loop through until we find a bracket, however if we reach len-2 then we are done

    while (place < len - 1){
        // When we find a bracket, start reading the name into the stringstream
        if (entityString[place] == '['){
            std::stringstream ss;
            place++;
            while(entityString[place] != '{'){
                if (place >= len-2){
                    std::cerr << "WARN: Entity type deserialization failed, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << std::endl;
                    return;
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
                    std::cerr << "WARN: Entity type deserialization early terminate, end of stream reached while in the middle of creating attribute, place " << place << ", last char: " << entityString[place] << " With full length at " << entityString.length() << std::endl;
                    return;
                }
                ss << entityString[place];
                place++;
            }
            ss << entityString[place];
            place++;
            //Should now be at the end of the attribute
            this->attributes.insert({attrName, ss.str()});
        } else {
            place++;
        }
    }
    return;
}

void ENTITY::out(){
    std::cout << "ENTITY OUTPUT:" << std::endl << "\tTYPE: \"" << this->type << "\"" << std::endl;
    std::cout << "\tERRORED: " << ((this->errored)? "TRUE" : "FALSE") << std::endl;
    std::cout << "\tUID: " << this->uuid << std::endl;
    std::cout << "\tX: " << this->getLocation().x << std::endl;
    std::cout << "\tY: " << this->getLocation().y << std::endl;
    std::cout << "\tZ: " << this->getLocation().z << std::endl;

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

COORDINATE::ENTITYCOORD ENTITY::getLastSavedLocation() {
    return this->lastSavedLocation;
}

void ENTITY::setLastSavedLocation(COORDINATE::ENTITYCOORD  l) {
    this->lastSavedLocation = l;
}

unsigned long long ENTITY::getUUID() const{
    return this->uuid;
}

void ENTITY::setFacing(float newFacing) {
    this->facing = newFacing;
}

float ENTITY::getFacing() const {
    return this->facing;
}

char ENTITY::getLayer() const {
    return this->layer;
}

void ENTITY::setLayer(char newLayer) {
    this->layer = newLayer;
}
