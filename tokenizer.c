# include "lexer_statics.h"

token_info get_nexttoken(twin_buf *tb) {
    token_info token;
    token.has_val = 0;
    token.line_no = tb->line_no;
    memset(token.lexeme, 0, MAX_LEXEME_LENGTH);
    
    dfa_state state = S0;
    char c;

    while (1) {
        // At the start of a new token, the lexeme begins where the pointer currently is
        if (state == S0) tb->lexeme_begin = tb->forward;
        c = get_nextchar(tb);

        if (c == EOF && state == S0) {
            token.tokenType = TK_EOF;
            strcpy(token.lexeme, "EOF");
            return token;
        }

        switch (state) {
            
            case S0:
                if (isWS(c)) state = S1;
                else if (c == '%') state = S2;
                else if (isDIG(c)) state = S50;
                else if (c == '_') state = S30;
                else if (c == '#') state = S40;
                else if (isBD(c)) state = S20;
                else if (isLOW(c)) state = S11; // Lowercase but not b, c, or d
                else if (c == '<') state = S60;
                else if (c == '>') state = S63;
                else if (c == '=') state = S64;
                else if (c == '!') state = S65;
                else if (c == '&') state = S66;
                else if (c == '@') state = S68;
                
                // Single-character symbols (returned immediately)
                else if (c == '+') { token.tokenType = TK_PLUS; strcpy(token.lexeme, "+"); return token; }
                else if (c == '-') { token.tokenType = TK_MINUS; strcpy(token.lexeme, "-"); return token; }
                else if (c == '*') { token.tokenType = TK_MUL; strcpy(token.lexeme, "*"); return token; }
                else if (c == '/') { token.tokenType = TK_DIV; strcpy(token.lexeme, "/"); return token; }
                else if (c == '(') { token.tokenType = TK_OP; strcpy(token.lexeme, "("); return token; }
                else if (c == ')') { token.tokenType = TK_CL; strcpy(token.lexeme, ")"); return token; }
                else if (c == '[') { token.tokenType = TK_SQL; strcpy(token.lexeme, "["); return token; }
                else if (c == ']') { token.tokenType = TK_SQR; strcpy(token.lexeme, "]"); return token; }
                else if (c == ';') { token.tokenType = TK_SEM; strcpy(token.lexeme, ";"); return token; }
                else if (c == ':') { token.tokenType = TK_COLON; strcpy(token.lexeme, ":"); return token; }
                else if (c == ',') { token.tokenType = TK_COMMA; strcpy(token.lexeme, ","); return token; }
                else if (c == '.') { token.tokenType = TK_DOT; strcpy(token.lexeme, "."); return token; }
                else if (c == '~') { token.tokenType = TK_NOT; strcpy(token.lexeme, "~"); return token; }
                
                else {
                    token.tokenType = TK_ERROR;
                    sprintf(token.lexeme, "%c", c);
                    return token;
                }
                break;

            case S1:
                if (!isWS(c)) { retract(tb, 1); return get_nexttoken(tb); }
                break;

            case S11:
                if (!isLOW(c)) {
                    retract(tb, 1);
                    get_lexeme(tb, token.lexeme);
                    token.tokenType = lookup_keyword(token.lexeme);
                    return token;
                }
                break;
            
            case S2:
                if (c == '\n' || c == EOF) return get_nexttoken(tb);
                break;

            case S20:
                if (isD27(c)) state = S21;
                else if (isLOW(c)) state = S11;
                else { retract(tb, 1); get_lexeme(tb, token.lexeme); token.tokenType = lookup_keyword(token.lexeme); return token; }
                break;

            case S21:
                if (isBD(c)) continue; 
                else if (isD27(c)) state = S22;
                else {
                    retract(tb, 1); get_lexeme(tb, token.lexeme);
                    token.tokenType = (strlen(token.lexeme) >= 2 && strlen(token.lexeme) <= 20) ? TK_ID : TK_ERROR;
                    return token;
                }
                break;

            case S22:
                if (!isD27(c)) {
                    retract(tb, 1); get_lexeme(tb, token.lexeme);
                    token.tokenType = (strlen(token.lexeme) >= 2 && strlen(token.lexeme) <= 20) ? TK_ID : TK_ERROR;
                    return token;
                }
                break;

            case S30:
                if (isLET(c)) state = S31;
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "_"); return token; }
                break;

            case S31:
                if (isLET(c)) continue;
                else if (isDIG(c)) state = S32;
                else {
                    retract(tb, 1); get_lexeme(tb, token.lexeme);
                    if (strcmp(token.lexeme, "_main") == 0) token.tokenType = TK_MAIN;
                    else token.tokenType = (strlen(token.lexeme) <= 30) ? TK_FUNID : TK_ERROR;
                    return token;
                }
                break;

            case S32:
                if (!isDIG(c)) {
                    retract(tb, 1); get_lexeme(tb, token.lexeme);
                    token.tokenType = (strlen(token.lexeme) <= 30) ? TK_FUNID : TK_ERROR;
                    return token;
                }
                break;

            case S40:
                if (isLOW(c)) state = S41;
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "#"); return token; }
                break;

            case S41:
                if (!isLOW(c)) {
                    retract(tb, 1); get_lexeme(tb, token.lexeme);
                    token.tokenType = TK_RUID;
                    return token;
                }
                break;

            case S50:
                if (isDIG(c)) continue;
                else if (c == '.') state = S51;
                else { 
                    retract(tb, 1); get_lexeme(tb, token.lexeme); 
                    token.tokenType = TK_NUM; token.value.int_val = atoi(token.lexeme); token.has_val = 1;
                    return token; 
                }
                break;

            case S51:
                if (isDIG(c)) state = S52;
                else { retract(tb, 1); token.tokenType = TK_ERROR; return token; }
                break;

            case S52:
                if (isDIG(c)) state = S53;
                else { retract(tb, 1); token.tokenType = TK_ERROR; return token; }
                break;

            case S53:
                if (c == 'E') state = S54;
                else { 
                    retract(tb, 1); get_lexeme(tb, token.lexeme); 
                    token.tokenType = TK_RNUM; token.value.real_val = atof(token.lexeme); token.has_val = 1;
                    return token; 
                }
                break;

            case S54:
                if (c == '+' || c == '-') state = S55;
                else if (isDIG(c)) state = S56;
                else { retract(tb, 1); token.tokenType = TK_ERROR; return token; }
                break;

            case S55:
                if (isDIG(c)) state = S56;
                else { retract(tb, 1); token.tokenType = TK_ERROR; return token; }
                break;

            case S56:
                if (isDIG(c)) state = S57;
                else { retract(tb, 1); token.tokenType = TK_ERROR; return token; }
                break;

            case S57:
                retract(tb, 1); get_lexeme(tb, token.lexeme);
                token.tokenType = TK_RNUM; token.value.real_val = atof(token.lexeme); token.has_val = 1;
                return token;

            case S60:
                if (c == '=') { token.tokenType = TK_LE; strcpy(token.lexeme, "<="); return token; }
                else if (c == '-') state = S61;
                else { retract(tb, 1); token.tokenType = TK_LT; strcpy(token.lexeme, "<"); return token; }
                break;

            case S61:
                if (c == '-') state = S62;
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "<-"); return token; }
                break;

            case S62:
                if (c == '-') { token.tokenType = TK_ASSIGNOP; strcpy(token.lexeme, "<---"); return token; }
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "<--"); return token; }
                break;

            case S63:
                if (c == '=') { token.tokenType = TK_GE; strcpy(token.lexeme, ">="); return token; }
                else { retract(tb, 1); token.tokenType = TK_GT; strcpy(token.lexeme, ">"); return token; }
                break;

            case S64:
                if (c == '=') { token.tokenType = TK_EQ; strcpy(token.lexeme, "=="); return token; }
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "="); return token; }
                break;

            case S65:
                if (c == '=') { token.tokenType = TK_NE; strcpy(token.lexeme, "!="); return token; }
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "!"); return token; }
                break;

            case S66:
                if (c == '&') state = S67;
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "&"); return token; }
                break;

            case S67:
                if (c == '&') { token.tokenType = TK_AND; strcpy(token.lexeme, "&&&"); return token; }
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "&&"); return token; }
                break;

            case S68:
                if (c == '@') state = S69;
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "@"); return token; }
                break;

            case S69:
                if (c == '@') { token.tokenType = TK_OR; strcpy(token.lexeme, "@@@"); return token; }
                else { retract(tb, 1); token.tokenType = TK_ERROR; strcpy(token.lexeme, "@@"); return token; }
                break;

            default:
                token.tokenType = TK_ERROR;
                return token;
        }
    }
}