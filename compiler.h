struct LitCompiler
{
    public:
        struct Parser;
        struct Scanner;

        struct Token
        {
            const char* start;
            LitTokenType type;
            size_t length;
            size_t line;
            LitValue value;
        };

        struct ASTNode
        {
            public:
                LitState* m_state;
                size_t line = 0;
        };

        struct ASTExpression: public ASTNode
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

                using List = PCGenericArray<ASTExpression*>;

            public:
                template<typename ClassT>
                static ClassT* make(LitState* state, uint64_t line, Type type)
                {
                    auto expr = Memory::allocate<ClassT>(state);
                    expr->m_state = state;
                    expr->type = type;
                    expr->line = line;
                    return expr;
                }

                static List* makeList(LitState* state)
                {
                    auto expressions = Memory::allocate<List>(state);
                    expressions->init(state);
                    return expressions;
                }

            public:
                Type type = Type::Unspecified;
        };

        struct ASTVariable
        {
            const char* name;
            size_t length;
            int depth;
            bool constant;
            bool used;
            LitValue constant_value;
            ASTExpression** declaration;
        };

        struct ASTParseRule
        {
            public:
                using PrefixFuncType = ASTExpression* (*)(Parser*, bool);
                using InfixFuncType = ASTExpression*(*)(Parser*, LitCompiler::ASTExpression*, bool);

            public:
                PrefixFuncType prefix;
                InfixFuncType infix;
                LitPrecedence precedence;
        };


        struct ASTExprLiteral: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprLiteral* make(LitState* state, size_t line, LitValue value)
                {
                    ASTExprLiteral* rt;
                    rt = LitCompiler::ASTExpression::make<ASTExprLiteral>(state, line, LitCompiler::ASTExpression::Type::Literal);
                    rt->value = value;
                    return rt;
                }

            public:
                LitValue value;
        };

        struct ASTExprBinary: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprBinary* make(LitState* state, size_t line, LitCompiler::ASTExpression* left, LitCompiler::ASTExpression* right, LitTokenType op)
                {
                    ASTExprBinary* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprBinary>(state, line, LitCompiler::ASTExpression::Type::Binary);
                    expression->left = left;
                    expression->right = right;
                    expression->op = op;
                    expression->ignore_left = false;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* left;
                LitCompiler::ASTExpression* right;
                LitTokenType op;
                bool ignore_left;
        };

        struct ASTExprUnary: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprUnary* make(LitState* state, size_t line, LitCompiler::ASTExpression* right, LitTokenType op)
                {
                    ASTExprUnary* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprUnary>(state, line, LitCompiler::ASTExpression::Type::Unary);
                    expression->right = right;
                    expression->op = op;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* right;
                LitTokenType op;
        };

        struct ASTExprVar: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprVar* make(LitState* state, size_t line, const char* name, size_t length)
                {
                    ASTExprVar* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprVar>(state, line, LitCompiler::ASTExpression::Type::Variable);
                    expression->name = name;
                    expression->length = length;
                    return expression;
                }

            public:
                const char* name;
                size_t length;
        };

        struct ASTExprAssign: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprAssign* make(LitState* state, size_t line, LitCompiler::ASTExpression* to, LitCompiler::ASTExpression* value)
                {
                    ASTExprAssign* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprAssign>(state, line, LitCompiler::ASTExpression::Type::Assign);
                    expression->to = to;
                    expression->value = value;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* to;
                LitCompiler::ASTExpression* value;
        };

        struct ASTExprCall: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprCall* make(LitState* state, size_t line, LitCompiler::ASTExpression* callee)
                {
                    ASTExprCall* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprCall>(state, line, LitCompiler::ASTExpression::Type::Call);
                    expression->callee = callee;
                    expression->objexpr = nullptr;
                    expression->args.init(state);
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* callee;
                LitCompiler::ASTExpression::List args;
                LitCompiler::ASTExpression* objexpr;
        };

        struct ASTExprIndexGet: public LitCompiler::ASTExpression
        {
            public:
               static ASTExprIndexGet* make(LitState* state, size_t line, LitCompiler::ASTExpression* where, const char* name, size_t length, bool questionable, bool ignore_result)
                {
                    ASTExprIndexGet* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprIndexGet>(state, line, LitCompiler::ASTExpression::Type::Get);
                    expression->where = where;
                    expression->name = name;
                    expression->length = length;
                    expression->ignore_emit = false;
                    expression->jump = questionable ? 0 : -1;
                    expression->ignore_result = ignore_result;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* where;
                const char* name;
                size_t length;
                int jump;
                bool ignore_emit;
                bool ignore_result;
        };

        struct ASTExprIndexSet: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprIndexSet* make(LitState* state, size_t line, LitCompiler::ASTExpression* where, const char* name, size_t length, LitCompiler::ASTExpression* value)
                {
                    ASTExprIndexSet* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprIndexSet>(state, line, LitCompiler::ASTExpression::Type::Set);
                    expression->where = where;
                    expression->name = name;
                    expression->length = length;
                    expression->value = value;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* where;
                const char* name;
                size_t length;
                LitCompiler::ASTExpression* value;
        };

        struct ASTExprFuncParam
        {
            const char* name;
            size_t length;
            LitCompiler::ASTExpression* default_value;
        };


        struct ASTExprLambda: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprLambda* make(LitState* state, size_t line)
                {
                    ASTExprLambda* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprLambda>(state, line, LitCompiler::ASTExpression::Type::Lambda);
                    expression->body = nullptr;
                    expression->parameters.init(state);
                    return expression;
                }

            public:
                PCGenericArray<ASTExprFuncParam> parameters;
                LitCompiler::ASTExpression* body;
        };

        struct ASTExprArray: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprArray* make(LitState* state, size_t line)
                {
                    ASTExprArray* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprArray>(state, line, LitCompiler::ASTExpression::Type::Array);
                    expression->values.init(state);
                    return expression;
                }

            public:
                LitCompiler::ASTExpression::List values;
        };

        struct ASTExprObject: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprObject* make(LitState* state, size_t line)
                {
                    ASTExprObject* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprObject>(state, line, LitCompiler::ASTExpression::Type::Object);
                    expression->keys.init(state);
                    expression->values.init(state);
                    return expression;
                }

            public:
                PCGenericArray<LitValue> keys;
                LitCompiler::ASTExpression::List values;
        };

        struct ASTExprSubscript: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprSubscript* make(LitState* state, size_t line, LitCompiler::ASTExpression* array, LitCompiler::ASTExpression* index)
                {
                    ASTExprSubscript* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprSubscript>(state, line, LitCompiler::ASTExpression::Type::Subscript);
                    expression->array = array;
                    expression->index = index;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* array;
                LitCompiler::ASTExpression* index;
        };

        struct ASTExprThis: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprThis* make(LitState* state, size_t line)
                {
                    return LitCompiler::ASTExpression::make<ASTExprThis>(state, line, LitCompiler::ASTExpression::Type::This);
                }

        };

        struct ASTExprSuper: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprSuper* make(LitState* state, size_t line, LitString* method, bool ignore_result)
                {
                    ASTExprSuper* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprSuper>(state, line, LitCompiler::ASTExpression::Type::Super);
                    expression->method = method;
                    expression->ignore_emit = false;
                    expression->ignore_result = ignore_result;
                    return expression;
                }

            public:
                LitString* method;
                bool ignore_emit;
                bool ignore_result;
        };

        struct ASTExprRange: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprRange* make(LitState* state, size_t line, LitCompiler::ASTExpression* from, LitCompiler::ASTExpression* to)
                {
                    ASTExprRange* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprRange>(state, line, LitCompiler::ASTExpression::Type::Range);
                    expression->from = from;
                    expression->to = to;
                    return expression;
                }

            public:
                LitCompiler::ASTExpression* from;
                LitCompiler::ASTExpression* to;
        };

        struct ASTExprIfClause: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprIfClause* make(LitState* state, size_t line, LitCompiler::ASTExpression* condition, LitCompiler::ASTExpression* if_branch, LitCompiler::ASTExpression* else_branch)
                {
                    ASTExprIfClause* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprIfClause>(state, line, LitCompiler::ASTExpression::Type::IfClause);
                    expression->condition = condition;
                    expression->if_branch = if_branch;
                    expression->else_branch = else_branch;

                    return expression;
                }

            public:
                LitCompiler::ASTExpression* condition;
                LitCompiler::ASTExpression* if_branch;
                LitCompiler::ASTExpression* else_branch;
        };

        struct ASTExprInterpolation: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprInterpolation* make(LitState* state, size_t line)
                {
                    ASTExprInterpolation* expression;
                    expression = LitCompiler::ASTExpression::make<ASTExprInterpolation>(state, line, LitCompiler::ASTExpression::Type::Interpolation);
                    expression->expressions.init(state);
                    return expression;
                }


            public:
                LitCompiler::ASTExpression::List expressions;
        };

        struct ASTExprReference: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprReference* make(LitState* state, size_t line, LitCompiler::ASTExpression* to)
                {
                    ASTExprReference* expr;
                    expr = LitCompiler::ASTExpression::make<ASTExprReference>(state, line, LitCompiler::ASTExpression::Type::Reference);
                    expr->to = to;
                    return expr;
                }

            public:
                LitCompiler::ASTExpression* to;
        };

        struct ASTExprStatement: public LitCompiler::ASTExpression
        {
            public:
                static ASTExprStatement* make(LitState* state, size_t line, LitCompiler::ASTExpression* expression)
                {
                    ASTExprStatement* statement;
                    statement = LitCompiler::ASTExpression::make<ASTExprStatement>(state, line, LitCompiler::ASTExpression::Type::Expression);
                    statement->expression = expression;
                    statement->pop = true;
                    return statement;
                }


            public:
                LitCompiler::ASTExpression* expression;
                bool pop;
        };

        struct ASTStmtBlock: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtBlock* make(LitState* state, size_t line)
                {
                    ASTStmtBlock* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtBlock>(state, line, LitCompiler::ASTExpression::Type::Block);
                    statement->statements.init(state);
                    return statement;
                }

            public:
                LitCompiler::ASTExpression::List statements;
        };

        struct ASTStmtVar: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtVar* make(LitState* state, size_t line, const char* name, size_t length, LitCompiler::ASTExpression* exprinit, bool constant)
                {
                    ASTStmtVar* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtVar>(state, line, LitCompiler::ASTExpression::Type::VarDecl);
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
                LitCompiler::ASTExpression* valexpr;
        };

        struct ASTStmtIfClause: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtIfClause* make(LitState* state,
                                                        size_t line,
                                                        LitCompiler::ASTExpression* condition,
                                                        LitCompiler::ASTExpression* if_branch,
                                                        LitCompiler::ASTExpression* else_branch,
                                                        LitCompiler::ASTExpression::List* elseif_conditions,
                                                        LitCompiler::ASTExpression::List* elseif_branches)
                {
                    ASTStmtIfClause* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtIfClause>(state, line, LitCompiler::ASTExpression::Type::IfClause);
                    statement->condition = condition;
                    statement->if_branch = if_branch;
                    statement->else_branch = else_branch;
                    statement->elseif_conditions = elseif_conditions;
                    statement->elseif_branches = elseif_branches;
                    return statement;
                }

            public:
                LitCompiler::ASTExpression* condition;
                LitCompiler::ASTExpression* if_branch;
                LitCompiler::ASTExpression* else_branch;
                LitCompiler::ASTExpression::List* elseif_conditions;
                LitCompiler::ASTExpression::List* elseif_branches;
        };

        struct ASTStmtWhileLoop: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtWhileLoop* make(LitState* state, size_t line, LitCompiler::ASTExpression* condition, LitCompiler::ASTExpression* body)
                {
                    ASTStmtWhileLoop* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtWhileLoop>(state, line, LitCompiler::ASTExpression::Type::WhileLoop);
                    statement->condition = condition;
                    statement->body = body;
                    return statement;
                }

            public:
                LitCompiler::ASTExpression* condition;
                LitCompiler::ASTExpression* body;
        };

        struct ASTStmtForLoop: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtForLoop* make(LitState* state,
                                                          size_t line,
                                                          LitCompiler::ASTExpression* exprinit,
                                                          LitCompiler::ASTExpression* var,
                                                          LitCompiler::ASTExpression* condition,
                                                          LitCompiler::ASTExpression* increment,
                                                          LitCompiler::ASTExpression* body,
                                                          bool c_style)
                {
                    ASTStmtForLoop* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtForLoop>(state, line, LitCompiler::ASTExpression::Type::ForLoop);
                    statement->exprinit = exprinit;
                    statement->var = var;
                    statement->condition = condition;
                    statement->increment = increment;
                    statement->body = body;
                    statement->c_style = c_style;
                    return statement;
                }


            public:
                LitCompiler::ASTExpression* exprinit;
                LitCompiler::ASTExpression* var;
                LitCompiler::ASTExpression* condition;
                LitCompiler::ASTExpression* increment;
                LitCompiler::ASTExpression* body;
                bool c_style;
        };

        struct ASTStmtContinue: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtContinue* make(LitState* state, size_t line)
                {
                    return LitCompiler::ASTExpression::make<ASTStmtContinue>(state, line, LitCompiler::ASTExpression::Type::ContinueClause);
                }

        };

        struct ASTStmtBreak: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtBreak* make(LitState* state, size_t line)
                {
                    return LitCompiler::ASTExpression::make<ASTStmtBreak>(state, line, LitCompiler::ASTExpression::Type::BreakClause);
                }
        };

        struct ASTStmtFunction: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtFunction* make(LitState* state, size_t line, const char* name, size_t length)
                {
                    ASTStmtFunction* function;
                    function = LitCompiler::ASTExpression::make<ASTStmtFunction>(state, line, LitCompiler::ASTExpression::Type::FunctionDecl);
                    function->name = name;
                    function->length = length;
                    function->body = nullptr;
                    function->parameters.init(state);
                    return function;
                }

            public:
                const char* name;
                size_t length;
                PCGenericArray<ASTExprFuncParam> parameters;
                LitCompiler::ASTExpression* body;
                bool exported;
        };

        struct ASTStmtReturn: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtReturn* make(LitState* state, size_t line, LitCompiler::ASTExpression* expression)
                {
                    ASTStmtReturn* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtReturn>(state, line, LitCompiler::ASTExpression::Type::ReturnClause);
                    statement->expression = expression;
                    return statement;
                }

            public:
                LitCompiler::ASTExpression* expression;
        };

        struct ASTStmtMethod: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtMethod* make(LitState* state, size_t line, LitString* name, bool is_static)
                {
                    ASTStmtMethod* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtMethod>(state, line, LitCompiler::ASTExpression::Type::MethodDecl);
                    statement->name = name;
                    statement->body = nullptr;
                    statement->is_static = is_static;
                    statement->parameters.init(state);
                    return statement;
                }

            public:
                LitString* name;
                PCGenericArray<ASTExprFuncParam> parameters;
                LitCompiler::ASTExpression* body;
                bool is_static;
        };

        struct ASTStmtClass: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtClass* make(LitState* state, size_t line, LitString* name, LitString* parent)
                {
                    ASTStmtClass* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtClass>(state, line, LitCompiler::ASTExpression::Type::ClassDecl);
                    statement->name = name;
                    statement->parent = parent;
                    statement->fields.init(state);
                    return statement;
                }

            public:
                LitString* name;
                LitString* parent;
                LitCompiler::ASTExpression::List fields;
        };

        struct ASTStmtField: public LitCompiler::ASTExpression
        {
            public:
                static ASTStmtField* make(LitState* state, size_t line, LitString* name, LitCompiler::ASTExpression* getter, LitCompiler::ASTExpression* setter, bool is_static)
                {
                    ASTStmtField* statement;
                    statement = LitCompiler::ASTExpression::make<ASTStmtField>(state, line, LitCompiler::ASTExpression::Type::FieldDecl);
                    statement->name = name;
                    statement->getter = getter;
                    statement->setter = setter;
                    statement->is_static = is_static;
                    return statement;
                }

            public:
                LitString* name;
                LitCompiler::ASTExpression* getter;
                LitCompiler::ASTExpression* setter;
                bool is_static;
        };

        struct Scanner
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
                size_t line;
                const char* start;
                const char* current;
                const char* file_name;
                LitState* m_state;
                size_t braces[LIT_MAX_INTERPOLATION_NESTING];
                size_t num_braces;
                bool had_error;

            public:
                void init(LitState* state, const char* file_name, const char* source)
                {
                    this->line = 1;
                    this->start = source;
                    this->current = source;
                    this->file_name = file_name;
                    this->m_state = state;
                    this->num_braces = 0;
                    this->had_error = false;
                }

                LitCompiler::Token make_token(LitTokenType type)
                {
                    LitCompiler::Token token;
                    token.type = type;
                    token.start = this->start;
                    token.length = (size_t)(this->current - this->start);
                    token.line = this->line;
                    return token;
                }

                LitCompiler::Token make_error_token(LitError error, ...)
                {
                    va_list args;
                    LitCompiler::Token token;
                    LitString* result;
                    this->had_error = true;
                    va_start(args, error);
                    result = lit_vformat_error(this->m_state, this->line, error, args);
                    va_end(args);
                    token.type = LITTOK_ERROR;
                    token.start = result->chars;
                    token.length = result->length();
                    token.line = this->line;
                    return token;
                }

                bool is_at_end()
                {
                    return *this->current == '\0';
                }

                char advance()
                {
                    this->current++;
                    return this->current[-1];
                }

                bool match(char expected)
                {
                    if(is_at_end())
                    {
                        return false;
                    }

                    if(*this->current != expected)
                    {
                        return false;
                    }
                    this->current++;
                    return true;
                }

                LitCompiler::Token match_token(char c, LitTokenType a, LitTokenType b)
                {
                    return make_token(match(c) ? a : b);
                }

                LitCompiler::Token match_tokens(char cr, char cb, LitTokenType a, LitTokenType b, LitTokenType c)
                {
                    return make_token(match(cr) ? a : (match(cb) ? b : c));
                }

                char peek()
                {
                    return *this->current;
                }

                char peek_next()
                {
                    if(is_at_end())
                    {
                        return '\0';
                    }
                    return this->current[1];
                }

                bool skip_whitespace()
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
                                    this->start = this->current;
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
                                                this->line++;
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

                LitCompiler::Token scan_string(bool interpolation)
                {
                    char c;
                    char newc;
                    char nextc;
                    int octval;
                    LitState* state;
                    PCGenericArray<uint8_t> bytes;
                    LitCompiler::Token token;
                    LitTokenType string_type;
                    state = this->m_state;
                    string_type = LITTOK_STRING;
                    bytes.init(this->m_state);
                    while(true)
                    {
                        c = advance();
                        if(c == '\"')
                        {
                            break;
                        }
                        else if(interpolation && c == '{')
                        {
                            if(this->num_braces >= LIT_MAX_INTERPOLATION_NESTING)
                            {
                                return make_error_token(LitError::LITERROR_INTERPOLATION_NESTING_TOO_DEEP, LIT_MAX_INTERPOLATION_NESTING);
                            }
                            string_type = LITTOK_INTERPOLATION;
                            this->braces[this->num_braces++] = 1;
                            break;
                        }
                        switch(c)
                        {
                            case '\0':
                                {
                                    return make_error_token(LitError::LITERROR_UNTERMINATED_STRING);
                                }
                                break;
                            case '\n':
                                {
                                    this->line++;
                                    bytes.push(c);
                                }
                                break;
                            case '\\':
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
                                                return make_error_token(LitError::LITERROR_INVALID_ESCAPE_CHAR, this->current[-1]);

                                                fprintf(stderr, "this->current[-1] = '%c', c = '%c'\n", this->current[-1], c);
                                                if(isdigit(this->current[-1]))
                                                {
                                                    //c*10 + (ls->current - '0')

                                                    /*
                                                    c = c*10 + (ls->current - '0');
                                                    if (lj_char_isdigit(next(ls)))
                                                    {
                                                        c = c*10 + (ls->current - '0');
                                                    */

                                                    newc = this->current[-1] - '0';
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
                                                    return make_error_token(LitError::LITERROR_INVALID_ESCAPE_CHAR, this->current[-1]);
                                                }
                                            }
                                            break;
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
                    token.value = LitString::copy(state, (const char*)bytes.m_values, bytes.m_count)->asValue();
                    bytes.release();
                    return token;
                }

                int scan_hexdigit()
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
                    this->current--;
                    return -1;
                }

                int scan_binarydigit()
                {
                    char c;
                    c = advance();
                    if(c >= '0' && c <= '1')
                    {
                        return c - '0';
                    }
                    this->current--;
                    return -1;
                }

                LitCompiler::Token make_number_token(bool is_hex, bool is_binary)
                {
                    LitCompiler::Token token;
                    LitValue value;
                    errno = 0;
                    if(is_hex)
                    {
                        value = LitObject::toValue((double)strtoll(this->start, nullptr, 16));
                    }
                    else if(is_binary)
                    {
                        value = LitObject::toValue((int)strtoll(this->start + 2, nullptr, 2));
                    }
                    else
                    {
                        value = LitObject::toValue(strtod(this->start, nullptr));
                    }

                    if(errno == ERANGE)
                    {
                        errno = 0;
                        return make_error_token(LitError::LITERROR_NUMBER_IS_TOO_BIG);
                    }
                    token = make_token(LITTOK_NUMBER);
                    token.value = value;
                    return token;
                }

                LitCompiler::Token scan_number()
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

                LitTokenType check_keyword(int start, int length, const char* rest, LitTokenType type)
                {
                    if(this->current - this->start == start + length && memcmp(this->start + start, rest, length) == 0)
                    {
                        return type;
                    }
                    return LITTOK_IDENTIFIER;
                }

                LitTokenType scan_identtype()
                {
                    switch(this->start[0])
                    {
                        case 'b':
                            return check_keyword(1, 4, "reak", LITTOK_BREAK);

                        case 'c':
                            {
                                if(this->current - this->start > 1)
                                {
                                    switch(this->start[1])
                                    {
                                        case 'l':
                                            return check_keyword(2, 3, "ass", LITTOK_CLASS);
                                        case 'o':
                                        {
                                            if(this->current - this->start > 3)
                                            {
                                                switch(this->start[3])
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
                                if(this->current - this->start > 1)
                                {
                                    switch(this->start[1])
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
                                if(this->current - this->start > 1)
                                {
                                    switch(this->start[1])
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
                                if(this->current - this->start > 1)
                                {
                                    switch(this->start[1])
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
                            if(this->current - this->start > 1)
                            {
                                switch(this->start[1])
                                {
                                    case 'u':
                                        return check_keyword(2, 2, "ll", LITTOK_NULL);
                                    case 'e':
                                        return check_keyword(2, 1, "w", LITTOK_NEW);
                                }
                            }

                            break;
                        }

                        case 'r':
                        {
                            if(this->current - this->start > 2)
                            {
                                switch(this->start[2])
                                {
                                    case 'f':
                                        return check_keyword(3, 0, "", LITTOK_REF);
                                    case 't':
                                        return check_keyword(3, 3, "urn", LITTOK_RETURN);
                                }
                            }

                            break;
                        }

                        case 'o':
                            return check_keyword(1, 7, "perator", LITTOK_OPERATOR);

                        case 's':
                        {
                            if(this->current - this->start > 1)
                            {
                                switch(this->start[1])
                                {
                                    case 'u':
                                        return check_keyword(2, 3, "per", LITTOK_SUPER);
                                    case 't':
                                        return check_keyword(2, 4, "atic", LITTOK_STATIC);
                                    case 'e':
                                        return check_keyword(2, 1, "t", LITTOK_SET);
                                }
                            }

                            break;
                        }

                        case 't':
                        {
                            if(this->current - this->start > 1)
                            {
                                switch(this->start[1])
                                {
                                    case 'h':
                                        return check_keyword(2, 2, "is", LITTOK_THIS);
                                    case 'r':
                                        return check_keyword(2, 2, "ue", LITTOK_TRUE);
                                }
                            }

                            break;
                        }

                        case 'v':
                            return check_keyword(1, 2, "ar", LITTOK_VAR);
                        case 'w':
                            return check_keyword(1, 4, "hile", LITTOK_WHILE);
                        case 'g':
                            return check_keyword(1, 2, "et", LITTOK_GET);
                    }

                    return LITTOK_IDENTIFIER;
                }

                LitCompiler::Token scan_identifier()
                {
                    while(char_is_alpha(peek()) || char_is_digit(peek()))
                    {
                        advance();
                    }

                    return make_token(scan_identtype());
                }

                LitCompiler::Token scan_token()
                {
                    if(skip_whitespace())
                    {
                        LitCompiler::Token token = make_token(LITTOK_NEW_LINE);
                        this->line++;

                        return token;
                    }

                    this->start = this->current;

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
                            if(this->num_braces > 0)
                            {
                                this->braces[this->num_braces - 1]++;
                            }

                            return make_token(LITTOK_LEFT_BRACE);
                        }

                        case '}':
                        {
                            if(this->num_braces > 0 && --this->braces[this->num_braces - 1] == 0)
                            {
                                this->num_braces--;
                                return scan_string(true);
                            }

                            return make_token(LITTOK_RIGHT_BRACE);
                        }

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
                            return match('>') ? make_token(LITTOK_SMALL_ARROW) :
                                                         match_tokens('=', '-', LITTOK_MINUS_EQUAL, LITTOK_MINUS_MINUS, LITTOK_MINUS);
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
                                return make_error_token(LitError::LITERROR_CHAR_EXPECTATION_UNMET, '\"', '$', peek());
                            }

                            return scan_string(true);
                        }

                        case '"':
                            return scan_string(false);
                    }

                    return make_error_token(LitError::LITERROR_UNEXPECTED_CHAR, c);
                }

        };

        struct Parser
        {
            public:
                static LitCompiler::ASTExpression* parse_block(LitCompiler::Parser* parser)
                {
                    ASTStmtBlock* statement;
                    parser->begin_scope();
                    statement = ASTStmtBlock::make(parser->m_state, parser->previous.line);
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
                    }
                    parser->ignore_new_lines();
                    parser->consume(LITTOK_RIGHT_BRACE, "'}'");
                    parser->ignore_new_lines();
                    parser->end_scope();
                    return (LitCompiler::ASTExpression*)statement;
                }

                static LitCompiler::ASTExpression* parse_precedence(LitCompiler::Parser* parser, LitPrecedence precedence, bool err)
                {
                    bool new_line;
                    bool prev_newline;
                    bool parser_prev_newline;
                    bool can_assign;
                    LitCompiler::ASTExpression* expr;
                    ASTParseRule::PrefixFuncType prefix_rule;
                    ASTParseRule::InfixFuncType infix_rule;
                    LitCompiler::Token previous;
                    (void)new_line;
                    prev_newline = false;
                    previous = parser->previous;
                    parser->advance();
                    prefix_rule = parser->get_rule(parser->previous.type)->prefix;
                    if(prefix_rule == nullptr)
                    {
                        //fprintf(stderr, "parser->previous.type=%s, parser->current.type=%s\n", token_name(parser->previous.type), token_name(parser->current.type));
                        if(parser->previous.type == LITTOK_SEMICOLON)
                        {
                            if(parser->current.type == LITTOK_NEW_LINE)
                            {
                                parser->advance();
                                return parse_precedence(parser, precedence, err);
                            }
                            return nullptr;
                        }
                        // todo: file start
                        new_line = previous.start != nullptr && *previous.start == '\n';
                        parser_prev_newline = parser->previous.start != nullptr && *parser->previous.start == '\n';
                        parser->raiseError(LitError::LITERROR_EXPECTED_EXPRESSION,
                            (prev_newline ? 8 : previous.length),
                            (prev_newline ? "new line" : previous.start),
                            (parser_prev_newline ? 8 : parser->previous.length),
                            (parser_prev_newline ? "new line" : parser->previous.start)
                        );
                        return nullptr;
                        
                    }
                    can_assign = precedence <= LITPREC_ASSIGNMENT;
                    expr = prefix_rule(parser, can_assign);
                    parser->ignore_new_lines();
                    while(precedence <= parser->get_rule(parser->current.type)->precedence)
                    {
                        parser->advance();
                        infix_rule = parser->get_rule(parser->previous.type)->infix;
                        expr = infix_rule(parser, expr, can_assign);
                    }
                    if(err && can_assign && parser->match(LITTOK_EQUAL))
                    {
                        parser->raiseError(LitError::LITERROR_INVALID_ASSIGMENT_TARGET);
                    }
                    return expr;
                }

                static LitCompiler::ASTExpression* parse_number(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    return (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, parser->previous.line, parser->previous.value);
                }

                static LitCompiler::ASTExpression* parse_lambda(LitCompiler::Parser* parser, ASTExprLambda* lambda)
                {
                    lambda->body = parse_statement(parser);
                    return (LitCompiler::ASTExpression*)lambda;
                }

                static void parse_parameters(LitCompiler::Parser* parser, PCGenericArray<ASTExprFuncParam>* parameters)
                {
                    bool had_default;
                    size_t arg_length;
                    const char* arg_name;
                    LitCompiler::ASTExpression* default_value;
                    had_default = false;
                    while(!parser->check(LITTOK_RIGHT_PAREN))
                    {
                        // Vararg ...
                        if(parser->match(LITTOK_DOT_DOT_DOT))
                        {
                            parameters->push(ASTExprFuncParam{ "...", 3, nullptr });
                            return;
                        }
                        parser->consume(LITTOK_IDENTIFIER, "argument name");
                        arg_name = parser->previous.start;
                        arg_length = parser->previous.length;
                        default_value = nullptr;
                        if(parser->match(LITTOK_EQUAL))
                        {
                            had_default = true;
                            default_value = parse_expression(parser);
                        }
                        else if(had_default)
                        {
                            parser->raiseError(LitError::LITERROR_DEFAULT_ARG_CENTRED);
                        }
                        parameters->push(ASTExprFuncParam{ arg_name, arg_length, default_value });
                        if(!parser->match(LITTOK_COMMA))
                        {
                            break;
                        }
                    }
                }

                static LitCompiler::ASTExpression* parse_grouping_or_lambda(LitCompiler::Parser* parser, bool can_assign)
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
                    LitCompiler::ASTExpression* expression;
                    LitCompiler::ASTExpression* default_value;
                    LitCompiler::Scanner* scanner;
                    (void)can_assign;
                    (void)had_arrow;
                    (void)had_array;
                    had_arrow = false;
                    if(parser->match(LITTOK_RIGHT_PAREN))
                    {
                        parser->consume(LITTOK_ARROW, "=> after lambda arguments");
                        return parse_lambda(parser, ASTExprLambda::make(parser->m_state, parser->previous.line));
                    }
                    start = parser->previous.start;
                    line = parser->previous.line;
                    if(parser->match(LITTOK_IDENTIFIER) || parser->match(LITTOK_DOT_DOT_DOT))
                    {
                        LitState* state = parser->m_state;
                        first_arg_start = parser->previous.start;
                        first_arg_length = parser->previous.length;
                        if(parser->match(LITTOK_COMMA) || (parser->match(LITTOK_RIGHT_PAREN) && parser->match(LITTOK_ARROW)))
                        {
                            had_array = parser->previous.type == LITTOK_ARROW;
                            had_vararg= parser->previous.type == LITTOK_DOT_DOT_DOT;
                            // This is a lambda
                            ASTExprLambda* lambda = ASTExprLambda::make(state, line);
                            LitCompiler::ASTExpression* def_value = nullptr;
                            had_default = parser->match(LITTOK_EQUAL);
                            if(had_default)
                            {
                                def_value = parse_expression(parser);
                            }
                            lambda->parameters.push(ASTExprFuncParam{ first_arg_start, first_arg_length, def_value });
                            if(!had_vararg && parser->previous.type == LITTOK_COMMA)
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

                                    arg_name = parser->previous.start;
                                    arg_length = parser->previous.length;
                                    default_value = nullptr;
                                    if(parser->match(LITTOK_EQUAL))
                                    {
                                        default_value = parse_expression(parser);
                                        had_default = true;
                                    }
                                    else if(had_default)
                                    {
                                        parser->raiseError(LitError::LITERROR_DEFAULT_ARG_CENTRED);
                                    }
                                    lambda->parameters.push(ASTExprFuncParam{ arg_name, arg_length, default_value });
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
                            scanner->current = start;
                            scanner->line = line;
                            parser->current = scanner->scan_token();
                            parser->advance();
                        }
                    }
                    expression = parse_expression(parser);
                    parser->consume(LITTOK_RIGHT_PAREN, "')' after grouping expression");
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_call(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    LitCompiler::ASTExpression* e;
                    ASTExprVar* ee;
                    ASTExprCall* expression;
                    expression = ASTExprCall::make(parser->m_state, parser->previous.line, prev);
                    while(!parser->check(LITTOK_RIGHT_PAREN))
                    {
                        e = parse_expression(parser);
                        expression->args.push(e);
                        if(!parser->match(LITTOK_COMMA))
                        {
                            break;
                        }
                        if(e->type == LitCompiler::ASTExpression::Type::Variable)
                        {
                            ee = (ASTExprVar*)e;
                            // Vararg ...
                            if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                            {
                                break;
                            }
                        }
                    }
                    if(expression->args.m_count > 255)
                    {
                        parser->raiseError(LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)expression->args.m_count);
                    }
                    parser->consume(LITTOK_RIGHT_PAREN, "')' after arguments");
                    return (LitCompiler::ASTExpression*)expression;
                }

                static LitCompiler::ASTExpression* parse_unary(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    LitCompiler::ASTExpression* expression;
                    LitTokenType op;
                    op = parser->previous.type;
                    line = parser->previous.line;
                    expression = parse_precedence(parser, LITPREC_UNARY, true);
                    return (LitCompiler::ASTExpression*)ASTExprUnary::make(parser->m_state, line, expression, op);
                }

                static LitCompiler::ASTExpression* parse_binary(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    bool invert;
                    size_t line;
                    ASTParseRule* rule;
                    LitCompiler::ASTExpression* expression;
                    LitTokenType op;
                    invert = parser->previous.type == LITTOK_BANG;
                    if(invert)
                    {
                        parser->consume(LITTOK_IS, "'is' after '!'");
                    }
                    op = parser->previous.type;
                    line = parser->previous.line;
                    rule = parser->get_rule(op);
                    expression = parse_precedence(parser, (LitPrecedence)(rule->precedence + 1), true);
                    expression = (LitCompiler::ASTExpression*)ASTExprBinary::make(parser->m_state, line, prev, expression, op);
                    if(invert)
                    {
                        expression = (LitCompiler::ASTExpression*)ASTExprUnary::make(parser->m_state, line, expression, LITTOK_BANG);
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_and(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    LitTokenType op;
                    op = parser->previous.type;
                    line = parser->previous.line;
                    return (LitCompiler::ASTExpression*)ASTExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_AND, true), op);
                }

                static LitCompiler::ASTExpression* parse_or(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    LitTokenType op;
                    op = parser->previous.type;
                    line = parser->previous.line;
                    return (LitCompiler::ASTExpression*)ASTExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_OR, true), op);
                }

                static LitCompiler::ASTExpression* parse_null_filter(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    LitTokenType op;
                    op = parser->previous.type;
                    line = parser->previous.line;
                    return (LitCompiler::ASTExpression*)ASTExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_NULL, true), op);
                }

                static LitTokenType convert_compound_operator(LitTokenType op)
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
                                UNREACHABLE
                            }
                            break;
                    }
                    return (LitTokenType)-1;
                }

                static LitCompiler::ASTExpression* parse_compound(LitCompiler::Parser* parser, LitCompiler::ASTExpression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    ASTExprBinary* binary;
                    LitCompiler::ASTExpression* expression;
                    ASTParseRule* rule;
                    LitTokenType op;
                    op = parser->previous.type;
                    line = parser->previous.line;
                    rule = parser->get_rule(op);
                    if(op == LITTOK_PLUS_PLUS || op == LITTOK_MINUS_MINUS)
                    {
                        expression = (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, line, LitObject::toValue(1));
                    }
                    else
                    {
                        expression = parse_precedence(parser, (LitPrecedence)(rule->precedence + 1), true);
                    }
                    binary = ASTExprBinary::make(parser->m_state, line, prev, expression, convert_compound_operator(op));
                    binary->ignore_left = true;// To make sure we don't free it twice
                    return (LitCompiler::ASTExpression*)ASTExprAssign::make(parser->m_state, line, prev, (LitCompiler::ASTExpression*)binary);
                }

                static LitCompiler::ASTExpression* parse_literal(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    line = parser->previous.line;
                    switch(parser->previous.type)
                    {
                        case LITTOK_TRUE:
                            {
                                return (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, line, LitObject::TrueVal);
                            }
                            break;
                        case LITTOK_FALSE:
                            {
                                return (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, line, LitObject::FalseVal);
                            }
                            break;
                        case LITTOK_NULL:
                            {
                                return (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, line, LitObject::NullVal);
                            }
                            break;
                        default:
                            {
                                UNREACHABLE
                            }
                            break;
                    }
                    return nullptr;
                }

                static LitCompiler::ASTExpression* parse_string(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    LitCompiler::ASTExpression* expression;
                    expression = (LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, parser->previous.line, parser->previous.value);
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_interpolation(LitCompiler::Parser* parser, bool can_assign)
                {
                    ASTExprInterpolation* expression;
                    (void)can_assign;
                    expression = ASTExprInterpolation::make(parser->m_state, parser->previous.line);
                    do
                    {
                        if(LitObject::as<LitString>(parser->previous.value)->length() > 0)
                        {
                            expression->expressions.push((LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, parser->previous.line, parser->previous.value));
                        }
                        expression->expressions.push(parse_expression(parser));
                    } while(parser->match(LITTOK_INTERPOLATION));
                    parser->consume(LITTOK_STRING, "end of interpolation");
                    if(LitObject::as<LitString>(parser->previous.value)->length() > 0)
                    {
                        expression->expressions.push((LitCompiler::ASTExpression*)ASTExprLiteral::make(parser->m_state, parser->previous.line, parser->previous.value));
                    }
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, (LitCompiler::ASTExpression*)expression, can_assign);
                    }
                    return (LitCompiler::ASTExpression*)expression;
                }

                static LitCompiler::ASTExpression* parse_object(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    ASTExprObject* objexpr;
                    objexpr = ASTExprObject::make(parser->m_state, parser->previous.line);
                    parser->ignore_new_lines();
                    while(!parser->check(LITTOK_RIGHT_BRACE))
                    {
                        parser->ignore_new_lines();
                        parser->consume(LITTOK_IDENTIFIER, "key string after '{'");
                        objexpr->keys.push(LitString::copy(parser->m_state, parser->previous.start, parser->previous.length)->asValue());
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
                    return (LitCompiler::ASTExpression*)objexpr;
                }

                static LitCompiler::ASTExpression* parse_variable_expression_base(LitCompiler::Parser* parser, bool can_assign, bool isnew)
                {
                    (void)can_assign;
                    bool had_args;
                    ASTExprCall* call;
                    LitCompiler::ASTExpression* expression;
                    expression = (LitCompiler::ASTExpression*)ASTExprVar::make(parser->m_state, parser->previous.line, parser->previous.start, parser->previous.length);
                    if(isnew)
                    {
                        had_args = parser->check(LITTOK_LEFT_PAREN);
                        call = nullptr;
                        if(had_args)
                        {
                            parser->advance();
                            call = (ASTExprCall*)parse_call(parser, expression, false);
                        }
                        if(parser->match(LITTOK_LEFT_BRACE))
                        {
                            if(call == nullptr)
                            {
                                call = ASTExprCall::make(parser->m_state, expression->line, expression);
                            }
                            call->objexpr = parse_object(parser, false);
                        }
                        else if(!had_args)
                        {
                            parser->errorAtCurrent(LitError::LITERROR_EXPECTATION_UNMET, "argument list for instance creation",
                                             parser->previous.length, parser->previous.start);
                        }
                        return (LitCompiler::ASTExpression*)call;
                    }
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    if(can_assign && parser->match(LITTOK_EQUAL))
                    {
                        return (LitCompiler::ASTExpression*)ASTExprAssign::make(parser->m_state, parser->previous.line, expression,
                                                                            parse_expression(parser));
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_variable_expression(LitCompiler::Parser* parser, bool can_assign)
                {
                    return parse_variable_expression_base(parser, can_assign, false);
                }

                static LitCompiler::ASTExpression* parse_new_expression(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    parser->consume(LITTOK_IDENTIFIER, "class name after 'new'");
                    return parse_variable_expression_base(parser, false, true);
                }

                static LitCompiler::ASTExpression* parse_dot(LitCompiler::Parser* parser, LitCompiler::ASTExpression* previous, bool can_assign)
                {
                    (void)can_assign;
                    bool ignored;
                    size_t line;
                    size_t length;
                    const char* name;
                    LitCompiler::ASTExpression* expression;
                    line = parser->previous.line;
                    ignored = parser->previous.type == LITTOK_SMALL_ARROW;
                    if(!(parser->match(LITTOK_CLASS) || parser->match(LITTOK_SUPER)))
                    {// class and super are allowed field names
                        parser->consume(LITTOK_IDENTIFIER, ignored ? "propety name after '->'" : "property name after '.'");
                    }
                    name = parser->previous.start;
                    length = parser->previous.length;
                    if(!ignored && can_assign && parser->match(LITTOK_EQUAL))
                    {
                        return (LitCompiler::ASTExpression*)ASTExprIndexSet::make(parser->m_state, line, previous, name, length, parse_expression(parser));
                    }
                    expression = (LitCompiler::ASTExpression*)ASTExprIndexGet::make(parser->m_state, line, previous, name, length, false, ignored);
                    if(!ignored && parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_range(LitCompiler::Parser* parser, LitCompiler::ASTExpression* previous, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    line = parser->previous.line;
                    return (LitCompiler::ASTExpression*)ASTExprRange::make(parser->m_state, line, previous, parse_expression(parser));
                }

                static LitCompiler::ASTExpression* parse_ternary_or_question(LitCompiler::Parser* parser, LitCompiler::ASTExpression* previous, bool can_assign)
                {
                    (void)can_assign;
                    bool ignored;
                    size_t line;
                    LitCompiler::ASTExpression* if_branch;
                    LitCompiler::ASTExpression* else_branch;
                    line = parser->previous.line;
                    if(parser->match(LITTOK_DOT) || parser->match(LITTOK_SMALL_ARROW))
                    {
                        ignored = parser->previous.type == LITTOK_SMALL_ARROW;
                        parser->consume(LITTOK_IDENTIFIER, ignored ? "property name after '->'" : "property name after '.'");
                        return (LitCompiler::ASTExpression*)ASTExprIndexGet::make(parser->m_state, line, previous, parser->previous.start,
                                                                         parser->previous.length, true, ignored);
                    }
                    if_branch = parse_expression(parser);
                    parser->consume(LITTOK_COLON, "':' after expression");
                    else_branch = parse_expression(parser);
                    return (LitCompiler::ASTExpression*)ASTExprIfClause::make(parser->m_state, line, previous, if_branch, else_branch);
                }

                static LitCompiler::ASTExpression* parse_array(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    ASTExprArray* array;
                    array = ASTExprArray::make(parser->m_state, parser->previous.line);
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
                        return parse_subscript(parser, (LitCompiler::ASTExpression*)array, can_assign);
                    }
                    return (LitCompiler::ASTExpression*)array;
                }

                static LitCompiler::ASTExpression* parse_subscript(LitCompiler::Parser* parser, LitCompiler::ASTExpression* previous, bool can_assign)
                {
                    size_t line;
                    LitCompiler::ASTExpression* index;
                    LitCompiler::ASTExpression* expression;
                    line = parser->previous.line;
                    index = parse_expression(parser);
                    parser->consume(LITTOK_RIGHT_BRACKET, "']' after subscript");
                    expression = (LitCompiler::ASTExpression*)ASTExprSubscript::make(parser->m_state, line, previous, index);
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    else if(can_assign && parser->match(LITTOK_EQUAL))
                    {
                        return (LitCompiler::ASTExpression*)ASTExprAssign::make(parser->m_state, parser->previous.line, expression, parse_expression(parser));
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_this(LitCompiler::Parser* parser, bool can_assign)
                {
                    LitCompiler::ASTExpression* expression;
                    expression = (LitCompiler::ASTExpression*)ASTExprThis::make(parser->m_state, parser->previous.line);
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_super(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    bool ignoring;
                    size_t line;
                    LitCompiler::ASTExpression* expression;
                    line = parser->previous.line;

                    if(!(parser->match(LITTOK_DOT) || parser->match(LITTOK_SMALL_ARROW)))
                    {
                        expression = (LitCompiler::ASTExpression*)ASTExprSuper::make(
                        parser->m_state, line, LitString::copy(parser->m_state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1), false);
                        parser->consume(LITTOK_LEFT_PAREN, "'(' after 'super'");
                        return parse_call(parser, expression, false);
                    }
                    ignoring = parser->previous.type == LITTOK_SMALL_ARROW;
                    parser->consume(LITTOK_IDENTIFIER, ignoring ? "super method name after '->'" : "super method name after '.'");
                    expression = (LitCompiler::ASTExpression*)ASTExprSuper::make(
                    parser->m_state, line, LitString::copy(parser->m_state, parser->previous.start, parser->previous.length), ignoring);
                    if(parser->match(LITTOK_LEFT_PAREN))
                    {
                        return parse_call(parser, expression, false);
                    }
                    return expression;
                }

                static LitCompiler::ASTExpression* parse_reference(LitCompiler::Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    ASTExprReference* expression;
                    line = parser->previous.line;
                    parser->ignore_new_lines();
                    expression = ASTExprReference::make(parser->m_state, line, parse_precedence(parser, LITPREC_CALL, false));
                    if(parser->match(LITTOK_EQUAL))
                    {
                        return (LitCompiler::ASTExpression*)ASTExprAssign::make(parser->m_state, line, (LitCompiler::ASTExpression*)expression, parse_expression(parser));
                    }
                    return (LitCompiler::ASTExpression*)expression;
                }

                static LitCompiler::ASTExpression* parse_expression(LitCompiler::Parser* parser)
                {
                    parser->ignore_new_lines();
                    return parse_precedence(parser, LITPREC_ASSIGNMENT, true);
                }

                static LitCompiler::ASTExpression* parse_var_declaration(LitCompiler::Parser* parser)
                {
                    bool constant;
                    size_t line;
                    size_t length;
                    const char* name;
                    LitCompiler::ASTExpression* vexpr;
                    constant = parser->previous.type == LITTOK_CONST;
                    line = parser->previous.line;
                    parser->consume(LITTOK_IDENTIFIER, "variable name");
                    name = parser->previous.start;
                    length = parser->previous.length;
                    vexpr = nullptr;
                    if(parser->match(LITTOK_EQUAL))
                    {
                        vexpr = parse_expression(parser);
                    }
                    return (LitCompiler::ASTExpression*)ASTStmtVar::make(parser->m_state, line, name, length, vexpr, constant);
                }

                static LitCompiler::ASTExpression* parse_if(LitCompiler::Parser* parser)
                {
                    size_t line;
                    bool invert;
                    bool had_paren;
                    LitCompiler::ASTExpression* condition;
                    LitCompiler::ASTExpression* if_branch;
                    LitCompiler::ASTExpression::List* elseif_conditions;
                    LitCompiler::ASTExpression::List* elseif_branches;
                    LitCompiler::ASTExpression* else_branch;
                    LitCompiler::ASTExpression* e;
                    line = parser->previous.line;
                    invert = parser->match(LITTOK_BANG);
                    had_paren = parser->match(LITTOK_LEFT_PAREN);
                    condition = parse_expression(parser);
                    if(had_paren)
                    {
                        parser->consume(LITTOK_RIGHT_PAREN, "')'");
                    }
                    if(invert)
                    {
                        condition = (LitCompiler::ASTExpression*)ASTExprUnary::make(parser->m_state, condition->line, condition, LITTOK_BANG);
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
                                elseif_conditions = LitCompiler::ASTExpression::makeList(parser->m_state);
                                elseif_branches = LitCompiler::ASTExpression::makeList(parser->m_state);
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
                                e = (LitCompiler::ASTExpression*)ASTExprUnary::make(parser->m_state, condition->line, e, LITTOK_BANG);
                            }
                            elseif_conditions->push(e);
                            elseif_branches->push(parse_statement(parser));
                            continue;
                        }
                        // else
                        if(else_branch != nullptr)
                        {
                            parser->raiseError(LitError::LITERROR_MULTIPLE_ELSE_BRANCHES);
                        }
                        parser->ignore_new_lines();
                        else_branch = parse_statement(parser);
                    }
                    return (LitCompiler::ASTExpression*)ASTStmtIfClause::make(parser->m_state, line, condition, if_branch, else_branch, elseif_conditions, elseif_branches);
                }

                static LitCompiler::ASTExpression* parse_for(LitCompiler::Parser* parser)
                {

                    bool c_style;
                    bool had_paren;
                    size_t line;
                    LitCompiler::ASTExpression* condition;
                    LitCompiler::ASTExpression* increment;
                    LitCompiler::ASTExpression* var;
                    LitCompiler::ASTExpression* exprinit;
                    line= parser->previous.line;
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
                            parser->raiseError(LitError::LITERROR_VAR_MISSING_IN_FORIN);
                        }
                    }
                    if(had_paren)
                    {
                        parser->consume(LITTOK_RIGHT_PAREN, "')'");
                    }
                    parser->ignore_new_lines();
                    return (LitCompiler::ASTExpression*)ASTStmtForLoop::make(parser->m_state, line, exprinit, var, condition, increment,
                                                                   parse_statement(parser), c_style);
                }

                static LitCompiler::ASTExpression* parse_while(LitCompiler::Parser* parser)
                {
                    bool had_paren;
                    size_t line;
                    LitCompiler::ASTExpression* body;
                    line = parser->previous.line;
                    had_paren = parser->match(LITTOK_LEFT_PAREN);
                    LitCompiler::ASTExpression* condition = parse_expression(parser);
                    if(had_paren)
                    {
                        parser->consume(LITTOK_RIGHT_PAREN, "')'");
                    }
                    parser->ignore_new_lines();
                    body = parse_statement(parser);
                    return (LitCompiler::ASTExpression*)ASTStmtWhileLoop::make(parser->m_state, line, condition, body);
                }

                static LitCompiler::ASTExpression* parse_function(LitCompiler::Parser* parser)
                {
                    size_t line;
                    size_t function_length;
                    bool isexport;
                    const char* function_name;
                    LitCompiler compiler;
                    ASTStmtFunction* function;
                    ASTExprLambda* lambda;
                    ASTExprIndexSet* to;
                    isexport = parser->previous.type == LITTOK_EXPORT;
                    if(isexport)
                    {
                        parser->consume(LITTOK_FUNCTION, "'function' after 'export'");
                    }
                    line = parser->previous.line;
                    parser->consume(LITTOK_IDENTIFIER, "function name");
                    function_name = parser->previous.start;
                    function_length = parser->previous.length;
                    if(parser->match(LITTOK_DOT))
                    {
                        parser->consume(LITTOK_IDENTIFIER, "function name");
                        lambda = ASTExprLambda::make(parser->m_state, line);
                        to = ASTExprIndexSet::make(
                        parser->m_state, line, (LitCompiler::ASTExpression*)ASTExprVar::make(parser->m_state, line, function_name, function_length),
                        parser->previous.start, parser->previous.length, (LitCompiler::ASTExpression*)lambda);
                        parser->consume(LITTOK_LEFT_PAREN, "'(' after function name");
                        compiler.init(parser);
                        parser->begin_scope();
                        parse_parameters(parser, &lambda->parameters);
                        if(lambda->parameters.m_count > 255)
                        {
                            parser->raiseError(LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)lambda->parameters.m_count);
                        }
                        parser->consume(LITTOK_RIGHT_PAREN, "')' after function arguments");
                        parser->ignore_new_lines();
                        lambda->body = parse_statement(parser);
                        parser->end_scope();
                        parser->end_compiler(&compiler);
                        return (LitCompiler::ASTExpression*)ASTExprStatement::make(parser->m_state, line, (LitCompiler::ASTExpression*)to);
                    }
                    function = ASTStmtFunction::make(parser->m_state, line, function_name, function_length);
                    function->exported = isexport;
                    parser->consume(LITTOK_LEFT_PAREN, "'(' after function name");
                    compiler.init(parser);
                    parser->begin_scope();
                    parse_parameters(parser, &function->parameters);
                    if(function->parameters.m_count > 255)
                    {
                        parser->raiseError(LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)function->parameters.m_count);
                    }
                    parser->consume(LITTOK_RIGHT_PAREN, "')' after function arguments");
                    function->body = parse_statement(parser);
                    parser->end_scope();
                    parser->end_compiler(&compiler);
                    return (LitCompiler::ASTExpression*)function;
                }

                static LitCompiler::ASTExpression* parse_return(LitCompiler::Parser* parser)
                {
                    size_t line;
                    LitCompiler::ASTExpression* expression;
                    line = parser->previous.line;
                    expression = nullptr;
                    if(!parser->check(LITTOK_NEW_LINE) && !parser->check(LITTOK_RIGHT_BRACE))
                    {
                        expression = parse_expression(parser);
                    }
                    return (LitCompiler::ASTExpression*)ASTStmtReturn::make(parser->m_state, line, expression);
                }

                static LitCompiler::ASTExpression* parse_field(LitCompiler::Parser* parser, LitString* name, bool is_static)
                {
                    size_t line;
                    LitCompiler::ASTExpression* getter;
                    LitCompiler::ASTExpression* setter;
                    line = parser->previous.line;
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
                            parser->raiseError(LitError::LITERROR_NO_GETTER_AND_SETTER);
                        }
                        parser->ignore_new_lines();
                        parser->consume(LITTOK_RIGHT_BRACE, "'}' after field declaration");
                    }
                    return (LitCompiler::ASTExpression*)ASTStmtField::make(parser->m_state, line, name, getter, setter, is_static);
                }

                static LitCompiler::ASTExpression* parse_method(LitCompiler::Parser* parser, bool is_static)
                {
                    size_t i;
                    LitCompiler compiler;
                    ASTStmtMethod* method;
                    LitString* name;
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
                            parser->raiseError(LitError::LITERROR_STATIC_OPERATOR);
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
                        if(parser->previous.type == LITTOK_LEFT_BRACKET)
                        {
                            parser->consume(LITTOK_RIGHT_BRACKET, "']' after '[' in op method declaration");
                            name = LitString::copy(parser->m_state, "[]", 2);
                        }
                        else
                        {
                            name = LitString::copy(parser->m_state, parser->previous.start, parser->previous.length);
                        }
                    }
                    else
                    {
                        parser->consume(LITTOK_IDENTIFIER, "method name");
                        name = LitString::copy(parser->m_state, parser->previous.start, parser->previous.length);
                        if(parser->check(LITTOK_LEFT_BRACE) || parser->check(LITTOK_ARROW))
                        {
                            return parse_field(parser, name, is_static);
                        }
                    }
                    method = ASTStmtMethod::make(parser->m_state, parser->previous.line, name, is_static);
                    compiler.init(parser);
                    parser->begin_scope();
                    parser->consume(LITTOK_LEFT_PAREN, "'(' after method name");
                    parse_parameters(parser, &method->parameters);
                    if(method->parameters.m_count > 255)
                    {
                        parser->raiseError(LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)method->parameters.m_count);
                    }
                    parser->consume(LITTOK_RIGHT_PAREN, "')' after method arguments");
                    method->body = parse_statement(parser);
                    parser->end_scope();
                    parser->end_compiler(&compiler);
                    return (LitCompiler::ASTExpression*)method;
                }

                static LitCompiler::ASTExpression* parse_class(LitCompiler::Parser* parser)
                {
                    bool finished_parsing_fields;
                    bool field_is_static;
                    size_t line;
                    bool is_static;
                    LitString* name;
                    LitString* super;
                    ASTStmtClass* klass;
                    LitCompiler::ASTExpression* var;
                    LitCompiler::ASTExpression* method;
                    if(setjmp(prs_jmpbuffer))
                    {
                        return nullptr;
                    }
                    line = parser->previous.line;
                    is_static = parser->previous.type == LITTOK_STATIC;
                    if(is_static)
                    {
                        parser->consume(LITTOK_CLASS, "'class' after 'static'");
                    }
                    parser->consume(LITTOK_IDENTIFIER, "class name after 'class'");
                    name = LitString::copy(parser->m_state, parser->previous.start, parser->previous.length);
                    super = nullptr;
                    if(parser->match(LITTOK_COLON))
                    {
                        parser->consume(LITTOK_IDENTIFIER, "super class name after ':'");
                        super = LitString::copy(parser->m_state, parser->previous.start, parser->previous.length);
                        if(super == name)
                        {
                            parser->raiseError(LitError::LITERROR_SELF_INHERITED_CLASS);
                        }
                    }
                    klass = ASTStmtClass::make(parser->m_state, line, name, super);
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
                                    parser->raiseError(LitError::LITERROR_STATIC_FIELDS_AFTER_METHODS);
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
                    return (LitCompiler::ASTExpression*)klass;
                }



                static LitCompiler::ASTExpression* parse_statement(LitCompiler::Parser* parser)
                {
                    LitCompiler::ASTExpression* expression;
                    parser->ignore_new_lines();
                    if(setjmp(prs_jmpbuffer))
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
                        return (LitCompiler::ASTExpression*)ASTStmtContinue::make(parser->m_state, parser->previous.line);
                    }
                    else if(parser->match(LITTOK_BREAK))
                    {
                        return (LitCompiler::ASTExpression*)ASTStmtBreak::make(parser->m_state, parser->previous.line);
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
                    expression = parse_expression(parser);
                    return expression == nullptr ? nullptr : (LitCompiler::ASTExpression*)ASTExprStatement::make(parser->m_state, parser->previous.line, expression);
                }

                static LitCompiler::ASTExpression* parse_declaration(LitCompiler::Parser* parser)
                {
                    LitCompiler::ASTExpression* statement;
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


            public:
                LitState* m_state;
                bool had_error;
                bool panic_mode;
                Token previous;
                Token current;
                LitCompiler* compiler;
                uint8_t expression_root_count;
                uint8_t statement_root_count;

            public:
                static ASTParseRule rules[LITTOK_EOF + 1];

            private:
                Scanner* getStateScanner(LitState* state);

                static void setup_rules()
                {
                    Parser::rules[LITTOK_LEFT_PAREN] = ASTParseRule{ parse_grouping_or_lambda, parse_call, LITPREC_CALL };
                    Parser::rules[LITTOK_PLUS] = ASTParseRule{ nullptr, parse_binary, LITPREC_TERM };
                    Parser::rules[LITTOK_MINUS] = ASTParseRule{ parse_unary, parse_binary, LITPREC_TERM };
                    Parser::rules[LITTOK_BANG] = ASTParseRule{ parse_unary, parse_binary, LITPREC_TERM };
                    Parser::rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_STAR_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_SLASH] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_SHARP] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_BAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_BOR };
                    Parser::rules[LITTOK_AMPERSAND] = ASTParseRule{ nullptr, parse_binary, LITPREC_BAND };
                    Parser::rules[LITTOK_TILDE] = ASTParseRule{ parse_unary, nullptr, LITPREC_UNARY };
                    Parser::rules[LITTOK_CARET] = ASTParseRule{ nullptr, parse_binary, LITPREC_BOR };
                    Parser::rules[LITTOK_LESS_LESS] = ASTParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
                    Parser::rules[LITTOK_GREATER_GREATER] = ASTParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
                    Parser::rules[LITTOK_PERCENT] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
                    Parser::rules[LITTOK_IS] = ASTParseRule{ nullptr, parse_binary, LITPREC_IS };
                    Parser::rules[LITTOK_NUMBER] = ASTParseRule{ parse_number, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_TRUE] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_FALSE] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_NULL] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_BANG_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
                    Parser::rules[LITTOK_EQUAL_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
                    Parser::rules[LITTOK_GREATER] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
                    Parser::rules[LITTOK_GREATER_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
                    Parser::rules[LITTOK_LESS] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
                    Parser::rules[LITTOK_LESS_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
                    Parser::rules[LITTOK_STRING] = ASTParseRule{ parse_string, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_INTERPOLATION] = ASTParseRule{ parse_interpolation, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_IDENTIFIER] = ASTParseRule{ parse_variable_expression, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_NEW] = ASTParseRule{ parse_new_expression, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_PLUS_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_MINUS_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_STAR_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_SLASH_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_SHARP_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_PERCENT_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_CARET_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_BAR_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_AMPERSAND_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_PLUS_PLUS] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_MINUS_MINUS] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
                    Parser::rules[LITTOK_AMPERSAND_AMPERSAND] = ASTParseRule{ nullptr, parse_and, LITPREC_AND };
                    Parser::rules[LITTOK_BAR_BAR] = ASTParseRule{ nullptr, parse_or, LITPREC_AND };
                    Parser::rules[LITTOK_QUESTION_QUESTION] = ASTParseRule{ nullptr, parse_null_filter, LITPREC_NULL };
                    Parser::rules[LITTOK_DOT] = ASTParseRule{ nullptr, parse_dot, LITPREC_CALL };
                    Parser::rules[LITTOK_SMALL_ARROW] = ASTParseRule{ nullptr, parse_dot, LITPREC_CALL };
                    Parser::rules[LITTOK_DOT_DOT] = ASTParseRule{ nullptr, parse_range, LITPREC_RANGE };
                    Parser::rules[LITTOK_DOT_DOT_DOT] = ASTParseRule{ parse_variable_expression, nullptr, LITPREC_ASSIGNMENT };
                    Parser::rules[LITTOK_LEFT_BRACKET] = ASTParseRule{ parse_array, parse_subscript, LITPREC_NONE };
                    Parser::rules[LITTOK_LEFT_BRACE] = ASTParseRule{ parse_object, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_THIS] = ASTParseRule{ parse_this, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_SUPER] = ASTParseRule{ parse_super, nullptr, LITPREC_NONE };
                    Parser::rules[LITTOK_QUESTION] = ASTParseRule{ nullptr, parse_ternary_or_question, LITPREC_EQUALITY };
                    Parser::rules[LITTOK_REF] = ASTParseRule{ parse_reference, nullptr, LITPREC_NONE };
                    //Parser::rules[LITTOK_SEMICOLON] = ASTParseRule{nullptr, nullptr, LITPREC_NONE};
                }

            public:
                void init(LitState* state);

                void release()
                {
                }

                void end_compiler(LitCompiler* compiler)
                {
                    this->compiler = (LitCompiler*)compiler->enclosing;
                }

                void begin_scope()
                {
                    this->compiler->scope_depth++;
                }

                void end_scope()
                {
                    this->compiler->scope_depth--;
                }

                LitCompiler::ASTParseRule* get_rule(LitTokenType type)
                {
                    return &Parser::rules[type];
                }

                inline bool is_at_end()
                {
                    return this->current.type == LITTOK_EOF;
                }

                //todo
                void stringError(LitCompiler::Token* token, const char* message);

                void errorAt(LitCompiler::Token* token, LitError error, va_list args)
                {
                    stringError(token, lit_vformat_error(this->m_state, token->line, error, args)->chars);
                }

                void errorAtCurrent(LitError error, ...)
                {
                    va_list args;
                    va_start(args, error);
                    errorAt(&this->current, error, args);
                    va_end(args);
                }

                void raiseError(LitError error, ...)
                {
                    va_list args;
                    va_start(args, error);
                    errorAt(&this->previous, error, args);
                    va_end(args);
                }

                void advance()
                {
                    this->previous = this->current;
                    while(true)
                    {
                        this->current = getStateScanner(this->m_state)->scan_token();
                        if(this->current.type != LITTOK_ERROR)
                        {
                            break;
                        }
                        stringError(&this->current, this->current.start);
                    }
                }

                bool check(LitTokenType type)
                {
                    return this->current.type == type;
                }

                bool match(LitTokenType type)
                {
                    if(this->current.type == type)
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

                void consume(LitTokenType type, const char* error)
                {
                    bool line;
                    size_t olen;
                    const char* fmt;
                    const char* otext;
                    if(this->current.type == type)
                    {
                        advance();
                        return;
                    }
                    line = this->previous.type == LITTOK_NEW_LINE;
                    olen = (line ? 8 : this->previous.length);
                    otext = (line ? "new line" : this->previous.start);
                    fmt = lit_format_error(this->m_state, this->current.line, LitError::LITERROR_EXPECTATION_UNMET, error, olen, otext)->chars;
                    stringError(&this->current,fmt);
                }

                void sync()
                {
                    this->panic_mode = false;
                    while(this->current.type != LITTOK_EOF)
                    {
                        if(this->previous.type == LITTOK_NEW_LINE)
                        {
                            longjmp(prs_jmpbuffer, 1);
                            return;
                        }
                        switch(this->current.type)
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
                                longjmp(prs_jmpbuffer, 1);
                                return;
                            }
                            default:
                            {
                                advance();
                            }
                        }
                    }
                }

                bool parse(const char* file_name, const char* source, LitCompiler::ASTExpression::List& statements)
                {
                    LitCompiler compiler;
                    LitCompiler::ASTExpression* statement;
                    this->had_error = false;
                    this->panic_mode = false;
                    this->getStateScanner(this->m_state)->init(this->m_state, file_name, source);
                    compiler.init(this);
                    this->advance();
                    this->ignore_new_lines();
                    if(!this->is_at_end())
                    {
                        do
                        {
                            statement = LitCompiler::Parser::parse_declaration(this);
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
                    return this->had_error || this->getStateScanner(this->m_state)->had_error;
                }

        };

        struct Optimizer
        {
            public:
                LitState* m_state;
                PCGenericArray<LitCompiler::ASTVariable> variables;
                int depth;
                bool mark_used;

            public:
                void init(LitState* state)
                {
                    this->m_state = state;
                    this->depth = -1;
                    this->mark_used = false;
                    this->variables.init(state);
                }

        };

    public:
        PCGenericArray<LitLocal> locals;
        int scope_depth;
        LitFunction* function;
        LitFunctionType type;
        LitCompilerUpvalue upvalues[UINT8_COUNT];
        LitCompiler* enclosing;
        bool skip_return;
        size_t loop_depth;
        int slots;
        int max_slots;

    public:
        void init(Parser* parser)
        {
            this->scope_depth = 0;
            this->function = nullptr;
            this->enclosing = (LitCompiler*)parser->compiler;
            parser->compiler = this;
        }
};
