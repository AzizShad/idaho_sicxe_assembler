#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "file_parser.h"
#include "file_parse_exception.h"

using namespace std;

/**
 * Constrctor
 */
file_parser::file_parser(string filename){
    this->filename = filename;
    source = NULL;
}

/**
 * Destructor
 */
file_parser::~file_parser(){
    delete source;
}

/**
 * Reads a file by line,
 */
void file_parser::read_file(){
    source = get_file_contents();

    istringstream sstream (*source);
    string str;
    while (getline(sstream, str)) {
        struct line line_item;
        // Detect comments and labels from str
        if ( str.size() > 0 ){
            // Reassigns str without any comments.
            str = tokenize_comment(str, line_item);
            
        }
        
        // Finally then pass to tokenizer
        set_token(line_item);
    }
}

/**
 *  
 */
string file_parser::get_token(unsigned int row, unsigned int column){
    try {
        switch (column) {
            case 0:
                return container.at(row).getlabel();
            case 1:
                return container.at(row).getopcode();
            case 2:
                return container.at(row).getoperand();
            case 3:
                return container.at(row).getcomment();
            default:
                throw file_parse_exception("Error: get token column is out of bounds, must be between 0 and 3");
        }
    } catch (out_of_range &oor) {
        throw file_parse_exception("Error: get token row is out of bounds, must be between 0 and size()-1");
    }
}

/**
 *  
 */
 void file_parser::set_token(struct line line_item){
     container.push_back( line_item );
 }

/**
 * Prints the contents of the file to stdout
 */
void file_parser::print_file(){
	vector<line>::iterator iter;
	for ( iter = container.begin() ; iter < container.end() ; iter++ ){
<<<<<<< HEAD
        // iomanip
=======
        cout << iter->label << setw(16) << iter->opcode << setw(16) << iter->operand << setw(16) << iter->comment << setw(16) << endl;
>>>>>>> 57a617d284d0a5ca700360b620f8f022c691d261
	}
}

/**
 * Returns the number of lines of the file.
 */
int file_parser::size(){
	return (int)container.size();
}
<<<<<<< HEAD

/**
 * Returns a pointer to a string with the file's contents
 */

const string* file_parser::get_file_contents() {
    if (source != NULL) {
        delete source;
    }
    
    ifstream stream;
    stream.open(filename.c_str(), ios::in);
    
    if (!stream.is_open()) {
        throw file_parse_exception("Error: could not open file "+filename+". Check that the file exists or was properly entered.");
    }
    stream.seekg(0, ios::end);
    long long filesize = stream.tellg();
    stream.seekg(0, ios::beg);
    
    char* buffer = NULL;
    buffer = new char[filesize+1];
    if (buffer == NULL) {
        stream.close();
        throw file_parse_exception("Error: could not allocate memory to read source file.");
    }
    stream.read(buffer, filesize);
    buffer[filesize] = '\0';
    string* contents = new string(buffer);
    
    stream.close();
    delete[] buffer;
    return contents;
}

string file_parser::tokenize_comment(string line, struct line &line_item){
    n = line.find('.');
    if ( n != string::npos ){
        line_item.setcomment(line.substr(n));
        // Resizes the string to not include the full stop
        // and anything to the right of the full stop.
        line = line.substr(0, n);
    }
    return line;
}
=======
>>>>>>> 57a617d284d0a5ca700360b620f8f022c691d261
