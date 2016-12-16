/*
 * @file    expression.l
 * @author  AJ Olpin <aolpin@uoguelph.ca>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The expression file is used to generate the lex class
 *
 */

%{
#include "helper.h"
#include "tokenizer.h"

TokenStruct yylval;
vector<TagStruct> *tagCollection;
TagStruct unknownTag;
stack<TagStruct> tagStack;
extern TokenStruct yylval;
extern bool writeData;

%}

/*Tags*/
opentag         <[^>\/]*>
closetag        <\/[^>]*>

/*Text*/
apostrophe      (([a-zA-Z]+('[a-zA-Z]{1,2}))|([a-zA-Z]{1,2}('[a-zA-Z]*)+))
dash            [a-zA-Z0-9]+(-[a-zA-Z0-9]+)+('[a-zA-Z])*
word            [A-Za-z]+

/*Numbers*/
number          -?[0-9]*\.?[0-9]

/*Punctuation*/
period          \.
question        \?
bang            \!
otherpunct      [^a-zA-z0-9\ \.\?\!\n]

/*Sepcial*/
newline         \n
whitespace      [ \t]+

%%

{closetag}          { yylval.row = rowno; yylval.value = normalizeTag(yytext); return popStack(yylval.value);}
{opentag}           { yylval.row = rowno; yylval.value = normalizeTag(yytext); return pushStack(yylval.value);}
{apostrophe}        { yylval.row = rowno; yylval.value = yytext; return APOSTROPHE;}
{dash}              { yylval.row = rowno; yylval.value = yytext; return DASH;}
{word}              { yylval.row = rowno; yylval.value = yytext; return WORD;}
{number}            { yylval.row = rowno; yylval.value = yytext; return NUMBER;}
{question}          { yylval.row = rowno; return QUESTION;}
{period}            { yylval.row = rowno; return PERIOD;}
{bang}              { yylval.row = rowno; return BANG;}
{otherpunct}        { yylval.row = rowno; yylval.value = yytext; return OTHERPUNCT;}
{newline}           { rowno++;}

<*>{whitespace}     { }
.                   { yylval.row = rowno; yylval.value = yytext; return ERROR; }

<<EOF>>             { return ENDFILE; }

%%

/*
 * Returns the token type
 *
 * @return The TokenType of the current token
 */
TokenType getToken(void) {
  return (TokenType)yylex();
}

/*
 * Initializes the Tag collection vector
 */
void initTagCollection(void){
    tagCollection = new vector<TagStruct>;
        TagStruct insertTag;
        //Insert Reuters tag
        insertTag.tagName = "REUTERS";
        insertTag.outputStatus = WRITE;
        tagCollection->push_back(insertTag);
        //Insert DATE tag
        insertTag.tagName = "DATE";
        insertTag.outputStatus = WRITE;
        tagCollection->push_back(insertTag);
        //Insert Topics tag
        insertTag.tagName = "TOPICS";
        insertTag.outputStatus = WRITE;
        tagCollection->push_back(insertTag);
        //Insert Topics tag
        insertTag.tagName = "BODY";
        insertTag.outputStatus = WRITE;
        tagCollection->push_back(insertTag);
        //Insert D tag
        insertTag.tagName = "D";
        insertTag.outputStatus = IGNORE;
        tagCollection->push_back(insertTag);
        //Initialize unknown struct
        unknownTag.tagName = "UNKNOWN";
        unknownTag.outputStatus = NOWRITE;
}

/*
 * Get the tag structure related to the passed in tag
 *
 * @param tagId The name of the tag to find a tag structure for
 * @return  A tag structure related to the passed in tag or an unknown
 *          structure if no match is found
 */
TagStruct getTagStruct(string tagId){
    //Find the matching TagStruct
    for(TagStruct currentStruct: *tagCollection){
        //printf("comparing: %s <-> %s\n", currentStruct.tagName.c_str(), tagId.c_str());
        if(currentStruct.tagName.compare(tagId) == 0){
            return currentStruct;
        }
    }
    //printf("returning %s\n", unknownTag.tagName.c_str());
    return unknownTag;
}


/*
 * Puts a token struct on the stack and sets the data
 * write flag appropriately based on the OutputStatus enum type
 *
 * @param tagName Name of the tag to retrieve the tagStruct for
 * @return TokenType a tokenType representing tag type
 */
TokenType pushStack(string tagName){
    TagStruct foundStruct = getTagStruct(tagName);
    //printf("pushing\n");
    switch(foundStruct.outputStatus){
        case(WRITE):
           writeData = true;
            tagStack.push(foundStruct);
            break;
        case(NOWRITE):
            writeData = false;
            break;
        case(IGNORE):
            tagStack.push(foundStruct);
            break;//Do nothing on an ignore tag
    }
    return OPENTAG;
}

/*
 * Compares the current tagStruct to the top most element on the stack
 * if a match pops the element, if not, returns an error as the tags
 * are now unbalanced
 *
 * @param tagName Name of the tag to retrieve the tagStruct for
 * @return TokenType a tokenType representing tag type
 */
TokenType popStack(string tagName){
    //printf("popping - %s\n", tagName.c_str());
    TagStruct foundStruct = getTagStruct(tagName);

    if(tagStack.size() == 0){
        return ERROR;
    }
    //printf("%s<->%s\n", foundStruct.tagName.c_str(),tagStack.top().tagName.c_str());
    if(foundStruct.tagName.compare(tagStack.top().tagName) == 0){
        tagStack.pop();
        return CLOSETAG;
    }
    else{
        return ERROR;
    }
}

/*
 * Extracts the name of the tag from a HTML tag
 *
 * @param tag The HTML tag to extract a name from
 * @return string the name of the HTML tag
 */
string normalizeTag(string tag){
    regex expression("[A-Za-z]+(?=\\s|\\>)");
    smatch match;

    if(regex_search(tag, match, expression)){
        string result = match.str();
        transform(result.begin(),result.end(), result.begin(), ::toupper);
        return result;
    }

    return tag;
}


