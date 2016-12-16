/*
 * @file    main.cpp
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The main entry point for extracting multiwords
 *
 */

#include "helper.h"
#include "utility.h"

using namespace std;
bool writeData = false;
extern const string multiWordFileJsonAvp;
extern const string multiWordFileJsonSquare;
extern const string multiWordFileAvp;
extern const string multiWordFileSquare;
extern const string probabilitiesFile;

int main( int argc, char * argv[] ) {
    cout << "Loading Probability Map...\n";
    loadProbabilityMap(&probabilitiesFile);
    cout << "Loading Preprocessed File for avp terms\n";
    loadPreProcessed(true, false, &preProccessFile, true);
    cout << "Extracting multiword terms for avp normalization...\n";
    outputMultiwords(&multiWordFileAvp);
    outputMultiwordsJson(&multiWordFileJsonAvp);
    cout << "Loading Preprocessed File for square terms\n";
    loadPreProcessed(false, true, &preProccessFile, true);
    cout << "Extracting multiword terms for square normalization...\n";
    outputMultiwords(&multiWordFileSquare);
    outputMultiwordsJson(&multiWordFileJsonSquare);
    cout << "Multiword term extraction is now complete\n";

    return 0;
}