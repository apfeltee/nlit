
#pragma once
#include "lit.h"

#define LIT_LONGEST_OP_NAME 13

#define LIT_OPCODE_SIZE 0x3f
#define LIT_A_ARG_SIZE 0xff
#define LIT_B_ARG_SIZE 0x1ff
#define LIT_C_ARG_SIZE 0x1ff
#define LIT_BX_ARG_SIZE 0x3ffff // 18 bits max
#define LIT_SBX_ARG_SIZE 0x1ffff // 17 bits max

#define LIT_A_ARG_POSITION 6
#define LIT_B_ARG_POSITION 14
#define LIT_C_ARG_POSITION 23
#define LIT_BX_ARG_POSITION 14
#define LIT_SBX_ARG_POSITION 15
#define LIT_SBX_FLAG_POSITION 14

/*
 * Instruction can follow one of the three formats:
 *
 * ABC  opcode:6 bits (starting from bit 0), A:8 bits, B:9 bits, C:9 bits
 * ABx  opcode:6 bits (starting from bit 0), A:8 bits, Bx:18 bits
 * AsBx opcode:6 bits (starting from bit 0), A:8 bits, sBx:18 bits (signed)
 */

#define LIT_INSTRUCTION_OPCODE(instruction) (instruction & LIT_OPCODE_SIZE)
#define LIT_INSTRUCTION_A(instruction) ((instruction >> LIT_A_ARG_POSITION) & LIT_A_ARG_SIZE)
#define LIT_INSTRUCTION_B(instruction) ((instruction >> LIT_B_ARG_POSITION) & LIT_B_ARG_SIZE)
#define LIT_INSTRUCTION_C(instruction) ((instruction >> LIT_C_ARG_POSITION) & LIT_C_ARG_SIZE)
#define LIT_INSTRUCTION_BX(instruction) ((instruction >> LIT_BX_ARG_POSITION) & LIT_BX_ARG_SIZE)
#define LIT_INSTRUCTION_SBX(instruction) (((instruction >> LIT_SBX_ARG_POSITION) & LIT_SBX_ARG_SIZE) \
	* (((instruction >> LIT_SBX_FLAG_POSITION) & 0x1) == 1 ? -1 : 1))

#define LIT_READ_ABC_INSTRUCTION(instruction) uint8_t a = LIT_INSTRUCTION_A(instruction); \
	uint16_t b = LIT_INSTRUCTION_B(instruction); \
	uint16_t c = LIT_INSTRUCTION_C(instruction);

#define LIT_READ_BX_INSTRUCTION(instruction) uint8_t a = LIT_INSTRUCTION_A(instruction); \
	uint32_t bx = LIT_INSTRUCTION_BX(instruction);

#define LIT_READ_SBX_INSTRUCTION(instruction) uint8_t a = LIT_INSTRUCTION_A(instruction); \
	int32_t sbx = LIT_INSTRUCTION_SBX(instruction);

#define LIT_FORM_ABC_INSTRUCTION(opcode, a, b, c) (((opcode) & LIT_OPCODE_SIZE) \
	| (((a) & LIT_A_ARG_SIZE) << LIT_A_ARG_POSITION) \
	| (((b) & LIT_B_ARG_SIZE) << LIT_B_ARG_POSITION) \
	| (((c) & LIT_C_ARG_SIZE) << LIT_C_ARG_POSITION))

#define LIT_FORM_ABX_INSTRUCTION(opcode, a, bx) (((opcode) & LIT_OPCODE_SIZE) \
	| (((a) & LIT_A_ARG_SIZE) << LIT_A_ARG_POSITION) \
	| (((bx) & LIT_BX_ARG_SIZE) << LIT_BX_ARG_POSITION))

#define LIT_FORM_ASBX_INSTRUCTION(opcode, a, sbx) (((opcode) & LIT_OPCODE_SIZE) \
	| (((a) & LIT_A_ARG_SIZE) << LIT_A_ARG_POSITION) \
	| ((abs((int) (sbx)) & LIT_SBX_ARG_SIZE) << LIT_SBX_ARG_POSITION)) \
	| ((((sbx) < 0 ? 1 : 0) << LIT_SBX_FLAG_POSITION))



namespace lit
{
    enum InstructionType
    {
        LIT_INSTRUCTION_ABC,
        LIT_INSTRUCTION_ABX,
        LIT_INSTRUCTION_ASBX,
    };

    enum OpCode
    {
        #define OPCODE(name, a) OP_##name,
        #include "opcode.inc"
        #undef OPCODE
    };

    enum TokenType
    {
        LITTOK_NEW_LINE,

        // Single-character tokens.
        LITTOK_LEFT_PAREN,
        LITTOK_RIGHT_PAREN,
        LITTOK_LEFT_BRACE,
        LITTOK_RIGHT_BRACE,
        LITTOK_LEFT_BRACKET,
        LITTOK_RIGHT_BRACKET,
        LITTOK_COMMA,
        LITTOK_SEMICOLON,
        LITTOK_COLON,

        // One or two character tokens.
        LITTOK_BAR_EQUAL,
        LITTOK_BAR,
        LITTOK_BAR_BAR,
        LITTOK_AMPERSAND_EQUAL,
        LITTOK_AMPERSAND,
        LITTOK_AMPERSAND_AMPERSAND,
        LITTOK_BANG,
        LITTOK_BANG_EQUAL,
        LITTOK_EQUAL,
        LITTOK_EQUAL_EQUAL,
        LITTOK_GREATER,
        LITTOK_GREATER_EQUAL,
        LITTOK_GREATER_GREATER,
        LITTOK_LESS,
        LITTOK_LESS_EQUAL,
        LITTOK_LESS_LESS,
        LITTOK_PLUS,
        LITTOK_PLUS_EQUAL,
        LITTOK_PLUS_PLUS,
        LITTOK_MINUS,
        LITTOK_MINUS_EQUAL,
        LITTOK_MINUS_MINUS,
        LITTOK_STAR,
        LITTOK_STAR_EQUAL,
        LITTOK_STAR_STAR,
        LITTOK_SLASH,
        LITTOK_SLASH_EQUAL,
        LITTOK_QUESTION,
        LITTOK_QUESTION_QUESTION,
        LITTOK_PERCENT,
        LITTOK_PERCENT_EQUAL,
        LITTOK_ARROW,
        LITTOK_SMALL_ARROW,
        LITTOK_TILDE,
        LITTOK_CARET,
        LITTOK_CARET_EQUAL,
        LITTOK_DOT,
        LITTOK_DOT_DOT,
        LITTOK_DOT_DOT_DOT,
        LITTOK_SHARP,
        LITTOK_SHARP_EQUAL,

        // erals.
        LITTOK_IDENTIFIER,
        LITTOK_STRING,
        LITTOK_INTERPOLATION,
        LITTOK_NUMBER,

        // Keywords.
        LITTOK_CLASS,
        LITTOK_ELSE,
        LITTOK_FALSE,
        LITTOK_FOR,
        LITTOK_FUNCTION,
        LITTOK_IF,
        LITTOK_NULL,
        LITTOK_RETURN,
        LITTOK_SUPER,
        LITTOK_THIS,
        LITTOK_TRUE,
        LITTOK_VAR,
        LITTOK_WHILE,
        LITTOK_CONTINUE,
        LITTOK_BREAK,
        LITTOK_NEW,
        LITTOK_EXPORT,
        LITTOK_IS,
        LITTOK_STATIC,
        LITTOK_OPERATOR,
        LITTOK_GET,
        LITTOK_SET,
        LITTOK_IN,
        LITTOK_CONST,
        LITTOK_REF,

        LITTOK_ERROR,
        LITTOK_EOF
    };

    enum Precedence
    {
        LITPREC_NONE,
        LITPREC_ASSIGNMENT,// =
        LITPREC_OR,// ||
        LITPREC_AND,// &&
        LITPREC_BOR,// | ^
        LITPREC_BAND,// &
        LITPREC_SHIFT,// << >>
        LITPREC_EQUALITY,// == !=
        LITPREC_COMPARISON,// < > <= >=
        LITPREC_COMPOUND,// += -= *= /= ++ --
        LITPREC_TERM,// + -
        LITPREC_FACTOR,// * /
        LITPREC_IS,// is
        LITPREC_RANGE,// ..
        LITPREC_UNARY,// ! - ~
        LITPREC_NULL,// ??
        LITPREC_CALL,// . ()
        LITPREC_PRIMARY
    };

    enum FunctionType
    {
        LITFUNC_REGULAR,
        LITFUNC_SCRIPT,
        LITFUNC_METHOD,
        LITFUNC_STATIC_METHOD,
        LITFUNC_CONSTRUCTOR
    };


    enum Optimization
    {
        LITOPTSTATE_CONSTANT_FOLDING,
        LITOPTSTATE_LITERAL_FOLDING,
        LITOPTSTATE_UNUSED_VAR,
        LITOPTSTATE_UNREACHABLE_CODE,
        LITOPTSTATE_EMPTY_BODY,
        LITOPTSTATE_LINE_INFO,
        LITOPTSTATE_PRIVATE_NAMES,
        LITOPTSTATE_C_FOR,

        LITOPTSTATE_TOTAL
    };

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
                static ExprLiteral* make(State* state, size_t line, Value value);

            public:
                Value value;
        };

        class ExprBinary: public Expression
        {
            public:
                static ExprBinary* make(State* state, size_t line, Expression* left, Expression* right, TokenType op);

            public:
                Expression* left;
                Expression* right;
                TokenType op;
                bool ignore_left;
        };

        class ExprUnary: public Expression
        {
            public:
                static ExprUnary* make(State* state, size_t line, Expression* right, TokenType op);

            public:
                Expression* right;
                TokenType op;
        };

        class ExprVar: public Expression
        {
            public:
                static ExprVar* make(State* state, size_t line, const char* name, size_t length);

            public:
                const char* name;
                size_t length;
        };

        class ExprAssign: public Expression
        {
            public:
                static ExprAssign* make(State* state, size_t line, Expression* to, Expression* value);

            public:
                Expression* to;
                Expression* value;
        };

        class ExprCall: public Expression
        {
            public:
                static ExprCall* make(State* state, size_t line, Expression* callee);

            public:
                Expression* callee;
                Expression::List args;
                Expression* objexpr;
        };

        class ExprIndexGet: public Expression
        {
            public:
                static ExprIndexGet* make(State* state, size_t line, Expression* where, const char* name, size_t length, bool questionable, bool ignore_result);

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
                static ExprIndexSet* make(State* state, size_t line, Expression* where, const char* name, size_t length, Expression* value);

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
                static ExprLambda* make(State* state, size_t line);

            public:
                PCGenericArray<ExprFuncParam> parameters;
                Expression* body;
        };

        class ExprArray: public Expression
        {
            public:
                static ExprArray* make(State* state, size_t line);

            public:
                Expression::List values;
        };

        class ExprObject: public Expression
        {
            public:
                static ExprObject* make(State* state, size_t line);

            public:
                PCGenericArray<Value> keys;
                Expression::List values;
        };

        class ExprSubscript: public Expression
        {
            public:
                static ExprSubscript* make(State* state, size_t line, Expression* array, Expression* index);

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
                static ExprSuper* make(State* state, size_t line, String* method, bool ignore_result);

            public:
                String* method;
                bool ignore_emit;
                bool ignore_result;
        };

        class ExprRange: public Expression
        {
            public:
                static ExprRange* make(State* state, size_t line, Expression* from, Expression* to);

            public:
                Expression* from;
                Expression* to;
        };

        class ExprIfClause: public Expression
        {
            public:
                static ExprIfClause* make(State* state, size_t line, Expression* condition, Expression* if_branch, Expression* else_branch);

            public:
                Expression* condition;
                Expression* if_branch;
                Expression* else_branch;
        };

        class ExprInterpolation: public Expression
        {
            public:
                static ExprInterpolation* make(State* state, size_t line);

            public:
                Expression::List expressions;
        };

        class ExprReference: public Expression
        {
            public:
                static ExprReference* make(State* state, size_t line, Expression* to);

            public:
                Expression* to;
        };

        class ExprStatement: public Expression
        {
            public:
                static ExprStatement* make(State* state, size_t line, Expression* expression);

            public:
                Expression* expression;
                bool pop;
        };

        class StmtBlock: public Expression
        {
            public:
                static StmtBlock* make(State* state, size_t line);

            public:
                Expression::List statements;
        };

        class StmtVar: public Expression
        {
            public:
                static StmtVar* make(State* state, size_t line, const char* name, size_t length, Expression* exprinit, bool constant);

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
                                                        Expression::List* elseif_branches);

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
                static StmtWhileLoop* make(State* state, size_t line, Expression* condition, Expression* body);

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
                                                          bool c_style);

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
                static StmtContinue* make(State* state, size_t line);

        };

        class StmtBreak: public Expression
        {
            public:
                static StmtBreak* make(State* state, size_t line);
        };

        class StmtFunction: public Expression
        {
            public:
                static StmtFunction* make(State* state, size_t line, const char* name, size_t length);

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
                static StmtMethod* make(State* state, size_t line, String* name, bool is_static);

            public:
                String* name;
                PCGenericArray<ExprFuncParam> parameters;
                Expression* body;
                bool is_static;
        };

        class StmtClass: public Expression
        {
            public:
                static StmtClass* make(State* state, size_t line, String* name, String* parent);

            public:
                String* name;
                String* parent;
                Expression::List fields;
        };

        class StmtField: public Expression
        {
            public:
                static StmtField* make(State* state, size_t line, String* name, Expression* getter, Expression* setter, bool is_static);

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
                static Expression* parse_block(Parser* parser);
                static Expression* parse_precedence(Parser* parser, Precedence precedence, bool err);
                static Expression* parse_number(Parser* parser, bool can_assign);
                static Expression* parse_lambda(Parser* parser, ExprLambda* lambda);
                static void parse_parameters(Parser* parser, PCGenericArray<ExprFuncParam>* parameters);
                static Expression* parse_grouping_or_lambda(Parser* parser, bool can_assign);
                static Expression* parse_call(Parser* parser, Expression* prev, bool can_assign);
                static Expression* parse_unary(Parser* parser, bool can_assign);
                static Expression* parse_binary(Parser* parser, Expression* prev, bool can_assign);
                static Expression* parse_and(Parser* parser, Expression* prev, bool can_assign);
                static Expression* parse_or(Parser* parser, Expression* prev, bool can_assign);
                static Expression* parse_null_filter(Parser* parser, Expression* prev, bool can_assign);
                static TokenType convert_compound_operator(TokenType op);
                static Expression* parse_compound(Parser* parser, Expression* prev, bool can_assign);
                static Expression* parse_literal(Parser* parser, bool can_assign);
                static Expression* parse_string(Parser* parser, bool can_assign);
                static Expression* parse_interpolation(Parser* parser, bool can_assign);
                static Expression* parse_object(Parser* parser, bool can_assign);
                static Expression* parse_variable_expression_base(Parser* parser, bool can_assign, bool isnew);
                static Expression* parse_variable_expression(Parser* parser, bool can_assign);
                static Expression* parse_new_expression(Parser* parser, bool can_assign);
                static Expression* parse_dot(Parser* parser, Expression* previous, bool can_assign);
                static Expression* parse_range(Parser* parser, Expression* previous, bool can_assign);
                static Expression* parse_ternary_or_question(Parser* parser, Expression* previous, bool can_assign);
                static Expression* parse_array(Parser* parser, bool can_assign);
                static Expression* parse_subscript(Parser* parser, Expression* previous, bool can_assign);
                static Expression* parse_this(Parser* parser, bool can_assign);
                static Expression* parse_super(Parser* parser, bool can_assign);
                static Expression* parse_reference(Parser* parser, bool can_assign);
                static Expression* parse_expression(Parser* parser);
                static Expression* parse_var_declaration(Parser* parser);
                static Expression* parse_if(Parser* parser);
                static Expression* parse_for(Parser* parser);
                static Expression* parse_while(Parser* parser);
                static Expression* parse_function(Parser* parser);
                static Expression* parse_return(Parser* parser);
                static Expression* parse_field(Parser* parser, String* name, bool is_static);
                static Expression* parse_method(Parser* parser, bool is_static);
                static Expression* parse_class(Parser* parser);
                static Expression* parse_statement(Parser* parser);
                static Expression* parse_declaration(Parser* parser);


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

                static void setup_rules();

            public:
                void init(State* state);

                void release();
                void end_compiler(Compiler* compiler);

                void begin_scope();
                void end_scope();
                ParseRule* get_rule(TokenType type);

                inline bool is_at_end()
                {
                    return m_currtoken.type == LITTOK_EOF;
                }

                //todo
                void stringError(Token* token, const char* message);

                void errorAt(Token* token, Error error, va_list args);
                void errorAtCurrent(Error error, ...);
                void raiseError(Error error, ...);
                void advance();
                bool check(TokenType type);
                bool match(TokenType type);
                bool match_new_line();
                void ignore_new_lines();
                void consume(TokenType type, const char* error);
                void sync();
                bool parse(const char* filename, const char* source, size_t length, Expression::List& statements);
        };


        class Optimizer
        {
            public:
                static void setup_optimization_states()
                {
                    Optimizer::set_level(LITOPTLEVEL_DEBUG);
                }

                static bool is_enabled(Optimization optimization);

                static void set_enabled(Optimization optimization, bool enabled);

                static void set_all_enabled(bool enabled);

                static void set_level(OptimizationLevel level);
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
                void init(State* state);
                bool is_empty(Expression* statement);
                Value evaluate_unary_op(Value value, TokenType op);
                Value evaluate_binary_op(Value a, Value b, TokenType op);
                void optimize(Expression::List* statements);
                void optdbg(const char* fmt, ...);
                void opt_begin_scope();
                void opt_end_scope();
                Variable* add_variable(const char* name, size_t length, bool constant, Expression** declaration);
                Variable* resolve_variable(const char* name, size_t length);
                Value attempt_to_optimize_binary(ExprBinary* expression, Value value, bool left);
                Value evaluate_expression(Expression* expression);
                void optimize_expression(Expression** slot);
                void optimize_expressions(Expression::List* expressions);
                void optimize_statement(Expression** slot);
                void optimize_statements(Expression::List* statements);
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
                void init(State* state);
                void release();
                void error(size_t line, Error error, ...);
                int8_t getStackEffect(int idx);
                void resolve_statements(Expression::List& statements);
                void emit_byte(uint16_t line, uint8_t byte);
                void emit_bytes(uint16_t line, uint8_t a, uint8_t b);
                void emit_op(uint16_t line, OpCode op);
                void emit_ops(uint16_t line, OpCode a, OpCode b);
                void emit_varying_op(uint16_t line, OpCode op, uint8_t arg);
                void emit_arged_op(uint16_t line, OpCode op, uint8_t arg);
                void emit_short(uint16_t line, uint16_t value);
                void emit_byte_or_short(uint16_t line, uint8_t a, uint8_t b, uint16_t index);
                const char* getStateScannerFilename();
                void init_compiler(Compiler* compiler, FunctionType type);
                void emit_return(size_t line);
                Function* end_compiler(String* name);
                void begin_scope();
                void end_scope(uint16_t line);
                uint16_t addConstant(size_t line, Value value);
                size_t emit_constant(size_t line, Value value);
                int add_private(const char* name, size_t length, size_t line, bool constant);
                int resolve_private(const char* name, size_t length, size_t line);
                int add_local(const char* name, size_t length, size_t line, bool constant);
                int resolve_local(Compiler* compiler, const char* name, size_t length, size_t line);
                int add_upvalue(Compiler* compiler, uint8_t index, size_t line, bool is_local);
                int resolve_upvalue(Compiler* compiler, const char* name, size_t length, size_t line);
                void mark_local_initialized(size_t index);
                void mark_private_initialized(size_t index);
                size_t emit_jump(OpCode code, size_t line);
                void patch_jump(size_t offset, size_t line);
                void emit_loop(size_t start, size_t line);
                void patch_loop_jumps(PCGenericArray<size_t>* breaks, size_t line);
                bool emit_parameters(PCGenericArray<ExprFuncParam>* parameters, size_t line);
                void resolve_statement(Expression* statement);
                void emit_expression(Expression* expression);
                bool emit_statement(Expression* statement);
                Module* run_emitter(Expression::List& statements, String* module_name);

        };
    }
    // endast
}


