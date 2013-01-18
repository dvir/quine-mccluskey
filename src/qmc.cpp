/**
 * Minimize Boolean Algebra Function - Quine-McCluskey algorithm implementation.
 *
 * @author: Dvir Azulay (dvir.azulay@gmail.com)
 *
 * @Example:
 *   - Compiling: g++ -g -Wall -Weffc++ -o qmc qmc.cpp -lboost_program_options
 *   - ./qmc -l 4 --sop 0 1 2 5 6 7 8 9 10 14
 *   - Output: f = cd' + b'c' + a'bd
*/

#include <vector>
#include <queue>
#include <tr1/unordered_map>
#include <iostream>
#include <ostream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include "../include/binary.h"
#include "../include/term.h"


typedef std::vector<Term*> Terms;
typedef std::tr1::unordered_map<size_t, Terms> ImplicantsMap;

std::string terms_function(const std::vector<Term*>& terms, bool debug=false) {
    std::string minimized_function;
    if (terms.size() > 0) {
        minimized_function.append(terms.at(0)->getLiterals());

        if (debug) {
            minimized_function.append(" (");
            minimized_function.append(terms.at(0)->getDecimals());
            minimized_function.append(")");
        }
    }
    
    for (Terms::const_iterator it = terms.begin()+1; 
        it < terms.end(); ++it) 
    {
        minimized_function.append(" + ");
        minimized_function.append((*it)->getLiterals());

        if (debug) {
            minimized_function.append(" (");
            minimized_function.append((*it)->getDecimals());
            minimized_function.append(")");
        }
    }

    return minimized_function;
}

std::ostream& operator<<(std::ostream& output, const std::vector<std::string>& list) {
    if (list.size() > 0) {
        output << list.at(0);
    }

    for (std::vector<std::string>::const_iterator it = list.begin()+1;
         it != list.end(); ++it)
    {
        output << " " << *it;
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const Terms& terms) {
    if (terms.size() > 0) {
        output << terms.at(0)->getLiterals() << " (" << terms.at(0)->getDecimals() << ")";
    }

    for (Terms::const_iterator it = terms.begin()+1;
         it != terms.end(); ++it)
    {
        output << (*it)->getLiterals() << " (" << (*it)->getDecimals() << ")";
    }

    return output;
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    int literals_count;
    bool debug;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("sop", po::value< std::vector<int> >()->multitoken(), "SOP terms")
        ("pos", po::value< std::vector<int> >()->multitoken(), "POS terms")
        ("dont-care", po::value< std::vector<int> >()->multitoken(), "Dont-Care terms")
        ("literals-count,l", po::value<int>(&literals_count)->required(), "Literals count")
        ("debug", po::value<bool>(&debug)->default_value(false), "Show debug information")
    ;

//    po::positional_options_description p;
//    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc)
//              .positional(p)
              .run()
              , vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    po::notify(vm);

    Terms terms;
    Terms dontCareTerms;
    Terms primeTerms;

    if (vm.count("sop")) {
        BOOST_FOREACH (int term_val, vm["sop"].as< std::vector<int>  >())
        {
            Term* term = new Term(term_val, literals_count);
            terms.push_back(term);
        }
    }
    
    if (vm.count("pos")) {
        BOOST_FOREACH (int term_val, vm["pos"].as< std::vector<int>  >())
        {
            // calculate the SOP term for the POS term - 2^(literals) - 1 - minterm
            Term* term = new Term(pow(2, literals_count) - 1 - term_val, literals_count);
            terms.push_back(term);
        }
    }
    
    if (vm.count("dont-care")) {
        BOOST_FOREACH (int term_val, vm["dont-care"].as< std::vector<int>  >())
        {
            Term* term = new Term(term_val, literals_count);
            term->dontCare();
            terms.push_back(term);
            dontCareTerms.push_back(term);
        }
    }

    std::sort(terms.begin(), terms.end(), Term::PointerCompare());

    std::queue<Term*> termsQueue;
    for (Terms::iterator it = terms.begin(); 
        it < terms.end(); ++it) 
    {
        termsQueue.push(*it);
    }

    termsQueue.push(0); // separating the algorithm steps
    
    Terms currentTerms;
    Terms nextTerms;
    while (!termsQueue.empty()) {
        // 1. go over the queue, while adding to current terms
        //    a. if 0 or different number of ones, stop adding.
        //    b. if 0, add unselected to primes.
        //    c. if number of ones is bigger by one, fetch until 0 or different number of ones.
        //    d. compare and push to queue.
        //    e. clear next and current.

        Term* first;
        Term* front;
        bool match_terms = true;
        bool find_next_terms = true;
        if (currentTerms.empty()) {
            // get the first term in the queue
            first = termsQueue.front();
            
            // get all the terms in the queue with the same amount of
            // ones in the binary representation
            while (!termsQueue.empty()) {
                front = termsQueue.front();
                
                if (front == 0 || first == 0) {
                    termsQueue.pop();
                    match_terms = false;
                    find_next_terms = false;
                    break;
                }
                
                if ((front->oneCount() - first->oneCount()) > 1) {
                    match_terms = false;
                    find_next_terms = false;
                    break;
                }

                if (front->oneCount() != first->oneCount()) {
                    // difference is exactly 1.
                    // we should start adding next terms.
                    break;
                }
                
                currentTerms.push_back(front);
                termsQueue.pop();
            }
        }

        // by now we have some currentTerms, and are ready to insert
        // to nextTerms.

        nextTerms.clear();
        
        if (find_next_terms && currentTerms.size() > 0) {
            // get the first term in the queue
            first = termsQueue.front();

            // get all the terms in the queue with the same amount of
            // ones in the binary representation
            while (!termsQueue.empty()) {
                front = termsQueue.front();

                if (front == 0 || first == 0) {
                    termsQueue.pop();
                    termsQueue.push(0);
                    break;
                }

                if (front->oneCount() != first->oneCount()) {
                    // we should start processing the current terms sets.
                    break;
                }

                nextTerms.push_back(front);
                termsQueue.pop();
            }
        }
        
        if (match_terms && currentTerms.size() > 0 && nextTerms.size() > 0) {
            // now we have two lists of terms - current and next, which
            // are adjacent groups in a Quine-McCluskey table.
            // we should compare each current term and next term and
            // push new terms to the end of the queue.
            for (Terms::iterator c_it = currentTerms.begin();
                 c_it < currentTerms.end(); ++c_it)
            {
                Term* current_term = *c_it;
                for (Terms::iterator n_it = nextTerms.begin();
                     n_it < nextTerms.end(); ++n_it)
                {
                    Term* next_term = *n_it;
                    int separating_bit = current_term->separatingBit(*next_term);
                    if (separating_bit > -1) {
                        current_term->select();
                        next_term->select();

                        Term* new_term = new Term(*current_term, 
                                                  *next_term, 
                                                  separating_bit+1);
                        termsQueue.push(new_term);
                        terms.push_back(new_term); // add to heap pointers vector
                    }
                }
            }

        }

        // if we got here, we should try to add unselected terms
        // from both current terms and next terms and then clear both lists.

        for (Terms::iterator it = currentTerms.begin();
             it < currentTerms.end(); ++it)
        {
            if (!(*it)->isSelected()) {
                // make sure that we don't add terms that are made of
                // only dont care terms.
                if ((*it)->isDontCare()) {
                    continue;
                }

                // make sure we don't have that term in the list already
                // (that can happen because 0,8-1,9 are equal to 0,1-8,9, for example)
                bool is_duplicate = false;

                for (Terms::iterator pt_it = primeTerms.begin(); 
                    pt_it < primeTerms.end(); ++pt_it) 
                {
                    if ((*pt_it)->getExpression() == (*it)->getExpression()) {
                        is_duplicate = true;
                        break;
                    }
                }

                if (!is_duplicate) {
                    primeTerms.push_back(*it);
                }
            }
        }

        currentTerms = nextTerms;
    }
    
//    std::cout << "pre-minimize: f = " << terms_function(primeTerms) << std::endl;

    Terms primeImplicants;
    ImplicantsMap implicantsMap;
    for (Terms::iterator it = primeTerms.begin(); 
        it < primeTerms.end(); ++it) 
    {
        Term* term = *it;
        term->primeImplicantCandidate();
        std::vector<size_t> minterms = term->getMinterms();
        for (std::vector<size_t>::iterator mt_it = minterms.begin();
             mt_it < minterms.end(); ++mt_it)
        {
            implicantsMap[*mt_it].push_back(term);
        }
    }

    // remove dontcare minterms from implicantsMap, as we don't need to
    // actually cover them.
    for (Terms::iterator it = dontCareTerms.begin();
         it < dontCareTerms.end(); ++it)
    {
        size_t minterm = (*it)->getDecimal();
        for (Terms::iterator t_it = implicantsMap[minterm].begin();
             t_it < implicantsMap[minterm].end(); ++t_it)
        {
            (*t_it)->coverMinterm(minterm);
        }

        implicantsMap[minterm].clear();
    }


    for (ImplicantsMap::iterator main_it = implicantsMap.begin();
         main_it != implicantsMap.end(); ++main_it)
    {
        // find minterms that are only covered by one implicant, and remove any
        // term from the map that cover the minterms that implicant does.
        bool found_prime_implicant = false;
        for (ImplicantsMap::iterator it = implicantsMap.begin();
             it != implicantsMap.end(); ++it)
        {
            if ((*it).second.size() == 1) {
                Term* term = (*it).second.at(0);
                
                // we found a new prime implicant!
//                std::cout << "New (natural) prime implicant for term " << (*it).first << "! " << term->getExpression() <<
//                        " (" << term->getDecimals() << ")" << std::endl;
//                std::cout << (*it).second << std::endl;

                term->primeImplicant();
                found_prime_implicant = true;

                primeImplicants.push_back(term);
                std::vector<size_t> minterms = term->getMinterms();
                for (std::vector<size_t>::iterator mt_it = minterms.begin();
                     mt_it < minterms.end(); ++mt_it)
                {
                    for (Terms::iterator t_it = implicantsMap[*mt_it].begin();
                         t_it < implicantsMap[*mt_it].end(); ++t_it)
                    {
                        (*t_it)->coverMinterm(*mt_it);
                    }

                    implicantsMap[*mt_it].clear();
                }
            }
        }

        if (!found_prime_implicant) {
            // choose an arbitrary implicant to be a prime implicant
            // -- we are choosing the first term we can find that appears the most
            std::sort(primeTerms.begin(), 
                      primeTerms.end(), 
                      Term::PrimeImplicantCandidatesCompare());

            // find the first non-prime implicant term
            Term* term = 0;
            for (Terms::iterator pt_it = primeTerms.begin();
                 pt_it < primeTerms.end(); ++pt_it)
            {
                if (!(*pt_it)->isPrimeImplicant() 
                    && (*pt_it)->getRemainingMinterms().size() > 0) 
                {
                    term = *pt_it;
                    break;
                }
            }
            
            if (term == 0) {
                // we don't have any more prime terms! we are done.
                break;
            }

            // we found a new prime implicant!
//            std::cout << "New prime implicant! " << term->getExpression() <<
//                    " (" << term->getDecimals() << ")" << std::endl;
            term->primeImplicant();
            primeImplicants.push_back(term);

            std::vector<size_t> minterms = term->getMinterms();
            for (std::vector<size_t>::iterator mt_it = minterms.begin();
                 mt_it < minterms.end(); ++mt_it)
            {
                for (Terms::iterator t_it = implicantsMap[*mt_it].begin();
                     t_it < implicantsMap[*mt_it].end(); ++t_it)
                {
                    (*t_it)->coverMinterm(*mt_it);
                }

                implicantsMap[*mt_it].clear();
            }
        }
    }

    std::cout << "f = " << terms_function(primeImplicants, debug) << std::endl;
    
    // clean everything!
 
    for (Terms::iterator it = terms.begin(); 
         it < terms.end(); ++it) 
    {
        delete (*it);
        *it = 0;
    }
}
