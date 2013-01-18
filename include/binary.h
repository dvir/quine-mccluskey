#ifndef BINARY_H
#define BINARY_H

#include <string>

class Binary {
    public:
        Binary();

        Binary(unsigned int num); 

        virtual ~Binary();

        std::string getBinary() const;
        
        virtual unsigned int at(size_t i) const;

        virtual unsigned int oneCount(std::string binary) const;

        virtual unsigned int oneCount() const;
        
        unsigned int getDecimal() const;

    protected:
        unsigned int _num;
};

#endif
