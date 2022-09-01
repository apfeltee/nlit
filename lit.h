
#pragma once

#include <assert.h>     // for assert
#include <ctype.h>      // for tolower
#include <setjmp.h>     // for longjmp, jmp_buf, setjmp
#include <stdarg.h>     // for va_list
#include <stdint.h>     // for uint8_t, uint16_t, uint32_t, int64_t, uint64_t
#include <stdio.h>      // for size_t, fwrite, FILE, fread, fflush, printf
#include <stdlib.h>     // for free, malloc, realloc
#include <string.h>     // for strlen, memcpy
#include <sys/time.h>   // for CLOCKS_PER_SEC
#include <sys/types.h>  // for clock_t
#include <time.h>       // for clock
#include <cmath>        // for isinf, isnan
#include <iosfwd>       // for nullptr_t
#include <string>       // for string, operator==, to_string, basic_string
#include <string_view>  // for string_view, operator==, hash
#include <utility>      // for forward

#if defined(__unix__) || defined(__linux__)
    #include <dirent.h>
#endif

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

#define UINT8_COUNT UINT8_MAX + 1
#define UINT16_COUNT UINT16_MAX + 1

#define TABLE_MAX_LOAD 0.75
// Do not change these, or old bytecode files will break!
#define LIT_BYTECODE_MAGIC_NUMBER 6932
#define LIT_BYTECODE_END_NUMBER 2942
#define LIT_STRING_KEY 48


#define LIT_EXIT_CODE_ARGUMENT_ERROR 1
#define LIT_EXIT_CODE_MEM_LEAK 2
#define LIT_EXIT_CODE_RUNTIME_ERROR 70
#define LIT_EXIT_CODE_COMPILE_ERROR 65


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


/**
* maybe use __init__ for constructor, and __fini__ for destructor?
* need a name that clearly distinguishes these functions from others.
*/
/* name of the constructor function, if defined*/
#define LIT_NAME_CONSTRUCTOR "constructor"
/* not yet implemented: name of the destructor, if defined */
#define LIT_NAME_DESTRUCTOR "destructor"

#define LIT_GROW_CAPACITY(capacity) \
    (((capacity) < 8) ? (8) : ((capacity) * 2))

#define LIT_GROW_ARRAY(state, previous, type, old_count, count) \
    (type*)Memory::reallocate(state, previous, sizeof(type) * (old_count), sizeof(type) * (count))

#define LIT_FREE_ARRAY(state, type, pointer, old_count) \
    Memory::reallocate(state, pointer, sizeof(type) * (old_count), 0)

#define LIT_ALLOCATE(state, type, count) \
    (type*)Memory::reallocate(state, nullptr, 0, sizeof(type) * (count))

#define LIT_FREE(state, type, pointer) \
    Memory::reallocate(state, pointer, sizeof(type), 0)

#define LIT_ENSURE_ARGS(count) \
    if(argc != count) \
    { \
        lit_runtime_error(vm, "expected %i argument, got %i", count, argc); \
        return lit::Object::NullVal; \
    }

#define LIT_ENSURE_MIN_ARGS(count) \
    if(argc < count) \
    { \
        lit_runtime_error(vm, "expected minimum %i argument, got %i", count, argc); \
        return lit::Object::NullVal; \
    }

#define LIT_ENSURE_MAX_ARGS(count) \
    if(argc > count) \
    { \
        lit_runtime_error(vm, "expected maximum %i argument, got %i", count, argc); \
        return lit::Object::NullVal; \
    }


namespace lit
{
    enum class Error
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

    enum OpCode
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


    enum OptimizationLevel
    {
        LITOPTLEVEL_NONE,
        LITOPTLEVEL_REPL,
        LITOPTLEVEL_DEBUG,
        LITOPTLEVEL_RELEASE,
        LITOPTLEVEL_EXTREME,

        LITOPTLEVEL_TOTAL
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

    enum InterpretResultType
    {
        LITRESULT_OK,
        LITRESULT_COMPILE_ERROR,
        LITRESULT_RUNTIME_ERROR,
        LITRESULT_INVALID
    };

    enum ErrorType
    {
        COMPILE_ERROR,
        RUNTIME_ERROR
    };

    enum FunctionType
    {
        LITFUNC_REGULAR,
        LITFUNC_SCRIPT,
        LITFUNC_METHOD,
        LITFUNC_STATIC_METHOD,
        LITFUNC_CONSTRUCTOR
    };

    class /**/Writer;
    class /**/State;
    class /**/VM;
    class /**/Array;
    class /**/Map;
    class /**/Userdata;
    class /**/String;
    class /**/Module;
    class /**/Fiber;
    class /**/Function;
    class /**/NativeMethod;
    class /**/Chunk;

    namespace AST
    {
        class /**/Parser;
        class /**/Scanner;
        class /**/Preprocessor;
        class /**/Compiler;
        class /**/Emitter;
        class /**/Optimizer;
    }

    typedef uint64_t Value;

    String* lit_vformat_error(State* state, size_t line, Error error, va_list args);
    String* lit_format_error(State* state, size_t line, Error error, ...);
    int lit_closest_power_of_two(int n);
    bool lit_handle_runtime_error(VM* vm, String* error_string);
    bool lit_vruntime_error(VM* vm, const char* format, va_list args);
    bool lit_runtime_error(VM* vm, const char* format, ...);
    bool lit_runtime_error_exiting(VM* vm, const char* format, ...);

    void lit_disassemble_module(State* state, Module* module, const char* source);
    void lit_disassemble_chunk(Chunk* chunk, const char* name, const char* source);
    size_t lit_disassemble_instruction(State* state, Chunk* chunk, size_t offset, const char* source);

    namespace Util
    {
        char* copyString(std::string_view sv);
        char* readFile(const char* path, size_t* destlen);
        char* patchFilename(char* file_name);
    }

    class Memory
    {
        private:
            static void setBytesAllocated(State* state, int64_t toadd);

            static void runGCIfNeeded(State* state);

            static void raiseMemoryError(State* state, const char* msg);

        public:
            /* allocate/reallocate memory. if new_size is 0, frees the pointer, and returns nullptr. */
            static void* reallocate(State* state, void* pointer, size_t oldsize, size_t newsize)
            {
                void* ptr;
                setBytesAllocated(state, (int64_t)newsize - (int64_t)oldsize);
                if(newsize > oldsize)
                {
                    runGCIfNeeded(state);
                }
                if(newsize == 0)
                {
                    free(pointer);
                    return nullptr;
                }
                ptr = (void*)realloc(pointer, newsize);
                if(ptr == nullptr)
                {
                    raiseMemoryError(state, "!!out of memory!!");
                }
                return ptr;
            }

            template<typename Type>
            static Type* reallocate(State* state, Type* pointer, size_t oldsize, size_t newsize)
            {
                return (Type*)Memory::reallocate(state, (void*)pointer, oldsize, newsize);
            }


            template<typename Type>
            static Type* allocate(State* state)
            {
                return Memory::reallocate<Type>(state, nullptr, 0, sizeof(Type));
            }
    };

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
            State* m_state = nullptr;
            //ElementT m_initialbuf[10];

        private:
            template<typename... ArgsT>
            inline void raiseError(const char* fmt, ArgsT&&... args);

        public:
            inline PCGenericArray()
            {
            }

            PCGenericArray(const PCGenericArray&) = delete;
            PCGenericArray& operator=(const PCGenericArray&) = delete;

            inline void init(State* state)
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

            inline ElementT& at(size_t idx)
            {
                return m_values[idx];
            }

            inline ElementT& at(size_t idx) const
            {
                return m_values[idx];
            }

            inline void set(size_t idx, const ElementT& val)
            {
                m_values[idx] = val;
            }

            inline void push(const ElementT& value)
            {
                size_t oldcap;
                //if(m_capacity < (m_count + 1))
                if((m_count + 1) > m_capacity)
                {
                    oldcap = m_capacity;
                    //fprintf(stderr, "<%p> push(): m_capacity=%d, m_count=%d: now growing ...\n", this, m_capacity, m_count);
                    m_capacity = LIT_GROW_CAPACITY(oldcap) + 2;
                    if((m_count + 1) > m_capacity)
                    {
                        m_capacity = LIT_GROW_CAPACITY(m_capacity + 1) * m_capacity;
                    }
                    //fprintf(stderr, "<%p> push(): new capacity=%d (m_count=%d)\n", this, m_capacity, m_count);
                    m_values = LIT_GROW_ARRAY(m_state, m_values, ElementT, oldcap, m_capacity);
                    if(m_values == nullptr)
                    {
                        raiseError("GenericArray: FAILED to grow array!");
                    }
                }
                //fprintf(stderr, "PCGenericArray: <%p> set at %d\n", this, m_count);
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

    class Writer
    {
        public:
            using WriteByteFuncType = void(*)(Writer*, int);
            using WriteStringFuncType = void(*)(Writer*, const char*, size_t);
            using WriteFormatFuncType = void(*)(Writer*, const char*, va_list);

        public:
            static void stringAppend(String* ds, int byte);
            static void stringAppend(String* ds, const char* str, size_t len);
            static void stringAppendFormat(String* ds, const char* fmt, va_list va);

            static void cb_writebyte(Writer* wr, int byte)
            {
                String* ds;
                if(wr->stringmode)
                {
                    ds = (String*)wr->uptr;
                    stringAppend(ds, byte);        
                }
                else
                {
                    fputc(byte, (FILE*)wr->uptr);
                }
            }

            static void cb_writestring(Writer* wr, const char* string, size_t len)
            {
                String* ds;
                if(wr->stringmode)
                {
                    ds = (String*)wr->uptr;
                    stringAppend(ds, string, len);
                }
                else
                {
                    fwrite(string, sizeof(char), len, (FILE*)wr->uptr);
                }
            }

            static void cb_writeformat(Writer* wr, const char* fmt, va_list va)
            {
                String* ds;
                if(wr->stringmode)
                {
                    ds = (String*)wr->uptr;
                    stringAppendFormat(ds, fmt, va);
                }
                else
                {
                    vfprintf((FILE*)wr->uptr, fmt, va);
                }
            }

            static void initDefault(State* state, Writer* wr)
            {
                wr->m_state = state;
                wr->forceflush = false;
                wr->stringmode = false;
                wr->fnbyte = cb_writebyte;
                wr->fnstring = cb_writestring;
                wr->fnformat = cb_writeformat;
            }

        public:
            State* m_state;
            /* the main pointer, that either holds a pointer to a String, or a FILE */
            void* uptr;

            /* if true, then uptr is a String, otherwise it's a FILE */
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
            * creates a Writer that writes to the given FILE.
            * if $forceflush is true, then fflush() is called after each i/o operation.
            */
            void initFile(State* state, FILE* fh, bool forceflush)
            {
                initDefault(state, this);
                this->uptr = fh;
                this->forceflush = forceflush;
            }

            /*
            * creates a Writer that writes to a buffer.
            */
            void initString(State* state);

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
            * returns a String* if this Writer was initiated via lit_writer_init_string, otherwise nullptr.
            */
            String* asString()
            {
                if(this->stringmode)
                {
                    return (String*)this->uptr;
                }
                return nullptr;
            }
    };


    struct Result
    {
        /* the result of this interpret/call attempt */
        InterpretResultType type;
        /* the value returned from this interpret/call attempt */
        Value result;
    };

    class Object
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

            static constexpr Value TAG_NULL = 1u;
            static constexpr Value TAG_FALSE = 2u;
            static constexpr Value TAG_TRUE = 3u;
            static constexpr Value FalseVal = ((Value)(uint64_t)(Object::QNAN_BIT | TAG_FALSE));
            static constexpr Value TrueVal = ((Value)(uint64_t)(Object::QNAN_BIT | TAG_TRUE));
            static constexpr Value NullVal = ((Value)(uint64_t)(Object::QNAN_BIT | TAG_NULL));

        public:
            static Object* make(State* state, size_t size, Object::Type type);

            template<typename ObjType>
            static inline ObjType* make(State* state, Object::Type type)
            {
                return (ObjType*)Object::make(state, sizeof(ObjType), type);
            }

            static void releaseObject(State* state, Object* obj);

            static inline Object* asObject(Value v)
            {
                return ((Object*)(uintptr_t)((v) & ~(Object::SIGN_BIT | Object::QNAN_BIT)));
            }

            static inline Value asValue(Object* obj)
            {
                return (Value)(Object::SIGN_BIT | Object::QNAN_BIT | (uint64_t)(uintptr_t)(obj));
            }

            template<typename ObjType>
            static inline ObjType* as(Value v)
            {
                return (ObjType*)Object::asObject(v);
            }

            static inline bool asBool(Value v)
            {
                return v == Object::TrueVal;
            }

            static inline String* asString(Value v)
            {
                return as<String>(v);
            }

            /* turn the given value to a number */
            static inline double toNumber(Value value)
            {
                return *((double*)&value);
            }

            /* turn a number into a value*/
            static inline Value toValue(double num)
            {
                return *((Value*)&num);
            }

            /* is this value falsey? */
            static inline bool isFalsey(Value value)
            {
                if(Object::isNull(value))
                {
                    return true;
                }
                return (
                    (Object::isBool(value) && value == Object::FalseVal) ||
                    (Object::isNumber(value) && (Object::toNumber(value) == 0))
                );
            }

            static inline bool isBool(Value v)
            {
                return ((v & Object::FalseVal) == Object::FalseVal);
            }

            static inline bool isNull(Value v)
            {
                return (v == Object::NullVal);
            }

            static inline bool isNumber(Value v)
            {
                return (( v & Object::QNAN_BIT) != Object::QNAN_BIT);
            }

            static inline bool isObject(Value v)
            {
                return (
                    (v & (Object::QNAN_BIT | Object::SIGN_BIT)) == (Object::QNAN_BIT | Object::SIGN_BIT)
                );
            }
            
            static inline bool isObjectType(Value value, Type t)
            {
                return (
                    Object::isObject(value) &&
                    (Object::asObject(value) != nullptr) &&
                    (Object::asObject(value)->type == t)
                );
            }

            static inline bool isString(Value value)
            {
                return Object::isObjectType(value, Object::Type::String);
            }

            static inline bool isFunction(Value value)
            {
                return Object::isObjectType(value, Object::Type::Function);
            }

            static inline bool isNativeFunction(Value value)
            {
                return Object::isObjectType(value, Object::Type::NativeFunction);
            }

            static inline bool isNativePrimitive(Value value)
            {
                return Object::isObjectType(value, Object::Type::NativePrimitive);
            }

            static inline bool isNativeMethod(Value value)
            {
                return Object::isObjectType(value, Object::Type::NativeMethod);
            }

            static inline bool isPrimitiveMethod(Value value)
            {
                return Object::isObjectType(value, Object::Type::PrimitiveMethod);
            }

            static inline bool isModule(Value value)
            {
                return Object::isObjectType(value, Object::Type::Module);
            }

            static inline bool isClosure(Value value)
            {
                return Object::isObjectType(value, Object::Type::Closure);
            }

            static inline bool isUpvalue(Value value)
            {
                return Object::isObjectType(value, Object::Type::Upvalue);
            }

            static inline bool isClass(Value value)
            {
                return Object::isObjectType(value, Object::Type::Class);
            }

            static inline bool isInstance(Value value)
            {
                return Object::isObjectType(value, Object::Type::Instance);
            }

            static inline bool isArray(Value value)
            {
                return Object::isObjectType(value, Object::Type::Array);
            }

            static inline bool isMap(Value value)
            {
                return Object::isObjectType(value, Object::Type::Map);
            }

            static inline bool isBoundMethod(Value value)
            {
                return Object::isObjectType(value, Object::Type::BoundMethod);
            }

            static inline bool isUserdata(Value value)
            {
                return Object::isObjectType(value, Object::Type::Userdata);
            }

            static inline bool isRange(Value value)
            {
                return Object::isObjectType(value, Object::Type::Range);
            }

            static inline bool isField(Value value)
            {
                return Object::isObjectType(value, Object::Type::Field);
            }

            static inline bool isReference(Value value)
            {
                return Object::isObjectType(value, Object::Type::Reference);
            }

            static inline bool isCallableFunction(Value value)
            {
                if(Object::isObject(value))
                {
                    Object::Type type = Object::asObject(value)->type;
                    return (
                        (type == Object::Type::Closure) ||
                        (type == Object::Type::Function) ||
                        (type == Object::Type::NativeFunction) ||
                        (type == Object::Type::NativePrimitive) ||
                        (type == Object::Type::NativeMethod) ||
                        (type == Object::Type::PrimitiveMethod) ||
                        (type == Object::Type::BoundMethod)
                    );
                }
                return false;
            }

            static inline Value fromBool(bool v)
            {
                if(v)
                {
                    return lit::Object::TrueVal;
                }
                return lit::Object::FalseVal;
            };

            static bool compare(State* state, const Value a, const Value b);

            static String* functionName(VM* vm, Value instance);

            static String* toString(State* state, Value valobj);

            static const char* valueName(Value value)
            {
                static const char* object_type_names[] =
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
                if((value == Object::NullVal) || (Object::isNull(value)))
                {
                    return "null";
                }
                else if(Object::isBool(value))
                {
                    return "bool";
                }
                else if(Object::isNumber(value))
                {
                    return "number";
                }
                else if(Object::isObject(value))
                {
                    return object_type_names[(int)Object::asObject(value)->type];
                }
                return "unknown";
            }

            // a helper to get State from a VM pointer
            static State* asState(VM* vm);

            static void printArray(State* state, Writer* wr, Array* array, size_t size);

            static void printMap(State* state, Writer* wr, Map* map, size_t size);

            static void printObject(State* state, Writer* wr, Value value);

            static void print(State* state, Writer* wr, Value value)
            {

                if(Object::isBool(value))
                {
                    wr->put(Object::asBool(value) ? "true" : "false");
                }
                else if(Object::isNull(value))
                {
                    wr->put("null");
                }
                else if(Object::isNumber(value))
                {
                    wr->format("%g", Object::toNumber(value));
                }
                else if(Object::isObject(value))
                {
                    printObject(state, wr, value);
                }
            }

        public:
            State* m_state;
            /* the type of this object */
            Type type;
            Object* next;
            bool marked;

        public:
            inline Value asValue()
            {
                return Object::asValue(this);
            }
    };

    class Field: public Object
    {
        public:
            static Field* make(State* state, String* name, Object* getter, Object* setter)
            {
                Field* field;
                field = Object::make<Field>(state, Object::Type::Field);
                field->m_name = name;
                field->getter = getter;
                field->setter = setter;
                return field;
            }

        public:
            String* m_name;
            Object* getter;
            Object* setter;
    };

    class Array: public Object
    {
        public:
            static Array* make(State* state)
            {
                Array* array;
                array = Object::make<Array>(state, Object::Type::Array);
                array->m_actualarray.init(state);
                return array;
            }

        public:
            PCGenericArray<Value> m_actualarray;

        public:
            void push(Value val)
            {
                m_actualarray.push(val);
            }

            inline size_t size()
            {
                return m_actualarray.m_count;
            }

            inline Value at(size_t idx)
            {
                return m_actualarray.at(idx);
            }

            int indexOf(Value value);

            Value removeAt(size_t index);
    };

    class String: public Object
    {
        public:
            class Regex
            {
                
            };

        public:
            static uint32_t makeHash(std::string_view sv)
            {
                return std::hash<std::string_view>{}(sv);
            }

            static uint32_t makeHash(const char* str, size_t length)
            {
                return makeHash(std::string_view(str, length));
            }

            static String* allocEmpty(State* state, size_t length);

            static String* allocate(State* state, const char* chars, size_t length, uint32_t hs, bool mustfree);

            static void statePutInterned(State* state, String* string);

            static bool stateGetGCAllowed(State* state);

            static void stateSetGCAllowed(State* state, bool v);

            static String* stateFindInterned(State* state, const char* chars, size_t length, uint32_t hs);

            /*
            * create a String by reusing $chars. ONLY use this if you're certain that $chars isn't being freed.
            * if $wassds is true, then the sds instance is reused as-is.
            */
            /* todo: track if $chars is a sds instance - additional argument $fromsds? */
            static String* take(State* state, char* chars, size_t length);

            /* copy a string, creating a full newly allocated String. */
            static String* copy(State* state, const char* chars, size_t length);

            static inline String* copy(State* state, std::string_view sv)
            {
                return String::copy(state, sv.data(), sv.size());
            }

            static inline String* make(State* state)
            {
                return String::copy(state, "");
            }

            static inline Value internValue(State* state, const char* str, size_t length)
            {
                return String::copy(state, str, length)->asValue();
            }

            static inline Value internValue(State* state, std::string_view sv)
            {
                return String::internValue(state, sv.data(), sv.size());
            }

            static inline String* intern(State* state, const char* str, size_t len)
            {
                return String::copy(state, str, len);
            }

            static inline String* intern(State* state, std::string_view sv)
            {
                return String::intern(state, sv.data(), sv.size());
            }

            static Value stringNumberToString(State* state, double value);

            static int decodeNumBytes(uint8_t byte);

            static int encodeNumBytes(int value);

            static String* fromCodePoint(State* state, int value);

            static String* fromRange(State* state, String* source, int start, uint32_t count);

            static int utfstringEncode(int value, uint8_t* bytes);

            static int utfstringDecode(const uint8_t* bytes, uint32_t length);

            static int utfcharOffset(const char* str, int index);

            /*
            * creates a formatted string. is NOT printf-compatible!
            *
            * #: assume number, or use toString()
            * $: assume string, or use toString()
            * @: value toString()-ified
            *
            * e.g.:
            *   foo = (String instance) "bar"
            *   String::format("foo=@", foo)
            *   => "foo=bar"
            *
            * it's extremely rudimentary, and the idea is to quickly join values.
            */
            static String* format(State* state, const char* format, ...);

            static String* vformat(State* state, const char* format, va_list arg_list);

            static bool equal(State* state, String* a, String* b);


        public:
            /* the hash of this string - note that it is only unique to the context! */
            uint32_t m_hash = 0;

            /* this is handled by sds - use lit_string_length to get the length! */
            std::string* m_chars = nullptr;

        public:
            inline const char* data() const
            {
                if(!m_chars)
                {
                    return nullptr;
                }
                return m_chars->data();
            }

            inline size_t size() const
            {
                if(!m_chars)
                {
                    return 0;
                }
                return m_chars->size();
            }

            inline size_t length() const
            {
                if(!m_chars)
                {
                    return 0;
                }
                return m_chars->size();
            }

            inline int at(size_t i) const
            {
                return (*m_chars)[i];
            }

            void append(std::nullptr_t)
            {
            }

            void append(const char* s, size_t len)
            {
                if(len > 0)
                {
                    m_chars->append(s, len);
                }
            }

            void append(std::string_view sv)
            {
                return append(sv.data(), sv.size());
            }

            void append(String* other)
            {
                if(other != nullptr)
                {
                    append(*(other->m_chars));
                }
            }

            void append(char ch)
            {
                m_chars->append((const char*)&ch, 1);
            }

            bool contains(String* other, bool icase) const
            {
                return contains(other->data(), other->size(), icase);
            }

            bool contains(std::string_view sv, bool icase) const
            {
                return contains(sv.data(), sv.size(), icase);
            }

            bool contains(const char* findme, size_t fmlen, bool icase) const;

            size_t utfLength() const;

            String* codePointAt(uint32_t index) const;

            Array* split(std::string_view sep, bool keepblanc) const;

            inline Array* split(String* sep, bool keepblanc) const
            {
                return split(*(sep->m_chars), keepblanc);
            }
    };

    class Chunk
    {
        public:
            State* m_state;
            /* how many items this chunk holds */
            size_t m_count;
            size_t m_capacity;
            uint8_t* m_code;
            bool m_haslineinfo;
            size_t m_linecount;
            size_t m_linecapacity;
            uint16_t* m_linedata;
            PCGenericArray<Value> m_constants;

        public:
            void init(State* state)
            {
                m_state = state;
                m_count = 0;
                m_capacity = 0;
                m_code = nullptr;
                m_haslineinfo = true;
                m_linecount = 0;
                m_linecapacity = 0;
                m_linedata = nullptr;
                m_constants.init(m_state);
            }

            void release()
            {
                LIT_FREE_ARRAY(m_state, uint8_t, m_code, m_capacity);
                LIT_FREE_ARRAY(m_state, uint16_t, m_linedata, m_linecapacity);
                m_constants.release();
                init(m_state);
            }

            void putChunk(uint8_t byte, uint16_t line)
            {
                if(m_capacity < m_count + 1)
                {
                    size_t old_capacity = m_capacity;
                    m_capacity = LIT_GROW_CAPACITY(old_capacity + 2);
                    m_code = LIT_GROW_ARRAY(m_state, m_code, uint8_t, old_capacity, m_capacity + 2);
                }
                m_code[m_count] = byte;
                m_count++;
                if(!m_haslineinfo)
                {
                    return;
                }

                if(m_linecapacity < m_linecount + 2)
                {
                    size_t old_capacity = m_linecapacity;
                    m_linecapacity = LIT_GROW_CAPACITY(m_linecapacity + 2);
                    m_linedata = LIT_GROW_ARRAY(m_state, m_linedata, uint16_t, old_capacity, m_linecapacity + 2);
                    if(old_capacity == 0)
                    {
                        m_linedata[0] = 0;
                        m_linedata[1] = 0;
                    }
                }
                size_t line_index = m_linecount;
                size_t value = m_linedata[line_index];
                if(value != 0 && value != line)
                {
                    m_linecount += 2;
                    line_index = m_linecount;

                    m_linedata[line_index + 1] = 0;
                }
                m_linedata[line_index] = line;
                m_linedata[line_index + 1]++;
            }

            size_t addConstant(Value constant);

            size_t get_line(size_t offset)
            {
                if(!m_haslineinfo)
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
                    line = m_linedata[index];
                    rle = m_linedata[index + 1];
                    if(rle > 0)
                    {
                        rle--;
                    }
                    index += 2;
                }
                return line;
            }

            void emit_byte(uint8_t byte)
            {
                putChunk(byte, 1);
            }

            void emit_bytes(uint8_t a, uint8_t b)
            {
                putChunk(a, 1);
                putChunk(b, 1);
            }

            void emit_short(uint16_t value)
            {
                emit_bytes((uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
            }
    };

    class Function: public Object
    {
        public:
            static Function* make(State* state, Module* mod);

        public:
            Chunk chunk;
            String* name;
            uint8_t arg_count;
            uint16_t upvalue_count;
            size_t max_slots;
            bool vararg;
            Module* module;
    };

    class Upvalue: public Object
    {
        public:
            static Upvalue* make(State* state, Value* slot);

        public:
            Value* location;
            Value closed;
            Upvalue* next;
    };

    class Closure: public Object
    {
        public:
            static Closure* make(State* state, Function* function);

        public:
            Function* function;
            Upvalue** upvalues;
            size_t upvalue_count;
    };

    class NativeFunction: public Object
    {
        public:
            using FuncType = Value(*)(VM*, size_t, Value*);

        public:
            static NativeFunction* make(State* state, NativeFunction::FuncType function, String* name);

        public:
            /* the native callback for this function */
            FuncType function;
            /* the name of this function */
            String* name;
    };


    class NativePrimFunction: public Object
    {
        public:
            using FuncType = bool(*)(VM*, size_t, Value*);

        public:
            static NativePrimFunction* make(State* state, NativePrimFunction::FuncType function, String* name);

        public:
            FuncType function;
            String* name;
    };

    class NativeMethod: public Object
    {
        public:
            using FuncType = Value(*)(VM*, Value, size_t arg_count, Value*);

        public:
            static NativeMethod* make(State* state, FuncType method, String* name);

            static NativeMethod* make(State* state, FuncType method, std::string_view name);

        public:
            FuncType method;
            String* name;
    };

    class PrimitiveMethod: public Object
    {
        public:
            using FuncType = bool (*)(VM*, Value, size_t, Value*);

        public:
            static PrimitiveMethod* make(State* state, FuncType method, String* name);

            static PrimitiveMethod* make(State* state, FuncType method, std::string_view sv);

        public:
            FuncType method;
            String* name;
    };

    
    class Table
    {
        public:
            struct Entry
            {
                // the key of this entry. can be nullptr!
                String* key = nullptr;

                // the associated value
                Value value = Object::NullVal;
            };

        public:
            template<typename FuncT>
            static void setFunctionValue(Table& dest, String* nm, typename FuncT::FuncType fn)
            {
                dest.set(nm, FuncT::make(dest.m_state, fn, nm)->asValue());
            }

            template<typename FuncT>
            static void setFunctionValue(Table& dest, std::string_view sv, typename FuncT::FuncType fn)
            {
                auto nm = String::copy(dest.m_state, sv.data(), sv.size());
                setFunctionValue<FuncT>(dest, nm, fn);
            }

            static void setNativeMethod(Table& dest, String* nm, NativeMethod::FuncType fn)
            {
                setFunctionValue<NativeMethod>(dest, nm, fn);
            }

            static void setNativeMethod(Table& dest, std::string_view sv, NativeMethod::FuncType fn)
            {
                setFunctionValue<NativeMethod>(dest, sv, fn);
            }

        private:
            static void vmMarkObject(VM* vm, Object* obj);
            static void vmMarkValue(VM* vm, Value val);

        public:
            State* m_state = nullptr;
            PCGenericArray<Entry*> m_inner;

        public:
            void init(State* state)
            {
                m_state = state;
                m_inner.init(state);
            }

            void release();

            void markForGC(VM* vm);

            inline size_t capacity() const
            {
                return m_inner.m_capacity;
            }

            inline size_t size() const
            {
                return m_inner.size();
            }

            inline Entry* at(size_t i)
            {
                return m_inner.at(i);
            }

            inline Entry* at(size_t i) const
            {
                return m_inner.at(i);
            }

            void setField(const char* name, Value value)
            {
                this->set(String::intern(m_state, name), value);
            }

            Value getField(const char* name) const
            {
                Value value;
                if(!this->get(String::intern(m_state, name), &value))
                {
                    value = Object::NullVal;
                }
                return value;
            }

            bool set(String* key, Value value);

            inline bool set(std::string_view sv, Value value)
            {
                return set(String::copy(m_state, sv.data(), sv.size()), value);
            }

            bool get(String* key, Value* value) const;

            bool remove(String* key);

            String* find(const char* str, size_t length, uint32_t hs) const;

            void addAll(Table* from)
            {
                return addAll(*from);
            }

            void addAll(const Table& from);

            void removeWhite();

            int64_t iterator(int64_t number) const;

            Value iterKey(int64_t index) const;
    };

    struct Local
    {
        const char* name;
        size_t length;
        int depth;
        bool captured;
        bool constant;
    };

    class FileIO
    {
        public:
            class EmulatedFile
            {
                public:
                    const char* source;
                    size_t length;
                    size_t position;

                public:
                    void init(const char* source, size_t len)
                    {
                        this->source = source;
                        this->length = len;
                        this->position = 0;
                    }

                    uint8_t read_euint8_t()
                    {
                        return (uint8_t)this->source[this->position++];
                    }

                    uint16_t read_euint16_t()
                    {
                        return (uint16_t)(read_euint8_t() | (read_euint8_t() << 8u));
                    }

                    uint32_t read_euint32_t()
                    {
                        return (uint32_t)(
                            read_euint8_t() |
                            (read_euint8_t() << 8u) |
                            (read_euint8_t() << 16u) |
                            (read_euint8_t() << 24u)
                        );
                    }

                    double read_edouble()
                    {
                        size_t i;
                        double result;
                        uint8_t values[8];
                        for(i = 0; i < 8; i++)
                        {
                            values[i] = read_euint8_t();
                        }
                        memcpy(&result, values, 8);
                        return result;
                    }

                    String* read_estring(State* state)
                    {
                        uint16_t i;
                        uint16_t length;
                        char* line;
                        length = read_euint16_t();
                        if(length < 1)
                        {
                            return nullptr;
                        }
                        line = (char*)malloc(length + 1);
                        for(i = 0; i < length; i++)
                        {
                            line[i] = (char)read_euint8_t() ^ LIT_STRING_KEY;
                        }
                        return String::take(state, line, length);
                    }
            };

        public:
            static size_t write_string(FILE* file, const char* data, size_t len);
            static size_t write_string(FILE* file, std::string_view sv);
            static size_t binwrite_uint8_t(FILE* file, uint8_t byte);
            static size_t binwrite_uint16_t(FILE* file, uint16_t byte);
            static size_t binwrite_uint32_t(FILE* file, uint32_t byte);
            static size_t binwrite_double(FILE* file, double byte);
            static size_t binwrite_string(FILE* file, String* string);
            static uint8_t binread_uint8_t(FILE* file);
            static uint16_t binread_uint16_t(FILE* file);
            static uint32_t binread_uint32_t(FILE* file);
            static double binread_double(FILE* file);
            static String* binread_string(State* state, FILE* file);
    };

    class Map: public Object
    {
        public:
            using IndexFuncType = Value(*)(VM*, Map*, String*, Value*);

        public:
            static Map* make(State* state)
            {
                Map* map;
                map = Object::make<Map>(state, Object::Type::Map);
                map->m_values.init(state);
                map->m_indexfn = nullptr;
                return map;
            }

        public:
            /* the table that holds the actual entries */
            Table m_values;
            /* the index function corresponding to operator[] */
            IndexFuncType m_indexfn;

        public:
            inline size_t capacity() const
            {
                return m_values.capacity();
            }

            inline size_t size() const
            {
                return m_values.size();
            }

            inline Table::Entry* at(size_t i)
            {
                return m_values.at(i);
            }

            inline bool set(String* key, Value value)
            {
                if(value == Object::NullVal)
                {
                    this->remove(key);
                    return false;
                }
                return m_values.set(key, value);
            }

            inline bool get(String* key, Value* value)
            {
                return m_values.get(key, value);
            }

            inline Value getField(State* state, const char* name)
            {
                Value value;
                if(!m_values.get(String::intern(state, name), &value))
                {
                    value = Object::NullVal;
                }
                return value;
            }

            inline bool remove(String* key)
            {
                return m_values.remove(key);
            }

            inline void addAll(Map* other)
            {
                m_values.addAll(&other->m_values);
            }
    };

    class Module: public Object
    {
        public:
            static Module* make(State* state, String* name)
            {
                Module* module;
                module = Object::make<Module>(state, Object::Type::Module);
                module->name = name;
                module->return_value = Object::NullVal;
                module->main_function = nullptr;
                module->privates = nullptr;
                module->ran = false;
                module->main_fiber = nullptr;
                module->private_count = 0;
                module->private_names = Map::make(state);
                return module;
            }

            static Module* getModule(State* state, String* name);

            static Module* getModule(State* state, std::string_view sv)
            {
                return getModule(state, String::intern(state, sv));
            }

        public:
            Value return_value;
            String* name;
            Value* privates;
            Map* private_names;
            size_t private_count;
            Function* main_function;
            Fiber* main_fiber;
            bool ran;
    };

    class Fiber: public Object
    {
        public:
            struct CallFrame
            {
                Function* function;
                Closure* closure;
                uint8_t* ip;
                Value* slots;
                bool result_ignored;
                bool return_to_c;
            };

        public:
            static Fiber* make(State* state, Module* module, Function* function)
            {
                size_t m_stackcapacity;
                Value* stack;
                CallFrame* frame;
                CallFrame* frames;
                Fiber* fiber;
                // Allocate in advance, just in case GC is triggered
                m_stackcapacity = function == nullptr ? 1 : (size_t)lit_closest_power_of_two(function->max_slots + 1);
                stack = LIT_ALLOCATE(state, Value, m_stackcapacity);
                frames = LIT_ALLOCATE(state, CallFrame, LIT_INITIAL_CALL_FRAMES);
                fiber = Object::make<Fiber>(state, Object::Type::Fiber);
                if(module != nullptr)
                {
                    if(module->main_fiber == nullptr)
                    {
                        module->main_fiber = fiber;
                    }
                }
                fiber->m_stackdata = stack;
                fiber->m_stackcapacity = m_stackcapacity;
                fiber->m_stacktop = fiber->m_stackdata;
                fiber->m_allframes = frames;
                fiber->m_framecapacity = LIT_INITIAL_CALL_FRAMES;
                fiber->m_parent = nullptr;
                fiber->m_framecount = 1;
                fiber->m_argcount = 0;
                fiber->m_module = module;
                fiber->m_havecatcher = false;
                fiber->m_error = Object::NullVal;
                fiber->m_openupvalues = nullptr;
                fiber->m_isaborting = false;
                frame = &fiber->m_allframes[0];
                frame->closure = nullptr;
                frame->function = function;
                frame->slots = fiber->m_stackdata;
                frame->result_ignored = false;
                frame->return_to_c = false;
                if(function != nullptr)
                {
                    frame->ip = function->chunk.m_code;
                }
                return fiber;
            }

            static bool ensureFiber(State* state, VM* vm, Fiber* fiber)
            {
                size_t newcapacity;
                size_t osize;
                size_t newsize;
                if(fiber == nullptr)
                {
                    lit_runtime_error(vm, "no fiber to run on");
                    return true;
                }
                if(fiber->m_framecount == LIT_CALL_FRAMES_MAX)
                {
                    lit_runtime_error(vm, "fiber frame overflow");
                    return true;
                }
                if(fiber->m_framecount + 1 > fiber->m_framecapacity)
                {
                    //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->m_framecapacity * 2);
                    newcapacity = (fiber->m_framecapacity * 2) + 1;
                    osize = (sizeof(CallFrame) * fiber->m_framecapacity);
                    newsize = (sizeof(CallFrame) * newcapacity);
                    fiber->m_allframes = (CallFrame*)Memory::reallocate(state, fiber->m_allframes, osize, newsize);
                    fiber->m_framecapacity = newcapacity;
                }
                return false;
            }

            static bool ensureFiber(VM* vm, Fiber* fiber);

        public:
            Fiber* m_parent = nullptr;
            Value* m_stackdata = nullptr;
            Value* m_stacktop = nullptr;
            size_t m_stackcapacity = 0;
            CallFrame* m_allframes = nullptr;
            size_t m_framecapacity = 0;
            size_t m_framecount = 0;
            size_t m_argcount = 0;
            Upvalue* m_openupvalues = nullptr;
            Module* m_module = nullptr;
            Value m_error = Object::NullVal;
            bool m_isaborting = false;
            bool m_havecatcher = false;

        public:
            void ensure_stack(size_t needed)
            {
                size_t i;
                size_t capacity;
                Value* old_stack;
                Upvalue* upvalue;
                if(m_stackcapacity >= needed)
                {
                    return;
                }
                capacity = (size_t)lit_closest_power_of_two((int)needed);
                old_stack = m_stackdata;
                m_stackdata = (Value*)Memory::reallocate(m_state, m_stackdata, sizeof(Value) * m_stackcapacity, sizeof(Value) * capacity);
                m_stackcapacity = capacity;
                if(m_stackdata != old_stack)
                {
                    for(i = 0; i < m_framecapacity; i++)
                    {
                        CallFrame* frame = &m_allframes[i];
                        frame->slots = m_stackdata + (frame->slots - old_stack);
                    }
                    for(upvalue = m_openupvalues; upvalue != nullptr; upvalue = upvalue->next)
                    {
                        upvalue->location = m_stackdata + (upvalue->location - old_stack);
                    }
                    m_stacktop = m_stackdata + (m_stacktop - old_stack);
                }
            }

            void push(Value val)
            {
                *m_stacktop++ = val;
            }
    };

    class Class: public Object
    {
        private:
            static Class* fromInstance(Value instance);

        public:
            static Value defaultfn_tostring(VM* vm, Value instance, size_t argc, Value* argv)
            {
                Class* selfklass;
                String* name;
                String* rt;
                const char* fmtpat;
                (void)argc;
                (void)argv;
                fmtpat = "[class ";
                name = String::intern(Object::asState(vm), "?unknown?");
                // this assumes toString() wasn't overriden
                if(!Object::isNull(instance))
                {
                    if(Object::isClass(instance))
                    {
                        selfklass = fromInstance(instance);
                    }
                    else
                    {
                        selfklass = Object::as<Class>(instance);
                        fmtpat = "[instance ";
                    }
                    name = selfklass->name;
                }
                //return String::format(Object::asState(vm), fmtpat, name->asValue())->asValue();
                rt = String::make(Object::asState(vm));
                rt->append(fmtpat);
                rt->append(name);
                rt->append("]");
                return rt->asValue();
            }

            static Class* getClassFor(State* state, Value value);

            static Class* make(State* state, String* name)
            {
                Class* klass;
                klass = Object::make<Class>(state, Object::Type::Class);
                klass->name = name;
                klass->init_method = nullptr;
                klass->super = nullptr;
                klass->methods.init(state);
                klass->static_fields.init(state);
                klass->bindMethod("toString", defaultfn_tostring);
                klass->setStaticMethod("toString", defaultfn_tostring);
                return klass;
            }

            static Class* make(State* state, std::string_view name)
            {
                auto nm = String::copy(state, name.data(), name.size());
                return Class::make(state, nm);
            }

        public:
            /* the name of this class */
            String* name = nullptr;
            /* the constructor object */
            Object* init_method = nullptr;
            /* runtime methods */
            Table methods;
            /* static fields, which include functions, and variables */
            Table static_fields;
            /*
            * the parent class - the topmost is always Class, followed by Object.
            * that is, eg for String: String <- Object <- Class
            */
            Class* super = nullptr;

        public:

        public:
            void inheritFrom(Class* superclass)
            {
                if(superclass != nullptr)
                {
                    //fprintf(stderr, "Class(%s)::inheritFrom(%s)\n", name->data(), superclass->name->data());
                    this->super = superclass;
                    if(this->init_method == nullptr)
                    {
                        this->init_method = superclass->init_method;
                    }
                    if(superclass->methods.size() > 0)
                    {
                        this->methods.addAll(superclass->methods);
                    }
                    if(superclass->static_fields.size() > 0)
                    {
                        this->static_fields.addAll(superclass->static_fields);
                    }
                }
            }

            void bindConstructor(NativeMethod::FuncType method)
            {
                auto nm = String::copy(m_state, LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1);
                auto m = NativeMethod::make(m_state, method, nm);
                this->init_method = (Object*)m;
                this->methods.set(nm, m->asValue());
            }

            void setField(const char* name, Value val)
            {
                this->static_fields.setField(name, val);
            }

            void bindField(String* nm, NativeMethod::FuncType fnget, NativeMethod::FuncType fnset)
            {
                this->methods.set(nm,
                    Field::make(m_state, nm,
                        (Object*)NativeMethod::make(m_state, fnget, nm),
                        (Object*)NativeMethod::make(m_state, fnset, nm))->asValue());
            }

            void bindField(std::string_view sv, NativeMethod::FuncType fnget, NativeMethod::FuncType fnset)
            {
                bindField(String::copy(m_state, sv.data(), sv.size()), fnget, fnset);
            }


            void bindMethod(String* nm, NativeMethod::FuncType method)
            {
                this->methods.set(nm, NativeMethod::make(m_state, method, name)->asValue());
            }

            void bindMethod(std::string_view sv, NativeMethod::FuncType method)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                bindMethod(nm, method);
            }


            void bindPrimitive(String* nm, PrimitiveMethod::FuncType method)
            {
                this->methods.set(nm, PrimitiveMethod::make(m_state, method, nm)->asValue());
            }

            void bindPrimitive(std::string_view sv, PrimitiveMethod::FuncType method)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                bindPrimitive(nm, method);
            }

            void setStaticField(String* nm, NativeMethod::FuncType fnget, NativeMethod::FuncType fnset)
            {
                this->static_fields.set(nm,
                    Field::make(m_state, nm,
                        (Object*)NativeMethod::make(m_state, fnget, nm),
                        (Object*)NativeMethod::make(m_state, fnset, nm))->asValue());
            }

            void setStaticField(std::string_view sv, NativeMethod::FuncType fnget, NativeMethod::FuncType fnset)
            {
                setStaticField(String::copy(m_state, sv.data(), sv.size()), fnget, fnset);
            }

            void setStaticMethod(String* nm, NativeMethod::FuncType fn)
            {
                Table::setNativeMethod(this->static_fields, nm, fn);
            }

            void setStaticMethod(std::string_view sv, NativeMethod::FuncType fn)
            {
                Table::setNativeMethod(this->static_fields, sv, fn);
            }

            void setStaticPrimitive(String* nm, PrimitiveMethod::FuncType fn)
            {
                Table::setFunctionValue<PrimitiveMethod>(this->static_fields, nm, fn);
            }

            void setStaticPrimitive(std::string_view sv, PrimitiveMethod::FuncType fn)
            {
                Table::setFunctionValue<PrimitiveMethod>(this->static_fields, sv, fn);
            }

            void setStaticSetter(String* nm, NativeMethod::FuncType fn)
            {
                this->static_fields.set(nm,
                    Field::make(m_state, nm, nullptr, (Object*)NativeMethod::make(m_state, fn, nm))->asValue());
            }

            void setStaticSetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                return setStaticSetter(nm, fn);
            }

            void setStaticGetter(String* nm, NativeMethod::FuncType fn)
            {
                this->static_fields.set(nm,
                    Field::make(m_state, nm,
                        (Object*)NativeMethod::make(m_state, fn, nm),
                        nullptr)->asValue());
            }

            void setStaticGetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                return setStaticGetter(nm, fn);
            }

            void setGetter(String* nm, NativeMethod::FuncType fn)
            {
                this->methods.set(nm, Field::make(m_state, nm, NativeMethod::make(m_state, fn, nm), nullptr)->asValue());
            }

            void setGetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                setGetter(nm, fn);
            }

            void setSetter(String* nm, NativeMethod::FuncType fn)
            {
                this->methods.set(nm, Field::make(m_state, nm, nullptr, NativeMethod::make(m_state, fn, nm))->asValue());
            }

            void setSetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                setSetter(nm, fn);
            }

    };

    class Instance: public Object
    {
        public:
            static Instance* make(State* state, Class* klass)
            {
                Instance* instance;
                instance = Object::make<Instance>(state, Object::Type::Instance);
                instance->klass = klass;
                instance->fields.init(state);
                return instance;
            }

            static Value getMethod(State* state, Value callee, String* mthname)
            {
                Value mthval;
                Class* klass;
                klass = Class::getClassFor(state, callee);
                if((Object::isInstance(callee) && Object::as<Instance>(callee)->fields.get(mthname, &mthval)) || klass->methods.get(mthname, &mthval))
                {
                    return mthval;
                }
                return Object::NullVal;
            }

            static Result callMethod(State* state, Value callee, String* mthname, Value* argv, size_t argc);

        public:
            /* the class that corresponds to this instance */
            Class* klass;
            Table fields;
    };

    class BoundMethod: public Object
    {
        public:
            static BoundMethod* make(State* state, Value receiver, Value method)
            {
                BoundMethod* bound_method;
                bound_method = Object::make<BoundMethod>(state, Object::Type::BoundMethod);
                bound_method->receiver = receiver;
                bound_method->method = method;
                return bound_method;
            }

        public:
            Value receiver;
            Value method;
    };

    class Userdata: public Object
    {
        public:
            using CleanupFuncType = void(*)(State*, Userdata*, bool);

        public:
            static Userdata* make(State* state, size_t size, bool ispointeronly)
            {
                Userdata* userdata;
                userdata = Object::make<Userdata>(state, Object::Type::Userdata);
                userdata->data = nullptr;
                if(size > 0)
                {
                    if(!ispointeronly)
                    {
                        userdata->data = Memory::reallocate(state, nullptr, 0, size);
                    }
                }
                userdata->size = size;
                userdata->cleanup_fn = nullptr;
                userdata->canfree = true;
                return userdata;
            }

        public:
            void* data;
            size_t size;
            CleanupFuncType cleanup_fn;
            bool canfree;
    };

    class Range: public Object
    {
        public:
            static Range* make(State* state, double from, double to)
            {
                Range* range;
                range = Object::make<Range>(state, Object::Type::Range);
                range->from = from;
                range->to = to;
                return range;
            }

        public:
            double from;
            double to;
    };

    class Reference: public Object
    {
        public:
            static Reference* make(State* state, Value* slot)
            {
                Reference* reference;
                reference = Object::make<Reference>(state, Object::Type::Reference);
                reference->slot = slot;
                return reference;
            }


        public:
            Value* slot;
    };

    class State
    {
        public:
            using ErrorFuncType = void(*)(State*, const char*);
            using PrintFuncType = void(*)(State*, const char*);

        public:
            static void default_error(State* state, const char* message)
            {
                (void)state;
                fflush(stdout);
                fprintf(stderr, "%s%s%s\n", COLOR_RED, message, COLOR_RESET);
                fflush(stderr);
            }

            static void default_printf(State* state, const char* message)
            {
                (void)state;
                printf("%s", message);
            }

            static void init_api(State* state)
            {
                state->api_name = String::copy(state, "c", 1);
                state->api_function = nullptr;
                state->api_fiber = nullptr;
            }

            static State* make();


        public:
            /* how much was allocated in total? */
            int64_t bytes_allocated;
            int64_t next_gc;
            bool allow_gc;
            ErrorFuncType error_fn;
            PrintFuncType print_fn;
            Value* roots;
            size_t root_count;
            size_t root_capacity;
            AST::Preprocessor* preprocessor;
            AST::Scanner* scanner;
            AST::Parser* parser;
            AST::Emitter* emitter;
            AST::Optimizer* optimizer;
            /*
            * recursive pointer to the current VM instance.
            * using 'state->vm->m_state' will in turn mean this instance, etc.
            */
            VM* vm;
            bool m_haderror;
            
            jmp_buf jump_buffer;

            Function* api_function;
            Fiber* api_fiber;
            String* api_name;
            /* when using debug routines, this is the writer that output is called on */
            Writer debugwriter;
            // class class
            // Mental note:
            // When adding another class here, DO NOT forget to mark it in lit_mem.c or it will be GC-ed
            Class* kernelvalue_class = nullptr;
            Class* classvalue_class = nullptr;
            Class* objectvalue_class = nullptr;
            Class* numbervalue_class = nullptr;
            Class* stringvalue_class = nullptr;
            Class* boolvalue_class = nullptr;
            Class* functionvalue_class = nullptr;
            Class* fibervalue_class = nullptr;
            Class* modulevalue_class = nullptr;
            Class* arrayvalue_class = nullptr;
            Class* mapvalue_class = nullptr;
            Class* rangevalue_class = nullptr;
            Module* last_module;

        public:
            void init(VM* vm);
            int64_t release();

            Fiber* getVMFiber();

            void native_exit_jump()
            {
                longjmp(jump_buffer, 1);
            }

            bool set_native_exit_jump()
            {
                return setjmp(jump_buffer);
            }

            void showDecompiled();

            void raiseError(ErrorType type, const char* message, ...);

            void releaseAPI();

            /* releases given objects, and their subobjects. */
            void releaseObjects(Object* objects);

            void pushRoot(Object* obj);

            void pushValueRoot(Value value);

            Value peekRoot(uint8_t distance);

            void popRoot();

            void popRoots(uint8_t amount);

            void setVMGlobal(String* name, Value val);

            bool getVMGlobal(String* name, Value* dest);

            Value getGlobal(String* name);

            Function* getGlobalFunction(String* name);

            void setGlobal(String* name, Value value);

            bool globalExists(String* name);

            void defineNative(const char* name, NativeFunction::FuncType native);

            void defineNativePrimitive(const char* name, NativePrimFunction::FuncType native);

            Upvalue* captureUpvalue(Value* local);

            Result execModule(Module* module);

            Result execFiber(Fiber* fiber);

            Fiber::CallFrame* setupCall(Function* callee, Value* argv, size_t argc);

            Result execCall(Fiber::CallFrame* frame);

            Result callFunction(Function* callee, Value* argv, size_t argc)
            {
                return execCall(this->setupCall(callee, argv, argc));
            }

            Result callClosure(Closure* callee, Value* argv, size_t argc);

            Result callMethod(Value instance, Value callee, Value* argv, size_t argc);

            Result call(Value callee, Value* argv, size_t argc)
            {
                return this->callMethod(callee, callee, argv, argc);
            }

            Result findAndCallMethod(Value callee, String* method_name, Value* argv, size_t argc);

            Result findAndCallMethod(Value callee, std::string_view mthname, Value* argv, size_t argc)
            {
                return findAndCallMethod(callee, String::intern(this, mthname), argv, argc);
            }

            Module* compileModule(String* module_name, const char* code, size_t length);

            Module* compileModule(String* modname, std::string_view code)
            {
                return compileModule(modname, code.data(), code.size());
            }

            char* readSource(std::string_view path, size_t* destlen, char** patched_file_name);

            Result interpretFile(std::string_view path);
            
            Result interpretSource(std::string_view modname, const char* code, size_t length)
            {
                return internalInterpret(String::copy(this, modname.data(), modname.size()), code, length);
            }

            Result interpretSource(std::string_view modname, std::string_view code)
            {
                return interpretSource(modname, code.data(), code.size());
            }

            Result internalInterpret(String* module_name, const char* code, size_t length);

    };

    class VM
    {
        public:
            /* the current state */
            State* m_state;
            /* currently held objects */
            Object* objects;
            /* currently cached strings */
            Table strings;
            /* currently loaded/defined modules */
            Map* modules;
            /* currently defined globals */
            Map* globals;
            Fiber* fiber;
            // For garbage collection
            size_t gray_count;
            size_t gray_capacity;
            Object** gray_stack;

        public:
            void release()
            {
                this->strings.release();
                m_state->releaseObjects(this->objects);
                this->reset(m_state);
            }

            void reset(State* state)
            {
                m_state = state;
                this->objects = nullptr;
                this->fiber = nullptr;
                this->gray_stack = nullptr;
                this->gray_count = 0;
                this->gray_capacity = 0;
                this->strings.init(state);
                this->globals = nullptr;
                this->modules = nullptr;
            }

            inline void push(Value value)
            {
                *(fiber->m_stacktop) = value;
                fiber->m_stacktop++;
            }

            inline Value pop()
            {
                Value rt;
                rt = *(fiber->m_stacktop);
                fiber->m_stacktop--;
                return rt;
            }

            void closeUpvalues(const Value* last);

            bool dispatchCall(Function* function, Closure* closure, uint8_t arg_count);

            bool callValue(std::string_view name, Value callee, uint8_t arg_count);

            void markObject(Object* obj);

            void markValue(Value value);

            void markArray(PCGenericArray<Value>* array);

            void markRoots();

            void blackenObject(Object* obj);

            void traceReferences();

            void sweep();

            uint64_t collectGarbage();
    };

    class BinaryData
    {
        public:
            static void storeFunction(FILE* file, Function* function)
            {
                storeChunk(file, &function->chunk);
                FileIO::binwrite_string(file, function->name);
                FileIO::binwrite_uint8_t(file, function->arg_count);
                FileIO::binwrite_uint16_t(file, function->upvalue_count);
                FileIO::binwrite_uint8_t(file, (uint8_t)function->vararg);
                FileIO::binwrite_uint16_t(file, (uint16_t)function->max_slots);
            }

            static Function* loadFunction(State* state, FileIO::EmulatedFile* file, Module* module)
            {
                Function* function = Function::make(state, module);
                loadChunk(state, file, module, &function->chunk);
                function->name = file->read_estring(state);
                function->arg_count = file->read_euint8_t();
                function->upvalue_count = file->read_euint16_t();
                function->vararg = (bool)file->read_euint8_t();
                function->max_slots = file->read_euint16_t();
                return function;
            }

            static void storeChunk(FILE* file, Chunk* chunk)
            {
                size_t i;
                size_t c;
                FileIO::binwrite_uint32_t(file, chunk->m_count);
                for(i = 0; i < chunk->m_count; i++)
                {
                    FileIO::binwrite_uint8_t(file, chunk->m_code[i]);
                }
                if(chunk->m_haslineinfo)
                {
                    c = chunk->m_linecount * 2 + 2;
                    FileIO::binwrite_uint32_t(file, c);
                    for(i = 0; i < c; i++)
                    {
                        FileIO::binwrite_uint16_t(file, chunk->m_linedata[i]);
                    }
                }
                else
                {
                    FileIO::binwrite_uint32_t(file, 0);
                }
                FileIO::binwrite_uint32_t(file, chunk->m_constants.m_count);
                for(i = 0; i < chunk->m_constants.m_count; i++)
                {
                    Value constant = chunk->m_constants.m_values[i];
                    if(Object::isObject(constant))
                    {
                        Object::Type type = Object::asObject(constant)->type;
                        FileIO::binwrite_uint8_t(file, (uint8_t)(type) + 1);
                        switch(type)
                        {
                            case Object::Type::String:
                                {
                                    FileIO::binwrite_string(file, Object::as<String>(constant));
                                }
                                break;
                            case Object::Type::Function:
                                {
                                    storeFunction(file, Object::as<Function>(constant));
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
                        FileIO::binwrite_uint8_t(file, 0);
                        FileIO::binwrite_double(file, Object::toNumber(constant));
                    }
                }
            }

            static void loadChunk(State* state, FileIO::EmulatedFile* file, Module* module, Chunk* chunk)
            {
                size_t i;
                size_t count;
                uint8_t type;
                chunk->init(state);
                count = file->read_euint32_t();
                chunk->m_code = (uint8_t*)Memory::reallocate(state, nullptr, 0, sizeof(uint8_t) * count);
                chunk->m_count = count;
                chunk->m_capacity = count;
                for(i = 0; i < count; i++)
                {
                    chunk->m_code[i] = file->read_euint8_t();
                }
                count = file->read_euint32_t();
                if(count > 0)
                {
                    chunk->m_linedata = (uint16_t*)Memory::reallocate(state, nullptr, 0, sizeof(uint16_t) * count);
                    chunk->m_linecount = count;
                    chunk->m_linecapacity = count;
                    for(i = 0; i < count; i++)
                    {
                        chunk->m_linedata[i] = file->read_euint16_t();
                    }
                }
                else
                {
                    chunk->m_haslineinfo = false;
                }
                count = file->read_euint32_t();
                chunk->m_constants.m_values = (Value*)Memory::reallocate(state, nullptr, 0, sizeof(Value) * count);
                chunk->m_constants.m_count = count;
                chunk->m_constants.m_capacity = count;
                for(i = 0; i < count; i++)
                {
                    type = file->read_euint8_t();
                    if(type == 0)
                    {
                        chunk->m_constants.m_values[i] = Object::toValue(file->read_edouble());
                    }
                    else
                    {
                        switch((Object::Type)(type - 1))
                        {
                            case Object::Type::String:
                                {
                                    chunk->m_constants.m_values[i] = file->read_estring(state)->asValue();
                                }
                                break;
                            case Object::Type::Function:
                                {
                                    chunk->m_constants.m_values[i] = loadFunction(state, file, module)->asValue();
                                }
                                break;
                            default:
                                {
                                    /* UNREACHABLE */
                                }
                                break;

                        }
                    }
                }
            }

            static void storeModule(Module* module, FILE* file);

            static Module* loadModule(State* state, const char* input, size_t length)
            {
                bool enabled;
                uint16_t i;
                uint16_t j;
                uint16_t module_count;
                uint16_t privates_count;
                uint8_t bytecode_version;
                String* name;
                Table* privates;
                Module* module;
                FileIO::EmulatedFile file;
                file.init(input, length);
                if(file.read_euint16_t() != LIT_BYTECODE_MAGIC_NUMBER)
                {
                    state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown magic number");
                    return nullptr;
                }
                bytecode_version = file.read_euint8_t();
                if(bytecode_version > LIT_BYTECODE_VERSION)
                {
                    state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown bytecode version '%i'", (int)bytecode_version);
                    return nullptr;
                }
                module_count = file.read_euint16_t();
                Module* first = nullptr;
                for(j = 0; j < module_count; j++)
                {
                    module = Module::make(state, file.read_estring(state));
                    privates = &module->private_names->m_values;
                    privates_count = file.read_euint16_t();
                    enabled = !((bool)file.read_euint8_t());
                    module->privates = LIT_ALLOCATE(state, Value, privates_count);
                    module->private_count = privates_count;
                    for(i = 0; i < privates_count; i++)
                    {
                        module->privates[i] = Object::NullVal;
                        if(enabled)
                        {
                            name = file.read_estring(state);
                            privates->set(name, Object::toValue(file.read_euint16_t()));
                        }
                    }
                    module->main_function = loadFunction(state, &file, module);
                    state->vm->modules->m_values.set(module->name, module->asValue());
                    if(j == 0)
                    {
                        first = module;
                    }
                }
                if(file.read_euint16_t() != LIT_BYTECODE_END_NUMBER)
                {
                    state->raiseError(COMPILE_ERROR, "Failed to read compiled code, unknown end number");
                    return nullptr;
                }
                return first;
            }
    };

    template<typename Type>
    template<typename... ArgsT>
    inline void PCGenericArray<Type>::raiseError(const char* fmt, ArgsT&&... args)
    {
        lit_runtime_error(m_state->vm, fmt, std::forward<ArgsT>(args)...);
    }

    //prototypes

    /**
    * utility functions
    */
    void util_custom_quick_sort(VM *vm, Value *l, int length, Value callee);
    bool util_is_fiber_done(Fiber *fiber);
    void util_run_fiber(VM *vm, Fiber *fiber, Value *argv, size_t argc, bool catcher);
    void util_basic_quick_sort(State *state, Value *clist, int length);
    bool util_interpret(VM *vm, Module *module);
    bool util_test_file_exists(const char *filename);
    bool util_attempt_to_require(VM *vm, Value *argv, size_t argc, const char *path, bool ignore_previous, bool folders);
    bool util_attempt_to_require_combined(VM *vm, Value *argv, size_t argc, const char *a, const char *b, bool ignore_previous);
    Value util_invalid_constructor(VM *vm, Value instance, size_t argc, Value *argv);

    /**
    * state functions
    */

    /*
     * Please, do not provide a const string source to the compiler, because it will
     * get modified, if it has any macros in it!
     */

    Result lit_dump_file(State* state, const char* file);
    bool lit_compile_and_save_files(State* state, char* files[], size_t num_files, const char* output_file);



    void lit_trace_vm_stack(VM* vm, Writer* wr);

    double lit_check_number(VM* vm, Value* args, uint8_t arg_count, uint8_t id);
    double lit_get_number(VM* vm, Value* args, uint8_t arg_count, uint8_t id, double def);

    bool lit_check_bool(VM* vm, Value* args, uint8_t arg_count, uint8_t id);
    bool lit_get_bool(VM* vm, Value* args, uint8_t arg_count, uint8_t id, bool def);

    const char* lit_check_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id);
    const char* lit_get_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id, const char* def);

    String* lit_check_object_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id);
    Instance* lit_check_instance(VM* vm, Value* args, uint8_t arg_count, uint8_t id);
    Value* lit_check_reference(VM* vm, Value* args, uint8_t arg_count, uint8_t id);

    void lit_ensure_bool(VM* vm, Value value, const char* error);
    void lit_ensure_string(VM* vm, Value value, const char* error);
    void lit_ensure_number(VM* vm, Value value, const char* error);
    void lit_ensure_object_type(VM* vm, Value value, Object::Type type, const char* error);


    void lit_trace_frame(Fiber* fiber, Writer* wr);


    bool lit_file_exists(const char* path);
    bool lit_dir_exists(const char* path);


    bool lit_generate_source_file(const char* file, const char* output);
    void lit_build_native_runner(const char* bytecode_file);

    namespace Builtins
    {
        void lit_open_libraries(State* state);
        void lit_open_core_library(State* state);
        void lit_open_math_library(State* state);
        void lit_open_file_library(State* state);
        void lit_open_gc_library(State* state);
        void lit_open_string_library(State* state);
        void lit_open_array_library(State* state);
        void lit_open_map_library(State* state);
        void lit_open_range_library(State* state);
        void lit_open_fiber_library(State* state);
        void lit_open_module_library(State* state);
        void lit_open_function_library(State* state);
        void lit_open_class_library(State* state);
        void lit_open_object_library(State* state);
        void lit_open_kernel_library(State* state);
    }

    void lit_add_definition(State* state, const char* name);

    bool lit_preprocess(AST::Preprocessor* preprocessor, char* source);
}

