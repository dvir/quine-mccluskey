#ifndef TERM_H
#define TERM_H

#include "../include/binary.h"

#include <vector>
#include <string>

class Term : public Binary {
    public:
        struct PointerCompare {
            bool operator()(const Term* left, const Term* right) {
                return (*left) < (*right);
            }
        };

        struct PrimeImplicantCandidatesCompare {
            bool operator()(const Term* left, const Term* right) {
                return (left->getRemainingMinterms().size() >= right->getRemainingMinterms().size());
            }
        };

        Term(unsigned int num, size_t literals_count);

        Term(Term& first, Term& second, size_t new_dash); 
        
        virtual ~Term();

        std::string getExpression() const;

        void addDash(size_t i);

        virtual unsigned int oneCount() const;

        bool operator<(const Term& other) const;

        bool operator>(const Term& other) const;

        bool isSelected() const;

        void select();
        
        bool isPrimeImplicant() const;

        void primeImplicant();
        
        bool isDontCare() const;

        void dontCare();
        
        virtual char charAt(size_t i) const;
        
        int separatingBit(const Term& other) const;

        std::vector<size_t>& getMinterms();
        
        std::vector<size_t>& getDashes();

        std::string getDecimals() const;
        
        size_t getLiteralCount() const;

        std::string getLiterals() const;

        std::string getDashesIndices() const;

        void coverMinterm(size_t i);

        void primeImplicantCandidate();

        std::vector<size_t> getRemainingMinterms() const;

    private:
        std::vector<size_t> _dashes;
        std::vector<size_t> _minterms;
        std::vector<size_t> _remaining_minterms;
        bool _selected;
        bool _prime_implicant;
        bool _dont_care;
        size_t _literals_count;
};

#endif
