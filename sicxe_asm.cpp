/* Phillip Domann, Shad Aziz, Melanie Reed, Matt Walther
   masc0832
   Team Idaho
   prog3
   CS530, Spring 2016
*/

#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctype.h>
#include "sicxe_asm.h"
#include "file_parse_exception.h"
#include "opcode_error_exception.h"
#include "symtab_exception.h"

using namespace std;


// Converts a hex string to an integer value
int hextoi(string str) {
    int integer;
    stringstream ss;
    ss << hex << str;
    ss >> integer;
    return integer;
}


// Checks that a string range contains only digits
bool isdecimal(string& str, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}


// Checks that a string range contains only hexadecimal digits
bool ishexadecimal(string& str, size_t start, size_t end) {
    string upper = str;
    transform(upper.begin(),upper.end(), upper.begin(), ::toupper);
    for (size_t i = start; i < end; ++i) {
        if (!(isdigit(upper[i]) || (65 <= upper[i] && upper[i] <= 70))) {
            return false;
        }
    }
    return true;
}

// Verifies that a string contains vaild hex or decimal digits
bool isconstant(string& str) {
    if (str.empty())
        return false;
    return (str[0] == '$') ? ishexadecimal(str, 1, str.length())
    	: isdecimal(str, 0, str.length());
}


// Converts a string constant to an integer
int ctoi(string& str) {
    return (str[0] == '$') ? hextoi(str.substr(1)) : hextoi(str);
}

// Verifies that a literal is quoted
bool isquoted(string& str) {
    return ((str[1] == '\'') && (str[str.length()-1] == '\'')) ? true : false;
}

// Verifies that a string is a valid literal string
bool isliteral(string& str) {
    if (str.empty() || str.length() < 3)
        return false;
    if ((str[0] == 'c') || (str[0] == 'C'))
        return isquoted(str);
    else if (((str[0] == 'x') || (str[0] == 'X')) && (str.length()%2 == 1)) {
        return (isquoted(str)) ? ishexadecimal(str, 2, str.length()-1): false;
    }
    return false;
}

// Returns the size for a quoted literal
unsigned int size_for_literal(string& str) {
    return ((str[0] == 'c') || (str[0] == 'C')) ? (unsigned int)str.length()-3
    	: (unsigned int)(str.length()-3)>>1;
}

// Converts an integer to a string
string itos(int integer) {
    stringstream itoss;
    itoss << integer;
    return itoss.str();
}

// Searches for the start directive
bool is_start( string opcode ){
    transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);
    return ( opcode.compare("START") == 0 ? true : false );
}

// Searches for the end directive
bool is_end( string opcode ){
    transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);
    return ( opcode.compare("END") == 0 ? true : false );
}

const struct sicxe_asm::dhpair sicxe_asm::dhpairs[9] = {
    {"START", &sicxe_asm::handle_start},
    {"END", &sicxe_asm::handle_directive},
    {"BYTE", &sicxe_asm::handle_byte_word},
    {"WORD", &sicxe_asm::handle_byte_word},
    {"RESB", &sicxe_asm::handle_resb},
    {"RESW", &sicxe_asm::handle_resw},
    {"BASE", &sicxe_asm::handle_directive},
    {"NOBASE", &sicxe_asm::handle_directive},
    {"EQU", &sicxe_asm::handle_equ}
};

sicxe_asm::sicxe_asm(string file) {
    intermed_filen = file;
    parser = new file_parser(file);
    locctr = 0;
    setup_handler_map();
    string listingFile = file.substr(0, file.rfind("."));
    listingFile = listingFile + ".lis";
    listing.open(listingFile.c_str());
    listing_head(file);
}

sicxe_asm::~sicxe_asm() {
    listing.close();
    delete parser;
}

void sicxe_asm::error_str(string msg) {
    string str = "***Error***\n";
    str.append(msg);
    throw str;
}

void sicxe_asm::error_ln_str(string msg) {
    string str = "***Error***\n";
    string lnmsg = "Line "+itos(index+1)+": "+label+" "+opcode+" "+operand+"\n";
    str.append(lnmsg);
    str.append(msg);
    throw str;
}

void sicxe_asm::pass1() {
    parser->read_file();
    
    unsigned int nlines = (unsigned int)parser->size();
    for (index = 0; index < nlines; ++index) {
        get_tokens();
        if (is_start(opcode)) {
            handle_start();
            listing_lnout();
            break;
        } else if (!opcode.empty()) {
            error_ln_str("There must be no operations before start directive.");
        } else {
            listing_lnout();
            line_addrs.push_back(locctr);
        }
    }
    
    if (index == nlines) {
        error_str("There is no start directive in sourcefile.");
    }
    sym_handler handle_symbol;
    for (++index; index < nlines; ++index) {
        line_addrs.push_back(locctr);
        get_tokens();
        listing_lnout();
        if (is_end(opcode)) {
            handle_directive();
            break;
        }
        else {
            handle_symbol = handler_for_symbol();
            (this->*handle_symbol)();
        }
    }
    
    if (index == nlines) {
        error_str("There is no end directive in sourcefile.");
    }
    for (++index; index < nlines; ++index) {
        line_addrs.push_back(locctr);
        get_tokens();
        listing_lnout();
        if (!opcode.empty()) {
            error_ln_str("Additional operations cannot exist after end directive.");
        }
    }
}

void sicxe_asm::pass2() {
    parser = new file_parser(file);
    
    unsigned int nlines = (unsigned int)parser->size();
    for ( index = 0 ; index < nlines ; index++ ){
        get_tokens();
        if ( is_start(opcode) ){
            
        }
    }
}

void sicxe_asm::get_tokens() {
    label = parser->get_token(index, 0);
    opcode = parser->get_token(index, 1);
    operand = parser->get_token(index, 2);
}

void sicxe_asm::listing_head(string filename) {
    int width = 38-(int)(filename.length()>>1);
    listing << setw(width) << "**" << filename << "**" << "\n\n";
    listing << setw(5) << "Line#";
    listing << setw(8) << " Address";
    listing << left << setw(16) << " Label";
    listing << setw(16) << "Opcode";
    listing << setw(36) << "Operand";
    listing << endl;
    listing << setw(5) << "=====" << setw(8) << " =======" << setw(16);
    listing << " =====" << setw(16) << "======" << setw(36) << "=======";
    listing << endl;
}

void sicxe_asm::listing_lnout() {
    listing << right << setw(5) << dec << index+1;
    listing << "   " << setw(5) << hex << uppercase << setfill('0') << locctr;
    listing << " " << left << setw(15) << setfill(' ') << label;
    listing << setw(16) << opcode;
    listing << setw(36) << operand;
    listing << endl;
}

void sicxe_asm::setup_handler_map() {
    unsigned int i;
    for (i = 0; i < sizeof(opcodetab::instrs)/sizeof(opcodetab::instr); ++i) {
        string instr = opcodetab::instrs[i].menmonic;
        transform(instr.begin(), instr.end(), instr.begin(), ::toupper);
        hmap.insert(make_pair(instr, &sicxe_asm::handle_instruction));
    }
    
    for (i = 0; i < sizeof(sicxe_asm::dhpairs)/sizeof(sicxe_asm::dhpair); ++i) {
        hmap.insert(make_pair(sicxe_asm::dhpairs[i].directive,
        	sicxe_asm::dhpairs[i].handler));
    }
}

sicxe_asm::sym_handler sicxe_asm::handler_for_symbol() {
    if (opcode.empty()) {
        return &sicxe_asm::handle_empty;
    }
    string symbol = (opcode[0] == '+') ? opcode.substr(1, string::npos) : opcode;
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
    map<string, sym_handler>::iterator iter = hmap.find(symbol);
    if (iter == hmap.end()) {
        error_ln_str("Invalid opcode.");
    }
    return iter->second;
}

void sicxe_asm::handle_instruction() {
    try {
        locctr += optab.get_instruction_size(opcode);
    }
    catch (opcode_error_exception e) {
        error_ln_str(e.getMessage());
    }
    add_symbol_for_label();
}

void sicxe_asm::handle_start() {
    if (isconstant(operand))
        locctr = ctoi(operand);
    else
        error_ln_str("Invalid constant.");
    
    try {
        symbols.add(opcode, label);
    }
    catch (symtab_exception e) {
        error_ln_str(e.getMessage());
    }
}

void sicxe_asm::handle_byte_word() {
    if (isliteral(operand))
        locctr += size_for_literal(operand);
    else if (isconstant(operand))
        locctr += (opcode[0] == 'b' || opcode[0] == 'B') ? 1: 3;
    else
        error_ln_str("Invalid quoted literal.");
    add_symbol_for_label();
}

void sicxe_asm::handle_resb() {
    if (isconstant(operand))
        locctr += ctoi(operand);
    else
        error_ln_str("Invalid constant.");
    add_symbol_for_label();
}

void sicxe_asm::handle_resw() {
    if (isconstant(operand))
        locctr += ctoi(operand)*3;
    else
        error_ln_str("Invalid constant.");
    add_symbol_for_label();
}

void sicxe_asm::handle_equ() {
    try {
        symbols.add(label, operand);
    }
    catch (symtab_exception e) {
        error_ln_str(e.getMessage());
    }
}

void sicxe_asm::handle_directive() {
    try {
        symbols.add(opcode, operand);
    }
    catch (symtab_exception e) {
        error_ln_str(e.getMessage());
    }
}


void sicxe_asm::handle_empty() {
    add_symbol_for_label();
}

void sicxe_asm::add_symbol_for_label() {
    if (!label.empty()) {
        try {
            symbols.add(label, itos(locctr));
        }
        catch (symtab_exception e) {
            error_ln_str(e.getMessage());
        }
    }
}
//returns objCode for format 3
void sicxe_asm::format3(){
   string tempOperand = operand;
   int addressCode;
   bool isX;
   nixbpe = 0;
   try {
      //Checks whether it has a symbol infront and changes the flags accordingly
      if(tempOperand[0] == '@'){
         nixbpe |= 0x20;
         isX = false;
         tempOperand = tempOperand.substr(1,tempOperand.size()-1);
      }
      else if(tempOperand[0] == '#'){
         nixbpe |= 0x10;
         isX = false;
         tempOperand = tempOperand.substr(1,tempOperand.size()-1);
      }
      else{
         nixbpe |= 0x20;
         nixbpe |= 0x10;
         isX = true;
      }
      //Checks if the operand has a X register then changes flags accordingly
      //If there is something else after the ',' then it throws an error
      if(tempOperand.find(',') != -1){
         string registerX = tempOperand.substr(tempOperand.find(','),tempOperand.size()-1);
         string rand1 = tempOperand.substr(0, tempOperand.find(','));
         if(registerX == "X" || registerX == "x" && isX){
            nixbpe |= 0x8;
         }else if(!registerX.empty()){
            throw;
         }
      }
      struct sicxe_asm::symbol sym;
      //gets address portion and checks if its a constant or an address.   
      if(!sym.isaddress){
         addressCode = sym.value;
      }else{
         addressCode = getDisplacement(sym.value,line_addrs.at(index) + 3);
      }
      
      int instruction = 0;
      instruction = hextoi(optab.get_machine_code(opcode)) << 18;
      instruction |= nixbpe << 12;
      instruction |= addressCode;
      objCode = itos(instruction, 6);
   }
   catch (opcode_error_exception e) {
      error_ln_str(e.getMessage());
   }
}

void sicxe_asm::format4(){
   string tempOpcode = opcode.substr(1,opcode.size()-1);
   string tempOperand = operand;
   int addressCode;
   bool isX;
   nixbpe = 0;
   try {
      nixbpe |= 0x1;
      if(tempOperand[0] == '@'){
         if(!isalpha(tempOperand[1])){
            throw;
         }
         nixbpe |= 0x20;
         isX = false;
         tempOperand = tempOperand.substr(1,tempOperand.size()-1);
      }
      else if(tempOperand[0] == '#'){
         nixbpe |= 0x10;
         isX = false;
         tempOperand = tempOperand.substr(1,tempOperand.size()-1);
      }
      else{
         nixbpe |= 0x20;
         nixbpe |= 0x10;
         isX = true;
      }
      if(tempOperand.find(',') != -1){
         string registerX = tempOperand.substr(tempOperand.find(','),tempOperand.size()-1);
         string rand1 = tempOperand.substr(0, tempOperand.find(','));
         if(registerX == "X" || registerX == "x" && isX){
            nixbpe |= 0x8;
         } else if(!registerX.empty()){
         throw;
         }
      }
      
      struct sicxe_asm::symbol sym;
      
      if(!sym.isaddress){
         addressCode = sym.value;
      }else{
         addressCode = getDisplacement(sym.value, line_addrs.at(index) + 4);
      }
      int intruction = 0;
      instruction = hextoi(optab.get_machine_code(TempOpcode)) << 26;
      instruction |= nixpbe << 20;
      instruction |= addressCode;
      objCode = itos(instruction, 8);
   }
   catch (opcode_error_exception e) {
      error_ln_str(e.getMessage());
   }
}

int sicxe_asm::getDisplacement( int addr1, int addr2 ){
    int disp = addr1 - addr2;
    int baseDisp = addr1 - base_addr;
    
    if ( disp >= -2048 && disp <= 2047 ){
        nixbpe = 0x2;
        return disp;
    } else if (!noBase && baseDisp >= 0 && baseDisp <= 4095){
        nixbpe = 0x4;
        return baseDisp;
    }else {
       error_ln_str("Addressing displacement out of bounds, use format 4");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Proper usage is " << argv[0] << " filename. " << endl;
        return 1;
    }
    
    sicxe_asm assembler(argv[1]);
    
    try {
    	assembler.pass1();
    }
    catch (file_parse_exception fpe) {
        cout << fpe.getMessage();
    }
    catch (opcode_error_exception oee) {
        cout << oee.getMessage();
    }
    catch (symtab_exception se) {
        cout << se.getMessage();
    }
    catch (string str) {
        cout << str;
    }
    
    try {
        assembler.pass2();
        return 0;
    }
    
    return 1;
}
