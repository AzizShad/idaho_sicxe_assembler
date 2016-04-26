/* Shad Aziz, Phillip Domann, Melanie Reed, Matt Walther
   masc0851
   Team Idaho
   prog4
   CS530, Spring 2016
*/

#include <map>
#include <string>
#include <sstream>
#include "symtab.h"
#include "symtab_exception.h"

using namespace std;

void symtab::add(string label, struct symbol value){
    transform(label.begin(), label.end(), label.begin(), ::toupper);
	symbol_iter = symbol_table.find(label);
	if( symbol_iter != symbol_table.end() ) {
		throw symtab_exception("Symbol " + label + " already declared");
    }
    	symbol_table[label] = value; 
}

struct symtab::symbol symtab::get(string label){
    transform(label.begin(), label.end(), label.begin(), ::toupper);
	symbol_iter = symbol_table.find(label);
	if( symbol_iter == symbol_table.end() ) {
		throw symtab_exception("Label " + label + " does not exist");
	 }
	return symbol_iter->second;
}
