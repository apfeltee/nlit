
#include "lit.h"
#include "priv.h"

namespace lit
{
    void lit_add_definition(State* state, const char* name)
    {
        state->preprocessor->defined.set(String::intern(state, name), Object::TrueVal);
    }

    static void override(char* source, int length)
    {
        while(length-- > 0)
        {
            if(*source != '\n')
            {
                *source = ' ';
            }
            source++;
        }
    }

    bool lit_preprocess(AST::Preprocessor* preprocessor, char* source)
    {
        bool close;
        bool in_macro;
        bool in_arg;
        bool on_new_line;
        int ignore_depth;
        int depth;
        char c;
        char* branch_start;
        char* macro_start;
        char* arg_start;
        char* current;
        State* state;
        Value tmp;
        String* arg;

        state = preprocessor->m_state;
        macro_start = source;
        arg_start = source;
        current = source;
        in_macro = false;
        in_arg = false;
        on_new_line = true;
        ignore_depth = -1;
        depth = 0;
        do
        {
            c = current[0];
            current++;
            // Single line comment
            if(c == '/' && current[0] == '/')
            {
                current++;
                do
                {
                    c = current[0];
                    current++;
                } while(c != '\n' && c != '\0');
                in_macro = false;
                on_new_line = true;
                continue;
            }
            else if(c == '/' && current[0] == '*')
            {
                // Multiline comment
                current++;
                do
                {
                    c = current[0];
                    current++;
                } while(c != '*' && c != '\0' && current[0] != '/');
                in_macro = false;
                on_new_line = true;
                continue;
            }
            if(in_macro)
            {
                if(!AST::Scanner::char_is_alpha(c) && !(((current - macro_start) > 1) && AST::Scanner::char_is_digit(c)))
                {
                    if(in_arg)
                    {
                        arg = String::copy(state, arg_start, (int)(current - arg_start) - 1);
                        if(memcmp(macro_start, "define", 6) == 0 || memcmp(macro_start, "undef", 5) == 0)
                        {
                            if(ignore_depth < 0)
                            {
                                close = macro_start[0] == 'u';
                                if(close)
                                {
                                    preprocessor->defined.remove(arg);
                                }
                                else
                                {
                                    preprocessor->defined.set(arg, Object::TrueVal);
                                }
                            }
                        }
                        else
                        {// ifdef || ifndef
                            depth++;
                            if(ignore_depth < 0)
                            {
                                close = macro_start[2] == 'n';
                                if((preprocessor->defined.get(arg, &tmp) ^ close) == false)
                                {
                                    ignore_depth = depth;
                                }

                                preprocessor->open_ifs.push((Value)macro_start);
                            }
                        }
                        // Remove the macro from code
                        override(macro_start - 1, (int)(current - macro_start));
                        in_macro = false;
                        in_arg = false;
                    }
                    else
                    {
                        if(memcmp(macro_start, "define", 6) == 0 || memcmp(macro_start, "undef", 5) == 0
                           || memcmp(macro_start, "ifdef", 5) == 0 || memcmp(macro_start, "ifndef", 6) == 0)
                        {
                            arg_start = current;
                            in_arg = true;
                        }
                        else if(memcmp(macro_start, "else", 4) == 0 || memcmp(macro_start, "endif", 5) == 0)
                        {
                            in_macro = false;
                            // If this is endif
                            if(macro_start[1] == 'n')
                            {
                                depth--;
                                if(ignore_depth > -1)
                                {
                                    // Remove the whole if branch from code
                                    branch_start = (char*)preprocessor->open_ifs.m_values[preprocessor->open_ifs.m_count - 1];
                                    override(branch_start - 1, (int)(current - branch_start));
                                    if(ignore_depth == depth + 1)
                                    {
                                        ignore_depth = -1;
                                        preprocessor->open_ifs.m_count--;
                                    }
                                }
                                else
                                {
                                    preprocessor->open_ifs.m_count--;
                                    // Remove #endif
                                    override(macro_start - 1, (int)(current - macro_start));
                                }
                            }
                            else if(ignore_depth < 0 || depth <= ignore_depth)
                            {
                                // #else
                                if(ignore_depth == depth)
                                {
                                    // Remove the macro from code
                                    branch_start = (char*)preprocessor->open_ifs.m_values[preprocessor->open_ifs.m_count - 1];
                                    override(branch_start - 1, (int)(current - branch_start));
                                    ignore_depth = -1;
                                }
                                else
                                {
                                    preprocessor->open_ifs.m_values[preprocessor->open_ifs.m_count - 1] = (Value)macro_start;
                                    ignore_depth = depth;
                                }
                            }
                        }
                        else
                        {
                            preprocessor->m_state->raiseError((ErrorType)0,
                                      lit_format_error(preprocessor->m_state, 0, Error::LITERROR_UNKNOWN_MACRO, (int)(current - macro_start) - 1, macro_start)
                                      ->data());
                            return false;
                        }
                    }
                }
            }
            else
            {
                macro_start = current;
                if(c == '\n')
                {
                    on_new_line = true;
                }
                else if(!(c == '\t' || c == ' ' || c == '#'))
                {
                    on_new_line = false;
                }
                else
                {
                    in_macro = on_new_line && c == '#';
                }
            }
        } while(c != '\0');
        if(in_macro || preprocessor->open_ifs.m_count > 0 || depth > 0)
        {
            preprocessor->m_state->raiseError((ErrorType)0, lit_format_error(preprocessor->m_state, 0, Error::LITERROR_UNCLOSED_MACRO)->data());
            return false;
        }
        preprocessor->open_ifs.release();
        return true;
    }

    namespace AST
    {

    }
}

