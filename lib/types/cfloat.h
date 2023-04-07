//
// Created by Aron Mantyla on 4/5/23.
//
#include <iostream>
#include <sstream>

#ifndef HALCYONICUS_CFLOAT_H
#define HALCYONICUS_CFLOAT_H

class cfloat{

    struct __attribute__ ((packed)){
        int whole {};
        unsigned short fraction {};
    };

public:

    cfloat(int whole = 0, unsigned short fraction = 0) : whole(whole), fraction(fraction) {}

    cfloat(float f ) {
        this->whole = (int) f;
        double temp = f - (uint) f;
        this->fraction = (temp * UINT16_MAX);
    }

    cfloat(double f ) {
        this->whole = (int) f;
        double temp = f - (uint) f;
        this->fraction = (temp * UINT16_MAX);
    }

    cfloat() = default;

    friend cfloat operator+(const cfloat& lhs, const cfloat& rhs) {
        int w = lhs.whole + rhs.whole;
        int frac = lhs.fraction + rhs.fraction;
        if (frac >= 65535) {
            w++;
            frac -= 65535;
        }
        return cfloat(w, frac);
    }

    friend cfloat operator-(const cfloat& lhs, const cfloat& rhs) {
        int w = lhs.whole - rhs.whole;
        int frac = lhs.fraction - rhs.fraction;
        if (frac < 0) {
            w--;
            frac += 65535;
        }
        return cfloat(w, frac);
    }

    friend cfloat operator*(const cfloat& lhs, const cfloat& rhs) {
        long long val = (lhs.whole * 100000LL + lhs.fraction) * (rhs.whole * 100000LL + rhs.fraction);
        int i = val / 65535;
        short frac = val % 65535;
        return cfloat(i, frac);
    }

    friend cfloat operator/(const cfloat& lhs, const cfloat& rhs) {
        long long val = (lhs.whole * 100000LL + lhs.fraction) * 100000LL / (rhs.whole * 100000LL + rhs.fraction);
        int i = val / 65535;
        short frac = val % 65535;
        return cfloat(i, frac);
    }

    friend bool operator==(const cfloat& lhs, const cfloat& rhs) {
        return lhs.whole == rhs.whole && lhs.fraction == rhs.fraction;
    }

    friend bool operator!=(const cfloat& lhs, const cfloat& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const cfloat& f) {
        os << f.whole << ".";

        os << getPercentDiff(f);
        return os;
    }

    [[nodiscard]] static short getPercentDiff(const cfloat& f) {
        double percentage = (static_cast<double>(f.fraction) / static_cast<double>(UINT16_MAX)) * 100.0;
        return (percentage * 100 ) + 1;
    }

    [[nodiscard]] short getPercentDiff() {
        double percentage = (static_cast<double>(this->fraction) / static_cast<double>(UINT16_MAX)) * 100.0;
        return (percentage * 100 ) + 1;
    }

    string serialize(){
        stringstream ss;
        int i = 0;
        for (; i < sizeof(int); i++){
            ss << ((char*) &this->whole)[i];
        }
        for (; i < sizeof(short) + sizeof(int); i++){
            ss << ((char*) &this->fraction)[i];
        }
        return ss.str();
    }

    void deserialize(string s){
        int i = 0;
        for (; i < sizeof(int); i++){
            ((char*) &this->whole)[i] = s[i];
        }
        for (; i < sizeof(short) + sizeof(int); i++){
            ((char*) &this->fraction)[i] = s[i];
        }
    }

    static cfloat deserializeToNewCFloat(string s){
        int i = 0;
        cfloat c(0.0f);
        for (; i < sizeof(int); i++){
            ((char*) &c.whole)[i] = s[i];
        }
        for (; i < sizeof(short) + sizeof(int); i++){
            ((char*) &c.fraction)[i] = s[i];
        }
        return c;
    }

    string asString(){
        stringstream ss;
        ss << this->whole << ".";

        ss << getPercentDiff();
        return ss.str();
    }


};


#endif //HALCYONICUS_CFLOAT_H
