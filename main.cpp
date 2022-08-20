
#pragma once
#include <stdbool.h>
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
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>



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

#define SIGN_BIT ((uint64_t)1 << 63u)
#define QNAN ((uint64_t)0x7ffc000000000000u)

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
    ((boolean) ? TRUE_VALUE : FALSE_VALUE)

#define FALSE_VALUE ((LitValue)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VALUE ((LitValue)(uint64_t)(QNAN | TAG_TRUE))
#define NULL_VALUE ((LitValue)(uint64_t)(QNAN | TAG_NULL))

/**
* maybe use __init__ for constructor, and __fini__ for destructor?
* need a name that clearly distinguishes these functions from others.
*/
/* name of the constructor function, if defined*/
#define LIT_NAME_CONSTRUCTOR "constructor"
/* not yet implemented: name of the destructor, if defined */
#define LIT_NAME_DESTRUCTOR "destructor"

#define OBJECT_VALUE(obj) \
    (LitValue)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

#define LIT_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define LIT_GROW_ARRAY(state, previous, type, old_count, count) \
    (type*)lit_reallocate(state, previous, sizeof(type) * (old_count), sizeof(type) * (count))

#define LIT_FREE_ARRAY(state, type, pointer, old_count) \
    lit_reallocate(state, pointer, sizeof(type) * (old_count), 0)

#define LIT_ALLOCATE(state, type, count) \
    (type*)lit_reallocate(state, NULL, 0, sizeof(type) * (count))

#define LIT_FREE(state, type, pointer) \
    lit_reallocate(state, pointer, sizeof(type), 0)

#define OBJECT_TYPE(value) \
    (lit_as_object(value)->type)

#define IS_BOOL(v) \
    (((v)&FALSE_VALUE) == FALSE_VALUE)

#define IS_NULL(v) \
    ((v) == NULL_VALUE)

#define IS_NUMBER(v) \
    (((v)&QNAN) != QNAN)

#define IS_OBJECT(v) \
    (((v) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_OBJECTS_TYPE(value, t) \
    (IS_OBJECT(value) && (lit_as_object(value) != NULL) && (lit_as_object(value)->type == t))

#define IS_STRING(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_STRING)

#define IS_FUNCTION(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_FUNCTION)

#define IS_NATIVE_FUNCTION(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_NATIVE_FUNCTION)

#define IS_NATIVE_PRIMITIVE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_NATIVE_PRIMITIVE)

#define IS_NATIVE_METHOD(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_NATIVE_METHOD)

#define IS_PRIMITIVE_METHOD(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_PRIMITIVE_METHOD)

#define IS_MODULE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_MODULE)

#define IS_CLOSURE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_CLOSURE)

#define IS_UPVALUE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_UPVALUE)

#define IS_CLASS(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_CLASS)

#define IS_INSTANCE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_INSTANCE)

#define IS_ARRAY(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_ARRAY)

#define IS_MAP(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_MAP)

#define IS_BOUND_METHOD(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_BOUND_METHOD)

#define IS_USERDATA(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_USERDATA)

#define IS_RANGE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_RANGE)

#define IS_FIELD(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_FIELD)

#define IS_REFERENCE(value) \
    IS_OBJECTS_TYPE(value, LITTYPE_REFERENCE)



#define lit_as_bool(v) ((v) == TRUE_VALUE)
#define lit_as_object(v) ((LitObject*)(uintptr_t)((v) & ~(SIGN_BIT | QNAN)))
#define lit_as_string(value) ((LitString*)lit_as_object(value))
#define lit_as_cstring(value) (lit_as_string(value)->chars)
#define AS_FUNCTION(value) ((LitFunction*)lit_as_object(value))
#define AS_NATIVE_FUNCTION(value) ((LitNativeFunction*)lit_as_object(value))
#define AS_NATIVE_PRIMITIVE(value) ((LitNativePrimFunction*)lit_as_object(value))
#define AS_NATIVE_METHOD(value) ((LitNativeMethod*)lit_as_object(value))
#define AS_PRIMITIVE_METHOD(value) ((LitPrimitiveMethod*)lit_as_object(value))
#define AS_MODULE(value) ((LitModule*)lit_as_object(value))
#define AS_CLOSURE(value) ((LitClosure*)lit_as_object(value))
#define AS_UPVALUE(value) ((LitUpvalue*)lit_as_object(value))
#define AS_CLASS(value) ((LitClass*)lit_as_object(value))
#define AS_INSTANCE(value) ((LitInstance*)lit_as_object(value))
#define AS_ARRAY(value) ((LitArray*)lit_as_object(value))
#define AS_MAP(value) ((LitMap*)lit_as_object(value))
#define AS_BOUND_METHOD(value) ((LitBoundMethod*)lit_as_object(value))
#define AS_USERDATA(value) ((LitUserdata*)lit_as_object(value))
#define AS_RANGE(value) ((LitRange*)lit_as_object(value))
#define AS_FIELD(value) ((LitField*)lit_as_object(value))
#define AS_FIBER(value) ((LitFiber*)lit_as_object(value))
#define AS_REFERENCE(value) ((LitReference*)lit_as_object(value))

#define ALLOCATE_OBJECT(state, type, objectType) (type*)lit_allocate_object(state, sizeof(type), objectType)
#define OBJECT_CONST_STRING(state, text) OBJECT_VALUE(lit_string_copy((state), (text), strlen(text)))
#define CONST_STRING(state, text) lit_string_copy((state), (text), strlen(text))

#define INTERPRET_RUNTIME_FAIL ((LitInterpretResult){ LITRESULT_INVALID, NULL_VALUE })



#define LIT_GET_FIELD(id) lit_get_field(vm->state, &AS_INSTANCE(instance)->fields, id)
#define LIT_GET_MAP_FIELD(id) lit_get_map_field(vm->state, &AS_INSTANCE(instance)->fields, id)
#define LIT_SET_FIELD(id, value) lit_set_field(vm->state, &AS_INSTANCE(instance)->fields, id, value)
#define LIT_SET_MAP_FIELD(id, value) lit_set_map_field(vm->state, &AS_INSTANCE(instance)->fields, id, value)


#define LIT_ENSURE_ARGS(count)                                                   \
    if(argc != count)                                                       \
    {                                                                            \
        lit_runtime_error(vm, "expected %i argument, got %i", count, argc); \
        return NULL_VALUE;                                                       \
    }

#define LIT_ENSURE_MIN_ARGS(count)                                                       \
    if(argc < count)                                                                \
    {                                                                                    \
        lit_runtime_error(vm, "expected minimum %i argument, got %i", count, argc); \
        return NULL_VALUE;                                                               \
    }

#define LIT_ENSURE_MAX_ARGS(count)                                                       \
    if(argc > count)                                                                \
    {                                                                                    \
        lit_runtime_error(vm, "expected maximum %i argument, got %i", count, argc); \
        return NULL_VALUE;                                                               \
    }


#define LIT_BEGIN_CLASS(name)                                               \
    {                                                                       \
        LitString* klass_name = lit_string_copy(state, name, strlen(name)); \
        LitClass* klass = lit_create_class(state, klass_name);

#define LIT_INHERIT_CLASS(super_klass)                                \
    klass->super = (LitClass*)super_klass;                            \
    if(klass->init_method == NULL)                                    \
    {                                                                 \
        klass->init_method = super_klass->init_method;                \
    }                                                                 \
    lit_table_add_all(state, &super_klass->methods, &klass->methods); \
    lit_table_add_all(state, &super_klass->static_fields, &klass->static_fields);

#define LIT_END_CLASS_IGNORING()                            \
    lit_set_global(state, klass_name, OBJECT_VALUE(klass)); \
    }


#define LIT_END_CLASS()                                     \
    lit_set_global(state, klass_name, OBJECT_VALUE(klass)); \
    if(klass->super == NULL) \
    {                                                       \
        LIT_INHERIT_CLASS(state->objectvalue_class);             \
    }                                                       \
    }


#define LIT_BIND_STATIC_METHOD(name, method)                                                                        \
    {                                                                                                               \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                 \
        lit_table_set(state, &klass->static_fields, nm, OBJECT_VALUE(lit_create_native_method(state, method, nm))); \
    }

#define LIT_BIND_STATIC_PRIMITIVE(name, method)                                                                        \
    {                                                                                                                  \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                    \
        lit_table_set(state, &klass->static_fields, nm, OBJECT_VALUE(lit_create_primitive_method(state, method, nm))); \
    }

#define LIT_SET_STATIC_FIELD(name, field)                           \
    {                                                               \
        LitString* nm = lit_string_copy(state, name, strlen(name)); \
        lit_table_set(state, &klass->static_fields, nm, field);     \
    }

#define LIT_BIND_SETTER(name, setter)                                                                                        \
    {                                                                                                                        \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                          \
        lit_table_set(state, &klass->methods, nm,                                                                            \
                      OBJECT_VALUE(lit_create_field(state, NULL, (LitObject*)lit_create_native_method(state, setter, nm)))); \
    }
#define LIT_BIND_GETTER(name, getter)                                                                                        \
    {                                                                                                                        \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                          \
        lit_table_set(state, &klass->methods, nm,                                                                            \
                      OBJECT_VALUE(lit_create_field(state, (LitObject*)lit_create_native_method(state, getter, nm), NULL))); \
    }
#define LIT_BIND_FIELD(name, getter, setter)                                                                        \
    {                                                                                                               \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                 \
        lit_table_set(state, &klass->methods, nm,                                                                   \
                      OBJECT_VALUE(lit_create_field(state, (LitObject*)lit_create_native_method(state, getter, nm), \
                                                    (LitObject*)lit_create_native_method(state, setter, nm))));     \
    }

#define LIT_BIND_STATIC_SETTER(name, setter)                                                                                 \
    {                                                                                                                        \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                          \
        lit_table_set(state, &klass->static_fields, nm,                                                                      \
                      OBJECT_VALUE(lit_create_field(state, NULL, (LitObject*)lit_create_native_method(state, setter, nm)))); \
    }
#define LIT_BIND_STATIC_GETTER(name, getter)                                                                                 \
    {                                                                                                                        \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                          \
        lit_table_set(state, &klass->static_fields, nm,                                                                      \
                      OBJECT_VALUE(lit_create_field(state, (LitObject*)lit_create_native_method(state, getter, nm), NULL))); \
    }
#define LIT_BIND_STATIC_FIELD(name, getter, setter)                                                                 \
    {                                                                                                               \
        LitString* nm = lit_string_copy(state, name, strlen(name));                                                 \
        lit_table_set(state, &klass->static_fields, nm,                                                             \
                      OBJECT_VALUE(lit_create_field(state, (LitObject*)lit_create_native_method(state, getter, nm), \
                                                    (LitObject*)lit_create_native_method(state, setter, nm))));     \
    }



typedef uint64_t LitValue;

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


enum LitExpressionType
{
    LITEXPR_LITERAL,
    LITEXPR_BINARY,
    LITEXPR_UNARY,
    LITEXPR_VAR,
    LITEXPR_ASSIGN,
    LITEXPR_CALL,
    LITEXPR_SET,
    LITEXPR_GET,
    LITEXPR_LAMBDA,
    LITEXPR_ARRAY,
    LITEXPR_OBJECT,
    LITEXPR_SUBSCRIPT,
    LITEXPR_THIS,
    LITEXPR_SUPER,
    LITEXPR_RANGE,
    LITEXPR_IF,
    LITEXPR_INTERPOLATION,
    LITEXPR_REFERENCE
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

enum LitError
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

enum LitStatementType
{
    LITSTMT_EXPRESSION,
    LITSTMT_BLOCK,
    LITSTMT_IF,
    LITSTMT_WHILE,
    LITSTMT_FOR,
    LITSTMT_VAR,
    LITSTMT_CONTINUE,
    LITSTMT_BREAK,
    LITSTMT_FUNCTION,
    LITSTMT_RETURN,
    LITSTMT_METHOD,
    LITSTMT_CLASS,
    LITSTMT_FIELD
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

enum LitObjectType
{
    LITTYPE_STRING,
    LITTYPE_FUNCTION,
    LITTYPE_NATIVE_FUNCTION,
    LITTYPE_NATIVE_PRIMITIVE,
    LITTYPE_NATIVE_METHOD,
    LITTYPE_PRIMITIVE_METHOD,
    LITTYPE_FIBER,
    LITTYPE_MODULE,
    LITTYPE_CLOSURE,
    LITTYPE_UPVALUE,
    LITTYPE_CLASS,
    LITTYPE_INSTANCE,
    LITTYPE_BOUND_METHOD,
    LITTYPE_ARRAY,
    LITTYPE_MAP,
    LITTYPE_USERDATA,
    LITTYPE_RANGE,
    LITTYPE_FIELD,
    LITTYPE_REFERENCE
};

/*
rename me:
struct LitExpression
struct LitStatement
struct LitLiteralExpr
struct LitBinaryExpr
struct LitUnaryExpr
struct LitVarExpr
struct LitAssignExpression
struct LitCallExpression
struct LitGetExpression
struct LitSetExpression
struct LitLambdaExpression
struct LitArrayExpression
struct LitObjectExpression
struct LitSubscriptExpression
struct LitThisExpression
struct LitSuperExpression
struct LitRangeExpression
struct LitIfExpression
struct LitInterpolationExpression
struct LitReferenceExpression
struct LitExpressionStatement
struct LitBlockStatement
struct LitVarStatement
struct LitIfStatement
struct LitWhileStatement
struct LitForStatement
struct LitContinueStatement
struct LitBreakStatement
struct LitFunctionStatement
struct LitReturnStatement
struct LitMethodStatement
struct LitClassStatement
struct LitFieldStatement

*/

struct /**/LitParser;
struct /**/LitExpression;
struct /**/LitWriter;
struct /**/LitState;
struct /**/LitVM;
struct /**/LitMap;
struct /**/LitUserdata;
struct /**/LitString;
struct /**/LitModule;
struct /**/LitFiber;
struct /**/LitPreprocessor;
struct /**/LitStatement;
struct /**/LitScanner;
struct /**/LitEmitter;
struct /**/LitOptimizer;

typedef LitExpression* (*LitPrefixParseFn)(LitParser*, bool);
typedef LitExpression* (*LitInfixParseFn)(LitParser*, LitExpression*, bool);

typedef LitValue (*LitNativeFunctionFn)(LitVM*, size_t, LitValue*);
typedef bool (*LitNativePrimitiveFn)(LitVM*, size_t, LitValue*);
typedef LitValue (*LitNativeMethodFn)(LitVM*, LitValue, size_t arg_count, LitValue*);
typedef bool (*LitPrimitiveMethodFn)(LitVM*, LitValue, size_t, LitValue*);
typedef LitValue (*LitMapIndexFn)(LitVM*, LitMap*, LitString*, LitValue*);
typedef void (*LitCleanupFn)(LitState*, LitUserdata*, bool mark);
typedef void (*LitErrorFn)(LitState*, const char*);
typedef void (*LitPrintFn)(LitState*, const char*);

typedef void(*LitWriterByteFN)(LitWriter*, int);
typedef void(*LitWriterStringFN)(LitWriter*, const char*, size_t);
typedef void(*LitWriterFormatFN)(LitWriter*, const char*, va_list);


/* allocate/reallocate memory. if new_size is 0, frees the pointer, and returns NULL. */
void* lit_reallocate(LitState* state, void* pointer, size_t old_size, size_t new_size);


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
        PCGenericArray()
        {
        }

        PCGenericArray(const PCGenericArray&) = delete;
        PCGenericArray& operator=(const PCGenericArray&) = delete;

        void init(LitState* state)
        {
            m_count = 0;
            m_capacity = 0;
            m_values = nullptr;
            m_state = state;
        }

        size_t capacity() const
        {
            return m_capacity;
        }

        size_t size() const
        {
            return m_count;
        }

        ElementT& at(size_t idx)
        {
            return m_values[idx];
        }

        void set(size_t idx, const ElementT& val)
        {
            m_values[idx] = val;
        }

        void push(const ElementT& value)
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
        void reserve(size_t size, const NullValT& nullval)
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

        void release(LitState*)
        {
            free(m_values);
        }

        ElementT& pop()
        {
            m_count--;
            return m_values[m_count];
        }
};



struct LitWriter
{
    LitState* state;
    /* the main pointer, that either holds a pointer to a LitString, or a FILE */
    void* uptr;

    /* if true, then uptr is a LitString, otherwise it's a FILE */
    bool stringmode;

    /* if true, and !stringmode, then calls fflush() after each i/o operations */
    bool forceflush;

    /* the callback that emits a single character */
    LitWriterByteFN fnbyte;

    /* the callback that emits a string */
    LitWriterStringFN fnstring;

    /* the callback that emits a format string (printf-style) */
    LitWriterFormatFN fnformat;
};


struct LitVariable
{
    const char* name;
    size_t length;
    int depth;
    bool constant;
    bool used;
    LitValue constant_value;
    LitStatement** declaration;
};

struct LitExpression
{
    LitExpressionType type;
    size_t line;
};

struct LitStatement
{
    LitStatementType type;
    size_t line;
};

struct LitChunk
{
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
};

struct LitTableEntry
{
    /* the key of this entry. can be NULL! */
    LitString* key;

    /* the associated value */
    LitValue value;
};


using LitTable = PCGenericArray<LitTableEntry>;

struct LitObject
{
    /* the type of this object */
    LitObjectType type;
    LitObject* next;
    bool marked;
};

struct LitString
{
    LitObject object;
    /* the hash of this string - note that it is only unique to the context! */
    uint32_t hash;
    /* this is handled by sds - use lit_string_length to get the length! */
    char* chars;
};

struct LitFunction
{
    LitObject object;
    LitChunk chunk;
    LitString* name;
    uint8_t arg_count;
    uint16_t upvalue_count;
    size_t max_slots;
    bool vararg;
    LitModule* module;
};

struct LitUpvalue
{
    LitObject object;
    LitValue* location;
    LitValue closed;
    LitUpvalue* next;
};

struct LitClosure
{
    LitObject object;
    LitFunction* function;
    LitUpvalue** upvalues;
    size_t upvalue_count;
};

struct LitNativeFunction
{
    LitObject object;
    /* the native callback for this function */
    LitNativeFunctionFn function;
    /* the name of this function */
    LitString* name;
};

struct LitNativePrimFunction
{
    LitObject object;
    LitNativePrimitiveFn function;
    LitString* name;
};

struct LitNativeMethod
{
    LitObject object;
    LitNativeMethodFn method;
    LitString* name;
};

struct LitPrimitiveMethod
{
    LitObject object;
    LitPrimitiveMethodFn method;
    LitString* name;
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

struct LitMap
{
    LitObject object;
    /* the table that holds the actual entries */
    LitTable values;
    /* the index function corresponding to operator[] */
    LitMapIndexFn index_fn;
};

struct LitModule
{
    LitObject object;
    LitValue return_value;
    LitString* name;
    LitValue* privates;
    LitMap* private_names;
    size_t private_count;
    LitFunction* main_function;
    LitFiber* main_fiber;
    bool ran;
};

struct LitFiber
{
    LitObject object;
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

struct LitClass
{
    LitObject object;
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
};

struct LitInstance
{
    LitObject object;
    /* the class that corresponds to this instance */
    LitClass* klass;
    LitTable fields;
};

struct LitBoundMethod
{
    LitObject object;
    LitValue receiver;
    LitValue method;
};

struct LitArray
{
    LitObject object;
    PCGenericArray<LitValue> values;
};

struct LitUserdata
{
    LitObject object;
    void* data;
    size_t size;
    LitCleanupFn cleanup_fn;
    bool canfree;
};

struct LitRange
{
    LitObject object;
    double from;
    double to;
};

struct LitField
{
    LitObject object;
    LitObject* getter;
    LitObject* setter;
};

struct LitReference
{
    LitObject object;
    LitValue* slot;
};

struct LitState
{
    /* how much was allocated in total? */
    int64_t bytes_allocated;
    int64_t next_gc;
    bool allow_gc;
    LitErrorFn error_fn;
    LitPrintFn print_fn;
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
};

struct LitVM
{
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
};

struct LitInterpretResult
{
    /* the result of this interpret/call attempt */
    LitInterpretResultType type;
    /* the value returned from this interpret/call attempt */
    LitValue result;
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
    LitState* state;
    bool had_error;
    bool panic_mode;
    LitToken previous;
    LitToken current;
    LitCompiler* compiler;
    uint8_t expression_root_count;
    uint8_t statement_root_count;
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
    LitState* state;
    PCGenericArray<LitVariable> variables;
    int depth;
    bool mark_used;
};

struct LitPreprocessor
{
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
};


struct LitPrivate
{
    bool initialized;
    bool constant;
};


struct LitEmitter
{
    LitState* state;
    LitChunk* chunk;
    LitCompiler* compiler;
    size_t last_line;
    size_t loop_start;
    PCGenericArray<LitPrivate> privates;
    PCGenericArray<size_t> breaks;
    PCGenericArray<size_t> continues;
    LitModule* module;
    LitString* class_name;
    bool class_has_super;
    bool previous_was_expression_statement;
    int emit_reference;
};

struct LitParseRule
{
    LitPrefixParseFn prefix;
    LitInfixParseFn infix;
    LitPrecedence precedence;
};


struct LitLiteralExpr
{
    LitExpression expression;
    LitValue value;
};

struct LitBinaryExpr
{
    LitExpression expression;
    LitExpression* left;
    LitExpression* right;
    LitTokenType op;
    bool ignore_left;
};

struct LitUnaryExpr
{
    LitExpression expression;
    LitExpression* right;
    LitTokenType op;
};

struct LitVarExpr
{
    LitExpression expression;
    const char* name;
    size_t length;
};

struct LitAssignExpression
{
    LitExpression expression;
    LitExpression* to;
    LitExpression* value;
};

struct LitCallExpression
{
    LitExpression expression;
    LitExpression* callee;
    PCGenericArray<LitExpression*> args;
    LitExpression* init;
};

struct LitGetExpression
{
    LitExpression expression;
    LitExpression* where;
    const char* name;
    size_t length;
    int jump;
    bool ignore_emit;
    bool ignore_result;
};

struct LitSetExpression
{
    LitExpression expression;
    LitExpression* where;
    const char* name;
    size_t length;
    LitExpression* value;
};

struct LitParameter
{
    const char* name;
    size_t length;
    LitExpression* default_value;
};


struct LitLambdaExpression
{
    LitExpression expression;
    PCGenericArray<LitParameter> parameters;
    LitStatement* body;
};

struct LitArrayExpression
{
    LitExpression expression;
    PCGenericArray<LitExpression*> values;
};

struct LitObjectExpression
{
    LitExpression expression;
    PCGenericArray<LitValue> keys;
    PCGenericArray<LitExpression*> values;
};

struct LitSubscriptExpression
{
    LitExpression expression;
    LitExpression* array;
    LitExpression* index;
};

struct LitThisExpression
{
    LitExpression expression;
};

struct LitSuperExpression
{
    LitExpression expression;
    LitString* method;
    bool ignore_emit;
    bool ignore_result;
};

struct LitRangeExpression
{
    LitExpression expression;
    LitExpression* from;
    LitExpression* to;
};

struct LitIfExpression
{
    LitStatement statement;
    LitExpression* condition;
    LitExpression* if_branch;
    LitExpression* else_branch;
};

struct LitInterpolationExpression
{
    LitExpression expression;
    PCGenericArray<LitExpression*> expressions;
};

struct LitReferenceExpression
{
    LitExpression expression;
    LitExpression* to;
};


struct LitExpressionStatement
{
    LitStatement statement;
    LitExpression* expression;
    bool pop;
};

struct LitBlockStatement
{
    LitStatement statement;
    PCGenericArray<LitStatement*> statements;
};

struct LitVarStatement
{
    LitStatement statement;
    const char* name;
    size_t length;
    bool constant;
    LitExpression* init;
};

struct LitIfStatement
{
    LitStatement statement;
    LitExpression* condition;
    LitStatement* if_branch;
    LitStatement* else_branch;
    PCGenericArray<LitExpression*>* elseif_conditions;
    PCGenericArray<LitStatement*>* elseif_branches;
};

struct LitWhileStatement
{
    LitStatement statement;
    LitExpression* condition;
    LitStatement* body;
};

struct LitForStatement
{
    LitStatement statement;
    LitExpression* init;
    LitStatement* var;
    LitExpression* condition;
    LitExpression* increment;
    LitStatement* body;
    bool c_style;
};

struct LitContinueStatement
{
    LitStatement statement;
};

struct LitBreakStatement
{
    LitStatement statement;
};

struct LitFunctionStatement
{
    LitStatement statement;
    const char* name;
    size_t length;
    PCGenericArray<LitParameter> parameters;
    LitStatement* body;
    bool exported;
};

struct LitReturnStatement
{
    LitStatement statement;
    LitExpression* expression;
};

struct LitMethodStatement
{
    LitStatement statement;
    LitString* name;
    PCGenericArray<LitParameter> parameters;
    LitStatement* body;
    bool is_static;
};

struct LitClassStatement
{
    LitStatement statement;
    LitString* name;
    LitString* parent;
    PCGenericArray<LitStatement*> fields;
};

struct LitFieldStatement
{
    LitStatement statement;
    LitString* name;
    LitStatement* getter;
    LitStatement* setter;
    bool is_static;
};

/**
* LitWriter stuff
*/
/*
* creates a LitWriter that writes to the given FILE.
* if $forceflush is true, then fflush() is called after each i/o operation.
*/
void lit_writer_init_file(LitState* state, LitWriter* wr, FILE* fh, bool forceflush);

/*
* creates a LitWriter that writes to a buffer.
*/
void lit_writer_init_string(LitState* state, LitWriter* wr);

/* emit a printf-style formatted string */
void lit_writer_writeformat(LitWriter* wr, const char* fmt, ...);

/* emit a string */
void lit_writer_writestringl(LitWriter* wr, const char* str, size_t len);

/* like lit_writer_writestringl, but calls strlen() on str */
void lit_writer_writestring(LitWriter* wr, const char* str);

/* emit a single byte */
void lit_writer_writebyte(LitWriter* wr, int byte);

/*
* returns a LitString* if this LitWriter was initiated via lit_writer_init_string, otherwise NULL.
*/
LitString* lit_writer_get_string(LitWriter* wr);

/**
* utility functions
*/
void util_custom_quick_sort(LitVM *vm, LitValue *l, int length, LitValue callee);
int util_table_iterator(LitTable *table, int number);
LitValue util_table_iterator_key(LitTable *table, int index);
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
/* retrieve the index of $value in this array */
int lit_array_indexof(LitArray *array, LitValue value);

/* remove the value at $index */
LitValue lit_array_removeat(LitArray *array, size_t index);

/* turn the given value to a number */
static inline double lit_value_to_number(LitValue value)
{
    return *((double*)&value);
}

/* turn a number into a value*/
static inline LitValue lit_number_to_value(double num)
{
    return *((LitValue*)&num);
}

/* is this value falsey? */
static inline bool lit_is_falsey(LitValue value)
{
    return (IS_BOOL(value) && value == FALSE_VALUE) || IS_NULL(value) || (IS_NUMBER(value) && lit_value_to_number(value) == 0);
}



void lit_init_chunk(LitState* state, LitChunk* chunk);
void lit_free_chunk(LitState* state, LitChunk* chunk);
void lit_write_chunk(LitState* state, LitChunk* chunk, uint8_t byte, uint16_t line);
size_t lit_chunk_add_constant(LitState* state, LitChunk* chunk, LitValue constant);
size_t lit_chunk_get_line(LitChunk* chunk, size_t offset);
void lit_shrink_chunk(LitState* state, LitChunk* chunk);
void lit_emit_byte(LitState* state, LitChunk* chunk, uint8_t byte);
void lit_emit_bytes(LitState* state, LitChunk* chunk, uint8_t a, uint8_t b);
void lit_emit_short(LitState* state, LitChunk* chunk, uint16_t value);

/**
* state functions
*/
/* creates a new state. */
LitState* lit_new_state();

/* frees a state, releasing associated memory. */
int64_t lit_free_state(LitState* state);

void lit_push_root(LitState* state, LitObject* object);
void lit_push_value_root(LitState* state, LitValue value);
LitValue lit_peek_root(LitState* state, uint8_t distance);
void lit_pop_root(LitState* state);
void lit_pop_roots(LitState* state, uint8_t amount);

LitClass* lit_get_class_for(LitState* state, LitValue value);

char* lit_patch_file_name(char* file_name);

/* call a function in an instance */
LitInterpretResult lit_instance_call_method(LitState* state, LitValue callee, LitString* mthname, LitValue* argv, size_t argc);
LitValue lit_instance_get_method(LitState* state, LitValue callee, LitString* mthname);

/* print a value to LitWriter */
void lit_print_value(LitState* state, LitWriter* wr, LitValue value);

/* returns the static string name of this type. does *not* represent class name, et al. just the LitValueType name! */
const char* lit_get_value_type(LitValue value);

/* releases given objects, and their subobjects. */
void lit_free_objects(LitState* state, LitObject* objects);

/* run garbage collector */
uint64_t lit_collect_garbage(LitVM* vm);

/* mark an object for collection. */
void lit_mark_object(LitVM* vm, LitObject* object);

/* mark a value for collection. */
void lit_mark_value(LitVM* vm, LitValue value);

/* free a object */
void lit_free_object(LitState* state, LitObject* object);

int lit_closest_power_of_two(int n);

void lit_init_table(LitTable* table);
bool lit_table_set(LitState* state, LitTable* table, LitString* key, LitValue value);
bool lit_table_get(LitTable* table, LitString* key, LitValue* value);
bool lit_table_get_slot(LitTable* table, LitString* key, LitValue** value);
bool lit_table_delete(LitTable* table, LitString* key);
LitString* lit_table_find_string(LitTable* table, const char* chars, size_t length, uint32_t hash);
void lit_table_add_all(LitState* state, LitTable* from, LitTable* to);
void lit_table_remove_white(LitTable* table);
void lit_mark_table(LitVM* vm, LitTable* table);
bool lit_is_callable_function(LitValue value);
LitObject* lit_allocate_object(LitState* state, size_t size, LitObjectType type);

/**
* string methods
*/
/* copy a string, creating a full newly allocated LitString. */
LitString* lit_string_copy(LitState* state, const char* chars, size_t length);

/*
* create a LitString by reusing $chars. ONLY use this if you're certain that $chars isn't being freed.
* if $wassds is true, then the sds instance is reused as-is.
*/
LitString* lit_string_take(LitState* state, char* chars, size_t length, bool wassds);

/*
* creates a formatted string. is NOT printf-compatible!
*
* #: assume number, or use toString()
* $: assume string, or use toString()
* @: value toString()-ified
*
* e.g.:
*   foo = (LitString instance) "bar"
*   lit_string_format("foo=@", foo)
*   => "foo=bar"
*
* it's extremely rudimentary, and the idea is to quickly join values.
*/
LitValue lit_string_format(LitState* state, const char* format, ...);

/* turn a given number to LitValue'd LitString. */
LitValue lit_string_number_to_string(LitState* state, double value);

/* registers a string in the string table. */
void lit_register_string(LitState* state, LitString* string);

/* get hash sum of given string */
uint32_t lit_hash_string(const char* key, size_t length);

/*
* create a new string instance.
* if $reuse is false, then a new sds-string is created, otherwise $chars is set to NULL.
* this is to avoid double-allocating, which would create a sds-instance that cannot be freed.
*/
LitString* lit_string_alloc_empty(LitState* state, size_t length, bool reuse);

/* get length of this string */
size_t lit_string_length(LitString* ls);
void lit_string_append_string(LitString* ls, const char* s, size_t len);
void lit_string_append_strobj(LitString* ls, LitString* other);
void lit_string_append_char(LitString* ls, char ch);
bool lit_string_equal(LitState* state, LitString* a, LitString* b);


LitFunction* lit_create_function(LitState* state, LitModule* module);
LitValue lit_get_function_name(LitVM* vm, LitValue instance);
LitUpvalue* lit_create_upvalue(LitState* state, LitValue* slot);
LitClosure* lit_create_closure(LitState* state, LitFunction* function);
LitNativeFunction* lit_create_native_function(LitState* state, LitNativeFunctionFn function, LitString* name);
LitNativePrimFunction* lit_create_native_primitive(LitState* state, LitNativePrimitiveFn function, LitString* name);
LitNativeMethod* lit_create_native_method(LitState* state, LitNativeMethodFn function, LitString* name);
LitPrimitiveMethod* lit_create_primitive_method(LitState* state, LitPrimitiveMethodFn method, LitString* name);
LitMap* lit_create_map(LitState* state);
bool lit_map_set(LitState* state, LitMap* map, LitString* key, LitValue value);
bool lit_map_get(LitMap* map, LitString* key, LitValue* value);
bool lit_map_delete(LitMap* map, LitString* key);
void lit_map_add_all(LitState* state, LitMap* from, LitMap* to);
LitModule* lit_create_module(LitState* state, LitString* name);
LitFiber* lit_create_fiber(LitState* state, LitModule* module, LitFunction* function);
void lit_ensure_fiber_stack(LitState* state, LitFiber* fiber, size_t needed);
LitClass* lit_create_class(LitState* state, LitString* name);
LitInstance* lit_create_instance(LitState* state, LitClass* klass);
LitBoundMethod* lit_create_bound_method(LitState* state, LitValue receiver, LitValue method);
LitArray* lit_create_array(LitState* state);
LitUserdata* lit_create_userdata(LitState* state, size_t size, bool ispointeronly);
LitRange* lit_create_range(LitState* state, double from, double to);
LitField* lit_create_field(LitState* state, LitObject* getter, LitObject* setter);
LitReference* lit_create_reference(LitState* state, LitValue* slot);

void lit_array_push(LitState* state, LitArray* array, LitValue val);


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

void lit_error(LitState* state, LitErrorType type, const char* message, ...);
void lit_printf(LitState* state, const char* message, ...);
void lit_enable_compilation_time_measurement();

void lit_init_vm(LitState* state, LitVM* vm);
void lit_free_vm(LitVM* vm);
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


void lit_init_api(LitState* state);
void lit_free_api(LitState* state);

LitValue lit_get_global(LitState* state, LitString* name);
LitFunction* lit_get_global_function(LitState* state, LitString* name);

void lit_set_global(LitState* state, LitString* name, LitValue value);
bool lit_global_exists(LitState* state, LitString* name);
void lit_define_native(LitState* state, const char* name, LitNativeFunctionFn native);
void lit_define_native_primitive(LitState* state, const char* name, LitNativePrimitiveFn native);

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
void lit_ensure_object_type(LitVM* vm, LitValue value, LitObjectType type, const char* error);


LitValue lit_get_field(LitState* state, LitTable* table, const char* name);
LitValue lit_get_map_field(LitState* state, LitMap* map, const char* name);

void lit_set_field(LitState* state, LitTable* table, const char* name, LitValue value);
void lit_set_map_field(LitState* state, LitMap* map, const char* name, LitValue value);
void lit_disassemble_module(LitState* state, LitModule* module, const char* source);
void lit_disassemble_chunk(LitState* state, LitChunk* chunk, const char* name, const char* source);
size_t lit_disassemble_instruction(LitState* state, LitChunk* chunk, size_t offset, const char* source);

void lit_trace_frame(LitFiber* fiber, LitWriter* wr);



bool lit_parse(LitParser* parser, const char* file_name, const char* source, PCGenericArray<LitStatement*>& statements);
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


int lit_decode_num_bytes(uint8_t byte);
int lit_ustring_length(LitString* string);
int lit_encode_num_bytes(int value);
int lit_ustring_decode(const uint8_t* bytes, uint32_t length);
int lit_ustring_encode(int value, uint8_t* bytes);

LitString* lit_ustring_code_point_at(LitState* state, LitString* string, uint32_t index);
LitString* lit_ustring_from_code_point(LitState* state, int value);
LitString* lit_ustring_from_range(LitState* state, LitString* source, int start, uint32_t count);

int lit_uchar_offset(char* str, int index);

static inline bool lit_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static inline bool lit_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


LitString* lit_vformat_error(LitState* state, size_t line, LitError error, va_list args);
LitString* lit_format_error(LitState* state, size_t line, LitError error, ...);

void lit_init_scanner(LitState* state, LitScanner* scanner, const char* file_name, const char* source);
LitToken lit_scan_token(LitScanner* scanner);
void lit_init_optimizer(LitState* state, LitOptimizer* optimizer);
void lit_optimize(LitOptimizer* optimizer, PCGenericArray<LitStatement*>* statements);
const char* lit_get_optimization_level_description(LitOptimizationLevel level);

bool lit_is_optimization_enabled(LitOptimization optimization);
void lit_set_optimization_enabled(LitOptimization optimization, bool enabled);
void lit_set_all_optimization_enabled(bool enabled);
void lit_set_optimization_level(LitOptimizationLevel level);

const char* lit_get_optimization_name(LitOptimization optimization);
const char* lit_get_optimization_description(LitOptimization optimization);
void lit_init_preprocessor(LitState* state, LitPreprocessor* preprocessor);
void lit_free_preprocessor(LitPreprocessor* preprocessor);
void lit_add_definition(LitState* state, const char* name);

bool lit_preprocess(LitPreprocessor* preprocessor, char* source);

void lit_free_expression(LitState* state, LitExpression* expression);
LitLiteralExpr* lit_create_literal_expression(LitState* state, size_t line, LitValue value);

LitBinaryExpr*
lit_create_binary_expression(LitState* state, size_t line, LitExpression* left, LitExpression* right, LitTokenType op);
LitUnaryExpr* lit_create_unary_expression(LitState* state, size_t line, LitExpression* right, LitTokenType op);
LitVarExpr* lit_create_var_expression(LitState* state, size_t line, const char* name, size_t length);
LitAssignExpression* lit_create_assign_expression(LitState* state, size_t line, LitExpression* to, LitExpression* value);
LitCallExpression* lit_create_call_expression(LitState* state, size_t line, LitExpression* callee);
LitGetExpression*
lit_create_get_expression(LitState* state, size_t line, LitExpression* where, const char* name, size_t length, bool questionable, bool ignore_result);
LitSetExpression*
lit_create_set_expression(LitState* state, size_t line, LitExpression* where, const char* name, size_t length, LitExpression* value);
LitLambdaExpression* lit_create_lambda_expression(LitState* state, size_t line);
LitArrayExpression* lit_create_array_expression(LitState* state, size_t line);
LitObjectExpression* lit_create_object_expression(LitState* state, size_t line);
LitSubscriptExpression* lit_create_subscript_expression(LitState* state, size_t line, LitExpression* array, LitExpression* index);
LitThisExpression* lit_create_this_expression(LitState* state, size_t line);

LitSuperExpression* lit_create_super_expression(LitState* state, size_t line, LitString* method, bool ignore_result);
LitRangeExpression* lit_create_range_expression(LitState* state, size_t line, LitExpression* from, LitExpression* to);
LitIfExpression*
lit_create_if_experssion(LitState* state, size_t line, LitExpression* condition, LitExpression* if_branch, LitExpression* else_branch);

LitInterpolationExpression* lit_create_interpolation_expression(LitState* state, size_t line);
LitReferenceExpression* lit_create_reference_expression(LitState* state, size_t line, LitExpression* to);
void lit_free_statement(LitState* state, LitStatement* statement);
LitExpressionStatement* lit_create_expression_statement(LitState* state, size_t line, LitExpression* expression);
LitBlockStatement* lit_create_block_statement(LitState* state, size_t line);
LitVarStatement* lit_create_var_statement(LitState* state, size_t line, const char* name, size_t length, LitExpression* init, bool constant);

LitIfStatement* lit_create_if_statement(LitState* state,
                                        size_t line,
                                        LitExpression* condition,
                                        LitStatement* if_branch,
                                        LitStatement* else_branch,
                                        PCGenericArray<LitExpression*>* elseif_conditions,
                                        PCGenericArray<LitStatement*>* elseif_branches);

LitWhileStatement* lit_create_while_statement(LitState* state, size_t line, LitExpression* condition, LitStatement* body);

LitForStatement* lit_create_for_statement(LitState* state,
                                          size_t line,
                                          LitExpression* init,
                                          LitStatement* var,
                                          LitExpression* condition,
                                          LitExpression* increment,
                                          LitStatement* body,
                                          bool c_style);
LitContinueStatement* lit_create_continue_statement(LitState* state, size_t line);
LitBreakStatement* lit_create_break_statement(LitState* state, size_t line);
LitFunctionStatement* lit_create_function_statement(LitState* state, size_t line, const char* name, size_t length);
LitReturnStatement* lit_create_return_statement(LitState* state, size_t line, LitExpression* expression);
LitMethodStatement* lit_create_method_statement(LitState* state, size_t line, LitString* name, bool is_static);
LitClassStatement* lit_create_class_statement(LitState* state, size_t line, LitString* name, LitString* parent);
LitFieldStatement*
lit_create_field_statement(LitState* state, size_t line, LitString* name, LitStatement* getter, LitStatement* setter, bool is_static);

PCGenericArray<LitExpression*>* lit_allocate_expressions(LitState* state);
void lit_free_allocated_expressions(LitState* state, PCGenericArray<LitExpression*>* expressions);

PCGenericArray<LitStatement*>* lit_allocate_statements(LitState* state);
void lit_free_allocated_statements(LitState* state, PCGenericArray<LitStatement*>* statements);
void lit_init_emitter(LitState* state, LitEmitter* emitter);
void lit_free_emitter(LitEmitter* emitter);

LitModule* lit_emit(LitEmitter* emitter, PCGenericArray<LitStatement*>& statements, LitString* module_name);

void lit_init_parser(LitState* state, LitParser* parser);
void lit_free_parser(LitParser* parser);


inline static void lit_bind_method(LitState* state, LitClass* klass, const char* name, LitNativeMethodFn method)
{
    LitString* nm;
    nm = lit_string_copy(state, name, strlen(name));
    lit_table_set(state, &klass->methods, nm, OBJECT_VALUE(lit_create_native_method(state, method, nm)));
}

inline static void lit_bind_primitive(LitState* state, LitClass* klass, const char* name, LitPrimitiveMethodFn method)
{
    LitString* nm;
    nm = lit_string_copy(state, name, strlen(name));
    lit_table_set(state, &klass->methods, nm, OBJECT_VALUE(lit_create_primitive_method(state, method, nm)));
}

inline static void lit_bind_constructor(LitState* state, LitClass* klass, LitNativeMethodFn method)
{
    LitString* nm;
    LitNativeMethod* m;
    nm = lit_string_copy(state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1);
    m = lit_create_native_method(state, method, nm);
    klass->init_method = (LitObject*)m;
    lit_table_set(state, &klass->methods, nm, OBJECT_VALUE(m));
}

static void free_parameters(LitState* state, PCGenericArray<LitParameter>* parameters)
{
    for(size_t i = 0; i < parameters->m_count; i++)
    {
        lit_free_expression(state, parameters->m_values[i].default_value);
    }
    parameters->release(state);
}

void free_expressions(LitState* state, PCGenericArray<LitExpression*>* expressions)
{
    if(expressions == NULL)
    {
        return;
    }
    for(size_t i = 0; i < expressions->m_count; i++)
    {
        lit_free_expression(state, expressions->m_values[i]);
    }
    expressions->release(state);
}

void internal_free_statements(LitState* state, PCGenericArray<LitStatement*>* statements)
{
    if(statements == NULL)
    {
        return;
    }

    for(size_t i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }

    statements->release(state);
}

void lit_free_expression(LitState* state, LitExpression* expression)
{
    if(expression == NULL)
    {
        return;
    }

    switch(expression->type)
    {
        case LITEXPR_LITERAL:
        {
            lit_reallocate(state, expression, sizeof(LitLiteralExpr), 0);
            break;
        }

        case LITEXPR_BINARY:
        {
            LitBinaryExpr* expr = (LitBinaryExpr*)expression;

            if(!expr->ignore_left)
            {
                lit_free_expression(state, expr->left);
            }

            lit_free_expression(state, expr->right);

            lit_reallocate(state, expression, sizeof(LitBinaryExpr), 0);
            break;
        }

        case LITEXPR_UNARY:
        {
            lit_free_expression(state, ((LitUnaryExpr*)expression)->right);
            lit_reallocate(state, expression, sizeof(LitUnaryExpr), 0);

            break;
        }

        case LITEXPR_VAR:
        {
            lit_reallocate(state, expression, sizeof(LitVarExpr), 0);
            break;
        }

        case LITEXPR_ASSIGN:
        {
            LitAssignExpression* expr = (LitAssignExpression*)expression;

            lit_free_expression(state, expr->to);
            lit_free_expression(state, expr->value);

            lit_reallocate(state, expression, sizeof(LitAssignExpression), 0);
            break;
        }

        case LITEXPR_CALL:
        {
            LitCallExpression* expr = (LitCallExpression*)expression;

            lit_free_expression(state, expr->callee);
            lit_free_expression(state, expr->init);

            free_expressions(state, &expr->args);

            lit_reallocate(state, expression, sizeof(LitCallExpression), 0);
            break;
        }

        case LITEXPR_GET:
        {
            lit_free_expression(state, ((LitGetExpression*)expression)->where);
            lit_reallocate(state, expression, sizeof(LitGetExpression), 0);
            break;
        }

        case LITEXPR_SET:
        {
            LitSetExpression* expr = (LitSetExpression*)expression;

            lit_free_expression(state, expr->where);
            lit_free_expression(state, expr->value);

            lit_reallocate(state, expression, sizeof(LitSetExpression), 0);
            break;
        }

        case LITEXPR_LAMBDA:
            {
                LitLambdaExpression* expr = (LitLambdaExpression*)expression;
                free_parameters(state, &expr->parameters);
                lit_free_statement(state, expr->body);
                lit_reallocate(state, expression, sizeof(LitLambdaExpression), 0);
            }
            break;
        case LITEXPR_ARRAY:
            {
                free_expressions(state, &((LitArrayExpression*)expression)->values);
                lit_reallocate(state, expression, sizeof(LitArrayExpression), 0);
            }
            break;
        case LITEXPR_OBJECT:
            {
                LitObjectExpression* map = (LitObjectExpression*)expression;
                map->keys.release(state);
                free_expressions(state, &map->values);
                lit_reallocate(state, expression, sizeof(LitObjectExpression), 0);
            }
            break;
        case LITEXPR_SUBSCRIPT:
            {
                LitSubscriptExpression* expr = (LitSubscriptExpression*)expression;
                lit_free_expression(state, expr->array);
                lit_free_expression(state, expr->index);
                lit_reallocate(state, expression, sizeof(LitSubscriptExpression), 0);
            }
            break;
        case LITEXPR_THIS:
            {
                lit_reallocate(state, expression, sizeof(LitThisExpression), 0);
            }
            break;
        case LITEXPR_SUPER:
            {
                lit_reallocate(state, expression, sizeof(LitSuperExpression), 0);
            }
            break;
        case LITEXPR_RANGE:
            {
                LitRangeExpression* expr = (LitRangeExpression*)expression;
                lit_free_expression(state, expr->from);
                lit_free_expression(state, expr->to);
                lit_reallocate(state, expression, sizeof(LitRangeExpression), 0);
            }
            break;
        case LITEXPR_IF:
            {
                LitIfExpression* expr = (LitIfExpression*)expression;
                lit_free_expression(state, expr->condition);
                lit_free_expression(state, expr->if_branch);
                lit_free_expression(state, expr->else_branch);
                lit_reallocate(state, expression, sizeof(LitIfExpression), 0);
            }
            break;
        case LITEXPR_INTERPOLATION:
            {
                free_expressions(state, &((LitInterpolationExpression*)expression)->expressions);
                lit_reallocate(state, expression, sizeof(LitInterpolationExpression), 0);
            }
            break;
        case LITEXPR_REFERENCE:
            {
                lit_free_expression(state, ((LitReferenceExpression*)expression)->to);
                lit_reallocate(state, expression, sizeof(LitReferenceExpression), 0);
            }
            break;
        default:
            {
                lit_error(state, COMPILE_ERROR, "Unknown expression type %d", (int)expression->type);
            }
            break;
    }
}

static LitExpression* allocate_expression(LitState* state, uint64_t line, size_t size, LitExpressionType type)
{
    LitExpression* object;
    object = (LitExpression*)lit_reallocate(state, NULL, 0, size);
    object->type = type;
    object->line = line;
    return object;
}

LitLiteralExpr* lit_create_literal_expression(LitState* state, size_t line, LitValue value)
{
    LitLiteralExpr* expression;
    expression = (LitLiteralExpr*)allocate_expression(state, line, sizeof(LitLiteralExpr), LITEXPR_LITERAL);
    expression->value = value;
    return expression;
}

LitBinaryExpr* lit_create_binary_expression(LitState* state, size_t line, LitExpression* left, LitExpression* right, LitTokenType op)
{
    LitBinaryExpr* expression;
    expression = (LitBinaryExpr*)allocate_expression(state, line, sizeof(LitBinaryExpr), LITEXPR_BINARY);
    expression->left = left;
    expression->right = right;
    expression->op = op;
    expression->ignore_left = false;
    return expression;
}

LitUnaryExpr* lit_create_unary_expression(LitState* state, size_t line, LitExpression* right, LitTokenType op)
{
    LitUnaryExpr* expression;
    expression = (LitUnaryExpr*)allocate_expression(state, line, sizeof(LitUnaryExpr), LITEXPR_UNARY);
    expression->right = right;
    expression->op = op;
    return expression;
}

LitVarExpr* lit_create_var_expression(LitState* state, size_t line, const char* name, size_t length)
{
    LitVarExpr* expression;
    expression = (LitVarExpr*)allocate_expression(state, line, sizeof(LitVarExpr), LITEXPR_VAR);
    expression->name = name;
    expression->length = length;
    return expression;
}

LitAssignExpression* lit_create_assign_expression(LitState* state, size_t line, LitExpression* to, LitExpression* value)
{
    LitAssignExpression* expression;
    expression = (LitAssignExpression*)allocate_expression(state, line, sizeof(LitAssignExpression), LITEXPR_ASSIGN);
    expression->to = to;
    expression->value = value;
    return expression;
}

LitCallExpression* lit_create_call_expression(LitState* state, size_t line, LitExpression* callee)
{
    LitCallExpression* expression;
    expression = (LitCallExpression*)allocate_expression(state, line, sizeof(LitCallExpression), LITEXPR_CALL);
    expression->callee = callee;
    expression->init = NULL;
    expression->args.init(state);
    return expression;
}

LitGetExpression* lit_create_get_expression(LitState* state, size_t line, LitExpression* where, const char* name, size_t length, bool questionable, bool ignore_result)
{
    LitGetExpression* expression;
    expression = (LitGetExpression*)allocate_expression(state, line, sizeof(LitGetExpression), LITEXPR_GET);
    expression->where = where;
    expression->name = name;
    expression->length = length;
    expression->ignore_emit = false;
    expression->jump = questionable ? 0 : -1;
    expression->ignore_result = ignore_result;
    return expression;
}

LitSetExpression* lit_create_set_expression(LitState* state, size_t line, LitExpression* where, const char* name, size_t length, LitExpression* value)
{
    LitSetExpression* expression;
    expression = (LitSetExpression*)allocate_expression(state, line, sizeof(LitSetExpression), LITEXPR_SET);
    expression->where = where;
    expression->name = name;
    expression->length = length;
    expression->value = value;
    return expression;
}

LitLambdaExpression* lit_create_lambda_expression(LitState* state, size_t line)
{
    LitLambdaExpression* expression;
    expression = (LitLambdaExpression*)allocate_expression(state, line, sizeof(LitLambdaExpression), LITEXPR_LAMBDA);
    expression->body = NULL;
    expression->parameters.init(state);
    return expression;
}

LitArrayExpression* lit_create_array_expression(LitState* state, size_t line)
{
    LitArrayExpression* expression;
    expression = (LitArrayExpression*)allocate_expression(state, line, sizeof(LitArrayExpression), LITEXPR_ARRAY);
    expression->values.init(state);
    return expression;
}

LitObjectExpression* lit_create_object_expression(LitState* state, size_t line)
{
    LitObjectExpression* expression;
    expression = (LitObjectExpression*)allocate_expression(state, line, sizeof(LitObjectExpression), LITEXPR_OBJECT);
    expression->keys.init(state);
    expression->values.init(state);
    return expression;
}

LitSubscriptExpression* lit_create_subscript_expression(LitState* state, size_t line, LitExpression* array, LitExpression* index)
{
    LitSubscriptExpression* expression;
    expression = (LitSubscriptExpression*)allocate_expression(state, line, sizeof(LitSubscriptExpression), LITEXPR_SUBSCRIPT);
    expression->array = array;
    expression->index = index;
    return expression;
}

LitThisExpression* lit_create_this_expression(LitState* state, size_t line)
{
    return (LitThisExpression*)allocate_expression(state, line, sizeof(LitThisExpression), LITEXPR_THIS);
}

LitSuperExpression* lit_create_super_expression(LitState* state, size_t line, LitString* method, bool ignore_result)
{
    LitSuperExpression* expression;
    expression = (LitSuperExpression*)allocate_expression(state, line, sizeof(LitSuperExpression), LITEXPR_SUPER);
    expression->method = method;
    expression->ignore_emit = false;
    expression->ignore_result = ignore_result;
    return expression;
}

LitRangeExpression* lit_create_range_expression(LitState* state, size_t line, LitExpression* from, LitExpression* to)
{
    LitRangeExpression* expression;
    expression = (LitRangeExpression*)allocate_expression(state, line, sizeof(LitRangeExpression), LITEXPR_RANGE);
    expression->from = from;
    expression->to = to;
    return expression;
}

LitIfExpression* lit_create_if_experssion(LitState* state, size_t line, LitExpression* condition, LitExpression* if_branch, LitExpression* else_branch)
{
    LitIfExpression* expression;
    expression = (LitIfExpression*)allocate_expression(state, line, sizeof(LitIfExpression), LITEXPR_IF);
    expression->condition = condition;
    expression->if_branch = if_branch;
    expression->else_branch = else_branch;

    return expression;
}

LitInterpolationExpression* lit_create_interpolation_expression(LitState* state, size_t line)
{
    LitInterpolationExpression* expression;
    expression = (LitInterpolationExpression*)allocate_expression(state, line, sizeof(LitInterpolationExpression), LITEXPR_INTERPOLATION);
    expression->expressions.init(state);
    return expression;
}

LitReferenceExpression* lit_create_reference_expression(LitState* state, size_t line, LitExpression* to)
{
    LitReferenceExpression* expression;
    expression = (LitReferenceExpression*)allocate_expression(state, line, sizeof(LitReferenceExpression), LITEXPR_REFERENCE);
    expression->to = to;
    return expression;
}

void lit_free_statement(LitState* state, LitStatement* statement)
{
    if(statement == NULL)
    {
        return;
    }
    switch(statement->type)
    {
        case LITSTMT_EXPRESSION:
            {
                lit_free_expression(state, ((LitExpressionStatement*)statement)->expression);
                lit_reallocate(state, statement, sizeof(LitExpressionStatement), 0);
            }
            break;
        case LITSTMT_BLOCK:
            {
                internal_free_statements(state, &((LitBlockStatement*)statement)->statements);
                lit_reallocate(state, statement, sizeof(LitBlockStatement), 0);
            }
            break;
        case LITSTMT_VAR:
            {
                lit_free_expression(state, ((LitVarStatement*)statement)->init);
                lit_reallocate(state, statement, sizeof(LitVarStatement), 0);
            }
            break;
        case LITSTMT_IF:
            {
                LitIfStatement* stmt = (LitIfStatement*)statement;
                lit_free_expression(state, stmt->condition);
                lit_free_statement(state, stmt->if_branch);
                lit_free_allocated_expressions(state, stmt->elseif_conditions);
                lit_free_allocated_statements(state, stmt->elseif_branches);
                lit_free_statement(state, stmt->else_branch);
                lit_reallocate(state, statement, sizeof(LitIfStatement), 0);
            }
            break;
        case LITSTMT_WHILE:
            {
                LitWhileStatement* stmt = (LitWhileStatement*)statement;
                lit_free_expression(state, stmt->condition);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(LitWhileStatement), 0);
            }
            break;
        case LITSTMT_FOR:
            {
                LitForStatement* stmt = (LitForStatement*)statement;
                lit_free_expression(state, stmt->increment);
                lit_free_expression(state, stmt->condition);
                lit_free_expression(state, stmt->init);

                lit_free_statement(state, stmt->var);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(LitForStatement), 0);
            }
            break;
        case LITSTMT_CONTINUE:
            {
                lit_reallocate(state, statement, sizeof(LitContinueStatement), 0);
            }
            break;
        case LITSTMT_BREAK:
            {
                lit_reallocate(state, statement, sizeof(LitBreakStatement), 0);
            }
            break;
        case LITSTMT_FUNCTION:
            {
                LitFunctionStatement* stmt = (LitFunctionStatement*)statement;
                lit_free_statement(state, stmt->body);
                free_parameters(state, &stmt->parameters);
                lit_reallocate(state, statement, sizeof(LitFunctionStatement), 0);
            }
            break;
        case LITSTMT_RETURN:
            {
                lit_free_expression(state, ((LitReturnStatement*)statement)->expression);
                lit_reallocate(state, statement, sizeof(LitReturnStatement), 0);
            }
            break;
        case LITSTMT_METHOD:
            {
                LitMethodStatement* stmt = (LitMethodStatement*)statement;
                free_parameters(state, &stmt->parameters);
                lit_free_statement(state, stmt->body);
                lit_reallocate(state, statement, sizeof(LitMethodStatement), 0);
            }
            break;
        case LITSTMT_CLASS:
            {
                internal_free_statements(state, &((LitClassStatement*)statement)->fields);
                lit_reallocate(state, statement, sizeof(LitClassStatement), 0);
            }
            break;
        case LITSTMT_FIELD:
            {
                LitFieldStatement* stmt = (LitFieldStatement*)statement;
                lit_free_statement(state, stmt->getter);
                lit_free_statement(state, stmt->setter);
                lit_reallocate(state, statement, sizeof(LitFieldStatement), 0);
            }
            break;
        default:
            {
                lit_error(state, COMPILE_ERROR, "Unknown statement type %d", (int)statement->type);
            }
            break;
    }
}

static LitStatement* allocate_statement(LitState* state, uint64_t line, size_t size, LitStatementType type)
{
    LitStatement* object;
    object = (LitStatement*)lit_reallocate(state, NULL, 0, size);
    object->type = type;
    object->line = line;
    return object;
}

LitExpressionStatement* lit_create_expression_statement(LitState* state, size_t line, LitExpression* expression)
{
    LitExpressionStatement* statement;
    statement = (LitExpressionStatement*)allocate_statement(state, line, sizeof(LitExpressionStatement), LITSTMT_EXPRESSION);
    statement->expression = expression;
    statement->pop = true;
    return statement;
}

LitBlockStatement* lit_create_block_statement(LitState* state, size_t line)
{
    LitBlockStatement* statement;
    statement = (LitBlockStatement*)allocate_statement(state, line, sizeof(LitBlockStatement), LITSTMT_BLOCK);
    statement->statements.init(state);
    return statement;
}

LitVarStatement* lit_create_var_statement(LitState* state, size_t line, const char* name, size_t length, LitExpression* init, bool constant)
{
    LitVarStatement* statement;
    statement = (LitVarStatement*)allocate_statement(state, line, sizeof(LitVarStatement), LITSTMT_VAR);
    statement->name = name;
    statement->length = length;
    statement->init = init;
    statement->constant = constant;
    return statement;
}

LitIfStatement* lit_create_if_statement(LitState* state,
                                        size_t line,
                                        LitExpression* condition,
                                        LitStatement* if_branch,
                                        LitStatement* else_branch,
                                        PCGenericArray<LitExpression*>* elseif_conditions,
                                        PCGenericArray<LitStatement*>* elseif_branches)
{
    LitIfStatement* statement;
    statement = (LitIfStatement*)allocate_statement(state, line, sizeof(LitIfStatement), LITSTMT_IF);
    statement->condition = condition;
    statement->if_branch = if_branch;
    statement->else_branch = else_branch;
    statement->elseif_conditions = elseif_conditions;
    statement->elseif_branches = elseif_branches;
    return statement;
}

LitWhileStatement* lit_create_while_statement(LitState* state, size_t line, LitExpression* condition, LitStatement* body)
{
    LitWhileStatement* statement;
    statement = (LitWhileStatement*)allocate_statement(state, line, sizeof(LitWhileStatement), LITSTMT_WHILE);
    statement->condition = condition;
    statement->body = body;
    return statement;
}

LitForStatement* lit_create_for_statement(LitState* state,
                                          size_t line,
                                          LitExpression* init,
                                          LitStatement* var,
                                          LitExpression* condition,
                                          LitExpression* increment,
                                          LitStatement* body,
                                          bool c_style)
{
    LitForStatement* statement;
    statement = (LitForStatement*)allocate_statement(state, line, sizeof(LitForStatement), LITSTMT_FOR);
    statement->init = init;
    statement->var = var;
    statement->condition = condition;
    statement->increment = increment;
    statement->body = body;
    statement->c_style = c_style;
    return statement;
}

LitContinueStatement* lit_create_continue_statement(LitState* state, size_t line)
{
    return (LitContinueStatement*)allocate_statement(state, line, sizeof(LitContinueStatement), LITSTMT_CONTINUE);
}

LitBreakStatement* lit_create_break_statement(LitState* state, size_t line)
{
    return (LitBreakStatement*)allocate_statement(state, line, sizeof(LitBreakStatement), LITSTMT_BREAK);
}

LitFunctionStatement* lit_create_function_statement(LitState* state, size_t line, const char* name, size_t length)
{
    LitFunctionStatement* function;
    function = (LitFunctionStatement*)allocate_statement(state, line, sizeof(LitFunctionStatement), LITSTMT_FUNCTION);
    function->name = name;
    function->length = length;
    function->body = NULL;
    function->parameters.init(state);
    return function;
}

LitReturnStatement* lit_create_return_statement(LitState* state, size_t line, LitExpression* expression)
{
    LitReturnStatement* statement;
    statement = (LitReturnStatement*)allocate_statement(state, line, sizeof(LitReturnStatement), LITSTMT_RETURN);
    statement->expression = expression;
    return statement;
}

LitMethodStatement* lit_create_method_statement(LitState* state, size_t line, LitString* name, bool is_static)
{
    LitMethodStatement* statement;
    statement = (LitMethodStatement*)allocate_statement(state, line, sizeof(LitMethodStatement), LITSTMT_METHOD);
    statement->name = name;
    statement->body = NULL;
    statement->is_static = is_static;
    statement->parameters.init(state);
    return statement;
}

LitClassStatement* lit_create_class_statement(LitState* state, size_t line, LitString* name, LitString* parent)
{
    LitClassStatement* statement;
    statement = (LitClassStatement*)allocate_statement(state, line, sizeof(LitClassStatement), LITSTMT_CLASS);
    statement->name = name;
    statement->parent = parent;
    statement->fields.init(state);
    return statement;
}

LitFieldStatement* lit_create_field_statement(LitState* state, size_t line, LitString* name, LitStatement* getter, LitStatement* setter, bool is_static)
{
    LitFieldStatement* statement;
    statement = (LitFieldStatement*)allocate_statement(state, line, sizeof(LitFieldStatement), LITSTMT_FIELD);
    statement->name = name;
    statement->getter = getter;
    statement->setter = setter;
    statement->is_static = is_static;
    return statement;
}

PCGenericArray<LitExpression*>* lit_allocate_expressions(LitState* state)
{
    PCGenericArray<LitExpression*>* expressions;
    expressions = (PCGenericArray<LitExpression*>*)lit_reallocate(state, NULL, 0, sizeof(PCGenericArray<LitExpression*>));
    expressions->init(state);
    return expressions;
}

void lit_free_allocated_expressions(LitState* state, PCGenericArray<LitExpression*>* expressions)
{
    size_t i;
    if(expressions == NULL)
    {
        return;
    }
    for(i = 0; i < expressions->m_count; i++)
    {
        lit_free_expression(state, expressions->m_values[i]);
    }
    expressions->release(state);
    lit_reallocate(state, expressions, sizeof(PCGenericArray<LitExpression*>), 0);
}

PCGenericArray<LitStatement*>* lit_allocate_statements(LitState* state)
{
    PCGenericArray<LitStatement*>* statements;
    statements = (PCGenericArray<LitStatement*>*)lit_reallocate(state, NULL, 0, sizeof(PCGenericArray<LitStatement*>));
    statements->init(state);
    return statements;
}

void lit_free_allocated_statements(LitState* state, PCGenericArray<LitStatement*>* statements)
{
    size_t i;
    if(statements == NULL)
    {
        return;
    }
    for(i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }
    statements->release(state);
    lit_reallocate(state, statements, sizeof(PCGenericArray<LitStatement*>), 0);
}


static void emit_expression(LitEmitter* emitter, LitExpression* expression);
static bool emit_statement(LitEmitter* emitter, LitStatement* statement);
static void resolve_statement(LitEmitter* emitter, LitStatement* statement);

static void resolve_statements(LitEmitter* emitter, PCGenericArray<LitStatement*>& statements)
{
    size_t i;
    for(i = 0; i < statements.m_count; i++)
    {
        resolve_statement(emitter, statements.m_values[i]);
    }
}



void lit_init_emitter(LitState* state, LitEmitter* emitter)
{
    emitter->state = state;
    emitter->loop_start = 0;
    emitter->emit_reference = 0;
    emitter->class_name = NULL;
    emitter->compiler = NULL;
    emitter->chunk = NULL;
    emitter->module = NULL;
    emitter->previous_was_expression_statement = false;
    emitter->class_has_super = false;

    emitter->privates.init(state);
    emitter->breaks.init(state);
    emitter->continues.init(state);
}

void lit_free_emitter(LitEmitter* emitter)
{
    emitter->breaks.release(emitter->state);
    emitter->continues.release(emitter->state);
}

static void emit_byte(LitEmitter* emitter, uint16_t line, uint8_t byte)
{
    if(line < emitter->last_line)
    {
        // Egor-fail proofing
        line = emitter->last_line;
    }

    lit_write_chunk(emitter->state, emitter->chunk, byte, line);
    emitter->last_line = line;
}

static const int8_t stack_effects[] = {
#define OPCODE(_, effect) effect,
#undef OPCODE
};

static void emit_bytes(LitEmitter* emitter, uint16_t line, uint8_t a, uint8_t b)
{
    if(line < emitter->last_line)
    {
        // Egor-fail proofing
        line = emitter->last_line;
    }

    lit_write_chunk(emitter->state, emitter->chunk, a, line);
    lit_write_chunk(emitter->state, emitter->chunk, b, line);

    emitter->last_line = line;
}

static void emit_op(LitEmitter* emitter, uint16_t line, LitOpCode op)
{
    LitCompiler* compiler = emitter->compiler;

    emit_byte(emitter, line, (uint8_t)op);
    compiler->slots += stack_effects[(int)op];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

static void emit_ops(LitEmitter* emitter, uint16_t line, LitOpCode a, LitOpCode b)
{
    LitCompiler* compiler = emitter->compiler;

    emit_bytes(emitter, line, (uint8_t)a, (uint8_t)b);
    compiler->slots += stack_effects[(int)a] + stack_effects[(int)b];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

static void emit_varying_op(LitEmitter* emitter, uint16_t line, LitOpCode op, uint8_t arg)
{
    LitCompiler* compiler = emitter->compiler;

    emit_bytes(emitter, line, (uint8_t)op, arg);
    compiler->slots -= arg;

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

static void emit_arged_op(LitEmitter* emitter, uint16_t line, LitOpCode op, uint8_t arg)
{
    LitCompiler* compiler = emitter->compiler;

    emit_bytes(emitter, line, (uint8_t)op, arg);
    compiler->slots += stack_effects[(int)op];

    if(compiler->slots > (int)compiler->function->max_slots)
    {
        compiler->function->max_slots = (size_t)compiler->slots;
    }
}

static void emit_short(LitEmitter* emitter, uint16_t line, uint16_t value)
{
    emit_bytes(emitter, line, (uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
}

static void emit_byte_or_short(LitEmitter* emitter, uint16_t line, uint8_t a, uint8_t b, uint16_t index)
{
    if(index > UINT8_MAX)
    {
        emit_op(emitter, line, (LitOpCode)b);
        emit_short(emitter, line, (uint16_t)index);
    }
    else
    {
        emit_arged_op(emitter, line, (LitOpCode)a, (uint8_t)index);
    }
}

static void init_compiler(LitEmitter* emitter, LitCompiler* compiler, LitFunctionType type)
{
    compiler->locals.init(emitter->state);

    compiler->type = type;
    compiler->scope_depth = 0;
    compiler->enclosing = (struct LitCompiler*)emitter->compiler;
    compiler->skip_return = false;
    compiler->function = lit_create_function(emitter->state, emitter->module);
    compiler->loop_depth = 0;

    emitter->compiler = compiler;

    const char* name = emitter->state->scanner->file_name;

    if(emitter->compiler == NULL)
    {
        compiler->function->name = lit_string_copy(emitter->state, name, strlen(name));
    }

    emitter->chunk = &compiler->function->chunk;

    if(lit_is_optimization_enabled(LITOPTSTATE_LINE_INFO))
    {
        emitter->chunk->has_line_info = false;
    }

    if(type == LITFUNC_METHOD || type == LITFUNC_STATIC_METHOD || type == LITFUNC_CONSTRUCTOR)
    {
        compiler->locals.push((LitLocal){ "this", 4, -1, false, false });
    }
    else
    {
        compiler->locals.push((LitLocal){ "", 0, -1, false, false });
    }

    compiler->slots = 1;
    compiler->max_slots = 1;
}

static void emit_return(LitEmitter* emitter, size_t line)
{
    if(emitter->compiler->type == LITFUNC_CONSTRUCTOR)
    {
        emit_arged_op(emitter, line, OP_GET_LOCAL, 0);
        emit_op(emitter, line, OP_RETURN);
    }
    else if(emitter->previous_was_expression_statement && emitter->chunk->m_count > 0)
    {
        emitter->chunk->m_count--;// Remove the OP_POP
        emit_op(emitter, line, OP_RETURN);
    }
    else
    {
        emit_ops(emitter, line, OP_NULL, OP_RETURN);
    }
}

static LitFunction* end_compiler(LitEmitter* emitter, LitString* name)
{
    if(!emitter->compiler->skip_return)
    {
        emit_return(emitter, emitter->last_line);
        emitter->compiler->skip_return = true;
    }

    LitFunction* function = emitter->compiler->function;

    emitter->compiler->locals.release(emitter->state);

    emitter->compiler = (LitCompiler*)emitter->compiler->enclosing;
    emitter->chunk = emitter->compiler == NULL ? NULL : &emitter->compiler->function->chunk;

    if(name != NULL)
    {
        function->name = name;
    }

#ifdef LIT_TRACE_CHUNK
    lit_disassemble_chunk(&function->chunk, function->name->chars, NULL);
#endif

    return function;
}

static void begin_scope(LitEmitter* emitter)
{
    emitter->compiler->scope_depth++;
}

static void end_scope(LitEmitter* emitter, uint16_t line)
{
    emitter->compiler->scope_depth--;

    LitCompiler* compiler = emitter->compiler;
    PCGenericArray<LitLocal>* locals = &compiler->locals;

    while(locals->m_count > 0 && locals->m_values[locals->m_count - 1].depth > compiler->scope_depth)
    {
        if(locals->m_values[locals->m_count - 1].captured)
        {
            emit_op(emitter, line, OP_CLOSE_UPVALUE);
        }
        else
        {
            emit_op(emitter, line, OP_POP);
        }

        locals->m_count--;
    }
}

static void error(LitEmitter* emitter, size_t line, LitError error, ...)
{
    va_list args;
    va_start(args, error);
    lit_error(emitter->state, COMPILE_ERROR, lit_vformat_error(emitter->state, line, error, args)->chars);
    va_end(args);
}

static uint16_t add_constant(LitEmitter* emitter, size_t line, LitValue value)
{
    size_t constant = lit_chunk_add_constant(emitter->state, emitter->chunk, value);

    if(constant >= UINT16_MAX)
    {
        error(emitter, line, LITERROR_TOO_MANY_CONSTANTS);
    }

    return constant;
}

static size_t emit_constant(LitEmitter* emitter, size_t line, LitValue value)
{
    size_t constant = lit_chunk_add_constant(emitter->state, emitter->chunk, value);

    if(constant < UINT8_MAX)
    {
        emit_arged_op(emitter, line, OP_CONSTANT, constant);
    }
    else if(constant < UINT16_MAX)
    {
        emit_op(emitter, line, OP_CONSTANT_LONG);
        emit_short(emitter, line, constant);
    }
    else
    {
        error(emitter, line, LITERROR_TOO_MANY_CONSTANTS);
    }

    return constant;
}

static int add_private(LitEmitter* emitter, const char* name, size_t length, size_t line, bool constant)
{
    PCGenericArray<LitPrivate>* privates = &emitter->privates;

    if(privates->m_count == UINT16_MAX)
    {
        error(emitter, line, LITERROR_TOO_MANY_PRIVATES);
    }

    LitTable* private_names = &emitter->module->private_names->values;
    LitString* key = lit_table_find_string(private_names, name, length, lit_hash_string(name, length));

    if(key != NULL)
    {
        error(emitter, line, LITERROR_VAR_REDEFINED, length, name);

        LitValue index;
        lit_table_get(private_names, key, &index);

        return lit_value_to_number(index);
    }

    LitState* state = emitter->state;
    int index = (int)privates->m_count;

    privates->push((LitPrivate){ false, constant });

    lit_table_set(state, private_names, lit_string_copy(state, name, length), lit_number_to_value(index));
    emitter->module->private_count++;

    return index;
}

static int resolve_private(LitEmitter* emitter, const char* name, size_t length, size_t line)
{
    LitTable* private_names = &emitter->module->private_names->values;
    LitString* key = lit_table_find_string(private_names, name, length, lit_hash_string(name, length));

    if(key != NULL)
    {
        LitValue index;
        lit_table_get(private_names, key, &index);

        int number_index = lit_value_to_number(index);

        if(!emitter->privates.m_values[number_index].initialized)
        {
            error(emitter, line, LITERROR_VARIABLE_USED_IN_INIT, length, name);
        }

        return number_index;
    }

    return -1;
}

static int add_local(LitEmitter* emitter, const char* name, size_t length, size_t line, bool constant)
{
    LitCompiler* compiler = emitter->compiler;
    PCGenericArray<LitLocal>* locals = &compiler->locals;

    if(locals->m_count == UINT16_MAX)
    {
        error(emitter, line, LITERROR_TOO_MANY_LOCALS);
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
            error(emitter, line, LITERROR_VAR_REDEFINED, length, name);
        }
    }

    locals->push((LitLocal){ name, length, UINT16_MAX, false, constant });

    return (int)locals->m_count - 1;
}

static int resolve_local(LitEmitter* emitter, LitCompiler* compiler, const char* name, size_t length, size_t line)
{
    PCGenericArray<LitLocal>* locals = &compiler->locals;

    for(int i = (int)locals->m_count - 1; i >= 0; i--)
    {
        LitLocal* local = &locals->m_values[i];

        if(local->length == length && memcmp(local->name, name, length) == 0)
        {
            if(local->depth == UINT16_MAX)
            {
                error(emitter, line, LITERROR_VARIABLE_USED_IN_INIT, length, name);
            }

            return i;
        }
    }

    return -1;
}

static int add_upvalue(LitEmitter* emitter, LitCompiler* compiler, uint8_t index, size_t line, bool is_local)
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
        error(emitter, line, LITERROR_TOO_MANY_UPVALUES);
        return 0;
    }

    compiler->upvalues[upvalue_count].isLocal = is_local;
    compiler->upvalues[upvalue_count].index = index;

    return compiler->function->upvalue_count++;
}

static int resolve_upvalue(LitEmitter* emitter, LitCompiler* compiler, const char* name, size_t length, size_t line)
{
    if(compiler->enclosing == NULL)
    {
        return -1;
    }

    int local = resolve_local(emitter, (LitCompiler*)compiler->enclosing, name, length, line);

    if(local != -1)
    {
        ((LitCompiler*)compiler->enclosing)->locals.m_values[local].captured = true;
        return add_upvalue(emitter, compiler, (uint8_t)local, line, true);
    }

    int upvalue = resolve_upvalue(emitter, (LitCompiler*)compiler->enclosing, name, length, line);

    if(upvalue != -1)
    {
        return add_upvalue(emitter, compiler, (uint8_t)upvalue, line, false);
    }

    return -1;
}

static void mark_local_initialized(LitEmitter* emitter, size_t index)
{
    emitter->compiler->locals.m_values[index].depth = emitter->compiler->scope_depth;
}

static void mark_private_initialized(LitEmitter* emitter, size_t index)
{
    emitter->privates.m_values[index].initialized = true;
}

static size_t emit_jump(LitEmitter* emitter, LitOpCode code, size_t line)
{
    emit_op(emitter, line, code);
    emit_bytes(emitter, line, 0xff, 0xff);

    return emitter->chunk->m_count - 2;
}

static void patch_jump(LitEmitter* emitter, size_t offset, size_t line)
{
    size_t jump = emitter->chunk->m_count - offset - 2;

    if(jump > UINT16_MAX)
    {
        error(emitter, line, LITERROR_JUMP_TOO_BIG);
    }

    emitter->chunk->code[offset] = (jump >> 8) & 0xff;
    emitter->chunk->code[offset + 1] = jump & 0xff;
}

static void emit_loop(LitEmitter* emitter, size_t start, size_t line)
{
    emit_op(emitter, line, OP_JUMP_BACK);
    size_t offset = emitter->chunk->m_count - start + 2;

    if(offset > UINT16_MAX)
    {
        error(emitter, line, LITERROR_JUMP_TOO_BIG);
    }

    emit_short(emitter, line, offset);
}

static void patch_loop_jumps(LitEmitter* emitter, PCGenericArray<size_t>* breaks, size_t line)
{
    for(size_t i = 0; i < breaks->m_count; i++)
    {
        patch_jump(emitter, breaks->m_values[i], line);
    }
    breaks->release(emitter->state);
}

static bool emit_parameters(LitEmitter* emitter, PCGenericArray<LitParameter>* parameters, size_t line)
{
    for(size_t i = 0; i < parameters->m_count; i++)
    {
        LitParameter* parameter = &parameters->m_values[i];

        int index = add_local(emitter, parameter->name, parameter->length, line, false);
        mark_local_initialized(emitter, index);

        // Vararg ...
        if(parameter->length == 3 && memcmp(parameter->name, "...", 3) == 0)
        {
            return true;
        }

        if(parameter->default_value != NULL)
        {
            emit_byte_or_short(emitter, line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
            size_t jump = emit_jump(emitter, OP_NULL_OR, line);

            emit_expression(emitter, parameter->default_value);
            patch_jump(emitter, jump, line);
            emit_byte_or_short(emitter, line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
            emit_op(emitter, line, OP_POP);
        }
    }

    return false;
}

static void resolve_statement(LitEmitter* emitter, LitStatement* statement)
{
    if(statement == NULL)
    {
        return;
    }
    switch(statement->type)
    {
        case LITSTMT_VAR:
            {
                LitVarStatement* stmt = (LitVarStatement*)statement;
                mark_private_initialized(emitter, add_private(emitter, stmt->name, stmt->length, statement->line, stmt->constant));
            }
            break;
        case LITSTMT_FUNCTION:
            {
                LitFunctionStatement* stmt = (LitFunctionStatement*)statement;
                if(!stmt->exported)
                {
                    mark_private_initialized(emitter, add_private(emitter, stmt->name, stmt->length, statement->line, false));
                }
            }
            break;
        case LITSTMT_CLASS:
        case LITSTMT_BLOCK:
        case LITSTMT_FOR:
        case LITSTMT_WHILE:
        case LITSTMT_IF:
        case LITSTMT_CONTINUE:
        case LITSTMT_BREAK:
        case LITSTMT_RETURN:
        case LITSTMT_METHOD:
        case LITSTMT_FIELD:
        case LITSTMT_EXPRESSION:
            {
            }
            break;
    }
}

static void emit_expression(LitEmitter* emitter, LitExpression* expression)
{
    switch(expression->type)
    {
        case LITEXPR_LITERAL:
            {
                LitValue value = ((LitLiteralExpr*)expression)->value;
                if(IS_NUMBER(value) || IS_STRING(value))
                {
                    emit_constant(emitter, expression->line, value);
                }
                else if(IS_BOOL(value))
                {
                    emit_op(emitter, expression->line, lit_as_bool(value) ? OP_TRUE : OP_FALSE);
                }
                else if(IS_NULL(value))
                {
                    emit_op(emitter, expression->line, OP_NULL);
                }
                else
                {
                    UNREACHABLE;
                }
            }
            break;
        case LITEXPR_BINARY:
            {
                LitBinaryExpr* expr = (LitBinaryExpr*)expression;
                emit_expression(emitter, expr->left);
                if(expr->right == NULL)
                {
                    break;
                }
                LitTokenType op = expr->op;
                if(op == LITTOK_AMPERSAND_AMPERSAND || op == LITTOK_BAR_BAR || op == LITTOK_QUESTION_QUESTION)
                {
                    size_t jump = emit_jump(emitter, op == LITTOK_BAR_BAR ? OP_OR : (op == LITTOK_QUESTION_QUESTION ? OP_NULL_OR : OP_AND),
                                          emitter->last_line);
                    emit_expression(emitter, expr->right);
                    patch_jump(emitter, jump, emitter->last_line);
                    break;
                }
                emit_expression(emitter, expr->right);
                switch(op)
                {
                    case LITTOK_PLUS:
                        {
                            emit_op(emitter, expression->line, OP_ADD);
                        }
                        break;
                    case LITTOK_MINUS:
                        {
                            emit_op(emitter, expression->line, OP_SUBTRACT);
                        }
                        break;
                    case LITTOK_STAR:
                        {
                            emit_op(emitter, expression->line, OP_MULTIPLY);
                        }
                        break;
                    case LITTOK_STAR_STAR:
                        {
                            emit_op(emitter, expression->line, OP_POWER);
                        }
                        break;
                    case LITTOK_SLASH:
                        {
                            emit_op(emitter, expression->line, OP_DIVIDE);
                        }
                        break;
                    case LITTOK_SHARP:
                        {
                            emit_op(emitter, expression->line, OP_FLOOR_DIVIDE);
                        }
                        break;
                    case LITTOK_PERCENT:
                        {
                            emit_op(emitter, expression->line, OP_MOD);
                        }
                        break;
                    case LITTOK_IS:
                        {
                            emit_op(emitter, expression->line, OP_IS);
                        }
                        break;
                    case LITTOK_EQUAL_EQUAL:
                        {
                            emit_op(emitter, expression->line, OP_EQUAL);
                        }
                        break;
                    case LITTOK_BANG_EQUAL:
                        {
                            emit_ops(emitter, expression->line, OP_EQUAL, OP_NOT);
                        }
                        break;
                    case LITTOK_GREATER:
                        {
                            emit_op(emitter, expression->line, OP_GREATER);
                        }
                        break;
                    case LITTOK_GREATER_EQUAL:
                        {
                            emit_op(emitter, expression->line, OP_GREATER_EQUAL);
                        }
                        break;
                    case LITTOK_LESS:
                        {
                            emit_op(emitter, expression->line, OP_LESS);
                        }
                        break;
                    case LITTOK_LESS_EQUAL:
                        {
                            emit_op(emitter, expression->line, OP_LESS_EQUAL);
                        }
                        break;
                    case LITTOK_LESS_LESS:
                        {
                            emit_op(emitter, expression->line, OP_LSHIFT);
                        }
                        break;
                    case LITTOK_GREATER_GREATER:
                        {
                            emit_op(emitter, expression->line, OP_RSHIFT);
                        }
                        break;
                    case LITTOK_BAR:
                        {
                            emit_op(emitter, expression->line, OP_BOR);
                        }
                        break;
                    case LITTOK_AMPERSAND:
                        {
                            emit_op(emitter, expression->line, OP_BAND);
                        }
                        break;
                    case LITTOK_CARET:
                        {
                            emit_op(emitter, expression->line, OP_BXOR);
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
        case LITEXPR_UNARY:
            {
                LitUnaryExpr* expr = (LitUnaryExpr*)expression;
                emit_expression(emitter, expr->right);
                switch(expr->op)
                {
                    case LITTOK_MINUS:
                        {
                            emit_op(emitter, expression->line, OP_NEGATE);
                        }
                        break;
                    case LITTOK_BANG:
                        {
                            emit_op(emitter, expression->line, OP_NOT);
                        }
                        break;
                    case LITTOK_TILDE:
                        {
                            emit_op(emitter, expression->line, OP_BNOT);
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
        case LITEXPR_VAR:
            {
                LitVarExpr* expr = (LitVarExpr*)expression;
                bool ref = emitter->emit_reference > 0;
                if(ref)
                {
                    emitter->emit_reference--;
                }
                int index = resolve_local(emitter, emitter->compiler, expr->name, expr->length, expression->line);
                if(index == -1)
                {
                    index = resolve_upvalue(emitter, emitter->compiler, expr->name, expr->length, expression->line);
                    if(index == -1)
                    {
                        index = resolve_private(emitter, expr->name, expr->length, expression->line);
                        if(index == -1)
                        {
                            emit_op(emitter, expression->line, ref ? OP_REFERENCE_GLOBAL : OP_GET_GLOBAL);
                            emit_short(emitter, expression->line,
                                       add_constant(emitter, expression->line,
                                                    OBJECT_VALUE(lit_string_copy(emitter->state, expr->name, expr->length))));
                        }
                        else
                        {
                            if(ref)
                            {
                                emit_op(emitter, expression->line, OP_REFERENCE_PRIVATE);
                                emit_short(emitter, expression->line, index);
                            }
                            else
                            {
                                emit_byte_or_short(emitter, expression->line, OP_GET_PRIVATE, OP_GET_PRIVATE_LONG, index);
                            }
                        }
                    }
                    else
                    {
                        emit_arged_op(emitter, expression->line, ref ? OP_REFERENCE_UPVALUE : OP_GET_UPVALUE, (uint8_t)index);
                    }
                }
                else
                {
                    if(ref)
                    {
                        emit_op(emitter, expression->line, OP_REFERENCE_LOCAL);
                        emit_short(emitter, expression->line, index);
                    }
                    else
                    {
                        emit_byte_or_short(emitter, expression->line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
                    }
                }
            }
            break;
        case LITEXPR_ASSIGN:
            {
                LitAssignExpression* expr = (LitAssignExpression*)expression;
                if(expr->to->type == LITEXPR_VAR)
                {
                    emit_expression(emitter, expr->value);
                    LitVarExpr* e = (LitVarExpr*)expr->to;
                    int index = resolve_local(emitter, emitter->compiler, e->name, e->length, expr->to->line);
                    if(index == -1)
                    {
                        index = resolve_upvalue(emitter, emitter->compiler, e->name, e->length, expr->to->line);
                        if(index == -1)
                        {
                            index = resolve_private(emitter, e->name, e->length, expr->to->line);
                            if(index == -1)
                            {
                                emit_op(emitter, expression->line, OP_SET_GLOBAL);
                                emit_short(emitter, expression->line,
                                           add_constant(emitter, expression->line,
                                                        OBJECT_VALUE(lit_string_copy(emitter->state, e->name, e->length))));
                            }
                            else
                            {
                                if(emitter->privates.m_values[index].constant)
                                {
                                    error(emitter, expression->line, LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                }
                                emit_byte_or_short(emitter, expression->line, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                            }
                        }
                        else
                        {
                            emit_arged_op(emitter, expression->line, OP_SET_UPVALUE, (uint8_t)index);
                        }
                        break;
                    }
                    else
                    {
                        if(emitter->compiler->locals.m_values[index].constant)
                        {
                            error(emitter, expression->line, LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                        }

                        emit_byte_or_short(emitter, expression->line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                    }
                }
                else if(expr->to->type == LITEXPR_GET)
                {
                    emit_expression(emitter, expr->value);
                    LitGetExpression* e = (LitGetExpression*)expr->to;
                    emit_expression(emitter, e->where);
                    emit_expression(emitter, expr->value);
                    emit_constant(emitter, emitter->last_line, OBJECT_VALUE(lit_string_copy(emitter->state, e->name, e->length)));
                    emit_ops(emitter, emitter->last_line, OP_SET_FIELD, OP_POP);
                }
                else if(expr->to->type == LITEXPR_SUBSCRIPT)
                {
                    LitSubscriptExpression* e = (LitSubscriptExpression*)expr->to;
                    emit_expression(emitter, e->array);
                    emit_expression(emitter, e->index);
                    emit_expression(emitter, expr->value);
                    emit_op(emitter, emitter->last_line, OP_SUBSCRIPT_SET);
                }
                else if(expr->to->type == LITEXPR_REFERENCE)
                {
                    emit_expression(emitter, expr->value);
                    emit_expression(emitter, ((LitReferenceExpression*)expr->to)->to);
                    emit_op(emitter, expression->line, OP_SET_REFERENCE);
                }
                else
                {
                    error(emitter, expression->line, LITERROR_INVALID_ASSIGMENT_TARGET);
                }
            }
            break;
        case LITEXPR_CALL:
            {
                LitCallExpression* expr = (LitCallExpression*)expression;
                bool method = expr->callee->type == LITEXPR_GET;
                bool super = expr->callee->type == LITEXPR_SUPER;
                if(method)
                {
                    ((LitGetExpression*)expr->callee)->ignore_emit = true;
                }
                else if(super)
                {
                    ((LitSuperExpression*)expr->callee)->ignore_emit = true;
                }
                emit_expression(emitter, expr->callee);
                if(super)
                {
                    emit_arged_op(emitter, expression->line, OP_GET_LOCAL, 0);
                }
                for(size_t i = 0; i < expr->args.m_count; i++)
                {
                    LitExpression* e = expr->args.m_values[i];
                    if(e->type == LITEXPR_VAR)
                    {
                        LitVarExpr* ee = (LitVarExpr*)e;
                        // Vararg ...
                        if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                        {
                            emit_arged_op(emitter, e->line, OP_VARARG,
                                          resolve_local(emitter, emitter->compiler, "...", 3, expression->line));
                            break;
                        }
                    }
                    emit_expression(emitter, e);
                }
                if(method || super)
                {
                    if(method)
                    {
                        LitGetExpression* e = (LitGetExpression*)expr->callee;

                        emit_varying_op(emitter, expression->line,
                                        ((LitGetExpression*)expr->callee)->ignore_result ? OP_INVOKE_IGNORING : OP_INVOKE,
                                        (uint8_t)expr->args.m_count);
                        emit_short(emitter, emitter->last_line,
                                   add_constant(emitter, emitter->last_line,
                                                OBJECT_VALUE(lit_string_copy(emitter->state, e->name, e->length))));
                    }
                    else
                    {
                        LitSuperExpression* e = (LitSuperExpression*)expr->callee;
                        uint8_t index = resolve_upvalue(emitter, emitter->compiler, "super", 5, emitter->last_line);
                        emit_arged_op(emitter, expression->line, OP_GET_UPVALUE, index);
                        emit_varying_op(emitter, emitter->last_line,
                                        ((LitSuperExpression*)expr->callee)->ignore_result ? OP_INVOKE_SUPER_IGNORING : OP_INVOKE_SUPER,
                                        (uint8_t)expr->args.m_count);
                        emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(e->method)));
                    }
                }
                else
                {
                    emit_varying_op(emitter, expression->line, OP_CALL, (uint8_t)expr->args.m_count);
                }
                if(method)
                {
                    LitExpression* get = expr->callee;
                    while(get != NULL)
                    {
                        if(get->type == LITEXPR_GET)
                        {
                            LitGetExpression* getter = (LitGetExpression*)get;
                            if(getter->jump > 0)
                            {
                                patch_jump(emitter, getter->jump, emitter->last_line);
                            }
                            get = getter->where;
                        }
                        else if(get->type == LITEXPR_SUBSCRIPT)
                        {
                            get = ((LitSubscriptExpression*)get)->array;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                if(expr->init == NULL)
                {
                    return;
                }
                LitObjectExpression* init = (LitObjectExpression*)expr->init;
                for(size_t i = 0; i < init->values.m_count; i++)
                {
                    LitExpression* e = init->values.m_values[i];
                    emitter->last_line = e->line;
                    emit_constant(emitter, emitter->last_line, init->keys.m_values[i]);
                    emit_expression(emitter, e);
                    emit_op(emitter, emitter->last_line, OP_PUSH_OBJECT_FIELD);
                }
            }
            break;
        case LITEXPR_GET:
            {
                LitGetExpression* expr = (LitGetExpression*)expression;
                bool ref = emitter->emit_reference > 0;
                if(ref)
                {
                    emitter->emit_reference--;
                }
                emit_expression(emitter, expr->where);
                if(expr->jump == 0)
                {
                    expr->jump = emit_jump(emitter, OP_JUMP_IF_NULL, emitter->last_line);
                    if(!expr->ignore_emit)
                    {
                        emit_constant(emitter, emitter->last_line,
                                      OBJECT_VALUE(lit_string_copy(emitter->state, expr->name, expr->length)));
                        emit_op(emitter, emitter->last_line, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                    }
                    patch_jump(emitter, expr->jump, emitter->last_line);
                }
                else if(!expr->ignore_emit)
                {
                    emit_constant(emitter, emitter->last_line, OBJECT_VALUE(lit_string_copy(emitter->state, expr->name, expr->length)));
                    emit_op(emitter, emitter->last_line, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                }
            }
            break;
        case LITEXPR_SET:
            {
                LitSetExpression* expr = (LitSetExpression*)expression;
                emit_expression(emitter, expr->where);
                emit_expression(emitter, expr->value);
                emit_constant(emitter, emitter->last_line, OBJECT_VALUE(lit_string_copy(emitter->state, expr->name, expr->length)));
                emit_op(emitter, emitter->last_line, OP_SET_FIELD);
            }
            break;
        case LITEXPR_LAMBDA:
            {
                LitLambdaExpression* expr = (LitLambdaExpression*)expression;
                LitString* name = lit_as_string(lit_string_format(emitter->state, "lambda @:@", OBJECT_VALUE(emitter->module->name),
                                                              lit_string_number_to_string(emitter->state, expression->line)));
                LitCompiler compiler;
                init_compiler(emitter, &compiler, LITFUNC_REGULAR);
                begin_scope(emitter);
                bool vararg = emit_parameters(emitter, &expr->parameters, expression->line);
                if(expr->body != NULL)
                {
                    bool single_expression = expr->body->type == LITSTMT_EXPRESSION;
                    if(single_expression)
                    {
                        compiler.skip_return = true;
                        ((LitExpressionStatement*)expr->body)->pop = false;
                    }
                    emit_statement(emitter, expr->body);
                    if(single_expression)
                    {
                        emit_op(emitter, emitter->last_line, OP_RETURN);
                    }
                }
                end_scope(emitter, emitter->last_line);
                LitFunction* function = end_compiler(emitter, name);
                function->arg_count = expr->parameters.m_count;
                function->max_slots += function->arg_count;
                function->vararg = vararg;
                if(function->upvalue_count > 0)
                {
                    emit_op(emitter, emitter->last_line, OP_CLOSURE);
                    emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(function)));
                    for(size_t i = 0; i < function->upvalue_count; i++)
                    {
                        emit_bytes(emitter, emitter->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                    }
                }
                else
                {
                    emit_constant(emitter, emitter->last_line, OBJECT_VALUE(function));
                }
            }
            break;
        case LITEXPR_ARRAY:
            {
                LitArrayExpression* expr = (LitArrayExpression*)expression;
                emit_op(emitter, expression->line, OP_ARRAY);
                for(size_t i = 0; i < expr->values.m_count; i++)
                {
                    emit_expression(emitter, expr->values.m_values[i]);
                    emit_op(emitter, emitter->last_line, OP_PUSH_ARRAY_ELEMENT);
                }
            }
            break;
        case LITEXPR_OBJECT:
            {
                LitObjectExpression* expr = (LitObjectExpression*)expression;
                emit_op(emitter, expression->line, OP_OBJECT);
                for(size_t i = 0; i < expr->values.m_count; i++)
                {
                    emit_constant(emitter, emitter->last_line, expr->keys.m_values[i]);
                    emit_expression(emitter, expr->values.m_values[i]);
                    emit_op(emitter, emitter->last_line, OP_PUSH_OBJECT_FIELD);
                }
            }
            break;
        case LITEXPR_SUBSCRIPT:
            {
                LitSubscriptExpression* expr = (LitSubscriptExpression*)expression;
                emit_expression(emitter, expr->array);
                emit_expression(emitter, expr->index);
                emit_op(emitter, expression->line, OP_SUBSCRIPT_GET);
            }
            break;
        case LITEXPR_THIS:
            {
                LitFunctionType type = emitter->compiler->type;
                if(type == LITFUNC_STATIC_METHOD)
                {
                    error(emitter, expression->line, LITERROR_THIS_MISSUSE, "in static methods");
                }
                if(type == LITFUNC_CONSTRUCTOR || type == LITFUNC_METHOD)
                {
                    emit_arged_op(emitter, expression->line, OP_GET_LOCAL, 0);
                }
                else
                {
                    if(emitter->compiler->enclosing == NULL)
                    {
                        error(emitter, expression->line, LITERROR_THIS_MISSUSE, "in functions outside of any class");
                    }
                    else
                    {
                        int local = resolve_local(emitter, (LitCompiler*)emitter->compiler->enclosing, "this", 4, expression->line);
                        emit_arged_op(emitter, expression->line, OP_GET_UPVALUE,
                                      add_upvalue(emitter, emitter->compiler, local, expression->line, true));
                    }
                }
            }
            break;
        case LITEXPR_SUPER:
            {
                if(emitter->compiler->type == LITFUNC_STATIC_METHOD)
                {
                    error(emitter, expression->line, LITERROR_SUPER_MISSUSE, "in static methods");
                }
                else if(!emitter->class_has_super)
                {
                    error(emitter, expression->line, LITERROR_NO_SUPER, emitter->class_name->chars);
                }
                LitSuperExpression* expr = (LitSuperExpression*)expression;
                if(!expr->ignore_emit)
                {
                    uint8_t index = resolve_upvalue(emitter, emitter->compiler, "super", 5, emitter->last_line);
                    emit_arged_op(emitter, expression->line, OP_GET_LOCAL, 0);
                    emit_arged_op(emitter, expression->line, OP_GET_UPVALUE, index);
                    emit_op(emitter, expression->line, OP_GET_SUPER_METHOD);
                    emit_short(emitter, expression->line, add_constant(emitter, expression->line, OBJECT_VALUE(expr->method)));
                }
            }
            break;
        case LITEXPR_RANGE:
            {
                LitRangeExpression* expr = (LitRangeExpression*)expression;
                emit_expression(emitter, expr->to);
                emit_expression(emitter, expr->from);
                emit_op(emitter, expression->line, OP_RANGE);
            }
            break;
        case LITEXPR_IF:
            {
                LitIfExpression* expr = (LitIfExpression*)expression;
                emit_expression(emitter, expr->condition);
                uint64_t else_jump = emit_jump(emitter, OP_JUMP_IF_FALSE, expression->line);
                emit_expression(emitter, expr->if_branch);
                uint64_t end_jump = emit_jump(emitter, OP_JUMP, emitter->last_line);
                patch_jump(emitter, else_jump, expr->else_branch->line);
                emit_expression(emitter, expr->else_branch);

                patch_jump(emitter, end_jump, emitter->last_line);
            }
            break;
        case LITEXPR_INTERPOLATION:
            {
                LitInterpolationExpression* expr = (LitInterpolationExpression*)expression;
                emit_op(emitter, expression->line, OP_ARRAY);
                for(size_t i = 0; i < expr->expressions.m_count; i++)
                {
                    emit_expression(emitter, expr->expressions.m_values[i]);
                    emit_op(emitter, emitter->last_line, OP_PUSH_ARRAY_ELEMENT);
                }
                emit_varying_op(emitter, emitter->last_line, OP_INVOKE, 0);
                emit_short(emitter, emitter->last_line,
                           add_constant(emitter, emitter->last_line, OBJECT_CONST_STRING(emitter->state, "join")));
            }
            break;
        case LITEXPR_REFERENCE:
            {
                LitExpression* to = ((LitReferenceExpression*)expression)->to;
                if(to->type != LITEXPR_VAR && to->type != LITEXPR_GET && to->type != LITEXPR_THIS && to->type != LITEXPR_SUPER)
                {
                    error(emitter, expression->line, LITERROR_INVALID_REFERENCE_TARGET);
                    break;
                }
                int old = emitter->emit_reference;
                emitter->emit_reference++;
                emit_expression(emitter, to);
                emitter->emit_reference = old;
            }
            break;
        default:
            {
                error(emitter, expression->line, LITERROR_UNKNOWN_EXPRESSION, (int)expression->type);
            }
            break;
    }
}

static bool emit_statement(LitEmitter* emitter, LitStatement* statement)
{

    LitClassStatement* clstmt;
    LitCompiler compiler;
    LitExpression* expression;
    LitExpression* e;
    LitExpressionStatement* expr;
    LitField* field;
    LitFieldStatement* fieldstmt;
    LitFunction* function;
    LitFunction* getter;
    LitFunction* setter;
    LitFunctionStatement* funcstmt;
    LitLocal* local;
    PCGenericArray<LitLocal>* locals;
    LitMethodStatement* mthstmt;
    LitStatement* blockstmt;
    LitStatement* s;
    PCGenericArray<LitStatement*>* statements;
    LitString* name;
    LitVarStatement* var;
    LitVarStatement* varstmt;
    LitForStatement* forstmt;
    LitWhileStatement* whilestmt;
    LitIfStatement* ifstmt;
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
    if(statement == NULL)
    {
        return false;
    }
    switch(statement->type)
    {
        case LITSTMT_EXPRESSION:
            {
                expr = (LitExpressionStatement*)statement;
                emit_expression(emitter, expr->expression);
                if(expr->pop)
                {
                    emit_op(emitter, statement->line, OP_POP);
                }
            }
            break;
        case LITSTMT_BLOCK:
            {
                statements = &((LitBlockStatement*)statement)->statements;
                begin_scope(emitter);
                {
                    for(i = 0; i < statements->m_count; i++)
                    {
                        blockstmt = statements->m_values[i];

                        if(emit_statement(emitter, blockstmt))
                        {
                            break;
                        }
                    }
                }
                end_scope(emitter, emitter->last_line);
            }
            break;
        case LITSTMT_VAR:
            {
                varstmt = (LitVarStatement*)statement;
                line = statement->line;
                isprivate = emitter->compiler->enclosing == NULL && emitter->compiler->scope_depth == 0;
                index = isprivate ? resolve_private(emitter, varstmt->name, varstmt->length, statement->line) :
                                      add_local(emitter, varstmt->name, varstmt->length, statement->line, varstmt->constant);
                if(varstmt->init == NULL)
                {
                    emit_op(emitter, line, OP_NULL);
                }
                else
                {
                    emit_expression(emitter, varstmt->init);
                }
                if(isprivate)
                {
                    mark_private_initialized(emitter, index);
                }
                else
                {
                    mark_local_initialized(emitter, index);
                }
                emit_byte_or_short(emitter, statement->line, isprivate ? OP_SET_PRIVATE : OP_SET_LOCAL,
                                   isprivate ? OP_SET_PRIVATE_LONG : OP_SET_LOCAL_LONG, index);
                if(isprivate)
                {
                    // Privates don't live on stack, so we need to pop them manually
                    emit_op(emitter, statement->line, OP_POP);
                }
            }
            break;
        case LITSTMT_IF:
            {
                ifstmt = (LitIfStatement*)statement;
                else_jump = 0;
                end_jump = 0;
                if(ifstmt->condition == NULL)
                {
                    else_jump = emit_jump(emitter, OP_JUMP, statement->line);
                }
                else
                {
                    emit_expression(emitter, ifstmt->condition);
                    else_jump = emit_jump(emitter, OP_JUMP_IF_FALSE, statement->line);
                    emit_statement(emitter, ifstmt->if_branch);
                    end_jump = emit_jump(emitter, OP_JUMP, emitter->last_line);
                }
                //uint64_t end_jumps[ifstmt->elseif_branches == NULL ? 1 : ifstmt->elseif_branches->m_count];
                end_jumps = (uint64_t*)malloc(ifstmt->elseif_branches == NULL ? 1 : ifstmt->elseif_branches->m_count);
                if(ifstmt->elseif_branches != NULL)
                {
                    for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                    {
                        e = ifstmt->elseif_conditions->m_values[i];
                        if(e == NULL)
                        {
                            continue;
                        }
                        patch_jump(emitter, else_jump, e->line);
                        emit_expression(emitter, e);
                        else_jump = emit_jump(emitter, OP_JUMP_IF_FALSE, emitter->last_line);
                        emit_statement(emitter, ifstmt->elseif_branches->m_values[i]);

                        end_jumps[i] = emit_jump(emitter, OP_JUMP, emitter->last_line);
                    }
                }
                if(ifstmt->else_branch != NULL)
                {
                    patch_jump(emitter, else_jump, ifstmt->else_branch->line);
                    emit_statement(emitter, ifstmt->else_branch);
                }
                else
                {
                    patch_jump(emitter, else_jump, emitter->last_line);
                }
                if(end_jump != 0)
                {
                    patch_jump(emitter, end_jump, emitter->last_line);
                }
                if(ifstmt->elseif_branches != NULL)
                {
                    for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                    {
                        if(ifstmt->elseif_branches->m_values[i] == NULL)
                        {
                            continue;
                        }
                        patch_jump(emitter, end_jumps[i], ifstmt->elseif_branches->m_values[i]->line);
                    }
                }
                free(end_jumps);
            }
            break;
        case LITSTMT_WHILE:
            {
                whilestmt = (LitWhileStatement*)statement;
                start = emitter->chunk->m_count;
                emitter->loop_start = start;
                emitter->compiler->loop_depth++;
                emit_expression(emitter, whilestmt->condition);
                exit_jump = emit_jump(emitter, OP_JUMP_IF_FALSE, statement->line);
                emit_statement(emitter, whilestmt->body);
                patch_loop_jumps(emitter, &emitter->continues, emitter->last_line);
                emit_loop(emitter, start, emitter->last_line);
                patch_jump(emitter, exit_jump, emitter->last_line);
                patch_loop_jumps(emitter, &emitter->breaks, emitter->last_line);
                emitter->compiler->loop_depth--;
            }
            break;
        case LITSTMT_FOR:
            {
                forstmt = (LitForStatement*)statement;
                begin_scope(emitter);
                emitter->compiler->loop_depth++;
                if(forstmt->c_style)
                {
                    if(forstmt->var != NULL)
                    {
                        emit_statement(emitter, forstmt->var);
                    }
                    else if(forstmt->init != NULL)
                    {
                        emit_expression(emitter, forstmt->init);
                    }
                    start = emitter->chunk->m_count;
                    exit_jump = 0;
                    if(forstmt->condition != NULL)
                    {
                        emit_expression(emitter, forstmt->condition);
                        exit_jump = emit_jump(emitter, OP_JUMP_IF_FALSE, emitter->last_line);
                    }
                    if(forstmt->increment != NULL)
                    {
                        body_jump = emit_jump(emitter, OP_JUMP, emitter->last_line);
                        increment_start = emitter->chunk->m_count;
                        emit_expression(emitter, forstmt->increment);
                        emit_op(emitter, emitter->last_line, OP_POP);
                        emit_loop(emitter, start, emitter->last_line);
                        start = increment_start;
                        patch_jump(emitter, body_jump, emitter->last_line);
                    }
                    emitter->loop_start = start;
                    begin_scope(emitter);
                    if(forstmt->body != NULL)
                    {
                        if(forstmt->body->type == LITSTMT_BLOCK)
                        {
                            statements = &((LitBlockStatement*)forstmt->body)->statements;
                            for(i = 0; i < statements->m_count; i++)
                            {
                                emit_statement(emitter, statements->m_values[i]);
                            }
                        }
                        else
                        {
                            emit_statement(emitter, forstmt->body);
                        }
                    }
                    patch_loop_jumps(emitter, &emitter->continues, emitter->last_line);
                    end_scope(emitter, emitter->last_line);
                    emit_loop(emitter, start, emitter->last_line);
                    if(forstmt->condition != NULL)
                    {
                        patch_jump(emitter, exit_jump, emitter->last_line);
                    }
                }
                else
                {
                    sequence = add_local(emitter, "seq ", 4, statement->line, false);
                    mark_local_initialized(emitter, sequence);
                    emit_expression(emitter, forstmt->condition);
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, sequence);
                    iterator = add_local(emitter, "iter ", 5, statement->line, false);
                    mark_local_initialized(emitter, iterator);
                    emit_op(emitter, emitter->last_line, OP_NULL);
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                    start = emitter->chunk->m_count;
                    emitter->loop_start = emitter->chunk->m_count;
                    // iter = seq.iterator(iter)
                    emit_byte_or_short(emitter, emitter->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                    emit_byte_or_short(emitter, emitter->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                    emit_varying_op(emitter, emitter->last_line, OP_INVOKE, 1);
                    emit_short(emitter, emitter->last_line,
                               add_constant(emitter, emitter->last_line, OBJECT_CONST_STRING(emitter->state, "iterator")));
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                    // If iter is null, just get out of the loop
                    exit_jump = emit_jump(emitter, OP_JUMP_IF_NULL_POPPING, emitter->last_line);
                    begin_scope(emitter);
                    // var i = seq.iteratorValue(iter)
                    var = (LitVarStatement*)forstmt->var;
                    localcnt = add_local(emitter, var->name, var->length, statement->line, false);
                    mark_local_initialized(emitter, localcnt);
                    emit_byte_or_short(emitter, emitter->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                    emit_byte_or_short(emitter, emitter->last_line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                    emit_varying_op(emitter, emitter->last_line, OP_INVOKE, 1);
                    emit_short(emitter, emitter->last_line,
                               add_constant(emitter, emitter->last_line, OBJECT_CONST_STRING(emitter->state, "iteratorValue")));
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, localcnt);
                    if(forstmt->body != NULL)
                    {
                        if(forstmt->body->type == LITSTMT_BLOCK)
                        {
                            statements = &((LitBlockStatement*)forstmt->body)->statements;
                            for(i = 0; i < statements->m_count; i++)
                            {
                                emit_statement(emitter, statements->m_values[i]);
                            }
                        }
                        else
                        {
                            emit_statement(emitter, forstmt->body);
                        }
                    }
                    patch_loop_jumps(emitter, &emitter->continues, emitter->last_line);
                    end_scope(emitter, emitter->last_line);
                    emit_loop(emitter, start, emitter->last_line);
                    patch_jump(emitter, exit_jump, emitter->last_line);
                }
                patch_loop_jumps(emitter, &emitter->breaks, emitter->last_line);
                end_scope(emitter, emitter->last_line);
                emitter->compiler->loop_depth--;
            }
            break;

        case LITSTMT_CONTINUE:
        {
            if(emitter->compiler->loop_depth == 0)
            {
                error(emitter, statement->line, LITERROR_LOOP_JUMP_MISSUSE, "continue");
            }
            emitter->continues.push(emit_jump(emitter, OP_JUMP, statement->line));
            break;
        }

        case LITSTMT_BREAK:
            {
                if(emitter->compiler->loop_depth == 0)
                {
                    error(emitter, statement->line, LITERROR_LOOP_JUMP_MISSUSE, "break");
                }
                emit_op(emitter, statement->line, OP_POP_LOCALS);
                depth = emitter->compiler->scope_depth;
                local_count = 0;
                locals = &emitter->compiler->locals;
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
                emit_short(emitter, statement->line, local_count);
                emitter->breaks.push(emit_jump(emitter, OP_JUMP, statement->line));
            }
            break;
        case LITSTMT_FUNCTION:
            {
                funcstmt = (LitFunctionStatement*)statement;
                isexport = funcstmt->exported;
                isprivate = !isexport && emitter->compiler->enclosing == NULL && emitter->compiler->scope_depth == 0;
                islocal = !(isexport || isprivate);
                index = 0;
                if(!isexport)
                {
                    index = isprivate ? resolve_private(emitter, funcstmt->name, funcstmt->length, statement->line) :
                                      add_local(emitter, funcstmt->name, funcstmt->length, statement->line, false);
                }
                name = lit_string_copy(emitter->state, funcstmt->name, funcstmt->length);
                if(islocal)
                {
                    mark_local_initialized(emitter, index);
                }
                else if(isprivate)
                {
                    mark_private_initialized(emitter, index);
                }
                init_compiler(emitter, &compiler, LITFUNC_REGULAR);
                begin_scope(emitter);
                vararg = emit_parameters(emitter, &funcstmt->parameters, statement->line);
                emit_statement(emitter, funcstmt->body);
                end_scope(emitter, emitter->last_line);
                function = end_compiler(emitter, name);
                function->arg_count = funcstmt->parameters.m_count;
                function->max_slots += function->arg_count;
                function->vararg = vararg;
                if(function->upvalue_count > 0)
                {
                    emit_op(emitter, emitter->last_line, OP_CLOSURE);
                    emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(function)));
                    for(i = 0; i < function->upvalue_count; i++)
                    {
                        emit_bytes(emitter, emitter->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                    }
                }
                else
                {
                    emit_constant(emitter, emitter->last_line, OBJECT_VALUE(function));
                }
                if(isexport)
                {
                    emit_op(emitter, emitter->last_line, OP_SET_GLOBAL);
                    emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(name)));
                }
                else if(isprivate)
                {
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                }
                else
                {
                    emit_byte_or_short(emitter, emitter->last_line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                }
                emit_op(emitter, emitter->last_line, OP_POP);
            }
            break;
        case LITSTMT_RETURN:
            {
                if(emitter->compiler->type == LITFUNC_CONSTRUCTOR)
                {
                    error(emitter, statement->line, LITERROR_RETURN_FROM_CONSTRUCTOR);
                }
                expression = ((LitReturnStatement*)statement)->expression;
                if(expression == NULL)
                {
                    emit_op(emitter, emitter->last_line, OP_NULL);
                }
                else
                {
                    emit_expression(emitter, expression);
                }
                emit_op(emitter, emitter->last_line, OP_RETURN);
                if(emitter->compiler->scope_depth == 0)
                {
                    emitter->compiler->skip_return = true;
                }
                return true;
            }
            break;
        case LITSTMT_METHOD:
            {
                mthstmt = (LitMethodStatement*)statement;
                constructor = lit_string_length(mthstmt->name) == (sizeof(LIT_NAME_CONSTRUCTOR)-1) && memcmp(mthstmt->name->chars, LIT_NAME_CONSTRUCTOR, strlen(LIT_NAME_CONSTRUCTOR)-1) == 0;
                if(constructor && mthstmt->is_static)
                {
                    error(emitter, statement->line, LITERROR_STATIC_CONSTRUCTOR);
                }
                init_compiler(emitter, &compiler,
                              constructor ? LITFUNC_CONSTRUCTOR : (mthstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD));
                begin_scope(emitter);
                vararg = emit_parameters(emitter, &mthstmt->parameters, statement->line);
                emit_statement(emitter, mthstmt->body);
                end_scope(emitter, emitter->last_line);
                function = end_compiler(emitter, lit_as_string(lit_string_format(emitter->state, "@:@", OBJECT_VALUE(emitter->class_name), OBJECT_VALUE(mthstmt->name))));
                function->arg_count = mthstmt->parameters.m_count;
                function->max_slots += function->arg_count;
                function->vararg = vararg;
                if(function->upvalue_count > 0)
                {
                    emit_op(emitter, emitter->last_line, OP_CLOSURE);
                    emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(function)));
                    for(i = 0; i < function->upvalue_count; i++)
                    {
                        emit_bytes(emitter, emitter->last_line, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                    }
                }
                else
                {
                    emit_constant(emitter, emitter->last_line, OBJECT_VALUE(function));
                }
                emit_op(emitter, emitter->last_line, mthstmt->is_static ? OP_STATIC_FIELD : OP_METHOD);
                emit_short(emitter, emitter->last_line, add_constant(emitter, statement->line, OBJECT_VALUE(mthstmt->name)));

            }
            break;
        case LITSTMT_CLASS:
            {
                clstmt = (LitClassStatement*)statement;
                emitter->class_name = clstmt->name;
                if(clstmt->parent != NULL)
                {
                    emit_op(emitter, emitter->last_line, OP_GET_GLOBAL);
                    emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(clstmt->parent)));
                }
                emit_op(emitter, statement->line, OP_CLASS);
                emit_short(emitter, emitter->last_line, add_constant(emitter, emitter->last_line, OBJECT_VALUE(clstmt->name)));
                if(clstmt->parent != NULL)
                {
                    emit_op(emitter, emitter->last_line, OP_INHERIT);
                    emitter->class_has_super = true;
                    begin_scope(emitter);
                    super = add_local(emitter, "super", 5, emitter->last_line, false);
                    
                    mark_local_initialized(emitter, super);
                }
                for(i = 0; i < clstmt->fields.m_count; i++)
                {
                    s = clstmt->fields.m_values[i];
                    if(s->type == LITSTMT_VAR)
                    {
                        var = (LitVarStatement*)s;
                        emit_expression(emitter, var->init);
                        emit_op(emitter, statement->line, OP_STATIC_FIELD);
                        emit_short(emitter, statement->line,
                                   add_constant(emitter, statement->line,
                                                OBJECT_VALUE(lit_string_copy(emitter->state, var->name, var->length))));
                    }
                    else
                    {
                        emit_statement(emitter, s);
                    }
                }
                emit_op(emitter, emitter->last_line, OP_POP);
                if(clstmt->parent != NULL)
                {
                    end_scope(emitter, emitter->last_line);
                }
                emitter->class_name = NULL;
                emitter->class_has_super = false;
            }
            break;
        case LITSTMT_FIELD:
            {
                fieldstmt = (LitFieldStatement*)statement;
                getter = NULL;
                setter = NULL;
                if(fieldstmt->getter != NULL)
                {
                    init_compiler(emitter, &compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                    begin_scope(emitter);
                    emit_statement(emitter, fieldstmt->getter);
                    end_scope(emitter, emitter->last_line);
                    getter = end_compiler(emitter,
                        lit_as_string(lit_string_format(emitter->state, "@:get @", OBJECT_VALUE(emitter->class_name), fieldstmt->name)));
                }
                if(fieldstmt->setter != NULL)
                {
                    init_compiler(emitter, &compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                    mark_local_initialized(emitter, add_local(emitter, "value", 5, statement->line, false));
                    begin_scope(emitter);
                    emit_statement(emitter, fieldstmt->setter);
                    end_scope(emitter, emitter->last_line);
                    setter = end_compiler(emitter,
                        lit_as_string(lit_string_format(emitter->state, "@:set @", OBJECT_VALUE(emitter->class_name), fieldstmt->name)));
                    setter->arg_count = 1;
                    setter->max_slots++;
                }
                field = lit_create_field(emitter->state, (LitObject*)getter, (LitObject*)setter);
                emit_constant(emitter, statement->line, OBJECT_VALUE(field));
                emit_op(emitter, statement->line, fieldstmt->is_static ? OP_STATIC_FIELD : OP_DEFINE_FIELD);
                emit_short(emitter, statement->line, add_constant(emitter, statement->line, OBJECT_VALUE(fieldstmt->name)));
            }
            break;
        default:
            {
                error(emitter, statement->line, LITERROR_UNKNOWN_STATEMENT, (int)statement->type);
            }
            break;
    }
    emitter->previous_was_expression_statement = statement->type == LITSTMT_EXPRESSION;
    return false;
}

LitModule* lit_emit(LitEmitter* emitter, PCGenericArray<LitStatement*>& statements, LitString* module_name)
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
    LitStatement* stmt;
    emitter->last_line = 1;
    emitter->emit_reference = 0;
    state = emitter->state;
    isnew = false;
    if(lit_table_get(&emitter->state->vm->modules->values, module_name, &module_value))
    {
        module = AS_MODULE(module_value);
    }
    else
    {
        module = lit_create_module(emitter->state, module_name);
        isnew = true;
    }
    emitter->module = module;
    old_privates_count = module->private_count;
    if(old_privates_count > 0)
    {
        privates = &emitter->privates;
        privates->m_count = old_privates_count - 1;
        privates->push((LitPrivate){ true, false });
        for(i = 0; i < old_privates_count; i++)
        {
            privates->m_values[i].initialized = true;
        }
    }
    init_compiler(emitter, &compiler, LITFUNC_SCRIPT);
    emitter->chunk = &compiler.function->chunk;
    resolve_statements(emitter, statements);
    for(i = 0; i < statements.m_count; i++)
    {
        stmt = statements.m_values[i];
        if(stmt != NULL)
        {
            if(emit_statement(emitter, stmt))
            {
                break;
            }
        }
    }
    end_scope(emitter, emitter->last_line);
    module->main_function = end_compiler(emitter, module_name);
    if(isnew)
    {
        total = emitter->privates.m_count;
        module->privates = LIT_ALLOCATE(emitter->state, LitValue, total);
        for(i = 0; i < total; i++)
        {
            module->privates[i] = NULL_VALUE;
        }
    }
    else
    {
        module->privates = LIT_GROW_ARRAY(emitter->state, module->privates, LitValue, old_privates_count, module->private_count);
        for(i = old_privates_count; i < module->private_count; i++)
        {
            module->privates[i] = NULL_VALUE;
        }
    }
    emitter->privates.release(emitter->state);
    if(lit_is_optimization_enabled(LITOPTSTATE_PRIVATE_NAMES))
    {
        emitter->module->private_names->values.release(emitter->state);
    }
    if(isnew && !state->had_error)
    {
        lit_table_set(state, &state->vm->modules->values, module_name, OBJECT_VALUE(module));
    }
    module->ran = true;
    return module;
}


#define LIT_DEBUG_OPTIMIZER

#define optc_do_binary_op(op) \
    if(IS_NUMBER(a) && IS_NUMBER(b)) \
    { \
        optdbg("translating constant binary expression of '" # op "' to constant value"); \
        return lit_number_to_value(lit_value_to_number(a) op lit_value_to_number(b)); \
    } \
    return NULL_VALUE;

#define optc_do_bitwise_op(op) \
    if(IS_NUMBER(a) && IS_NUMBER(b)) \
    { \
        optdbg("translating constant bitwise expression of '" #op "' to constant value"); \
        return lit_number_to_value((int)lit_value_to_number(a) op(int) lit_value_to_number(b)); \
    } \
    return NULL_VALUE;

#define optc_do_fn_op(fn, tokstr) \
    if(IS_NUMBER(a) && IS_NUMBER(b)) \
    { \
        optdbg("translating constant expression of '" tokstr "' to constant value via call to '" #fn "'"); \
        return lit_number_to_value(fn(lit_value_to_number(a), lit_value_to_number(b))); \
    } \
    return NULL_VALUE;



static void optimize_expression(LitOptimizer* optimizer, LitExpression** slot);
static void optimize_expressions(LitOptimizer* optimizer, PCGenericArray<LitExpression*>* expressions);
static void optimize_statements(LitOptimizer* optimizer, PCGenericArray<LitStatement*>* statements);
static void optimize_statement(LitOptimizer* optimizer, LitStatement** slot);

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


void lit_init_optimizer(LitState* state, LitOptimizer* optimizer)
{
    optimizer->state = state;
    optimizer->depth = -1;
    optimizer->mark_used = false;
    optimizer->variables.init(state);
}

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
            *variable->declaration = NULL;
        }
        variables->m_count--;
    }
}

static LitVariable* add_variable(LitOptimizer* optimizer, const char* name, size_t length, bool constant, LitStatement** declaration)
{
    optimizer->variables.push((LitVariable){ name, length, optimizer->depth, constant, optimizer->mark_used, NULL_VALUE, declaration });

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
    return NULL;
}

static bool is_empty(LitStatement* statement)
{
    return statement == NULL || (statement->type == LITSTMT_BLOCK && ((LitBlockStatement*)statement)->statements.m_count == 0);
}

static LitValue evaluate_unary_op(LitValue value, LitTokenType op)
{
    switch(op)
    {
        case LITTOK_MINUS:
            {
                if(IS_NUMBER(value))
                {
                    optdbg("translating constant unary minus on number to literal value");
                    return lit_number_to_value(-lit_value_to_number(value));
                }
            }
            break;
        case LITTOK_BANG:
            {
                optdbg("translating constant expression of '=' to literal value");
                return BOOL_VALUE(lit_is_falsey(value));
            }
            break;
        case LITTOK_TILDE:
            {
                if(IS_NUMBER(value))
                {
                    optdbg("translating unary tile (~) on number to literal value");
                    return lit_number_to_value(~((int)lit_value_to_number(value)));
                }
            }
            break;
        default:
            {
            }
            break;
    }
    return NULL_VALUE;
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
                if(IS_NUMBER(a) && IS_NUMBER(b))
                {
                    return lit_number_to_value(floor(lit_value_to_number(a) / lit_value_to_number(b)));
                }
                return NULL_VALUE;
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
    return NULL_VALUE;
}

static LitValue attempt_to_optimize_binary(LitOptimizer* optimizer, LitBinaryExpr* expression, LitValue value, bool left)
{
    double number;
    LitTokenType op;
    op = expression->op;
    LitExpression* branch;
    branch = left ? expression->left : expression->right;
    if(IS_NUMBER(value))
    {
        number = lit_value_to_number(value);
        if(op == LITTOK_STAR)
        {
            if(number == 0)
            {
                optdbg("reducing expression to literal '0'");
                return lit_number_to_value(0);
            }
            else if(number == 1)
            {
                optdbg("reducing expression to literal '1'");
                lit_free_expression(optimizer->state, left ? expression->right : expression->left);
                expression->left = branch;
                expression->right = NULL;
            }
        }
        else if((op == LITTOK_PLUS || op == LITTOK_MINUS) && number == 0)
        {
            optdbg("reducing expression that would result in '0' to literal '0'");
            lit_free_expression(optimizer->state, left ? expression->right : expression->left);
            expression->left = branch;
            expression->right = NULL;
        }
        else if(((left && op == LITTOK_SLASH) || op == LITTOK_STAR_STAR) && number == 1)
        {
            optdbg("reducing expression that would result in '1' to literal '1'");
            lit_free_expression(optimizer->state, left ? expression->right : expression->left);
            expression->left = branch;
            expression->right = NULL;
        }
    }
    return NULL_VALUE;
}

static LitValue evaluate_expression(LitOptimizer* optimizer, LitExpression* expression)
{
    LitUnaryExpr* uexpr;
    LitBinaryExpr* bexpr;
    LitValue a;
    LitValue b;
    LitValue branch;
    if(expression == NULL)
    {
        return NULL_VALUE;
    }
    switch(expression->type)
    {
        case LITEXPR_LITERAL:
            {
                return ((LitLiteralExpr*)expression)->value;
            }
            break;
        case LITEXPR_UNARY:
            {
                uexpr = (LitUnaryExpr*)expression;
                branch = evaluate_expression(optimizer, uexpr->right);
                if(branch != NULL_VALUE)
                {
                    return evaluate_unary_op(branch, uexpr->op);
                }
            }
            break;
        case LITEXPR_BINARY:
            {
                bexpr = (LitBinaryExpr*)expression;
                a = evaluate_expression(optimizer, bexpr->left);
                b = evaluate_expression(optimizer, bexpr->right);
                if(a != NULL_VALUE && b != NULL_VALUE)
                {
                    return evaluate_binary_op(a, b, bexpr->op);
                }
                else if(a != NULL_VALUE)
                {
                    return attempt_to_optimize_binary(optimizer, bexpr, a, false);
                }
                else if(b != NULL_VALUE)
                {
                    return attempt_to_optimize_binary(optimizer, bexpr, b, true);
                }
            }
            break;
        default:
            {
                return NULL_VALUE;
            }
            break;
    }
    return NULL_VALUE;
}

static void optimize_expression(LitOptimizer* optimizer, LitExpression** slot)
{
    LitExpression* expression = *slot;

    if(expression == NULL)
    {
        return;
    }

    LitState* state = optimizer->state;

    switch(expression->type)
    {
        case LITEXPR_UNARY:
        case LITEXPR_BINARY:
            {
                if(lit_is_optimization_enabled(LITOPTSTATE_LITERAL_FOLDING))
                {
                    LitValue optimized = evaluate_expression(optimizer, expression);
                    if(optimized != NULL_VALUE)
                    {
                        *slot = (LitExpression*)lit_create_literal_expression(state, expression->line, optimized);
                        lit_free_expression(state, expression);
                        break;
                    }
                }
                switch(expression->type)
                {
                    case LITEXPR_UNARY:
                        {
                            optimize_expression(optimizer, &((LitUnaryExpr*)expression)->right);
                        }
                        break;
                    case LITEXPR_BINARY:
                        {
                            LitBinaryExpr* expr = (LitBinaryExpr*)expression;
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
        case LITEXPR_ASSIGN:
            {
                LitAssignExpression* expr = (LitAssignExpression*)expression;
                optimize_expression(optimizer, &expr->to);
                optimize_expression(optimizer, &expr->value);
            }
            break;
        case LITEXPR_CALL:
            {
                LitCallExpression* expr = (LitCallExpression*)expression;
                optimize_expression(optimizer, &expr->callee);
                optimize_expressions(optimizer, &expr->args);
            }
            break;
        case LITEXPR_SET:
            {
                LitSetExpression* expr = (LitSetExpression*)expression;
                optimize_expression(optimizer, &expr->where);
                optimize_expression(optimizer, &expr->value);
            }
            break;
        case LITEXPR_GET:
            {
                optimize_expression(optimizer, &((LitGetExpression*)expression)->where);
            }
            break;
        case LITEXPR_LAMBDA:
            {
                opt_begin_scope(optimizer);
                optimize_statement(optimizer, &((LitLambdaExpression*)expression)->body);
                opt_end_scope(optimizer);
            }
            break;

        case LITEXPR_ARRAY:
        {
            optimize_expressions(optimizer, &((LitArrayExpression*)expression)->values);
            break;
        }

        case LITEXPR_OBJECT:
        {
            optimize_expressions(optimizer, &((LitObjectExpression*)expression)->values);
            break;
        }

        case LITEXPR_SUBSCRIPT:
        {
            LitSubscriptExpression* expr = (LitSubscriptExpression*)expression;

            optimize_expression(optimizer, &expr->array);
            optimize_expression(optimizer, &expr->index);

            break;
        }

        case LITEXPR_RANGE:
        {
            LitRangeExpression* expr = (LitRangeExpression*)expression;

            optimize_expression(optimizer, &expr->from);
            optimize_expression(optimizer, &expr->to);

            break;
        }

        case LITEXPR_IF:
        {
            LitIfExpression* expr = (LitIfExpression*)expression;
            LitValue optimized = evaluate_expression(optimizer, expr->condition);

            if(optimized != NULL_VALUE)
            {
                if(lit_is_falsey(optimized))
                {
                    *slot = expr->else_branch;
                    expr->else_branch = NULL;// So that it doesn't get freed
                }
                else
                {
                    *slot = expr->if_branch;
                    expr->if_branch = NULL;// So that it doesn't get freed
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

        case LITEXPR_INTERPOLATION:
        {
            optimize_expressions(optimizer, &((LitInterpolationExpression*)expression)->expressions);
            break;
        }

        case LITEXPR_VAR:
        {
            LitVarExpr* expr = (LitVarExpr*)expression;
            LitVariable* variable = resolve_variable(optimizer, expr->name, expr->length);

            if(variable != NULL)
            {
                variable->used = true;

                // Not checking here for the enable-ness of constant-folding, since if its off
                // the constant_value would be NULL_VALUE anyway (:thinkaboutit:)
                if(variable->constant && variable->constant_value != NULL_VALUE)
                {
                    *slot = (LitExpression*)lit_create_literal_expression(state, expression->line, variable->constant_value);
                    lit_free_expression(state, expression);
                }
            }

            break;
        }

        case LITEXPR_REFERENCE:
        {
            optimize_expression(optimizer, &((LitReferenceExpression*)expression)->to);
            break;
        }

        case LITEXPR_LITERAL:
        case LITEXPR_THIS:
        case LITEXPR_SUPER:
        {
            // Nothing, that we can do here
            break;
        }
    }
}

static void optimize_expressions(LitOptimizer* optimizer, PCGenericArray<LitExpression*>* expressions)
{
    for(size_t i = 0; i < expressions->m_count; i++)
    {
        optimize_expression(optimizer, &expressions->m_values[i]);
    }
}

static void optimize_statement(LitOptimizer* optimizer, LitStatement** slot)
{
    LitState* state;
    LitStatement* statement;
    statement = *slot;
    if(statement == NULL)
    {
        return;
    }
    state = optimizer->state;
    switch(statement->type)
    {
        case LITSTMT_EXPRESSION:
            {
                optimize_expression(optimizer, &((LitExpressionStatement*)statement)->expression);
            }
            break;
        case LITSTMT_BLOCK:
            {
                LitBlockStatement* stmt;
                stmt = (LitBlockStatement*)statement;
                if(stmt->statements.m_count == 0)
                {
                    lit_free_statement(state, statement);
                    *slot = NULL;
                    break;
                }
                opt_begin_scope(optimizer);
                optimize_statements(optimizer, &stmt->statements);
                opt_end_scope(optimizer);
                bool found = false;
                for(size_t i = 0; i < stmt->statements.m_count; i++)
                {
                    LitStatement* step = stmt->statements.m_values[i];
                    if(!is_empty(step))
                    {
                        found = true;
                        if(step->type == LITSTMT_RETURN)
                        {
                            // Remove all the statements post return
                            for(size_t j = i + 1; j < stmt->statements.m_count; j++)
                            {
                                step = stmt->statements.m_values[j];
                                if(step != NULL)
                                {
                                    lit_free_statement(state, step);
                                    stmt->statements.m_values[j] = NULL;
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
                    *slot = NULL;
                }
            }
            break;

        case LITSTMT_IF:
        {
            LitIfStatement* stmt = (LitIfStatement*)statement;

            optimize_expression(optimizer, &stmt->condition);
            optimize_statement(optimizer, &stmt->if_branch);

            bool empty = lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY);
            bool dead = lit_is_optimization_enabled(LITOPTSTATE_UNREACHABLE_CODE);

            LitValue optimized = empty ? evaluate_expression(optimizer, stmt->condition) : NULL_VALUE;

            if((optimized != NULL_VALUE && lit_is_falsey(optimized)) || (dead && is_empty(stmt->if_branch)))
            {
                lit_free_expression(state, stmt->condition);
                stmt->condition = NULL;

                lit_free_statement(state, stmt->if_branch);
                stmt->if_branch = NULL;
            }

            if(stmt->elseif_conditions != NULL)
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
                            stmt->elseif_conditions->m_values[i] = NULL;

                            lit_free_statement(state, stmt->elseif_branches->m_values[i]);
                            stmt->elseif_branches->m_values[i] = NULL;

                            continue;
                        }

                        if(dead)
                        {
                            LitValue value = evaluate_expression(optimizer, stmt->elseif_conditions->m_values[i]);

                            if(value != NULL_VALUE && lit_is_falsey(value))
                            {
                                lit_free_expression(state, stmt->elseif_conditions->m_values[i]);
                                stmt->elseif_conditions->m_values[i] = NULL;

                                lit_free_statement(state, stmt->elseif_branches->m_values[i]);
                                stmt->elseif_branches->m_values[i] = NULL;
                            }
                        }
                    }
                }
            }

            optimize_statement(optimizer, &stmt->else_branch);
            break;
        }

        case LITSTMT_WHILE:
        {
            LitWhileStatement* stmt = (LitWhileStatement*)statement;
            optimize_expression(optimizer, &stmt->condition);

            if(lit_is_optimization_enabled(LITOPTSTATE_UNREACHABLE_CODE))
            {
                LitValue optimized = evaluate_expression(optimizer, stmt->condition);

                if(optimized != NULL_VALUE && lit_is_falsey(optimized))
                {
                    lit_free_statement(optimizer->state, statement);
                    *slot = NULL;
                    break;
                }
            }

            optimize_statement(optimizer, &stmt->body);

            if(lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
            {
                lit_free_statement(optimizer->state, statement);
                *slot = NULL;
            }

            break;
        }

        case LITSTMT_FOR:
            {
                LitForStatement* stmt = (LitForStatement*)statement;
                opt_begin_scope(optimizer);
                // This is required, so that optimizer doesn't optimize out our i variable (and such)
                optimizer->mark_used = true;
                optimize_expression(optimizer, &stmt->init);
                optimize_expression(optimizer, &stmt->condition);
                optimize_expression(optimizer, &stmt->increment);
                optimize_statement(optimizer, &stmt->var);
                optimizer->mark_used = false;
                optimize_statement(optimizer, &stmt->body);
                opt_end_scope(optimizer);
                if(lit_is_optimization_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                {
                    lit_free_statement(optimizer->state, statement);
                    *slot = NULL;
                    break;
                }
                if(stmt->c_style || !lit_is_optimization_enabled(LITOPTSTATE_C_FOR) || stmt->condition->type != LITEXPR_RANGE)
                {
                    break;
                }
                LitRangeExpression* range = (LitRangeExpression*)stmt->condition;
                LitValue from = evaluate_expression(optimizer, range->from);
                LitValue to = evaluate_expression(optimizer, range->to);
                if(!IS_NUMBER(from) || !IS_NUMBER(to))
                {
                    break;
                }
                bool reverse = lit_value_to_number(from) > lit_value_to_number(to);
                LitVarStatement* var = (LitVarStatement*)stmt->var;
                size_t line = range->expression.line;
                // var i = from
                var->init = range->from;
                // i <= to
                stmt->condition = (LitExpression*)lit_create_binary_expression(
                state, line, (LitExpression*)lit_create_var_expression(state, line, var->name, var->length), range->to, LITTOK_LESS_EQUAL);
                // i++ (or i--)
                LitExpression* var_get = (LitExpression*)lit_create_var_expression(state, line, var->name, var->length);
                LitBinaryExpr* assign_value = lit_create_binary_expression(
                state, line, var_get, (LitExpression*)lit_create_literal_expression(state, line, lit_number_to_value(1)),
                reverse ? LITTOK_MINUS_MINUS : LITTOK_PLUS);
                assign_value->ignore_left = true;
                LitExpression* increment
                = (LitExpression*)lit_create_assign_expression(state, line, var_get, (LitExpression*)assign_value);
                stmt->increment = (LitExpression*)increment;
                range->from = NULL;
                range->to = NULL;
                stmt->c_style = true;
                lit_free_expression(state, (LitExpression*)range);
            }
            break;

        case LITSTMT_VAR:
            {
                LitVarStatement* stmt = (LitVarStatement*)statement;
                LitVariable* variable = add_variable(optimizer, stmt->name, stmt->length, stmt->constant, slot);
                optimize_expression(optimizer, &stmt->init);
                if(stmt->constant && lit_is_optimization_enabled(LITOPTSTATE_CONSTANT_FOLDING))
                {
                    LitValue value = evaluate_expression(optimizer, stmt->init);

                    if(value != NULL_VALUE)
                    {
                        variable->constant_value = value;
                    }
                }
            }
            break;
        case LITSTMT_FUNCTION:
            {
                LitFunctionStatement* stmt = (LitFunctionStatement*)statement;
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
        case LITSTMT_RETURN:
            {
                optimize_expression(optimizer, &((LitReturnStatement*)statement)->expression);
            }
            break;
        case LITSTMT_METHOD:
            {
                opt_begin_scope(optimizer);
                optimize_statement(optimizer, &((LitMethodStatement*)statement)->body);
                opt_end_scope(optimizer);
            }
            break;
        case LITSTMT_CLASS:
            {
                optimize_statements(optimizer, &((LitClassStatement*)statement)->fields);
            }
            break;
        case LITSTMT_FIELD:
            {
                LitFieldStatement* stmt = (LitFieldStatement*)statement;
                if(stmt->getter != NULL)
                {
                    opt_begin_scope(optimizer);
                    optimize_statement(optimizer, &stmt->getter);
                    opt_end_scope(optimizer);
                }
                if(stmt->setter != NULL)
                {
                    opt_begin_scope(optimizer);
                    optimize_statement(optimizer, &stmt->setter);
                    opt_end_scope(optimizer);
                }
            }
            break;
        // Nothing to optimize there
        case LITSTMT_CONTINUE:
        case LITSTMT_BREAK:
            {
            }
            break;

    }
}

static void optimize_statements(LitOptimizer* optimizer, PCGenericArray<LitStatement*>* statements)
{
    size_t i;
    for(i = 0; i < statements->m_count; i++)
    {
        optimize_statement(optimizer, &statements->m_values[i]);
    }
}

void lit_optimize(LitOptimizer* optimizer, PCGenericArray<LitStatement*>* statements)
{
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
    optimizer->variables.release(optimizer->state);
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



static jmp_buf prs_jmpbuffer;
static LitParseRule rules[LITTOK_EOF + 1];


static LitTokenType operators[]=
{
    LITTOK_PLUS, LITTOK_MINUS, LITTOK_STAR, LITTOK_PERCENT, LITTOK_SLASH,
    LITTOK_SHARP, LITTOK_BANG, LITTOK_LESS, LITTOK_LESS_EQUAL, LITTOK_GREATER,
    LITTOK_GREATER_EQUAL, LITTOK_EQUAL_EQUAL, LITTOK_LEFT_BRACKET, LITTOK_EOF
};


static bool did_setup_rules;
static void setup_rules();
static void sync(LitParser* parser);

static LitStatement *parse_block(LitParser *parser);
static LitExpression *parse_precedence(LitParser *parser, LitPrecedence precedence, bool err);
static LitExpression *parse_number(LitParser *parser, bool can_assign);
static LitExpression *parse_lambda(LitParser *parser, LitLambdaExpression *lambda);
static void parse_parameters(LitParser *parser, PCGenericArray<LitParameter> *parameters);
static LitExpression *parse_grouping_or_lambda(LitParser *parser, bool can_assign);
static LitExpression *parse_call(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_unary(LitParser *parser, bool can_assign);
static LitExpression *parse_binary(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_and(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_or(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_null_filter(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_compound(LitParser *parser, LitExpression *prev, bool can_assign);
static LitExpression *parse_literal(LitParser *parser, bool can_assign);
static LitExpression *parse_string(LitParser *parser, bool can_assign);
static LitExpression *parse_interpolation(LitParser *parser, bool can_assign);
static LitExpression *parse_object(LitParser *parser, bool can_assign);
static LitExpression *parse_variable_expression_base(LitParser *parser, bool can_assign, bool isnew);
static LitExpression *parse_variable_expression(LitParser *parser, bool can_assign);
static LitExpression *parse_new_expression(LitParser *parser, bool can_assign);
static LitExpression *parse_dot(LitParser *parser, LitExpression *previous, bool can_assign);
static LitExpression *parse_range(LitParser *parser, LitExpression *previous, bool can_assign);
static LitExpression *parse_ternary_or_question(LitParser *parser, LitExpression *previous, bool can_assign);
static LitExpression *parse_array(LitParser *parser, bool can_assign);
static LitExpression *parse_subscript(LitParser *parser, LitExpression *previous, bool can_assign);
static LitExpression *parse_this(LitParser *parser, bool can_assign);
static LitExpression *parse_super(LitParser *parser, bool can_assign);
static LitExpression *parse_reference(LitParser *parser, bool can_assign);
static LitExpression *parse_expression(LitParser *parser);
static LitStatement *parse_var_declaration(LitParser *parser);
static LitStatement *parse_if(LitParser *parser);
static LitStatement *parse_for(LitParser *parser);
static LitStatement *parse_while(LitParser *parser);
static LitStatement *parse_function(LitParser *parser);
static LitStatement *parse_return(LitParser *parser);
static LitStatement *parse_field(LitParser *parser, LitString *name, bool is_static);
static LitStatement *parse_method(LitParser *parser, bool is_static);
static LitStatement *parse_class(LitParser *parser);
static LitStatement *parse_statement(LitParser *parser);
static LitStatement *parse_declaration(LitParser *parser);

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
    rules[LITTOK_LEFT_PAREN] = (LitParseRule){ parse_grouping_or_lambda, parse_call, LITPREC_CALL };
    rules[LITTOK_PLUS] = (LitParseRule){ NULL, parse_binary, LITPREC_TERM };
    rules[LITTOK_MINUS] = (LitParseRule){ parse_unary, parse_binary, LITPREC_TERM };
    rules[LITTOK_BANG] = (LitParseRule){ parse_unary, parse_binary, LITPREC_TERM };
    rules[LITTOK_STAR] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR_STAR] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_SLASH] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_SHARP] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_STAR] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_BAR] = (LitParseRule){ NULL, parse_binary, LITPREC_BOR };
    rules[LITTOK_AMPERSAND] = (LitParseRule){ NULL, parse_binary, LITPREC_BAND };
    rules[LITTOK_TILDE] = (LitParseRule){ parse_unary, NULL, LITPREC_UNARY };
    rules[LITTOK_CARET] = (LitParseRule){ NULL, parse_binary, LITPREC_BOR };
    rules[LITTOK_LESS_LESS] = (LitParseRule){ NULL, parse_binary, LITPREC_SHIFT };
    rules[LITTOK_GREATER_GREATER] = (LitParseRule){ NULL, parse_binary, LITPREC_SHIFT };
    rules[LITTOK_PERCENT] = (LitParseRule){ NULL, parse_binary, LITPREC_FACTOR };
    rules[LITTOK_IS] = (LitParseRule){ NULL, parse_binary, LITPREC_IS };
    rules[LITTOK_NUMBER] = (LitParseRule){ parse_number, NULL, LITPREC_NONE };
    rules[LITTOK_TRUE] = (LitParseRule){ parse_literal, NULL, LITPREC_NONE };
    rules[LITTOK_FALSE] = (LitParseRule){ parse_literal, NULL, LITPREC_NONE };
    rules[LITTOK_NULL] = (LitParseRule){ parse_literal, NULL, LITPREC_NONE };
    rules[LITTOK_BANG_EQUAL] = (LitParseRule){ NULL, parse_binary, LITPREC_EQUALITY };
    rules[LITTOK_EQUAL_EQUAL] = (LitParseRule){ NULL, parse_binary, LITPREC_EQUALITY };
    rules[LITTOK_GREATER] = (LitParseRule){ NULL, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_GREATER_EQUAL] = (LitParseRule){ NULL, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_LESS] = (LitParseRule){ NULL, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_LESS_EQUAL] = (LitParseRule){ NULL, parse_binary, LITPREC_COMPARISON };
    rules[LITTOK_STRING] = (LitParseRule){ parse_string, NULL, LITPREC_NONE };
    rules[LITTOK_INTERPOLATION] = (LitParseRule){ parse_interpolation, NULL, LITPREC_NONE };
    rules[LITTOK_IDENTIFIER] = (LitParseRule){ parse_variable_expression, NULL, LITPREC_NONE };
    rules[LITTOK_NEW] = (LitParseRule){ parse_new_expression, NULL, LITPREC_NONE };
    rules[LITTOK_PLUS_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_MINUS_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_STAR_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_SLASH_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_SHARP_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_PERCENT_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_CARET_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_BAR_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_AMPERSAND_EQUAL] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_PLUS_PLUS] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_MINUS_MINUS] = (LitParseRule){ NULL, parse_compound, LITPREC_COMPOUND };
    rules[LITTOK_AMPERSAND_AMPERSAND] = (LitParseRule){ NULL, parse_and, LITPREC_AND };
    rules[LITTOK_BAR_BAR] = (LitParseRule){ NULL, parse_or, LITPREC_AND };
    rules[LITTOK_QUESTION_QUESTION] = (LitParseRule){ NULL, parse_null_filter, LITPREC_NULL };
    rules[LITTOK_DOT] = (LitParseRule){ NULL, parse_dot, LITPREC_CALL };
    rules[LITTOK_SMALL_ARROW] = (LitParseRule){ NULL, parse_dot, LITPREC_CALL };
    rules[LITTOK_DOT_DOT] = (LitParseRule){ NULL, parse_range, LITPREC_RANGE };
    rules[LITTOK_DOT_DOT_DOT] = (LitParseRule){ parse_variable_expression, NULL, LITPREC_ASSIGNMENT };
    rules[LITTOK_LEFT_BRACKET] = (LitParseRule){ parse_array, parse_subscript, LITPREC_NONE };
    rules[LITTOK_LEFT_BRACE] = (LitParseRule){ parse_object, NULL, LITPREC_NONE };
    rules[LITTOK_THIS] = (LitParseRule){ parse_this, NULL, LITPREC_NONE };
    rules[LITTOK_SUPER] = (LitParseRule){ parse_super, NULL, LITPREC_NONE };
    rules[LITTOK_QUESTION] = (LitParseRule){ NULL, parse_ternary_or_question, LITPREC_EQUALITY };
    rules[LITTOK_REF] = (LitParseRule){ parse_reference, NULL, LITPREC_NONE };
    //rules[LITTOK_SEMICOLON] = (LitParseRule){NULL, NULL, LITPREC_NONE};
}


static void prs_init_compiler(LitParser* parser, LitCompiler* compiler)
{
    compiler->scope_depth = 0;
    compiler->function = NULL;
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

static LitParseRule* get_rule(LitTokenType type)
{
    return &rules[type];
}

static inline bool prs_is_at_end(LitParser* parser)
{
    return parser->current.type == LITTOK_EOF;
}

void lit_init_parser(LitState* state, LitParser* parser)
{
    if(!did_setup_rules)
    {
        did_setup_rules = true;
        setup_rules();
    }
    parser->state = state;
    parser->had_error = false;
    parser->panic_mode = false;
}

void lit_free_parser(LitParser* parser)
{
    (void)parser;
}

static void string_error(LitParser* parser, LitToken* token, const char* message)
{
    (void)token;
    if(parser->panic_mode)
    {
        return;
    }
    lit_error(parser->state, COMPILE_ERROR, message);
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
    fmt = lit_format_error(parser->state, parser->current.line, LITERROR_EXPECTATION_UNMET, error, olen, otext)->chars;
    string_error(parser, &parser->current,fmt);
}

static LitStatement* parse_block(LitParser* parser)
{
    LitBlockStatement* statement;
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
    return (LitStatement*)statement;
}

static LitExpression* parse_precedence(LitParser* parser, LitPrecedence precedence, bool err)
{
    bool new_line;
    bool prev_newline;
    bool parser_prev_newline;
    bool can_assign;
    LitExpression* expr;
    LitPrefixParseFn prefix_rule;
    LitInfixParseFn infix_rule;
    LitToken previous;
    (void)new_line;
    prev_newline = false;
    previous = parser->previous;
    prs_advance(parser);
    prefix_rule = get_rule(parser->previous.type)->prefix;
    if(prefix_rule == NULL)
    {
        //fprintf(stderr, "parser->previous.type=%s, parser->current.type=%s\n", token_name(parser->previous.type), token_name(parser->current.type));
        if(parser->previous.type == LITTOK_SEMICOLON)
        {
            if(parser->current.type == LITTOK_NEW_LINE)
            {
                prs_advance(parser);
                return parse_precedence(parser, precedence, err);
            }
            return NULL;
        }
        // todo: file start
        new_line = previous.start != NULL && *previous.start == '\n';
        parser_prev_newline = parser->previous.start != NULL && *parser->previous.start == '\n';
        prs_error(parser, LITERROR_EXPECTED_EXPRESSION,
            (prev_newline ? 8 : previous.length),
            (prev_newline ? "new line" : previous.start),
            (parser_prev_newline ? 8 : parser->previous.length),
            (parser_prev_newline ? "new line" : parser->previous.start)
        );
        return NULL;
        
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
        prs_error(parser, LITERROR_INVALID_ASSIGMENT_TARGET);
    }
    return expr;
}

static LitExpression* parse_number(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    return (LitExpression*)lit_create_literal_expression(parser->state, parser->previous.line, parser->previous.value);
}

static LitExpression* parse_lambda(LitParser* parser, LitLambdaExpression* lambda)
{
    lambda->body = parse_statement(parser);
    return (LitExpression*)lambda;
}

static void parse_parameters(LitParser* parser, PCGenericArray<LitParameter>* parameters)
{
    bool had_default;
    size_t arg_length;
    const char* arg_name;
    LitExpression* default_value;
    had_default = false;
    while(!check(parser, LITTOK_RIGHT_PAREN))
    {
        // Vararg ...
        if(prs_match(parser, LITTOK_DOT_DOT_DOT))
        {
            parameters->push((LitParameter){ "...", 3, NULL });
            return;
        }
        consume(parser, LITTOK_IDENTIFIER, "argument name");
        arg_name = parser->previous.start;
        arg_length = parser->previous.length;
        default_value = NULL;
        if(prs_match(parser, LITTOK_EQUAL))
        {
            had_default = true;
            default_value = parse_expression(parser);
        }
        else if(had_default)
        {
            prs_error(parser, LITERROR_DEFAULT_ARG_CENTRED);
        }
        parameters->push((LitParameter){ arg_name, arg_length, default_value });
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
    }
}

static LitExpression* parse_grouping_or_lambda(LitParser* parser, bool can_assign)
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
    LitExpression* expression;
    LitExpression* default_value;
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
            LitLambdaExpression* lambda = lit_create_lambda_expression(state, line);
            LitExpression* def_value = NULL;
            had_default = prs_match(parser, LITTOK_EQUAL);
            if(had_default)
            {
                def_value = parse_expression(parser);
            }
            lambda->parameters.push((LitParameter){ first_arg_start, first_arg_length, def_value });
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
                    default_value = NULL;
                    if(prs_match(parser, LITTOK_EQUAL))
                    {
                        default_value = parse_expression(parser);
                        had_default = true;
                    }
                    else if(had_default)
                    {
                        prs_error(parser, LITERROR_DEFAULT_ARG_CENTRED);
                    }
                    lambda->parameters.push((LitParameter){ arg_name, arg_length, default_value });
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

static LitExpression* parse_call(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    LitExpression* e;
    LitVarExpr* ee;
    LitCallExpression* expression;
    expression = lit_create_call_expression(parser->state, parser->previous.line, prev);
    while(!check(parser, LITTOK_RIGHT_PAREN))
    {
        e = parse_expression(parser);
        expression->args.push(e);
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
        if(e->type == LITEXPR_VAR)
        {
            ee = (LitVarExpr*)e;
            // Vararg ...
            if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
            {
                break;
            }
        }
    }
    if(expression->args.m_count > 255)
    {
        prs_error(parser, LITERROR_TOO_MANY_FUNCTION_ARGS, (int)expression->args.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after arguments");
    return (LitExpression*)expression;
}

static LitExpression* parse_unary(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitExpression* expression;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    expression = parse_precedence(parser, LITPREC_UNARY, true);
    return (LitExpression*)lit_create_unary_expression(parser->state, line, expression, op);
}

static LitExpression* parse_binary(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    bool invert;
    size_t line;
    LitParseRule* rule;
    LitExpression* expression;
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
    expression = (LitExpression*)lit_create_binary_expression(parser->state, line, prev, expression, op);
    if(invert)
    {
        expression = (LitExpression*)lit_create_unary_expression(parser->state, line, expression, LITTOK_BANG);
    }
    return expression;
}

static LitExpression* parse_and(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (LitExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_AND, true), op);
}

static LitExpression* parse_or(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (LitExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_OR, true), op);
}

static LitExpression* parse_null_filter(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    return (LitExpression*)lit_create_binary_expression(parser->state, line, prev, parse_precedence(parser, LITPREC_NULL, true), op);
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

static LitExpression* parse_compound(LitParser* parser, LitExpression* prev, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitBinaryExpr* binary;
    LitExpression* expression;
    LitParseRule* rule;
    LitTokenType op;
    op = parser->previous.type;
    line = parser->previous.line;
    rule = get_rule(op);
    if(op == LITTOK_PLUS_PLUS || op == LITTOK_MINUS_MINUS)
    {
        expression = (LitExpression*)lit_create_literal_expression(parser->state, line, lit_number_to_value(1));
    }
    else
    {
        expression = parse_precedence(parser, (LitPrecedence)(rule->precedence + 1), true);
    }
    binary = lit_create_binary_expression(parser->state, line, prev, expression, convert_compound_operator(op));
    binary->ignore_left = true;// To make sure we don't free it twice
    return (LitExpression*)lit_create_assign_expression(parser->state, line, prev, (LitExpression*)binary);
}

static LitExpression* parse_literal(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    line = parser->previous.line;
    switch(parser->previous.type)
    {
        case LITTOK_TRUE:
            {
                return (LitExpression*)lit_create_literal_expression(parser->state, line, TRUE_VALUE);
            }
            break;
        case LITTOK_FALSE:
            {
                return (LitExpression*)lit_create_literal_expression(parser->state, line, FALSE_VALUE);
            }
            break;
        case LITTOK_NULL:
            {
                return (LitExpression*)lit_create_literal_expression(parser->state, line, NULL_VALUE);
            }
            break;
        default:
            {
                UNREACHABLE
            }
            break;
    }
    return NULL;
}

static LitExpression* parse_string(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    LitExpression* expression;
    expression = (LitExpression*)lit_create_literal_expression(parser->state, parser->previous.line, parser->previous.value);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static LitExpression* parse_interpolation(LitParser* parser, bool can_assign)
{
    LitInterpolationExpression* expression;
    (void)can_assign;
    expression = lit_create_interpolation_expression(parser->state, parser->previous.line);
    do
    {
        if(lit_string_length(lit_as_string(parser->previous.value)) > 0)
        {
            expression->expressions.push((LitExpression*)lit_create_literal_expression(parser->state, parser->previous.line, parser->previous.value));
        }
        expression->expressions.push(parse_expression(parser));
    } while(prs_match(parser, LITTOK_INTERPOLATION));
    consume(parser, LITTOK_STRING, "end of interpolation");
    if(lit_string_length(lit_as_string(parser->previous.value)) > 0)
    {
        expression->expressions.push((LitExpression*)lit_create_literal_expression(parser->state, parser->previous.line, parser->previous.value));
    }
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, (LitExpression*)expression, can_assign);
    }
    return (LitExpression*)expression;
}

static LitExpression* parse_object(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    LitObjectExpression* object;
    object = lit_create_object_expression(parser->state, parser->previous.line);
    ignore_new_lines(parser);
    while(!check(parser, LITTOK_RIGHT_BRACE))
    {
        ignore_new_lines(parser);
        consume(parser, LITTOK_IDENTIFIER, "key string after '{'");
        object->keys.push(OBJECT_VALUE(lit_string_copy(parser->state, parser->previous.start, parser->previous.length)));
        ignore_new_lines(parser);
        consume(parser, LITTOK_EQUAL, "'=' after key string");
        ignore_new_lines(parser);
        object->values.push(parse_expression(parser));
        if(!prs_match(parser, LITTOK_COMMA))
        {
            break;
        }
    }
    ignore_new_lines(parser);
    consume(parser, LITTOK_RIGHT_BRACE, "'}' after object");
    return (LitExpression*)object;
}

static LitExpression* parse_variable_expression_base(LitParser* parser, bool can_assign, bool isnew)
{
    (void)can_assign;
    bool had_args;
    LitCallExpression* call;
    LitExpression* expression;
    expression = (LitExpression*)lit_create_var_expression(parser->state, parser->previous.line, parser->previous.start, parser->previous.length);
    if(isnew)
    {
        had_args = check(parser, LITTOK_LEFT_PAREN);
        call = NULL;
        if(had_args)
        {
            prs_advance(parser);
            call = (LitCallExpression*)parse_call(parser, expression, false);
        }
        if(prs_match(parser, LITTOK_LEFT_BRACE))
        {
            if(call == NULL)
            {
                call = lit_create_call_expression(parser->state, expression->line, expression);
            }
            call->init = parse_object(parser, false);
        }
        else if(!had_args)
        {
            error_at_current(parser, LITERROR_EXPECTATION_UNMET, "argument list for instance creation",
                             parser->previous.length, parser->previous.start);
        }
        return (LitExpression*)call;
    }
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    if(can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        return (LitExpression*)lit_create_assign_expression(parser->state, parser->previous.line, expression,
                                                            parse_expression(parser));
    }
    return expression;
}

static LitExpression* parse_variable_expression(LitParser* parser, bool can_assign)
{
    return parse_variable_expression_base(parser, can_assign, false);
}

static LitExpression* parse_new_expression(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    consume(parser, LITTOK_IDENTIFIER, "class name after 'new'");
    return parse_variable_expression_base(parser, false, true);
}

static LitExpression* parse_dot(LitParser* parser, LitExpression* previous, bool can_assign)
{
    (void)can_assign;
    bool ignored;
    size_t line;
    size_t length;
    const char* name;
    LitExpression* expression;
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
        return (LitExpression*)lit_create_set_expression(parser->state, line, previous, name, length, parse_expression(parser));
    }
    expression = (LitExpression*)lit_create_get_expression(parser->state, line, previous, name, length, false, ignored);
    if(!ignored && prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static LitExpression* parse_range(LitParser* parser, LitExpression* previous, bool can_assign)
{
    (void)can_assign;
    size_t line;
    line = parser->previous.line;
    return (LitExpression*)lit_create_range_expression(parser->state, line, previous, parse_expression(parser));
}

static LitExpression* parse_ternary_or_question(LitParser* parser, LitExpression* previous, bool can_assign)
{
    (void)can_assign;
    bool ignored;
    size_t line;
    LitExpression* if_branch;
    LitExpression* else_branch;
    line = parser->previous.line;
    if(prs_match(parser, LITTOK_DOT) || prs_match(parser, LITTOK_SMALL_ARROW))
    {
        ignored = parser->previous.type == LITTOK_SMALL_ARROW;
        consume(parser, LITTOK_IDENTIFIER, ignored ? "property name after '->'" : "property name after '.'");
        return (LitExpression*)lit_create_get_expression(parser->state, line, previous, parser->previous.start,
                                                         parser->previous.length, true, ignored);
    }
    if_branch = parse_expression(parser);
    consume(parser, LITTOK_COLON, "':' after expression");
    else_branch = parse_expression(parser);
    return (LitExpression*)lit_create_if_experssion(parser->state, line, previous, if_branch, else_branch);
}

static LitExpression* parse_array(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    LitArrayExpression* array;
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
        return parse_subscript(parser, (LitExpression*)array, can_assign);
    }
    return (LitExpression*)array;
}

static LitExpression* parse_subscript(LitParser* parser, LitExpression* previous, bool can_assign)
{
    size_t line;
    LitExpression* index;
    LitExpression* expression;
    line = parser->previous.line;
    index = parse_expression(parser);
    consume(parser, LITTOK_RIGHT_BRACKET, "']' after subscript");
    expression = (LitExpression*)lit_create_subscript_expression(parser->state, line, previous, index);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    else if(can_assign && prs_match(parser, LITTOK_EQUAL))
    {
        return (LitExpression*)lit_create_assign_expression(parser->state, parser->previous.line, expression, parse_expression(parser));
    }
    return expression;
}

static LitExpression* parse_this(LitParser* parser, bool can_assign)
{
    LitExpression* expression;
    expression = (LitExpression*)lit_create_this_expression(parser->state, parser->previous.line);
    if(prs_match(parser, LITTOK_LEFT_BRACKET))
    {
        return parse_subscript(parser, expression, can_assign);
    }
    return expression;
}

static LitExpression* parse_super(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    bool ignoring;
    size_t line;
    LitExpression* expression;
    line = parser->previous.line;

    if(!(prs_match(parser, LITTOK_DOT) || prs_match(parser, LITTOK_SMALL_ARROW)))
    {
        expression = (LitExpression*)lit_create_super_expression(
        parser->state, line, lit_string_copy(parser->state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1), false);
        consume(parser, LITTOK_LEFT_PAREN, "'(' after 'super'");
        return parse_call(parser, expression, false);
    }
    ignoring = parser->previous.type == LITTOK_SMALL_ARROW;
    consume(parser, LITTOK_IDENTIFIER, ignoring ? "super method name after '->'" : "super method name after '.'");
    expression = (LitExpression*)lit_create_super_expression(
    parser->state, line, lit_string_copy(parser->state, parser->previous.start, parser->previous.length), ignoring);
    if(prs_match(parser, LITTOK_LEFT_PAREN))
    {
        return parse_call(parser, expression, false);
    }
    return expression;
}

static LitExpression* parse_reference(LitParser* parser, bool can_assign)
{
    (void)can_assign;
    size_t line;
    LitReferenceExpression* expression;
    line = parser->previous.line;
    ignore_new_lines(parser);
    expression = lit_create_reference_expression(parser->state, line, parse_precedence(parser, LITPREC_CALL, false));
    if(prs_match(parser, LITTOK_EQUAL))
    {
        return (LitExpression*)lit_create_assign_expression(parser->state, line, (LitExpression*)expression, parse_expression(parser));
    }
    return (LitExpression*)expression;
}

static LitExpression* parse_expression(LitParser* parser)
{
    ignore_new_lines(parser);
    return parse_precedence(parser, LITPREC_ASSIGNMENT, true);
}

static LitStatement* parse_var_declaration(LitParser* parser)
{
    bool constant;
    size_t line;
    size_t length;
    const char* name;
    LitExpression* init;
    constant = parser->previous.type == LITTOK_CONST;
    line = parser->previous.line;
    consume(parser, LITTOK_IDENTIFIER, "variable name");
    name = parser->previous.start;
    length = parser->previous.length;
    init = NULL;
    if(prs_match(parser, LITTOK_EQUAL))
    {
        init = parse_expression(parser);
    }
    return (LitStatement*)lit_create_var_statement(parser->state, line, name, length, init, constant);
}

static LitStatement* parse_if(LitParser* parser)
{
    size_t line;
    bool invert;
    bool had_paren;
    LitExpression* condition;
    LitStatement* if_branch;
    PCGenericArray<LitExpression*>* elseif_conditions;
    PCGenericArray<LitStatement*>* elseif_branches;
    LitStatement* else_branch;
    LitExpression* e;
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
        condition = (LitExpression*)lit_create_unary_expression(parser->state, condition->line, condition, LITTOK_BANG);
    }
    ignore_new_lines(parser);
    if_branch = parse_statement(parser);
    elseif_conditions = NULL;
    elseif_branches = NULL;
    else_branch = NULL;
    ignore_new_lines(parser);
    while(prs_match(parser, LITTOK_ELSE))
    {
        // else if
        ignore_new_lines(parser);
        if(prs_match(parser, LITTOK_IF))
        {
            if(elseif_conditions == NULL)
            {
                elseif_conditions = lit_allocate_expressions(parser->state);
                elseif_branches = lit_allocate_statements(parser->state);
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
                e = (LitExpression*)lit_create_unary_expression(parser->state, condition->line, e, LITTOK_BANG);
            }
            elseif_conditions->push(e);
            elseif_branches->push(parse_statement(parser));
            continue;
        }
        // else
        if(else_branch != NULL)
        {
            prs_error(parser, LITERROR_MULTIPLE_ELSE_BRANCHES);
        }
        ignore_new_lines(parser);
        else_branch = parse_statement(parser);
    }
    return (LitStatement*)lit_create_if_statement(parser->state, line, condition, if_branch, else_branch, elseif_conditions, elseif_branches);
}

static LitStatement* parse_for(LitParser* parser)
{

    bool c_style;
    bool had_paren;
    size_t line;
    LitExpression* condition;
    LitExpression* increment;
    LitStatement* var;
    LitExpression* init;
    line= parser->previous.line;
    had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
    var = NULL;
    init = NULL;
    if(!check(parser, LITTOK_SEMICOLON))
    {
        if(prs_match(parser, LITTOK_VAR))
        {
            var = parse_var_declaration(parser);
        }
        else
        {
            init = parse_expression(parser);
        }
    }
    c_style = !prs_match(parser, LITTOK_IN);
    condition= NULL;
    increment = NULL;
    if(c_style)
    {
        consume(parser, LITTOK_SEMICOLON, "';'");
        condition = check(parser, LITTOK_SEMICOLON) ? NULL : parse_expression(parser);
        consume(parser, LITTOK_SEMICOLON, "';'");
        increment = check(parser, LITTOK_RIGHT_PAREN) ? NULL : parse_expression(parser);
    }
    else
    {
        condition = parse_expression(parser);
        if(var == NULL)
        {
            prs_error(parser, LITERROR_VAR_MISSING_IN_FORIN);
        }
    }
    if(had_paren)
    {
        consume(parser, LITTOK_RIGHT_PAREN, "')'");
    }
    ignore_new_lines(parser);
    return (LitStatement*)lit_create_for_statement(parser->state, line, init, var, condition, increment,
                                                   parse_statement(parser), c_style);
}

static LitStatement* parse_while(LitParser* parser)
{
    bool had_paren;
    size_t line;
    LitStatement* body;
    line = parser->previous.line;
    had_paren = prs_match(parser, LITTOK_LEFT_PAREN);
    LitExpression* condition = parse_expression(parser);
    if(had_paren)
    {
        consume(parser, LITTOK_RIGHT_PAREN, "')'");
    }
    ignore_new_lines(parser);
    body = parse_statement(parser);
    return (LitStatement*)lit_create_while_statement(parser->state, line, condition, body);
}

static LitStatement* parse_function(LitParser* parser)
{
    size_t line;
    size_t function_length;
    bool isexport;
    const char* function_name;
    LitCompiler compiler;
    LitFunctionStatement* function;
    LitLambdaExpression* lambda;
    LitSetExpression* to;
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
        parser->state, line, (LitExpression*)lit_create_var_expression(parser->state, line, function_name, function_length),
        parser->previous.start, parser->previous.length, (LitExpression*)lambda);
        consume(parser, LITTOK_LEFT_PAREN, "'(' after function name");
        prs_init_compiler(parser, &compiler);
        prs_begin_scope(parser);
        parse_parameters(parser, &lambda->parameters);
        if(lambda->parameters.m_count > 255)
        {
            prs_error(parser, LITERROR_TOO_MANY_FUNCTION_ARGS, (int)lambda->parameters.m_count);
        }
        consume(parser, LITTOK_RIGHT_PAREN, "')' after function arguments");
        ignore_new_lines(parser);
        lambda->body = parse_statement(parser);
        prs_end_scope(parser);
        prs_end_compiler(parser, &compiler);
        return (LitStatement*)lit_create_expression_statement(parser->state, line, (LitExpression*)to);
    }
    function = lit_create_function_statement(parser->state, line, function_name, function_length);
    function->exported = isexport;
    consume(parser, LITTOK_LEFT_PAREN, "'(' after function name");
    prs_init_compiler(parser, &compiler);
    prs_begin_scope(parser);
    parse_parameters(parser, &function->parameters);
    if(function->parameters.m_count > 255)
    {
        prs_error(parser, LITERROR_TOO_MANY_FUNCTION_ARGS, (int)function->parameters.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after function arguments");
    function->body = parse_statement(parser);
    prs_end_scope(parser);
    prs_end_compiler(parser, &compiler);
    return (LitStatement*)function;
}

static LitStatement* parse_return(LitParser* parser)
{
    size_t line;
    LitExpression* expression;
    line = parser->previous.line;
    expression = NULL;
    if(!check(parser, LITTOK_NEW_LINE) && !check(parser, LITTOK_RIGHT_BRACE))
    {
        expression = parse_expression(parser);
    }
    return (LitStatement*)lit_create_return_statement(parser->state, line, expression);
}

static LitStatement* parse_field(LitParser* parser, LitString* name, bool is_static)
{
    size_t line;
    LitStatement* getter;
    LitStatement* setter;
    line = parser->previous.line;
    getter = NULL;
    setter = NULL;
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
        if(getter == NULL && setter == NULL)
        {
            prs_error(parser, LITERROR_NO_GETTER_AND_SETTER);
        }
        ignore_new_lines(parser);
        consume(parser, LITTOK_RIGHT_BRACE, "'}' after field declaration");
    }
    return (LitStatement*)lit_create_field_statement(parser->state, line, name, getter, setter, is_static);
}

static LitStatement* parse_method(LitParser* parser, bool is_static)
{
    size_t i;
    LitCompiler compiler;
    LitMethodStatement* method;
    LitString* name;
    if(prs_match(parser, LITTOK_STATIC))
    {
        is_static = true;
    }
    name = NULL;
    consume(parser, LITTOK_FUNCTION, "expected 'function'");
    if(prs_match(parser, LITTOK_OPERATOR))
    {
        if(is_static)
        {
            prs_error(parser, LITERROR_STATIC_OPERATOR);
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
            name = lit_string_copy(parser->state, "[]", 2);
        }
        else
        {
            name = lit_string_copy(parser->state, parser->previous.start, parser->previous.length);
        }
    }
    else
    {
        consume(parser, LITTOK_IDENTIFIER, "method name");
        name = lit_string_copy(parser->state, parser->previous.start, parser->previous.length);
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
        prs_error(parser, LITERROR_TOO_MANY_FUNCTION_ARGS, (int)method->parameters.m_count);
    }
    consume(parser, LITTOK_RIGHT_PAREN, "')' after method arguments");
    method->body = parse_statement(parser);
    prs_end_scope(parser);
    prs_end_compiler(parser, &compiler);
    return (LitStatement*)method;
}

static LitStatement* parse_class(LitParser* parser)
{
    bool finished_parsing_fields;
    bool field_is_static;
    size_t line;
    bool is_static;
    LitString* name;
    LitString* super;
    LitClassStatement* klass;
    LitStatement* var;
    LitStatement* method;
    if(setjmp(prs_jmpbuffer))
    {
        return NULL;
    }
    line = parser->previous.line;
    is_static = parser->previous.type == LITTOK_STATIC;
    if(is_static)
    {
        consume(parser, LITTOK_CLASS, "'class' after 'static'");
    }
    consume(parser, LITTOK_IDENTIFIER, "class name after 'class'");
    name = lit_string_copy(parser->state, parser->previous.start, parser->previous.length);
    super = NULL;
    if(prs_match(parser, LITTOK_COLON))
    {
        consume(parser, LITTOK_IDENTIFIER, "super class name after ':'");
        super = lit_string_copy(parser->state, parser->previous.start, parser->previous.length);
        if(super == name)
        {
            prs_error(parser, LITERROR_SELF_INHERITED_CLASS);
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
                    prs_error(parser, LITERROR_STATIC_FIELDS_AFTER_METHODS);
                }
                var = parse_var_declaration(parser);
                if(var != NULL)
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
        if(method != NULL)
        {
            klass->fields.push(method);
        }
        ignore_new_lines(parser);
    }
    consume(parser, LITTOK_RIGHT_BRACE, "'}' after class body");
    return (LitStatement*)klass;
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

static LitStatement* parse_statement(LitParser* parser)
{
    LitExpression* expression;
    ignore_new_lines(parser);
    if(setjmp(prs_jmpbuffer))
    {
        return NULL;
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
        return (LitStatement*)lit_create_continue_statement(parser->state, parser->previous.line);
    }
    else if(prs_match(parser, LITTOK_BREAK))
    {
        return (LitStatement*)lit_create_break_statement(parser->state, parser->previous.line);
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
    return expression == NULL ? NULL : (LitStatement*)lit_create_expression_statement(parser->state, parser->previous.line, expression);
}

static LitStatement* parse_declaration(LitParser* parser)
{
    LitStatement* statement;
    statement = NULL;
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

bool lit_parse(LitParser* parser, const char* file_name, const char* source, PCGenericArray<LitStatement*>& statements)
{
    LitCompiler compiler;
    LitStatement* statement;
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
            if(statement != NULL)
            {
                fprintf(stderr, "lit_parse:statement=%p\n", statement);
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


void lit_init_preprocessor(LitState* state, LitPreprocessor* preprocessor)
{
    preprocessor->state = state;
    preprocessor->defined.init(state);
    preprocessor->open_ifs.init(state);
}

void lit_free_preprocessor(LitPreprocessor* preprocessor)
{
    preprocessor->defined.release(preprocessor->state);
    preprocessor->open_ifs.release(preprocessor->state);
}

void lit_add_definition(LitState* state, const char* name)
{
    lit_table_set(state, &state->preprocessor->defined, CONST_STRING(state, name), TRUE_VALUE);
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
                    arg = lit_string_copy(state, arg_start, (int)(current - arg_start) - 1);
                    if(memcmp(macro_start, "define", 6) == 0 || memcmp(macro_start, "undef", 5) == 0)
                    {
                        if(ignore_depth < 0)
                        {
                            close = macro_start[0] == 'u';
                            if(close)
                            {
                                lit_table_delete(&preprocessor->defined, arg);
                            }
                            else
                            {
                                lit_table_set(state, &preprocessor->defined, arg, TRUE_VALUE);
                            }
                        }
                    }
                    else
                    {// ifdef || ifndef
                        depth++;
                        if(ignore_depth < 0)
                        {
                            close = macro_start[2] == 'n';
                            if((lit_table_get(&preprocessor->defined, arg, &tmp) ^ close) == false)
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
                        lit_error(preprocessor->state, (LitErrorType)0,
                                  lit_format_error(preprocessor->state, 0, LITERROR_UNKNOWN_MACRO, (int)(current - macro_start) - 1, macro_start)
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
        lit_error(preprocessor->state, (LitErrorType)0, lit_format_error(preprocessor->state, 0, LITERROR_UNCLOSED_MACRO)->chars);
        return false;
    }
    preprocessor->open_ifs.release(preprocessor->state);
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
    token.length = lit_string_length(result);
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
                return make_error_token(scanner, LITERROR_INTERPOLATION_NESTING_TOO_DEEP, LIT_MAX_INTERPOLATION_NESTING);
            }
            string_type = LITTOK_INTERPOLATION;
            scanner->braces[scanner->num_braces++] = 1;
            break;
        }
        switch(c)
        {
            case '\0':
                {
                    return make_error_token(scanner, LITERROR_UNTERMINATED_STRING);
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
                                bytes.push('\e');
                            }
                            break;
                        default:
                            {
                                return make_error_token(scanner, LITERROR_INVALID_ESCAPE_CHAR, scanner->current[-1]);

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
                                    return make_error_token(scanner, LITERROR_INVALID_ESCAPE_CHAR, scanner->current[-1]);
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
    token.value = OBJECT_VALUE(lit_string_copy(state, (const char*)bytes.m_values, bytes.m_count));
    bytes.release(state);
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
        value = lit_number_to_value((double)strtoll(scanner->start, NULL, 16));
    }
    else if(is_binary)
    {
        value = lit_number_to_value((int)strtoll(scanner->start + 2, NULL, 2));
    }
    else
    {
        value = lit_number_to_value(strtod(scanner->start, NULL));
    }

    if(errno == ERANGE)
    {
        errno = 0;
        return make_error_token(scanner, LITERROR_NUMBER_IS_TOO_BIG);
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
                return make_error_token(scanner, LITERROR_CHAR_EXPECTATION_UNMET, '\"', '$', peek(scanner));
            }

            return scan_string(scanner, true);
        }

        case '"':
            return scan_string(scanner, false);
    }

    return make_error_token(scanner, LITERROR_UNEXPECTED_CHAR, c);
}

#define PUSH(value) (*fiber->stack_top++ = value)

#define RETURN_OK(r) return (LitInterpretResult){ LITRESULT_OK, r };

#define RETURN_RUNTIME_ERROR() return (LitInterpretResult){ LITRESULT_RUNTIME_ERROR, NULL_VALUE };



void lit_init_api(LitState* state)
{
    state->api_name = lit_string_copy(state, "c", 1);
    state->api_function = NULL;
    state->api_fiber = NULL;
}

void lit_free_api(LitState* state)
{
    state->api_name = NULL;
    state->api_function = NULL;
    state->api_fiber = NULL;
}

LitValue lit_get_global(LitState* state, LitString* name)
{
    LitValue global;
    if(!lit_table_get(&state->vm->globals->values, name, &global))
    {
        return NULL_VALUE;
    }
    return global;
}

LitFunction* lit_get_global_function(LitState* state, LitString* name)
{
    LitValue function = lit_get_global(state, name);
    if(IS_FUNCTION(function))
    {
        return AS_FUNCTION(function);
    }
    return NULL;
}

void lit_set_global(LitState* state, LitString* name, LitValue value)
{
    lit_push_root(state, (LitObject*)name);
    lit_push_value_root(state, value);
    lit_table_set(state, &state->vm->globals->values, name, value);
    lit_pop_roots(state, 2);
}

bool lit_global_exists(LitState* state, LitString* name)
{
    LitValue global;
    return lit_table_get(&state->vm->globals->values, name, &global);
}

void lit_define_native(LitState* state, const char* name, LitNativeFunctionFn native)
{
    lit_push_root(state, (LitObject*)CONST_STRING(state, name));
    lit_push_root(state, (LitObject*)lit_create_native_function(state, native, lit_as_string(lit_peek_root(state, 0))));
    lit_table_set(state, &state->vm->globals->values, lit_as_string(lit_peek_root(state, 1)), lit_peek_root(state, 0));
    lit_pop_roots(state, 2);
}

void lit_define_native_primitive(LitState* state, const char* name, LitNativePrimitiveFn native)
{
    lit_push_root(state, (LitObject*)CONST_STRING(state, name));
    lit_push_root(state, (LitObject*)lit_create_native_primitive(state, native, lit_as_string(lit_peek_root(state, 0))));
    lit_table_set(state, &state->vm->globals->values, lit_as_string(lit_peek_root(state, 1)), lit_peek_root(state, 0));
    lit_pop_roots(state, 2);
}

LitValue lit_instance_get_method(LitState* state, LitValue callee, LitString* mthname)
{
    LitValue mthval;
    LitClass* klass;
    klass = lit_get_class_for(state, callee);
    if((IS_INSTANCE(callee) && lit_table_get(&AS_INSTANCE(callee)->fields, mthname, &mthval)) || lit_table_get(&klass->methods, mthname, &mthval))
    {
        return mthval;
    }
    return NULL_VALUE;
}

LitInterpretResult lit_instance_call_method(LitState* state, LitValue callee, LitString* mthname, LitValue* argv, size_t argc)
{
    LitValue mthval;
    mthval = lit_instance_get_method(state, callee, mthname);
    if(!IS_NULL(mthval))
    {
        return lit_call(state, mthval, argv, argc, false);
    }
    return INTERPRET_RUNTIME_FAIL;    
}


double lit_check_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_NUMBER(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a number as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }
    return lit_value_to_number(args[id]);
}

double lit_get_number(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, double def)
{
    (void)vm;
    if(arg_count <= id || !IS_NUMBER(args[id]))
    {
        return def;
    }
    return lit_value_to_number(args[id]);
}

bool lit_check_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_BOOL(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a boolean as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return lit_as_bool(args[id]);
}

bool lit_get_bool(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, bool def)
{
    (void)vm;
    if(arg_count <= id || !IS_BOOL(args[id]))
    {
        return def;
    }
    return lit_as_bool(args[id]);
}

const char* lit_check_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_STRING(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return lit_as_string(args[id])->chars;
}

const char* lit_get_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id, const char* def)
{
    (void)vm;
    if(arg_count <= id || !IS_STRING(args[id]))
    {
        return def;
    }

    return lit_as_string(args[id])->chars;
}

LitString* lit_check_object_string(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_STRING(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return lit_as_string(args[id]);
}

LitInstance* lit_check_instance(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_INSTANCE(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected an instance as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return AS_INSTANCE(args[id]);
}

LitValue* lit_check_reference(LitVM* vm, LitValue* args, uint8_t arg_count, uint8_t id)
{
    if(arg_count <= id || !IS_REFERENCE(args[id]))
    {
        lit_runtime_error_exiting(vm, "expected a reference as argument #%i, got a %s", (int)id,
                                  id >= arg_count ? "null" : lit_get_value_type(args[id]));
    }

    return AS_REFERENCE(args[id])->slot;
}

void lit_ensure_bool(LitVM* vm, LitValue value, const char* error)
{
    if(!IS_BOOL(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_string(LitVM* vm, LitValue value, const char* error)
{
    if(!IS_STRING(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_number(LitVM* vm, LitValue value, const char* error)
{
    if(!IS_NUMBER(value))
    {
        lit_runtime_error_exiting(vm, error);
    }
}

void lit_ensure_object_type(LitVM* vm, LitValue value, LitObjectType type, const char* error)
{
    if(!IS_OBJECT(value) || OBJECT_TYPE(value) != type)
    {
        lit_runtime_error_exiting(vm, error);
    }
}

LitValue lit_get_field(LitState* state, LitTable* table, const char* name)
{
    LitValue value;

    if(!lit_table_get(table, CONST_STRING(state, name), &value))
    {
        value = NULL_VALUE;
    }

    return value;
}

LitValue lit_get_map_field(LitState* state, LitMap* map, const char* name)
{
    LitValue value;

    if(!lit_table_get(&map->values, CONST_STRING(state, name), &value))
    {
        value = NULL_VALUE;
    }

    return value;
}

void lit_set_field(LitState* state, LitTable* table, const char* name, LitValue value)
{
    lit_table_set(state, table, CONST_STRING(state, name), value);
}

void lit_set_map_field(LitState* state, LitMap* map, const char* name, LitValue value)
{
    lit_table_set(state, &map->values, CONST_STRING(state, name), value);
}


static bool ensure_fiber(LitVM* vm, LitFiber* fiber)
{
    size_t newcapacity;
    size_t osize;
    size_t newsize;
    if(fiber == NULL)
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
    if(callee == NULL)
    {
        lit_runtime_error(vm, "attempt to call a null value");
        return NULL;
    }
    if(ignfiber)
    {
        if(fiber == NULL)
        {
            fiber = state->api_fiber;
        }
    }
    if(!ignfiber)
    {
        if(ensure_fiber(vm, fiber))
        {
            return NULL;
        }        
    }
    lit_ensure_fiber_stack(state, fiber, callee->max_slots + (int)(fiber->stack_top - fiber->stack));
    frame = &fiber->frames[fiber->frame_count++];
    frame->slots = fiber->stack_top;
    PUSH(OBJECT_VALUE(callee));
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
                PUSH(NULL_VALUE);
            }
            if(vararg)
            {
                PUSH(OBJECT_VALUE(lit_create_array(vm->state)));
            }
        }
        else if(callee->vararg)
        {
            array = lit_create_array(vm->state);
            varargc = argc - function_arg_count + 1;
            array->values.reserve(varargc + 1, NULL_VALUE);
            for(i = 0; i < varargc; i++)
            {
                array->values.m_values[i] = fiber->stack_top[(int)i - (int)varargc];
            }

            fiber->stack_top -= varargc;
            lit_push(vm, OBJECT_VALUE(array));
        }
        else
        {
            fiber->stack_top -= (argc - function_arg_count);
        }
    }
    else if(callee->vararg)
    {
        array = lit_create_array(vm->state);
        varargc = argc - function_arg_count + 1;
        array->values.push(*(fiber->stack_top - 1));
        *(fiber->stack_top - 1) = OBJECT_VALUE(array);
    }
    frame->ip = callee->chunk.code;
    frame->closure = NULL;
    frame->function = callee;
    frame->result_ignored = false;
    frame->return_to_c = true;
    return frame;
}

static inline LitInterpretResult execute_call(LitState* state, LitCallFrame* frame)
{
    LitFiber* fiber;
    LitInterpretResult result;
    if(frame == NULL)
    {
        RETURN_RUNTIME_ERROR();
    }
    fiber = state->vm->fiber;
    result = lit_interpret_fiber(state, fiber);
    if(fiber->error != NULL_VALUE)
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
    if(frame == NULL)
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
    LitObjectType type;
    LitClass* klass;
    LitFiber* fiber;
    LitValue* slot;
    LitNativeMethod* natmethod;
    LitBoundMethod* bound_method;
    LitValue mthval;
    LitValue result;
    lir.result = NULL_VALUE;
    lir.type = LITRESULT_OK;
    vm = state->vm;
    if(IS_OBJECT(callee))
    {
        if(lit_set_native_exit_jump())
        {
            RETURN_RUNTIME_ERROR();
        }
        type = OBJECT_TYPE(callee);

        if(type == LITTYPE_FUNCTION)
        {
            return lit_call_function(state, AS_FUNCTION(callee), argv, argc, ignfiber);
        }
        else if(type == LITTYPE_CLOSURE)
        {
            return lit_call_closure(state, AS_CLOSURE(callee), argv, argc, ignfiber);
        }
        fiber = vm->fiber;
        if(ignfiber)
        {
            if(fiber == NULL)
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
        if(type != LITTYPE_CLASS)
        {
            for(i = 0; i < argc; i++)
            {
                PUSH(argv[i]);
            }
        }
        switch(type)
        {
            case LITTYPE_NATIVE_FUNCTION:
                {
                    LitValue result = AS_NATIVE_FUNCTION(callee)->function(vm, argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(result);
                }
                break;
            case LITTYPE_NATIVE_PRIMITIVE:
                {
                    AS_NATIVE_PRIMITIVE(callee)->function(vm, argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(NULL_VALUE);
                }
                break;
            case LITTYPE_NATIVE_METHOD:
                {
                    natmethod = AS_NATIVE_METHOD(callee);
                    result = natmethod->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(result);
                }
                break;
            case LITTYPE_CLASS:
                {
                    klass = AS_CLASS(callee);
                    *slot = OBJECT_VALUE(lit_create_instance(vm->state, klass));
                    if(klass->init_method != NULL)
                    {
                        lir = lit_call_method(state, *slot, OBJECT_VALUE(klass->init_method), argv, argc, ignfiber);
                    }
                    // TODO: when should this return *slot instead of lir?
                    fiber->stack_top = slot;
                    //RETURN_OK(*slot);
                    return lir;
                }
                break;
            case LITTYPE_BOUND_METHOD:
                {
                    bound_method = AS_BOUND_METHOD(callee);
                    mthval = bound_method->method;
                    *slot = bound_method->receiver;
                    if(IS_NATIVE_METHOD(mthval))
                    {
                        result = AS_NATIVE_METHOD(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(result);
                    }
                    else if(IS_PRIMITIVE_METHOD(mthval))
                    {
                        AS_PRIMITIVE_METHOD(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);

                        fiber->stack_top = slot;
                        RETURN_OK(NULL_VALUE);
                    }
                    else
                    {
                        fiber->stack_top = slot;
                        return lit_call_function(state, AS_FUNCTION(mthval), argv, argc, ignfiber);
                    }
                }
                break;
            case LITTYPE_PRIMITIVE_METHOD:
                {
                    AS_PRIMITIVE_METHOD(callee)->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                    fiber->stack_top = slot;
                    RETURN_OK(NULL_VALUE);
                }
                break;
            default:
                {
                }
                break;
        }
    }
    if(IS_NULL(callee))
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
    if(fiber == NULL)
    {
        if(!ignfiber)
        {
            lit_runtime_error(vm, "no fiber to run on");
            RETURN_RUNTIME_ERROR();
        }
    }
    klass = lit_get_class_for(state, callee);
    if((IS_INSTANCE(callee) && lit_table_get(&AS_INSTANCE(callee)->fields, method_name, &mthval)) || lit_table_get(&klass->methods, method_name, &mthval))
    {
        return lit_call_method(state, callee, mthval, argv, argc, ignfiber);
    }
    return (LitInterpretResult){ LITRESULT_INVALID, NULL_VALUE };
}

LitString* lit_to_string(LitState* state, LitValue object)
{
    LitValue* slot;
    LitVM* vm;
    LitFiber* fiber;
    LitFunction* function;
    LitChunk* chunk;
    LitCallFrame* frame;
    LitInterpretResult result;
    if(IS_STRING(object))
    {
        return lit_as_string(object);
    }
    else if(!IS_OBJECT(object))
    {
        if(IS_NULL(object))
        {
            return CONST_STRING(state, "null");
        }
        else if(IS_NUMBER(object))
        {
            return lit_as_string(lit_string_number_to_string(state, lit_value_to_number(object)));
        }
        else if(IS_BOOL(object))
        {
            return CONST_STRING(state, lit_as_bool(object) ? "true" : "false");
        }
    }
    else if(IS_REFERENCE(object))
    {
        slot = AS_REFERENCE(object)->slot;

        if(slot == NULL)
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
    if(function == NULL)
    {
        function = state->api_function = lit_create_function(state, fiber->module);
        function->chunk.has_line_info = false;
        function->name = state->api_name;
        chunk = &function->chunk;
        chunk->m_count = 0;
        chunk->constants.m_count = 0;
        function->max_slots = 3;
        lit_write_chunk(state, chunk, OP_INVOKE, 1);
        lit_emit_byte(state, chunk, 0);
        lit_emit_short(state, chunk, lit_chunk_add_constant(state, chunk, OBJECT_CONST_STRING(state, "toString")));
        lit_emit_byte(state, chunk, OP_RETURN);
    }
    lit_ensure_fiber_stack(state, fiber, function->max_slots + (int)(fiber->stack_top - fiber->stack));
    frame = &fiber->frames[fiber->frame_count++];
    frame->ip = function->chunk.code;
    frame->closure = NULL;
    frame->function = function;
    frame->slots = fiber->stack_top;
    frame->result_ignored = false;
    frame->return_to_c = true;
    PUSH(OBJECT_VALUE(function));
    PUSH(object);
    result = lit_interpret_fiber(state, fiber);
    if(result.type != LITRESULT_OK)
    {
        return CONST_STRING(state, "null");
    }
    return lit_as_string(result.result);
}

LitValue lit_call_new(LitVM* vm, const char* name, LitValue* args, size_t argc, bool ignfiber)
{
    LitValue value;
    LitClass* klass;
    if(!lit_table_get(&vm->globals->values, CONST_STRING(vm->state, name), &value))
    {
        lit_runtime_error(vm, "failed to create instance of class %s: class not found", name);
        return NULL_VALUE;
    }
    klass = AS_CLASS(value);
    if(klass->init_method == NULL)
    {
        return OBJECT_VALUE(lit_create_instance(vm->state, klass));
    }
    return lit_call_method(vm->state, value, value, args, argc, ignfiber).result;
}


void lit_init_chunk(LitState* state, LitChunk* chunk)
{
    chunk->state = state;
    chunk->m_count = 0;
    chunk->m_capacity = 0;
    chunk->code = NULL;

    chunk->has_line_info = true;
    chunk->line_count = 0;
    chunk->line_capacity = 0;
    chunk->lines = NULL;

    chunk->constants.init(chunk->state);
}

void lit_free_chunk(LitState* state, LitChunk* chunk)
{
    LIT_FREE_ARRAY(state, uint8_t, chunk->code, chunk->m_capacity);
    LIT_FREE_ARRAY(state, uint16_t, chunk->lines, chunk->line_capacity);

    chunk->constants.release(state);
    lit_init_chunk(state, chunk);
}

void lit_write_chunk(LitState* state, LitChunk* chunk, uint8_t byte, uint16_t line)
{
    if(chunk->m_capacity < chunk->m_count + 1)
    {
        size_t old_capacity = chunk->m_capacity;

        chunk->m_capacity = LIT_GROW_CAPACITY(old_capacity + 2);
        chunk->code = LIT_GROW_ARRAY(state, chunk->code, uint8_t, old_capacity, chunk->m_capacity + 2);
    }

    chunk->code[chunk->m_count] = byte;
    chunk->m_count++;

    if(!chunk->has_line_info)
    {
        return;
    }

    if(chunk->line_capacity < chunk->line_count + 2)
    {
        size_t old_capacity = chunk->line_capacity;

        chunk->line_capacity = LIT_GROW_CAPACITY(chunk->line_capacity + 2);
        chunk->lines = LIT_GROW_ARRAY(state, chunk->lines, uint16_t, old_capacity, chunk->line_capacity + 2);

        if(old_capacity == 0)
        {
            chunk->lines[0] = 0;
            chunk->lines[1] = 0;
        }
    }

    size_t line_index = chunk->line_count;
    size_t value = chunk->lines[line_index];

    if(value != 0 && value != line)
    {
        chunk->line_count += 2;
        line_index = chunk->line_count;

        chunk->lines[line_index + 1] = 0;
    }

    chunk->lines[line_index] = line;
    chunk->lines[line_index + 1]++;
}

size_t lit_chunk_add_constant(LitState* state, LitChunk* chunk, LitValue constant)
{
    for(size_t i = 0; i < chunk->constants.m_count; i++)
    {
        if(chunk->constants.m_values[i] == constant)
        {
            return i;
        }
    }

    lit_push_value_root(state, constant);
    chunk->constants.push(constant);
    lit_pop_root(state);

    return chunk->constants.m_count - 1;
}

size_t lit_chunk_get_line(LitChunk* chunk, size_t offset)
{
    if(!chunk->has_line_info)
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

        line = chunk->lines[index];
        rle = chunk->lines[index + 1];

        if(rle > 0)
        {
            rle--;
        }

        index += 2;
    }

    return line;
}

void lit_shrink_chunk(LitState* state, LitChunk* chunk)
{
    if(chunk->m_capacity > chunk->m_count)
    {
        size_t old_capacity = chunk->m_capacity;

        chunk->m_capacity = chunk->m_count;
        chunk->code = LIT_GROW_ARRAY(state, chunk->code, uint8_t, old_capacity, chunk->m_capacity);
    }

    if(chunk->line_capacity > chunk->line_count)
    {
        size_t old_capacity = chunk->line_capacity;

        chunk->line_capacity = chunk->line_count + 2;
        chunk->lines = LIT_GROW_ARRAY(state, chunk->lines, uint16_t, old_capacity, chunk->line_capacity);
    }
}

void lit_emit_byte(LitState* state, LitChunk* chunk, uint8_t byte)
{
    lit_write_chunk(state, chunk, byte, 1);
}

void lit_emit_bytes(LitState* state, LitChunk* chunk, uint8_t a, uint8_t b)
{
    lit_write_chunk(state, chunk, a, 1);
    lit_write_chunk(state, chunk, b, 1);
}

void lit_emit_short(LitState* state, LitChunk* chunk, uint16_t value)
{
    lit_emit_bytes(state, chunk, (uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
}

void lit_disassemble_module(LitState* state, LitModule* module, const char* source)
{
    lit_disassemble_chunk(state, &module->main_function->chunk, module->main_function->name->chars, source);
}

void lit_disassemble_chunk(LitState* state, LitChunk* chunk, const char* name, const char* source)
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
        if(IS_FUNCTION(value))
        {
            function = AS_FUNCTION(value);
            lit_disassemble_chunk(state, &function->chunk, function->name->chars, source);
        }
    }
    lit_writer_writeformat(&state->debugwriter, "== %s ==\n", name);
    for(offset = 0; offset < chunk->m_count;)
    {
        offset = lit_disassemble_instruction(state, chunk, offset, source);
    }
}

static size_t print_simple_op(LitState* state, LitWriter* wr, const char* name, size_t offset)
{
    (void)state;
    lit_writer_writeformat(wr, "%s%s%s\n", COLOR_YELLOW, name, COLOR_RESET);
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
    lit_writer_writeformat(wr, "%s%-16s%s %4d '", COLOR_YELLOW, name, COLOR_RESET, constant);
    lit_print_value(state, wr, chunk->constants.m_values[constant]);
    lit_writer_writeformat(wr, "'\n");
    return offset + (big ? 3 : 2);
}

static size_t print_byte_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset)
{
    uint8_t slot;
    (void)state;
    slot = chunk->code[offset + 1];
    lit_writer_writeformat(wr, "%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
    return offset + 2;
}

static size_t print_short_op(LitState* state, LitWriter* wr, const char* name, LitChunk* chunk, size_t offset)
{
    uint16_t slot;
    (void)state;
    slot = (uint16_t)(chunk->code[offset + 1] << 8);
    slot |= chunk->code[offset + 2];
    lit_writer_writeformat(wr, "%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
    return offset + 2;
}

static size_t print_jump_op(LitState* state, LitWriter* wr, const char* name, int sign, LitChunk* chunk, size_t offset)
{
    uint16_t jump;
    (void)state;
    jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    lit_writer_writeformat(wr, "%s%-16s%s %4d -> %d\n", COLOR_YELLOW, name, COLOR_RESET, (int)offset, (int)(offset + 3 + sign * jump));
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
    lit_writer_writeformat(wr, "%s%-16s%s (%d args) %4d '", COLOR_YELLOW, name, COLOR_RESET, arg_count, constant);
    lit_print_value(state, wr, chunk->constants.m_values[constant]);
    lit_writer_writeformat(wr, "'\n");
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
    line = lit_chunk_get_line(chunk, offset);
    same = !chunk->has_line_info || (offset > 0 && line == lit_chunk_get_line(chunk, offset - 1));
    if(!same && source != NULL)
    {
        index = 0;
        current_line = (char*)source;
        while(current_line)
        {
            next_line = strchr(current_line, '\n');
            prev_line = current_line;
            index++;
            current_line = next_line ? (next_line + 1) : NULL;
            if(index == line)
            {
                output_line = prev_line ? prev_line : next_line;
                while((c = *output_line) && (c == '\t' || c == ' '))
                {
                    output_line++;
                }
                lit_writer_writeformat(wr, "%s        %.*s%s\n", COLOR_RED, next_line ? (int)(next_line - output_line) : (int)strlen(prev_line), output_line, COLOR_RESET);
                break;
            }
        }
    }
    lit_writer_writeformat(wr, "%04d ", (int)offset);
    if(same)
    {
        lit_writer_writestring(wr, "   | ");
    }
    else
    {
        lit_writer_writeformat(wr, "%s%4d%s ", COLOR_BLUE, (int)line, COLOR_RESET);
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
                lit_writer_writeformat(wr, "%-16s %4d ", "OP_CLOSURE", constant);
                lit_print_value(state, wr, chunk->constants.m_values[constant]);
                lit_writer_writeformat(wr, "\n");
                function = AS_FUNCTION(chunk->constants.m_values[constant]);
                for(j = 0; j < function->upvalue_count; j++)
                {
                    is_local = chunk->code[offset++];
                    index = chunk->code[offset++];
                    lit_writer_writeformat(wr, "%04d      |                     %s %d\n", (int)(offset - 2), is_local ? "local" : "upvalue", (int)index);
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
                lit_writer_writeformat(wr, "Unknown opcode %d\n", instruction);
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
    if(fiber == NULL)
    {
        return;
    }
    frame = &fiber->frames[fiber->frame_count - 1];
    lit_writer_writeformat(wr, "== fiber %p f%i %s (expects %i, max %i, added %i, current %i, exits %i) ==\n", fiber,
           fiber->frame_count - 1, frame->function->name->chars, frame->function->arg_count, frame->function->max_slots,
           frame->function->max_slots + (int)(fiber->stack_top - fiber->stack), fiber->stack_capacity, frame->return_to_c);
#endif
}

static const char* error_messages[LITERROR_TOTAL] =
{
   // LITERROR_UNCLOSED_MACRO
   "unclosed macro.",

   // LITERROR_UNKNOWN_MACRO
   "unknown macro '%.*s'.",

   // LITERROR_UNEXPECTED_CHAR
   "unexpected character '%c'",

   // LITERROR_UNTERMINATED_STRING
   "unterminated string",

   // LITERROR_INVALID_ESCAPE_CHAR
   "invalid escape character '%c'",

   // LITERROR_INTERPOLATION_NESTING_TOO_DEEP
   "interpolation nesting is too deep, maximum is %i",

   // LITERROR_NUMBER_IS_TOO_BIG
   "number is too big to be represented by a single literal",

   // LITERROR_CHAR_EXPECTATION_UNMET
   "expected '%c' after '%c', got '%c'",

   // LITERROR_EXPECTATION_UNMET
   "expected %s, got '%.*s'",

   // LITERROR_INVALID_ASSIGMENT_TARGET
   "invalid assigment target",

   // LITERROR_TOO_MANY_FUNCTION_ARGS
   "function cannot have more than 255 arguments, got %i",

   // LITERROR_MULTIPLE_ELSE_BRANCHES
   "if-statement can have only one else-branch",

   // LITERROR_VAR_MISSING_IN_FORIN
   "for-loops using in-iteration must declare a new variable",

   // LITERROR_NO_GETTER_AND_SETTER
   "expected declaration of either getter or setter, got none",

   // LITERROR_STATIC_OPERATOR
   "operator methods cannot be static or defined in static classes",

   // LITERROR_SELF_INHERITED_CLASS
   "class cannot inherit itself",

   // LITERROR_STATIC_FIELDS_AFTER_METHODS
   "all static fields must be defined before the methods",

   // LITERROR_MISSING_STATEMENT
   "expected statement but got nothing",

   // LITERROR_EXPECTED_EXPRESSION
   "expected expression after '%.*s', got '%.*s'",

   // LITERROR_DEFAULT_ARG_CENTRED
   "default arguments must always be in the end of the argument list.",

   // LITERROR_TOO_MANY_CONSTANTS
   "too many constants for one chunk",

   // LITERROR_TOO_MANY_PRIVATES
   "too many private locals for one module",

   // LITERROR_VAR_REDEFINED
   "variable '%.*s' was already declared in this scope",

   // LITERROR_TOO_MANY_LOCALS
   "too many local variables for one function",

   // LITERROR_TOO_MANY_UPVALUES
   "too many upvalues for one function",

   // LITERROR_VARIABLE_USED_IN_INIT
   "variable '%.*s' cannot use itself in its initializer",

   // LITERROR_JUMP_TOO_BIG
   "too much code to jump over",

   // LITERROR_NO_SUPER
   "'super' cannot be used in class '%s', because it does not have a super class",

   // LITERROR_THIS_MISSUSE
   "'this' cannot be used %s",

   // LITERROR_SUPER_MISSUSE
   "'super' cannot be used %s",

   // LITERROR_UNKNOWN_EXPRESSION
   "unknown expression with id '%i'",

   // LITERROR_UNKNOWN_STATEMENT
   "unknown statement with id '%i'",

   // LITERROR_LOOP_JUMP_MISSUSE
   "cannot use '%s' outside of loops",

   // LITERROR_RETURN_FROM_CONSTRUCTOR
   "cannot use 'return' in constructors",

   // LITERROR_STATIC_CONSTRUCTOR
   "constructors cannot be static (at least for now)",

   // LITERROR_CONSTANT_MODIFIED
   "attempt to modify constant '%.*s'",

   // LITERROR_INVALID_REFERENCE_TARGET
   "invalid refence target",

};

LitString* lit_vformat_error(LitState* state, size_t line, LitError error, va_list args)
{
    int error_id;
    size_t buffer_size;
    char* buffer;
    const char* error_message;
    LitString* rt;
    va_list args_copy;
    error_id = (int)error;
    error_message = error_messages[error_id];
    va_copy(args_copy, args);
    buffer_size = vsnprintf(NULL, 0, error_message, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, error_message, args);
    buffer[buffer_size - 1] = '\0';
    if(line != 0)
    {
        rt = lit_as_string(lit_string_format(state, "[err # line #]: $", (double)error_id, (double)line, (const char*)buffer));
    }
    else
    {
        rt = lit_as_string(lit_string_format(state, "[err #]: $", (double)error_id, (const char*)buffer));
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
    if(file == NULL)
    {
        return NULL;
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
    c = lit_string_length(string);
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
        return NULL;
    }
    line = (char*)malloc(length + 1);
    for(i = 0; i < length; i++)
    {
        line[i] = (char)lit_read_uint8_t(file) ^ LIT_STRING_KEY;
    }
    return lit_string_take(state, line, length, false);
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
        return NULL;
    }
    line = (char*)malloc(length + 1);
    for(i = 0; i < length; i++)
    {
        line[i] = (char)lit_read_euint8_t(file) ^ LIT_STRING_KEY;
    }
    return lit_string_take(state, line, length, false);
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
    LitFunction* function = lit_create_function(state, module);

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

        if(IS_OBJECT(constant))
        {
            LitObjectType type = lit_as_object(constant)->type;
            lit_write_uint8_t(file, (uint8_t)(type + 1));

            switch(type)
            {
                case LITTYPE_STRING:
                {
                    lit_write_string(file, lit_as_string(constant));
                    break;
                }

                case LITTYPE_FUNCTION:
                {
                    save_function(file, AS_FUNCTION(constant));
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
            lit_write_double(file, lit_value_to_number(constant));
        }
    }
}

static void load_chunk(LitState* state, LitEmulatedFile* file, LitModule* module, LitChunk* chunk)
{
    size_t i;
    size_t count;
    uint8_t type;
    lit_init_chunk(state, chunk);
    count = lit_read_euint32_t(file);
    chunk->code = (uint8_t*)lit_reallocate(state, NULL, 0, sizeof(uint8_t) * count);
    chunk->m_count = count;
    chunk->m_capacity = count;
    for(i = 0; i < count; i++)
    {
        chunk->code[i] = lit_read_euint8_t(file);
    }
    count = lit_read_euint32_t(file);
    if(count > 0)
    {
        chunk->lines = (uint16_t*)lit_reallocate(state, NULL, 0, sizeof(uint16_t) * count);
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
    chunk->constants.m_values = (LitValue*)lit_reallocate(state, NULL, 0, sizeof(LitValue) * count);
    chunk->constants.m_count = count;
    chunk->constants.m_capacity = count;
    for(i = 0; i < count; i++)
    {
        type = lit_read_euint8_t(file);
        if(type == 0)
        {
            chunk->constants.m_values[i] = lit_number_to_value(lit_read_edouble(file));
        }
        else
        {
            switch((LitObjectType)(type - 1))
            {
                case LITTYPE_STRING:
                    {
                        chunk->constants.m_values[i] = OBJECT_VALUE(lit_read_estring(state, file));
                    }
                    break;
                case LITTYPE_FUNCTION:
                    {
                        chunk->constants.m_values[i] = OBJECT_VALUE(load_function(state, file, module));
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
            if(privates->m_values[i].key != NULL)
            {
                lit_write_string(file, privates->m_values[i].key);
                lit_write_uint16_t(file, (uint16_t)lit_value_to_number(privates->m_values[i].value));
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
        lit_error(state, COMPILE_ERROR, "Failed to read compiled code, unknown magic number");
        return NULL;
    }
    bytecode_version = lit_read_euint8_t(&file);
    if(bytecode_version > LIT_BYTECODE_VERSION)
    {
        lit_error(state, COMPILE_ERROR, "Failed to read compiled code, unknown bytecode version '%i'", (int)bytecode_version);
        return NULL;
    }
    module_count = lit_read_euint16_t(&file);
    LitModule* first = NULL;
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
            module->privates[i] = NULL_VALUE;
            if(enabled)
            {
                name = lit_read_estring(state, &file);
                lit_table_set(state, privates, name, lit_number_to_value(lit_read_euint16_t(&file)));
            }
        }
        module->main_function = load_function(state, &file, module);
        lit_table_set(state, &state->vm->modules->values, module->name, OBJECT_VALUE(module));
        if(j == 0)
        {
            first = module;
        }
    }
    if(lit_read_euint16_t(&file) != LIT_BYTECODE_END_NUMBER)
    {
        lit_error(state, COMPILE_ERROR, "Failed to read compiled code, unknown end number");
        return NULL;
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
        lit_collect_garbage(state->vm);
#endif
        if(state->bytes_allocated > state->next_gc)
        {
            lit_collect_garbage(state->vm);
        }
    }
    if(new_size == 0)
    {
        free(pointer);
        return NULL;
    }
    ptr = (void*)realloc(pointer, new_size);
    if(ptr == NULL)
    {
        lit_error(state, RUNTIME_ERROR, "Fatal error:\nOut of memory\nProgram terminated");
        exit(111);
    }
    return ptr;
}

void lit_free_object(LitState* state, LitObject* object)
{
    LitString* string;
    LitFunction* function;
    LitFiber* fiber;
    LitModule* module;
    LitClosure* closure;
#ifdef LIT_LOG_ALLOCATION
    printf("(");
    lit_print_value(OBJECT_VALUE(object));
    printf(") %p free %s\n", (void*)object, lit_get_value_type(object->type));
#endif

    switch(object->type)
    {
        case LITTYPE_STRING:
            {
                string = (LitString*)object;
                //LIT_FREE_ARRAY(state, char, string->chars, string->length + 1);
                sdsfree(string->chars);
                string->chars = NULL;
                LIT_FREE(state, LitString, object);
            }
            break;

        case LITTYPE_FUNCTION:
            {
                function = (LitFunction*)object;
                lit_free_chunk(state, &function->chunk);
                LIT_FREE(state, LitFunction, object);
            }
            break;
        case LITTYPE_NATIVE_FUNCTION:
            {
                LIT_FREE(state, LitNativeFunction, object);
            }
            break;
        case LITTYPE_NATIVE_PRIMITIVE:
            {
                LIT_FREE(state, LitNativePrimFunction, object);
            }
            break;
        case LITTYPE_NATIVE_METHOD:
            {
                LIT_FREE(state, LitNativeMethod, object);
            }
            break;
        case LITTYPE_PRIMITIVE_METHOD:
            {
                LIT_FREE(state, LitPrimitiveMethod, object);
            }
            break;
        case LITTYPE_FIBER:
            {
                fiber = (LitFiber*)object;
                LIT_FREE_ARRAY(state, LitCallFrame, fiber->frames, fiber->frame_capacity);
                LIT_FREE_ARRAY(state, LitValue, fiber->stack, fiber->stack_capacity);
                LIT_FREE(state, LitFiber, object);
            }
            break;
        case LITTYPE_MODULE:
            {
                module = (LitModule*)object;
                LIT_FREE_ARRAY(state, LitValue, module->privates, module->private_count);
                LIT_FREE(state, LitModule, object);
            }
            break;
        case LITTYPE_CLOSURE:
            {
                closure = (LitClosure*)object;
                LIT_FREE_ARRAY(state, LitUpvalue*, closure->upvalues, closure->upvalue_count);
                LIT_FREE(state, LitClosure, object);
            }
            break;
        case LITTYPE_UPVALUE:
            {
                LIT_FREE(state, LitUpvalue, object);
            }
            break;
        case LITTYPE_CLASS:
            {
                LitClass* klass = (LitClass*)object;
                klass->methods.release(state);
                klass->static_fields.release(state);
                LIT_FREE(state, LitClass, object);
            }
            break;

        case LITTYPE_INSTANCE:
            {
                ((LitInstance*)object)->fields.release(state);
                LIT_FREE(state, LitInstance, object);
            }
            break;
        case LITTYPE_BOUND_METHOD:
            {
                LIT_FREE(state, LitBoundMethod, object);
            }
            break;
        case LITTYPE_ARRAY:
            {
                ((LitArray*)object)->values.release(state);
                LIT_FREE(state, LitArray, object);
            }
            break;
        case LITTYPE_MAP:
            {
                ((LitMap*)object)->values.release(state);
                LIT_FREE(state, LitMap, object);
            }
            break;
        case LITTYPE_USERDATA:
            {
                LitUserdata* data = (LitUserdata*)object;
                if(data->cleanup_fn != NULL)
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
        case LITTYPE_RANGE:
            {
                LIT_FREE(state, LitRange, object);
            }
            break;
        case LITTYPE_FIELD:
            {
                LIT_FREE(state, LitField, object);
            }
            break;
        case LITTYPE_REFERENCE:
            {
                LIT_FREE(state, LitReference, object);
            }
            break;
        default:
            {
                UNREACHABLE
            }
            break;
    }
}

void lit_free_objects(LitState* state, LitObject* objects)
{
    LitObject* object = objects;

    while(object != NULL)
    {
        LitObject* next = object->next;
        lit_free_object(state, object);
        object = next;
    }

    free(state->vm->gray_stack);
    state->vm->gray_capacity = 0;
}

void lit_mark_object(LitVM* vm, LitObject* object)
{
    if(object == NULL || object->marked)
    {
        return;
    }

    object->marked = true;

#ifdef LIT_LOG_MARKING
    printf("%p mark ", (void*)object);
    lit_print_value(OBJECT_VALUE(object));
    printf("\n");
#endif

    if(vm->gray_capacity < vm->gray_count + 1)
    {
        vm->gray_capacity = LIT_GROW_CAPACITY(vm->gray_capacity);
        vm->gray_stack = (LitObject**)realloc(vm->gray_stack, sizeof(LitObject*) * vm->gray_capacity);
    }

    vm->gray_stack[vm->gray_count++] = object;
}

void lit_mark_value(LitVM* vm, LitValue value)
{
    if(IS_OBJECT(value))
    {
        lit_mark_object(vm, lit_as_object(value));
    }
}

static void mark_roots(LitVM* vm)
{
    size_t i;
    LitState* state;
    state = vm->state;
    for(i = 0; i < state->root_count; i++)
    {
        lit_mark_value(vm, state->roots[i]);
    }
    lit_mark_object(vm, (LitObject*)vm->fiber);
    lit_mark_object(vm, (LitObject*)state->classvalue_class);
    lit_mark_object(vm, (LitObject*)state->objectvalue_class);
    lit_mark_object(vm, (LitObject*)state->numbervalue_class);
    lit_mark_object(vm, (LitObject*)state->stringvalue_class);
    lit_mark_object(vm, (LitObject*)state->boolvalue_class);
    lit_mark_object(vm, (LitObject*)state->functionvalue_class);
    lit_mark_object(vm, (LitObject*)state->fibervalue_class);
    lit_mark_object(vm, (LitObject*)state->modulevalue_class);
    lit_mark_object(vm, (LitObject*)state->arrayvalue_class);
    lit_mark_object(vm, (LitObject*)state->mapvalue_class);
    lit_mark_object(vm, (LitObject*)state->rangevalue_class);
    lit_mark_object(vm, (LitObject*)state->api_name);
    lit_mark_object(vm, (LitObject*)state->api_function);
    lit_mark_object(vm, (LitObject*)state->api_fiber);
    lit_mark_table(vm, &state->preprocessor->defined);
    lit_mark_table(vm, &vm->modules->values);
    lit_mark_table(vm, &vm->globals->values);
}

static void mark_array(LitVM* vm, PCGenericArray<LitValue>* array)
{
    size_t i;
    for(i = 0; i < array->m_count; i++)
    {
        lit_mark_value(vm, array->m_values[i]);
    }
}

static void blacken_object(LitVM* vm, LitObject* object)
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
    printf("%p blacken ", (void*)object);
    lit_print_value(OBJECT_VALUE(object));
    printf("\n");
#endif
    switch(object->type)
    {
        case LITTYPE_NATIVE_FUNCTION:
        case LITTYPE_NATIVE_PRIMITIVE:
        case LITTYPE_NATIVE_METHOD:
        case LITTYPE_PRIMITIVE_METHOD:
        case LITTYPE_RANGE:
        case LITTYPE_STRING:
            {
            }
            break;
        case LITTYPE_USERDATA:
            {
                data = (LitUserdata*)object;
                if(data->cleanup_fn != NULL)
                {
                    data->cleanup_fn(vm->state, data, true);
                }
            }
            break;
        case LITTYPE_FUNCTION:
            {
                function = (LitFunction*)object;
                lit_mark_object(vm, (LitObject*)function->name);
                mark_array(vm, &function->chunk.constants);
            }
            break;
        case LITTYPE_FIBER:
            {
                fiber = (LitFiber*)object;
                for(LitValue* slot = fiber->stack; slot < fiber->stack_top; slot++)
                {
                    lit_mark_value(vm, *slot);
                }
                for(i = 0; i < fiber->frame_count; i++)
                {
                    frame = &fiber->frames[i];
                    if(frame->closure != NULL)
                    {
                        lit_mark_object(vm, (LitObject*)frame->closure);
                    }
                    else
                    {
                        lit_mark_object(vm, (LitObject*)frame->function);
                    }
                }
                for(upvalue = fiber->open_upvalues; upvalue != NULL; upvalue = upvalue->next)
                {
                    lit_mark_object(vm, (LitObject*)upvalue);
                }
                lit_mark_value(vm, fiber->error);
                lit_mark_object(vm, (LitObject*)fiber->module);
                lit_mark_object(vm, (LitObject*)fiber->parent);
            }
            break;
        case LITTYPE_MODULE:
            {
                module = (LitModule*)object;
                lit_mark_value(vm, module->return_value);
                lit_mark_object(vm, (LitObject*)module->name);
                lit_mark_object(vm, (LitObject*)module->main_function);
                lit_mark_object(vm, (LitObject*)module->main_fiber);
                lit_mark_object(vm, (LitObject*)module->private_names);
                for(i = 0; i < module->private_count; i++)
                {
                    lit_mark_value(vm, module->privates[i]);
                }
            }
            break;
        case LITTYPE_CLOSURE:
            {
                closure = (LitClosure*)object;
                lit_mark_object(vm, (LitObject*)closure->function);
                // Check for NULL is needed for a really specific gc-case
                if(closure->upvalues != NULL)
                {
                    for(i = 0; i < closure->upvalue_count; i++)
                    {
                        lit_mark_object(vm, (LitObject*)closure->upvalues[i]);
                    }
                }
            }
            break;
        case LITTYPE_UPVALUE:
            {
                lit_mark_value(vm, ((LitUpvalue*)object)->closed);
            }
            break;
        case LITTYPE_CLASS:
            {
                klass = (LitClass*)object;
                lit_mark_object(vm, (LitObject*)klass->name);
                lit_mark_object(vm, (LitObject*)klass->super);
                lit_mark_table(vm, &klass->methods);
                lit_mark_table(vm, &klass->static_fields);
            }
            break;
        case LITTYPE_INSTANCE:
            {
                LitInstance* instance = (LitInstance*)object;
                lit_mark_object(vm, (LitObject*)instance->klass);
                lit_mark_table(vm, &instance->fields);
            }
            break;
        case LITTYPE_BOUND_METHOD:
            {
                bound_method = (LitBoundMethod*)object;
                lit_mark_value(vm, bound_method->receiver);
                lit_mark_value(vm, bound_method->method);
            }
            break;
        case LITTYPE_ARRAY:
            {
                mark_array(vm, &((LitArray*)object)->values);
            }
            break;
        case LITTYPE_MAP:
            {
                lit_mark_table(vm, &((LitMap*)object)->values);
            }
            break;
        case LITTYPE_FIELD:
            {
                field = (LitField*)object;
                lit_mark_object(vm, (LitObject*)field->getter);
                lit_mark_object(vm, (LitObject*)field->setter);
            }
            break;
        case LITTYPE_REFERENCE:
            {
                lit_mark_value(vm, *((LitReference*)object)->slot);
            }
            break;
        default:
            {
                UNREACHABLE
            }
            break;
    }
}

static void trace_references(LitVM* vm)
{
    LitObject* object;
    while(vm->gray_count > 0)
    {
        object = vm->gray_stack[--vm->gray_count];
        blacken_object(vm, object);
    }
}

static void sweep(LitVM* vm)
{
    LitObject* unreached;
    LitObject* previous;
    LitObject* object;
    previous = NULL;
    object = vm->objects;
    while(object != NULL)
    {
        if(object->marked)
        {
            object->marked = false;
            previous = object;
            object = object->next;
        }
        else
        {
            unreached = object;
            object = object->next;
            if(previous != NULL)
            {
                previous->next = object;
            }
            else
            {
                vm->objects = object;
            }
            lit_free_object(vm->state, unreached);
        }
    }
}

uint64_t lit_collect_garbage(LitVM* vm)
{
    clock_t t;
    uint64_t before;
    uint64_t collected;
    (void)t;
    if(!vm->state->allow_gc)
    {
        return 0;
    }

    vm->state->allow_gc = false;
    before = vm->state->bytes_allocated;

#ifdef LIT_LOG_GC
    printf("-- gc begin\n");
    t = clock();
#endif

    mark_roots(vm);
    trace_references(vm);
    lit_table_remove_white(&vm->strings);
    sweep(vm);
    vm->state->next_gc = vm->state->bytes_allocated * LIT_GC_HEAP_GROW_FACTOR;
    vm->state->allow_gc = true;
    collected = before - vm->state->bytes_allocated;

#ifdef LIT_LOG_GC
    printf("-- gc end. Collected %imb in %gms\n", ((int)((collected / 1024.0 + 0.5) / 10)) * 10,
           (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
#endif
    return collected;
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
    if(IS_OBJECT(value))
    {
        LitObjectType type = OBJECT_TYPE(value);
        return (
            (type == LITTYPE_CLOSURE) ||
            (type == LITTYPE_FUNCTION) ||
            (type == LITTYPE_NATIVE_FUNCTION) ||
            (type == LITTYPE_NATIVE_PRIMITIVE) ||
            (type == LITTYPE_NATIVE_METHOD) ||
            (type == LITTYPE_PRIMITIVE_METHOD) ||
            (type == LITTYPE_BOUND_METHOD)
        );
    }

    return false;
}

LitObject* lit_allocate_object(LitState* state, size_t size, LitObjectType type)
{
    LitObject* object;
    object = (LitObject*)lit_reallocate(state, NULL, 0, size);
    object->type = type;
    object->marked = false;
    object->next = state->vm->objects;
    state->vm->objects = object;
#ifdef LIT_LOG_ALLOCATION
    printf("%p allocate %ld for %s\n", (void*)object, size, lit_get_value_type(type));
#endif
    return object;
}

LitFunction* lit_create_function(LitState* state, LitModule* module)
{
    LitFunction* function;
    function = ALLOCATE_OBJECT(state, LitFunction, LITTYPE_FUNCTION);
    lit_init_chunk(state, &function->chunk);
    function->name = NULL;
    function->arg_count = 0;
    function->upvalue_count = 0;
    function->max_slots = 0;
    function->module = module;
    function->vararg = false;
    return function;
}

LitValue lit_get_function_name(LitVM* vm, LitValue instance)
{
    LitString* name;
    LitField* field;
    name = NULL;
    switch(OBJECT_TYPE(instance))
    {
        case LITTYPE_FUNCTION:
            {
                name = AS_FUNCTION(instance)->name;
            }
            break;
        case LITTYPE_CLOSURE:
            {
                name = AS_CLOSURE(instance)->function->name;
            }
            break;
        case LITTYPE_FIELD:
            {
                field = AS_FIELD(instance);
                if(field->getter != NULL)
                {
                    return lit_get_function_name(vm, OBJECT_VALUE(field->getter));
                }
                return lit_get_function_name(vm, OBJECT_VALUE(field->setter));
            }
            break;
        case LITTYPE_NATIVE_PRIMITIVE:
            {
                name = AS_NATIVE_PRIMITIVE(instance)->name;
            }
            break;
        case LITTYPE_NATIVE_FUNCTION:
            {
                name = AS_NATIVE_FUNCTION(instance)->name;
            }
            break;
        case LITTYPE_NATIVE_METHOD:
            {
                name = AS_NATIVE_METHOD(instance)->name;
            }
            break;
        case LITTYPE_PRIMITIVE_METHOD:
            {
                name = AS_PRIMITIVE_METHOD(instance)->name;
            }
            break;
        case LITTYPE_BOUND_METHOD:
            {
                return lit_get_function_name(vm, AS_BOUND_METHOD(instance)->method);
            }
            break;
        default:
            {
            }
            break;
    }
    if(name == NULL)
    {
        return OBJECT_VALUE(lit_string_format(vm->state, "function #", *((double*)lit_as_object(instance))));
    }

    return OBJECT_VALUE(lit_string_format(vm->state, "function @", OBJECT_VALUE(name)));
}

LitUpvalue* lit_create_upvalue(LitState* state, LitValue* slot)
{
    LitUpvalue* upvalue;
    upvalue = ALLOCATE_OBJECT(state, LitUpvalue, LITTYPE_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NULL_VALUE;
    upvalue->next = NULL;
    return upvalue;
}

LitClosure* lit_create_closure(LitState* state, LitFunction* function)
{
    size_t i;
    LitClosure* closure;
    LitUpvalue** upvalues;
    closure = ALLOCATE_OBJECT(state, LitClosure, LITTYPE_CLOSURE);
    lit_push_root(state, (LitObject*)closure);
    upvalues = LIT_ALLOCATE(state, LitUpvalue*, function->upvalue_count);
    lit_pop_root(state);
    for(i = 0; i < function->upvalue_count; i++)
    {
        upvalues[i] = NULL;
    }
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}

LitNativeFunction* lit_create_native_function(LitState* state, LitNativeFunctionFn function, LitString* name)
{
    LitNativeFunction* native;
    native = ALLOCATE_OBJECT(state, LitNativeFunction, LITTYPE_NATIVE_FUNCTION);
    native->function = function;
    native->name = name;
    return native;
}

LitNativePrimFunction* lit_create_native_primitive(LitState* state, LitNativePrimitiveFn function, LitString* name)
{
    LitNativePrimFunction* native;
    native = ALLOCATE_OBJECT(state, LitNativePrimFunction, LITTYPE_NATIVE_PRIMITIVE);
    native->function = function;
    native->name = name;
    return native;
}

LitNativeMethod* lit_create_native_method(LitState* state, LitNativeMethodFn method, LitString* name)
{
    LitNativeMethod* native;
    native = ALLOCATE_OBJECT(state, LitNativeMethod, LITTYPE_NATIVE_METHOD);
    native->method = method;
    native->name = name;
    return native;
}

LitPrimitiveMethod* lit_create_primitive_method(LitState* state, LitPrimitiveMethodFn method, LitString* name)
{
    LitPrimitiveMethod* native;
    native = ALLOCATE_OBJECT(state, LitPrimitiveMethod, LITTYPE_PRIMITIVE_METHOD);
    native->method = method;
    native->name = name;
    return native;
}

LitFiber* lit_create_fiber(LitState* state, LitModule* module, LitFunction* function)
{
    size_t stack_capacity;
    LitValue* stack;
    LitCallFrame* frame;
    LitCallFrame* frames;
    LitFiber* fiber;
    // Allocate in advance, just in case GC is triggered
    stack_capacity = function == NULL ? 1 : (size_t)lit_closest_power_of_two(function->max_slots + 1);
    stack = LIT_ALLOCATE(state, LitValue, stack_capacity);
    frames = LIT_ALLOCATE(state, LitCallFrame, LIT_INITIAL_CALL_FRAMES);
    fiber = ALLOCATE_OBJECT(state, LitFiber, LITTYPE_FIBER);
    if(module != NULL)
    {
        if(module->main_fiber == NULL)
        {
            module->main_fiber = fiber;
        }
    }
    fiber->stack = stack;
    fiber->stack_capacity = stack_capacity;
    fiber->stack_top = fiber->stack;
    fiber->frames = frames;
    fiber->frame_capacity = LIT_INITIAL_CALL_FRAMES;
    fiber->parent = NULL;
    fiber->frame_count = 1;
    fiber->arg_count = 0;
    fiber->module = module;
    fiber->catcher = false;
    fiber->error = NULL_VALUE;
    fiber->open_upvalues = NULL;
    fiber->abort = false;
    frame = &fiber->frames[0];
    frame->closure = NULL;
    frame->function = function;
    frame->slots = fiber->stack;
    frame->result_ignored = false;
    frame->return_to_c = false;
    if(function != NULL)
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
        for(upvalue = fiber->open_upvalues; upvalue != NULL; upvalue = upvalue->next)
        {
            upvalue->location = fiber->stack + (upvalue->location - old_stack);
        }
        fiber->stack_top = fiber->stack + (fiber->stack_top - old_stack);
    }
}

LitModule* lit_create_module(LitState* state, LitString* name)
{
    LitModule* module;
    module = ALLOCATE_OBJECT(state, LitModule, LITTYPE_MODULE);
    module->name = name;
    module->return_value = NULL_VALUE;
    module->main_function = NULL;
    module->privates = NULL;
    module->ran = false;
    module->main_fiber = NULL;
    module->private_count = 0;
    module->private_names = lit_create_map(state);
    return module;
}

LitClass* lit_create_class(LitState* state, LitString* name)
{
    LitClass* klass;
    klass = ALLOCATE_OBJECT(state, LitClass, LITTYPE_CLASS);
    klass->name = name;
    klass->init_method = NULL;
    klass->super = NULL;
    klass->methods.init(state);
    klass->static_fields.init(state);
    return klass;
}

LitInstance* lit_create_instance(LitState* state, LitClass* klass)
{
    LitInstance* instance;
    instance = ALLOCATE_OBJECT(state, LitInstance, LITTYPE_INSTANCE);
    instance->klass = klass;
    instance->fields.init(state);
    instance->fields.m_count = 0;
    return instance;
}

LitBoundMethod* lit_create_bound_method(LitState* state, LitValue receiver, LitValue method)
{
    LitBoundMethod* bound_method;
    bound_method = ALLOCATE_OBJECT(state, LitBoundMethod, LITTYPE_BOUND_METHOD);
    bound_method->receiver = receiver;
    bound_method->method = method;
    return bound_method;
}

LitArray* lit_create_array(LitState* state)
{
    LitArray* array;
    array = ALLOCATE_OBJECT(state, LitArray, LITTYPE_ARRAY);
    array->values.init(state);
    return array;
}

LitMap* lit_create_map(LitState* state)
{
    LitMap* map;
    map = ALLOCATE_OBJECT(state, LitMap, LITTYPE_MAP);
    map->values.init(state);
    map->index_fn = NULL;
    return map;
}

bool lit_map_set(LitState* state, LitMap* map, LitString* key, LitValue value)
{
    if(value == NULL_VALUE)
    {
        lit_map_delete(map, key);
        return false;
    }
    return lit_table_set(state, &map->values, key, value);
}

bool lit_map_get(LitMap* map, LitString* key, LitValue* value)
{
    return lit_table_get(&map->values, key, value);
}

bool lit_map_delete(LitMap* map, LitString* key)
{
    return lit_table_delete(&map->values, key);
}

void lit_map_add_all(LitState* state, LitMap* from, LitMap* to)
{
    int i;
    LitTableEntry* entry;
    for(i = 0; i <= from->values.m_capacity; i++)
    {
        entry = &from->values.m_values[i];
        if(entry->key != NULL)
        {
            lit_table_set(state, &to->values, entry->key, entry->value);
        }
    }
}

LitUserdata* lit_create_userdata(LitState* state, size_t size, bool ispointeronly)
{
    LitUserdata* userdata;
    userdata = ALLOCATE_OBJECT(state, LitUserdata, LITTYPE_USERDATA);
    userdata->data = NULL;
    if(size > 0)
    {
        if(!ispointeronly)
        {
            userdata->data = lit_reallocate(state, NULL, 0, size);
        }
    }
    userdata->size = size;
    userdata->cleanup_fn = NULL;
    userdata->canfree = true;
    return userdata;
}

LitRange* lit_create_range(LitState* state, double from, double to)
{
    LitRange* range;
    range = ALLOCATE_OBJECT(state, LitRange, LITTYPE_RANGE);
    range->from = from;
    range->to = to;
    return range;
}

LitField* lit_create_field(LitState* state, LitObject* getter, LitObject* setter)
{
    LitField* field;
    field = ALLOCATE_OBJECT(state, LitField, LITTYPE_FIELD);
    field->getter = getter;
    field->setter = setter;
    return field;
}

LitReference* lit_create_reference(LitState* state, LitValue* slot)
{
    LitReference* reference;
    reference = ALLOCATE_OBJECT(state, LitReference, LITTYPE_REFERENCE);
    reference->slot = slot;
    return reference;
}


static bool measure_compilation_time;
static double last_source_time = 0;

void lit_enable_compilation_time_measurement()
{
    measure_compilation_time = true;
}

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

LitState* lit_new_state()
{
    LitState* state;
    state = (LitState*)malloc(sizeof(LitState));
    state->classvalue_class = NULL;
    state->objectvalue_class = NULL;
    state->numbervalue_class = NULL;
    state->stringvalue_class = NULL;
    state->boolvalue_class = NULL;
    state->functionvalue_class = NULL;
    state->fibervalue_class = NULL;
    state->modulevalue_class = NULL;
    state->arrayvalue_class = NULL;
    state->mapvalue_class = NULL;
    state->rangevalue_class = NULL;
    state->bytes_allocated = 0;
    state->next_gc = 256 * 1024;
    state->allow_gc = false;
    state->error_fn = default_error;
    state->print_fn = default_printf;
    state->had_error = false;
    state->roots = NULL;
    state->root_count = 0;
    state->root_capacity = 0;
    state->last_module = NULL;
    lit_writer_init_file(state, &state->debugwriter, stdout, true);
    state->preprocessor = (LitPreprocessor*)malloc(sizeof(LitPreprocessor));
    lit_init_preprocessor(state, state->preprocessor);
    state->scanner = (LitScanner*)malloc(sizeof(LitScanner));
    state->parser = (LitParser*)malloc(sizeof(LitParser));
    lit_init_parser(state, (LitParser*)state->parser);
    state->emitter = (LitEmitter*)malloc(sizeof(LitEmitter));
    lit_init_emitter(state, state->emitter);
    state->optimizer = (LitOptimizer*)malloc(sizeof(LitOptimizer));
    lit_init_optimizer(state, state->optimizer);
    state->vm = (LitVM*)malloc(sizeof(LitVM));
    lit_init_vm(state, state->vm);
    lit_init_api(state);
    lit_open_core_library(state);
    return state;
}

int64_t lit_free_state(LitState* state)
{
    int64_t amount;
    if(state->roots != NULL)
    {
        free(state->roots);
        state->roots = NULL;
    }
    lit_free_api(state);
    lit_free_preprocessor(state->preprocessor);
    free(state->preprocessor);
    free(state->scanner);
    lit_free_parser(state->parser);
    free(state->parser);
    lit_free_emitter(state->emitter);
    free(state->emitter);
    free(state->optimizer);
    lit_free_vm(state->vm);
    free(state->vm);
    amount = state->bytes_allocated;
    free(state);
    return amount;
}

void lit_push_root(LitState* state, LitObject* object)
{
    lit_push_value_root(state, OBJECT_VALUE(object));
}

void lit_push_value_root(LitState* state, LitValue value)
{
    if(state->root_count + 1 >= state->root_capacity)
    {
        state->root_capacity = LIT_GROW_CAPACITY(state->root_capacity);
        state->roots = (LitValue*)realloc(state->roots, state->root_capacity * sizeof(LitValue));
    }
    state->roots[state->root_count++] = value;
}

LitValue lit_peek_root(LitState* state, uint8_t distance)
{
    assert(state->root_count - distance + 1 > 0);
    return state->roots[state->root_count - distance - 1];
}

void lit_pop_root(LitState* state)
{
    state->root_count--;
}

void lit_pop_roots(LitState* state, uint8_t amount)
{
    state->root_count -= amount;
}

LitClass* lit_get_class_for(LitState* state, LitValue value)
{
    LitValue* slot;
    LitUpvalue* upvalue;
    if(IS_OBJECT(value))
    {
        switch(OBJECT_TYPE(value))
        {
            case LITTYPE_STRING:
                {
                    return state->stringvalue_class;
                }
                break;
            case LITTYPE_USERDATA:
                {
                    return state->objectvalue_class;
                }
                break;
            case LITTYPE_FIELD:
            case LITTYPE_FUNCTION:
            case LITTYPE_CLOSURE:
            case LITTYPE_NATIVE_FUNCTION:
            case LITTYPE_NATIVE_PRIMITIVE:
            case LITTYPE_BOUND_METHOD:
            case LITTYPE_PRIMITIVE_METHOD:
            case LITTYPE_NATIVE_METHOD:
                {
                    return state->functionvalue_class;
                }
                break;
            case LITTYPE_FIBER:
                {
                    //fprintf(stderr, "should return fiber class ....\n");
                    return state->fibervalue_class;
                }
                break;
            case LITTYPE_MODULE:
                {
                    return state->modulevalue_class;
                }
                break;
            case LITTYPE_UPVALUE:
                {
                    upvalue = AS_UPVALUE(value);
                    if(upvalue->location == NULL)
                    {
                        return lit_get_class_for(state, upvalue->closed);
                    }
                    return lit_get_class_for(state, *upvalue->location);
                }
                break;
            case LITTYPE_INSTANCE:
                {
                    return AS_INSTANCE(value)->klass;
                }
                break;
            case LITTYPE_CLASS:
                {
                    return state->classvalue_class;
                }
                break;
            case LITTYPE_ARRAY:
                {
                    return state->arrayvalue_class;
                }
                break;
            case LITTYPE_MAP:
                {
                    return state->mapvalue_class;
                }
                break;
            case LITTYPE_RANGE:
                {
                    return state->rangevalue_class;
                }
                break;
            case LITTYPE_REFERENCE:
                {
                    slot = AS_REFERENCE(value)->slot;
                    if(slot != NULL)
                    {
                        return lit_get_class_for(state, *slot);
                    }

                    return state->objectvalue_class;
                }
                break;
        }
    }
    else if(IS_NUMBER(value))
    {
        return state->numbervalue_class;
    }
    else if(IS_BOOL(value))
    {
        return state->boolvalue_class;
    }
    //fprintf(stderr, "failed to find class object!\n");
    return NULL;
}

static void free_statements(LitState* state, PCGenericArray<LitStatement*>* statements)
{
    size_t i;
    for(i = 0; i < statements->m_count; i++)
    {
        lit_free_statement(state, statements->m_values[i]);
    }
    statements->release(state);
}

LitInterpretResult lit_interpret(LitState* state, const char* module_name, char* code)
{
    return lit_internal_interpret(state, lit_string_copy(state, module_name, strlen(module_name)), code);
}

LitModule* lit_compile_module(LitState* state, LitString* module_name, char* code)
{
    clock_t t;
    clock_t total_t;
    bool allowed_gc;
    LitModule* module;
    PCGenericArray<LitStatement*> statements;
    allowed_gc = state->allow_gc;
    state->allow_gc = false;
    state->had_error = false;
    module = NULL;
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
            return NULL;
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
            return NULL;
        }
        fprintf(stderr, "statements.m_values(%d items)[0]=%p\n", statements.m_count, statements.m_values[0]);
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
        module = lit_emit(state->emitter, statements, module_name);
        free_statements(state, &statements);
        //if(measure_compilation_time)
        {
            printf("Emitting:       %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            printf("\nTotal:          %gms\n-----------------------\n",
                   (double)(clock() - total_t) / CLOCKS_PER_SEC * 1000 + last_source_time);
        }
    }
    state->allow_gc = allowed_gc;
    return state->had_error ? NULL : module;
}

LitModule* lit_get_module(LitState* state, const char* name)
{
    LitValue value;
    if(lit_table_get(&state->vm->modules->values, CONST_STRING(state, name), &value))
    {
        return AS_MODULE(value);
    }
    return NULL;
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
    if(module == NULL)
    {
        return (LitInterpretResult){ LITRESULT_COMPILE_ERROR, NULL_VALUE };
    }
    result = lit_interpret_module(state, module);
    fiber = module->main_fiber;
    if(!state->had_error && !fiber->abort && fiber->stack_top != fiber->stack)
    {
        istack = (intptr_t)(fiber->stack);
        itop = (intptr_t)(fiber->stack_top);
        idif = (intptr_t)(fiber->stack - fiber->stack_top);
        /* me fail english. how do i put this better? */
        lit_error(state, RUNTIME_ERROR, "stack should be same as stack top", idif, istack, istack, itop, itop);
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
        if(source == NULL)
        {
            lit_error(state, COMPILE_ERROR, "failed to open file '%s' for reading", file_name);
            return false;
        }
        file_name = lit_patch_file_name(file_name);
        module_name = lit_string_copy(state, file_name, strlen(file_name));
        module = lit_compile_module(state, module_name, source);
        compiled_modules[i] = module;
        free((void*)source);
        free((void*)file_name);
        if(module == NULL)
        {
            return false;
        }
    }
    file = fopen(output_file, "w+b");
    if(file == NULL)
    {
        lit_error(state, COMPILE_ERROR, "failed to open file '%s' for writing", output_file);
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
    if(source == NULL)
    {
        lit_error(state, RUNTIME_ERROR, "failed to open file '%s' for reading", file_name);
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
    if(source == NULL)
    {
        return INTERPRET_RUNTIME_FAIL;
    }
    result = lit_interpret(state, patched_file_name, source);
    free((void*)source);
    free(patched_file_name);
    return result;
}

LitInterpretResult lit_dump_file(LitState* state, const char* file)
{
    char* patched_file_name;
    char* source;
    LitInterpretResult result;
    LitString* module_name;
    LitModule* module;
    source = read_source(state, file, &patched_file_name);
    if(source == NULL)
    {
        return INTERPRET_RUNTIME_FAIL;
    }
    module_name = lit_string_copy(state, patched_file_name, strlen(patched_file_name));
    module = lit_compile_module(state, module_name, source);
    if(module == NULL)
    {
        result = INTERPRET_RUNTIME_FAIL;
    }
    else
    {
        lit_disassemble_module(state, module, source);
        result = (LitInterpretResult){ LITRESULT_OK, NULL_VALUE };
    }
    free((void*)source);
    free((void*)patched_file_name);
    return result;
}

void lit_error(LitState* state, LitErrorType type, const char* message, ...)
{
    size_t buffer_size;
    char* buffer;
    va_list args;
    va_list args_copy;
    (void)type;
    va_start(args, message);
    va_copy(args_copy, args);
    buffer_size = vsnprintf(NULL, 0, message, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, message, args);
    va_end(args);
    state->error_fn(state, buffer);
    state->had_error = true;
    /* TODO: is this safe? */
    free(buffer);
}

void lit_printf(LitState* state, const char* message, ...)
{
    size_t buffer_size;
    char* buffer;
    va_list args;
    va_start(args, message);
    va_list args_copy;
    va_copy(args_copy, args);
    buffer_size = vsnprintf(NULL, 0, message, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, message, args);
    va_end(args);
    state->print_fn(state, buffer);
    free(buffer);
}

static LitTableEntry* find_entry(LitTableEntry* entries, int capacity, LitString* key)
{
    uint32_t index;
    LitTableEntry* entry;
    LitTableEntry* tombstone;
    index = key->hash % capacity;
    tombstone = NULL;
    while(true)
    {
        entry = &entries[index];
        if(entry->key == NULL)
        {
            if(IS_NULL(entry->value))
            {
                return tombstone != NULL ? tombstone : entry;
            }
            else if(tombstone == NULL)
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

static void adjust_capacity(LitState* state, LitTable* table, int capacity)
{
    size_t i;
    LitTableEntry* destination;
    LitTableEntry* entries;
    LitTableEntry* entry;
    entries = LIT_ALLOCATE(state, LitTableEntry, capacity + 1);
    for(i = 0; i <= capacity; i++)
    {
        entries[i].key = NULL;
        entries[i].value = NULL_VALUE;
    }
    table->m_count = 0;
    for(i = 0; i <= table->m_capacity; i++)
    {
        entry = &table->m_values[i];
        if(entry == NULL)
        {
            continue;
        }
        if(entry->key == NULL)
        {
            continue;
        }
        destination = find_entry(entries, capacity, entry->key);
        destination->key = entry->key;
        destination->value = entry->value;
        table->m_count++;
    }
    LIT_FREE_ARRAY(state, LitTableEntry, table->m_values, table->m_capacity + 1);
    table->m_capacity = capacity;
    table->m_values = entries;
}

bool lit_table_set(LitState* state, LitTable* table, LitString* key, LitValue value)
{
    bool is_new;
    int capacity;
    LitTableEntry* entry;
    if(table->m_count + 1 > (table->m_capacity + 1) * TABLE_MAX_LOAD)
    {
        capacity = LIT_GROW_CAPACITY(table->m_capacity + 1) - 1;
        adjust_capacity(state, table, capacity);
    }
    entry = find_entry(table->m_values, table->m_capacity, key);
    is_new = entry->key == NULL;
    if(is_new && IS_NULL(entry->value))
    {
        table->m_count++;
    }
    entry->key = key;
    entry->value = value;
    return is_new;
}

bool lit_table_get(LitTable* table, LitString* key, LitValue* value)
{
    LitTableEntry* entry;
    if(table->m_count == 0)
    {
        return false;
    }
    entry = find_entry(table->m_values, table->m_capacity, key);
    if(entry->key == NULL)
    {
        return false;
    }
    *value = entry->value;
    return true;
}

bool lit_table_get_slot(LitTable* table, LitString* key, LitValue** value)
{
    LitTableEntry* entry;
    if(table->m_count == 0)
    {
        return false;
    }
    entry = find_entry(table->m_values, table->m_capacity, key);
    if(entry->key == NULL)
    {
        return false;
    }
    *value = &entry->value;
    return true;
}

bool lit_table_delete(LitTable* table, LitString* key)
{
    LitTableEntry* entry;
    if(table->m_count == 0)
    {
        return false;
    }
    entry = find_entry(table->m_values, table->m_capacity, key);
    if(entry->key == NULL)
    {
        return false;
    }
    entry->key = NULL;
    entry->value = BOOL_VALUE(true);
    return true;
}

LitString* lit_table_find_string(LitTable* table, const char* chars, size_t length, uint32_t hash)
{
    uint32_t index;
    LitTableEntry* entry;
    if(table->m_count == 0)
    {
        return NULL;
    }
    index = hash % table->m_capacity;
    while(true)
    {
        entry = &table->m_values[index];
        if(entry->key == NULL)
        {
            if(IS_NULL(entry->value))
            {
                return NULL;
            }
        }
        else if(lit_string_length(entry->key) == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0)
        {
            return entry->key;
        }
        index = (index + 1) % table->m_capacity;
    }
}

void lit_table_add_all(LitState* state, LitTable* from, LitTable* to)
{
    int i;
    LitTableEntry* entry;
    for(i = 0; i <= from->m_capacity; i++)
    {
        entry = &from->m_values[i];
        if(entry->key != NULL)
        {
            lit_table_set(state, to, entry->key, entry->value);
        }
    }
}

void lit_table_remove_white(LitTable* table)
{
    int i;
    LitTableEntry* entry;
    for(i = 0; i <= table->m_capacity; i++)
    {
        entry = &table->m_values[i];
        if(entry->key != NULL && !entry->key->object.marked)
        {
            lit_table_delete(table, entry->key);
        }
    }
}

void lit_mark_table(LitVM* vm, LitTable* table)
{
    int i;
    LitTableEntry* entry;
    for(i = 0; i <= table->m_capacity; i++)
    {
        entry = &table->m_values[i];
        lit_mark_object(vm, (LitObject*)entry->key);
        lit_mark_value(vm, entry->value);
    }
}


int lit_decode_num_bytes(uint8_t byte)
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

int lit_ustring_length(LitString* string)
{
    int length;
    uint32_t i;
    length = 0;
    for(i = 0; i < lit_string_length(string);)
    {
        i += lit_decode_num_bytes(string->chars[i]);
        length++;
    }
    return length;
}

LitString* lit_ustring_code_point_at(LitState* state, LitString* string, uint32_t index)
{
    char bytes[2];
    int code_point;
    if(index >= lit_string_length(string))
    {
        return NULL;
    }
    code_point = lit_ustring_decode((uint8_t*)string->chars + index, lit_string_length(string) - index);
    if(code_point == -1)
    {
        bytes[0] = string->chars[index];
        bytes[1] = '\0';
        return lit_string_copy(state, bytes, 1);
    }
    return lit_ustring_from_code_point(state, code_point);
}

LitString* lit_ustring_from_code_point(LitState* state, int value)
{
    int length;
    char* bytes;
    LitString* rt;
    length = lit_encode_num_bytes(value);
    bytes = LIT_ALLOCATE(state, char, length + 1);
    lit_ustring_encode(value, (uint8_t*)bytes);
    /* this should be lit_string_take, but something prevents the memory from being free'd. */
    rt = lit_string_copy(state, bytes, length);
    LIT_FREE(state, char, bytes);
    return rt;
}

LitString* lit_ustring_from_range(LitState* state, LitString* source, int start, uint32_t count)
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
        length += lit_decode_num_bytes(from[start + i]);
    }
    bytes = (char*)malloc(length + 1);
    to = (uint8_t*)bytes;
    for(i = 0; i < count; i++)
    {
        index = start + i;
        code_point = lit_ustring_decode(from + index, lit_string_length(source) - index);
        if(code_point != -1)
        {
            to += lit_ustring_encode(code_point, to);
        }
    }
    return lit_string_take(state, bytes, length, false);
}

int lit_encode_num_bytes(int value)
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

int lit_ustring_encode(int value, uint8_t* bytes)
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

int lit_ustring_decode(const uint8_t* bytes, uint32_t length)
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

int lit_uchar_offset(char* str, int index)
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

static void litwr_cb_writebyte(LitWriter* wr, int byte)
{
    LitString* ds;
    if(wr->stringmode)
    {
        ds = (LitString*)wr->uptr;
        lit_string_append_char(ds, byte);        
    }
    else
    {
        fputc(byte, (FILE*)wr->uptr);
    }
}

static void litwr_cb_writestring(LitWriter* wr, const char* string, size_t len)
{
    LitString* ds;
    if(wr->stringmode)
    {
        ds = (LitString*)wr->uptr;
        lit_string_append_string(ds, string, len);
    }
    else
    {
        fwrite(string, sizeof(char), len, (FILE*)wr->uptr);
    }
}

static void litwr_cb_writeformat(LitWriter* wr, const char* fmt, va_list va)
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

static void lit_writer_init_default(LitState* state, LitWriter* wr)
{
    wr->state = state;
    wr->forceflush = false;
    wr->stringmode = false;
    wr->fnbyte = litwr_cb_writebyte;
    wr->fnstring = litwr_cb_writestring;
    wr->fnformat = litwr_cb_writeformat;
}

void lit_writer_init_file(LitState* state, LitWriter* wr, FILE* fh, bool forceflush)
{
    lit_writer_init_default(state, wr);
    wr->uptr = fh;
    wr->forceflush = forceflush;
}

void lit_writer_init_string(LitState* state, LitWriter* wr)
{
    lit_writer_init_default(state, wr);
    wr->stringmode = true;
    wr->uptr = lit_string_alloc_empty(state, 0, false);
}

void lit_writer_writebyte(LitWriter* wr, int byte)
{
    wr->fnbyte(wr, byte);
}

void lit_writer_writestringl(LitWriter* wr, const char* str, size_t len)
{
    wr->fnstring(wr, str, len);
}

void lit_writer_writestring(LitWriter* wr, const char* str)
{
    wr->fnstring(wr, str, strlen(str));
}

void lit_writer_writeformat(LitWriter* wr, const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    wr->fnformat(wr, fmt, va);
    va_end(va);
}

LitString* lit_writer_get_string(LitWriter* wr)
{
    if(wr->stringmode)
    {
        return (LitString*)wr->uptr;
    }
    return NULL;
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
    lit_writer_writeformat(wr, "(%u) [", (unsigned int)size);
    if(size > 0)
    {
        lit_writer_writestring(wr, " ");
        for(i = 0; i < size; i++)
        {
            if(IS_ARRAY(array->values.m_values[i]) && (array == AS_ARRAY(array->values.m_values[i])))
            {
                lit_writer_writestring(wr, "(recursion)");
            }
            else
            {
                lit_print_value(state, wr, array->values.m_values[i]);
            }
            if(i + 1 < size)
            {
                lit_writer_writestring(wr, ", ");
            }
            else
            {
                lit_writer_writestring(wr, " ");
            }
        }
    }
    lit_writer_writestring(wr, "]");
}

static void print_map(LitState* state, LitWriter* wr, LitMap* map, size_t size)
{
    bool had_before;
    size_t i;
    LitTableEntry* entry;
    lit_writer_writeformat(wr, "(%u) {", (unsigned int)size);
    had_before = false;
    if(size > 0)
    {
        for(i = 0; i < (size_t)map->values.m_capacity; i++)
        {
            entry = &map->values.m_values[i];
            if(entry->key != NULL)
            {
                if(had_before)
                {
                    lit_writer_writestring(wr, ", ");
                }
                else
                {
                    lit_writer_writestring(wr, " ");
                }
                lit_writer_writeformat(wr, "%s = ", entry->key->chars);
                if(IS_MAP(entry->value) && (map == AS_MAP(entry->value)))
                {
                    lit_writer_writestring(wr, "(recursion)");
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
        lit_writer_writestring(wr, " }");
    }
    else
    {
        lit_writer_writestring(wr, "}");
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
    obj = lit_as_object(value);
    if(obj != NULL)
    {
        switch(obj->type)
        {
            case LITTYPE_STRING:
                {
                    lit_writer_writeformat(wr, "%s", lit_as_cstring(value));
                }
                break;
            case LITTYPE_FUNCTION:
                {
                    lit_writer_writeformat(wr, "function %s", AS_FUNCTION(value)->name->chars);
                }
                break;
            case LITTYPE_CLOSURE:
                {
                    lit_writer_writeformat(wr, "closure %s", AS_CLOSURE(value)->function->name->chars);
                }
                break;
            case LITTYPE_NATIVE_PRIMITIVE:
                {
                    lit_writer_writeformat(wr, "function %s", AS_NATIVE_PRIMITIVE(value)->name->chars);
                }
                break;
            case LITTYPE_NATIVE_FUNCTION:
                {
                    lit_writer_writeformat(wr, "function %s", AS_NATIVE_FUNCTION(value)->name->chars);
                }
                break;
            case LITTYPE_PRIMITIVE_METHOD:
                {
                    lit_writer_writeformat(wr, "function %s", AS_PRIMITIVE_METHOD(value)->name->chars);
                }
                break;
            case LITTYPE_NATIVE_METHOD:
                {
                    lit_writer_writeformat(wr, "function %s", AS_NATIVE_METHOD(value)->name->chars);
                }
                break;
            case LITTYPE_FIBER:
                {
                    lit_writer_writeformat(wr, "fiber");
                }
                break;
            case LITTYPE_MODULE:
                {
                    lit_writer_writeformat(wr, "module %s", AS_MODULE(value)->name->chars);
                }
                break;

            case LITTYPE_UPVALUE:
                {
                    upvalue = AS_UPVALUE(value);
                    if(upvalue->location == NULL)
                    {
                        lit_print_value(state, wr, upvalue->closed);
                    }
                    else
                    {
                        print_object(state, wr, *upvalue->location);
                    }
                }
                break;
            case LITTYPE_CLASS:
                {
                    lit_writer_writeformat(wr, "class %s", AS_CLASS(value)->name->chars);
                }
                break;
            case LITTYPE_INSTANCE:
                {
                    /*
                    if(AS_INSTANCE(value)->klass->object.type == LITTYPE_MAP)
                    {
                        fprintf(stderr, "instance is a map\n");
                    }
                    printf("%s instance", AS_INSTANCE(value)->klass->name->chars);
                    */
                    lit_writer_writeformat(wr, "<instance '%s' ", AS_INSTANCE(value)->klass->name->chars);
                    map = AS_MAP(value);
                    size = map->values.m_count;
                    print_map(state, wr, map, size);
                    lit_writer_writestring(wr, ">");
                }
                break;
            case LITTYPE_BOUND_METHOD:
                {
                    lit_print_value(state, wr, AS_BOUND_METHOD(value)->method);
                    return;
                }
                break;
            case LITTYPE_ARRAY:
                {
                    #ifdef LIT_MINIMIZE_CONTAINERS
                        lit_writer_writestring(wr, "array");
                    #else
                        array = AS_ARRAY(value);
                        size = array->values.m_count;
                        print_array(state, wr, array, size);
                    #endif
                }
                break;
            case LITTYPE_MAP:
                {
                    #ifdef LIT_MINIMIZE_CONTAINERS
                        lit_writer_writeformat(wr, "map");
                    #else
                        map = AS_MAP(value);
                        size = map->values.m_count;
                        print_map(state, wr, map, size);
                    #endif
                }
                break;
            case LITTYPE_USERDATA:
                {
                    lit_writer_writeformat(wr, "userdata");
                }
                break;
            case LITTYPE_RANGE:
                {
                    range = AS_RANGE(value);
                    lit_writer_writeformat(wr, "%g .. %g", range->from, range->to);
                }
                break;
            case LITTYPE_FIELD:
                {
                    lit_writer_writeformat(wr, "field");
                }
                break;
            case LITTYPE_REFERENCE:
                {
                    lit_writer_writeformat(wr, "reference => ");
                    slot = AS_REFERENCE(value)->slot;
                    if(slot == NULL)
                    {
                        lit_writer_writestring(wr, "null");
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
        lit_writer_writestring(wr, "!nullpointer!");
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
    LitValue args[1] = {NULL_VALUE};
    mthname = CONST_STRING(state, "toString");
    fprintf(stderr, "lit_print_value: checking if toString() exists for '%s' ...\n", lit_get_value_type(value));
    if(AS_CLASS(value) != NULL)
    {
        mthtostring = lit_instance_get_method(state, value, mthname);
        if(!IS_NULL(mthtostring))
        {
            fprintf(stderr, "lit_print_value: we got toString()! now checking if calling it works ...\n");
            inret = lit_instance_call_method(state, value, mthname, args, 0);
            if(inret.type == LITRESULT_OK)
            {
                fprintf(stderr, "lit_print_value: calling toString() succeeded! but is it a string? ...\n");
                tstrval = inret.result;
                if(!IS_NULL(tstrval))
                {
                    fprintf(stderr, "lit_print_value: toString() returned a string! so that's what we'll use.\n");
                    tstring = lit_as_string(tstrval);
                    printf("%.*s", (int)lit_string_length(tstring), tstring->chars);
                    return;
                }
            }
        }
    }
    fprintf(stderr, "lit_print_value: nope, no toString(), or it didn't return a string. falling back to manual stringification\n");
    */
    if(IS_BOOL(value))
    {
        lit_writer_writestring(wr, lit_as_bool(value) ? "true" : "false");
    }
    else if(IS_NULL(value))
    {
        lit_writer_writestring(wr, "null");
    }
    else if(IS_NUMBER(value))
    {
        lit_writer_writeformat(wr, "%g", lit_value_to_number(value));
    }
    else if(IS_OBJECT(value))
    {
        print_object(state, wr, value);
    }
}


const char* lit_get_value_type(LitValue value)
{
    if(IS_BOOL(value))
    {
        return "bool";
    }
    else if(IS_NULL(value))
    {
        return "null";
    }
    else if(IS_NUMBER(value))
    {
        return "number";
    }
    else if(IS_OBJECT(value))
    {
        return lit_object_type_names[OBJECT_TYPE(value)];
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
    lit_as_string(vm_readconstant(current_chunk))

#define vm_readstringlong(current_chunk, ip) \
    lit_as_string(vm_readconstantlong(current_chunk, ip))


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
    upvalues = frame->closure == NULL ? NULL : frame->closure->upvalues;

#define vm_writeframe(frame, ip) \
    frame->ip = ip;

#define vm_returnerror() \
    vm_popgc(state); \
    return (LitInterpretResult){ LITRESULT_RUNTIME_ERROR, NULL_VALUE };

#define vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues) \
    vm_writeframe(frame, ip); \
    fiber = vm->fiber; \
    if(fiber == NULL) \
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
        vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues);  \
        continue; \
    } \
    else \
    { \
        vm_returnerror(); \
    }

#define vm_invoke_from_class_advanced(zklass, method_name, arg_count, error, stat, ignoring, callee) \
    LitValue mthval; \
    if((IS_INSTANCE(callee) && (lit_table_get(&AS_INSTANCE(callee)->fields, method_name, &mthval))) \
       || lit_table_get(&zklass->stat, method_name, &mthval)) \
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
    LitClass* klass = lit_get_class_for(state, instance); \
    if(klass == NULL) \
    { \
        vm_rterror("invokemethod: only instances and classes have methods"); \
    } \
    vm_writeframe(frame, ip); \
    vm_invoke_from_class_advanced(klass, CONST_STRING(state, method_name), arg_count, true, methods, false, instance); \
    vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues)

#define vm_binaryop(type, op, op_string) \
    LitValue a = vm_peek(fiber, 1); \
    LitValue b = vm_peek(fiber, 0); \
    if(IS_NUMBER(a)) \
    { \
        if(!IS_NUMBER(b)) \
        { \
            if(!IS_NULL(b)) \
            { \
                vm_rterrorvarg("Attempt to use op %s with a number and a %s", op_string, lit_get_value_type(b)); \
            } \
        } \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = (type(lit_value_to_number(a) op lit_value_to_number(b))); \
        continue; \
    } \
    if(IS_NULL(a)) \
    { \
    /* vm_rterrorvarg("Attempt to use op %s on a null value", op_string); */ \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = TRUE_VALUE; \
    } \
    else \
    { \
        vm_invokemethod(a, op_string, 1); \
    }

#define vm_bitwiseop(op, op_string) \
    LitValue a = vm_peek(fiber, 1); \
    LitValue b = vm_peek(fiber, 0); \
    if(!IS_NUMBER(a) || !IS_NUMBER(b)) \
    { \
        vm_rterrorvarg("Operands of bitwise op %s must be two numbers, got %s and %s", op_string, \
                           lit_get_value_type(a), lit_get_value_type(b)); \
    } \
    vm_drop(fiber); \
    *(fiber->stack_top - 1) = (lit_number_to_value((int)lit_value_to_number(a) op(int) lit_value_to_number(b)));

#define vm_invokeoperation(ignoring) \
    uint8_t arg_count = vm_readbyte(ip); \
    LitString* method_name = vm_readstringlong(current_chunk, ip); \
    LitValue receiver = vm_peek(fiber, arg_count); \
    if(IS_NULL(receiver)) \
    { \
        vm_rterror("Attempt to index a null value"); \
    } \
    vm_writeframe(frame, ip); \
    if(IS_CLASS(receiver)) \
    { \
        vm_invoke_from_class_advanced(AS_CLASS(receiver), method_name, arg_count, true, static_fields, ignoring, receiver); \
        continue; \
    } \
    else if(IS_INSTANCE(receiver)) \
    { \
        LitInstance* instance = AS_INSTANCE(receiver); \
        LitValue value; \
        if(lit_table_get(&instance->fields, method_name, &value)) \
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
        LitClass* type = lit_get_class_for(state, receiver); \
        if(type == NULL) \
        { \
            vm_rterror("invokeoperation: only instances and classes have methods"); \
        } \
        vm_invoke_from_class_advanced(type, method_name, arg_count, true, methods, ignoring, receiver); \
    }

static jmp_buf jump_buffer;

static void reset_stack(LitVM* vm)
{
    if(vm->fiber != NULL)
    {
        vm->fiber->stack_top = vm->fiber->stack;
    }
}

static void reset_vm(LitState* state, LitVM* vm)
{
    vm->state = state;
    vm->objects = NULL;
    vm->fiber = NULL;
    vm->gray_stack = NULL;
    vm->gray_count = 0;
    vm->gray_capacity = 0;
    vm->strings.init(state);
    vm->globals = NULL;
    vm->modules = NULL;
}

void lit_init_vm(LitState* state, LitVM* vm)
{
    reset_vm(state, vm);
    vm->globals = lit_create_map(state);
    vm->modules = lit_create_map(state);
}

void lit_free_vm(LitVM* vm)
{
    vm->strings.release(vm->state);
    lit_free_objects(vm->state, vm->objects);
    reset_vm(vm->state, vm);
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
    lit_writer_writeformat(wr, "        | %s", COLOR_GREEN);
    for(slot = fiber->stack; slot < top; slot++)
    {
        lit_writer_writeformat(wr, "[ ");
        lit_print_value(vm->state, wr, *slot);
        lit_writer_writeformat(wr, " ]");
    }
    lit_writer_writeformat(wr, "%s", COLOR_RESET);
    for(slot = top; slot < fiber->stack_top; slot++)
    {
        lit_writer_writeformat(wr, "[ ");
        lit_print_value(vm->state, wr, *slot);
        lit_writer_writeformat(wr, " ]");
    }
    lit_writer_writeformat(wr, "\n");
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
    error = OBJECT_VALUE(error_string);
    fiber = vm->fiber;
    while(fiber != NULL)
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
        fiber->parent = NULL;
        fiber = caller;
    }
    fiber = vm->fiber;
    fiber->abort = true;
    fiber->error = error;
    if(fiber->parent != NULL)
    {
        fiber->parent->abort = true;
    }
    // Maan, formatting c strings is hard...
    count = (int)fiber->frame_count - 1;
    length = snprintf(NULL, 0, "%s%s\n", COLOR_RED, error_string->chars);
    for(i = count; i >= 0; i--)
    {
        frame = &fiber->frames[i];
        function = frame->function;
        chunk = &function->chunk;
        name = function->name == NULL ? "unknown" : function->name->chars;

        if(chunk->has_line_info)
        {
            length += snprintf(NULL, 0, "[line %d] in %s()\n", (int)lit_chunk_get_line(chunk, frame->ip - chunk->code - 1), name);
        }
        else
        {
            length += snprintf(NULL, 0, "\tin %s()\n", name);
        }
    }
    length += snprintf(NULL, 0, "%s", COLOR_RESET);
    buffer = (char*)malloc(length + 1);
    buffer[length] = '\0';
    start = buffer + sprintf(buffer, "%s%s\n", COLOR_RED, error_string->chars);
    for(i = count; i >= 0; i--)
    {
        frame = &fiber->frames[i];
        function = frame->function;
        chunk = &function->chunk;
        name = function->name == NULL ? "unknown" : function->name->chars;
        if(chunk->has_line_info)
        {
            start += sprintf(start, "[line %d] in %s()\n", (int)lit_chunk_get_line(chunk, frame->ip - chunk->code - 1), name);
        }
        else
        {
            start += sprintf(start, "\tin %s()\n", name);
        }
    }
    start += sprintf(start, "%s", COLOR_RESET);
    lit_error(vm->state, RUNTIME_ERROR, buffer);
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
    buffer_size = vsnprintf(NULL, 0, format, args_copy) + 1;
    va_end(args_copy);
    buffer = (char*)malloc(buffer_size+1);
    vsnprintf(buffer, buffer_size, format, args);
    return lit_handle_runtime_error(vm, lit_string_take(vm->state, buffer, buffer_size, false));
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
                lit_push(vm, NULL_VALUE);
            }
            if(vararg)
            {
                lit_push(vm, OBJECT_VALUE(lit_create_array(vm->state)));
            }
        }
        else if(function->vararg)
        {
            array = lit_create_array(vm->state);
            vararg_count = arg_count - function_arg_count + 1;
            lit_push_root(vm->state, (LitObject*)array);
            array->values.reserve(vararg_count, NULL_VALUE);
            lit_pop_root(vm->state);
            for(i = 0; i < vararg_count; i++)
            {
                array->values.m_values[i] = vm->fiber->stack_top[(int)i - (int)vararg_count];
            }
            vm->fiber->stack_top -= vararg_count;
            lit_push(vm, OBJECT_VALUE(array));
        }
        else
        {
            vm->fiber->stack_top -= (arg_count - function_arg_count);
        }
    }
    else if(function->vararg)
    {
        array = lit_create_array(vm->state);
        vararg_count = arg_count - function_arg_count + 1;
        lit_push_root(vm->state, (LitObject*)array);
        array->values.push(*(fiber->stack_top - 1));
        *(fiber->stack_top - 1) = OBJECT_VALUE(array);
        lit_pop_root(vm->state);
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
    if(IS_OBJECT(callee))
    {
        if(lit_set_native_exit_jump())
        {
            return true;
        }
        switch(OBJECT_TYPE(callee))
        {
            case LITTYPE_FUNCTION:
                {
                    return call(vm, AS_FUNCTION(callee), NULL, arg_count);
                }
                break;
            case LITTYPE_CLOSURE:
                {
                    closure = AS_CLOSURE(callee);
                    return call(vm, closure->function, closure, arg_count);
                }
                break;
            case LITTYPE_NATIVE_FUNCTION:
                {
                    vm_pushgc(vm->state, false)
                    result = AS_NATIVE_FUNCTION(callee)->function(vm, arg_count, vm->fiber->stack_top - arg_count);
                    vm->fiber->stack_top -= arg_count + 1;
                    lit_push(vm, result);
                    vm_popgc(vm->state);
                    return false;
                }
                break;
            case LITTYPE_NATIVE_PRIMITIVE:
                {
                    vm_pushgc(vm->state, false)
                    fiber = vm->fiber;
                    bres = AS_NATIVE_PRIMITIVE(callee)->function(vm, arg_count, fiber->stack_top - arg_count);
                    if(bres)
                    {
                        fiber->stack_top -= arg_count;
                    }
                    vm_popgc(vm->state);
                    return bres;
                }
                break;
            case LITTYPE_NATIVE_METHOD:
                {
                    vm_pushgc(vm->state, false);
                    mthobj = AS_NATIVE_METHOD(callee);
                    fiber = vm->fiber;
                    result = mthobj->method(vm, *(vm->fiber->stack_top - arg_count - 1), arg_count, vm->fiber->stack_top - arg_count);
                    vm->fiber->stack_top -= arg_count + 1;
                    //if(!IS_NULL(result))
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
            case LITTYPE_PRIMITIVE_METHOD:
                {
                    vm_pushgc(vm->state, false);
                    fiber = vm->fiber;
                    bres = AS_PRIMITIVE_METHOD(callee)->method(vm, *(fiber->stack_top - arg_count - 1), arg_count, fiber->stack_top - arg_count);
                    if(bres)
                    {
                        fiber->stack_top -= arg_count;
                    }
                    vm_popgc(vm->state);
                    return bres;
                }
                break;
            case LITTYPE_CLASS:
                {
                    klass = AS_CLASS(callee);
                    instance = lit_create_instance(vm->state, klass);
                    vm->fiber->stack_top[-arg_count - 1] = OBJECT_VALUE(instance);
                    if(klass->init_method != NULL)
                    {
                        return call_value(vm, OBJECT_VALUE(klass->init_method), arg_count);
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
            case LITTYPE_BOUND_METHOD:
                {
                    bound_method = AS_BOUND_METHOD(callee);
                    mthval = bound_method->method;
                    if(IS_NATIVE_METHOD(mthval))
                    {
                        vm_pushgc(vm->state, false);
                        result = AS_NATIVE_METHOD(mthval)->method(vm, bound_method->receiver, arg_count, vm->fiber->stack_top - arg_count);
                        vm->fiber->stack_top -= arg_count + 1;
                        lit_push(vm, result);
                        vm_popgc(vm->state);
                        return false;
                    }
                    else if(IS_PRIMITIVE_METHOD(mthval))
                    {
                        fiber = vm->fiber;
                        vm_pushgc(vm->state, false);
                        if(AS_PRIMITIVE_METHOD(mthval)->method(vm, bound_method->receiver, arg_count, fiber->stack_top - arg_count))
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
                        return call(vm, AS_FUNCTION(mthval), NULL, arg_count);
                    }
                }
                break;
            default:
                {
                }
                break;

        }
    }
    if(IS_NULL(callee))
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
    previous_upvalue = NULL;
    upvalue = state->vm->fiber->open_upvalues;
    while(upvalue != NULL && upvalue->location > local)
    {
        previous_upvalue = upvalue;
        upvalue = upvalue->next;
    }
    if(upvalue != NULL && upvalue->location == local)
    {
        return upvalue;
    }
    created_upvalue = lit_create_upvalue(state, local);
    created_upvalue->next = upvalue;
    if(previous_upvalue == NULL)
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
    while(fiber->open_upvalues != NULL && fiber->open_upvalues->location >= last)
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
    lit_push(vm, OBJECT_VALUE(module->main_function));
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
    LitValue object;
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
    upvalues = frame->closure == NULL ? NULL : frame->closure->upvalues;

    // Has to be inside of the function in order for goto to work
    #ifdef LIT_USE_COMPUTEDGOTO
        static void* dispatch_table[] =
        {
            #define OPCODE(name, effect) &&OP_##name,
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
                lit_disassemble_instruction(state, current_chunk, (size_t)(ip - current_chunk->code - 1), NULL);
                goto* dispatch_table[instruction];
            #else
                goto* dispatch_table[*ip++];
            #endif
        #else
            instruction = *ip++;
            #ifdef LIT_TRACE_EXECUTION
                lit_disassemble_instruction(state, current_chunk, (size_t)(ip - current_chunk->code - 1), NULL);
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
                    if(fiber->parent == NULL)
                    {
                        vm_drop(fiber);
                        state->allow_gc = was_allowed;
                        return (LitInterpretResult){ LITRESULT_OK, result };
                    }
                    arg_count = fiber->arg_count;
                    parent = fiber->parent;
                    fiber->parent = NULL;
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
                vm_push(fiber, TRUE_VALUE);
                continue;
            }
            op_case(FALSE)
            {
                vm_push(fiber, FALSE_VALUE);
                continue;
            }
            op_case(NULL)
            {
                vm_push(fiber, NULL_VALUE);
                continue;
            }
            op_case(ARRAY)
            {
                vm_push(fiber, OBJECT_VALUE(lit_create_array(state)));
                continue;
            }
            op_case(OBJECT)
            {
                // TODO: use object, or map for literal '{...}' constructs?
                // objects would be more general-purpose, but don't implement anything map-like.
                //vm_push(fiber, OBJECT_VALUE(lit_create_instance(state, state->objectvalue_class)));
                vm_push(fiber, OBJECT_VALUE(lit_create_map(state)));

                continue;
            }
            op_case(RANGE)
            {
                a = vm_pop(fiber);
                b = vm_pop(fiber);
                if(!IS_NUMBER(a) || !IS_NUMBER(b))
                {
                    vm_rterror("Range operands must be number");
                }
                vm_push(fiber, OBJECT_VALUE(lit_create_range(state, lit_value_to_number(a), lit_value_to_number(b))));
                continue;
            }
            op_case(NEGATE)
            {
                if(!IS_NUMBER(vm_peek(fiber, 0)))
                {
                    arg = vm_peek(fiber, 0);
                    // Don't even ask me why
                    // This doesn't kill our performance, since it's a error anyway
                    if(IS_STRING(arg) && strcmp(lit_as_cstring(arg), "muffin") == 0)
                    {
                        vm_rterror("Idk, can you negate a muffin?");
                    }
                    else
                    {
                        vm_rterror("Operand must be a number");
                    }
                }
                tmpval = lit_number_to_value(-lit_value_to_number(vm_pop(fiber)));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(NOT)
            {
                if(IS_INSTANCE(vm_peek(fiber, 0)))
                {
                    vm_writeframe(frame, ip);
                    vm_invoke_from_class(AS_INSTANCE(vm_peek(fiber, 0))->klass, CONST_STRING(state, "!"), 0, false, methods, false);
                    continue;
                }
                tmpval = BOOL_VALUE(lit_is_falsey(vm_pop(fiber)));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(BNOT)
            {
                if(!IS_NUMBER(vm_peek(fiber, 0)))
                {
                    vm_rterror("Operand must be a number");
                }
                tmpval = lit_number_to_value(~((int)lit_value_to_number(vm_pop(fiber))));
                vm_push(fiber, tmpval);
                continue;
            }
            op_case(ADD)
            {
                vm_binaryop(lit_number_to_value, +, "+");
                continue;
            }
            op_case(SUBTRACT)
            {
                vm_binaryop(lit_number_to_value, -, "-");
                continue;
            }
            op_case(MULTIPLY)
            {
                vm_binaryop(lit_number_to_value, *, "*");
                continue;
            }
            op_case(POWER)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(IS_NUMBER(a) && IS_NUMBER(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = (lit_number_to_value(pow(lit_value_to_number(a), lit_value_to_number(b))));
                    continue;
                }
                vm_invokemethod(a, "**", 1);
                continue;
            }
            op_case(DIVIDE)
            {
                vm_binaryop(lit_number_to_value, /, "/");
                continue;
            }
            op_case(FLOOR_DIVIDE)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(IS_NUMBER(a) && IS_NUMBER(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = (lit_number_to_value(floor(lit_value_to_number(a) / lit_value_to_number(b))));

                    continue;
                }

                vm_invokemethod(a, "#", 1);
                continue;
            }
            op_case(MOD)
            {
                a = vm_peek(fiber, 1);
                b = vm_peek(fiber, 0);
                if(IS_NUMBER(a) && IS_NUMBER(b))
                {
                    vm_drop(fiber);
                    *(fiber->stack_top - 1) = lit_number_to_value(fmod(lit_value_to_number(a), lit_value_to_number(b)));
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
                if(IS_INSTANCE(vm_peek(fiber, 1)))
                {
                    vm_writeframe(frame, ip);
                    fprintf(stderr, "OP_EQUAL: trying to invoke '==' ...\n");
                    vm_invoke_from_class(AS_INSTANCE(vm_peek(fiber, 1))->klass, CONST_STRING(state, "=="), 1, false, methods, false);
                    continue;
                }
                a = vm_pop(fiber);
                b = vm_pop(fiber);
                vm_push(fiber, BOOL_VALUE(a == b));
                */
                vm_binaryop(lit_number_to_value, ==, "==");
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
                lit_table_set(state, &vm->globals->values, name, vm_peek(fiber, 0));
                continue;
            }

            op_case(GET_GLOBAL)
            {
                name = vm_readstringlong(current_chunk, ip);
                if(!lit_table_get(&vm->globals->values, name, &setval))
                {
                    vm_push(fiber, NULL_VALUE);
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
                if(lit_is_falsey(vm_pop(fiber)))
                {
                    ip += offset;
                }
                continue;
            }
            op_case(JUMP_IF_NULL)
            {
                offset = vm_readshort(ip);
                if(IS_NULL(vm_peek(fiber, 0)))
                {
                    ip += offset;
                }
                continue;
            }
            op_case(JUMP_IF_NULL_POPPING)
            {
                offset = vm_readshort(ip);
                if(IS_NULL(vm_pop(fiber)))
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
                if(lit_is_falsey(vm_peek(fiber, 0)))
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
                if(lit_is_falsey(vm_peek(fiber, 0)))
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
                if(IS_NULL(vm_peek(fiber, 0)))
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
                function = AS_FUNCTION(vm_readconstantlong(current_chunk, ip));
                closure = lit_create_closure(state, function);
                vm_push(fiber, OBJECT_VALUE(closure));
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
                klassobj = lit_create_class(state, name);
                vm_push(fiber, OBJECT_VALUE(klassobj));
                klassobj->super = state->objectvalue_class;
                lit_table_add_all(state, &klassobj->super->methods, &klassobj->methods);
                lit_table_add_all(state, &klassobj->super->static_fields, &klassobj->static_fields);
                lit_table_set(state, &vm->globals->values, name, OBJECT_VALUE(klassobj));
                continue;
            }
            op_case(GET_FIELD)
            {
                object = vm_peek(fiber, 1);
                if(IS_NULL(object))
                {
                    vm_rterror("Attempt to index a null value");
                }
                name = lit_as_string(vm_peek(fiber, 0));
                if(IS_INSTANCE(object))
                {
                    instobj = AS_INSTANCE(object);

                    if(!lit_table_get(&instobj->fields, name, &getval))
                    {
                        if(lit_table_get(&instobj->klass->methods, name, &getval))
                        {
                            if(IS_FIELD(getval))
                            {
                                field = AS_FIELD(getval);
                                if(field->getter == NULL)
                                {
                                    vm_rterrorvarg("Class %s does not have a getter for the field %s",
                                                       instobj->klass->name->chars, name->chars);
                                }
                                vm_drop(fiber);
                                vm_writeframe(frame, ip);
                                vm_callvalue(OBJECT_VALUE(AS_FIELD(getval)->getter), 0);
                                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                                continue;
                            }
                            else
                            {
                                getval = OBJECT_VALUE(lit_create_bound_method(state, OBJECT_VALUE(instobj), getval));
                            }
                        }
                        else
                        {
                            getval = NULL_VALUE;
                        }
                    }
                }
                else if(IS_CLASS(object))
                {
                    klassobj = AS_CLASS(object);
                    if(lit_table_get(&klassobj->static_fields, name, &getval))
                    {
                        if(IS_NATIVE_METHOD(getval) || IS_PRIMITIVE_METHOD(getval))
                        {
                            getval = OBJECT_VALUE(lit_create_bound_method(state, OBJECT_VALUE(klassobj), getval));
                        }
                        else if(IS_FIELD(getval))
                        {
                            field = AS_FIELD(getval);
                            if(field->getter == NULL)
                            {
                                vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->chars,
                                                   name->chars);
                            }
                            vm_drop(fiber);
                            vm_writeframe(frame, ip);
                            vm_callvalue(OBJECT_VALUE(field->getter), 0);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                    }
                    else
                    {
                        getval = NULL_VALUE;
                    }
                }
                else
                {
                    klassobj = lit_get_class_for(state, object);
                    if(klassobj == NULL)
                    {
                        vm_rterror("GET_FIELD: only instances and classes have fields");
                    }
                    if(lit_table_get(&klassobj->methods, name, &getval))
                    {
                        if(IS_FIELD(getval))
                        {
                            field = AS_FIELD(getval);
                            if(field->getter == NULL)
                            {
                                vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->chars,
                                                   name->chars);
                            }
                            vm_drop(fiber);
                            vm_writeframe(frame, ip);
                            vm_callvalue(OBJECT_VALUE(AS_FIELD(getval)->getter), 0);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                        else if(IS_NATIVE_METHOD(getval) || IS_PRIMITIVE_METHOD(getval))
                        {
                            getval = OBJECT_VALUE(lit_create_bound_method(state, object, getval));
                        }
                    }
                    else
                    {
                        getval = NULL_VALUE;
                    }
                }
                vm_drop(fiber);// Pop field name
                fiber->stack_top[-1] = getval;
                continue;
            }
            op_case(SET_FIELD)
            {
                instval = vm_peek(fiber, 2);
                if(IS_NULL(instval))
                {
                    vm_rterror("Attempt to index a null value")
                }
                value = vm_peek(fiber, 1);
                field_name = lit_as_string(vm_peek(fiber, 0));
                if(IS_CLASS(instval))
                {
                    klassobj = AS_CLASS(instval);
                    if(lit_table_get(&klassobj->static_fields, field_name, &setter) && IS_FIELD(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == NULL)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->chars,
                                               field_name->chars);
                        }

                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(OBJECT_VALUE(field->setter), 1);
                        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                        continue;
                    }
                    if(IS_NULL(value))
                    {
                        lit_table_delete(&klassobj->static_fields, field_name);
                    }
                    else
                    {
                        lit_table_set(state, &klassobj->static_fields, field_name, value);
                    }
                    vm_dropn(fiber, 2);// Pop field name and the value
                    fiber->stack_top[-1] = value;
                }
                else if(IS_INSTANCE(instval))
                {
                    instobj = AS_INSTANCE(instval);
                    if(lit_table_get(&instobj->klass->methods, field_name, &setter) && IS_FIELD(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == NULL)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", instobj->klass->name->chars,
                                               field_name->chars);
                        }
                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(OBJECT_VALUE(field->setter), 1);
                        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                        continue;
                    }
                    if(IS_NULL(value))
                    {
                        lit_table_delete(&instobj->fields, field_name);
                    }
                    else
                    {
                        lit_table_set(state, &instobj->fields, field_name, value);
                    }
                    vm_dropn(fiber, 2);// Pop field name and the value
                    fiber->stack_top[-1] = value;
                }
                else
                {
                    klassobj = lit_get_class_for(state, instval);
                    if(klassobj == NULL)
                    {
                        vm_rterror("SET_FIELD: only instances and classes have fields");
                    }
                    if(lit_table_get(&klassobj->methods, field_name, &setter) && IS_FIELD(setter))
                    {
                        field = AS_FIELD(setter);
                        if(field->setter == NULL)
                        {
                            vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->chars,
                                               field_name->chars);
                        }
                        vm_dropn(fiber, 2);
                        vm_push(fiber, value);
                        vm_writeframe(frame, ip);
                        vm_callvalue(OBJECT_VALUE(field->setter), 1);
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
                values = &AS_ARRAY(vm_peek(fiber, 1))->values;
                arindex = values->m_count;
                values->reserve(arindex + 1, NULL_VALUE);
                values->m_values[arindex] = vm_peek(fiber, 0);
                vm_drop(fiber);
                continue;
            }
            op_case(PUSH_OBJECT_FIELD)
            {
                operand = vm_peek(fiber, 2);
                if(IS_MAP(operand))
                {
                    lit_table_set(state, &AS_MAP(operand)->values, lit_as_string(vm_peek(fiber, 1)), vm_peek(fiber, 0));
                }
                else if(IS_INSTANCE(operand))
                {
                    lit_table_set(state, &AS_INSTANCE(operand)->fields, lit_as_string(vm_peek(fiber, 1)), vm_peek(fiber, 0));
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
                lit_table_set(state, &AS_CLASS(vm_peek(fiber, 1))->static_fields, vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
                vm_drop(fiber);
                continue;
            }
            op_case(METHOD)
            {
                klassobj = AS_CLASS(vm_peek(fiber, 1));
                name = vm_readstringlong(current_chunk, ip);
                if((klassobj->init_method == NULL || (klassobj->super != NULL && klassobj->init_method == ((LitClass*)klassobj->super)->init_method))
                   && lit_string_length(name) == 11 && memcmp(name->chars, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1) == 0)
                {
                    klassobj->init_method = lit_as_object(vm_peek(fiber, 0));
                }
                lit_table_set(state, &klassobj->methods, name, vm_peek(fiber, 0));
                vm_drop(fiber);
                continue;
            }
            op_case(DEFINE_FIELD)
            {
                lit_table_set(state, &AS_CLASS(vm_peek(fiber, 1))->methods, vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
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
                if(lit_table_get(&klassobj->methods, method_name, &value))
                {
                    value = OBJECT_VALUE(lit_create_bound_method(state, instval, value));
                }
                else
                {
                    value = NULL_VALUE;
                }
                vm_push(fiber, value);
                continue;
            }
            op_case(INHERIT)
            {
                super = vm_peek(fiber, 1);
                if(!IS_CLASS(super))
                {
                    vm_rterror("Superclass must be a class");
                }
                klassobj = AS_CLASS(vm_peek(fiber, 0));
                super_klass = AS_CLASS(super);
                klassobj->super = super_klass;
                klassobj->init_method = super_klass->init_method;
                lit_table_add_all(state, &super_klass->methods, &klassobj->methods);
                lit_table_add_all(state, &klassobj->super->static_fields, &klassobj->static_fields);
                continue;
            }
            op_case(IS)
            {
                instval = vm_peek(fiber, 1);
                if(IS_NULL(instval))
                {
                    vm_dropn(fiber, 2);
                    vm_push(fiber, FALSE_VALUE);

                    continue;
                }
                instance_klass = lit_get_class_for(state, instval);
                klassval = vm_peek(fiber, 0);
                if(instance_klass == NULL || !IS_CLASS(klassval))
                {
                    vm_rterror("operands must be an instance and a class");
                }            
                type = AS_CLASS(klassval);
                found = false;
                while(instance_klass != NULL)
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
                if(!IS_ARRAY(slot))
                {
                    continue;
                }
                values = &AS_ARRAY(slot)->values;
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
                if(lit_table_get_slot(&vm->globals->values, name, &pval))
                {
                    vm_push(fiber, OBJECT_VALUE(lit_create_reference(state, pval)));
                }
                else
                {
                    vm_rterror("Attempt to reference a null value");
                }
                continue;
            }
            op_case(REFERENCE_PRIVATE)
            {
                vm_push(fiber, OBJECT_VALUE(lit_create_reference(state, &privates[vm_readshort(ip)])));
                continue;
            }
            op_case(REFERENCE_LOCAL)
            {
                vm_push(fiber, OBJECT_VALUE(lit_create_reference(state, &slots[vm_readshort(ip)])));
                continue;
            }
            op_case(REFERENCE_UPVALUE)
            {
                vm_push(fiber, OBJECT_VALUE(lit_create_reference(state, upvalues[vm_readbyte(ip)]->location)));
                continue;
            }
            op_case(REFERENCE_FIELD)
            {
                object = vm_peek(fiber, 1);
                if(IS_NULL(object))
                {
                    vm_rterror("Attempt to index a null value");
                }
                name = lit_as_string(vm_peek(fiber, 0));
                if(IS_INSTANCE(object))
                {
                    if(!lit_table_get_slot(&AS_INSTANCE(object)->fields, name, &pval))
                    {
                        vm_rterror("Attempt to reference a null value");
                    }
                }
                else
                {
                    lit_print_value(state, &state->debugwriter, object);
                    printf("\n");
                    vm_rterror("You can only reference fields of real instances");
                }
                vm_drop(fiber);// Pop field name
                fiber->stack_top[-1] = OBJECT_VALUE(lit_create_reference(state, pval));
                continue;
            }
            op_case(SET_REFERENCE)
            {
                reference = vm_pop(fiber);
                if(!IS_REFERENCE(reference))
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



int lit_array_indexof(LitArray* array, LitValue value)
{
    size_t i;
    for(i = 0; i < array->values.m_count; i++)
    {
        if(array->values.m_values[i] == value)
        {
            return (int)i;
        }
    }
    return -1;
}


LitValue lit_array_removeat(LitArray* array, size_t index)
{
    size_t i;
    size_t count;
    LitValue value;
    PCGenericArray<LitValue>* values;
    values = &array->values;
    count = values->m_count;
    if(index >= count)
    {
        return NULL_VALUE;
    }
    value = values->m_values[index];
    if(index == count - 1)
    {
        values->m_values[index] = NULL_VALUE;
    }
    else
    {
        for(i = index; i < values->m_count - 1; i++)
        {
            values->m_values[i] = values->m_values[i + 1];
        }
        values->m_values[count - 1] = NULL_VALUE;
    }
    values->m_count--;
    return value;
}

void lit_array_push(LitState* state, LitArray* array, LitValue val)
{
    array->values.push(val);
}

static LitValue objfn_array_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_create_array(vm->state));
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
    new_array = lit_create_array(vm->state);
    for(i = 0; i < length; i++)
    {
        new_array->values.push(array->values.m_values[from + i]);
    }
    return OBJECT_VALUE(new_array);
}

static LitValue objfn_array_slice(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int from;
    int to;
    from = lit_check_number(vm, argv, argc, 0);
    to = lit_check_number(vm, argv, argc, 1);
    return objfn_array_splice(vm, AS_ARRAY(instance), from, to);
}

static LitValue objfn_array_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitRange* range;
    PCGenericArray<LitValue>* values;
    if(argc == 2)
    {
        if(!IS_NUMBER(argv[0]))
        {
            lit_runtime_error_exiting(vm, "array index must be a number");
        }
        values = &AS_ARRAY(instance)->values;
        index = lit_value_to_number(argv[0]);
        if(index < 0)
        {
            index = fmax(0, values->m_count + index);
        }
        values->reserve(index + 1, NULL_VALUE);
        return values->m_values[index] = argv[1];
    }
    if(!IS_NUMBER(argv[0]))
    {
        if(IS_RANGE(argv[0]))
        {
            range = AS_RANGE(argv[0]);
            return objfn_array_splice(vm, AS_ARRAY(instance), (int)range->from, (int)range->to);
        }
        lit_runtime_error_exiting(vm, "array index must be a number");
        return NULL_VALUE;
    }
    values = &AS_ARRAY(instance)->values;
    index = lit_value_to_number(argv[0]);
    if(index < 0)
    {
        index = fmax(0, values->m_count + index);
    }
    if(values->m_capacity <= (size_t)index)
    {
        return NULL_VALUE;
    }
    return values->m_values[index];
}

bool lit_compare_values(LitState* state, const LitValue a, const LitValue b)
{
    LitInterpretResult inret;
    LitValue args[3];
    if(IS_INSTANCE(a))
    {
        args[0] = b;
        inret = lit_instance_call_method(state, a, CONST_STRING(state, "=="), args, 1);
        if(inret.type == LITRESULT_OK)
        {
            if(BOOL_VALUE(inret.result) == TRUE_VALUE)
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
    self = AS_ARRAY(instance);
    if(IS_ARRAY(argv[0]))
    {
        other = AS_ARRAY(argv[0]);
        if(self->values.m_count == other->values.m_count)
        {
            for(i=0; i<self->values.m_count; i++)
            {
                if(!lit_compare_values(vm->state, self->values.m_values[i], other->values.m_values[i]))
                {
                    return FALSE_VALUE;
                }
            }
            return TRUE_VALUE;
        }
        return FALSE_VALUE;
    }
    lit_runtime_error_exiting(vm, "can only compare array to another array or null");
    return FALSE_VALUE;
}


static LitValue objfn_array_add(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    for(i=0; i<argc; i++)
    {
        lit_array_push(vm->state, AS_ARRAY(instance), argv[i]);
    }
    return NULL_VALUE;
}


static LitValue objfn_array_insert(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int i;
    int index;
    LitValue value;
    PCGenericArray<LitValue>* values;
    LIT_ENSURE_ARGS(2);
    values = &AS_ARRAY(instance)->values;
    index = lit_check_number(vm, argv, argc, 0);

    if(index < 0)
    {
        index = fmax(0, values->m_count + index);
    }
    value = argv[1];
    if((int)values->m_count <= index)
    {
        values->reserve(index + 1, NULL_VALUE);
    }
    else
    {
        values->reserve(values->m_count + 1, NULL_VALUE);
        for(i = values->m_count - 1; i > index; i--)
        {
            values->m_values[i] = values->m_values[i - 1];
        }
    }
    values->m_values[index] = value;
    return NULL_VALUE;
}

static LitValue objfn_array_addall(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    LitArray* array;
    LitArray* toadd;
    LIT_ENSURE_ARGS(1);
    if(!IS_ARRAY(argv[0]))
    {
        lit_runtime_error_exiting(vm, "expected array as the argument");
    }
    array = AS_ARRAY(instance);
    toadd = AS_ARRAY(argv[0]);
    for(i = 0; i < toadd->values.m_count; i++)
    {
        lit_array_push(vm->state, array, toadd->values.m_values[i]);
    }
    return NULL_VALUE;
}

static LitValue objfn_array_indexof(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1)

        int index = lit_array_indexof(AS_ARRAY(instance), argv[0]);
    return index == -1 ? NULL_VALUE : lit_number_to_value(index);
}


static LitValue objfn_array_remove(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitArray* array;
    LIT_ENSURE_ARGS(1);
    array = AS_ARRAY(instance);
    index = lit_array_indexof(array, argv[0]);
    if(index != -1)
    {
        return lit_array_removeat(array, (size_t)index);
    }
    return NULL_VALUE;
}

static LitValue objfn_array_removeat(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    index = lit_check_number(vm, argv, argc, 0);
    if(index < 0)
    {
        return NULL_VALUE;
    }
    return lit_array_removeat(AS_ARRAY(instance), (size_t)index);
}

static LitValue objfn_array_contains(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    return BOOL_VALUE(lit_array_indexof(AS_ARRAY(instance), argv[0]) != -1);
}

static LitValue objfn_array_clear(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    AS_ARRAY(instance)->values.m_count = 0;
    return NULL_VALUE;
}

static LitValue objfn_array_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int number;
    LitArray* array;
    (void)vm;
    LIT_ENSURE_ARGS(1);
    array = AS_ARRAY(instance);
    number = 0;
    if(IS_NUMBER(argv[0]))
    {
        number = lit_value_to_number(argv[0]);
        if(number >= (int)array->values.m_count - 1)
        {
            return NULL_VALUE;
        }
        number++;
    }
    return array->values.m_count == 0 ? NULL_VALUE : lit_number_to_value(number);
}

static LitValue objfn_array_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    PCGenericArray<LitValue>* values;
    index = lit_check_number(vm, argv, argc, 0);
    values = &AS_ARRAY(instance)->values;
    if(values->m_count <= index)
    {
        return NULL_VALUE;
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
    joinee = NULL;
    length = 0;
    if(argc > 0)
    {
        joinee = lit_as_string(argv[0]);
    }
    values = &AS_ARRAY(instance)->values;
    //LitString* strings[values.m_count];
    strings = LIT_ALLOCATE(vm->state, LitString*, values->m_count+1);
    for(i = 0; i < values->m_count; i++)
    {
        string = lit_to_string(vm->state, values->m_values[i]);
        strings[i] = string;
        length += lit_string_length(string);
        if(joinee != NULL)
        {
            length += lit_string_length(joinee);
        }
    }
    jlen = 0;
    index = 0;
    chars = sdsempty();
    chars = sdsMakeRoomFor(chars, length + 1);
    if(joinee != NULL)
    {
        jlen = lit_string_length(joinee);
    }
    for(i = 0; i < values->m_count; i++)
    {
        string = strings[i];
        memcpy(chars + index, string->chars, lit_string_length(string));
        chars = sdscatlen(chars, string->chars, lit_string_length(string));
        index += lit_string_length(string);
        if(joinee != NULL)
        {
            
            //if((i+1) < values.m_count)
            {
                chars = sdscatlen(chars, joinee->chars, jlen);
            }
            index += jlen;
        }
    }
    LIT_FREE(vm->state, LitString*, strings);
    return OBJECT_VALUE(lit_string_take(vm->state, chars, length, true));
}

static LitValue objfn_array_sort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    PCGenericArray<LitValue>* values;
    values = &AS_ARRAY(instance)->values;
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
    values = &AS_ARRAY(instance)->values;
    array = lit_create_array(state);
    new_values = &array->values;
    new_values->reserve(values->m_count, NULL_VALUE);
    // lit_values_ensure_size sets the count to max of previous count (0 in this case) and new count, so we have to reset it
    new_values->m_count = 0;
    for(i = 0; i < values->m_count; i++)
    {
        new_values->push(values->m_values[i]);
    }
    return OBJECT_VALUE(array);
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
    self = AS_ARRAY(instance);
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
        if(IS_ARRAY(val) && (AS_ARRAY(val) == self))
        {
            stringified = lit_string_copy(state, recstring, strlen(recstring));
        }
        else
        {
            stringified = lit_to_string(state, val);
        }
        part = stringified;
        buffer = sdscatlen(buffer, part->chars, lit_string_length(part));
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
    // should be lit_string_take, but it doesn't get picked up by the GC for some reason
    //rt = lit_string_take(vm->state, buffer, olength);
    rt = lit_string_take(vm->state, buffer, olength, true);
    return OBJECT_VALUE(rt);
}

static LitValue objfn_array_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return lit_number_to_value(AS_ARRAY(instance)->values.m_count);
}

void lit_open_array_library(LitState* state)
{
    LIT_BEGIN_CLASS("Array");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, objfn_array_constructor);
        lit_bind_method(state, klass, "[]", objfn_array_subscript);
        lit_bind_method(state, klass, "==", objfn_array_compare);
        lit_bind_method(state, klass, "add", objfn_array_add);
        lit_bind_method(state, klass, "push", objfn_array_add);
        lit_bind_method(state, klass, "insert", objfn_array_insert);
        lit_bind_method(state, klass, "slice", objfn_array_slice);
        lit_bind_method(state, klass, "addAll", objfn_array_addall);
        lit_bind_method(state, klass, "remove", objfn_array_remove);
        lit_bind_method(state, klass, "removeAt", objfn_array_removeat);
        lit_bind_method(state, klass, "indexOf", objfn_array_indexof);
        lit_bind_method(state, klass, "contains", objfn_array_contains);
        lit_bind_method(state, klass, "clear", objfn_array_clear);
        lit_bind_method(state, klass, "iterator", objfn_array_iterator);
        lit_bind_method(state, klass, "iteratorValue", objfn_array_iteratorvalue);
        lit_bind_method(state, klass, "join", objfn_array_join);
        lit_bind_method(state, klass, "sort", objfn_array_sort);
        lit_bind_method(state, klass, "clone", objfn_array_clone);
        lit_bind_method(state, klass, "toString", objfn_array_tostring);
        LIT_BIND_GETTER("length", objfn_array_length);
        state->arrayvalue_class = klass;
    }
    LIT_END_CLASS();
}



static LitValue objfn_class_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_string_format(vm->state, "class @", OBJECT_VALUE(AS_CLASS(instance)->name)));
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
    index = argv[0] == NULL_VALUE ? -1 : lit_value_to_number(argv[0]);
    mthcap = (int)klass->methods.m_capacity;
    fields = index >= mthcap;
    value = util_table_iterator(fields ? &klass->static_fields : &klass->methods, fields ? index - mthcap : index);
    if(value == -1)
    {
        if(fields)
        {
            return NULL_VALUE;
        }
        index++;
        fields = true;
        value = util_table_iterator(&klass->static_fields, index - mthcap);
    }
    if(value == -1)
    {
        return NULL_VALUE;
    }
    return lit_number_to_value(fields ? value + mthcap : value);
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
    return util_table_iterator_key(fields ? &klass->static_fields : &klass->methods, fields ? index - mthcap : index);
}


static LitValue objfn_class_super(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* super;
    (void)vm;
    (void)argc;
    (void)argv;
    super = NULL;
    if(IS_INSTANCE(instance))
    {
        super = AS_INSTANCE(instance)->klass->super;
    }
    else
    {
        super = AS_CLASS(instance)->super;
    }
    if(super == NULL)
    {
        return NULL_VALUE;
    }
    return OBJECT_VALUE(super);
}

static LitValue objfn_class_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* klass;    
    LitValue value;
    (void)argc;
    klass = AS_CLASS(instance);
    if(argc == 2)
    {
        if(!IS_STRING(argv[0]))
        {
            lit_runtime_error_exiting(vm, "class index must be a string");
        }

        lit_table_set(vm->state, &klass->static_fields, lit_as_string(argv[0]), argv[1]);
        return argv[1];
    }
    if(!IS_STRING(argv[0]))
    {
        lit_runtime_error_exiting(vm, "class index must be a string");
    }
    if(lit_table_get(&klass->static_fields, lit_as_string(argv[0]), &value))
    {
        return value;
    }
    if(lit_table_get(&klass->methods, lit_as_string(argv[0]), &value))
    {
        return value;
    }
    return NULL_VALUE;
}

static LitValue objfn_class_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitClass* selfclass;
    LitClass* otherclass;
    (void)vm;
    (void)argc;
    if(IS_CLASS(argv[0]))
    {
        selfclass = AS_CLASS(instance);
        otherclass = AS_CLASS(argv[0]);
        if(lit_string_equal(vm->state, selfclass->name, otherclass->name))
        {
            if(selfclass == otherclass)
            {
                return TRUE_VALUE;
            }
        }
    }
    return FALSE_VALUE;
}

static LitValue objfn_class_name(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(AS_CLASS(instance)->name);
}

void lit_open_class_library(LitState* state)
{
    LIT_BEGIN_CLASS("Class");
    {
        lit_bind_method(state, klass, "[]", objfn_class_subscript);
        lit_bind_method(state, klass, "==", objfn_class_compare);
        lit_bind_method(state, klass, "toString", objfn_class_tostring);
        LIT_BIND_STATIC_METHOD("toString", objfn_class_tostring);
        LIT_BIND_STATIC_METHOD("iterator", objfn_class_iterator);
        LIT_BIND_STATIC_METHOD("iteratorValue", objfn_class_iteratorvalue);
        LIT_BIND_GETTER("super", objfn_class_super);
        LIT_BIND_STATIC_GETTER("super", objfn_class_super);
        LIT_BIND_STATIC_GETTER("name", objfn_class_name);
        state->classvalue_class = klass;
    }
    LIT_END_CLASS_IGNORING();
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
        !lit_is_falsey(r.result); \
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
            if(lit_is_falsey(rt.result))
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
            if(lit_is_falsey(rt.result))
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
        lit_push(vm, OBJECT_VALUE(frame->function));
        vararg = frame->function->vararg;
        objfn_function_arg_count = frame->function->arg_count;
        to = objfn_function_arg_count - (vararg ? 1 : 0);
        fiber->arg_count = objfn_function_arg_count;
        for(i = 0; i < to; i++)
        {
            lit_push(vm, i < (int)argc ? argv[i] : NULL_VALUE);
        }
        if(vararg)
        {
            array = lit_create_array(vm->state);
            lit_push(vm, OBJECT_VALUE(array));
            vararg_count = argc - objfn_function_arg_count + 1;
            if(vararg_count > 0)
            {
                array->values.reserve(vararg_count, NULL_VALUE);
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
    if(IS_NUMBER(a) && IS_NUMBER(b))
    {
        return lit_value_to_number(a) < lit_value_to_number(b);
    }
    argv[0] = b;
    return !lit_is_falsey(lit_find_and_call_method(state, a, CONST_STRING(state, "<"), argv, 1, false).result);
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
        lit_push(vm, OBJECT_VALUE(frame->function));
    }
    return true;
}

static bool compile_and_interpret(LitVM* vm, LitString* modname, char* source)
{
    LitModule* module;
    module = lit_compile_module(vm->state, modname, source);
    if(module == NULL)
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
                if(dir == NULL)
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
                            char dir_path[length + name_length - 2];
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
            char dir_name[length + 6];
            dir_name[length + 5] = '\0';
            memcpy(dir_name, modname, length);
            memcpy(dir_name + length, ".init", 5);
            if(util_attempt_to_require(vm, argv, argc, dir_name, ignore_previous, false))
            {
                return true;
            }
        }
    }
    else if(folders)
    {
        return false;
    }
    modname[length] = '.';
    LitString* name = lit_string_copy(vm->state, modnamedotted, length);
    if(!ignore_previous)
    {
        LitValue existing_module;
        if(lit_table_get(&vm->modules->values, name, &existing_module))
        {
            LitModule* loaded_module = AS_MODULE(existing_module);
            if(loaded_module->ran)
            {
                vm->fiber->stack_top -= argc;
                argv[-1] = AS_MODULE(existing_module)->return_value;
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
    if(source == NULL)
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
    lit_runtime_error_exiting(vm, "cannot create an instance of built-in type", AS_INSTANCE(instance)->klass->name);
    return NULL_VALUE;
}

static LitValue objfn_number_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_string_number_to_string(vm->state, lit_value_to_number(instance)));
}

static LitValue objfn_number_tochar(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    char ch;
    (void)argc;
    (void)argv;
    ch = lit_value_to_number(instance);
    return OBJECT_VALUE(lit_string_copy(vm->state, &ch, 1));
}

static LitValue objfn_bool_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool bv;
    (void)vm;
    (void)argc;
    bv = lit_as_bool(instance);
    if(IS_NULL(argv[0]))
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
    return lit_number_to_value((double)clock() / CLOCKS_PER_SEC);
}

static LitValue cfn_systemTime(LitVM* vm, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return lit_number_to_value(time(NULL));
}

static LitValue cfn_print(LitVM* vm, size_t argc, LitValue* argv)
{
    size_t i;
    size_t written = 0;
    LitString* sv;
    written = 0;
    if(argc == 0)
    {
        return lit_number_to_value(0);
    }
    for(i = 0; i < argc; i++)
    {
        sv = lit_to_string(vm->state, argv[i]);
        written += fwrite(sv->chars, sizeof(char), lit_string_length(sv), stdout);
    }
    return lit_number_to_value(written);
}

static LitValue cfn_println(LitVM* vm, size_t argc, LitValue* argv)
{
    size_t i;
    if(argc == 0)
    {
        return NULL_VALUE;
    }
    for(i = 0; i < argc; i++)
    {
        lit_printf(vm->state, "%s", lit_to_string(vm->state, argv[i])->chars);
    }
    lit_printf(vm->state, "\n");
    return NULL_VALUE;
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
    ignore_previous = argc > 1 && IS_BOOL(argv[1]) && lit_as_bool(argv[1]);
    // First check, if a file with this name exists in the local path
    if(util_attempt_to_require(vm, argv, argc, name->chars, ignore_previous, false))
    {
        return should_update_locals;
    }
    // If not, we join the path of the current module to it (the path goes all the way from the root)
    modname = vm->fiber->module->name;
    // We need to get rid of the module name (test.folder.module -> test.folder)
    index = strrchr(modname->chars, '.');
    if(index != NULL)
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
        LIT_BEGIN_CLASS("Number");
        {
            LIT_INHERIT_CLASS(state->objectvalue_class);
            lit_bind_constructor(state, klass, util_invalid_constructor);
            lit_bind_method(state, klass, "toString", objfn_number_tostring);
            lit_bind_method(state, klass, "toChar", objfn_number_tochar);
            LIT_BIND_GETTER("chr", objfn_number_tochar);
            state->numbervalue_class = klass;
        }
        LIT_END_CLASS();
    }
    {
        LIT_BEGIN_CLASS("Bool");
        {
            LIT_INHERIT_CLASS(state->objectvalue_class);
            lit_bind_constructor(state, klass, util_invalid_constructor);
            lit_bind_method(state, klass, "==", objfn_bool_compare);
            lit_bind_method(state, klass, "toString", objfn_bool_tostring);
            state->boolvalue_class = klass;
        }
        LIT_END_CLASS();
    }
    {
        lit_define_native(state, "time", cfn_time);
        lit_define_native(state, "systemTime", cfn_systemTime);
        lit_define_native(state, "print", cfn_print);
        lit_define_native(state, "println", cfn_println);
        //lit_define_native_primitive(state, "require", cfn_require);
        lit_define_native_primitive(state, "eval", cfn_eval);
        lit_set_global(state, CONST_STRING(state, "globals"), OBJECT_VALUE(state->vm->globals));
    }
}


static LitValue objfn_fiber_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    if(argc < 1 || !IS_FUNCTION(argv[0]))
    {
        lit_runtime_error_exiting(vm, "Fiber constructor expects a function as its argument");
    }

    LitFunction* function = AS_FUNCTION(argv[0]);
    LitModule* module = vm->fiber->module;
    LitFiber* fiber = lit_create_fiber(vm->state, module, function);

    fiber->parent = vm->fiber;

    return OBJECT_VALUE(fiber);
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
    return OBJECT_VALUE(vm->fiber);
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
    if(vm->fiber->parent == NULL)
    {
        lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was yielded") :
        lit_to_string(vm->state, argv[0]));
        return true;
    }

    LitFiber* fiber = vm->fiber;

    vm->fiber = vm->fiber->parent;
    vm->fiber->stack_top -= fiber->arg_count;
    vm->fiber->stack_top[-1] = argc == 0 ? NULL_VALUE : OBJECT_VALUE(lit_to_string(vm->state, argv[0]));

    argv[-1] = NULL_VALUE;
    return true;
}


static bool objfn_fiber_yeet(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    if(vm->fiber->parent == NULL)
    {
        lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was yeeted") :
        lit_to_string(vm->state, argv[0]));
        return true;
    }

    LitFiber* fiber = vm->fiber;

    vm->fiber = vm->fiber->parent;
    vm->fiber->stack_top -= fiber->arg_count;
    vm->fiber->stack_top[-1] = argc == 0 ? NULL_VALUE : OBJECT_VALUE(lit_to_string(vm->state, argv[0]));

    argv[-1] = NULL_VALUE;
    return true;
}


static bool objfn_fiber_abort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    lit_handle_runtime_error(vm, argc == 0 ? CONST_STRING(vm->state, "Fiber was aborted") :
    lit_to_string(vm->state, argv[0]));
    argv[-1] = NULL_VALUE;
    return true;
}

static LitValue objfn_fiber_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_string_format(vm->state, "fiber@%p", &instance));

}

void lit_open_fiber_library(LitState* state)
{
    LIT_BEGIN_CLASS("Fiber");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, objfn_fiber_constructor);
        lit_bind_method(state, klass, "toString", objfn_fiber_tostring);
        lit_bind_primitive(state, klass, "run", objfn_fiber_run);
        lit_bind_primitive(state, klass, "try", objfn_fiber_try);
        LIT_BIND_GETTER("done", objfn_fiber_done);
        LIT_BIND_GETTER("error", objfn_fiber_error);
        LIT_BIND_STATIC_PRIMITIVE("yield", objfn_fiber_yield);
        LIT_BIND_STATIC_PRIMITIVE("yeet", objfn_fiber_yeet);
        LIT_BIND_STATIC_PRIMITIVE("abort", objfn_fiber_abort);
        LIT_BIND_STATIC_GETTER("current", objfn_fiber_current);
        state->fibervalue_class = klass;
    }
    LIT_END_CLASS();
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

typedef struct LitDirReader LitDirReader;
typedef struct LitDirItem LitDirItem;

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
        if((rd->handle = opendir(path)) == NULL)
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
        if((ent = readdir((DIR*)(rd->handle))) == NULL)
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



typedef struct LitFileData LitFileData;
typedef struct LitStdioHandle LitStdioHandle;

struct LitFileData
{
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

typedef void(*CleanupFunc)(LitState*, LitUserdata*, bool);

static void* LIT_INSERT_DATA(LitVM* vm, LitValue instance, size_t typsz, CleanupFunc cleanup)
{
    LitUserdata* userdata = lit_create_userdata(vm->state, typsz, false);
    userdata->cleanup_fn = cleanup;
    lit_table_set(vm->state, &AS_INSTANCE(instance)->fields, CONST_STRING(vm->state, "_data"), OBJECT_VALUE(userdata));
    return userdata->data;
}

static void* LIT_EXTRACT_DATA(LitVM* vm, LitValue instance)
{
    LitValue _d;
    if(!lit_table_get(&AS_INSTANCE(instance)->fields, CONST_STRING(vm->state, "_data"), &_d))
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
    if(data != NULL)
    {
        fd = ((LitFileData*)data->data);
        if(fd != NULL)
        {
            if((fd->handle != NULL) && (fd->isopen == true))
            {
                fclose(fd->handle);
                fd->handle = NULL;
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
        if(IS_USERDATA(argv[0]))
        {
            hstd = (LitStdioHandle*)(AS_USERDATA(argv[0])->data);
            hnd = hstd->handle;
            //fprintf(stderr, "FILE: hnd=%p name=%s\n", hstd->handle, hstd->name);
            data = (LitFileData*)LIT_INSERT_DATA(vm, instance, sizeof(LitFileData), NULL);
            data->path = NULL;
            data->handle = hnd;
            data->isopen = true;
        }
        else
        {
            path = lit_check_string(vm, argv, argc, 0);
            mode = lit_get_string(vm, argv, argc, 1, "r");
            hnd = fopen(path, mode);
            if(hnd == NULL)
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
        return NULL_VALUE;
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
    data->handle = NULL;
    data->isopen = false;
    return NULL_VALUE;
}

static LitValue objmethod_file_exists(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    char* file_name;
    file_name = NULL;
    if(IS_INSTANCE(instance))
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
    rt = fwrite(value->chars, lit_string_length(value), 1, ((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle);
    return lit_number_to_value(rt);
}

static LitValue objmethod_file_writebyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint8_t rt;
    uint8_t byte;
    byte = (uint8_t)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, byte);
    return lit_number_to_value(rt);
}

static LitValue objmethod_file_writeshort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint16_t rt;
    uint16_t shrt;
    shrt = (uint16_t)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint16_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, shrt);
    return lit_number_to_value(rt);
}

static LitValue objmethod_file_writenumber(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint32_t rt;
    float num;
    num = (float)lit_check_number(vm, argv, argc, 0);
    rt = lit_write_uint32_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, num);
    return lit_number_to_value(rt);
}

static LitValue objmethod_file_writebool(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    bool value;
    uint8_t rt;
    value = lit_check_bool(vm, argv, argc, 0);
    rt = lit_write_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle, (uint8_t)value ? '1' : '0');
    return lit_number_to_value(rt);
}

static LitValue objmethod_file_writestring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* string;
    LitFileData* data;
    if(lit_check_string(vm, argv, argc, 0) == NULL)
    {
        return NULL_VALUE;
    }
    string = lit_as_string(argv[0]);
    data = (LitFileData*)LIT_EXTRACT_DATA(vm, instance);
    lit_write_string(data->handle, string);
    return NULL_VALUE;
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
        result = lit_string_alloc_empty(vm->state, 0, false);
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
        result = lit_string_alloc_empty(vm->state, length, false);
        actuallength = fread(result->chars, sizeof(char), length, data->handle);
        /*
        * after reading, THIS actually sets the correct length.
        * before that, it would be 0.
        */
        sdsIncrLen(result->chars, actuallength);
    }
    result->hash = lit_hash_string(result->chars, actuallength);
    lit_register_string(vm->state, result);
    return OBJECT_VALUE(result);
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
        return NULL_VALUE;
    }
    return OBJECT_VALUE(lit_string_take(vm->state, line, strlen(line) - 1, false));
}

static LitValue objmethod_file_readbyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return lit_number_to_value(lit_read_uint8_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
}

static LitValue objmethod_file_readshort(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return lit_number_to_value(lit_read_uint16_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
}

static LitValue objmethod_file_readnumber(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)instance;
    (void)argc;
    (void)argv;
    return lit_number_to_value(lit_read_uint32_t(((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
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

    return string == NULL ? NULL_VALUE : OBJECT_VALUE(string);
}

static LitValue objmethod_file_getlastmodified(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    struct stat buffer;
    char* file_name = NULL;
    (void)vm;
    (void)argc;
    (void)argv;
    if(IS_INSTANCE(instance))
    {
        file_name = ((LitFileData*)LIT_EXTRACT_DATA(vm, instance))->path;
    }
    else
    {
        file_name = (char*)lit_check_string(vm, argv, argc, 0);
    }

    if(stat(file_name, &buffer) != 0)
    {
        return lit_number_to_value(0);
    }
    #if defined(__unix__) || defined(__linux__)
    return lit_number_to_value(buffer.st_mtim.tv_sec);
    #else
        return lit_number_to_value(0);
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
    array = lit_create_array(state);
    #if defined(__unix__) || defined(__linux__)
    {
        struct dirent* ep;
        DIR* dir = opendir(lit_check_string(vm, argv, argc, 0));
        if(dir == NULL)
        {
            return OBJECT_VALUE(array);
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
    return OBJECT_VALUE(array);
}

static LitValue objfunction_directory_listdirs(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitArray* array;
    LitState* state;
    LitDirReader rd;
    LitDirItem ent;
    (void)instance;
    state = vm->state;
    array = lit_create_array(state);

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
    return OBJECT_VALUE(array);
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
    state->vm->fiber = lit_create_fiber(state, state->last_module, NULL);
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
        args[0] = OBJECT_VALUE(userhnd);
        args[1] = OBJECT_VALUE(descname);
        res = lit_call(state, fileval, args, 2, false);
        //fprintf(stderr, "make_handle(%s, hnd=%p): res.type=%d, res.result=%s\n", name, hnd, res.type, lit_get_value_type(res.result));
        lit_set_global(state, varname, res.result);
    }
    state->vm->fiber = oldfiber;
}

static void make_stdhandles(LitState* state)
{
    LitValue fileval;
    fileval = lit_get_global(state, CONST_STRING(state, "File"));
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
        LIT_BEGIN_CLASS("File");
        {
            LIT_BIND_STATIC_METHOD("exists", objmethod_file_exists);
            LIT_BIND_STATIC_METHOD("getLastModified", objmethod_file_getlastmodified);
            lit_bind_constructor(state, klass, objmethod_file_constructor);
            lit_bind_method(state, klass, "close", objmethod_file_close);
            lit_bind_method(state, klass, "write", objmethod_file_write);
            lit_bind_method(state, klass, "writeByte", objmethod_file_writebyte);
            lit_bind_method(state, klass, "writeShort", objmethod_file_writeshort);
            lit_bind_method(state, klass, "writeNumber", objmethod_file_writenumber);
            lit_bind_method(state, klass, "writeBool", objmethod_file_writebool);
            lit_bind_method(state, klass, "writeString", objmethod_file_writestring);
            lit_bind_method(state, klass, "readAll", objmethod_file_readall);
            lit_bind_method(state, klass, "readLine", objmethod_file_readline);
            lit_bind_method(state, klass, "readByte", objmethod_file_readbyte);
            lit_bind_method(state, klass, "readShort", objmethod_file_readshort);
            lit_bind_method(state, klass, "readNumber", objmethod_file_readnumber);
            lit_bind_method(state, klass, "readBool", objmethod_file_readbool);
            lit_bind_method(state, klass, "readString", objmethod_file_readstring);
            lit_bind_method(state, klass, "getLastModified", objmethod_file_getlastmodified);
            LIT_BIND_GETTER("exists", objmethod_file_exists);
        }
        LIT_END_CLASS();
    }
    {
        LIT_BEGIN_CLASS("Directory");
        {
            LIT_BIND_STATIC_METHOD("exists", objfunction_directory_exists);
            LIT_BIND_STATIC_METHOD("listFiles", objfunction_directory_listfiles);
            LIT_BIND_STATIC_METHOD("listDirectories", objfunction_directory_listdirs);
        }
        LIT_END_CLASS();
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
    LIT_BEGIN_CLASS("Function");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, util_invalid_constructor);
        lit_bind_method(state, klass, "toString", objfn_function_tostring);
        LIT_BIND_GETTER("name", objfn_function_name);
        state->functionvalue_class = klass;
    }
    LIT_END_CLASS();
}


static LitValue objfn_gc_memory_used(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    return lit_number_to_value(vm->state->bytes_allocated);
}

static LitValue objfn_gc_next_round(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    return lit_number_to_value(vm->state->next_gc);
}

static LitValue objfn_gc_trigger(LitVM* vm, LitValue instance, size_t arg_count, LitValue* args)
{
    (void)instance;
    (void)arg_count;
    (void)args;
    int64_t collected;
    vm->state->allow_gc = true;
    collected = lit_collect_garbage(vm);
    vm->state->allow_gc = false;

    return lit_number_to_value(collected);
}

void lit_open_gc_library(LitState* state)
{
    LIT_BEGIN_CLASS("GC");
    {
        LIT_BIND_STATIC_GETTER("memoryUsed", objfn_gc_memory_used);
        LIT_BIND_STATIC_GETTER("nextRound", objfn_gc_next_round);
        LIT_BIND_STATIC_METHOD("trigger", objfn_gc_trigger);
    }
    LIT_END_CLASS();
}



int util_table_iterator(LitTable* table, int number)
{
    if(table->m_count == 0)
    {
        return -1;
    }
    if(number >= (int)table->m_capacity)
    {
        return -1;
    }
    number++;
    for(; number < table->m_capacity; number++)
    {
        if(table->m_values[number].key != NULL)
        {
            return number;
        }
    }

    return -1;
}

LitValue util_table_iterator_key(LitTable* table, int index)
{
    if(table->m_capacity <= index)
    {
        return NULL_VALUE;
    }
    return OBJECT_VALUE(table->m_values[index].key);
}

static LitValue objfn_map_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_create_map(vm->state));
}

static LitValue objfn_map_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitValue val;
    LitValue value;
    LitMap* map;
    LitString* index;
    if(!IS_STRING(argv[0]))
    {
        lit_runtime_error_exiting(vm, "map index must be a string");
    }
    map = AS_MAP(instance);
    index = lit_as_string(argv[0]);
    if(argc == 2)
    {
        val = argv[1];
        if(map->index_fn != NULL)
        {
            return map->index_fn(vm, map, index, &val);
        }
        lit_map_set(vm->state, map, index, val);
        return val;
    }
    if(map->index_fn != NULL)
    {
        return map->index_fn(vm, map, index, NULL);
    }
    if(!lit_table_get(&map->values, index, &value))
    {
        return NULL_VALUE;
    }
    return value;
}

static LitValue objfn_map_addall(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    if(!IS_MAP(argv[0]))
    {
        lit_runtime_error_exiting(vm, "expected map as the argument");
    }
    lit_map_add_all(vm->state, AS_MAP(argv[0]), AS_MAP(instance));
    return NULL_VALUE;
}


static LitValue objfn_map_clear(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argv;
    (void)argc;
    AS_MAP(instance)->values.m_count = 0;
    return NULL_VALUE;
}

static LitValue objfn_map_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LIT_ENSURE_ARGS(1);
    (void)vm;
    int index;
    int value;
    index = argv[0] == NULL_VALUE ? -1 : lit_value_to_number(argv[0]);
    value = util_table_iterator(&AS_MAP(instance)->values, index);
    return value == -1 ? NULL_VALUE : lit_number_to_value(value);
}

static LitValue objfn_map_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    index = lit_check_number(vm, argv, argc, 0);
    return util_table_iterator_key(&AS_MAP(instance)->values, index);
}

static LitValue objfn_map_clone(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitState* state;
    LitMap* map;
    state = vm->state;
    map = lit_create_map(state);
    lit_table_add_all(state, &AS_MAP(instance)->values, &map->values);
    return OBJECT_VALUE(map);
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
    has_wrapper = map->index_fn != NULL;
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
        if(entry->key != NULL)
        {
            // Special hidden key
            field = has_wrapper ? map->index_fn(vm, map, entry->key, NULL) : entry->value;
            // This check is required to prevent infinite loops when playing with Module.privates and such
            strobval = (IS_MAP(field) && AS_MAP(field)->index_fn != NULL) ? CONST_STRING(state, "map") : lit_to_string(state, field);
            lit_push_root(state, (LitObject*)strobval);
            values_converted[i] = strobval;
            keys[i] = entry->key;
            olength += (
                lit_string_length(entry->key) + 3 + lit_string_length(strobval) +
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
        memcpy(&buffer[buffer_index], key->chars, lit_string_length(key));
        buffer_index += lit_string_length(key);
        memcpy(&buffer[buffer_index], " = ", 3);
        buffer_index += 3;
        memcpy(&buffer[buffer_index], value->chars, lit_string_length(value));
        buffer_index += lit_string_length(value);
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
        lit_pop_root(state);
    }
    buffer[olength] = '\0';
    LIT_FREE(vm->state, LitString*, keys);
    LIT_FREE(vm->state, LitString*, values_converted);
    return OBJECT_VALUE(lit_string_take(vm->state, buffer, olength, false));
}

static LitValue objfn_map_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return lit_number_to_value(AS_MAP(instance)->values.m_count);
}

void lit_open_map_library(LitState* state)
{
    LIT_BEGIN_CLASS("Map");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, objfn_map_constructor);
        lit_bind_method(state, klass, "[]", objfn_map_subscript);
        lit_bind_method(state, klass, "addAll", objfn_map_addall);
        lit_bind_method(state, klass, "clear", objfn_map_clear);
        lit_bind_method(state, klass, "iterator", objfn_map_iterator);
        lit_bind_method(state, klass, "iteratorValue", objfn_map_iteratorvalue);
        lit_bind_method(state, klass, "clone", objfn_map_clone);
        lit_bind_method(state, klass, "toString", objfn_map_tostring);
        LIT_BIND_GETTER("length", objfn_map_length);
        state->mapvalue_class = klass;
    }
    LIT_END_CLASS();
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
    return lit_number_to_value(fabs(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_cos(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(cos(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_sin(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(sin(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_tan(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(tan(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_acos(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(acos(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_asin(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(asin(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_atan(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(atan(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_atan2(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(atan2(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
}

static LitValue math_floor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(floor(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_ceil(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(ceil(lit_check_number(vm, argv, argc, 0)));
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
        return lit_number_to_value(round(value * places) / places);
    }
    return lit_number_to_value(round(value));
}

static LitValue math_min(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(fmin(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
}

static LitValue math_max(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(fmax(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
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
        return lit_number_to_value(fmax(x, fmin(y, z)));
    }
    return lit_number_to_value(fmax(y, fmin(x, z)));
}

static LitValue math_toRadians(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(lit_check_number(vm, argv, argc, 0) * M_PI / 180.0);
}

static LitValue math_toDegrees(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(lit_check_number(vm, argv, argc, 0) * 180.0 / M_PI);
}

static LitValue math_sqrt(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(sqrt(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_log(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(exp(lit_check_number(vm, argv, argc, 0)));
}

static LitValue math_exp(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    return lit_number_to_value(exp(lit_check_number(vm, argv, argc, 0)));
}

/*
 * Random
 */

static size_t static_random_data;

static size_t* extract_random_data(LitState* state, LitValue instance)
{
    if(IS_CLASS(instance))
    {
        return &static_random_data;
    }

    LitValue data;

    if(!lit_table_get(&AS_INSTANCE(instance)->fields, CONST_STRING(state, "_data"), &data))
    {
        return 0;
    }

    return (size_t*)AS_USERDATA(data)->data;
}

static LitValue random_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitUserdata* userdata = lit_create_userdata(vm->state, sizeof(size_t), false);
    lit_table_set(vm->state, &AS_INSTANCE(instance)->fields, CONST_STRING(vm->state, "_data"), OBJECT_VALUE(userdata));

    size_t* data = (size_t*)userdata->data;

    if(argc == 1)
    {
        size_t number = (size_t)lit_check_number(vm, argv, argc, 0);
        *data = number;
    }
    else
    {
        *data = time(NULL);
    }

    return OBJECT_VALUE(instance);
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
        *data = time(NULL);
    }

    return NULL_VALUE;
}

static LitValue random_int(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t* data = extract_random_data(vm->state, instance);

    if(argc == 1)
    {
        int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
        return lit_number_to_value(rand_r((unsigned int*)data) % bound);
    }
    else if(argc == 2)
    {
        int min = (int)lit_get_number(vm, argv, argc, 0, 0);
        int max = (int)lit_get_number(vm, argv, argc, 1, 1);

        if(max - min == 0)
        {
            return lit_number_to_value(max);
        }

        return lit_number_to_value(min + rand_r((unsigned int*)data) % (max - min));
    }

    return lit_number_to_value(rand_r((unsigned int*)data));
}

static LitValue random_float(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t* data = extract_random_data(vm->state, instance);
    double value = (double)rand_r((unsigned int*)data) / RAND_MAX;

    if(argc == 1)
    {
        int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
        return lit_number_to_value(value * bound);
    }
    else if(argc == 2)
    {
        int min = (int)lit_get_number(vm, argv, argc, 0, 0);
        int max = (int)lit_get_number(vm, argv, argc, 1, 1);

        if(max - min == 0)
        {
            return lit_number_to_value(max);
        }

        return lit_number_to_value(min + value * (max - min));
    }

    return lit_number_to_value(value);
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
        return TRUE_VALUE;
    }
    return FALSE_VALUE;
}

static LitValue random_pick(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int value = rand_r((unsigned int*)extract_random_data(vm->state, instance));

    if(argc == 1)
    {
        if(IS_ARRAY(argv[0]))
        {
            LitArray* array = AS_ARRAY(argv[0]);

            if(array->values.m_count == 0)
            {
                return NULL_VALUE;
            }

            return array->values.m_values[value % array->values.m_count];
        }
        else if(IS_MAP(argv[0]))
        {
            LitMap* map = AS_MAP(argv[0]);
            size_t length = map->values.m_count;
            size_t m_capacity = map->values.m_capacity;

            if(length == 0)
            {
                return NULL_VALUE;
            }

            size_t target = value % length;
            size_t index = 0;

            for(size_t i = 0; i < m_capacity; i++)
            {
                if(map->values.m_values[i].key != NULL)
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

    return NULL_VALUE;
}

void lit_open_math_library(LitState* state)
{
    {
        LIT_BEGIN_CLASS("Math");
        {
            LIT_SET_STATIC_FIELD("Pi", lit_number_to_value(M_PI));
            LIT_SET_STATIC_FIELD("Tau", lit_number_to_value(M_PI * 2));
            LIT_BIND_STATIC_METHOD("abs", math_abs);
            LIT_BIND_STATIC_METHOD("sin", math_sin);
            LIT_BIND_STATIC_METHOD("cos", math_cos);
            LIT_BIND_STATIC_METHOD("tan", math_tan);
            LIT_BIND_STATIC_METHOD("asin", math_asin);
            LIT_BIND_STATIC_METHOD("acos", math_acos);
            LIT_BIND_STATIC_METHOD("atan", math_atan);
            LIT_BIND_STATIC_METHOD("atan2", math_atan2);
            LIT_BIND_STATIC_METHOD("floor", math_floor);
            LIT_BIND_STATIC_METHOD("ceil", math_ceil);
            LIT_BIND_STATIC_METHOD("round", math_round);
            LIT_BIND_STATIC_METHOD("min", math_min);
            LIT_BIND_STATIC_METHOD("max", math_max);
            LIT_BIND_STATIC_METHOD("mid", math_mid);
            LIT_BIND_STATIC_METHOD("toRadians", math_toRadians);
            LIT_BIND_STATIC_METHOD("toDegrees", math_toDegrees);
            LIT_BIND_STATIC_METHOD("sqrt", math_sqrt);
            LIT_BIND_STATIC_METHOD("log", math_log);
            LIT_BIND_STATIC_METHOD("exp", math_exp);
        }
        LIT_END_CLASS();
    }
    srand(time(NULL));
    static_random_data = time(NULL);
    {
        LIT_BEGIN_CLASS("Random");
        {
            lit_bind_constructor(state, klass, random_constructor);
            lit_bind_method(state, klass, "setSeed", random_setSeed);
            lit_bind_method(state, klass, "int", random_int);
            lit_bind_method(state, klass, "float", random_float);
            lit_bind_method(state, klass, "chance", random_chance);
            lit_bind_method(state, klass, "pick", random_pick);
            LIT_BIND_STATIC_METHOD("setSeed", random_setSeed);
            LIT_BIND_STATIC_METHOD("int", random_int);
            LIT_BIND_STATIC_METHOD("float", random_float);
            LIT_BIND_STATIC_METHOD("bool", random_bool);
            LIT_BIND_STATIC_METHOD("chance", random_chance);
            LIT_BIND_STATIC_METHOD("pick", random_pick);
        }
        LIT_END_CLASS()
    }
}


static LitValue access_private(LitVM* vm, LitMap* map, LitString* name, LitValue* val)
{
    int index;
    LitValue value;
    LitString* id;
    LitModule* module;
    id = CONST_STRING(vm->state, "_module");
    if(!lit_table_get(&map->values, id, &value) || !IS_MODULE(value))
    {
        return NULL_VALUE;
    }
    module = AS_MODULE(value);

    if(id == name)
    {
        return OBJECT_VALUE(module);
    }

    if(lit_table_get(&module->private_names->values, name, &value))
    {
        index = (int)lit_value_to_number(value);
        if(index > -1 && index < (int)module->private_count)
        {
            if(val != NULL)
            {
                module->privates[index] = *val;
                return *val;
            }
            return module->privates[index];
        }
    }
    return NULL_VALUE;
}


static LitValue objfn_module_privates(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitModule* module;
    LitMap* map;
    (void)argc;
    (void)argv;
    module = IS_MODULE(instance) ? AS_MODULE(instance) : vm->fiber->module;
    map = module->private_names;
    if(map->index_fn == NULL)
    {
        map->index_fn = access_private;
        lit_table_set(vm->state, &map->values, CONST_STRING(vm->state, "_module"), OBJECT_VALUE(module));
    }
    return OBJECT_VALUE(map);
}

static LitValue objfn_module_current(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)instance;
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(vm->fiber->module);
}

static LitValue objfn_module_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_string_format(vm->state, "Module @", OBJECT_VALUE(AS_MODULE(instance)->name)));
}

static LitValue objfn_module_name(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(AS_MODULE(instance)->name);
}

void lit_open_module_library(LitState* state)
{
    LIT_BEGIN_CLASS("Module");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, util_invalid_constructor);
        LIT_SET_STATIC_FIELD("loaded", OBJECT_VALUE(state->vm->modules));
        LIT_BIND_STATIC_GETTER("privates", objfn_module_privates);
        LIT_BIND_STATIC_GETTER("current", objfn_module_current);
        lit_bind_method(state, klass, "toString", objfn_module_tostring);
        LIT_BIND_GETTER("name", objfn_module_name);
        LIT_BIND_GETTER("privates", objfn_module_privates);
        state->modulevalue_class = klass;
    }
    LIT_END_CLASS();
}



static LitValue objfn_object_class(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_get_class_for(vm->state, instance));
}

static LitValue objfn_object_super(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitClass* cl;
    cl = lit_get_class_for(vm->state, instance)->super;
    if(cl == NULL)
    {
        return NULL_VALUE;
    }
    return OBJECT_VALUE(cl);
}

static LitValue objfn_object_tostring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    return OBJECT_VALUE(lit_string_format(vm->state, "@ instance", OBJECT_VALUE(lit_get_class_for(vm->state, instance)->name)));
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
        if(key != NULL)
        {
            val = fromtbl->m_values[i].value;
            lit_map_set(state, destmap, key, OBJECT_VALUE(val));
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
    mclass = NULL;
    if(!IS_INSTANCE(instance))
    {
        lit_runtime_error_exiting(vm, "toMap() can only be used on instances");
    }
    inst = AS_INSTANCE(instance);
    map = lit_create_map(vm->state);
    {
        minst = lit_create_map(vm->state);
        fillmap(vm->state, minst, &(inst->fields), true);
    }
    {
        mclass = lit_create_map(vm->state);
        {
            mclstatics = lit_create_map(vm->state);
            fillmap(vm->state, mclstatics, &(inst->klass->static_fields), false);
        }
        {
            mclmethods = lit_create_map(vm->state);
            fillmap(vm->state, mclmethods, &(inst->klass->methods), false);
        }
        lit_map_set(vm->state, mclass, CONST_STRING(vm->state, "statics"), OBJECT_VALUE(mclstatics));
        lit_map_set(vm->state, mclass, CONST_STRING(vm->state, "methods"), OBJECT_VALUE(mclmethods));
    }
    lit_map_set(vm->state, map, CONST_STRING(vm->state, "instance"), OBJECT_VALUE(minst));
    lit_map_set(vm->state, map, CONST_STRING(vm->state, "class"), OBJECT_VALUE(mclass));
    return OBJECT_VALUE(map);
}

static LitValue objfn_object_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)argc;
    (void)argv;
    LitValue value;
    LitInstance* inst;
    if(!IS_INSTANCE(instance))
    {
        lit_runtime_error_exiting(vm, "cannot modify built-in types");
    }
    inst = AS_INSTANCE(instance);
    if(argc == 2)
    {
        if(!IS_STRING(argv[0]))
        {
            lit_runtime_error_exiting(vm, "object index must be a string");
        }

        lit_table_set(vm->state, &inst->fields, lit_as_string(argv[0]), argv[1]);
        return argv[1];
    }
    if(!IS_STRING(argv[0]))
    {
        lit_runtime_error_exiting(vm, "object index must be a string");
    }
    if(lit_table_get(&inst->fields, lit_as_string(argv[0]), &value))
    {
        return value;
    }
    if(lit_table_get(&inst->klass->static_fields, lit_as_string(argv[0]), &value))
    {
        return value;
    }
    if(lit_table_get(&inst->klass->methods, lit_as_string(argv[0]), &value))
    {
        return value;
    }
    return NULL_VALUE;
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
    self = AS_INSTANCE(instance);
    index = argv[0] == NULL_VALUE ? -1 : lit_value_to_number(argv[0]);
    value = util_table_iterator(&self->fields, index);
    return value == -1 ? NULL_VALUE : lit_number_to_value(value);
}


static LitValue objfn_object_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t index;
    LitInstance* self;
    index = lit_check_number(vm, argv, argc, 0);
    self = AS_INSTANCE(instance);
    return util_table_iterator_key(&self->fields, index);
}

void lit_open_object_library(LitState* state)
{
    LIT_BEGIN_CLASS("Object");
    {
        LIT_INHERIT_CLASS(state->classvalue_class);
        LIT_BIND_GETTER("class", objfn_object_class);
        LIT_BIND_GETTER("super", objfn_object_super);
        lit_bind_method(state, klass, "[]", objfn_object_subscript);
        #if 0
        lit_bind_method(state, klass, "hasMethod", objfn_object_hasmethod);
        
        #endif
        lit_bind_method(state, klass, "toString", objfn_object_tostring);
        lit_bind_method(state, klass, "toMap", objfn_object_tomap);
        lit_bind_method(state, klass, "iterator", objfn_object_iterator);
        lit_bind_method(state, klass, "iteratorValue", objfn_object_iteratorvalue);

        state->objectvalue_class = klass;
        state->objectvalue_class->super = state->classvalue_class;
    }
    LIT_END_CLASS();
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
    if(IS_NUMBER(argv[0]))
    {
        number = lit_value_to_number(argv[0]);
        if((range->to > range->from) ? (number >= range->to) : (number >= range->from))
        {
            return NULL_VALUE;
        }
        number += (((range->from - range->to) > 0) ? -1 : 1);
    }
    return lit_number_to_value(number);
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
    return OBJECT_VALUE(lit_string_format(vm->state, "Range(#, #)", range->from, range->to));
}

static LitValue objfn_range_from(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argv;
    (void)argc;
    return lit_number_to_value(AS_RANGE(instance)->from);
}

static LitValue objfn_range_set_from(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    AS_RANGE(instance)->from = lit_value_to_number(argv[0]);
    return argv[0];
}

static LitValue objfn_range_to(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return lit_number_to_value(AS_RANGE(instance)->to);
}

static LitValue objfn_range_set_to(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    AS_RANGE(instance)->to = lit_value_to_number(argv[0]);
    return argv[0];
}

static LitValue objfn_range_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    LitRange* range;
    range = AS_RANGE(instance);
    return lit_number_to_value(range->to - range->from);
}

void lit_open_range_library(LitState* state)
{
    LIT_BEGIN_CLASS("Range");
    {
        LIT_INHERIT_CLASS(state->objectvalue_class);
        lit_bind_constructor(state, klass, util_invalid_constructor);
        lit_bind_method(state, klass, "iterator", objfn_range_iterator);
        lit_bind_method(state, klass, "iteratorValue", objfn_range_iteratorvalue);
        lit_bind_method(state, klass, "toString", objfn_range_tostring);
        LIT_BIND_FIELD("from", objfn_range_from, objfn_range_set_from);
        LIT_BIND_FIELD("to", objfn_range_to, objfn_range_set_to);
        LIT_BIND_GETTER("length", objfn_range_length);
        state->rangevalue_class  = klass;
    }
    LIT_END_CLASS();
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
        return NULL;
    }
    if (x <= -10)
    {
        dest = int_to_string_helper(dest, n - 1, x / 10);
        if (dest == NULL)
        {
            return NULL;
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
        return NULL;
    }
    n--;
    if (x < 0)
    {
        if(n == 0)
        {
            return NULL;
        }
        n--;
        *p++ = '-';
    }
    else
    {
        x = -x;
    }
    p = int_to_string_helper(p, n, x);
    if(p == NULL)
    {
        return NULL;
    }
    *p = 0;
    return dest;
}

uint32_t lit_hash_string(const char* key, size_t length)
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

LitString* lit_string_alloc_empty(LitState* state, size_t length, bool reuse)
{
    LitString* string;
    string = ALLOCATE_OBJECT(state, LitString, LITTYPE_STRING);
    if(!reuse)
    {
        string->chars = sdsempty();
        /* reserving the required space may reduce number of allocations */
        string->chars = sdsMakeRoomFor(string->chars, length);
    }
    //string->chars = NULL;
    string->hash = 0;
    return string;
}

/*
* if given $chars was alloc'd via sds, then only a LitString instance is created, without initializing
* string->chars.
* if it was not, and not scheduled for reuse, then first an empty sds string is created,
* and $chars is appended, and finally, $chars is freed.
* NB. do *not* actually allocate any sds instance here - this is already done in lit_string_alloc_empty().
*/
LitString* lit_string_alloc(LitState* state, char* chars, size_t length, uint32_t hash, bool wassds, bool reuse)
{
    LitString* string;
    string = lit_string_alloc_empty(state, length, reuse);
    if(wassds && reuse)
    {
        string->chars = chars;
    }
    else
    {
        /*
        * string->chars is initialized in lit_string_alloc_empty(),
        * as an empty string!
        */
        string->chars = sdscatlen(string->chars, chars, length);
    }
    string->hash = hash;
    if(!wassds)
    {
        LIT_FREE(state, char, chars);
    }
    lit_register_string(state, string);
    return string;
}

void lit_register_string(LitState* state, LitString* string)
{
    if(lit_string_length(string) > 0)
    {
        lit_push_root(state, (LitObject*)string);
        lit_table_set(state, &state->vm->strings, string, NULL_VALUE);
        lit_pop_root(state);
    }
}

/* todo: track if $chars is a sds instance - additional argument $fromsds? */
LitString* lit_string_take(LitState* state, char* chars, size_t length, bool wassds)
{
    bool reuse;
    uint32_t hash;
    hash = lit_hash_string(chars, length);
    LitString* interned;
    interned = lit_table_find_string(&state->vm->strings, chars, length, hash);
    if(interned != NULL)
    {
        if(!wassds)
        {
            LIT_FREE(state, char, chars);
            //sdsfree(chars);
        }
        return interned;
    }
    reuse = wassds;
    return lit_string_alloc(state, (char*)chars, length, hash, wassds, reuse);
}

LitString* lit_string_copy(LitState* state, const char* chars, size_t length)
{
    uint32_t hash;
    char* heap_chars;
    LitString* interned;
    hash= lit_hash_string(chars, length);
    interned = lit_table_find_string(&state->vm->strings, chars, length, hash);
    if(interned != NULL)
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
    return lit_string_alloc(state, heap_chars, length, hash, true, true);
}

size_t lit_string_length(LitString* ls)
{
    if(ls->chars == NULL)
    {
        return 0;
    }
    return sdslen(ls->chars);
}

void lit_string_append_string(LitString* ls, const char* s, size_t len)
{
    if(len > 0)
    {
        if(ls->chars == NULL)
        {
            ls->chars = sdsnewlen(s, len);
        }
        else
        {
            ls->chars = sdscatlen(ls->chars, s, len);
        }
    }
}

void lit_string_append_strobj(LitString* ls, LitString* other)
{
    lit_string_append_string(ls, other->chars, lit_string_length(other));
}

void lit_string_append_char(LitString* ls, char ch)
{
    ls->chars = sdscatlen(ls->chars, (const char*)&ch, 1);
}

bool lit_string_contains_string(LitString* ls, const char* findme, size_t fmlen, bool icase)
{
    int selfch;
    int findch;
    size_t i;
    size_t j;
    size_t slen;
    size_t found;
    slen = sdslen(ls->chars);
    found = 0;
    if(slen >= fmlen)
    {
        for(i=0, j=0; i<slen; i++)
        {
            do
            {
                selfch = ls->chars[i];
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

LitValue lit_string_number_to_string(LitState* state, double value)
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

    return OBJECT_VALUE(lit_string_copy(state, buffer, length));
}


LitValue lit_string_format(LitState* state, const char* format, ...)
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
    result = lit_string_alloc_empty(state, total_length + 1, false);
    va_start(arg_list, format);
    for(c = format; *c != '\0'; c++)
    {
        switch(*c)
        {
            case '$':
                {
                    strval = va_arg(arg_list, const char*);
                    if(strval != NULL)
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
                    if(IS_STRING(val))
                    {
                        string = lit_as_string(val);
                    }
                    else
                    {
                        //fprintf(stderr, "format: not a string, but a '%s'\n", lit_get_value_type(val));
                        //string = lit_to_string(state, val);
                        goto default_ending_copying;
                    }
                    if(string != NULL)
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
                    string = lit_as_string(lit_string_number_to_string(state, va_arg(arg_list, double)));
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
    result->hash = lit_hash_string(result->chars, lit_string_length(result));
    lit_register_string(state, result);
    state->allow_gc = was_allowed;
    return OBJECT_VALUE(result);
}

bool lit_string_equal(LitState* state, LitString* a, LitString* b)
{
    (void)state;
    if((a == NULL) || (b == NULL))
    {
        return false;
    }
    return (sdscmp(a->chars, b->chars) == 0);
}

LitValue util_invalid_constructor(LitVM* vm, LitValue instance, size_t argc, LitValue* argv);

static LitValue objfn_string_plus(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* selfstr;
    LitString* result;
    LitValue value;
    (void)argc;
    selfstr = lit_as_string(instance);
    value = argv[0];
    LitString* strval = NULL;
    if(IS_STRING(value))
    {
        strval = lit_as_string(value);
    }
    else
    {
        strval = lit_to_string(vm->state, value);
    }
    result = lit_string_alloc_empty(vm->state, lit_string_length(selfstr) + lit_string_length(strval), false);
    lit_string_append_strobj(result, selfstr);
    lit_string_append_strobj(result, strval);
    lit_register_string(vm->state, result);
    return OBJECT_VALUE(result);
}

static LitValue objfn_string_splice(LitVM* vm, LitString* string, int from, int to)
{
    int length;
    length = lit_ustring_length(string);
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
    from = lit_uchar_offset(string->chars, from);
    to = lit_uchar_offset(string->chars, to);
    return OBJECT_VALUE(lit_ustring_from_range(vm->state, string, from, to - from + 1));
}

static LitValue objfn_string_subscript(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitString* c;
    LitString* string;
    if(IS_RANGE(argv[0]))
    {
        LitRange* range = AS_RANGE(argv[0]);
        return objfn_string_splice(vm, lit_as_string(instance), range->from, range->to);
    }
    string = lit_as_string(instance);
    index = lit_check_number(vm, argv, argc, 0);
    if(argc != 1)
    {
        lit_runtime_error_exiting(vm, "cannot modify strings with the subscript op");
    }
    if(index < 0)
    {
        index = lit_ustring_length(string) + index;

        if(index < 0)
        {
            return NULL_VALUE;
        }
    }
    c = lit_ustring_code_point_at(vm->state, string, lit_uchar_offset(string->chars, index));
    return c == NULL ? NULL_VALUE : OBJECT_VALUE(c);
}


static LitValue objfn_string_compare(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    LitString* self;
    LitString* other;
    (void)argc;
    self = lit_as_string(instance);
    if(IS_STRING(argv[0]))
    {
        other = lit_as_string(argv[0]);
        if(lit_string_length(self) == lit_string_length(other))
        {
            //fprintf(stderr, "string: same length(self=\"%s\" other=\"%s\")... strncmp=%d\n", self->chars, other->chars, strncmp(self->chars, other->chars, self->length));
            if(memcmp(self->chars, other->chars, lit_string_length(self)) == 0)
            {
                return TRUE_VALUE;
            }
        }
        return FALSE_VALUE;
    }
    else if(IS_NULL(argv[0]))
    {
        if((self == NULL) || IS_NULL(instance))
        {
            return TRUE_VALUE;
        }
        return FALSE_VALUE;
    }
    lit_runtime_error_exiting(vm, "can only compare string to another string or null");
    return FALSE_VALUE;
}

static LitValue objfn_string_less(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    return BOOL_VALUE(strcmp(lit_as_string(instance)->chars, lit_check_string(vm, argv, argc, 0)) < 0);
}

static LitValue objfn_string_greater(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    return BOOL_VALUE(strcmp(lit_as_string(instance)->chars, lit_check_string(vm, argv, argc, 0)) > 0);
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
    result = strtod(lit_as_string(instance)->chars, NULL);
    if(errno == ERANGE)
    {
        errno = 0;
        return NULL_VALUE;
    }
    return lit_number_to_value(result);
}

static LitValue objfn_string_touppercase(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    char* buffer;
    LitString* rt;
    LitString* string;
    string = lit_as_string(instance);
    (void)argc;
    (void)argv;
    buffer = LIT_ALLOCATE(vm->state, char, lit_string_length(string) + 1);
    for(i = 0; i < lit_string_length(string); i++)
    {
        buffer[i] = (char)toupper(string->chars[i]);
    }
    buffer[i] = 0;
    rt = lit_string_take(vm->state, buffer, lit_string_length(string), false);
    return OBJECT_VALUE(rt);
}

static LitValue objfn_string_tolowercase(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    char* buffer;
    LitString* rt;
    LitString* string;
    string = lit_as_string(instance);
    (void)argc;
    (void)argv;
    buffer = LIT_ALLOCATE(vm->state, char, lit_string_length(string) + 1);
    for(i = 0; i < lit_string_length(string); i++)
    {
        buffer[i] = (char)tolower(string->chars[i]);
    }
    buffer[i] = 0;
    rt = lit_string_take(vm->state, buffer, lit_string_length(string), false);
    return OBJECT_VALUE(rt);
}

static LitValue objfn_string_tobyte(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int iv;
    LitString* selfstr;
    (void)vm;
    (void)argc;
    (void)argv;
    selfstr = lit_as_string(instance);
    iv = lit_ustring_decode((const uint8_t*)selfstr->chars, lit_string_length(selfstr));
    return lit_number_to_value(iv);
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
    selfstr = lit_as_string(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(argc > 1)
    {
        icase = lit_check_bool(vm, argv, argc, 1);
    }
    if(lit_string_contains_string(selfstr, sub->chars, sdslen(sub->chars), icase))
    {
        return TRUE_VALUE;
    }
    return FALSE_VALUE;
}

static LitValue objfn_string_startswith(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    LitString* sub;
    LitString* string;
    string = lit_as_string(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(sub == string)
    {
        return TRUE_VALUE;
    }
    if(lit_string_length(sub) > lit_string_length(string))
    {
        return FALSE_VALUE;
    }
    for(i = 0; i < lit_string_length(sub); i++)
    {
        if(sub->chars[i] != string->chars[i])
        {
            return FALSE_VALUE;
        }
    }
    return TRUE_VALUE;
}

static LitValue objfn_string_endswith(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    size_t i;
    size_t start;
    LitString* sub;
    LitString* string;
    string = lit_as_string(instance);
    sub = lit_check_object_string(vm, argv, argc, 0);
    if(sub == string)
    {
        return TRUE_VALUE;
    }
    if(lit_string_length(sub) > lit_string_length(string))
    {
        return FALSE_VALUE;
    }
    start = lit_string_length(string) - lit_string_length(sub);
    for(i = 0; i < lit_string_length(sub); i++)
    {
        if(sub->chars[i] != string->chars[i + start])
        {
            return FALSE_VALUE;
        }
    }
    return TRUE_VALUE;
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
    if(!IS_STRING(argv[0]) || !IS_STRING(argv[1]))
    {
        lit_runtime_error_exiting(vm, "expected 2 string arguments");
    }
    string = lit_as_string(instance);
    what = lit_as_string(argv[0]);
    with = lit_as_string(argv[1]);
    buffer_length = 0;
    for(i = 0; i < lit_string_length(string); i++)
    {
        if(strncmp(string->chars + i, what->chars, lit_string_length(what)) == 0)
        {
            i += lit_string_length(what) - 1;
            buffer_length += lit_string_length(with);
        }
        else
        {
            buffer_length++;
        }
    }
    buffer_index = 0;
    buffer = LIT_ALLOCATE(vm->state, char, buffer_length+1);
    for(i = 0; i < lit_string_length(string); i++)
    {
        if(strncmp(string->chars + i, what->chars, lit_string_length(what)) == 0)
        {
            memcpy(buffer + buffer_index, with->chars, lit_string_length(with));
            buffer_index += lit_string_length(with);
            i += lit_string_length(what) - 1;
        }
        else
        {
            buffer[buffer_index] = string->chars[i];
            buffer_index++;
        }
    }
    buffer[buffer_length] = '\0';
    return OBJECT_VALUE(lit_string_take(vm->state, buffer, buffer_length, false));
}

static LitValue objfn_string_substring(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int to;
    int from;
    from = lit_check_number(vm, argv, argc, 0);
    to = lit_check_number(vm, argv, argc, 1);
    return objfn_string_splice(vm, lit_as_string(instance), from, to);
}


static LitValue objfn_string_length(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    (void)vm;
    (void)argc;
    (void)argv;
    return lit_number_to_value(lit_ustring_length(lit_as_string(instance)));
}

static LitValue objfn_string_iterator(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    int index;
    LitString* string;
    string = lit_as_string(instance);
    if(IS_NULL(argv[0]))
    {
        if(lit_string_length(string) == 0)
        {
            return NULL_VALUE;
        }
        return lit_number_to_value(0);
    }
    index = lit_check_number(vm, argv, argc, 0);
    if(index < 0)
    {
        return NULL_VALUE;
    }
    do
    {
        index++;
        if(index >= (int)lit_string_length(string))
        {
            return NULL_VALUE;
        }
    } while((string->chars[index] & 0xc0) == 0x80);
    return lit_number_to_value(index);
}


static LitValue objfn_string_iteratorvalue(LitVM* vm, LitValue instance, size_t argc, LitValue* argv)
{
    uint32_t index;
    LitString* string;
    string = lit_as_string(instance);
    index = lit_check_number(vm, argv, argc, 0);
    if(index == UINT32_MAX)
    {
        return false;
    }
    return OBJECT_VALUE(lit_ustring_code_point_at(vm->state, string, index));
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
    selfstr = lit_as_string(instance);
    selflen = lit_string_length(selfstr);
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
                                return NULL_VALUE;
                            }
                            buf = sdscatlen(buf, lit_as_string(argv[ai])->chars, lit_string_length(lit_as_string(argv[ai])));
                        }
                        break;
                    case 'd':
                    case 'i':
                        {
                            if(!check_fmt_arg(vm, buf, ai, argc, argv, "%d"))
                            {
                                return NULL_VALUE;
                            }
                            if(IS_NUMBER(argv[ai]))
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
                                return NULL_VALUE;
                            }
                            if(!IS_NUMBER(argv[ai]))
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
                            return NULL_VALUE;
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
    rtv = OBJECT_VALUE(lit_string_copy(vm->state, buf, sdslen(buf)));
    sdsfree(buf);
    return rtv;
}

void lit_open_string_library(LitState* state)
{
    {
        LIT_BEGIN_CLASS("String");
        {
            LIT_INHERIT_CLASS(state->objectvalue_class);
            lit_bind_constructor(state, klass, util_invalid_constructor);
            lit_bind_method(state, klass, "+", objfn_string_plus);
            lit_bind_method(state, klass, "[]", objfn_string_subscript);
            lit_bind_method(state, klass, "<", objfn_string_less);
            lit_bind_method(state, klass, ">", objfn_string_greater);
            lit_bind_method(state, klass, "==", objfn_string_compare);
            lit_bind_method(state, klass, "toString", objfn_string_tostring);
            {
                lit_bind_method(state, klass, "toNumber", objfn_string_tonumber);
                LIT_BIND_GETTER("to_i", objfn_string_tonumber);
            }
            /*
            * String.toUpper()
            * turns string to uppercase.
            */
            {
                lit_bind_method(state, klass, "toUpperCase", objfn_string_touppercase);
                lit_bind_method(state, klass, "toUpper", objfn_string_touppercase);
                LIT_BIND_GETTER("upper", objfn_string_touppercase);
            }
            /*
            * String.toLower()
            * turns string to lowercase.
            */
            {
                lit_bind_method(state, klass, "toLowerCase", objfn_string_tolowercase);
                lit_bind_method(state, klass, "toLower", objfn_string_tolowercase);
                LIT_BIND_GETTER("lower", objfn_string_tolowercase);
            }
            /*
            * String.toByte
            * turns string into a byte integer.
            * if the string is multi character string, then $self[0] is used.
            * i.e., "foo".toByte == 102 (equiv: "foo"[0].toByte)
            */
            {
                LIT_BIND_GETTER("toByte", objfn_string_tobyte);
            }

            //TODO: byteAt
            //lit_bind_method(state, klass, "byteAt", objfn_string_byteat);

            /*
            * String.contains(String str[, bool icase])
            * returns true if $self contains $str.
            * if optional param $icase is true, then search is case-insensitive.
            * this is faster than, eg, "FOO".lower.contains("BLAH".lower), since
            * only the characters searched are tolower()'d.
            */
            lit_bind_method(state, klass, "contains", objfn_string_contains);

            /*
            * String.startsWith(String str)
            * returns true if $self starts with $str
            */
            lit_bind_method(state, klass, "startsWith", objfn_string_startswith);

            /*
            * String.endsWith(String str)
            * returns true if $self ends with $str
            */
            lit_bind_method(state, klass, "endsWith", objfn_string_endswith);

            /*
            * String.replace(String find, String rep)
            * replaces every $find with $rep
            */
            lit_bind_method(state, klass, "replace", objfn_string_replace);
            {
                lit_bind_method(state, klass, "substring", objfn_string_substring);
                lit_bind_method(state, klass, "substr", objfn_string_substring);
            }
            lit_bind_method(state, klass, "iterator", objfn_string_iterator);
            lit_bind_method(state, klass, "iteratorValue", objfn_string_iteratorvalue);
            LIT_BIND_GETTER("length", objfn_string_length);
            lit_bind_method(state, klass, "format", objfn_string_format);
            state->stringvalue_class = klass;
        }
        LIT_END_CLASS();
    }
}


#if defined(__unix__) || defined(__linux__)
#endif

#ifndef __TINYC__
    #if __has_include(<readline/readline.h>)
        #define LIT_HAVE_READLINE
    #endif
#endif


// Used for clean up on Ctrl+C / Ctrl+Z
static LitState* repl_state;

void interupt_handler(int signal_id)
{
    (void)signal_id;
    lit_free_state(repl_state);
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
        if(line == NULL)
        {
            return 0;
        }
        add_history(line);
        LitInterpretResult result = lit_interpret(state, "repl", line);
        if(result.type == LITRESULT_OK && result.result != NULL_VALUE)
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

    if(dir == NULL)
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

            if(subdir == NULL)
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

    amount = lit_free_state(state);
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
    state = lit_new_state();
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
    arg_array = NULL;
    show_repl = false;
    evaled = false;
    showed_help = false;
    perform_tests = false;
    bytecode_file = NULL;
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
                if(module == NULL)
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
            arg_array = lit_create_array(state);

            for(j = 0; j < (size_t)args_left; j++)
            {
                arg_string = argv[i + j + 1];
                arg_array->values.push(OBJECT_CONST_STRING(state, arg_string));
            }

            lit_set_global(state, CONST_STRING(state, "args"), OBJECT_VALUE(arg_array));
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
        if(bytecode_file != NULL)
        {
            if(!lit_compile_and_save_files(state, files_to_run, num_files_to_run, bytecode_file))
            {
                result = LITRESULT_COMPILE_ERROR;
            }
        }
        else
        {
            if(arg_array == NULL)
            {
                arg_array = lit_create_array(state);
            }
            lit_set_global(state, CONST_STRING(state, "args"), OBJECT_VALUE(arg_array));
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
