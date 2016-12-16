/*
 * @file    main.cpp
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The main entry point for the application
 *
 */

#include "helper.h"
#include "tokenizer.h"
#include "utility.h"

using namespace std;

// allocate global variables
extern FILE * yyin;
extern FILE * yyout;
extern TokenStruct yylval;
ofstream outstream;
int rowno = 1;
bool writeData = false;
extern const string preProccessFileJson;
extern const string preProccessFile;
extern const string preProccessFileTemp;

int main( int argc, char * argv[] ) {
    //Initialize variables
    yyin = stdin;
    yyout = stdout;
    TokenType ttype;
    outstream.open(preProccessFileTemp, ofstream::app);

    cout << "Intitalizing tag collection...\n";
    initTagCollection();

    cout << "Intitalizing stop words collection...\n";
    //Load stop words
    loadStopWords();

    cout << "Begining preprocess file generation...\n";
    //Execute program
    while( (ttype = getToken()) != ENDFILE ) {
        processToken(&outstream, ttype, yylval);
    }
    outstream.close();
    cout << "Loading temporary preprocessed file...\n";
    loadPreProcessed(false, false, &preProccessFileTemp, false);
    cout << "Generating vertical file...\n";
    outputSinglewordsVertical(&preProccessFile);
    cout << "Generating preprocessed Json file...\n";
    outputSinglewordsJson(&preProccessFileJson);
    cout << "Creation of pre-processed file is now complete.\n";

    return 0;
}




