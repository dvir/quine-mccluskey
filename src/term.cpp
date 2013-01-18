#include "../include/term.h"

#include <algorithm>
#include <cmath>
#include <sstream>

Term::Term(unsigned int num, size_t literals_count) : 
    Binary(num), 
    _dashes(), 
    _minterms(),
    _remaining_minterms(),
    _selected(false),
    _prime_implicant(false),
    _dont_care(false),
    _literals_count(literals_count)
{
    _minterms.push_back(num);
}

Term::Term(Term& first, Term& second, size_t new_dash) : 
    Binary(std::max(first.getDecimal(), second.getDecimal())),
    _dashes(),
    _minterms(),
    _remaining_minterms(),
    _selected(false),
    _prime_implicant(false),
    _dont_care(first.isDontCare() && second.isDontCare()),
    _literals_count(std::max(first.getLiteralCount(), second.getLiteralCount()))
{
    for (std::vector<size_t>::iterator it = first.getMinterms().begin();
         it < first.getMinterms().end(); ++it)
    {
        _minterms.push_back(*it);
    }
    
    for (std::vector<size_t>::iterator it = second.getMinterms().begin();
         it < second.getMinterms().end(); ++it)
    {
        _minterms.push_back(*it);
    }

    for (std::vector<size_t>::iterator it = first.getDashes().begin();
         it < first.getDashes().end(); ++it)
    {
        _dashes.push_back(*it);
    }

    for (std::vector<size_t>::iterator it = second.getDashes().begin();
         it < second.getDashes().end(); ++it)
    {
        _dashes.push_back(*it);
    }

    _dashes.push_back(new_dash);
}

Term::~Term() { }

std::string Term::getExpression() const {
    std::string expression = getBinary();
    expression.insert(0, _literals_count - expression.length(), '0');

    for (std::vector<size_t>::const_iterator it = _dashes.begin();
         it < _dashes.end(); ++it)
    {
        expression.replace(expression.length() - (*it), 1, "-");
    }

    return expression;
}

void Term::addDash(size_t i) {
    _dashes.push_back(i);
}

unsigned int Term::oneCount() const {
    return Binary::oneCount(getExpression());  
}

bool Term::operator<(const Term& other) const {
    return (this->oneCount() < other.oneCount());
}

bool Term::operator>(const Term& other) const {
    return (this->oneCount() > other.oneCount());
}

bool Term::isSelected() const {
    return _selected;
}

void Term::select() {
    _selected = true;
}

bool Term::isPrimeImplicant() const {
    return _prime_implicant;
}

void Term::primeImplicant() {
    _prime_implicant = true;
}

bool Term::isDontCare() const {
    return _dont_care;
}

void Term::dontCare() {
    _dont_care = true;
}

char Term::charAt(size_t i) const {
    if (i >= getExpression().size()) {
        return '0';
    }

    return getExpression().at(getExpression().length() - i - 1);
}

int Term::separatingBit(const Term& other) const {
    int separating_bit = -1;
    int max_num = std::max(_num, other.getDecimal());
    for (size_t i = 0; max_num > 0; ++i) {
        if (charAt(i) != other.charAt(i)) {
            if (separating_bit != -1) {
                // we already had one bit separating between
                // the terms! that means the terms are separated
                // by two or more bits.
                return -1;
            }

            separating_bit = i;
        }

        max_num = max_num / 2;
    }

    return separating_bit;
}

std::vector<size_t>& Term::getMinterms() {
    return _minterms;
}

std::vector<size_t>& Term::getDashes() {
    return _dashes;
}

std::string Term::getDecimals() const {
    std::stringstream decimals;
    if (_minterms.size() > 0) {
        decimals << _minterms.at(0);
    }

    for (std::vector<size_t>::const_iterator it = _minterms.begin()+1;
         it < _minterms.end(); ++it)
    {
        decimals << "," << *it;
    }

    return decimals.str();
}

size_t Term::getLiteralCount() const {
    return _literals_count;
}

std::string Term::getLiterals() const {
    std::string expression = getExpression();
    std::string literal;
    for (size_t i = 0; i < _literals_count; ++i) {
        char c = '0';
        if (i < expression.length()) {
            c = expression.at(i);
        }

        if (c != '-') {
            literal += (char)(i + 'a');
        }

        if (c == '0') {
            literal += '\'';
        }
    }
    
    return literal;
}

std::string Term::getDashesIndices() const {
    std::stringstream indices;
    if (_dashes.size() > 0) {
        indices << _dashes.at(0);
    }

    for (std::vector<size_t>::const_iterator it = _dashes.begin()+1;
         it < _dashes.end(); ++it)
    {
        indices << "," << *it;
    }

    return indices.str();
}

void Term::coverMinterm(size_t i) {
    std::vector<size_t>::iterator position = std::find(_remaining_minterms.begin(), _remaining_minterms.end(), i);
    if (position != _remaining_minterms.end()) {
        _remaining_minterms.erase(position);
    }
}

void Term::primeImplicantCandidate() {
    _remaining_minterms = _minterms;   
}

std::vector<size_t> Term::getRemainingMinterms() const {
    return _remaining_minterms;
}

std::ostream& operator<<(std::ostream &strm, const Term& term) {
    return strm << term.getLiterals();
}
