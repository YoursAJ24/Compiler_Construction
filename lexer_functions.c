#include "lexer_statics.h"

static keyword keyword_table[] = {
    {"with", TK_WITH}, {"parameters", TK_PARAMETERS}, {"end", TK_END},
    {"while", TK_WHILE}, {"union", TK_UNION}, {"endunion", TK_ENDUNION},
    {"definetype", TK_DEFINETYPE}, {"as", TK_AS}, {"type", TK_TYPE},
    {"global", TK_GLOBAL}, {"parameter", TK_PARAMETER}, {"list", TK_LIST},
    {"input", TK_INPUT}, {"output", TK_OUTPUT}, {"int", TK_INT},
    {"real", TK_REAL}, {"endwhile", TK_ENDWHILE}, {"if", TK_IF},
    {"then", TK_THEN}, {"endif", TK_ENDIF}, {"read", TK_READ},
    {"write", TK_WRITE}, {"return", TK_RETURN}, {"call", TK_CALL},
    {"record", TK_RECORD}, {"endrecord", TK_ENDRECORD}, {"else", TK_ELSE},
    {NULL, TK_ERROR}
};

FILE* get_stream(FILE *fp) {
    return fp;
}

int isDIG(char c) { return c >= '0' && c <= '9'; }
int isD27(char c) { return c >= '2' && c <= '7'; }
int isLOW(char c) { return c >= 'a' && c <= 'z'; }
int isBD(char c)  { return c == 'b' || c == 'c' || c == 'd'; }
int isLET(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
int isWS(char c)  { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

twin_buf* initialize_twin_buf(FILE *fp) {
    twin_buf *tb = (twin_buf*)malloc(sizeof(twin_buf));
    tb->fp = fp;
    tb->forward = 0;
    tb->lexeme_begin = 0;
    tb->line_no = 1;
    tb->eof1 = tb->eof2 = 0;

    memset(tb->buffer, 0, TWIN_BUFFER_SIZE + 1);
    
    size_t n = fread(tb->buffer, 1, BUFFER_SIZE, fp);
    if (n < BUFFER_SIZE) { tb->buffer[n] = EOF; tb->eof1 = 1; }

    return tb;
}

char get_nextchar(twin_buf *tb) {
    char c = tb->buffer[tb->forward];

    if (c == EOF && ((tb->forward < BUFFER_SIZE && tb->eof1) || (tb->forward >= BUFFER_SIZE && tb->eof2)))
        return EOF;

    tb->forward++;

    if (tb->forward == BUFFER_SIZE && !tb->eof1) {
        size_t n = fread(tb->buffer + BUFFER_SIZE, 1, BUFFER_SIZE, tb->fp);
        if (n < BUFFER_SIZE) { tb->buffer[BUFFER_SIZE + n] = EOF; tb->eof2 = 1; }
    } 
    else if (tb->forward == TWIN_BUFFER_SIZE) {
        tb->forward = 0;
        if (!tb->eof2) {
            size_t n = fread(tb->buffer, 1, BUFFER_SIZE, tb->fp);
            if (n < BUFFER_SIZE) { tb->buffer[n] = EOF; tb->eof1 = 1; }
        }
    }

    if (c == '\n') tb->line_no++;
    return c;
}

void retract(twin_buf *tb, int n) {
    for (int i = 0; i < n; i++) {
        if (tb->forward > 0) tb->forward--;
        else tb->forward = TWIN_BUFFER_SIZE - 1;
        
        if (tb->buffer[tb->forward] == '\n') tb->line_no--;
    }
}

void get_lexeme(twin_buf *tb, char *lexeme) {
    int i = 0;
    int pos = tb->lexeme_begin;
    
    while (pos != tb->forward && i < MAX_LEXEME_LENGTH - 1) {
        lexeme[i++] = tb->buffer[pos++];
        if (pos == TWIN_BUFFER_SIZE) {
            pos = 0;
        }
    }
    lexeme[i] = '\0';
}

void remove_comments(char *input_file, char *output_file) {
    FILE *in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "w");
    
    if (!in || !out) {
        printf("Error: Could not open files for comment removal.\n");
        if (in) fclose(in);
        return;
    }
    
    int c;
    int inside_comment = 0;
    while ((c = fgetc(in)) != EOF) {
        if (c == '%') {
            inside_comment = 1;
        }
        if (!inside_comment) {
            fputc(c, out);
        }
        if (inside_comment && c == '\n') {
            inside_comment = 0;
            fputc(c, out);
        }
    }
    fclose(in);
    fclose(out);
}

token_type lookup_keyword(const char *lexeme) {
    for (int i = 0; keyword_table[i].keyword != NULL; i++) {
        if (strcmp(lexeme, keyword_table[i].keyword) == 0) {
            return keyword_table[i].token;
        }
    }
    return TK_FIELDID;
}

const char* get_token_name(token_type type) {
    switch (type) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case TK_EOF: return "TK_EOF";
        case TK_ERROR: return "TK_ERROR";
        case TK_EPSILON: return "TK_EPSILON";
        default: return "UNKNOWN";
    }
}

void print_token(token_info token) {
    printf("%-20s %-30s %d", get_token_name(token.tokenType), token.lexeme, token.line_no);
    if (token.has_val) {
        if (token.tokenType == TK_NUM) {
            printf(" %-15d", token.value.int_val);
        } else if (token.tokenType == TK_RNUM) {
            printf(" %-15.2f", token.value.real_val);
        }
    }
    printf("\n");
}

void free_twin_buf(twin_buf *tb) {
    if (tb) {
        if (tb->fp) {
            fclose(tb->fp);
        }
        free(tb);
    }
}