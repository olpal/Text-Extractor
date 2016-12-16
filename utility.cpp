/*
 * @file    utility.cpp
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The utility file is used to hold utility functions to support the main program
 *
 */

#include "utility.h"
#include <cstring>
#include <cmath>
#include "circular_array.h"
#include "circular_array.cpp"

//Track the previous token written
string previousToken;

/*Track numbers*/
int documents = 0;
int tokens=0;
int preProcessRow=1;
int ngramSize=7;
extern int rowno;

/*Map for storing ngrams of various lengths*/
unordered_map<string, TokenMeta> wordMap;

/*Array to store tokens*/
Circular_Array<string> tokenArray = Circular_Array<string>(ngramSize);

/*Array for storing token counts
 * position 0=unigram, 1=bigram 2=trigram etc...*/
int * token_counts = new int[ngramSize] ();

/*Filenames*/
extern const string multiWordFileJsonAvp = "./data/multiword-avp-json.txt";
extern const string multiWordFileJsonSquare = "./data/multiword-square-json.txt";
extern const string multiWordFileAvp = "./data/multiword-avp.txt";
extern const string multiWordFileSquare = "./data/multiword-square.txt";
extern const string preProccessFileTemp = "./data/preprocessed_temp.txt";
extern const string preProccessFile = "./data/preprocessed.txt";
extern const string preProccessFileJson = "./data/preprocessed-json.txt";
extern const string probabilitiesFile = "./data/probabilities.txt";

extern const string preProccessFileCsv = "./data/preprocessed-csv.txt";
extern const string stopWordsFile = "stopwords.txt";


/*Iterators*/
vector<string>::iterator stopWords_it;
unordered_map<string, TokenMeta>::iterator wordMap_it;
unordered_map<string, TokenMeta>::iterator wordMapProb_it;

/*Vectors*/
vector<string> stopWords;
vector<proccessed> proccessedwords;


//MARK PRIVATE
void mapToken(string token, string nexttoken, int docnum);
void calculateSquare(string * token, TokenMeta * toProcess);
void calculateAvp(string * token, TokenMeta * toProcess);
void processTag(ofstream * listing, string tag);
bool isStopWord(string word);
void trim(string * toTrim);
void split(string tosplit, vector<string> * result, string delim);
void writeToken(ofstream * listing, string token);
void writeWord(ofstream * listing, TokenStruct tokenStruct );

//PRE PROCESS FILE

/*
 * Processes the token value to the passed in file
 *
 * @param listing File to output to
 * @param token the type of token being output
 * @param tokenStruct The tokenStructure to retrieve the token value from
 */
void processToken( ofstream * listing, TokenType token, TokenStruct & tokenStruct ) {
    if(!writeData){
        return;
    }
    //Don't write consecutive newline characters
    switch (token) {
        case WORD: {
            writeWord(listing, tokenStruct);
            break;
        }
        case APOSTROPHE: {
            writeWord(listing, tokenStruct);
            break;
        }
        case DASH: {
            writeWord(listing, tokenStruct);
            break;
        }
        case OPENTAG:
            processTag(listing, tokenStruct.value.c_str());
        case CLOSETAG:
            //Don't write consecutive new line characters
            if(previousToken.compare("\n") == 0)
                return;
            writeToken(listing, "\n");
        default:
            break;
    }
}

/*
 * This function writes a word out
 *
 * @param listing a pointer to an output stream
 * @param tokenStruct a token structure to write out
 */
void writeWord(ofstream * listing, TokenStruct tokenStruct ){
    string toWrite = tokenStruct.value.c_str();
    transform(toWrite.begin(), toWrite.end(), toWrite.begin(), ::tolower);
    //Don't write stop words
    if (isStopWord(toWrite))
        return;
    //If the previous character was not a new line, add a space before
    if (previousToken.compare("\n") != 0) {
        //Add a space
        string spaceAppend = " ";
        spaceAppend.append(toWrite);
        toWrite = spaceAppend;
    }
    writeToken(listing, toWrite);
}

/*
 * This function loads a list of stopwords from a file and populates a vector with them
 */
void loadStopWords(){
    //Read in all stopwords and add them to the vector
    ifstream infile (stopWordsFile);
    string word;
    while(infile >> word){
        transform(word.begin(),word.end(), word.begin(), ::tolower);
        stopWords.push_back(word);
    }
}

/*
 * This function checks if a word is a stop word
 *
 * @param word is the word to look for in the stopWords vector
 * @return a boolean indicating if the passed in word is a stopped word
 */
bool isStopWord(string word){
    stopWords_it = find(stopWords.begin(), stopWords.end(), word);
    if(stopWords_it == stopWords.end()){
        return false;
    }
    return true;
}

/*
 * This function processes open tags
 */
void processTag(ofstream * listing, string tag){
    //If its the start of a new doc, set the token and the new document record info
    if(tag.compare("REUTERS") == 0){
        writeToken(listing,"$DOC");
    }
    else if(tag.compare("DATE") == 0){
        writeToken(listing,"$DATE");
        writeToken(listing,"\n");
    }
    else if(tag.compare("TOPICS") == 0){
        writeToken(listing,"$TOPICS");
        writeToken(listing,"\n");
    }
    else if(tag.compare("TITLE") == 0){
        writeToken(listing,"$TITLE");
        writeToken(listing,"\n");
    }
    else if(tag.compare("BODY") == 0){
        writeToken(listing,"$BODY");
        writeToken(listing,"\n");
    }
}

/*
 * This function is used to write a token out to the preprocessed file
 *
 * @param listing a pointer to a stream to write tokens to
 * @param token the token to write a value to
 */
void writeToken(ofstream * listing, string token){
    if(token.compare("\n") == 0)
        preProcessRow++;
    *listing << token;
    previousToken = token;

}

// Probabilities

/*
 * This function creates a map of terms
 *
 * @param token the token to add to the map
 * @param the document number related to the term
 */
void mapToken(string token, int tokens){
    trim(&token);
    wordMap_it = wordMap.find(token);
    if(wordMap_it != wordMap.end()){
        wordMap_it->second.count++;
        return;
    }
    TokenMeta newMeta;
    newMeta.count = 1;
    newMeta.probability = 0.0;
    newMeta.tokens = tokens;
    pair<string,TokenMeta> newpair (token,newMeta);
    wordMap.insert(newpair);
}

/*
 * This function retrieves the number of tokens
 * @param tokens - the size of the token to retrieve a count for
 */
int getTokenCount(int tokens){
    return token_counts[tokens-1];
}

/*
 * This function calculates the probability of a token over its appropriate ngram count
 * @param probability - a pointer to float to store the probability calculation in
 * @param token - a pointer to a token to calculate the probability for
 */
void calculateProbability(float * probability, string * token){
    wordMapProb_it = wordMap.find(*token);
    if(wordMapProb_it != wordMap.end()) {
        float prob = (float)wordMapProb_it->second.count/(float)getTokenCount(wordMapProb_it->second.tokens);
        //printf("calc prob for %s: %f\n", token->c_str(), prob);
        *probability *= prob;
    }
}

/*
 * Calculates AVP normalization probability
 * @param token - string representing the full token
 * @param toProcess - a pointer to a TokenMeta struct of the token to process
 */
void calculateAvp(string * token, TokenMeta * toProcess){
    float denominator = 0.0;
    float probability = 1.0;
    string partx = "";

    //If the token is a unigram or bigram
    if(toProcess->tokens <= 2){
        calculateProbability(&probability,token);
        toProcess->avp=pow(toProcess->probability,2)/probability;
        return;
    }
    //printf("-----Token: %s\n",token->c_str());
    //If the token is greater than a bigram
    vector<string> tokens;
    split(*token, &tokens, " ");
    for(int i = 0; i < tokens.size()-1; i++){
        probability = 1.0;
        partx += (tokens[i]);
        string party = "";
        for(int j = i+1; j<tokens.size();j++) {
            party += (tokens[j] + " ");
        }
        trim(&partx);
        trim(&party);
        //printf("token1:%s:token2:%s:\n",partx.c_str(),party.c_str());
        calculateProbability(&probability,&partx);
        calculateProbability(&probability,&party);
        //printf("prob:%f \n", probability);
        denominator+=probability;

        partx+=" ";
    }
    //printf("denom:%d \n", toProcess->tokens);
    toProcess->avp = pow(toProcess->probability,2)/(denominator * (1.0/(float(toProcess->tokens-1))));
}

/*
 * Calculates Squared normalization probability
 * @param token - string representing the full token
 * @param toProcess - a pointer to a TokenMeta struct of the token to process
 */
void calculateSquare(string * token, TokenMeta * toProcess){
    toProcess->square = pow(toProcess->probability,2)/((float)pow((float)toProcess->probability,(1.0/((float)toProcess->tokens))));
}

/*
 * This function calculates the various types of probabilities
 */
void calculateProbabilities(){
    for(wordMap_it = wordMap.begin(); wordMap_it != wordMap.end(); wordMap_it++){
        string token = wordMap_it->first;
        TokenMeta meta = wordMap_it->second;
        meta.probability = ((float)meta.count/(float)getTokenCount(meta.tokens));
        calculateAvp(&token,&meta);
        calculateSquare(&token,&meta);
        wordMap_it->second = meta;
    }
}

/*
 * Find all specified sizes of ngrams currently existing in the token array
 */
void findNgrams() {
    string token = "";
    //work backwards getting tokens
    for (int j = tokenArray.elements()-1; j >= 0; j--){
        int ngram = (tokenArray.elements()-1) - j;
        //calculate the correct postion in the counts array
        token_counts[ngram]++;
        token = (tokenArray.element(j) + " " + token);
        mapToken(token, ngram+1);
        //cout << "Adding token: " << token << "\n";
    }
}

/*
 * This function generates a probability map for all tokens in the data set
 * reads its data from files passed in through stdin
 * @param probabilitiesFile - pointer to a file to read probability records from
 */
void generateProbabilityMap(const string * preProcessedFile){
    ifstream preStream (*preProcessedFile);
    string word;
    //Current section of the document
    ReadStatus status = BODY;
    //Load data from docids file
    while(preStream >> word){
        //Set the status appropriately
        if(word.compare("$DOC") == 0)
        {
            status = DOC;
            tokenArray.clear();
            continue;
        }
        else if(word.compare("$DATE") == 0){
            status = DATE;
            continue;
        }
        else if(word.compare("$TOPICS") == 0){
            status = TOPICS;
            continue;
        }
        else if(word.compare("$BODY") == 0){
            documents++;
            status = BODY;
            continue;
        }
        switch(status){
            case BODY:
                tokenArray.push_back(word);
                findNgrams();
                break;
            default:
                break;
        }
    }
    preStream.close();
}

/*
 * This function outputs probabilities to the probabilities file
 * @param probabilitiesFile - pointer to a file to write probability records to
 */
void outputProbabilities(const string * probabilitiesFile){
    ofstream multiWordStream (*probabilitiesFile);
    //Output token counts
    multiWordStream << "Document Total: " << documents << "\n";
    for(int i =0; i<ngramSize; i++){
        multiWordStream << token_counts[i] << ",";
    }
    //Output token probabilities
    multiWordStream << "\ntoken,count,tokens,avp,square\n";
    for(wordMap_it = wordMap.begin(); wordMap_it != wordMap.end(); wordMap_it++){
        multiWordStream << wordMap_it->first << "," << wordMap_it->second.count << "," << wordMap_it->second.tokens << "," << wordMap_it->second.avp << "," << wordMap_it->second.square <<"\n";
    }
    multiWordStream.close();
}

//MultiWord Extraction

/*
 * This function extracts all multiword terms from a document
 * @param toprocess - pointer to a struct containing document information
 * @param avp - boolean indicating if avp normalization should be used
 * @param square - boolean indicating if square normalization should be used
 */
void extractMultiWords(proccessed * toprocess, bool avp, bool square){
    //cout << "Extracting terms..." << toprocess->words.size() << "\n";
    for(int i = 0; i < toprocess->words.size(); i++){
        vector<TokenScore> tokenScore (ngramSize);
        string token = toprocess->words[i];
        //cout << "Current Token: " << token << "\n";
        //Get all tokens up to the max ngram size
        for(int j = 1; j < ngramSize && (i+j) < toprocess->words.size(); j++){
            token += " " + toprocess->words[i+j];
            //Store in the vector for comparison
            TokenScore toAdd;
            toAdd.token = token;
            wordMapProb_it = wordMap.find(token);
            if(wordMapProb_it == wordMap.end()) {
                toAdd.score = 0.0;
                //cout << "NOT FOUND:" << token << "\n";
            }
            else{
                //cout << "FOUND:" << token << "\n";
                if(avp){toAdd.score = wordMapProb_it->second.avp;}
                else if(square){toAdd.score = wordMapProb_it->second.square;}
            }
            tokenScore[j-1] = toAdd;
            //cout << "Adding: " << toAdd.token << " Score: " << toAdd.score << "\n";
        }
        //Add all the peaks to the multiword array
        for(int k = 0; k<tokenScore.size(); k++){
            if(k == ngramSize-1 || k == tokenScore.size()-1){
                if(tokenScore[k-1].score < tokenScore[k].score){
                    //cout << "Integrating: Token: " << tokenScore[k-1].token << " Score: " << tokenScore[k].score << "\n";
                    toprocess->multiwords.push_back(tokenScore[k]);
                }
            }
            if(k == 0){
                if(tokenScore[k].score > tokenScore[k+1].score){
                    //cout << "Integrating: Token: " << tokenScore[k].token << " Score: " << tokenScore[k].score << "\n";
                    toprocess->multiwords.push_back(tokenScore[k]);
                }
            }
            else{
                if(tokenScore[k-1].score < tokenScore[k].score && tokenScore[k+1].score < tokenScore[k].score){
                    //cout << "Integrating: Token: " << tokenScore[k-1].token << " Score: " << tokenScore[k].score << "\n";
                    toprocess->multiwords.push_back(tokenScore[k]);
                }
            }
        }
    }
}

/*
 * This function loads all probabilities from the probability file
 * @param probabilitiesFile pointer to a file to load probabilities from
 */
void loadProbabilityMap(const string * probabilitiesFile){
    ifstream probstream (*probabilitiesFile);
    string line;
    vector<string> lineValues;
    //iterate the first three lines
    getline(probstream,line);
    getline(probstream,line);
    getline(probstream,line);
    while(probstream.good()){
        getline(probstream,line);
        split(line,&lineValues,",");
        if(lineValues.size() < 5){
            continue;
        }
        TokenMeta newMeta;
        //cout << "count:" << lineValues[1] << "token:" << lineValues[2] << "\n";
        newMeta.count = stoi(lineValues[1]);
        newMeta.tokens = stoi(lineValues[2]);
        newMeta.avp = stof(lineValues[3]);
        newMeta.square = stof(lineValues[4]);
        pair<string,TokenMeta> newpair (lineValues[0],newMeta);
        //cout << "Inserting: " << lineValues[0] << "\n";
        wordMap.insert(newpair);
    }
    probstream.close();
}

/*
 * This function iterates a preprocessed file extracting all multiword terms using the specified normalization method
 * @param avp indicates the use of avp normalization method
 * @param square indicates the use of square root normalization method
 * @param preProcessFile pointer to a file that contains data in preprocessed format
 * @param extract boolean indicating extraction of multiword terms
 */
void loadPreProcessed(bool avp, bool square, const string * preProcessFile, bool extract){
    ifstream preStream (*preProcessFile);
    proccessed toProcess;
    string word;
    proccessedwords.clear();
    documents = 0;
    //Current section of the document
    ReadStatus status = BODY;
    //Load data from docids file
    while(preStream >> word){
        //Set the status appropriately
        if(word.compare("$DOC") == 0)
        {
            if(toProcess.words.size() > 0){
                if(extract) {
                    extractMultiWords(&toProcess, avp, square);
                }
                proccessedwords.push_back(toProcess);
            }
            status = DOC;
            toProcess.multiwords.clear();
            toProcess.topics.clear();
            toProcess.words.clear();
            toProcess.date = "";
            continue;
        }
        else if(word.compare("$DATE") == 0){
            status = DATE;
            continue;
        }
        else if(word.compare("$TOPICS") == 0){
            status = TOPICS;
            continue;
        }
        else if(word.compare("$BODY") == 0){
            documents++;
            status = BODY;
            continue;
        }
        switch(status){
            case DATE:
                toProcess.date += word;
                break;
            case TOPICS:
                toProcess.topics.push_back(word);
                break;
            case BODY:
                toProcess.words.push_back(word);
                break;
            default:
                break;
        }
    }
    preStream.close();
    if(extract) {
        //Ensure the last record is processed
        extractMultiWords(&toProcess, true, false);
    }
    proccessedwords.push_back(toProcess);

}

//UTILITY

/*
 * This function outputs multiword data in vertical format
 * @param outputFile - file to output data to
 */
void outputMultiwords(const string * outputFile){
    ofstream outmulti (*outputFile);
    int i = 0;
    for(vector<proccessed>::iterator it = proccessedwords.begin(); it != proccessedwords.end(); it++) {
        //Check to ensure the output document is complete
        if (it->multiwords.size() == 0 || it->topics.size() == 0) {
            continue;
        }
        outmulti << "$DOC\n" << "$DATE\n" << it->date << "\n$Topics\n";
        //Output every topic
        for (vector<string>::iterator itt = it->topics.begin(); itt != it->topics.end(); itt++) {
            outmulti << *itt << "\n";
        }
        outmulti << "$BODY\n";
        //Output every multiword
        for (vector<TokenScore>::iterator itb = it->multiwords.begin(); itb != it->multiwords.end(); itb++) {
            outmulti << itb->token << "\n";
        }
        i++;
    }
    cout << "Total Documents Output: " << (i) << "\n";
}

/*
 * This function outputs single term data in json format
 * @param outputFile - file to output data to
 */
void outputSinglewordsJson(const string * outputFile){
    ofstream outmulti (*outputFile);
    for(vector<proccessed>::iterator it = proccessedwords.begin(); it != proccessedwords.end(); it++) {
        //Check to ensure the output document is complete
        if (it->topics.size() == 0 || it->words.size() == 0) {
            continue;
        }
        outmulti << "{ \"date\":\"";
        outmulti << it->date << "\",\"topics\":[";
        //Output every topic
        int arraysize = it->topics.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << "\"" << it->topics[i] << "\"";
            if(i != arraysize-1){
                outmulti << ",";
            }
        }
        outmulti << "],\"terms\":[";
        //Output every single word
        arraysize = it->words.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << "\"" << it->words[i] << "\"";
            if(i != arraysize-1){
                outmulti << ",";
            }
        }
        outmulti << "]}\n";
    }
}

/*
 * This function outputs single term data in vertical format
 * @param outputFile - file to output data to
 */
void outputSinglewordsVertical(const string * outputFile){
    ofstream outmulti (*outputFile);
    for(vector<proccessed>::iterator it = proccessedwords.begin(); it != proccessedwords.end(); it++) {
        //Check to ensure the output document is complete
        if (it->topics.size() == 0 || it->words.size() == 0 || it->words.size() < ngramSize) {
            continue;
        }
        outmulti << "$DOC\n$DATE\n" << it->date << "\n$TOPICS\n";
        //Output every topic
        int arraysize = it->topics.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << it->topics[i];
            if(i != arraysize-1){
                outmulti << " ";
            }
        }
        outmulti << "\n$BODY\n";
        //Output every single word
        arraysize = it->words.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << it->words[i];
            if(i != arraysize-1){
                outmulti << " ";
            }
        }
        outmulti << "\n";
    }
}

/*
 * This function outputs data in csv format
 * @param outputFile - file to output data to
 */
void outputMultiwordsCsv(const string * outputFile){
    ofstream outmulti (*outputFile);
    for(vector<proccessed>::iterator it = proccessedwords.begin(); it != proccessedwords.end(); it++) {
        //Check to ensure the output document is complete
        if (it->topics.size() == 0 || it->multiwords.size() == 0) {
            continue;
        }
        outmulti << it->date << ",";
        //Output every topic
        for (vector<string>::iterator itt = it->topics.begin(); itt != it->topics.end(); itt++) {
            outmulti << *itt << ";";
        }
        outmulti << ",";
        //Output every multiword
        for (vector<TokenScore>::iterator itb = it->multiwords.begin(); itb != it->multiwords.end(); itb++) {
            outmulti << itb->token << ";";
        }
        outmulti << "\n";
    }
}

/*
 * This function outputs data in json format
 * @param outputFile - file to output data to
 */
void outputMultiwordsJson(const string * outputFile){
    ofstream outmulti (*outputFile);
    for(vector<proccessed>::iterator it = proccessedwords.begin(); it != proccessedwords.end(); it++) {
        //Check to ensure the output document is complete
        if (it->topics.size() == 0 || it->multiwords.size() == 0) {
            continue;
        }
        outmulti << "{ \"date\":\"";
        outmulti << it->date << "\",\"topics\":[";
        //Output every topic
        int arraysize = it->topics.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << "\"" << it->topics[i] << "\"";
            if(i != arraysize-1){
                outmulti << ",";
            }
        }
        outmulti << "],\"terms\":[";
        //Output every multiword
        arraysize = it->multiwords.size();
        for (int i = 0; i < arraysize; i++) {
            outmulti << "{\"" << it->multiwords[i].token << "\":\"" << it->multiwords[i].score << "\"}";
            if(i != arraysize-1){
                outmulti << ",";
            }
        }
        outmulti << "]}\n";
    }
}


/*
 * Trims trailing white space from a string
 * @param toTrim - a pointer to a string to trim
 */
void trim(string * toTrim) {
    size_t last = toTrim->find_last_not_of(' ');
    *toTrim = toTrim->substr(0, (last+1));
}

/*
 * This function is used to split a string into tokens
 *
 * @param tosplit a string to split into tokens
 * @param result a pointer to a vector to store tokens in
 */
void split(string tosplit, vector<string> * result, string delim){
    int start = 0;
    int end = tosplit.find(delim);
    result->clear();
    //Capture single token
    if (end == string::npos) {
        result->push_back(tosplit.substr(start, tosplit.length()));
    }
    //Capture all tokens
    while (end != string::npos) {
        result->push_back(tosplit.substr(start, end-start));
        start = (++end);
        end = tosplit.find(delim, end);
        //Capture last token
        if (end == string::npos)
            result->push_back(tosplit.substr(start, tosplit.length()));
    }
}
