
#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        static bool did_setup_rules;

        ParseRule Parser::rules[LITTOK_EOF + 1];

        void Parser::init(State* state)
        {
            if(!did_setup_rules)
            {
                did_setup_rules = true;
                Parser::setup_rules();
            }
            m_state = state;
            m_haderror = false;
            m_panicmode = false;
        }

        void Parser::stringError(Token* token, const char* message)
        {
            (void)token;
            if(m_panicmode)
            {
                return;
            }
            m_state->raiseError(COMPILE_ERROR, message);
            m_haderror = true;
            this->sync();
        }

        Scanner* Parser::getStateScanner(State* state)
        {
            return state->scanner;
        }

        void Parser::release()
        {
        }

        void Parser::end_compiler(Compiler* compiler)
        {
            m_compiler = (Compiler*)compiler->enclosing;
        }

        void Parser::begin_scope()
        {
            m_compiler->scope_depth++;
        }

        void Parser::end_scope()
        {
            m_compiler->scope_depth--;
        }

        ParseRule* Parser::get_rule(TokenType type)
        {
            return &Parser::rules[type];
        }

        void Parser::errorAt(Token* token, Error error, va_list args)
        {
            stringError(token, lit_vformat_error(m_state, token->line, error, args)->data());
        }

        void Parser::errorAtCurrent(Error error, ...)
        {
            va_list args;
            va_start(args, error);
            errorAt(&m_currtoken, error, args);
            va_end(args);
        }

        void Parser::raiseError(Error error, ...)
        {
            va_list args;
            va_start(args, error);
            errorAt(&m_prevtoken, error, args);
            va_end(args);
        }

        void Parser::advance()
        {
            m_prevtoken = m_currtoken;
            while(true)
            {
                m_currtoken = getStateScanner(m_state)->scan_token();
                if(m_currtoken.type != LITTOK_ERROR)
                {
                    break;
                }
                stringError(&m_currtoken, m_currtoken.start);
            }
        }

        bool Parser::check(TokenType type)
        {
            return m_currtoken.type == type;
        }

        bool Parser::match(TokenType type)
        {
            if(m_currtoken.type == type)
            {
                advance();
                return true;
            }
            return false;
        }

        bool Parser::match_new_line()
        {
            while(true)
            {
                if(!match(LITTOK_NEW_LINE))
                {
                    return false;
                }
            }
            return true;
        }

        void Parser::ignore_new_lines()
        {
            match_new_line();
        }

        void Parser::consume(TokenType type, const char* error)
        {
            bool line;
            size_t olen;
            const char* fmt;
            const char* otext;
            if(m_currtoken.type == type)
            {
                advance();
                return;
            }
            line = m_prevtoken.type == LITTOK_NEW_LINE;
            olen = (line ? 8 : m_prevtoken.length);
            otext = (line ? "new line" : m_prevtoken.start);
            fmt = lit_format_error(m_state, m_currtoken.line, Error::LITERROR_EXPECTATION_UNMET, error, olen, otext)->data();
            stringError(&m_currtoken,fmt);
        }

        void Parser::sync()
        {
            m_panicmode = false;
            while(m_currtoken.type != LITTOK_EOF)
            {
                if(m_prevtoken.type == LITTOK_NEW_LINE)
                {
                    longjmp(m_jumpbuffer, 1);
                    return;
                }
                switch(m_currtoken.type)
                {
                    case LITTOK_CLASS:
                    case LITTOK_FUNCTION:
                    case LITTOK_EXPORT:
                    case LITTOK_VAR:
                    case LITTOK_CONST:
                    case LITTOK_FOR:
                    case LITTOK_STATIC:
                    case LITTOK_IF:
                    case LITTOK_WHILE:
                    case LITTOK_RETURN:
                        {
                            longjmp(m_jumpbuffer, 1);
                            return;
                        }
                        break;
                    default:
                        {
                            advance();
                        }
                }
            }
        }

        bool Parser::parse(const char* filename, const char* source, size_t length, Expression::List& statements)
        {
            Compiler compiler;
            Expression* statement;
            m_haderror = false;
            m_panicmode = false;
            this->getStateScanner(m_state)->init(m_state, filename, source, length);
            compiler.init(this);
            this->advance();
            this->ignore_new_lines();
            if(!this->is_at_end())
            {
                do
                {
                    statement = Parser::parse_declaration(this);
                    if(statement != nullptr)
                    {
                        statements.push(statement);
                    }
                    if(!this->match_new_line())
                    {
                        if(this->match(LITTOK_EOF))
                        {
                            break;
                        }
                    }
                } while(!this->is_at_end());
            }
            return m_haderror || this->getStateScanner(m_state)->m_haderror;
        }

        void Parser::setup_rules()
        {
            Parser::rules[LITTOK_LEFT_PAREN] = ParseRule{ parse_grouping_or_lambda, parse_call, LITPREC_CALL };
            Parser::rules[LITTOK_PLUS] = ParseRule{ nullptr, parse_binary, LITPREC_TERM };
            Parser::rules[LITTOK_MINUS] = ParseRule{ parse_unary, parse_binary, LITPREC_TERM };
            Parser::rules[LITTOK_BANG] = ParseRule{ parse_unary, parse_binary, LITPREC_TERM };
            Parser::rules[LITTOK_STAR] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_STAR_STAR] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_SLASH] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_SHARP] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_STAR] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_STAR] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_BAR] = ParseRule{ nullptr, parse_binary, LITPREC_BOR };
            Parser::rules[LITTOK_AMPERSAND] = ParseRule{ nullptr, parse_binary, LITPREC_BAND };
            Parser::rules[LITTOK_TILDE] = ParseRule{ parse_unary, nullptr, LITPREC_UNARY };
            Parser::rules[LITTOK_CARET] = ParseRule{ nullptr, parse_binary, LITPREC_BOR };
            Parser::rules[LITTOK_LESS_LESS] = ParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
            Parser::rules[LITTOK_GREATER_GREATER] = ParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
            Parser::rules[LITTOK_PERCENT] = ParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
            Parser::rules[LITTOK_IS] = ParseRule{ nullptr, parse_binary, LITPREC_IS };
            Parser::rules[LITTOK_NUMBER] = ParseRule{ parse_number, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_TRUE] = ParseRule{ parse_literal, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_FALSE] = ParseRule{ parse_literal, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_NULL] = ParseRule{ parse_literal, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_BANG_EQUAL] = ParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
            Parser::rules[LITTOK_EQUAL_EQUAL] = ParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
            Parser::rules[LITTOK_GREATER] = ParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
            Parser::rules[LITTOK_GREATER_EQUAL] = ParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
            Parser::rules[LITTOK_LESS] = ParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
            Parser::rules[LITTOK_LESS_EQUAL] = ParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
            Parser::rules[LITTOK_STRING] = ParseRule{ parse_string, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_INTERPOLATION] = ParseRule{ parse_interpolation, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_IDENTIFIER] = ParseRule{ parse_variable_expression, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_NEW] = ParseRule{ parse_new_expression, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_PLUS_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_MINUS_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_STAR_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_SLASH_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_SHARP_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_PERCENT_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_CARET_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_BAR_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_AMPERSAND_EQUAL] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_PLUS_PLUS] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_MINUS_MINUS] = ParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
            Parser::rules[LITTOK_AMPERSAND_AMPERSAND] = ParseRule{ nullptr, parse_and, LITPREC_AND };
            Parser::rules[LITTOK_BAR_BAR] = ParseRule{ nullptr, parse_or, LITPREC_AND };
            Parser::rules[LITTOK_QUESTION_QUESTION] = ParseRule{ nullptr, parse_null_filter, LITPREC_NULL };
            Parser::rules[LITTOK_DOT] = ParseRule{ nullptr, parse_dot, LITPREC_CALL };
            Parser::rules[LITTOK_SMALL_ARROW] = ParseRule{ nullptr, parse_dot, LITPREC_CALL };
            Parser::rules[LITTOK_DOT_DOT] = ParseRule{ nullptr, parse_range, LITPREC_RANGE };
            Parser::rules[LITTOK_DOT_DOT_DOT] = ParseRule{ parse_variable_expression, nullptr, LITPREC_ASSIGNMENT };
            Parser::rules[LITTOK_LEFT_BRACKET] = ParseRule{ parse_array, parse_subscript, LITPREC_NONE };
            Parser::rules[LITTOK_LEFT_BRACE] = ParseRule{ parse_object, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_THIS] = ParseRule{ parse_this, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_SUPER] = ParseRule{ parse_super, nullptr, LITPREC_NONE };
            Parser::rules[LITTOK_QUESTION] = ParseRule{ nullptr, parse_ternary_or_question, LITPREC_EQUALITY };
            Parser::rules[LITTOK_REF] = ParseRule{ parse_reference, nullptr, LITPREC_NONE };
            //Parser::rules[LITTOK_SEMICOLON] = ParseRule{nullptr, nullptr, LITPREC_NONE};
        }

        Expression* Parser::parse_block(Parser* parser)
        {
            parser->begin_scope();
            auto statement = StmtBlock::make(parser->m_state, parser->m_prevtoken.line);
            while(true)
            {
                parser->ignore_new_lines();
                if(parser->check(LITTOK_RIGHT_BRACE) || parser->check(LITTOK_EOF))
                {
                    break;
                }
                parser->ignore_new_lines();
                statement->statements.push(parse_statement(parser));
                parser->ignore_new_lines();
                parser->match(LITTOK_SEMICOLON);
            }
            parser->ignore_new_lines();
            parser->consume(LITTOK_RIGHT_BRACE, "'}'");
            parser->ignore_new_lines();
            parser->end_scope();
            return (Expression*)statement;
        }

        Expression* Parser::parse_precedence(Parser* parser, Precedence precedence, bool err)
        {
            bool new_line;
            bool prev_newline;
            bool parser_prev_newline;
            bool can_assign;
            Expression* expr;
            ParseRule::PrefixFuncType prefix_rule;
            ParseRule::InfixFuncType infix_rule;
            Token previous;
            (void)new_line;
            prev_newline = false;
            previous = parser->m_prevtoken;
            parser->advance();
            prefix_rule = parser->get_rule(parser->m_prevtoken.type)->prefix;
            if(prefix_rule == nullptr)
            {
                //fprintf(stderr, "parser->m_prevtoken.type=%s, parser->m_currtoken.type=%s\n", token_name(parser->m_prevtoken.type), token_name(parser->m_currtoken.type));
                if(parser->m_prevtoken.type == LITTOK_SEMICOLON)
                {
                    if(parser->m_currtoken.type == LITTOK_NEW_LINE)
                    {
                        parser->advance();
                        return parse_precedence(parser, precedence, err);
                    }
                    return nullptr;
                }
                if(parser->m_currtoken.type == LITTOK_EOF)
                {
                    return nullptr;
                }
                // todo: file start
                new_line = previous.start != nullptr && *previous.start == '\n';
                parser_prev_newline = parser->m_prevtoken.start != nullptr && *parser->m_prevtoken.start == '\n';
                parser->raiseError(Error::LITERROR_EXPECTED_EXPRESSION,
                    (prev_newline ? 8 : previous.length),
                    (prev_newline ? "new line" : previous.start),
                    (parser_prev_newline ? 8 : parser->m_prevtoken.length),
                    (parser_prev_newline ? "new line" : parser->m_prevtoken.start)
                );
                return nullptr;
                
            }
            can_assign = precedence <= LITPREC_ASSIGNMENT;
            expr = prefix_rule(parser, can_assign);
            parser->ignore_new_lines();
            while(precedence <= parser->get_rule(parser->m_currtoken.type)->precedence)
            {
                parser->advance();
                infix_rule = parser->get_rule(parser->m_prevtoken.type)->infix;
                expr = infix_rule(parser, expr, can_assign);
            }
            if(err && can_assign && parser->match(LITTOK_EQUAL))
            {
                parser->raiseError(Error::LITERROR_INVALID_ASSIGMENT_TARGET);
            }
            return expr;
        }

        Expression* Parser::parse_number(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            return (Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value);
        }

        Expression* Parser::parse_lambda(Parser* parser, ExprLambda* lambda)
        {
            lambda->body = parse_statement(parser);
            return (Expression*)lambda;
        }

        void Parser::parse_parameters(Parser* parser, PCGenericArray<ExprFuncParam>* parameters)
        {
            bool had_default;
            size_t arg_length;
            const char* arg_name;
            Expression* default_value;
            had_default = false;
            while(!parser->check(LITTOK_RIGHT_PAREN))
            {
                // Vararg ...
                if(parser->match(LITTOK_DOT_DOT_DOT))
                {
                    parameters->push(ExprFuncParam{ "...", 3, nullptr });
                    return;
                }
                parser->consume(LITTOK_IDENTIFIER, "argument name");
                arg_name = parser->m_prevtoken.start;
                arg_length = parser->m_prevtoken.length;
                default_value = nullptr;
                if(parser->match(LITTOK_EQUAL))
                {
                    had_default = true;
                    default_value = parse_expression(parser);
                }
                else if(had_default)
                {
                    parser->raiseError(Error::LITERROR_DEFAULT_ARG_CENTRED);
                }
                parameters->push(ExprFuncParam{ arg_name, arg_length, default_value });
                if(!parser->match(LITTOK_COMMA))
                {
                    break;
                }
            }
        }

        Expression* Parser::parse_grouping_or_lambda(Parser* parser, bool can_assign)
        {
            bool stop;
            bool had_default;
            bool had_vararg;
            bool had_array;
            bool had_arrow;
            size_t line;
            size_t first_arg_length;
            size_t arg_length;
            const char* start;
            const char* arg_name;
            const char* first_arg_start;
            Expression* expression;
            Expression* default_value;
            Scanner* scanner;
            (void)can_assign;
            (void)had_arrow;
            (void)had_array;
            had_arrow = false;
            if(parser->match(LITTOK_RIGHT_PAREN))
            {
                parser->consume(LITTOK_ARROW, "=> after lambda arguments");
                return parse_lambda(parser, ExprLambda::make(parser->m_state, parser->m_prevtoken.line));
            }
            start = parser->m_prevtoken.start;
            line = parser->m_prevtoken.line;
            if(parser->match(LITTOK_IDENTIFIER) || parser->match(LITTOK_DOT_DOT_DOT))
            {
                auto state = parser->m_state;
                first_arg_start = parser->m_prevtoken.start;
                first_arg_length = parser->m_prevtoken.length;
                if(parser->match(LITTOK_COMMA) || (parser->match(LITTOK_RIGHT_PAREN) && parser->match(LITTOK_ARROW)))
                {
                    had_array = parser->m_prevtoken.type == LITTOK_ARROW;
                    had_vararg= parser->m_prevtoken.type == LITTOK_DOT_DOT_DOT;
                    // This is a lambda
                    auto lambda = ExprLambda::make(state, line);
                    Expression* def_value = nullptr;
                    had_default = parser->match(LITTOK_EQUAL);
                    if(had_default)
                    {
                        def_value = parse_expression(parser);
                    }
                    lambda->parameters.push(ExprFuncParam{ first_arg_start, first_arg_length, def_value });
                    if(!had_vararg && parser->m_prevtoken.type == LITTOK_COMMA)
                    {
                        do
                        {
                            stop = false;
                            if(parser->match(LITTOK_DOT_DOT_DOT))
                            {
                                stop = true;
                            }
                            else
                            {
                                parser->consume(LITTOK_IDENTIFIER, "argument name");
                            }

                            arg_name = parser->m_prevtoken.start;
                            arg_length = parser->m_prevtoken.length;
                            default_value = nullptr;
                            if(parser->match(LITTOK_EQUAL))
                            {
                                default_value = parse_expression(parser);
                                had_default = true;
                            }
                            else if(had_default)
                            {
                                parser->raiseError(Error::LITERROR_DEFAULT_ARG_CENTRED);
                            }
                            lambda->parameters.push(ExprFuncParam{ arg_name, arg_length, default_value });
                            if(stop)
                            {
                                break;
                            }
                        } while(parser->match(LITTOK_COMMA));
                    }
                    #if 0
                    if(!had_arrow)
                    {
                        parser->consume(LITTOK_RIGHT_PAREN, "')' after lambda parameters");
                        parser->consume(LITTOK_ARROW, "=> after lambda parameters");
                    }
                    #endif
                    return parse_lambda(parser, lambda);
                }
                else
                {
                    // Ouch, this was a grouping with a single identifier
                    scanner = parser->getStateScanner(state);
                    scanner->m_currsrc = start;
                    scanner->m_line = line;
                    parser->m_currtoken = scanner->scan_token();
                    parser->advance();
                }
            }
            expression = parse_expression(parser);
            parser->consume(LITTOK_RIGHT_PAREN, "')' after grouping expression");
            return expression;
        }

        Expression* Parser::parse_call(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            Expression* e;
            ExprVar* ee;
            ExprCall* expression;
            expression = ExprCall::make(parser->m_state, parser->m_prevtoken.line, prev);
            while(!parser->check(LITTOK_RIGHT_PAREN))
            {
                e = parse_expression(parser);
                expression->args.push(e);
                if(!parser->match(LITTOK_COMMA))
                {
                    break;
                }
                if(e->type == Expression::Type::Variable)
                {
                    ee = (ExprVar*)e;
                    // Vararg ...
                    if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                    {
                        break;
                    }
                }
            }
            if(expression->args.m_count > 255)
            {
                parser->raiseError(Error::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)expression->args.m_count);
            }
            parser->consume(LITTOK_RIGHT_PAREN, "')' after arguments");
            return (Expression*)expression;
        }

        Expression* Parser::parse_unary(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            auto op = parser->m_prevtoken.type;
            auto line = parser->m_prevtoken.line;
            auto expression = parse_precedence(parser, LITPREC_UNARY, true);
            return (Expression*)ExprUnary::make(parser->m_state, line, expression, op);
        }

        Expression* Parser::parse_binary(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            bool invert;
            size_t line;
            ParseRule* rule;
            Expression* expression;
            TokenType op;
            invert = parser->m_prevtoken.type == LITTOK_BANG;
            if(invert)
            {
                parser->consume(LITTOK_IS, "'is' after '!'");
            }
            op = parser->m_prevtoken.type;
            line = parser->m_prevtoken.line;
            rule = parser->get_rule(op);
            expression = parse_precedence(parser, (Precedence)(rule->precedence + 1), true);
            expression = (Expression*)ExprBinary::make(parser->m_state, line, prev, expression, op);
            if(invert)
            {
                expression = (Expression*)ExprUnary::make(parser->m_state, line, expression, LITTOK_BANG);
            }
            return expression;
        }

        Expression* Parser::parse_and(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            TokenType op;
            op = parser->m_prevtoken.type;
            line = parser->m_prevtoken.line;
            return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_AND, true), op);
        }

        Expression* Parser::parse_or(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            TokenType op;
            op = parser->m_prevtoken.type;
            line = parser->m_prevtoken.line;
            return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_OR, true), op);
        }

        Expression* Parser::parse_null_filter(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            TokenType op;
            op = parser->m_prevtoken.type;
            line = parser->m_prevtoken.line;
            return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_NULL, true), op);
        }

        TokenType Parser::convert_compound_operator(TokenType op)
        {
            switch(op)
            {
                case LITTOK_PLUS_EQUAL:
                    {
                        return LITTOK_PLUS;
                    }
                    break;
                case LITTOK_MINUS_EQUAL:
                    {
                        return LITTOK_MINUS;
                    }
                    break;
                case LITTOK_STAR_EQUAL:
                    {
                        return LITTOK_STAR;
                    }
                    break;
                case LITTOK_SLASH_EQUAL:
                    {
                        return LITTOK_SLASH;
                    }
                    break;
                case LITTOK_SHARP_EQUAL:
                    {
                        return LITTOK_SHARP;
                    }
                    break;
                case LITTOK_PERCENT_EQUAL:
                    {
                        return LITTOK_PERCENT;
                    }
                    break;
                case LITTOK_CARET_EQUAL:
                    {
                        return LITTOK_CARET;
                    }
                    break;
                case LITTOK_BAR_EQUAL:
                    {
                        return LITTOK_BAR;
                    }
                    break;
                case LITTOK_AMPERSAND_EQUAL:
                    {
                        return LITTOK_AMPERSAND;
                    }
                    break;
                case LITTOK_PLUS_PLUS:
                    {
                        return LITTOK_PLUS;
                    }
                    break;
                case LITTOK_MINUS_MINUS:
                    {
                        return LITTOK_MINUS;
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
            return (TokenType)-1;
        }

        Expression* Parser::parse_compound(Parser* parser, Expression* prev, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            ExprBinary* binary;
            Expression* expression;
            ParseRule* rule;
            TokenType op;
            op = parser->m_prevtoken.type;
            line = parser->m_prevtoken.line;
            rule = parser->get_rule(op);
            if(op == LITTOK_PLUS_PLUS || op == LITTOK_MINUS_MINUS)
            {
                expression = (Expression*)ExprLiteral::make(parser->m_state, line, Object::toValue(1));
            }
            else
            {
                expression = parse_precedence(parser, (Precedence)(rule->precedence + 1), true);
            }
            binary = ExprBinary::make(parser->m_state, line, prev, expression, convert_compound_operator(op));
            binary->ignore_left = true;// To make sure we don't free it twice
            return (Expression*)ExprAssign::make(parser->m_state, line, prev, (Expression*)binary);
        }

        Expression* Parser::parse_literal(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            line = parser->m_prevtoken.line;
            switch(parser->m_prevtoken.type)
            {
                case LITTOK_TRUE:
                    {
                        return (Expression*)ExprLiteral::make(parser->m_state, line, Object::TrueVal);
                    }
                    break;
                case LITTOK_FALSE:
                    {
                        return (Expression*)ExprLiteral::make(parser->m_state, line, Object::FalseVal);
                    }
                    break;
                case LITTOK_NULL:
                    {
                        return (Expression*)ExprLiteral::make(parser->m_state, line, Object::NullVal);
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
            return nullptr;
        }

        Expression* Parser::parse_string(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            auto expression = (Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value);
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, expression, can_assign);
            }
            return expression;
        }

        Expression* Parser::parse_interpolation(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            auto expression = ExprInterpolation::make(parser->m_state, parser->m_prevtoken.line);
            do
            {
                if(Object::as<String>(parser->m_prevtoken.value)->length() > 0)
                {
                    expression->expressions.push((Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value));
                }
                expression->expressions.push(parse_expression(parser));
            } while(parser->match(LITTOK_INTERPOLATION));
            parser->consume(LITTOK_STRING, "end of interpolation");
            if(Object::as<String>(parser->m_prevtoken.value)->length() > 0)
            {
                expression->expressions.push((Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value));
            }
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, (Expression*)expression, can_assign);
            }
            return (Expression*)expression;
        }

        Expression* Parser::parse_object(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            auto objexpr = ExprObject::make(parser->m_state, parser->m_prevtoken.line);
            parser->ignore_new_lines();
            while(!parser->check(LITTOK_RIGHT_BRACE))
            {
                parser->ignore_new_lines();
                parser->consume(LITTOK_IDENTIFIER, "key string after '{'");
                objexpr->keys.push(String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length)->asValue());
                parser->ignore_new_lines();
                parser->consume(LITTOK_EQUAL, "'=' after key string");
                parser->ignore_new_lines();
                objexpr->values.push(parse_expression(parser));
                if(!parser->match(LITTOK_COMMA))
                {
                    break;
                }
            }
            parser->ignore_new_lines();
            parser->consume(LITTOK_RIGHT_BRACE, "'}' after object");
            return (Expression*)objexpr;
        }

        Expression* Parser::parse_variable_expression_base(Parser* parser, bool can_assign, bool isnew)
        {
            (void)can_assign;
            bool had_args;
            ExprCall* call;
            auto expression = (Expression*)ExprVar::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.start, parser->m_prevtoken.length);
            if(isnew)
            {
                had_args = parser->check(LITTOK_LEFT_PAREN);
                call = nullptr;
                if(had_args)
                {
                    parser->advance();
                    call = (ExprCall*)parse_call(parser, expression, false);
                }
                if(parser->match(LITTOK_LEFT_BRACE))
                {
                    if(call == nullptr)
                    {
                        call = ExprCall::make(parser->m_state, expression->line, expression);
                    }
                    call->objexpr = parse_object(parser, false);
                }
                else if(!had_args)
                {
                    parser->errorAtCurrent(Error::LITERROR_EXPECTATION_UNMET, "argument list for instance creation",
                                     parser->m_prevtoken.length, parser->m_prevtoken.start);
                }
                return (Expression*)call;
            }
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, expression, can_assign);
            }
            if(can_assign && parser->match(LITTOK_EQUAL))
            {
                return (Expression*)ExprAssign::make(parser->m_state, parser->m_prevtoken.line, expression,
                                                                    parse_expression(parser));
            }
            return expression;
        }

        Expression* Parser::parse_variable_expression(Parser* parser, bool can_assign)
        {
            return parse_variable_expression_base(parser, can_assign, false);
        }

        Expression* Parser::parse_new_expression(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            parser->consume(LITTOK_IDENTIFIER, "class name after 'new'");
            return parse_variable_expression_base(parser, false, true);
        }

        Expression* Parser::parse_dot(Parser* parser, Expression* previous, bool can_assign)
        {
            (void)can_assign;
            bool ignored;
            size_t line;
            size_t length;
            const char* name;
            line = parser->m_prevtoken.line;
            ignored = parser->m_prevtoken.type == LITTOK_SMALL_ARROW;
            if(!(parser->match(LITTOK_CLASS) || parser->match(LITTOK_SUPER)))
            {// class and super are allowed field names
                parser->consume(LITTOK_IDENTIFIER, ignored ? "propety name after '->'" : "property name after '.'");
            }
            name = parser->m_prevtoken.start;
            length = parser->m_prevtoken.length;
            if(!ignored && can_assign && parser->match(LITTOK_EQUAL))
            {
                return (Expression*)ExprIndexSet::make(parser->m_state, line, previous, name, length, parse_expression(parser));
            }
            auto expression = (Expression*)ExprIndexGet::make(parser->m_state, line, previous, name, length, false, ignored);
            if(!ignored && parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, expression, can_assign);
            }
            return expression;
        }

        Expression* Parser::parse_range(Parser* parser, Expression* previous, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            line = parser->m_prevtoken.line;
            return (Expression*)ExprRange::make(parser->m_state, line, previous, parse_expression(parser));
        }

        Expression* Parser::parse_ternary_or_question(Parser* parser, Expression* previous, bool can_assign)
        {
            (void)can_assign;
            bool ignored;
            size_t line;
            line = parser->m_prevtoken.line;
            if(parser->match(LITTOK_DOT) || parser->match(LITTOK_SMALL_ARROW))
            {
                ignored = parser->m_prevtoken.type == LITTOK_SMALL_ARROW;
                parser->consume(LITTOK_IDENTIFIER, ignored ? "property name after '->'" : "property name after '.'");
                return (Expression*)ExprIndexGet::make(parser->m_state, line, previous, parser->m_prevtoken.start,
                                                                 parser->m_prevtoken.length, true, ignored);
            }
            auto if_branch = parse_expression(parser);
            parser->consume(LITTOK_COLON, "':' after expression");
            auto else_branch = parse_expression(parser);
            return (Expression*)ExprIfClause::make(parser->m_state, line, previous, if_branch, else_branch);
        }

        Expression* Parser::parse_array(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            auto array = ExprArray::make(parser->m_state, parser->m_prevtoken.line);
            parser->ignore_new_lines();
            while(!parser->check(LITTOK_RIGHT_BRACKET))
            {
                parser->ignore_new_lines();
                array->values.push(parse_expression(parser));
                if(!parser->match(LITTOK_COMMA))
                {
                    break;
                }
            }
            parser->ignore_new_lines();
            parser->consume(LITTOK_RIGHT_BRACKET, "']' after array");
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, (Expression*)array, can_assign);
            }
            return (Expression*)array;
        }

        Expression* Parser::parse_subscript(Parser* parser, Expression* previous, bool can_assign)
        {
            size_t line;
            line = parser->m_prevtoken.line;
            auto index = parse_expression(parser);
            parser->consume(LITTOK_RIGHT_BRACKET, "']' after subscript");
            auto expression = (Expression*)ExprSubscript::make(parser->m_state, line, previous, index);
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, expression, can_assign);
            }
            else if(can_assign && parser->match(LITTOK_EQUAL))
            {
                return (Expression*)ExprAssign::make(parser->m_state, parser->m_prevtoken.line, expression, parse_expression(parser));
            }
            return expression;
        }

        Expression* Parser::parse_this(Parser* parser, bool can_assign)
        {
            auto expression = (Expression*)ExprThis::make(parser->m_state, parser->m_prevtoken.line);
            if(parser->match(LITTOK_LEFT_BRACKET))
            {
                return parse_subscript(parser, expression, can_assign);
            }
            return expression;
        }

        Expression* Parser::parse_super(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            bool ignoring;
            size_t line;
            Expression* expression;
            line = parser->m_prevtoken.line;
            if(!(parser->match(LITTOK_DOT) || parser->match(LITTOK_SMALL_ARROW)))
            {
                expression = (Expression*)ExprSuper::make(
                parser->m_state, line, String::copy(parser->m_state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1), false);
                parser->consume(LITTOK_LEFT_PAREN, "'(' after 'super'");
                return parse_call(parser, expression, false);
            }
            ignoring = parser->m_prevtoken.type == LITTOK_SMALL_ARROW;
            parser->consume(LITTOK_IDENTIFIER, ignoring ? "super method name after '->'" : "super method name after '.'");
            expression = (Expression*)ExprSuper::make(
            parser->m_state, line, String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length), ignoring);
            if(parser->match(LITTOK_LEFT_PAREN))
            {
                return parse_call(parser, expression, false);
            }
            return expression;
        }

        Expression* Parser::parse_reference(Parser* parser, bool can_assign)
        {
            (void)can_assign;
            size_t line;
            line = parser->m_prevtoken.line;
            parser->ignore_new_lines();
            auto expression = ExprReference::make(parser->m_state, line, parse_precedence(parser, LITPREC_CALL, false));
            if(parser->match(LITTOK_EQUAL))
            {
                return (Expression*)ExprAssign::make(parser->m_state, line, (Expression*)expression, parse_expression(parser));
            }
            return (Expression*)expression;
        }

        Expression* Parser::parse_expression(Parser* parser)
        {
            parser->ignore_new_lines();
            return parse_precedence(parser, LITPREC_ASSIGNMENT, true);
        }

        Expression* Parser::parse_var_declaration(Parser* parser)
        {
            bool constant;
            size_t line;
            size_t length;
            const char* name;
            Expression* vexpr;
            constant = parser->m_prevtoken.type == LITTOK_CONST;
            line = parser->m_prevtoken.line;
            parser->consume(LITTOK_IDENTIFIER, "variable name");
            name = parser->m_prevtoken.start;
            length = parser->m_prevtoken.length;
            vexpr = nullptr;
            if(parser->match(LITTOK_EQUAL))
            {
                vexpr = parse_expression(parser);
            }
            return (Expression*)StmtVar::make(parser->m_state, line, name, length, vexpr, constant);
        }

        Expression* Parser::parse_if(Parser* parser)
        {
            size_t line;
            bool invert;
            bool had_paren;
            Expression* condition;
            Expression* if_branch;
            Expression::List* elseif_conditions;
            Expression::List* elseif_branches;
            Expression* else_branch;
            Expression* e;
            line = parser->m_prevtoken.line;
            invert = parser->match(LITTOK_BANG);
            had_paren = parser->match(LITTOK_LEFT_PAREN);
            condition = parse_expression(parser);
            if(had_paren)
            {
                parser->consume(LITTOK_RIGHT_PAREN, "')'");
            }
            if(invert)
            {
                condition = (Expression*)ExprUnary::make(parser->m_state, condition->line, condition, LITTOK_BANG);
            }
            parser->ignore_new_lines();
            if_branch = parse_statement(parser);
            elseif_conditions = nullptr;
            elseif_branches = nullptr;
            else_branch = nullptr;
            parser->ignore_new_lines();
            while(parser->match(LITTOK_ELSE))
            {
                // else if
                parser->ignore_new_lines();
                if(parser->match(LITTOK_IF))
                {
                    if(elseif_conditions == nullptr)
                    {
                        elseif_conditions = Expression::makeList(parser->m_state);
                        elseif_branches = Expression::makeList(parser->m_state);
                    }
                    invert = parser->match(LITTOK_BANG);
                    had_paren = parser->match(LITTOK_LEFT_PAREN);
                    parser->ignore_new_lines();
                    e = parse_expression(parser);
                    if(had_paren)
                    {
                        parser->consume(LITTOK_RIGHT_PAREN, "')'");
                    }
                    parser->ignore_new_lines();
                    if(invert)
                    {
                        e = (Expression*)ExprUnary::make(parser->m_state, condition->line, e, LITTOK_BANG);
                    }
                    elseif_conditions->push(e);
                    elseif_branches->push(parse_statement(parser));
                    continue;
                }
                // else
                if(else_branch != nullptr)
                {
                    parser->raiseError(Error::LITERROR_MULTIPLE_ELSE_BRANCHES);
                }
                parser->ignore_new_lines();
                else_branch = parse_statement(parser);
            }
            return (Expression*)StmtIfClause::make(parser->m_state, line, condition, if_branch, else_branch, elseif_conditions, elseif_branches);
        }

        Expression* Parser::parse_for(Parser* parser)
        {

            bool c_style;
            bool had_paren;
            size_t line;
            Expression* condition;
            Expression* increment;
            Expression* var;
            Expression* exprinit;
            line= parser->m_prevtoken.line;
            had_paren = parser->match(LITTOK_LEFT_PAREN);
            var = nullptr;
            exprinit = nullptr;
            if(!parser->check(LITTOK_SEMICOLON))
            {
                if(parser->match(LITTOK_VAR))
                {
                    var = parse_var_declaration(parser);
                }
                else
                {
                    exprinit = parse_expression(parser);
                }
            }
            c_style = !parser->match(LITTOK_IN);
            condition= nullptr;
            increment = nullptr;
            if(c_style)
            {
                parser->consume(LITTOK_SEMICOLON, "';'");
                condition = parser->check(LITTOK_SEMICOLON) ? nullptr : parse_expression(parser);
                parser->consume(LITTOK_SEMICOLON, "';'");
                increment = parser->check(LITTOK_RIGHT_PAREN) ? nullptr : parse_expression(parser);
            }
            else
            {
                condition = parse_expression(parser);
                if(var == nullptr)
                {
                    parser->raiseError(Error::LITERROR_VAR_MISSING_IN_FORIN);
                }
            }
            if(had_paren)
            {
                parser->consume(LITTOK_RIGHT_PAREN, "')'");
            }
            parser->ignore_new_lines();
            return (Expression*)StmtForLoop::make(parser->m_state, line, exprinit, var, condition, increment,
                                                           parse_statement(parser), c_style);
        }

        Expression* Parser::parse_while(Parser* parser)
        {
            bool had_paren;
            size_t line;
            Expression* body;
            line = parser->m_prevtoken.line;
            had_paren = parser->match(LITTOK_LEFT_PAREN);
            Expression* condition = parse_expression(parser);
            if(had_paren)
            {
                parser->consume(LITTOK_RIGHT_PAREN, "')'");
            }
            parser->ignore_new_lines();
            body = parse_statement(parser);
            return (Expression*)StmtWhileLoop::make(parser->m_state, line, condition, body);
        }

        Expression* Parser::parse_function(Parser* parser)
        {
            size_t line;
            size_t function_length;
            bool isexport;
            const char* function_name;
            Compiler compiler;
            StmtFunction* function;
            ExprLambda* lambda;
            ExprIndexSet* to;
            isexport = parser->m_prevtoken.type == LITTOK_EXPORT;
            if(isexport)
            {
                parser->consume(LITTOK_FUNCTION, "'function' after 'export'");
            }
            line = parser->m_prevtoken.line;
            parser->consume(LITTOK_IDENTIFIER, "function name");
            function_name = parser->m_prevtoken.start;
            function_length = parser->m_prevtoken.length;
            if(parser->match(LITTOK_DOT))
            {
                parser->consume(LITTOK_IDENTIFIER, "function name");
                lambda = ExprLambda::make(parser->m_state, line);
                to = ExprIndexSet::make(
                parser->m_state, line, (Expression*)ExprVar::make(parser->m_state, line, function_name, function_length),
                parser->m_prevtoken.start, parser->m_prevtoken.length, (Expression*)lambda);
                parser->consume(LITTOK_LEFT_PAREN, "'(' after function name");
                compiler.init(parser);
                parser->begin_scope();
                parse_parameters(parser, &lambda->parameters);
                if(lambda->parameters.m_count > 255)
                {
                    parser->raiseError(Error::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)lambda->parameters.m_count);
                }
                parser->consume(LITTOK_RIGHT_PAREN, "')' after function arguments");
                parser->ignore_new_lines();
                lambda->body = parse_statement(parser);
                parser->end_scope();
                parser->end_compiler(&compiler);
                return (Expression*)ExprStatement::make(parser->m_state, line, (Expression*)to);
            }
            function = StmtFunction::make(parser->m_state, line, function_name, function_length);
            function->exported = isexport;
            parser->consume(LITTOK_LEFT_PAREN, "'(' after function name");
            compiler.init(parser);
            parser->begin_scope();
            parse_parameters(parser, &function->parameters);
            if(function->parameters.m_count > 255)
            {
                parser->raiseError(Error::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)function->parameters.m_count);
            }
            parser->consume(LITTOK_RIGHT_PAREN, "')' after function arguments");
            function->body = parse_statement(parser);
            parser->end_scope();
            parser->end_compiler(&compiler);
            return (Expression*)function;
        }

        Expression* Parser::parse_return(Parser* parser)
        {
            size_t line;
            Expression* expression;
            line = parser->m_prevtoken.line;
            expression = nullptr;
            if(!parser->check(LITTOK_NEW_LINE) && !parser->check(LITTOK_RIGHT_BRACE))
            {
                expression = parse_expression(parser);
            }
            return (Expression*)StmtReturn::make(parser->m_state, line, expression);
        }

        Expression* Parser::parse_field(Parser* parser, String* name, bool is_static)
        {
            size_t line;
            Expression* getter;
            Expression* setter;
            line = parser->m_prevtoken.line;
            getter = nullptr;
            setter = nullptr;
            if(parser->match(LITTOK_ARROW))
            {
                getter = parse_statement(parser);
            }
            else
            {
                parser->match(LITTOK_LEFT_BRACE);// Will be LITTOK_LEFT_BRACE, otherwise this method won't be called
                parser->ignore_new_lines();
                if(parser->match(LITTOK_GET))
                {
                    parser->match(LITTOK_ARROW);// Ignore it if it's present
                    getter = parse_statement(parser);
                }
                parser->ignore_new_lines();
                if(parser->match(LITTOK_SET))
                {
                    parser->match(LITTOK_ARROW);// Ignore it if it's present
                    setter = parse_statement(parser);
                }
                if(getter == nullptr && setter == nullptr)
                {
                    parser->raiseError(Error::LITERROR_NO_GETTER_AND_SETTER);
                }
                parser->ignore_new_lines();
                parser->consume(LITTOK_RIGHT_BRACE, "'}' after field declaration");
            }
            return (Expression*)StmtField::make(parser->m_state, line, name, getter, setter, is_static);
        }

        Expression* Parser::parse_method(Parser* parser, bool is_static)
        {
            static TokenType operators[]=
            {
                LITTOK_PLUS, LITTOK_MINUS, LITTOK_STAR, LITTOK_PERCENT, LITTOK_SLASH,
                LITTOK_SHARP, LITTOK_BANG, LITTOK_LESS, LITTOK_LESS_EQUAL, LITTOK_GREATER,
                LITTOK_GREATER_EQUAL, LITTOK_EQUAL_EQUAL, LITTOK_LEFT_BRACKET, LITTOK_EOF
            };
            size_t i;
            Compiler compiler;
            StmtMethod* method;
            String* name;
            if(parser->match(LITTOK_STATIC))
            {
                is_static = true;
            }
            name = nullptr;
            parser->consume(LITTOK_FUNCTION, "expected 'function'");
            if(parser->match(LITTOK_OPERATOR))
            {
                if(is_static)
                {
                    parser->raiseError(Error::LITERROR_STATIC_OPERATOR);
                }
                i = 0;
                while(operators[i] != LITTOK_EOF)
                {
                    if(parser->match(operators[i]))
                    {
                        break;
                    }
                    i++;
                }
                if(parser->m_prevtoken.type == LITTOK_LEFT_BRACKET)
                {
                    parser->consume(LITTOK_RIGHT_BRACKET, "']' after '[' in op method declaration");
                    name = String::copy(parser->m_state, "[]", 2);
                }
                else
                {
                    name = String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length);
                }
            }
            else
            {
                parser->consume(LITTOK_IDENTIFIER, "method name");
                name = String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length);
                if(parser->check(LITTOK_LEFT_BRACE) || parser->check(LITTOK_ARROW))
                {
                    return parse_field(parser, name, is_static);
                }
            }
            method = StmtMethod::make(parser->m_state, parser->m_prevtoken.line, name, is_static);
            compiler.init(parser);
            parser->begin_scope();
            parser->consume(LITTOK_LEFT_PAREN, "'(' after method name");
            parse_parameters(parser, &method->parameters);
            if(method->parameters.m_count > 255)
            {
                parser->raiseError(Error::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)method->parameters.m_count);
            }
            parser->consume(LITTOK_RIGHT_PAREN, "')' after method arguments");
            method->body = parse_statement(parser);
            parser->end_scope();
            parser->end_compiler(&compiler);
            return (Expression*)method;
        }

        Expression* Parser::parse_class(Parser* parser)
        {
            bool finished_parsing_fields;
            bool field_is_static;
            size_t line;
            bool is_static;
            String* name;
            String* super;
            StmtClass* klass;
            Expression* var;
            Expression* method;
            if(setjmp(parser->m_jumpbuffer))
            {
                return nullptr;
            }
            line = parser->m_prevtoken.line;
            is_static = parser->m_prevtoken.type == LITTOK_STATIC;
            if(is_static)
            {
                parser->consume(LITTOK_CLASS, "'class' after 'static'");
            }
            parser->consume(LITTOK_IDENTIFIER, "class name after 'class'");
            name = String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length);
            super = nullptr;
            if(parser->match(LITTOK_COLON))
            {
                parser->consume(LITTOK_IDENTIFIER, "super class name after ':'");
                super = String::copy(parser->m_state, parser->m_prevtoken.start, parser->m_prevtoken.length);
                if(super == name)
                {
                    parser->raiseError(Error::LITERROR_SELF_INHERITED_CLASS);
                }
            }
            klass = StmtClass::make(parser->m_state, line, name, super);
            parser->ignore_new_lines();
            parser->consume(LITTOK_LEFT_BRACE, "'{' before class body");
            parser->ignore_new_lines();
            finished_parsing_fields = false;
            while(!parser->check(LITTOK_RIGHT_BRACE))
            {
                field_is_static = false;
                if(parser->match(LITTOK_STATIC))
                {
                    field_is_static = true;
                    if(parser->match(LITTOK_VAR))
                    {
                        if(finished_parsing_fields)
                        {
                            parser->raiseError(Error::LITERROR_STATIC_FIELDS_AFTER_METHODS);
                        }
                        var = parse_var_declaration(parser);
                        if(var != nullptr)
                        {
                            klass->fields.push(var);
                        }
                        parser->ignore_new_lines();
                        continue;
                    }
                    else
                    {
                        finished_parsing_fields = true;
                    }
                }
                method = parse_method(parser, is_static || field_is_static);
                if(method != nullptr)
                {
                    klass->fields.push(method);
                }
                parser->ignore_new_lines();
            }
            parser->consume(LITTOK_RIGHT_BRACE, "'}' after class body");
            return (Expression*)klass;
        }

        Expression* Parser::parse_statement(Parser* parser)
        {
            Expression* expression;
            parser->ignore_new_lines();
            if(setjmp(parser->m_jumpbuffer))
            {
                return nullptr;
            }
            if(parser->match(LITTOK_VAR) || parser->match(LITTOK_CONST))
            {
                return parse_var_declaration(parser);
            }
            else if(parser->match(LITTOK_IF))
            {
                return parse_if(parser);
            }
            else if(parser->match(LITTOK_FOR))
            {
                return parse_for(parser);
            }
            else if(parser->match(LITTOK_WHILE))
            {
                return parse_while(parser);
            }
            else if(parser->match(LITTOK_CONTINUE))
            {
                return (Expression*)StmtContinue::make(parser->m_state, parser->m_prevtoken.line);
            }
            else if(parser->match(LITTOK_BREAK))
            {
                return (Expression*)StmtBreak::make(parser->m_state, parser->m_prevtoken.line);
            }
            else if(parser->match(LITTOK_FUNCTION) || parser->match(LITTOK_EXPORT))
            {
                return parse_function(parser);
            }
            else if(parser->match(LITTOK_RETURN))
            {
                return parse_return(parser);
            }
            else if(parser->match(LITTOK_LEFT_BRACE))
            {
                return parse_block(parser);
            }
            else if(parser->match(LITTOK_SEMICOLON))
            {
                return parse_statement(parser);
            }
            expression = parse_expression(parser);
            return expression == nullptr ? nullptr : (Expression*)ExprStatement::make(parser->m_state, parser->m_prevtoken.line, expression);
        }

        Expression* Parser::parse_declaration(Parser* parser)
        {
            Expression* statement;
            statement = nullptr;
            if(parser->match(LITTOK_CLASS) || parser->match(LITTOK_STATIC))
            {
                statement = parse_class(parser);
            }
            else
            {
                statement = parse_statement(parser);
            }
            return statement;
        }


    }
}


