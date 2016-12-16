/*
 * @file    utility.h
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The utility header file to provide utility functions
 *
 */

#ifndef SCANNER_UTILITY_H
#define SCANNER_UTILITY_H

#include "helper.h"
#include <fstream>

void processToken( ofstream *, TokenType, TokenStruct & );
void loadStopWords(void);
void calculateProbabilities(void);
void calculateProbabilities(void);
void generateProbabilityMap(const string *);
void outputProbabilities(const string *);
void loadProbabilityMap(const string *);
void outputToken(void);
void outputMultiwords(const string *);
void loadPreProcessed(bool, bool, const string *, bool);
void outputMultiwordsCsv(const string * fileName);
void outputMultiwordsJson(const string * fileName);
void outputSinglewordsJson(const string * outputFile);
void outputSinglewordsVertical(const string * outputFile);

#endif //SCANNER_UTILITY_H
