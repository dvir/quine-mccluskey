#include "../include/binary.h"

Binary::Binary() : 
    _num(0) 
{
}

Binary::Binary(unsigned int num) : 
    _num(num) 
{
}

Binary::~Binary() { 
}

std::string Binary::getBinary() const {
    if (_num == 0) {
        return "0";
    }

    unsigned int num = _num;
    std::string binary;

    while (num > 0) {
        std::string mod = "0";
        if (num % 2 == 1) {
            mod = "1";
        }
       
        binary.insert(0, mod);
        num = num / 2;
    }

    return binary;
}

unsigned int Binary::at(size_t i) const {
    return (_num & (2 << i));
}

unsigned int Binary::oneCount(std::string binary) const {
    unsigned int one_count = 0;
    for (size_t i = 0; i < binary.length(); i++) {
        if (binary.at(i) == '1') {
            one_count++;
        }
    }

    return one_count;
}

unsigned int Binary::oneCount() const {
    return oneCount(getBinary());
}

unsigned int Binary::getDecimal() const {
    return _num;
}
