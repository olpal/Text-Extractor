/*
 * @file    main.cpp
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The main entry point for calculating probabilities
 *
 */

#include "helper.h"
#include "utility.h"

using namespace std;
bool writeData = false;
extern const string probabilitiesFile;

int main( int argc, char * argv[] ) {
    cout << "Generating Probability Map...\n";
    generateProbabilityMap(&preProccessFile);
    cout << "Begining probability calculations...\n";
    calculateProbabilities();
    cout << "Writing output file to: " << probabilitiesFile << "\n";
    outputProbabilities(&probabilitiesFile);
    cout << "Probability process is now complete\n";

    return 0;
}

