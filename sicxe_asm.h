/* Shad Aziz, Phillip Domann, Melanie Reed, Matt Walther
   masc0851
   Team Idaho
   prog4
   CS530, Spring 2016
*/

#ifndef SICXE_ASM_H
#define SICXE_ASM_H

#include <map>
#include "file_parser.h"
#include "opcodetab.h"
#include "symtab.h"

using namespace std;

class sicxe_asm {
    typedef void (sicxe_asm::*sym_handler)();
    
    struct symbol {
        bool isaddress;
        int value;
    };
    // Pass1 and 2 symbol handlers
    struct hpair {
        sym_handler pass1;
        sym_handler pass2;
    };
    
    struct dhpair {
        string directive;
        struct hpair handlers;
    };
    
    static const struct dhpair dhpairs[9];
    
    file_parser* parser;
    opcodetab optab;
    symtab symbols;
    // Symbol handler map to lookup handlers
    map<string, struct sicxe_asm::hpair> hmap;
    // Contains the address for each line
    vector<unsigned int> line_addrs;
    unsigned int index;
    int locctr;
    int base_addr;
    bool noBase;
    int nixbpe;
    string label;
    string opcode;
    string operand;
    string objCode;
    stringstream listing;
    
    // Retrives the line tokens from the file parser
    void get_tokens();
    // Setups the listing file with the program name and column headers
    void listing_head(string filename);
    // Writes a line to the listing file
    void listing_lnout();
    // Creates the map for looking up handlers for instructions and directives
    void setup_handler_map();
    // Retreives the symbol handler for the symbol
    sym_handler handler_for_symbol(bool pass);
    // Symbol handelers for instructions and directives
    void handle_instruction();
    void handle_start();
    void handle_end();
    void handle_byte();
    void handle_word();
    void handle_resb();
    void handle_resw();
    void handle_equ();
    void handle_base();
    void handle_nobase();
    void handle_empty();
    // Pass2 objcode generators
    void format1_objcode();
    void format2_objcode();
    void format3_objcode();
    void format4_objcode();
    void byte_objcode();
    void word_objcode();
    void set_base_address();
    void set_nobase();
    void empty_objcode();
    
    // Adds the label address to the symbol table if it has one
    void add_symbol_for_label();
    // Throws an error string
    void error_str(string msg);
    // Throws an error string also printing the line contents
    void error_ln_str(string msg);
    // Converts a label, numeric constant, or hex constant to a int
    struct symbol symtoval(string symbol);
    bool islabel(string&);
    int hextoi(string str);
    string itos(int integer, int width);
    bool isdecimal(string& str, size_t start, size_t end);
    // Converts a numeric constant or hex constant to a int
    int ctoi(string& str);
    bool isconstant(string& str);
    // Gets pc or base displacement setting flags
    int getDisplacement( int addr1, int addr2 );
    bool isformat3value(int value);
    bool isformat4value(int value);
    bool iswordvalue(int value);
    string get_reg_val(string);
    int str_toint(string);
    string int_tohex_tostr(int);
    
public:
    // Sets up the handler map and listing file
    sicxe_asm(string file);
    ~sicxe_asm();
    
    // Assigns addresses to each line, adds labels and directives to the symtab
    void pass1();
    // Generates objcode and writes line to listing file
    void pass2();
    // Writes the listing file to the filesystem
    void write_listing(string file);
    
};

#endif
