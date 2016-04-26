/* Shad Aziz, Phillip Domann, Melanie Reed, Matt Walther
   masc0851
   Team Idaho
   prog4
   CS530, Spring 2016
*/

#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>

#ifndef SYMTAB_H_
#define SYMTAB_H_

using namespace std;

 class symtab {
 public:
     // Holds symbol value and if it is a constant or address
     struct symbol {
         bool isaddress;
         string value;
     };
	// Adds a symbol to the tab, checking if it already exists.
	void add(string, struct symbol);
    //Returns the symbol throwing an error if it does not exist.
	struct symbol get(string);

 private:
	// Symbol tab and iterator
	map<string, struct symbol> symbol_table;
	map<string, struct symbol>::iterator symbol_iter;
 };

 #endif
