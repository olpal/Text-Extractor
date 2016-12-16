/*
 * @file    helper.h
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The helper header file contains all central structs and variables needed by all other files
 *
 */

#ifndef SCANNER_HELPER_H
#define SCANNER_HELPER_H

#include <string>
#include <vector>
#include <regex>
#include <stack>
#include <map>
#include <unordered_map>
#include <iostream>
#include <list>
#include <math.h>
#include <deque>

using namespace std;

typedef enum {
    //Tag tokens
            OPENTAG, CLOSETAG,
    //Text Tokens
            WORD, APOSTROPHE, DASH,
    //Numbers
            NUMBER,
    //Punctuation
            PERIOD, QUESTION, BANG, OTHERPUNCT,
    //File tokens
            ENDFILE, ERROR, NEWLINE
} TokenType;

typedef enum {
    /*Write status flags*/
    WRITE, NOWRITE, IGNORE
} OutputStatus;

typedef enum{
    DOC, DATE, TOPICS, BODY
} ReadStatus;

/*Tag struct to store the HTML tag type, and its output status*/
struct TagStruct {
    string tagName;
    OutputStatus outputStatus;
};

/*Struct for holding token information*/
struct TokenMeta{
    float probability;
    int count;
    int tokens;
    float avp;
    float square;
};

/*Token struct for storing value and input file position*/
struct TokenStruct {
    string value;
    int row;
};

/*Structure to hold token and scores*/
struct TokenScore{
    string token;
    float score;
};

struct proccessed{
    string date;
    vector<string> topics;
    vector<string> words;
    vector<TokenScore> multiwords;
};

/*Input file row number*/
extern int rowno;

extern bool writeData;

/*This map is used for offline processing*/
extern unordered_map<string,TokenMeta> wordMap;

/*This vector is used to store all stop words*/
extern vector<string> stopWords;

/*Filenames*/
extern const string stopWordsFile;
extern const string preProccessFile;

/*File Output stream*/
extern ofstream outstream;

#endif //SCANNER_HELPER_H
