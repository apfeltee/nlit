
#pragma once
#include <string_view>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>
#include <limits.h>

#if defined(__unix__) || defined(__linux__)
    #include <dirent.h>
#endif

#ifndef __TINYC__
    #if __has_include(<readline/readline.h>)
        #include <readline/readline.h>
        #include <readline/history.h>
        #define LIT_HAVE_READLINE
    #endif
#endif

#include "sds.h"


#define LIT_REPOSITORY "https://github.com/egordorichev/lit"

#define LIT_VERSION_MAJOR 0
#define LIT_VERSION_MINOR 1
#define LIT_VERSION_STRING "0.1"
#define LIT_BYTECODE_VERSION 0

#define TESTING
// #define DEBUG

#ifdef DEBUG
    #define LIT_TRACE_EXECUTION
    #define LIT_TRACE_STACK
    #define LIT_CHECK_STACK_SIZE
// #define LIT_TRACE_CHUNK
// #define LIT_MINIMIZE_CONTAINERS
// #define LIT_LOG_GC
// #define LIT_LOG_ALLOCATION
// #define LIT_LOG_MARKING
// #define LIT_LOG_BLACKING
// #define LIT_STRESS_TEST_GC
#endif

#ifdef TESTING
    // So that we can actually test the map contents with a single-line expression
    #define SINGLE_LINE_MAPS
    #define SINGLE_LINE_MAPS_ENABLED true

// Make sure that we did not break anything
// #define LIT_STRESS_TEST_GC
#else
    #define SINGLE_LINE_MAPS_ENABLED false
#endif

#define LIT_MAX_INTERPOLATION_NESTING 4

#define LIT_GC_HEAP_GROW_FACTOR 2
#define LIT_CALL_FRAMES_MAX (1024*8)
#define LIT_INITIAL_CALL_FRAMES 128
#define LIT_CONTAINER_OUTPUT_MAX 10


#if defined(__ANDROID__) || defined(_ANDROID_)
    #define LIT_OS_ANDROID
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define LIT_OS_WINDOWS
#elif __APPLE__
    #define LIT_OS_MAC
    #define LIT_OS_UNIX_LIKE
#elif __linux__
    #define LIT_OS_LINUX
    #define LIT_OS_UNIX_LIKE
#else
    #define LIT_OS_UNKNOWN
#endif

#ifdef LIT_OS_UNIX_LIKE
    #define LIT_USE_LIBREADLINE
#endif

#ifdef LIT_USE_LIBREADLINE
#else
    #define LIT_REPL_INPUT_MAX 1024
#endif

#define UNREACHABLE assert(false);
#define UINT8_COUNT UINT8_MAX + 1
#define UINT16_COUNT UINT16_MAX + 1

#define TABLE_MAX_LOAD 0.75
// Do not change these, or old bytecode files will break!
#define LIT_BYTECODE_MAGIC_NUMBER 6932
#define LIT_BYTECODE_END_NUMBER 2942
#define LIT_STRING_KEY 48



#define TAG_NULL 1u
#define TAG_FALSE 2u
#define TAG_TRUE 3u

#define LIT_EXIT_CODE_ARGUMENT_ERROR 1
#define LIT_EXIT_CODE_MEM_LEAK 2
#define LIT_EXIT_CODE_RUNTIME_ERROR 70
#define LIT_EXIT_CODE_COMPILE_ERROR 65

#define LIT_TESTS_DIRECTORY "tests"


#if !defined(LIT_DISABLE_COLOR) && !defined(LIT_ENABLE_COLOR) && !(defined(LIT_OS_WINDOWS) || defined(EMSCRIPTEN))
    #define LIT_ENABLE_COLOR
#endif

#ifdef LIT_ENABLE_COLOR
    #define COLOR_RESET "\x1B[0m"
    #define COLOR_RED "\x1B[31m"
    #define COLOR_GREEN "\x1B[32m"
    #define COLOR_YELLOW "\x1B[33m"
    #define COLOR_BLUE "\x1B[34m"
    #define COLOR_MAGENTA "\x1B[35m"
    #define COLOR_CYAN "\x1B[36m"
    #define COLOR_WHITE "\x1B[37m"
#else
    #define COLOR_RESET ""
    #define COLOR_RED ""
    #define COLOR_GREEN ""
    #define COLOR_YELLOW ""
    #define COLOR_BLUE ""
    #define COLOR_MAGENTA ""
    #define COLOR_CYAN ""
    #define COLOR_WHITE ""
#endif

#define BOOL_VALUE(boolean) \
    ((boolean) ? LitObject::TrueVal : LitObject::FalseVal)


/**
* maybe use __init__ for constructor, and __fini__ for destructor?
* need a name that clearly distinguishes these functions from others.
*/
/* name of the constructor function, if defined*/
#define LIT_NAME_CONSTRUCTOR "constructor"
/* not yet implemented: name of the destructor, if defined */
#define LIT_NAME_DESTRUCTOR "destructor"


#define LIT_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define LIT_GROW_ARRAY(state, previous, type, old_count, count) \
    (type*)lit_reallocate(state, previous, sizeof(type) * (old_count), sizeof(type) * (count))

#define LIT_FREE_ARRAY(state, type, pointer, old_count) \
    lit_reallocate(state, pointer, sizeof(type) * (old_count), 0)

#define LIT_ALLOCATE(state, type, count) \
    (type*)lit_reallocate(state, nullptr, 0, sizeof(type) * (count))

#define LIT_FREE(state, type, pointer) \
    lit_reallocate(state, pointer, sizeof(type), 0)

#define OBJECT_TYPE(value) \
    (LitObject::asObject(value)->type)



#define lit_as_bool(v) ((v) == LitObject::TrueVal)
#define lit_as_cstring(value) (LitObject::as<LitString>(value)->chars)
#define AS_UPVALUE(value) ((LitUpvalue*)LitObject::asObject(value))
#define AS_CLASS(value) ((LitClass*)LitObject::asObject(value))
#define AS_MAP(value) ((LitMap*)LitObject::asObject(value))
#define AS_BOUND_METHOD(value) ((LitBoundMethod*)LitObject::asObject(value))
#define AS_USERDATA(value) ((LitUserdata*)LitObject::asObject(value))
#define AS_RANGE(value) ((LitRange*)LitObject::asObject(value))
#define AS_FIELD(value) ((LitField*)LitObject::asObject(value))
#define AS_FIBER(value) ((LitFiber*)LitObject::asObject(value))
#define AS_REFERENCE(value) ((LitReference*)LitObject::asObject(value))

#define OBJECT_CONST_STRING(state, text) LitString::copy((state), (text), strlen(text))->asValue()
#define CONST_STRING(state, text) LitString::copy((state), (text), strlen(text))

#define INTERPRET_RUNTIME_FAIL ((LitInterpretResult){ LITRESULT_INVALID, LitObject::NullVal })



#define LIT_GET_FIELD(id) LitObject::as<LitInstance>(instance)->fields.getField(vm->state, id)
#define LIT_GET_MAP_FIELD(id) LitObject::as<LitInstance>(instance).fields.getField(vm->state, id))
#define LIT_SET_FIELD(id, value) LitObject::as<LitInstance>(instance)->fields.setField(id, value)
#define LIT_SET_MAP_FIELD(id, value) lit_set_map_field(vm->state, &LitObject::as<LitInstance>(instance)->fields, id, value)


#define LIT_ENSURE_ARGS(count) \
    if(argc != count) \
    { \
        lit_runtime_error(vm, "expected %i argument, got %i", count, argc); \
        return LitObject::NullVal; \
    }

#define LIT_ENSURE_MIN_ARGS(count) \
    if(argc < count) \
    { \
        lit_runtime_error(vm, "expected minimum %i argument, got %i", count, argc); \
        return LitObject::NullVal; \
    }

#define LIT_ENSURE_MAX_ARGS(count) \
    if(argc > count) \
    { \
        lit_runtime_error(vm, "expected maximum %i argument, got %i", count, argc); \
        return LitObject::NullVal; \
    }


enum class LitError
{
    // Preprocessor errors
    LITERROR_UNCLOSED_MACRO,
    LITERROR_UNKNOWN_MACRO,

    // Scanner errors
    LITERROR_UNEXPECTED_CHAR,
    LITERROR_UNTERMINATED_STRING,
    LITERROR_INVALID_ESCAPE_CHAR,
    LITERROR_INTERPOLATION_NESTING_TOO_DEEP,
    LITERROR_NUMBER_IS_TOO_BIG,
    LITERROR_CHAR_EXPECTATION_UNMET,

    // Parser errors
    LITERROR_EXPECTATION_UNMET,
    LITERROR_INVALID_ASSIGMENT_TARGET,
    LITERROR_TOO_MANY_FUNCTION_ARGS,
    LITERROR_MULTIPLE_ELSE_BRANCHES,
    LITERROR_VAR_MISSING_IN_FORIN,
    LITERROR_NO_GETTER_AND_SETTER,
    LITERROR_STATIC_OPERATOR,
    LITERROR_SELF_INHERITED_CLASS,
    LITERROR_STATIC_FIELDS_AFTER_METHODS,
    LITERROR_MISSING_STATEMENT,
    LITERROR_EXPECTED_EXPRESSION,
    LITERROR_DEFAULT_ARG_CENTRED,

    // Emitter errors
    LITERROR_TOO_MANY_CONSTANTS,
    LITERROR_TOO_MANY_PRIVATES,
    LITERROR_VAR_REDEFINED,
    LITERROR_TOO_MANY_LOCALS,
    LITERROR_TOO_MANY_UPVALUES,
    LITERROR_VARIABLE_USED_IN_INIT,
    LITERROR_JUMP_TOO_BIG,
    LITERROR_NO_SUPER,
    LITERROR_THIS_MISSUSE,
    LITERROR_SUPER_MISSUSE,
    LITERROR_UNKNOWN_EXPRESSION,
    LITERROR_UNKNOWN_STATEMENT,
    LITERROR_LOOP_JUMP_MISSUSE,
    LITERROR_RETURN_FROM_CONSTRUCTOR,
    LITERROR_STATIC_CONSTRUCTOR,
    LITERROR_CONSTANT_MODIFIED,
    LITERROR_INVALID_REFERENCE_TARGET,

    LITERROR_TOTAL
};


enum LitOpCode
{
    OP_POP,
    OP_RETURN,
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_TRUE,
    OP_FALSE,
    OP_NULL,
    OP_ARRAY,
    OP_OBJECT,
    OP_RANGE,
    OP_NEGATE,
    OP_NOT,

    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_POWER,
    OP_DIVIDE,
    OP_FLOOR_DIVIDE,
    OP_MOD,
    OP_BAND,
    OP_BOR,
    OP_BXOR,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_BNOT,

    OP_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,

    OP_SET_GLOBAL,
    OP_GET_GLOBAL,

    OP_SET_LOCAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL_LONG,
    OP_GET_LOCAL_LONG,

    OP_SET_PRIVATE,
    OP_GET_PRIVATE,
    OP_SET_PRIVATE_LONG,
    OP_GET_PRIVATE_LONG,

    OP_SET_UPVALUE,
    OP_GET_UPVALUE,

    OP_JUMP_IF_FALSE,
    OP_JUMP_IF_NULL,
    OP_JUMP_IF_NULL_POPPING,
    OP_JUMP,
    OP_JUMP_BACK,
    OP_AND,
    OP_OR,
    OP_NULL_OR,

    OP_CLOSURE,
    OP_CLOSE_UPVALUE,

    OP_CLASS,
    OP_GET_FIELD,
    OP_SET_FIELD,

    // [array] [index] -> [value]
    OP_SUBSCRIPT_GET,
    // [array] [index] [value] -> [value]
    OP_SUBSCRIPT_SET,
    // [array] [value] -> [array]
    OP_PUSH_ARRAY_ELEMENT,
    // [map] [slot] [value] -> [map]
    OP_PUSH_OBJECT_FIELD,

    // [class] [method] -> [class]
    OP_METHOD,
    // [class] [method] -> [class]
    OP_STATIC_FIELD,
    OP_DEFINE_FIELD,
    OP_INHERIT,
    // [instance] [class] -> [bool]
    OP_IS,
    OP_GET_SUPER_METHOD,

    // Varying stack effect
    OP_CALL,
    OP_INVOKE,
    OP_INVOKE_SUPER,
    OP_INVOKE_IGNORING,
    OP_INVOKE_SUPER_IGNORING,
    OP_POP_LOCALS,
    OP_VARARG,

    OP_REFERENCE_GLOBAL,
    OP_REFERENCE_PRIVATE,
    OP_REFERENCE_LOCAL,
    OP_REFERENCE_UPVALUE,
    OP_REFERENCE_FIELD,

    OP_SET_REFERENCE,

};


enum LitOptimizationLevel
{
    LITOPTLEVEL_NONE,
    LITOPTLEVEL_REPL,
    LITOPTLEVEL_DEBUG,
    LITOPTLEVEL_RELEASE,
    LITOPTLEVEL_EXTREME,

    LITOPTLEVEL_TOTAL
};

enum LitOptimization
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



enum LitPrecedence
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

enum LitTokenType
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

    // Literals.
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

enum LitInterpretResultType
{
    LITRESULT_OK,
    LITRESULT_COMPILE_ERROR,
    LITRESULT_RUNTIME_ERROR,
    LITRESULT_INVALID
};

enum LitErrorType
{
    COMPILE_ERROR,
    RUNTIME_ERROR
};

enum LitFunctionType
{
    LITFUNC_REGULAR,
    LITFUNC_SCRIPT,
    LITFUNC_METHOD,
    LITFUNC_STATIC_METHOD,
    LITFUNC_CONSTRUCTOR
};

struct /**/LitParser;
struct /**/ASTExpression;
struct /**/LitWriter;
struct /**/LitState;
struct /**/LitVM;
struct /**/LitMap;
struct /**/LitUserdata;
struct /**/LitString;
struct /**/LitModule;
struct /**/LitFiber;
struct /**/LitPreprocessor;
struct /**/ASTStatement;
struct /**/LitScanner;
struct /**/LitEmitter;
struct /**/LitOptimizer;
struct /**/LitFunction;
struct /**/LitNativeMethod;

typedef uint64_t LitValue;


/* allocate/reallocate memory. if new_size is 0, frees the pointer, and returns nullptr. */
void* lit_reallocate(LitState* state, void* pointer, size_t old_size, size_t new_size);

LitString* lit_vformat_error(LitState* state, size_t line, LitError error, va_list args);
LitString* lit_format_error(LitState* state, size_t line, LitError error, ...);


template<typename ElementT>
class PCGenericArray
{
    public:
        struct Config
        {
            enum
            {
                ResizeMultiplier = 4,
            };
        };

    public:
        ElementT* m_values = nullptr;
        size_t m_count = 0;

        size_t m_capacity = 0;
        LitState* m_state = nullptr;
        //ElementT m_initialbuf[10];

    public:
        inline PCGenericArray()
        {
        }

        PCGenericArray(const PCGenericArray&) = delete;
        PCGenericArray& operator=(const PCGenericArray&) = delete;

        inline void init(LitState* state)
        {
            m_count = 0;
            m_capacity = 0;
            m_values = nullptr;
            m_state = state;
        }

        inline size_t capacity() const
        {
            return m_capacity;
        }

        inline size_t size() const
        {
            return m_count;
        }

        inline constexpr ElementT& at(size_t idx)
        {
            return m_values[idx];
        }

        inline constexpr void set(size_t idx, const ElementT& val)
        {
            m_values[idx] = val;
        }

        inline void push(const ElementT& value)
        {
            size_t oldcap;
            if(m_capacity < m_count + 1)
            {
                oldcap = m_capacity;
                m_capacity = LIT_GROW_CAPACITY(oldcap);
                m_values = LIT_GROW_ARRAY(m_state, m_values, ElementT, oldcap, m_capacity);
            }
            m_values[m_count] = value;
            m_count++;
        }

        template<typename NullValT>
        inline void reserve(size_t size, const NullValT& nullval)
        {
            size_t i;
            size_t old_capacity;
            if(m_capacity < size)
            {
                old_capacity = m_capacity;
                m_capacity = size;
                m_values = LIT_GROW_ARRAY(m_state, m_values, ElementT, old_capacity, size);
                for(i = old_capacity; i < size; i++)
                {
                    m_values[i] = nullval;
                }
            }
            if(m_count < size)
            {
                m_count = size;
            }
        }

        inline void release()
        {
            LIT_FREE_ARRAY(m_state, ElementT, m_values, m_capacity);
            init(m_state);
        }

        inline ElementT& pop()
        {
            m_count--;
            return m_values[m_count];
        }
};

struct LitObject
{
    public:
        enum class Type
        {
            String,
            Function,
            NativeFunction,
            NativePrimitive,
            NativeMethod,
            PrimitiveMethod,
            Fiber,
            Module,
            Closure,
            Upvalue,
            Class,
            Instance,
            BoundMethod,
            Array,
            Map,
            Userdata,
            Range,
            Field,
            Reference
        };

        static constexpr uint64_t SIGN_BIT = ((uint64_t)1 << 63u);
        static constexpr uint64_t QNAN_BIT = ((uint64_t)0x7ffc000000000000u);

        static constexpr LitValue FalseVal = ((LitValue)(uint64_t)(LitObject::QNAN_BIT | TAG_FALSE));

        static constexpr LitValue TrueVal = ((LitValue)(uint64_t)(LitObject::QNAN_BIT | TAG_TRUE));

        static constexpr LitValue NullVal = ((LitValue)(uint64_t)(LitObject::QNAN_BIT | TAG_NULL));


    public:
        static LitObject* make(LitState* state, size_t size, LitObject::Type type);

        template<typename ObjType>
        static inline ObjType* make(LitState* state, LitObject::Type type)
        {
            return (ObjType*)LitObject::make(state, sizeof(ObjType), type);
        }

        static void releaseObject(LitState* state, LitObject* obj);

        static inline LitObject* asObject(LitValue v)
        {
            return ((LitObject*)(uintptr_t)((v) & ~(LitObject::SIGN_BIT | LitObject::QNAN_BIT)));
        }

        static inline LitValue asValue(LitObject* obj)
        {
            return (LitValue)(LitObject::SIGN_BIT | LitObject::QNAN_BIT | (uint64_t)(uintptr_t)(obj));
        }

        template<typename ObjType>
        static inline ObjType* as(LitValue v)
        {
            return (ObjType*)LitObject::asObject(v);
        }

        /* turn the given value to a number */
        static inline double toNumber(LitValue value)
        {
            return *((double*)&value);
        }

        /* turn a number into a value*/
        static inline LitValue toValue(double num)
        {
            return *((LitValue*)&num);
        }

        /* is this value falsey? */
        static inline bool isFalsey(LitValue value)
        {
            return (
                (LitObject::isBool(value) && value == LitObject::FalseVal) ||
                LitObject::isNull(value) ||
                (LitObject::isNumber(value) && (LitObject::toNumber(value) == 0))
            );
        }

        static inline bool isBool(LitValue v)
        {
            return ((v & LitObject::FalseVal) == LitObject::FalseVal);
        }

        static inline bool isNull(LitValue v)
        {
            return (v == LitObject::NullVal);
        }

        static inline bool isNumber(LitValue v)
        {
            return (( v & LitObject::QNAN_BIT) != LitObject::QNAN_BIT);
        }

        static inline bool isObject(LitValue v)
        {
            return (
                (v & (LitObject::QNAN_BIT | LitObject::SIGN_BIT)) == (LitObject::QNAN_BIT | LitObject::SIGN_BIT)
            );
        }
        
        static inline bool isObjectType(LitValue value, Type t)
        {
            return (
                LitObject::isObject(value) &&
                (LitObject::asObject(value) != nullptr) &&
                (LitObject::asObject(value)->type == t)
            );
        }

        static inline bool isString(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::String);
        }

        static inline bool isFunction(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Function);
        }

        static inline bool isNativeFunction(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::NativeFunction);
        }

        static inline bool isNativePrimitive(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::NativePrimitive);
        }

        static inline bool isNativeMethod(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::NativeMethod);
        }

        static inline bool isPrimitiveMethod(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::PrimitiveMethod);
        }

        static inline bool isModule(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Module);
        }

        static inline bool isClosure(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Closure);
        }

        static inline bool isUpvalue(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Upvalue);
        }

        static inline bool isClass(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Class);
        }

        static inline bool isInstance(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Instance);
        }

        static inline bool isArray(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Array);
        }

        static inline bool isMap(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Map);
        }

        static inline bool isBoundMethod(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::BoundMethod);
        }

        static inline bool isUserdata(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Userdata);
        }

        static inline bool isRange(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Range);
        }

        static inline bool isField(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Field);
        }

        static inline bool isReference(LitValue value)
        {
            return LitObject::isObjectType(value, LitObject::Type::Reference);
        }

    public:
        LitState* m_state;
        /* the type of this object */
        Type type;
        LitObject* next;
        bool marked;

    public:
        inline LitValue asValue()
        {
            return LitObject::asValue(this);
        }
};

struct LitString: public LitObject
{
    public:
        static uint32_t makeHash(const char* key, size_t length)
        {
            size_t i;
            uint32_t hash = 2166136261u;
            for(i = 0; i < length; i++)
            {
                hash ^= key[i];
                hash *= 16777619;
            }
            return hash;
        }

        static LitString* allocEmpty(LitState* state, size_t length, bool reuse)
        {
            LitString* string;
            string = LitObject::make<LitString>(state, LitObject::Type::String);
            if(!reuse)
            {
                string->chars = sdsempty();
                /* reserving the required space may reduce number of allocations */
                string->chars = sdsMakeRoomFor(string->chars, length);
            }
            //string->chars = nullptr;
            string->hash = 0;
            return string;
        }

        /*
        * if given $chars was alloc'd via sds, then only a LitString instance is created, without initializing
        * string->chars.
        * if it was not, and not scheduled for reuse, then first an empty sds string is created,
        * and $chars is appended, and finally, $chars is freed.
        * NB. do *not* actually allocate any sds instance here - this is already done in allocEmpty().
        */
        static LitString* allocate(LitState* state, char* chars, size_t length, uint32_t hash, bool wassds, bool reuse)
        {
            LitString* string;
            string = LitString::allocEmpty(state, length, reuse);
            if(wassds && reuse)
            {
                string->chars = chars;
            }
            else
            {
                /*
                * string->chars is initialized in LitString::allocEmpty(),
                * as an empty string!
                */
                string->chars = sdscatlen(string->chars, chars, length);
            }
            string->hash = hash;
            if(!wassds)
            {
                LIT_FREE(state, char, chars);
            }
            LitString::cache(state, string);
            return string;
        }

        static void cache(LitState* state, LitString* string);

        /*
        * create a LitString by reusing $chars. ONLY use this if you're certain that $chars isn't being freed.
        * if $wassds is true, then the sds instance is reused as-is.
        */
        /* todo: track if $chars is a sds instance - additional argument $fromsds? */
        static LitString* take(LitState* state, char* chars, size_t length, bool wassds);

        /* copy a string, creating a full newly allocated LitString. */
        static LitString* copy(LitState* state, const char* chars, size_t length);

        static LitString* copy(LitState* state, std::string_view sv)
        {
            return LitString::copy(state, sv.data(), sv.size());
        }

        static LitValue stringNumberToString(LitState* state, double value)
        {
            if(isnan(value))
            {
                return OBJECT_CONST_STRING(state, "nan");
            }

            if(isinf(value))
            {
                if(value > 0.0)
                {
                    return OBJECT_CONST_STRING(state, "infinity");
                }
                else
                {
                    return OBJECT_CONST_STRING(state, "-infinity");
                }
            }

            char buffer[24];
            int length = sprintf(buffer, "%.14g", value);

            return LitString::copy(state, buffer, length)->asValue();
        }

        static int decodeNumBytes(uint8_t byte)
        {
            if((byte & 0xc0) == 0x80)
            {
                return 0;
            }
            if((byte & 0xf8) == 0xf0)
            {
                return 4;
            }
            if((byte & 0xf0) == 0xe0)
            {
                return 3;
            }
            if((byte & 0xe0) == 0xc0)
            {
                return 2;
            }
            return 1;
        }

        static int encodeNumBytes(int value)
        {
            if(value <= 0x7f)
            {
                return 1;
            }
            if(value <= 0x7ff)
            {
                return 2;
            }
            if(value <= 0xffff)
            {
                return 3;
            }
            if(value <= 0x10ffff)
            {
                return 4;
            }
            return 0;
        }

        static LitString* fromCodePoint(LitState* state, int value)
        {
            int length;
            char* bytes;
            LitString* rt;
            length = LitString::encodeNumBytes(value);
            bytes = LIT_ALLOCATE(state, char, length + 1);
            LitString::utfstringEncode(value, (uint8_t*)bytes);
            /* this should be LitString::take, but something prevents the memory from being free'd. */
            rt = LitString::copy(state, bytes, length);
            LIT_FREE(state, char, bytes);
            return rt;
        }

        static LitString* fromRange(LitState* state, LitString* source, int start, uint32_t count)
        {
            int length;
            int index;
            int code_point;
            uint32_t i;
            uint8_t* to;
            uint8_t* from;
            char* bytes;
            from = (uint8_t*)source->chars;
            length = 0;
            for(i = 0; i < count; i++)
            {
                length += LitString::decodeNumBytes(from[start + i]);
            }
            bytes = (char*)malloc(length + 1);
            to = (uint8_t*)bytes;
            for(i = 0; i < count; i++)
            {
                index = start + i;
                code_point = LitString::utfstringDecode(from + index, source->length() - index);
                if(code_point != -1)
                {
                    to += LitString::utfstringEncode(code_point, to);
                }
            }
            return LitString::take(state, bytes, length, false);
        }

        static int utfstringEncode(int value, uint8_t* bytes)
        {
            if(value <= 0x7f)
            {
                *bytes = value & 0x7f;
                return 1;
            }
            else if(value <= 0x7ff)
            {
                *bytes = 0xc0 | ((value & 0x7c0) >> 6);
                bytes++;
                *bytes = 0x80 | (value & 0x3f);
                return 2;
            }
            else if(value <= 0xffff)
            {
                *bytes = 0xe0 | ((value & 0xf000) >> 12);
                bytes++;
                *bytes = 0x80 | ((value & 0xfc0) >> 6);
                bytes++;
                *bytes = 0x80 | (value & 0x3f);
                return 3;
            }
            else if(value <= 0x10ffff)
            {
                *bytes = 0xf0 | ((value & 0x1c0000) >> 18);
                bytes++;
                *bytes = 0x80 | ((value & 0x3f000) >> 12);
                bytes++;
                *bytes = 0x80 | ((value & 0xfc0) >> 6);
                bytes++;
                *bytes = 0x80 | (value & 0x3f);
                return 4;
            }
            UNREACHABLE
            return 0;
        }

        static int utfstringDecode(const uint8_t* bytes, uint32_t length)
        {
            int value;
            uint32_t remaining_bytes;
            if(*bytes <= 0x7f)
            {
                return *bytes;
            }
            if((*bytes & 0xe0) == 0xc0)
            {
                value = *bytes & 0x1f;
                remaining_bytes = 1;
            }
            else if((*bytes & 0xf0) == 0xe0)
            {
                value = *bytes & 0x0f;
                remaining_bytes = 2;
            }
            else if((*bytes & 0xf8) == 0xf0)
            {
                value = *bytes & 0x07;
                remaining_bytes = 3;
            }
            else
            {
                return -1;
            }
            if(remaining_bytes > length - 1)
            {
                return -1;
            }
            while(remaining_bytes > 0)
            {
                bytes++;
                remaining_bytes--;
                if((*bytes & 0xc0) != 0x80)
                {
                    return -1;
                }
                value = value << 6 | (*bytes & 0x3f);
            }
            return value;
        }

        static int utfcharOffset(char* str, int index)
        {
        #define is_utf(c) (((c)&0xC0) != 0x80)
            int offset;
            offset = 0;
            while(index > 0 && str[offset])
            {
                (void)(is_utf(str[++offset]) || is_utf(str[++offset]) || is_utf(str[++offset]) || ++offset);
                index--;
            }
            return offset;
        #undef is_utf
        }

        /*
        * creates a formatted string. is NOT printf-compatible!
        *
        * #: assume number, or use toString()
        * $: assume string, or use toString()
        * @: value toString()-ified
        *
        * e.g.:
        *   foo = (LitString instance) "bar"
        *   LitString::format("foo=@", foo)
        *   => "foo=bar"
        *
        * it's extremely rudimentary, and the idea is to quickly join values.
        */
        static LitString* format(LitState* state, const char* format, ...);

        static bool equal(LitState* state, LitString* a, LitString* b)
        {
            (void)state;
            if((a == nullptr) || (b == nullptr))
            {
                return false;
            }
            return (sdscmp(a->chars, b->chars) == 0);
        }


    public:
        /* the hash of this string - note that it is only unique to the context! */
        uint32_t hash;
        /* this is handled by sds - use lit_string_length to get the length! */
        char* chars;

    public:
        size_t length()
        {
            if(this->chars == nullptr)
            {
                return 0;
            }
            return sdslen(this->chars);
        }

        void append(const char* s, size_t len)
        {
            if(len > 0)
            {
                if(this->chars == nullptr)
                {
                    this->chars = sdsnewlen(s, len);
                }
                else
                {
                    this->chars = sdscatlen(this->chars, s, len);
                }
            }
        }

        void append(LitString* other)
        {
            append(other->chars, other->length());
        }

        void append(char ch)
        {
            this->chars = sdscatlen(this->chars, (const char*)&ch, 1);
        }

        bool contains(const char* findme, size_t fmlen, bool icase)
        {
            int selfch;
            int findch;
            size_t i;
            size_t j;
            size_t slen;
            size_t found;
            slen = sdslen(this->chars);
            found = 0;
            if(slen >= fmlen)
            {
                for(i=0, j=0; i<slen; i++)
                {
                    do
                    {
                        selfch = this->chars[i];
                        findch = findme[j];
                        if(icase)
                        {
                            selfch = tolower(selfch);
                            findch = tolower(findch);
                        }
                        if(selfch == findch)
                        {
                            if(++found == fmlen)
                            {
                                return true;
                            }
                            i++;
                            j++;
                        }
                        else
                        {
                            i -= found;
                            found = 0;
                            j = 0;
                        }
                    } while(found);
                }
                return false;
            }
            return false;
        }

        size_t utfLength()
        {
            size_t length;
            uint32_t i;
            length = 0;
            for(i = 0; i < this->length();)
            {
                i += LitString::decodeNumBytes(this->chars[i]);
                length++;
            }
            return length;
        }

        LitString* codePointAt(uint32_t index)
        {
            char bytes[2];
            int code_point;
            if(index >= this->length())
            {
                return nullptr;
            }
            code_point = LitString::utfstringDecode((uint8_t*)this->chars + index, this->length() - index);
            if(code_point == -1)
            {
                bytes[0] = this->chars[index];
                bytes[1] = '\0';
                return LitString::copy(m_state, bytes, 1);
            }
            return LitString::fromCodePoint(m_state, code_point);
        }
};

struct LitChunk
{
    public:
        LitState* state;
        /* how many items this chunk holds */
        size_t m_count;
        size_t m_capacity;
        uint8_t* code;
        bool has_line_info;
        size_t line_count;
        size_t line_capacity;
        uint16_t* lines;
        PCGenericArray<LitValue> constants;

    public:

        void init(LitState* state)
        {
            this->state = state;
            this->m_count = 0;
            this->m_capacity = 0;
            this->code = nullptr;

            this->has_line_info = true;
            this->line_count = 0;
            this->line_capacity = 0;
            this->lines = nullptr;

            this->constants.init(this->state);
        }

        void release()
        {
            LIT_FREE_ARRAY(state, uint8_t, this->code, this->m_capacity);
            LIT_FREE_ARRAY(state, uint16_t, this->lines, this->line_capacity);

            this->constants.release();
            init(this->state);
        }

        void write_chunk(uint8_t byte, uint16_t line)
        {
            if(this->m_capacity < this->m_count + 1)
            {
                size_t old_capacity = this->m_capacity;

                this->m_capacity = LIT_GROW_CAPACITY(old_capacity + 2);
                this->code = LIT_GROW_ARRAY(this->state, this->code, uint8_t, old_capacity, this->m_capacity + 2);
            }

            this->code[this->m_count] = byte;
            this->m_count++;

            if(!this->has_line_info)
            {
                return;
            }

            if(this->line_capacity < this->line_count + 2)
            {
                size_t old_capacity = this->line_capacity;

                this->line_capacity = LIT_GROW_CAPACITY(this->line_capacity + 2);
                this->lines = LIT_GROW_ARRAY(this->state, this->lines, uint16_t, old_capacity, this->line_capacity + 2);

                if(old_capacity == 0)
                {
                    this->lines[0] = 0;
                    this->lines[1] = 0;
                }
            }

            size_t line_index = this->line_count;
            size_t value = this->lines[line_index];

            if(value != 0 && value != line)
            {
                this->line_count += 2;
                line_index = this->line_count;

                this->lines[line_index + 1] = 0;
            }

            this->lines[line_index] = line;
            this->lines[line_index + 1]++;
        }

        size_t add_constant(LitValue constant);

        size_t get_line(size_t offset)
        {
            if(!this->has_line_info)
            {
                return 0;
            }

            size_t rle = 0;
            size_t line = 0;
            size_t index = 0;

            for(size_t i = 0; i < offset + 1; i++)
            {
                if(rle > 0)
                {
                    rle--;
                    continue;
                }

                line = this->lines[index];
                rle = this->lines[index + 1];

                if(rle > 0)
                {
                    rle--;
                }

                index += 2;
            }

            return line;
        }

        void lit_shrink_chunk()
        {
            if(this->m_capacity > this->m_count)
            {
                size_t old_capacity = this->m_capacity;

                this->m_capacity = this->m_count;
                this->code = LIT_GROW_ARRAY(this->state, this->code, uint8_t, old_capacity, this->m_capacity);
            }

            if(this->line_capacity > this->line_count)
            {
                size_t old_capacity = this->line_capacity;

                this->line_capacity = this->line_count + 2;
                this->lines = LIT_GROW_ARRAY(this->state, this->lines, uint16_t, old_capacity, this->line_capacity);
            }
        }

        void emit_byte(uint8_t byte)
        {
            write_chunk(byte, 1);
        }

        void emit_bytes(uint8_t a, uint8_t b)
        {
            write_chunk(a, 1);
            write_chunk(b, 1);
        }

        void emit_short(uint16_t value)
        {
            emit_bytes((uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
        }



};

struct LitFunction: public LitObject
{
    public:
        static LitFunction* make(LitState* state, LitModule* module)
        {
            LitFunction* function;
            function = LitObject::make<LitFunction>(state, LitObject::Type::Function);
            function->chunk.init(state);
            function->name = nullptr;
            function->arg_count = 0;
            function->upvalue_count = 0;
            function->max_slots = 0;
            function->module = module;
            function->vararg = false;
            return function;
        }

    public:
        LitChunk chunk;
        LitString* name;
        uint8_t arg_count;
        uint16_t upvalue_count;
        size_t max_slots;
        bool vararg;
        LitModule* module;
};

struct LitUpvalue: public LitObject
{
    LitValue* location;
    LitValue closed;
    LitUpvalue* next;
};

struct LitClosure: public LitObject
{
    LitFunction* function;
    LitUpvalue** upvalues;
    size_t upvalue_count;
};

struct LitNativeFunction: public LitObject
{
    public:
        using FuncType = LitValue(*)(LitVM*, size_t, LitValue*);

    public:
        static LitNativeFunction* make(LitState* state, LitNativeFunction::FuncType function, LitString* name)
        {
            LitNativeFunction* native;
            native = LitObject::make<LitNativeFunction>(state, LitObject::Type::NativeFunction);
            native->function = function;
            native->name = name;
            return native;
        }

    public:
        /* the native callback for this function */
        FuncType function;
        /* the name of this function */
        LitString* name;
};


struct LitNativePrimFunction: public LitObject
{
    public:
        using FuncType = bool(*)(LitVM*, size_t, LitValue*);

    public:
        static LitNativePrimFunction* make(LitState* state, LitNativePrimFunction::FuncType function, LitString* name)
        {
            LitNativePrimFunction* native;
            native = LitObject::make<LitNativePrimFunction>(state, LitObject::Type::NativePrimitive);
            native->function = function;
            native->name = name;
            return native;
        }

    public:
        FuncType function;
        LitString* name;
};

struct LitNativeMethod: public LitObject
{
    public:
        using FuncType = LitValue(*)(LitVM*, LitValue, size_t arg_count, LitValue*);

    public:
        static LitNativeMethod* make(LitState* state, FuncType method, LitString* name)
        {
            LitNativeMethod* native;
            native = LitObject::make<LitNativeMethod>(state, LitObject::Type::NativeMethod);
            native->method = method;
            native->name = name;
            return native;
        }

        static LitNativeMethod* make(LitState* state, FuncType method, std::string_view name)
        {
            return LitNativeMethod::make(state, method, LitString::copy(state, name.data(), name.size()));
        }

    public:
        FuncType method;
        LitString* name;
};

struct LitPrimitiveMethod: public LitObject
{
    public:
        using FuncType = bool (*)(LitVM*, LitValue, size_t, LitValue*);

    public:
        static LitPrimitiveMethod* make(LitState* state, FuncType method, LitString* name)
        {
            LitPrimitiveMethod* native;
            native = LitObject::make<LitPrimitiveMethod>(state, LitObject::Type::PrimitiveMethod);
            native->method = method;
            native->name = name;
            return native;
        }

        static LitPrimitiveMethod* make(LitState* state, FuncType method, std::string_view sv)
        {
            return make(state, method, LitString::copy(state, sv.data(), sv.size()));
        }

    public:
        FuncType method;
        LitString* name;
};


struct LitTableEntry
{
    /* the key of this entry. can be nullptr! */
    LitString* key;

    /* the associated value */
    LitValue value;
};


struct LitTable: public PCGenericArray<LitTableEntry>
{

    public:
        template<typename FuncT>
        static void setFunctionValue(LitTable& dest, LitString* nm, typename FuncT::FuncType fn)
        {
            dest.set(nm, FuncT::make(dest.m_state, fn, nm)->asValue());
        }

        template<typename FuncT>
        static void setFunctionValue(LitTable& dest, std::string_view sv, typename FuncT::FuncType fn)
        {
            auto nm = LitString::copy(dest.m_state, sv.data(), sv.size());
            setFunctionValue<FuncT>(dest, nm, fn);
        }

        static void setNativeMethod(LitTable& dest, LitString* nm, LitNativeMethod::FuncType fn)
        {
            setFunctionValue<LitNativeMethod>(dest, nm, fn);
        }

        static void setNativeMethod(LitTable& dest, std::string_view sv, LitNativeMethod::FuncType fn)
        {
            setFunctionValue<LitNativeMethod>(dest, sv, fn);
        }

        static LitTableEntry* findEntry(LitTableEntry* entries, int capacity, LitString* key)
        {
            uint32_t index;
            LitTableEntry* entry;
            LitTableEntry* tombstone;
            index = key->hash % capacity;
            tombstone = nullptr;
            while(true)
            {
                entry = &entries[index];
                if(entry->key == nullptr)
                {
                    if(LitObject::isNull(entry->value))
                    {
                        return tombstone != nullptr ? tombstone : entry;
                    }
                    else if(tombstone == nullptr)
                    {
                        tombstone = entry;
                    }
                }
                if(entry->key == key)
                {
                    return entry;
                }
                index = (index + 1) % capacity;
            }
        }

        static void adjustCapacity(LitState* state, LitTable* table, size_t capacity)
        {
            size_t i;
            LitTableEntry* destination;
            LitTableEntry* entries;
            LitTableEntry* entry;
            entries = LIT_ALLOCATE(state, LitTableEntry, capacity + 1);
            for(i = 0; i <= capacity; i++)
            {
                entries[i].key = nullptr;
                entries[i].value = LitObject::NullVal;
            }
            table->m_count = 0;
            for(i = 0; i <= table->m_capacity; i++)
            {
                entry = &table->m_values[i];
                if(entry == nullptr)
                {
                    continue;
                }
                if(entry->key == nullptr)
                {
                    continue;
                }
                destination = findEntry(entries, capacity, entry->key);
                destination->key = entry->key;
                destination->value = entry->value;
                table->m_count++;
            }
            LIT_FREE_ARRAY(state, LitTableEntry, table->m_values, table->m_capacity + 1);
            table->m_capacity = capacity;
            table->m_values = entries;
        }

    public:
        void markForGC(LitVM* vm);

        void setField(const char* name, LitValue value)
        {
            this->set(CONST_STRING(m_state, name), value);
        }

        LitValue getField(const char* name)
        {
            LitValue value;
            if(!this->get(CONST_STRING(m_state, name), &value))
            {
                value = LitObject::NullVal;
            }
            return value;
        }

        bool set(LitString* key, LitValue value)
        {
            bool is_new;
            size_t capacity;
            LitTableEntry* entry;
            if(this->m_count + 1 > (this->m_capacity + 1) * TABLE_MAX_LOAD)
            {
                capacity = LIT_GROW_CAPACITY(this->m_capacity + 1) - 1;
                adjustCapacity(m_state, this, capacity);
            }
            entry = findEntry(this->m_values, this->m_capacity, key);
            is_new = entry->key == nullptr;
            if(is_new && LitObject::isNull(entry->value))
            {
                this->m_count++;
            }
            entry->key = key;
            entry->value = value;
            return is_new;
        }

        inline bool set(std::string_view sv, LitValue value)
        {
            return set(LitString::copy(m_state, sv.data(), sv.size()), value);
        }

        bool get(LitString* key, LitValue* value)
        {
            LitTableEntry* entry;
            if(this->m_count == 0)
            {
                return false;
            }
            entry = findEntry(this->m_values, this->m_capacity, key);
            if(entry->key == nullptr)
            {
                return false;
            }
            *value = entry->value;
            return true;
        }

        bool getSlot(LitString* key, LitValue** value)
        {
            LitTableEntry* entry;
            if(this->m_count == 0)
            {
                return false;
            }
            entry = findEntry(this->m_values, this->m_capacity, key);
            if(entry->key == nullptr)
            {
                return false;
            }
            *value = &entry->value;
            return true;
        }

        bool remove(LitString* key)
        {
            LitTableEntry* entry;
            if(this->m_count == 0)
            {
                return false;
            }
            entry = findEntry(this->m_values, this->m_capacity, key);
            if(entry->key == nullptr)
            {
                return false;
            }
            entry->key = nullptr;
            entry->value = BOOL_VALUE(true);
            return true;
        }

        LitString* find(const char* chars, size_t length, uint32_t hash)
        {
            uint32_t index;
            LitTableEntry* entry;
            if(this->m_count == 0)
            {
                return nullptr;
            }
            index = hash % this->m_capacity;
            while(true)
            {
                entry = &this->m_values[index];
                if(entry->key == nullptr)
                {
                    if(LitObject::isNull(entry->value))
                    {
                        return nullptr;
                    }
                }
                else if(entry->key->length() == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0)
                {
                    return entry->key;
                }
                index = (index + 1) % this->m_capacity;
            }
        }

        void addAll(LitTable* from)
        {
            size_t i;
            LitTableEntry* entry;
            for(i = 0; i <= from->m_capacity; i++)
            {
                entry = &from->m_values[i];
                if(entry != nullptr)
                {
                    if(entry->key != nullptr)
                    {
                        set(entry->key, entry->value);
                    }
                }
            }
        }

        void removeWhite()
        {
            size_t i;
            LitTableEntry* entry;
            for(i = 0; i <= this->m_capacity; i++)
            {
                entry = &this->m_values[i];
                if(entry->key != nullptr && !entry->key->marked)
                {
                    this->remove(entry->key);
                }
            }
        }

        int64_t iterator(int64_t number)
        {
            if(this->m_count == 0)
            {
                return -1;
            }
            if(number >= this->m_capacity)
            {
                return -1;
            }
            number++;
            for(; number < (int64_t)this->m_capacity; number++)
            {
                if(this->m_values[number].key != nullptr)
                {
                    return number;
                }
            }

            return -1;
        }

        LitValue iterKey(int64_t index)
        {
            if(this->m_capacity <= index)
            {
                return LitObject::NullVal;
            }
            return this->m_values[index].key->asValue();
        }
};

struct LitWriter
{
    public:
        using WriteByteFuncType = void(*)(LitWriter*, int);
        using WriteStringFuncType = void(*)(LitWriter*, const char*, size_t);
        using WriteFormatFuncType = void(*)(LitWriter*, const char*, va_list);

    public:
        static void cb_writebyte(LitWriter* wr, int byte)
        {
            LitString* ds;
            if(wr->stringmode)
            {
                ds = (LitString*)wr->uptr;
                ds->append(byte);        
            }
            else
            {
                fputc(byte, (FILE*)wr->uptr);
            }
        }

        static void cb_writestring(LitWriter* wr, const char* string, size_t len)
        {
            LitString* ds;
            if(wr->stringmode)
            {
                ds = (LitString*)wr->uptr;
                ds->append(string, len);
            }
            else
            {
                fwrite(string, sizeof(char), len, (FILE*)wr->uptr);
            }
        }

        static void cb_writeformat(LitWriter* wr, const char* fmt, va_list va)
        {
            LitString* ds;
            if(wr->stringmode)
            {
                ds = (LitString*)wr->uptr;
                ds->chars = sdscatvprintf(ds->chars, fmt, va);
            }
            else
            {
                vfprintf((FILE*)wr->uptr, fmt, va);
            }
        }

        static void initDefault(LitState* state, LitWriter* wr)
        {
            wr->state = state;
            wr->forceflush = false;
            wr->stringmode = false;
            wr->fnbyte = cb_writebyte;
            wr->fnstring = cb_writestring;
            wr->fnformat = cb_writeformat;
        }


    public:
        LitState* state;
        /* the main pointer, that either holds a pointer to a LitString, or a FILE */
        void* uptr;

        /* if true, then uptr is a LitString, otherwise it's a FILE */
        bool stringmode;

        /* if true, and !stringmode, then calls fflush() after each i/o operations */
        bool forceflush;

        /* the callback that emits a single character */
        WriteByteFuncType fnbyte;

        /* the callback that emits a string */
        WriteStringFuncType fnstring;

        /* the callback that emits a format string (printf-style) */
        WriteFormatFuncType fnformat;

    public:
        /*
        * creates a LitWriter that writes to the given FILE.
        * if $forceflush is true, then fflush() is called after each i/o operation.
        */
        void initFile(LitState* state, FILE* fh, bool forceflush)
        {
            initDefault(state, this);
            this->uptr = fh;
            this->forceflush = forceflush;
        }

        /*
        * creates a LitWriter that writes to a buffer.
        */
        void initString(LitState* state)
        {
            initDefault(state, this);
            this->stringmode = true;
            this->uptr = LitString::allocEmpty(state, 0, false);
        }

        /* emit a single byte */
        void put(int byte)
        {
            this->fnbyte(this, byte);
        }

        void put(const char* str, size_t len)
        {
            this->fnstring(this, str, len);
        }

        /* emit a string */
        void put(std::string_view sv)
        {
            this->fnstring(this, sv.data(), sv.size());
        }

        /* emit a printf-style formatted string */
        void format(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            this->fnformat(this, fmt, va);
            va_end(va);
        }

        /*
        * returns a LitString* if this LitWriter was initiated via lit_writer_init_string, otherwise nullptr.
        */
        LitString* asString()
        {
            if(this->stringmode)
            {
                return (LitString*)this->uptr;
            }
            return nullptr;
        }

};

struct LitVariable
{
    const char* name;
    size_t length;
    int depth;
    bool constant;
    bool used;
    LitValue constant_value;
    ASTStatement** declaration;
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
            IfClause,
            Interpolation,
            Reference
        };

    public:
        static ASTExpression* make(LitState* state, uint64_t line, size_t size, ASTExpression::Type type)
        {
            ASTExpression* expr;
            expr = (ASTExpression*)lit_reallocate(state, nullptr, 0, size);
            expr->m_state = state;
            expr->type = type;
            expr->line = line;
            return expr;
        }

        static PCGenericArray<ASTExpression*>* makeList(LitState* state)
        {
            PCGenericArray<ASTExpression*>* expressions;
            expressions = (PCGenericArray<ASTExpression*>*)lit_reallocate(state, nullptr, 0, sizeof(PCGenericArray<ASTExpression*>));
            expressions->init(state);
            return expressions;
        }

    public:
        Type type = Type::Unspecified;
};

struct ASTStatement: public ASTNode
{
    public:
        enum class Type
        {
            Unspecified,
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

    public:
        static ASTStatement* make(LitState* state, uint64_t line, size_t size, ASTStatement::Type type)
        {
            ASTStatement* stmt;
            stmt = (ASTStatement*)lit_reallocate(state, nullptr, 0, size);
            stmt->m_state = state;
            stmt->type = type;
            stmt->line = line;
            return stmt;
        }

        static PCGenericArray<ASTStatement*>* makeList(LitState* state)
        {
            PCGenericArray<ASTStatement*>* statements;
            statements = (PCGenericArray<ASTStatement*>*)lit_reallocate(state, nullptr, 0, sizeof(PCGenericArray<ASTStatement*>));
            statements->init(state);
            return statements;
        }

    public:
        Type type = Type::Unspecified;

};

struct LitToken
{
    const char* start;
    LitTokenType type;
    size_t length;
    size_t line;
    LitValue value;
};

struct LitLocal
{
    const char* name;
    size_t length;
    int depth;
    bool captured;
    bool constant;
};

struct LitCompilerUpvalue
{
    uint8_t index;
    bool isLocal;
};

struct LitCompiler
{
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
};

struct LitParser
{
    public:
        LitState* state;
        bool had_error;
        bool panic_mode;
        LitToken previous;
        LitToken current;
        LitCompiler* compiler;
        uint8_t expression_root_count;
        uint8_t statement_root_count;

    public:
        void init(LitState* state);

        void release()
        {
        }

};

struct LitEmulatedFile
{
    const char* source;
    size_t position;
};

struct LitScanner
{
    size_t line;
    const char* start;
    const char* current;
    const char* file_name;
    LitState* state;
    size_t braces[LIT_MAX_INTERPOLATION_NESTING];
    size_t num_braces;
    bool had_error;
};

struct LitOptimizer
{
    public:
        LitState* state;
        PCGenericArray<LitVariable> variables;
        int depth;
        bool mark_used;

    public:
        void init(LitState* state)
        {
            this->state = state;
            this->depth = -1;
            this->mark_used = false;
            this->variables.init(state);
        }

};

struct LitPreprocessor
{
    public:
        LitState* state;
        LitTable defined;
        /*
         * A little bit dirty hack:
         * We need to store pointers (8 bytes in size),
         * and so that we don't have to declare a new
         * array type, that we will use only once,
         * I'm using LitValueList here, because LitValue
         * also has the same size (8 bytes)
         */
        PCGenericArray<LitValue> open_ifs;

    public:
        void init(LitState* state)
        {
            this->state = state;
            this->defined.init(state);
            this->open_ifs.init(state);
        }

        void release()
        {
            this->defined.release();
            this->open_ifs.release();
        }
};

struct LitPrivate
{
    bool initialized;
    bool constant;
};




struct ASTParseRule
{
    public:
        using PrefixFuncType = ASTExpression* (*)(LitParser*, bool);
        using InfixFuncType = ASTExpression*(*)(LitParser*, ASTExpression*, bool);

    public:
        PrefixFuncType prefix;
        InfixFuncType infix;
        LitPrecedence precedence;
};


struct ASTExprLiteral: public ASTExpression
{
    public:
        static ASTExprLiteral* make(LitState* state, size_t line, LitValue value)
        {
            ASTExprLiteral* rt;
            rt = (ASTExprLiteral*)ASTExpression::make(state, line, sizeof(ASTExprLiteral), ASTExpression::Type::Literal);
            rt->value = value;
            return rt;
        }

    public:
        LitValue value;
};

struct ASTExprBinary: public ASTExpression
{
    ASTExpression* left;
    ASTExpression* right;
    LitTokenType op;
    bool ignore_left;
};

struct ASTExprUnary: public ASTExpression
{
    ASTExpression* right;
    LitTokenType op;
};

struct ASTExprVar: public ASTExpression
{
    const char* name;
    size_t length;
};

struct ASTExprAssign: public ASTExpression
{
    ASTExpression* to;
    ASTExpression* value;
};

struct ASTExprCall: public ASTExpression
{
    ASTExpression* callee;
    PCGenericArray<ASTExpression*> args;
    ASTExpression* objexpr;
};

struct ASTExprIndexGet: public ASTExpression
{
    ASTExpression* where;
    const char* name;
    size_t length;
    int jump;
    bool ignore_emit;
    bool ignore_result;
};

struct ASTExprIndexSet: public ASTExpression
{
    ASTExpression* where;
    const char* name;
    size_t length;
    ASTExpression* value;
};

struct ASTExprFuncParam
{
    const char* name;
    size_t length;
    ASTExpression* default_value;
};


struct ASTExprLambda: public ASTExpression
{
    PCGenericArray<ASTExprFuncParam> parameters;
    ASTStatement* body;
};

struct ASTExprArray: public ASTExpression
{
    PCGenericArray<ASTExpression*> values;
};

struct ASTExprObject: public ASTExpression
{
    PCGenericArray<LitValue> keys;
    PCGenericArray<ASTExpression*> values;
};

struct ASTExprSubscript: public ASTExpression
{
    ASTExpression* array;
    ASTExpression* index;
};

struct ASTExprThis: public ASTExpression
{
};

struct ASTExprSuper: public ASTExpression
{
    LitString* method;
    bool ignore_emit;
    bool ignore_result;
};

struct ASTExprRange: public ASTExpression
{
    ASTExpression* from;
    ASTExpression* to;
};

struct ASTExprIfClause: public ASTStatement
{
    ASTExpression* condition;
    ASTExpression* if_branch;
    ASTExpression* else_branch;
};

struct ASTExprInterpolation: public ASTExpression
{
    PCGenericArray<ASTExpression*> expressions;
};

struct ASTExprReference: public ASTExpression
{
    public:
        static ASTExprReference* make(LitState* state, size_t line, ASTExpression* to)
        {
            ASTExprReference* expr;
            expr = (ASTExprReference*)ASTExpression::make(state, line, sizeof(ASTExprReference), ASTExpression::Type::Reference);
            expr->to = to;
            return expr;
        }

    public:
        ASTExpression* to;
};

struct ASTExprStatement: public ASTStatement
{
    ASTExpression* expression;
    bool pop;
};

struct ASTStmtBlock: public ASTStatement
{
    PCGenericArray<ASTStatement*> statements;
};

struct ASTStmtVar: public ASTStatement
{
    const char* name;
    size_t length;
    bool constant;
    ASTExpression* valexpr;
};

struct ASTStmtIfClause: public ASTStatement
{
    ASTExpression* condition;
    ASTStatement* if_branch;
    ASTStatement* else_branch;
    PCGenericArray<ASTExpression*>* elseif_conditions;
    PCGenericArray<ASTStatement*>* elseif_branches;
};

struct ASTStmtWhileLoop: public ASTStatement
{
    ASTExpression* condition;
    ASTStatement* body;
};

struct ASTStmtForLoop: public ASTStatement
{
    ASTExpression* exprinit;
    ASTStatement* var;
    ASTExpression* condition;
    ASTExpression* increment;
    ASTStatement* body;
    bool c_style;
};

struct ASTStmtContinue: public ASTStatement
{
};

struct ASTStmtBreak: public ASTStatement
{
};

struct ASTStmtFunction: public ASTStatement
{
    const char* name;
    size_t length;
    PCGenericArray<ASTExprFuncParam> parameters;
    ASTStatement* body;
    bool exported;
};

struct ASTStmtReturn: public ASTStatement
{
    ASTExpression* expression;
};

struct ASTStmtMethod: public ASTStatement
{
    LitString* name;
    PCGenericArray<ASTExprFuncParam> parameters;
    ASTStatement* body;
    bool is_static;
};

struct ASTStmtClass: public ASTStatement
{
    LitString* name;
    LitString* parent;
    PCGenericArray<ASTStatement*> fields;
};

struct ASTStmtField: public ASTStatement
{
    LitString* name;
    ASTStatement* getter;
    ASTStatement* setter;
    bool is_static;
};



struct LitCallFrame
{
    LitFunction* function;
    LitClosure* closure;
    uint8_t* ip;
    LitValue* slots;
    bool result_ignored;
    bool return_to_c;
};

struct LitMap: public LitObject
{
    public:
        using IndexFuncType = LitValue(*)(LitVM*, LitMap*, LitString*, LitValue*);

    public:
        static LitMap* make(LitState* state)
        {
            LitMap* map;
            map = LitObject::make<LitMap>(state, LitObject::Type::Map);
            map->values.init(state);
            map->index_fn = nullptr;
            return map;
        }

    public:
        /* the table that holds the actual entries */
        LitTable values;
        /* the index function corresponding to operator[] */
        IndexFuncType index_fn;

    public:
        bool set(LitString* key, LitValue value)
        {
            if(value == LitObject::NullVal)
            {
                this->remove(key);
                return false;
            }
            return this->values.set(key, value);
        }

        bool get(LitString* key, LitValue* value)
        {
            return this->values.get(key, value);
        }

        void lit_set_map_field(LitState* state, const char* name, LitValue value)
        {
            this->values.set(CONST_STRING(state, name), value);
        }

        LitValue getField(LitState* state, const char* name)
        {
            LitValue value;
            if(!this->values.get(CONST_STRING(state, name), &value))
            {
                value = LitObject::NullVal;
            }
            return value;
        }

        bool remove(LitString* key)
        {
            return this->values.remove(key);
        }

        void addAll(LitMap* other)
        {
            this->values.addAll(&other->values);
        }
};

struct LitModule: public LitObject
{
    LitValue return_value;
    LitString* name;
    LitValue* privates;
    LitMap* private_names;
    size_t private_count;
    LitFunction* main_function;
    LitFiber* main_fiber;
    bool ran;
};

struct LitFiber: public LitObject
{
    LitFiber* parent;
    LitValue* stack;
    LitValue* stack_top;
    size_t stack_capacity;
    LitCallFrame* frames;
    size_t frame_capacity;
    size_t frame_count;
    size_t arg_count;
    LitUpvalue* open_upvalues;
    LitModule* module;
    LitValue error;
    bool abort;
    bool catcher;
};

struct LitField: public LitObject
{
    public:
        static LitField* make(LitState* state, LitObject* getter, LitObject* setter)
        {
            LitField* field;
            field = LitObject::make<LitField>(state, LitObject::Type::Field);
            field->getter = getter;
            field->setter = setter;
            return field;
        }

    public:
        LitObject* getter;
        LitObject* setter;
};

struct LitClass: public LitObject
{
    public:
        static LitClass* make(LitState* state, LitString* name)
        {
            LitClass* klass;
            klass = LitObject::make<LitClass>(state, LitObject::Type::Class);
            klass->name = name;
            klass->init_method = nullptr;
            klass->super = nullptr;
            klass->methods.init(state);
            klass->static_fields.init(state);
            return klass;
        }

        static LitClass* make(LitState* state, std::string_view name)
        {
            auto nm = LitString::copy(state, name.data(), name.size());
            return LitClass::make(state, nm);
        }

    public:
        /* the name of this class */
        LitString* name;
        /* the constructor object */
        LitObject* init_method;
        /* runtime methods */
        LitTable methods;
        /* static fields, which include functions, and variables */
        LitTable static_fields;
        /*
        * the parent class - the topmost is always LitClass, followed by LitObject.
        * that is, eg for LitString: LitString <- LitObject <- LitClass
        */
        LitClass* super;

    public:

    public:
        void inheritFrom(LitClass* superclass)
        {
            this->super = superclass;
            if(this->init_method == nullptr)
            {
                this->init_method = superclass->init_method;
            }
            superclass->methods.addAll(&this->methods); \
            superclass->static_fields.addAll(&this->static_fields);
        }

        void bindConstructor(LitNativeMethod::FuncType method)
        {
            auto nm = LitString::copy(m_state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1);
            auto m = LitNativeMethod::make(m_state, method, nm);
            this->init_method = (LitObject*)m;
            this->methods.set(nm, m->asValue());
        }


        void setField(const char* name, LitValue val)
        {
            this->static_fields.setField(name, val);
        }

        void bindField(LitString* nm, LitNativeMethod::FuncType fnget, LitNativeMethod::FuncType fnset)
        {
            this->methods.set(nm,
                LitField::make(m_state,
                    (LitObject*)LitNativeMethod::make(m_state, fnget, nm),
                    (LitObject*)LitNativeMethod::make(m_state, fnset, nm))->asValue());
        }

        void bindField(std::string_view sv, LitNativeMethod::FuncType fnget, LitNativeMethod::FuncType fnset)
        {
            bindField(LitString::copy(m_state, sv.data(), sv.size()), fnget, fnset);
        }


        void bindMethod(LitString* nm, LitNativeMethod::FuncType method)
        {
            this->methods.set(nm, LitNativeMethod::make(m_state, method, name)->asValue());
        }

        void bindMethod(std::string_view sv, LitNativeMethod::FuncType method)
        {
            auto nm = LitString::copy(m_state, sv.data(), sv.size());
            bindMethod(nm, method);
        }


        void bindPrimitive(LitString* nm, LitPrimitiveMethod::FuncType method)
        {
            this->methods.set(nm, LitPrimitiveMethod::make(m_state, method, nm)->asValue());
        }

        void bindPrimitive(std::string_view sv, LitPrimitiveMethod::FuncType method)
        {
            auto nm = LitString::copy(m_state, sv.data(), sv.size());
            bindPrimitive(nm, method);
        }

        void setStaticField(LitString* nm, LitNativeMethod::FuncType fnget, LitNativeMethod::FuncType fnset)
        {
            this->static_fields.set(nm,
                LitField::make(m_state,
                    (LitObject*)LitNativeMethod::make(m_state, fnget, nm),
                    (LitObject*)LitNativeMethod::make(m_state, fnset, nm))->asValue());
        }

        void setStaticField(std::string_view sv, LitNativeMethod::FuncType fnget, LitNativeMethod::FuncType fnset)
        {
            setStaticField(LitString::copy(m_state, sv.data(), sv.size()), fnget, fnset);
        }

        void setStaticMethod(LitString* nm, LitNativeMethod::FuncType fn)
        {
            LitTable::setNativeMethod(this->static_fields, nm, fn);
        }

        void setStaticMethod(std::string_view sv, LitNativeMethod::FuncType fn)
        {
            LitTable::setNativeMethod(this->static_fields, sv, fn);
        }

        void setStaticPrimitive(LitString* nm, LitPrimitiveMethod::FuncType fn)
        {
            LitTable::setFunctionValue<LitPrimitiveMethod>(this->static_fields, nm, fn);
        }

        void setStaticPrimitive(std::string_view sv, LitPrimitiveMethod::FuncType fn)
        {
            LitTable::setFunctionValue<LitPrimitiveMethod>(this->static_fields, sv, fn);
        }

        void setStaticSetter(LitString* nm, LitNativeMethod::FuncType fn)
        {
            this->static_fields.set(nm,
                LitField::make(m_state,
                    nullptr,
                    (LitObject*)LitNativeMethod::make(m_state, fn, nm))->asValue());
        }

        void setStaticSetter(std::string_view sv, LitNativeMethod::FuncType fn)
        {
            auto nm = LitString::copy(m_state, sv.data(), sv.size());
            return setStaticSetter(nm, fn);
        }

        void setStaticGetter(LitString* nm, LitNativeMethod::FuncType fn)
        {
            this->static_fields.set(nm,
                LitField::make(m_state,
                    (LitObject*)LitNativeMethod::make(m_state, fn, nm),
                    nullptr)->asValue());
        }

        void setStaticGetter(std::string_view sv, LitNativeMethod::FuncType fn)
        {
            auto nm = LitString::copy(m_state, sv.data(), sv.size());
            return setStaticGetter(nm, fn);
        }

        void setGetter(LitString* nm, LitNativeMethod::FuncType fn)
        {
            this->methods.set(nm, LitField::make(this->m_state, LitNativeMethod::make(m_state, fn, nm), nullptr)->asValue());
        }

        void setGetter(std::string_view sv, LitNativeMethod::FuncType fn)
        {
            auto nm = LitString::copy(this->m_state, sv.data(), sv.size());
            setGetter(nm, fn);
        }

        void setSetter(LitString* nm, LitNativeMethod::FuncType fn)
        {
            this->methods.set(nm, LitField::make(this->m_state, nullptr, LitNativeMethod::make(m_state, fn, nm))->asValue());
        }

        void setSetter(std::string_view sv, LitNativeMethod::FuncType fn)
        {
            auto nm = LitString::copy(this->m_state, sv.data(), sv.size());
            setSetter(nm, fn);
        }

};

struct LitInstance: public LitObject
{
    public:
        static LitInstance* make(LitState* state, LitClass* klass)
        {
            LitInstance* instance;
            instance = LitObject::make<LitInstance>(state, LitObject::Type::Instance);
            instance->klass = klass;
            instance->fields.init(state);
            instance->fields.m_count = 0;
            return instance;
        }

    public:
        /* the class that corresponds to this instance */
        LitClass* klass;
        LitTable fields;
};

struct LitBoundMethod: public LitObject
{
    LitValue receiver;
    LitValue method;
};

struct LitArray: public LitObject
{
    public:
        static LitArray* make(LitState* state)
        {
            LitArray* array;
            array = LitObject::make<LitArray>(state, LitObject::Type::Array);
            array->values.init(state);
            return array;
        }

    public:
        PCGenericArray<LitValue> values;

    public:
        void push(LitValue val)
        {
            this->values.push(val);
        }

        int indexOf(LitValue value)
        {
            size_t i;
            for(i = 0; i < this->values.m_count; i++)
            {
                if(this->values.m_values[i] == value)
                {
                    return (int)i;
                }
            }
            return -1;
        }

        LitValue removeAt(size_t index)
        {
            size_t i;
            size_t count;
            LitValue value;
            PCGenericArray<LitValue>* values;
            values = &this->values;
            count = values->m_count;
            if(index >= count)
            {
                return LitObject::NullVal;
            }
            value = values->m_values[index];
            if(index == count - 1)
            {
                values->m_values[index] = LitObject::NullVal;
            }
            else
            {
                for(i = index; i < values->m_count - 1; i++)
                {
                    values->m_values[i] = values->m_values[i + 1];
                }
                values->m_values[count - 1] = LitObject::NullVal;
            }
            values->m_count--;
            return value;
        }
};

struct LitUserdata: public LitObject
{
    public:
        using CleanupFuncType = void(*)(LitState*, LitUserdata*, bool);

    public:
        void* data;
        size_t size;
        CleanupFuncType cleanup_fn;
        bool canfree;
};

struct LitRange: public LitObject
{
    double from;
    double to;
};



struct LitReference: public LitObject
{
    LitValue* slot;
};

struct LitEmitter
{
    public:
        LitState* state = nullptr;
        LitChunk* chunk = nullptr;
        LitCompiler* compiler = nullptr;
        size_t last_line = 0;
        size_t loop_start = 0;
        PCGenericArray<LitPrivate> privates;
        PCGenericArray<size_t> breaks;
        PCGenericArray<size_t> continues;
        LitModule* module = nullptr;
        LitString* class_name = nullptr;
        bool class_has_super = false;
        bool previous_was_expression_statement = false;
        int emit_reference = 0;

    public:

        void init(LitState* state)
        {
            this->state = state;
            this->loop_start = 0;
            this->emit_reference = 0;
            this->class_name = nullptr;
            this->compiler = nullptr;
            this->chunk = nullptr;
            this->module = nullptr;
            this->previous_was_expression_statement = false;
            this->class_has_super = false;
            this->privates.init(state);
            this->breaks.init(state);
            this->continues.init(state);
        }

        void release()
        {
            this->breaks.release();
            this->continues.release();
        }

        void error(size_t line, LitError error, ...);

        void resolve_statements(PCGenericArray<ASTStatement*>& statements)
        {
            size_t i;
            for(i = 0; i < statements.m_count; i++)
            {
                resolve_statement(statements.m_values[i]);
            }
        }


        void emit_byte(uint16_t line, uint8_t byte)
        {
            if(line < this->last_line)
            {
                // Egor-fail proofing
                line = this->last_line;
            }

            this->chunk->write_chunk(byte, line);
            this->last_line = line;
        }

        void emit_bytes(uint16_t line, uint8_t a, uint8_t b)
        {
            if(line < this->last_line)
            {
                // Egor-fail proofing
                line = this->last_line;
            }

            this->chunk->write_chunk(a, line);
            this->chunk->write_chunk(b, line);

            this->last_line = line;
        }

        void emit_op(uint16_t line, LitOpCode op);
        void emit_ops(uint16_t line, LitOpCode a, LitOpCode b);

        void emit_varying_op(uint16_t line, LitOpCode op, uint8_t arg)
        {
            LitCompiler* compiler = this->compiler;

            emit_bytes(line, (uint8_t)op, arg);
            compiler->slots -= arg;

            if(compiler->slots > (int)compiler->function->max_slots)
            {
                compiler->function->max_slots = (size_t)compiler->slots;
            }
        }



        void emit_arged_op(uint16_t line, LitOpCode op, uint8_t arg);

        void emit_short(uint16_t line, uint16_t value)
        {
            emit_bytes(line, (uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
        }

        void emit_byte_or_short(uint16_t line, uint8_t a, uint8_t b, uint16_t index)
        {
            if(index > UINT8_MAX)
            {
                emit_op(line, (LitOpCode)b);
                emit_short(line, (uint16_t)index);
            }
            else
            {
                emit_arged_op(line, (LitOpCode)a, (uint8_t)index);
            }
        }

        void init_compiler(LitCompiler* compiler, LitFunctionType type);

        void emit_return(size_t line)
        {
            if(this->compiler->type == LITFUNC_CONSTRUCTOR)
            {
                emit_arged_op(line, OP_GET_LOCAL, 0);
                emit_op(line, OP_RETURN);
            }
            else if(this->previous_was_expression_statement && this->chunk->m_count > 0)
            {
                this->chunk->m_count--;// Remove the OP_POP
                emit_op(line, OP_RETURN);
            }
            else
            {
                emit_ops(line, OP_NULL, OP_RETURN);
            }
        }


        LitFunction* end_compiler(LitString* name)
        {
            if(!this->compiler->skip_return)
            {
                emit_return(this->last_line);
                this->compiler->skip_return = true;
            }

            LitFunction* function = this->compiler->function;

            this->compiler->locals.release();

            this->compiler = (LitCompiler*)this->compiler->enclosing;
            this->chunk = this->compiler == nullptr ? nullptr : &this->compiler->function->chunk;

            if(name != nullptr)
            {
                function->name = name;
            }

        #ifdef LIT_TRACE_CHUNK
            lit_disassemble_chunk(&function->chunk, function->name->chars, nullptr);
        #endif

            return function;
        }

        void begin_scope()
        {
            this->compiler->scope_depth++;
        }

        void end_scope(uint16_t line)
        {
            this->compiler->scope_depth--;

            LitCompiler* compiler = this->compiler;
            PCGenericArray<LitLocal>* locals = &compiler->locals;

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

        uint16_t add_constant(size_t line, LitValue value)
        {
            size_t constant = this->chunk->add_constant(value);

            if(constant >= UINT16_MAX)
            {
                error(line, LitError::LITERROR_TOO_MANY_CONSTANTS);
            }

            return constant;
        }

        size_t emit_constant(size_t line, LitValue value)
        {
            size_t constant = this->chunk->add_constant(value);

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
                error(line, LitError::LITERROR_TOO_MANY_CONSTANTS);
            }

            return constant;
        }

        int add_private(const char* name, size_t length, size_t line, bool constant)
        {
            auto privates = &this->privates;
            if(privates->m_count == UINT16_MAX)
            {
                error(line, LitError::LITERROR_TOO_MANY_PRIVATES);
            }
            auto private_names = &this->module->private_names->values;
            auto key = private_names->find(name, length, LitString::makeHash(name, length));
            if(key != nullptr)
            {
                error(line, LitError::LITERROR_VAR_REDEFINED, length, name);
                LitValue index;
                if(private_names->get(key, &index))
                {
                    return LitObject::toNumber(index);
                }
            }
            auto state = this->state;
            int index = (int)privates->m_count;
            privates->push(LitPrivate{ false, constant });
            private_names->set(LitString::copy(state, name, length), LitObject::toValue(index));
            this->module->private_count++;
            return index;
        }

        int resolve_private(const char* name, size_t length, size_t line)
        {
            auto private_names = &this->module->private_names->values;
            auto key = private_names->find(name, length, LitString::makeHash(name, length));
            if(key != nullptr)
            {
                LitValue index;
                if(private_names->get(key, &index))
                {
                    int number_index = LitObject::toNumber(index);
                    if(!this->privates.m_values[number_index].initialized)
                    {
                        error(line, LitError::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                    }
                    return number_index;
                }
            }
            return -1;
        }

        int add_local(const char* name, size_t length, size_t line, bool constant)
        {
            auto compiler = this->compiler;
            auto locals = &compiler->locals;
            if(locals->m_count == UINT16_MAX)
            {
                error(line, LitError::LITERROR_TOO_MANY_LOCALS);
            }
            for(int i = (int)locals->m_count - 1; i >= 0; i--)
            {
                LitLocal* local = &locals->m_values[i];
                if(local->depth != UINT16_MAX && local->depth < compiler->scope_depth)
                {
                    break;
                }
                if(length == local->length && memcmp(local->name, name, length) == 0)
                {
                    error(line, LitError::LITERROR_VAR_REDEFINED, length, name);
                }
            }
            locals->push(LitLocal{ name, length, UINT16_MAX, false, constant });
            return (int)locals->m_count - 1;
        }

        int resolve_local(LitCompiler* compiler, const char* name, size_t length, size_t line)
        {
            auto locals = &compiler->locals;
            for(int i = (int)locals->m_count - 1; i >= 0; i--)
            {
                LitLocal* local = &locals->m_values[i];
                if(local->length == length && memcmp(local->name, name, length) == 0)
                {
                    if(local->depth == UINT16_MAX)
                    {
                        error(line, LitError::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                    }
                    return i;
                }
            }
            return -1;
        }

        int add_upvalue(LitCompiler* compiler, uint8_t index, size_t line, bool is_local)
        {
            size_t upvalue_count = compiler->function->upvalue_count;

            for(size_t i = 0; i < upvalue_count; i++)
            {
                LitCompilerUpvalue* upvalue = &compiler->upvalues[i];

                if(upvalue->index == index && upvalue->isLocal == is_local)
                {
                    return i;
                }
            }

            if(upvalue_count == UINT16_COUNT)
            {
                error(line, LitError::LITERROR_TOO_MANY_UPVALUES);
                return 0;
            }

            compiler->upvalues[upvalue_count].isLocal = is_local;
            compiler->upvalues[upvalue_count].index = index;

            return compiler->function->upvalue_count++;
        }

        int resolve_upvalue(LitCompiler* compiler, const char* name, size_t length, size_t line)
        {
            if(compiler->enclosing == nullptr)
            {
                return -1;
            }

            int local = resolve_local((LitCompiler*)compiler->enclosing, name, length, line);

            if(local != -1)
            {
                ((LitCompiler*)compiler->enclosing)->locals.m_values[local].captured = true;
                return add_upvalue(compiler, (uint8_t)local, line, true);
            }

            int upvalue = resolve_upvalue((LitCompiler*)compiler->enclosing, name, length, line);

            if(upvalue != -1)
            {
                return add_upvalue(compiler, (uint8_t)upvalue, line, false);
            }

            return -1;
        }

        void mark_local_initialized(size_t index)
        {
            this->compiler->locals.m_values[index].depth = this->compiler->scope_depth;
        }

        void mark_private_initialized(size_t index)
        {
            this->privates.m_values[index].initialized = true;
        }

        size_t emit_jump(LitOpCode code, size_t line)
        {
            emit_op(line, code);
            emit_bytes(line, 0xff, 0xff);

            return this->chunk->m_count - 2;
        }

        void patch_jump(size_t offset, size_t line)
        {
            size_t jump = this->chunk->m_count - offset - 2;

            if(jump > UINT16_MAX)
            {
                error(line, LitError::LITERROR_JUMP_TOO_BIG);
            }

            this->chunk->code[offset] = (jump >> 8) & 0xff;
            this->chunk->code[offset + 1] = jump & 0xff;
        }

        void emit_loop(size_t start, size_t line)
        {
            emit_op(line, OP_JUMP_BACK);
            size_t offset = this->chunk->m_count - start + 2;

            if(offset > UINT16_MAX)
            {
                error(line, LitError::LITERROR_JUMP_TOO_BIG);
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

        bool emit_parameters(PCGenericArray<ASTExprFuncParam>* parameters, size_t line)
        {
            for(size_t i = 0; i < parameters->m_count; i++)
            {
                ASTExprFuncParam* parameter = &parameters->m_values[i];

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

        void resolve_statement(ASTStatement* statement)
        {
            if(statement == nullptr)
            {
                return;
            }
            switch(statement->type)
            {
                case ASTStatement::Type::VarDecl:
                    {
                        ASTStmtVar* stmt = (ASTStmtVar*)statement;
                        mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, stmt->constant));
                    }
                    break;
                case ASTStatement::Type::FunctionDecl:
                    {
                        ASTStmtFunction* stmt = (ASTStmtFunction*)statement;
                        if(!stmt->exported)
                        {
                            mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, false));
                        }
                    }
                    break;
                case ASTStatement::Type::ClassDecl:
                case ASTStatement::Type::Block:
                case ASTStatement::Type::ForLoop:
                case ASTStatement::Type::WhileLoop:
                case ASTStatement::Type::IfClause:
                case ASTStatement::Type::ContinueClause:
                case ASTStatement::Type::BreakClause:
                case ASTStatement::Type::ReturnClause:
                case ASTStatement::Type::MethodDecl:
                case ASTStatement::Type::FieldDecl:
                case ASTStatement::Type::Expression:
                    {
                    }
                    break;
                default:
                    break;
            }
        }

        void emit_expression(ASTExpression* expression)
        {
            switch(expression->type)
            {
                case ASTExpression::Type::Literal:
                    {
                        LitValue value = ((ASTExprLiteral*)expression)->value;
                        if(LitObject::isNumber(value) || LitObject::isString(value))
                        {
                            emit_constant(expression->line, value);
                        }
                        else if(LitObject::isBool(value))
                        {
                            emit_op(expression->line, lit_as_bool(value) ? OP_TRUE : OP_FALSE);
                        }
                        else if(LitObject::isNull(value))
                        {
                            emit_op(expression->line, OP_NULL);
                        }
                        else
                        {
                            UNREACHABLE;
                        }
                    }
                    break;
                case ASTExpression::Type::Binary:
                    {
                        ASTExprBinary* expr = (ASTExprBinary*)expression;
                        emit_expression(expr->left);
                        if(expr->right == nullptr)
                        {
                            break;
                        }
                        LitTokenType op = expr->op;
                        if(op == LITTOK_AMPERSAND_AMPERSAND || op == LITTOK_BAR_BAR || op == LITTOK_QUESTION_QUESTION)
                        {
                            size_t jump = emit_jump(op == LITTOK_BAR_BAR ? OP_OR : (op == LITTOK_QUESTION_QUESTION ? OP_NULL_OR : OP_AND),
                                                  this->last_line);
                            emit_expression(expr->right);
                            patch_jump(jump, this->last_line);
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
                                    UNREACHABLE;
                                }
                            break;
                        }
                    }
                    break;
                case ASTExpression::Type::Unary:
                    {
                        ASTExprUnary* expr = (ASTExprUnary*)expression;
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
                                    UNREACHABLE;
                                }
                                break;
                        }
                    }
                    break;
                case ASTExpression::Type::Variable:
                    {
                        ASTExprVar* expr = (ASTExprVar*)expression;
                        bool ref = this->emit_reference > 0;
                        if(ref)
                        {
                            this->emit_reference--;
                        }
                        int index = resolve_local(this->compiler, expr->name, expr->length, expression->line);
                        if(index == -1)
                        {
                            index = resolve_upvalue(this->compiler, expr->name, expr->length, expression->line);
                            if(index == -1)
                            {
                                index = resolve_private(expr->name, expr->length, expression->line);
                                if(index == -1)
                                {
                                    emit_op(expression->line, ref ? OP_REFERENCE_GLOBAL : OP_GET_GLOBAL);
                                    emit_short(expression->line,
                                               add_constant(expression->line,
                                                            LitString::copy(this->state, expr->name, expr->length)->asValue()));
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
                case ASTExpression::Type::Assign:
                    {
                        ASTExprAssign* expr = (ASTExprAssign*)expression;
                        if(expr->to->type == ASTExpression::Type::Variable)
                        {
                            emit_expression(expr->value);
                            ASTExprVar* e = (ASTExprVar*)expr->to;
                            int index = resolve_local(this->compiler, e->name, e->length, expr->to->line);
                            if(index == -1)
                            {
                                index = resolve_upvalue(this->compiler, e->name, e->length, expr->to->line);
                                if(index == -1)
                                {
                                    index = resolve_private(e->name, e->length, expr->to->line);
                                    if(index == -1)
                                    {
                                        emit_op(expression->line, OP_SET_GLOBAL);
                                        emit_short(expression->line,
                                                   add_constant(expression->line,
                                                                LitString::copy(this->state, e->name, e->length)->asValue()));
                                    }
                                    else
                                    {
                                        if(this->privates.m_values[index].constant)
                                        {
                                            error(expression->line, LitError::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
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
                                if(this->compiler->locals.m_values[index].constant)
                                {
                                    error(expression->line, LitError::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                }

                                emit_byte_or_short(expression->line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                            }
                        }
                        else if(expr->to->type == ASTExpression::Type::Get)
                        {
                            emit_expression(expr->value);
                            ASTExprIndexGet* e = (ASTExprIndexGet*)expr->to;
                            emit_expression(e->where);
                            emit_expression(expr->value);
                            emit_constant(this->last_line, LitString::copy(this->state, e->name, e->length)->asValue());
                            emit_ops(this->last_line, OP_SET_FIELD, OP_POP);
                        }
                        else if(expr->to->type == ASTExpression::Type::Subscript)
                        {
                            ASTExprSubscript* e = (ASTExprSubscript*)expr->to;
                            emit_expression(e->array);
                            emit_expression(e->index);
                            emit_expression(expr->value);
                            emit_op(this->last_line, OP_SUBSCRIPT_SET);
                        }
                        else if(expr->to->type == ASTExpression::Type::Reference)
                        {
                            emit_expression(expr->value);
                            emit_expression(((ASTExprReference*)expr->to)->to);
                            emit_op(expression->line, OP_SET_REFERENCE);
                        }
                        else
                        {
                            error(expression->line, LitError::LITERROR_INVALID_ASSIGMENT_TARGET);
                        }
                    }
                    break;
                case ASTExpression::Type::Call:
                    {
                        ASTExprCall* expr = (ASTExprCall*)expression;
                        bool method = expr->callee->type == ASTExpression::Type::Get;
                        bool super = expr->callee->type == ASTExpression::Type::Super;
                        if(method)
                        {
                            ((ASTExprIndexGet*)expr->callee)->ignore_emit = true;
                        }
                        else if(super)
                        {
                            ((ASTExprSuper*)expr->callee)->ignore_emit = true;
                        }
                        emit_expression(expr->callee);
                        if(super)
                        {
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                        }
                        for(size_t i = 0; i < expr->args.m_count; i++)
                        {
                            ASTExpression* e = expr->args.m_values[i];
                            if(e->type == ASTExpression::Type::Variable)
                            {
                                ASTExprVar* ee = (ASTExprVar*)e;
                                // Vararg ...
                                if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                                {
                                    emit_arged_op(e->line, OP_VARARG,
                                                  resolve_local(this->compiler, "...", 3, expression->line));
                                    break;
                                }
                            }
                            emit_expression(e);
                        }
                        if(method || super)
                        {
                            if(method)
                            {
                                ASTExprIndexGet* e = (ASTExprIndexGet*)expr->callee;

                                emit_varying_op(expression->line,
                                                ((ASTExprIndexGet*)expr->callee)->ignore_result ? OP_INVOKE_IGNORING : OP_INVOKE,
                                                (uint8_t)expr->args.m_count);
                                emit_short(this->last_line,
                                           add_constant(this->last_line,
                                                        LitString::copy(this->state, e->name, e->length)->asValue()));
                            }
                            else
                            {
                                ASTExprSuper* e = (ASTExprSuper*)expr->callee;
                                uint8_t index = resolve_upvalue(this->compiler, "super", 5, this->last_line);
                                emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                                emit_varying_op(this->last_line,
                                                ((ASTExprSuper*)expr->callee)->ignore_result ? OP_INVOKE_SUPER_IGNORING : OP_INVOKE_SUPER,
                                                (uint8_t)expr->args.m_count);
                                emit_short(this->last_line, add_constant(this->last_line, e->method->asValue()));
                            }
                        }
                        else
                        {
                            emit_varying_op(expression->line, OP_CALL, (uint8_t)expr->args.m_count);
                        }
                        if(method)
                        {
                            ASTExpression* get = expr->callee;
                            while(get != nullptr)
                            {
                                if(get->type == ASTExpression::Type::Get)
                                {
                                    ASTExprIndexGet* getter = (ASTExprIndexGet*)get;
                                    if(getter->jump > 0)
                                    {
                                        patch_jump(getter->jump, this->last_line);
                                    }
                                    get = getter->where;
                                }
                                else if(get->type == ASTExpression::Type::Subscript)
                                {
                                    get = ((ASTExprSubscript*)get)->array;
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
                        ASTExprObject* objinit = (ASTExprObject*)expr->objexpr;
                        for(size_t i = 0; i < objinit->values.m_count; i++)
                        {
                            ASTExpression* e = objinit->values.m_values[i];
                            this->last_line = e->line;
                            emit_constant(this->last_line, objinit->keys.m_values[i]);
                            emit_expression(e);
                            emit_op(this->last_line, OP_PUSH_OBJECT_FIELD);
                        }
                    }
                    break;
                case ASTExpression::Type::Get:
                    {
                        ASTExprIndexGet* expr = (ASTExprIndexGet*)expression;
                        bool ref = this->emit_reference > 0;
                        if(ref)
                        {
                            this->emit_reference--;
                        }
                        emit_expression(expr->where);
                        if(expr->jump == 0)
                        {
                            expr->jump = emit_jump(OP_JUMP_IF_NULL, this->last_line);
                            if(!expr->ignore_emit)
                            {
                                emit_constant(this->last_line,
                                              LitString::copy(this->state, expr->name, expr->length)->asValue());
                                emit_op(this->last_line, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                            }
                            patch_jump(expr->jump, this->last_line);
                        }
                        else if(!expr->ignore_emit)
                        {
                            emit_constant(this->last_line, LitString::copy(this->state, expr->name, expr->length)->asValue());
                            emit_op(this->last_line, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                        }
                    }
                    break;
                case ASTExpression::Type::Set:
                    {
                        ASTExprIndexSet* expr = (ASTExprIndexSet*)expression;
                        emit_expression(expr->where);
                        emit_expression(expr->value);
                        emit_constant(this->last_line, LitString::copy(this->state, expr->name, expr->length)->asValue());
                        emit_op(this->last_line, OP_SET_FIELD);
                    }
                    break;
                case ASTExpression::Type::Lambda:
                    {
                        ASTExprLambda* expr = (ASTExprLambda*)expression;
                        LitString* name = LitString::format(this->state, "lambda @:@", this->module->name->asValue(),
                                                                      LitString::stringNumberToString(this->state, expression->line));
                        LitCompiler compiler;
                        init_compiler(&compiler, LITFUNC_REGULAR);
                        begin_scope();
                        bool vararg = emit_parameters(&expr->parameters, expression->line);
                        if(expr->body != nullptr)
                        {
                            bool single_expression = expr->body->type == ASTStatement::Type::Expression;
                            if(single_expression)
                            {
                                compiler.skip_return = true;
                                ((ASTExprStatement*)expr->body)->pop = false;
                            }
                            emit_statement(expr->body);
                            if(single_expression)
                            {
                                emit_op(this->last_line, OP_RETURN);
                            }
                        }
                        end_scope(this->last_line);
                        LitFunction* function = end_compiler(name);
                        function->arg_count = expr->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(this->last_line, OP_CLOSURE);
                            emit_short(this->last_line, add_constant(this->last_line, function->asValue()));
                            for(size_t i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(this->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(this->last_line, function->asValue());
                        }
                    }
                    break;
                case ASTExpression::Type::Array:
                    {
                        ASTExprArray* expr = (ASTExprArray*)expression;
                        emit_op(expression->line, OP_ARRAY);
                        for(size_t i = 0; i < expr->values.m_count; i++)
                        {
                            emit_expression(expr->values.m_values[i]);
                            emit_op(this->last_line, OP_PUSH_ARRAY_ELEMENT);
                        }
                    }
                    break;
                case ASTExpression::Type::Object:
                    {
                        ASTExprObject* expr = (ASTExprObject*)expression;
                        emit_op(expression->line, OP_OBJECT);
                        for(size_t i = 0; i < expr->values.m_count; i++)
                        {
                            emit_constant(this->last_line, expr->keys.m_values[i]);
                            emit_expression(expr->values.m_values[i]);
                            emit_op(this->last_line, OP_PUSH_OBJECT_FIELD);
                        }
                    }
                    break;
                case ASTExpression::Type::Subscript:
                    {
                        ASTExprSubscript* expr = (ASTExprSubscript*)expression;
                        emit_expression(expr->array);
                        emit_expression(expr->index);
                        emit_op(expression->line, OP_SUBSCRIPT_GET);
                    }
                    break;
                case ASTExpression::Type::This:
                    {
                        LitFunctionType type = this->compiler->type;
                        if(type == LITFUNC_STATIC_METHOD)
                        {
                            error(expression->line, LitError::LITERROR_THIS_MISSUSE, "in static methods");
                        }
                        if(type == LITFUNC_CONSTRUCTOR || type == LITFUNC_METHOD)
                        {
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                        }
                        else
                        {
                            if(this->compiler->enclosing == nullptr)
                            {
                                error(expression->line, LitError::LITERROR_THIS_MISSUSE, "in functions outside of any class");
                            }
                            else
                            {
                                int local = resolve_local((LitCompiler*)this->compiler->enclosing, "this", 4, expression->line);
                                emit_arged_op(expression->line, OP_GET_UPVALUE,
                                              add_upvalue(this->compiler, local, expression->line, true));
                            }
                        }
                    }
                    break;
                case ASTExpression::Type::Super:
                    {
                        if(this->compiler->type == LITFUNC_STATIC_METHOD)
                        {
                            error(expression->line, LitError::LITERROR_SUPER_MISSUSE, "in static methods");
                        }
                        else if(!this->class_has_super)
                        {
                            error(expression->line, LitError::LITERROR_NO_SUPER, this->class_name->chars);
                        }
                        ASTExprSuper* expr = (ASTExprSuper*)expression;
                        if(!expr->ignore_emit)
                        {
                            uint8_t index = resolve_upvalue(this->compiler, "super", 5, this->last_line);
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                            emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                            emit_op(expression->line, OP_GET_SUPER_METHOD);
                            emit_short(expression->line, add_constant(expression->line, expr->method->asValue()));
                        }
                    }
                    break;
                case ASTExpression::Type::Range:
                    {
                        ASTExprRange* expr = (ASTExprRange*)expression;
                        emit_expression(expr->to);
                        emit_expression(expr->from);
                        emit_op(expression->line, OP_RANGE);
                    }
                    break;
                case ASTExpression::Type::IfClause:
                    {
                        ASTExprIfClause* expr = (ASTExprIfClause*)expression;
                        emit_expression(expr->condition);
                        uint64_t else_jump = emit_jump(OP_JUMP_IF_FALSE, expression->line);
                        emit_expression(expr->if_branch);
                        uint64_t end_jump = emit_jump(OP_JUMP, this->last_line);
                        patch_jump(else_jump, expr->else_branch->line);
                        emit_expression(expr->else_branch);

                        patch_jump(end_jump, this->last_line);
                    }
                    break;
                case ASTExpression::Type::Interpolation:
                    {
                        ASTExprInterpolation* expr = (ASTExprInterpolation*)expression;
                        emit_op(expression->line, OP_ARRAY);
                        for(size_t i = 0; i < expr->expressions.m_count; i++)
                        {
                            emit_expression(expr->expressions.m_values[i]);
                            emit_op(this->last_line, OP_PUSH_ARRAY_ELEMENT);
                        }
                        emit_varying_op(this->last_line, OP_INVOKE, 0);
                        emit_short(this->last_line,
                                   add_constant(this->last_line, OBJECT_CONST_STRING(this->state, "join")));
                    }
                    break;
                case ASTExpression::Type::Reference:
                    {
                        ASTExpression* to = ((ASTExprReference*)expression)->to;
                        if(to->type != ASTExpression::Type::Variable && to->type != ASTExpression::Type::Get && to->type != ASTExpression::Type::This && to->type != ASTExpression::Type::Super)
                        {
                            error(expression->line, LitError::LITERROR_INVALID_REFERENCE_TARGET);
                            break;
                        }
                        int old = this->emit_reference;
                        this->emit_reference++;
                        emit_expression(to);
                        this->emit_reference = old;
                    }
                    break;
                default:
                    {
                        error(expression->line, LitError::LITERROR_UNKNOWN_EXPRESSION, (int)expression->type);
                    }
                    break;
            }
        }

        bool emit_statement(ASTStatement* statement)
        {
            ASTStmtClass* clstmt;
            LitCompiler compiler;
            ASTExpression* expression;
            ASTExpression* e;
            ASTExprStatement* expr;
            LitField* field;
            ASTStmtField* fieldstmt;
            LitFunction* function;
            LitFunction* getter;
            LitFunction* setter;
            ASTStmtFunction* funcstmt;
            LitLocal* local;
            PCGenericArray<LitLocal>* locals;
            ASTStmtMethod* mthstmt;
            ASTStatement* blockstmt;
            ASTStatement* s;
            PCGenericArray<ASTStatement*>* statements;
            LitString* name;
            ASTStmtVar* var;
            ASTStmtVar* varstmt;
            ASTStmtForLoop* forstmt;
            ASTStmtWhileLoop* whilestmt;
            ASTStmtIfClause* ifstmt;
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
                case ASTStatement::Type::Expression:
                    {
                        expr = (ASTExprStatement*)statement;
                        emit_expression(expr->expression);
                        if(expr->pop)
                        {
                            emit_op(statement->line, OP_POP);
                        }
                    }
                    break;
                case ASTStatement::Type::Block:
                    {
                        statements = &((ASTStmtBlock*)statement)->statements;
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
                        end_scope(this->last_line);
                    }
                    break;
                case ASTStatement::Type::VarDecl:
                    {
                        varstmt = (ASTStmtVar*)statement;
                        line = statement->line;
                        isprivate = this->compiler->enclosing == nullptr && this->compiler->scope_depth == 0;
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
                case ASTStatement::Type::IfClause:
                    {
                        ifstmt = (ASTStmtIfClause*)statement;
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
                            end_jump = emit_jump(OP_JUMP, this->last_line);
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
                                else_jump = emit_jump(OP_JUMP_IF_FALSE, this->last_line);
                                emit_statement(ifstmt->elseif_branches->m_values[i]);
                                end_jumps[i] = emit_jump(OP_JUMP, this->last_line);
                            }
                        }
                        if(ifstmt->else_branch != nullptr)
                        {
                            patch_jump(else_jump, ifstmt->else_branch->line);
                            emit_statement(ifstmt->else_branch);
                        }
                        else
                        {
                            patch_jump(else_jump, this->last_line);
                        }
                        if(end_jump != 0)
                        {
                            patch_jump(end_jump, this->last_line);
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
                case ASTStatement::Type::WhileLoop:
                    {
                        whilestmt = (ASTStmtWhileLoop*)statement;
                        start = this->chunk->m_count;
                        this->loop_start = start;
                        this->compiler->loop_depth++;
                        emit_expression(whilestmt->condition);
                        exit_jump = emit_jump(OP_JUMP_IF_FALSE, statement->line);
                        emit_statement(whilestmt->body);
                        patch_loop_jumps(&this->continues, this->last_line);
                        emit_loop(start, this->last_line);
                        patch_jump(exit_jump, this->last_line);
                        patch_loop_jumps(&this->breaks, this->last_line);
                        this->compiler->loop_depth--;
                    }
                    break;
                case ASTStatement::Type::ForLoop:
                    {
                        forstmt = (ASTStmtForLoop*)statement;
                        begin_scope();
                        this->compiler->loop_depth++;
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
                            start = this->chunk->m_count;
                            exit_jump = 0;
                            if(forstmt->condition != nullptr)
                            {
                                emit_expression(forstmt->condition);
                                exit_jump = emit_jump(OP_JUMP_IF_FALSE, this->last_line);
                            }
                            if(forstmt->increment != nullptr)
                            {
                                body_jump = emit_jump(OP_JUMP, this->last_line);
                                increment_start = this->chunk->m_count;
                                emit_expression(forstmt->increment);
                                emit_op(this->last_line, OP_POP);
                                emit_loop(start, this->last_line);
                                start = increment_start;
                                patch_jump(body_jump, this->last_line);
                            }
                            this->loop_start = start;
                            begin_scope();
                            if(forstmt->body != nullptr)
                            {
                                if(forstmt->body->type == ASTStatement::Type::Block)
                                {
                                    statements = &((ASTStmtBlock*)forstmt->body)->statements;
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
                            patch_loop_jumps(&this->continues, this->last_line);
                            end_scope(this->last_line);
                            emit_loop(start, this->last_line);
                            if(forstmt->condition != nullptr)
                            {
                                patch_jump(exit_jump, this->last_line);
                            }
                        }
                        else
                        {
                            sequence = add_local("seq ", 4, statement->line, false);
                            mark_local_initialized(sequence);
                            emit_expression(forstmt->condition);
                            emit_byte_or_short(this->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, sequence);
                            iterator = add_local("iter ", 5, statement->line, false);
                            mark_local_initialized(iterator);
                            emit_op(this->last_line, OP_NULL);
                            emit_byte_or_short(this->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                            start = this->chunk->m_count;
                            this->loop_start = this->chunk->m_count;
                            // iter = seq.iterator(iter)
                            emit_byte_or_short(this->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                            emit_byte_or_short(this->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                            emit_varying_op(this->last_line, OP_INVOKE, 1);
                            emit_short(this->last_line,
                                       add_constant(this->last_line, OBJECT_CONST_STRING(this->state, "iterator")));
                            emit_byte_or_short(this->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                            // If iter is null, just get out of the loop
                            exit_jump = emit_jump(OP_JUMP_IF_NULL_POPPING, this->last_line);
                            begin_scope();
                            // var i = seq.iteratorValue(iter)
                            var = (ASTStmtVar*)forstmt->var;
                            localcnt = add_local(var->name, var->length, statement->line, false);
                            mark_local_initialized(localcnt);
                            emit_byte_or_short(this->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                            emit_byte_or_short(this->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                            emit_varying_op(this->last_line, OP_INVOKE, 1);
                            emit_short(this->last_line,
                                       add_constant(this->last_line, OBJECT_CONST_STRING(this->state, "iteratorValue")));
                            emit_byte_or_short(this->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, localcnt);
                            if(forstmt->body != nullptr)
                            {
                                if(forstmt->body->type == ASTStatement::Type::Block)
                                {
                                    statements = &((ASTStmtBlock*)forstmt->body)->statements;
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
                            patch_loop_jumps(&this->continues, this->last_line);
                            end_scope(this->last_line);
                            emit_loop(start, this->last_line);
                            patch_jump(exit_jump, this->last_line);
                        }
                        patch_loop_jumps(&this->breaks, this->last_line);
                        end_scope(this->last_line);
                        this->compiler->loop_depth--;
                    }
                    break;

                case ASTStatement::Type::ContinueClause:
                {
                    if(this->compiler->loop_depth == 0)
                    {
                        error(statement->line, LitError::LITERROR_LOOP_JUMP_MISSUSE, "continue");
                    }
                    this->continues.push(emit_jump(OP_JUMP, statement->line));
                    break;
                }

                case ASTStatement::Type::BreakClause:
                    {
                        if(this->compiler->loop_depth == 0)
                        {
                            error(statement->line, LitError::LITERROR_LOOP_JUMP_MISSUSE, "break");
                        }
                        emit_op(statement->line, OP_POP_LOCALS);
                        depth = this->compiler->scope_depth;
                        local_count = 0;
                        locals = &this->compiler->locals;
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
                        this->breaks.push(emit_jump(OP_JUMP, statement->line));
                    }
                    break;
                case ASTStatement::Type::FunctionDecl:
                    {
                        funcstmt = (ASTStmtFunction*)statement;
                        isexport = funcstmt->exported;
                        isprivate = !isexport && this->compiler->enclosing == nullptr && this->compiler->scope_depth == 0;
                        islocal = !(isexport || isprivate);
                        index = 0;
                        if(!isexport)
                        {
                            index = isprivate ? resolve_private(funcstmt->name, funcstmt->length, statement->line) :
                                              add_local(funcstmt->name, funcstmt->length, statement->line, false);
                        }
                        name = LitString::copy(this->state, funcstmt->name, funcstmt->length);
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
                        end_scope(this->last_line);
                        function = end_compiler(name);
                        function->arg_count = funcstmt->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(this->last_line, OP_CLOSURE);
                            emit_short(this->last_line, add_constant(this->last_line, function->asValue()));
                            for(i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(this->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(this->last_line, function->asValue());
                        }
                        if(isexport)
                        {
                            emit_op(this->last_line, OP_SET_GLOBAL);
                            emit_short(this->last_line, add_constant(this->last_line, name->asValue()));
                        }
                        else if(isprivate)
                        {
                            emit_byte_or_short(this->last_line, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                        }
                        else
                        {
                            emit_byte_or_short(this->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                        }
                        emit_op(this->last_line, OP_POP);
                    }
                    break;
                case ASTStatement::Type::ReturnClause:
                    {
                        if(this->compiler->type == LITFUNC_CONSTRUCTOR)
                        {
                            error(statement->line, LitError::LITERROR_RETURN_FROM_CONSTRUCTOR);
                        }
                        expression = ((ASTStmtReturn*)statement)->expression;
                        if(expression == nullptr)
                        {
                            emit_op(this->last_line, OP_NULL);
                        }
                        else
                        {
                            emit_expression(expression);
                        }
                        emit_op(this->last_line, OP_RETURN);
                        if(this->compiler->scope_depth == 0)
                        {
                            this->compiler->skip_return = true;
                        }
                        return true;
                    }
                    break;
                case ASTStatement::Type::MethodDecl:
                    {
                        mthstmt = (ASTStmtMethod*)statement;
                        constructor = mthstmt->name->length() == (sizeof(LIT_NAME_CONSTRUCTOR)-1) && memcmp(mthstmt->name->chars, LIT_NAME_CONSTRUCTOR, strlen(LIT_NAME_CONSTRUCTOR)-1) == 0;
                        if(constructor && mthstmt->is_static)
                        {
                            error(statement->line, LitError::LITERROR_STATIC_CONSTRUCTOR);
                        }
                        init_compiler(&compiler,
                                      constructor ? LITFUNC_CONSTRUCTOR : (mthstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD));
                        begin_scope();
                        vararg = emit_parameters(&mthstmt->parameters, statement->line);
                        emit_statement(mthstmt->body);
                        end_scope(this->last_line);
                        function = end_compiler(LitString::format(this->state, "@:@", this->class_name->asValue(), mthstmt->name->asValue()));
                        function->arg_count = mthstmt->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(this->last_line, OP_CLOSURE);
                            emit_short(this->last_line, add_constant(this->last_line, function->asValue()));
                            for(i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(this->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(this->last_line, function->asValue());
                        }
                        emit_op(this->last_line, mthstmt->is_static ? OP_STATIC_FIELD : OP_METHOD);
                        emit_short(this->last_line, add_constant(statement->line, mthstmt->name->asValue()));

                    }
                    break;
                case ASTStatement::Type::ClassDecl:
                    {
                        clstmt = (ASTStmtClass*)statement;
                        this->class_name = clstmt->name;
                        if(clstmt->parent != nullptr)
                        {
                            emit_op(this->last_line, OP_GET_GLOBAL);
                            emit_short(this->last_line, add_constant(this->last_line, clstmt->parent->asValue()));
                        }
                        emit_op(statement->line, OP_CLASS);
                        emit_short(this->last_line, add_constant(this->last_line, clstmt->name->asValue()));
                        if(clstmt->parent != nullptr)
                        {
                            emit_op(this->last_line, OP_INHERIT);
                            this->class_has_super = true;
                            begin_scope();
                            super = add_local("super", 5, this->last_line, false);
                            
                            mark_local_initialized(super);
                        }
                        for(i = 0; i < clstmt->fields.m_count; i++)
                        {
                            s = clstmt->fields.m_values[i];
                            if(s->type == ASTStatement::Type::VarDecl)
                            {
                                var = (ASTStmtVar*)s;
                                emit_expression(var->valexpr);
                                emit_op(statement->line, OP_STATIC_FIELD);
                                emit_short(statement->line,
                                           add_constant(statement->line,
                                                        LitString::copy(this->state, var->name, var->length)->asValue()));
                            }
                            else
                            {
                                emit_statement(s);
                            }
                        }
                        emit_op(this->last_line, OP_POP);
                        if(clstmt->parent != nullptr)
                        {
                            end_scope(this->last_line);
                        }
                        this->class_name = nullptr;
                        this->class_has_super = false;
                    }
                    break;
                case ASTStatement::Type::FieldDecl:
                    {
                        fieldstmt = (ASTStmtField*)statement;
                        getter = nullptr;
                        setter = nullptr;
                        if(fieldstmt->getter != nullptr)
                        {
                            init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                            begin_scope();
                            emit_statement(fieldstmt->getter);
                            end_scope(this->last_line);
                            getter = end_compiler(LitString::format(this->state, "@:get @", this->class_name->asValue(), fieldstmt->name));
                        }
                        if(fieldstmt->setter != nullptr)
                        {
                            init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                            mark_local_initialized(add_local("value", 5, statement->line, false));
                            begin_scope();
                            emit_statement(fieldstmt->setter);
                            end_scope(this->last_line);
                            setter = end_compiler(LitString::format(this->state, "@:set @", this->class_name->asValue(), fieldstmt->name));
                            setter->arg_count = 1;
                            setter->max_slots++;
                        }
                        field = LitField::make(this->state, (LitObject*)getter, (LitObject*)setter);
                        emit_constant(statement->line, field->asValue());
                        emit_op(statement->line, fieldstmt->is_static ? OP_STATIC_FIELD : OP_DEFINE_FIELD);
                        emit_short(statement->line, add_constant(statement->line, fieldstmt->name->asValue()));
                    }
                    break;
                default:
                    {
                        error(statement->line, LitError::LITERROR_UNKNOWN_STATEMENT, (int)statement->type);
                    }
                    break;
            }
            this->previous_was_expression_statement = statement->type == ASTStatement::Type::Expression;
            return false;
        }

        LitModule* run_emitter(PCGenericArray<ASTStatement*>& statements, LitString* module_name);

};

struct LitState
{
    public:
        using ErrorFuncType = void(*)(LitState*, const char*);
        using PrintFuncType = void(*)(LitState*, const char*);

    public:
        static void default_error(LitState* state, const char* message)
        {
            (void)state;
            fflush(stdout);
            fprintf(stderr, "%s%s%s\n", COLOR_RED, message, COLOR_RESET);
            fflush(stderr);
        }

        static void default_printf(LitState* state, const char* message)
        {
            (void)state;
            printf("%s", message);
        }

        static void init_api(LitState* state)
        {
            state->api_name = LitString::copy(state, "c", 1);
            state->api_function = nullptr;
            state->api_fiber = nullptr;
        }

        static LitState* make();

    public:
        /* how much was allocated in total? */
        int64_t bytes_allocated;
        int64_t next_gc;
        bool allow_gc;
        ErrorFuncType error_fn;
        PrintFuncType print_fn;
        LitValue* roots;
        size_t root_count;
        size_t root_capacity;
        LitPreprocessor* preprocessor;
        LitScanner* scanner;
        LitParser* parser;
        LitEmitter* emitter;
        LitOptimizer* optimizer;
        /*
        * recursive pointer to the current VM instance.
        * using 'state->vm->state' will in turn mean this instance, etc.
        */
        LitVM* vm;
        bool had_error;
        LitFunction* api_function;
        LitFiber* api_fiber;
        LitString* api_name;
        /* when using debug routines, this is the writer that output is called on */
        LitWriter debugwriter;
        // class class
        // Mental note:
        // When adding another class here, DO NOT forget to mark it in lit_mem.c or it will be GC-ed
        LitClass* classvalue_class;
        LitClass* objectvalue_class;
        LitClass* numbervalue_class;
        LitClass* stringvalue_class;
        LitClass* boolvalue_class;
        LitClass* functionvalue_class;
        LitClass* fibervalue_class;
        LitClass* modulevalue_class;
        LitClass* arrayvalue_class;
        LitClass* mapvalue_class;
        LitClass* rangevalue_class;
        LitModule* last_module;

    public:
        void init(LitVM* vm);
        int64_t release();

        void raiseError(LitErrorType type, const char* message, ...)
        {
            size_t buffer_size;
            char* buffer;
            va_list args;
            va_list args_copy;
            (void)type;
            va_start(args, message);
            va_copy(args_copy, args);
            buffer_size = vsnprintf(nullptr, 0, message, args_copy) + 1;
            va_end(args_copy);
            buffer = (char*)malloc(buffer_size+1);
            vsnprintf(buffer, buffer_size, message, args);
            va_end(args);
            this->error_fn(this, buffer);
            this->had_error = true;
            /* TODO: is this safe? */
            free(buffer);
        }

        void releaseAPI()
        {
            this->api_name = nullptr;
            this->api_function = nullptr;
            this->api_fiber = nullptr;
        }

        /* releases given objects, and their subobjects. */
        void releaseObjects(LitObject* objects);

        void pushRoot(LitObject* obj)
        {
            pushValueRoot(obj->asValue());
        }

        void pushValueRoot(LitValue value)
        {
            if(this->root_count + 1 >= this->root_capacity)
            {
                this->root_capacity = LIT_GROW_CAPACITY(this->root_capacity);
                this->roots = (LitValue*)realloc(this->roots, this->root_capacity * sizeof(LitValue));
            }
            this->roots[this->root_count++] = value;
        }

        LitValue peekRoot(uint8_t distance)
        {
            assert(this->root_count - distance + 1 > 0);
            return this->roots[this->root_count - distance - 1];
        }

        void popRoot()
        {
            this->root_count--;
        }

        void popRoots(uint8_t amount)
        {
            this->root_count -= amount;
        }

        LitClass* getClassFor(LitValue value)
        {
            LitValue* slot;
            LitUpvalue* upvalue;
            if(LitObject::isObject(value))
            {
                switch(OBJECT_TYPE(value))
                {
                    case LitObject::Type::String:
                        {
                            return this->stringvalue_class;
                        }
                        break;
                    case LitObject::Type::Userdata:
                        {
                            return this->objectvalue_class;
                        }
                        break;
                    case LitObject::Type::Field:
                    case LitObject::Type::Function:
                    case LitObject::Type::Closure:
                    case LitObject::Type::NativeFunction:
                    case LitObject::Type::NativePrimitive:
                    case LitObject::Type::BoundMethod:
                    case LitObject::Type::PrimitiveMethod:
                    case LitObject::Type::NativeMethod:
                        {
                            return this->functionvalue_class;
                        }
                        break;
                    case LitObject::Type::Fiber:
                        {
                            //fprintf(stderr, "should return fiber class ....\n");
                            return this->fibervalue_class;
                        }
                        break;
                    case LitObject::Type::Module:
                        {
                            return this->modulevalue_class;
                        }
                        break;
                    case LitObject::Type::Upvalue:
                        {
                            upvalue = AS_UPVALUE(value);
                            if(upvalue->location == nullptr)
                            {
                                return this->getClassFor(upvalue->closed);
                            }
                            return this->getClassFor(*upvalue->location);
                        }
                        break;
                    case LitObject::Type::Instance:
                        {
                            return LitObject::as<LitInstance>(value)->klass;
                        }
                        break;
                    case LitObject::Type::Class:
                        {
                            return this->classvalue_class;
                        }
                        break;
                    case LitObject::Type::Array:
                        {
                            return this->arrayvalue_class;
                        }
                        break;
                    case LitObject::Type::Map:
                        {
                            return this->mapvalue_class;
                        }
                        break;
                    case LitObject::Type::Range:
                        {
                            return this->rangevalue_class;
                        }
                        break;
                    case LitObject::Type::Reference:
                        {
                            slot = AS_REFERENCE(value)->slot;
                            if(slot != nullptr)
                            {
                                return this->getClassFor(*slot);
                            }

                            return this->objectvalue_class;
                        }
                        break;
                }
            }
            else if(LitObject::isNumber(value))
            {
                return this->numbervalue_class;
            }
            else if(LitObject::isBool(value))
            {
                return this->boolvalue_class;
            }
            //fprintf(stderr, "failed to find class object!\n");
            return nullptr;
        }

        void setVMGlobal(LitString* name, LitValue val);
        bool getVMGlobal(LitString* name, LitValue* dest);

        LitValue getGlobal(LitString* name)
        {
            LitValue global;
            if(!this->getVMGlobal(name, &global))
            {
                return LitObject::NullVal;
            }
            return global;
        }

        LitFunction* getGlobalFunction(LitString* name)
        {
            LitValue function = getGlobal(name);
            if(LitObject::isFunction(function))
            {
                return LitObject::as<LitFunction>(function);
            }
            return nullptr;
        }

        void setGlobal(LitString* name, LitValue value)
        {
            this->pushRoot((LitObject*)name);
            this->pushValueRoot(value);
            this->setVMGlobal(name, value);
            this->popRoots(2);
        }

        bool globalExists(LitString* name)
        {
            LitValue global;
            return this->getVMGlobal(name, &global);
        }

        void defineNative(const char* name, LitNativeFunction::FuncType native)
        {
            this->pushRoot((LitObject*)CONST_STRING(this, name));
            this->pushRoot((LitObject*)LitNativeFunction::make(this, native, LitObject::as<LitString>(this->peekRoot(0))));
            this->setVMGlobal(LitObject::as<LitString>(this->peekRoot(1)), this->peekRoot(0));
            this->popRoots(2);
        }

        void defineNativePrimitive(const char* name, LitNativePrimFunction::FuncType native)
        {
            this->pushRoot((LitObject*)CONST_STRING(this, name));
            this->pushRoot((LitObject*)LitNativePrimFunction::make(this, native, LitObject::as<LitString>(this->peekRoot(0))));
            this->setVMGlobal(LitObject::as<LitString>(this->peekRoot(1)), this->peekRoot(0));
            this->popRoots(2);
        }

};

struct LitVM
{

    public:
        /* the current state */
        LitState* state;
        /* currently held objects */
        LitObject* objects;
        /* currently cached strings */
        LitTable strings;
        /* currently loaded/defined modules */
        LitMap* modules;
        /* currently defined globals */
        LitMap* globals;
        LitFiber* fiber;
        // For garbage collection
        size_t gray_count;
        size_t gray_capacity;
        LitObject** gray_stack;

    public:
        void release()
        {
            this->strings.release();
            this->state->releaseObjects(this->objects);
            this->reset(this->state);
        }

        void reset(LitState* state)
        {
            this->state = state;
            this->objects = nullptr;
            this->fiber = nullptr;
            this->gray_stack = nullptr;
            this->gray_count = 0;
            this->gray_capacity = 0;
            this->strings.init(state);
            this->globals = nullptr;
            this->modules = nullptr;
        }

        void markObject(LitObject* obj)
        {
            if(obj == nullptr || obj->marked)
            {
                return;
            }
            obj->marked = true;
        #ifdef LIT_LOG_MARKING
            printf("%p mark ", (void*)obj);
            lit_print_value(obj->asValue());
            printf("\n");
        #endif
            if(this->gray_capacity < this->gray_count + 1)
            {
                this->gray_capacity = LIT_GROW_CAPACITY(this->gray_capacity);
                this->gray_stack = (LitObject**)realloc(this->gray_stack, sizeof(LitObject*) * this->gray_capacity);
            }
            this->gray_stack[this->gray_count++] = obj;
        }

        void markValue(LitValue value)
        {
            if(LitObject::isObject(value))
            {
                markObject(LitObject::asObject(value));
            }
        }

        void markArray(PCGenericArray<LitValue>* array)
        {
            size_t i;
            for(i = 0; i < array->m_count; i++)
            {
                this->markValue(array->m_values[i]);
            }
        }

        void markRoots();

        void blackenObject(LitObject* obj)
        {
            size_t i;
            LitUserdata* data;
            LitFunction* function;
            LitFiber* fiber;
            LitUpvalue* upvalue;
            LitCallFrame* frame;
            LitModule* module;
            LitClosure* closure;
            LitClass* klass;
            LitBoundMethod* bound_method;
            LitField* field;

        #ifdef LIT_LOG_BLACKING
            printf("%p blacken ", (void*)obj);
            lit_print_value(obj->asValue());
            printf("\n");
        #endif
            switch(obj->type)
            {
                case LitObject::Type::NativeFunction:
                case LitObject::Type::NativePrimitive:
                case LitObject::Type::NativeMethod:
                case LitObject::Type::PrimitiveMethod:
                case LitObject::Type::Range:
                case LitObject::Type::String:
                    {
                    }
                    break;
                case LitObject::Type::Userdata:
                    {
                        data = (LitUserdata*)obj;
                        if(data->cleanup_fn != nullptr)
                        {
                            data->cleanup_fn(this->state, data, true);
                        }
                    }
                    break;
                case LitObject::Type::Function:
                    {
                        function = (LitFunction*)obj;
                        this->markObject((LitObject*)function->name);
                        this->markArray(&function->chunk.constants);
                    }
                    break;
                case LitObject::Type::Fiber:
                    {
                        fiber = (LitFiber*)obj;
                        for(LitValue* slot = fiber->stack; slot < fiber->stack_top; slot++)
                        {
                            this->markValue(*slot);
                        }
                        for(i = 0; i < fiber->frame_count; i++)
                        {
                            frame = &fiber->frames[i];
                            if(frame->closure != nullptr)
                            {
                                this->markObject((LitObject*)frame->closure);
                            }
                            else
                            {
                                this->markObject((LitObject*)frame->function);
                            }
                        }
                        for(upvalue = fiber->open_upvalues; upvalue != nullptr; upvalue = upvalue->next)
                        {
                            this->markObject((LitObject*)upvalue);
                        }
                        this->markValue(fiber->error);
                        this->markObject((LitObject*)fiber->module);
                        this->markObject((LitObject*)fiber->parent);
                    }
                    break;
                case LitObject::Type::Module:
                    {
                        module = (LitModule*)obj;
                        this->markValue(module->return_value);
                        this->markObject((LitObject*)module->name);
                        this->markObject((LitObject*)module->main_function);
                        this->markObject((LitObject*)module->main_fiber);
                        this->markObject((LitObject*)module->private_names);
                        for(i = 0; i < module->private_count; i++)
                        {
                            this->markValue(module->privates[i]);
                        }
                    }
                    break;
                case LitObject::Type::Closure:
                    {
                        closure = (LitClosure*)obj;
                        this->markObject((LitObject*)closure->function);
                        // Check for nullptr is needed for a really specific gc-case
                        if(closure->upvalues != nullptr)
                        {
                            for(i = 0; i < closure->upvalue_count; i++)
                            {
                                this->markObject((LitObject*)closure->upvalues[i]);
                            }
                        }
                    }
                    break;
                case LitObject::Type::Upvalue:
                    {
                        this->markValue(((LitUpvalue*)obj)->closed);
                    }
                    break;
                case LitObject::Type::Class:
                    {
                        klass = (LitClass*)obj;
                        this->markObject((LitObject*)klass->name);
                        this->markObject((LitObject*)klass->super);
                        klass->methods.markForGC(this);
                        klass->static_fields.markForGC(this);
                    }
                    break;
                case LitObject::Type::Instance:
                    {
                        LitInstance* instance = (LitInstance*)obj;
                        this->markObject((LitObject*)instance->klass);
                        instance->fields.markForGC(this);
                    }
                    break;
                case LitObject::Type::BoundMethod:
                    {
                        bound_method = (LitBoundMethod*)obj;
                        this->markValue(bound_method->receiver);
                        this->markValue(bound_method->method);
                    }
                    break;
                case LitObject::Type::Array:
                    {
                        this->markArray(&((LitArray*)obj)->values);
                    }
                    break;
                case LitObject::Type::Map:
                    {
                        ((LitMap*)obj)->values.markForGC(this);
                    }
                    break;
                case LitObject::Type::Field:
                    {
                        field = (LitField*)obj;
                        this->markObject((LitObject*)field->getter);
                        this->markObject((LitObject*)field->setter);
                    }
                    break;
                case LitObject::Type::Reference:
                    {
                        this->markValue(*((LitReference*)obj)->slot);
                    }
                    break;
                default:
                    {
                        UNREACHABLE
                    }
                    break;
            }
        }

        void traceReferences()
        {
            LitObject* obj;
            while(this->gray_count > 0)
            {
                obj = this->gray_stack[--this->gray_count];
                this->blackenObject(obj);
            }
        }

        void sweep()
        {
            LitObject* unreached;
            LitObject* previous;
            LitObject* obj;
            previous = nullptr;
            obj = this->objects;
            while(obj != nullptr)
            {
                if(obj->marked)
                {
                    obj->marked = false;
                    previous = obj;
                    obj = obj->next;
                }
                else
                {
                    unreached = obj;
                    obj = obj->next;
                    if(previous != nullptr)
                    {
                        previous->next = obj;
                    }
                    else
                    {
                        this->objects = obj;
                    }
                    LitObject::releaseObject(this->state, unreached);
                }
            }
        }

        uint64_t collectGarbage()
        {
            clock_t t;
            uint64_t before;
            uint64_t collected;
            (void)t;
            if(!this->state->allow_gc)
            {
                return 0;
            }

            this->state->allow_gc = false;
            before = this->state->bytes_allocated;

        #ifdef LIT_LOG_GC
            printf("-- gc begin\n");
            t = clock();
        #endif

            markRoots();
            this->traceReferences();
            this->strings.removeWhite();
            this->sweep();
            this->state->next_gc = this->state->bytes_allocated * LIT_GC_HEAP_GROW_FACTOR;
            this->state->allow_gc = true;
            collected = before - this->state->bytes_allocated;

        #ifdef LIT_LOG_GC
            printf("-- gc end. Collected %imb in %gms\n", ((int)((collected / 1024.0 + 0.5) / 10)) * 10,
                   (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
        #endif
            return collected;
        }
};

struct LitInterpretResult
{
    /* the result of this interpret/call attempt */
    LitInterpretResultType type;
    /* the value returned from this interpret/call attempt */
    LitValue result;
};

void LitState::setVMGlobal(LitString* name, LitValue val)
{
    this->vm->globals->values.set(name, val);
}

bool LitState::getVMGlobal(LitString* name, LitValue* dest)
{
    return this->vm->globals->values.get(name, dest);
}


/**
* utility functions
*/
void util_custom_quick_sort(LitVM *vm, LitValue *l, int length, LitValue callee);
bool util_is_fiber_done(LitFiber *fiber);
void util_run_fiber(LitVM *vm, LitFiber *fiber, LitValue *argv, size_t argc, bool catcher);
void util_basic_quick_sort(LitState *state, LitValue *clist, int length);
bool util_interpret(LitVM *vm, LitModule *module);
bool util_test_file_exists(const char *filename);
bool util_attempt_to_require(LitVM *vm, LitValue *argv, size_t argc, const char *path, bool ignore_previous, bool folders);
bool util_attempt_to_require_combined(LitVM *vm, LitValue *argv, size_t argc, const char *a, const char *b, bool ignore_previous);
LitValue util_invalid_constructor(LitVM *vm, LitValue instance, size_t argc, LitValue *argv);


/**
* array functions 
*/






/**
* state functions
*/
char* lit_patch_file_name(char* file_name);

/* call a function in an instance */
LitInterpretResult lit_instance_call_method(LitState* state, LitValue callee, LitString* mthname, LitValue* argv, size_t argc);
LitValue lit_instance_get_method(LitState* state, LitValue callee, LitString* mthname);

/* print a value to LitWriter */
void lit_print_value(LitState* state, LitWriter* wr, LitValue value);

/* returns the static string name of this type. does *not* represent class name, et al. just the LitValueType name! */
const char* lit_get_value_type(LitValue value);


int lit_closest_power_of_two(int n);


bool lit_is_callable_function(LitValue value);



LitValue lit_get_function_name(LitVM* vm, LitValue instance);
LitUpvalue* lit_create_upvalue(LitState* state, LitValue* slot);
LitClosure* lit_create_closure(LitState* state, LitFunction* function);

LitModule* lit_create_module(LitState* state, LitString* name);
LitFiber* lit_create_fiber(LitState* state, LitModule* module, LitFunction* function);
void lit_ensure_fiber_stack(LitState* state, LitFiber* fiber, size_t needed);
LitBoundMethod* lit_create_bound_method(LitState* state, LitValue receiver, LitValue method);
LitUserdata* lit_create_userdata(LitState* state, size_t size, bool ispointeronly);
LitRange* lit_create_range(LitState* state, double from, double to);
LitReference* lit_create_reference(LitState* state, LitValue* slot);


/*
 * Please, do not provide a const string source to the compiler, because it will
 * get modified, if it has any macros in it!
 */
LitModule* lit_compile_module(LitState* state, LitString* module_name, char* code);
LitModule* lit_get_module(LitState* state, const char* name);

LitInterpretResult lit_internal_interpret(LitState* state, LitString* module_name, char* code);
LitInterpretResult lit_interpret(LitState* state, const char* module_name, char* code);
LitInterpretResult lit_interpret_file(LitState* state, const char* file);
LitInterpretResult lit_dump_file(LitState* state, const char* file);
bool lit_compile_and_save_files(LitState* state, char* files[], size_t num_files, const char* output_file);

void lit_printf(LitState* state, const char* message, ...);
void lit_enable_compilation_time_measurement();

void lit_trace_vm_stack(LitVM* vm, LitWriter* wr);

static inline void lit_push(LitVM* vm, LitValue value)
{
    *(vm->fiber->stack_top) = value;
    vm->fiber->stack_top++;

}

static inline LitValue lit_pop(LitVM* vm)
{
    LitValue rt;
    rt = *(vm->fiber->stack_top);
    vm->fiber->stack_top--;
    return rt;
}

LitInterpretResult lit_interpret_module(LitState* state, LitModule* module);
LitInterpretResult lit_interpret_fiber(LitState* state, LitFiber* fiber);
bool lit_handle_runtime_error(LitVM* vm, LitString* error_string);
bool lit_vruntime_error(LitVM* vm, const char* format, va_list args);
bool lit_runtime_error(LitVM* vm, const char* format, ...);
bool lit_runtime_error_exiting(LitVM* vm, const char* format, ...);

void lit_native_exit_jump();
bool lit_set_native_exit_jump();


LitInterpretResult lit_call_function(LitState* state, LitFunction* callee, LitValue* arguments, uint8_t argument_count, bool ignfiber);
LitInterpretResult lit_call_method(LitState* state, LitValue instance, LitValue callee, LitValue* arguments, uint8_t argument_count, bool ignfiber);
LitInterpretResult lit_call(LitState* state, LitValue callee, LitValue* arguments, uint8_t argument_count, bool ignfiber);
LitInterpretResult lit_find_and_call_method(LitState* state, LitValue callee, LitString* method_name, LitValue* arguments, uint8_t argument_count, bool ignfiber);

LitString* lit_to_string(LitState* state, LitValue object);
LitValue lit_call_new(LitVM* vm, const char* name, LitValue* args, size_t arg_count, bool ignfiber);



double lit_check_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);
double lit_get_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, double def);

bool lit_check_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);
bool lit_get_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, bool def);

const char* lit_check_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);
const char* lit_get_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, const char* def);

LitString* lit_check_object_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);
LitInstance* lit_check_instance(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);
LitValue* lit_check_reference(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id);

void lit_ensure_bool(LitVM* vm, LitValue value, const char* error);
void lit_ensure_string(LitVM* vm, LitValue value, const char* error);
void lit_ensure_number(LitVM* vm, LitValue value, const char* error);
void lit_ensure_object_type(LitVM* vm, LitValue value, LitObject::Type type, const char* error);


void lit_disassemble_module(LitState* state, LitModule* module, const char* source);
void lit_disassemble_chunk(LitChunk* chunk, const char* name, const char* source);
size_t lit_disassemble_instruction(LitState* state, LitChunk* chunk, size_t offset, const char* source);

void lit_trace_frame(LitFiber* fiber, LitWriter* wr);



bool lit_parse(LitParser* parser, const char* file_name, const char* source, PCGenericArray<ASTStatement*>& statements);
char* lit_read_file(const char* path);
bool lit_file_exists(const char* path);
bool lit_dir_exists(const char* path);

size_t lit_write_uint8_t(FILE* file, uint8_t byte);
size_t lit_write_uint16_t(FILE* file, uint16_t byte);
size_t lit_write_uint32_t(FILE* file, uint32_t byte);
size_t lit_write_double(FILE* file, double byte);
size_t lit_write_string(FILE* file, LitString* string);

uint8_t lit_read_uint8_t(FILE* file);
uint16_t lit_read_uint16_t(FILE* file);
uint32_t lit_read_uint32_t(FILE* file);
double lit_read_double(FILE* file);
LitString* lit_read_string(LitState* state, FILE* file);

void lit_init_emulated_file(LitEmulatedFile* file, const char* source);

uint8_t lit_read_euint8_t(LitEmulatedFile* file);
uint16_t lit_read_euint16_t(LitEmulatedFile* file);
uint32_t lit_read_euint32_t(LitEmulatedFile* file);
double lit_read_edouble(LitEmulatedFile* file);
LitString* lit_read_estring(LitState* state, LitEmulatedFile* file);

void lit_save_module(LitModule* module, FILE* file);
LitModule* lit_load_module(LitState* state, const char* input);
bool lit_generate_source_file(const char* file, const char* output);
void lit_build_native_runner(const char* bytecode_file);

void lit_open_libraries(LitState* state);
void lit_open_core_library(LitState* state);

void lit_open_math_library(LitState* state);

void lit_open_file_library(LitState* state);

void lit_open_gc_library(LitState* state);




static inline bool lit_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static inline bool lit_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


void lit_init_scanner(LitState* state, LitScanner* scanner, const char* file_name, const char* source);
LitToken lit_scan_token(LitScanner* scanner);
void lit_init_optimizer(LitState* state, LitOptimizer* optimizer);
void lit_optimize(LitOptimizer* optimizer, PCGenericArray<ASTStatement*>* statements);
const char* lit_get_optimization_level_description(LitOptimizationLevel level);

bool lit_is_optimization_enabled(LitOptimization optimization);
void lit_set_optimization_enabled(LitOptimization optimization, bool enabled);
void lit_set_all_optimization_enabled(bool enabled);
void lit_set_optimization_level(LitOptimizationLevel level);

const char* lit_get_optimization_name(LitOptimization optimization);
const char* lit_get_optimization_description(LitOptimization optimization);
void lit_add_definition(LitState* state, const char* name);

bool lit_preprocess(LitPreprocessor* preprocessor, char* source);

void lit_free_expression(LitState* state, ASTExpression* expression);
ASTExprBinary*
lit_create_binary_expression(LitState* state, size_t line, ASTExpression* left, ASTExpression* right, LitTokenType op);
ASTExprUnary* lit_create_unary_expression(LitState* state, size_t line, ASTExpression* right, LitTokenType op);
ASTExprVar* lit_create_var_expression(LitState* state, size_t line, const char* name, size_t length);
ASTExprAssign* lit_create_assign_expression(LitState* state, size_t line, ASTExpression* to, ASTExpression* value);
ASTExprCall* lit_create_call_expression(LitState* state, size_t line, ASTExpression* callee);
ASTExprIndexGet*
lit_create_get_expression(LitState* state, size_t line, ASTExpression* where, const char* name, size_t length, bool questionable, bool ignore_result);
ASTExprIndexSet*
lit_create_set_expression(LitState* state, size_t line, ASTExpression* where, const char* name, size_t length, ASTExpression* value);
ASTExprLambda* lit_create_lambda_expression(LitState* state, size_t line);
ASTExprArray* lit_create_array_expression(LitState* state, size_t line);
ASTExprObject* lit_create_object_expression(LitState* state, size_t line);
ASTExprSubscript* lit_create_subscript_expression(LitState* state, size_t line, ASTExpression* array, ASTExpression* index);
ASTExprThis* lit_create_this_expression(LitState* state, size_t line);

ASTExprSuper* lit_create_super_expression(LitState* state, size_t line, LitString* method, bool ignore_result);
ASTExprRange* lit_create_range_expression(LitState* state, size_t line, ASTExpression* from, ASTExpression* to);
ASTExprIfClause*
lit_create_if_experssion(LitState* state, size_t line, ASTExpression* condition, ASTExpression* if_branch, ASTExpression* else_branch);

ASTExprInterpolation* lit_create_interpolation_expression(LitState* state, size_t line);

void lit_free_statement(LitState* state, ASTStatement* statement);
ASTExprStatement* lit_create_expression_statement(LitState* state, size_t line, ASTExpression* expression);
ASTStmtBlock* lit_create_block_statement(LitState* state, size_t line);
ASTStmtVar* lit_create_var_statement(LitState* state, size_t line, const char* name, size_t length, ASTExpression* exprinit, bool constant);

ASTStmtIfClause* lit_create_if_statement(LitState* state,
                                        size_t line,
                                        ASTExpression* condition,
                                        ASTStatement* if_branch,
                                        ASTStatement* else_branch,
                                        PCGenericArray<ASTExpression*>* elseif_conditions,
                                        PCGenericArray<ASTStatement*>* elseif_branches);

ASTStmtWhileLoop* lit_create_while_statement(LitState* state, size_t line, ASTExpression* condition, ASTStatement* body);

ASTStmtForLoop* lit_create_for_statement(LitState* state,
                                          size_t line,
                                          ASTExpression* exprinit,
                                          ASTStatement* var,
                                          ASTExpression* condition,
                                          ASTExpression* increment,
                                          ASTStatement* body,
                                          bool c_style);
ASTStmtContinue* lit_create_continue_statement(LitState* state, size_t line);
ASTStmtBreak* lit_create_break_statement(LitState* state, size_t line);
ASTStmtFunction* lit_create_function_statement(LitState* state, size_t line, const char* name, size_t length);
ASTStmtReturn* lit_create_return_statement(LitState* state, size_t line, ASTExpression* expression);
ASTStmtMethod* lit_create_method_statement(LitState* state, size_t line, LitString* name, bool is_static);
ASTStmtClass* lit_create_class_statement(LitState* state, size_t line, LitString* name, LitString* parent);
ASTStmtField*
lit_create_field_statement(LitState* state, size_t line, LitString* name, ASTStatement* getter, ASTStatement* setter, bool is_static);

void lit_free_allocated_expressions(LitState* state, PCGenericArray<ASTExpression*>* expressions);

void lit_free_allocated_statements(LitState* state, PCGenericArray<ASTStatement*>* statements);


static const int8_t stack_effects[] = {
#define OPCODE(_, effect) effect,
OPCODE(POP, -1)
OPCODE(RETURN, 0)
OPCODE(CONSTANT, 1)
OPCODE(CONSTANT_LONG, 1)
OPCODE(TRUE, 1)
OPCODE(FALSE, 1)
OPCODE(NULL, 1)
OPCODE(ARRAY, 1)
OPCODE(OBJECT, 1)
OPCODE(RANGE, -1)
OPCODE(NEGATE, 0)
OPCODE(NOT, 0)

OPCODE(ADD, -1)
OPCODE(SUBTRACT, -1)
OPCODE(MULTIPLY, -1)
OPCODE(POWER, -1)
OPCODE(DIVIDE, -1)
OPCODE(FLOOR_DIVIDE, -1)
OPCODE(MOD, -1)
OPCODE(BAND, -1)
OPCODE(BOR, -1)
OPCODE(BXOR, -1)
OPCODE(LSHIFT, -1)
OPCODE(RSHIFT, -1)
OPCODE(BNOT, 0)

OPCODE(EQUAL, -1)
OPCODE(GREATER, -1)
OPCODE(GREATER_EQUAL, -1)
OPCODE(LESS, -1)
OPCODE(LESS_EQUAL, -1)

OPCODE(SET_GLOBAL, 0)
OPCODE(GET_GLOBAL, 1)

OPCODE(SET_LOCAL, 0)
OPCODE(GET_LOCAL, 1)
OPCODE(SET_LOCAL_LONG, 0)
OPCODE(GET_LOCAL_LONG, 1)

OPCODE(SET_PRIVATE, 0)
OPCODE(GET_PRIVATE, 1)
OPCODE(SET_PRIVATE_LONG, 0)
OPCODE(GET_PRIVATE_LONG, 1)

OPCODE(SET_UPVALUE, 0)
OPCODE(GET_UPVALUE, 1)

OPCODE(JUMP_IF_FALSE, -1)
OPCODE(JUMP_IF_NULL, 0)
OPCODE(JUMP_IF_NULL_POPPING, -1)
OPCODE(JUMP, 0)
OPCODE(JUMP_BACK, 0)
OPCODE(AND, -1)
OPCODE(OR, -1)
OPCODE(NULL_OR, -1)

OPCODE(CLOSURE, 1)
OPCODE(CLOSE_UPVALUE, -1)

OPCODE(CLASS, 1)
OPCODE(GET_FIELD, -1)
OPCODE(SET_FIELD, -2)

// [array] [index] -> [value]
OPCODE(SUBSCRIPT_GET, -1)
// [array] [index] [value] -> [value]
OPCODE(SUBSCRIPT_SET, -2)
// [array] [value] -> [array]
OPCODE(PUSH_ARRAY_ELEMENT, -1)
// [map] [slot] [value] -> [map]
OPCODE(PUSH_OBJECT_FIELD, -2)

// [class] [method] -> [class]
OPCODE(METHOD, -1)
// [class] [method] -> [class]
OPCODE(STATIC_FIELD, -1)
OPCODE(DEFINE_FIELD, -1)
OPCODE(INHERIT, 0)
// [instance] [class] -> [bool]
OPCODE(IS, -1)
OPCODE(GET_SUPER_METHOD, 0)

// Varying stack effect
OPCODE(CALL, 0)
OPCODE(INVOKE, 0)
OPCODE(INVOKE_SUPER, 0)
OPCODE(INVOKE_IGNORING, 0)
OPCODE(INVOKE_SUPER_IGNORING, 0)
OPCODE(POP_LOCALS, 0)
OPCODE(VARARG, 0)

OPCODE(REFERENCE_GLOBAL, 1)
OPCODE(REFERENCE_PRIVATE, 0)
OPCODE(REFERENCE_LOCAL, 1)
OPCODE(REFERENCE_UPVALUE, 1)
OPCODE(REFERENCE_FIELD, -1)

OPCODE(SET_REFERENCE, -1)
#undef OPCODE
};

static jmp_buf prs_jmpbuffer;
static ASTParseRule rules[LITTOK_EOF + 1];


static LitTokenType operators[]=
{
    LITTOK_PLUS, LITTOK_MINUS, LITTOK_STAR, LITTOK_PERCENT, LITTOK_SLASH,
    LITTOK_SHARP, LITTOK_BANG, LITTOK_LESS, LITTOK_LESS_EQUAL, LITTOK_GREATER,
    LITTOK_GREATER_EQUAL, LITTOK_EQUAL_EQUAL, LITTOK_LEFT_BRACKET, LITTOK_EOF
};


static bool did_setup_rules;
static void setup_rules();
static void sync(LitParser* parser);
// typimpls 


void LitParser::init(LitState* state)
{
    if(!did_setup_rules)
    {
        did_setup_rules = true;
        setup_rules();
    }
    this->state = state;
    this->had_error = false;
    this->panic_mode = false;
}


size_t LitChunk::add_constant(LitValue constant)
{
    for(size_t i = 0; i < this->constants.m_count; i++)
    {
        if(this->constants.m_values[i] == constant)
        {
            return i;
        }
    }

    this->state->pushValueRoot(constant);
    this->constants.push(constant);
    this->state->popRoot();

    return this->constants.m_count - 1;
}

void LitEmitter::error(size_t line, LitError error, ...)
{
    va_list args;
    va_start(args, error);
    this->state->raiseError(COMPILE_ERROR, lit_vformat_error(this->state, line, error, args)->chars);
    va_end(args);
}


void LitEmitter::init_compiler(LitCompiler* compiler, LitFunctionType type)
{
    compiler->locals.init(this->state);

    compiler->type = type;
    compiler->scope_depth = 0;
    compiler->enclosing = (struct LitCompiler*)this->compiler;
    compiler->skip_return = false;
    compiler->function = LitFunction::make(this->state, this->module);
    compiler->loop_depth = 0;

    this->compiler = compiler;
    const char* name = this->state->scanner->file_name;
    if(this->compiler == nullptr)
    {
        compiler->function->name = LitString::copy(this->state, name, strlen(name));
    }

    this->chunk = &compiler->function->chunk;

    if(lit_is_optimization_enabled(LITOPTSTATE_LINE_INFO))
    {
        this->chunk->has_line_info = false;
    }

    if(type == LITFUNC_METHOD || type == LITFUNC_STATIC_METHOD || type == LITFUNC_CONSTRUCTOR)
    {
        compiler->locals.push(LitLocal{ "this", 4, -1, false, false });
    }
    else
    {
        compiler->locals.push(LitLocal{ "", 0, -1, false, false });
    }

    compiler->slots = 1;
    compiler->max_slots = 1;
}

void LitEmitter::emit_op(uint16_t line, LitOpCode op)
{
    LitCompiler* compiler = this->compiler;

    emit_byte(line, (uint8_t)op);
    compiler->slots += stack_effects[(int)op];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

void LitEmitter::emit_ops(uint16_t line, LitOpCode a, LitOpCode b)
{
    LitCompiler* compiler = this->compiler;

    emit_bytes(line, (uint8_t)a, (uint8_t)b);
    compiler->slots += stack_effects[(int)a] + stack_effects[(int)b];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

void LitEmitter::emit_arged_op(uint16_t line, LitOpCode op, uint8_t arg)
{
    LitCompiler* compiler = this->compiler;

    emit_bytes(line, (uint8_t)op, arg);
    compiler->slots += stack_effects[(int)op];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}


LitModule* LitEmitter::run_emitter(PCGenericArray<ASTStatement*>& statements, LitString* module_name)
{
    size_t i;
    size_t total;
    size_t old_privates_count;
    bool isnew;
    LitState* state;        
    LitValue module_value;
    LitModule* module;
    PCGenericArray<LitPrivate>* privates;
    LitCompiler compiler;
    ASTStatement* stmt;
    this->last_line = 1;
    this->emit_reference = 0;
    state = this->state;
    isnew = false;
    if(this->state->vm->modules->values.get(module_name, &module_value))
    {
        module = LitObject::as<LitModule>(module_value);
    }
    else
    {
        module = lit_create_module(this->state, module_name);
        isnew = true;
    }
    this->module = module;
    old_privates_count = module->private_count;
    if(old_privates_count > 0)
    {
        privates = &this->privates;
        privates->m_count = old_privates_count - 1;
        privates->push(LitPrivate{ true, false });
        for(i = 0; i < old_privates_count; i++)
        {
            privates->m_values[i].initialized = true;
        }
    }
    init_compiler(&compiler, LITFUNC_SCRIPT);
    this->chunk = &compiler.function->chunk;
    this->resolve_statements(statements);
    for(i = 0; i < statements.m_count; i++)
    {
        stmt = statements.m_values[i];
        if(stmt != nullptr)
        {
            if(emit_statement(stmt))
            {
                break;
            }
        }
    }
    end_scope(this->last_line);
    module->main_function = end_compiler(module_name);
    if(isnew)
    {
        total = this->privates.m_count;
        module->privates = LIT_ALLOCATE(this->state, LitValue, total);
        for(i = 0; i < total; i++)
        {
            module->privates[i] = LitObject::NullVal;
        }
    }
    else
    {
        module->privates = LIT_GROW_ARRAY(this->state, module->privates, LitValue, old_privates_count, module->private_count);
        for(i = old_privates_count; i < module->private_count; i++)
        {
            module->privates[i] = LitObject::NullVal;
        }
    }
    this->privates.release();
    if(lit_is_optimization_enabled(LITOPTSTATE_PRIVATE_NAMES))
    {
        this->module->private_names->values.release();
    }
    if(isnew && !state->had_error)
    {
        state->vm->modules->values.set(module_name, module->asValue());
    }
    module->ran = true;
    return module;
}


void LitState::init(LitVM* vm)
{
    vm->reset(this);
    vm->globals = LitMap::make(this);
    vm->modules = LitMap::make(this);
}

LitState* LitState::make()
{
    LitState* state;
    state = (LitState*)malloc(sizeof(LitState));
    state->classvalue_class = nullptr;
    state->objectvalue_class = nullptr;
    state->numbervalue_class = nullptr;
    state->stringvalue_class = nullptr;
    state->boolvalue_class = nullptr;
    state->functionvalue_class = nullptr;
    state->fibervalue_class = nullptr;
    state->modulevalue_class = nullptr;
    state->arrayvalue_class = nullptr;
    state->mapvalue_class = nullptr;
    state->rangevalue_class = nullptr;
    state->bytes_allocated = 0;
    state->next_gc = 256 * 1024;
    state->allow_gc = false;
    state->error_fn = default_error;
    state->print_fn = default_printf;
    state->had_error = false;
    state->roots = nullptr;
    state->root_count = 0;
    state->root_capacity = 0;
    state->last_module = nullptr;
    state->debugwriter.initFile(state, stdout, true);
    state->preprocessor = (LitPreprocessor*)malloc(sizeof(LitPreprocessor));
    state->preprocessor->init(state);
    state->scanner = (LitScanner*)malloc(sizeof(LitScanner));
    state->parser = (LitParser*)malloc(sizeof(LitParser));
    ((LitParser*)state->parser)->init(state);
    state->emitter = (LitEmitter*)malloc(sizeof(LitEmitter));
    state->emitter->init(state);
    state->optimizer = (LitOptimizer*)malloc(sizeof(LitOptimizer));
    state->optimizer->init(state);
    state->vm = (LitVM*)malloc(sizeof(LitVM));
    state->init(state->vm);
    init_api(state);
    lit_open_core_library(state);
    return state;
}

int64_t LitState::release()
{
    int64_t amount;
    if(this->roots != nullptr)
    {
        free(this->roots);
        this->roots = nullptr;
    }
    this->releaseAPI();
    this->preprocessor->release();
    free(this->preprocessor);
    free(this->scanner);
    this->parser->release();
    free(this->parser);
    this->emitter->release();
    free(this->emitter);
    free(this->optimizer);
    this->vm->release();
    free(this->vm);
    amount = this->bytes_allocated;
    free(this);
    return amount;
}

void LitState::releaseObjects(LitObject* objects)
{
    LitObject* obj;
    obj = objects;
    while(obj != nullptr)
    {
        LitObject* next = obj->next;
        LitObject::releaseObject(this, obj);
        obj = next;
    }
    free(this->vm->gray_stack);
    this->vm->gray_capacity = 0;
}

void LitVM::markRoots()
{
    size_t i;
    LitState* state;
    state = this->state;
    for(i = 0; i < state->root_count; i++)
    {
        this->markValue(state->roots[i]);
    }
    this->markObject((LitObject*)this->fiber);
    this->markObject((LitObject*)state->classvalue_class);
    this->markObject((LitObject*)state->objectvalue_class);
    this->markObject((LitObject*)state->numbervalue_class);
    this->markObject((LitObject*)state->stringvalue_class);
    this->markObject((LitObject*)state->boolvalue_class);
    this->markObject((LitObject*)state->functionvalue_class);
    this->markObject((LitObject*)state->fibervalue_class);
    this->markObject((LitObject*)state->modulevalue_class);
    this->markObject((LitObject*)state->arrayvalue_class);
    this->markObject((LitObject*)state->mapvalue_class);
    this->markObject((LitObject*)state->rangevalue_class);
    this->markObject((LitObject*)state->api_name);
    this->markObject((LitObject*)state->api_function);
    this->markObject((LitObject*)state->api_fiber);
    state->preprocessor->defined.markForGC(this);
    this->modules->values.markForGC(this);
    this->globals->values.markForGC(this);
}

LitObject* LitObject::make(LitState* state, size_t size, LitObject::Type type)
{
    LitObject* obj;
    obj = (LitObject*)lit_reallocate(state, nullptr, 0, size);
    obj->m_state = state;
    obj->type = type;
    obj->marked = false;
    obj->next = state->vm->objects;
    state->vm->objects = obj;
#ifdef LIT_LOG_ALLOCATION
    printf("%p allocate %ld for %s\n", (void*)obj, size, lit_get_value_type(type));
#endif
    return obj;
}

void LitObject::releaseObject(LitState* state, LitObject* obj)
{
    LitString* string;
    LitFunction* function;
    LitFiber* fiber;
    LitModule* module;
    LitClosure* closure;
#ifdef LIT_LOG_ALLOCATION
    printf("(");
    lit_print_value(obj->asValue());
    printf(") %p free %s\n", (void*)obj, lit_get_value_type(obj->type));
#endif

    switch(obj->type)
    {
        case LitObject::Type::String:
            {
                string = (LitString*)obj;
                //LIT_FREE_ARRAY(state, char, string->chars, string->length + 1);
                sdsfree(string->chars);
                string->chars = nullptr;
                LIT_FREE(state, LitString, obj);
            }
            break;

        case LitObject::Type::Function:
            {
                function = (LitFunction*)obj;
                function->chunk.release();
                LIT_FREE(state, LitFunction, obj);
            }
            break;
        case LitObject::Type::NativeFunction:
            {
                LIT_FREE(state, LitNativeFunction, obj);
            }
            break;
        case LitObject::Type::NativePrimitive:
            {
                LIT_FREE(state, LitNativePrimFunction, obj);
            }
            break;
        case LitObject::Type::NativeMethod:
            {
                LIT_FREE(state, LitNativeMethod, obj);
            }
            break;
        case LitObject::Type::PrimitiveMethod:
            {
                LIT_FREE(state, LitPrimitiveMethod, obj);
            }
            break;
        case LitObject::Type::Fiber:
            {
                fiber = (LitFiber*)obj;
                LIT_FREE_ARRAY(state, LitCallFrame, fiber->frames, fiber->frame_capacity);
                LIT_FREE_ARRAY(state, LitValue, fiber->stack, fiber->stack_capacity);
                LIT_FREE(state, LitFiber, obj);
            }
            break;
        case LitObject::Type::Module:
            {
                module = (LitModule*)obj;
                LIT_FREE_ARRAY(state, LitValue, module->privates, module->private_count);
                LIT_FREE(state, LitModule, obj);
            }
            break;
        case LitObject::Type::Closure:
            {
                closure = (LitClosure*)obj;
                LIT_FREE_ARRAY(state, LitUpvalue*, closure->upvalues, closure->upvalue_count);
                LIT_FREE(state, LitClosure, obj);
            }
            break;
        case LitObject::Type::Upvalue:
            {
                LIT_FREE(state, LitUpvalue, obj);
            }
            break;
        case LitObject::Type::Class:
            {
                LitClass* klass = (LitClass*)obj;
                klass->methods.release();
                klass->static_fields.release();
                LIT_FREE(state, LitClass, obj);
            }
            break;

        case LitObject::Type::Instance:
            {
                ((LitInstance*)obj)->fields.release();
                LIT_FREE(state, LitInstance, obj);
            }
            break;
        case LitObject::Type::BoundMethod:
            {
                LIT_FREE(state, LitBoundMethod, obj);
            }
            break;
        case LitObject::Type::Array:
            {
                ((LitArray*)obj)->values.release();
                LIT_FREE(state, LitArray, obj);
            }
            break;
        case LitObject::Type::Map:
            {
                ((LitMap*)obj)->values.release();
                LIT_FREE(state, LitMap, obj);
            }
            break;
        case LitObject::Type::Userdata:
            {
                LitUserdata* data = (LitUserdata*)obj;
                if(data->cleanup_fn != nullptr)
                {
                    data->cleanup_fn(state, data, false);
                }
                if(data->size > 0)
                {
                    if(data->canfree)
                    {
                        lit_reallocate(state, data->data, data->size, 0);
                    }
                }
                LIT_FREE(state, LitUserdata, data);
                //free(data);
            }
            break;
        case LitObject::Type::Range:
            {
                LIT_FREE(state, LitRange, obj);
            }
            break;
        case LitObject::Type::Field:
            {
                LIT_FREE(state, LitField, obj);
            }
            break;
        case LitObject::Type::Reference:
            {
                LIT_FREE(state, LitReference, obj);
            }
            break;
        default:
            {
                UNREACHABLE
            }
            break;
    }
}


void LitTable::markForGC(LitVM* vm)
{
    size_t i;
    LitTableEntry* entry;
    for(i = 0; i <= this->m_capacity; i++)
    {
        entry = &this->m_values[i];
        if(entry != nullptr)
        {
            vm->markObject((LitObject*)entry->key);
            vm->markValue(entry->value);
        }
    }
}

void LitString::cache(LitState* state, LitString* string)
{
    if(string->length() > 0)
    {
        state->pushRoot((LitObject*)string);
        state->vm->strings.set(string, LitObject::NullVal);
        state->popRoot();
    }
}

LitString* LitString::take(LitState* state, char* chars, size_t length, bool wassds)
{
    bool reuse;
    uint32_t hash;
    hash = LitString::makeHash(chars, length);
    LitString* interned;
    interned = state->vm->strings.find(chars, length, hash);
    if(interned != nullptr)
    {
        if(!wassds)
        {
            LIT_FREE(state, char, chars);
            //sdsfree(chars);
        }
        return interned;
    }
    reuse = wassds;
    return LitString::allocate(state, (char*)chars, length, hash, wassds, reuse);
}

/* copy a string, creating a full newly allocated LitString. */
LitString* LitString::copy(LitState* state, const char* chars, size_t length)
{
    uint32_t hash;
    char* heap_chars;
    LitString* interned;
    hash= LitString::makeHash(chars, length);
    interned = state->vm->strings.find(chars, length, hash);
    if(interned != nullptr)
    {
        return interned;
    }
    /*
    heap_chars = LIT_ALLOCATE(state, char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    */
    heap_chars = sdsnewlen(chars, length);
#ifdef LIT_LOG_ALLOCATION
    printf("Allocated new string '%s'\n", chars);
#endif
    return LitString::allocate(state, heap_chars, length, hash, true, true);
}

LitString* LitString::format(LitState* state, const char* format, ...)
{
    char ch;
    size_t length;
    size_t total_length;
    bool was_allowed;
    const char* c;
    const char* strval;
    va_list arg_list;
    LitValue val;
    LitString* string;
    LitString* result;
    was_allowed = state->allow_gc;
    state->allow_gc = false;
    va_start(arg_list, format);
    total_length = strlen(format);
    va_end(arg_list);
    result = LitString::allocEmpty(state, total_length + 1, false);
    va_start(arg_list, format);
    for(c = format; *c != '\0'; c++)
    {
        switch(*c)
        {
            case '$':
                {
                    strval = va_arg(arg_list, const char*);
                    if(strval != nullptr)
                    {
                        length = strlen(strval);
                        result->chars = sdscatlen(result->chars, strval, length);
                    }
                    else
                    {
                        goto default_ending_copying;
                    }
                }
                break;
            case '@':
                {
                    val = va_arg(arg_list, LitValue);
                    if(LitObject::isString(val))
                    {
                        string = LitObject::as<LitString>(val);
                    }
                    else
                    {
                        //fprintf(stderr, "format: not a string, but a '%s'\n", lit_get_value_type(val));
                        //string = lit_to_string(state, val);
                        goto default_ending_copying;
                    }
                    if(string != nullptr)
                    {
                        length = sdslen(string->chars);
                        if(length > 0)
                        {
                            result->chars = sdscatlen(result->chars, string->chars, length);
                        }
                    }
                    else
                    {
                        goto default_ending_copying;
                    }
                }
                break;
            case '#':
                {
                    string = LitObject::as<LitString>(LitString::stringNumberToString(state, va_arg(arg_list, double)));
                    length = sdslen(string->chars);
                    if(length > 0)
                    {
                        result->chars = sdscatlen(result->chars, string->chars, length);
                    }
                }
                break;
            default:
                {
                    default_ending_copying:
                    ch = *c;
                    result->chars = sdscatlen(result->chars, &ch, 1);
                }
                break;
        }
    }
    va_end(arg_list);
    result->hash = LitString::makeHash(result->chars, result->length());
    LitString::cache(state, result);
    state->allow_gc = was_allowed;
    return result;
}


/**
* impls
*/



static void free_parameters(LitState* state, PCGenericArray<ASTExprFuncParam>* parameters)
{
    for(size_t i = 0; i < parameters->m_count; i++)
    {
        lit_free_expression(state, parameters->m_values[i].default_value);
    }
    parameters->release();
}

void free_expressions(LitState* state, PCGenericArray<ASTExpression*>* expressions)
{
    if(expressions == nullptr)
    {
        return;
    }
    for(size_t i = 0; i < expressions->m_count; i++)
    {
        lit_free_expression(state, expressions->m_values[i]);
    }
    expressions->release();
}

void internal_free_statements(LitState* state, PCGenericArray<ASTStatement*>* statements)
{
    if(statements == nullptr)
    {
        return;
    }

    for(size_t i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }

    statements->release();
}

void lit_free_expression(LitState* state, ASTExpression* expression)
{
    if(expression == nullptr)
    {
        return;
    }

    switch(expression->type)
    {
        case ASTExpression::Type::Literal:
        {
            lit_reallocate(state, expression, sizeof(ASTExprLiteral), 0);
            break;
        }

        case ASTExpression::Type::Binary:
        {
            ASTExprBinary* expr = (ASTExprBinary*)expression;

            if(!expr->ignore_left)
            {
                lit_free_expression(state, expr->left);
            }

            lit_free_expression(state, expr->right);

            lit_reallocate(state, expression, sizeof(ASTExprBinary), 0);
            break;
        }

        case ASTExpression::Type::Unary:
        {
            lit_free_expression(state, ((ASTExprUnary*)expression)->right);
            lit_reallocate(state, expression, sizeof(ASTExprUnary), 0);

            break;
        }

        case ASTExpression::Type::Variable:
        {
            lit_reallocate(state, expression, sizeof(ASTExprVar), 0);
            break;
        }

        case ASTExpression::Type::Assign:
        {
            ASTExprAssign* expr = (ASTExprAssign*)expression;

            lit_free_expression(state, expr->to);
            lit_free_expression(state, expr->value);

            lit_reallocate(state, expression, sizeof(ASTExprAssign), 0);
            break;
        }

        case ASTExpression::Type::Call:
        {
            ASTExprCall* expr = (ASTExprCall*)expression;

            lit_free_expression(state, expr->callee);
            lit_free_expression(state, expr->objexpr);

            free_expressions(state, &expr->args);

            lit_reallocate(state, expression, sizeof(ASTExprCall), 0);
            break;
        }

        case ASTExpression::Type::Get:
        {
            lit_free_expression(state, ((ASTExprIndexGet*)expression)->where);
            lit_reallocate(state, expression, sizeof(ASTExprIndexGet), 0);
            break;
        }

        case ASTExpression::Type::Set:
        {
            ASTExprIndexSet* expr = (ASTExprIndexSet*)expression;

            lit_free_expression(state, expr->where);
            lit_free_expression(state, expr->value);

            lit_reallocate(state, expression, sizeof(ASTExprIndexSet), 0);
            break;
        }

        case ASTExpression::Type::Lambda:
            {
                ASTExprLambda* expr = (ASTExprLambda*)expression;
                free_parameters(state, &expr->parameters);
                lit_free_statement(state, expr->body);
                lit_reallocate(state, expression, sizeof(ASTExprLambda), 0);
            }
            break;
        case ASTExpression::Type::Array:
            {
                free_expressions(state, &((ASTExprArray*)expression)->values);
                lit_reallocate(state, expression, sizeof(ASTExprArray), 0);
            }
            break;
        case ASTExpression::Type::Object:
            {
                ASTExprObject* map = (ASTExprObject*)expression;
                map->keys.release();
                free_expressions(state, &map->values);
                lit_reallocate(state, expression, sizeof(ASTExprObject), 0);
            }
            break;
        case ASTExpression::Type::Subscript:
            {
                ASTExprSubscript* expr = (ASTExprSubscript*)expression;
                lit_free_expression(state, expr->array);
                lit_free_expression(state, expr->index);
                lit_reallocate(state, expression, sizeof(ASTExprSubscript), 0);
            }
            break;
        case ASTExpression::Type::This:
            {
                lit_reallocate(state, expression, sizeof(ASTExprThis), 0);
            }
            break;
        case ASTExpression::Type::Super:
            {
                lit_reallocate(state, expression, sizeof(ASTExprSuper), 0);
            }
            break;
        case ASTExpression::Type::Range:
            {
                ASTExprRange* expr = (ASTExprRange*)expression;
                lit_free_expression(state, expr->from);
                lit_free_expression(state, expr->to);
                lit_reallocate(state, expression, sizeof(ASTExprRange), 0);
            }
            break;
        case ASTExpression::Type::IfClause:
            {
                ASTExprIfClause* expr = (ASTExprIfClause*)expression;
                lit_free_expression(state, expr->condition);
                lit_free_expression(state, expr->if_branch);
                lit_free_expression(state, expr->else_branch);
                lit_reallocate(state, expression, sizeof(ASTExprIfClause), 0);
            }
            break;
        case ASTExpression::Type::Interpolation:
            {
                free_expressions(state, &((ASTExprInterpolation*)expression)->expressions);
                lit_reallocate(state, expression, sizeof(ASTExprInterpolation), 0);
            }
            break;
        case ASTExpression::Type::Reference:
            {
                lit_free_expression(state, ((ASTExprReference*)expression)->to);
                lit_reallocate(state, expression, sizeof(ASTExprReference), 0);
            }
            break;
        default:
            {
                state->raiseError(COMPILE_ERROR, "Unknown expression type %d", (int)expression->type);
            }
            break;
    }
}


ASTExprBinary* lit_create_binary_expression(LitState* state, size_t line, ASTExpression* left, ASTExpression* right, LitTokenType op)
{
    ASTExprBinary* expression;
    expression = (ASTExprBinary*)ASTExpression::make(state, line, sizeof(ASTExprBinary), ASTExpression::Type::Binary);
    expression->left = left;
    expression->right = right;
    expression->op = op;
    expression->ignore_left = false;
    return expression;
}

ASTExprUnary* lit_create_unary_expression(LitState* state, size_t line, ASTExpression* right, LitTokenType op)
{
    ASTExprUnary* expression;
    expression = (ASTExprUnary*)ASTExpression::make(state, line, sizeof(ASTExprUnary), ASTExpression::Type::Unary);
    expression->right = right;
    expression->op = op;
    return expression;
}

ASTExprVar* lit_create_var_expression(LitState* state, size_t line, const char* name, size_t length)
{
    ASTExprVar* expression;
    expression = (ASTExprVar*)ASTExpression::make(state, line, sizeof(ASTExprVar), ASTExpression::Type::Variable);
    expression->name = name;
    expression->length = length;
    return expression;
}

ASTExprAssign* lit_create_assign_expression(LitState* state, size_t line, ASTExpression* to, ASTExpression* value)
{
    ASTExprAssign* expression;
    expression = (ASTExprAssign*)ASTExpression::make(state, line, sizeof(ASTExprAssign), ASTExpression::Type::Assign);
    expression->to = to;
    expression->value = value;
    return expression;
}

ASTExprCall* lit_create_call_expression(LitState* state, size_t line, ASTExpression* callee)
{
    ASTExprCall* expression;
    expression = (ASTExprCall*)ASTExpression::make(state, line, sizeof(ASTExprCall), ASTExpression::Type::Call);
    expression->callee = callee;
    expression->objexpr = nullptr;
    expression->args.init(state);
    return expression;
}

ASTExprIndexGet* lit_create_get_expression(LitState* state, size_t line, ASTExpression* where, const char* name, size_t length, bool questionable, bool ignore_result)
{
    ASTExprIndexGet* expression;
    expression = (ASTExprIndexGet*)ASTExpression::make(state, line, sizeof(ASTExprIndexGet), ASTExpression::Type::Get);
    expression->where = where;
    expression->name = name;
    expression->length = length;
    expression->ignore_emit = false;
    expression->jump = questionable ? 0 : -1;
    expression->ignore_result = ignore_result;
    return expression;
}

ASTExprIndexSet* lit_create_set_expression(LitState* state, size_t line, ASTExpression* where, const char* name, size_t length, ASTExpression* value)
{
    ASTExprIndexSet* expression;
    expression = (ASTExprIndexSet*)ASTExpression::make(state, line, sizeof(ASTExprIndexSet), ASTExpression::Type::Set);
    expression->where = where;
    expression->name = name;
    expression->length = length;
    expression->value = value;
    return expression;
}

ASTExprLambda* lit_create_lambda_expression(LitState* state, size_t line)
{
    ASTExprLambda* expression;
    expression = (ASTExprLambda*)ASTExpression::make(state, line, sizeof(ASTExprLambda), ASTExpression::Type::Lambda);
    expression->body = nullptr;
    expression->parameters.init(state);
    return expression;
}

ASTExprArray* lit_create_array_expression(LitState* state, size_t line)
{
    ASTExprArray* expression;
    expression = (ASTExprArray*)ASTExpression::make(state, line, sizeof(ASTExprArray), ASTExpression::Type::Array);
    expression->values.init(state);
    return expression;
}

ASTExprObject* lit_create_object_expression(LitState* state, size_t line)
{
    ASTExprObject* expression;
    expression = (ASTExprObject*)ASTExpression::make(state, line, sizeof(ASTExprObject), ASTExpression::Type::Object);
    expression->keys.init(state);
    expression->values.init(state);
    return expression;
}

ASTExprSubscript* lit_create_subscript_expression(LitState* state, size_t line, ASTExpression* array, ASTExpression* index)
{
    ASTExprSubscript* expression;
    expression = (ASTExprSubscript*)ASTExpression::make(state, line, sizeof(ASTExprSubscript), ASTExpression::Type::Subscript);
    expression->array = array;
    expression->index = index;
    return expression;
}

ASTExprThis* lit_create_this_expression(LitState* state, size_t line)
{
    return (ASTExprThis*)ASTExpression::make(state, line, sizeof(ASTExprThis), ASTExpression::Type::This);
}

ASTExprSuper* lit_create_super_expression(LitState* state, size_t line, LitString* method, bool ignore_result)
{
    ASTExprSuper* expression;
    expression = (ASTExprSuper*)ASTExpression::make(state, line, sizeof(ASTExprSuper), ASTExpression::Type::Super);
    expression->method = method;
    expression->ignore_emit = false;
    expression->ignore_result = ignore_result;
    return expression;
}

ASTExprRange* lit_create_range_expression(LitState* state, size_t line, ASTExpression* from, ASTExpression* to)
{
    ASTExprRange* expression;
    expression = (ASTExprRange*)ASTExpression::make(state, line, sizeof(ASTExprRange), ASTExpression::Type::Range);
    expression->from = from;
    expression->to = to;
    return expression;
}

ASTExprIfClause* lit_create_if_experssion(LitState* state, size_t line, ASTExpression* condition, ASTExpression* if_branch, ASTExpression* else_branch)
{
    ASTExprIfClause* expression;
    expression = (ASTExprIfClause*)ASTExpression::make(state, line, sizeof(ASTExprIfClause), ASTExpression::Type::IfClause);
    expression->condition = condition;
    expression->if_branch = if_branch;
    expression->else_branch = else_branch;

    return expression;
}

ASTExprInterpolation* lit_create_interpolation_expression(LitState* state, size_t line)
{
    ASTExprInterpolation* expression;
    expression = (ASTExprInterpolation*)ASTExpression::make(state, line, sizeof(ASTExprInterpolation), ASTExpression::Type::Interpolation);
    expression->expressions.init(state);
    return expression;
}


void lit_free_statement(LitState* state, ASTStatement* statement)
{
    if(statement == nullptr)
    {
        return;
    }
    switch(statement->type)
    {
        case ASTStatement::Type::Expression:
            {
                lit_free_expression(state, ((ASTExprStatement*)statement)->expression);
                lit_reallocate(state, statement, sizeof(ASTExprStatement), 0);
            }
            break;
        case ASTStatement::Type::Block:
            {
                internal_free_statements(state, &((ASTStmtBlock*)statement)->statements);
                lit_reallocate(state, statement, sizeof(ASTStmtBlock), 0);
            }
            break;
        case ASTStatement::Type::VarDecl:
            {
                lit_free_expression(state, ((ASTStmtVar*)statement)->valexpr);
                lit_reallocate(state, statement, sizeof(ASTStmtVar), 0);
            }
            break;
        case ASTStatement::Type::IfClause:
            {
                ASTStmtIfClause* stmt = (ASTStmtIfClause*)statement;
                lit_free_expression(state, stmt->condition);
                lit_free_statement(state, stmt->if_branch);
                lit_free_allocated_expressions(state, stmt->elseif_conditions);
                lit_free_allocated_statements(state, stmt->elseif_branches);
                lit_free_statement(state, stmt->else_branch);
                lit_reallocate(state, statement, sizeof(ASTStmtIfClause), 0);
            }
            break;
        case ASTStatement::Type::WhileLoop:
            {
                ASTStmtWhileLoop* stmt = (ASTStmtWhileLoop*)statement;
                lit_free_expression(state, stmt->condition);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(ASTStmtWhileLoop), 0);
            }
            break;
        case ASTStatement::Type::ForLoop:
            {
                ASTStmtForLoop* stmt = (ASTStmtForLoop*)statement;
                lit_free_expression(state, stmt->increment);
                lit_free_expression(state, stmt->condition);
                lit_free_expression(state, stmt->exprinit);

                lit_free_statement(state, stmt->var);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(ASTStmtForLoop), 0);
            }
            break;
        case ASTStatement::Type::ContinueClause:
            {
                lit_reallocate(state, statement, sizeof(ASTStmtContinue), 0);
            }
            break;
        case ASTStatement::Type::BreakClause:
            {
                lit_reallocate(state, statement, sizeof(ASTStmtBreak), 0);
            }
            break;
        case ASTStatement::Type::FunctionDecl:
            {
                ASTStmtFunction* stmt = (ASTStmtFunction*)statement;
                lit_free_statement(state, stmt->body);
                free_parameters(state, &stmt->parameters);
                lit_reallocate(state, statement, sizeof(ASTStmtFunction), 0);
            }
            break;
        case ASTStatement::Type::ReturnClause:
            {
                lit_free_expression(state, ((ASTStmtReturn*)statement)->expression);
                lit_reallocate(state, statement, sizeof(ASTStmtReturn), 0);
            }
            break;
        case ASTStatement::Type::MethodDecl:
            {
                ASTStmtMethod* stmt = (ASTStmtMethod*)statement;
                free_parameters(state, &stmt->parameters);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(ASTStmtMethod), 0);
            }
            break;
        case ASTStatement::Type::ClassDecl:
            {
                internal_free_statements(state, &((ASTStmtClass*)statement)->fields);
                lit_reallocate(state, statement, sizeof(ASTStmtClass), 0);
            }
            break;
        case ASTStatement::Type::FieldDecl:
            {
                ASTStmtField* stmt = (ASTStmtField*)statement;
                lit_free_statement(state, stmt->getter);
                lit_free_statement(state, stmt->setter);
                lit_reallocate(state, statement, sizeof(ASTStmtField), 0);
            }
            break;
        default:
            {
                state->raiseError(COMPILE_ERROR, "Unknown statement type %d", (int)statement->type);
            }
            break;
    }
}


ASTExprStatement* lit_create_expression_statement(LitState* state, size_t line, ASTExpression* expression)
{
    ASTExprStatement* statement;
    statement = (ASTExprStatement*)ASTStatement::make(state, line, sizeof(ASTExprStatement), ASTStatement::Type::Expression);
    statement->expression = expression;
    statement->pop = true;
    return statement;
}

ASTStmtBlock* lit_create_block_statement(LitState* state, size_t line)
{
    ASTStmtBlock* statement;
    statement = (ASTStmtBlock*)ASTStatement::make(state, line, sizeof(ASTStmtBlock), ASTStatement::Type::Block);
    statement->statements.init(state);
    return statement;
}

ASTStmtVar* lit_create_var_statement(LitState* state, size_t line, const char* name, size_t length, ASTExpression* exprinit, bool constant)
{
    ASTStmtVar* statement;
    statement = (ASTStmtVar*)ASTStatement::make(state, line, sizeof(ASTStmtVar), ASTStatement::Type::VarDecl);
    statement->name = name;
    statement->length = length;
    statement->valexpr = exprinit;
    statement->constant = constant;
    return statement;
}

ASTStmtIfClause* lit_create_if_statement(LitState* state,
                                        size_t line,
                                        ASTExpression* condition,
                                        ASTStatement* if_branch,
                                        ASTStatement* else_branch,
                                        PCGenericArray<ASTExpression*>* elseif_conditions,
                                        PCGenericArray<ASTStatement*>* elseif_branches)
{
    ASTStmtIfClause* statement;
    statement = (ASTStmtIfClause*)ASTStatement::make(state, line, sizeof(ASTStmtIfClause), ASTStatement::Type::IfClause);
    statement->condition = condition;
    statement->if_branch = if_branch;
    statement->else_branch = else_branch;
    statement->elseif_conditions = elseif_conditions;
    statement->elseif_branches = elseif_branches;
    return statement;
}

ASTStmtWhileLoop* lit_create_while_statement(LitState* state, size_t line, ASTExpression* condition, ASTStatement* body)
{
    ASTStmtWhileLoop* statement;
    statement = (ASTStmtWhileLoop*)ASTStatement::make(state, line, sizeof(ASTStmtWhileLoop), ASTStatement::Type::WhileLoop);
    statement->condition = condition;
    statement->body = body;
    return statement;
}

ASTStmtForLoop* lit_create_for_statement(LitState* state,
                                          size_t line,
                                          ASTExpression* exprinit,
                                          ASTStatement* var,
                                          ASTExpression* condition,
                                          ASTExpression* increment,
                                          ASTStatement* body,
                                          bool c_style)
{
    ASTStmtForLoop* statement;
    statement = (ASTStmtForLoop*)ASTStatement::make(state, line, sizeof(ASTStmtForLoop), ASTStatement::Type::ForLoop);
    statement->exprinit = exprinit;
    statement->var = var;
    statement->condition = condition;
    statement->increment = increment;
    statement->body = body;
    statement->c_style = c_style;
    return statement;
}

ASTStmtContinue* lit_create_continue_statement(LitState* state, size_t line)
{
    return (ASTStmtContinue*)ASTStatement::make(state, line, sizeof(ASTStmtContinue), ASTStatement::Type::ContinueClause);
}

ASTStmtBreak* lit_create_break_statement(LitState* state, size_t line)
{
    return (ASTStmtBreak*)ASTStatement::make(state, line, sizeof(ASTStmtBreak), ASTStatement::Type::BreakClause);
}

ASTStmtFunction* lit_create_function_statement(LitState* state, size_t line, const char* name, size_t length)
{
    ASTStmtFunction* function;
    function = (ASTStmtFunction*)ASTStatement::make(state, line, sizeof(ASTStmtFunction), ASTStatement::Type::FunctionDecl);
    function->name = name;
    function->length = length;
    function->body = nullptr;
    function->parameters.init(state);
    return function;
}

ASTStmtReturn* lit_create_return_statement(LitState* state, size_t line, ASTExpression* expression)
{
    ASTStmtReturn* statement;
    statement = (ASTStmtReturn*)ASTStatement::make(state, line, sizeof(ASTStmtReturn), ASTStatement::Type::ReturnClause);
    statement->expression = expression;
    return statement;
}

ASTStmtMethod* lit_create_method_statement(LitState* state, size_t line, LitString* name, bool is_static)
{
    ASTStmtMethod* statement;
    statement = (ASTStmtMethod*)ASTStatement::make(state, line, sizeof(ASTStmtMethod), ASTStatement::Type::MethodDecl);
    statement->name = name;
    statement->body = nullptr;
    statement->is_static = is_static;
    statement->parameters.init(state);
    return statement;
}

ASTStmtClass* lit_create_class_statement(LitState* state, size_t line, LitString* name, LitString* parent)
{
    ASTStmtClass* statement;
    statement = (ASTStmtClass*)ASTStatement::make(state, line, sizeof(ASTStmtClass), ASTStatement::Type::ClassDecl);
    statement->name = name;
    statement->parent = parent;
    statement->fields.init(state);
    return statement;
}

ASTStmtField* lit_create_field_statement(LitState* state, size_t line, LitString* name, ASTStatement* getter, ASTStatement* setter, bool is_static)
{
    ASTStmtField* statement;
    statement = (ASTStmtField*)ASTStatement::make(state, line, sizeof(ASTStmtField), ASTStatement::Type::FieldDecl);
    statement->name = name;
    statement->getter = getter;
    statement->setter = setter;
    statement->is_static = is_static;
    return statement;
}


void lit_free_allocated_expressions(LitState* state, PCGenericArray<ASTExpression*>* expressions)
{
    size_t i;
    if(expressions == nullptr)
    {
        return;
    }
    for(i = 0; i < expressions->m_count; i++)
    {
        lit_free_expression(state, expressions->m_values[i]);
    }
    expressions->release();
    lit_reallocate(state, expressions, sizeof(PCGenericArray<ASTExpression*>), 0);
}


void lit_free_allocated_statements(LitState* state, PCGenericArray<ASTStatement*>* statements)
{
    size_t i;
    if(statements == nullptr)
    {
        return;
    }
    for(i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }
    statements->release();
    lit_reallocate(state, statements, sizeof(PCGenericArray<ASTStatement*>), 0);
}










//--------------------------------------------------

#define LIT_DEBUG_OPTIMIZER

#define optc_do_binary_op(op) \
    if(LitObject::isNumber(a) && LitObject::isNumber(b)) \
    { \
        optdbg("translating constant binary expression of '" # op "' to constant value"); \
        return LitObject::toValue(LitObject::toNumber(a) op LitObject::toNumber(b)); \
    } \
    return LitObject::NullVal;

#define optc_do_bitwise_op(op) \
    if(LitObject::isNumber(a) && LitObject::isNumber(b)) \
    { \
        optdbg("translating constant bitwise expression of '" #op "' to constant value"); \
        return LitObject::toValue((int)LitObject::toNumber(a) op(int) LitObject::toNumber(b)); \
    } \
    return LitObject::NullVal;

#define optc_do_fn_op(fn, tokstr) \
    if(LitObject::isNumber(a) && LitObject::isNumber(b)) \
    { \
        optdbg("translating constant expression of '" tokstr "' to constant value via call to '" #fn "'"); \
        return LitObject::toValue(fn(LitObject::toNumber(a), LitObject::toNumber(b))); \
    } \
    return LitObject::NullVal;



static void optimize_expression(LitOptimizer* optimizer, ASTExpression** slot);
static void optimize_expressions(LitOptimizer* optimizer, PCGenericArray<ASTExpression*>* expressions);
static void optimize_statements(LitOptimizer* optimizer, PCGenericArray<ASTStatement*>* statements);
static void optimize_statement(LitOptimizer* optimizer, ASTStatement** slot);

static const char* optimization_level_descriptions[LITOPTLEVEL_TOTAL]
= { "No optimizations (same as -Ono-all)", "Super light optimizations, sepcific to interactive shell.",
    "(default) Recommended optimization level for the development.", "Medium optimization, recommended for the release.",
    "(default for bytecode) Extreme optimization, throws out most of the variable/function names, used for bytecode compilation." };

static const char* optimization_names[LITOPTSTATE_TOTAL]
= { "constant-folding", "literal-folding", "unused-var",    "unreachable-code",
    "empty-body",       "line-info",       "private-names", "c-for" };

static const char* optimization_descriptions[LITOPTSTATE_TOTAL]
= { "Replaces constants in code with their values.",
    "Precalculates literal expressions (3 + 4 is replaced with 7).",
    "Removes user-declared all variables, that were not used.",
    "Removes code that will never be reached.",
    "Removes loops with empty bodies.",
    "Removes line information from chunks to save on space.",
    "Removes names of the private locals from modules (they are indexed by id at runtime).",
    "Replaces for-in loops with c-style for loops where it can." };

static bool optimization_states[LITOPTSTATE_TOTAL];

static bool optimization_states_setup = false;
static bool any_optimization_enabled = false;

static void setup_optimization_states();

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


static void opt_begin_scope(LitOptimizer* optimizer)
{
    optimizer->depth++;
}

static void opt_end_scope(LitOptimizer* optimizer)
{
    bool remove_unused;
    LitVariable* variable;
    PCGenericArray<LitVariable>* variables;
    optimizer->depth--;
    variables = &optimizer->variables;
    remove_unused = lit_is_optimization_enabled(LITOPTSTATE_UNUSED_VAR);
    while(variables->m_count > 0 && variables->m_values[variables->m_count - 1].depth > optimizer->depth)
    {
        if(remove_unused && !variables->m_values[variables->m_count - 1].used)
        {
            variable = &variables->m_values[variables->m_count - 1];
            lit_free_statement(optimizer->state, *variable->declaration);
            *variable->declaration = nullptr;
        }
        variables->m_count--;
    }
}

static LitVariable* add_variable(LitOptimizer* optimizer, const char* name, size_t length, bool constant, ASTStatement** declaration)
{
    optimizer->variables.push(LitVariable{ name, length, optimizer->depth, constant, optimizer->mark_used, LitObject::NullVal, declaration });

    return &optimizer->variables.m_values[optimizer->variables.m_count - 1];
}

static LitVariable* resolve_variable(LitOptimizer* optimizer, const char* name, size_t length)
{
    int i;
    PCGenericArray<LitVariable>* variables;
    LitVariable* variable;
    variables = &optimizer->variables;
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

static bool is_empty(ASTStatement* statement)
{
    return statement == nullptr || (statement->type == ASTStatement::Type::Block && ((ASTStmtBlock*)statement)->statements.m_count == 0);
}

static LitValue evaluate_unary_op(LitValue value, LitTokenType op)
{
    switch(op)
    {
        case LITTOK_MINUS:
            {
                if(LitObject::isNumber(value))
                {
                    optdbg("translating constant unary minus on number to literal value");
                    return LitObject::toValue(-LitObject::toNumber(value));
                }
            }
            break;
        case LITTOK_BANG:
            {
                optdbg("translating constant expression of '=' to literal value");
                return BOOL_VALUE(LitObject::isFalsey(value));
            }
            break;
        case LITTOK_TILDE:
            {
                if(LitObject::isNumber(value))
                {
                    optdbg("translating unary tile (~) on number to literal value");
                    return LitObject::toValue(~((int)LitObject::toNumber(value)));
                }
            }
            break;
        default:
            {
            }
            break;
    }
    return LitObject::NullVal;
}

static LitValue evaluate_binary_op(LitValue a, LitValue b, LitTokenType op)
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
                optc_do_bitwise_op(<<);
            }
            break;
        case LITTOK_GREATER_GREATER:
            {
                optc_do_bitwise_op(>>);
            }
            break;
        case LITTOK_BAR:
            {
                optc_do_bitwise_op(|);
            }
            break;
        case LITTOK_AMPERSAND:
            {
                optc_do_bitwise_op(&);
            }
            break;
        case LITTOK_CARET:
            {
                optc_do_bitwise_op(^);
            }
            break;
        case LITTOK_SHARP:
            {
                if(LitObject::isNumber(a) && LitObject::isNumber(b))
                {
                    return LitObject::toValue(floor(LitObject::toNumber(a) / LitObject::toNumber(b)));
                }
                return LitObject::NullVal;
            }
            break;
        case LITTOK_EQUAL_EQUAL:
            {
                return BOOL_VALUE(a == b);
            }
            break;
        case LITTOK_BANG_EQUAL:
            {
                return BOOL_VALUE(a != b);
            }
            break;
        case LITTOK_IS:
        default:
            {
            }
            break;
    }
    return LitObject::NullVal;
}

static LitValue attempt_to_optimize_binary(LitOptimizer* optimizer, ASTExprBinary* expression, LitValue value, bool left)
{
    double number;
    LitTokenType op;
    op = expression->op;
    ASTExpression* branch;
    branch = left ? expression->left : expression->right;
    if(LitObject::isNumber(value))
    {
        number = LitObject::toNumber(value);
        if(op == LITTOK_STAR)
        {
            if(number == 0)
            {
                optdbg("reducing expression to literal '0'");
                return LitObject::toValue(0);
            }
            else if(number == 1)
            {
                optdbg("reducing expression to literal '1'");
                lit_free_expression(optimizer->state, left ? expression->right : expression->left);
                expression->left = branch;
                expression->right = nullptr;
            }
        }
        else if((op == LITTOK_PLUS || op == LITTOK_MINUS) && number == 0)
        {
            optdbg("reducing expression that would result in '0' to literal '0'");
            lit_free_expression(optimizer->state, left ? expression->right : expression->left);
            expression->left = branch;
            expression->right = nullptr;
        }
        else if(((left && op == LITTOK_SLASH) || op == LITTOK_STAR_STAR) && number == 1)
        {
            optdbg("reducing expression that would result in '1' to literal '1'");
            lit_free_expression(optimizer->state, left ? expression->right : expression->left);
            expression->left = branch;
            expression->right = nullptr;
        }
    }
    return LitObject::NullVal;
}

static LitValue evaluate_expression(LitOptimizer* optimizer, ASTExpression* expression)
{
    ASTExprUnary* uexpr;
    ASTExprBinary* bexpr;
    LitValue a;
    LitValue b;
    LitValue branch;
    if(expression == nullptr)
    {
        return LitObject::NullVal;
    }
    switch(expression->type)
    {
        case ASTExpression::Type::Literal:
            {
                return ((ASTExprLiteral*)expression)->value;
            }
            break;
        case ASTExpression::Type::Unary:
            {
                uexpr = (ASTExprUnary*)expression;
                branch = evaluate_expression(optimizer, uexpr->right);
                if(branch != LitObject::NullVal)
                {
                    return evaluate_unary_op(branch, uexpr->op);
                }
            }
            break;
        case ASTExpression::Type::Binary:
            {
                bexpr = (ASTExprBinary*)expression;
                a = evaluate_expression(optimizer, bexpr->left);
                b = evaluate_expression(optimizer, bexpr->right);
                if(a != LitObject::NullVal && b != LitObject::NullVal)
                {
                    return evaluate_binary_op(a, b, bexpr->op);
                }
                else if(a != LitObject::NullVal)
                {
                    return attempt_to_optimize_binary(optimizer, bexpr, a, false);
                }
                else if(b != LitObject::NullVal)
                {
                    return attempt_to_optimize_binary(optimizer, bexpr, b, true);
                }
            }
            break;
        default:
            {
                return LitObject::NullVal;
            }
            break;
    }
    return LitObject::NullVal;
}

static void optimize_expression(LitOptimizer* optimizer, ASTExpression** slot)
{
    ASTExpression* expression = *slot;

    if(expression == nullptr)
    {
        return;
    }

    LitState* state = optimizer->state;

    switch(expression->type)
    {
        case ASTExpression::Type::Unary:
        case ASTExpression::Type::Binary:
            {
                if(lit_is_optimization_enabled(LITOPTSTATE_LITERAL_FOLDING))
                {
                    LitValue optimized = evaluate_expression(optimizer, expression);
                    if(optimized != LitObject::NullVal)
                    {
                        *slot = (ASTExpression*)ASTExprLiteral::make(state, expression->line, optimized);
                        lit_free_expression(state, expression);
                        break;
                    }
                }
                switch(expression->type)
                {
                    case ASTExpression::Type::Unary:
                        {
                            optimize_expression(optimizer, &((ASTExprUnary*)expression)->right);
                        }
                        break;
                    case ASTExpression::Type::Binary:
                        {
                            ASTExprBinary* expr = (ASTExprBinary*)expression;
                            optimize_expression(optimizer, &expr->left);
                            optimize_expression(optimizer, &expr->right);
                        }
                        break;
                    default:
                        {
                            UNREACHABLE
                        }
                        break;
                }
            }
            break;
        case ASTExpression::Type::Assign:
            {
                ASTExprAssign* expr = (ASTExprAssign*)expression;
                optimize_expression(optimizer, &expr->to);
                optimize_expression(optimizer, &expr->value);
            }
            break;
        case ASTExpression::Type::Call:
            {
                ASTExprCall* expr = (ASTExprCall*)expression;
                optimize_expression(optimizer, &expr->callee);
                optimize_expressions(optimizer, &expr->args);
            }
            break;
        case ASTExpression::Type::Set:
            {
                ASTExprIndexSet* expr = (ASTExprIndexSet*)expression;
                optimize_expression(optimizer, &expr->where);
                optimize_expression(optimizer, &expr->value);
            }
            break;
        case ASTExpression::Type::Get:
            {
                optimize_expression(optimizer, &((ASTExprIndexGet*)expression)->where);
            }
            break;
        case ASTExpression::Type::Lambda:
            {
                opt_begin_scope(optimizer);
                optimize_statement(optimizer, &((ASTExprLambda*)expression)->body);
                opt_end_scope(optimizer);
            }
            break;

        case ASTExpression::Type::Array:
        {
            optimize_expressions(optimizer, &((ASTExprArray*)expression)->values);
            break;
        }

        case ASTExpression::Type::Object:
        {
            optimize_expressions(optimizer, &((ASTExprObject*)expression)->values);
            break;
        }

        case ASTExpression::Type::Subscript:
        {
            ASTExprSubscript* expr = (ASTExprSubscript*)expression;

            optimize_expression(optimizer, &expr->array);
            optimize_expression(optimizer, &expr->index);

            break;
        }

        case ASTExpression::Type::Range:
        {
            ASTExprRange* expr = (ASTExprRange*)expression;

            optimize_expression(optimizer, &expr->from);
            optimize_expression(optimizer, &expr->to);

            break;
        }

        case ASTExpression::Type::IfClause:
        {
            ASTExprIfClause* expr = (ASTExprIfClause*)expression;
            LitValue optimized = evaluate_expression(optimizer, expr->condition);

            if(optimized != LitObject::NullVal)
            {
                if(LitObject::isFalsey(optimized))
                {
                    *slot = expr->else_branch;
                    expr->else_branch = nullptr;// So that it doesn't get freed
                }
                else
                {
                    *slot = expr->if_branch;
                    expr->if_branch = nullptr;// So that it doesn't get freed
                }

                optimize_expression(optimizer, slot);
                lit_free_expression(state, expression);
            }
            else
            {
                optimize_expression(optimizer, &expr->if_branch);
                optimize_expression(optimizer, &expr->else_branch);
            }

            break;
        }

        case ASTExpression::Type::Interpolation:
        {
            optimize_expressions(optimizer, &((ASTExprInterpolation*)expression)->expressions);
            break;
        }

        case ASTExpression::Type::Variable:
        {
            ASTExprVar* expr = (ASTExprVar*)expression;
            LitVariable* variable = resolve_variable(optimizer, expr->name, expr->length);

            if(variable != nullptr)
            {
                variable->used = true;

                // Not checking here for the enable-ness of constant-folding, since if its off
                // the constant_value would be LitObject::NullVal anyway (:thinkaboutit:)
                if(variable->constant && variable->constant_value != LitObject::NullVal)
                {
                    *slot = (ASTExpression*)ASTExprLiteral::make(state, expression->line, variable->constant_value);
                    lit_free_expression(state, expression);
                }
            }

            break;
        }

        case ASTExpression::Type::Reference:
        {
            optimize_expression(optimizer, &((ASTExprReference*)expression)->to);
            break;
        }

        case ASTExpression::Type::Literal:
        case ASTExpression::Type::This:
        case ASTExpression::Type::Super:
        {
            // Nothing, that we can do here
            break;
        }
        case ASTExpression::Type::Unspecified:
            break;
    }
}

static void optimize_expressions(LitOptimizer* optimizer, PCGenericArray<ASTExpression*>* expressions)
{
    for(size_t i = 0; i < expressions->m_count; i++)
    {
        optimize_expression(optimizer, &expressions->m_values[i]);
    }
}

static void optimize_statement(LitOptimizer* optimizer, ASTStatement** slot)
{
    LitState* state;
    ASTStatement* statement;
    statement = *slot;
    if(statement == nullptr)
    {
        return;
    }
    state = optimizer->state;
    switch(statement->type)
    {
        case ASTStatement::Type::Unspecified:
            {
            }
            break;
        case ASTStatement::Type::Expression:
            {
                optimize_expression(optimizer, &((ASTExprStatement*)statement)->expression);
            }
            break;
        case ASTStatement::Type::Block:
            {
                ASTStmtBlock* stmt;
                stmt = (ASTStmtBlock*)statement;
                if(stmt->statements.m_count == 0)
                {
                    lit_free_statement(state, statement);
                    *slot = nullptr;
                    break;
                }
                opt_begin_scope(optimizer);
                optimize_statements(optimizer, &stmt->statements);
                opt_end_scope(optimizer);
                bool found = false;
                for(size_t i = 0; i < stmt->statements.m_count; i++)
                {
                    ASTStatement* step = stmt->statements.m_values[i];
                    if(!is_empty(step))
                    {
                        found = true;
                        if(step->type == ASTStatement::Type::ReturnClause)
                        {
                            // Remove all the statements post return
                            for(size_t j = i + 1; j < stmt->statements.m_count; j++)
                            {
                                step = stmt->statements.m_values[j];
                                if(step != nullptr)
                                {
                                    lit_free_statement(state, step);
                                    stmt->statements.m_values[j] = nullptr;
                                }
                            }
                            stmt->statements.m_count = i + 1;
                            break;
                        }
                    }
                }
                if(!found && lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY))
                {
                    lit_free_statement(optimizer->state, statement);
                    *slot = nullptr;
                }
            }
            break;

        case ASTStatement::Type::IfClause:
        {
            ASTStmtIfClause* stmt = (ASTStmtIfClause*)statement;

            optimize_expression(optimizer, &stmt->condition);
            optimize_statement(optimizer, &stmt->if_branch);

            bool empty = lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY);
            bool dead = lit_is_optimization_enabled(LITOPTSTATE_UNREACHABLE_CODE);

            LitValue optimized = empty ? evaluate_expression(optimizer, stmt->condition) : LitObject::NullVal;

            if((optimized != LitObject::NullVal && LitObject::isFalsey(optimized)) || (dead && is_empty(stmt->if_branch)))
            {
                lit_free_expression(state, stmt->condition);
                stmt->condition = nullptr;

                lit_free_statement(state, stmt->if_branch);
                stmt->if_branch = nullptr;
            }

            if(stmt->elseif_conditions != nullptr)
            {
                optimize_expressions(optimizer, stmt->elseif_conditions);
                optimize_statements(optimizer, stmt->elseif_branches);

                if(dead || empty)
                {
                    for(size_t i = 0; i < stmt->elseif_conditions->m_count; i++)
                    {
                        if(empty && is_empty(stmt->elseif_branches->m_values[i]))
                        {
                            lit_free_expression(state, stmt->elseif_conditions->m_values[i]);
                            stmt->elseif_conditions->m_values[i] = nullptr;

                            lit_free_statement(state, stmt->elseif_branches->m_values[i]);
                            stmt->elseif_branches->m_values[i] = nullptr;

                            continue;
                        }

                        if(dead)
                        {
                            LitValue value = evaluate_expression(optimizer, stmt->elseif_conditions->m_values[i]);

                            if(value != LitObject::NullVal && LitObject::isFalsey(value))
                            {
                                lit_free_expression(state, stmt->elseif_conditions->m_values[i]);
                                stmt->elseif_conditions->m_values[i] = nullptr;

                                lit_free_statement(state, stmt->elseif_branches->m_values[i]);
                                stmt->elseif_branches->m_values[i] = nullptr;
                            }
                        }
                    }
                }
            }

            optimize_statement(optimizer, &stmt->else_branch);
            break;
        }

        case ASTStatement::Type::WhileLoop:
        {
            ASTStmtWhileLoop* stmt = (ASTStmtWhileLoop*)statement;
            optimize_expression(optimizer, &stmt->condition);

            if(lit_is_optimization_enabled(LITOPTSTATE_UNREACHABLE_CODE))
            {
                LitValue optimized = evaluate_expression(optimizer, stmt->condition);

                if(optimized != LitObject::NullVal && LitObject::isFalsey(optimized))
                {
                    lit_free_statement(optimizer->state, statement);
                    *slot = nullptr;
                    break;
                }
            }

            optimize_statement(optimizer, &stmt->body);

            if(lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
            {
                lit_free_statement(optimizer->state, statement);
                *slot = nullptr;
            }

            break;
        }

        case ASTStatement::Type::ForLoop:
            {
                ASTStmtForLoop* stmt = (ASTStmtForLoop*)statement;
                opt_begin_scope(optimizer);
                // This is required, so that optimizer doesn't optimize out our i variable (and such)
                optimizer->mark_used = true;
                optimize_expression(optimizer, &stmt->exprinit);
                optimize_expression(optimizer, &stmt->condition);
                optimize_expression(optimizer, &stmt->increment);
                optimize_statement(optimizer, &stmt->var);
                optimizer->mark_used = false;
                optimize_statement(optimizer, &stmt->body);
                opt_end_scope(optimizer);
                if(lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                {
                    lit_free_statement(optimizer->state, statement);
                    *slot = nullptr;
                    break;
                }
                if(stmt->c_style || !lit_is_optimization_enabled(LITOPTSTATE_C_FOR) || stmt->condition->type != ASTExpression::Type::Range)
                {
                    break;
                }
                ASTExprRange* range = (ASTExprRange*)stmt->condition;
                LitValue from = evaluate_expression(optimizer, range->from);
                LitValue to = evaluate_expression(optimizer, range->to);
                if(!LitObject::isNumber(from) || !LitObject::isNumber(to))
                {
                    break;
                }
                bool reverse = LitObject::toNumber(from) > LitObject::toNumber(to);
                ASTStmtVar* var = (ASTStmtVar*)stmt->var;
                size_t line = range->line;
                // var i = from
                var->valexpr = range->from;
                // i <= to
                stmt->condition = (ASTExpression*)lit_create_binary_expression(
                state, line, (ASTExpression*)lit_create_var_expression(state, line, var->name, var->length), range->to, LITTOK_LESS_EQUAL);
                // i++ (or i--)
                ASTExpression* var_get = (ASTExpression*)lit_create_var_expression(state, line, var->name, var->length);
                ASTExprBinary* assign_value = lit_create_binary_expression(
                state, line, var_get, (ASTExpression*)ASTExprLiteral::make(state, line, LitObject::toValue(1)),
                reverse ? LITTOK_MINUS_MINUS : LITTOK_PLUS);
                assign_value->ignore_left = true;
                ASTExpression* increment
                = (ASTExpression*)lit_create_assign_expression(state, line, var_get, (ASTExpression*)assign_value);
                stmt->increment = (ASTExpression*)increment;
                range->from = nullptr;
                range->to = nullptr;
                stmt->c_style = true;
                lit_free_expression(state, (ASTExpression*)range);
            }
            break;

        case ASTStatement::Type::VarDecl:
            {
                ASTStmtVar* stmt = (ASTStmtVar*)statement;
                LitVariable* variable = add_variable(optimizer, stmt->name, stmt->length, stmt->constant, slot);
                optimize_expression(optimizer, &stmt->valexpr);
                if(stmt->constant && lit_is_optimization_enabled(LITOPTSTATE_CONSTANT_FOLDING))
                {
                    LitValue value = evaluate_expression(optimizer, stmt->valexpr);

                    if(value != LitObject::NullVal)
                    {
                        variable->constant_value = value;
                    }
                }
            }
            break;
        case ASTStatement::Type::FunctionDecl:
            {
                ASTStmtFunction* stmt = (ASTStmtFunction*)statement;
                LitVariable* variable = add_variable(optimizer, stmt->name, stmt->length, false, slot);
                if(stmt->exported)
                {
                    // Otherwise it will get optimized-out with a big chance
                    variable->used = true;
                }
                opt_begin_scope(optimizer);
                optimize_statement(optimizer, &stmt->body);
                opt_end_scope(optimizer);
            }
            break;
        case ASTStatement::Type::ReturnClause:
            {
                optimize_expression(optimizer, &((ASTStmtReturn*)statement)->expression);
            }
            break;
        case ASTStatement::Type::MethodDecl:
            {
                opt_begin_scope(optimizer);
                optimize_statement(optimizer, &((ASTStmtMethod*)statement)->body);
                opt_end_scope(optimizer);
            }
            break;
        case ASTStatement::Type::ClassDecl:
            {
                optimize_statements(optimizer, &((ASTStmtClass*)statement)->fields);
            }
            break;
        case ASTStatement::Type::FieldDecl:
            {
                ASTStmtField* stmt = (ASTStmtField*)statement;
                if(stmt->getter != nullptr)
                {
                    opt_begin_scope(optimizer);
                    optimize_statement(optimizer, &stmt->getter);
                    opt_end_scope(optimizer);
                }
                if(stmt->setter != nullptr)
                {
                    opt_begin_scope(optimizer);
                    optimize_statement(optimizer, &stmt->setter);
                    opt_end_scope(optimizer);
                }
            }
            break;
        // Nothing to optimize there
        case ASTStatement::Type::ContinueClause:
        case ASTStatement::Type::BreakClause:
            {
            }
            break;

    }
}

static void optimize_statements(LitOptimizer* optimizer, PCGenericArray<ASTStatement*>* statements)
{
    size_t i;
    for(i = 0; i < statements->m_count; i++)
    {
        optimize_statement(optimizer, &statements->m_values[i]);
    }
}

void lit_optimize(LitOptimizer* optimizer, PCGenericArray<ASTStatement*>* statements)
{
    return;
    if(!optimization_states_setup)
    {
        setup_optimization_states();
    }
    if(!any_optimization_enabled)
    {
        return;
    }
    opt_begin_scope(optimizer);
    optimize_statements(optimizer, statements);
    opt_end_scope(optimizer);
    optimizer->variables.release();
}

static void setup_optimization_states()
{
    lit_set_optimization_level(LITOPTLEVEL_DEBUG);
}

bool lit_is_optimization_enabled(LitOptimization optimization)
{
    if(!optimization_states_setup)
    {
        setup_optimization_states();
    }
    return optimization_states[(int)optimization];
}

void lit_set_optimization_enabled(LitOptimization optimization, bool enabled)
{
    size_t i;
    if(!optimization_states_setup)
    {
        setup_optimization_states();
    }
    optimization_states[(int)optimization] = enabled;
    if(enabled)
    {
        any_optimization_enabled = true;
    }
    else
    {
        for(i = 0; i < LITOPTSTATE_TOTAL; i++)
        {
            if(optimization_states[i])
            {
                return;
            }
        }
        any_optimization_enabled = false;
    }
}

void lit_set_all_optimization_enabled(bool enabled)
{
    size_t i;
    optimization_states_setup = true;
    any_optimization_enabled = enabled;
    for(i = 0; i < LITOPTSTATE_TOTAL; i++)
    {
        optimization_states[i] = enabled;
    }
}

void lit_set_optimization_level(LitOptimizationLevel level)
{
    switch(level)
    {
        case LITOPTLEVEL_NONE:
            {
                lit_set_all_optimization_enabled(false);
            }
            break;
        case LITOPTLEVEL_REPL:
            {
                lit_set_all_optimization_enabled(true);
                lit_set_optimization_enabled(LITOPTSTATE_UNUSED_VAR, false);
                lit_set_optimization_enabled(LITOPTSTATE_UNREACHABLE_CODE, false);
                lit_set_optimization_enabled(LITOPTSTATE_EMPTY_BODY, false);
                lit_set_optimization_enabled(LITOPTSTATE_LINE_INFO, false);
                lit_set_optimization_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
            }
            break;
        case LITOPTLEVEL_DEBUG:
            {
                lit_set_all_optimization_enabled(true);
                lit_set_optimization_enabled(LITOPTSTATE_UNUSED_VAR, false);
                lit_set_optimization_enabled(LITOPTSTATE_LINE_INFO, false);
                lit_set_optimization_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
            }
            break;
        case LITOPTLEVEL_RELEASE:
            {
                lit_set_all_optimization_enabled(true);
                lit_set_optimization_enabled(LITOPTSTATE_LINE_INFO, false);
            }
            break;
        case LITOPTLEVEL_EXTREME:
            {
                lit_set_all_optimization_enabled(true);
            }
            break;
        case LITOPTLEVEL_TOTAL:
            {
            }
            break;

    }
}

const char* lit_get_optimization_name(LitOptimization optimization)
{
    return optimization_names[(int)optimization];
}

const char* lit_get_optimization_description(LitOptimization optimization)
{
    return optimization_descriptions[(int)optimization];
}

const char* lit_get_optimization_level_description(LitOptimizationLevel level)
{
    return optimization_level_descriptions[(int)level];
}





static ASTStatement *parse_block(LitParser *parser);
static ASTExpression *parse_precedence(LitParser *parser, LitPrecedence precedence, bool err);
static ASTExpression *parse_number(LitParser *parser, bool can_assign);
static ASTExpression *parse_lambda(LitParser *parser, ASTExprLambda *lambda);
static void parse_parameters(LitParser *parser, PCGenericArray<ASTExprFuncParam> *parameters);
static ASTExpression *parse_grouping_or_lambda(LitParser *parser, bool can_assign);
static ASTExpression *parse_call(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_unary(LitParser *parser, bool can_assign);
static ASTExpression *parse_binary(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_and(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_or(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_null_filter(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_compound(LitParser *parser, ASTExpression *prev, bool can_assign);
static ASTExpression *parse_literal(LitParser *parser, bool can_assign);
static ASTExpression *parse_string(LitParser *parser, bool can_assign);
static ASTExpression *parse_interpolation(LitParser *parser, bool can_assign);
static ASTExpression *parse_object(LitParser *parser, bool can_assign);
static ASTExpression *parse_variable_expression_base(LitParser *parser, bool can_assign, bool isnew);
static ASTExpression *parse_variable_expression(LitParser *parser, bool can_assign);
static ASTExpression *parse_new_expression(LitParser *parser, bool can_assign);
static ASTExpression *parse_dot(LitParser *parser, ASTExpression *previous, bool can_assign);
static ASTExpression *parse_range(LitParser *parser, ASTExpression *previous, bool can_assign);
static ASTExpression *parse_ternary_or_question(LitParser *parser, ASTExpression *previous, bool can_assign);
static ASTExpression *parse_array(LitParser *parser, bool can_assign);
static ASTExpression *parse_subscript(LitParser *parser, ASTExpression *previous, bool can_assign);
static ASTExpression *parse_this(LitParser *parser, bool can_assign);
static ASTExpression *parse_super(LitParser *parser, bool can_assign);
static ASTExpression *parse_reference(LitParser *parser, bool can_assign);
static ASTExpression *parse_expression(LitParser *parser);
static ASTStatement *parse_var_declaration(LitParser *parser);
static ASTStatement *parse_if(LitParser *parser);
static ASTStatement *parse_for(LitParser *parser);
static ASTStatement *parse_while(LitParser *parser);
static ASTStatement *parse_function(LitParser *parser);
static ASTStatement *parse_return(LitParser *parser);
static ASTStatement *parse_field(LitParser *parser, LitString *name, bool is_static);
static ASTStatement *parse_method(LitParser *parser, bool is_static);
static ASTStatement *parse_class(LitParser *parser);
static ASTStatement *parse_statement(LitParser *parser);
static ASTStatement *parse_declaration(LitParser *parser);

const char* token_name(int t)
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

static void setup_rules()
{
    rules[LITTOK_LEFT_PAREN] = ASTParseRule{ parse_grouping_or_lambda, parse_call, LITPREC_CALL };
    rules[LITTOK_PLUS] = ASTParseRule{ nullptr, parse_binary, LITPREC_TERM };
    rules[LITTOK_MINUS] = ASTParseRule{ parse_unary, parse_binary, LITPREC_TERM };
    rules[LITTOK_BANG] = ASTParseRule{ parse_unary, parse_binary, LITPREC_TERM };
    rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_SLASH] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_SHARP] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_BAR] = ASTParseRule{ nullptr, parse_binary, LITPREC_BOR };
    rules[LITTOK_AMPERSAND] = ASTParseRule{ nullptr, parse_binary, LITPREC_BAND };
    rules[LITTOK_TILDE] = ASTParseRule{ parse_unary, nullptr, LITPREC_UNARY };
    rules[LITTOK_CARET] = ASTParseRule{ nullptr, parse_binary, LITPREC_BOR };
    rules[LITTOK_LESS_LESS] = ASTParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
    rules[LITTOK_GREATER_GREATER] = ASTParseRule{ nullptr, parse_binary, LITPREC_SHIFT };
    rules[LITTOK_PERCENT] = ASTParseRule{ nullptr, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_IS] = ASTParseRule{ nullptr, parse_binary, LITPREC_IS };
    rules[LITTOK_NUMBER] = ASTParseRule{ parse_number, nullptr, LITPREC_NONE };
    rules[LITTOK_TRUE] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
    rules[LITTOK_FALSE] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
    rules[LITTOK_NULL] = ASTParseRule{ parse_literal, nullptr, LITPREC_NONE };
    rules[LITTOK_BANG_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
    rules[LITTOK_EQUAL_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_EQUALITY };
    rules[LITTOK_GREATER] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_GREATER_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_LESS] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_LESS_EQUAL] = ASTParseRule{ nullptr, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_STRING] = ASTParseRule{ parse_string, nullptr, LITPREC_NONE };
    rules[LITTOK_INTERPOLATION] = ASTParseRule{ parse_interpolation, nullptr, LITPREC_NONE };
    rules[LITTOK_IDENTIFIER] = ASTParseRule{ parse_variable_expression, nullptr, LITPREC_NONE };
    rules[LITTOK_NEW] = ASTParseRule{ parse_new_expression, nullptr, LITPREC_NONE };
    rules[LITTOK_PLUS_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_MINUS_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_STAR_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_SLASH_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_SHARP_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_PERCENT_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_CARET_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_BAR_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_AMPERSAND_EQUAL] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_PLUS_PLUS] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_MINUS_MINUS] = ASTParseRule{ nullptr, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_AMPERSAND_AMPERSAND] = ASTParseRule{ nullptr, parse_and, LITPREC_AND };
    rules[LITTOK_BAR_BAR] = ASTParseRule{ nullptr, parse_or, LITPREC_AND };
    rules[LITTOK_QUESTION_QUESTION] = ASTParseRule{ nullptr, parse_null_filter, LITPREC_NULL };
    rules[LITTOK_DOT] = ASTParseRule{ nullptr, parse_dot, LITPREC_CALL };
    rules[LITTOK_SMALL_ARROW] = ASTParseRule{ nullptr, parse_dot, LITPREC_CALL };
    rules[LITTOK_DOT_DOT] = ASTParseRule{ nullptr, parse_range, LITPREC_RANGE };
    rules[LITTOK_DOT_DOT_DOT] = ASTParseRule{ parse_variable_expression, nullptr, LITPREC_ASSIGNMENT };
    rules[LITTOK_LEFT_BRACKET] = ASTParseRule{ parse_array, parse_subscript, LITPREC_NONE };
    rules[LITTOK_LEFT_BRACE] = ASTParseRule{ parse_object, nullptr, LITPREC_NONE };
    rules[LITTOK_THIS] = ASTParseRule{ parse_this, nullptr, LITPREC_NONE };
    rules[LITTOK_SUPER] = ASTParseRule{ parse_super, nullptr, LITPREC_NONE };
    rules[LITTOK_QUESTION] = ASTParseRule{ nullptr, parse_ternary_or_question, LITPREC_EQUALITY };
    rules[LITTOK_REF] = ASTParseRule{ parse_reference, nullptr, LITPREC_NONE };
    //rules[LITTOK_SEMICOLON] = ASTParseRule{nullptr, nullptr, LITPREC_NONE};
}


static void prs_init_compiler(LitParser* parser, LitCompiler* compiler)
{
    compiler->scope_depth = 0;
    compiler->function = nullptr;
    compiler->enclosing = (struct LitCompiler*)parser->compiler;

    parser->compiler = compiler;
}

static void prs_end_compiler(LitParser* parser, LitCompiler* compiler)
{
    parser->compiler = (LitCompiler*)compiler->enclosing;
}

static void prs_begin_scope(LitParser* parser)
{
    parser->compiler->scope_depth++;
}

static void prs_end_scope(LitParser* parser)
{
    parser->compiler->scope_depth--;
}

static ASTParseRule* get_rule(LitTokenType type)
{
    return &rules[type];
}

static inline bool prs_is_at_end(LitParser* parser)
{
    return parser->current.type == LITTOK_EOF;
}



static void string_error(LitParser* parser, LitToken* token, const char* message)
{
    (void)token;
    if(parser->panic_mode)
    {
        return;
    }
    parser->state->raiseError(COMPILE_ERROR, message);
    parser->had_error = true;
    sync(parser);
}

static void error_at(LitParser* parser, LitToken* token, LitError error, va_list args)
{
    string_error(parser, token, lit_vformat_error(parser->state, token->line, error, args)->chars);
}

static void error_at_current(LitParser* parser, LitError error, ...)
{
    va_list args;
    va_start(args, error);
    error_at(parser, &parser->current, error, args);
    va_end(args);
}

static void prs_error(LitParser* parser, LitError error, ...)
{
    va_list args;
    va_start(args, error);
    error_at(parser, &parser->previous, error, args);
    va_end(args);
}

static void prs_advance(LitParser* parser)
{
    parser->previous = parser->current;

    while(true)
    {
        parser->current = lit_scan_token(parser->state->scanner);
        if(parser->current.type != LITTOK_ERROR)
        {
            break;
        }
        string_error(parser, &parser->current, parser->current.start);
    }
}

static bool check(LitParser* parser, LitTokenType type)
{
    return parser->current.type == type;
}

static bool prs_match(LitParser* parser, LitTokenType type)
{
    if(parser->current.type == type)
    {
        prs_advance(parser);
        return true;
    }
    return false;
}

static bool match_new_line(LitParser* parser)
{
    while(true)
    {
        if(!prs_match(parser, LITTOK_NEW_LINE))
        {
            return false;
        }
    }
    return true;
}

static void ignore_new_lines(LitParser* parser)
{
    match_new_line(parser);
}

static void consume(LitParser* parser, LitTokenType type, const char* error)
{
    bool line;
    size_t olen;
    const char* fmt;
    const char* otext;
    if(parser->current.type == type)
    {
        prs_advance(parser);
        return;
    }
    line = parser->previous.type == LITTOK_NEW_LINE;
    olen = (line ? 8 : parser->previous.length);
    otext = (line ? "new line" : parser->previous.start);
    fmt = lit_format_error(parser->state, parser->current.line, LitError::LITERROR_EXPECTATION_UNMET, error, olen, otext)->chars;
    string_error(parser, &parser->current,fmt);
}

static ASTStatement* parse_block(LitParser* parser)
{
    ASTStmtBlock* statement;
    prs_begin_scope(parser);
    statement = lit_create_block_statement(parser->state, parser->previous.line);
    while(true)
    {
        ignore_new_lines(parser);
        if(check(parser, LITTOK_RIGHT_BRACE) || check(parser, LITTOK_EOF))
        {
            break;
        }
        ignore_new_lines(parser);
        statement->statements.push(parse_statement(parser));
        ignore_new_lines(parser);
    }
    ignore_new_lines(parser);
    consume(parser, LITTOK_RIGHT_BRACE, "'}'");
    ignore_new_lines(parser);
    prs_end_scope(parser);
    return (ASTStatement*)statement;
}

static ASTExpression* parse_precedence(LitParser* parser, LitPrecedence precedence, bool err)
{
    bool new_line;
    bool prev_newline;
    bool parser_prev_newline;
    bool can_assign;
    ASTExpression* expr;
    ASTParseRule::PrefixFuncType prefix_rule;
    ASTParseRule::InfixFuncType infix_rule;
    LitToken previous;
    (void)new_line;
    prev_newline = false;
    previous = parser->previous;
    prs_advance(parser);
    prefix_rule = get_rule(parser->previous.type)->prefix;
    if(prefix_rule == nullptr)
    {
        //fprintf(stderr, "parser->previous.type=%s, parser->current.type=%s\n", token_name(parser->previous.type), token_name(parser->current.type));
        if(parser->previous.type == LITTOK_SEMICOLON)
        {
            if(parser->current.type == LITTOK_NEW_LINE)
            {
                prs_advance(parser);
                return parse_precedence(parser, precedence, err);
            }
            return nullptr;
        }
        // todo: file start
        new_line = previous.start != nullptr && *previous.start == '\n';
        parser_prev_newline = parser->previous.start != nullptr && *parser->previous.start == '\n';
        prs_error(parser, LitError::LITERROR_EXPECTED_EXPRESSION,
            (prev_newline ? 8 : previous.length),
            (prev_newline ? "new line" : previous.start),
            (parser_prev_newline ? 8 : parser->previous.length),
            (parser_prev_newline ? "new line" : parser->previous.start)
        );
        return nullptr;
        
    }
    can_assign = precedence <= LITPREC_ASSIGNMENT;
    expr = prefix_rule(parser, can_assign);
    ignore_new_lines(parser);
    while(precedence <= get_rule(parser->current.type)->precedence)
    {
        prs_advance(parser);
        infix_rule = get_rule(parser->previous.type)->infix;
        expr = infix_rule(parser, expr, can_assign);
    }
    if(err && can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        prs_error(parser, LitError::LITERROR_INVALID_ASSIGMENT_TARGET);
    }
    return expr;
}

static ASTExpression* parse_number(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    return (ASTExpression*)ASTExprLiteral::make(parser->state, parser->previous.line, parser->previous.value);
}

static ASTExpression* parse_lambda(LitParser* parser, ASTExprLambda* lambda)
{
    lambda->body = parse_statement(parser);
    return (ASTExpression*)lambda;
}

static void parse_parameters(LitParser* parser, PCGenericArray<ASTExprFuncParam>* parameters)
{
    bool had_default;
    size_t arg_length;
    const char* arg_name;
    ASTExpression* default_value;
    had_default = false;
    while(!check(parser, LITTOK_RIGHT_PAREN))
    {
        // Vararg ...
        if(prs_match(parser, LITTOK_DOT_DOT_DOT))
        {
            parameters->push(ASTExprFuncParam{ "...", 3, nullptr });
            return;
        }
        consume(parser, LITTOK_IDENTIFIER, "argument name");
        arg_name = parser->previous.start;
        arg_length = parser->previous.length;
        default_value = nullptr;
        if(prs_match(parser, LITTOK_EQUAL))
        {
            had_default = true;
            default_value = parse_expression(parser);
        }
        else if(had_default)
        {
            prs_error(parser, LitError::LITERROR_DEFAULT_ARG_CENTRED);
        }
        parameters->push(ASTExprFuncParam{ arg_name, arg_length, default_value });
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
    }
}

static ASTExpression* parse_grouping_or_lambda(LitParser* parser, bool can_assign)
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
    ASTExpression* expression;
    ASTExpression* default_value;
    LitScanner* scanner;
    (void)can_assign;
    (void)had_arrow;
    (void)had_array;
    had_arrow = false;
    if(prs_match(parser, LITTOK_RIGHT_PAREN))
    {
        consume(parser, LITTOK_ARROW, "=> after lambda arguments");
        return parse_lambda(parser, lit_create_lambda_expression(parser->state, parser->previous.line));
    }
    start = parser->previous.start;
    line = parser->previous.line;
    if(prs_match(parser, LITTOK_IDENTIFIER) || prs_match(parser, LITTOK_DOT_DOT_DOT))
    {
        LitState* state = parser->state;
        first_arg_start = parser->previous.start;
        first_arg_length = parser->previous.length;
        if(prs_match(parser, LITTOK_COMMA) || (prs_match(parser, LITTOK_RIGHT_PAREN) && prs_match(parser, LITTOK_ARROW)))
        {
            had_array = parser->previous.type == LITTOK_ARROW;
            had_vararg= parser->previous.type == LITTOK_DOT_DOT_DOT;
            // This is a lambda
            ASTExprLambda* lambda = lit_create_lambda_expression(state, line);
            ASTExpression* def_value = nullptr;
            had_default = prs_match(parser, LITTOK_EQUAL);
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
                    if(prs_match(parser, LITTOK_DOT_DOT_DOT))
                    {
                        stop = true;
                    }
                    else
                    {
                        consume(parser, LITTOK_IDENTIFIER, "argument name");
                    }

                    arg_name = parser->previous.start;
                    arg_length = parser->previous.length;
                    default_value = nullptr;
                    if(prs_match(parser, LITTOK_EQUAL))
                    {
                        default_value = parse_expression(parser);
                        had_default = true;
                    }
                    else if(had_default)
                    {
                        prs_error(parser, LitError::LITERROR_DEFAULT_ARG_CENTRED);
                    }
                    lambda->parameters.push(ASTExprFuncParam{ arg_name, arg_length, default_value });
                    if(stop)
                    {
                        break;
                    }
                } while(prs_match(parser, LITTOK_COMMA));
            }
            #if 0
            if(!had_arrow)
            {
                consume(parser, LITTOK_RIGHT_PAREN, "')' after lambda parameters");
                consume(parser, LITTOK_ARROW, "=> after lambda parameters");
            }
            #endif
            return parse_lambda(parser, lambda);
        }
        else
        {
            // Ouch, this was a grouping with a single identifier
            scanner = state->scanner;
            scanner->current = start;
            scanner->line = line;
            parser->current = lit_scan_token(scanner);
            prs_advance(parser);
        }
    }
    expression = parse_expression(parser);
    consume(parser, LITTOK_RIGHT_PAREN, "')' after grouping expression");
    return expression;
}

static ASTExpression* parse_call(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    ASTExpression* e;
    ASTExprVar* ee;
    ASTExprCall* expression;
    expression = lit_create_call_expression(parser->state, parser->previous.line, prev);
    while(!check(parser, LITTOK_RIGHT_PAREN))
    {
        e = parse_expression(parser);
        expression->args.push(e);
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
        if(e->type == ASTExpression::Type::Variable)
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
        prs_error(parser, LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)expression->args.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after arguments");
    return (ASTExpression*)expression;
}

static ASTExpression* parse_unary(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    ASTExpression* expression;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    expression = parse_precedence(parser, LITPREC_UNARY, true);
    return (ASTExpression*)lit_create_unary_expression(parser->state, line, expression, op);
}

static ASTExpression* parse_binary(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    bool invert;
    size_t line;
    ASTParseRule* rule;
    ASTExpression* expression;
    LitTokenType op;
    invert = parser->previous.type == LITTOK_BANG;
    if(invert)
    {
        consume(parser, LITTOK_IS, "'is' after '!'");
    }
    op = parser->previous.type;
    line = parser->previous.line;
    rule = get_rule(op);
    expression = parse_precedence(parser, (LitPrecedence)(rule->precedence + 1), true);
    expression = (ASTExpression*)lit_create_binary_expression(parser->state, line, prev, expression, op);
    if(invert)
    {
        expression = (ASTExpression*)lit_create_unary_expression(parser->state, line, expression, LITTOK_BANG);
    }
    return expression;
}

static ASTExpression* parse_and(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (ASTExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_AND, true), op);
}

static ASTExpression* parse_or(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (ASTExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_OR, true), op);
}

static ASTExpression* parse_null_filter(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (ASTExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_NULL, true), op);
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

static ASTExpression* parse_compound(LitParser* parser, ASTExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    ASTExprBinary* binary;
    ASTExpression* expression;
    ASTParseRule* rule;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    rule = get_rule(op);
    if(op == LITTOK_PLUS_PLUS || op == LITTOK_MINUS_MINUS)
    {
        expression = (ASTExpression*)ASTExprLiteral::make(parser->state, line, LitObject::toValue(1));
    }
    else
    {
        expression = parse_precedence(parser, (LitPrecedence)(rule->precedence + 1), true);
    }
    binary = lit_create_binary_expression(parser->state, line, prev, expression, convert_compound_operator(op));
    binary->ignore_left = true;// To make sure we don't free it twice
    return (ASTExpression*)lit_create_assign_expression(parser->state, line, prev, (ASTExpression*)binary);
}

static ASTExpression* parse_literal(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    line = parser->previous.line;
    switch(parser->previous.type)
    {
        case LITTOK_TRUE:
            {
                return (ASTExpression*)ASTExprLiteral::make(parser->state, line, LitObject::TrueVal);
            }
            break;
        case LITTOK_FALSE:
            {
                return (ASTExpression*)ASTExprLiteral::make(parser->state, line, LitObject::FalseVal);
            }
            break;
        case LITTOK_NULL:
            {
                return (ASTExpression*)ASTExprLiteral::make(parser->state, line, LitObject::NullVal);
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

static ASTExpression* parse_string(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    ASTExpression* expression;
    expression = (ASTExpression*)ASTExprLiteral::make(parser->state, parser->previous.line, parser->previous.value);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static ASTExpression* parse_interpolation(LitParser* parser, bool can_assign)
{
    ASTExprInterpolation* expression;
    (void)can_assign;
    expression = lit_create_interpolation_expression(parser->state, parser->previous.line);
    do
    {
        if(LitObject::as<LitString>(parser->previous.value)->length() > 0)
        {
            expression->expressions.push((ASTExpression*)ASTExprLiteral::make(parser->state, parser->previous.line, parser->previous.value));
        }
        expression->expressions.push(parse_expression(parser));
    } while(prs_match(parser, LITTOK_INTERPOLATION));
    consume(parser, LITTOK_STRING, "end of interpolation");
    if(LitObject::as<LitString>(parser->previous.value)->length() > 0)
    {
        expression->expressions.push((ASTExpression*)ASTExprLiteral::make(parser->state, parser->previous.line, parser->previous.value));
    }
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, (ASTExpression*)expression, can_assign);
    }
    return (ASTExpression*)expression;
}

static ASTExpression* parse_object(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    ASTExprObject* objexpr;
    objexpr = lit_create_object_expression(parser->state, parser->previous.line);
    ignore_new_lines(parser);
    while(!check(parser, LITTOK_RIGHT_BRACE))
    {
        ignore_new_lines(parser);
        consume(parser, LITTOK_IDENTIFIER, "key string after '{'");
        objexpr->keys.push(LitString::copy(parser->state, parser->previous.start, parser->previous.length)->asValue());
        ignore_new_lines(parser);
        consume(parser, LITTOK_EQUAL, "'=' after key string");
        ignore_new_lines(parser);
        objexpr->values.push(parse_expression(parser));
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
    }
    ignore_new_lines(parser);
    consume(parser, LITTOK_RIGHT_BRACE, "'}' after object");
    return (ASTExpression*)objexpr;
}

static ASTExpression* parse_variable_expression_base(LitParser* parser, bool can_assign, bool isnew)
{
    (void)can_assign;
    bool had_args;
    ASTExprCall* call;
    ASTExpression* expression;
    expression = (ASTExpression*)lit_create_var_expression(parser->state, parser->previous.line, parser->previous.start, parser->previous.length);
    if(isnew)
    {
        had_args = check(parser, LITTOK_LEFT_PAREN);
        call = nullptr;
        if(had_args)
        {
            prs_advance(parser);
            call = (ASTExprCall*)parse_call(parser, expression, false);
        }
        if(prs_match(parser, LITTOK_LEFT_BRACE))
        {
            if(call == nullptr)
            {
                call = lit_create_call_expression(parser->state, expression->line, expression);
            }
            call->objexpr = parse_object(parser, false);
        }
        else if(!had_args)
        {
            error_at_current(parser, LitError::LITERROR_EXPECTATION_UNMET, "argument list for instance creation",
                             parser->previous.length, parser->previous.start);
        }
        return (ASTExpression*)call;
    }
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    if(can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        return (ASTExpression*)lit_create_assign_expression(parser->state, parser->previous.line, expression,
                                                            parse_expression(parser));
    }
    return expression;
}

static ASTExpression* parse_variable_expression(LitParser* parser, bool can_assign)
{
    return parse_variable_expression_base(parser, can_assign, false);
}

static ASTExpression* parse_new_expression(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    consume(parser, LITTOK_IDENTIFIER, "class name after 'new'");
    return parse_variable_expression_base(parser, false, true);
}

static ASTExpression* parse_dot(LitParser* parser, ASTExpression* previous, bool can_assign)
{
    (void)can_assign;
    bool ignored;
    size_t line;
    size_t length;
    const char* name;
    ASTExpression* expression;
    line = parser->previous.line;
    ignored = parser->previous.type == LITTOK_SMALL_ARROW;
    if(!(prs_match(parser, LITTOK_CLASS) || prs_match(parser, LITTOK_SUPER)))
    {// class and super are allowed field names
        consume(parser, LITTOK_IDENTIFIER, ignored ? "propety name after '->'" : "property name after '.'");
    }
    name = parser->previous.start;
    length = parser->previous.length;
    if(!ignored && can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        return (ASTExpression*)lit_create_set_expression(parser->state, line, previous, name, length, parse_expression(parser));
    }
    expression = (ASTExpression*)lit_create_get_expression(parser->state, line, previous, name, length, false, ignored);
    if(!ignored && prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static ASTExpression* parse_range(LitParser* parser, ASTExpression* previous, bool can_assign)
{
    (void)can_assign;
    size_t line;
    line = parser->previous.line;
    return (ASTExpression*)lit_create_range_expression(parser->state, line, previous, parse_expression(parser));
}

static ASTExpression* parse_ternary_or_question(LitParser* parser, ASTExpression* previous, bool can_assign)
{
    (void)can_assign;
    bool ignored;
    size_t line;
    ASTExpression* if_branch;
    ASTExpression* else_branch;
    line = parser->previous.line;
    if(prs_match(parser, LITTOK_DOT) || prs_match(parser, LITTOK_SMALL_ARROW))
    {
        ignored = parser->previous.type == LITTOK_SMALL_ARROW;
        consume(parser, LITTOK_IDENTIFIER, ignored ? "property name after '->'" : "property name after '.'");
        return (ASTExpression*)lit_create_get_expression(parser->state, line, previous, parser->previous.start,
                                                         parser->previous.length, true, ignored);
    }
    if_branch = parse_expression(parser);
    consume(parser, LITTOK_COLON, "':' after expression");
    else_branch = parse_expression(parser);
    return (ASTExpression*)lit_create_if_experssion(parser->state, line, previous, if_branch, else_branch);
}

static ASTExpression* parse_array(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    ASTExprArray* array;
    array = lit_create_array_expression(parser->state, parser->previous.line);
    ignore_new_lines(parser);
    while(!check(parser, LITTOK_RIGHT_BRACKET))
    {
        ignore_new_lines(parser);
        array->values.push(parse_expression(parser));
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
    }
    ignore_new_lines(parser);
    consume(parser, LITTOK_RIGHT_BRACKET, "']' after array");
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, (ASTExpression*)array, can_assign);
    }
    return (ASTExpression*)array;
}

static ASTExpression* parse_subscript(LitParser* parser, ASTExpression* previous, bool can_assign)
{
    size_t line;
    ASTExpression* index;
    ASTExpression* expression;
    line = parser->previous.line;
    index = parse_expression(parser);
    consume(parser, LITTOK_RIGHT_BRACKET, "']' after subscript");
    expression = (ASTExpression*)lit_create_subscript_expression(parser->state, line, previous, index);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    else if(can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        return (ASTExpression*)lit_create_assign_expression(parser->state, parser->previous.line, expression, parse_expression(parser));
    }
    return expression;
}

static ASTExpression* parse_this(LitParser* parser, bool can_assign)
{
    ASTExpression* expression;
    expression = (ASTExpression*)lit_create_this_expression(parser->state, parser->previous.line);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static ASTExpression* parse_super(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    bool ignoring;
    size_t line;
    ASTExpression* expression;
    line = parser->previous.line;

    if(!(prs_match(parser, LITTOK_DOT) || prs_match(parser, LITTOK_SMALL_ARROW)))
    {
        expression = (ASTExpression*)lit_create_super_expression(
        parser->state, line, LitString::copy(parser->state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1), false);
        consume(parser, LITTOK_LEFT_PAREN, "'(' after 'super'");
        return parse_call(parser, expression, false);
    }
    ignoring = parser->previous.type == LITTOK_SMALL_ARROW;
    consume(parser, LITTOK_IDENTIFIER, ignoring ? "super method name after '->'" : "super method name after '.'");
    expression = (ASTExpression*)lit_create_super_expression(
    parser->state, line, LitString::copy(parser->state, parser->previous.start, parser->previous.length), ignoring);
    if(prs_match(parser, LITTOK_LEFT_PAREN))
    {
        return parse_call(parser, expression, false);
    }
    return expression;
}

static ASTExpression* parse_reference(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    ASTExprReference* expression;
    line = parser->previous.line;
    ignore_new_lines(parser);
    expression = ASTExprReference::make(parser->state, line, parse_precedence(parser, LITPREC_CALL, false));
    if(prs_match(parser, LITTOK_EQUAL))
    {
        return (ASTExpression*)lit_create_assign_expression(parser->state, line, (ASTExpression*)expression, parse_expression(parser));
    }
    return (ASTExpression*)expression;
}

static ASTExpression* parse_expression(LitParser* parser)
{
    ignore_new_lines(parser);
    return parse_precedence(parser, LITPREC_ASSIGNMENT, true);
}

static ASTStatement* parse_var_declaration(LitParser* parser)
{
    bool constant;
    size_t line;
    size_t length;
    const char* name;
    ASTExpression* vexpr;
    constant = parser->previous.type == LITTOK_CONST;
    line = parser->previous.line;
    consume(parser, LITTOK_IDENTIFIER, "variable name");
    name = parser->previous.start;
    length = parser->previous.length;
    vexpr = nullptr;
    if(prs_match(parser, LITTOK_EQUAL))
    {
        vexpr = parse_expression(parser);
    }
    return (ASTStatement*)lit_create_var_statement(parser->state, line, name, length, vexpr, constant);
}

static ASTStatement* parse_if(LitParser* parser)
{
    size_t line;
    bool invert;
    bool had_paren;
    ASTExpression* condition;
    ASTStatement* if_branch;
    PCGenericArray<ASTExpression*>* elseif_conditions;
    PCGenericArray<ASTStatement*>* elseif_branches;
    ASTStatement* else_branch;
    ASTExpression* e;
    line = parser->previous.line;
    invert = prs_match(parser, LITTOK_BANG);
    had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
    condition = parse_expression(parser);
    if(had_paren)
    {
        consume(parser, LITTOK_RIGHT_PAREN, "')'");
    }
    if(invert)
    {
        condition = (ASTExpression*)lit_create_unary_expression(parser->state, condition->line, condition, LITTOK_BANG);
    }
    ignore_new_lines(parser);
    if_branch = parse_statement(parser);
    elseif_conditions = nullptr;
    elseif_branches = nullptr;
    else_branch = nullptr;
    ignore_new_lines(parser);
    while(prs_match(parser, LITTOK_ELSE))
    {
        // else if
        ignore_new_lines(parser);
        if(prs_match(parser, LITTOK_IF))
        {
            if(elseif_conditions == nullptr)
            {
                elseif_conditions = ASTExpression::makeList(parser->state);
                elseif_branches = ASTStatement::makeList(parser->state);
            }
            invert = prs_match(parser, LITTOK_BANG);
            had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
            ignore_new_lines(parser);
            e = parse_expression(parser);
            if(had_paren)
            {
                consume(parser, LITTOK_RIGHT_PAREN, "')'");
            }
            ignore_new_lines(parser);
            if(invert)
            {
                e = (ASTExpression*)lit_create_unary_expression(parser->state, condition->line, e, LITTOK_BANG);
            }
            elseif_conditions->push(e);
            elseif_branches->push(parse_statement(parser));
            continue;
        }
        // else
        if(else_branch != nullptr)
        {
            prs_error(parser, LitError::LITERROR_MULTIPLE_ELSE_BRANCHES);
        }
        ignore_new_lines(parser);
        else_branch = parse_statement(parser);
    }
    return (ASTStatement*)lit_create_if_statement(parser->state, line, condition, if_branch, else_branch, elseif_conditions, elseif_branches);
}

static ASTStatement* parse_for(LitParser* parser)
{

    bool c_style;
    bool had_paren;
    size_t line;
    ASTExpression* condition;
    ASTExpression* increment;
    ASTStatement* var;
    ASTExpression* exprinit;
    line= parser->previous.line;
    had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
    var = nullptr;
    exprinit = nullptr;
    if(!check(parser, LITTOK_SEMICOLON))
    {
        if(prs_match(parser, LITTOK_VAR))
        {
            var = parse_var_declaration(parser);
        }
        else
        {
            exprinit = parse_expression(parser);
        }
    }
    c_style = !prs_match(parser, LITTOK_IN);
    condition= nullptr;
    increment = nullptr;
    if(c_style)
    {
        consume(parser, LITTOK_SEMICOLON, "';'");
        condition = check(parser, LITTOK_SEMICOLON) ? nullptr : parse_expression(parser);
        consume(parser, LITTOK_SEMICOLON, "';'");
        increment = check(parser, LITTOK_RIGHT_PAREN) ? nullptr : parse_expression(parser);
    }
    else
    {
        condition = parse_expression(parser);
        if(var == nullptr)
        {
            prs_error(parser, LitError::LITERROR_VAR_MISSING_IN_FORIN);
        }
    }
    if(had_paren)
    {
        consume(parser, LITTOK_RIGHT_PAREN, "')'");
    }
    ignore_new_lines(parser);
    return (ASTStatement*)lit_create_for_statement(parser->state, line, exprinit, var, condition, increment,
                                                   parse_statement(parser), c_style);
}

static ASTStatement* parse_while(LitParser* parser)
{
    bool had_paren;
    size_t line;
    ASTStatement* body;
    line = parser->previous.line;
    had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
    ASTExpression* condition = parse_expression(parser);
    if(had_paren)
    {
        consume(parser, LITTOK_RIGHT_PAREN, "')'");
    }
    ignore_new_lines(parser);
    body = parse_statement(parser);
    return (ASTStatement*)lit_create_while_statement(parser->state, line, condition, body);
}

static ASTStatement* parse_function(LitParser* parser)
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
        consume(parser, LITTOK_FUNCTION, "'function' after 'export'");
    }
    line = parser->previous.line;
    consume(parser, LITTOK_IDENTIFIER, "function name");
    function_name = parser->previous.start;
    function_length = parser->previous.length;
    if(prs_match(parser, LITTOK_DOT))
    {
        consume(parser, LITTOK_IDENTIFIER, "function name");
        lambda = lit_create_lambda_expression(parser->state, line);
        to = lit_create_set_expression(
        parser->state, line, (ASTExpression*)lit_create_var_expression(parser->state, line, function_name, function_length),
        parser->previous.start, parser->previous.length, (ASTExpression*)lambda);
        consume(parser, LITTOK_LEFT_PAREN, "'(' after function name");
        prs_init_compiler(parser, &compiler);
        prs_begin_scope(parser);
        parse_parameters(parser, &lambda->parameters);
        if(lambda->parameters.m_count > 255)
        {
            prs_error(parser, LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)lambda->parameters.m_count);
        }
        consume(parser, LITTOK_RIGHT_PAREN, "')' after function arguments");
        ignore_new_lines(parser);
        lambda->body = parse_statement(parser);
        prs_end_scope(parser);
        prs_end_compiler(parser, &compiler);
        return (ASTStatement*)lit_create_expression_statement(parser->state, line, (ASTExpression*)to);
    }
    function = lit_create_function_statement(parser->state, line, function_name, function_length);
    function->exported = isexport;
    consume(parser, LITTOK_LEFT_PAREN, "'(' after function name");
    prs_init_compiler(parser, &compiler);
    prs_begin_scope(parser);
    parse_parameters(parser, &function->parameters);
    if(function->parameters.m_count > 255)
    {
        prs_error(parser, LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)function->parameters.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after function arguments");
    function->body = parse_statement(parser);
    prs_end_scope(parser);
    prs_end_compiler(parser, &compiler);
    return (ASTStatement*)function;
}

static ASTStatement* parse_return(LitParser* parser)
{
    size_t line;
    ASTExpression* expression;
    line = parser->previous.line;
    expression = nullptr;
    if(!check(parser, LITTOK_NEW_LINE) && !check(parser, LITTOK_RIGHT_BRACE))
    {
        expression = parse_expression(parser);
    }
    return (ASTStatement*)lit_create_return_statement(parser->state, line, expression);
}

static ASTStatement* parse_field(LitParser* parser, LitString* name, bool is_static)
{
    size_t line;
    ASTStatement* getter;
    ASTStatement* setter;
    line = parser->previous.line;
    getter = nullptr;
    setter = nullptr;
    if(prs_match(parser, LITTOK_ARROW))
    {
        getter = parse_statement(parser);
    }
    else
    {
        prs_match(parser, LITTOK_LEFT_BRACE);// Will be LITTOK_LEFT_BRACE, otherwise this method won't be called
        ignore_new_lines(parser);
        if(prs_match(parser, LITTOK_GET))
        {
            prs_match(parser, LITTOK_ARROW);// Ignore it if it's present
            getter = parse_statement(parser);
        }
        ignore_new_lines(parser);
        if(prs_match(parser, LITTOK_SET))
        {
            prs_match(parser, LITTOK_ARROW);// Ignore it if it's present
            setter = parse_statement(parser);
        }
        if(getter == nullptr && setter == nullptr)
        {
            prs_error(parser, LitError::LITERROR_NO_GETTER_AND_SETTER);
        }
        ignore_new_lines(parser);
        consume(parser, LITTOK_RIGHT_BRACE, "'}' after field declaration");
    }
    return (ASTStatement*)lit_create_field_statement(parser->state, line, name, getter, setter, is_static);
}

static ASTStatement* parse_method(LitParser* parser, bool is_static)
{
    size_t i;
    LitCompiler compiler;
    ASTStmtMethod* method;
    LitString* name;
    if(prs_match(parser, LITTOK_STATIC))
    {
        is_static = true;
    }
    name = nullptr;
    consume(parser, LITTOK_FUNCTION, "expected 'function'");
    if(prs_match(parser, LITTOK_OPERATOR))
    {
        if(is_static)
        {
            prs_error(parser, LitError::LITERROR_STATIC_OPERATOR);
        }
        i = 0;
        while(operators[i] != LITTOK_EOF)
        {
            if(prs_match(parser, operators[i]))
            {
                break;
            }
            i++;
        }
        if(parser->previous.type == LITTOK_LEFT_BRACKET)
        {
            consume(parser, LITTOK_RIGHT_BRACKET, "']' after '[' in op method declaration");
            name = LitString::copy(parser->state, "[]", 2);
        }
        else
        {
            name = LitString::copy(parser->state, parser->previous.start, parser->previous.length);
        }
    }
    else
    {
        consume(parser, LITTOK_IDENTIFIER, "method name");
        name = LitString::copy(parser->state, parser->previous.start, parser->previous.length);
        if(check(parser, LITTOK_LEFT_BRACE) || check(parser, LITTOK_ARROW))
        {
            return parse_field(parser, name, is_static);
        }
    }
    method = lit_create_method_statement(parser->state, parser->previous.line, name, is_static);
    prs_init_compiler(parser, &compiler);
    prs_begin_scope(parser);
    consume(parser, LITTOK_LEFT_PAREN, "'(' after method name");
    parse_parameters(parser, &method->parameters);
    if(method->parameters.m_count > 255)
    {
        prs_error(parser, LitError::LITERROR_TOO_MANY_FUNCTION_ARGS, (int)method->parameters.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after method arguments");
    method->body = parse_statement(parser);
    prs_end_scope(parser);
    prs_end_compiler(parser, &compiler);
    return (ASTStatement*)method;
}

static ASTStatement* parse_class(LitParser* parser)
{
    bool finished_parsing_fields;
    bool field_is_static;
    size_t line;
    bool is_static;
    LitString* name;
    LitString* super;
    ASTStmtClass* klass;
    ASTStatement* var;
    ASTStatement* method;
    if(setjmp(prs_jmpbuffer))
    {
        return nullptr;
    }
    line = parser->previous.line;
    is_static = parser->previous.type == LITTOK_STATIC;
    if(is_static)
    {
        consume(parser, LITTOK_CLASS, "'class' after 'static'");
    }
    consume(parser, LITTOK_IDENTIFIER, "class name after 'class'");
    name = LitString::copy(parser->state, parser->previous.start, parser->previous.length);
    super = nullptr;
    if(prs_match(parser, LITTOK_COLON))
    {
        consume(parser, LITTOK_IDENTIFIER, "super class name after ':'");
        super = LitString::copy(parser->state, parser->previous.start, parser->previous.length);
        if(super == name)
        {
            prs_error(parser, LitError::LITERROR_SELF_INHERITED_CLASS);
        }
    }
    klass = lit_create_class_statement(parser->state, line, name, super);
    ignore_new_lines(parser);
    consume(parser, LITTOK_LEFT_BRACE, "'{' before class body");
    ignore_new_lines(parser);
    finished_parsing_fields = false;
    while(!check(parser, LITTOK_RIGHT_BRACE))
    {
        field_is_static = false;
        if(prs_match(parser, LITTOK_STATIC))
        {
            field_is_static = true;
            if(prs_match(parser, LITTOK_VAR))
            {
                if(finished_parsing_fields)
                {
                    prs_error(parser, LitError::LITERROR_STATIC_FIELDS_AFTER_METHODS);
                }
                var = parse_var_declaration(parser);
                if(var != nullptr)
                {
                    klass->fields.push(var);
                }
                ignore_new_lines(parser);
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
        ignore_new_lines(parser);
    }
    consume(parser, LITTOK_RIGHT_BRACE, "'}' after class body");
    return (ASTStatement*)klass;
}

static void sync(LitParser* parser)
{
    parser->panic_mode = false;
    while(parser->current.type != LITTOK_EOF)
    {
        if(parser->previous.type == LITTOK_NEW_LINE)
        {
            longjmp(prs_jmpbuffer, 1);
            return;
        }
        switch(parser->current.type)
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
                prs_advance(parser);
            }
        }
    }
}

static ASTStatement* parse_statement(LitParser* parser)
{
    ASTExpression* expression;
    ignore_new_lines(parser);
    if(setjmp(prs_jmpbuffer))
    {
        return nullptr;
    }
    if(prs_match(parser, LITTOK_VAR) || prs_match(parser, LITTOK_CONST))
    {
        return parse_var_declaration(parser);
    }
    else if(prs_match(parser, LITTOK_IF))
    {
        return parse_if(parser);
    }
    else if(prs_match(parser, LITTOK_FOR))
    {
        return parse_for(parser);
    }
    else if(prs_match(parser, LITTOK_WHILE))
    {
        return parse_while(parser);
    }
    else if(prs_match(parser, LITTOK_CONTINUE))
    {
        return (ASTStatement*)lit_create_continue_statement(parser->state, parser->previous.line);
    }
    else if(prs_match(parser, LITTOK_BREAK))
    {
        return (ASTStatement*)lit_create_break_statement(parser->state, parser->previous.line);
    }
    else if(prs_match(parser, LITTOK_FUNCTION) || prs_match(parser, LITTOK_EXPORT))
    {
        return parse_function(parser);
    }
    else if(prs_match(parser, LITTOK_RETURN))
    {
        return parse_return(parser);
    }
    else if(prs_match(parser, LITTOK_LEFT_BRACE))
    {
        return parse_block(parser);
    }
    expression = parse_expression(parser);
    return expression == nullptr ? nullptr : (ASTStatement*)lit_create_expression_statement(parser->state, parser->previous.line, expression);
}

static ASTStatement* parse_declaration(LitParser* parser)
{
    ASTStatement* statement;
    statement = nullptr;
    if(prs_match(parser, LITTOK_CLASS) || prs_match(parser, LITTOK_STATIC))
    {
        statement = parse_class(parser);
    }
    else
    {
        statement = parse_statement(parser);
    }
    return statement;
}

bool lit_parse(LitParser* parser, const char* file_name, const char* source, PCGenericArray<ASTStatement*>& statements)
{
    LitCompiler compiler;
    ASTStatement* statement;
    parser->had_error = false;
    parser->panic_mode = false;
    lit_init_scanner(parser->state, parser->state->scanner, file_name, source);
    prs_init_compiler(parser, &compiler);
    prs_advance(parser);
    ignore_new_lines(parser);
    if(!prs_is_at_end(parser))
    {
        do
        {
            statement = parse_declaration(parser);
            if(statement != nullptr)
            {
                statements.push(statement);
            }
            if(!match_new_line(parser))
            {
                if(prs_match(parser, LITTOK_EOF))
                {
                    break;
                }
            }
        } while(!prs_is_at_end(parser));
    }
    return parser->had_error || parser->state->scanner->had_error;
}


void lit_add_definition(LitState* state, const char* name)
{
    state->preprocessor->defined.set(CONST_STRING(state, name), LitObject::TrueVal);
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

bool lit_preprocess(LitPreprocessor* preprocessor, char* source)
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
    LitState* state;
    LitValue tmp;
    LitString* arg;

    state = preprocessor->state;
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
            if(!lit_is_alpha(c) && !(((current - macro_start) > 1) && lit_is_digit(c)))
            {
                if(in_arg)
                {
                    arg = LitString::copy(state, arg_start, (int)(current - arg_start) - 1);
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
                                preprocessor->defined.set(arg, LitObject::TrueVal);
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

                            preprocessor->open_ifs.push((LitValue)macro_start);
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
                                preprocessor->open_ifs.m_values[preprocessor->open_ifs.m_count - 1] = (LitValue)macro_start;
                                ignore_depth = depth;
                            }
                        }
                    }
                    else
                    {
                        preprocessor->state->raiseError((LitErrorType)0,
                                  lit_format_error(preprocessor->state, 0, LitError::LITERROR_UNKNOWN_MACRO, (int)(current - macro_start) - 1, macro_start)
                                  ->chars);
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
        preprocessor->state->raiseError((LitErrorType)0, lit_format_error(preprocessor->state, 0, LitError::LITERROR_UNCLOSED_MACRO)->chars);
        return false;
    }
    preprocessor->open_ifs.release();
    return true;
}


void lit_init_scanner(LitState* state, LitScanner* scanner, const char* file_name, const char* source)
{
    scanner->line = 1;
    scanner->start = source;
    scanner->current = source;
    scanner->file_name = file_name;
    scanner->state = state;
    scanner->num_braces = 0;
    scanner->had_error = false;
}

static LitToken make_token(LitScanner* scanner, LitTokenType type)
{
    LitToken token;
    token.type = type;
    token.start = scanner->start;
    token.length = (size_t)(scanner->current - scanner->start);
    token.line = scanner->line;
    return token;
}

static LitToken make_error_token(LitScanner* scanner, LitError error, ...)
{
    va_list args;
    LitToken token;
    LitString* result;
    scanner->had_error = true;
    va_start(args, error);
    result = lit_vformat_error(scanner->state, scanner->line, error, args);
    va_end(args);
    token.type = LITTOK_ERROR;
    token.start = result->chars;
    token.length = result->length();
    token.line = scanner->line;
    return token;
}

static bool is_at_end(LitScanner* scanner)
{
    return *scanner->current == '\0';
}

static char advance(LitScanner* scanner)
{
    scanner->current++;
    return scanner->current[-1];
}

static bool match(LitScanner* scanner, char expected)
{
    if(is_at_end(scanner))
    {
        return false;
    }

    if(*scanner->current != expected)
    {
        return false;
    }
    scanner->current++;
    return true;
}

static LitToken match_token(LitScanner* scanner, char c, LitTokenType a, LitTokenType b)
{
    return make_token(scanner, match(scanner, c) ? a : b);
}

static LitToken match_tokens(LitScanner* scanner, char cr, char cb, LitTokenType a, LitTokenType b, LitTokenType c)
{
    return make_token(scanner, match(scanner, cr) ? a : (match(scanner, cb) ? b : c));
}

static char peek(LitScanner* scanner)
{
    return *scanner->current;
}

static char peek_next(LitScanner* scanner)
{
    if(is_at_end(scanner))
    {
        return '\0';
    }
    return scanner->current[1];
}

static bool skip_whitespace(LitScanner* scanner)
{
    char a;
    char b;
    char c;
    (void)a;
    (void)b;
    while(true)
    {
        c = peek(scanner);
        switch(c)
        {
            case ' ':
            case '\r':
            case '\t':
                {
                    advance(scanner);
                }
                break;
            case '\n':
                {
                    scanner->start = scanner->current;
                    advance(scanner);
                    return true;
                }
                break;
            case '/':
                {
                    if(peek_next(scanner) == '/')
                    {
                        while(peek(scanner) != '\n' && !is_at_end(scanner))
                        {
                            advance(scanner);
                        }
                        return skip_whitespace(scanner);
                    }
                    else if(peek_next(scanner) == '*')
                    {
                        advance(scanner);
                        advance(scanner);
                        a = peek(scanner);
                        b = peek_next(scanner);
                        while((peek(scanner) != '*' || peek_next(scanner) != '/') && !is_at_end(scanner))
                        {
                            if(peek(scanner) == '\n')
                            {
                                scanner->line++;
                            }
                            advance(scanner);
                        }
                        advance(scanner);
                        advance(scanner);
                        return skip_whitespace(scanner);
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

static LitToken scan_string(LitScanner* scanner, bool interpolation)
{
    char c;
    char newc;
    char nextc;
    int octval;
    LitState* state;
    PCGenericArray<uint8_t> bytes;
    LitToken token;
    LitTokenType string_type;
    state = scanner->state;
    string_type = LITTOK_STRING;
    bytes.init(scanner->state);
    while(true)
    {
        c = advance(scanner);
        if(c == '\"')
        {
            break;
        }
        else if(interpolation && c == '{')
        {
            if(scanner->num_braces >= LIT_MAX_INTERPOLATION_NESTING)
            {
                return make_error_token(scanner, LitError::LITERROR_INTERPOLATION_NESTING_TOO_DEEP, LIT_MAX_INTERPOLATION_NESTING);
            }
            string_type = LITTOK_INTERPOLATION;
            scanner->braces[scanner->num_braces++] = 1;
            break;
        }
        switch(c)
        {
            case '\0':
                {
                    return make_error_token(scanner, LitError::LITERROR_UNTERMINATED_STRING);
                }
                break;
            case '\n':
                {
                    scanner->line++;
                    bytes.push(c);
                }
                break;
            case '\\':
                {
                    switch(advance(scanner))
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
                                return make_error_token(scanner, LitError::LITERROR_INVALID_ESCAPE_CHAR, scanner->current[-1]);

                                fprintf(stderr, "scanner->current[-1] = '%c', c = '%c'\n", scanner->current[-1], c);
                                if(isdigit(scanner->current[-1]))
                                {
                                    //c*10 + (ls->current - '0')

                                    /*
                                    c = c*10 + (ls->current - '0');
                                    if (lj_char_isdigit(next(ls)))
                                    {
                                        c = c*10 + (ls->current - '0');
                                    */

                                    newc = scanner->current[-1] - '0';
                                    octval = 0;
                                    octval |= (newc * 8);
                                    while(true)
                                    {
                                        nextc = peek(scanner);
                                        fprintf(stderr, "nextc = '%c'\n", nextc);
                                        if(!isdigit(nextc))
                                        {
                                            break;
                                        }
                                        newc = advance(scanner);
                                        fprintf(stderr, "newc = '%c'\n", nextc);
                                        octval = octval << 3;
                                        octval |= ((newc - '0') * 8);
                                    }
                                    bytes.push(octval);
                                }
                                else
                                {
                                    return make_error_token(scanner, LitError::LITERROR_INVALID_ESCAPE_CHAR, scanner->current[-1]);
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
    token = make_token(scanner, string_type);
    token.value = LitString::copy(state, (const char*)bytes.m_values, bytes.m_count)->asValue();
    bytes.release();
    return token;
}

static int scan_hexdigit(LitScanner* scanner)
{
    char c;
    c = advance(scanner);
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
    scanner->current--;
    return -1;
}

static int scan_binarydigit(LitScanner* scanner)
{
    char c;
    c = advance(scanner);
    if(c >= '0' && c <= '1')
    {
        return c - '0';
    }
    scanner->current--;
    return -1;
}

static LitToken make_number_token(LitScanner* scanner, bool is_hex, bool is_binary)
{
    LitToken token;
    LitValue value;
    errno = 0;
    if(is_hex)
    {
        value = LitObject::toValue((double)strtoll(scanner->start, nullptr, 16));
    }
    else if(is_binary)
    {
        value = LitObject::toValue((int)strtoll(scanner->start + 2, nullptr, 2));
    }
    else
    {
        value = LitObject::toValue(strtod(scanner->start, nullptr));
    }

    if(errno == ERANGE)
    {
        errno = 0;
        return make_error_token(scanner, LitError::LITERROR_NUMBER_IS_TOO_BIG);
    }
    token = make_token(scanner, LITTOK_NUMBER);
    token.value = value;
    return token;
}

static LitToken scan_number(LitScanner* scanner)
{
    if(match(scanner, 'x'))
    {
        while(scan_hexdigit(scanner) != -1)
        {
            continue;
        }
        return make_number_token(scanner, true, false);
    }
    if(match(scanner, 'b'))
    {
        while(scan_binarydigit(scanner) != -1)
        {
            continue;
        }
        return make_number_token(scanner, false, true);
    }
    while(lit_is_digit(peek(scanner)))
    {
        advance(scanner);
    }
    // Look for a fractional part.
    if(peek(scanner) == '.' && lit_is_digit(peek_next(scanner)))
    {
        // Consume the '.'
        advance(scanner);
        while(lit_is_digit(peek(scanner)))
        {
            advance(scanner);
        }
    }
    return make_number_token(scanner, false, false);
}

static LitTokenType check_keyword(LitScanner* scanner, int start, int length, const char* rest, LitTokenType type)
{
    if(scanner->current - scanner->start == start + length && memcmp(scanner->start + start, rest, length) == 0)
    {
        return type;
    }
    return LITTOK_IDENTIFIER;
}

static LitTokenType scan_identtype(LitScanner* scanner)
{
    switch(scanner->start[0])
    {
        case 'b':
            return check_keyword(scanner, 1, 4, "reak", LITTOK_BREAK);

        case 'c':
            {
                if(scanner->current - scanner->start > 1)
                {
                    switch(scanner->start[1])
                    {
                        case 'l':
                            return check_keyword(scanner, 2, 3, "ass", LITTOK_CLASS);
                        case 'o':
                        {
                            if(scanner->current - scanner->start > 3)
                            {
                                switch(scanner->start[3])
                                {
                                    case 's':
                                        return check_keyword(scanner, 2, 3, "nst", LITTOK_CONST);
                                    case 't':
                                        return check_keyword(scanner, 2, 6, "ntinue", LITTOK_CONTINUE);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 'e':
            {
                if(scanner->current - scanner->start > 1)
                {
                    switch(scanner->start[1])
                    {
                        case 'l':
                            return check_keyword(scanner, 2, 2, "se", LITTOK_ELSE);
                        case 'x':
                            return check_keyword(scanner, 2, 4, "port", LITTOK_EXPORT);
                    }
                }
            }
            break;
        case 'f':
            {
                if(scanner->current - scanner->start > 1)
                {
                    switch(scanner->start[1])
                    {
                        case 'a':
                            return check_keyword(scanner, 2, 3, "lse", LITTOK_FALSE);
                        case 'o':
                            return check_keyword(scanner, 2, 1, "r", LITTOK_FOR);
                        case 'u':
                            return check_keyword(scanner, 2, 6, "nction", LITTOK_FUNCTION);
                    }
                }
            }
            break;
        case 'i':
            {
                if(scanner->current - scanner->start > 1)
                {
                    switch(scanner->start[1])
                    {
                        case 's':
                            return check_keyword(scanner, 2, 0, "", LITTOK_IS);
                        case 'f':
                            return check_keyword(scanner, 2, 0, "", LITTOK_IF);
                        case 'n':
                            return check_keyword(scanner, 2, 0, "", LITTOK_IN);
                    }
                }
            }
            break;
        case 'n':
        {
            if(scanner->current - scanner->start > 1)
            {
                switch(scanner->start[1])
                {
                    case 'u':
                        return check_keyword(scanner, 2, 2, "ll", LITTOK_NULL);
                    case 'e':
                        return check_keyword(scanner, 2, 1, "w", LITTOK_NEW);
                }
            }

            break;
        }

        case 'r':
        {
            if(scanner->current - scanner->start > 2)
            {
                switch(scanner->start[2])
                {
                    case 'f':
                        return check_keyword(scanner, 3, 0, "", LITTOK_REF);
                    case 't':
                        return check_keyword(scanner, 3, 3, "urn", LITTOK_RETURN);
                }
            }

            break;
        }

        case 'o':
            return check_keyword(scanner, 1, 7, "perator", LITTOK_OPERATOR);

        case 's':
        {
            if(scanner->current - scanner->start > 1)
            {
                switch(scanner->start[1])
                {
                    case 'u':
                        return check_keyword(scanner, 2, 3, "per", LITTOK_SUPER);
                    case 't':
                        return check_keyword(scanner, 2, 4, "atic", LITTOK_STATIC);
                    case 'e':
                        return check_keyword(scanner, 2, 1, "t", LITTOK_SET);
                }
            }

            break;
        }

        case 't':
        {
            if(scanner->current - scanner->start > 1)
            {
                switch(scanner->start[1])
                {
                    case 'h':
                        return check_keyword(scanner, 2, 2, "is", LITTOK_THIS);
                    case 'r':
                        return check_keyword(scanner, 2, 2, "ue", LITTOK_TRUE);
                }
            }

            break;
        }

        case 'v':
            return check_keyword(scanner, 1, 2, "ar", LITTOK_VAR);
        case 'w':
            return check_keyword(scanner, 1, 4, "hile", LITTOK_WHILE);
        case 'g':
            return check_keyword(scanner, 1, 2, "et", LITTOK_GET);
    }

    return LITTOK_IDENTIFIER;
}

static LitToken scan_identifier(LitScanner* scanner)
{
    while(lit_is_alpha(peek(scanner)) || lit_is_digit(peek(scanner)))
    {
        advance(scanner);
    }

    return make_token(scanner, scan_identtype(scanner));
}

LitToken lit_scan_token(LitScanner* scanner)
{
    if(skip_whitespace(scanner))
    {
        LitToken token = make_token(scanner, LITTOK_NEW_LINE);
        scanner->line++;

        return token;
    }

    scanner->start = scanner->current;

    if(is_at_end(scanner))
    {
        return make_token(scanner, LITTOK_EOF);
    }

    char c = advance(scanner);

    if(lit_is_digit(c))
    {
        return scan_number(scanner);
    }

    if(lit_is_alpha(c))
    {
        return scan_identifier(scanner);
    }

    switch(c)
    {
        case '(':
            return make_token(scanner, LITTOK_LEFT_PAREN);
        case ')':
            return make_token(scanner, LITTOK_RIGHT_PAREN);

        case '{':
        {
            if(scanner->num_braces > 0)
            {
                scanner->braces[scanner->num_braces - 1]++;
            }

            return make_token(scanner, LITTOK_LEFT_BRACE);
        }

        case '}':
        {
            if(scanner->num_braces > 0 && --scanner->braces[scanner->num_braces - 1] == 0)
            {
                scanner->num_braces--;
                return scan_string(scanner, true);
            }

            return make_token(scanner, LITTOK_RIGHT_BRACE);
        }

        case '[':
            return make_token(scanner, LITTOK_LEFT_BRACKET);
        case ']':
            return make_token(scanner, LITTOK_RIGHT_BRACKET);
        case ';':
            return make_token(scanner, LITTOK_SEMICOLON);
        case ',':
            return make_token(scanner, LITTOK_COMMA);
        case ':':
            return make_token(scanner, LITTOK_COLON);
        case '~':
            return make_token(scanner, LITTOK_TILDE);

        case '+':
            return match_tokens(scanner, '=', '+', LITTOK_PLUS_EQUAL, LITTOK_PLUS_PLUS, LITTOK_PLUS);
        case '-':
            return match(scanner, '>') ? make_token(scanner, LITTOK_SMALL_ARROW) :
                                         match_tokens(scanner, '=', '-', LITTOK_MINUS_EQUAL, LITTOK_MINUS_MINUS, LITTOK_MINUS);
        case '/':
            return match_token(scanner, '=', LITTOK_SLASH_EQUAL, LITTOK_SLASH);
        case '#':
            return match_token(scanner, '=', LITTOK_SHARP_EQUAL, LITTOK_SHARP);
        case '!':
            return match_token(scanner, '=', LITTOK_BANG_EQUAL, LITTOK_BANG);
        case '?':
            return match_token(scanner, '?', LITTOK_QUESTION_QUESTION, LITTOK_QUESTION);
        case '%':
            return match_token(scanner, '=', LITTOK_PERCENT_EQUAL, LITTOK_PERCENT);
        case '^':
            return match_token(scanner, '=', LITTOK_CARET_EQUAL, LITTOK_CARET);

        case '>':
            return match_tokens(scanner, '=', '>', LITTOK_GREATER_EQUAL, LITTOK_GREATER_GREATER, LITTOK_GREATER);
        case '<':
            return match_tokens(scanner, '=', '<', LITTOK_LESS_EQUAL, LITTOK_LESS_LESS, LITTOK_LESS);
        case '*':
            return match_tokens(scanner, '=', '*', LITTOK_STAR_EQUAL, LITTOK_STAR_STAR, LITTOK_STAR);
        case '=':
            return match_tokens(scanner, '=', '>', LITTOK_EQUAL_EQUAL, LITTOK_ARROW, LITTOK_EQUAL);
        case '|':
            return match_tokens(scanner, '=', '|', LITTOK_BAR_EQUAL, LITTOK_BAR_BAR, LITTOK_BAR);
        case '&':
            return match_tokens(scanner, '=', '&', LITTOK_AMPERSAND_EQUAL, LITTOK_AMPERSAND_AMPERSAND, LITTOK_AMPERSAND);


        case '.':
        {
            if(!match(scanner, '.'))
            {
                return make_token(scanner, LITTOK_DOT);
            }

            return match_token(scanner, '.', LITTOK_DOT_DOT_DOT, LITTOK_DOT_DOT);
        }

        case '$':
        {
            if(!match(scanner, '\"'))
            {
                return make_error_token(scanner, LitError::LITERROR_CHAR_EXPECTATION_UNMET, '\"', '$', peek(scanner));
            }

            return scan_string(scanner, true);
        }

        case '"':
            return scan_string(scanner, false);
    }

    return make_error_token(scanner, LitError::LITERROR_UNEXPECTED_CHAR, c);
}

#define PUSH(value) (*fiber->stack_top++ = value)

#define RETURN_OK(r) return LitInterpretResult{ LITRESULT_OK, r };

#define RETURN_RUNTIME_ERROR() return LitInterpretResult{ LITRESULT_RUNTIME_ERROR, LitObject::NullVal };




LitValue lit_instance_get_method(LitState* state, LitValue callee, LitString* mthname)
{
    LitValue mthval;
    LitClass* klass;
    klass = state->getClassFor(callee);
    if((LitObject::isInstance(callee) && LitObject::as<LitInstance>(callee)->fields.get(mthname, &mthval)) || klass->methods.get(mthname, &mthval))
    {
        return mthval;
    }
    return LitObject::NullVal;
}

LitInterpretResult lit_instance_call_method(LitState* state, LitValue callee, LitString* mthname, LitValue* argv, size_t argc)
{
    LitValue mthval;
    mthval = lit_instance_get_method(state, callee, mthname);
    if(!LitObject::isNull(mthval))
    {
        return lit_call(state, mthval, argv, argc, false);
    }
    return INTERPRET_RUNTIME_FAIL;    
}


double lit_check_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isNumber(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a number as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }
    return LitObject::toNumber(args[id]);
}

double lit_get_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, double def)
{
    (void)vm;
    if(arg_count <= id || !LitObject::isNumber(args[id]))
    {
        return def;
    }
    return LitObject::toNumber(args[id]);
}

bool lit_check_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isBool(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a boolean as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return lit_as_bool(args[id]);
}

bool lit_get_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, bool def)
{
    (void)vm;
    if(arg_count <= id || !LitObject::isBool(args[id]))
    {
        return def;
    }
    return lit_as_bool(args[id]);
}

const char* lit_check_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isString(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return LitObject::as<LitString>(args[id])->chars;
}

const char* lit_get_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, const char* def)
{
    (void)vm;
    if(arg_count <= id || !LitObject::isString(args[id]))
    {
        return def;
    }

    return LitObject::as<LitString>(args[id])->chars;
}

LitString* lit_check_object_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isString(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return LitObject::as<LitString>(args[id]);
}

LitInstance* lit_check_instance(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isInstance(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected an instance as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return LitObject::as<LitInstance>(args[id]);
}

LitValue* lit_check_reference(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !LitObject::isReference(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a reference as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return AS_REFERENCE(args[id])->slot;
}

void lit_ensure_bool(LitVM* vm, LitValue value, const char* error)
{
    if(!LitObject::isBool(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_string(LitVM* vm, LitValue value, const char* error)
{
    if(!LitObject::isString(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_number(LitVM* vm, LitValue value, const char* error)
{
    if(!LitObject::isNumber(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_object_type(LitVM* vm, LitValue value, LitObject::Type type, const char* error)
{
    if(!LitObject::isObject(value) || OBJECT_TYPE(value) != type)
    {
        lit_runtime_error_exiting(vm, error);
    }
}




static bool ensure_fiber(LitVM* vm, LitFiber* fiber)
{
    size_t newcapacity;
    size_t osize;
    size_t newsize;
    if(fiber == nullptr)
    {
        lit_runtime_error(vm, "no fiber to run on");
        return true;
    }
    if(fiber->frame_count == LIT_CALL_FRAMES_MAX)
    {
        lit_runtime_error(vm, "fiber frame overflow");
        return true;
    }
    if(fiber->frame_count + 1 > fiber->frame_capacity)
    {
        //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->frame_capacity * 2);
        newcapacity = (fiber->frame_capacity * 2) + 1;
        osize = (sizeof(LitCallFrame) * fiber->frame_capacity);
        newsize = (sizeof(LitCallFrame) * newcapacity);
        fiber->frames = (LitCallFrame*)lit_reallocate(vm->state, fiber->frames, osize, newsize);
        fiber->frame_capacity = newcapacity;
    }

    return false;
}

static inline LitCallFrame* setup_call(LitState* state, LitFunction* callee, LitValue* argv, uint8_t argc, bool ignfiber)
{
    bool vararg;
    int amount;
    size_t i;
    size_t varargc;
    size_t function_arg_count;
    LitVM* vm;
    LitFiber* fiber;
    LitCallFrame* frame;
    LitArray* array;
    (void)argc;
    (void)varargc;
    vm = state->vm;
    fiber = vm->fiber;
    if(callee == nullptr)
    {
        lit_runtime_error(vm, "attempt to call a null value");
        return nullptr;
    }
    if(ignfiber)
    {
        if(fiber == nullptr)
        {
            fiber = state->api_fiber;
        }
    }
    if(!ignfiber)
    {
        if(ensure_fiber(vm, fiber))
        {
            return nullptr;
        }        
    }
    lit_ensure_fiber_stack(state, fiber, callee->max_slots + (int)(fiber->stack_top - fiber->stack));
    frame = &fiber->frames[fiber->frame_count++];
    frame->slots = fiber->stack_top;
    PUSH(callee->asValue());
    for(i = 0; i < argc; i++)
    {
        PUSH(argv[i]);
    }
    function_arg_count = callee->arg_count;
    if(argc != function_arg_count)
    {
        vararg = callee->vararg;
        if(argc < function_arg_count)
        {
            amount = (int)function_arg_count - argc - (vararg ? 1 : 0);
            for(i = 0; i < (size_t)amount; i++)
            {
                PUSH(LitObject::NullVal);
            }
            if(vararg)
            {
                PUSH(LitArray::make(vm->state)->asValue());
            }
        }
        else if(callee->vararg)
        {
            array = LitArray::make(vm->state);
            varargc = argc - function_arg_count + 1;
            array->values.reserve(varargc + 1, LitObject::NullVal);
            for(i = 0; i < varargc; i++)
            {
                array->values.m_values[i] = fiber->stack_top[(int)i - (int)varargc];
            }

            fiber->stack_top -= varargc;
            lit_push(vm, array->asValue());
        }
        else
        {
            fiber->stack_top -= (argc - function_arg_count);
        }
    }
    else if(callee->vararg)
    {
        array = LitArray::make(vm->state);
        varargc = argc - function_arg_count + 1;
        array->values.push(*(fiber->stack_top - 1));
        *(fiber->stack_top - 1) = array->asValue();
    }
    frame->ip = callee->chunk.code;
    frame->closure = nullptr;
    frame->function = callee;
    frame->result_ignored = false;
    frame->return_to_c = true;
    return frame;
}

static inline LitInterpretResult execute_call(LitState* state, LitCallFrame* frame)
{
    LitFiber* fiber;
    LitInterpretResult result;
    if(frame == nullptr)
    {
        RETURN_RUNTIME_ERROR();
    }
    fiber = state->vm->fiber;
    result = lit_interpret_fiber(state, fiber);
    if(fiber->error != LitObject::NullVal)
    {
        result.result = fiber->error;
    }
    return result;
}

LitInterpretResult lit_call_function(LitState* state, LitFunction* callee, LitValue* argv, uint8_t argc, bool ignfiber)
{
    return execute_call(state, setup_call(state, callee, argv, argc, ignfiber));
}

LitInterpretResult lit_call_closure(LitState* state, LitClosure* callee, LitValue* argv, uint8_t argc, bool ignfiber)
{
    LitCallFrame* frame;
    frame = setup_call(state, callee->function, argv, argc, ignfiber);
    if(frame == nullptr)
    {
        RETURN_RUNTIME_ERROR();
    }
    frame->closure = callee;
    return execute_call(state, frame);
}

LitInterpretResult lit_call_method(LitState* state, LitValue instance, LitValue callee, LitValue* argv, uint8_t argc, bool ignfiber)
{
    uint8_t i;
    LitVM* vm;
    LitInterpretResult lir;
    LitObject::Type type;
    LitClass* klass;
    LitFiber* fiber;
    LitValue* slot;
    LitNativeMethod* natmethod;
    LitBoundMethod* bound_method;
    LitValue mthval;
    LitValue result;
    lir.result = LitObject::NullVal;
    lir.type = LITRESULT_OK;
    vm = state->vm;
    if(LitObject::isObject(callee))
    {
        if(lit_set_native_exit_jump())
        {
            RETURN_RUNTIME_ERROR();
        }
        type = OBJECT_TYPE(callee);

        if(type == LitObject::Type::Function)
        {
            return lit_call_function(state, LitObject::as<LitFunction>(callee), argv, argc, ignfiber);
        }
        else if(type == LitObject::Type::Closure)
        {
            return lit_call_closure(state, LitObject::as<LitClosure>(callee), argv, argc, ignfiber);
        }
        fiber = vm->fiber;
        if(ignfiber)
        {
            if(fiber == nullptr)
            {
                fiber = state->api_fiber;
            }
        }
        if(!ignfiber)
        {
            if(ensure_fiber(vm, fiber))
            {
                RETURN_RUNTIME_ERROR();
            }
        }
        lit_ensure_fiber_stack(state, fiber, 3 + argc + (int)(fiber->stack_top - fiber->stack));
        slot = fiber->stack_top;
        PUSH(instance);
        if(type != LitObject::Type::Class)
        {
            for(i = 0; i < argc; i++)
            {
                PUSH(argv[i]);
            }
        }
        switch(type)
        {
            case LitObject::Type::NativeFunction:
                {
                    LitValue result = LitObject::as<LitNativeFunction>(callee)->function(vm, argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(result);
                }
                break;
            case LitObject::Type::NativePrimitive:
                {
                    LitObject::as<LitNativePrimFunction>(callee)->function(vm, argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(LitObject::NullVal);
                }
                break;
            case LitObject::Type::NativeMethod:
                {
                    natmethod = LitObject::as<LitNativeMethod>(callee);
                    result = natmethod->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(result);
                }
                break;
            case LitObject::Type::Class:
                {
                    klass = AS_CLASS(callee);
                    *slot = LitInstance::make(vm->state, klass)->asValue();
                    if(klass->init_method != nullptr)
                    {
                        lir = lit_call_method(state, *slot, klass->init_method->asValue(), argv, argc, ignfiber);
                    }
                    // TODO: when should this return *slot instead of lir?
                    fiber->stack_top = slot;
                    //RETURN_OK(*slot);
                    return lir;
                }
                break;
            case LitObject::Type::BoundMethod:
                {
                    bound_method = AS_BOUND_METHOD(callee);
                    mthval = bound_method->method;
                    *slot = bound_method->receiver;
                    if(LitObject::isNativeMethod(mthval))
                    {
                        result = LitObject::as<LitNativeMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(result);
                    }
                    else if(LitObject::isPrimitiveMethod(mthval))
                    {
                        LitObject::as<LitPrimitiveMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);

                        fiber->stack_top = slot;
                        RETURN_OK(LitObject::NullVal);
                    }
                    else
                    {
                        fiber->stack_top = slot;
                        return lit_call_function(state, LitObject::as<LitFunction>(mthval), argv, argc, ignfiber);
                    }
                }
                break;
            case LitObject::Type::PrimitiveMethod:
                {
                    LitObject::as<LitPrimitiveMethod>(callee)->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(LitObject::NullVal);
                }
                break;
            default:
                {
                }
                break;
        }
    }
    if(LitObject::isNull(callee))
    {
        lit_runtime_error(vm, "attempt to call a null value");
    }
    else
    {
        lit_runtime_error(vm, "can only call functions and classes");
    }

    RETURN_RUNTIME_ERROR();
}

LitInterpretResult lit_call(LitState* state, LitValue callee, LitValue* argv, uint8_t argc, bool ignfiber)
{
    return lit_call_method(state, callee, callee, argv, argc, ignfiber);
}

LitInterpretResult lit_find_and_call_method(LitState* state, LitValue callee, LitString* method_name, LitValue* argv, uint8_t argc, bool ignfiber)
{
    LitClass* klass;
    LitVM* vm;
    LitFiber* fiber;
    LitValue mthval;
    vm = state->vm;
    fiber = vm->fiber;
    if(fiber == nullptr)
    {
        if(!ignfiber)
        {
            lit_runtime_error(vm, "no fiber to run on");
            RETURN_RUNTIME_ERROR();
        }
    }
    klass = state->getClassFor(callee);
    if((LitObject::isInstance(callee) && LitObject::as<LitInstance>(callee)->fields.get(method_name, &mthval)) || klass->methods.get(method_name, &mthval))
    {
        return lit_call_method(state, callee, mthval, argv, argc, ignfiber);
    }
    return LitInterpretResult{ LITRESULT_INVALID, LitObject::NullVal };
}

LitString* lit_to_string(LitState* state, LitValue valobj)
{
    LitValue* slot;
    LitVM* vm;
    LitFiber* fiber;
    LitFunction* function;
    LitChunk* chunk;
    LitCallFrame* frame;
    LitInterpretResult result;
    if(LitObject::isString(valobj))
    {
        return LitObject::as<LitString>(valobj);
    }
    else if(!LitObject::isObject(valobj))
    {
        if(LitObject::isNull(valobj))
        {
            return CONST_STRING(state, "null");
        }
        else if(LitObject::isNumber(valobj))
        {
            return LitObject::as<LitString>(LitString::stringNumberToString(state, LitObject::toNumber(valobj)));
        }
        else if(LitObject::isBool(valobj))
        {
            return CONST_STRING(state, lit_as_bool(valobj) ? "true" : "false");
        }
    }
    else if(LitObject::isReference(valobj))
    {
        slot = AS_REFERENCE(valobj)->slot;

        if(slot == nullptr)
        {
            return CONST_STRING(state, "null");
        }
        return lit_to_string(state, *slot);
    }
    vm = state->vm;
    fiber = vm->fiber;
    if(ensure_fiber(vm, fiber))
    {
        return CONST_STRING(state, "null");
    }
    function = state->api_function;
    if(function == nullptr)
    {
        function = state->api_function = LitFunction::make(state, fiber->module);
        function->chunk.has_line_info = false;
        function->name = state->api_name;
        chunk = &function->chunk;
        chunk->m_count = 0;
        chunk->constants.m_count = 0;
        function->max_slots = 3;
        chunk->write_chunk(OP_INVOKE, 1);
        chunk->emit_byte(0);
        chunk->emit_short(chunk->add_constant(OBJECT_CONST_STRING(state, "toString")));
        chunk->emit_byte(OP_RETURN);
    }
    lit_ensure_fiber_stack(state, fiber, function->max_slots + (int)(fiber->stack_top - fiber->stack));
    frame = &fiber->frames[fiber->frame_count++];
    frame->ip = function->chunk.code;
    frame->closure = nullptr;
    frame->function = function;
    frame->slots = fiber->stack_top;
    frame->result_ignored = false;
    frame->return_to_c = true;
    PUSH(function->asValue());
    PUSH(valobj);
    result = lit_interpret_fiber(state, fiber);
    if(result.type != LITRESULT_OK)
    {
        return CONST_STRING(state, "null");
    }
    return LitObject::as<LitString>(result.result);
}

LitValue lit_call_new(LitVM* vm, const char* name, LitValue* args, size_t argc, bool ignfiber)
{
    LitValue value;
    LitClass* klass;
    if(!vm->globals->values.get(CONST_STRING(vm->state, name), &value))
    {
        lit_runtime_error(vm, "failed to create instance of class %s: class not found", name);
        return LitObject::NullVal;
    }
    klass = AS_CLASS(value);
    if(klass->init_method == nullptr)
    {
        return LitInstance::make(vm->state, klass)->asValue();
    }
    return lit_call_method(vm->state, value, value, args, argc, ignfiber).result;
}




static size_t print_simple_op(LitState* state, LitWriter* wr, const char* name, size_t offset)
{
    (void)state;
    wr->format("%s%s%s\n", COLOR_YELLOW, name, COLOR_RESET);
    return offset + 1;
}

static size_t print_constant_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset, bool big)
{
    uint8_t constant;
    if(big)
    {
        constant = (uint16_t)(chunk->code[offset + 1] << 8);
        constant |= chunk->code[offset + 2];
    }
    else
    {
        constant = chunk->code[offset + 1];
    }
    wr->format("%s%-16s%s %4d '", COLOR_YELLOW, name, COLOR_RESET, constant);
    lit_print_value(state, wr, chunk->constants.m_values[constant]);
    wr->format("'\n");
    return offset + (big ? 3 : 2);
}

static size_t print_byte_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset)
{
    uint8_t slot;
    (void)state;
    slot = chunk->code[offset + 1];
    wr->format("%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
    return offset + 2;
}

static size_t print_short_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset)
{
    uint16_t slot;
    (void)state;
    slot = (uint16_t)(chunk->code[offset + 1] << 8);
    slot |= chunk->code[offset + 2];
    wr->format("%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
    return offset + 2;
}

static size_t print_jump_op(LitState* state, LitWriter* wr, const char* name, int sign, LitChunk* chunk, size_t offset)
{
    uint16_t jump;
    (void)state;
    jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    wr->format("%s%-16s%s %4d -> %d\n", COLOR_YELLOW, name, COLOR_RESET, (int)offset, (int)(offset + 3 + sign * jump));
    return offset + 3;
}

static size_t print_invoke_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset)
{
    uint8_t arg_count;
    uint8_t constant;
    (void)state;
    arg_count = chunk->code[offset + 1];
    constant = chunk->code[offset + 2];
    constant |= chunk->code[offset + 3];
    wr->format("%s%-16s%s (%d args) %4d '", COLOR_YELLOW, name, COLOR_RESET, arg_count, constant);
    lit_print_value(state, wr, chunk->constants.m_values[constant]);
    wr->format("'\n");
    return offset + 4;
}

size_t lit_disassemble_instruction(LitState* state, LitChunk* chunk, size_t offset, const char* source)
{
    bool same;
    int is_local;
    size_t j;
    size_t line;
    size_t index;
    int16_t constant;
    uint8_t instruction;
    char c;
    char* next_line;
    char* prev_line;
    char* output_line;
    char* current_line;
    LitWriter* wr;
    LitFunction* function;
    wr = &state->debugwriter;
    line = chunk->get_line(offset);
    same = !chunk->has_line_info || (offset > 0 && line == chunk->get_line(offset - 1));
    if(!same && source != nullptr)
    {
        index = 0;
        current_line = (char*)source;
        while(current_line)
        {
            next_line = strchr(current_line, '\n');
            prev_line = current_line;
            index++;
            current_line = next_line ? (next_line + 1) : nullptr;
            if(index == line)
            {
                output_line = prev_line ? prev_line : next_line;
                while((c = *output_line) && (c == '\t' || c == ' '))
                {
                    output_line++;
                }
                wr->format("%s        %.*s%s\n", COLOR_RED, next_line ? (int)(next_line - output_line) : (int)strlen(prev_line), output_line, COLOR_RESET);
                break;
            }
        }
    }
    wr->format("%04d ", (int)offset);
    if(same)
    {
        wr->put("   | ");
    }
    else
    {
        wr->format("%s%4d%s ", COLOR_BLUE, (int)line, COLOR_RESET);
    }
    instruction = chunk->code[offset];
    switch(instruction)
    {
        case OP_POP:
            return print_simple_op(state, wr, "OP_POP", offset);
        case OP_POP_LOCALS:
            return print_constant_op(state, wr, "OP_POP_LOCALS", chunk, offset, true);
        case OP_RETURN:
            return print_simple_op(state, wr, "OP_RETURN", offset);
        case OP_CONSTANT:
            return print_constant_op(state, wr, "OP_CONSTANT", chunk, offset, false);
        case OP_CONSTANT_LONG:
            return print_constant_op(state, wr, "OP_CONSTANT_LONG", chunk, offset, true);
        case OP_TRUE:
            return print_simple_op(state, wr, "OP_TRUE", offset);
        case OP_FALSE:
            return print_simple_op(state, wr, "OP_FALSE", offset);
        case OP_NULL:
            return print_simple_op(state, wr, "OP_NULL", offset);
        case OP_NEGATE:
            return print_simple_op(state, wr, "OP_NEGATE", offset);
        case OP_NOT:
            return print_simple_op(state, wr, "OP_NOT", offset);
        case OP_ADD:
            return print_simple_op(state, wr, "OP_ADD", offset);
        case OP_SUBTRACT:
            return print_simple_op(state, wr, "OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return print_simple_op(state, wr, "OP_MULTIPLY", offset);
        case OP_POWER:
            return print_simple_op(state, wr, "OP_POWER", offset);
        case OP_DIVIDE:
            return print_simple_op(state, wr, "OP_DIVIDE", offset);
        case OP_FLOOR_DIVIDE:
            return print_simple_op(state, wr, "OP_FLOOR_DIVIDE", offset);
        case OP_MOD:
            return print_simple_op(state, wr, "OP_MOD", offset);
        case OP_BAND:
            return print_simple_op(state, wr, "OP_BAND", offset);
        case OP_BOR:
            return print_simple_op(state, wr, "OP_BOR", offset);
        case OP_BXOR:
            return print_simple_op(state, wr, "OP_BXOR", offset);
        case OP_LSHIFT:
            return print_simple_op(state, wr, "OP_LSHIFT", offset);
        case OP_RSHIFT:
            return print_simple_op(state, wr, "OP_RSHIFT", offset);
        case OP_BNOT:
            return print_simple_op(state, wr, "OP_BNOT", offset);
        case OP_EQUAL:
            return print_simple_op(state, wr, "OP_EQUAL", offset);
        case OP_GREATER:
            return print_simple_op(state, wr, "OP_GREATER", offset);
        case OP_GREATER_EQUAL:
            return print_simple_op(state, wr, "OP_GREATER_EQUAL", offset);
        case OP_LESS:
            return print_simple_op(state, wr, "OP_LESS", offset);
        case OP_LESS_EQUAL:
            return print_simple_op(state, wr, "OP_LESS_EQUAL", offset);
        case OP_SET_GLOBAL:
            return print_constant_op(state, wr, "OP_SET_GLOBAL", chunk, offset, true);
        case OP_GET_GLOBAL:
            return print_constant_op(state, wr, "OP_GET_GLOBAL", chunk, offset, true);
        case OP_SET_LOCAL:
            return print_byte_op(state, wr, "OP_SET_LOCAL", chunk, offset);
        case OP_GET_LOCAL:
            return print_byte_op(state, wr, "OP_GET_LOCAL", chunk, offset);
        case OP_SET_LOCAL_LONG:
            return print_short_op(state, wr, "OP_SET_LOCAL_LONG", chunk, offset);
        case OP_GET_LOCAL_LONG:
            return print_short_op(state, wr, "OP_GET_LOCAL_LONG", chunk, offset);
        case OP_SET_PRIVATE:
            return print_byte_op(state, wr, "OP_SET_PRIVATE", chunk, offset);
        case OP_GET_PRIVATE:
            return print_byte_op(state, wr, "OP_GET_PRIVATE", chunk, offset);
        case OP_SET_PRIVATE_LONG:
            return print_short_op(state, wr, "OP_SET_PRIVATE_LONG", chunk, offset);
        case OP_GET_PRIVATE_LONG:
            return print_short_op(state, wr, "OP_GET_PRIVATE_LONG", chunk, offset);
        case OP_SET_UPVALUE:
            return print_byte_op(state, wr, "OP_SET_UPVALUE", chunk, offset);
        case OP_GET_UPVALUE:
            return print_byte_op(state, wr, "OP_GET_UPVALUE", chunk, offset);
        case OP_JUMP_IF_FALSE:
            return print_jump_op(state, wr, "OP_JUMP_IF_FALSE", 1, chunk, offset);
        case OP_JUMP_IF_NULL:
            return print_jump_op(state, wr, "OP_JUMP_IF_NULL", 1, chunk, offset);
        case OP_JUMP_IF_NULL_POPPING:
            return print_jump_op(state, wr, "OP_JUMP_IF_NULL_POPPING", 1, chunk, offset);
        case OP_JUMP:
            return print_jump_op(state, wr, "OP_JUMP", 1, chunk, offset);
        case OP_JUMP_BACK:
            return print_jump_op(state, wr, "OP_JUMP_BACK", -1, chunk, offset);
        case OP_AND:
            return print_jump_op(state, wr, "OP_AND", 1, chunk, offset);
        case OP_OR:
            return print_jump_op(state, wr, "OP_OR", 1, chunk, offset);
        case OP_NULL_OR:
            return print_jump_op(state, wr, "OP_NULL_OR", 1, chunk, offset);
        case OP_CALL:
            return print_byte_op(state, wr, "OP_CALL", chunk, offset);
        case OP_CLOSURE:
            {
                offset++;
                constant = (uint16_t)(chunk->code[offset] << 8);
                offset++;
                constant |= chunk->code[offset];
                wr->format("%-16s %4d ", "OP_CLOSURE", constant);
                lit_print_value(state, wr, chunk->constants.m_values[constant]);
                wr->format("\n");
                function = LitObject::as<LitFunction>(chunk->constants.m_values[constant]);
                for(j = 0; j < function->upvalue_count; j++)
                {
                    is_local = chunk->code[offset++];
                    index = chunk->code[offset++];
                    wr->format("%04d      |                     %s %d\n", (int)(offset - 2), is_local ? "local" : "upvalue", (int)index);
                }
                return offset;
            }
            break;
        case OP_CLOSE_UPVALUE:
            return print_simple_op(state, wr, "OP_CLOSE_UPVALUE", offset);
        case OP_CLASS:
            return print_constant_op(state, wr, "OP_CLASS", chunk, offset, true);

        case OP_GET_FIELD:
            return print_simple_op(state, wr, "OP_GET_FIELD", offset);
        case OP_SET_FIELD:
            return print_simple_op(state, wr, "OP_SET_FIELD", offset);

        case OP_SUBSCRIPT_GET:
            return print_simple_op(state, wr, "OP_SUBSCRIPT_GET", offset);
        case OP_SUBSCRIPT_SET:
            return print_simple_op(state, wr, "OP_SUBSCRIPT_SET", offset);
        case OP_ARRAY:
            return print_simple_op(state, wr, "OP_ARRAY", offset);
        case OP_PUSH_ARRAY_ELEMENT:
            return print_simple_op(state, wr, "OP_PUSH_ARRAY_ELEMENT", offset);
        case OP_OBJECT:
            return print_simple_op(state, wr, "OP_OBJECT", offset);
        case OP_PUSH_OBJECT_FIELD:
            return print_simple_op(state, wr, "OP_PUSH_OBJECT_FIELD", offset);
        case OP_RANGE:
            return print_simple_op(state, wr, "OP_RANGE", offset);
        case OP_METHOD:
            return print_constant_op(state, wr, "OP_METHOD", chunk, offset, true);
        case OP_STATIC_FIELD:
            return print_constant_op(state, wr, "OP_STATIC_FIELD", chunk, offset, true);
        case OP_DEFINE_FIELD:
            return print_constant_op(state, wr, "OP_DEFINE_FIELD", chunk, offset, true);
        case OP_INVOKE:
            return print_invoke_op(state, wr, "OP_INVOKE", chunk, offset);
        case OP_INVOKE_SUPER:
            return print_invoke_op(state, wr, "OP_INVOKE_SUPER", chunk, offset);
        case OP_INVOKE_IGNORING:
            return print_invoke_op(state, wr, "OP_INVOKE_IGNORING", chunk, offset);
        case OP_INVOKE_SUPER_IGNORING:
            return print_invoke_op(state, wr, "OP_INVOKE_SUPER_IGNORING", chunk, offset);
        case OP_INHERIT:
            return print_simple_op(state, wr, "OP_INHERIT", offset);
        case OP_IS:
            return print_simple_op(state, wr, "OP_IS", offset);
        case OP_GET_SUPER_METHOD:
            return print_constant_op(state, wr, "OP_GET_SUPER_METHOD", chunk, offset, true);
        case OP_VARARG:
            return print_byte_op(state, wr, "OP_VARARG", chunk, offset);
        case OP_REFERENCE_FIELD:
            return print_simple_op(state, wr, "OP_REFERENCE_FIELD", offset);
        case OP_REFERENCE_UPVALUE:
            return print_byte_op(state, wr, "OP_REFERENCE_UPVALUE", chunk, offset);
        case OP_REFERENCE_PRIVATE:
            return print_short_op(state, wr, "OP_REFERENCE_PRIVATE", chunk, offset);
        case OP_REFERENCE_LOCAL:
            return print_short_op(state, wr, "OP_REFERENCE_LOCAL", chunk, offset);
        case OP_REFERENCE_GLOBAL:
            return print_constant_op(state, wr, "OP_REFERENCE_GLOBAL", chunk, offset, true);
        case OP_SET_REFERENCE:
            return print_simple_op(state, wr, "OP_SET_REFERENCE", offset);
        default:
            {
                wr->format("Unknown opcode %d\n", instruction);
                return offset + 1;
            }
            break;
    }
}

void lit_trace_frame(LitFiber* fiber, LitWriter* wr)
{
    LitCallFrame* frame;
    (void)fiber;
    (void)frame;
    (void)wr;
#ifdef LIT_TRACE_STACK
    if(fiber == nullptr)
    {
        return;
    }
    frame = &fiber->frames[fiber->frame_count - 1];
    wr->format("== fiber %p f%i %s (expects %i, max %i, added %i, current %i, exits %i) ==\n", fiber,
           fiber->frame_count - 1, frame->function->name->chars, frame->function->arg_count, frame->function->max_slots,
           frame->function->max_slots + (int)(fiber->stack_top - fiber->stack), fiber->stack_capacity, frame->return_to_c);
#endif
}

static const char* errorCodeToString(LitError eid)
{
    switch(eid)
    {
        case LitError::LITERROR_UNCLOSED_MACRO:
            return "unclosed macro.";
        case LitError::LITERROR_UNKNOWN_MACRO:
            return "unknown macro '%.*s'.";
        case LitError::LITERROR_UNEXPECTED_CHAR:
            return "unexpected character '%c'";
        case LitError::LITERROR_UNTERMINATED_STRING:
            return "unterminated string";
        case LitError::LITERROR_INVALID_ESCAPE_CHAR:
            return "invalid escape character '%c'";
        case LitError::LITERROR_INTERPOLATION_NESTING_TOO_DEEP:
            return "interpolation nesting is too deep, maximum is %i";
        case LitError::LITERROR_NUMBER_IS_TOO_BIG:
            return "number is too big to be represented by a single literal";
        case LitError::LITERROR_CHAR_EXPECTATION_UNMET:
            return "expected '%c' after '%c', got '%c'";
        case LitError::LITERROR_EXPECTATION_UNMET:
            return "expected %s, got '%.*s'";
        case LitError::LITERROR_INVALID_ASSIGMENT_TARGET:
            return "invalid assigment target";
        case LitError::LITERROR_TOO_MANY_FUNCTION_ARGS:
            return "function cannot have more than 255 arguments, got %i";
        case LitError::LITERROR_MULTIPLE_ELSE_BRANCHES:
            return "if-statement can have only one else-branch";
        case LitError::LITERROR_VAR_MISSING_IN_FORIN:
            return "for-loops using in-iteration must declare a new variable";
        case LitError::LITERROR_NO_GETTER_AND_SETTER:
            return "expected declaration of either getter or setter, got none";
        case LitError::LITERROR_STATIC_OPERATOR:
            return "operator methods cannot be static or defined in static classes";
        case LitError::LITERROR_SELF_INHERITED_CLASS:
            return "class cannot inherit itself";
        case LitError::LITERROR_STATIC_FIELDS_AFTER_METHODS:
            return "all static fields must be defined before the methods";
        case LitError::LITERROR_MISSING_STATEMENT:
            return "expected statement but got nothing";
        case LitError::LITERROR_EXPECTED_EXPRESSION:
            return "expected expression after '%.*s', got '%.*s'";
        case LitError::LITERROR_DEFAULT_ARG_CENTRED:
            return "default arguments must always be in the end of the argument list.";
        case LitError::LITERROR_TOO_MANY_CONSTANTS:
            return "too many constants for one chunk";
        case LitError::LITERROR_TOO_MANY_PRIVATES:
            return "too many private locals for one module";
        case LitError::LITERROR_VAR_REDEFINED:
            return "variable '%.*s' was already declared in this scope";
        case LitError::LITERROR_TOO_MANY_LOCALS:
            return "too many local variables for one function";
        case LitError::LITERROR_TOO_MANY_UPVALUES:
            return "too many upvalues for one function";
        case LitError::LITERROR_VARIABLE_USED_IN_INIT:
            return "variable '%.*s' cannot use itself in its initializer";
        case LitError::LITERROR_JUMP_TOO_BIG:
            return "too much code to jump over";
        case LitError::LITERROR_NO_SUPER:
            return "'super' cannot be used in class '%s', because it does not have a super class";
        case LitError::LITERROR_THIS_MISSUSE:
            return "'this' cannot be used %s";
        case LitError::LITERROR_SUPER_MISSUSE:
            return "'super' cannot be used %s";
        case LitError::LITERROR_UNKNOWN_EXPRESSION:
            return "unknown expression with id '%i'";
        case LitError::LITERROR_UNKNOWN_STATEMENT:
            return "unknown statement with id '%i'";
        case LitError::LITERROR_LOOP_JUMP_MISSUSE:
            return "cannot use '%s' outside of loops";
        case LitError::LITERROR_RETURN_FROM_CONSTRUCTOR:
            return "cannot use 'return' in constructors";
        case LitError::LITERROR_STATIC_CONSTRUCTOR:
            return "constructors cannot be static (at least for now)";
        case LitError::LITERROR_CONSTANT_MODIFIED:
            return "attempt to modify constant '%.*s'";
        case LitError::LITERROR_INVALID_REFERENCE_TARGET:
            return "invalid refence target";
        default:
            break;
    }
    return "(nothing)";
}

LitString* lit_vformat_error(LitState* state, size_t line, LitError error_id, va_list args)
{
    size_t buffer_size;
    char* buffer;
    const char* error_message;
    LitString* rt;
    va_list args_copy;
    error_message = errorCodeToString(error_id);
    va_copy(args_copy, args);
    buffer_size = vsnprintf(nullptr, 0, error_message, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, error_message, args);
    buffer[buffer_size - 1] = '\0';
    if(line != 0)
    {
        rt = LitString::format(state, "[err # line #]: $", (double)error_id, (double)line, (const char*)buffer);
    }
    else
    {
        rt = LitString::format(state, "[err #]: $", (double)error_id, (const char*)buffer);
    }
    free(buffer);
    return rt;
}

LitString* lit_format_error(LitState* state, size_t line, LitError error, ...)
{
    va_list args;
    LitString* result;
    va_start(args, error);
    result = lit_vformat_error(state, line, error, args);
    va_end(args);

    return result;
}
#define _XOPEN_SOURCE 700

#if defined (S_IFDIR) && !defined (S_ISDIR)
    #define	S_ISDIR(m)	(((m)&S_IFMT) == S_IFDIR)	/* directory */
#endif
#if defined (S_IFREG) && !defined (S_ISREG)
    #define	S_ISREG(m)	(((m)&S_IFMT) == S_IFREG)	/* file */
#endif


static uint8_t btmp;
static uint16_t stmp;
static uint32_t itmp;
static double dtmp;

char* lit_read_file(const char* path)
{
    FILE* file;
    file = fopen(path, "rb");
    if(file == nullptr)
    {
        return nullptr;
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(fileSize + 1);
    size_t bytes_read = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    return buffer;
}

bool lit_file_exists(const char* path)
{
    struct stat buffer;
    return stat(path, &buffer) == 0 && S_ISREG(buffer.st_mode);
}

bool lit_dir_exists(const char* path)
{
    struct stat buffer;
    return stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode);
}

size_t lit_write_uint8_t(FILE* file, uint8_t byte)
{
    return fwrite(&byte, sizeof(uint8_t), 1, file);
}

size_t lit_write_uint16_t(FILE* file, uint16_t byte)
{
    return fwrite(&byte, sizeof(uint16_t), 1, file);
}

size_t lit_write_uint32_t(FILE* file, uint32_t byte)
{
    return fwrite(&byte, sizeof(uint32_t), 1, file);
}

size_t lit_write_double(FILE* file, double byte)
{
    return fwrite(&byte, sizeof(double), 1, file);
}

size_t lit_write_string(FILE* file, LitString* string)
{
    uint16_t i;
    uint16_t c;
    size_t rt;
    c = string->length();
    rt = fwrite(&c, 2, 1, file);
    for(i = 0; i < c; i++)
    {
        lit_write_uint8_t(file, (uint8_t)string->chars[i] ^ LIT_STRING_KEY);
    }
    return (rt + i);
}

uint8_t lit_read_uint8_t(FILE* file)
{
    size_t rt;
    (void)rt;
    rt = fread(&btmp, sizeof(uint8_t), 1, file);
    return btmp;
}

uint16_t lit_read_uint16_t(FILE* file)
{
    size_t rt;
    (void)rt;
    rt = fread(&stmp, sizeof(uint16_t), 1, file);
    return stmp;
}

uint32_t lit_read_uint32_t(FILE* file)
{
    size_t rt;
    (void)rt;
    rt = fread(&itmp, sizeof(uint32_t), 1, file);
    return itmp;
}

double lit_read_double(FILE* file)
{
    size_t rt;
    (void)rt;
    rt = fread(&dtmp, sizeof(double), 1, file);
    return dtmp;
}

LitString* lit_read_string(LitState* state, FILE* file)
{
    size_t rt;
    uint16_t i;
    uint16_t length;
    char* line;
    (void)rt;
    rt = fread(&length, 2, 1, file);
    if(length < 1)
    {
        return nullptr;
    }
    line = (char*)malloc(length + 1);
    for(i = 0; i < length; i++)
    {
        line[i] = (char)lit_read_uint8_t(file) ^ LIT_STRING_KEY;
    }
    return LitString::take(state, line, length, false);
}

void lit_init_emulated_file(LitEmulatedFile* file, const char* source)
{
    file->source = source;
    file->position = 0;
}

uint8_t lit_read_euint8_t(LitEmulatedFile* file)
{
    return (uint8_t)file->source[file->position++];
}

uint16_t lit_read_euint16_t(LitEmulatedFile* file)
{
    return (uint16_t)(lit_read_euint8_t(file) | (lit_read_euint8_t(file) << 8u));
}

uint32_t lit_read_euint32_t(LitEmulatedFile* file)
{
    return (uint32_t)(
        lit_read_euint8_t(file) |
        (lit_read_euint8_t(file) << 8u) |
        (lit_read_euint8_t(file) << 16u) |
        (lit_read_euint8_t(file) << 24u)
    );
}

double lit_read_edouble(LitEmulatedFile* file)
{
    size_t i;
    double result;
    uint8_t values[8];
    for(i = 0; i < 8; i++)
    {
        values[i] = lit_read_euint8_t(file);
    }
    memcpy(&result, values, 8);
    return result;
}

LitString* lit_read_estring(LitState* state, LitEmulatedFile* file)
{
    uint16_t i;
    uint16_t length;
    char* line;
    length = lit_read_euint16_t(file);
    if(length < 1)
    {
        return nullptr;
    }
    line = (char*)malloc(length + 1);
    for(i = 0; i < length; i++)
    {
        line[i] = (char)lit_read_euint8_t(file) ^ LIT_STRING_KEY;
    }
    return LitString::take(state, line, length, false);
}

static void save_chunk(FILE* file, LitChunk* chunk);
static void load_chunk(LitState* state, LitEmulatedFile* file, LitModule* module, LitChunk* chunk);

static void save_function(FILE* file, LitFunction* function)
{
    save_chunk(file, &function->chunk);
    lit_write_string(file, function->name);
    lit_write_uint8_t(file, function->arg_count);
    lit_write_uint16_t(file, function->upvalue_count);
    lit_write_uint8_t(file, (uint8_t)function->vararg);
    lit_write_uint16_t(file, (uint16_t)function->max_slots);
}

static LitFunction* load_function(LitState* state, LitEmulatedFile* file, LitModule* module)
{
    LitFunction* function = LitFunction::make(state, module);

    load_chunk(state, file, module, &function->chunk);
    function->name = lit_read_estring(state, file);

    function->arg_count = lit_read_euint8_t(file);
    function->upvalue_count = lit_read_euint16_t(file);
    function->vararg = (bool)lit_read_euint8_t(file);
    function->max_slots = lit_read_euint16_t(file);

    return function;
}

static void save_chunk(FILE* file, LitChunk* chunk)
{
    lit_write_uint32_t(file, chunk->m_count);

    for(size_t i = 0; i < chunk->m_count; i++)
    {
        lit_write_uint8_t(file, chunk->code[i]);
    }

    if(chunk->has_line_info)
    {
        size_t c = chunk->line_count * 2 + 2;
        lit_write_uint32_t(file, c);

        for(size_t i = 0; i < c; i++)
        {
            lit_write_uint16_t(file, chunk->lines[i]);
        }
    }
    else
    {
        lit_write_uint32_t(file, 0);
    }

    lit_write_uint32_t(file, chunk->constants.m_count);

    for(size_t i = 0; i < chunk->constants.m_count; i++)
    {
        LitValue constant = chunk->constants.m_values[i];

        if(LitObject::isObject(constant))
        {
            LitObject::Type type = LitObject::asObject(constant)->type;
            lit_write_uint8_t(file, (uint8_t)(type) + 1);

            switch(type)
            {
                case LitObject::Type::String:
                {
                    lit_write_string(file, LitObject::as<LitString>(constant));
                    break;
                }

                case LitObject::Type::Function:
                {
                    save_function(file, LitObject::as<LitFunction>(constant));
                    break;
                }

                default:
                {
                    UNREACHABLE
                    break;
                }
            }
        }
        else
        {
            lit_write_uint8_t(file, 0);
            lit_write_double(file, LitObject::toNumber(constant));
        }
    }
}

static void load_chunk(LitState* state, LitEmulatedFile* file, LitModule* module, LitChunk* chunk)
{
    size_t i;
    size_t count;
    uint8_t type;
    chunk->init(state);
    count = lit_read_euint32_t(file);
    chunk->code = (uint8_t*)lit_reallocate(state, nullptr, 0, sizeof(uint8_t) * count);
    chunk->m_count = count;
    chunk->m_capacity = count;
    for(i = 0; i < count; i++)
    {
        chunk->code[i] = lit_read_euint8_t(file);
    }
    count = lit_read_euint32_t(file);
    if(count > 0)
    {
        chunk->lines = (uint16_t*)lit_reallocate(state, nullptr, 0, sizeof(uint16_t) * count);
        chunk->line_count = count;
        chunk->line_capacity = count;
        for(i = 0; i < count; i++)
        {
            chunk->lines[i] = lit_read_euint16_t(file);
        }
    }
    else
    {
        chunk->has_line_info = false;
    }
    count = lit_read_euint32_t(file);
    chunk->constants.m_values = (LitValue*)lit_reallocate(state, nullptr, 0, sizeof(LitValue) * count);
    chunk->constants.m_count = count;
    chunk->constants.m_capacity = count;
    for(i = 0; i < count; i++)
    {
        type = lit_read_euint8_t(file);
        if(type == 0)
        {
            chunk->constants.m_values[i] = LitObject::toValue(lit_read_edouble(file));
        }
        else
        {
            switch((LitObject::Type)(type - 1))
            {
                case LitObject::Type::String:
                    {
                        chunk->constants.m_values[i] = lit_read_estring(state, file)->asValue();
                    }
                    break;
                case LitObject::Type::Function:
                    {
                        chunk->constants.m_values[i] = load_function(state, file, module)->asValue();
                    }
                    break;
                default:
                    {
                        UNREACHABLE
                    }
                    break;

            }
        }
    }
}

void lit_save_module(LitModule* module, FILE* file)
{
    size_t i;
    bool disabled;
    LitTable* privates;
    disabled = lit_is_optimization_enabled(LITOPTSTATE_PRIVATE_NAMES);
    lit_write_string(file, module->name);
    lit_write_uint16_t(file, module->private_count);
    lit_write_uint8_t(file, (uint8_t)disabled);
    if(!disabled)
    {
        privates = &module->private_names->values;
        for(i = 0; i < module->private_count; i++)
        {
            if(privates->m_values[i].key != nullptr)
            {
                lit_write_string(file, privates->m_values[i].key);
                lit_write_uint16_t(file, (uint16_t)LitObject::toNumber(privates->m_values[i].value));
            }
        }
    }
    save_function(file, module->main_function);
}

LitModule* lit_load_module(LitState* state, const char* input)
{
    bool enabled;
    uint16_t i;
    uint16_t j;
    uint16_t module_count;
    uint16_t privates_count;
    uint8_t bytecode_version;
    LitString* name;
    LitTable* privates;
    LitModule* module;
    LitEmulatedFile file;
    lit_init_emulated_file(&file, input);
    if(lit_read_euint16_t(&file) != LIT_BYTECODE_MAGIC_NUMBER)
    {
        state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown magic number");
        return nullptr;
    }
    bytecode_version = lit_read_euint8_t(&file);
    if(bytecode_version > LIT_BYTECODE_VERSION)
    {
        state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown bytecode version '%i'", (int)bytecode_version);
        return nullptr;
    }
    module_count = lit_read_euint16_t(&file);
    LitModule* first = nullptr;
    for(j = 0; j < module_count; j++)
    {
        module = lit_create_module(state, lit_read_estring(state, &file));
        privates = &module->private_names->values;
        privates_count = lit_read_euint16_t(&file);
        enabled = !((bool)lit_read_euint8_t(&file));
        module->privates = LIT_ALLOCATE(state, LitValue, privates_count);
        module->private_count = privates_count;
        for(i = 0; i < privates_count; i++)
        {
            module->privates[i] = LitObject::NullVal;
            if(enabled)
            {
                name = lit_read_estring(state, &file);
                privates->set(name, LitObject::toValue(lit_read_euint16_t(&file)));
            }
        }
        module->main_function = load_function(state, &file, module);
        state->vm->modules->values.set(module->name, module->asValue());
        if(j == 0)
        {
            first = module;
        }
    }
    if(lit_read_euint16_t(&file) != LIT_BYTECODE_END_NUMBER)
    {
        state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown end number");
        return nullptr;
    }
    return first;
}

void* lit_reallocate(LitState* state, void* pointer, size_t old_size, size_t new_size)
{
    void* ptr;
    state->bytes_allocated += (int64_t)new_size - (int64_t)old_size;
    if(new_size > old_size)
    {
#ifdef LIT_STRESS_TEST_GC
        state->vm->collectGarbage();
#endif
        if(state->bytes_allocated > state->next_gc)
        {
            state->vm->collectGarbage();
        }
    }
    if(new_size == 0)
    {
        free(pointer);
        return nullptr;
    }
    ptr = (void*)realloc(pointer, new_size);
    if(ptr == nullptr)
    {
        state->raiseError(RUNTIME_ERROR, "Fatal error:\nOut of memory\nProgram terminated");
        exit(111);
    }
    return ptr;
}



// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
int lit_closest_power_of_two(int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

bool lit_is_callable_function(LitValue value)
{
    if(LitObject::isObject(value))
    {
        LitObject::Type type = OBJECT_TYPE(value);
        return (
            (type == LitObject::Type::Closure) ||
            (type == LitObject::Type::Function) ||
            (type == LitObject::Type::NativeFunction) ||
            (type == LitObject::Type::NativePrimitive) ||
            (type == LitObject::Type::NativeMethod) ||
            (type == LitObject::Type::PrimitiveMethod) ||
            (type == LitObject::Type::BoundMethod)
        );
    }

    return false;
}

LitValue lit_get_function_name(LitVM* vm, LitValue instance)
{
    LitString* name;
    LitField* field;
    name = nullptr;
    switch(OBJECT_TYPE(instance))
    {
        case LitObject::Type::Function:
            {
                name = LitObject::as<LitFunction>(instance)->name;
            }
            break;
        case LitObject::Type::Closure:
            {
                name = LitObject::as<LitClosure>(instance)->function->name;
            }
            break;
        case LitObject::Type::Field:
            {
                field = AS_FIELD(instance);
                if(field->getter != nullptr)
                {
                    return lit_get_function_name(vm, field->getter->asValue());
                }
                return lit_get_function_name(vm, field->setter->asValue());
            }
            break;
        case LitObject::Type::NativePrimitive:
            {
                name = LitObject::as<LitNativePrimFunction>(instance)->name;
            }
            break;
        case LitObject::Type::NativeFunction:
            {
                name = LitObject::as<LitNativeFunction>(instance)->name;
            }
            break;
        case LitObject::Type::NativeMethod:
            {
                name = LitObject::as<LitNativeMethod>(instance)->name;
            }
            break;
        case LitObject::Type::PrimitiveMethod:
            {
                name = LitObject::as<LitPrimitiveMethod>(instance)->name;
            }
            break;
        case LitObject::Type::BoundMethod:
            {
                return lit_get_function_name(vm, AS_BOUND_METHOD(instance)->method);
            }
            break;
        default:
            {
            }
            break;
    }
    if(name == nullptr)
    {
        return LitString::format(vm->state, "function #", *((double*)LitObject::asObject(instance)))->asValue();
    }

    return LitString::format(vm->state, "function @", name->asValue())->asValue();
}

LitUpvalue* lit_create_upvalue(LitState* state, LitValue* slot)
{
    LitUpvalue* upvalue;
    upvalue = LitObject::make<LitUpvalue>(state, LitObject::Type::Upvalue);
    upvalue->location = slot;
    upvalue->closed = LitObject::NullVal;
    upvalue->next = nullptr;
    return upvalue;
}

LitClosure* lit_create_closure(LitState* state, LitFunction* function)
{
    size_t i;
    LitClosure* closure;
    LitUpvalue** upvalues;
    closure = LitObject::make<LitClosure>(state, LitObject::Type::Closure);
    state->pushRoot((LitObject*)closure);
    upvalues = LIT_ALLOCATE(state, LitUpvalue*, function->upvalue_count);
    state->popRoot();
    for(i = 0; i < function->upvalue_count; i++)
    {
        upvalues[i] = nullptr;
    }
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}


LitFiber* lit_create_fiber(LitState* state, LitModule* module, LitFunction* function)
{
    size_t stack_capacity;
    LitValue* stack;
    LitCallFrame* frame;
    LitCallFrame* frames;
    LitFiber* fiber;
    // Allocate in advance, just in case GC is triggered
    stack_capacity = function == nullptr ? 1 : (size_t)lit_closest_power_of_two(function->max_slots + 1);
    stack = LIT_ALLOCATE(state, LitValue, stack_capacity);
    frames = LIT_ALLOCATE(state, LitCallFrame, LIT_INITIAL_CALL_FRAMES);
    fiber = LitObject::make<LitFiber>(state, LitObject::Type::Fiber);
    if(module != nullptr)
    {
        if(module->main_fiber == nullptr)
        {
            module->main_fiber = fiber;
        }
    }
    fiber->stack = stack;
    fiber->stack_capacity = stack_capacity;
    fiber->stack_top = fiber->stack;
    fiber->frames = frames;
    fiber->frame_capacity = LIT_INITIAL_CALL_FRAMES;
    fiber->parent = nullptr;
    fiber->frame_count = 1;
    fiber->arg_count = 0;
    fiber->module = module;
    fiber->catcher = false;
    fiber->error = LitObject::NullVal;
    fiber->open_upvalues = nullptr;
    fiber->abort = false;
    frame = &fiber->frames[0];
    frame->closure = nullptr;
    frame->function = function;
    frame->slots = fiber->stack;
    frame->result_ignored = false;
    frame->return_to_c = false;
    if(function != nullptr)
    {
        frame->ip = function->chunk.code;
    }
    return fiber;
}

void lit_ensure_fiber_stack(LitState* state, LitFiber* fiber, size_t needed)
{
    size_t i;
    size_t capacity;
    LitValue* old_stack;
    LitUpvalue* upvalue;
    if(fiber->stack_capacity >= needed)
    {
        return;
    }
    capacity = (size_t)lit_closest_power_of_two((int)needed);
    old_stack = fiber->stack;
    fiber->stack = (LitValue*)lit_reallocate(state, fiber->stack, sizeof(LitValue) * fiber->stack_capacity, sizeof(LitValue) * capacity);
    fiber->stack_capacity = capacity;
    if(fiber->stack != old_stack)
    {
        for(i = 0; i < fiber->frame_capacity; i++)
        {
            LitCallFrame* frame = &fiber->frames[i];
            frame->slots = fiber->stack + (frame->slots - old_stack);
        }
        for(upvalue = fiber->open_upvalues; upvalue != nullptr; upvalue = upvalue->next)
        {
            upvalue->location = fiber->stack + (upvalue->location - old_stack);
        }
        fiber->stack_top = fiber->stack + (fiber->stack_top - old_stack);
    }
}

LitModule* lit_create_module(LitState* state, LitString* name)
{
    LitModule* module;
    module = LitObject::make<LitModule>(state, LitObject::Type::Module);
    module->name = name;
    module->return_value = LitObject::NullVal;
    module->main_function = nullptr;
    module->privates = nullptr;
    module->ran = false;
    module->main_fiber = nullptr;
    module->private_count = 0;
    module->private_names = LitMap::make(state);
    return module;
}

LitBoundMethod* lit_create_bound_method(LitState* state, LitValue receiver, LitValue method)
{
    LitBoundMethod* bound_method;
    bound_method = LitObject::make<LitBoundMethod>(state, LitObject::Type::BoundMethod);
    bound_method->receiver = receiver;
    bound_method->method = method;
    return bound_method;
}



LitUserdata* lit_create_userdata(LitState* state, size_t size, bool ispointeronly)
{
    LitUserdata* userdata;
    userdata = LitObject::make<LitUserdata>(state, LitObject::Type::Userdata);
    userdata->data = nullptr;
    if(size > 0)
    {
        if(!ispointeronly)
        {
            userdata->data = lit_reallocate(state, nullptr, 0, size);
        }
    }
    userdata->size = size;
    userdata->cleanup_fn = nullptr;
    userdata->canfree = true;
    return userdata;
}

LitRange* lit_create_range(LitState* state, double from, double to)
{
    LitRange* range;
    range = LitObject::make<LitRange>(state, LitObject::Type::Range);
    range->from = from;
    range->to = to;
    return range;
}

LitReference* lit_create_reference(LitState* state, LitValue* slot)
{
    LitReference* reference;
    reference = LitObject::make<LitReference>(state, LitObject::Type::Reference);
    reference->slot = slot;
    return reference;
}


static bool measure_compilation_time;
static double last_source_time = 0;

void lit_enable_compilation_time_measurement()
{
    measure_compilation_time = true;
}




static void free_statements(LitState* state, PCGenericArray<ASTStatement*>* statements)
{
    size_t i;
    for(i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }
    statements->release();
}

LitInterpretResult lit_interpret(LitState* state, const char* module_name, char* code)
{
    return lit_internal_interpret(state, LitString::copy(state, module_name, strlen(module_name)), code);
}

LitModule* lit_compile_module(LitState* state, LitString* module_name, char* code)
{
    clock_t t;
    clock_t total_t;
    bool allowed_gc;
    LitModule* module;
    PCGenericArray<ASTStatement*> statements;
    allowed_gc = state->allow_gc;
    state->allow_gc = false;
    state->had_error = false;
    module = nullptr;
    // This is a lbc format
    if((code[1] << 8 | code[0]) == LIT_BYTECODE_MAGIC_NUMBER)
    {
        module = lit_load_module(state, code);
    }
    else
    {
        t = 0;
        total_t = 0;
        if(measure_compilation_time)
        {
            total_t = t = clock();
        }
        if(!lit_preprocess(state->preprocessor, code))
        {
            return nullptr;
        }
        if(measure_compilation_time)
        {
            printf("-----------------------\nPreprocessing:  %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            t = clock();
        }
        statements.init(state);
        if(lit_parse(state->parser, module_name->chars, code, statements))
        {
            free_statements(state, &statements);
            return nullptr;
        }
        //if(measure_compilation_time)
        {
            printf("Parsing:        %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            t = clock();
        }
        lit_optimize(state->optimizer, &statements);
        //if(measure_compilation_time)
        {
            printf("Optimization:   %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            t = clock();
        }
        module = state->emitter->run_emitter(statements, module_name);
        free_statements(state, &statements);
        //if(measure_compilation_time)
        {
            printf("Emitting:       %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            printf("\nTotal:          %gms\n-----------------------\n",
                   (double)(clock() - total_t) / CLOCKS_PER_SEC * 1000 + last_source_time);
        }
    }
    state->allow_gc = allowed_gc;
    return state->had_error ? nullptr : module;
}

LitModule* lit_get_module(LitState* state, const char* name)
{
    LitValue value;
    if(state->vm->modules->values.get(CONST_STRING(state, name), &value))
    {
        return LitObject::as<LitModule>(value);
    }
    return nullptr;
}

LitInterpretResult lit_internal_interpret(LitState* state, LitString* module_name, char* code)
{
    intptr_t istack;
    intptr_t itop;
    intptr_t idif;
    LitModule* module;
    LitFiber* fiber;
    LitInterpretResult result;
    module = lit_compile_module(state, module_name, code);
    if(module == nullptr)
    {
        return LitInterpretResult{ LITRESULT_COMPILE_ERROR, LitObject::NullVal };
    }
    result = lit_interpret_module(state, module);
    fiber = module->main_fiber;
    if(!state->had_error && !fiber->abort && fiber->stack_top != fiber->stack)
    {
        istack = (intptr_t)(fiber->stack);
        itop = (intptr_t)(fiber->stack_top);
        idif = (intptr_t)(fiber->stack - fiber->stack_top);
        /* me fail english. how do i put this better? */
        state->raiseError(RUNTIME_ERROR, "stack should be same as stack top", idif, istack, istack, itop, itop);
    }
    state->last_module = module;
    return result;
}

char* lit_patch_file_name(char* file_name)
{
    int i;
    int name_length;
    char c;
    name_length = strlen(file_name);
    // Check, if our file_name ends with .lit or lbc, and remove it
    if(name_length > 4 && (memcmp(file_name + name_length - 4, ".lit", 4) == 0 || memcmp(file_name + name_length - 4, ".lbc", 4) == 0))
    {
        file_name[name_length - 4] = '\0';
        name_length -= 4;
    }
    // Check, if our file_name starts with ./ and remove it (useless, and makes the module name be ..main)
    if(name_length > 2 && memcmp(file_name, "./", 2) == 0)
    {
        file_name += 2;
        name_length -= 2;
    }
    for(i = 0; i < name_length; i++)
    {
        c = file_name[i];
        if(c == '/' || c == '\\')
        {
            file_name[i] = '.';
        }
    }
    return file_name;
}

char* copy_string(const char* string)
{
    size_t length;
    char* new_string;
    length = strlen(string) + 1;
    new_string = (char*)malloc(length);
    memcpy(new_string, string, length);
    return new_string;
}

bool lit_compile_and_save_files(LitState* state, char* files[], size_t num_files, const char* output_file)
{
    size_t i;
    char* file_name;
    char* source;
    FILE* file;
    LitString* module_name;
    LitModule* module;
    LitModule** compiled_modules;
    compiled_modules = LIT_ALLOCATE(state, LitModule*, num_files+1);
    lit_set_optimization_level(LITOPTLEVEL_EXTREME);
    for(i = 0; i < num_files; i++)
    {
        file_name = copy_string(files[i]);
        source = lit_read_file(file_name);
        if(source == nullptr)
        {
            state->raiseError(COMPILE_ERROR, "failed to open file '%s' for reading", file_name);
            return false;
        }
        file_name = lit_patch_file_name(file_name);
        module_name = LitString::copy(state, file_name, strlen(file_name));
        module = lit_compile_module(state, module_name, source);
        compiled_modules[i] = module;
        free((void*)source);
        free((void*)file_name);
        if(module == nullptr)
        {
            return false;
        }
    }
    file = fopen(output_file, "w+b");
    if(file == nullptr)
    {
        state->raiseError(COMPILE_ERROR, "failed to open file '%s' for writing", output_file);
        return false;
    }
    lit_write_uint16_t(file, LIT_BYTECODE_MAGIC_NUMBER);
    lit_write_uint8_t(file, LIT_BYTECODE_VERSION);
    lit_write_uint16_t(file, num_files);
    for(i = 0; i < num_files; i++)
    {
        lit_save_module(compiled_modules[i], file);
    }
    lit_write_uint16_t(file, LIT_BYTECODE_END_NUMBER);
    LIT_FREE(state, LitModule, compiled_modules);
    fclose(file);
    return true;
}

static char* read_source(LitState* state, const char* file, char** patched_file_name)
{
    clock_t t;
    char* file_name;
    char* source;
    t = 0;
    if(measure_compilation_time)
    {
        t = clock();
    }
    file_name = copy_string(file);
    source = lit_read_file(file_name);
    if(source == nullptr)
    {
        state->raiseError(RUNTIME_ERROR, "failed to open file '%s' for reading", file_name);
    }
    file_name = lit_patch_file_name(file_name);
    if(measure_compilation_time)
    {
        printf("reading source: %gms\n", last_source_time = (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
    }
    *patched_file_name = file_name;
    return source;
}

LitInterpretResult lit_interpret_file(LitState* state, const char* file)
{
    char* source;
    char* patched_file_name;
    LitInterpretResult result;
    source = read_source(state, file, &patched_file_name);
    if(source == nullptr)
    {
        return INTERPRET_RUNTIME_FAIL;
    }
    result = lit_interpret(state, patched_file_name, source);
    free((void*)source);
    free(patched_file_name);
    return result;
}


void lit_disassemble_chunk(LitChunk* chunk, const char* name, const char* source)
{
    size_t i;
    size_t offset;
    LitValue value;
    PCGenericArray<LitValue>* values;
    LitFunction* function;
    values = &chunk->constants;

    for(i = 0; i < values->m_count; i++)
    {
        value = values->m_values[i];
        if(LitObject::isFunction(value))
        {
            function = LitObject::as<LitFunction>(value);
            lit_disassemble_chunk(&function->chunk, function->name->chars, source);
        }
    }
    chunk->state->debugwriter.format("== %s ==\n", name);
    for(offset = 0; offset < chunk->m_count;)
    {
        offset = lit_disassemble_instruction(chunk->state, chunk, offset, source);
    }
}

void lit_disassemble_module(LitState* state, LitModule* module, const char* source)
{
    lit_disassemble_chunk(&module->main_function->chunk, module->main_function->name->chars, source);
}


LitInterpretResult lit_dump_file(LitState* state, const char* file)
{
    char* patched_file_name;
    char* source;
    LitInterpretResult result;
    LitString* module_name;
    LitModule* module;
    source = read_source(state, file, &patched_file_name);
    if(source == nullptr)
    {
        return INTERPRET_RUNTIME_FAIL;
    }
    module_name = LitString::copy(state, patched_file_name, strlen(patched_file_name));
    module = lit_compile_module(state, module_name, source);
    if(module == nullptr)
    {
        result = INTERPRET_RUNTIME_FAIL;
    }
    else
    {
        lit_disassemble_module(state, module, source);
        result = (LitInterpretResult){ LITRESULT_OK, LitObject::NullVal };
    }
    free((void*)source);
    free((void*)patched_file_name);
    return result;
}



void lit_printf(LitState* state, const char* message, ...)
{
    size_t buffer_size;
    char* buffer;
    va_list args;
    va_start(args, message);
    va_list args_copy;
    va_copy(args_copy, args);
    buffer_size = vsnprintf(nullptr, 0, message, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, message, args);
    va_end(args);
    state->print_fn(state, buffer);
    free(buffer);
}





static const char* lit_object_type_names[] =
{
    "string",
    "function",
    "native_function",
    "native_primitive",
    "native_method",
    "primitive_method",
    "fiber",
    "module",
    "closure",
    "upvalue",
    "class",
    "instance",
    "bound_method",
    "array",
    "map",
    "userdata",
    "range",
    "field",
    "reference"
};



static void print_array(LitState* state, LitWriter* wr, LitArray* array, size_t size)
{
    size_t i;
    wr->format("(%u) [", (unsigned int)size);
    if(size > 0)
    {
        wr->put(" ");
        for(i = 0; i < size; i++)
        {
            if(LitObject::isArray(array->values.m_values[i]) && (array == LitObject::as<LitArray>(array->values.m_values[i])))
            {
                wr->put("(recursion)");
            }
            else
            {
                lit_print_value(state, wr, array->values.m_values[i]);
            }
            if(i + 1 < size)
            {
                wr->put(", ");
            }
            else
            {
                wr->put(" ");
            }
        }
    }
    wr->put("]");
}

static void print_map(LitState* state, LitWriter* wr, LitMap* map, size_t size)
{
    bool had_before;
    size_t i;
    LitTableEntry* entry;
    wr->format("(%u) {", (unsigned int)size);
    had_before = false;
    if(size > 0)
    {
        for(i = 0; i < (size_t)map->values.m_capacity; i++)
        {
            entry = &map->values.m_values[i];
            if(entry->key != nullptr)
            {
                if(had_before)
                {
                    wr->put(", ");
                }
                else
                {
                    wr->put(" ");
                }
                wr->format("%s = ", entry->key->chars);
                if(LitObject::isMap(entry->value) && (map == AS_MAP(entry->value)))
                {
                    wr->put("(recursion)");
                }
                else
                {
                    lit_print_value(state, wr, entry->value);
                }
                had_before = true;
            }
        }
    }
    if(had_before)
    {
        wr->put(" }");
    }
    else
    {
        wr->put("}");
    }
}

static void print_object(LitState* state, LitWriter* wr, LitValue value)
{
    size_t size;
    LitMap* map;
    LitArray* array;
    LitRange* range;
    LitValue* slot;
    LitObject* obj;
    LitUpvalue* upvalue;
    obj = LitObject::asObject(value);
    if(obj != nullptr)
    {
        switch(obj->type)
        {
            case LitObject::Type::String:
                {
                    wr->format("%s", lit_as_cstring(value));
                }
                break;
            case LitObject::Type::Function:
                {
                    wr->format("function %s", LitObject::as<LitFunction>(value)->name->chars);
                }
                break;
            case LitObject::Type::Closure:
                {
                    wr->format("closure %s", LitObject::as<LitClosure>(value)->function->name->chars);
                }
                break;
            case LitObject::Type::NativePrimitive:
                {
                    wr->format("function %s", LitObject::as<LitNativePrimFunction>(value)->name->chars);
                }
                break;
            case LitObject::Type::NativeFunction:
                {
                    wr->format("function %s", LitObject::as<LitNativeFunction>(value)->name->chars);
                }
                break;
            case LitObject::Type::PrimitiveMethod:
                {
                    wr->format("function %s", LitObject::as<LitPrimitiveMethod>(value)->name->chars);
                }
                break;
            case LitObject::Type::NativeMethod:
                {
                    wr->format("function %s", LitObject::as<LitNativeMethod>(value)->name->chars);
                }
                break;
            case LitObject::Type::Fiber:
                {
                    wr->format("fiber");
                }
                break;
            case LitObject::Type::Module:
                {
                    wr->format("module %s", LitObject::as<LitModule>(value)->name->chars);
                }
                break;

            case LitObject::Type::Upvalue:
                {
                    upvalue = AS_UPVALUE(value);
                    if(upvalue->location == nullptr)
                    {
                        lit_print_value(state, wr, upvalue->closed);
                    }
                    else
                    {
                        print_object(state, wr, *upvalue->location);
                    }
                }
                break;
            case LitObject::Type::Class:
                {
                    wr->format("class %s", AS_CLASS(value)->name->chars);
                }
                break;
            case LitObject::Type::Instance:
                {
                    /*
                    if(LitObject::as<LitInstance>(value)->klass->type == LitObject::Type::Map)
                    {
                        fprintf(stderr, "instance is a map\n");
                    }
                    printf("%s instance", LitObject::as<LitInstance>(value)->klass->name->chars);
                    */
                    wr->format("<instance '%s' ", LitObject::as<LitInstance>(value)->klass->name->chars);
                    map = AS_MAP(value);
                    size = map->values.m_count;
                    print_map(state, wr, map, size);
                    wr->put(">");
                }
                break;
            case LitObject::Type::BoundMethod:
                {
                    lit_print_value(state, wr, AS_BOUND_METHOD(value)->method);
                    return;
                }
                break;
            case LitObject::Type::Array:
                {
                    #ifdef LIT_MINIMIZE_CONTAINERS
                        wr->put("array");
                    #else
                        array = LitObject::as<LitArray>(value);
                        size = array->values.m_count;
                        print_array(state, wr, array, size);
                    #endif
                }
                break;
            case LitObject::Type::Map:
                {
                    #ifdef LIT_MINIMIZE_CONTAINERS
                        wr->format("map");
                    #else
                        map = AS_MAP(value);
                        size = map->values.m_count;
                        print_map(state, wr, map, size);
                    #endif
                }
                break;
            case LitObject::Type::Userdata:
                {
                    wr->format("userdata");
                }
                break;
            case LitObject::Type::Range:
                {
                    range = AS_RANGE(value);
                    wr->format("%g .. %g", range->from, range->to);
                }
                break;
            case LitObject::Type::Field:
                {
                    wr->format("field");
                }
                break;
            case LitObject::Type::Reference:
                {
                    wr->format("reference => ");
                    slot = AS_REFERENCE(value)->slot;
                    if(slot == nullptr)
                    {
                        wr->put("null");
                    }
                    else
                    {
                        lit_print_value(state, wr, *slot);
                    }
                }
                break;
            default:
                {
                }
                break;
        }
    }
    else
    {
        wr->put("!nullpointer!");
    }
}

//LitInterpretResult lit_call_instance_method(LitState* state, LitInstance* instance, LitString* mthname, LitValue* argv, size_t argc)
//
void lit_print_value(LitState* state, LitWriter* wr, LitValue value)
{
    /*
    LitValue mthtostring;
    LitValue tstrval;
    LitString* tstring;
    LitString* mthname;
    LitInterpretResult inret;
    LitValue args[1] = {LitObject::NullVal};
    mthname = CONST_STRING(state, "toString");
    fprintf(stderr, "lit_print_value: checking if toString() exists for '%s' ...\n", lit_get_value_type(value));
    if(AS_CLASS(value) != nullptr)
    {
        mthtostring = lit_instance_get_method(state, value, mthname);
        if(!LitObject::isNull(mthtostring))
        {
            fprintf(stderr, "lit_print_value: we got toString()! now checking if calling it works ...\n");
            inret = lit_instance_call_method(state, value, mthname, args, 0);
            if(inret.type == LITRESULT_OK)
            {
                fprintf(stderr, "lit_print_value: calling toString() succeeded! but is it a string? ...\n");
                tstrval = inret.result;
                if(!LitObject::isNull(tstrval))
                {
                    fprintf(stderr, "lit_print_value: toString() returned a string! so that's what we'll use.\n");
                    tstring = LitObject::as<LitString>(tstrval);
                    printf("%.*s", (int)tstring->length(), tstring->chars);
                    return;
                }
            }
        }
    }
    fprintf(stderr, "lit_print_value: nope, no toString(), or it didn't return a string. falling back to manual stringification\n");
    */
    if(LitObject::isBool(value))
    {
        wr->put(lit_as_bool(value) ? "true" : "false");
    }
    else if(LitObject::isNull(value))
    {
        wr->put("null");
    }
    else if(LitObject::isNumber(value))
    {
        wr->format("%g", LitObject::toNumber(value));
    }
    else if(LitObject::isObject(value))
    {
        print_object(state, wr, value);
    }
}


const char* lit_get_value_type(LitValue value)
{
    if(LitObject::isBool(value))
    {
        return "bool";
    }
    else if(LitObject::isNull(value))
    {
        return "null";
    }
    else if(LitObject::isNumber(value))
    {
        return "number";
    }
    else if(LitObject::isObject(value))
    {
        return lit_object_type_names[(int)OBJECT_TYPE(value)];
    }
    return "unknown";
}


//#define LIT_TRACE_EXECUTION

/*
* visual studio doesn't support computed gotos, so
* instead a switch-case is used. 
*/
#if !defined(_MSC_VER)
    //#define LIT_USE_COMPUTEDGOTO
#endif

#ifdef LIT_TRACE_EXECUTION
    #define vm_traceframe(fiber)\
        lit_trace_frame(fiber);
#else
    #define vm_traceframe(fiber) \
        do \
        { \
        } while(0);
#endif

#ifdef LIT_USE_COMPUTEDGOTO
    #define vm_default()
    #define op_case(name) \
        OP_##name:
#else
    #define vm_default() default:
    #define op_case(name) \
        case OP_##name:
#endif

#define vm_pushgc(state, allow) \
    bool was_allowed = state->allow_gc; \
    state->allow_gc = allow;

#define vm_popgc(state) \
    state->allow_gc = was_allowed;

#define vm_push(fiber, value) \
    (*fiber->stack_top++ = value)

#define vm_pop(fiber) \
    (*(--fiber->stack_top))

#define vm_drop(fiber) \
    (fiber->stack_top--)

#define vm_dropn(fiber, amount) \
    (fiber->stack_top -= amount)

#define vm_readbyte(ip) \
    (*ip++)

#if 1
#define vm_readshort(ip) \
    (ip += 2u, (uint16_t)((ip[-2] << 8u) | ip[-1]))
#else
/* todo: why does this seemingly break everything? */
static inline uint16_t vm_readshort(uint8_t* ip)
{
    return ip += 2u, (uint16_t)((ip[-2] << 8u) | ip[-1]);
}
#endif

#define vm_readconstant(current_chunk) \
    (current_chunk->constants.m_values[vm_readbyte(ip)])

#define vm_readconstantlong(current_chunk, ip) \
    (current_chunk->constants.m_values[vm_readshort(ip)])

#define vm_readstring(current_chunk) \
    LitObject::as<LitString>(vm_readconstant(current_chunk))

#define vm_readstringlong(current_chunk, ip) \
    LitObject::as<LitString>(vm_readconstantlong(current_chunk, ip))


static inline LitValue vm_peek(LitFiber* fiber, short distance)
{
    return fiber->stack_top[(-1) - distance];
}

#define vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues) \
    frame = &fiber->frames[fiber->frame_count - 1]; \
    current_chunk = &frame->function->chunk; \
    ip = frame->ip; \
    slots = frame->slots; \
    fiber->module = frame->function->module; \
    privates = fiber->module->privates; \
    upvalues = frame->closure == nullptr ? nullptr : frame->closure->upvalues;

#define vm_writeframe(frame, ip) \
    frame->ip = ip;

#define vm_returnerror() \
    vm_popgc(state); \
    return (LitInterpretResult){ LITRESULT_RUNTIME_ERROR, LitObject::NullVal };

#define vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues) \
    vm_writeframe(frame, ip); \
    fiber = vm->fiber; \
    if(fiber == nullptr) \
    { \
        return (LitInterpretResult){ LITRESULT_OK, vm_pop(fiber) }; \
    } \
    if(fiber->abort) \
    { \
        vm_returnerror(); \
    } \
    vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues); \
    vm_traceframe(fiber);

#define vm_callvalue(callee, arg_count) \
    if(call_value(vm, callee, arg_count)) \
    { \
        vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues); \
    }

#define vm_rterror(format) \
    if(lit_runtime_error(vm, format)) \
    { \
        vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues); \
        continue; \
    } \
    else \
    { \
        vm_returnerror(); \
    }

#define vm_rterrorvarg(format, ...) \
    if(lit_runtime_error(vm, format, __VA_ARGS__)) \
    { \
        vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues); \
        continue; \
    } \
    else \
    { \
        vm_returnerror(); \
    }

#define vm_invoke_from_class_advanced(zklass, method_name, arg_count, error, stat, ignoring, callee) \
    LitValue mthval; \
    if((LitObject::isInstance(callee) && (LitObject::as<LitInstance>(callee)->fields.get(method_name, &mthval))) \
       || zklass->stat.get(method_name, &mthval)) \
    { \
        if(ignoring) \
        { \
            if(call_value(vm, mthval, arg_count)) \
            { \
                vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues); \
                frame->result_ignored = true; \
            } \
            else \
            { \
                fiber->stack_top[-1] = callee; \
            } \
        } \
        else \
        { \
            vm_callvalue(mthval, arg_count); \
        } \
    } \
    else \
    { \
        if(error) \
        { \
            vm_rterrorvarg("Attempt to call method '%s', that is not defined in class %s", method_name->chars, \
                               zklass->name->chars) \
        } \
    } \
    if(error) \
    { \
        continue; \
    }

#define vm_invoke_from_class(klass, method_name, arg_count, error, stat, ignoring) \
    vm_invoke_from_class_advanced(klass, method_name, arg_count, error, stat, ignoring, vm_peek(fiber, arg_count))

#define vm_invokemethod(instance, method_name, arg_count) \
    LitClass* klass = state->getClassFor(instance); \
    if(klass == nullptr) \
    { \
        vm_rterror("invokemethod: only instances and classes have methods"); \
    } \
    vm_writeframe(frame, ip); \
    vm_invoke_from_class_advanced(klass, CONST_STRING(state, method_name), arg_count, true, methods, false, instance); \
    vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues)

#define vm_binaryop(type, op, op_string) \
    LitValue a = vm_peek(fiber, 1); \
    LitValue b = vm_peek(fiber, 0); \
    if(LitObject::isNumber(a)) \
    { \
        if(!LitObject::isNumber(b)) \
        { \
            if(!LitObject::isNull(b)) \
            { \
                vm_rterrorvarg("Attempt to use op %s with a number and a %s", op_string, lit_get_value_type(b)); \
            } \
        } \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = (type(LitObject::toNumber(a) op LitObject::toNumber(b))); \
        continue; \
    } \
    if(LitObject::isNull(a)) \
    { \
    /* vm_rterrorvarg("Attempt to use op %s on a null value", op_string); */ \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = LitObject::TrueVal; \
    } \
    else \
    { \
        vm_invokemethod(a, op_string, 1); \
    }

#define vm_bitwiseop(op, op_string) \
    LitValue a = vm_peek(fiber, 1); \
    LitValue b = vm_peek(fiber, 0); \
    if(!LitObject::isNumber(a) || !LitObject::isNumber(b)) \
    { \
        vm_rterrorvarg("Operands of bitwise op %s must be two numbers, got %s and %s", op_string, \
                           lit_get_value_type(a), lit_get_value_type(b)); \
    } \
    vm_drop(fiber); \
    *(fiber->stack_top - 1) = (LitObject::toValue((int)LitObject::toNumber(a) op(int) LitObject::toNumber(b)));

#define vm_invokeoperation(ignoring) \
    uint8_t arg_count = vm_readbyte(ip); \
    LitString* method_name = vm_readstringlong(current_chunk, ip); \
    LitValue receiver = vm_peek(fiber, arg_count); \
    if(LitObject::isNull(receiver)) \
    { \
        vm_rterror("Attempt to index a null value"); \
    } \
    vm_writeframe(frame, ip); \
    if(LitObject::isClass(receiver)) \
    { \
        vm_invoke_from_class_advanced(AS_CLASS(receiver), method_name, arg_count, true, static_fields, ignoring, receiver); \
        continue; \
    } \
    else if(LitObject::isInstance(receiver)) \
    { \
        LitInstance* instance = LitObject::as<LitInstance>(receiver); \
        LitValue value; \
        if(instance->fields.get(method_name, &value)) \
        { \
            fiber->stack_top[-arg_count - 1] = value; \
            vm_callvalue(value, arg_count); \
            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues); \
            continue; \
        } \
        vm_invoke_from_class_advanced(instance->klass, method_name, arg_count, true, methods, ignoring, receiver); \
    } \
    else \
    { \
        LitClass* type = state->getClassFor(receiver); \
        if(type == nullptr) \
        { \
            vm_rterror("invokeoperation: only instances and classes have methods"); \
        } \
        vm_invoke_from_class_advanced(type, method_name, arg_count, true, methods, ignoring, receiver); \
    }

static jmp_buf jump_buffer;

static void reset_stack(LitVM* vm)
{
    if(vm->fiber != nullptr)
    {
        vm->fiber->stack_top = vm->fiber->stack;
    }
}





void lit_trace_vm_stack(LitVM* vm, LitWriter* wr)
{
    LitValue* top;
    LitValue* slot;
    LitFiber* fiber;
    fiber = vm->fiber;
    if(fiber->stack_top == fiber->stack || fiber->frame_count == 0)
    {
        return;
    }
    top = fiber->frames[fiber->frame_count - 1].slots;
    wr->format("        | %s", COLOR_GREEN);
    for(slot = fiber->stack; slot < top; slot++)
    {
        wr->format("[ ");
        lit_print_value(vm->state, wr, *slot);
        wr->format(" ]");
    }
    wr->format("%s", COLOR_RESET);
    for(slot = top; slot < fiber->stack_top; slot++)
    {
        wr->format("[ ");
        lit_print_value(vm->state, wr, *slot);
        wr->format(" ]");
    }
    wr->format("\n");
}

bool lit_handle_runtime_error(LitVM* vm, LitString* error_string)
{
    int i;
    int count;
    size_t length;
    char* start;
    char* buffer;
    const char* name;
    LitCallFrame* frame;
    LitFunction* function;
    LitChunk* chunk;
    LitValue error;
    LitFiber* fiber;
    LitFiber* caller;
    error = error_string->asValue();
    fiber = vm->fiber;
    while(fiber != nullptr)
    {
        fiber->error = error;
        if(fiber->catcher)
        {
            vm->fiber = fiber->parent;
            vm->fiber->stack_top -= fiber->arg_count;
            vm->fiber->stack_top[-1] = error;
            return true;
        }
        caller = fiber->parent;
        fiber->parent = nullptr;
        fiber = caller;
    }
    fiber = vm->fiber;
    fiber->abort = true;
    fiber->error = error;
    if(fiber->parent != nullptr)
    {
        fiber->parent->abort = true;
    }
    // Maan, formatting c strings is hard...
    count = (int)fiber->frame_count - 1;
    length = snprintf(nullptr, 0, "%s%s\n", COLOR_RED, error_string->chars);
    for(i = count; i >= 0; i--)
    {
        frame = &fiber->frames[i];
        function = frame->function;
        chunk = &function->chunk;
        name = function->name == nullptr ? "unknown" : function->name->chars;

        if(chunk->has_line_info)
        {
            length += snprintf(nullptr, 0, "[line %d] in %s()\n", (int)chunk->get_line(frame->ip - chunk->code - 1), name);
        }
        else
        {
            length += snprintf(nullptr, 0, "\tin %s()\n", name);
        }
    }
    length += snprintf(nullptr, 0, "%s", COLOR_RESET);
    buffer = (char*)malloc(length + 1);
    buffer[length] = '\0';
    start = buffer + sprintf(buffer, "%s%s\n", COLOR_RED, error_string->chars);
    for(i = count; i >= 0; i--)
    {
        frame = &fiber->frames[i];
        function = frame->function;
        chunk = &function->chunk;
        name = function->name == nullptr ? "unknown" : function->name->chars;
        if(chunk->has_line_info)
        {
            start += sprintf(start, "[line %d] in %s()\n", (int)chunk->get_line(frame->ip - chunk->code - 1), name);
        }
        else
        {
            start += sprintf(start, "\tin %s()\n", name);
        }
    }
    start += sprintf(start, "%s", COLOR_RESET);
    vm->state->raiseError(RUNTIME_ERROR, buffer);
    free(buffer);
    reset_stack(vm);
    return false;
}

bool lit_vruntime_error(LitVM* vm, const char* format, va_list args)
{
    size_t buffer_size;
    char* buffer;
    va_list args_copy;
    va_copy(args_copy, args);
    buffer_size = vsnprintf(nullptr, 0, format, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, format, args);
    return lit_handle_runtime_error(vm, LitString::take(vm->state, buffer, buffer_size, false));
}

bool lit_runtime_error(LitVM* vm, const char* format, ...)
{
    bool result;
    va_list args;
    va_start(args, format);
    result = lit_vruntime_error(vm, format, args);
    va_end(args);
    return result;
}

bool lit_runtime_error_exiting(LitVM* vm, const char* format, ...)
{
    bool result;
    va_list args;
    va_start(args, format);
    result = lit_vruntime_error(vm, format, args);
    va_end(args);
    lit_native_exit_jump();
    return result;
}

static bool call(LitVM* vm, LitFunction* function, LitClosure* closure, uint8_t arg_count)
{
    bool vararg;
    size_t amount;
    size_t i;
    size_t osize;
    size_t newcapacity;
    size_t newsize;
    size_t vararg_count;
    size_t function_arg_count;
    LitCallFrame* frame;
    LitFiber* fiber;
    LitArray* array;
    fiber = vm->fiber;

    #if 0
    //if(fiber->frame_count == LIT_CALL_FRAMES_MAX)
    //{
        //lit_runtime_error(vm, "call stack overflow");
        //return true;
    //}
    #endif
    if(fiber->frame_count + 1 > fiber->frame_capacity)
    {
        //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->frame_capacity * 2);
        newcapacity = (fiber->frame_capacity * 2);
        newsize = (sizeof(LitCallFrame) * newcapacity);
        osize = (sizeof(LitCallFrame) * fiber->frame_capacity);
        fiber->frames = (LitCallFrame*)lit_reallocate(vm->state, fiber->frames, osize, newsize);
        fiber->frame_capacity = newcapacity;
    }

    function_arg_count = function->arg_count;
    lit_ensure_fiber_stack(vm->state, fiber, function->max_slots + (int)(fiber->stack_top - fiber->stack));
    frame = &fiber->frames[fiber->frame_count++];
    frame->function = function;
    frame->closure = closure;
    frame->ip = function->chunk.code;
    frame->slots = fiber->stack_top - arg_count - 1;
    frame->result_ignored = false;
    frame->return_to_c = false;
    if(arg_count != function_arg_count)
    {
        vararg = function->vararg;
        if(arg_count < function_arg_count)
        {
            amount = (int)function_arg_count - arg_count - (vararg ? 1 : 0);
            for(i = 0; i < amount; i++)
            {
                lit_push(vm, LitObject::NullVal);
            }
            if(vararg)
            {
                lit_push(vm, LitArray::make(vm->state)->asValue());
            }
        }
        else if(function->vararg)
        {
            array = LitArray::make(vm->state);
            vararg_count = arg_count - function_arg_count + 1;
            vm->state->pushRoot((LitObject*)array);
            array->values.reserve(vararg_count, LitObject::NullVal);
            vm->state->popRoot();
            for(i = 0; i < vararg_count; i++)
            {
                array->values.m_values[i] = vm->fiber->stack_top[(int)i - (int)vararg_count];
            }
            vm->fiber->stack_top -= vararg_count;
            lit_push(vm, array->asValue());
        }
        else
        {
            vm->fiber->stack_top -= (arg_count - function_arg_count);
        }
    }
    else if(function->vararg)
    {
        array = LitArray::make(vm->state);
        vararg_count = arg_count - function_arg_count + 1;
        vm->state->pushRoot((LitObject*)array);
        array->values.push(*(fiber->stack_top - 1));
        *(fiber->stack_top - 1) = array->asValue();
        vm->state->popRoot();
    }
    return true;
}

static bool call_value(LitVM* vm, LitValue callee, uint8_t arg_count)
{
    size_t i;
    bool bres;
    LitNativeMethod* mthobj;
    LitFiber* fiber;
    LitClosure* closure;
    LitBoundMethod* bound_method;
    LitValue mthval;
    LitValue result;
    LitInstance* instance;
    LitClass* klass;
    (void)fiber;
    if(LitObject::isObject(callee))
    {
        if(lit_set_native_exit_jump())
        {
            return true;
        }
        switch(OBJECT_TYPE(callee))
        {
            case LitObject::Type::Function:
                {
                    return call(vm, LitObject::as<LitFunction>(callee), nullptr, arg_count);
                }
                break;
            case LitObject::Type::Closure:
                {
                    closure = LitObject::as<LitClosure>(callee);
                    return call(vm, closure->function, closure, arg_count);
                }
                break;
            case LitObject::Type::NativeFunction:
                {
                    vm_pushgc(vm->state, false)
                    result = LitObject::as<LitNativeFunction>(callee)->function(vm, arg_count, vm->fiber->stack_top - arg_count);
                    vm->fiber->stack_top -= arg_count + 1;
                    lit_push(vm, result);
                    vm_popgc(vm->state);
                    return false;
                }
                break;
            case LitObject::Type::NativePrimitive:
                {
                    vm_pushgc(vm->state, false)
                    fiber = vm->fiber;
                    bres = LitObject::as<LitNativePrimFunction>(callee)->function(vm, arg_count, fiber->stack_top - arg_count);
                    if(bres)
                    {
                        fiber->stack_top -= arg_count;
                    }
                    vm_popgc(vm->state);
                    return bres;
                }
                break;
            case LitObject::Type::NativeMethod:
                {
                    vm_pushgc(vm->state, false);
                    mthobj = LitObject::as<LitNativeMethod>(callee);
                    fiber = vm->fiber;
                    result = mthobj->method(vm, *(vm->fiber->stack_top - arg_count - 1), arg_count, vm->fiber->stack_top - arg_count);
                    vm->fiber->stack_top -= arg_count + 1;
                    //if(!LitObject::isNull(result))
                    {
                        if(!vm->fiber->abort)
                        {
                            lit_push(vm, result);
                        }
                    }
                    vm_popgc(vm->state);
                    return false;
                }
                break;
            case LitObject::Type::PrimitiveMethod:
                {
                    vm_pushgc(vm->state, false);
                    fiber = vm->fiber;
                    bres = LitObject::as<LitPrimitiveMethod>(callee)->method(vm, *(fiber->stack_top - arg_count - 1), arg_count, fiber->stack_top - arg_count);
                    if(bres)
                    {
                        fiber->stack_top -= arg_count;
                    }
                    vm_popgc(vm->state);
                    return bres;
                }
                break;
            case LitObject::Type::Class:
                {
                    klass = AS_CLASS(callee);
                    instance = LitInstance::make(vm->state, klass);
                    vm->fiber->stack_top[-arg_count - 1] = instance->asValue();
                    if(klass->init_method != nullptr)
                    {
                        return call_value(vm, klass->init_method->asValue(), arg_count);
                    }
                    // Remove the arguments, so that they don't mess up the stack
                    // (default constructor has no arguments)
                    for(i = 0; i < arg_count; i++)
                    {
                        lit_pop(vm);
                    }
                    return false;
                }
                break;
            case LitObject::Type::BoundMethod:
                {
                    bound_method = AS_BOUND_METHOD(callee);
                    mthval = bound_method->method;
                    if(LitObject::isNativeMethod(mthval))
                    {
                        vm_pushgc(vm->state, false);
                        result = LitObject::as<LitNativeMethod>(mthval)->method(vm, bound_method->receiver, arg_count, vm->fiber->stack_top - arg_count);
                        vm->fiber->stack_top -= arg_count + 1;
                        lit_push(vm, result);
                        vm_popgc(vm->state);
                        return false;
                    }
                    else if(LitObject::isPrimitiveMethod(mthval))
                    {
                        fiber = vm->fiber;
                        vm_pushgc(vm->state, false);
                        if(LitObject::as<LitPrimitiveMethod>(mthval)->method(vm, bound_method->receiver, arg_count, fiber->stack_top - arg_count))
                        {
                            fiber->stack_top -= arg_count;
                            return true;
                        }
                        vm_popgc(vm->state);
                        return false;
                    }
                    else
                    {
                        vm->fiber->stack_top[-arg_count - 1] = bound_method->receiver;
                        return call(vm, LitObject::as<LitFunction>(mthval), nullptr, arg_count);
                    }
                }
                break;
            default:
                {
                }
                break;

        }
    }
    if(LitObject::isNull(callee))
    {
        lit_runtime_error(vm, "Attempt to call a null value");
    }
    else
    {
        lit_runtime_error(vm, "Can only call functions and classes, got %s", lit_get_value_type(callee));
    }
    return true;
}

static LitUpvalue* capture_upvalue(LitState* state, LitValue* local)
{
    LitUpvalue* upvalue;
    LitUpvalue* created_upvalue;
    LitUpvalue* previous_upvalue;
    previous_upvalue = nullptr;
    upvalue = state->vm->fiber->open_upvalues;
    while(upvalue != nullptr && upvalue->location > local)
    {
        previous_upvalue = upvalue;
        upvalue = upvalue->next;
    }
    if(upvalue != nullptr && upvalue->location == local)
    {
        return upvalue;
    }
    created_upvalue = lit_create_upvalue(state, local);
    created_upvalue->next = upvalue;
    if(previous_upvalue == nullptr)
    {
        state->vm->fiber->open_upvalues = created_upvalue;
    }
    else
    {
        previous_upvalue->next = created_upvalue;
    }
    return created_upvalue;
}

static void close_upvalues(LitVM* vm, const LitValue* last)
{
    LitFiber* fiber;
    LitUpvalue* upvalue;
    fiber = vm->fiber;
    while(fiber->open_upvalues != nullptr && fiber->open_upvalues->location >= last)
    {
        upvalue = fiber->open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        fiber->open_upvalues = upvalue->next;
    }
}

LitInterpretResult lit_interpret_module(LitState* state, LitModule* module)
{
    LitVM* vm;
    LitFiber* fiber;
    LitInterpretResult result;
    vm = state->vm;
    fiber = lit_create_fiber(state, module, module->main_function);
    vm->fiber = fiber;
    lit_push(vm, module->main_function->asValue());
    result = lit_interpret_fiber(state, fiber);
    return result;
}

LitInterpretResult lit_interpret_fiber(LitState* state, LitFiber* fiber)
{
    bool found;
    size_t arg_count;
    size_t arindex;
    size_t i;
    uint16_t offset;
    uint8_t index;
    uint8_t is_local;
    uint8_t instruction;
    uint8_t* ip;
    LitCallFrame* frame;
    LitChunk* current_chunk;
    LitClass* instance_klass;
    LitClass* klassobj;
    LitClass* super_klass;
    LitClass* type;
    LitClosure* closure;
    LitFiber* parent;
    LitField* field;
    LitFunction* function;
    LitInstance* instobj;
    LitString* field_name;
    LitString* method_name;
    LitString* name;
    LitUpvalue** upvalues;
    LitValue a;
    LitValue arg;
    LitValue b;
    LitValue getval;
    LitValue instval;
    LitValue klassval;
    LitValue vobj;
    LitValue operand;
    LitValue reference;
    LitValue result;
    LitValue setter;
    LitValue setval;
    LitValue slot;
    LitValue super;
    LitValue tmpval;
    LitValue value;
    LitValue* privates;
    LitValue* pval;
    LitValue* slots;
    PCGenericArray<LitValue>* values;
    LitVM* vm;
    (void)instruction;
    vm = state->vm;
    vm_pushgc(state, true);
    vm->fiber = fiber;
    fiber->abort = false;
    frame = &fiber->frames[fiber->frame_count - 1];
    current_chunk = &frame->function->chunk;
    fiber->module = frame->function->module;
    ip = frame->ip;
    slots = frame->slots;
    privates = fiber->module->privates;
    upvalues = frame->closure == nullptr ? nullptr : frame->closure->upvalues;

    // Has to be inside of the function in order for goto to work
    #ifdef LIT_USE_COMPUTEDGOTO
        static void* dispatch_table[] =
        {
            #define OPCODE(name, effect) &&OP_##name,
                OPCODE(POP, -1)
                OPCODE(RETURN, 0)
                OPCODE(CONSTANT, 1)
                OPCODE(CONSTANT_LONG, 1)
                OPCODE(TRUE, 1)
                OPCODE(FALSE, 1)
                OPCODE(NULL, 1)
                OPCODE(ARRAY, 1)
                OPCODE(OBJECT, 1)
                OPCODE(RANGE, -1)
                OPCODE(NEGATE, 0)
                OPCODE(NOT, 0)
                OPCODE(ADD, -1)
                OPCODE(SUBTRACT, -1)
                OPCODE(MULTIPLY, -1)
                OPCODE(POWER, -1)
                OPCODE(DIVIDE, -1)
                OPCODE(FLOOR_DIVIDE, -1)
                OPCODE(MOD, -1)
                OPCODE(BAND, -1)
                OPCODE(BOR, -1)
                OPCODE(BXOR, -1)
                OPCODE(LSHIFT, -1)
                OPCODE(RSHIFT, -1)
                OPCODE(BNOT, 0)
                OPCODE(EQUAL, -1)
                OPCODE(GREATER, -1)
                OPCODE(GREATER_EQUAL, -1)
                OPCODE(LESS, -1)
                OPCODE(LESS_EQUAL, -1)
                OPCODE(SET_GLOBAL, 0)
                OPCODE(GET_GLOBAL, 1)
                OPCODE(SET_LOCAL, 0)
                OPCODE(GET_LOCAL, 1)
                OPCODE(SET_LOCAL_LONG, 0)
                OPCODE(GET_LOCAL_LONG, 1)
                OPCODE(SET_PRIVATE, 0)
                OPCODE(GET_PRIVATE, 1)
                OPCODE(SET_PRIVATE_LONG, 0)
                OPCODE(GET_PRIVATE_LONG, 1)
                OPCODE(SET_UPVALUE, 0)
                OPCODE(GET_UPVALUE, 1)
                OPCODE(JUMP_IF_FALSE, -1)
                OPCODE(JUMP_IF_NULL, 0)
                OPCODE(JUMP_IF_NULL_POPPING, -1)
                OPCODE(JUMP, 0)
                OPCODE(JUMP_BACK, 0)
                OPCODE(AND, -1)
                OPCODE(OR, -1)
                OPCODE(NULL_OR, -1)
                OPCODE(CLOSURE, 1)
                OPCODE(CLOSE_UPVALUE, -1)
                OPCODE(CLASS, 1)
                OPCODE(GET_FIELD, -1)
                OPCODE(SET_FIELD, -2)
                // [array] [index] -> [value]
                OPCODE(SUBSCRIPT_GET, -1)
                // [array] [index] [value] -> [value]
                OPCODE(SUBSCRIPT_SET, -2)
                // [array] [value] -> [array]
                OPCODE(PUSH_ARRAY_ELEMENT, -1)
                // [map] [slot] [value] -> [map]
                OPCODE(PUSH_OBJECT_FIELD, -2)
                // [class] [method] -> [class]
                OPCODE(METHOD, -1)
                // [class] [method] -> [class]
                OPCODE(STATIC_FIELD, -1)
                OPCODE(DEFINE_FIELD, -1)
                OPCODE(INHERIT, 0)
                // [instance] [class] -> [bool]
                OPCODE(IS, -1)
                OPCODE(GET_SUPER_METHOD, 0)
                // Varying stack effect
                OPCODE(CALL, 0)
                OPCODE(INVOKE, 0)
                OPCODE(INVOKE_SUPER, 0)
                OPCODE(INVOKE_IGNORING, 0)
                OPCODE(INVOKE_SUPER_IGNORING, 0)
                OPCODE(POP_LOCALS, 0)
                OPCODE(VARARG, 0)
                OPCODE(REFERENCE_GLOBAL, 1)
                OPCODE(REFERENCE_PRIVATE, 0)
                OPCODE(REFERENCE_LOCAL, 1)
                OPCODE(REFERENCE_UPVALUE, 1)
                OPCODE(REFERENCE_FIELD, -1)
                OPCODE(SET_REFERENCE, -1)
            #undef OPCODE
        };
    #endif
#ifdef LIT_TRACE_EXECUTION
    vm_traceframe(fiber);
#endif

    while(true)
    {
#ifdef LIT_TRACE_STACK
        lit_trace_vm_stack(vm);
#endif

#ifdef LIT_CHECK_STACK_SIZE
        if((fiber->stack_top - frame->slots) > fiber->stack_capacity)
        {
            vm_rterrorvarg("Fiber stack is not large enough (%i > %i)", (int)(fiber->stack_top - frame->slots),
                               fiber->stack_capacity);
        }
#endif

        #ifdef LIT_USE_COMPUTEDGOTO
            #ifdef LIT_TRACE_EXECUTION
                instruction = *ip++;
                lit_disassemble_instruction(state, current_chunk, (size_t)(ip - current_chunk->code - 1), nullptr);
                goto* dispatch_table[instruction];
            #else
                goto* dispatch_table[*ip++];
            #endif
        #else
            instruction = *ip++;
            #ifdef LIT_TRACE_EXECUTION
                lit_disassemble_instruction(state, current_chunk, (size_t)(ip - current_chunk->code - 1), nullptr);
            #endif
            switch(instruction)
        #endif
        /*
        * each op_case(...){...} *MUST* end with either break, return, or continue.
        * computationally, fall-throughs differ wildly between computed gotos or switch/case statements.
        * in computed gotos, a "fall-through" just executes the next block (since it's just a labelled block),
        * which may invalidate the stack, and while the same is technically true for switch/case, they
        * could end up executing completely unrelated instructions.
        * think, declaring a block for OP_BUILDHOUSE, and the next block is OP_SETHOUSEONFIRE.
        * an easy mistake to make, but crucial to check.
        */
        {
            op_case(POP)
            {
                vm_drop(fiber);
                continue;
            }
            op_case(RETURN)
            {
                result = vm_pop(fiber);
                close_upvalues(vm, slots);
                vm_writeframe(frame, ip);
                fiber->frame_count--;
                if(frame->return_to_c)
                {
                    frame->return_to_c = false;
                    fiber->module->return_value = result;
                    fiber->stack_top = frame->slots;
                    return (LitInterpretResult){ LITRESULT_OK, result };
                }
                if(fiber->frame_count == 0)
                {
                    fiber->module->return_value = result;
                    if(fiber->parent == nullptr)
                    {
                        vm_drop(fiber);
                        state->allow_gc = was_allowed;
                        return (LitInterpretResult){ LITRESULT_OK, result };
                    }
                    arg_count = fiber->arg_count;
                    parent = fiber->parent;
                    fiber->parent = nullptr;
                    vm->fiber = fiber = parent;
                    vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                    vm_traceframe(fiber);
                    fiber->stack_top -= arg_count;
                    fiber->stack_top[-1] = result;
                    continue;
                }
                fiber->stack_top = frame->slots;
                if(frame->result_ignored)
                {
                    fiber->stack_top++;
                    frame->result_ignored = false;
                }
                else
                {
                    vm_push(fiber, result);
                }
                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                vm_traceframe(fiber);
                continue;
            }
            op_case(CONSTANT)
            {
                vm_push(fiber, vm_readconstant(current_chunk));
                continue;
            }
            op_case(CONSTANT_LONG)
            {
                vm_push(fiber, vm_readconstantlong(current_chunk, ip));
                continue;
            }
            op_case(TRUE)
            {
                vm_push(fiber, LitObject::TrueVal);
                continue;
            }
            op_case(FALSE)
            {
                vm_push(fiber, LitObject::FalseVal);
                continue;
            }
            op_case(NULL)
            {
                vm_push(fiber, LitObject::NullVal);
                continue;
            }
            op_case(ARRAY)
            {
                vm_push(fiber, LitArray::make(state)->asValue());
                continue;
            }
            op_case(OBJECT)
            {
                // TODO: use object, or map for literal '{...}' constructs?
                // objects would be more general-purpose, but don't implement anything map-like.
                //vm_push(fiber, LitInstance::make(state, state->objectvalue_class)->asValue());
                vm_push(fiber, LitMap::make(state)->asValue());

                continue;
            }
            op_case(RANGE)
            {
                a = vm_pop(fiber);
                b = vm_pop(fiber);
                if(!LitObject::isNumber(a) || !LitObject::isNumber(b))
                {
                    vm_rterror("Range operands must be number");
                }
                vm_push(fiber, lit_create_range(state, LitObject::toNumber(a), LitObject::toNumber(b))->asValue());
                continue;
            }
            op_case(NEGATE)
            {
                if(!LitObject::isNumber(vm_peek(fiber, 0)))
                {
                    arg = vm_peek(fiber, 0);
                    // Don't even ask me why
                    // This doesn't kill our performance, since it's a error anyway
                    if(LitObject::isString(arg) && strcmp(lit_as_cstring(arg), "muffin") == 0)
                    {
                        vm_rterror("Idk, can you negate a muffin?");
                    }
                    else
                    {
                        vm_rterror("Operand must be a number");
                    }
                }
                tmpval = LitObject::toValue(-LitObject::toNumber(vm_pop(fiber)));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(NOT)
            {
                if(LitObject::isInstance(vm_peek(fiber, 0)))
                {
                    vm_writeframe(frame, ip);
                    vm_invoke_from_class(LitObject::as<LitInstance>(vm_peek(fiber, 0))->klass, CONST_STRING(state, "!"), 0, false, methods, false);
                    continue;
                }
                tmpval = BOOL_VALUE(LitObject::isFalsey(vm_pop(fiber)));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(BNOT)
            {
                if(!LitObject::isNumber(vm_peek(fiber, 0)))
                {
                    vm_rterror("Operand must be a number");
                }
                tmpval = LitObject::toValue(~((int)LitObject::toNumber(vm_pop(fiber))));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(ADD)
            {
                vm_binaryop(LitObject::toValue, +, "+");
                continue;
            }
            op_case(SUBTRACT)
            {
                vm_binaryop(LitObject::toValue, -, "-");
                continue;
            }
            op_case(MULTIPLY)
            {
                vm_binaryop(LitObject::toValue, *, "*");
                continue;
            }
            op_case(POWER)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(LitObject::isNumber(a) && LitObject::isNumber(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = (LitObject::toValue(pow(LitObject::toNumber(a), LitObject::toNumber(b))));
                    continue;
                }
                vm_invokemethod(a, "**", 1);
                continue;
            }
            op_case(DIVIDE)
            {
                vm_binaryop(LitObject::toValue, /, "/");
                continue;
            }
            op_case(FLOOR_DIVIDE)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(LitObject::isNumber(a) && LitObject::isNumber(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = (LitObject::toValue(floor(LitObject::toNumber(a) / LitObject::toNumber(b))));

                    continue;
                }

                vm_invokemethod(a, "#", 1);
                continue;
            }
            op_case(MOD)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(LitObject::isNumber(a) && LitObject::isNumber(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = LitObject::toValue(fmod(LitObject::toNumber(a), LitObject::toNumber(b)));
                    continue;
                }
                vm_invokemethod(a, "%", 1);
                continue;
            }
            op_case(BAND)
            {
                vm_bitwiseop(&, "&");
                continue;
            }
            op_case(BOR)
            {
                vm_bitwiseop(|, "|");
                continue;
            }
            op_case(BXOR)
            {
                vm_bitwiseop(^, "^");
                continue;
            }
            op_case(LSHIFT)
            {
                vm_bitwiseop(<<, "<<");
                continue;
            }
            op_case(RSHIFT)
            {
                vm_bitwiseop(>>, ">>");
                continue;
            }
            op_case(EQUAL)
            {
                /*
                if(LitObject::isInstance(vm_peek(fiber, 1)))
                {
                    vm_writeframe(frame, ip);
                    fprintf(stderr, "OP_EQUAL: trying to invoke '==' ...\n");
                    vm_invoke_from_class(LitObject::as<LitInstance>(vm_peek(fiber, 1))->klass, CONST_STRING(state, "=="), 1, false, methods, false);
                    continue;
                }
                a = vm_pop(fiber);
                b = vm_pop(fiber);
                vm_push(fiber, BOOL_VALUE(a == b));
                */
                vm_binaryop(LitObject::toValue, ==, "==");
                continue;
            }

            op_case(GREATER)
            {
                vm_binaryop(BOOL_VALUE, >, ">");
                continue;
            }
            op_case(GREATER_EQUAL)
            {
                vm_binaryop(BOOL_VALUE, >=, ">=");
                continue;
            }
            op_case(LESS)
            {
                vm_binaryop(BOOL_VALUE, <, "<");
                continue;
            }
            op_case(LESS_EQUAL)
            {
                vm_binaryop(BOOL_VALUE, <=, "<=");
                continue;
            }

            op_case(SET_GLOBAL)
            {
                name = vm_readstringlong(current_chunk, ip);
                vm->globals->values.set(name, vm_peek(fiber, 0));
                continue;
            }

            op_case(GET_GLOBAL)
            {
                name = vm_readstringlong(current_chunk, ip);
                if(!vm->globals->values.get(name, &setval))
                {
                    vm_push(fiber, LitObject::NullVal);
                }
                else
                {
                    vm_push(fiber, setval);
                }
                continue;
            }
            op_case(SET_LOCAL)
            {
                index = vm_readbyte(ip);
                slots[index] = vm_peek(fiber, 0);
                continue;
            }
            op_case(GET_LOCAL)
            {
                vm_push(fiber, slots[vm_readbyte(ip)]);
                continue;
            }
            op_case(SET_LOCAL_LONG)
            {
                index = vm_readshort(ip);
                slots[index] = vm_peek(fiber, 0);
                continue;
            }
            op_case(GET_LOCAL_LONG)
            {
                vm_push(fiber, slots[vm_readshort(ip)]);
                continue;
            }
            op_case(SET_PRIVATE)
            {
                index = vm_readbyte(ip);
                privates[index] = vm_peek(fiber, 0);
                continue;
            }
            op_case(GET_PRIVATE)
            {
                vm_push(fiber, privates[vm_readbyte(ip)]);
                continue;
            }
            op_case(SET_PRIVATE_LONG)
            {
                index = vm_readshort(ip);
                privates[index] = vm_peek(fiber, 0);
                continue;
            }
            op_case(GET_PRIVATE_LONG)
            {
                vm_push(fiber, privates[vm_readshort(ip)]);
                continue;
            }
            op_case(SET_UPVALUE)
            {
                index = vm_readbyte(ip);
                *upvalues[index]->location = vm_peek(fiber, 0);
                continue;
            }
            op_case(GET_UPVALUE)
            {
                vm_push(fiber, *upvalues[vm_readbyte(ip)]->location);
                continue;
            }

            op_case(JUMP_IF_FALSE)
            {
                offset = vm_readshort(ip);
                if(LitObject::isFalsey(vm_pop(fiber)))
                {
                    ip += offset;
                }
                continue;
            }
            op_case(JUMP_IF_NULL)
            {
                offset = vm_readshort(ip);
                if(LitObject::isNull(vm_peek(fiber, 0)))
                {
                    ip += offset;
                }
                continue;
            }
            op_case(JUMP_IF_NULL_POPPING)
            {
                offset = vm_readshort(ip);
                if(LitObject::isNull(vm_pop(fiber)))
                {
                    ip += offset;
                }
                continue;
            }
            op_case(JUMP)
            {
                offset = vm_readshort(ip);
                ip += offset;
                continue;
            }
            op_case(JUMP_BACK)
            {
                offset = vm_readshort(ip);
                ip -= offset;
                continue;
            }
            op_case(AND)
            {
                offset = vm_readshort(ip);
                if(LitObject::isFalsey(vm_peek(fiber, 0)))
                {
                    ip += offset;
                }
                else
                {
                    vm_drop(fiber);
                }
                continue;
            }
            op_case(OR)
            {
                offset = vm_readshort(ip);
                if(LitObject::isFalsey(vm_peek(fiber, 0)))
                {
                    vm_drop(fiber);
                }
                else
                {
                    ip += offset;
                }
                continue;
            }
            op_case(NULL_OR)
            {
                offset = vm_readshort(ip);
                if(LitObject::isNull(vm_peek(fiber, 0)))
                {
                    vm_drop(fiber);
                }
                else
                {
                    ip += offset;
                }
                continue;
            }
            op_case(CALL)
            {
                arg_count = vm_readbyte(ip);
                vm_writeframe(frame, ip);
                vm_callvalue(vm_peek(fiber, arg_count), arg_count);
                continue;
            }
            op_case(CLOSURE)
            {
                function = LitObject::as<LitFunction>(vm_readconstantlong(current_chunk, ip));
                closure = lit_create_closure(state, function);
                vm_push(fiber, closure->asValue());
                for(i = 0; i < closure->upvalue_count; i++)
                {
                    is_local = vm_readbyte(ip);
                    index = vm_readbyte(ip);
                    if(is_local)
                    {
                        closure->upvalues[i] = capture_upvalue(state, frame->slots + index);
                    }
                    else
                    {
                        closure->upvalues[i] = upvalues[index];
                    }
                }
                continue;
            }
            op_case(CLOSE_UPVALUE)
            {
                close_upvalues(vm, fiber->stack_top - 1);
                vm_drop(fiber);
                continue;
            }
            op_case(CLASS)
            {
                name = vm_readstringlong(current_chunk, ip);
                klassobj = LitClass::make(state, name);
                vm_push(fiber, klassobj->asValue());
                klassobj->super = state->objectvalue_class;
                klassobj->super->methods.addAll(&klassobj->methods);
                klassobj->super->static_fields.addAll(&klassobj->static_fields);
                vm->globals->values.set(name, klassobj->asValue());
                continue;
            }
            op_case(GET_FIELD)
            {
                vobj = vm_peek(fiber, 1);
                if(LitObject::isNull(vobj))
                {
                    vm_rterror("Attempt to index a null value");
                }
                name = LitObject::as<LitString>(vm_peek(fiber, 0));
                if(LitObject::isInstance(vobj))
                {
                    instobj = LitObject::as<LitInstance>(vobj);

                    if(!instobj->fields.get(name, &getval))
                    {
                        if(instobj->klass->methods.get(name, &getval))
                        {
                            if(LitObject::isField(getval))
                            {
                                field = AS_FIELD(getval);
                                if(field->getter == nullptr)
                                {
                                    vm_rterrorvarg("Class %s does not have a getter for the field %s",
                                                       instobj->klass->name->chars, name->chars);
                                }
                                vm_drop(fiber);
                                vm_writeframe(frame, ip);
                                vm_callvalue(AS_FIELD(getval)->getter->asValue(), 0);
                                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                                continue;
                            }
                            else
                            {
                                getval = lit_create_bound_method(state, instobj->asValue(), getval)->asValue();
                            }
                        }
                        else
                        {
                            getval = LitObject::NullVal;
                        }
                    }
                }
                else if(LitObject::isClass(vobj))
                {
                    klassobj = AS_CLASS(vobj);
                    if(klassobj->static_fields.get(name, &getval))
                    {
                        if(LitObject::isNativeMethod(getval) || LitObject::isPrimitiveMethod(getval))
                        {
                            getval = lit_create_bound_method(state, klassobj->asValue(), getval)->asValue();
                        }
                        else if(LitObject::isField(getval))
                        {
                            field = AS_FIELD(getval);
                            if(field->getter == nullptr)
                            {
                                vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->chars,
                                                   name->chars);
                            }
                            vm_drop(fiber);
                            vm_writeframe(frame, ip);
                            vm_callvalue(field->getter->asValue(), 0);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                    }
                    else
                    {
                        getval = LitObject::NullVal;
                    }
                }
                else
                {
                    klassobj = state->getClassFor(vobj);
                    if(klassobj == nullptr)
                    {
                        vm_rterror("GET_FIELD: only instances and classes have fields");
                    }
                    if(klassobj->methods.get(name, &getval))
                    {
                        if(LitObject::isField(getval))
                        {
                            field = AS_FIELD(getval);
                            if(field->getter == nullptr)
                            {
                                vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->chars,
                                                   name->chars);
                            }
                            vm_drop(fiber);
                            vm_writeframe(frame, ip);
                            vm_callvalue(AS_FIELD(getval)->getter->asValue(), 0);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                        else if(LitObject::isNativeMethod(getval) || LitObject::isPrimitiveMethod(getval))
                        {
                            getval = lit_create_bound_method(state, vobj, getval)->asValue();
                        }
                    }
                    else
                    {
                        getval = LitObject::NullVal;
                    }
                }
                vm_drop(fiber);// Pop field name
                fiber->stack_top[-1] = getval;
                continue;
            }
            op_case(SET_FIELD)
            {
                instval = vm_peek(fiber, 2);
                if(LitObject::isNull(instval))
                {
                    vm_rterror("Attempt to index a null value")
                }
                value = vm_peek(fiber, 1);
                field_name = LitObject::as<LitString>(vm_peek(fiber, 0));
                if(LitObject::isClass(instval))
                {
                    klassobj = AS_CLASS(instval);
                    if(klassobj->static_fields.get(field_name, &setter) && LitObject::isField(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == nullptr)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->chars,
                                               field_name->chars);
                        }

                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(field->setter->asValue(), 1);
                        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                        continue;
                    }
                    if(LitObject::isNull(value))
                    {
                        klassobj->static_fields.remove(field_name);
                    }
                    else
                    {
                        klassobj->static_fields.set(field_name, value);
                    }
                    vm_dropn(fiber, 2);// Pop field name and the value
                    fiber->stack_top[-1] = value;
                }
                else if(LitObject::isInstance(instval))
                {
                    instobj = LitObject::as<LitInstance>(instval);
                    if(instobj->klass->methods.get(field_name, &setter) && LitObject::isField(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == nullptr)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", instobj->klass->name->chars,
                                               field_name->chars);
                        }
                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(field->setter->asValue(), 1);
                        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                        continue;
                    }
                    if(LitObject::isNull(value))
                    {
                        instobj->fields.remove(field_name);
                    }
                    else
                    {
                        instobj->fields.set(field_name, value);
                    }
                    vm_dropn(fiber, 2);// Pop field name and the value
                    fiber->stack_top[-1] = value;
                }
                else
                {
                    klassobj = state->getClassFor(instval);
                    if(klassobj == nullptr)
                    {
                        vm_rterror("SET_FIELD: only instances and classes have fields");
                    }
                    if(klassobj->methods.get(field_name, &setter) && LitObject::isField(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == nullptr)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->chars,
                                               field_name->chars);
                        }
                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(field->setter->asValue(), 1);
                        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                        continue;
                    }
                    else
                    {
                        vm_rterrorvarg("Class %s does not contain field %s", klassobj->name->chars, field_name->chars);
                    }
                }
                continue;
            }
            op_case(SUBSCRIPT_GET)
            {
                vm_invokemethod(vm_peek(fiber, 1), "[]", 1);
                continue;
            }
            op_case(SUBSCRIPT_SET)
            {
                vm_invokemethod(vm_peek(fiber, 2), "[]", 2);
                continue;
            }
            op_case(PUSH_ARRAY_ELEMENT)
            {
                values = &LitObject::as<LitArray>(vm_peek(fiber, 1))->values;
                arindex = values->m_count;
                values->reserve(arindex + 1, LitObject::NullVal);
                values->m_values[arindex] = vm_peek(fiber, 0);
                vm_drop(fiber);
                continue;
            }
            op_case(PUSH_OBJECT_FIELD)
            {
                operand = vm_peek(fiber, 2);
                if(LitObject::isMap(operand))
                {
                    AS_MAP(operand)->values.set(LitObject::as<LitString>(vm_peek(fiber, 1)), vm_peek(fiber, 0));
                }
                else if(LitObject::isInstance(operand))
                {
                    LitObject::as<LitInstance>(operand)->fields.set(LitObject::as<LitString>(vm_peek(fiber, 1)), vm_peek(fiber, 0));
                }
                else
                {
                    vm_rterrorvarg("Expected an object or a map as the operand, got %s", lit_get_value_type(operand));
                }
                vm_dropn(fiber, 2);
                continue;
            }
            op_case(STATIC_FIELD)
            {
                AS_CLASS(vm_peek(fiber, 1))->static_fields.set(vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
                vm_drop(fiber);
                continue;
            }
            op_case(METHOD)
            {
                klassobj = AS_CLASS(vm_peek(fiber, 1));
                name = vm_readstringlong(current_chunk, ip);
                if((klassobj->init_method == nullptr || (klassobj->super != nullptr && klassobj->init_method == ((LitClass*)klassobj->super)->init_method))
                   && name->length() == 11 && memcmp(name->chars, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1) == 0)
                {
                    klassobj->init_method = LitObject::asObject(vm_peek(fiber, 0));
                }
                klassobj->methods.set(name, vm_peek(fiber, 0));
                vm_drop(fiber);
                continue;
            }
            op_case(DEFINE_FIELD)
            {
                AS_CLASS(vm_peek(fiber, 1))->methods.set(vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
                vm_drop(fiber);
                continue;
            }
            op_case(INVOKE)
            {
                vm_invokeoperation(false);
                continue;
            }
            op_case(INVOKE_IGNORING)
            {
                vm_invokeoperation(true);
                continue;
            }
            op_case(INVOKE_SUPER)
            {
                arg_count = vm_readbyte(ip);
                method_name = vm_readstringlong(current_chunk, ip);
                klassobj = AS_CLASS(vm_pop(fiber));
                vm_writeframe(frame, ip);
                vm_invoke_from_class(klassobj, method_name, arg_count, true, methods, false);
                continue;
            }
            op_case(INVOKE_SUPER_IGNORING)
            {
                arg_count = vm_readbyte(ip);
                method_name = vm_readstringlong(current_chunk, ip);
                klassobj = AS_CLASS(vm_pop(fiber));
                vm_writeframe(frame, ip);
                vm_invoke_from_class(klassobj, method_name, arg_count, true, methods, true);
                continue;
            }
            op_case(GET_SUPER_METHOD)
            {
                method_name = vm_readstringlong(current_chunk, ip);
                klassobj = AS_CLASS(vm_pop(fiber));
                instval = vm_pop(fiber);
                if(klassobj->methods.get(method_name, &value))
                {
                    value = lit_create_bound_method(state, instval, value)->asValue();
                }
                else
                {
                    value = LitObject::NullVal;
                }
                vm_push(fiber, value);
                continue;
            }
            op_case(INHERIT)
            {
                super = vm_peek(fiber, 1);
                if(!LitObject::isClass(super))
                {
                    vm_rterror("Superclass must be a class");
                }
                klassobj = AS_CLASS(vm_peek(fiber, 0));
                super_klass = AS_CLASS(super);
                klassobj->super = super_klass;
                klassobj->init_method = super_klass->init_method;
                super_klass->methods.addAll(&klassobj->methods);
                klassobj->super->static_fields.addAll(&klassobj->static_fields);
                continue;
            }
            op_case(IS)
            {
                instval = vm_peek(fiber, 1);
                if(LitObject::isNull(instval))
                {
                    vm_dropn(fiber, 2);
                    vm_push(fiber, LitObject::FalseVal);

                    continue;
                }
                instance_klass = state->getClassFor(instval);
                klassval = vm_peek(fiber, 0);
                if(instance_klass == nullptr || !LitObject::isClass(klassval))
                {
                    vm_rterror("operands must be an instance and a class");
                }            
                type = AS_CLASS(klassval);
                found = false;
                while(instance_klass != nullptr)
                {
                    if(instance_klass == type)
                    {
                        found = true;
                        break;
                    }
                    instance_klass = (LitClass*)instance_klass->super;
                }
                vm_dropn(fiber, 2);// Drop the instance and class
                vm_push(fiber, BOOL_VALUE(found));
                continue;
            }
            op_case(POP_LOCALS)
            {
                vm_dropn(fiber, vm_readshort(ip));
                continue;
            }
            op_case(VARARG)
            {
                slot = slots[vm_readbyte(ip)];
                if(!LitObject::isArray(slot))
                {
                    continue;
                }
                values = &LitObject::as<LitArray>(slot)->values;
                lit_ensure_fiber_stack(state, fiber, values->m_count + frame->function->max_slots + (int)(fiber->stack_top - fiber->stack));
                for(i = 0; i < values->m_count; i++)
                {
                    vm_push(fiber, values->m_values[i]);
                }
                // Hot-bytecode patching, increment the amount of arguments to OP_CALL
                ip[1] = ip[1] + values->m_count - 1;
                continue;
            }

            op_case(REFERENCE_GLOBAL)
            {
                name = vm_readstringlong(current_chunk, ip);
                if(vm->globals->values.getSlot(name, &pval))
                {
                    vm_push(fiber, lit_create_reference(state, pval)->asValue());
                }
                else
                {
                    vm_rterror("Attempt to reference a null value");
                }
                continue;
            }
            op_case(REFERENCE_PRIVATE)
            {
                vm_push(fiber, lit_create_reference(state, &privates[vm_readshort(ip)])->asValue());
                continue;
            }
            op_case(REFERENCE_LOCAL)
            {
                vm_push(fiber, lit_create_reference(state, &slots[vm_readshort(ip)])->asValue());
                continue;
            }
            op_case(REFERENCE_UPVALUE)
            {
                vm_push(fiber, lit_create_reference(state, upvalues[vm_readbyte(ip)]->location)->asValue());
                continue;
            }
            op_case(REFERENCE_FIELD)
            {
                vobj = vm_peek(fiber, 1);
                if(LitObject::isNull(vobj))
                {
                    vm_rterror("Attempt to index a null value");
                }
                name = LitObject::as<LitString>(vm_peek(fiber, 0));
                if(LitObject::isInstance(vobj))
                {
                    if(!LitObject::as<LitInstance>(vobj)->fields.getSlot(name, &pval))
                    {
                        vm_rterror("Attempt to reference a null value");
                    }
                }
                else
                {
                    lit_print_value(state, &state->debugwriter, vobj);
                    printf("\n");
                    vm_rterror("You can only reference fields of real instances");
                }
                vm_drop(fiber);// Pop field name
                fiber->stack_top[-1] = lit_create_reference(state, pval)->asValue();
                continue;
            }
            op_case(SET_REFERENCE)
            {
                reference = vm_pop(fiber);
                if(!LitObject::isReference(reference))
                {
                    vm_rterror("Provided value is not a reference");
                }
                *AS_REFERENCE(reference)->slot = vm_peek(fiber, 0);
                continue;
            }
            vm_default()
            {
                vm_rterrorvarg("Unknown op code '%d'", *ip);
                break;
            }
        }
    }

    vm_returnerror();
}

#undef vm_rterrorvarg
#undef vm_rterror
#undef vm_invokemethod
#undef vm_invoke_from_class
#undef vm_invoke_from_class_advanced
#undef vm_dropn
#undef vm_push
#undef vm_drop
#undef vm_pop
#undef vm_callvalue
#undef vm_recoverstate
#undef vm_writeframe
#undef vm_readframe
#undef vm_bitwiseop
#undef vm_binaryop
#undef vm_readconstantlong
#undef vm_readconstant
#undef op_case
#undef vm_readstring
#undef vm_readshort
#undef vm_readbyte
#undef vm_returnerror

void lit_native_exit_jump()
{
    longjmp(jump_buffer, 1);
}

bool lit_set_native_exit_jump()
{
    return setjmp(jump_buffer);
}

#undef vm_pushgc
#undef vm_popgc
#undef LIT_TRACE_FRAME





static LitValue objfn_array_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return LitArray::make(vm->state)->asValue();
}

static LitValue objfn_array_splice(LitVM* vm, LitArray* array, int from, int to)
{
    size_t i;
    size_t length;
    LitArray* new_array;
    length = array->values.m_count;
    if(from < 0)
    {
        from = (int)length + from;
    }
    if(to < 0)
    {
        to = (int)length + to;
    }
    if(from > to)
    {
        lit_runtime_error_exiting(vm, "Array.splice argument 'from' is larger than argument 'to'");
    }
    from = fmax(from, 0);
    to = fmin(to, (int)length - 1);
    length = fmin(length, to - from + 1);
    new_array = LitArray::make(vm->state);
    for(i = 0; i < length; i++)
    {
        new_array->values.push(array->values.m_values[from + i]);
    }
    return new_array->asValue();
}

static LitValue objfn_array_slice(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int from;
    int to;
    from = lit_check_number(vm, argv, argc, 0);
    to = lit_check_number(vm, argv, argc, 1);
    return objfn_array_splice(vm, LitObject::as<LitArray>(instance), from, to);
}

static LitValue objfn_array_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitRange* range;
    PCGenericArray<LitValue>* values;
    if(argc == 2)
    {
        if(!LitObject::isNumber(argv[0]))
        {
            lit_runtime_error_exiting(vm, "array index must be a number");
        }
        values = &LitObject::as<LitArray>(instance)->values;
        index = LitObject::toNumber(argv[0]);
        if(index < 0)
        {
            index = fmax(0, values->m_count + index);
        }
        values->reserve(index + 1, LitObject::NullVal);
        return values->m_values[index] = argv[1];
    }
    if(!LitObject::isNumber(argv[0]))
    {
        if(LitObject::isRange(argv[0]))
        {
            range = AS_RANGE(argv[0]);
            return objfn_array_splice(vm, LitObject::as<LitArray>(instance), (int)range->from, (int)range->to);
        }
        lit_runtime_error_exiting(vm, "array index must be a number");
        return LitObject::NullVal;
    }
    values = &LitObject::as<LitArray>(instance)->values;
    index = LitObject::toNumber(argv[0]);
    if(index < 0)
    {
        index = fmax(0, values->m_count + index);
    }
    if(values->m_capacity <= (size_t)index)
    {
        return LitObject::NullVal;
    }
    return values->m_values[index];
}

bool lit_compare_values(LitState* state, const LitValue a, const LitValue b)
{
    LitInterpretResult inret;
    LitValue args[3];
    if(LitObject::isInstance(a))
    {
        args[0] = b;
        inret = lit_instance_call_method(state, a, CONST_STRING(state, "=="), args, 1);
        if(inret.type == LITRESULT_OK)
        {
            if(BOOL_VALUE(inret.result) == LitObject::TrueVal)
            {
                return true;
            }
            return false;
        }
    }
    return (a == b);
}

static LitValue objfn_array_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    LitArray* self;
    LitArray* other;
    (void)argc;
    fprintf(stderr, "call to objfn_array_compare\n");
    self = LitObject::as<LitArray>(instance);
    if(LitObject::isArray(argv[0]))
    {
        other = LitObject::as<LitArray>(argv[0]);
        if(self->values.m_count == other->values.m_count)
        {
            for(i=0; i<self->values.m_count; i++)
            {
                if(!lit_compare_values(vm->state, self->values.m_values[i], other->values.m_values[i]))
                {
                    return LitObject::FalseVal;
                }
            }
            return LitObject::TrueVal;
        }
        return LitObject::FalseVal;
    }
    lit_runtime_error_exiting(vm, "can only compare array to another array or null");
    return LitObject::FalseVal;
}

static LitValue objfn_array_add(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    (void)vm;
    for(i=0; i<argc; i++)
    {
        LitObject::as<LitArray>(instance)->push(argv[i]);
    }
    return LitObject::NullVal;
}

static LitValue objfn_array_insert(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int i;
    int index;
    LitValue value;
    PCGenericArray<LitValue>* values;
    LIT_ENSURE_ARGS(2);
    values = &LitObject::as<LitArray>(instance)->values;
    index = lit_check_number(vm, argv, argc, 0);

    if(index < 0)
    {
        index = fmax(0, values->m_count + index);
    }
    value = argv[1];
    if((int)values->m_count <= index)
    {
        values->reserve(index + 1, LitObject::NullVal);
    }
    else
    {
        values->reserve(values->m_count + 1, LitObject::NullVal);
        for(i = values->m_count - 1; i > index; i--)
        {
            values->m_values[i] = values->m_values[i - 1];
        }
    }
    values->m_values[index] = value;
    return LitObject::NullVal;
}

static LitValue objfn_array_addall(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    LitArray* array;
    LitArray* toadd;
    LIT_ENSURE_ARGS(1);
    if(!LitObject::isArray(argv[0]))
    {
        lit_runtime_error_exiting(vm, "expected array as the argument");
    }
    array = LitObject::as<LitArray>(instance);
    toadd = LitObject::as<LitArray>(argv[0]);
    for(i = 0; i < toadd->values.m_count; i++)
    {
        array->push(toadd->values.m_values[i]);
    }
    return LitObject::NullVal;
}

static LitValue objfn_array_indexof(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1)

        int index = LitObject::as<LitArray>(instance)->indexOf(argv[0]);
    return index == -1 ? LitObject::NullVal : LitObject::toValue(index);
}


static LitValue objfn_array_remove(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitArray* array;
    LIT_ENSURE_ARGS(1);
    array = LitObject::as<LitArray>(instance);
    index = array->indexOf(argv[0]);
    if(index != -1)
    {
        return array->removeAt((size_t)index);
    }
    return LitObject::NullVal;
}

static LitValue objfn_array_removeat(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    index = lit_check_number(vm, argv, argc, 0);
    if(index < 0)
    {
        return LitObject::NullVal;
    }
    return LitObject::as<LitArray>(instance)->removeAt((size_t)index);
}

static LitValue objfn_array_contains(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    return BOOL_VALUE(LitObject::as<LitArray>(instance)->indexOf(argv[0]) != -1);
}

static LitValue objfn_array_clear(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    LitObject::as<LitArray>(instance)->values.m_count = 0;
    return LitObject::NullVal;
}

static LitValue objfn_array_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int number;
    LitArray* array;
    (void)vm;
    LIT_ENSURE_ARGS(1);
    array = LitObject::as<LitArray>(instance);
    number = 0;
    if(LitObject::isNumber(argv[0]))
    {
        number = LitObject::toNumber(argv[0]);
        if(number >= (int)array->values.m_count - 1)
        {
            return LitObject::NullVal;
        }
        number++;
    }
    return array->values.m_count == 0 ? LitObject::NullVal : LitObject::toValue(number);
}

static LitValue objfn_array_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    PCGenericArray<LitValue>* values;
    index = lit_check_number(vm, argv, argc, 0);
    values = &LitObject::as<LitArray>(instance)->values;
    if(values->m_count <= index)
    {
        return LitObject::NullVal;
    }
    return values->m_values[index];
}

static LitValue objfn_array_join(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    size_t jlen;
    size_t index;
    size_t length;
    char* chars;
    PCGenericArray<LitValue>* values;
    LitString* string;
    LitString* joinee;
    LitString** strings;
    (void)argc;
    (void)argv;
    joinee = nullptr;
    length = 0;
    if(argc > 0)
    {
        joinee = LitObject::as<LitString>(argv[0]);
    }
    values = &LitObject::as<LitArray>(instance)->values;
    //LitString* strings[values.m_count];
    strings = LIT_ALLOCATE(vm->state, LitString*, values->m_count+1);
    for(i = 0; i < values->m_count; i++)
    {
        string = lit_to_string(vm->state, values->m_values[i]);
        strings[i] = string;
        length += string->length();
        if(joinee != nullptr)
        {
            length += joinee->length();
        }
    }
    jlen = 0;
    index = 0;
    chars = sdsempty();
    chars = sdsMakeRoomFor(chars, length + 1);
    if(joinee != nullptr)
    {
        jlen = joinee->length();
    }
    for(i = 0; i < values->m_count; i++)
    {
        string = strings[i];
        memcpy(chars + index, string->chars, string->length());
        chars = sdscatlen(chars, string->chars, string->length());
        index += string->length();
        if(joinee != nullptr)
        {
            
            //if((i+1) < values.m_count)
            {
                chars = sdscatlen(chars, joinee->chars, jlen);
            }
            index += jlen;
        }
    }
    LIT_FREE(vm->state, LitString*, strings);
    return LitString::take(vm->state, chars, length, true)->asValue();
}

static LitValue objfn_array_sort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    PCGenericArray<LitValue>* values;
    values = &LitObject::as<LitArray>(instance)->values;
    if(argc == 1 && lit_is_callable_function(argv[0]))
    {
        util_custom_quick_sort(vm, values->m_values, values->m_count, argv[0]);
    }
    else
    {
        util_basic_quick_sort(vm->state, values->m_values, values->m_count);
    }
    return instance;
}

static LitValue objfn_array_clone(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    size_t i;
    LitState* state;
    PCGenericArray<LitValue>* values;
    LitArray* array;
    PCGenericArray<LitValue>* new_values;
    state = vm->state;
    values = &LitObject::as<LitArray>(instance)->values;
    array = LitArray::make(state);
    new_values = &array->values;
    new_values->reserve(values->m_count, LitObject::NullVal);
    // lit_values_ensure_size sets the count to max of previous count (0 in this case) and new count, so we have to reset it
    new_values->m_count = 0;
    for(i = 0; i < values->m_count; i++)
    {
        new_values->push(values->m_values[i]);
    }
    return array->asValue();
}

static LitValue objfn_array_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    bool has_more;
    size_t i;
    size_t value_amount;
    size_t olength;
    char* buffer;
    LitArray* self;
    PCGenericArray<LitValue>* values;
    LitValue val;
    LitState* state;
    LitString* rt;
    LitString* part;
    LitString* stringified;
    LitString** values_converted;
    static const char* recstring = "(recursion)";
    self = LitObject::as<LitArray>(instance);
    values = &self->values;
    state = vm->state;
    if(values->m_count == 0)
    {
        return OBJECT_CONST_STRING(state, "[]");
    }
    has_more = values->m_count > LIT_CONTAINER_OUTPUT_MAX;
    value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->m_count;
    values_converted = LIT_ALLOCATE(vm->state, LitString*, value_amount+1);
    // "[ ]"
    olength = 3;
    if(has_more)
    {
        olength += 3;
    }
    buffer = sdsempty();
    buffer = sdsMakeRoomFor(buffer, olength+1);
    buffer = sdscat(buffer, "[");
    for(i = 0; i < value_amount; i++)
    {
        val = values->m_values[(has_more && i == value_amount - 1) ? values->m_count - 1 : i];
        if(LitObject::isArray(val) && (LitObject::as<LitArray>(val) == self))
        {
            stringified = LitString::copy(state, recstring, strlen(recstring));
        }
        else
        {
            stringified = lit_to_string(state, val);
        }
        part = stringified;
        buffer = sdscatlen(buffer, part->chars, part->length());
        if(has_more && i == value_amount - 2)
        {
            buffer = sdscat(buffer, " ... ");
        }
        else
        {
            if(i == (value_amount - 1))
            {
                buffer = sdscat(buffer, " ]");
            }
            else
            {
                buffer = sdscat(buffer, ", ");
            }
        }
    }
    LIT_FREE(vm->state, LitString*, values_converted);
    // should be LitString::take, but it doesn't get picked up by the GC for some reason
    //rt = LitString::take(vm->state, buffer, olength);
    rt = LitString::take(vm->state, buffer, olength, true);
    return rt->asValue();
}

static LitValue objfn_array_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue(LitObject::as<LitArray>(instance)->values.m_count);
}

void lit_open_array_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Array");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(objfn_array_constructor);
        klass->bindMethod("[]", objfn_array_subscript);
        klass->bindMethod("==", objfn_array_compare);
        klass->bindMethod("add", objfn_array_add);
        klass->bindMethod("push", objfn_array_add);
        klass->bindMethod("insert", objfn_array_insert);
        klass->bindMethod("slice", objfn_array_slice);
        klass->bindMethod("addAll", objfn_array_addall);
        klass->bindMethod("remove", objfn_array_remove);
        klass->bindMethod("removeAt", objfn_array_removeat);
        klass->bindMethod("indexOf", objfn_array_indexof);
        klass->bindMethod("contains", objfn_array_contains);
        klass->bindMethod("clear", objfn_array_clear);
        klass->bindMethod("iterator", objfn_array_iterator);
        klass->bindMethod("iteratorValue", objfn_array_iteratorvalue);
        klass->bindMethod("join", objfn_array_join);
        klass->bindMethod("sort", objfn_array_sort);
        klass->bindMethod("clone", objfn_array_clone);
        klass->bindMethod("toString", objfn_array_tostring);
        klass->setGetter("length", objfn_array_length);;
        state->arrayvalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}



static LitValue objfn_class_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return LitString::format(vm->state, "class @", AS_CLASS(instance)->name->asValue())->asValue();
}

static LitValue objfn_class_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool fields;
    int value;
    int index;
    int mthcap;
    LitClass* klass;
    (void)argc;
    LIT_ENSURE_ARGS(1);
    klass = AS_CLASS(instance);
    index = argv[0] == LitObject::NullVal ? -1 : LitObject::toNumber(argv[0]);
    mthcap = (int)klass->methods.m_capacity;
    fields = index >= mthcap;
    value = (fields ? &klass->static_fields : &klass->methods)->iterator(fields ? index - mthcap : index);
    if(value == -1)
    {
        if(fields)
        {
            return LitObject::NullVal;
        }
        index++;
        fields = true;
        value = klass->static_fields.iterator(index - mthcap);
    }
    if(value == -1)
    {
        return LitObject::NullVal;
    }
    return LitObject::toValue(fields ? value + mthcap : value);
}


static LitValue objfn_class_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    bool fields;
    size_t index;
    size_t mthcap;
    LitClass* klass;
    index = lit_check_number(vm, argv, argc, 0);
    klass = AS_CLASS(instance);
    mthcap = klass->methods.m_capacity;
    fields = index >= mthcap;
    return (fields ? &klass->static_fields : &klass->methods)->iterKey(fields ? index - mthcap : index);
}


static LitValue objfn_class_super(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* super;
    (void)vm;
    (void)argc;
    (void)argv;
    super = nullptr;
    if(LitObject::isInstance(instance))
    {
        super = LitObject::as<LitInstance>(instance)->klass->super;
    }
    else
    {
        super = AS_CLASS(instance)->super;
    }
    if(super == nullptr)
    {
        return LitObject::NullVal;
    }
    return super->asValue();
}

static LitValue objfn_class_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* klass;    
    LitValue value;
    (void)argc;
    klass = AS_CLASS(instance);
    if(argc == 2)
    {
        if(!LitObject::isString(argv[0]))
        {
            lit_runtime_error_exiting(vm, "class index must be a string");
        }

        klass->static_fields.set(LitObject::as<LitString>(argv[0]), argv[1]);
        return argv[1];
    }
    if(!LitObject::isString(argv[0]))
    {
        lit_runtime_error_exiting(vm, "class index must be a string");
    }
    if(klass->static_fields.get(LitObject::as<LitString>(argv[0]), &value))
    {
        return value;
    }
    if(klass->methods.get(LitObject::as<LitString>(argv[0]), &value))
    {
        return value;
    }
    return LitObject::NullVal;
}

static LitValue objfn_class_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* selfclass;
    LitClass* otherclass;
    (void)vm;
    (void)argc;
    if(LitObject::isClass(argv[0]))
    {
        selfclass = AS_CLASS(instance);
        otherclass = AS_CLASS(argv[0]);
        if(LitString::equal(vm->state, selfclass->name, otherclass->name))
        {
            if(selfclass == otherclass)
            {
                return LitObject::TrueVal;
            }
        }
    }
    return LitObject::FalseVal;
}

static LitValue objfn_class_name(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return AS_CLASS(instance)->name->asValue();
}

void lit_open_class_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Class");;
    {
        klass->bindMethod("[]", objfn_class_subscript);
        klass->bindMethod("==", objfn_class_compare);
        klass->bindMethod("toString", objfn_class_tostring);
        klass->setStaticMethod("toString", objfn_class_tostring);;
        klass->setStaticMethod("iterator", objfn_class_iterator);;
        klass->setStaticMethod("iteratorValue", objfn_class_iteratorvalue);;
        klass->setGetter("super", objfn_class_super);;
        klass->setStaticGetter("super", objfn_class_super);
        klass->setStaticGetter("name", objfn_class_name);
        state->classvalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
}


#if defined(__unix__) || defined(__linux__)
#endif

static bool should_update_locals;


void lit_open_libraries(LitState* state)
{
    lit_open_math_library(state);
    lit_open_file_library(state);
    lit_open_gc_library(state);
}

#if 0
#define COMPARE(state, callee, a, b) \
    ( \
    { \
        LitValue argv[2]; \
        argv[0] = a; \
        argv[1] = b; \
        LitInterpretResult r = lit_call(state, callee, argv, 2, false); \
        if(r.type != LITRESULT_OK) \
        { \
            return; \
        } \
        !LitObject::isFalsey(r.result); \
    })
#else
static LitInterpretResult COMPARE_inl(LitState* state, LitValue callee, LitValue a, LitValue b)
{
    LitValue argv[2];
    argv[0] = a;
    argv[1] = b;
    return lit_call(state, callee, argv, 2, false);
}

#define COMPARE(state, callee, a, b) \
    COMPARE_inl(state, callee, a, b)
#endif



void util_custom_quick_sort(LitVM* vm, LitValue* l, int length, LitValue callee)
{
    LitInterpretResult rt;
    LitState* state;
    if(length < 2)
    {
        return;
    }
    state = vm->state;
    int pivot_index = length / 2;
    int i;
    int j;
    LitValue pivot = l[pivot_index];
    for(i = 0, j = length - 1;; i++, j--)
    {
        //while(i < pivot_index && COMPARE(state, callee, l[i], pivot))
        while(i < pivot_index)
        {
            if((rt = COMPARE(state, callee, l[i], pivot)).type != LITRESULT_OK)
            {
                return;
            }
            if(LitObject::isFalsey(rt.result))
            {
                break;
            }
            i++;
        }
        //while(j > pivot_index && COMPARE(state, callee, pivot, l[j]))
        while(j > pivot_index)
        {
            if((rt = COMPARE(state, callee, pivot, l[j])).type != LITRESULT_OK)
            {
                return;
            }
            if(LitObject::isFalsey(rt.result))
            {
                break;
            }
            j--;
        }
        if(i >= j)
        {
            break;
        }
        LitValue tmp = l[i];
        l[i] = l[j];
        l[j] = tmp;
    }
    util_custom_quick_sort(vm, l, i, callee);
    util_custom_quick_sort(vm, l + i, length - i, callee);
}

bool util_is_fiber_done(LitFiber* fiber)
{
    return fiber->frame_count == 0 || fiber->abort;
}

void util_run_fiber(LitVM* vm, LitFiber* fiber, LitValue* argv, size_t argc, bool catcher)
{
    bool vararg;
    int i;
    int to;
    int vararg_count;
    int objfn_function_arg_count;
    LitArray* array;
    LitCallFrame* frame;
    if(util_is_fiber_done(fiber))
    {
        lit_runtime_error_exiting(vm, "Fiber already finished executing");
    }
    fiber->parent = vm->fiber;
    fiber->catcher = catcher;
    vm->fiber = fiber;
    frame = &fiber->frames[fiber->frame_count - 1];
    if(frame->ip == frame->function->chunk.code)
    {
        fiber->arg_count = argc;
        lit_ensure_fiber_stack(vm->state, fiber, frame->function->max_slots + 1 + (int)(fiber->stack_top - fiber->stack));
        frame->slots = fiber->stack_top;
        lit_push(vm, frame->function->asValue());
        vararg = frame->function->vararg;
        objfn_function_arg_count = frame->function->arg_count;
        to = objfn_function_arg_count - (vararg ? 1 : 0);
        fiber->arg_count = objfn_function_arg_count;
        for(i = 0; i < to; i++)
        {
            lit_push(vm, i < (int)argc ? argv[i] : LitObject::NullVal);
        }
        if(vararg)
        {
            array = LitArray::make(vm->state);
            lit_push(vm, array->asValue());
            vararg_count = argc - objfn_function_arg_count + 1;
            if(vararg_count > 0)
            {
                array->values.reserve(vararg_count, LitObject::NullVal);
                for(i = 0; i < vararg_count; i++)
                {
                    array->values.m_values[i] = argv[i + objfn_function_arg_count - 1];
                }
            }
        }
    }
}

static inline bool compare(LitState* state, LitValue a, LitValue b)
{
    LitValue argv[1];
    if(LitObject::isNumber(a) && LitObject::isNumber(b))
    {
        return LitObject::toNumber(a) < LitObject::toNumber(b);
    }
    argv[0] = b;
    return !LitObject::isFalsey(lit_find_and_call_method(state, a, CONST_STRING(state, "<"), argv, 1, false).result);
}

void util_basic_quick_sort(LitState* state, LitValue* clist, int length)
{
    int i;
    int j;
    int pivot_index;
    LitValue tmp;
    LitValue pivot;
    if(length < 2)
    {
        return;
    }
    pivot_index = length / 2;
    pivot = clist[pivot_index];
    for(i = 0, j = length - 1;; i++, j--)
    {
        while(i < pivot_index && compare(state, clist[i], pivot))
        {
            i++;
        }

        while(j > pivot_index && compare(state, pivot, clist[j]))
        {
            j--;
        }

        if(i >= j)
        {
            break;
        }
        tmp = clist[i];
        clist[i] = clist[j];
        clist[j] = tmp;
    }
    util_basic_quick_sort(state, clist, i);
    util_basic_quick_sort(state, clist + i, length - i);
}

bool util_interpret(LitVM* vm, LitModule* module)
{
    LitFunction* function;
    LitFiber* fiber;
    LitCallFrame* frame;
    function = module->main_function;
    fiber = lit_create_fiber(vm->state, module, function);
    fiber->parent = vm->fiber;
    vm->fiber = fiber;
    frame = &fiber->frames[fiber->frame_count - 1];
    if(frame->ip == frame->function->chunk.code)
    {
        frame->slots = fiber->stack_top;
        lit_push(vm, frame->function->asValue());
    }
    return true;
}

static bool compile_and_interpret(LitVM* vm, LitString* modname, char* source)
{
    LitModule* module;
    module = lit_compile_module(vm->state, modname, source);
    if(module == nullptr)
    {
        return false;
    }
    module->ran = true;
    return util_interpret(vm, module);
}

bool util_test_file_exists(const char* filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

#if 1
bool util_attempt_to_require(LitVM* vm, LitValue* argv, size_t argc, const char* path, bool ignore_previous, bool folders)
{
    bool rt;
    bool found;
    size_t i;
    size_t length;
    char c;
    char* source;
    char* dir_path;
    char* modname;
    char* modnamedotted;
    found = false;
    length = strlen(path);
    should_update_locals = false;
    if(path[length - 2] == '.' && path[length - 1] == '*')
    {
        if(folders)
        {
            lit_runtime_error_exiting(vm, "cannot recursively require folders");
        }
        dir_path = LIT_ALLOCATE(vm->state, char, length+1);
        dir_path[length - 2] = '\0';
        memcpy((void*)dir_path, path, length - 2);
        rt = util_attempt_to_require(vm, argv, argc, dir_path, ignore_previous, true);
        LIT_FREE(vm->state, char, dir_path);
        return rt;
    }
    //char modname[length + 5];
    modname = LIT_ALLOCATE(vm->state, char, length+5);
    //char modnamedotted[length + 5];
    modnamedotted = LIT_ALLOCATE(vm->state, char, length+5);
    memcpy(modnamedotted, path, length);
    memcpy(modnamedotted + length, ".lit", 4);
    modnamedotted[length + 4] = '\0';
    for(i = 0; i < length + 5; i++)
    {
        c = modnamedotted[i];
        if(c == '.' || c == '\\')
        {
            modname[i] = '/';
        }
        else
        {
            modname[i] = c;
        }
    }
    // You can require dirs if they have init.lit in them
    modname[length] = '\0';
    if(lit_dir_exists(modname))
    {
        if(folders)
        {
            #if defined(__unix__) || defined(__linux__)
            {
                struct dirent* ep;
                DIR* dir = opendir(modname);
                if(dir == nullptr)
                {
                    lit_runtime_error_exiting(vm, "failed to open folder '%s'", modname);
                }
                while((ep = readdir(dir)))
                {
                    if(ep->d_type == DT_REG)
                    {
                        const char* name = ep->d_name;
                        int name_length = strlen(name);
                        if(name_length > 4 && (strcmp(name + name_length - 4, ".lit") == 0 || strcmp(name + name_length - 4, ".lbc")))
                        {
                            char* dir_path;
                            dir_path = LIT_ALLOCATE(vm->state, char, length + name_length - 2);
                            dir_path[length + name_length - 3] = '\0';
                            memcpy(dir_path, path, length);
                            memcpy(dir_path + length + 1, name, name_length - 4);
                            dir_path[length] = '.';
                            if(!util_attempt_to_require(vm, argv + argc, 0, dir_path, false, false))
                            {
                                lit_runtime_error_exiting(vm, "failed to require module '%s'", name);
                            }
                            else
                            {
                                found = true;
                            }
                            LIT_FREE(vm->state, char, dir_path);
                        }
                    }
                }
                closedir(dir);
            }
            #endif
            if(!found)
            {
                lit_runtime_error_exiting(vm, "folder '%s' contains no modules that can be required", modname);
            }
            return found;
        }
        else
        {
            bool success;
            char* dir_name;
            success = false;
            dir_name = LIT_ALLOCATE(vm->state, char, length + 6);
            dir_name[length + 5] = '\0';
            memcpy(dir_name, modname, length);
            memcpy(dir_name + length, ".init", 5);
            if(util_attempt_to_require(vm, argv, argc, dir_name, ignore_previous, false))
            {
                success = true;
            }
            LIT_FREE(vm->state, char, dir_name);
            return success;
        }
    }
    else if(folders)
    {
        return false;
    }
    modname[length] = '.';
    LitString* name = LitString::copy(vm->state, modnamedotted, length);
    if(!ignore_previous)
    {
        LitValue existing_module;
        if(vm->modules->values.get(name, &existing_module))
        {
            LitModule* loaded_module = LitObject::as<LitModule>(existing_module);
            if(loaded_module->ran)
            {
                vm->fiber->stack_top -= argc;
                argv[-1] = LitObject::as<LitModule>(existing_module)->return_value;
            }
            else
            {
                if(util_interpret(vm, loaded_module))
                {
                    should_update_locals = true;
                }
            }
            return true;
        }
    }
    if(!util_test_file_exists(modname))
    {
        // .lit -> .lbc
        memcpy(modname + length + 2, "bc", 2);
        if(!util_test_file_exists(modname))
        {
            return false;
        }
    }
    source = lit_read_file(modname);
    if(source == nullptr)
    {
        return false;
    }
    if(compile_and_interpret(vm, name, source))
    {
        should_update_locals = true;
    }
    free(source);
    return true;
}


bool util_attempt_to_require_combined(LitVM* vm, LitValue* argv, size_t argc, const char* a, const char* b, bool ignore_previous)
{
    bool rt;
    size_t a_length;
    size_t b_length;
    size_t total_length;
    char* path;
    a_length = strlen(a);
    b_length = strlen(b);
    total_length = a_length + b_length + 1;
    path = LIT_ALLOCATE(vm->state, char, total_length+1);
    memcpy(path, a, a_length);
    memcpy(path + a_length + 1, b, b_length);
    path[a_length] = '.';
    path[total_length] = '\0';
    rt = util_attempt_to_require(vm, argv, argc, (const char*)&path, ignore_previous, false);
    LIT_FREE(vm->state, char, path);
    return rt;
}
#endif

LitValue util_invalid_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    lit_runtime_error_exiting(vm, "cannot create an instance of built-in type", LitObject::as<LitInstance>(instance)->klass->name);
    return LitObject::NullVal;
}

static LitValue objfn_number_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return LitString::stringNumberToString(vm->state, LitObject::toNumber(instance));
}

static LitValue objfn_number_tochar(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    char ch;
    (void)argc;
    (void)argv;
    ch = LitObject::toNumber(instance);
    return LitString::copy(vm->state, &ch, 1)->asValue();
}

static LitValue objfn_bool_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool bv;
    (void)vm;
    (void)argc;
    bv = lit_as_bool(instance);
    if(LitObject::isNull(argv[0]))
    {
        return BOOL_VALUE(false);
    }
    return BOOL_VALUE(lit_as_bool(argv[0]) == bv);
}

static LitValue objfn_bool_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_CONST_STRING(vm->state, lit_as_bool(instance) ? "true" : "false");
}

static LitValue cfn_time(LitVM* vm, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue((double)clock() / CLOCKS_PER_SEC);
}

static LitValue cfn_systemTime(LitVM* vm, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue(time(nullptr));
}

static LitValue cfn_print(LitVM* vm, size_t argc, LitValue* argv)
{
    size_t i;
    size_t written = 0;
    LitString* sv;
    written = 0;
    if(argc == 0)
    {
        return LitObject::toValue(0);
    }
    for(i = 0; i < argc; i++)
    {
        sv = lit_to_string(vm->state, argv[i]);
        written += fwrite(sv->chars, sizeof(char), sv->length(), stdout);
    }
    return LitObject::toValue(written);
}

static LitValue cfn_println(LitVM* vm, size_t argc, LitValue* argv)
{
    size_t i;
    if(argc == 0)
    {
        return LitObject::NullVal;
    }
    for(i = 0; i < argc; i++)
    {
        lit_printf(vm->state, "%s", lit_to_string(vm->state, argv[i])->chars);
    }
    lit_printf(vm->state, "\n");
    return LitObject::NullVal;
}

static bool cfn_eval(LitVM* vm, size_t argc, LitValue* argv)
{
    char* code;
    (void)argc;
    (void)argv;
    code = (char*)lit_check_string(vm, argv, argc, 0);
    return compile_and_interpret(vm, vm->fiber->module->name, code);
}

#if 0
static bool cfn_require(LitVM* vm, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    bool ignore_previous;
    size_t length;
    char* buffer;
    char* index;
    LitString* name;
    LitString* modname;
    name = lit_check_object_string(vm, argv, argc, 0);
    ignore_previous = argc > 1 && LitObject::isBool(argv[1]) && lit_as_bool(argv[1]);
    // First check, if a file with this name exists in the local path
    if(util_attempt_to_require(vm, argv, argc, name->chars, ignore_previous, false))
    {
        return should_update_locals;
    }
    // If not, we join the path of the current module to it (the path goes all the way from the root)
    modname = vm->fiber->module->name;
    // We need to get rid of the module name (test.folder.module -> test.folder)
    index = strrchr(modname->chars, '.');
    if(index != nullptr)
    {
        length = index - modname->chars;
        buffer = (char*)malloc(length + 1);
        //char buffer[length + 1];
        memcpy((void*)buffer, modname->chars, length);
        buffer[length] = '\0';
        if(util_attempt_to_require_combined(vm, argv, argc, (const char*)&buffer, name->chars, ignore_previous))
        {
            free(buffer);
            return should_update_locals;
        }
        else
        {
            free(buffer);
        }
    }
    lit_runtime_error_exiting(vm, "failed to require module '%s'", name->chars);
    return false;
}
#endif

void lit_open_string_library(LitState* state);
void lit_open_array_library(LitState* state);
void lit_open_map_library(LitState* state);
void lit_open_range_library(LitState* state);
void lit_open_fiber_library(LitState* state);
void lit_open_module_library(LitState* state);
void lit_open_function_library(LitState* state);
void lit_open_class_library(LitState* state);
void lit_open_object_library(LitState* state);


void lit_open_core_library(LitState* state)
{
    /*
    * the order here is important: class must be declared first, and object second,
    * since object derives class, and everything else derives object.
    */
    {
        lit_open_class_library(state);
        lit_open_object_library(state);
        lit_open_string_library(state);
        lit_open_array_library(state);
        lit_open_map_library(state);
        lit_open_range_library(state);
        lit_open_fiber_library(state);
        lit_open_module_library(state);
        lit_open_function_library(state);
    }
    {
        LitClass* klass = LitClass::make(state, "Number");;
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->bindMethod("toString", objfn_number_tostring);
            klass->bindMethod("toChar", objfn_number_tochar);
            klass->setGetter("chr", objfn_number_tochar);;
            state->numbervalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }
    {
        LitClass* klass = LitClass::make(state, "Bool");;
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->bindMethod("==", objfn_bool_compare);
            klass->bindMethod("toString", objfn_bool_tostring);
            state->boolvalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        };
    }
    {
        state->defineNative("time", cfn_time);
        state->defineNative("systemTime", cfn_systemTime);
        state->defineNative("print", cfn_print);
        state->defineNative("println", cfn_println);
        //state->defineNativePrimitive("require", cfn_require);
        state->defineNativePrimitive("eval", cfn_eval);
        state->setGlobal(CONST_STRING(state, "globals"), state->vm->globals->asValue());
    }
}

static LitValue objfn_fiber_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    if(argc < 1 || !LitObject::isFunction(argv[0]))
    {
        lit_runtime_error_exiting(vm, "Fiber constructor expects a function as its argument");
    }

    LitFunction* function = LitObject::as<LitFunction>(argv[0]);
    LitModule* module = vm->fiber->module;
    LitFiber* fiber = lit_create_fiber(vm->state, module, function);

    fiber->parent = vm->fiber;

    return fiber->asValue();
}


static LitValue objfn_fiber_done(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return BOOL_VALUE(util_is_fiber_done(AS_FIBER(instance)));
}


static LitValue objfn_fiber_error(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return AS_FIBER(instance)->error;
}


static LitValue objfn_fiber_current(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return vm->fiber->asValue();
}


static bool objfn_fiber_run(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    util_run_fiber(vm, AS_FIBER(instance), argv, argc, false);
    return true;
}


static bool objfn_fiber_try(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    util_run_fiber(vm, AS_FIBER(instance), argv, argc, true);
    return true;
}


static bool objfn_fiber_yield(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    if(vm->fiber->parent == nullptr)
    {
        lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was yielded") :
        lit_to_string(vm->state, argv[0]));
        return true;
    }

    LitFiber* fiber = vm->fiber;

    vm->fiber = vm->fiber->parent;
    vm->fiber->stack_top -= fiber->arg_count;
    vm->fiber->stack_top[-1] = argc == 0 ? LitObject::NullVal : lit_to_string(vm->state, argv[0])->asValue();

    argv[-1] = LitObject::NullVal;
    return true;
}


static bool objfn_fiber_yeet(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    if(vm->fiber->parent == nullptr)
    {
        lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was yeeted") :
        lit_to_string(vm->state, argv[0]));
        return true;
    }

    LitFiber* fiber = vm->fiber;

    vm->fiber = vm->fiber->parent;
    vm->fiber->stack_top -= fiber->arg_count;
    vm->fiber->stack_top[-1] = argc == 0 ? LitObject::NullVal : lit_to_string(vm->state, argv[0])->asValue();

    argv[-1] = LitObject::NullVal;
    return true;
}


static bool objfn_fiber_abort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was aborted") :
    lit_to_string(vm->state, argv[0]));
    argv[-1] = LitObject::NullVal;
    return true;
}

static LitValue objfn_fiber_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return LitString::format(vm->state, "fiber@%p", &instance)->asValue();

}

void lit_open_fiber_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Fiber");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(objfn_fiber_constructor);
        klass->bindMethod("toString", objfn_fiber_tostring);
        klass->bindPrimitive("run", objfn_fiber_run);
        klass->bindPrimitive("try", objfn_fiber_try);
        klass->setGetter("done", objfn_fiber_done);;
        klass->setGetter("error", objfn_fiber_error);;
        klass->setStaticPrimitive("yield", objfn_fiber_yield);;
        klass->setStaticPrimitive("yeet", objfn_fiber_yeet);;
        klass->setStaticPrimitive("abort", objfn_fiber_abort);;
        klass->setStaticGetter("current", objfn_fiber_current);
        state->fibervalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}



#ifdef LIT_OS_WINDOWS
    #define stat _stat
#endif

#define LITDIR_PATHSIZE 1024
#if defined(__unix__) || defined(__linux__)
    #define LITDIR_ISUNIX
#endif

#if defined(LITDIR_ISUNIX)
#else
#endif

#if defined (S_IFDIR) && !defined (S_ISDIR)
    #define	S_ISDIR(m)	(((m)&S_IFMT) == S_IFDIR)	/* directory */
#endif
#if defined (S_IFREG) && !defined (S_ISREG)
    #define	S_ISREG(m)	(((m)&S_IFMT) == S_IFREG)	/* file */
#endif

struct LitDirReader
{
    void* handle;
};

struct LitDirItem
{
    char name[LITDIR_PATHSIZE + 1];
    bool isdir;
    bool isfile;
};

bool lit_dir_open(LitDirReader* rd, const char* path)
{
    #if defined(LITDIR_ISUNIX)
        if((rd->handle = opendir(path)) == nullptr)
        {
            return false;
        }
        return true;
    #endif
    return false;
}

bool lit_dir_read(LitDirReader* rd, LitDirItem* itm)
{
    itm->isdir = false;
    itm->isfile = false;
    memset(itm->name, 0, LITDIR_PATHSIZE);
    #if defined(LITDIR_ISUNIX)
        struct dirent* ent;
        if((ent = readdir((DIR*)(rd->handle))) == nullptr)
        {
            return false;
        }
        if(ent->d_type == DT_DIR)
        {
            itm->isdir = true;
        }
        if(ent->d_type == DT_REG)
        {
            itm->isfile = true;
        }
        strcpy(itm->name, ent->d_name);
        return true;
    #endif
    return false;
}

bool lit_dir_close(LitDirReader* rd)
{
    #if defined(LITDIR_ISUNIX)
        closedir((DIR*)(rd->handle));
    #endif
    return false;
}


struct LitFileData
{
    public:
        char* path;
        FILE* handle;
        bool isopen;
};

struct LitStdioHandle
{
    FILE* handle;
    const char* name;
    bool canread;
    bool canwrite;
};

static void* LIT_INSERT_DATA(LitVM* vm, LitValue instance, size_t typsz, LitUserdata::CleanupFuncType cleanup)
{
    LitUserdata* userdata = lit_create_userdata(vm->state, typsz, false);
    userdata->cleanup_fn = cleanup;
    LitObject::as<LitInstance>(instance)->fields.set(CONST_STRING(vm->state, "_data"), userdata->asValue());
    return userdata->data;
}

static void* LIT_EXTRACT_DATA(LitVM* vm, LitValue instance)
{
    LitValue _d;
    if(!LitObject::as<LitInstance>(instance)->fields.get(CONST_STRING(vm->state, "_data"), &_d))
    {
        lit_runtime_error_exiting(vm, "failed to extract userdata");
    }
    return AS_USERDATA(_d)->data;
}

/*
 * File
 */
void cleanup_file(LitState* state, LitUserdata* data, bool mark)
{
    (void)state;
    LitFileData* fd;
    if(mark)
    {
        return;
    }
    if(data != nullptr)
    {
        fd = ((LitFileData*)data->data);
        if(fd != nullptr)
        {
            if((fd->handle != nullptr) && (fd->isopen == true))
            {
                fclose(fd->handle);
                fd->handle = nullptr;
                fd->isopen = false;
            }
        }
    }
}

static LitValue objmethod_file_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    const char* path;
    const char* mode;
    FILE* hnd;
    LitFileData* data;
    LitStdioHandle* hstd;    
    if(argc > 1)
    {
        if(LitObject::isUserdata(argv[0]))
        {
            hstd = (LitStdioHandle*)(AS_USERDATA(argv[0])->data);
            hnd = hstd->handle;
            //fprintf(stderr, "FILE: hnd=%p name=%s\n", hstd->handle, hstd->name);
            data = (LitFileData*)LIT_INSERT_DATA(vm, instance, sizeof(LitFileData), nullptr);
            data->path = nullptr;
            data->handle = hnd;
            data->isopen = true;
        }
        else
        {
            path = lit_check_string(vm, argv, argc, 0);
            mode = lit_get_string(vm, argv, argc, 1, "r");
            hnd = fopen(path, mode);
            if(hnd == nullptr)
            {
                lit_runtime_error_exiting(vm, "Failed to open file %s with mode %s (C error: %s)", path, mode, strerror(errno));
            }
            data = (LitFileData*)LIT_INSERT_DATA(vm, instance, sizeof(LitFileData), cleanup_file);
            data->path = (char*)path;
            data->handle = hnd;
            data->isopen = true;
        }
    }
    else
    {
        lit_runtime_error_exiting(vm, "File() expects either string|string, or userdata|string");
        return LitObject::NullVal;
    }
    return instance;
}


static LitValue objmethod_file_close(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    LitFileData* data;
    data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    fclose(data->handle);
    data->handle = nullptr;
    data->isopen = false;
    return LitObject::NullVal;
}

static LitValue objmethod_file_exists(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    char* file_name;
    file_name = nullptr;
    if(LitObject::isInstance(instance))
    {
        file_name = ((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->path;
    }
    else
    {
        file_name = (char*)lit_check_string(vm, argv, argc, 0);
    }
    return BOOL_VALUE(lit_file_exists(file_name));
}

/*
 * ==
 * File writing
 */

static LitValue objmethod_file_write(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1)
    size_t rt;
    LitString* value;
    value = lit_to_string(vm->state, argv[0]);
    rt = fwrite(value->chars, value->length(), 1, ((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle);
    return LitObject::toValue(rt);
}

static LitValue objmethod_file_writebyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint8_t rt;
    uint8_t byte;
    byte = (uint8_t)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, byte);
    return LitObject::toValue(rt);
}

static LitValue objmethod_file_writeshort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint16_t rt;
    uint16_t shrt;
    shrt = (uint16_t)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint16_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, shrt);
    return LitObject::toValue(rt);
}

static LitValue objmethod_file_writenumber(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint32_t rt;
    float num;
    num = (float)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint32_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, num);
    return LitObject::toValue(rt);
}

static LitValue objmethod_file_writebool(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool value;
    uint8_t rt;
    value = lit_check_bool(vm, argv, argc, 0);
    rt = lit_write_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, (uint8_t)value ? '1' : '0');
    return LitObject::toValue(rt);
}

static LitValue objmethod_file_writestring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* string;
    LitFileData* data;
    if(lit_check_string(vm, argv, argc, 0) == nullptr)
    {
        return LitObject::NullVal;
    }
    string = LitObject::as<LitString>(argv[0]);
    data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    lit_write_string(data->handle, string);
    return LitObject::NullVal;
}

/*
 * ==
 * File reading
 */

static LitValue objmethod_file_readall(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    char c;
    long length;
    long actuallength;
    LitFileData* data;
    LitString* result;
    data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    if(fseek(data->handle, 0, SEEK_END) == -1)
    {
        /*
        * cannot seek, must read each byte.
        */
        result = LitString::allocEmpty(vm->state, 0, false);
        actuallength = 0;
        while((c = fgetc(data->handle)) != EOF)
        {
            result->chars = sdscatlen(result->chars, &c, 1);
            actuallength++;
        }
    }
    else
    {
        length = ftell(data->handle);
        fseek(data->handle, 0, SEEK_SET);
        result = LitString::allocEmpty(vm->state, length, false);
        actuallength = fread(result->chars, sizeof(char), length, data->handle);
        /*
        * after reading, THIS actually sets the correct length.
        * before that, it would be 0.
        */
        sdsIncrLen(result->chars, actuallength);
    }
    result->hash = LitString::makeHash(result->chars, actuallength);
    LitString::cache(vm->state, result);
    return result->asValue();
}

static LitValue objmethod_file_readline(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    size_t max_length;
    char* line;
    LitFileData* data;
    max_length = (size_t)lit_get_number(vm, argv, argc, 0, 128);
    data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    line = LIT_ALLOCATE(vm->state, char, max_length + 1);
    if(!fgets(line, max_length, data->handle))
    {
        LIT_FREE(vm->state, char, line);
        return LitObject::NullVal;
    }
    return LitString::take(vm->state, line, strlen(line) - 1, false)->asValue();
}

static LitValue objmethod_file_readbyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return LitObject::toValue(lit_read_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
}

static LitValue objmethod_file_readshort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return LitObject::toValue(lit_read_uint16_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
}

static LitValue objmethod_file_readnumber(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return LitObject::toValue(lit_read_uint32_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
}

static LitValue objmethod_file_readbool(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return BOOL_VALUE((char)lit_read_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle) == '1');
}

static LitValue objmethod_file_readstring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    LitFileData* data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    LitString* string = lit_read_string(vm->state, data->handle);

    return string == nullptr ? LitObject::NullVal : string->asValue();
}

static LitValue objmethod_file_getlastmodified(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    struct stat buffer;
    char* file_name = nullptr;
    (void)vm;
    (void)argc;
    (void)argv;
    if(LitObject::isInstance(instance))
    {
        file_name = ((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->path;
    }
    else
    {
        file_name = (char*)lit_check_string(vm, argv, argc, 0);
    }

    if(stat(file_name, &buffer) != 0)
    {
        return LitObject::toValue(0);
    }
    #if defined(__unix__) || defined(__linux__)
    return LitObject::toValue(buffer.st_mtim.tv_sec);
    #else
        return LitObject::toValue(0);
    #endif
}


/*
* Directory
*/

static LitValue objfunction_directory_exists(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    const char* directory_name = lit_check_string(vm, argv, argc, 0);
    struct stat buffer;
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return BOOL_VALUE(stat(directory_name, &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

static LitValue objfunction_directory_listfiles(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitState* state;
    LitArray* array;
    (void)instance;
    state = vm->state;
    array = LitArray::make(state);
    #if defined(__unix__) || defined(__linux__)
    {
        struct dirent* ep;
        DIR* dir = opendir(lit_check_string(vm, argv, argc, 0));
        if(dir == nullptr)
        {
            return array->asValue();
        }
        while((ep = readdir(dir)))
        {
            if(ep->d_type == DT_REG)
            {
                array->values.push(OBJECT_CONST_STRING(state, ep->d_name));
            }
        }
        closedir(dir);
    }
    #endif
    return array->asValue();
}

static LitValue objfunction_directory_listdirs(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitArray* array;
    LitState* state;
    LitDirReader rd;
    LitDirItem ent;
    (void)instance;
    state = vm->state;
    array = LitArray::make(state);

    if(lit_dir_open(&rd, lit_check_string(vm, argv, argc, 0)))
    {
        while(true)
        {
            if(lit_dir_read(&rd, &ent))
            {
                if(ent.isdir && ((strcmp(ent.name, ".") != 0) || (strcmp(ent.name, "..") != 0)))
                {
                    array->values.push(OBJECT_CONST_STRING(state, ent.name));
                }
            }
            else
            {
                break;
            }
        }
        lit_dir_close(&rd);
    }
    return array->asValue();
}

static void free_handle(LitState* state, LitUserdata* userdata, bool mark)
{
    LitStdioHandle* hstd;
    (void)mark;
    hstd = (LitStdioHandle*)(userdata->data);
    LIT_FREE(state, LitStdioHandle, hstd);
}

static void make_handle(LitState* state, LitValue fileval, const char* name, FILE* hnd, bool canread, bool canwrite)
{
    LitUserdata* userhnd;
    LitValue args[5];
    LitString* varname;
    LitString* descname;
    LitInterpretResult res;
    LitFiber* oldfiber;
    LitStdioHandle* hstd;
    oldfiber = state->vm->fiber;
    state->vm->fiber = lit_create_fiber(state, state->last_module, nullptr);
    {
        hstd = LIT_ALLOCATE(state, LitStdioHandle, 1);
        hstd->handle = hnd;
        hstd->name = name;
        hstd->canread = canread;
        hstd->canwrite = canwrite; 
        userhnd = lit_create_userdata(state, sizeof(LitStdioHandle), true);
        userhnd->data = hstd;
        userhnd->canfree = false;
        userhnd->cleanup_fn = free_handle;
        varname = CONST_STRING(state, name);
        descname = CONST_STRING(state, name);
        args[0] = userhnd->asValue();
        args[1] = descname->asValue();
        res = lit_call(state, fileval, args, 2, false);
        //fprintf(stderr, "make_handle(%s, hnd=%p): res.type=%d, res.result=%s\n", name, hnd, res.type, lit_get_value_type(res.result));
        state->setGlobal(varname, res.result);
    }
    state->vm->fiber = oldfiber;
}

static void make_stdhandles(LitState* state)
{
    LitValue fileval;
    fileval = state->getGlobal(CONST_STRING(state, "File"));
    //fprintf(stderr, "fileval=%s\n", lit_get_value_type(fileval));
    {
        make_handle(state, fileval, "STDIN", stdin, true, false);
        make_handle(state, fileval, "STDOUT", stdout, false, true);
        make_handle(state, fileval, "STDERR", stderr, false, true);
    }
}

void lit_open_file_library(LitState* state)
{
    {
        LitClass* klass = LitClass::make(state, "File");;
        {
            klass->setStaticMethod("exists", objmethod_file_exists);;
            klass->setStaticMethod("getLastModified", objmethod_file_getlastmodified);;
            klass->bindConstructor(objmethod_file_constructor);
            klass->bindMethod("close", objmethod_file_close);
            klass->bindMethod("write", objmethod_file_write);
            klass->bindMethod("writeByte", objmethod_file_writebyte);
            klass->bindMethod("writeShort", objmethod_file_writeshort);
            klass->bindMethod("writeNumber", objmethod_file_writenumber);
            klass->bindMethod("writeBool", objmethod_file_writebool);
            klass->bindMethod("writeString", objmethod_file_writestring);
            klass->bindMethod("readAll", objmethod_file_readall);
            klass->bindMethod("readLine", objmethod_file_readline);
            klass->bindMethod("readByte", objmethod_file_readbyte);
            klass->bindMethod("readShort", objmethod_file_readshort);
            klass->bindMethod("readNumber", objmethod_file_readnumber);
            klass->bindMethod("readBool", objmethod_file_readbool);
            klass->bindMethod("readString", objmethod_file_readstring);
            klass->bindMethod("getLastModified", objmethod_file_getlastmodified);
            klass->setGetter("exists", objmethod_file_exists);;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }
    {
        LitClass* klass = LitClass::make(state, "Directory");;
        {
            klass->setStaticMethod("exists", objfunction_directory_exists);;
            klass->setStaticMethod("listFiles", objfunction_directory_listfiles);;
            klass->setStaticMethod("listDirectories", objfunction_directory_listdirs);;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

    make_stdhandles(state);
}



static LitValue objfn_function_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return lit_get_function_name(vm, instance);
}


static LitValue objfn_function_name(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return lit_get_function_name(vm, instance);
}

void lit_open_function_library(LitState* state)
{
     LitClass* klass = LitClass::make(state, "Function");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(util_invalid_constructor);
        klass->bindMethod("toString", objfn_function_tostring);
        klass->setGetter("name", objfn_function_name);;
        state->functionvalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}


static LitValue objfn_gc_memory_used(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    return LitObject::toValue(vm->state->bytes_allocated);
}

static LitValue objfn_gc_next_round(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    return LitObject::toValue(vm->state->next_gc);
}

static LitValue objfn_gc_trigger(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    int64_t collected;
    vm->state->allow_gc = true;
    collected = vm->collectGarbage();
    vm->state->allow_gc = false;

    return LitObject::toValue(collected);
}

void lit_open_gc_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "GC");;
    {
        klass->setStaticGetter("memoryUsed", objfn_gc_memory_used);
        klass->setStaticGetter("nextRound", objfn_gc_next_round);
        klass->setStaticMethod("trigger", objfn_gc_trigger);;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}

static LitValue objfn_map_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return LitMap::make(vm->state)->asValue();
}

static LitValue objfn_map_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitValue val;
    LitValue value;
    LitMap* map;
    LitString* index;
    if(!LitObject::isString(argv[0]))
    {
        lit_runtime_error_exiting(vm, "map index must be a string");
    }
    map = AS_MAP(instance);
    index = LitObject::as<LitString>(argv[0]);
    if(argc == 2)
    {
        val = argv[1];
        if(map->index_fn != nullptr)
        {
            return map->index_fn(vm, map, index, &val);
        }
        map->set(index, val);
        return val;
    }
    if(map->index_fn != nullptr)
    {
        return map->index_fn(vm, map, index, nullptr);
    }
    if(!map->values.get(index, &value))
    {
        return LitObject::NullVal;
    }
    return value;
}

static LitValue objfn_map_addall(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    if(!LitObject::isMap(argv[0]))
    {
        lit_runtime_error_exiting(vm, "expected map as the argument");
    }
    AS_MAP(argv[0])->addAll(AS_MAP(instance));
    return LitObject::NullVal;
}


static LitValue objfn_map_clear(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argv;
    (void)argc;
    AS_MAP(instance)->values.m_count = 0;
    return LitObject::NullVal;
}

static LitValue objfn_map_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    (void)vm;
    int index;
    int value;
    index = argv[0] == LitObject::NullVal ? -1 : LitObject::toNumber(argv[0]);
    value = AS_MAP(instance)->values.iterator(index);
    return value == -1 ? LitObject::NullVal : LitObject::toValue(value);
}

static LitValue objfn_map_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    index = lit_check_number(vm, argv, argc, 0);
    return AS_MAP(instance)->values.iterKey(index);
}

static LitValue objfn_map_clone(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitState* state;
    LitMap* map;
    state = vm->state;
    map = LitMap::make(state);
    AS_MAP(instance)->values.addAll(&map->values);
    return map->asValue();
}

static LitValue objfn_map_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    bool has_wrapper;
    bool has_more;
    size_t i;
    size_t index;
    size_t value_amount;
    size_t olength;
    size_t buffer_index;
    char* buffer;
    LitState* state;
    LitMap* map;
    LitTable* values;
    LitTableEntry* entry;
    LitValue field;
    LitString* strobval;
    LitString* key;
    LitString* value;
    LitString** values_converted;
    LitString** keys;
    state = vm->state;
    map = AS_MAP(instance);
    values = &map->values;
    if(values->m_count == 0)
    {
        return OBJECT_CONST_STRING(state, "{}");
    }
    has_wrapper = map->index_fn != nullptr;
    has_more = values->m_count > LIT_CONTAINER_OUTPUT_MAX;
    value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->m_count;
    values_converted = LIT_ALLOCATE(vm->state, LitString*, value_amount+1);
    keys = LIT_ALLOCATE(vm->state, LitString*, value_amount+1);
    olength = 3;
    if(has_more)
    {
        olength += SINGLE_LINE_MAPS_ENABLED ? 5 : 6;
    }
    i = 0;
    index = 0;
    do
    {
        entry = &values->m_values[index++];
        if(entry->key != nullptr)
        {
            // Special hidden key
            field = has_wrapper ? map->index_fn(vm, map, entry->key, nullptr) : entry->value;
            // This check is required to prevent infinite loops when playing with Module.privates and such
            strobval = (LitObject::isMap(field) && AS_MAP(field)->index_fn != nullptr) ? CONST_STRING(state, "map") : lit_to_string(state, field);
            state->pushRoot((LitObject*)strobval);
            values_converted[i] = strobval;
            keys[i] = entry->key;
            olength += (
                entry->key->length() + 3 + strobval->length() +
                #ifdef SINGLE_LINE_MAPS
                    (i == value_amount - 1 ? 1 : 2)
                #else
                    (i == value_amount - 1 ? 2 : 3)
                #endif
            );
            i++;
        }
    } while(i < value_amount);
    buffer = LIT_ALLOCATE(vm->state, char, olength+1);
    #ifdef SINGLE_LINE_MAPS
    memcpy(buffer, "{ ", 2);
    #else
    memcpy(buffer, "{\n", 2);
    #endif
    buffer_index = 2;
    for(i = 0; i < value_amount; i++)
    {
        key = keys[i];
        value = values_converted[i];
        #ifndef SINGLE_LINE_MAPS
        buffer[buffer_index++] = '\t';
        #endif
        memcpy(&buffer[buffer_index], key->chars, key->length());
        buffer_index += key->length();
        memcpy(&buffer[buffer_index], " = ", 3);
        buffer_index += 3;
        memcpy(&buffer[buffer_index], value->chars, value->length());
        buffer_index += value->length();
        if(has_more && i == value_amount - 1)
        {
            #ifdef SINGLE_LINE_MAPS
            memcpy(&buffer[buffer_index], ", ... }", 7);
            #else
            memcpy(&buffer[buffer_index], ",\n\t...\n}", 8);
            #endif
            buffer_index += 8;
        }
        else
        {
            #ifdef SINGLE_LINE_MAPS
            memcpy(&buffer[buffer_index], (i == value_amount - 1) ? " }" : ", ", 2);
            #else
            memcpy(&buffer[buffer_index], (i == value_amount - 1) ? "\n}" : ",\n", 2);
            #endif
            buffer_index += 2;
        }
        state->popRoot();
    }
    buffer[olength] = '\0';
    LIT_FREE(vm->state, LitString*, keys);
    LIT_FREE(vm->state, LitString*, values_converted);
    return LitString::take(vm->state, buffer, olength, false)->asValue();
}

static LitValue objfn_map_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue(AS_MAP(instance)->values.m_count);
}

void lit_open_map_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Map");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(objfn_map_constructor);
        klass->bindMethod("[]", objfn_map_subscript);
        klass->bindMethod("addAll", objfn_map_addall);
        klass->bindMethod("clear", objfn_map_clear);
        klass->bindMethod("iterator", objfn_map_iterator);
        klass->bindMethod("iteratorValue", objfn_map_iteratorvalue);
        klass->bindMethod("clone", objfn_map_clone);
        klass->bindMethod("toString", objfn_map_tostring);
        klass->setGetter("length", objfn_map_length);;
        state->mapvalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }

}

#define _USE_MATH_DEFINES

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#if !(defined(__unix__) || defined(__linux__))
    #define rand_r(v) (*v)
#endif

static LitValue math_abs(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(fabs(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_cos(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(cos(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_sin(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(sin(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_tan(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(tan(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_acos(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(acos(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_asin(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(asin(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_atan(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(atan(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_atan2(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(atan2(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
}

static LitValue math_floor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(floor(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_ceil(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(ceil(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_round(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int places;
    double value;
    (void)instance;
    value = lit_check_number(vm, argv, argc, 0);
    if(argc > 1)
    {
        places = (int)pow(10, lit_check_number(vm, argv, argc, 1));
        return LitObject::toValue(round(value * places) / places);
    }
    return LitObject::toValue(round(value));
}

static LitValue math_min(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(fmin(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
}

static LitValue math_max(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(fmax(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
}

static LitValue math_mid(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    double x;
    double y;
    double z;
    (void)instance;
    x = lit_check_number(vm, argv, argc, 0);
    y = lit_check_number(vm, argv, argc, 1);
    z = lit_check_number(vm, argv, argc, 2);
    if(x > y)
    {
        return LitObject::toValue(fmax(x, fmin(y, z)));
    }
    return LitObject::toValue(fmax(y, fmin(x, z)));
}

static LitValue math_toRadians(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(lit_check_number(vm, argv, argc, 0) * M_PI / 180.0);
}

static LitValue math_toDegrees(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(lit_check_number(vm, argv, argc, 0) * 180.0 / M_PI);
}

static LitValue math_sqrt(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(sqrt(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_log(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(exp(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_exp(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return LitObject::toValue(exp(lit_check_number(vm, argv, argc, 0)));
}

/*
 * Random
 */

static size_t static_random_data;

static size_t* extract_random_data(LitState* state, LitValue instance)
{
    if(LitObject::isClass(instance))
    {
        return &static_random_data;
    }

    LitValue data;

    if(!LitObject::as<LitInstance>(instance)->fields.get(CONST_STRING(state, "_data"), &data))
    {
        return 0;
    }

    return (size_t*)AS_USERDATA(data)->data;
}

static LitValue random_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitUserdata* userdata = lit_create_userdata(vm->state, sizeof(size_t), false);
    LitObject::as<LitInstance>(instance)->fields.set(CONST_STRING(vm->state, "_data"), userdata->asValue());

    size_t* data = (size_t*)userdata->data;

    if(argc == 1)
    {
        size_t number = (size_t)lit_check_number(vm, argv, argc, 0);
        *data = number;
    }
    else
    {
        *data = time(nullptr);
    }

    return instance;
}

static LitValue random_setSeed(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t* data = extract_random_data(vm->state, instance);

    if(argc == 1)
    {
        size_t number = (size_t)lit_check_number(vm, argv, argc, 0);
        *data = number;
    }
    else
    {
        *data = time(nullptr);
    }

    return LitObject::NullVal;
}

static LitValue random_int(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t* data = extract_random_data(vm->state, instance);

    if(argc == 1)
    {
        int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
        return LitObject::toValue(rand_r((unsigned int*)data) % bound);
    }
    else if(argc == 2)
    {
        int min = (int)lit_get_number(vm, argv, argc, 0, 0);
        int max = (int)lit_get_number(vm, argv, argc, 1, 1);

        if(max - min == 0)
        {
            return LitObject::toValue(max);
        }

        return LitObject::toValue(min + rand_r((unsigned int*)data) % (max - min));
    }

    return LitObject::toValue(rand_r((unsigned int*)data));
}

static LitValue random_float(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t* data = extract_random_data(vm->state, instance);
    double value = (double)rand_r((unsigned int*)data) / RAND_MAX;

    if(argc == 1)
    {
        int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
        return LitObject::toValue(value * bound);
    }
    else if(argc == 2)
    {
        int min = (int)lit_get_number(vm, argv, argc, 0, 0);
        int max = (int)lit_get_number(vm, argv, argc, 1, 1);

        if(max - min == 0)
        {
            return LitObject::toValue(max);
        }

        return LitObject::toValue(min + value * (max - min));
    }

    return LitObject::toValue(value);
}

static LitValue random_bool(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return BOOL_VALUE(rand_r((unsigned int*)extract_random_data(vm->state, instance)) % 2);
}

static LitValue random_chance(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    float c;
    c = lit_get_number(vm, argv, argc, 0, 50);
    if((((float)rand_r((unsigned int*)extract_random_data(vm->state, instance))) / ((float)(RAND_MAX-1)) * 100) <= c)
    {
        return LitObject::TrueVal;
    }
    return LitObject::FalseVal;
}

static LitValue random_pick(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int value = rand_r((unsigned int*)extract_random_data(vm->state, instance));

    if(argc == 1)
    {
        if(LitObject::isArray(argv[0]))
        {
            LitArray* array = LitObject::as<LitArray>(argv[0]);

            if(array->values.m_count == 0)
            {
                return LitObject::NullVal;
            }

            return array->values.m_values[value % array->values.m_count];
        }
        else if(LitObject::isMap(argv[0]))
        {
            LitMap* map = AS_MAP(argv[0]);
            size_t length = map->values.m_count;
            size_t m_capacity = map->values.m_capacity;

            if(length == 0)
            {
                return LitObject::NullVal;
            }

            size_t target = value % length;
            size_t index = 0;

            for(size_t i = 0; i < m_capacity; i++)
            {
                if(map->values.m_values[i].key != nullptr)
                {
                    if(index == target)
                    {
                        return map->values.m_values[i].value;
                    }

                    index++;
                }
            }
        }
        else
        {
            lit_runtime_error_exiting(vm, "Expected map or array as the argument");
        }
    }
    else
    {
        return argv[value % argc];
    }

    return LitObject::NullVal;
}

void lit_open_math_library(LitState* state)
{
    {
        LitClass* klass = LitClass::make(state, "Math");;
        {
            klass->static_fields.setField("Pi", LitObject::toValue(M_PI));;
            klass->static_fields.setField("Tau", LitObject::toValue(M_PI * 2));;
            klass->setStaticMethod("abs", math_abs);;
            klass->setStaticMethod("sin", math_sin);;
            klass->setStaticMethod("cos", math_cos);;
            klass->setStaticMethod("tan", math_tan);;
            klass->setStaticMethod("asin", math_asin);;
            klass->setStaticMethod("acos", math_acos);;
            klass->setStaticMethod("atan", math_atan);;
            klass->setStaticMethod("atan2", math_atan2);;
            klass->setStaticMethod("floor", math_floor);;
            klass->setStaticMethod("ceil", math_ceil);;
            klass->setStaticMethod("round", math_round);;
            klass->setStaticMethod("min", math_min);;
            klass->setStaticMethod("max", math_max);;
            klass->setStaticMethod("mid", math_mid);;
            klass->setStaticMethod("toRadians", math_toRadians);;
            klass->setStaticMethod("toDegrees", math_toDegrees);;
            klass->setStaticMethod("sqrt", math_sqrt);;
            klass->setStaticMethod("log", math_log);;
            klass->setStaticMethod("exp", math_exp);;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        };
    }
    srand(time(nullptr));
    static_random_data = time(nullptr);
    {
        LitClass* klass = LitClass::make(state, "Random");;
        {
            klass->bindConstructor(random_constructor);
            klass->bindMethod("setSeed", random_setSeed);
            klass->bindMethod("int", random_int);
            klass->bindMethod("float", random_float);
            klass->bindMethod("chance", random_chance);
            klass->bindMethod("pick", random_pick);
            klass->setStaticMethod("setSeed", random_setSeed);;
            klass->setStaticMethod("int", random_int);;
            klass->setStaticMethod("float", random_float);;
            klass->setStaticMethod("bool", random_bool);;
            klass->setStaticMethod("chance", random_chance);;
            klass->setStaticMethod("pick", random_pick);;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

}


static LitValue access_private(LitVM* vm, LitMap* map, LitString* name, LitValue* val)
{
    int index;
    LitValue value;
    LitString* id;
    LitModule* module;
    id = CONST_STRING(vm->state, "_module");
    if(!map->values.get(id, &value) || !LitObject::isModule(value))
    {
        return LitObject::NullVal;
    }
    module = LitObject::as<LitModule>(value);

    if(id == name)
    {
        return module->asValue();
    }

    if(module->private_names->values.get(name, &value))
    {
        index = (int)LitObject::toNumber(value);
        if(index > -1 && index < (int)module->private_count)
        {
            if(val != nullptr)
            {
                module->privates[index] = *val;
                return *val;
            }
            return module->privates[index];
        }
    }
    return LitObject::NullVal;
}


static LitValue objfn_module_privates(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitModule* module;
    LitMap* map;
    (void)argc;
    (void)argv;
    module = LitObject::isModule(instance) ? LitObject::as<LitModule>(instance) : vm->fiber->module;
    map = module->private_names;
    if(map->index_fn == nullptr)
    {
        map->index_fn = access_private;
        map->values.set(CONST_STRING(vm->state, "_module"), module->asValue());
    }
    return map->asValue();
}

static LitValue objfn_module_current(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return vm->fiber->module->asValue();
}

static LitValue objfn_module_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return LitString::format(vm->state, "Module @", LitObject::as<LitModule>(instance)->name->asValue())->asValue();
}

static LitValue objfn_module_name(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::as<LitModule>(instance)->name->asValue();
}

void lit_open_module_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Module");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(util_invalid_constructor);
        klass->static_fields.setField("loaded", state->vm->modules->asValue());;
        klass->setStaticGetter("privates", objfn_module_privates);
        klass->setStaticGetter("current", objfn_module_current);
        klass->bindMethod("toString", objfn_module_tostring);
        klass->setGetter("name", objfn_module_name);;
        klass->setGetter("privates", objfn_module_privates);;
        state->modulevalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}

static LitValue objfn_object_class(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return vm->state->getClassFor(instance)->asValue();
}

static LitValue objfn_object_super(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitClass* cl;
    cl = vm->state->getClassFor(instance)->super;
    if(cl == nullptr)
    {
        return LitObject::NullVal;
    }
    return cl->asValue();
}

static LitValue objfn_object_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return LitString::format(vm->state, "@ instance", vm->state->getClassFor(instance)->name->asValue())->asValue();
}

static void fillmap(LitState* state, LitMap* destmap, LitTable* fromtbl, bool includenullkeys)
{
    size_t i;
    LitString* key;
    LitValue val;
    (void)includenullkeys;
    for(i=0; i<(size_t)(fromtbl->m_count); i++)
    {
        key = fromtbl->m_values[i].key;
        if(key != nullptr)
        {
            val = fromtbl->m_values[i].value;
            destmap->set(key, val);
        }
    }
}

static LitValue objfn_object_tomap(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitMap* map;
    LitMap* minst;
    LitMap* mclass;
    LitMap* mclstatics;
    LitMap* mclmethods;
    LitInstance* inst;
    mclass = nullptr;
    if(!LitObject::isInstance(instance))
    {
        lit_runtime_error_exiting(vm, "toMap() can only be used on instances");
    }
    inst = LitObject::as<LitInstance>(instance);
    map = LitMap::make(vm->state);
    {
        minst = LitMap::make(vm->state);
        fillmap(vm->state, minst, &(inst->fields), true);
    }
    {
        mclass = LitMap::make(vm->state);
        {
            mclstatics = LitMap::make(vm->state);
            fillmap(vm->state, mclstatics, &(inst->klass->static_fields), false);
        }
        {
            mclmethods = LitMap::make(vm->state);
            fillmap(vm->state, mclmethods, &(inst->klass->methods), false);
        }
        mclass->set(CONST_STRING(vm->state, "statics"), mclstatics->asValue());
        mclass->set(CONST_STRING(vm->state, "methods"), mclmethods->asValue());
    }
    map->set(CONST_STRING(vm->state, "instance"), minst->asValue());
    map->set(CONST_STRING(vm->state, "class"), mclass->asValue());
    return map->asValue();
}

static LitValue objfn_object_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitValue value;
    LitInstance* inst;
    if(!LitObject::isInstance(instance))
    {
        lit_runtime_error_exiting(vm, "cannot modify built-in types");
    }
    inst = LitObject::as<LitInstance>(instance);
    if(argc == 2)
    {
        if(!LitObject::isString(argv[0]))
        {
            lit_runtime_error_exiting(vm, "object index must be a string");
        }
        inst->fields.set(LitObject::as<LitString>(argv[0]), argv[1]);
        return argv[1];
    }
    if(!LitObject::isString(argv[0]))
    {
        lit_runtime_error_exiting(vm, "object index must be a string");
    }
    if(inst->fields.get(LitObject::as<LitString>(argv[0]), &value))
    {
        return value;
    }
    if(inst->klass->static_fields.get(LitObject::as<LitString>(argv[0]), &value))
    {
        return value;
    }
    if(inst->klass->methods.get(LitObject::as<LitString>(argv[0]), &value))
    {
        return value;
    }
    return LitObject::NullVal;
}

static LitValue objfn_object_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    int value;
    int index;
    LitInstance* self;
    LIT_ENSURE_ARGS(1);
    self = LitObject::as<LitInstance>(instance);
    index = argv[0] == LitObject::NullVal ? -1 : LitObject::toNumber(argv[0]);
    value = self->fields.iterator(index);
    return value == -1 ? LitObject::NullVal : LitObject::toValue(value);
}


static LitValue objfn_object_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    LitInstance* self;
    index = lit_check_number(vm, argv, argc, 0);
    self = LitObject::as<LitInstance>(instance);
    return self->fields.iterKey(index);
}

void lit_open_object_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Object");;
    {
        klass->inheritFrom(state->classvalue_class);
        klass->setGetter("class", objfn_object_class);;
        klass->setGetter("super", objfn_object_super);;
        klass->bindMethod("[]", objfn_object_subscript);
        klass->bindMethod("toString", objfn_object_tostring);
        klass->bindMethod("toMap", objfn_object_tomap);
        klass->bindMethod("iterator", objfn_object_iterator);
        klass->bindMethod("iteratorValue", objfn_object_iteratorvalue);
        state->objectvalue_class = klass;
        state->objectvalue_class->super = state->classvalue_class;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}


static LitValue objfn_range_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    int number;
    LitRange* range;
    range = AS_RANGE(instance);
    number = range->from;
    (void)vm;
    (void)argc;
    if(LitObject::isNumber(argv[0]))
    {
        number = LitObject::toNumber(argv[0]);
        if((range->to > range->from) ? (number >= range->to) : (number >= range->from))
        {
            return LitObject::NullVal;
        }
        number += (((range->from - range->to) > 0) ? -1 : 1);
    }
    return LitObject::toValue(number);
}

static LitValue objfn_range_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    (void)vm;
    (void)instance;
    return argv[0];
}

static LitValue objfn_range_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitRange* range;
    range = AS_RANGE(instance);
    return LitString::format(vm->state, "Range(#, #)", range->from, range->to)->asValue();
}

static LitValue objfn_range_from(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argv;
    (void)argc;
    return LitObject::toValue(AS_RANGE(instance)->from);
}

static LitValue objfn_range_set_from(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    AS_RANGE(instance)->from = LitObject::toNumber(argv[0]);
    return argv[0];
}

static LitValue objfn_range_to(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue(AS_RANGE(instance)->to);
}

static LitValue objfn_range_set_to(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    AS_RANGE(instance)->to = LitObject::toNumber(argv[0]);
    return argv[0];
}

static LitValue objfn_range_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    LitRange* range;
    range = AS_RANGE(instance);
    return LitObject::toValue(range->to - range->from);
}

void lit_open_range_library(LitState* state)
{
    LitClass* klass = LitClass::make(state, "Range");;
    {
        klass->inheritFrom(state->objectvalue_class);
        klass->bindConstructor(util_invalid_constructor);
        klass->bindMethod("iterator", objfn_range_iterator);
        klass->bindMethod("iteratorValue", objfn_range_iteratorvalue);
        klass->bindMethod("toString", objfn_range_tostring);
        klass->bindField("from", objfn_range_from, objfn_range_set_from);
        klass->bindField("to", objfn_range_to, objfn_range_set_to);
        klass->setGetter("length", objfn_range_length);;
        state->rangevalue_class = klass;
    }
    state->setGlobal(klass->name, klass->asValue());
    if(klass->super == nullptr)
    {
        klass->inheritFrom(state->objectvalue_class);
    }
}


char* itoa(int value, char* result, int base)
{
    int tmp_value;
    char* ptr;
    char* ptr1;
    char tmp_char;
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }
    ptr = result;
    ptr1 = result;
    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
    // Apply negative sign
    if (tmp_value < 0)
    {
        *ptr++ = '-';
    }
    *ptr-- = '\0';
    while(ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

static char *int_to_string_helper(char *dest, size_t n, int x)
{
    if (n == 0)
    {
        return nullptr;
    }
    if (x <= -10)
    {
        dest = int_to_string_helper(dest, n - 1, x / 10);
        if (dest == nullptr)
        {
            return nullptr;
        }
    }
    *dest = (char) ('0' - x % 10);
    return dest + 1;
}

char *int_to_string(char *dest, size_t n, int x)
{
    char *p;
    p = dest;
    if (n == 0)
    {
        return nullptr;
    }
    n--;
    if (x < 0)
    {
        if(n == 0)
        {
            return nullptr;
        }
        n--;
        *p++ = '-';
    }
    else
    {
        x = -x;
    }
    p = int_to_string_helper(p, n, x);
    if(p == nullptr)
    {
        return nullptr;
    }
    *p = 0;
    return dest;
}


LitValue util_invalid_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv);

static LitValue objfn_string_plus(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* selfstr;
    LitString* result;
    LitValue value;
    (void)argc;
    selfstr = LitObject::as<LitString>(instance);
    value = argv[0];
    LitString* strval = nullptr;
    if(LitObject::isString(value))
    {
        strval = LitObject::as<LitString>(value);
    }
    else
    {
        strval = lit_to_string(vm->state, value);
    }
    result = LitString::allocEmpty(vm->state, selfstr->length() + strval->length(), false);
    result->append(selfstr);
    result->append(strval);
    LitString::cache(vm->state, result);
    return result->asValue();
}

static LitValue objfn_string_splice(LitVM* vm, LitString* string, int from, int to)
{
    int length;
    length = string->utfLength();
    if(from < 0)
    {
        from = length + from;
    }
    if(to < 0)
    {
        to = length + to;
    }
    from = fmax(from, 0);
    to = fmin(to, length - 1);
    if(from > to)
    {
        lit_runtime_error_exiting(vm, "String.splice argument 'from' is larger than argument 'to'");
    }
    from = LitString::utfcharOffset(string->chars, from);
    to = LitString::utfcharOffset(string->chars, to);
    return LitString::fromRange(vm->state, string, from, to - from + 1)->asValue();
}

static LitValue objfn_string_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitString* c;
    LitString* string;
    if(LitObject::isRange(argv[0]))
    {
        LitRange* range = AS_RANGE(argv[0]);
        return objfn_string_splice(vm, LitObject::as<LitString>(instance), range->from, range->to);
    }
    string = LitObject::as<LitString>(instance);
    index = lit_check_number(vm, argv, argc, 0);
    if(argc != 1)
    {
        lit_runtime_error_exiting(vm, "cannot modify strings with the subscript op");
    }
    if(index < 0)
    {
        index = string->utfLength() + index;

        if(index < 0)
        {
            return LitObject::NullVal;
        }
    }
    c = string->codePointAt(LitString::utfcharOffset(string->chars, index));
    return c == nullptr ? LitObject::NullVal : c->asValue();
}


static LitValue objfn_string_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* self;
    LitString* other;
    (void)argc;
    self = LitObject::as<LitString>(instance);
    if(LitObject::isString(argv[0]))
    {
        other = LitObject::as<LitString>(argv[0]);
        if(self->length() == other->length())
        {
            //fprintf(stderr, "string: same length(self=\"%s\" other=\"%s\")... strncmp=%d\n", self->chars, other->chars, strncmp(self->chars, other->chars, self->length));
            if(memcmp(self->chars, other->chars, self->length()) == 0)
            {
                return LitObject::TrueVal;
            }
        }
        return LitObject::FalseVal;
    }
    else if(LitObject::isNull(argv[0]))
    {
        if((self == nullptr) || LitObject::isNull(instance))
        {
            return LitObject::TrueVal;
        }
        return LitObject::FalseVal;
    }
    lit_runtime_error_exiting(vm, "can only compare string to another string or null");
    return LitObject::FalseVal;
}

static LitValue objfn_string_less(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    return BOOL_VALUE(strcmp(LitObject::as<LitString>(instance)->chars, lit_check_string(vm, argv, argc, 0)) < 0);
}

static LitValue objfn_string_greater(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    return BOOL_VALUE(strcmp(LitObject::as<LitString>(instance)->chars, lit_check_string(vm, argv, argc, 0)) > 0);
}

static LitValue objfn_string_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return instance;
}

static LitValue objfn_string_tonumber(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    double result;
    (void)vm;
    (void)argc;
    (void)argv;
    result = strtod(LitObject::as<LitString>(instance)->chars, nullptr);
    if(errno == ERANGE)
    {
        errno = 0;
        return LitObject::NullVal;
    }
    return LitObject::toValue(result);
}

static LitValue objfn_string_touppercase(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    char* buffer;
    LitString* rt;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    (void)argc;
    (void)argv;
    buffer = LIT_ALLOCATE(vm->state, char, string->length() + 1);
    for(i = 0; i < string->length(); i++)
    {
        buffer[i] = (char)toupper(string->chars[i]);
    }
    buffer[i] = 0;
    rt = LitString::take(vm->state, buffer, string->length(), false);
    return rt->asValue();
}

static LitValue objfn_string_tolowercase(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    char* buffer;
    LitString* rt;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    (void)argc;
    (void)argv;
    buffer = LIT_ALLOCATE(vm->state, char, string->length() + 1);
    for(i = 0; i < string->length(); i++)
    {
        buffer[i] = (char)tolower(string->chars[i]);
    }
    buffer[i] = 0;
    rt = LitString::take(vm->state, buffer, string->length(), false);
    return rt->asValue();
}

static LitValue objfn_string_tobyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int iv;
    LitString* selfstr;
    (void)vm;
    (void)argc;
    (void)argv;
    selfstr = LitObject::as<LitString>(instance);
    iv = LitString::utfstringDecode((const uint8_t*)selfstr->chars, selfstr->length());
    return LitObject::toValue(iv);
}

static LitValue objfn_string_contains(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool icase;
    LitString* sub;
    LitString* selfstr;
    (void)vm;
    (void)argc;
    (void)argv;
    icase = false;
    selfstr = LitObject::as<LitString>(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(argc > 1)
    {
        icase = lit_check_bool(vm, argv, argc, 1);
    }
    if(selfstr->contains(sub->chars, sdslen(sub->chars), icase))
    {
        return LitObject::TrueVal;
    }
    return LitObject::FalseVal;
}

static LitValue objfn_string_startswith(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    LitString* sub;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(sub == string)
    {
        return LitObject::TrueVal;
    }
    if(sub->length() > string->length())
    {
        return LitObject::FalseVal;
    }
    for(i = 0; i < sub->length(); i++)
    {
        if(sub->chars[i] != string->chars[i])
        {
            return LitObject::FalseVal;
        }
    }
    return LitObject::TrueVal;
}

static LitValue objfn_string_endswith(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    size_t start;
    LitString* sub;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(sub == string)
    {
        return LitObject::TrueVal;
    }
    if(sub->length() > string->length())
    {
        return LitObject::FalseVal;
    }
    start = string->length() - sub->length();
    for(i = 0; i < sub->length(); i++)
    {
        if(sub->chars[i] != string->chars[i + start])
        {
            return LitObject::FalseVal;
        }
    }
    return LitObject::TrueVal;
}


static LitValue objfn_string_replace(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    size_t buffer_length;
    size_t buffer_index;
    char* buffer;
    LitString* string;
    LitString* what;
    LitString* with;
    LIT_ENSURE_ARGS(2);
    if(!LitObject::isString(argv[0]) || !LitObject::isString(argv[1]))
    {
        lit_runtime_error_exiting(vm, "expected 2 string arguments");
    }
    string = LitObject::as<LitString>(instance);
    what = LitObject::as<LitString>(argv[0]);
    with = LitObject::as<LitString>(argv[1]);
    buffer_length = 0;
    for(i = 0; i < string->length(); i++)
    {
        if(strncmp(string->chars + i, what->chars, what->length()) == 0)
        {
            i += what->length() - 1;
            buffer_length += with->length();
        }
        else
        {
            buffer_length++;
        }
    }
    buffer_index = 0;
    buffer = LIT_ALLOCATE(vm->state, char, buffer_length+1);
    for(i = 0; i < string->length(); i++)
    {
        if(strncmp(string->chars + i, what->chars, what->length()) == 0)
        {
            memcpy(buffer + buffer_index, with->chars, with->length());
            buffer_index += with->length();
            i += what->length() - 1;
        }
        else
        {
            buffer[buffer_index] = string->chars[i];
            buffer_index++;
        }
    }
    buffer[buffer_length] = '\0';
    return LitString::take(vm->state, buffer, buffer_length, false)->asValue();
}

static LitValue objfn_string_substring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int to;
    int from;
    from = lit_check_number(vm, argv, argc, 0);
    to = lit_check_number(vm, argv, argc, 1);
    return objfn_string_splice(vm, LitObject::as<LitString>(instance), from, to);
}


static LitValue objfn_string_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return LitObject::toValue(LitObject::as<LitString>(instance)->utfLength());
}

static LitValue objfn_string_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    if(LitObject::isNull(argv[0]))
    {
        if(string->length() == 0)
        {
            return LitObject::NullVal;
        }
        return LitObject::toValue(0);
    }
    index = lit_check_number(vm, argv, argc, 0);
    if(index < 0)
    {
        return LitObject::NullVal;
    }
    do
    {
        index++;
        if(index >= (int)string->length())
        {
            return LitObject::NullVal;
        }
    } while((string->chars[index] & 0xc0) == 0x80);
    return LitObject::toValue(index);
}


static LitValue objfn_string_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint32_t index;
    LitString* string;
    string = LitObject::as<LitString>(instance);
    index = lit_check_number(vm, argv, argc, 0);
    if(index == UINT32_MAX)
    {
        return false;
    }
    return string->codePointAt(index)->asValue();
}


bool check_fmt_arg(LitVM* vm, char* buf, size_t ai, size_t argc, LitValue* argv, const char* fmttext)
{
    (void)argv;
    if(ai <= argc)
    {
        return true;
    }
    sdsfree(buf);
    lit_runtime_error_exiting(vm, "too few arguments for format flag '%s' at position %d (argc=%d)", fmttext, ai, argc);
    return false;
}

static LitValue objfn_string_format(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    char ch;
    char pch;
    char nch;
    char tmpch;
    int iv;
    size_t i;
    size_t ai;
    size_t selflen;
    LitString* selfstr;
    LitValue rtv;
    char* buf;
    (void)pch;
    selfstr = LitObject::as<LitString>(instance);
    selflen = selfstr->length();
    buf = sdsempty();
    buf = sdsMakeRoomFor(buf, selflen + 10);
    ai = 0;
    ch = -1;
    pch = -1;
    nch = -1;
    for(i=0; i<selflen; i++)
    {
        pch = ch;
        ch = selfstr->chars[i];
        if(i <= selflen)
        {
            nch = selfstr->chars[i + 1];
        }
        if(ch == '%')
        {
            if(nch == '%')
            {
                buf = sdscatlen(buf, &ch, 1);
                i += 1;
            }
            else
            {
                i++;
                switch(nch)
                {
                    case 's':
                        {
                            if(!check_fmt_arg(vm, buf, ai, argc, argv, "%s"))
                            {
                                return LitObject::NullVal;
                            }
                            buf = sdscatlen(buf, LitObject::as<LitString>(argv[ai])->chars, LitObject::as<LitString>(argv[ai])->length());
                        }
                        break;
                    case 'd':
                    case 'i':
                        {
                            if(!check_fmt_arg(vm, buf, ai, argc, argv, "%d"))
                            {
                                return LitObject::NullVal;
                            }
                            if(LitObject::isNumber(argv[ai]))
                            {
                                iv = lit_check_number(vm, argv, argc, ai);
                                buf = sdscatfmt(buf, "%i", iv);
                            }
                            break;
                        }
                        break;
                    case 'c':
                        {
                            if(!check_fmt_arg(vm, buf, ai, argc, argv, "%d"))
                            {
                                return LitObject::NullVal;
                            }
                            if(!LitObject::isNumber(argv[ai]))
                            {
                                sdsfree(buf);
                                lit_runtime_error_exiting(vm, "flag 'c' expects a number");
                            }
                            iv = lit_check_number(vm, argv, argc, ai);
                            /* TODO: both of these use the same amount of memory. but which is faster? */
                            #if 0
                                buf = sdscatfmt(buf, "%c", iv);
                            #else
                                tmpch = iv;
                                buf = sdscatlen(buf, &tmpch, 1);
                            #endif
                        }
                        break;
                    default:
                        {
                            sdsfree(buf);
                            lit_runtime_error_exiting(vm, "unrecognized formatting flag '%c'", nch);
                            return LitObject::NullVal;
                        }
                        break;
                }
                ai++;
            }
        }
        else
        {
            buf = sdscatlen(buf, &ch, 1);
        }
    }
    rtv = LitString::copy(vm->state, buf, sdslen(buf))->asValue();
    sdsfree(buf);
    return rtv;
}

void lit_open_string_library(LitState* state)
{
    {
        LitClass* klass = LitClass::make(state, "String");;
        
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->bindMethod("+", objfn_string_plus);
            klass->bindMethod("[]", objfn_string_subscript);
            klass->bindMethod("<", objfn_string_less);
            klass->bindMethod(">", objfn_string_greater);
            klass->bindMethod("==", objfn_string_compare);
            klass->bindMethod("toString", objfn_string_tostring);
            {
                klass->bindMethod("toNumber", objfn_string_tonumber);
                klass->setGetter("to_i", objfn_string_tonumber);;
            }
            /*
            * String.toUpper()
            * turns string to uppercase.
            */
            {
                klass->bindMethod("toUpperCase", objfn_string_touppercase);
                klass->bindMethod("toUpper", objfn_string_touppercase);
                klass->setGetter("upper", objfn_string_touppercase);;
            }
            /*
            * String.toLower()
            * turns string to lowercase.
            */
            {
                klass->bindMethod("toLowerCase", objfn_string_tolowercase);
                klass->bindMethod("toLower", objfn_string_tolowercase);
                klass->setGetter("lower", objfn_string_tolowercase);;
            }
            /*
            * String.toByte
            * turns string into a byte integer.
            * if the string is multi character string, then $self[0] is used.
            * i.e., "foo".toByte == 102 (equiv: "foo"[0].toByte)
            */
            {
                klass->setGetter("toByte", objfn_string_tobyte);;
            }
            //TODO: byteAt
            //LIT_BIND_METHOD(state, klass, "byteAt", objfn_string_byteat);
            /*
            * String.contains(String str[, bool icase])
            * returns true if $self contains $str.
            * if optional param $icase is true, then search is case-insensitive.
            * this is faster than, eg, "FOO".lower.contains("BLAH".lower), since
            * only the characters searched are tolower()'d.
            */
            klass->bindMethod("contains", objfn_string_contains);
            /*
            * String.startsWith(String str)
            * returns true if $self starts with $str
            */
            klass->bindMethod("startsWith", objfn_string_startswith);
            /*
            * String.endsWith(String str)
            * returns true if $self ends with $str
            */
            klass->bindMethod("endsWith", objfn_string_endswith);
            /*
            * String.replace(String find, String rep)
            * replaces every $find with $rep
            */
            klass->bindMethod("replace", objfn_string_replace);
            {
                klass->bindMethod("substring", objfn_string_substring);
                klass->bindMethod("substr", objfn_string_substring);
            }
            klass->bindMethod("iterator", objfn_string_iterator);
            klass->bindMethod("iteratorValue", objfn_string_iteratorvalue);
            klass->setGetter("length", objfn_string_length);;
            klass->bindMethod("format", objfn_string_format);
            state->stringvalue_class = klass;
        
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

}

// Used for clean up on Ctrl+C / Ctrl+Z
static LitState* repl_state;

void interupt_handler(int signal_id)
{
    (void)signal_id;
    repl_state->release();
    printf("\nExiting.\n");
    exit(0);
}

static int run_repl(LitState* state)
{
    //fprintf(stderr, "in repl...\n");
    #if defined(LIT_HAVE_READLINE)
    char* line;
    repl_state = state;
    signal(SIGINT, interupt_handler);
    //signal(SIGTSTP, interupt_handler);
    lit_set_optimization_level(LITOPTLEVEL_REPL);
    printf("lit v%s, developed by @egordorichev\n", LIT_VERSION_STRING);
    while(true)
    {
        line = readline("> ");
        if(line == nullptr)
        {
            return 0;
        }
        add_history(line);
        LitInterpretResult result = lit_interpret(state, "repl", line);
        if(result.type == LITRESULT_OK && result.result != LitObject::NullVal)
        {
            printf("%s%s%s\n", COLOR_GREEN, lit_to_string(state, result.result)->chars, COLOR_RESET);
        }
    }
    #endif
    return 0;
}

static void run_tests(LitState* state)
{
    #if defined(__unix__) || defined(__linux__)
    DIR* dir = opendir(LIT_TESTS_DIRECTORY);

    if(dir == nullptr)
    {
        fprintf(stderr, "Could not find '%s' directory\n", LIT_TESTS_DIRECTORY);
        return;
    }

    struct dirent* ep;
    struct dirent* node;

    size_t tests_dir_length = strlen(LIT_TESTS_DIRECTORY);

    while((ep = readdir(dir)))
    {
        if(ep->d_type == DT_DIR)
        {
            const char* dir_name = ep->d_name;

            if(strcmp(dir_name, "..") == 0 || strcmp(dir_name, ".") == 0)
            {
                continue;
            }

            size_t dir_name_length = strlen(dir_name);
            size_t total_length = dir_name_length + tests_dir_length + 2;

            char subdir_name[total_length];

            memcpy(subdir_name, LIT_TESTS_DIRECTORY, tests_dir_length);
            memcpy(subdir_name + tests_dir_length + 1, dir_name, dir_name_length);

            subdir_name[tests_dir_length] = '/';
            subdir_name[total_length - 1] = '\0';

            DIR* subdir = opendir(subdir_name);

            if(subdir == nullptr)
            {
                fprintf(stderr, "Failed to open tests subdirectory '%s'\n", subdir_name);
                continue;
            }

            while((node = readdir(subdir)))
            {
                if(node->d_type == DT_REG)
                {
                    const char* file_name = node->d_name;
                    size_t name_length = strlen(file_name);

                    if(name_length < 4 || memcmp(".lit", file_name + name_length - 4, 4) != 0)
                    {
                        continue;
                    }

                    char file_path[total_length + name_length + 1];

                    memcpy(file_path, subdir_name, total_length - 1);
                    memcpy(file_path + total_length, file_name, name_length);

                    file_path[total_length - 1] = '/';
                    file_path[total_length + name_length] = '\0';

                    printf("Testing %s...\n", file_path);
                    lit_interpret_file(state, file_path);
                }
            }

            closedir(subdir);
        }
    }

    closedir(dir);
    #endif
}

static void show_help()
{
    printf("lit [options] [files]\n");
    printf("    -o --output [file]  Instead of running the file the compiled bytecode will be saved.\n");
    printf(" -O[name] [string] Enables given optimization. For the list of aviable optimizations run with -Ohelp\n");
    printf(" -D[name]  Defines given symbol.\n");
    printf(" -e --eval [string] Runs the given code string.\n");
    printf(" -p --pass [args] Passes the rest of the arguments to the script.\n");
    printf(" -i --interactive Starts an interactive shell.\n");
    printf(" -d --dump  Dumps all the bytecode chunks from the given file.\n");
    printf(" -t --time  Measures and prints the compilation timings.\n");
    printf(" -c --test  Runs all tests (useful for code coverage testing).\n");
    printf(" -h --help  I wonder, what this option does.\n");
    printf(" If no code to run is provided, lit will try to run either main.lbc or main.lit and, if fails, default to an interactive shell will start.\n");
}

static void show_optimization_help()
{
    int i;
    printf(
        "Lit has a lot of optimzations.\n"
        "You can turn each one on or off or use a predefined optimization level to set them to a default value.\n"
        "The more optimizations are enabled, the longer it takes to compile, but the program should run better.\n"
        "So I recommend using low optimization for development and high optimization for release.\n"
        "To enable an optimization, run lit with argument -O[optimization], for example -Oconstant-folding.\n"
        "Using flag -Oall will enable all optimizations.\n"
        "To disable an optimization, run lit with argument -Ono-[optimization], for example -Ono-constant-folding.\n"
        "Using flag -Oall will disable all optimizations.\n"
    );
    printf("Here is a list of all supported optimizations:\n\n");
    for(i = 0; i < LITOPTSTATE_TOTAL; i++)
    {
        printf(" %s  %s\n", lit_get_optimization_name((LitOptimization)i),
               lit_get_optimization_description((LitOptimization)i));
    }
    printf("\nIf you want to use a predefined optimization level (recommended), run lit with argument -O[optimization level], for example -O1.\n\n");
    for(i = 0; i < LITOPTLEVEL_TOTAL; i++)
    {
        printf("\t-O%i\t\t%s\n", i, lit_get_optimization_level_description((LitOptimizationLevel)i));
    }
}

static bool match_arg(const char* arg, const char* a, const char* b)
{
    return strcmp(arg, a) == 0 || strcmp(arg, b) == 0;
}

int exitstate(LitState* state, LitInterpretResultType result)
{
    int64_t amount;

    amount = state->release();
    if(result != LITRESULT_COMPILE_ERROR && amount != 0)
    {
        fprintf(stderr, "gc: freed residual %i bytes\n", (int)amount);
        return LIT_EXIT_CODE_MEM_LEAK;
    }
    if(result != LITRESULT_OK)
    {
        return result == LITRESULT_RUNTIME_ERROR ? LIT_EXIT_CODE_RUNTIME_ERROR : LIT_EXIT_CODE_COMPILE_ERROR;
    }
    return 0;
}

int main(int argc, const char* argv[])
{
    int i;
    int args_left;
    int num_files_to_run;
    char c;
    bool found;
    bool dump;
    bool show_repl;
    bool evaled;
    bool showed_help;
    bool perform_tests;
    bool enable_optimization;
    size_t j;
    size_t length;
    char* file;
    char* bytecode_file;
    char* source;
    char* optimization_name;
    const char* arg_string;
    const char* string;
    const char* arg;
    const char* module_name;
    char* files_to_run[128];
    LitModule* module;
    LitInterpretResultType result;
    LitArray* arg_array;
    LitState* state;
    state = LitState::make();
    lit_open_libraries(state);
    num_files_to_run = 0;
    result = LITRESULT_OK;
    dump = false;
    for(i = 1; i < argc; i++)
    {
        arg = argv[i];
        if(arg[0] == '-')
        {
            if(match_arg(arg, "-e", "--eval") || match_arg(arg, "-o", "--output"))
            {
                // It takes an extra argument, count it or we will use it as the file name to run :P
                i++;
            }
            else if(match_arg(arg, "-p", "--pass"))
            {
                // The rest of the args go to the script, go home pls
                break;
            }
            else if(match_arg(arg, "-d", "--dump"))
            {
                dump = true;
            }
            continue;
        }
        files_to_run[num_files_to_run++] = (char*)arg;
        fprintf(stderr, "num_files_to_run=%d, (argc+1)=%d\n", num_files_to_run, argc+1);
    }
    arg_array = nullptr;
    show_repl = false;
    evaled = false;
    showed_help = false;
    perform_tests = false;
    bytecode_file = nullptr;
    for(i = 1; i < argc; i++)
    {
        args_left = argc - i - 1;
        arg = argv[i];
        if(arg[0] == '-' && arg[1] == 'D')
        {
            lit_add_definition(state, arg + 2);
        }
        else if(arg[0] == '-' && arg[1] == 'O')
        {
            enable_optimization = true;

            // -Ono-whatever
            if(memcmp((char*)(arg + 2), "no-", 3) == 0)
            {
                enable_optimization = false;
                optimization_name = (char*)(arg + 5);
            }
            else
            {
                optimization_name = (char*)(arg + 2);
            }

            if(strlen(optimization_name) == 1)
            {
                c = optimization_name[0];

                if(c >= '0' && c <= '4')
                {
                    lit_set_optimization_level((LitOptimizationLevel)(c - '0'));
                    continue;
                }
            }

            if(enable_optimization && strcmp(optimization_name, "help") == 0)
            {
                show_optimization_help();
                showed_help = true;
            }
            else if(strcmp(optimization_name, "all") == 0)
            {
                lit_set_all_optimization_enabled(enable_optimization);
            }
            else
            {
                found = false;
                // Yes I know, this is not the fastest way, and what now?
                for(j = 0; j < LITOPTSTATE_TOTAL; j++)
                {
                    if(strcmp(lit_get_optimization_name((LitOptimization)j), optimization_name) == 0)
                    {
                        found = true;
                        lit_set_optimization_enabled((LitOptimization)j, enable_optimization);

                        break;
                    }
                }
                if(!found)
                {
                    fprintf(stderr, "Unknown optimization '%s'. Run with -Ohelp for a list of all optimizations.\n", optimization_name);
                    return LIT_EXIT_CODE_ARGUMENT_ERROR;
                }
            }
        }
        else if(match_arg(arg, "-e", "--eval"))
        {
            evaled = true;
            if(args_left == 0)
            {
                fprintf(stderr, "Expected code to run for the eval argument.\n");
                //return LIT_EXIT_CODE_ARGUMENT_ERROR;
                return exitstate(state, result);
            }
            string = argv[++i];
            length = strlen(string) + 1;
            source = (char*)malloc(length + 1);
            memcpy(source, string, length);
            module_name = num_files_to_run == 0 ? "repl" : files_to_run[0];
            if(dump)
            {
                module = lit_compile_module(state, CONST_STRING(state, module_name), source);
                if(module == nullptr)
                {
                    break;
                }
                lit_disassemble_module(state, module, source);
                free(source);
            }
            else
            {
                result = lit_interpret(state, module_name, source).type;
                free(source);
                if(result != LITRESULT_OK)
                {
                    break;
                }
            }
        }
        else if(match_arg(arg, "-h", "--help"))
        {
            show_help();
            showed_help = true;
        }
        else if(match_arg(arg, "-t", "--time"))
        {
            lit_enable_compilation_time_measurement();
        }
        else if(match_arg(arg, "-i", "--interactive"))
        {
            show_repl = true;
        }
        else if(match_arg(arg, "-c", "--test"))
        {
            perform_tests = true;
        }
        else if(match_arg(arg, "-d", "--dump"))
        {
            dump = true;
        }
        else if(match_arg(arg, "-o", "--output"))
        {
            if(args_left == 0)
            {
                fprintf(stderr, "Expected file name where to save the bytecode.\n");
                //return LIT_EXIT_CODE_ARGUMENT_ERROR;
                return exitstate(state, result);
            }

            bytecode_file = (char*)argv[++i];
            lit_set_optimization_level(LITOPTLEVEL_EXTREME);
        }
        else if(match_arg(arg, "-p", "--pass"))
        {
            arg_array = LitArray::make(state);

            for(j = 0; j < (size_t)args_left; j++)
            {
                arg_string = argv[i + j + 1];
                arg_array->values.push(OBJECT_CONST_STRING(state, arg_string));
            }

            state->setGlobal(CONST_STRING(state, "args"), arg_array->asValue());
            break;
        }
        else if(arg[0] == '-')
        {
            fprintf(stderr, "Unknown argument '%s', run 'lit --help' for help.\n", arg);
            //return LIT_EXIT_CODE_ARGUMENT_ERROR;
            return exitstate(state, result);
        }
    }

    if(num_files_to_run > 0)
    {
        if(bytecode_file != nullptr)
        {
            if(!lit_compile_and_save_files(state, files_to_run, num_files_to_run, bytecode_file))
            {
                result = LITRESULT_COMPILE_ERROR;
            }
        }
        else
        {
            if(arg_array == nullptr)
            {
                arg_array = LitArray::make(state);
            }
            state->setGlobal(CONST_STRING(state, "args"), arg_array->asValue());
            for(i = 0; i < num_files_to_run; i++)
            {
                file = files_to_run[i];
                result = (dump ? lit_dump_file(state, file) : lit_interpret_file(state, file)).type;
                if(result != LITRESULT_OK)
                {
                    return exitstate(state, result);
                }
            }
        }
    }
    if(perform_tests)
    {
        run_tests(state);
        return exitstate(state, result);
    }
    if(show_repl)
    {
        run_repl(state);
    }
    else if((showed_help == false) && (evaled == false) && (num_files_to_run == 0))
    {
        run_repl(state);
    }
    return exitstate(state, result);
}
