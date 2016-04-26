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
     
     struct symbol {
         bool isaddress;
         string value;
     };
     
    symtab();
  	void add(string, struct symbol);
 	struct symbol get(string);
 	bool exists(string);

 private:
    map<string, struct symbol> symbol_table;
	map<string, struct symbol>::iterator symbol_iter;
 };

 #endif
