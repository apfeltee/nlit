
#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        const char* Token::token_name(int t)
        {
            switch(t)
            {
                case LITTOK_NEW_LINE: return "LITTOK_NEW_LINE";
                case LITTOK_LEFT_PAREN: return "LITTOK_LEFT_PAREN";
                case LITTOK_RIGHT_PAREN: return "LITTOK_RIGHT_PAREN";
                case LITTOK_LEFT_BRACE: return "LITTOK_LEFT_BRACE";
                case LITTOK_RIGHT_BRACE: return "LITTOK_RIGHT_BRACE";
                case LITTOK_LEFT_BRACKET: return "LITTOK_LEFT_BRACKET";
                case LITTOK_RIGHT_BRACKET: return "LITTOK_RIGHT_BRACKET";
                case LITTOK_COMMA: return "LITTOK_COMMA";
                case LITTOK_SEMICOLON: return "LITTOK_SEMICOLON";
                case LITTOK_COLON: return "LITTOK_COLON";
                case LITTOK_BAR_EQUAL: return "LITTOK_BAR_EQUAL";
                case LITTOK_BAR: return "LITTOK_BAR";
                case LITTOK_BAR_BAR: return "LITTOK_BAR_BAR";
                case LITTOK_AMPERSAND_EQUAL: return "LITTOK_AMPERSAND_EQUAL";
                case LITTOK_AMPERSAND: return "LITTOK_AMPERSAND";
                case LITTOK_AMPERSAND_AMPERSAND: return "LITTOK_AMPERSAND_AMPERSAND";
                case LITTOK_BANG: return "LITTOK_BANG";
                case LITTOK_BANG_EQUAL: return "LITTOK_BANG_EQUAL";
                case LITTOK_EQUAL: return "LITTOK_EQUAL";
                case LITTOK_EQUAL_EQUAL: return "LITTOK_EQUAL_EQUAL";
                case LITTOK_GREATER: return "LITTOK_GREATER";
                case LITTOK_GREATER_EQUAL: return "LITTOK_GREATER_EQUAL";
                case LITTOK_GREATER_GREATER: return "LITTOK_GREATER_GREATER";
                case LITTOK_LESS: return "LITTOK_LESS";
                case LITTOK_LESS_EQUAL: return "LITTOK_LESS_EQUAL";
                case LITTOK_LESS_LESS: return "LITTOK_LESS_LESS";
                case LITTOK_PLUS: return "LITTOK_PLUS";
                case LITTOK_PLUS_EQUAL: return "LITTOK_PLUS_EQUAL";
                case LITTOK_PLUS_PLUS: return "LITTOK_PLUS_PLUS";
                case LITTOK_MINUS: return "LITTOK_MINUS";
                case LITTOK_MINUS_EQUAL: return "LITTOK_MINUS_EQUAL";
                case LITTOK_MINUS_MINUS: return "LITTOK_MINUS_MINUS";
                case LITTOK_STAR: return "LITTOK_STAR";
                case LITTOK_STAR_EQUAL: return "LITTOK_STAR_EQUAL";
                case LITTOK_STAR_STAR: return "LITTOK_STAR_STAR";
                case LITTOK_SLASH: return "LITTOK_SLASH";
                case LITTOK_SLASH_EQUAL: return "LITTOK_SLASH_EQUAL";
                case LITTOK_QUESTION: return "LITTOK_QUESTION";
                case LITTOK_QUESTION_QUESTION: return "LITTOK_QUESTION_QUESTION";
                case LITTOK_PERCENT: return "LITTOK_PERCENT";
                case LITTOK_PERCENT_EQUAL: return "LITTOK_PERCENT_EQUAL";
                case LITTOK_ARROW: return "LITTOK_ARROW";
                case LITTOK_SMALL_ARROW: return "LITTOK_SMALL_ARROW";
                case LITTOK_TILDE: return "LITTOK_TILDE";
                case LITTOK_CARET: return "LITTOK_CARET";
                case LITTOK_CARET_EQUAL: return "LITTOK_CARET_EQUAL";
                case LITTOK_DOT: return "LITTOK_DOT";
                case LITTOK_DOT_DOT: return "LITTOK_DOT_DOT";
                case LITTOK_DOT_DOT_DOT: return "LITTOK_DOT_DOT_DOT";
                case LITTOK_SHARP: return "LITTOK_SHARP";
                case LITTOK_SHARP_EQUAL: return "LITTOK_SHARP_EQUAL";
                case LITTOK_IDENTIFIER: return "LITTOK_IDENTIFIER";
                case LITTOK_STRING: return "LITTOK_STRING";
                case LITTOK_INTERPOLATION: return "LITTOK_INTERPOLATION";
                case LITTOK_NUMBER: return "LITTOK_NUMBER";
                case LITTOK_CLASS: return "LITTOK_CLASS";
                case LITTOK_ELSE: return "LITTOK_ELSE";
                case LITTOK_FALSE: return "LITTOK_FALSE";
                case LITTOK_FOR: return "LITTOK_FOR";
                case LITTOK_FUNCTION: return "LITTOK_FUNCTION";
                case LITTOK_IF: return "LITTOK_IF";
                case LITTOK_NULL: return "LITTOK_NULL";
                case LITTOK_RETURN: return "LITTOK_RETURN";
                case LITTOK_SUPER: return "LITTOK_SUPER";
                case LITTOK_THIS: return "LITTOK_THIS";
                case LITTOK_TRUE: return "LITTOK_TRUE";
                case LITTOK_VAR: return "LITTOK_VAR";
                case LITTOK_WHILE: return "LITTOK_WHILE";
                case LITTOK_CONTINUE: return "LITTOK_CONTINUE";
                case LITTOK_BREAK: return "LITTOK_BREAK";
                case LITTOK_NEW: return "LITTOK_NEW";
                case LITTOK_EXPORT: return "LITTOK_EXPORT";
                case LITTOK_IS: return "LITTOK_IS";
                case LITTOK_STATIC: return "LITTOK_STATIC";
                case LITTOK_OPERATOR: return "LITTOK_OPERATOR";
                case LITTOK_GET: return "LITTOK_GET";
                case LITTOK_SET: return "LITTOK_SET";
                case LITTOK_IN: return "LITTOK_IN";
                case LITTOK_CONST: return "LITTOK_CONST";
                case LITTOK_REF: return "LITTOK_REF";
                case LITTOK_ERROR: return "LITTOK_ERROR";
                case LITTOK_EOF: return "LITTOK_EOF";
                default:
                    break;
            }
            return "?unknown?";
        }

        void Scanner::init(State* state, const char* fname, const char* source, size_t len)
        {
            m_length = len;
            m_line = 1;
            m_startsrc = source;
            m_currsrc = source;
            m_filename = fname;
            m_state = state;
            m_numbraces = 0;
            m_haderror = false;
        }

        Token Scanner::make_token(TokenType type)
        {
            Token token;
            token.type = type;
            token.start = m_startsrc;
            token.length = (size_t)(m_currsrc - m_startsrc);
            token.line = m_line;
            return token;
        }

        Token Scanner::make_error_token(Error error, ...)
        {
            va_list args;
            Token token;
            String* result;
            m_haderror = true;
            va_start(args, error);
            result = lit_vformat_error(m_state, m_line, error, args);
            va_end(args);
            token.type = LITTOK_ERROR;
            token.start = result->data();
            token.length = result->length();
            token.line = m_line;
            return token;
        }

        bool Scanner::is_at_end()
        {
            return *m_currsrc == '\0';
        }

        char Scanner::advance()
        {
            m_currsrc++;
            return m_currsrc[-1];
        }

        bool Scanner::match(char expected)
        {
            if(is_at_end())
            {
                return false;
            }

            if(*m_currsrc != expected)
            {
                return false;
            }
            m_currsrc++;
            return true;
        }

        Token Scanner::match_token(char c, TokenType a, TokenType b)
        {
            return make_token(match(c) ? a : b);
        }

        Token Scanner::match_tokens(char cr, char cb, TokenType a, TokenType b, TokenType c)
        {
            return make_token(match(cr) ? a : (match(cb) ? b : c));
        }

        char Scanner::peek()
        {
            return *m_currsrc;
        }

        char Scanner::peek_next()
        {
            if(is_at_end())
            {
                return '\0';
            }
            return m_currsrc[1];
        }

        bool Scanner::skip_whitespace()
        {
            char a;
            char b;
            char c;
            (void)a;
            (void)b;
            while(true)
            {
                c = peek();
                switch(c)
                {
                    case ' ':
                    case '\r':
                    case '\t':
                        {
                            advance();
                        }
                        break;
                    case '\n':
                        {
                            m_startsrc = m_currsrc;
                            advance();
                            return true;
                        }
                        break;
                    case '/':
                        {
                            if(peek_next() == '/')
                            {
                                while(peek() != '\n' && !is_at_end())
                                {
                                    advance();
                                }
                                return skip_whitespace();
                            }
                            else if(peek_next() == '*')
                            {
                                advance();
                                advance();
                                a = peek();
                                b = peek_next();
                                while((peek() != '*' || peek_next() != '/') && !is_at_end())
                                {
                                    if(peek() == '\n')
                                    {
                                        m_line++;
                                    }
                                    advance();
                                }
                                advance();
                                advance();
                                return skip_whitespace();
                            }
                            return false;
                        }
                        break;
                    default:
                        {
                            return false;
                        }
                }
            }
        }

        Token Scanner::scan_string(char stype, bool interpolation)
        {
            char c;
            char newc;
            char nextc;
            int octval;
            State* state;
            PCGenericArray<uint8_t> bytes;
            Token token;
            TokenType string_type;
            state = m_state;
            string_type = LITTOK_STRING;
            bytes.init(m_state);
            while(true)
            {
                c = advance();
                if(c == stype)
                {
                    break;
                }
                else if(interpolation && c == '{')
                {
                    if(m_numbraces >= LIT_MAX_INTERPOLATION_NESTING)
                    {
                        return make_error_token(Error::LITERROR_INTERPOLATION_NESTING_TOO_DEEP, LIT_MAX_INTERPOLATION_NESTING);
                    }
                    string_type = LITTOK_INTERPOLATION;
                    m_braces[m_numbraces++] = 1;
                    break;
                }
                switch(c)
                {
                    case '\0':
                        {
                            return make_error_token(Error::LITERROR_UNTERMINATED_STRING);
                        }
                        break;
                    case '\n':
                        {
                            m_line++;
                            bytes.push(c);
                        }
                        break;
                    case '\\':
                        {
                            if(stype == '\'')
                            {
                                nextc = advance();
                                bytes.push('\\');
                                bytes.push(nextc);
                            }
                            else
                            {
                                switch(advance())
                                {
                                    case '\"':
                                        {
                                            bytes.push('\"');
                                        }
                                        break;
                                    case '\\':
                                        {
                                            bytes.push('\\');
                                        }
                                        break;
                                    case '0':
                                        {
                                            bytes.push('\0');
                                        }
                                        break;
                                    case '{':
                                        {
                                            bytes.push('{');
                                        }
                                        break;
                                    case 'a':
                                        {
                                            bytes.push('\a');
                                        }
                                        break;
                                    case 'b':
                                        {
                                            bytes.push('\b');
                                        }
                                        break;
                                    case 'f':
                                        {
                                            bytes.push('\f');
                                        }
                                        break;
                                    case 'n':
                                        {
                                            bytes.push('\n');
                                        }
                                        break;
                                    case 'r':
                                        {
                                            bytes.push('\r');
                                        }
                                        break;
                                    case 't':
                                        {
                                            bytes.push('\t');
                                        }
                                        break;
                                    case 'v':
                                        {
                                            bytes.push('\v');
                                        }
                                        break;
                                    case 'e':
                                        {
                                            bytes.push(27);
                                        }
                                        break;
                                    default:
                                        {
                                            return make_error_token(Error::LITERROR_INVALID_ESCAPE_CHAR, m_currsrc[-1]);

                                            fprintf(stderr, "m_currsrc[-1] = '%c', c = '%c'\n", m_currsrc[-1], c);
                                            if(isdigit(m_currsrc[-1]))
                                            {
                                                newc = m_currsrc[-1] - '0';
                                                octval = 0;
                                                octval |= (newc * 8);
                                                while(true)
                                                {
                                                    nextc = peek();
                                                    fprintf(stderr, "nextc = '%c'\n", nextc);
                                                    if(!isdigit(nextc))
                                                    {
                                                        break;
                                                    }
                                                    newc = advance();
                                                    fprintf(stderr, "newc = '%c'\n", nextc);
                                                    octval = octval << 3;
                                                    octval |= ((newc - '0') * 8);
                                                }
                                                bytes.push(octval);
                                            }
                                            else
                                            {
                                                return make_error_token(Error::LITERROR_INVALID_ESCAPE_CHAR, m_currsrc[-1]);
                                            }
                                        }
                                        break;
                                }
                            }
                        }
                        break;
                    default:
                        {
                            bytes.push(c);
                        }
                        break;
                }
            }
            token = make_token(string_type);
            token.value = String::copy(state, (const char*)bytes.m_values, bytes.m_count)->asValue();
            bytes.release();
            return token;
        }

        int Scanner::scan_hexdigit()
        {
            char c;
            c = advance();
            if((c >= '0') && (c <= '9'))
            {
                return (c - '0');
            }
            if((c >= 'a') && (c <= 'f'))
            {
                return (c - 'a' + 10);
            }
            if((c >= 'A') && (c <= 'F'))
            {
                return (c - 'A' + 10);
            }
            m_currsrc--;
            return -1;
        }

        int Scanner::scan_binarydigit()
        {
            char c;
            c = advance();
            if(c >= '0' && c <= '1')
            {
                return c - '0';
            }
            m_currsrc--;
            return -1;
        }

        Token Scanner::make_number_token(bool is_hex, bool is_binary)
        {
            Token token;
            Value value;
            errno = 0;
            if(is_hex)
            {
                value = Object::toValue((double)strtoll(m_startsrc, nullptr, 16));
            }
            else if(is_binary)
            {
                value = Object::toValue(strtoll(m_startsrc + 2, nullptr, 2));
            }
            else
            {
                value = Object::toValue(strtod(m_startsrc, nullptr));
            }

            if(errno == ERANGE)
            {
                errno = 0;
                return make_error_token(Error::LITERROR_NUMBER_IS_TOO_BIG);
            }
            token = make_token(LITTOK_NUMBER);
            token.value = value;
            return token;
        }

        Token Scanner::scan_number()
        {
            if(match('x'))
            {
                while(scan_hexdigit() != -1)
                {
                    continue;
                }
                return make_number_token(true, false);
            }
            if(match('b'))
            {
                while(scan_binarydigit() != -1)
                {
                    continue;
                }
                return make_number_token(false, true);
            }
            while(char_is_digit(peek()))
            {
                advance();
            }
            // Look for a fractional part.
            if(peek() == '.' && char_is_digit(peek_next()))
            {
                // Consume the '.'
                advance();
                while(char_is_digit(peek()))
                {
                    advance();
                }
            }
            return make_number_token(false, false);
        }

        TokenType Scanner::check_keyword(int start, int length, const char* rest, TokenType type)
        {
            if(m_currsrc - m_startsrc == start + length && memcmp(m_startsrc + start, rest, length) == 0)
            {
                return type;
            }
            return LITTOK_IDENTIFIER;
        }

        TokenType Scanner::scan_identtype()
        {
            switch(m_startsrc[0])
            {
                case 'b':
                    return check_keyword(1, 4, "reak", LITTOK_BREAK);

                case 'c':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'l':
                                    return check_keyword(2, 3, "ass", LITTOK_CLASS);
                                case 'o':
                                {
                                    if(m_currsrc - m_startsrc > 3)
                                    {
                                        switch(m_startsrc[3])
                                        {
                                            case 's':
                                                return check_keyword(2, 3, "nst", LITTOK_CONST);
                                            case 't':
                                                return check_keyword(2, 6, "ntinue", LITTOK_CONTINUE);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                case 'e':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'l':
                                    return check_keyword(2, 2, "se", LITTOK_ELSE);
                                case 'x':
                                    return check_keyword(2, 4, "port", LITTOK_EXPORT);
                            }
                        }
                    }
                    break;
                case 'f':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'a':
                                    return check_keyword(2, 3, "lse", LITTOK_FALSE);
                                case 'o':
                                    return check_keyword(2, 1, "r", LITTOK_FOR);
                                case 'u':
                                    return check_keyword(2, 6, "nction", LITTOK_FUNCTION);
                            }
                        }
                    }
                    break;
                case 'i':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 's':
                                    return check_keyword(2, 0, "", LITTOK_IS);
                                case 'f':
                                    return check_keyword(2, 0, "", LITTOK_IF);
                                case 'n':
                                    return check_keyword(2, 0, "", LITTOK_IN);
                            }
                        }
                    }
                    break;
                case 'n':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'u':
                                    return check_keyword(2, 2, "ll", LITTOK_NULL);
                                case 'e':
                                    return check_keyword(2, 1, "w", LITTOK_NEW);
                            }
                        }
                    }
                    break;
                case 'r':
                    {
                        if(m_currsrc - m_startsrc > 2)
                        {
                            switch(m_startsrc[2])
                            {
                                case 'f':
                                    return check_keyword(3, 0, "", LITTOK_REF);
                                case 't':
                                    return check_keyword(3, 3, "urn", LITTOK_RETURN);
                            }
                        }
                    }
                    break;
                case 'o':
                    return check_keyword(1, 7, "perator", LITTOK_OPERATOR);
                case 's':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'u':
                                    return check_keyword(2, 3, "per", LITTOK_SUPER);
                                case 't':
                                    return check_keyword(2, 4, "atic", LITTOK_STATIC);
                                case 'e':
                                    return check_keyword(2, 1, "t", LITTOK_SET);
                            }
                        }
                    }
                    break;
                case 't':
                    {
                        if(m_currsrc - m_startsrc > 1)
                        {
                            switch(m_startsrc[1])
                            {
                                case 'h':
                                    return check_keyword(2, 2, "is", LITTOK_THIS);
                                case 'r':
                                    return check_keyword(2, 2, "ue", LITTOK_TRUE);
                            }
                        }

                    }
                    break;
                case 'v':
                    return check_keyword(1, 2, "ar", LITTOK_VAR);
                case 'w':
                    return check_keyword(1, 4, "hile", LITTOK_WHILE);
                case 'g':
                    return check_keyword(1, 2, "et", LITTOK_GET);
            }
            return LITTOK_IDENTIFIER;
        }

        Token Scanner::scan_identifier()
        {
            while(char_is_alpha(peek()) || char_is_digit(peek()))
            {
                advance();
            }
            return make_token(scan_identtype());
        }

        Token Scanner::scan_token()
        {
            if(skip_whitespace())
            {
                Token token = make_token(LITTOK_NEW_LINE);
                m_line++;

                return token;
            }
            m_startsrc = m_currsrc;
            if(is_at_end())
            {
                return make_token(LITTOK_EOF);
            }
            char c = advance();
            if(char_is_digit(c))
            {
                return scan_number();
            }
            if(char_is_alpha(c))
            {
                return scan_identifier();
            }
            switch(c)
            {
                case '(':
                    return make_token(LITTOK_LEFT_PAREN);
                case ')':
                    return make_token(LITTOK_RIGHT_PAREN);
                case '{':
                    {
                        if(m_numbraces > 0)
                        {
                            m_braces[m_numbraces - 1]++;
                        }
                        return make_token(LITTOK_LEFT_BRACE);
                    }
                    break;
                case '}':
                    {
                        if(m_numbraces > 0 && --m_braces[m_numbraces - 1] == 0)
                        {
                            m_numbraces--;
                            return scan_string('"', true);
                        }
                        return make_token(LITTOK_RIGHT_BRACE);
                    }
                    break;
                case '[':
                    return make_token(LITTOK_LEFT_BRACKET);
                case ']':
                    return make_token(LITTOK_RIGHT_BRACKET);
                case ';':
                    return make_token(LITTOK_SEMICOLON);
                case ',':
                    return make_token(LITTOK_COMMA);
                case ':':
                    return make_token(LITTOK_COLON);
                case '~':
                    return make_token(LITTOK_TILDE);
                case '+':
                    return match_tokens('=', '+', LITTOK_PLUS_EQUAL, LITTOK_PLUS_PLUS, LITTOK_PLUS);
                case '-':
                    {
                        if(match('>'))
                        {
                            return make_token(LITTOK_SMALL_ARROW);
                        }
                        return match_tokens('=', '-', LITTOK_MINUS_EQUAL, LITTOK_MINUS_MINUS, LITTOK_MINUS);
                    }
                    break;
                case '/':
                    return match_token('=', LITTOK_SLASH_EQUAL, LITTOK_SLASH);
                case '#':
                    return match_token('=', LITTOK_SHARP_EQUAL, LITTOK_SHARP);
                case '!':
                    return match_token('=', LITTOK_BANG_EQUAL, LITTOK_BANG);
                case '?':
                    return match_token('?', LITTOK_QUESTION_QUESTION, LITTOK_QUESTION);
                case '%':
                    return match_token('=', LITTOK_PERCENT_EQUAL, LITTOK_PERCENT);
                case '^':
                    return match_token('=', LITTOK_CARET_EQUAL, LITTOK_CARET);

                case '>':
                    return match_tokens('=', '>', LITTOK_GREATER_EQUAL, LITTOK_GREATER_GREATER, LITTOK_GREATER);
                case '<':
                    return match_tokens('=', '<', LITTOK_LESS_EQUAL, LITTOK_LESS_LESS, LITTOK_LESS);
                case '*':
                    return match_tokens('=', '*', LITTOK_STAR_EQUAL, LITTOK_STAR_STAR, LITTOK_STAR);
                case '=':
                    return match_tokens('=', '>', LITTOK_EQUAL_EQUAL, LITTOK_ARROW, LITTOK_EQUAL);
                case '|':
                    return match_tokens('=', '|', LITTOK_BAR_EQUAL, LITTOK_BAR_BAR, LITTOK_BAR);
                case '&':
                    return match_tokens('=', '&', LITTOK_AMPERSAND_EQUAL, LITTOK_AMPERSAND_AMPERSAND, LITTOK_AMPERSAND);
                case '.':
                {
                    if(!match('.'))
                    {
                        return make_token(LITTOK_DOT);
                    }
                    return match_token('.', LITTOK_DOT_DOT_DOT, LITTOK_DOT_DOT);
                }
                case '$':
                {
                    if(!match('\"'))
                    {
                        return make_error_token(Error::LITERROR_CHAR_EXPECTATION_UNMET, '\"', '$', peek());
                    }
                    return scan_string('"', true);
                }
                case '"':
                case '\'':
                    return scan_string(c, false);
            }
            return make_error_token(Error::LITERROR_UNEXPECTED_CHAR, c);
        }
    }
}




