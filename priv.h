

#include "lit.h"

namespace lit
{
    namespace AST
    {
        struct Token
        {
            public:
                static const char* token_name(int t);

            public:
                const char* start;
                TokenType type;
                size_t length;
                size_t line;
                Value value;
        };

        class SyntaxNode
        {
            public:
                State* m_state;
                size_t line = 0;
        };

        class Expression: public SyntaxNode
        {
            public:
                enum class Type
                {
                    Unspecified,
                    Literal,
                    Binary,
                    Unary,
                    Variable,
                    Assign,
                    Call,
                    Set,
                    Get,
                    Lambda,
                    Array,
                    Object,
                    Subscript,
                    This,
                    Super,
                    Range,
                    Interpolation,
                    Reference,
                    Expression,
                    Block,
                    IfClause,
                    WhileLoop,
                    ForLoop,
                    VarDecl,
                    ContinueClause,
                    BreakClause,
                    FunctionDecl,
                    ReturnClause,
                    MethodDecl,
                    ClassDecl,
                    FieldDecl
                };

                using List = PCGenericArray<Expression*>;

            public:
                template<typename ClassT>
                static ClassT* make(State* state, uint64_t line, Type type)
                {
                    auto expr = Memory::allocate<ClassT>(state);
                    expr->m_state = state;
                    expr->type = type;
                    expr->line = line;
                    return expr;
                }

                static List* makeList(State* state)
                {
                    auto expressions = Memory::allocate<List>(state);
                    expressions->init(state);
                    return expressions;
                }

                template<typename ExprT>
                static void releaseParameters(State* state, PCGenericArray<ExprT>* parameters)
                {
                    for(size_t i = 0; i < parameters->m_count; i++)
                    {
                        Expression::releaseExpression(state, parameters->m_values[i].default_value);
                    }
                    parameters->release();
                }

                static void releaseExpressionList(State* state, Expression::List* expressions);

                static void releaseStatementList(State* state, Expression::List* statements);

                static void internalReleaseStatementList(State* state, Expression::List* statements);

                static void releaseAllocatedExpressionList(State* state, Expression::List* expressions);

                static void releaseAllocatedStatementList(State* state, Expression::List* statements);

                static void releaseExpression(State* state, Expression* expression);
                static void releaseStatement(State* state, Expression* statement);

            public:
                Type type = Type::Unspecified;
        };

        struct Variable
        {
            const char* name;
            size_t length;
            int depth;
            bool constant;
            bool used;
            Value constant_value;
            Expression** declaration;
        };

        class ParseRule
        {
            public:
                using PrefixFuncType = Expression* (*)(Parser*, bool);
                using InfixFuncType = Expression*(*)(Parser*, Expression*, bool);

            public:
                PrefixFuncType prefix;
                InfixFuncType infix;
                Precedence precedence;
        };

        class ExprLiteral: public Expression
        {
            public:
                static ExprLiteral* make(State* state, size_t line, Value value)
                {
                    auto rt = Expression::make<ExprLiteral>(state, line, Expression::Type::Literal);
                    rt->value = value;
                    return rt;
                }

            public:
                Value value;
        };

        class ExprBinary: public Expression
        {
            public:
                static ExprBinary* make(State* state, size_t line, Expression* left, Expression* right, TokenType op)
                {
                    auto expression = Expression::make<ExprBinary>(state, line, Expression::Type::Binary);
                    expression->left = left;
                    expression->right = right;
                    expression->op = op;
                    expression->ignore_left = false;
                    return expression;
                }

            public:
                Expression* left;
                Expression* right;
                TokenType op;
                bool ignore_left;
        };

        class ExprUnary: public Expression
        {
            public:
                static ExprUnary* make(State* state, size_t line, Expression* right, TokenType op)
                {
                    auto expression = Expression::make<ExprUnary>(state, line, Expression::Type::Unary);
                    expression->right = right;
                    expression->op = op;
                    return expression;
                }

            public:
                Expression* right;
                TokenType op;
        };

        class ExprVar: public Expression
        {
            public:
                static ExprVar* make(State* state, size_t line, const char* name, size_t length)
                {
                    auto expression = Expression::make<ExprVar>(state, line, Expression::Type::Variable);
                    expression->name = name;
                    expression->length = length;
                    return expression;
                }

            public:
                const char* name;
                size_t length;
        };

        class ExprAssign: public Expression
        {
            public:
                static ExprAssign* make(State* state, size_t line, Expression* to, Expression* value)
                {
                    auto expression = Expression::make<ExprAssign>(state, line, Expression::Type::Assign);
                    expression->to = to;
                    expression->value = value;
                    return expression;
                }

            public:
                Expression* to;
                Expression* value;
        };

        class ExprCall: public Expression
        {
            public:
                static ExprCall* make(State* state, size_t line, Expression* callee)
                {
                    auto expression = Expression::make<ExprCall>(state, line, Expression::Type::Call);
                    expression->callee = callee;
                    expression->objexpr = nullptr;
                    expression->args.init(state);
                    return expression;
                }

            public:
                Expression* callee;
                Expression::List args;
                Expression* objexpr;
        };

        class ExprIndexGet: public Expression
        {
            public:
               static ExprIndexGet* make(State* state, size_t line, Expression* where, const char* name, size_t length, bool questionable, bool ignore_result)
                {
                    auto expression = Expression::make<ExprIndexGet>(state, line, Expression::Type::Get);
                    expression->where = where;
                    expression->name = name;
                    expression->length = length;
                    expression->ignore_emit = false;
                    expression->jump = questionable ? 0 : -1;
                    expression->ignore_result = ignore_result;
                    return expression;
                }

            public:
                Expression* where;
                const char* name;
                size_t length;
                int jump;
                bool ignore_emit;
                bool ignore_result;
        };

        class ExprIndexSet: public Expression
        {
            public:
                static ExprIndexSet* make(State* state, size_t line, Expression* where, const char* name, size_t length, Expression* value)
                {
                    auto expression = Expression::make<ExprIndexSet>(state, line, Expression::Type::Set);
                    expression->where = where;
                    expression->name = name;
                    expression->length = length;
                    expression->value = value;
                    return expression;
                }

            public:
                Expression* where;
                const char* name;
                size_t length;
                Expression* value;
        };

        struct ExprFuncParam
        {
            const char* name;
            size_t length;
            Expression* default_value;
        };

        class ExprLambda: public Expression
        {
            public:
                static ExprLambda* make(State* state, size_t line)
                {
                    auto expression = Expression::make<ExprLambda>(state, line, Expression::Type::Lambda);
                    expression->body = nullptr;
                    expression->parameters.init(state);
                    return expression;
                }

            public:
                PCGenericArray<ExprFuncParam> parameters;
                Expression* body;
        };

        class ExprArray: public Expression
        {
            public:
                static ExprArray* make(State* state, size_t line)
                {
                    auto expression = Expression::make<ExprArray>(state, line, Expression::Type::Array);
                    expression->values.init(state);
                    return expression;
                }

            public:
                Expression::List values;
        };

        class ExprObject: public Expression
        {
            public:
                static ExprObject* make(State* state, size_t line)
                {
                    auto expression = Expression::make<ExprObject>(state, line, Expression::Type::Object);
                    expression->keys.init(state);
                    expression->values.init(state);
                    return expression;
                }

            public:
                PCGenericArray<Value> keys;
                Expression::List values;
        };

        class ExprSubscript: public Expression
        {
            public:
                static ExprSubscript* make(State* state, size_t line, Expression* array, Expression* index)
                {
                    auto expression = Expression::make<ExprSubscript>(state, line, Expression::Type::Subscript);
                    expression->array = array;
                    expression->index = index;
                    return expression;
                }

            public:
                Expression* array;
                Expression* index;
        };

        class ExprThis: public Expression
        {
            public:
                static ExprThis* make(State* state, size_t line)
                {
                    return Expression::make<ExprThis>(state, line, Expression::Type::This);
                }
        };

        class ExprSuper: public Expression
        {
            public:
                static ExprSuper* make(State* state, size_t line, String* method, bool ignore_result)
                {
                    auto expression = Expression::make<ExprSuper>(state, line, Expression::Type::Super);
                    expression->method = method;
                    expression->ignore_emit = false;
                    expression->ignore_result = ignore_result;
                    return expression;
                }

            public:
                String* method;
                bool ignore_emit;
                bool ignore_result;
        };

        class ExprRange: public Expression
        {
            public:
                static ExprRange* make(State* state, size_t line, Expression* from, Expression* to)
                {
                    auto expression = Expression::make<ExprRange>(state, line, Expression::Type::Range);
                    expression->from = from;
                    expression->to = to;
                    return expression;
                }

            public:
                Expression* from;
                Expression* to;
        };

        class ExprIfClause: public Expression
        {
            public:
                static ExprIfClause* make(State* state, size_t line, Expression* condition, Expression* if_branch, Expression* else_branch)
                {
                    auto expression = Expression::make<ExprIfClause>(state, line, Expression::Type::IfClause);
                    expression->condition = condition;
                    expression->if_branch = if_branch;
                    expression->else_branch = else_branch;
                    return expression;
                }

            public:
                Expression* condition;
                Expression* if_branch;
                Expression* else_branch;
        };

        class ExprInterpolation: public Expression
        {
            public:
                static ExprInterpolation* make(State* state, size_t line)
                {
                    auto expression = Expression::make<ExprInterpolation>(state, line, Expression::Type::Interpolation);
                    expression->expressions.init(state);
                    return expression;
                }

            public:
                Expression::List expressions;
        };

        class ExprReference: public Expression
        {
            public:
                static ExprReference* make(State* state, size_t line, Expression* to)
                {
                    auto expr = Expression::make<ExprReference>(state, line, Expression::Type::Reference);
                    expr->to = to;
                    return expr;
                }

            public:
                Expression* to;
        };

        class ExprStatement: public Expression
        {
            public:
                static ExprStatement* make(State* state, size_t line, Expression* expression)
                {
                    auto statement = Expression::make<ExprStatement>(state, line, Expression::Type::Expression);
                    statement->expression = expression;
                    statement->pop = true;
                    return statement;
                }

            public:
                Expression* expression;
                bool pop;
        };

        class StmtBlock: public Expression
        {
            public:
                static StmtBlock* make(State* state, size_t line)
                {
                    auto statement = Expression::make<StmtBlock>(state, line, Expression::Type::Block);
                    statement->statements.init(state);
                    return statement;
                }

            public:
                Expression::List statements;
        };

        class StmtVar: public Expression
        {
            public:
                static StmtVar* make(State* state, size_t line, const char* name, size_t length, Expression* exprinit, bool constant)
                {
                    auto statement = Expression::make<StmtVar>(state, line, Expression::Type::VarDecl);
                    statement->name = name;
                    statement->length = length;
                    statement->valexpr = exprinit;
                    statement->constant = constant;
                    return statement;
                }

            public:
                const char* name;
                size_t length;
                bool constant;
                Expression* valexpr;
        };

        class StmtIfClause: public Expression
        {
            public:
                static StmtIfClause* make(State* state,
                                                        size_t line,
                                                        Expression* condition,
                                                        Expression* if_branch,
                                                        Expression* else_branch,
                                                        Expression::List* elseif_conditions,
                                                        Expression::List* elseif_branches)
                {
                    auto statement = Expression::make<StmtIfClause>(state, line, Expression::Type::IfClause);
                    statement->condition = condition;
                    statement->if_branch = if_branch;
                    statement->else_branch = else_branch;
                    statement->elseif_conditions = elseif_conditions;
                    statement->elseif_branches = elseif_branches;
                    return statement;
                }

            public:
                Expression* condition;
                Expression* if_branch;
                Expression* else_branch;
                Expression::List* elseif_conditions;
                Expression::List* elseif_branches;
        };

        class StmtWhileLoop: public Expression
        {
            public:
                static StmtWhileLoop* make(State* state, size_t line, Expression* condition, Expression* body)
                {
                    auto statement = Expression::make<StmtWhileLoop>(state, line, Expression::Type::WhileLoop);
                    statement->condition = condition;
                    statement->body = body;
                    return statement;
                }

            public:
                Expression* condition;
                Expression* body;
        };

        class StmtForLoop: public Expression
        {
            public:
                static StmtForLoop* make(State* state,
                                                          size_t line,
                                                          Expression* exprinit,
                                                          Expression* var,
                                                          Expression* condition,
                                                          Expression* increment,
                                                          Expression* body,
                                                          bool c_style)
                {
                    auto statement = Expression::make<StmtForLoop>(state, line, Expression::Type::ForLoop);
                    statement->exprinit = exprinit;
                    statement->var = var;
                    statement->condition = condition;
                    statement->increment = increment;
                    statement->body = body;
                    statement->c_style = c_style;
                    return statement;
                }

            public:
                Expression* exprinit;
                Expression* var;
                Expression* condition;
                Expression* increment;
                Expression* body;
                bool c_style;
        };

        class StmtContinue: public Expression
        {
            public:
                static StmtContinue* make(State* state, size_t line)
                {
                    return Expression::make<StmtContinue>(state, line, Expression::Type::ContinueClause);
                }

        };

        class StmtBreak: public Expression
        {
            public:
                static StmtBreak* make(State* state, size_t line)
                {
                    return Expression::make<StmtBreak>(state, line, Expression::Type::BreakClause);
                }
        };

        class StmtFunction: public Expression
        {
            public:
                static StmtFunction* make(State* state, size_t line, const char* name, size_t length)
                {
                    auto function = Expression::make<StmtFunction>(state, line, Expression::Type::FunctionDecl);
                    function->name = name;
                    function->length = length;
                    function->body = nullptr;
                    function->parameters.init(state);
                    return function;
                }

            public:
                const char* name;
                size_t length;
                PCGenericArray<ExprFuncParam> parameters;
                Expression* body;
                bool exported;
        };

        class StmtReturn: public Expression
        {
            public:
                static StmtReturn* make(State* state, size_t line, Expression* expression)
                {
                    auto statement = Expression::make<StmtReturn>(state, line, Expression::Type::ReturnClause);
                    statement->expression = expression;
                    return statement;
                }

            public:
                Expression* expression;
        };

        class StmtMethod: public Expression
        {
            public:
                static StmtMethod* make(State* state, size_t line, String* name, bool is_static)
                {
                    auto statement = Expression::make<StmtMethod>(state, line, Expression::Type::MethodDecl);
                    statement->name = name;
                    statement->body = nullptr;
                    statement->is_static = is_static;
                    statement->parameters.init(state);
                    return statement;
                }

            public:
                String* name;
                PCGenericArray<ExprFuncParam> parameters;
                Expression* body;
                bool is_static;
        };

        class StmtClass: public Expression
        {
            public:
                static StmtClass* make(State* state, size_t line, String* name, String* parent)
                {
                    auto statement = Expression::make<StmtClass>(state, line, Expression::Type::ClassDecl);
                    statement->name = name;
                    statement->parent = parent;
                    statement->fields.init(state);
                    return statement;
                }

            public:
                String* name;
                String* parent;
                Expression::List fields;
        };

        class StmtField: public Expression
        {
            public:
                static StmtField* make(State* state, size_t line, String* name, Expression* getter, Expression* setter, bool is_static)
                {
                    auto statement = Expression::make<StmtField>(state, line, Expression::Type::FieldDecl);
                    statement->name = name;
                    statement->getter = getter;
                    statement->setter = setter;
                    statement->is_static = is_static;
                    return statement;
                }

            public:
                String* name;
                Expression* getter;
                Expression* setter;
                bool is_static;
        };

        class Scanner
        {
            public:
                static inline bool char_is_digit(char c)
                {
                    return c >= '0' && c <= '9';
                }

                static inline bool char_is_alpha(char c)
                {
                    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
                }

            public:
                size_t m_length;
                size_t m_line;
                const char* m_startsrc;
                const char* m_currsrc;
                const char* m_filename;
                State* m_state;
                size_t m_braces[LIT_MAX_INTERPOLATION_NESTING];
                size_t m_numbraces;
                bool m_haderror;

            public:
                void init(State* state, const char* fname, const char* source, size_t len);
                Token make_token(TokenType type);
                Token make_error_token(Error error, ...);
                bool is_at_end();
                char advance();
                bool match(char expected);
                Token match_token(char c, TokenType a, TokenType b);
                Token match_tokens(char cr, char cb, TokenType a, TokenType b, TokenType c);
                char peek();
                char peek_next();

                bool skip_whitespace();
                Token scan_string(char stype, bool interpolation);
                int scan_hexdigit();
                int scan_binarydigit();
                Token make_number_token(bool is_hex, bool is_binary);
                Token scan_number();
                TokenType check_keyword(int start, int length, const char* rest, TokenType type);
                TokenType scan_identtype();
                Token scan_identifier();
                Token scan_token();
        };

        class Compiler
        {
            public:
                struct CCUpvalue
                {
                    uint8_t index;
                    bool isLocal;
                };

            public:
                PCGenericArray<Local> locals;
                int scope_depth;
                Function* function;
                FunctionType type;
                CCUpvalue upvalues[UINT8_COUNT];
                Compiler* enclosing;
                bool skip_return;
                size_t loop_depth;
                int slots;
                int max_slots;

            public:
                void init(Parser* parser);
        };

        class Preprocessor
        {
            public:
                State* m_state;
                Table defined;
                /*
                 * A little bit dirty hack:
                 * We need to store pointers (8 bytes in size),
                 * and so that we don't have to declare a new
                 * array type, that we will use only once,
                 * I'm using ValueList here, because Value
                 * also has the same size (8 bytes)
                 */
                PCGenericArray<Value> open_ifs;

            public:
                void init(State* state)
                {
                    m_state = state;
                    this->defined.init(state);
                    this->open_ifs.init(state);
                }

                void release()
                {
                    this->defined.release();
                    this->open_ifs.release();
                }
        };

        class Parser
        {
            public:

            #include "parse.h"

            // parservars
            public:
                State* m_state;
                bool m_haderror;
                bool m_panicmode;
                Token m_prevtoken;
                Token m_currtoken;
                Compiler* m_compiler;
                jmp_buf m_jumpbuffer;

            public:
                static ParseRule rules[LITTOK_EOF + 1];

            private:
                Scanner* getStateScanner(State* state);

                static void setup_rules()
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

            public:
                void init(State* state);

                void release()
                {
                }

                void end_compiler(Compiler* compiler)
                {
                    m_compiler = (Compiler*)compiler->enclosing;
                }

                void begin_scope()
                {
                    m_compiler->scope_depth++;
                }

                void end_scope()
                {
                    m_compiler->scope_depth--;
                }

                ParseRule* get_rule(TokenType type)
                {
                    return &Parser::rules[type];
                }

                inline bool is_at_end()
                {
                    return m_currtoken.type == LITTOK_EOF;
                }

                //todo
                void stringError(Token* token, const char* message);

                void errorAt(Token* token, Error error, va_list args)
                {
                    stringError(token, lit_vformat_error(m_state, token->line, error, args)->data());
                }

                void errorAtCurrent(Error error, ...)
                {
                    va_list args;
                    va_start(args, error);
                    errorAt(&m_currtoken, error, args);
                    va_end(args);
                }

                void raiseError(Error error, ...)
                {
                    va_list args;
                    va_start(args, error);
                    errorAt(&m_prevtoken, error, args);
                    va_end(args);
                }

                void advance()
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

                bool check(TokenType type)
                {
                    return m_currtoken.type == type;
                }

                bool match(TokenType type)
                {
                    if(m_currtoken.type == type)
                    {
                        advance();
                        return true;
                    }
                    return false;
                }

                bool match_new_line()
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

                void ignore_new_lines()
                {
                    match_new_line();
                }

                void consume(TokenType type, const char* error)
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

                void sync()
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

                bool parse(const char* filename, const char* source, size_t length, Expression::List& statements)
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

        };


        class Optimizer
        {
            #define LIT_DEBUG_OPTIMIZER

            #define optc_do_binary_op(op) \
                if(Object::isNumber(a) && Object::isNumber(b)) \
                { \
                    optdbg("translating constant binary expression of '" # op "' to constant value"); \
                    return Object::toValue(Object::toNumber(a) op Object::toNumber(b)); \
                } \
                return Object::NullVal;

            #define optc_do_bitwise_op(op, typ) \
                if(Object::isNumber(a) && Object::isNumber(b)) \
                { \
                    optdbg("translating constant bitwise expression of '" #op "' to constant value"); \
                    return Object::toValue(typ(Object::toNumber(a)) op typ(Object::toNumber(b))); \
                } \
                return Object::NullVal;

            #define optc_do_fn_op(fn, tokstr) \
                if(Object::isNumber(a) && Object::isNumber(b)) \
                { \
                    optdbg("translating constant expression of '" tokstr "' to constant value via call to '" #fn "'"); \
                    return Object::toValue(fn(Object::toNumber(a), Object::toNumber(b))); \
                } \
                return Object::NullVal;

            public:
                static void setup_optimization_states()
                {
                    Optimizer::set_level(LITOPTLEVEL_DEBUG);
                }

                static bool is_enabled(Optimization optimization);

                static void set_enabled(Optimization optimization, bool enabled);

                static void set_all_enabled(bool enabled);

                static void set_level(OptimizationLevel level)
                {
                    switch(level)
                    {
                        case LITOPTLEVEL_NONE:
                            {
                                Optimizer::set_all_enabled(false);
                            }
                            break;
                        case LITOPTLEVEL_REPL:
                            {
                                Optimizer::set_all_enabled(true);
                                Optimizer::set_enabled(LITOPTSTATE_UNUSED_VAR, false);
                                Optimizer::set_enabled(LITOPTSTATE_UNREACHABLE_CODE, false);
                                Optimizer::set_enabled(LITOPTSTATE_EMPTY_BODY, false);
                                Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                                Optimizer::set_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
                            }
                            break;
                        case LITOPTLEVEL_DEBUG:
                            {
                                Optimizer::set_all_enabled(true);
                                Optimizer::set_enabled(LITOPTSTATE_UNUSED_VAR, false);
                                Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                                Optimizer::set_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
                            }
                            break;
                        case LITOPTLEVEL_RELEASE:
                            {
                                Optimizer::set_all_enabled(true);
                                Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                            }
                            break;
                        case LITOPTLEVEL_EXTREME:
                            {
                                Optimizer::set_all_enabled(true);
                            }
                            break;
                        case LITOPTLEVEL_TOTAL:
                            {
                            }
                            break;

                    }
                }

                static const char* get_optimization_name(Optimization optimization);
                static const char* get_optimization_description(Optimization optimization);
                static const char* get_optimization_level_description(OptimizationLevel level);

            public:
                State* m_state;
                PCGenericArray<Variable> m_variables;
                int m_depth;
                bool m_markused;
                size_t m_optcount = 0;

            public:
                void init(State* state)
                {
                    m_state = state;
                    m_depth = -1;
                    m_markused = false;
                    m_variables.init(state);
                }

                bool is_empty(Expression* statement)
                {
                    return statement == nullptr || (statement->type == Expression::Type::Block && ((StmtBlock*)statement)->statements.m_count == 0);
                }

                Value evaluate_unary_op(Value value, TokenType op)
                {
                    switch(op)
                    {
                        case LITTOK_MINUS:
                            {
                                if(Object::isNumber(value))
                                {
                                    m_optcount++;
                                    optdbg("translating constant unary minus on number to literal value");
                                    return Object::toValue(-Object::toNumber(value));
                                }
                            }
                            break;
                        case LITTOK_BANG:
                            {
                                m_optcount++;
                                optdbg("translating constant expression of '=' to literal value");
                                return Object::fromBool(Object::isFalsey(value));
                            }
                            break;
                        case LITTOK_TILDE:
                            {
                                if(Object::isNumber(value))
                                {
                                    m_optcount++;
                                    optdbg("translating unary tile (~) on number to literal value");
                                    return Object::toValue(~((int)Object::toNumber(value)));
                                }
                            }
                            break;
                        default:
                            {
                            }
                            break;
                    }
                    return Object::NullVal;
                }

                Value evaluate_binary_op(Value a, Value b, TokenType op)
                {
                    switch(op)
                    {
                        case LITTOK_PLUS:
                            {
                                optc_do_binary_op(+);
                            }
                            break;
                        case LITTOK_MINUS:
                            {
                                optc_do_binary_op(-);
                            }
                            break;
                        case LITTOK_STAR:
                            {
                                optc_do_binary_op(*);
                            }
                            break;
                        case LITTOK_SLASH:
                            {
                                optc_do_binary_op(/);
                            }
                            break;
                        case LITTOK_STAR_STAR:
                            {
                                optc_do_fn_op(pow, "**");
                            }
                            break;
                        case LITTOK_PERCENT:
                            {
                                optc_do_fn_op(fmod, "%");
                            }
                            break;
                        case LITTOK_GREATER:
                            {
                                optc_do_binary_op(>);
                            }
                            break;
                        case LITTOK_GREATER_EQUAL:
                            {
                                optc_do_binary_op(>=);
                            }
                            break;
                        case LITTOK_LESS:
                            {
                                optc_do_binary_op(<);
                            }
                            break;
                        case LITTOK_LESS_EQUAL:
                            {
                                optc_do_binary_op(<=);
                            }
                            break;
                        case LITTOK_LESS_LESS:
                            {
                                optc_do_bitwise_op(<<, int64_t);
                            }
                            break;
                        case LITTOK_GREATER_GREATER:
                            {
                                optc_do_bitwise_op(>>, int64_t);
                            }
                            break;
                        case LITTOK_BAR:
                            {
                                optc_do_bitwise_op(|, int64_t);
                            }
                            break;
                        case LITTOK_AMPERSAND:
                            {
                                optc_do_bitwise_op(&, int64_t);
                            }
                            break;
                        case LITTOK_CARET:
                            {
                                optc_do_bitwise_op(^, int64_t);
                            }
                            break;
                        case LITTOK_SHARP:
                            {
                                if(Object::isNumber(a) && Object::isNumber(b))
                                {
                                    return Object::toValue(floor(Object::toNumber(a) / Object::toNumber(b)));
                                }
                                return Object::NullVal;
                            }
                            break;
                        case LITTOK_EQUAL_EQUAL:
                            {
                                return Object::fromBool(a == b);
                            }
                            break;
                        case LITTOK_BANG_EQUAL:
                            {
                                return Object::fromBool(a != b);
                            }
                            break;
                        case LITTOK_IS:
                        default:
                            {
                            }
                            break;
                    }
                    return Object::NullVal;
                }

                void optimize(Expression::List* statements);

                #if defined(LIT_DEBUG_OPTIMIZER)
                void optdbg(const char* fmt, ...)
                {
                    va_list va;
                    va_start(va, fmt);
                    fprintf(stderr, "optimizer: ");
                    vfprintf(stderr, fmt, va);
                    fprintf(stderr, "\n");
                    va_end(va);
                }
                #else
                    #define optdbg(msg, ...)
                #endif

                void opt_begin_scope()
                {
                    m_depth++;
                }

                void opt_end_scope()
                {
                    bool remove_unused;
                    Variable* variable;
                    PCGenericArray<Variable>* variables;
                    m_depth--;
                    variables = &m_variables;
                    remove_unused = Optimizer::is_enabled(LITOPTSTATE_UNUSED_VAR);
                    while(variables->m_count > 0 && variables->m_values[variables->m_count - 1].depth > m_depth)
                    {
                        if(remove_unused && !variables->m_values[variables->m_count - 1].used)
                        {
                            variable = &variables->m_values[variables->m_count - 1];
                            Expression::releaseStatement(m_state, *variable->declaration);
                            *variable->declaration = nullptr;
                        }
                        variables->m_count--;
                    }
                }

                Variable* add_variable(const char* name, size_t length, bool constant, Expression** declaration)
                {
                    m_variables.push(Variable{ name, length, m_depth, constant, m_markused, Object::NullVal, declaration });
                    return &m_variables.m_values[m_variables.m_count - 1];
                }

                Variable* resolve_variable(const char* name, size_t length)
                {
                    int i;
                    PCGenericArray<Variable>* variables;
                    Variable* variable;
                    variables = &m_variables;
                    for(i = variables->m_count - 1; i >= 0; i--)
                    {
                        variable = &variables->m_values[i];
                        if(length == variable->length && memcmp(variable->name, name, length) == 0)
                        {
                            return variable;
                        }
                    }
                    return nullptr;
                }

                Value attempt_to_optimize_binary(ExprBinary* expression, Value value, bool left)
                {
                    double number;
                    TokenType op;
                    op = expression->op;
                    Expression* branch;
                    branch = left ? expression->left : expression->right;
                    if(Object::isNumber(value))
                    {
                        number = Object::toNumber(value);
                        if(op == LITTOK_STAR)
                        {
                            if(number == 0)
                            {
                                optdbg("reducing expression to literal '0'");
                                return Object::toValue(0);
                            }
                            else if(number == 1)
                            {
                                optdbg("reducing expression to literal '1'");
                                Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                                expression->left = branch;
                                expression->right = nullptr;
                            }
                        }
                        else if((op == LITTOK_PLUS || op == LITTOK_MINUS) && number == 0)
                        {
                            optdbg("reducing expression that would result in '0' to literal '0'");
                            Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                            expression->left = branch;
                            expression->right = nullptr;
                        }
                        else if(((left && op == LITTOK_SLASH) || op == LITTOK_STAR_STAR) && number == 1)
                        {
                            optdbg("reducing expression that would result in '1' to literal '1'");
                            Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                            expression->left = branch;
                            expression->right = nullptr;
                        }
                    }
                    return Object::NullVal;
                }

                Value evaluate_expression(Expression* expression)
                {
                    ExprUnary* uexpr;
                    ExprBinary* bexpr;
                    Value a;
                    Value b;
                    Value branch;
                    if(expression == nullptr)
                    {
                        return Object::NullVal;
                    }
                    switch(expression->type)
                    {
                        case Expression::Type::Literal:
                            {
                                return ((ExprLiteral*)expression)->value;
                            }
                            break;
                        case Expression::Type::Unary:
                            {
                                uexpr = (ExprUnary*)expression;
                                branch = evaluate_expression(uexpr->right);
                                if(branch != Object::NullVal)
                                {
                                    return evaluate_unary_op(branch, uexpr->op);
                                }
                            }
                            break;
                        case Expression::Type::Binary:
                            {
                                bexpr = (ExprBinary*)expression;
                                a = evaluate_expression(bexpr->left);
                                b = evaluate_expression(bexpr->right);
                                if(a != Object::NullVal && b != Object::NullVal)
                                {
                                    return evaluate_binary_op(a, b, bexpr->op);
                                }
                                else if(a != Object::NullVal)
                                {
                                    return attempt_to_optimize_binary(bexpr, a, false);
                                }
                                else if(b != Object::NullVal)
                                {
                                    return attempt_to_optimize_binary(bexpr, b, true);
                                }
                            }
                            break;
                        default:
                            {
                                return Object::NullVal;
                            }
                            break;
                    }
                    return Object::NullVal;
                }

                void optimize_expression(Expression** slot)
                {
                    auto expression = *slot;
                    if(expression == nullptr)
                    {
                        return;
                    }
                    auto state = m_state;
                    switch(expression->type)
                    {
                        case Expression::Type::Unary:
                        case Expression::Type::Binary:
                            {
                                if(Optimizer::is_enabled(LITOPTSTATE_LITERAL_FOLDING))
                                {
                                    Value optimized = evaluate_expression(expression);
                                    if(optimized != Object::NullVal)
                                    {
                                        *slot = (Expression*)ExprLiteral::make(state, expression->line, optimized);
                                        Expression::releaseExpression(state, expression);
                                        break;
                                    }
                                }
                                switch(expression->type)
                                {
                                    case Expression::Type::Unary:
                                        {
                                            optimize_expression(&((ExprUnary*)expression)->right);
                                        }
                                        break;
                                    case Expression::Type::Binary:
                                        {
                                            auto expr = (ExprBinary*)expression;
                                            optimize_expression(&expr->left);
                                            optimize_expression(&expr->right);
                                        }
                                        break;
                                    default:
                                        {
                                        }
                                        break;
                                }
                            }
                            break;
                        case Expression::Type::Assign:
                            {
                                auto expr = (ExprAssign*)expression;
                                optimize_expression(&expr->to);
                                optimize_expression(&expr->value);
                            }
                            break;
                        case Expression::Type::Call:
                            {
                                auto expr = (ExprCall*)expression;
                                optimize_expression(&expr->callee);
                                optimize_expressions(&expr->args);
                            }
                            break;
                        case Expression::Type::Set:
                            {
                                auto expr = (ExprIndexSet*)expression;
                                optimize_expression(&expr->where);
                                optimize_expression(&expr->value);
                            }
                            break;
                        case Expression::Type::Get:
                            {
                                optimize_expression(&((ExprIndexGet*)expression)->where);
                            }
                            break;
                        case Expression::Type::Lambda:
                            {
                                opt_begin_scope();
                                optimize_statement(&((ExprLambda*)expression)->body);
                                opt_end_scope();
                            }
                            break;
                        case Expression::Type::Array:
                            {
                                optimize_expressions(&((ExprArray*)expression)->values);
                            }
                            break;
                        case Expression::Type::Object:
                            {
                                optimize_expressions(&((ExprObject*)expression)->values);
                            }
                            break;
                        case Expression::Type::Subscript:
                            {
                                auto expr = (ExprSubscript*)expression;
                                optimize_expression(&expr->array);
                                optimize_expression(&expr->index);
                            }
                            break;
                        case Expression::Type::Range:
                            {
                                auto expr = (ExprRange*)expression;
                                optimize_expression(&expr->from);
                                optimize_expression(&expr->to);
                            }
                            break;
                        case Expression::Type::IfClause:
                            {
                                auto expr = (ExprIfClause*)expression;
                                Value optimized = evaluate_expression(expr->condition);
                                if(optimized != Object::NullVal)
                                {
                                    if(Object::isFalsey(optimized))
                                    {
                                        *slot = expr->else_branch;
                                        expr->else_branch = nullptr;// So that it doesn't get freed
                                    }
                                    else
                                    {
                                        *slot = expr->if_branch;
                                        expr->if_branch = nullptr;// So that it doesn't get freed
                                    }
                                    optimize_expression(slot);
                                    Expression::releaseExpression(state, expression);
                                }
                                else
                                {
                                    optimize_expression(&expr->if_branch);
                                    optimize_expression(&expr->else_branch);
                                }
                            }
                            break;
                        case Expression::Type::Interpolation:
                            {
                                optimize_expressions(&((ExprInterpolation*)expression)->expressions);
                            }
                            break;
                        case Expression::Type::Variable:
                            {
                                auto expr = (ExprVar*)expression;
                                auto variable = resolve_variable(expr->name, expr->length);
                                if(variable != nullptr)
                                {
                                    variable->used = true;

                                    // Not checking here for the enable-ness of constant-folding, since if its off
                                    // the constant_value would be Object::NullVal anyway (:thinkaboutit:)
                                    if(variable->constant && variable->constant_value != Object::NullVal)
                                    {
                                        *slot = (Expression*)ExprLiteral::make(state, expression->line, variable->constant_value);
                                        Expression::releaseExpression(state, expression);
                                    }
                                }
                            }
                            break;
                        case Expression::Type::Reference:
                            {
                                optimize_expression(&((ExprReference*)expression)->to);
                            }
                            break;
                        case Expression::Type::Literal:
                        case Expression::Type::This:
                        case Expression::Type::Super:
                            {
                                // nothing to do here
                            }
                            break;
                        default:
                            {
                            }
                            break;
                    }
                }

                void optimize_expressions(Expression::List* expressions)
                {
                    size_t i;
                    for(i = 0; i < expressions->m_count; i++)
                    {
                        optimize_expression(&expressions->m_values[i]);
                    }
                }

                void optimize_statement(Expression** slot)
                {
                    size_t i;
                    size_t j;
                    State* state;
                    Expression* statement;
                    statement = *slot;
                    if(statement == nullptr)
                    {
                        return;
                    }
                    state = m_state;
                    switch(statement->type)
                    {
                        case Expression::Type::Unspecified:
                            {
                            }
                            break;
                        case Expression::Type::Expression:
                            {
                                optimize_expression(&((ExprStatement*)statement)->expression);
                            }
                            break;
                        case Expression::Type::Block:
                            {
                                auto stmt = (StmtBlock*)statement;
                                if(stmt->statements.m_count == 0)
                                {
                                    Expression::releaseStatement(state, statement);
                                    *slot = nullptr;
                                    break;
                                }
                                opt_begin_scope();
                                optimize_statements(&stmt->statements);
                                opt_end_scope();
                                bool found = false;
                                for(i = 0; i < stmt->statements.m_count; i++)
                                {
                                    auto step = stmt->statements.m_values[i];
                                    if(!is_empty(step))
                                    {
                                        found = true;
                                        if(step->type == Expression::Type::ReturnClause)
                                        {
                                            // Remove all the statements post return
                                            for(j = i + 1; j < stmt->statements.m_count; j++)
                                            {
                                                step = stmt->statements.m_values[j];
                                                if(step != nullptr)
                                                {
                                                    Expression::releaseStatement(state, step);
                                                    stmt->statements.m_values[j] = nullptr;
                                                }
                                            }
                                            stmt->statements.m_count = i + 1;
                                            break;
                                        }
                                    }
                                }
                                if(!found && Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY))
                                {
                                    Expression::releaseStatement(m_state, statement);
                                    *slot = nullptr;
                                }
                            }
                            break;
                        case Expression::Type::IfClause:
                            {
                                auto stmt = (StmtIfClause*)statement;
                                optimize_expression(&stmt->condition);
                                optimize_statement(&stmt->if_branch);
                                bool empty = Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY);
                                bool dead = Optimizer::is_enabled(LITOPTSTATE_UNREACHABLE_CODE);
                                Value optimized = empty ? evaluate_expression(stmt->condition) : Object::NullVal;
                                if((optimized != Object::NullVal && Object::isFalsey(optimized)) || (dead && is_empty(stmt->if_branch)))
                                {
                                    Expression::releaseExpression(state, stmt->condition);
                                    stmt->condition = nullptr;
                                    Expression::releaseStatement(state, stmt->if_branch);
                                    stmt->if_branch = nullptr;
                                }
                                if(stmt->elseif_conditions != nullptr)
                                {
                                    optimize_expressions(stmt->elseif_conditions);
                                    optimize_statements(stmt->elseif_branches);
                                    if(dead || empty)
                                    {
                                        for(i = 0; i < stmt->elseif_conditions->m_count; i++)
                                        {
                                            if(empty && is_empty(stmt->elseif_branches->m_values[i]))
                                            {
                                                Expression::releaseExpression(state, stmt->elseif_conditions->m_values[i]);
                                                stmt->elseif_conditions->m_values[i] = nullptr;
                                                Expression::releaseStatement(state, stmt->elseif_branches->m_values[i]);
                                                stmt->elseif_branches->m_values[i] = nullptr;
                                                continue;
                                            }
                                            if(dead)
                                            {
                                                Value value = evaluate_expression(stmt->elseif_conditions->m_values[i]);
                                                if(value != Object::NullVal && Object::isFalsey(value))
                                                {
                                                    Expression::releaseExpression(state, stmt->elseif_conditions->m_values[i]);
                                                    stmt->elseif_conditions->m_values[i] = nullptr;
                                                    Expression::releaseStatement(state, stmt->elseif_branches->m_values[i]);
                                                    stmt->elseif_branches->m_values[i] = nullptr;
                                                }
                                            }
                                        }
                                    }
                                }
                                optimize_statement(&stmt->else_branch);
                            }
                            break;
                        case Expression::Type::WhileLoop:
                            {
                                auto stmt = (StmtWhileLoop*)statement;
                                optimize_expression(&stmt->condition);
                                if(Optimizer::is_enabled(LITOPTSTATE_UNREACHABLE_CODE))
                                {
                                    Value optimized = evaluate_expression(stmt->condition);
                                    if(optimized != Object::NullVal && Object::isFalsey(optimized))
                                    {
                                        Expression::releaseStatement(m_state, statement);
                                        *slot = nullptr;
                                        break;
                                    }
                                }
                                optimize_statement(&stmt->body);
                                if(Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                                {
                                    Expression::releaseStatement(m_state, statement);
                                    *slot = nullptr;
                                }
                            }
                            break;
                        case Expression::Type::ForLoop:
                            {
                                auto stmt = (StmtForLoop*)statement;
                                opt_begin_scope();
                                // This is required, so that this doesn't optimize out our i variable (and such)
                                m_markused = true;
                                optimize_expression(&stmt->exprinit);
                                optimize_expression(&stmt->condition);
                                optimize_expression(&stmt->increment);
                                optimize_statement(&stmt->var);
                                m_markused = false;
                                optimize_statement(&stmt->body);
                                opt_end_scope();
                                if(Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                                {
                                    Expression::releaseStatement(m_state, statement);
                                    *slot = nullptr;
                                    break;
                                }
                                if(stmt->c_style || !Optimizer::is_enabled(LITOPTSTATE_C_FOR) || stmt->condition->type != Expression::Type::Range)
                                {
                                    break;
                                }
                                auto range = (ExprRange*)stmt->condition;
                                Value from = evaluate_expression(range->from);
                                Value to = evaluate_expression(range->to);
                                if(!Object::isNumber(from) || !Object::isNumber(to))
                                {
                                    break;
                                }
                                bool reverse = Object::toNumber(from) > Object::toNumber(to);
                                auto var = (StmtVar*)stmt->var;
                                size_t line = range->line;
                                // var i = from
                                var->valexpr = range->from;
                                // i <= to
                                stmt->condition = (Expression*)ExprBinary::make(
                                state, line, (Expression*)ExprVar::make(state, line, var->name, var->length), range->to, LITTOK_LESS_EQUAL);
                                // i++ (or i--)
                                auto var_get = (Expression*)ExprVar::make(state, line, var->name, var->length);
                                auto assign_value = ExprBinary::make(state, line, var_get, (Expression*)ExprLiteral::make(state, line, Object::toValue(1)),
                                reverse ? LITTOK_MINUS_MINUS : LITTOK_PLUS);
                                assign_value->ignore_left = true;
                                auto increment = (Expression*)ExprAssign::make(state, line, var_get, (Expression*)assign_value);
                                stmt->increment = (Expression*)increment;
                                range->from = nullptr;
                                range->to = nullptr;
                                stmt->c_style = true;
                                Expression::releaseExpression(state, (Expression*)range);
                            }
                            break;
                        case Expression::Type::VarDecl:
                            {
                                auto stmt = (StmtVar*)statement;
                                auto variable = add_variable(stmt->name, stmt->length, stmt->constant, slot);
                                optimize_expression(&stmt->valexpr);
                                if(stmt->constant && Optimizer::is_enabled(LITOPTSTATE_CONSTANT_FOLDING))
                                {
                                    Value value = evaluate_expression(stmt->valexpr);
                                    if(value != Object::NullVal)
                                    {
                                        variable->constant_value = value;
                                    }
                                }
                            }
                            break;
                        case Expression::Type::FunctionDecl:
                            {
                                auto stmt = (StmtFunction*)statement;
                                auto variable = add_variable(stmt->name, stmt->length, false, slot);
                                if(stmt->exported)
                                {
                                    // Otherwise it will get optimized-out with a big chance
                                    variable->used = true;
                                }
                                opt_begin_scope();
                                optimize_statement(&stmt->body);
                                opt_end_scope();
                            }
                            break;
                        case Expression::Type::ReturnClause:
                            {
                                optimize_expression(&((StmtReturn*)statement)->expression);
                            }
                            break;
                        case Expression::Type::MethodDecl:
                            {
                                opt_begin_scope();
                                optimize_statement(&((StmtMethod*)statement)->body);
                                opt_end_scope();
                            }
                            break;
                        case Expression::Type::ClassDecl:
                            {
                                optimize_statements(&((StmtClass*)statement)->fields);
                            }
                            break;
                        case Expression::Type::FieldDecl:
                            {
                                auto stmt = (StmtField*)statement;
                                if(stmt->getter != nullptr)
                                {
                                    opt_begin_scope();
                                    optimize_statement(&stmt->getter);
                                    opt_end_scope();
                                }
                                if(stmt->setter != nullptr)
                                {
                                    opt_begin_scope();
                                    optimize_statement(&stmt->setter);
                                    opt_end_scope();
                                }
                            }
                            break;
                        // Nothing to optimize there
                        case Expression::Type::ContinueClause:
                        case Expression::Type::BreakClause:
                            {
                            }
                            break;
                        default:
                            break;
                    }
                }

                void optimize_statements(Expression::List* statements)
                {
                    size_t i;
                    for(i = 0; i < statements->m_count; i++)
                    {
                        optimize_statement(&statements->m_values[i]);
                    }
                }
        };

        class Emitter
        {
            public:
                struct Private
                {
                    bool initialized;
                    bool constant;
                };

            public:
                State* m_state = nullptr;
                Chunk* m_chunk = nullptr;
                Compiler* m_compiler = nullptr;
                size_t m_lastline = 0;
                size_t m_loopstart = 0;
                PCGenericArray<Private> m_privates;
                PCGenericArray<size_t> m_breaks;
                PCGenericArray<size_t> m_continues;
                Module* m_module = nullptr;
                String* m_classname = nullptr;
                bool m_havesuperclass = false;
                bool m_prevwasexprstmt = false;
                int m_emitreference = 0;

            public:
                void init(State* state)
                {
                    m_state = state;
                    m_loopstart = 0;
                    m_emitreference = 0;
                    m_classname = nullptr;
                    m_compiler = nullptr;
                    m_chunk = nullptr;
                    m_module = nullptr;
                    m_prevwasexprstmt = false;
                    m_havesuperclass = false;
                    m_privates.init(state);
                    m_breaks.init(state);
                    m_continues.init(state);
                }

                void release()
                {
                    m_breaks.release();
                    m_continues.release();
                }

                void error(size_t line, Error error, ...);

                int8_t getStackEffect(int idx);

                void resolve_statements(Expression::List& statements)
                {
                    size_t i;
                    for(i = 0; i < statements.m_count; i++)
                    {
                        resolve_statement(statements.m_values[i]);
                    }
                }

                void emit_byte(uint16_t line, uint8_t byte)
                {
                    if(line < m_lastline)
                    {
                        // Egor-fail proofing
                        line = m_lastline;
                    }
                    m_chunk->putChunk(byte, line);
                    m_lastline = line;
                }

                void emit_bytes(uint16_t line, uint8_t a, uint8_t b)
                {
                    if(line < m_lastline)
                    {
                        // Egor-fail proofing
                        line = m_lastline;
                    }
                    m_chunk->putChunk(a, line);
                    m_chunk->putChunk(b, line);
                    m_lastline = line;
                }

                void emit_op(uint16_t line, OpCode op)
                {
                    auto compiler = m_compiler;
                    emit_byte(line, (uint8_t)op);
                    compiler->slots += getStackEffect((int)op);
                    if(compiler->slots > (int)compiler->function->max_slots)
                    {
                        compiler->function->max_slots = (size_t)compiler->slots;
                    }
                }

                void emit_ops(uint16_t line, OpCode a, OpCode b)
                {
                    auto compiler = m_compiler;
                    emit_bytes(line, (uint8_t)a, (uint8_t)b);
                    compiler->slots += getStackEffect((int)a) + getStackEffect((int)b);
                    if(compiler->slots > (int)compiler->function->max_slots)
                    {
                        compiler->function->max_slots = (size_t)compiler->slots;
                    }
                }

                void emit_varying_op(uint16_t line, OpCode op, uint8_t arg)
                {
                    auto compiler = m_compiler;
                    emit_bytes(line, (uint8_t)op, arg);
                    compiler->slots -= arg;
                    if(compiler->slots > (int)compiler->function->max_slots)
                    {
                        compiler->function->max_slots = (size_t)compiler->slots;
                    }
                }

                void emit_arged_op(uint16_t line, OpCode op, uint8_t arg)
                {
                    auto compiler = m_compiler;
                    emit_bytes(line, (uint8_t)op, arg);
                    compiler->slots += getStackEffect((int)op);
                    if(compiler->slots > (int)compiler->function->max_slots)
                    {
                        compiler->function->max_slots = (size_t)compiler->slots;
                    }
                }

                void emit_short(uint16_t line, uint16_t value)
                {
                    emit_bytes(line, (uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
                }

                void emit_byte_or_short(uint16_t line, uint8_t a, uint8_t b, uint16_t index)
                {
                    if(index > UINT8_MAX)
                    {
                        emit_op(line, (OpCode)b);
                        emit_short(line, (uint16_t)index);
                    }
                    else
                    {
                        emit_arged_op(line, (OpCode)a, (uint8_t)index);
                    }
                }

                const char* getStateScannerFilename();

                void init_compiler(Compiler* compiler, FunctionType type)
                {
                    compiler->locals.init(m_state);
                    compiler->type = type;
                    compiler->scope_depth = 0;
                    compiler->enclosing = (Compiler*)m_compiler;
                    compiler->skip_return = false;
                    compiler->function = Function::make(m_state, m_module);
                    compiler->loop_depth = 0;
                    m_compiler = compiler;
                    const char* name = getStateScannerFilename();
                    if(m_compiler == nullptr)
                    {
                        compiler->function->name = String::copy(m_state, name, strlen(name));
                    }
                    m_chunk = &compiler->function->chunk;
                    if(Optimizer::is_enabled(LITOPTSTATE_LINE_INFO))
                    {
                        m_chunk->m_haslineinfo = false;
                    }
                    if(type == LITFUNC_METHOD || type == LITFUNC_STATIC_METHOD || type == LITFUNC_CONSTRUCTOR)
                    {
                        compiler->locals.push(Local{ "this", 4, -1, false, false });
                    }
                    else
                    {
                        compiler->locals.push(Local{ "", 0, -1, false, false });
                    }
                    compiler->slots = 1;
                    compiler->max_slots = 1;
                }

                void emit_return(size_t line)
                {
                    if(m_compiler->type == LITFUNC_CONSTRUCTOR)
                    {
                        emit_arged_op(line, OP_GET_LOCAL, 0);
                        emit_op(line, OP_RETURN);
                    }
                    else if(m_prevwasexprstmt && m_chunk->m_count > 0)
                    {
                        m_chunk->m_count--;// Remove the OP_POP
                        emit_op(line, OP_RETURN);
                    }
                    else
                    {
                        emit_ops(line, OP_NULL, OP_RETURN);
                    }
                }

                Function* end_compiler(String* name)
                {
                    if(!m_compiler->skip_return)
                    {
                        emit_return(m_lastline);
                        m_compiler->skip_return = true;
                    }
                    auto function = m_compiler->function;
                    m_compiler->locals.release();
                    m_compiler = (Compiler*)m_compiler->enclosing;
                    m_chunk = m_compiler == nullptr ? nullptr : &m_compiler->function->chunk;
                    if(name != nullptr)
                    {
                        function->name = name;
                    }
                #ifdef LIT_TRACE_CHUNK
                    lit_disassemble_chunk(&function->chunk, function->name->data(), nullptr);
                #endif
                    return function;
                }

                void begin_scope()
                {
                    m_compiler->scope_depth++;
                }

                void end_scope(uint16_t line)
                {
                    m_compiler->scope_depth--;
                    auto compiler = m_compiler;
                    PCGenericArray<Local>* locals = &compiler->locals;
                    while(locals->m_count > 0 && locals->m_values[locals->m_count - 1].depth > compiler->scope_depth)
                    {
                        if(locals->m_values[locals->m_count - 1].captured)
                        {
                            emit_op(line, OP_CLOSE_UPVALUE);
                        }
                        else
                        {
                            emit_op(line, OP_POP);
                        }

                        locals->m_count--;
                    }
                }

                uint16_t addConstant(size_t line, Value value)
                {
                    size_t constant = m_chunk->addConstant(value);
                    if(constant >= UINT16_MAX)
                    {
                        error(line, Error::LITERROR_TOO_MANY_CONSTANTS);
                    }
                    return constant;
                }

                size_t emit_constant(size_t line, Value value)
                {
                    size_t constant = m_chunk->addConstant(value);
                    if(constant < UINT8_MAX)
                    {
                        emit_arged_op(line, OP_CONSTANT, constant);
                    }
                    else if(constant < UINT16_MAX)
                    {
                        emit_op(line, OP_CONSTANT_LONG);
                        emit_short(line, constant);
                    }
                    else
                    {
                        error(line, Error::LITERROR_TOO_MANY_CONSTANTS);
                    }

                    return constant;
                }

                int add_private(const char* name, size_t length, size_t line, bool constant)
                {
                    auto privates = &m_privates;
                    if(privates->m_count == UINT16_MAX)
                    {
                        error(line, Error::LITERROR_TOO_MANY_PRIVATES);
                    }
                    auto private_names = &m_module->private_names->m_values;
                    auto key = private_names->find(name, length, String::makeHash(name, length));
                    if(key != nullptr)
                    {
                        error(line, Error::LITERROR_VAR_REDEFINED, length, name);
                        Value index;
                        if(private_names->get(key, &index))
                        {
                            return Object::toNumber(index);
                        }
                    }
                    auto state = m_state;
                    int index = (int)privates->m_count;
                    privates->push(Private{ false, constant });
                    private_names->set(String::copy(state, name, length), Object::toValue(index));
                    m_module->private_count++;
                    return index;
                }

                int resolve_private(const char* name, size_t length, size_t line)
                {
                    auto private_names = &m_module->private_names->m_values;
                    auto key = private_names->find(name, length, String::makeHash(name, length));
                    if(key != nullptr)
                    {
                        Value index;
                        if(private_names->get(key, &index))
                        {
                            int number_index = Object::toNumber(index);
                            if(!m_privates.m_values[number_index].initialized)
                            {
                                error(line, Error::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                            }
                            return number_index;
                        }
                    }
                    return -1;
                }
                int add_local(const char* name, size_t length, size_t line, bool constant)
                {
                    auto compiler = m_compiler;
                    auto locals = &compiler->locals;
                    if(locals->m_count == UINT16_MAX)
                    {
                        error(line, Error::LITERROR_TOO_MANY_LOCALS);
                    }
                    for(int i = (int)locals->m_count - 1; i >= 0; i--)
                    {
                        auto local = &locals->m_values[i];
                        if(local->depth != UINT16_MAX && local->depth < compiler->scope_depth)
                        {
                            break;
                        }
                        if(length == local->length && memcmp(local->name, name, length) == 0)
                        {
                            error(line, Error::LITERROR_VAR_REDEFINED, length, name);
                        }
                    }
                    locals->push(Local{ name, length, UINT16_MAX, false, constant });
                    return (int)locals->m_count - 1;
                }

                int resolve_local(Compiler* compiler, const char* name, size_t length, size_t line)
                {
                    auto locals = &compiler->locals;
                    for(int i = (int)locals->m_count - 1; i >= 0; i--)
                    {
                        auto local = &locals->m_values[i];
                        if(local->length == length && memcmp(local->name, name, length) == 0)
                        {
                            if(local->depth == UINT16_MAX)
                            {
                                error(line, Error::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                            }
                            return i;
                        }
                    }
                    return -1;
                }

                int add_upvalue(Compiler* compiler, uint8_t index, size_t line, bool is_local)
                {
                    size_t upvalue_count = compiler->function->upvalue_count;
                    for(size_t i = 0; i < upvalue_count; i++)
                    {
                        auto upvalue = &compiler->upvalues[i];
                        if(upvalue->index == index && upvalue->isLocal == is_local)
                        {
                            return i;
                        }
                    }
                    if(upvalue_count == UINT16_COUNT)
                    {
                        error(line, Error::LITERROR_TOO_MANY_UPVALUES);
                        return 0;
                    }
                    compiler->upvalues[upvalue_count].isLocal = is_local;
                    compiler->upvalues[upvalue_count].index = index;
                    return compiler->function->upvalue_count++;
                }

                int resolve_upvalue(Compiler* compiler, const char* name, size_t length, size_t line)
                {
                    if(compiler->enclosing == nullptr)
                    {
                        return -1;
                    }
                    int local = resolve_local((Compiler*)compiler->enclosing, name, length, line);
                    if(local != -1)
                    {
                        ((Compiler*)compiler->enclosing)->locals.m_values[local].captured = true;
                        return add_upvalue(compiler, (uint8_t)local, line, true);
                    }
                    int upvalue = resolve_upvalue((Compiler*)compiler->enclosing, name, length, line);
                    if(upvalue != -1)
                    {
                        return add_upvalue(compiler, (uint8_t)upvalue, line, false);
                    }
                    return -1;
                }

                void mark_local_initialized(size_t index)
                {
                    m_compiler->locals.m_values[index].depth = m_compiler->scope_depth;
                }

                void mark_private_initialized(size_t index)
                {
                    m_privates.m_values[index].initialized = true;
                }

                size_t emit_jump(OpCode code, size_t line)
                {
                    emit_op(line, code);
                    emit_bytes(line, 0xff, 0xff);
                    return m_chunk->m_count - 2;
                }

                void patch_jump(size_t offset, size_t line)
                {
                    size_t jump = m_chunk->m_count - offset - 2;
                    if(jump > UINT16_MAX)
                    {
                        error(line, Error::LITERROR_JUMP_TOO_BIG);
                    }
                    m_chunk->m_code[offset] = (jump >> 8) & 0xff;
                    m_chunk->m_code[offset + 1] = jump & 0xff;
                }

                void emit_loop(size_t start, size_t line)
                {
                    emit_op(line, OP_JUMP_BACK);
                    size_t offset = m_chunk->m_count - start + 2;
                    if(offset > UINT16_MAX)
                    {
                        error(line, Error::LITERROR_JUMP_TOO_BIG);
                    }
                    emit_short(line, offset);
                }

                void patch_loop_jumps(PCGenericArray<size_t>* breaks, size_t line)
                {
                    for(size_t i = 0; i < breaks->m_count; i++)
                    {
                        patch_jump(breaks->m_values[i], line);
                    }
                    breaks->release();
                }

                bool emit_parameters(PCGenericArray<ExprFuncParam>* parameters, size_t line)
                {
                    for(size_t i = 0; i < parameters->m_count; i++)
                    {
                        auto parameter = &parameters->m_values[i];
                        int index = add_local(parameter->name, parameter->length, line, false);
                        mark_local_initialized(index);
                        // Vararg ...
                        if(parameter->length == 3 && memcmp(parameter->name, "...", 3) == 0)
                        {
                            return true;
                        }

                        if(parameter->default_value != nullptr)
                        {
                            emit_byte_or_short(line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
                            size_t jump = emit_jump(OP_NULL_OR, line);

                            emit_expression(parameter->default_value);
                            patch_jump(jump, line);
                            emit_byte_or_short(line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                            emit_op(line, OP_POP);
                        }
                    }

                    return false;
                }

                void resolve_statement(Expression* statement)
                {
                    if(statement == nullptr)
                    {
                        return;
                    }
                    switch(statement->type)
                    {
                        case Expression::Type::VarDecl:
                            {
                                auto stmt = (StmtVar*)statement;
                                mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, stmt->constant));
                            }
                            break;
                        case Expression::Type::FunctionDecl:
                            {
                                auto stmt = (StmtFunction*)statement;
                                if(!stmt->exported)
                                {
                                    mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, false));
                                }
                            }
                            break;
                        case Expression::Type::ClassDecl:
                        case Expression::Type::Block:
                        case Expression::Type::ForLoop:
                        case Expression::Type::WhileLoop:
                        case Expression::Type::IfClause:
                        case Expression::Type::ContinueClause:
                        case Expression::Type::BreakClause:
                        case Expression::Type::ReturnClause:
                        case Expression::Type::MethodDecl:
                        case Expression::Type::FieldDecl:
                        case Expression::Type::Expression:
                            {
                            }
                            break;
                        default:
                            break;
                    }
                }

                void emit_expression(Expression* expression)
                {
                    switch(expression->type)
                    {
                        case Expression::Type::Literal:
                            {
                                Value value = ((ExprLiteral*)expression)->value;
                                if(Object::isNumber(value) || Object::isString(value))
                                {
                                    emit_constant(expression->line, value);
                                }
                                else if(Object::isBool(value))
                                {
                                    emit_op(expression->line, Object::asBool(value) ? OP_TRUE : OP_FALSE);
                                }
                                else if(Object::isNull(value))
                                {
                                    emit_op(expression->line, OP_NULL);
                                }
                                else
                                {
                                    /* UNREACHABLE */
                                }
                            }
                            break;
                        case Expression::Type::Binary:
                            {
                                auto expr = (ExprBinary*)expression;
                                emit_expression(expr->left);
                                if(expr->right == nullptr)
                                {
                                    break;
                                }
                                TokenType op = expr->op;
                                if(op == LITTOK_AMPERSAND_AMPERSAND || op == LITTOK_BAR_BAR || op == LITTOK_QUESTION_QUESTION)
                                {
                                    size_t jump = emit_jump(op == LITTOK_BAR_BAR ? OP_OR : (op == LITTOK_QUESTION_QUESTION ? OP_NULL_OR : OP_AND),
                                                          m_lastline);
                                    emit_expression(expr->right);
                                    patch_jump(jump, m_lastline);
                                    break;
                                }
                                emit_expression(expr->right);
                                switch(op)
                                {
                                    case LITTOK_PLUS:
                                        {
                                            emit_op(expression->line, OP_ADD);
                                        }
                                        break;
                                    case LITTOK_MINUS:
                                        {
                                            emit_op(expression->line, OP_SUBTRACT);
                                        }
                                        break;
                                    case LITTOK_STAR:
                                        {
                                            emit_op(expression->line, OP_MULTIPLY);
                                        }
                                        break;
                                    case LITTOK_STAR_STAR:
                                        {
                                            emit_op(expression->line, OP_POWER);
                                        }
                                        break;
                                    case LITTOK_SLASH:
                                        {
                                            emit_op(expression->line, OP_DIVIDE);
                                        }
                                        break;
                                    case LITTOK_SHARP:
                                        {
                                            emit_op(expression->line, OP_FLOOR_DIVIDE);
                                        }
                                        break;
                                    case LITTOK_PERCENT:
                                        {
                                            emit_op(expression->line, OP_MOD);
                                        }
                                        break;
                                    case LITTOK_IS:
                                        {
                                            emit_op(expression->line, OP_IS);
                                        }
                                        break;
                                    case LITTOK_EQUAL_EQUAL:
                                        {
                                            emit_op(expression->line, OP_EQUAL);
                                        }
                                        break;
                                    case LITTOK_BANG_EQUAL:
                                        {
                                            emit_ops(expression->line, OP_EQUAL, OP_NOT);
                                        }
                                        break;
                                    case LITTOK_GREATER:
                                        {
                                            emit_op(expression->line, OP_GREATER);
                                        }
                                        break;
                                    case LITTOK_GREATER_EQUAL:
                                        {
                                            emit_op(expression->line, OP_GREATER_EQUAL);
                                        }
                                        break;
                                    case LITTOK_LESS:
                                        {
                                            emit_op(expression->line, OP_LESS);
                                        }
                                        break;
                                    case LITTOK_LESS_EQUAL:
                                        {
                                            emit_op(expression->line, OP_LESS_EQUAL);
                                        }
                                        break;
                                    case LITTOK_LESS_LESS:
                                        {
                                            emit_op(expression->line, OP_LSHIFT);
                                        }
                                        break;
                                    case LITTOK_GREATER_GREATER:
                                        {
                                            emit_op(expression->line, OP_RSHIFT);
                                        }
                                        break;
                                    case LITTOK_BAR:
                                        {
                                            emit_op(expression->line, OP_BOR);
                                        }
                                        break;
                                    case LITTOK_AMPERSAND:
                                        {
                                            emit_op(expression->line, OP_BAND);
                                        }
                                        break;
                                    case LITTOK_CARET:
                                        {
                                            emit_op(expression->line, OP_BXOR);
                                        }
                                        break;
                                    default:
                                        {
                                            /* UNREACHABLE */
                                        }
                                    break;
                                }
                            }
                            break;
                        case Expression::Type::Unary:
                            {
                                auto expr = (ExprUnary*)expression;
                                emit_expression(expr->right);
                                switch(expr->op)
                                {
                                    case LITTOK_MINUS:
                                        {
                                            emit_op(expression->line, OP_NEGATE);
                                        }
                                        break;
                                    case LITTOK_BANG:
                                        {
                                            emit_op(expression->line, OP_NOT);
                                        }
                                        break;
                                    case LITTOK_TILDE:
                                        {
                                            emit_op(expression->line, OP_BNOT);
                                        }
                                        break;
                                    default:
                                        {
                                            /* UNREACHABLE */
                                        }
                                        break;
                                }
                            }
                            break;
                        case Expression::Type::Variable:
                            {
                                auto expr = (ExprVar*)expression;
                                bool ref = m_emitreference > 0;
                                if(ref)
                                {
                                    m_emitreference--;
                                }
                                int index = resolve_local(m_compiler, expr->name, expr->length, expression->line);
                                if(index == -1)
                                {
                                    index = resolve_upvalue(m_compiler, expr->name, expr->length, expression->line);
                                    if(index == -1)
                                    {
                                        index = resolve_private(expr->name, expr->length, expression->line);
                                        if(index == -1)
                                        {
                                            emit_op(expression->line, ref ? OP_REFERENCE_GLOBAL : OP_GET_GLOBAL);
                                            emit_short(expression->line,
                                                       addConstant(expression->line,
                                                                    String::copy(m_state, expr->name, expr->length)->asValue()));
                                        }
                                        else
                                        {
                                            if(ref)
                                            {
                                                emit_op(expression->line, OP_REFERENCE_PRIVATE);
                                                emit_short(expression->line, index);
                                            }
                                            else
                                            {
                                                emit_byte_or_short(expression->line, OP_GET_PRIVATE, OP_GET_PRIVATE_LONG, index);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        emit_arged_op(expression->line, ref ? OP_REFERENCE_UPVALUE : OP_GET_UPVALUE, (uint8_t)index);
                                    }
                                }
                                else
                                {
                                    if(ref)
                                    {
                                        emit_op(expression->line, OP_REFERENCE_LOCAL);
                                        emit_short(expression->line, index);
                                    }
                                    else
                                    {
                                        emit_byte_or_short(expression->line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
                                    }
                                }
                            }
                            break;
                        case Expression::Type::Assign:
                            {
                                auto expr = (ExprAssign*)expression;
                                if(expr->to->type == Expression::Type::Variable)
                                {
                                    emit_expression(expr->value);
                                    auto e = (ExprVar*)expr->to;
                                    int index = resolve_local(m_compiler, e->name, e->length, expr->to->line);
                                    if(index == -1)
                                    {
                                        index = resolve_upvalue(m_compiler, e->name, e->length, expr->to->line);
                                        if(index == -1)
                                        {
                                            index = resolve_private(e->name, e->length, expr->to->line);
                                            if(index == -1)
                                            {
                                                emit_op(expression->line, OP_SET_GLOBAL);
                                                emit_short(expression->line,
                                                           addConstant(expression->line,
                                                                        String::copy(m_state, e->name, e->length)->asValue()));
                                            }
                                            else
                                            {
                                                if(m_privates.m_values[index].constant)
                                                {
                                                    error(expression->line, Error::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                                }
                                                emit_byte_or_short(expression->line, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                                            }
                                        }
                                        else
                                        {
                                            emit_arged_op(expression->line, OP_SET_UPVALUE, (uint8_t)index);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        if(m_compiler->locals.m_values[index].constant)
                                        {
                                            error(expression->line, Error::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                        }

                                        emit_byte_or_short(expression->line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                                    }
                                }
                                else if(expr->to->type == Expression::Type::Get)
                                {
                                    emit_expression(expr->value);
                                    auto e = (ExprIndexGet*)expr->to;
                                    emit_expression(e->where);
                                    emit_expression(expr->value);
                                    emit_constant(m_lastline, String::copy(m_state, e->name, e->length)->asValue());
                                    emit_ops(m_lastline, OP_SET_FIELD, OP_POP);
                                }
                                else if(expr->to->type == Expression::Type::Subscript)
                                {
                                    auto e = (ExprSubscript*)expr->to;
                                    emit_expression(e->array);
                                    emit_expression(e->index);
                                    emit_expression(expr->value);
                                    emit_op(m_lastline, OP_SUBSCRIPT_SET);
                                }
                                else if(expr->to->type == Expression::Type::Reference)
                                {
                                    emit_expression(expr->value);
                                    emit_expression(((ExprReference*)expr->to)->to);
                                    emit_op(expression->line, OP_SET_REFERENCE);
                                }
                                else
                                {
                                    error(expression->line, Error::LITERROR_INVALID_ASSIGMENT_TARGET);
                                }
                            }
                            break;
                        case Expression::Type::Call:
                            {
                                auto expr = (ExprCall*)expression;
                                bool method = expr->callee->type == Expression::Type::Get;
                                bool super = expr->callee->type == Expression::Type::Super;
                                if(method)
                                {
                                    ((ExprIndexGet*)expr->callee)->ignore_emit = true;
                                }
                                else if(super)
                                {
                                    ((ExprSuper*)expr->callee)->ignore_emit = true;
                                }
                                emit_expression(expr->callee);
                                if(super)
                                {
                                    emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                                }
                                for(size_t i = 0; i < expr->args.m_count; i++)
                                {
                                    auto e = expr->args.m_values[i];
                                    if(e->type == Expression::Type::Variable)
                                    {
                                        auto ee = (ExprVar*)e;
                                        // Vararg ...
                                        if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                                        {
                                            emit_arged_op(e->line, OP_VARARG,
                                                          resolve_local(m_compiler, "...", 3, expression->line));
                                            break;
                                        }
                                    }
                                    emit_expression(e);
                                }
                                if(method || super)
                                {
                                    if(method)
                                    {
                                        auto e = (ExprIndexGet*)expr->callee;
                                        emit_varying_op(expression->line,
                                                        ((ExprIndexGet*)expr->callee)->ignore_result ? OP_INVOKE_IGNORING : OP_INVOKE,
                                                        (uint8_t)expr->args.m_count);
                                        emit_short(m_lastline,
                                                   addConstant(m_lastline,
                                                                String::copy(m_state, e->name, e->length)->asValue()));
                                    }
                                    else
                                    {
                                        auto e = (ExprSuper*)expr->callee;
                                        uint8_t index = resolve_upvalue(m_compiler, "super", 5, m_lastline);
                                        emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                                        emit_varying_op(m_lastline,
                                                        ((ExprSuper*)expr->callee)->ignore_result ? OP_INVOKE_SUPER_IGNORING : OP_INVOKE_SUPER,
                                                        (uint8_t)expr->args.m_count);
                                        emit_short(m_lastline, addConstant(m_lastline, e->method->asValue()));
                                    }
                                }
                                else
                                {
                                    emit_varying_op(expression->line, OP_CALL, (uint8_t)expr->args.m_count);
                                }
                                if(method)
                                {
                                    auto get = expr->callee;
                                    while(get != nullptr)
                                    {
                                        if(get->type == Expression::Type::Get)
                                        {
                                            auto getter = (ExprIndexGet*)get;
                                            if(getter->jump > 0)
                                            {
                                                patch_jump(getter->jump, m_lastline);
                                            }
                                            get = getter->where;
                                        }
                                        else if(get->type == Expression::Type::Subscript)
                                        {
                                            get = ((ExprSubscript*)get)->array;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                }
                                if(expr->objexpr == nullptr)
                                {
                                    return;
                                }
                                auto objinit = (ExprObject*)expr->objexpr;
                                for(size_t i = 0; i < objinit->values.m_count; i++)
                                {
                                    auto e = objinit->values.m_values[i];
                                    m_lastline = e->line;
                                    emit_constant(m_lastline, objinit->keys.m_values[i]);
                                    emit_expression(e);
                                    emit_op(m_lastline, OP_PUSH_OBJECT_FIELD);
                                }
                            }
                            break;
                        case Expression::Type::Get:
                            {
                                auto expr = (ExprIndexGet*)expression;
                                bool ref = m_emitreference > 0;
                                if(ref)
                                {
                                    m_emitreference--;
                                }
                                emit_expression(expr->where);
                                if(expr->jump == 0)
                                {
                                    expr->jump = emit_jump(OP_JUMP_IF_NULL, m_lastline);
                                    if(!expr->ignore_emit)
                                    {
                                        emit_constant(m_lastline,
                                                      String::copy(m_state, expr->name, expr->length)->asValue());
                                        emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                                    }
                                    patch_jump(expr->jump, m_lastline);
                                }
                                else if(!expr->ignore_emit)
                                {
                                    emit_constant(m_lastline, String::copy(m_state, expr->name, expr->length)->asValue());
                                    emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                                }
                            }
                            break;
                        case Expression::Type::Set:
                            {
                                auto expr = (ExprIndexSet*)expression;
                                emit_expression(expr->where);
                                emit_expression(expr->value);
                                emit_constant(m_lastline, String::copy(m_state, expr->name, expr->length)->asValue());
                                emit_op(m_lastline, OP_SET_FIELD);
                            }
                            break;
                        case Expression::Type::Lambda:
                            {
                                auto expr = (ExprLambda*)expression;
                                auto name = String::format(m_state, "[lambda @:@]", m_module->name->asValue(),
                                                                              String::stringNumberToString(m_state, expression->line));
                                Compiler compiler;
                                init_compiler(&compiler, LITFUNC_REGULAR);
                                begin_scope();
                                bool vararg = emit_parameters(&expr->parameters, expression->line);
                                if(expr->body != nullptr)
                                {
                                    bool single_expression = expr->body->type == Expression::Type::Expression;
                                    if(single_expression)
                                    {
                                        compiler.skip_return = true;
                                        ((ExprStatement*)expr->body)->pop = false;
                                    }
                                    emit_statement(expr->body);
                                    if(single_expression)
                                    {
                                        emit_op(m_lastline, OP_RETURN);
                                    }
                                }
                                end_scope(m_lastline);
                                auto function = end_compiler(name);
                                function->arg_count = expr->parameters.m_count;
                                function->max_slots += function->arg_count;
                                function->vararg = vararg;
                                if(function->upvalue_count > 0)
                                {
                                    emit_op(m_lastline, OP_CLOSURE);
                                    emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                                    for(size_t i = 0; i < function->upvalue_count; i++)
                                    {
                                        emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                                    }
                                }
                                else
                                {
                                    emit_constant(m_lastline, function->asValue());
                                }
                            }
                            break;
                        case Expression::Type::Array:
                            {
                                auto expr = (ExprArray*)expression;
                                emit_op(expression->line, OP_ARRAY);
                                for(size_t i = 0; i < expr->values.m_count; i++)
                                {
                                    emit_expression(expr->values.m_values[i]);
                                    emit_op(m_lastline, OP_PUSH_ARRAY_ELEMENT);
                                }
                            }
                            break;
                        case Expression::Type::Object:
                            {
                                auto expr = (ExprObject*)expression;
                                emit_op(expression->line, OP_OBJECT);
                                for(size_t i = 0; i < expr->values.m_count; i++)
                                {
                                    emit_constant(m_lastline, expr->keys.m_values[i]);
                                    emit_expression(expr->values.m_values[i]);
                                    emit_op(m_lastline, OP_PUSH_OBJECT_FIELD);
                                }
                            }
                            break;
                        case Expression::Type::Subscript:
                            {
                                auto expr = (ExprSubscript*)expression;
                                emit_expression(expr->array);
                                emit_expression(expr->index);
                                emit_op(expression->line, OP_SUBSCRIPT_GET);
                            }
                            break;
                        case Expression::Type::This:
                            {
                                FunctionType type = m_compiler->type;
                                if(type == LITFUNC_STATIC_METHOD)
                                {
                                    error(expression->line, Error::LITERROR_THIS_MISSUSE, "in static methods");
                                }
                                if(type == LITFUNC_CONSTRUCTOR || type == LITFUNC_METHOD)
                                {
                                    emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                                }
                                else
                                {
                                    if(m_compiler->enclosing == nullptr)
                                    {
                                        error(expression->line, Error::LITERROR_THIS_MISSUSE, "in functions outside of any class");
                                    }
                                    else
                                    {
                                        int local = resolve_local((Compiler*)m_compiler->enclosing, "this", 4, expression->line);
                                        emit_arged_op(expression->line, OP_GET_UPVALUE,
                                                      add_upvalue(m_compiler, local, expression->line, true));
                                    }
                                }
                            }
                            break;
                        case Expression::Type::Super:
                            {
                                if(m_compiler->type == LITFUNC_STATIC_METHOD)
                                {
                                    error(expression->line, Error::LITERROR_SUPER_MISSUSE, "in static methods");
                                }
                                else if(!m_havesuperclass)
                                {
                                    error(expression->line, Error::LITERROR_NO_SUPER, m_classname->data());
                                }
                                auto expr = (ExprSuper*)expression;
                                if(!expr->ignore_emit)
                                {
                                    uint8_t index = resolve_upvalue(m_compiler, "super", 5, m_lastline);
                                    emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                                    emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                                    emit_op(expression->line, OP_GET_SUPER_METHOD);
                                    emit_short(expression->line, addConstant(expression->line, expr->method->asValue()));
                                }
                            }
                            break;
                        case Expression::Type::Range:
                            {
                                auto expr = (ExprRange*)expression;
                                emit_expression(expr->to);
                                emit_expression(expr->from);
                                emit_op(expression->line, OP_RANGE);
                            }
                            break;
                        case Expression::Type::IfClause:
                            {
                                auto expr = (ExprIfClause*)expression;
                                emit_expression(expr->condition);
                                uint64_t else_jump = emit_jump(OP_JUMP_IF_FALSE, expression->line);
                                emit_expression(expr->if_branch);
                                uint64_t end_jump = emit_jump(OP_JUMP, m_lastline);
                                patch_jump(else_jump, expr->else_branch->line);
                                emit_expression(expr->else_branch);

                                patch_jump(end_jump, m_lastline);
                            }
                            break;
                        case Expression::Type::Interpolation:
                            {
                                auto expr = (ExprInterpolation*)expression;
                                emit_op(expression->line, OP_ARRAY);
                                for(size_t i = 0; i < expr->expressions.m_count; i++)
                                {
                                    emit_expression(expr->expressions.m_values[i]);
                                    emit_op(m_lastline, OP_PUSH_ARRAY_ELEMENT);
                                }
                                emit_varying_op(m_lastline, OP_INVOKE, 0);
                                emit_short(m_lastline,
                                           addConstant(m_lastline, String::internValue(m_state, "join")));
                            }
                            break;
                        case Expression::Type::Reference:
                            {
                                auto to = ((ExprReference*)expression)->to;
                                if(to->type != Expression::Type::Variable && to->type != Expression::Type::Get && to->type != Expression::Type::This && to->type != Expression::Type::Super)
                                {
                                    error(expression->line, Error::LITERROR_INVALID_REFERENCE_TARGET);
                                    break;
                                }
                                int old = m_emitreference;
                                m_emitreference++;
                                emit_expression(to);
                                m_emitreference = old;
                            }
                            break;
                        default:
                            {
                                error(expression->line, Error::LITERROR_UNKNOWN_EXPRESSION, (int)expression->type);
                            }
                            break;
                    }
                }

                bool emit_statement(Expression* statement)
                {
                    StmtClass* clstmt;
                    Compiler compiler;
                    Expression* expression;
                    Expression* e;
                    ExprStatement* expr;
                    Field* field;
                    StmtField* fieldstmt;
                    Function* function;
                    Function* getter;
                    Function* setter;
                    StmtFunction* funcstmt;
                    Local* local;
                    PCGenericArray<Local>* locals;
                    StmtMethod* mthstmt;
                    Expression* blockstmt;
                    Expression* s;
                    Expression::List* statements;
                    String* name;
                    StmtVar* var;
                    StmtVar* varstmt;
                    StmtForLoop* forstmt;
                    StmtWhileLoop* whilestmt;
                    StmtIfClause* ifstmt;
                    bool constructor;
                    bool isexport;
                    bool isprivate;
                    bool islocal;
                    bool vararg;
                    int depth;
                    int ii;
                    int index;
                    size_t start;
                    size_t else_jump;
                    size_t exit_jump;
                    size_t body_jump;
                    size_t end_jump;
                    size_t i;
                    size_t increment_start;
                    size_t localcnt;
                    size_t iterator;
                    size_t line;
                    size_t sequence;
                    uint16_t local_count;
                    uint8_t super;
                    uint64_t* end_jumps;
                    if(statement == nullptr)
                    {
                        return false;
                    }
                    switch(statement->type)
                    {
                        case Expression::Type::Expression:
                            {
                                expr = (ExprStatement*)statement;
                                emit_expression(expr->expression);
                                if(expr->pop)
                                {
                                    emit_op(statement->line, OP_POP);
                                }
                            }
                            break;
                        case Expression::Type::Block:
                            {
                                statements = &((StmtBlock*)statement)->statements;
                                begin_scope();
                                {
                                    for(i = 0; i < statements->m_count; i++)
                                    {
                                        blockstmt = statements->m_values[i];

                                        if(emit_statement(blockstmt))
                                        {
                                            break;
                                        }
                                    }
                                }
                                end_scope(m_lastline);
                            }
                            break;
                        case Expression::Type::VarDecl:
                            {
                                varstmt = (StmtVar*)statement;
                                line = statement->line;
                                isprivate = m_compiler->enclosing == nullptr && m_compiler->scope_depth == 0;
                                index = isprivate ? resolve_private(varstmt->name, varstmt->length, statement->line) :
                                                      add_local(varstmt->name, varstmt->length, statement->line, varstmt->constant);
                                if(varstmt->valexpr == nullptr)
                                {
                                    emit_op(line, OP_NULL);
                                }
                                else
                                {
                                    emit_expression(varstmt->valexpr);
                                }
                                if(isprivate)
                                {
                                    mark_private_initialized(index);
                                }
                                else
                                {
                                    mark_local_initialized(index);
                                }
                                emit_byte_or_short(statement->line, isprivate ? OP_SET_PRIVATE : OP_SET_LOCAL,
                                                   isprivate ? OP_SET_PRIVATE_LONG : OP_SET_LOCAL_LONG, index);
                                if(isprivate)
                                {
                                    // Privates don't live on stack, so we need to pop them manually
                                    emit_op(statement->line, OP_POP);
                                }
                            }
                            break;
                        case Expression::Type::IfClause:
                            {
                                ifstmt = (StmtIfClause*)statement;
                                else_jump = 0;
                                end_jump = 0;
                                if(ifstmt->condition == nullptr)
                                {
                                    else_jump = emit_jump(OP_JUMP, statement->line);
                                }
                                else
                                {
                                    emit_expression(ifstmt->condition);
                                    else_jump = emit_jump(OP_JUMP_IF_FALSE, statement->line);
                                    emit_statement(ifstmt->if_branch);
                                    end_jump = emit_jump(OP_JUMP, m_lastline);
                                }
                                size_t jmpcnt = (ifstmt->elseif_branches == nullptr ? 1 : ifstmt->elseif_branches->m_count);
                                //uint64_t end_jumps[ifstmt->elseif_branches == nullptr ? 1 : ifstmt->elseif_branches->m_count];
                                end_jumps = (uint64_t*)calloc(jmpcnt + 1, sizeof(uint64_t));
                                memset(end_jumps, 0, jmpcnt);
                                if(ifstmt->elseif_branches != nullptr)
                                {
                                    for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                                    {
                                        e = ifstmt->elseif_conditions->m_values[i];
                                        if(e == nullptr)
                                        {
                                            continue;
                                        }
                                        patch_jump(else_jump, e->line);
                                        emit_expression(e);
                                        else_jump = emit_jump(OP_JUMP_IF_FALSE, m_lastline);
                                        emit_statement(ifstmt->elseif_branches->m_values[i]);
                                        end_jumps[i] = emit_jump(OP_JUMP, m_lastline);
                                    }
                                }
                                if(ifstmt->else_branch != nullptr)
                                {
                                    patch_jump(else_jump, ifstmt->else_branch->line);
                                    emit_statement(ifstmt->else_branch);
                                }
                                else
                                {
                                    patch_jump(else_jump, m_lastline);
                                }
                                if(end_jump != 0)
                                {
                                    patch_jump(end_jump, m_lastline);
                                }
                                if(ifstmt->elseif_branches != nullptr)
                                {
                                    for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                                    {
                                        if(ifstmt->elseif_branches->m_values[i] == nullptr)
                                        {
                                            continue;
                                        }
                                        patch_jump(end_jumps[i], ifstmt->elseif_branches->m_values[i]->line);
                                    }
                                }
                                free(end_jumps);
                            }
                            break;
                        case Expression::Type::WhileLoop:
                            {
                                whilestmt = (StmtWhileLoop*)statement;
                                start = m_chunk->m_count;
                                m_loopstart = start;
                                m_compiler->loop_depth++;
                                emit_expression(whilestmt->condition);
                                exit_jump = emit_jump(OP_JUMP_IF_FALSE, statement->line);
                                emit_statement(whilestmt->body);
                                patch_loop_jumps(&m_continues, m_lastline);
                                emit_loop(start, m_lastline);
                                patch_jump(exit_jump, m_lastline);
                                patch_loop_jumps(&m_breaks, m_lastline);
                                m_compiler->loop_depth--;
                            }
                            break;
                        case Expression::Type::ForLoop:
                            {
                                forstmt = (StmtForLoop*)statement;
                                begin_scope();
                                m_compiler->loop_depth++;
                                if(forstmt->c_style)
                                {
                                    if(forstmt->var != nullptr)
                                    {
                                        emit_statement(forstmt->var);
                                    }
                                    else if(forstmt->exprinit != nullptr)
                                    {
                                        emit_expression(forstmt->exprinit);
                                    }
                                    start = m_chunk->m_count;
                                    exit_jump = 0;
                                    if(forstmt->condition != nullptr)
                                    {
                                        emit_expression(forstmt->condition);
                                        exit_jump = emit_jump(OP_JUMP_IF_FALSE, m_lastline);
                                    }
                                    if(forstmt->increment != nullptr)
                                    {
                                        body_jump = emit_jump(OP_JUMP, m_lastline);
                                        increment_start = m_chunk->m_count;
                                        emit_expression(forstmt->increment);
                                        emit_op(m_lastline, OP_POP);
                                        emit_loop(start, m_lastline);
                                        start = increment_start;
                                        patch_jump(body_jump, m_lastline);
                                    }
                                    m_loopstart = start;
                                    begin_scope();
                                    if(forstmt->body != nullptr)
                                    {
                                        if(forstmt->body->type == Expression::Type::Block)
                                        {
                                            statements = &((StmtBlock*)forstmt->body)->statements;
                                            for(i = 0; i < statements->m_count; i++)
                                            {
                                                emit_statement(statements->m_values[i]);
                                            }
                                        }
                                        else
                                        {
                                            emit_statement(forstmt->body);
                                        }
                                    }
                                    patch_loop_jumps(&m_continues, m_lastline);
                                    end_scope(m_lastline);
                                    emit_loop(start, m_lastline);
                                    if(forstmt->condition != nullptr)
                                    {
                                        patch_jump(exit_jump, m_lastline);
                                    }
                                }
                                else
                                {
                                    sequence = add_local("seq ", 4, statement->line, false);
                                    mark_local_initialized(sequence);
                                    emit_expression(forstmt->condition);
                                    emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, sequence);
                                    iterator = add_local("iter ", 5, statement->line, false);
                                    mark_local_initialized(iterator);
                                    emit_op(m_lastline, OP_NULL);
                                    emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                                    start = m_chunk->m_count;
                                    m_loopstart = m_chunk->m_count;
                                    // iter = seq.iterator(iter)
                                    emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                                    emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                                    emit_varying_op(m_lastline, OP_INVOKE, 1);
                                    emit_short(m_lastline,
                                               addConstant(m_lastline, String::internValue(m_state, "iterator")));
                                    emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                                    // If iter is null, just get out of the loop
                                    exit_jump = emit_jump(OP_JUMP_IF_NULL_POPPING, m_lastline);
                                    begin_scope();
                                    // var i = seq.iteratorValue(iter)
                                    var = (StmtVar*)forstmt->var;
                                    localcnt = add_local(var->name, var->length, statement->line, false);
                                    mark_local_initialized(localcnt);
                                    emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                                    emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                                    emit_varying_op(m_lastline, OP_INVOKE, 1);
                                    emit_short(m_lastline,
                                               addConstant(m_lastline, String::internValue(m_state, "iteratorValue")));
                                    emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, localcnt);
                                    if(forstmt->body != nullptr)
                                    {
                                        if(forstmt->body->type == Expression::Type::Block)
                                        {
                                            statements = &((StmtBlock*)forstmt->body)->statements;
                                            for(i = 0; i < statements->m_count; i++)
                                            {
                                                emit_statement(statements->m_values[i]);
                                            }
                                        }
                                        else
                                        {
                                            emit_statement(forstmt->body);
                                        }
                                    }
                                    patch_loop_jumps(&m_continues, m_lastline);
                                    end_scope(m_lastline);
                                    emit_loop(start, m_lastline);
                                    patch_jump(exit_jump, m_lastline);
                                }
                                patch_loop_jumps(&m_breaks, m_lastline);
                                end_scope(m_lastline);
                                m_compiler->loop_depth--;
                            }
                            break;

                        case Expression::Type::ContinueClause:
                        {
                            if(m_compiler->loop_depth == 0)
                            {
                                error(statement->line, Error::LITERROR_LOOP_JUMP_MISSUSE, "continue");
                            }
                            m_continues.push(emit_jump(OP_JUMP, statement->line));
                            break;
                        }

                        case Expression::Type::BreakClause:
                            {
                                if(m_compiler->loop_depth == 0)
                                {
                                    error(statement->line, Error::LITERROR_LOOP_JUMP_MISSUSE, "break");
                                }
                                emit_op(statement->line, OP_POP_LOCALS);
                                depth = m_compiler->scope_depth;
                                local_count = 0;
                                locals = &m_compiler->locals;
                                for(ii = locals->m_count - 1; ii >= 0; ii--)
                                {
                                    local = &locals->m_values[ii];
                                    if(local->depth < depth)
                                    {
                                        break;
                                    }

                                    if(!local->captured)
                                    {
                                        local_count++;
                                    }
                                }
                                emit_short(statement->line, local_count);
                                m_breaks.push(emit_jump(OP_JUMP, statement->line));
                            }
                            break;
                        case Expression::Type::FunctionDecl:
                            {
                                funcstmt = (StmtFunction*)statement;
                                isexport = funcstmt->exported;
                                isprivate = !isexport && m_compiler->enclosing == nullptr && m_compiler->scope_depth == 0;
                                islocal = !(isexport || isprivate);
                                index = 0;
                                if(!isexport)
                                {
                                    index = isprivate ? resolve_private(funcstmt->name, funcstmt->length, statement->line) :
                                                      add_local(funcstmt->name, funcstmt->length, statement->line, false);
                                }
                                name = String::copy(m_state, funcstmt->name, funcstmt->length);
                                if(islocal)
                                {
                                    mark_local_initialized(index);
                                }
                                else if(isprivate)
                                {
                                    mark_private_initialized(index);
                                }
                                init_compiler(&compiler, LITFUNC_REGULAR);
                                begin_scope();
                                vararg = emit_parameters(&funcstmt->parameters, statement->line);
                                emit_statement(funcstmt->body);
                                end_scope(m_lastline);
                                function = end_compiler(name);
                                function->arg_count = funcstmt->parameters.m_count;
                                function->max_slots += function->arg_count;
                                function->vararg = vararg;
                                if(function->upvalue_count > 0)
                                {
                                    emit_op(m_lastline, OP_CLOSURE);
                                    emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                                    for(i = 0; i < function->upvalue_count; i++)
                                    {
                                        emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                                    }
                                }
                                else
                                {
                                    emit_constant(m_lastline, function->asValue());
                                }
                                if(isexport)
                                {
                                    emit_op(m_lastline, OP_SET_GLOBAL);
                                    emit_short(m_lastline, addConstant(m_lastline, name->asValue()));
                                }
                                else if(isprivate)
                                {
                                    emit_byte_or_short(m_lastline, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                                }
                                else
                                {
                                    emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                                }
                                emit_op(m_lastline, OP_POP);
                            }
                            break;
                        case Expression::Type::ReturnClause:
                            {
                                if(m_compiler->type == LITFUNC_CONSTRUCTOR)
                                {
                                    error(statement->line, Error::LITERROR_RETURN_FROM_CONSTRUCTOR);
                                }
                                expression = ((StmtReturn*)statement)->expression;
                                if(expression == nullptr)
                                {
                                    emit_op(m_lastline, OP_NULL);
                                }
                                else
                                {
                                    emit_expression(expression);
                                }
                                emit_op(m_lastline, OP_RETURN);
                                if(m_compiler->scope_depth == 0)
                                {
                                    m_compiler->skip_return = true;
                                }
                                return true;
                            }
                            break;
                        case Expression::Type::MethodDecl:
                            {
                                mthstmt = (StmtMethod*)statement;
                                constructor = mthstmt->name->length() == (sizeof(LIT_NAME_CONSTRUCTOR)-1) && memcmp(mthstmt->name->data(), LIT_NAME_CONSTRUCTOR, strlen(LIT_NAME_CONSTRUCTOR)-1) == 0;
                                if(constructor && mthstmt->is_static)
                                {
                                    error(statement->line, Error::LITERROR_STATIC_CONSTRUCTOR);
                                }
                                init_compiler(&compiler,
                                              constructor ? LITFUNC_CONSTRUCTOR : (mthstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD));
                                begin_scope();
                                vararg = emit_parameters(&mthstmt->parameters, statement->line);
                                emit_statement(mthstmt->body);
                                end_scope(m_lastline);
                                function = end_compiler(String::format(m_state, "@:@", m_classname->asValue(), mthstmt->name->asValue()));
                                function->arg_count = mthstmt->parameters.m_count;
                                function->max_slots += function->arg_count;
                                function->vararg = vararg;
                                if(function->upvalue_count > 0)
                                {
                                    emit_op(m_lastline, OP_CLOSURE);
                                    emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                                    for(i = 0; i < function->upvalue_count; i++)
                                    {
                                        emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                                    }
                                }
                                else
                                {
                                    emit_constant(m_lastline, function->asValue());
                                }
                                emit_op(m_lastline, mthstmt->is_static ? OP_STATIC_FIELD : OP_METHOD);
                                emit_short(m_lastline, addConstant(statement->line, mthstmt->name->asValue()));

                            }
                            break;
                        case Expression::Type::ClassDecl:
                            {
                                clstmt = (StmtClass*)statement;
                                m_classname = clstmt->name;
                                if(clstmt->parent != nullptr)
                                {
                                    emit_op(m_lastline, OP_GET_GLOBAL);
                                    emit_short(m_lastline, addConstant(m_lastline, clstmt->parent->asValue()));
                                }
                                emit_op(statement->line, OP_CLASS);
                                emit_short(m_lastline, addConstant(m_lastline, clstmt->name->asValue()));
                                if(clstmt->parent != nullptr)
                                {
                                    emit_op(m_lastline, OP_INHERIT);
                                    m_havesuperclass = true;
                                    begin_scope();
                                    super = add_local("super", 5, m_lastline, false);
                                    
                                    mark_local_initialized(super);
                                }
                                for(i = 0; i < clstmt->fields.m_count; i++)
                                {
                                    s = clstmt->fields.m_values[i];
                                    if(s->type == Expression::Type::VarDecl)
                                    {
                                        var = (StmtVar*)s;
                                        emit_expression(var->valexpr);
                                        emit_op(statement->line, OP_STATIC_FIELD);
                                        emit_short(statement->line,
                                                   addConstant(statement->line,
                                                                String::copy(m_state, var->name, var->length)->asValue()));
                                    }
                                    else
                                    {
                                        emit_statement(s);
                                    }
                                }
                                emit_op(m_lastline, OP_POP);
                                if(clstmt->parent != nullptr)
                                {
                                    end_scope(m_lastline);
                                }
                                m_classname = nullptr;
                                m_havesuperclass = false;
                            }
                            break;
                        case Expression::Type::FieldDecl:
                            {
                                fieldstmt = (StmtField*)statement;
                                getter = nullptr;
                                setter = nullptr;
                                if(fieldstmt->getter != nullptr)
                                {
                                    init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                                    begin_scope();
                                    emit_statement(fieldstmt->getter);
                                    end_scope(m_lastline);
                                    getter = end_compiler(String::format(m_state, "@:get @", m_classname->asValue(), fieldstmt->name));
                                }
                                if(fieldstmt->setter != nullptr)
                                {
                                    init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                                    mark_local_initialized(add_local("value", 5, statement->line, false));
                                    begin_scope();
                                    emit_statement(fieldstmt->setter);
                                    end_scope(m_lastline);
                                    setter = end_compiler(String::format(m_state, "@:set @", m_classname->asValue(), fieldstmt->name));
                                    setter->arg_count = 1;
                                    setter->max_slots++;
                                }
                                field = Field::make(m_state, fieldstmt->name, (Object*)getter, (Object*)setter);
                                emit_constant(statement->line, field->asValue());
                                emit_op(statement->line, fieldstmt->is_static ? OP_STATIC_FIELD : OP_DEFINE_FIELD);
                                emit_short(statement->line, addConstant(statement->line, fieldstmt->name->asValue()));
                            }
                            break;
                        default:
                            {
                                error(statement->line, Error::LITERROR_UNKNOWN_STATEMENT, (int)statement->type);
                            }
                            break;
                    }
                    m_prevwasexprstmt = statement->type == Expression::Type::Expression;
                    return false;
                }

                Module* run_emitter(Expression::List& statements, String* module_name);

        };
    }
    // endast
}

