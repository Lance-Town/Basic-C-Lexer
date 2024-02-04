/*********************************************
 * Lance Townsend
 *
 * Compile: gcc -Wall
 * Run: ./a.out input.txt
 *
 * Lexer for a C-like language
 *********************************************/

#define MAXTOKEN 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Assign meaning to tokens
void lex(char *line, char *type, int start, int end, int length);

// Break the string down into tokens
void tokenize(char *line, int length, bool* inComment);

// find the end of the string
void findEndString(char* line, int start, int* end, int length);

// check if we have found an operator or not
int isOperator(char* line, int end, int length);

// check if the string is a keyword or not
bool isKeyword(char* line, int start, int end, int length);

// lexes the characters that come before the different types.
// It will decide whether to lex a keyword or a token
void lexPrevious(char* line, int start, int end, int length);

// returns true if the string is hexadecimal numeric
bool checkHexNumeric(char* line, int start, int end, int length);

// returns true if the string is a decimal numeric
bool checkDecNumeric(char* line, int start, int end, int length);

// returns true if the string is an identifier
bool checkIdentifier(char* line, int start, int end, int length);

int main(int argc, char *argv[])
{
   if(argc<2) {
      printf("Please specify input file.\n");
      printf("%s /y/shared/Engineering/cs-drbc/assignments/cs210/w01_in1.txt\n", argv[0]);
      return 1;
   }
  
   FILE * fp;
   char * line = NULL;
   size_t len = 0;
   ssize_t read;
   bool inComment = false;

   fp = fopen(argv[1], "r");
   if (fp == NULL) {
      printf("Error: Could not open file %s\n", argv[1]);
      exit(EXIT_FAILURE);
  }

   while ((read = getline(&line, &len, fp)) != -1) {
      tokenize(line, read, &inComment);
   }

   fclose(fp);
   if (line) {
      free(line);
   } exit(EXIT_SUCCESS);
}

// find the end of a string
void findEndString(char* line, int start, int* end, int length) {
   while (*end < length && line[*end] != '"') {
      *end += 1;
   }
}

// check if we have found an operator or not
int isOperator(char* line, int end, int length) {
   int operatorsOneSize = 20;
   int operatorsTwoSize = 12;
   static char *operatorsOne[] = {"<", ">", "(", ")", "+", "-", "*", "/", "|", "&", ";", ",", ":", "=", "[", "]", "{", "}", "$", "@"};
   static char* operatorsTwo[] = {":=", "..", "<<", ">>", "<>", "<=", ">=", "**", "!=", "=>", "{:", "}:"};

   for (int i = 0; i < operatorsTwoSize; i++) {
      if ((end+1 < length) && (strncmp(&line[end], operatorsTwo[i], 2) == 0)) {
         return 2;
      }
   }

   for (int i = 0; i < operatorsOneSize; i++) {
      if (strncmp(&line[end], operatorsOne[i], 1) == 0) {
         return 1;
      }
   }

   return 0;
}

// check for keywords
bool isKeyword(char* line, int start, int end, int length) {
   int keywordSize = 35;
   static char* keywords[] = {"accessor", "and", "array", "bool", "case", "character", "constant", "else", "elsif", "end", "exit", "float",
   "func", "if", "ifc", "in", "integer", "is", "mutator", "natural", "null", "of", "or", "others", "out", "pkg", "positive",
   "proc", "ptr", "range", "subtype", "then", "type", "when", "while"};

   while (start < length && isspace(line[start])) {
      start++;
   }

   if (start >= length || end >= length || start > end) {
      return false;
   }

   char str[end-start+1];
   strncpy(str, &line[start], end - start);
   str[end-start] = '\0';

   for (int i = 0; i < keywordSize; i++) {
      if (strcmp(str, keywords[i]) == 0) {
         return true;
      }
   }

   return false;
}


// lexes the characters that come before the different types.
// It will decide whether to lex a keyword or a token
void lexPrevious(char* line, int start, int end, int length) {
   if (isKeyword(line, start, end, length)) {
      lex(line, "Keyword", start, end, length);
   }
   else if (checkHexNumeric(line, start, end, length)) {
      lex(line, "Numeric", start, end, length);
   }
   else if (checkDecNumeric(line, start, end, length)) {
      lex(line, "Numeric", start, end, length);
   }
   else if (checkIdentifier(line, start, end, length)) {
      lex(line, "Identifier", start, end, length);
   }
   else {
      lex(line, "Token", start, end, length);
   }
}

// break the string down into tokens
void tokenize(char *line, int length, bool *inComment) {
   int start = 0;
   int end = 0;

   for (end = 0; end < length; end++) {
      if (line[end] == '"') {
         lexPrevious(line, start, end, length);
         start = end;
         end++;
         findEndString(line, start, &end, length);
         lex(line, "String", start, end+1, length);
         start = end+1;
      }
      else if (line[end] == '\'') {
         /* in a comment  */
         lexPrevious(line, start, end, length);
         start = end;
         end++;
         while (end < length && line[end] != '\'') {
            if (line[end] == '\\') {
               end++;
            }
            end++;
         }
         lex(line, "Char", start, end+1, length);
         start = end+1;
      }
      else if ( *inComment || ( end < length && ( (line[end] == '/' && line[end+1] == '*') ) ) ) {
         lexPrevious(line, start, end, length);
         start = end;

         if (*inComment && line[end] == '*' && line[end+1] == '/') {
            // do nothing
         }
         else {
            end++;

            while (end < length && (line[end] != '*' || line[end+1] != '/')) {
               end++;
            }
         }

         if (end == length) {
            *inComment = true;
            lex(line, "Comment", start, end-1, length);
            start = end;
         }
         else {
            *inComment = false;
            lex(line, "Comment", start, end+2, length);
            start = end+2;
            end++;
         }
      }
      else if (isOperator(line, end, length)) {
         lexPrevious(line, start, end, length);

         start = end;

         end += isOperator(line, end, length);

         lex(line, "Operator", start, end, length);
         start = end;
         end--;
      }
      else if (isspace(line[end])) {
         lexPrevious(line, start, end, length);
         start = end;
      }
   }
}

// assign meanings to the tokens
void lex(char *line, char *type, int start, int end, int length) {
   char token[MAXTOKEN];

   // get rid of leading white space
   while (start < length && strcmp(type, "Comment") != 0 && isspace(line[start])) {
      start++;
   }
   if (start >= end) {
      return;
   }

   // printf("Start: %i\tEnd: %i\t", start, end);

   strncpy(token, &line[start], end - start);

   token[end-start] = '\0';

   printf("%s: %s\n",type, token);

   return;
}

// returns true if the string is numeric
bool checkHexNumeric(char* line, int start, int end, int length) {
   bool isHex = true;

   while (start < end && isspace(line[start])) {
      start++;
   }

   while (start < end && isxdigit(line[start]) != 0 && isdigit(line[start]) == 0) {
      //printf("Char: %c Start: %d End: %d\n", line[start], start, end);
      start++;
   }

   //printf("\nStart: %d\tEnd: %d\n", start, end);

   if (start != end) {
      isHex = false;
   }

   return isHex;
}

// returns true if the string is a digit numeric
bool checkDecNumeric(char* line, int start, int end, int length) {
   int hasPoint = false;
   int isDigit = true;

   while (start < length && isspace(line[start])) {
      start++;
   }

   if (abs(start - end) == 1) {
      return (isdigit(line[start]) != 0);
   }

   while (start < end && (isdigit(line[start]) != 0 || line[start] == '.' )) {
      if (line[start] == '.') {
         if (hasPoint) {
            isDigit = false;
            return isDigit;
         } else {
            hasPoint = true;
         }
      }

      start++;
   }

   if (start != end) {
      isDigit = false;
   }

   return isDigit;
}

// return true if the string is an identifier
bool checkIdentifier(char* line, int start, int end, int length) {
   bool isIdentifier = true;

   while (start < length && isspace(line[start])) {
      start++;
   }

   // must start with a letter
   if (isalpha(line[start]) == 0) {
      isIdentifier = false;
      return isIdentifier;
   }

   while (start < end && (isalpha(line[start]) != 0 || isdigit(line[start]) != 0 || line[start] == '_')) {
      start++;
   }

   if (start != end) {
      isIdentifier = false;
   }

   return isIdentifier;

}
