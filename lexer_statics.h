#ifndef LEXER_STATICS_H
#define LEXER_STATICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 4096
#define TWIN_BUFFER_SIZE (2 * BUFFER_SIZE)
#define MAX_LEXEME_LENGTH 256

// Constant definitions
typedef enum {
    TK_ASSIGNOP,    // <---
    TK_COMMENT,     // %
    TK_FIELDID,     // [a-z][a-z]*
    TK_ID,          // [b-d][2-7][b-d]*[2-7]*
    TK_NUM,         // [0-9][0-9]*
    TK_RNUM,        // [0-9][0-9]*.[0-9][0-9] with optional E[+|-][0-9][0-9]
    TK_FUNID,       // _[a-z|A-Z][a-z|A-Z]*[0-9]*
    TK_RUID,        // #[a-z][a-z]*
    TK_WITH,        // with
    TK_PARAMETERS,  // parameters
    TK_END,         // end
    TK_WHILE,       // while
    TK_UNION,       // union
    TK_ENDUNION,    // endunion
    TK_DEFINETYPE,  // definetype
    TK_AS,          // as
    TK_TYPE,        // type
    TK_MAIN,        // _main
    TK_GLOBAL,      // global
    TK_PARAMETER,   // parameter
    TK_LIST,        // list
    TK_SQL,         // [
    TK_SQR,         // ]
    TK_INPUT,       // input
    TK_OUTPUT,      // output
    TK_INT,         // int
    TK_REAL,        // real
    TK_COMMA,       // ,
    TK_SEM,         // ;
    TK_COLON,       // :
    TK_DOT,         // .
    TK_ENDWHILE,    // endwhile
    TK_OP,          // (
    TK_CL,          // )
    TK_IF,          // if
    TK_THEN,        // then
    TK_ENDIF,       // endif
    TK_READ,        // read
    TK_WRITE,       // write
    TK_RETURN,      // return
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_MUL,         // *
    TK_DIV,         // /
    TK_CALL,        // call
    TK_RECORD,      // record
    TK_ENDRECORD,   // endrecord
    TK_ELSE,        // else
    TK_AND,         // &&&
    TK_OR,          // @@@
    TK_NOT,         // ~
    TK_LT,          // <
    TK_LE,          // <=
    TK_EQ,          // ==
    TK_GT,          // >
    TK_GE,          // >=
    TK_NE,          // !=
    TK_EOF,         // End of file
    TK_ERROR,       // Lexical error
    TK_EPSILON      // For grammar purposes
} token_type;

typedef enum {
    // Start state
    S0,

    // Regular transition states
    S1, S11,
    S2, S20, S21, S22,
    S30, S31, S32,
    S40, S41,
    S50, S51, S52, S53, S54, S55, S56, S57,
    S60, S61, S62, S63, S64, S65, S66, S67, S68,S69,

    // Accept states
    A_SKIP_WS,
    A_SKIP_COMMENT,
    A_WORD,
    A_VARID,
    A_FUNID,
    A_RUID,
    A_INT,
    A_ERR_INTDOT,
    A_REAL,
    A_REAL_EXP,
    A_ASSIGNOP,
    A_LT,
    A_LE,
    A_GT,
    A_GE,
    A_EQ,
    A_NE,
    A_AND,
    A_OR,
    A_NOT,
    A_PLUS,
    A_MINUS,
    A_MUL,
    A_DIV,
    A_COMMA,
    A_SEM,
    A_COLON,
    A_DOT,
    A_OP,
    A_CL,
    A_SQL,
    A_SQR,
    A_EOF,

    // Error accept states
    A_ERR_CHAR,
    A_ERR_FUN,
    A_ERR_RUID,
    A_ERR_RNUM_1DIG,
    A_ERR_EXP_NOSIGN,
    A_ERR_EXP_NODIG1,
    A_ERR_EXP_1DIG,
    A_ERR_EQ,
    A_ERR_BANG,
    A_ERR_AMP,
    A_ERR_AND2,
    A_ERR_AT,
    A_ERR_AT2,
    A_ERR_LTMINUS,
    A_ERR_LTDOUBLE
} dfa_state;

// Structure definitions
typedef struct {
    token_type tokenType;
    char lexeme[MAX_LEXEME_LENGTH];
    int line_no;
    union {
        int int_val;
        double real_val;
    } value;
    int has_val;
} token_info;

typedef struct {
    FILE *fp;
    char buffer[TWIN_BUFFER_SIZE + 1];
    int currentBuffer;
    int forward;
    int line_no;
    int lexeme_begin;
    int eof1, eof2;
} twin_buf;

typedef struct {
    char *keyword;
    token_type token;
} keyword;

typedef struct {
    int line_no;
    char message[256];
} lex_error;

// function definitions
FILE* get_stream(FILE *fp);

int isDIG(char c);
int isD27(char c);
int isLOW(char c);
int isBD(char c);
int isLET(char c);
int isWS(char c);

twin_buf* initialize_twin_buf(FILE *fp);
char get_nextchar(twin_buf *tb);
void retract(twin_buf *tb, int n);
void get_lexeme(twin_buf *tb, char *lexeme);
token_info get_nexttoken(twin_buf *tb);
void remove_comments(char *testcasefile, char *cleanfile);
token_type lookup_keyword(const char *lexeme);
const char* get_token_name(token_type token);
void print_token(token_info token);
void free_twin_buf(twin_buf *tb);

#endif