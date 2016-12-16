/*
 * @file    tokenizer.h
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The tokenizer header file defines the functions and variables
 * needed to initialize token collections, process tokens and
 * provide a token stack
 *
 */

#ifndef SCANNER_TOKENIZER_H
#define SCANNER_TOKENIZER_H

#include "helper.h"

//A collection of all TagStructs indexed by their text identifier
extern vector<TagStruct> *tagCollection;
extern TagStruct unknownTag;

//Stack to store tags
extern stack<TagStruct> tagStack;

//MARK PUBLIC METHODS
void initTagCollection(void);
TokenType getToken(void);
string normalizeTag(string);
TagStruct getTagStruct(string);
TokenType pushStack(string);
TokenType popStack(string);


#endif //SCANNER_TOKENIZER_H
