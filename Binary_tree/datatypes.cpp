#include "datatypes.h"
#include <iostream>
using namespace std;

/**
 * Returns the ASCII sum of all characters in a string
 * To be used as the indexing key of a string into the hash table,
 * e.g. k = sumStringChars(str)
*/
unsigned int sumStringChars(const string& str) 
{
    unsigned int sum = 0;
    for (unsigned int i=0; i<str.length(); ++i)
        sum += static_cast<unsigned int>(str[i]);
    return sum;
}


// VaccinationStatus default constructor
VaccinationStatus::VaccinationStatus() {
    numVaccines=0;
    for (unsigned int i=0; i<VACCINE_TABLE_SIZE; i++){
        vaccineHashTable[i]="";
    }

}

// VaccinationStatus::addVaccine(const string&)
// - If the table is not at least half empty, output the first error message and return.
// - Else, calculate k = sumStringChars(v) and hash(k) = k % VACCINE_TABLE_SIZE
//   Follow the quadratic probing formula: h(i) = [hash(k) + i^2] % VACCINE_TABLE_SIZE
//   to add the vaccine to the table.
//   If the vaccine already exists, output the second error message and return.
void VaccinationStatus::addVaccine(const string& v)
{
    if (numVaccines>VACCINE_TABLE_SIZE/2 /* TODO: Table is not half-empty */) {
        cout << "This system does not support having taken more than " << numVaccines << " vaccines." << endl;
        return;
    }

    //Quadratic probing
    unsigned int k=sumStringChars(v)%VACCINE_TABLE_SIZE;
    unsigned int i=0;
    bool exist=false;
    while (1){
        unsigned int hash=(k+i*i)%VACCINE_TABLE_SIZE;
        if (vaccineHashTable[hash].empty()){
            vaccineHashTable[hash]=v;
            numVaccines++;
            return;
        }
        else if (vaccineHashTable[hash]==v){
            exist=true;
            break;
        }
        else{
            
            i++;
        } 
    }

    if (exist) {
        cout << "This animal has already taken " << v << "." << endl;
        return;
    }

}

// VaccinationStatus::hasVaccine(const string&) const
// Returns true if the vaccine exists in the table, false otherwise
bool VaccinationStatus::hasVaccine(const string& v) const
{
    unsigned int k=sumStringChars(v)%VACCINE_TABLE_SIZE;
    unsigned int i=0;
    while (1){
        unsigned int hash=(k+i*i)%VACCINE_TABLE_SIZE;
        if (vaccineHashTable[hash].empty()){
            return false;
        }
        else if (vaccineHashTable[hash]==v){
            return true;
        }
        else
            i++;
    }

    return false;
}


/**
 * Returns a value representing the current hash table
 * Used for comparing 2 animals with different vaccination status
*/
unsigned int VaccinationStatus::getTotalHashValue() const 
{
    unsigned int hash = 0;
    for (unsigned int i=0; i<VACCINE_TABLE_SIZE; ++i)
        hash += sumStringChars(vaccineHashTable[i]);
    return hash * numVaccines;
}
