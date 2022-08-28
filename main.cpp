
#include <string>
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



#define TAG_NULL 1u
#define TAG_FALSE 2u
#define TAG_TRUE 3u

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

#define BOOL_VALUE(boolean) \
    ((boolean) ? lit::Object::TrueVal : lit::Object::FalseVal)


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

#define OBJECT_TYPE(value) \
    (lit::Object::asObject(value)->type)


#define INTERPRET_RUNTIME_FAIL ((lit::Result){ LITRESULT_INVALID, lit::Object::NullVal })

#define LIT_GET_FIELD(id) lit::Object::as<lit::Instance>(instance)->fields.getField(vm->m_state, id)
#define LIT_GET_MAP_FIELD(id) lit::Object::as<lit::Instance>(instance).fields.getField(vm->m_state, id))
#define LIT_SET_FIELD(id, value) lit::Object::as<lit::Instance>(instance)->fields.setField(id, value)
#define LIT_SET_MAP_FIELD(id, value) lit_set_map_field(vm->m_state, &lit::Object::as<lit::Instance>(instance)->fields, id, value)

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


#define PUSH(value) (*fiber->stack_top++ = value)

#define RETURN_OK(r) return Result{ LITRESULT_OK, r };

#define RETURN_RUNTIME_ERROR() return Result{ LITRESULT_RUNTIME_ERROR, Object::NullVal };

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
        static bool measure_compilation_time = true;
        static double last_source_time = 0;

        class /**/Parser;
        class /**/Scanner;
        class /**/Preprocessor;
        class /**/Compiler;
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
        char* copyString(std::string_view sv)
        {
            size_t length;
            char* rtbuf;
            length = sv.size() + 1;
            rtbuf = (char*)malloc(length);
            memcpy(rtbuf, sv.data(), length);
            return rtbuf;
        }

        char* readFile(const char* path, size_t* destlen)
        {
            size_t toldsz;
            size_t actualsz;
            char* buffer;
            FILE* hnd;
            hnd = fopen(path, "rb");
            if(hnd == nullptr)
            {
                return nullptr;
            }
            fseek(hnd, 0L, SEEK_END);
            toldsz = ftell(hnd);
            rewind(hnd);
            buffer = (char*)malloc(toldsz + 1);
            actualsz = fread(buffer, sizeof(char), toldsz, hnd);
            *destlen = actualsz;
            buffer[actualsz] = '\0';
            fclose(hnd);
            return buffer;
        }


        char* patchFilename(char* file_name)
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

            inline constexpr size_t capacity() const
            {
                return m_capacity;
            }

            inline constexpr size_t size() const
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
                if(m_capacity < (m_count + 1))
                {
                    oldcap = m_capacity;
                    m_capacity = LIT_GROW_CAPACITY(oldcap);
                    m_values = LIT_GROW_ARRAY(m_state, m_values, ElementT, oldcap, m_capacity);
                    if(m_values == nullptr)
                    {
                        raiseError("GenericArray: FAILED to grow array!");
                    }
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
                return (
                    (Object::isBool(value) && value == Object::FalseVal) ||
                    Object::isNull(value) ||
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

            static bool isCallableFunction(Value value)
            {
                if(Object::isObject(value))
                {
                    Object::Type type = OBJECT_TYPE(value);
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
                if(Object::isBool(value))
                {
                    return "bool";
                }
                else if(Object::isNull(value))
                {
                    return "null";
                }
                else if(Object::isNumber(value))
                {
                    return "number";
                }
                else if(Object::isObject(value))
                {
                    return object_type_names[(int)OBJECT_TYPE(value)];
                }
                return "unknown";
            }

            // a helper to get State from a VM pointer
            static inline State* asState(VM* vm);

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
            static Field* make(State* state, Object* getter, Object* setter)
            {
                Field* field;
                field = Object::make<Field>(state, Object::Type::Field);
                field->getter = getter;
                field->setter = setter;
                return field;
            }

        public:
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
                this->m_actualarray.push(val);
            }

            inline constexpr size_t size()
            {
                return m_actualarray.m_count;
            }

            inline constexpr Value at(size_t idx)
            {
                return m_actualarray.at(idx);
            }

            int indexOf(Value value)
            {
                size_t i;
                for(i = 0; i < this->m_actualarray.m_count; i++)
                {
                    if(this->m_actualarray.m_values[i] == value)
                    {
                        return (int)i;
                    }
                }
                return -1;
            }

            Value removeAt(size_t index)
            {
                size_t i;
                size_t count;
                Value value;
                PCGenericArray<Value>* values;
                values = &this->m_actualarray;
                count = values->m_count;
                if(index >= count)
                {
                    return Object::NullVal;
                }
                value = values->m_values[index];
                if(index == count - 1)
                {
                    values->m_values[index] = Object::NullVal;
                }
                else
                {
                    for(i = index; i < values->m_count - 1; i++)
                    {
                        values->m_values[i] = values->m_values[i + 1];
                    }
                    values->m_values[count - 1] = Object::NullVal;
                }
                values->m_count--;
                return value;
            }
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
                /*
                size_t i;
                uint32_t hs = 2166136261u;
                for(i = 0; i < length; i++)
                {
                    hs ^= str[i];
                    hs *= 16777619;
                }
                return hs;
                */
                return makeHash(std::string_view(str, length));
            }

            static String* allocEmpty(State* state, size_t length)
            {
                String* string;
                string = Object::make<String>(state, Object::Type::String);
                {
                    // important: since String is not created via new(), non-primitives and
                    // non-pointers (like std::string) are *not* allocated until they're
                    // explicitly initialized.
                    string->m_chars = new std::string();
                    /* reserving the required space may reduce number of allocations */
                    string->m_chars->reserve(length);
                }
                string->m_hash = 0;
                return string;
            }

            static String* allocate(State* state, const char* chars, size_t length, uint32_t hs, bool mustfree)
            {
                String* string;
                string = String::allocEmpty(state, length);
                {
                    /*
                    * string->m_chars is initialized in String::allocEmpty(),
                    * as an empty string!
                    */
                    string->m_chars->append(chars, length);
                }
                string->m_hash = hs;
                if(mustfree)
                {
                    LIT_FREE(state, char, chars);
                }
                String::statePutInterned(state, string);
                return string;
            }

            static void statePutInterned(State* state, String* string);

            static bool stateGetGCAllowed(State* state);

            static void stateSetGCAllowed(State* state, bool v);

            static String* stateFindInterned(State* state, const char* chars, size_t length, uint32_t hs);

            /*
            * create a String by reusing $chars. ONLY use this if you're certain that $chars isn't being freed.
            * if $wassds is true, then the sds instance is reused as-is.
            */
            /* todo: track if $chars is a sds instance - additional argument $fromsds? */
            static String* take(State* state, char* chars, size_t length)
            {
                uint32_t hs;
                hs = String::makeHash(chars, length);
                String* interned;
                interned = stateFindInterned(state, chars, length, hs);
                if(interned != nullptr)
                {
                    LIT_FREE(state, char, chars);
                    return interned;
                }
                return String::allocate(state, (char*)chars, length, hs, true);
            }

            /* copy a string, creating a full newly allocated String. */
            static String* copy(State* state, const char* chars, size_t length)
            {
                uint32_t hs;
                String* interned;
                hs = String::makeHash(chars, length);
                interned = stateFindInterned(state, chars, length, hs);
                if(interned != nullptr)
                {
                    return interned;
                }
                return String::allocate(state, chars, length, hs, false);
            }

            static String* copy(State* state, std::string_view sv)
            {
                return String::copy(state, sv.data(), sv.size());
            }

            static String* make(State* state)
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

            static Value stringNumberToString(State* state, double value)
            {
                if(isnan(value))
                {
                    return String::internValue(state, "nan");
                }

                if(isinf(value))
                {
                    if(value > 0.0)
                    {
                        return String::internValue(state, "infinity");
                    }
                    else
                    {
                        return String::internValue(state, "-infinity");
                    }
                }
                char buffer[24];
                int length = sprintf(buffer, "%.14g", value);
                return String::copy(state, buffer, length)->asValue();
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

            static String* fromCodePoint(State* state, int value)
            {
                int length;
                char* bytes;
                String* rt;
                length = String::encodeNumBytes(value);
                bytes = LIT_ALLOCATE(state, char, length + 1);
                String::utfstringEncode(value, (uint8_t*)bytes);
                /* this should be String::take, but something prevents the memory from being free'd. */
                rt = String::copy(state, bytes, length);
                LIT_FREE(state, char, bytes);
                return rt;
            }

            static String* fromRange(State* state, String* source, int start, uint32_t count)
            {
                int length;
                int index;
                int code_point;
                uint32_t i;
                uint8_t* to;
                uint8_t* from;
                char* bytes;
                from = (uint8_t*)source->data();
                length = 0;
                for(i = 0; i < count; i++)
                {
                    length += String::decodeNumBytes(from[start + i]);
                }
                bytes = (char*)malloc(length + 1);
                to = (uint8_t*)bytes;
                for(i = 0; i < count; i++)
                {
                    index = start + i;
                    code_point = String::utfstringDecode(from + index, source->length() - index);
                    if(code_point != -1)
                    {
                        to += String::utfstringEncode(code_point, to);
                    }
                }
                return String::take(state, bytes, length);
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
                /* UNREACHABLE */
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

            static int utfcharOffset(const char* str, int index)
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
            *   foo = (String instance) "bar"
            *   String::format("foo=@", foo)
            *   => "foo=bar"
            *
            * it's extremely rudimentary, and the idea is to quickly join values.
            */
            static String* format(State* state, const char* format, ...)
            {
                char ch;
                size_t length;
                size_t total_length;
                bool was_allowed;
                const char* c;
                const char* strval;
                va_list arg_list;
                Value val;
                String* string;
                String* result;
                was_allowed = stateGetGCAllowed(state);
                stateSetGCAllowed(state, false);
                va_start(arg_list, format);
                total_length = strlen(format);
                va_end(arg_list);
                result = String::allocEmpty(state, total_length + 1);
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
                                    result->m_chars->append(strval, length);
                                }
                                else
                                {
                                    goto default_ending_copying;
                                }
                            }
                            break;
                        case '@':
                            {
                                val = va_arg(arg_list, Value);
                                if(Object::isString(val))
                                {
                                    string = Object::as<String>(val);
                                }
                                else
                                {
                                    //fprintf(stderr, "format: not a string, but a '%s'\n", Object::valueName(val));
                                    //string = Object::toString(state, val);
                                    goto default_ending_copying;
                                }
                                if(string != nullptr)
                                {
                                    length = string->m_chars->size();
                                    if(length > 0)
                                    {
                                        result->m_chars->append(*(string->m_chars), length);
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
                                /*
                                string = Object::as<String>(String::stringNumberToString(state, va_arg(arg_list, double)));
                                length = string->m_chars->size();
                                if(length > 0)
                                {
                                    result->m_chars->append(*(string->m_chars), length);
                                }
                                */
                                auto d = std::to_string((int64_t)va_arg(arg_list, double));
                                result->append(d);
                            }
                            break;
                        default:
                            {
                                default_ending_copying:
                                ch = *c;
                                result->m_chars->append(&ch, 1);
                            }
                            break;
                    }
                }
                va_end(arg_list);
                result->m_hash = String::makeHash(*(result->m_chars));
                String::statePutInterned(state, result);
                stateSetGCAllowed(state, was_allowed);
                return result;
            }

            static bool equal(State* state, String* a, String* b)
            {
                (void)state;
                if((a == nullptr) || (b == nullptr))
                {
                    return false;
                }
                return (*(a->m_chars) == *(b->m_chars));
            }


        public:
            /* the hash of this string - note that it is only unique to the context! */
            uint32_t m_hash = 0;

            /* this is handled by sds - use lit_string_length to get the length! */
            std::string* m_chars = nullptr;

        public:
            inline const char* data() const
            {
                return m_chars->data();
            }

            inline size_t size() const
            {
                return m_chars->size();
            }

            inline size_t length() const
            {
                return m_chars->size();
            }

            inline int at(size_t i)
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

            bool contains(const char* findme, size_t fmlen, bool icase) const
            {
                int selfch;
                int findch;
                size_t i;
                size_t j;
                size_t slen;
                size_t found;
                slen = m_chars->size();
                found = 0;
                if(slen >= fmlen)
                {
                    for(i=0, j=0; i<slen; i++)
                    {
                        do
                        {
                            selfch = (*m_chars)[i];
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

            size_t utfLength() const
            {
                size_t length;
                uint32_t i;
                length = 0;
                for(i = 0; i < this->length();)
                {
                    i += String::decodeNumBytes((*m_chars)[i]);
                    length++;
                }
                return length;
            }

            String* codePointAt(uint32_t index) const
            {
                char bytes[2];
                int code_point;
                if(index >= this->length())
                {
                    return nullptr;
                }
                code_point = String::utfstringDecode((uint8_t*)m_chars->data() + index, this->length() - index);
                if(code_point == -1)
                {
                    bytes[0] = (*m_chars)[index];
                    bytes[1] = '\0';
                    return String::copy(m_state, bytes, 1);
                }
                return String::fromCodePoint(m_state, code_point);
            }

            Array* split(std::string_view sep, bool keepblanc)
            {
                char ch;
                size_t i;
                size_t last;
                size_t next;
                Array* rt;
                std::string sub;
                last = 0;
                next = 0;
                rt = Array::make(m_state);
                if(sep.size() == 0)
                {
                    for(i=0; i<size(); i++)
                    {
                        ch = at(i);
                        rt->push(String::copy(m_state, std::string_view(&ch, 1))->asValue());
                    }
                }
                else
                {
                    while((next = m_chars->find(sep, last)) != std::string::npos)
                    {
                        sub = m_chars->substr(last, next-last);
                        last = next + 1;
                        if(!sub.empty() || keepblanc)
                        {
                            rt->push(String::copy(m_state, sub)->asValue());
                        }
                    }
                    sub = m_chars->substr(last);
                    if(!sub.empty() || keepblanc)
                    {
                        rt->push(String::copy(m_state, sub)->asValue());
                    }
                }
                return rt;
            }

            Array* split(String* sep, bool keepblanc)
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
            bool has_line_info;
            size_t line_count;
            size_t line_capacity;
            uint16_t* lines;
            PCGenericArray<Value> constants;

        public:
            void init(State* state)
            {
                this->m_state = state;
                this->m_count = 0;
                this->m_capacity = 0;
                m_code = nullptr;
                this->has_line_info = true;
                this->line_count = 0;
                this->line_capacity = 0;
                this->lines = nullptr;
                this->constants.init(this->m_state);
            }

            void release()
            {
                LIT_FREE_ARRAY(m_state, uint8_t, m_code, this->m_capacity);
                LIT_FREE_ARRAY(m_state, uint16_t, this->lines, this->line_capacity);
                this->constants.release();
                init(this->m_state);
            }

            void write_chunk(uint8_t byte, uint16_t line)
            {
                if(this->m_capacity < this->m_count + 1)
                {
                    size_t old_capacity = this->m_capacity;
                    this->m_capacity = LIT_GROW_CAPACITY(old_capacity + 2);
                    m_code = LIT_GROW_ARRAY(this->m_state, m_code, uint8_t, old_capacity, this->m_capacity + 2);
                }
                m_code[this->m_count] = byte;
                this->m_count++;
                if(!this->has_line_info)
                {
                    return;
                }

                if(this->line_capacity < this->line_count + 2)
                {
                    size_t old_capacity = this->line_capacity;
                    this->line_capacity = LIT_GROW_CAPACITY(this->line_capacity + 2);
                    this->lines = LIT_GROW_ARRAY(this->m_state, this->lines, uint16_t, old_capacity, this->line_capacity + 2);
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

            size_t add_constant(Value constant);

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
                    m_code = LIT_GROW_ARRAY(this->m_state, m_code, uint8_t, old_capacity, this->m_capacity);
                }
                if(this->line_capacity > this->line_count)
                {
                    size_t old_capacity = this->line_capacity;

                    this->line_capacity = this->line_count + 2;
                    this->lines = LIT_GROW_ARRAY(this->m_state, this->lines, uint16_t, old_capacity, this->line_capacity);
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

    class Function: public Object
    {
        public:
            static Function* make(State* state, Module* module)
            {
                Function* function;
                function = Object::make<Function>(state, Object::Type::Function);
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
            static Upvalue* make(State* state, Value* slot)
            {
                Upvalue* upvalue;
                upvalue = Object::make<Upvalue>(state, Object::Type::Upvalue);
                upvalue->location = slot;
                upvalue->closed = Object::NullVal;
                upvalue->next = nullptr;
                return upvalue;
            }


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
            static NativeFunction* make(State* state, NativeFunction::FuncType function, String* name)
            {
                NativeFunction* native;
                native = Object::make<NativeFunction>(state, Object::Type::NativeFunction);
                native->function = function;
                native->name = name;
                return native;
            }

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
            static NativePrimFunction* make(State* state, NativePrimFunction::FuncType function, String* name)
            {
                NativePrimFunction* native;
                native = Object::make<NativePrimFunction>(state, Object::Type::NativePrimitive);
                native->function = function;
                native->name = name;
                return native;
            }

        public:
            FuncType function;
            String* name;
    };

    class NativeMethod: public Object
    {
        public:
            using FuncType = Value(*)(VM*, Value, size_t arg_count, Value*);

        public:
            static NativeMethod* make(State* state, FuncType method, String* name)
            {
                NativeMethod* native;
                native = Object::make<NativeMethod>(state, Object::Type::NativeMethod);
                native->method = method;
                native->name = name;
                return native;
            }

            static NativeMethod* make(State* state, FuncType method, std::string_view name)
            {
                return NativeMethod::make(state, method, String::copy(state, name.data(), name.size()));
            }

        public:
            FuncType method;
            String* name;
    };

    class PrimitiveMethod: public Object
    {
        public:
            using FuncType = bool (*)(VM*, Value, size_t, Value*);

        public:
            static PrimitiveMethod* make(State* state, FuncType method, String* name)
            {
                PrimitiveMethod* native;
                native = Object::make<PrimitiveMethod>(state, Object::Type::PrimitiveMethod);
                native->method = method;
                native->name = name;
                return native;
            }

            static PrimitiveMethod* make(State* state, FuncType method, std::string_view sv)
            {
                return make(state, method, String::copy(state, sv.data(), sv.size()));
            }

        public:
            FuncType method;
            String* name;
    };

    class Table
    {
        public:
            struct Entry
            {
                /* the key of this entry. can be nullptr! */
                String* key;

                /* the associated value */
                Value value;
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

            static Entry* findEntry(Entry* entries, int capacity, String* key)
            {
                uint32_t index;
                Entry* entry;
                Entry* tombstone;
                index = key->m_hash % capacity;
                tombstone = nullptr;
                while(true)
                {
                    entry = &entries[index];
                    if(entry->key == nullptr)
                    {
                        if(Object::isNull(entry->value))
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

            static void adjustCapacity(State* state, Table* table, size_t capacity)
            {
                size_t i;
                Entry* destination;
                Entry* entries;
                Entry* entry;
                entries = LIT_ALLOCATE(state, Entry, capacity + 1);
                for(i = 0; i <= capacity; i++)
                {
                    entries[i].key = nullptr;
                    entries[i].value = Object::NullVal;
                }
                table->m_inner.m_count = 0;
                for(i = 0; i <= table->m_inner.m_capacity; i++)
                {
                    entry = &table->m_inner.m_values[i];
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
                    table->m_inner.m_count++;
                }
                LIT_FREE_ARRAY(state, Entry, table->m_inner.m_values, table->m_inner.m_capacity + 1);
                table->m_inner.m_capacity = capacity;
                table->m_inner.m_values = entries;
            }

        public:
            State* m_state = nullptr;
            PCGenericArray<Entry> m_inner;

        public:
            void init(State* state)
            {
                m_state = state;
                m_inner.init(state);
            }

            void release()
            {
                m_inner.release();
            }

            void markForGC(VM* vm);

            inline size_t capacity() const
            {
                return m_inner.m_capacity;
            }

            inline size_t size() const
            {
                return m_inner.size();
            }

            inline constexpr Entry& at(size_t i)
            {
                return m_inner.at(i);
            }

            void setField(const char* name, Value value)
            {
                this->set(String::intern(m_state, name), value);
            }

            Value getField(const char* name)
            {
                Value value;
                if(!this->get(String::intern(m_state, name), &value))
                {
                    value = Object::NullVal;
                }
                return value;
            }

            bool set(String* key, Value value)
            {
                bool is_new;
                size_t capacity;
                Entry* entry;
                if(this->m_inner.m_count + 1 > (this->m_inner.m_capacity + 1) * TABLE_MAX_LOAD)
                {
                    capacity = LIT_GROW_CAPACITY(this->m_inner.m_capacity + 1) - 1;
                    adjustCapacity(m_state, this, capacity);
                }
                entry = findEntry(this->m_inner.m_values, this->m_inner.m_capacity, key);
                is_new = entry->key == nullptr;
                if(is_new && Object::isNull(entry->value))
                {
                    this->m_inner.m_count++;
                }
                entry->key = key;
                entry->value = value;
                return is_new;
            }

            inline bool set(std::string_view sv, Value value)
            {
                return set(String::copy(m_state, sv.data(), sv.size()), value);
            }

            bool get(String* key, Value* value)
            {
                Entry* entry;
                if(this->m_inner.m_count == 0)
                {
                    return false;
                }
                entry = findEntry(this->m_inner.m_values, this->m_inner.m_capacity, key);
                if(entry->key == nullptr)
                {
                    return false;
                }
                *value = entry->value;
                return true;
            }

            bool getSlot(String* key, Value** value)
            {
                Entry* entry;
                if(this->m_inner.m_count == 0)
                {
                    return false;
                }
                entry = findEntry(this->m_inner.m_values, this->m_inner.m_capacity, key);
                if(entry->key == nullptr)
                {
                    return false;
                }
                *value = &entry->value;
                return true;
            }

            bool remove(String* key)
            {
                Entry* entry;
                if(this->m_inner.m_count == 0)
                {
                    return false;
                }
                entry = findEntry(this->m_inner.m_values, this->m_inner.m_capacity, key);
                if(entry->key == nullptr)
                {
                    return false;
                }
                entry->key = nullptr;
                entry->value = BOOL_VALUE(true);
                return true;
            }

            String* find(const char* str, size_t length, uint32_t hs)
            {
                uint32_t index;
                Entry* entry;
                if(this->m_inner.m_count == 0)
                {
                    return nullptr;
                }
                index = hs % this->m_inner.m_capacity;
                while(true)
                {
                    entry = &this->m_inner.m_values[index];
                    if(entry->key == nullptr)
                    {
                        if(Object::isNull(entry->value))
                        {
                            return nullptr;
                        }
                    }
                    else if(entry->key->length() == length && entry->key->m_hash == hs && memcmp(entry->key->data(), str, length) == 0)
                    {
                        return entry->key;
                    }
                    index = (index + 1) % this->m_inner.m_capacity;
                }
            }

            void addAll(Table* from)
            {
                size_t i;
                Entry* entry;
                for(i = 0; i <= from->m_inner.m_capacity; i++)
                {
                    entry = &from->m_inner.m_values[i];
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
                Entry* entry;
                for(i = 0; i <= this->m_inner.m_capacity; i++)
                {
                    entry = &this->m_inner.m_values[i];
                    if(entry->key != nullptr && !entry->key->marked)
                    {
                        this->remove(entry->key);
                    }
                }
            }

            int64_t iterator(int64_t number)
            {
                if(this->m_inner.m_count == 0)
                {
                    return -1;
                }
                if(number >= int64_t(this->m_inner.m_capacity))
                {
                    return -1;
                }
                number++;
                for(; number < int64_t(this->m_inner.m_capacity); number++)
                {
                    if(this->m_inner.m_values[number].key != nullptr)
                    {
                        return number;
                    }
                }

                return -1;
            }

            Value iterKey(int64_t index)
            {
                if(int64_t(this->m_inner.m_capacity) <= index)
                {
                    return Object::NullVal;
                }
                return this->m_inner.m_values[index].key->asValue();
            }
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
            static size_t write_uint8_t(FILE* file, uint8_t byte)
            {
                return fwrite(&byte, sizeof(uint8_t), 1, file);
            }

            static size_t write_uint16_t(FILE* file, uint16_t byte)
            {
                return fwrite(&byte, sizeof(uint16_t), 1, file);
            }

            static size_t write_uint32_t(FILE* file, uint32_t byte)
            {
                return fwrite(&byte, sizeof(uint32_t), 1, file);
            }

            static size_t write_double(FILE* file, double byte)
            {
                return fwrite(&byte, sizeof(double), 1, file);
            }

            static size_t write_string(FILE* file, String* string)
            {
                uint16_t i;
                uint16_t c;
                size_t rt;
                c = string->length();
                rt = fwrite(&c, 2, 1, file);
                auto ch = string->data();
                for(i = 0; i < c; i++)
                {
                    write_uint8_t(file, (uint8_t)ch[i] ^ LIT_STRING_KEY);
                }
                return (rt + i);
            }

            static uint8_t read_uint8_t(FILE* file)
            {
                size_t rt;
                (void)rt;
                static uint8_t btmp;
                rt = fread(&btmp, sizeof(uint8_t), 1, file);
                return btmp;
            }

            static uint16_t read_uint16_t(FILE* file)
            {
                size_t rt;
                (void)rt;
                static uint16_t stmp;
                rt = fread(&stmp, sizeof(uint16_t), 1, file);
                return stmp;
            }

            static uint32_t read_uint32_t(FILE* file)
            {
                size_t rt;
                (void)rt;
                static uint32_t itmp;
                rt = fread(&itmp, sizeof(uint32_t), 1, file);
                return itmp;
            }

            static double read_double(FILE* file)
            {
                size_t rt;
                (void)rt;
                static double dtmp;
                rt = fread(&dtmp, sizeof(double), 1, file);
                return dtmp;
            }

            static String* read_string(State* state, FILE* file)
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
                    line[i] = (char)read_uint8_t(file) ^ LIT_STRING_KEY;
                }
                return String::take(state, line, length);
            }
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

            inline constexpr Table::Entry& at(size_t i)
            {
                return m_values.at(i);
            }

            bool set(String* key, Value value)
            {
                if(value == Object::NullVal)
                {
                    this->remove(key);
                    return false;
                }
                return this->m_values.set(key, value);
            }

            bool get(String* key, Value* value)
            {
                return m_values.get(key, value);
            }

            void lit_set_map_field(State* state, const char* name, Value value)
            {
                m_values.set(String::intern(state, name), value);
            }

            Value getField(State* state, const char* name)
            {
                Value value;
                if(!m_values.get(String::intern(state, name), &value))
                {
                    value = Object::NullVal;
                }
                return value;
            }

            bool remove(String* key)
            {
                return m_values.remove(key);
            }

            void addAll(Map* other)
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
                size_t stack_capacity;
                Value* stack;
                CallFrame* frame;
                CallFrame* frames;
                Fiber* fiber;
                // Allocate in advance, just in case GC is triggered
                stack_capacity = function == nullptr ? 1 : (size_t)lit_closest_power_of_two(function->max_slots + 1);
                stack = LIT_ALLOCATE(state, Value, stack_capacity);
                frames = LIT_ALLOCATE(state, CallFrame, LIT_INITIAL_CALL_FRAMES);
                fiber = Object::make<Fiber>(state, Object::Type::Fiber);
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
                fiber->error = Object::NullVal;
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
                if(fiber->frame_count == LIT_CALL_FRAMES_MAX)
                {
                    lit_runtime_error(vm, "fiber frame overflow");
                    return true;
                }
                if(fiber->frame_count + 1 > fiber->frame_capacity)
                {
                    //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->frame_capacity * 2);
                    newcapacity = (fiber->frame_capacity * 2) + 1;
                    osize = (sizeof(CallFrame) * fiber->frame_capacity);
                    newsize = (sizeof(CallFrame) * newcapacity);
                    fiber->frames = (CallFrame*)Memory::reallocate(state, fiber->frames, osize, newsize);
                    fiber->frame_capacity = newcapacity;
                }
                return false;
            }

            static bool ensureFiber(VM* vm, Fiber* fiber);

        public:
            Fiber* parent = nullptr;
            Value* stack = nullptr;
            Value* stack_top = nullptr;
            size_t stack_capacity = 0;
            CallFrame* frames = nullptr;
            size_t frame_capacity = 0;
            size_t frame_count = 0;
            size_t arg_count = 0;
            Upvalue* open_upvalues = nullptr;
            Module* module = nullptr;
            Value error = Object::NullVal;
            bool abort = false;
            bool catcher = false;

        public:
            void ensure_stack(size_t needed)
            {
                size_t i;
                size_t capacity;
                Value* old_stack;
                Upvalue* upvalue;
                if(this->stack_capacity >= needed)
                {
                    return;
                }
                capacity = (size_t)lit_closest_power_of_two((int)needed);
                old_stack = this->stack;
                this->stack = (Value*)Memory::reallocate(m_state, this->stack, sizeof(Value) * this->stack_capacity, sizeof(Value) * capacity);
                this->stack_capacity = capacity;
                if(this->stack != old_stack)
                {
                    for(i = 0; i < this->frame_capacity; i++)
                    {
                        CallFrame* frame = &this->frames[i];
                        frame->slots = this->stack + (frame->slots - old_stack);
                    }
                    for(upvalue = this->open_upvalues; upvalue != nullptr; upvalue = upvalue->next)
                    {
                        upvalue->location = this->stack + (upvalue->location - old_stack);
                    }
                    this->stack_top = this->stack + (this->stack_top - old_stack);
                }
            }

    };

    class Class: public Object
    {
        public:
            static Value defaultfn_tostring(VM* vm, Value instance, size_t argc, Value* argv)
            {
                const char* fmtpat;
                (void)argc;
                (void)argv;
                fmtpat = "<class @>";
                // this assumes toString() wasn't overriden
                if(!Object::isNull(instance))
                {
                    if(!Object::isClass(instance))
                    {
                        fmtpat = "<instance @>";
                    }
                }
                return String::format(Object::asState(vm), fmtpat, Object::as<Class>(instance)->name->asValue())->asValue();
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
            String* name;
            /* the constructor object */
            Object* init_method;
            /* runtime methods */
            Table methods;
            /* static fields, which include functions, and variables */
            Table static_fields;
            /*
            * the parent class - the topmost is always Class, followed by Object.
            * that is, eg for String: String <- Object <- Class
            */
            Class* super;

        public:

        public:
            void inheritFrom(Class* superclass)
            {
                this->super = superclass;
                if(this->init_method == nullptr)
                {
                    this->init_method = superclass->init_method;
                }
                superclass->methods.addAll(&this->methods); \
                superclass->static_fields.addAll(&this->static_fields);
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
                    Field::make(m_state,
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
                    Field::make(m_state,
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
                    Field::make(m_state,
                        nullptr,
                        (Object*)NativeMethod::make(m_state, fn, nm))->asValue());
            }

            void setStaticSetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(m_state, sv.data(), sv.size());
                return setStaticSetter(nm, fn);
            }

            void setStaticGetter(String* nm, NativeMethod::FuncType fn)
            {
                this->static_fields.set(nm,
                    Field::make(m_state,
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
                this->methods.set(nm, Field::make(this->m_state, NativeMethod::make(m_state, fn, nm), nullptr)->asValue());
            }

            void setGetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(this->m_state, sv.data(), sv.size());
                setGetter(nm, fn);
            }

            void setSetter(String* nm, NativeMethod::FuncType fn)
            {
                this->methods.set(nm, Field::make(this->m_state, nullptr, NativeMethod::make(m_state, fn, nm))->asValue());
            }

            void setSetter(std::string_view sv, NativeMethod::FuncType fn)
            {
                auto nm = String::copy(this->m_state, sv.data(), sv.size());
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

    namespace AST
    {
        struct Token
        {
            public:
                static const char* token_name(int t)
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

                static void releaseExpressionList(State* state, Expression::List* expressions)
                {
                    if(expressions == nullptr)
                    {
                        return;
                    }
                    for(size_t i = 0; i < expressions->m_count; i++)
                    {
                        Expression::releaseExpression(state, expressions->m_values[i]);
                    }
                    expressions->release();
                }

                static void releaseStatementList(State* state, Expression::List* statements)
                {
                    size_t i;
                    for(i = 0; i < statements->m_count; i++)
                    {
                        Expression::releaseStatement(state, statements->m_values[i]);
                    }
                    statements->release();
                }

                static void internalReleaseStatementList(State* state, Expression::List* statements)
                {
                    if(statements == nullptr)
                    {
                        return;
                    }
                    for(size_t i = 0; i < statements->m_count; i++)
                    {
                        Expression::releaseStatement(state, statements->m_values[i]);
                    }
                    statements->release();
                }

                static void releaseAllocatedExpressionList(State* state, Expression::List* expressions)
                {
                    size_t i;
                    if(expressions == nullptr)
                    {
                        return;
                    }
                    for(i = 0; i < expressions->m_count; i++)
                    {
                        Expression::releaseExpression(state, expressions->m_values[i]);
                    }
                    expressions->release();
                    Memory::reallocate(state, expressions, sizeof(Expression::List), 0);
                }

                static void releaseAllocatedStatementList(State* state, Expression::List* statements)
                {
                    size_t i;
                    if(statements == nullptr)
                    {
                        return;
                    }
                    for(i = 0; i < statements->m_count; i++)
                    {
                        Expression::releaseStatement(state, statements->m_values[i]);
                    }
                    statements->release();
                    Memory::reallocate(state, statements, sizeof(Expression::List), 0);
                }

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
                void init(State* state, const char* fname, const char* source, size_t len)
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

                Token make_token(TokenType type)
                {
                    Token token;
                    token.type = type;
                    token.start = m_startsrc;
                    token.length = (size_t)(m_currsrc - m_startsrc);
                    token.line = m_line;
                    return token;
                }

                Token make_error_token(Error error, ...)
                {
                    va_list args;
                    Token token;
                    String* result;
                    m_haderror = true;
                    va_start(args, error);
                    result = lit_vformat_error(this->m_state, m_line, error, args);
                    va_end(args);
                    token.type = LITTOK_ERROR;
                    token.start = result->data();
                    token.length = result->length();
                    token.line = m_line;
                    return token;
                }

                bool is_at_end()
                {
                    return *m_currsrc == '\0';
                }

                char advance()
                {
                    m_currsrc++;
                    return m_currsrc[-1];
                }

                bool match(char expected)
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

                Token match_token(char c, TokenType a, TokenType b)
                {
                    return make_token(match(c) ? a : b);
                }

                Token match_tokens(char cr, char cb, TokenType a, TokenType b, TokenType c)
                {
                    return make_token(match(cr) ? a : (match(cb) ? b : c));
                }

                char peek()
                {
                    return *m_currsrc;
                }

                char peek_next()
                {
                    if(is_at_end())
                    {
                        return '\0';
                    }
                    return m_currsrc[1];
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

                Token scan_string(bool interpolation)
                {
                    char c;
                    char newc;
                    char nextc;
                    int octval;
                    State* state;
                    PCGenericArray<uint8_t> bytes;
                    Token token;
                    TokenType string_type;
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
                    m_currsrc--;
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
                    m_currsrc--;
                    return -1;
                }

                Token make_number_token(bool is_hex, bool is_binary)
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
                        value = Object::toValue((int)strtoll(m_startsrc + 2, nullptr, 2));
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

                Token scan_number()
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

                TokenType check_keyword(int start, int length, const char* rest, TokenType type)
                {
                    if(m_currsrc - m_startsrc == start + length && memcmp(m_startsrc + start, rest, length) == 0)
                    {
                        return type;
                    }
                    return LITTOK_IDENTIFIER;
                }

                TokenType scan_identtype()
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

                Token scan_identifier()
                {
                    while(char_is_alpha(peek()) || char_is_digit(peek()))
                    {
                        advance();
                    }
                    return make_token(scan_identtype());
                }

                Token scan_token()
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
                                    return scan_string(true);
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
                            return scan_string(true);
                        }
                        case '"':
                            return scan_string(false);
                    }
                    return make_error_token(Error::LITERROR_UNEXPECTED_CHAR, c);
                }
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
                    this->m_state = state;
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
                static Expression* parse_block(Parser* parser)
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
                        if(parser->match(LITTOK_SEMICOLON))
                        {
                            parser->advance();

                        }
                            parser->ignore_new_lines();
                        
                    }
                    parser->ignore_new_lines();
                    parser->consume(LITTOK_RIGHT_BRACE, "'}'");
                    parser->ignore_new_lines();
                    parser->end_scope();
                    return (Expression*)statement;
                }

                static Expression* parse_precedence(Parser* parser, Precedence precedence, bool err)
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
                    parser->ignore_new_lines();

                    prev_newline = false;
                    previous = parser->m_prevtoken;
                    parser->advance();

                    prefix_rule = parser->get_rule(parser->m_prevtoken.type)->prefix;
                    if(prefix_rule == nullptr)
                    {
                        
                        fprintf(stderr, "parser->m_prevtoken.type=%s, parser->m_currtoken.type=%s\n", Token::token_name(parser->m_prevtoken.type), Token::token_name(parser->m_currtoken.type));
                        if(parser->m_prevtoken.type == LITTOK_SEMICOLON)
                        {
                            if(parser->m_currtoken.type == LITTOK_NEW_LINE)
                            {
                                parser->advance();
                                return parse_precedence(parser, precedence, err);
                            }
                            return nullptr;
                        }
                        
                        

                        // todo: file start
                        size_t toklen;
                        size_t prevtoklen;
                        const char* tokname;
                        const char* prevtokname;
                        tokname = "new line";
                        toklen = strlen(tokname);
                        prevtokname = "new line";
                        prevtoklen = strlen(prevtokname);
                        new_line = previous.start != nullptr && *previous.start == '\n';
                        parser_prev_newline = parser->m_prevtoken.start != nullptr && *parser->m_prevtoken.start == '\n';
                        if(!prev_newline)
                        {
                            tokname = previous.start;
                            toklen = previous.length;
                        }
                        if(!parser_prev_newline)
                        {
                            prevtokname = parser->m_prevtoken.start;
                            prevtoklen = parser->m_prevtoken.length;
                        }
                        parser->raiseError(Error::LITERROR_EXPECTED_EXPRESSION, toklen, tokname, prevtoklen, prevtokname);
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

                static Expression* parse_number(Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    return (Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value);
                }

                static Expression* parse_lambda(Parser* parser, ExprLambda* lambda)
                {
                    lambda->body = parse_statement(parser);
                    return (Expression*)lambda;
                }

                static void parse_parameters(Parser* parser, PCGenericArray<ExprFuncParam>* parameters)
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

                static Expression* parse_grouping_or_lambda(Parser* parser, bool can_assign)
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

                static Expression* parse_call(Parser* parser, Expression* prev, bool can_assign)
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

                static Expression* parse_unary(Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    parser->ignore_new_lines();
                    auto op = parser->m_prevtoken.type;
                    auto line = parser->m_prevtoken.line;
                    auto expression = parse_precedence(parser, LITPREC_UNARY, true);
                    return (Expression*)ExprUnary::make(parser->m_state, line, expression, op);
                }

                static Expression* parse_binary(Parser* parser, Expression* prev, bool can_assign)
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
                    parser->ignore_new_lines();
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

                static Expression* parse_and(Parser* parser, Expression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    TokenType op;
                    parser->ignore_new_lines();
                    op = parser->m_prevtoken.type;
                    line = parser->m_prevtoken.line;
                    return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_AND, true), op);
                }

                static Expression* parse_or(Parser* parser, Expression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    TokenType op;
                    parser->ignore_new_lines();
                    op = parser->m_prevtoken.type;
                    line = parser->m_prevtoken.line;
                    return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_OR, true), op);
                }

                static Expression* parse_null_filter(Parser* parser, Expression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    TokenType op;
                    parser->ignore_new_lines();
                    op = parser->m_prevtoken.type;
                    line = parser->m_prevtoken.line;
                    return (Expression*)ExprBinary::make(parser->m_state, line, prev, parse_precedence(parser, LITPREC_NULL, true), op);
                }

                static TokenType convert_compound_operator(TokenType op)
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

                static Expression* parse_compound(Parser* parser, Expression* prev, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    ExprBinary* binary;
                    Expression* expression;
                    ParseRule* rule;
                    TokenType op;
                    parser->ignore_new_lines();
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

                static Expression* parse_literal(Parser* parser, bool can_assign)
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

                static Expression* parse_string(Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    auto expression = (Expression*)ExprLiteral::make(parser->m_state, parser->m_prevtoken.line, parser->m_prevtoken.value);
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    return expression;
                }

                static Expression* parse_interpolation(Parser* parser, bool can_assign)
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

                static Expression* parse_object(Parser* parser, bool can_assign)
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

                static Expression* parse_variable_expression_base(Parser* parser, bool can_assign, bool isnew)
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

                static Expression* parse_variable_expression(Parser* parser, bool can_assign)
                {
                    return parse_variable_expression_base(parser, can_assign, false);
                }

                static Expression* parse_new_expression(Parser* parser, bool can_assign)
                {
                    (void)can_assign;
                    parser->consume(LITTOK_IDENTIFIER, "class name after 'new'");
                    return parse_variable_expression_base(parser, false, true);
                }

                static Expression* parse_dot(Parser* parser, Expression* previous, bool can_assign)
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

                static Expression* parse_range(Parser* parser, Expression* previous, bool can_assign)
                {
                    (void)can_assign;
                    size_t line;
                    line = parser->m_prevtoken.line;
                    return (Expression*)ExprRange::make(parser->m_state, line, previous, parse_expression(parser));
                }

                static Expression* parse_ternary_or_question(Parser* parser, Expression* previous, bool can_assign)
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

                static Expression* parse_array(Parser* parser, bool can_assign)
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

                static Expression* parse_subscript(Parser* parser, Expression* previous, bool can_assign)
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

                static Expression* parse_this(Parser* parser, bool can_assign)
                {
                    auto expression = (Expression*)ExprThis::make(parser->m_state, parser->m_prevtoken.line);
                    if(parser->match(LITTOK_LEFT_BRACKET))
                    {
                        return parse_subscript(parser, expression, can_assign);
                    }
                    return expression;
                }

                static Expression* parse_super(Parser* parser, bool can_assign)
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

                static Expression* parse_reference(Parser* parser, bool can_assign)
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

                static Expression* parse_expression(Parser* parser)
                {
                    parser->ignore_new_lines();
                    return parse_precedence(parser, LITPREC_ASSIGNMENT, true);
                }

                static Expression* parse_var_declaration(Parser* parser)
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

                static Expression* parse_if(Parser* parser)
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

                static Expression* parse_for(Parser* parser)
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

                static Expression* parse_while(Parser* parser)
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

                static Expression* parse_function(Parser* parser)
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

                static Expression* parse_return(Parser* parser)
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

                static Expression* parse_field(Parser* parser, String* name, bool is_static)
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

                static Expression* parse_method(Parser* parser, bool is_static)
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

                static Expression* parse_class(Parser* parser)
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

                static Expression* parse_statement(Parser* parser)
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
                    else if(parser->match(LITTOK_CLASS))
                    {
                        parse_class(parser);
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
                        //return parser->parse();
                        return parse_declaration(parser);
                    }
                    expression = parse_expression(parser);
                    if(expression == nullptr)
                    {
                        return nullptr;
                    }
                    return (Expression*)ExprStatement::make(parser->m_state, parser->m_prevtoken.line, expression);
                }

                static Expression* parse_declaration(Parser* parser)
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
                    stringError(token, lit_vformat_error(this->m_state, token->line, error, args)->data());
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
                        m_currtoken = getStateScanner(this->m_state)->scan_token();
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
                    fmt = lit_format_error(this->m_state, m_currtoken.line, Error::LITERROR_EXPECTATION_UNMET, error, olen, otext)->data();
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
                    this->getStateScanner(this->m_state)->init(this->m_state, filename, source, length);
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
                    return m_haderror || this->getStateScanner(this->m_state)->m_haderror;
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

            #define optc_do_bitwise_op(op) \
                if(Object::isNumber(a) && Object::isNumber(b)) \
                { \
                    optdbg("translating constant bitwise expression of '" #op "' to constant value"); \
                    return Object::toValue((int)Object::toNumber(a) op(int) Object::toNumber(b)); \
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
                                return BOOL_VALUE(Object::isFalsey(value));
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
                                if(Object::isNumber(a) && Object::isNumber(b))
                                {
                                    return Object::toValue(floor(Object::toNumber(a) / Object::toNumber(b)));
                                }
                                return Object::NullVal;
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
                            Expression::releaseStatement(this->m_state, *variable->declaration);
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
                                Expression::releaseExpression(this->m_state, left ? expression->right : expression->left);
                                expression->left = branch;
                                expression->right = nullptr;
                            }
                        }
                        else if((op == LITTOK_PLUS || op == LITTOK_MINUS) && number == 0)
                        {
                            optdbg("reducing expression that would result in '0' to literal '0'");
                            Expression::releaseExpression(this->m_state, left ? expression->right : expression->left);
                            expression->left = branch;
                            expression->right = nullptr;
                        }
                        else if(((left && op == LITTOK_SLASH) || op == LITTOK_STAR_STAR) && number == 1)
                        {
                            optdbg("reducing expression that would result in '1' to literal '1'");
                            Expression::releaseExpression(this->m_state, left ? expression->right : expression->left);
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
                    auto state = this->m_state;
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
                    state = this->m_state;
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
                                    Expression::releaseStatement(this->m_state, statement);
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
                                        Expression::releaseStatement(this->m_state, statement);
                                        *slot = nullptr;
                                        break;
                                    }
                                }
                                optimize_statement(&stmt->body);
                                if(Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                                {
                                    Expression::releaseStatement(this->m_state, statement);
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
                                    Expression::releaseStatement(this->m_state, statement);
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
                    this->m_state = state;
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
                    m_chunk->write_chunk(byte, line);
                    m_lastline = line;
                }

                void emit_bytes(uint16_t line, uint8_t a, uint8_t b)
                {
                    if(line < m_lastline)
                    {
                        // Egor-fail proofing
                        line = m_lastline;
                    }
                    m_chunk->write_chunk(a, line);
                    m_chunk->write_chunk(b, line);
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
                    compiler->locals.init(this->m_state);
                    compiler->type = type;
                    compiler->scope_depth = 0;
                    compiler->enclosing = (Compiler*)m_compiler;
                    compiler->skip_return = false;
                    compiler->function = Function::make(this->m_state, m_module);
                    compiler->loop_depth = 0;
                    m_compiler = compiler;
                    const char* name = getStateScannerFilename();
                    if(m_compiler == nullptr)
                    {
                        compiler->function->name = String::copy(this->m_state, name, strlen(name));
                    }
                    m_chunk = &compiler->function->chunk;
                    if(Optimizer::is_enabled(LITOPTSTATE_LINE_INFO))
                    {
                        m_chunk->has_line_info = false;
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

                uint16_t add_constant(size_t line, Value value)
                {
                    size_t constant = m_chunk->add_constant(value);
                    if(constant >= UINT16_MAX)
                    {
                        error(line, Error::LITERROR_TOO_MANY_CONSTANTS);
                    }
                    return constant;
                }

                size_t emit_constant(size_t line, Value value)
                {
                    size_t constant = m_chunk->add_constant(value);
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
                    auto state = this->m_state;
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
                                                       add_constant(expression->line,
                                                                    String::copy(this->m_state, expr->name, expr->length)->asValue()));
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
                                                           add_constant(expression->line,
                                                                        String::copy(this->m_state, e->name, e->length)->asValue()));
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
                                    emit_constant(m_lastline, String::copy(this->m_state, e->name, e->length)->asValue());
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
                                                   add_constant(m_lastline,
                                                                String::copy(this->m_state, e->name, e->length)->asValue()));
                                    }
                                    else
                                    {
                                        auto e = (ExprSuper*)expr->callee;
                                        uint8_t index = resolve_upvalue(m_compiler, "super", 5, m_lastline);
                                        emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                                        emit_varying_op(m_lastline,
                                                        ((ExprSuper*)expr->callee)->ignore_result ? OP_INVOKE_SUPER_IGNORING : OP_INVOKE_SUPER,
                                                        (uint8_t)expr->args.m_count);
                                        emit_short(m_lastline, add_constant(m_lastline, e->method->asValue()));
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
                                                      String::copy(this->m_state, expr->name, expr->length)->asValue());
                                        emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                                    }
                                    patch_jump(expr->jump, m_lastline);
                                }
                                else if(!expr->ignore_emit)
                                {
                                    emit_constant(m_lastline, String::copy(this->m_state, expr->name, expr->length)->asValue());
                                    emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                                }
                            }
                            break;
                        case Expression::Type::Set:
                            {
                                auto expr = (ExprIndexSet*)expression;
                                emit_expression(expr->where);
                                emit_expression(expr->value);
                                emit_constant(m_lastline, String::copy(this->m_state, expr->name, expr->length)->asValue());
                                emit_op(m_lastline, OP_SET_FIELD);
                            }
                            break;
                        case Expression::Type::Lambda:
                            {
                                auto expr = (ExprLambda*)expression;
                                auto name = String::format(this->m_state, "<lambda @:@>", m_module->name->asValue(),
                                                                              String::stringNumberToString(this->m_state, expression->line));
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
                                    emit_short(m_lastline, add_constant(m_lastline, function->asValue()));
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
                                    emit_short(expression->line, add_constant(expression->line, expr->method->asValue()));
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
                                           add_constant(m_lastline, String::internValue(this->m_state, "join")));
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
                                               add_constant(m_lastline, String::internValue(this->m_state, "iterator")));
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
                                               add_constant(m_lastline, String::internValue(this->m_state, "iteratorValue")));
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
                                name = String::copy(this->m_state, funcstmt->name, funcstmt->length);
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
                                    emit_short(m_lastline, add_constant(m_lastline, function->asValue()));
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
                                    emit_short(m_lastline, add_constant(m_lastline, name->asValue()));
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
                                function = end_compiler(String::format(this->m_state, "@:@", m_classname->asValue(), mthstmt->name->asValue()));
                                function->arg_count = mthstmt->parameters.m_count;
                                function->max_slots += function->arg_count;
                                function->vararg = vararg;
                                if(function->upvalue_count > 0)
                                {
                                    emit_op(m_lastline, OP_CLOSURE);
                                    emit_short(m_lastline, add_constant(m_lastline, function->asValue()));
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
                                emit_short(m_lastline, add_constant(statement->line, mthstmt->name->asValue()));

                            }
                            break;
                        case Expression::Type::ClassDecl:
                            {
                                clstmt = (StmtClass*)statement;
                                m_classname = clstmt->name;
                                if(clstmt->parent != nullptr)
                                {
                                    emit_op(m_lastline, OP_GET_GLOBAL);
                                    emit_short(m_lastline, add_constant(m_lastline, clstmt->parent->asValue()));
                                }
                                emit_op(statement->line, OP_CLASS);
                                emit_short(m_lastline, add_constant(m_lastline, clstmt->name->asValue()));
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
                                                   add_constant(statement->line,
                                                                String::copy(this->m_state, var->name, var->length)->asValue()));
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
                                    getter = end_compiler(String::format(this->m_state, "@:get @", m_classname->asValue(), fieldstmt->name));
                                }
                                if(fieldstmt->setter != nullptr)
                                {
                                    init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                                    mark_local_initialized(add_local("value", 5, statement->line, false));
                                    begin_scope();
                                    emit_statement(fieldstmt->setter);
                                    end_scope(m_lastline);
                                    setter = end_compiler(String::format(this->m_state, "@:set @", m_classname->asValue(), fieldstmt->name));
                                    setter->arg_count = 1;
                                    setter->max_slots++;
                                }
                                field = Field::make(this->m_state, (Object*)getter, (Object*)setter);
                                emit_constant(statement->line, field->asValue());
                                emit_op(statement->line, fieldstmt->is_static ? OP_STATIC_FIELD : OP_DEFINE_FIELD);
                                emit_short(statement->line, add_constant(statement->line, fieldstmt->name->asValue()));
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
            Class* classvalue_class;
            Class* objectvalue_class;
            Class* numbervalue_class;
            Class* stringvalue_class;
            Class* boolvalue_class;
            Class* functionvalue_class;
            Class* fibervalue_class;
            Class* modulevalue_class;
            Class* arrayvalue_class;
            Class* mapvalue_class;
            Class* rangevalue_class;
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

            /*
            void lit_disassemble_module(State* state, Module* module, const char* source);
            void lit_disassemble_chunk(Chunk* chunk, const char* name, const char* source);
            size_t lit_disassemble_instruction(State* state, Chunk* chunk, size_t offset, const char* source);
            */
            void showDecompiled()
            {
                
            }

            void raiseError(ErrorType type, const char* message, ...)
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
                showDecompiled();
                m_haderror = true;
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
            void releaseObjects(Object* objects);

            void pushRoot(Object* obj)
            {
                pushValueRoot(obj->asValue());
            }

            void pushValueRoot(Value value)
            {
                if(this->root_count + 1 >= this->root_capacity)
                {
                    this->root_capacity = LIT_GROW_CAPACITY(this->root_capacity);
                    this->roots = (Value*)realloc(this->roots, this->root_capacity * sizeof(Value));
                }
                this->roots[this->root_count++] = value;
            }

            Value peekRoot(uint8_t distance)
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



            void setVMGlobal(String* name, Value val);
            bool getVMGlobal(String* name, Value* dest);

            Value getGlobal(String* name)
            {
                Value global;
                if(!this->getVMGlobal(name, &global))
                {
                    return Object::NullVal;
                }
                return global;
            }

            Function* getGlobalFunction(String* name)
            {
                Value function = getGlobal(name);
                if(Object::isFunction(function))
                {
                    return Object::as<Function>(function);
                }
                return nullptr;
            }

            void setGlobal(String* name, Value value)
            {
                this->pushRoot((Object*)name);
                this->pushValueRoot(value);
                this->setVMGlobal(name, value);
                this->popRoots(2);
            }

            bool globalExists(String* name)
            {
                Value global;
                return this->getVMGlobal(name, &global);
            }

            void defineNative(const char* name, NativeFunction::FuncType native)
            {
                this->pushRoot((Object*)String::intern(this, name));
                this->pushRoot((Object*)NativeFunction::make(this, native, Object::as<String>(this->peekRoot(0))));
                this->setVMGlobal(Object::as<String>(this->peekRoot(1)), this->peekRoot(0));
                this->popRoots(2);
            }

            void defineNativePrimitive(const char* name, NativePrimFunction::FuncType native)
            {
                this->pushRoot((Object*)String::intern(this, name));
                this->pushRoot((Object*)NativePrimFunction::make(this, native, Object::as<String>(this->peekRoot(0))));
                this->setVMGlobal(Object::as<String>(this->peekRoot(1)), this->peekRoot(0));
                this->popRoots(2);
            }

            Upvalue* captureUpvalue(Value* local);

            Result execModule(Module* module);

            Result execFiber(Fiber* fiber);

            Fiber::CallFrame* setupCall(Function* callee, Value* argv, size_t argc);

            Result execCall(Fiber::CallFrame* frame);

            Result callFunction(Function* callee, Value* argv, size_t argc)
            {
                return execCall(this->setupCall(callee, argv, argc));
            }

            Result callClosure(Closure* callee, Value* argv, size_t argc)
            {
                Fiber::CallFrame* frame;
                frame = this->setupCall(callee->function, argv, argc);
                if(frame == nullptr)
                {
                    RETURN_RUNTIME_ERROR();
                }
                frame->closure = callee;
                return this->execCall(frame);
            }

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

            char* readSource(std::string_view path, size_t* destlen, char** patched_file_name)
            {
                clock_t t;
                char* fname;
                char* source;
                t = 0;
                if(AST::measure_compilation_time)
                {
                    t = clock();
                }
                fname = Util::copyString(path);
                source = Util::readFile(fname, destlen);
                if(source == nullptr)
                {
                    this->raiseError(RUNTIME_ERROR, "failed to open file '%s' for reading", fname);
                }
                fname = Util::patchFilename(fname);
                if(AST::measure_compilation_time)
                {
                    printf("reading source: %gms\n", AST::last_source_time = (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
                }
                *patched_file_name = fname;
                return source;
            }

            Result interpretFile(std::string_view path)
            {
                size_t srclen;
                char* source;
                char* patchedpath;
                Result result;
                source = this->readSource(path.data(), &srclen, &patchedpath);
                if(source == nullptr)
                {
                    return INTERPRET_RUNTIME_FAIL;
                }
                result = this->interpretSource(patchedpath, source, srclen);
                free((void*)source);
                free(patchedpath);
                return result;
            }
            
            Result interpretSource(std::string_view modname, const char* code, size_t length)
            {
                return internalInterpret(String::copy(this, modname.data(), modname.size()), code, length);
            }

            Result interpretSource(std::string_view modname, std::string_view code)
            {
                return interpretSource(modname, code.data(), code.size());
            }

            Result internalInterpret(String* module_name, const char* code, size_t length)
            {
                intptr_t istack;
                intptr_t itop;
                intptr_t idif;
                Module* module;
                Fiber* fiber;
                Result result;
                module = this->compileModule(module_name, code, length);
                if(module == nullptr)
                {
                    return Result{ LITRESULT_COMPILE_ERROR, Object::NullVal };
                }
                result = this->execModule(module);
                fiber = module->main_fiber;
                if(!m_haderror && !fiber->abort && fiber->stack_top != fiber->stack)
                {
                    istack = (intptr_t)(fiber->stack);
                    itop = (intptr_t)(fiber->stack_top);
                    idif = (intptr_t)(fiber->stack - fiber->stack_top);
                    /* me fail english. how do i put this better? */
                    this->raiseError(RUNTIME_ERROR, "stack should be same as stack top", idif, istack, istack, itop, itop);
                }
                this->last_module = module;
                return result;
            }

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
                this->m_state->releaseObjects(this->objects);
                this->reset(this->m_state);
            }

            void reset(State* state)
            {
                this->m_state = state;
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
                *(fiber->stack_top) = value;
                fiber->stack_top++;
            }

            inline Value pop()
            {
                Value rt;
                rt = *(fiber->stack_top);
                fiber->stack_top--;
                return rt;
            }

            void closeUpvalues(const Value* last)
            {
                Fiber* fiber;
                Upvalue* upvalue;
                fiber = this->fiber;
                while(fiber->open_upvalues != nullptr && fiber->open_upvalues->location >= last)
                {
                    upvalue = fiber->open_upvalues;
                    upvalue->closed = *upvalue->location;
                    upvalue->location = &upvalue->closed;
                    fiber->open_upvalues = upvalue->next;
                }
            }

            bool dispatchCall(Function* function, Closure* closure, uint8_t arg_count)
            {
                bool vararg;
                size_t amount;
                size_t i;
                size_t osize;
                size_t newcapacity;
                size_t newsize;
                size_t vararg_count;
                size_t function_arg_count;
                Fiber::CallFrame* frame;
                Fiber* fiber;
                Array* array;
                fiber = this->fiber;

                #if 0
                //if(fiber->frame_count == LIT_CALL_FRAMES_MAX)
                //{
                    //lit_runtime_error(this, "call stack overflow");
                    //return true;
                //}
                #endif
                if(fiber->frame_count + 1 > fiber->frame_capacity)
                {
                    //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->frame_capacity * 2);
                    newcapacity = (fiber->frame_capacity * 2);
                    newsize = (sizeof(Fiber::CallFrame) * newcapacity);
                    osize = (sizeof(Fiber::CallFrame) * fiber->frame_capacity);
                    fiber->frames = (Fiber::CallFrame*)Memory::reallocate(this->m_state, fiber->frames, osize, newsize);
                    fiber->frame_capacity = newcapacity;
                }

                function_arg_count = function->arg_count;
                fiber->ensure_stack(function->max_slots + (int)(fiber->stack_top - fiber->stack));
                frame = &fiber->frames[fiber->frame_count++];
                frame->function = function;
                frame->closure = closure;
                frame->ip = function->chunk.m_code;
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
                            this->push(Object::NullVal);
                        }
                        if(vararg)
                        {
                            this->push(Array::make(this->m_state)->asValue());
                        }
                    }
                    else if(function->vararg)
                    {
                        array = Array::make(this->m_state);
                        vararg_count = arg_count - function_arg_count + 1;
                        this->m_state->pushRoot((Object*)array);
                        array->m_actualarray.reserve(vararg_count, Object::NullVal);
                        this->m_state->popRoot();
                        for(i = 0; i < vararg_count; i++)
                        {
                            array->m_actualarray.m_values[i] = this->fiber->stack_top[(int)i - (int)vararg_count];
                        }
                        this->fiber->stack_top -= vararg_count;
                        this->push(array->asValue());
                    }
                    else
                    {
                        this->fiber->stack_top -= (arg_count - function_arg_count);
                    }
                }
                else if(function->vararg)
                {
                    array = Array::make(this->m_state);
                    vararg_count = arg_count - function_arg_count + 1;
                    this->m_state->pushRoot((Object*)array);
                    array->push(*(fiber->stack_top - 1));
                    *(fiber->stack_top - 1) = array->asValue();
                    this->m_state->popRoot();
                }
                return true;
            }

            bool callValue(std::string_view name, Value callee, uint8_t arg_count);

            void markObject(Object* obj)
            {
                if(obj == nullptr || obj->marked)
                {
                    return;
                }
                obj->marked = true;
            #ifdef LIT_LOG_MARKING
                printf("%p mark ", (void*)obj);
                Object::print(obj->asValue());
                printf("\n");
            #endif
                if(this->gray_capacity < this->gray_count + 1)
                {
                    this->gray_capacity = LIT_GROW_CAPACITY(this->gray_capacity);
                    this->gray_stack = (Object**)realloc(this->gray_stack, sizeof(Object*) * this->gray_capacity);
                }
                this->gray_stack[this->gray_count++] = obj;
            }

            void markValue(Value value)
            {
                if(Object::isObject(value))
                {
                    markObject(Object::asObject(value));
                }
            }

            void markArray(PCGenericArray<Value>* array)
            {
                size_t i;
                for(i = 0; i < array->m_count; i++)
                {
                    this->markValue(array->m_values[i]);
                }
            }

            void markRoots();

            void blackenObject(Object* obj)
            {
                size_t i;
                Userdata* data;
                Function* function;
                Fiber* fiber;
                Upvalue* upvalue;
                Fiber::CallFrame* frame;
                Module* module;
                Closure* closure;
                Class* klass;
                BoundMethod* bound_method;
                Field* field;

            #ifdef LIT_LOG_BLACKING
                printf("%p blacken ", (void*)obj);
                Object::print(obj->asValue());
                printf("\n");
            #endif
                switch(obj->type)
                {
                    case Object::Type::NativeFunction:
                    case Object::Type::NativePrimitive:
                    case Object::Type::NativeMethod:
                    case Object::Type::PrimitiveMethod:
                    case Object::Type::Range:
                    case Object::Type::String:
                        {
                        }
                        break;
                    case Object::Type::Userdata:
                        {
                            data = (Userdata*)obj;
                            if(data->cleanup_fn != nullptr)
                            {
                                data->cleanup_fn(this->m_state, data, true);
                            }
                        }
                        break;
                    case Object::Type::Function:
                        {
                            function = (Function*)obj;
                            this->markObject((Object*)function->name);
                            this->markArray(&function->chunk.constants);
                        }
                        break;
                    case Object::Type::Fiber:
                        {
                            fiber = (Fiber*)obj;
                            for(Value* slot = fiber->stack; slot < fiber->stack_top; slot++)
                            {
                                this->markValue(*slot);
                            }
                            for(i = 0; i < fiber->frame_count; i++)
                            {
                                frame = &fiber->frames[i];
                                if(frame->closure != nullptr)
                                {
                                    this->markObject((Object*)frame->closure);
                                }
                                else
                                {
                                    this->markObject((Object*)frame->function);
                                }
                            }
                            for(upvalue = fiber->open_upvalues; upvalue != nullptr; upvalue = upvalue->next)
                            {
                                this->markObject((Object*)upvalue);
                            }
                            this->markValue(fiber->error);
                            this->markObject((Object*)fiber->module);
                            this->markObject((Object*)fiber->parent);
                        }
                        break;
                    case Object::Type::Module:
                        {
                            module = (Module*)obj;
                            this->markValue(module->return_value);
                            this->markObject((Object*)module->name);
                            this->markObject((Object*)module->main_function);
                            this->markObject((Object*)module->main_fiber);
                            this->markObject((Object*)module->private_names);
                            for(i = 0; i < module->private_count; i++)
                            {
                                this->markValue(module->privates[i]);
                            }
                        }
                        break;
                    case Object::Type::Closure:
                        {
                            closure = (Closure*)obj;
                            this->markObject((Object*)closure->function);
                            // Check for nullptr is needed for a really specific gc-case
                            if(closure->upvalues != nullptr)
                            {
                                for(i = 0; i < closure->upvalue_count; i++)
                                {
                                    this->markObject((Object*)closure->upvalues[i]);
                                }
                            }
                        }
                        break;
                    case Object::Type::Upvalue:
                        {
                            this->markValue(((Upvalue*)obj)->closed);
                        }
                        break;
                    case Object::Type::Class:
                        {
                            klass = (Class*)obj;
                            this->markObject((Object*)klass->name);
                            this->markObject((Object*)klass->super);
                            klass->methods.markForGC(this);
                            klass->static_fields.markForGC(this);
                        }
                        break;
                    case Object::Type::Instance:
                        {
                            Instance* instance = (Instance*)obj;
                            this->markObject((Object*)instance->klass);
                            instance->fields.markForGC(this);
                        }
                        break;
                    case Object::Type::BoundMethod:
                        {
                            bound_method = (BoundMethod*)obj;
                            this->markValue(bound_method->receiver);
                            this->markValue(bound_method->method);
                        }
                        break;
                    case Object::Type::Array:
                        {
                            this->markArray(&((Array*)obj)->m_actualarray);
                        }
                        break;
                    case Object::Type::Map:
                        {
                            ((Map*)obj)->m_values.markForGC(this);
                        }
                        break;
                    case Object::Type::Field:
                        {
                            field = (Field*)obj;
                            this->markObject((Object*)field->getter);
                            this->markObject((Object*)field->setter);
                        }
                        break;
                    case Object::Type::Reference:
                        {
                            this->markValue(*((Reference*)obj)->slot);
                        }
                        break;
                    default:
                        {
                            /* UNREACHABLE */
                        }
                        break;
                }
            }

            void traceReferences()
            {
                Object* obj;
                while(this->gray_count > 0)
                {
                    obj = this->gray_stack[--this->gray_count];
                    this->blackenObject(obj);
                }
            }

            void sweep()
            {
                Object* unreached;
                Object* previous;
                Object* obj;
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
                        Object::releaseObject(this->m_state, unreached);
                    }
                }
            }

            uint64_t collectGarbage()
            {
                clock_t t;
                uint64_t before;
                uint64_t collected;
                (void)t;
                if(!this->m_state->allow_gc)
                {
                    return 0;
                }

                this->m_state->allow_gc = false;
                before = this->m_state->bytes_allocated;

            #ifdef LIT_LOG_GC
                printf("-- gc begin\n");
                t = clock();
            #endif

                markRoots();
                this->traceReferences();
                this->strings.removeWhite();
                this->sweep();
                this->m_state->next_gc = this->m_state->bytes_allocated * LIT_GC_HEAP_GROW_FACTOR;
                this->m_state->allow_gc = true;
                collected = before - this->m_state->bytes_allocated;

            #ifdef LIT_LOG_GC
                printf("-- gc end. Collected %imb in %gms\n", ((int)((collected / 1024.0 + 0.5) / 10)) * 10,
                       (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            #endif
                return collected;
            }
    };

    class BinaryData
    {
        public:
            static void storeFunction(FILE* file, Function* function)
            {
                storeChunk(file, &function->chunk);
                FileIO::write_string(file, function->name);
                FileIO::write_uint8_t(file, function->arg_count);
                FileIO::write_uint16_t(file, function->upvalue_count);
                FileIO::write_uint8_t(file, (uint8_t)function->vararg);
                FileIO::write_uint16_t(file, (uint16_t)function->max_slots);
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
                FileIO::write_uint32_t(file, chunk->m_count);
                for(i = 0; i < chunk->m_count; i++)
                {
                    FileIO::write_uint8_t(file, chunk->m_code[i]);
                }
                if(chunk->has_line_info)
                {
                    c = chunk->line_count * 2 + 2;
                    FileIO::write_uint32_t(file, c);
                    for(i = 0; i < c; i++)
                    {
                        FileIO::write_uint16_t(file, chunk->lines[i]);
                    }
                }
                else
                {
                    FileIO::write_uint32_t(file, 0);
                }
                FileIO::write_uint32_t(file, chunk->constants.m_count);
                for(i = 0; i < chunk->constants.m_count; i++)
                {
                    Value constant = chunk->constants.m_values[i];
                    if(Object::isObject(constant))
                    {
                        Object::Type type = Object::asObject(constant)->type;
                        FileIO::write_uint8_t(file, (uint8_t)(type) + 1);
                        switch(type)
                        {
                            case Object::Type::String:
                                {
                                    FileIO::write_string(file, Object::as<String>(constant));
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
                        FileIO::write_uint8_t(file, 0);
                        FileIO::write_double(file, Object::toNumber(constant));
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
                    chunk->lines = (uint16_t*)Memory::reallocate(state, nullptr, 0, sizeof(uint16_t) * count);
                    chunk->line_count = count;
                    chunk->line_capacity = count;
                    for(i = 0; i < count; i++)
                    {
                        chunk->lines[i] = file->read_euint16_t();
                    }
                }
                else
                {
                    chunk->has_line_info = false;
                }
                count = file->read_euint32_t();
                chunk->constants.m_values = (Value*)Memory::reallocate(state, nullptr, 0, sizeof(Value) * count);
                chunk->constants.m_count = count;
                chunk->constants.m_capacity = count;
                for(i = 0; i < count; i++)
                {
                    type = file->read_euint8_t();
                    if(type == 0)
                    {
                        chunk->constants.m_values[i] = Object::toValue(file->read_edouble());
                    }
                    else
                    {
                        switch((Object::Type)(type - 1))
                        {
                            case Object::Type::String:
                                {
                                    chunk->constants.m_values[i] = file->read_estring(state)->asValue();
                                }
                                break;
                            case Object::Type::Function:
                                {
                                    chunk->constants.m_values[i] = loadFunction(state, file, module)->asValue();
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

            static void storeModule(Module* module, FILE* file)
            {
                size_t i;
                bool disabled;
                Table* privates;
                disabled = AST::Optimizer::is_enabled(LITOPTSTATE_PRIVATE_NAMES);
                FileIO::write_string(file, module->name);
                FileIO::write_uint16_t(file, module->private_count);
                FileIO::write_uint8_t(file, (uint8_t)disabled);
                if(!disabled)
                {
                    privates = &module->private_names->m_values;
                    for(i = 0; i < module->private_count; i++)
                    {
                        if(privates->at(i).key != nullptr)
                        {
                            FileIO::write_string(file, privates->at(i).key);
                            FileIO::write_uint16_t(file, (uint16_t)Object::toNumber(privates->at(i).value));
                        }
                    }
                }
                storeFunction(file, module->main_function);
            }

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


    void State::setVMGlobal(String* name, Value val)
    {
        this->vm->globals->m_values.set(name, val);
    }

    bool State::getVMGlobal(String* name, Value* dest)
    {
        return this->vm->globals->m_values.get(name, dest);
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

    void lit_printf(State* state, const char* message, ...);

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

    void lit_open_libraries(State* state);
    void lit_open_core_library(State* state);
    void lit_open_math_library(State* state);
    void lit_open_file_library(State* state);
    void lit_open_gc_library(State* state);

    void lit_add_definition(State* state, const char* name);

    bool lit_preprocess(AST::Preprocessor* preprocessor, char* source);

    static const int8_t stack_effects[] = {
    #define OPCODE(_, effect) effect,
    #include "opcode.inc"
    #undef OPCODE
    };

    static bool did_setup_rules;

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


    // typimpls 


    template<typename Type>
    template<typename... ArgsT>
    inline void PCGenericArray<Type>::raiseError(const char* fmt, ArgsT&&... args)
    {
        lit_runtime_error(m_state->vm, fmt, std::forward<ArgsT>(args)...);
    }

    void Memory::setBytesAllocated(State* state, int64_t toadd)
    {
        state->bytes_allocated += toadd;
    }

    void Memory::runGCIfNeeded(State* state)
    {
        #ifdef LIT_STRESS_TEST_GC
        state->vm->collectGarbage();
        #endif
        if(state->bytes_allocated > state->next_gc)
        {
            state->vm->collectGarbage();
        }
    }

    void Memory::raiseMemoryError(State* state, const char* msg)
    {
        state->raiseError(RUNTIME_ERROR, msg);
        exit(111);
    }

    namespace AST
    {
        ParseRule Parser::rules[LITTOK_EOF + 1];

        void Expression::releaseExpression(State* state, Expression* expression)
        {
            if(expression == nullptr)
            {
                return;
            }

            switch(expression->type)
            {
                case Expression::Type::Literal:
                {
                    Memory::reallocate(state, expression, sizeof(ExprLiteral), 0);
                    break;
                }

                case Expression::Type::Binary:
                {
                    ExprBinary* expr = (ExprBinary*)expression;

                    if(!expr->ignore_left)
                    {
                        Expression::releaseExpression(state, expr->left);
                    }

                    Expression::releaseExpression(state, expr->right);

                    Memory::reallocate(state, expression, sizeof(ExprBinary), 0);
                    break;
                }

                case Expression::Type::Unary:
                {
                    Expression::releaseExpression(state, ((ExprUnary*)expression)->right);
                    Memory::reallocate(state, expression, sizeof(ExprUnary), 0);

                    break;
                }

                case Expression::Type::Variable:
                {
                    Memory::reallocate(state, expression, sizeof(ExprVar), 0);
                    break;
                }

                case Expression::Type::Assign:
                {
                    ExprAssign* expr = (ExprAssign*)expression;

                    Expression::releaseExpression(state, expr->to);
                    Expression::releaseExpression(state, expr->value);

                    Memory::reallocate(state, expression, sizeof(ExprAssign), 0);
                    break;
                }

                case Expression::Type::Call:
                {
                    ExprCall* expr = (ExprCall*)expression;

                    Expression::releaseExpression(state, expr->callee);
                    Expression::releaseExpression(state, expr->objexpr);

                    Expression::releaseExpressionList(state, &expr->args);

                    Memory::reallocate(state, expression, sizeof(ExprCall), 0);
                    break;
                }

                case Expression::Type::Get:
                {
                    Expression::releaseExpression(state, ((ExprIndexGet*)expression)->where);
                    Memory::reallocate(state, expression, sizeof(ExprIndexGet), 0);
                    break;
                }

                case Expression::Type::Set:
                {
                    ExprIndexSet* expr = (ExprIndexSet*)expression;

                    Expression::releaseExpression(state, expr->where);
                    Expression::releaseExpression(state, expr->value);

                    Memory::reallocate(state, expression, sizeof(ExprIndexSet), 0);
                    break;
                }

                case Expression::Type::Lambda:
                    {
                        ExprLambda* expr = (ExprLambda*)expression;
                        Expression::releaseParameters(state, &expr->parameters);
                        Expression::releaseStatement(state, expr->body);
                        Memory::reallocate(state, expression, sizeof(ExprLambda), 0);
                    }
                    break;
                case Expression::Type::Array:
                    {
                        Expression::releaseExpressionList(state, &((ExprArray*)expression)->values);
                        Memory::reallocate(state, expression, sizeof(ExprArray), 0);
                    }
                    break;
                case Expression::Type::Object:
                    {
                        ExprObject* map = (ExprObject*)expression;
                        map->keys.release();
                        Expression::releaseExpressionList(state, &map->values);
                        Memory::reallocate(state, expression, sizeof(ExprObject), 0);
                    }
                    break;
                case Expression::Type::Subscript:
                    {
                        ExprSubscript* expr = (ExprSubscript*)expression;
                        Expression::releaseExpression(state, expr->array);
                        Expression::releaseExpression(state, expr->index);
                        Memory::reallocate(state, expression, sizeof(ExprSubscript), 0);
                    }
                    break;
                case Expression::Type::This:
                    {
                        Memory::reallocate(state, expression, sizeof(ExprThis), 0);
                    }
                    break;
                case Expression::Type::Super:
                    {
                        Memory::reallocate(state, expression, sizeof(ExprSuper), 0);
                    }
                    break;
                case Expression::Type::Range:
                    {
                        ExprRange* expr = (ExprRange*)expression;
                        Expression::releaseExpression(state, expr->from);
                        Expression::releaseExpression(state, expr->to);
                        Memory::reallocate(state, expression, sizeof(ExprRange), 0);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        ExprIfClause* expr = (ExprIfClause*)expression;
                        Expression::releaseExpression(state, expr->condition);
                        Expression::releaseExpression(state, expr->if_branch);
                        Expression::releaseExpression(state, expr->else_branch);
                        Memory::reallocate(state, expression, sizeof(ExprIfClause), 0);
                    }
                    break;
                case Expression::Type::Interpolation:
                    {
                        Expression::releaseExpressionList(state, &((ExprInterpolation*)expression)->expressions);
                        Memory::reallocate(state, expression, sizeof(ExprInterpolation), 0);
                    }
                    break;
                case Expression::Type::Reference:
                    {
                        Expression::releaseExpression(state, ((ExprReference*)expression)->to);
                        Memory::reallocate(state, expression, sizeof(ExprReference), 0);
                    }
                    break;
                default:
                    {
                        state->raiseError(COMPILE_ERROR, "Unknown expression type %d", (int)expression->type);
                    }
                    break;
            }
        }

        void Expression::releaseStatement(State* state, Expression* statement)
        {
            if(statement == nullptr)
            {
                return;
            }
            switch(statement->type)
            {
                case Expression::Type::Expression:
                    {
                        Expression::releaseExpression(state, ((ExprStatement*)statement)->expression);
                        Memory::reallocate(state, statement, sizeof(ExprStatement), 0);
                    }
                    break;
                case Expression::Type::Block:
                    {
                        Expression::internalReleaseStatementList(state, &((StmtBlock*)statement)->statements);
                        Memory::reallocate(state, statement, sizeof(StmtBlock), 0);
                    }
                    break;
                case Expression::Type::VarDecl:
                    {
                        Expression::releaseExpression(state, ((StmtVar*)statement)->valexpr);
                        Memory::reallocate(state, statement, sizeof(StmtVar), 0);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        StmtIfClause* stmt = (StmtIfClause*)statement;
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseStatement(state, stmt->if_branch);
                        Expression::releaseAllocatedExpressionList(state, stmt->elseif_conditions);
                        Expression::releaseAllocatedStatementList(state, stmt->elseif_branches);
                        Expression::releaseStatement(state, stmt->else_branch);
                        Memory::reallocate(state, statement, sizeof(StmtIfClause), 0);
                    }
                    break;
                case Expression::Type::WhileLoop:
                    {
                        StmtWhileLoop* stmt = (StmtWhileLoop*)statement;
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtWhileLoop), 0);
                    }
                    break;
                case Expression::Type::ForLoop:
                    {
                        StmtForLoop* stmt = (StmtForLoop*)statement;
                        Expression::releaseExpression(state, stmt->increment);
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseExpression(state, stmt->exprinit);

                        Expression::releaseStatement(state, stmt->var);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtForLoop), 0);
                    }
                    break;
                case Expression::Type::ContinueClause:
                    {
                        Memory::reallocate(state, statement, sizeof(StmtContinue), 0);
                    }
                    break;
                case Expression::Type::BreakClause:
                    {
                        Memory::reallocate(state, statement, sizeof(StmtBreak), 0);
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        StmtFunction* stmt = (StmtFunction*)statement;
                        Expression::releaseStatement(state, stmt->body);
                        Expression::releaseParameters(state, &stmt->parameters);
                        Memory::reallocate(state, statement, sizeof(StmtFunction), 0);
                    }
                    break;
                case Expression::Type::ReturnClause:
                    {
                        Expression::releaseExpression(state, ((StmtReturn*)statement)->expression);
                        Memory::reallocate(state, statement, sizeof(StmtReturn), 0);
                    }
                    break;
                case Expression::Type::MethodDecl:
                    {
                        StmtMethod* stmt = (StmtMethod*)statement;
                        Expression::releaseParameters(state, &stmt->parameters);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtMethod), 0);
                    }
                    break;
                case Expression::Type::ClassDecl:
                    {
                        Expression::internalReleaseStatementList(state, &((StmtClass*)statement)->fields);
                        Memory::reallocate(state, statement, sizeof(StmtClass), 0);
                    }
                    break;
                case Expression::Type::FieldDecl:
                    {
                        StmtField* stmt = (StmtField*)statement;
                        Expression::releaseStatement(state, stmt->getter);
                        Expression::releaseStatement(state, stmt->setter);
                        Memory::reallocate(state, statement, sizeof(StmtField), 0);
                    }
                    break;
                default:
                    {
                        state->raiseError(COMPILE_ERROR, "Unknown statement type %d", (int)statement->type);
                    }
                    break;
            }
        }

        void Compiler::init(Parser* parser)
        {
            this->scope_depth = 0;
            this->function = nullptr;
            this->enclosing = (Compiler*)parser->m_compiler;
            parser->m_compiler = this;
        }


        const char* Emitter::getStateScannerFilename()
        {
            return this->m_state->scanner->m_filename;
        }

        bool Optimizer::is_enabled(Optimization optimization)
        {
            if(!optimization_states_setup)
            {
                setup_optimization_states();
            }
            return optimization_states[(int)optimization];
        }

        void Optimizer::set_enabled(Optimization optimization, bool enabled)
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

        void Optimizer::set_all_enabled(bool enabled)
        {
            size_t i;
            optimization_states_setup = true;
            any_optimization_enabled = enabled;
            for(i = 0; i < LITOPTSTATE_TOTAL; i++)
            {
                optimization_states[i] = enabled;
            }
        }

        const char* Optimizer::get_optimization_name(Optimization optimization)
        {
            return optimization_names[(int)optimization];
        }

        const char* Optimizer::get_optimization_description(Optimization optimization)
        {
            return optimization_descriptions[(int)optimization];
        }

        const char* Optimizer::get_optimization_level_description(OptimizationLevel level)
        {
            return optimization_level_descriptions[(int)level];
        }

        void Optimizer::optimize(Expression::List* statements)
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
            opt_begin_scope();
            optimize_statements(statements);
            opt_end_scope();
            m_variables.release();
        }

        void Parser::init(State* state)
        {
            if(!did_setup_rules)
            {
                did_setup_rules = true;
                Parser::setup_rules();
            }
            this->m_state = state;
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
            this->m_state->raiseError(COMPILE_ERROR, message);
            m_haderror = true;
            this->sync();
        }

        Scanner* Parser::getStateScanner(State* state)
        {
            return state->scanner;
        }

        void Emitter::error(size_t line, Error error, ...)
        {
            va_list args;
            va_start(args, error);
            this->m_state->raiseError(COMPILE_ERROR, lit_vformat_error(this->m_state, line, error, args)->data());
            va_end(args);
        }

        int8_t Emitter::getStackEffect(int idx)
        {
            return stack_effects[idx];
        }

        Module* Emitter::run_emitter(Expression::List& statements, String* module_name)
        {
            size_t i;
            size_t total;
            size_t old_privates_count;
            bool isnew;
            State* state;        
            Value module_value;
            Module* module;
            PCGenericArray<Private>* privates;
            Compiler compiler;
            Expression* stmt;
            m_lastline = 1;
            m_emitreference = 0;
            state = this->m_state;
            isnew = false;
            if(this->m_state->vm->modules->m_values.get(module_name, &module_value))
            {
                module = Object::as<Module>(module_value);
            }
            else
            {
                module = Module::make(this->m_state, module_name);
                isnew = true;
            }
            m_module = module;
            old_privates_count = module->private_count;
            if(old_privates_count > 0)
            {
                privates = &m_privates;
                privates->m_count = old_privates_count - 1;
                privates->push(Private{ true, false });
                for(i = 0; i < old_privates_count; i++)
                {
                    privates->m_values[i].initialized = true;
                }
            }
            init_compiler(&compiler, LITFUNC_SCRIPT);
            m_chunk = &compiler.function->chunk;
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
            end_scope(m_lastline);
            module->main_function = end_compiler(module_name);
            if(isnew)
            {
                total = m_privates.m_count;
                module->privates = LIT_ALLOCATE(this->m_state, Value, total);
                for(i = 0; i < total; i++)
                {
                    module->privates[i] = Object::NullVal;
                }
            }
            else
            {
                module->privates = LIT_GROW_ARRAY(this->m_state, module->privates, Value, old_privates_count, module->private_count);
                for(i = old_privates_count; i < module->private_count; i++)
                {
                    module->privates[i] = Object::NullVal;
                }
            }
            m_privates.release();
            if(Optimizer::is_enabled(LITOPTSTATE_PRIVATE_NAMES))
            {
                m_module->private_names->m_values.release();
            }
            if(isnew && !state->m_haderror)
            {
                state->vm->modules->m_values.set(module_name, module->asValue());
            }
            module->ran = true;
            return module;
        }
    }

    // impl::chunk
    size_t Chunk::add_constant(Value constant)
    {
        for(size_t i = 0; i < this->constants.m_count; i++)
        {
            if(this->constants.m_values[i] == constant)
            {
                return i;
            }
        }

        this->m_state->pushValueRoot(constant);
        this->constants.push(constant);
        this->m_state->popRoot();

        return this->constants.m_count - 1;
    }

    // impl::fiber
    bool Fiber::ensureFiber(VM* vm, Fiber* fiber)
    {
        return ensureFiber(vm->m_state, vm, fiber);
    }

    // impl::state
    void State::init(VM* vm)
    {
        vm->reset(this);
        vm->globals = Map::make(this);
        vm->modules = Map::make(this);
    }

    State* State::make()
    {
        State* state;
        state = (State*)malloc(sizeof(State));
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
        state->m_haderror = false;
        state->roots = nullptr;
        state->root_count = 0;
        state->root_capacity = 0;
        state->last_module = nullptr;
        state->debugwriter.initFile(state, stdout, true);
        state->preprocessor = (AST::Preprocessor*)malloc(sizeof(AST::Preprocessor));
        state->preprocessor->init(state);
        state->scanner = (AST::Scanner*)malloc(sizeof(AST::Scanner));
        state->parser = (AST::Parser*)malloc(sizeof(AST::Parser));
        ((AST::Parser*)state->parser)->init(state);
        state->emitter = (AST::Emitter*)malloc(sizeof(AST::Emitter));
        state->emitter->init(state);
        state->optimizer = (AST::Optimizer*)malloc(sizeof(AST::Optimizer));
        state->optimizer->init(state);
        state->vm = (VM*)malloc(sizeof(VM));
        state->init(state->vm);
        init_api(state);
        lit_open_core_library(state);
        return state;
    }

    int64_t State::release()
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

    Module* State::compileModule(String* module_name, const char* code, size_t length)
    {
        clock_t t;
        clock_t total_t;
        bool allowed_gc;
        Module* module;
        AST::Expression::List statements;
        allowed_gc = this->allow_gc;
        this->allow_gc = false;
        m_haderror = false;
        module = nullptr;
        // This is a lbc format
        if((code[1] << 8 | code[0]) == LIT_BYTECODE_MAGIC_NUMBER)
        {
            module = BinaryData::loadModule(this, code, length);
        }
        else
        {
            t = 0;
            total_t = 0;
            if(AST::measure_compilation_time)
            {
                total_t = t = clock();
            }
            /*
            if(!lit_preprocess(this->preprocessor, code))
            {
                return nullptr;
            }
            */
            if(AST::measure_compilation_time)
            {
                printf("-----------------------\nPreprocessing:  %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
                t = clock();
            }
            statements.init(this);
            if(this->parser->parse(module_name->data(), code, length, statements))
            {
                AST::Expression::releaseStatementList(this, &statements);
                return nullptr;
            }
            if(AST::measure_compilation_time)
            {
                printf("Parsing:        %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
                t = clock();
            }
            this->optimizer->optimize(&statements);
            if(AST::measure_compilation_time)
            {
                printf("Optimization:   %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
                t = clock();
            }
            module = this->emitter->run_emitter(statements, module_name);
            AST::Expression::releaseStatementList(this, &statements);
            if(AST::measure_compilation_time)
            {
                printf("Emitting:       %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
                printf("\nTotal:          %gms\n-----------------------\n",
                       (double)(clock() - total_t) / CLOCKS_PER_SEC * 1000 + AST::last_source_time);
            }
        }
        this->allow_gc = allowed_gc;
        return m_haderror ? nullptr : module;
    }

    Fiber* State::getVMFiber()
    {
        return vm->fiber;
    }

    void State::releaseObjects(Object* objects)
    {
        Object* obj;
        obj = objects;
        while(obj != nullptr)
        {
            Object* next = obj->next;
            Object::releaseObject(this, obj);
            obj = next;
        }
        free(this->vm->gray_stack);
        this->vm->gray_capacity = 0;
    }

    Upvalue* State::captureUpvalue(Value* local)
    {
        Upvalue* upvalue;
        Upvalue* created_upvalue;
        Upvalue* previous_upvalue;
        previous_upvalue = nullptr;
        upvalue = this->vm->fiber->open_upvalues;
        while(upvalue != nullptr && upvalue->location > local)
        {
            previous_upvalue = upvalue;
            upvalue = upvalue->next;
        }
        if(upvalue != nullptr && upvalue->location == local)
        {
            return upvalue;
        }
        created_upvalue = Upvalue::make(this, local);
        created_upvalue->next = upvalue;
        if(previous_upvalue == nullptr)
        {
            this->vm->fiber->open_upvalues = created_upvalue;
        }
        else
        {
            previous_upvalue->next = created_upvalue;
        }
        return created_upvalue;
    }

    Result State::execModule(Module* module)
    {
        VM* vm;
        Fiber* fiber;
        Result result;
        vm = this->vm;
        fiber = Fiber::make(this, module, module->main_function);
        vm->fiber = fiber;
        vm->push(module->main_function->asValue());
        result = this->execFiber(fiber);
        return result;
    }

    Fiber::CallFrame* State::setupCall(Function* callee, Value* argv, size_t argc)
    {
        bool vararg;
        int amount;
        size_t i;
        size_t varargc;
        size_t function_arg_count;
        VM* vm;
        Fiber* fiber;
        Fiber::CallFrame* frame;
        Array* array;
        (void)argc;
        (void)varargc;
        vm = this->vm;
        fiber = vm->fiber;
        if(callee == nullptr)
        {
            lit_runtime_error(vm, "attempt to call a null function object");
            return nullptr;
        }
        if(Fiber::ensureFiber(vm, fiber))
        {
            return nullptr;
        }
        fiber->ensure_stack(callee->max_slots + (int)(fiber->stack_top - fiber->stack));
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
                    PUSH(Object::NullVal);
                }
                if(vararg)
                {
                    PUSH(Array::make(vm->m_state)->asValue());
                }
            }
            else if(callee->vararg)
            {
                array = Array::make(vm->m_state);
                varargc = argc - function_arg_count + 1;
                array->m_actualarray.reserve(varargc + 1, Object::NullVal);
                for(i = 0; i < varargc; i++)
                {
                    array->m_actualarray.m_values[i] = fiber->stack_top[(int)i - (int)varargc];
                }

                fiber->stack_top -= varargc;
                vm->push(array->asValue());
            }
            else
            {
                fiber->stack_top -= (argc - function_arg_count);
            }
        }
        else if(callee->vararg)
        {
            array = Array::make(vm->m_state);
            varargc = argc - function_arg_count + 1;
            array->push(*(fiber->stack_top - 1));
            *(fiber->stack_top - 1) = array->asValue();
        }
        frame->ip = callee->chunk.m_code;
        frame->closure = nullptr;
        frame->function = callee;
        frame->result_ignored = false;
        frame->return_to_c = true;
        return frame;
    }

    Result State::findAndCallMethod(Value callee, String* method_name, Value* argv, size_t argc)
    {
        Class* klass;
        VM* vm;
        Fiber* fiber;
        Value mthval;
        vm = this->vm;
        fiber = vm->fiber;
        if(fiber == nullptr)
        {
            lit_runtime_error(vm, "no fiber to run on");
            RETURN_RUNTIME_ERROR();
        }
        klass = Class::getClassFor(this, callee);
        if((Object::isInstance(callee) && Object::as<Instance>(callee)->fields.get(method_name, &mthval)) || klass->methods.get(method_name, &mthval))
        {
            return this->callMethod(callee, mthval, argv, argc);
        }
        return Result{ LITRESULT_INVALID, Object::NullVal };
    }

    Result State::execCall(Fiber::CallFrame* frame)
    {
        Fiber* fiber;
        Result result;
        if(frame == nullptr)
        {
            RETURN_RUNTIME_ERROR();
        }
        fiber = this->vm->fiber;
        result = this->execFiber(fiber);
        if(fiber->error != Object::NullVal)
        {
            result.result = fiber->error;
        }
        return result;
    }

    Result State::callMethod(Value instance, Value callee, Value* argv, size_t argc)
    {
        uint8_t i;
        VM* vm;
        Result lir;
        Object::Type type;
        Class* klass;
        Fiber* fiber;
        Value* slot;
        NativeMethod* natmethod;
        BoundMethod* bound_method;
        Value mthval;
        Value result;
        lir.result = Object::NullVal;
        lir.type = LITRESULT_OK;
        vm = this->vm;
        if(Object::isObject(callee))
        {
            if(this->set_native_exit_jump())
            {
                RETURN_RUNTIME_ERROR();
            }
            type = OBJECT_TYPE(callee);

            if(type == Object::Type::Function)
            {
                return this->callFunction(Object::as<Function>(callee), argv, argc);
            }
            else if(type == Object::Type::Closure)
            {
                return this->callClosure(Object::as<Closure>(callee), argv, argc);
            }
            fiber = vm->fiber;
            if(Fiber::ensureFiber(vm, fiber))
            {
                RETURN_RUNTIME_ERROR();
            }
            fiber->ensure_stack(3 + argc + (int)(fiber->stack_top - fiber->stack));
            slot = fiber->stack_top;
            PUSH(instance);
            if(type != Object::Type::Class)
            {
                for(i = 0; i < argc; i++)
                {
                    PUSH(argv[i]);
                }
            }
            switch(type)
            {
                case Object::Type::NativeFunction:
                    {
                        Value result = Object::as<NativeFunction>(callee)->function(vm, argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(result);
                    }
                    break;
                case Object::Type::NativePrimitive:
                    {
                        Object::as<NativePrimFunction>(callee)->function(vm, argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(Object::NullVal);
                    }
                    break;
                case Object::Type::NativeMethod:
                    {
                        natmethod = Object::as<NativeMethod>(callee);
                        result = natmethod->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(result);
                    }
                    break;
                case Object::Type::Class:
                    {
                        klass = Object::as<Class>(callee);
                        *slot = Instance::make(vm->m_state, klass)->asValue();
                        if(klass->init_method != nullptr)
                        {
                            lir = this->callMethod(*slot, klass->init_method->asValue(), argv, argc);
                        }
                        // TODO: when should this return *slot instead of lir?
                        fiber->stack_top = slot;
                        //RETURN_OK(*slot);
                        return lir;
                    }
                    break;
                case Object::Type::BoundMethod:
                    {
                        bound_method = Object::as<BoundMethod>(callee);
                        mthval = bound_method->method;
                        *slot = bound_method->receiver;
                        if(Object::isNativeMethod(mthval))
                        {
                            result = Object::as<NativeMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);
                            fiber->stack_top = slot;
                            RETURN_OK(result);
                        }
                        else if(Object::isPrimitiveMethod(mthval))
                        {
                            Object::as<PrimitiveMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->stack_top - argc);

                            fiber->stack_top = slot;
                            RETURN_OK(Object::NullVal);
                        }
                        else
                        {
                            fiber->stack_top = slot;
                            return this->callFunction(Object::as<Function>(mthval), argv, argc);
                        }
                    }
                    break;
                case Object::Type::PrimitiveMethod:
                    {
                        Object::as<PrimitiveMethod>(callee)->method(vm, *(fiber->stack_top - argc - 1), argc, fiber->stack_top - argc);
                        fiber->stack_top = slot;
                        RETURN_OK(Object::NullVal);
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
        }
        if(Object::isNull(callee))
        {
            lit_runtime_error(vm, "attempt to call a null value of a '%s'", Object::toString(this, instance)->data());
        }
        else
        {
            lit_runtime_error(vm, "can only call functions and classes");
        }

        RETURN_RUNTIME_ERROR();
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

    #define vm_readshort(ip) \
        (ip += 2u, (uint16_t)((ip[-2] << 8u) | ip[-1]))

    #define vm_readconstant(current_chunk) \
        (current_chunk->constants.m_values[vm_readbyte(ip)])

    #define vm_readconstantlong(current_chunk, ip) \
        (current_chunk->constants.m_values[vm_readshort(ip)])

    #define vm_readstring(current_chunk) \
        Object::as<String>(vm_readconstant(current_chunk))

    #define vm_readstringlong(current_chunk, ip) \
        Object::as<String>(vm_readconstantlong(current_chunk, ip))


    static inline Value vm_peek(Fiber* fiber, short distance)
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
        vm_popgc(this); \
        return (Result){ LITRESULT_RUNTIME_ERROR, Object::NullVal };

    #define vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues) \
        vm_writeframe(frame, ip); \
        fiber = vm->fiber; \
        if(fiber == nullptr) \
        { \
            return (Result){ LITRESULT_OK, vm_pop(fiber) }; \
        } \
        if(fiber->abort) \
        { \
            vm_returnerror(); \
        } \
        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues); \
        vm_traceframe(fiber);

    #define vm_callvalue(name, callee, arg_count) \
        if(vm->callValue(name, callee, arg_count)) \
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
        Value mthval; \
        if((Object::isInstance(callee) && (Object::as<Instance>(callee)->fields.get(method_name, &mthval))) \
           || zklass->stat.get(method_name, &mthval)) \
        { \
            if(ignoring) \
            { \
                if(vm->callValue(method_name->data(), mthval, arg_count)) \
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
                vm_callvalue(method_name->data(), mthval, arg_count); \
            } \
        } \
        else \
        { \
            if(error) \
            { \
                vm_rterrorvarg("Attempt to call method '%s', that is not defined in class %s", method_name->data(), \
                                   zklass->name->data()) \
            } \
        } \
        if(error) \
        { \
            continue; \
        }

    #define vm_invoke_from_class(klass, method_name, arg_count, error, stat, ignoring) \
        vm_invoke_from_class_advanced(klass, method_name, arg_count, error, stat, ignoring, vm_peek(fiber, arg_count))

    #define vm_invokemethod(instance, method_name, arg_count) \
        Class* klass = Class::getClassFor(this, instance); \
        if(klass == nullptr) \
        { \
            vm_rterror("invokemethod: only instances and classes have methods"); \
        } \
        vm_writeframe(frame, ip); \
        vm_invoke_from_class_advanced(klass, String::intern(this, method_name), arg_count, true, methods, false, instance); \
        vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues)

    #define vm_binaryop(type, op, op_string) \
        Value a = vm_peek(fiber, 1); \
        Value b = vm_peek(fiber, 0); \
        if(Object::isNumber(a)) \
        { \
            if(!Object::isNumber(b)) \
            { \
                if(!Object::isNull(b)) \
                { \
                    vm_rterrorvarg("Attempt to use op %s with a number and a %s", op_string, Object::valueName(b)); \
                } \
            } \
            vm_drop(fiber); \
            *(fiber->stack_top - 1) = (type(Object::toNumber(a) op Object::toNumber(b))); \
            continue; \
        } \
        if(Object::isNull(a)) \
        { \
        /* vm_rterrorvarg("Attempt to use op %s on a null value", op_string); */ \
            vm_drop(fiber); \
            *(fiber->stack_top - 1) = Object::TrueVal; \
        } \
        else \
        { \
            vm_invokemethod(a, op_string, 1); \
        }

    #define vm_bitwiseop(op, op_string) \
        Value a = vm_peek(fiber, 1); \
        Value b = vm_peek(fiber, 0); \
        if(!Object::isNumber(a) || !Object::isNumber(b)) \
        { \
            vm_rterrorvarg("Operands of bitwise op %s must be two numbers, got %s and %s", op_string, \
                               Object::valueName(a), Object::valueName(b)); \
        } \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = (Object::toValue((int)Object::toNumber(a) op(int) Object::toNumber(b)));

    #define vm_invokeoperation(ignoring) \
        uint8_t arg_count = vm_readbyte(ip); \
        String* method_name = vm_readstringlong(current_chunk, ip); \
        Value receiver = vm_peek(fiber, arg_count); \
        if(Object::isNull(receiver)) \
        { \
            vm_rterrorvarg("Attempt to index a null value with '%s'", method_name->data()); \
        } \
        vm_writeframe(frame, ip); \
        if(Object::isClass(receiver)) \
        { \
            vm_invoke_from_class_advanced(Object::as<Class>(receiver), method_name, arg_count, true, static_fields, ignoring, receiver); \
            continue; \
        } \
        else if(Object::isInstance(receiver)) \
        { \
            Instance* instance = Object::as<Instance>(receiver); \
            Value value; \
            if(instance->fields.get(method_name, &value)) \
            { \
                fiber->stack_top[-arg_count - 1] = value; \
                vm_callvalue(method_name->data(), value, arg_count); \
                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues); \
                continue; \
            } \
            vm_invoke_from_class_advanced(instance->klass, method_name, arg_count, true, methods, ignoring, receiver); \
        } \
        else \
        { \
            Class* type = Class::getClassFor(this, receiver); \
            if(type == nullptr) \
            { \
                vm_rterror("invokeoperation: only instances and classes have methods"); \
            } \
            vm_invoke_from_class_advanced(type, method_name, arg_count, true, methods, ignoring, receiver); \
        }

    static void reset_stack(VM* vm)
    {
        if(vm->fiber != nullptr)
        {
            vm->fiber->stack_top = vm->fiber->stack;
        }
    }

    Result State::execFiber(Fiber* fiber)
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
        Fiber::CallFrame* frame;
        Chunk* current_chunk;
        Class* instance_klass;
        Class* klassobj;
        Class* super_klass;
        Class* type;
        Closure* closure;
        Fiber* parent;
        Field* field;
        Function* function;
        Instance* instobj;
        String* field_name;
        String* method_name;
        String* name;
        Upvalue** upvalues;
        Value a;
        Value arg;
        Value b;
        Value getval;
        Value instval;
        Value klassval;
        Value vobj;
        Value operand;
        Value reference;
        Value result;
        Value setter;
        Value setval;
        Value slot;
        Value super;
        Value tmpval;
        Value value;
        Value* privates;
        Value* pval;
        Value* slots;
        PCGenericArray<Value>* values;
        VM* vm;
        (void)instruction;
        vm = this->vm;
        vm_pushgc(this, true);
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
                #include "opcode.inc"
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
                    lit_disassemble_instruction(this, current_chunk, (size_t)(ip - current_chunk->m_code - 1), nullptr);
                    goto* dispatch_table[instruction];
                #else
                    goto* dispatch_table[*ip++];
                #endif
            #else
                instruction = *ip++;
                #ifdef LIT_TRACE_EXECUTION
                    lit_disassemble_instruction(this, current_chunk, (size_t)(ip - current_chunk->m_code - 1), nullptr);
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
                    vm->closeUpvalues(slots);
                    vm_writeframe(frame, ip);
                    fiber->frame_count--;
                    if(frame->return_to_c)
                    {
                        frame->return_to_c = false;
                        fiber->module->return_value = result;
                        fiber->stack_top = frame->slots;
                        return (Result){ LITRESULT_OK, result };
                    }
                    if(fiber->frame_count == 0)
                    {
                        fiber->module->return_value = result;
                        if(fiber->parent == nullptr)
                        {
                            vm_drop(fiber);
                            this->allow_gc = was_allowed;
                            return (Result){ LITRESULT_OK, result };
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
                    vm_push(fiber, Object::TrueVal);
                    continue;
                }
                op_case(FALSE)
                {
                    vm_push(fiber, Object::FalseVal);
                    continue;
                }
                op_case(NULL)
                {
                    vm_push(fiber, Object::NullVal);
                    continue;
                }
                op_case(ARRAY)
                {
                    vm_push(fiber, Array::make(this)->asValue());
                    continue;
                }
                op_case(OBJECT)
                {
                    // TODO: use object, or map for literal '{...}' constructs?
                    // objects would be more general-purpose, but don't implement anything map-like.
                    //vm_push(fiber, Instance::make(this, state->objectvalue_class)->asValue());
                    vm_push(fiber, Map::make(this)->asValue());

                    continue;
                }
                op_case(RANGE)
                {
                    a = vm_pop(fiber);
                    b = vm_pop(fiber);
                    if(!Object::isNumber(a) || !Object::isNumber(b))
                    {
                        vm_rterror("Range operands must be number");
                    }
                    vm_push(fiber, Range::make(this, Object::toNumber(a), Object::toNumber(b))->asValue());
                    continue;
                }
                op_case(NEGATE)
                {
                    if(!Object::isNumber(vm_peek(fiber, 0)))
                    {
                        arg = vm_peek(fiber, 0);
                        // Don't even ask me why
                        // This doesn't kill our performance, since it's a error anyway
                        if(Object::isString(arg) && strcmp(Object::asString(arg)->data(), "muffin") == 0)
                        {
                            vm_rterror("Idk, can you negate a muffin?");
                        }
                        else
                        {
                            vm_rterror("Operand must be a number");
                        }
                    }
                    tmpval = Object::toValue(-Object::toNumber(vm_pop(fiber)));
                    vm_push(fiber, tmpval);
                    continue;
                }
                op_case(NOT)
                {
                    if(Object::isInstance(vm_peek(fiber, 0)))
                    {
                        vm_writeframe(frame, ip);
                        vm_invoke_from_class(Object::as<Instance>(vm_peek(fiber, 0))->klass, String::intern(this, "!"), 0, false, methods, false);
                        continue;
                    }
                    tmpval = BOOL_VALUE(Object::isFalsey(vm_pop(fiber)));
                    vm_push(fiber, tmpval);
                    continue;
                }
                op_case(BNOT)
                {
                    if(!Object::isNumber(vm_peek(fiber, 0)))
                    {
                        vm_rterror("Operand must be a number");
                    }
                    tmpval = Object::toValue(~((int)Object::toNumber(vm_pop(fiber))));
                    vm_push(fiber, tmpval);
                    continue;
                }
                op_case(ADD)
                {
                    vm_binaryop(Object::toValue, +, "+");
                    continue;
                }
                op_case(SUBTRACT)
                {
                    vm_binaryop(Object::toValue, -, "-");
                    continue;
                }
                op_case(MULTIPLY)
                {
                    vm_binaryop(Object::toValue, *, "*");
                    continue;
                }
                op_case(POWER)
                {
                    a = vm_peek(fiber, 1);
                    b = vm_peek(fiber, 0);
                    if(Object::isNumber(a) && Object::isNumber(b))
                    {
                        vm_drop(fiber);
                        *(fiber->stack_top - 1) = (Object::toValue(pow(Object::toNumber(a), Object::toNumber(b))));
                        continue;
                    }
                    vm_invokemethod(a, "**", 1);
                    continue;
                }
                op_case(DIVIDE)
                {
                    vm_binaryop(Object::toValue, /, "/");
                    continue;
                }
                op_case(FLOOR_DIVIDE)
                {
                    a = vm_peek(fiber, 1);
                    b = vm_peek(fiber, 0);
                    if(Object::isNumber(a) && Object::isNumber(b))
                    {
                        vm_drop(fiber);
                        *(fiber->stack_top - 1) = (Object::toValue(floor(Object::toNumber(a) / Object::toNumber(b))));

                        continue;
                    }

                    vm_invokemethod(a, "#", 1);
                    continue;
                }
                op_case(MOD)
                {
                    a = vm_peek(fiber, 1);
                    b = vm_peek(fiber, 0);
                    if(Object::isNumber(a) && Object::isNumber(b))
                    {
                        vm_drop(fiber);
                        *(fiber->stack_top - 1) = Object::toValue(fmod(Object::toNumber(a), Object::toNumber(b)));
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
                    if(Object::isInstance(vm_peek(fiber, 1)))
                    {
                        vm_writeframe(frame, ip);
                        fprintf(stderr, "OP_EQUAL: trying to invoke '==' ...\n");
                        vm_invoke_from_class(Object::as<Instance>(vm_peek(fiber, 1))->klass, String::intern(state, "=="), 1, false, methods, false);
                        continue;
                    }
                    a = vm_pop(fiber);
                    b = vm_pop(fiber);
                    vm_push(fiber, BOOL_VALUE(a == b));
                    */
                    vm_binaryop(Object::toValue, ==, "==");
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
                    vm->globals->m_values.set(name, vm_peek(fiber, 0));
                    continue;
                }

                op_case(GET_GLOBAL)
                {
                    name = vm_readstringlong(current_chunk, ip);
                    if(!vm->globals->m_values.get(name, &setval))
                    {
                        vm_push(fiber, Object::NullVal);
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
                    if(Object::isFalsey(vm_pop(fiber)))
                    {
                        ip += offset;
                    }
                    continue;
                }
                op_case(JUMP_IF_NULL)
                {
                    offset = vm_readshort(ip);
                    if(Object::isNull(vm_peek(fiber, 0)))
                    {
                        ip += offset;
                    }
                    continue;
                }
                op_case(JUMP_IF_NULL_POPPING)
                {
                    offset = vm_readshort(ip);
                    if(Object::isNull(vm_pop(fiber)))
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
                    if(Object::isFalsey(vm_peek(fiber, 0)))
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
                    if(Object::isFalsey(vm_peek(fiber, 0)))
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
                    if(Object::isNull(vm_peek(fiber, 0)))
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
                    const char* name = "unknown";
                    arg_count = vm_readbyte(ip);
                    vm_writeframe(frame, ip);
                    //auto nameval = vm_peek(fiber, arg_count-1);
                    //fprintf(stderr, "arg_count-1=%s\n", Object::toString(this, nameval)->data());
                    // todo: figure out callee name!
                    vm_callvalue(name, vm_peek(fiber, arg_count), arg_count);
                    continue;
                }
                op_case(CLOSURE)
                {
                    function = Object::as<Function>(vm_readconstantlong(current_chunk, ip));
                    closure = Closure::make(this, function);
                    vm_push(fiber, closure->asValue());
                    for(i = 0; i < closure->upvalue_count; i++)
                    {
                        is_local = vm_readbyte(ip);
                        index = vm_readbyte(ip);
                        if(is_local)
                        {
                            closure->upvalues[i] = this->captureUpvalue(frame->slots + index);
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
                    vm->closeUpvalues(fiber->stack_top - 1);
                    vm_drop(fiber);
                    continue;
                }
                op_case(CLASS)
                {
                    name = vm_readstringlong(current_chunk, ip);
                    klassobj = Class::make(this, name);
                    vm_push(fiber, klassobj->asValue());
                    klassobj->super = this->objectvalue_class;
                    klassobj->super->methods.addAll(&klassobj->methods);
                    klassobj->super->static_fields.addAll(&klassobj->static_fields);
                    vm->globals->m_values.set(name, klassobj->asValue());
                    continue;
                }
                op_case(GET_FIELD)
                {
                    vobj = vm_peek(fiber, 1);
                    if(Object::isNull(vobj))
                    {
                        vm_rterror("Attempt to index a null value");
                    }
                    name = Object::as<String>(vm_peek(fiber, 0));
                    if(Object::isInstance(vobj))
                    {
                        instobj = Object::as<Instance>(vobj);

                        if(!instobj->fields.get(name, &getval))
                        {
                            if(instobj->klass->methods.get(name, &getval))
                            {
                                if(Object::isField(getval))
                                {
                                    field = Object::as<Field>(getval);
                                    if(field->getter == nullptr)
                                    {
                                        vm_rterrorvarg("Class %s does not have a getter for the field %s",
                                                           instobj->klass->name->data(), name->data());
                                    }
                                    vm_drop(fiber);
                                    vm_writeframe(frame, ip);
                                    vm_callvalue(name->data(), Object::as<Field>(getval)->getter->asValue(), 0);
                                    vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                                    continue;
                                }
                                else
                                {
                                    getval = BoundMethod::make(this, instobj->asValue(), getval)->asValue();
                                }
                            }
                            else
                            {
                                getval = Object::NullVal;
                            }
                        }
                    }
                    else if(Object::isClass(vobj))
                    {
                        klassobj = Object::as<Class>(vobj);
                        if(klassobj->static_fields.get(name, &getval))
                        {
                            if(Object::isNativeMethod(getval) || Object::isPrimitiveMethod(getval))
                            {
                                getval = BoundMethod::make(this, klassobj->asValue(), getval)->asValue();
                            }
                            else if(Object::isField(getval))
                            {
                                field = Object::as<Field>(getval);
                                if(field->getter == nullptr)
                                {
                                    vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->data(),
                                                       name->data());
                                }
                                vm_drop(fiber);
                                vm_writeframe(frame, ip);
                                vm_callvalue(name->data(), field->getter->asValue(), 0);
                                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                                continue;
                            }
                        }
                        else
                        {
                            getval = Object::NullVal;
                        }
                    }
                    else
                    {
                        klassobj = Class::getClassFor(this, vobj);
                        if(klassobj == nullptr)
                        {
                            vm_rterror("GET_FIELD: only instances and classes have fields");
                        }
                        if(klassobj->methods.get(name, &getval))
                        {
                            if(Object::isField(getval))
                            {
                                field = Object::as<Field>(getval);
                                if(field->getter == nullptr)
                                {
                                    vm_rterrorvarg("Class %s does not have a getter for the field %s", klassobj->name->data(),
                                                       name->data());
                                }
                                vm_drop(fiber);
                                vm_writeframe(frame, ip);
                                vm_callvalue(name->data(), Object::as<Field>(getval)->getter->asValue(), 0);
                                vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                                continue;
                            }
                            else if(Object::isNativeMethod(getval) || Object::isPrimitiveMethod(getval))
                            {
                                getval = BoundMethod::make(this, vobj, getval)->asValue();
                            }
                        }
                        else
                        {
                            getval = Object::NullVal;
                        }
                    }
                    vm_drop(fiber);// Pop field name
                    fiber->stack_top[-1] = getval;
                    continue;
                }
                op_case(SET_FIELD)
                {
                    instval = vm_peek(fiber, 2);
                    if(Object::isNull(instval))
                    {
                        vm_rterror("Attempt to index a null value")
                    }
                    value = vm_peek(fiber, 1);
                    field_name = Object::as<String>(vm_peek(fiber, 0));
                    if(Object::isClass(instval))
                    {
                        klassobj = Object::as<Class>(instval);
                        if(klassobj->static_fields.get(field_name, &setter) && Object::isField(setter))
                        {
                            field = Object::as<Field>(setter);
                            if(field->setter == nullptr)
                            {
                                vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->data(),
                                                   field_name->data());
                            }

                            vm_dropn(fiber, 2);
                            vm_push(fiber, value);
                            vm_writeframe(frame, ip);
                            vm_callvalue(field_name->data(), field->setter->asValue(), 1);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                        if(Object::isNull(value))
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
                    else if(Object::isInstance(instval))
                    {
                        instobj = Object::as<Instance>(instval);
                        if(instobj->klass->methods.get(field_name, &setter) && Object::isField(setter))
                        {
                            field = Object::as<Field>(setter);
                            if(field->setter == nullptr)
                            {
                                vm_rterrorvarg("Class %s does not have a setter for the field %s", instobj->klass->name->data(),
                                                   field_name->data());
                            }
                            vm_dropn(fiber, 2);
                            vm_push(fiber, value);
                            vm_writeframe(frame, ip);
                            vm_callvalue(field_name->data(), field->setter->asValue(), 1);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                        if(Object::isNull(value))
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
                        klassobj = Class::getClassFor(this, instval);
                        if(klassobj == nullptr)
                        {
                            vm_rterror("SET_FIELD: only instances and classes have fields");
                        }
                        if(klassobj->methods.get(field_name, &setter) && Object::isField(setter))
                        {
                            field = Object::as<Field>(setter);
                            if(field->setter == nullptr)
                            {
                                vm_rterrorvarg("Class %s does not have a setter for the field %s", klassobj->name->data(),
                                                   field_name->data());
                            }
                            vm_dropn(fiber, 2);
                            vm_push(fiber, value);
                            vm_writeframe(frame, ip);
                            vm_callvalue(field_name->data(), field->setter->asValue(), 1);
                            vm_readframe(fiber, frame, current_chunk, ip, slots, privates, upvalues);
                            continue;
                        }
                        else
                        {
                            vm_rterrorvarg("Class %s does not contain field %s", klassobj->name->data(), field_name->data());
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
                    values = &Object::as<Array>(vm_peek(fiber, 1))->m_actualarray;
                    arindex = values->m_count;
                    values->reserve(arindex + 1, Object::NullVal);
                    values->m_values[arindex] = vm_peek(fiber, 0);
                    vm_drop(fiber);
                    continue;
                }
                op_case(PUSH_OBJECT_FIELD)
                {
                    operand = vm_peek(fiber, 2);
                    if(Object::isMap(operand))
                    {
                        Object::as<Map>(operand)->m_values.set(Object::as<String>(vm_peek(fiber, 1)), vm_peek(fiber, 0));
                    }
                    else if(Object::isInstance(operand))
                    {
                        Object::as<Instance>(operand)->fields.set(Object::as<String>(vm_peek(fiber, 1)), vm_peek(fiber, 0));
                    }
                    else
                    {
                        vm_rterrorvarg("Expected an object or a map as the operand, got %s", Object::valueName(operand));
                    }
                    vm_dropn(fiber, 2);
                    continue;
                }
                op_case(STATIC_FIELD)
                {
                    Object::as<Class>(vm_peek(fiber, 1))->static_fields.set(vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
                    vm_drop(fiber);
                    continue;
                }
                op_case(METHOD)
                {
                    klassobj = Object::as<Class>(vm_peek(fiber, 1));
                    name = vm_readstringlong(current_chunk, ip);
                    if((klassobj->init_method == nullptr || (klassobj->super != nullptr && klassobj->init_method == ((Class*)klassobj->super)->init_method))
                       && name->length() == 11 && memcmp(name->data(), LIT_NAME_CONSTRUCTOR, sizeof(LIT_NAME_CONSTRUCTOR)-1) == 0)
                    {
                        klassobj->init_method = Object::asObject(vm_peek(fiber, 0));
                    }
                    klassobj->methods.set(name, vm_peek(fiber, 0));
                    vm_drop(fiber);
                    continue;
                }
                op_case(DEFINE_FIELD)
                {
                    Object::as<Class>(vm_peek(fiber, 1))->methods.set(vm_readstringlong(current_chunk, ip), vm_peek(fiber, 0));
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
                    klassobj = Object::as<Class>(vm_pop(fiber));
                    vm_writeframe(frame, ip);
                    vm_invoke_from_class(klassobj, method_name, arg_count, true, methods, false);
                    continue;
                }
                op_case(INVOKE_SUPER_IGNORING)
                {
                    arg_count = vm_readbyte(ip);
                    method_name = vm_readstringlong(current_chunk, ip);
                    klassobj = Object::as<Class>(vm_pop(fiber));
                    vm_writeframe(frame, ip);
                    vm_invoke_from_class(klassobj, method_name, arg_count, true, methods, true);
                    continue;
                }
                op_case(GET_SUPER_METHOD)
                {
                    method_name = vm_readstringlong(current_chunk, ip);
                    klassobj = Object::as<Class>(vm_pop(fiber));
                    instval = vm_pop(fiber);
                    if(klassobj->methods.get(method_name, &value))
                    {
                        value = BoundMethod::make(this, instval, value)->asValue();
                    }
                    else
                    {
                        value = Object::NullVal;
                    }
                    vm_push(fiber, value);
                    continue;
                }
                op_case(INHERIT)
                {
                    super = vm_peek(fiber, 1);
                    if(!Object::isClass(super))
                    {
                        vm_rterror("Superclass must be a class");
                    }
                    klassobj = Object::as<Class>(vm_peek(fiber, 0));
                    super_klass = Object::as<Class>(super);
                    klassobj->super = super_klass;
                    klassobj->init_method = super_klass->init_method;
                    super_klass->methods.addAll(&klassobj->methods);
                    klassobj->super->static_fields.addAll(&klassobj->static_fields);
                    continue;
                }
                op_case(IS)
                {
                    instval = vm_peek(fiber, 1);
                    if(Object::isNull(instval))
                    {
                        vm_dropn(fiber, 2);
                        vm_push(fiber, Object::FalseVal);

                        continue;
                    }
                    instance_klass = Class::getClassFor(this, instval);
                    klassval = vm_peek(fiber, 0);
                    if(instance_klass == nullptr || !Object::isClass(klassval))
                    {
                        vm_rterror("operands must be an instance and a class");
                    }            
                    type = Object::as<Class>(klassval);
                    found = false;
                    while(instance_klass != nullptr)
                    {
                        if(instance_klass == type)
                        {
                            found = true;
                            break;
                        }
                        instance_klass = (Class*)instance_klass->super;
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
                    if(!Object::isArray(slot))
                    {
                        continue;
                    }
                    values = &Object::as<Array>(slot)->m_actualarray;
                    fiber->ensure_stack(values->m_count + frame->function->max_slots + (int)(fiber->stack_top - fiber->stack));
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
                    if(vm->globals->m_values.getSlot(name, &pval))
                    {
                        vm_push(fiber, Reference::make(this, pval)->asValue());
                    }
                    else
                    {
                        vm_rterror("Attempt to reference a null value");
                    }
                    continue;
                }
                op_case(REFERENCE_PRIVATE)
                {
                    vm_push(fiber, Reference::make(this, &privates[vm_readshort(ip)])->asValue());
                    continue;
                }
                op_case(REFERENCE_LOCAL)
                {
                    vm_push(fiber, Reference::make(this, &slots[vm_readshort(ip)])->asValue());
                    continue;
                }
                op_case(REFERENCE_UPVALUE)
                {
                    vm_push(fiber, Reference::make(this, upvalues[vm_readbyte(ip)]->location)->asValue());
                    continue;
                }
                op_case(REFERENCE_FIELD)
                {
                    vobj = vm_peek(fiber, 1);
                    if(Object::isNull(vobj))
                    {
                        vm_rterror("Attempt to index a null value");
                    }
                    name = Object::as<String>(vm_peek(fiber, 0));
                    if(Object::isInstance(vobj))
                    {
                        if(!Object::as<Instance>(vobj)->fields.getSlot(name, &pval))
                        {
                            vm_rterror("Attempt to reference a null value");
                        }
                    }
                    else
                    {
                        Object::print(this, &this->debugwriter, vobj);
                        printf("\n");
                        vm_rterror("You can only reference fields of real instances");
                    }
                    vm_drop(fiber);// Pop field name
                    fiber->stack_top[-1] = Reference::make(this, pval)->asValue();
                    continue;
                }
                op_case(SET_REFERENCE)
                {
                    reference = vm_pop(fiber);
                    if(!Object::isReference(reference))
                    {
                        vm_rterror("Provided value is not a reference");
                    }
                    *Object::as<Reference>(reference)->slot = vm_peek(fiber, 0);
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

    // impl::vm
    void VM::markRoots()
    {
        size_t i;
        State* state;
        state = this->m_state;
        for(i = 0; i < state->root_count; i++)
        {
            this->markValue(state->roots[i]);
        }
        this->markObject((Object*)this->fiber);
        this->markObject((Object*)state->classvalue_class);
        this->markObject((Object*)state->objectvalue_class);
        this->markObject((Object*)state->numbervalue_class);
        this->markObject((Object*)state->stringvalue_class);
        this->markObject((Object*)state->boolvalue_class);
        this->markObject((Object*)state->functionvalue_class);
        this->markObject((Object*)state->fibervalue_class);
        this->markObject((Object*)state->modulevalue_class);
        this->markObject((Object*)state->arrayvalue_class);
        this->markObject((Object*)state->mapvalue_class);
        this->markObject((Object*)state->rangevalue_class);
        this->markObject((Object*)state->api_name);
        this->markObject((Object*)state->api_function);
        this->markObject((Object*)state->api_fiber);
        state->preprocessor->defined.markForGC(this);
        this->modules->m_values.markForGC(this);
        this->globals->m_values.markForGC(this);
    }

    bool VM::callValue(std::string_view name, Value callee, uint8_t arg_count)
    {
        size_t i;
        bool bres;
        NativeMethod* mthobj;
        Fiber* fiber;
        Closure* closure;
        BoundMethod* bound_method;
        Value mthval;
        Value result;
        Instance* instance;
        Class* klass;
        (void)fiber;
        if(Object::isObject(callee))
        {
            if(this->m_state->set_native_exit_jump())
            {
                return true;
            }
            switch(OBJECT_TYPE(callee))
            {
                case Object::Type::Function:
                    {
                        return this->dispatchCall(Object::as<Function>(callee), nullptr, arg_count);
                    }
                    break;
                case Object::Type::Closure:
                    {
                        closure = Object::as<Closure>(callee);
                        return this->dispatchCall(closure->function, closure, arg_count);
                    }
                    break;
                case Object::Type::NativeFunction:
                    {
                        vm_pushgc(this->m_state, false)
                        result = Object::as<NativeFunction>(callee)->function(this, arg_count, this->fiber->stack_top - arg_count);
                        this->fiber->stack_top -= arg_count + 1;
                        this->push(result);
                        vm_popgc(this->m_state);
                        return false;
                    }
                    break;
                case Object::Type::NativePrimitive:
                    {
                        vm_pushgc(this->m_state, false)
                        fiber = this->fiber;
                        bres = Object::as<NativePrimFunction>(callee)->function(this, arg_count, fiber->stack_top - arg_count);
                        if(bres)
                        {
                            fiber->stack_top -= arg_count;
                        }
                        vm_popgc(this->m_state);
                        return bres;
                    }
                    break;
                case Object::Type::NativeMethod:
                    {
                        vm_pushgc(this->m_state, false);
                        mthobj = Object::as<NativeMethod>(callee);
                        fiber = this->fiber;
                        result = mthobj->method(this, *(this->fiber->stack_top - arg_count - 1), arg_count, this->fiber->stack_top - arg_count);
                        this->fiber->stack_top -= arg_count + 1;
                        //if(!Object::isNull(result))
                        {
                            if(!this->fiber->abort)
                            {
                                this->push(result);
                            }
                        }
                        vm_popgc(this->m_state);
                        return false;
                    }
                    break;
                case Object::Type::PrimitiveMethod:
                    {
                        vm_pushgc(this->m_state, false);
                        fiber = this->fiber;
                        bres = Object::as<PrimitiveMethod>(callee)->method(this, *(fiber->stack_top - arg_count - 1), arg_count, fiber->stack_top - arg_count);
                        if(bres)
                        {
                            fiber->stack_top -= arg_count;
                        }
                        vm_popgc(this->m_state);
                        return bres;
                    }
                    break;
                case Object::Type::Class:
                    {
                        klass = Object::as<Class>(callee);
                        instance = Instance::make(this->m_state, klass);
                        this->fiber->stack_top[-arg_count - 1] = instance->asValue();
                        if(klass->init_method != nullptr)
                        {
                            return this->callValue(LIT_NAME_CONSTRUCTOR, klass->init_method->asValue(), arg_count);
                        }
                        // Remove the arguments, so that they don't mess up the stack
                        // (default constructor has no arguments)
                        for(i = 0; i < arg_count; i++)
                        {
                            this->pop();
                        }
                        return false;
                    }
                    break;
                case Object::Type::BoundMethod:
                    {
                        bound_method = Object::as<BoundMethod>(callee);
                        mthval = bound_method->method;
                        if(Object::isNativeMethod(mthval))
                        {
                            vm_pushgc(this->m_state, false);
                            result = Object::as<NativeMethod>(mthval)->method(this, bound_method->receiver, arg_count, this->fiber->stack_top - arg_count);
                            this->fiber->stack_top -= arg_count + 1;
                            this->push(result);
                            vm_popgc(this->m_state);
                            return false;
                        }
                        else if(Object::isPrimitiveMethod(mthval))
                        {
                            fiber = this->fiber;
                            vm_pushgc(this->m_state, false);
                            if(Object::as<PrimitiveMethod>(mthval)->method(this, bound_method->receiver, arg_count, fiber->stack_top - arg_count))
                            {
                                fiber->stack_top -= arg_count;
                                return true;
                            }
                            vm_popgc(this->m_state);
                            return false;
                        }
                        else
                        {
                            this->fiber->stack_top[-arg_count - 1] = bound_method->receiver;
                            return this->dispatchCall(Object::as<Function>(mthval), nullptr, arg_count);
                        }
                    }
                    break;
                default:
                    {
                    }
                    break;

            }
        }
        if(Object::isNull(callee))
        {
            lit_runtime_error(this, "Attempt to call a null value '%s'", name.data());
        }
        else
        {
            lit_runtime_error(this, "Can only call functions and classes, got %s", Object::valueName(callee));
        }
        return true;
    }

    // impl::writer
    void Writer::initString(State* state)
    {
        initDefault(state, this);
        this->stringmode = true;
        this->uptr = String::allocEmpty(state, 0);
    }

    void Writer::stringAppend(String* ds, int byte)
    {
        ds->append(byte);
    }

    void Writer::stringAppend(String* ds, const char* str, size_t len)
    {
        ds->append(str, len);
    }

    // todo: this would have been done via sds - so needs to be reimplemented
    void Writer::stringAppendFormat(String* ds, const char* fmt, va_list va)
    {
        size_t actual;
        size_t needed;
        char* buffer;
        needed = vsnprintf(nullptr, 0, fmt, va);
        buffer = LIT_ALLOCATE(ds->m_state, char, needed+1);
        actual = vsnprintf(buffer, needed, fmt, va);
        ds->append(buffer, actual);
        LIT_FREE(ds->m_state, char, buffer);
    }
    
    // impl::object
    Object* Object::make(State* state, size_t size, Object::Type type)
    {
        Object* obj;
        obj = (Object*)Memory::reallocate(state, nullptr, 0, size);
        obj->m_state = state;
        obj->type = type;
        obj->marked = false;
        obj->next = state->vm->objects;
        state->vm->objects = obj;
    #ifdef LIT_LOG_ALLOCATION
        printf("%p allocate %ld for %s\n", (void*)obj, size, Object::valueName(type));
    #endif
        return obj;
    }

    void Object::releaseObject(State* state, Object* obj)
    {
        String* string;
        Function* function;
        Fiber* fiber;
        Module* module;
        Closure* closure;
    #ifdef LIT_LOG_ALLOCATION
        printf("(");
        Object::print(obj->asValue());
        printf(") %p free %s\n", (void*)obj, Object::valueName(obj->type));
    #endif

        switch(obj->type)
        {
            case Object::Type::String:
                {
                    string = (String*)obj;
                    //LIT_FREE_ARRAY(state, char, string->m_chars, string->length + 1);
                    delete string->m_chars;
                    LIT_FREE(state, String, obj);
                }
                break;

            case Object::Type::Function:
                {
                    function = (Function*)obj;
                    function->chunk.release();
                    LIT_FREE(state, Function, obj);
                }
                break;
            case Object::Type::NativeFunction:
                {
                    LIT_FREE(state, NativeFunction, obj);
                }
                break;
            case Object::Type::NativePrimitive:
                {
                    LIT_FREE(state, NativePrimFunction, obj);
                }
                break;
            case Object::Type::NativeMethod:
                {
                    LIT_FREE(state, NativeMethod, obj);
                }
                break;
            case Object::Type::PrimitiveMethod:
                {
                    LIT_FREE(state, PrimitiveMethod, obj);
                }
                break;
            case Object::Type::Fiber:
                {
                    fiber = (Fiber*)obj;
                    LIT_FREE_ARRAY(state, Fiber::CallFrame, fiber->frames, fiber->frame_capacity);
                    LIT_FREE_ARRAY(state, Value, fiber->stack, fiber->stack_capacity);
                    LIT_FREE(state, Fiber, obj);
                }
                break;
            case Object::Type::Module:
                {
                    module = (Module*)obj;
                    LIT_FREE_ARRAY(state, Value, module->privates, module->private_count);
                    LIT_FREE(state, Module, obj);
                }
                break;
            case Object::Type::Closure:
                {
                    closure = (Closure*)obj;
                    LIT_FREE_ARRAY(state, Upvalue*, closure->upvalues, closure->upvalue_count);
                    LIT_FREE(state, Closure, obj);
                }
                break;
            case Object::Type::Upvalue:
                {
                    LIT_FREE(state, Upvalue, obj);
                }
                break;
            case Object::Type::Class:
                {
                    Class* klass = (Class*)obj;
                    klass->methods.release();
                    klass->static_fields.release();
                    LIT_FREE(state, Class, obj);
                }
                break;

            case Object::Type::Instance:
                {
                    ((Instance*)obj)->fields.release();
                    LIT_FREE(state, Instance, obj);
                }
                break;
            case Object::Type::BoundMethod:
                {
                    LIT_FREE(state, BoundMethod, obj);
                }
                break;
            case Object::Type::Array:
                {
                    ((Array*)obj)->m_actualarray.release();
                    LIT_FREE(state, Array, obj);
                }
                break;
            case Object::Type::Map:
                {
                    ((Map*)obj)->m_values.release();
                    LIT_FREE(state, Map, obj);
                }
                break;
            case Object::Type::Userdata:
                {
                    Userdata* data = (Userdata*)obj;
                    if(data->cleanup_fn != nullptr)
                    {
                        data->cleanup_fn(state, data, false);
                    }
                    if(data->size > 0)
                    {
                        if(data->canfree)
                        {
                            Memory::reallocate(state, data->data, data->size, 0);
                        }
                    }
                    LIT_FREE(state, Userdata, data);
                    //free(data);
                }
                break;
            case Object::Type::Range:
                {
                    LIT_FREE(state, Range, obj);
                }
                break;
            case Object::Type::Field:
                {
                    LIT_FREE(state, Field, obj);
                }
                break;
            case Object::Type::Reference:
                {
                    LIT_FREE(state, Reference, obj);
                }
                break;
            default:
                {
                    /* UNREACHABLE */
                }
                break;
        }
    }

    inline State* Object::asState(VM* vm)
    {
        return vm->m_state;
    }

    String* Object::functionName(VM* vm, Value instance)
    {
        String* name;
        Field* field;
        name = nullptr;
        switch(OBJECT_TYPE(instance))
        {
            case Object::Type::Function:
                {
                    name = Object::as<Function>(instance)->name;
                }
                break;
            case Object::Type::Closure:
                {
                    name = Object::as<Closure>(instance)->function->name;
                }
                break;
            case Object::Type::Field:
                {
                    field = Object::as<Field>(instance);
                    if(field->getter != nullptr)
                    {
                        return functionName(vm, field->getter->asValue());
                    }
                    return functionName(vm, field->setter->asValue());
                }
                break;
            case Object::Type::NativePrimitive:
                {
                    name = Object::as<NativePrimFunction>(instance)->name;
                }
                break;
            case Object::Type::NativeFunction:
                {
                    name = Object::as<NativeFunction>(instance)->name;
                }
                break;
            case Object::Type::NativeMethod:
                {
                    name = Object::as<NativeMethod>(instance)->name;
                }
                break;
            case Object::Type::PrimitiveMethod:
                {
                    name = Object::as<PrimitiveMethod>(instance)->name;
                }
                break;
            case Object::Type::BoundMethod:
                {
                    return functionName(vm, Object::as<BoundMethod>(instance)->method);
                }
                break;
            default:
                {
                }
                break;
        }
        if(name == nullptr)
        {
            return String::format(vm->m_state, "<function #>", *((double*)Object::asObject(instance)));
        }
        return String::format(vm->m_state, "<function @>", name->asValue());
    }

    String* Object::toString(State* state, Value valobj)
    {
        Value* slot;
        VM* vm;
        Fiber* fiber;
        Function* function;
        Chunk* chunk;
        Fiber::CallFrame* frame;
        Result result;
        if(Object::isString(valobj))
        {
            return Object::as<String>(valobj);
        }
        else if(!Object::isObject(valobj))
        {
            if(Object::isNull(valobj))
            {
                return String::intern(state, "null");
            }
            else if(Object::isNumber(valobj))
            {
                return Object::as<String>(String::stringNumberToString(state, Object::toNumber(valobj)));
            }
            else if(Object::isBool(valobj))
            {
                return String::intern(state, Object::asBool(valobj) ? "true" : "false");
            }
        }
        else if(Object::isReference(valobj))
        {
            slot = Object::as<Reference>(valobj)->slot;

            if(slot == nullptr)
            {
                return String::intern(state, "null");
            }
            return Object::toString(state, *slot);
        }
        vm = state->vm;
        fiber = vm->fiber;
        if(Fiber::ensureFiber(vm, fiber))
        {
            return String::intern(state, "null");
        }
        function = state->api_function;
        if(function == nullptr)
        {
            function = state->api_function = Function::make(state, fiber->module);
            function->chunk.has_line_info = false;
            function->name = state->api_name;
            chunk = &function->chunk;
            chunk->m_count = 0;
            chunk->constants.m_count = 0;
            function->max_slots = 3;
            chunk->write_chunk(OP_INVOKE, 1);
            chunk->emit_byte(0);
            chunk->emit_short(chunk->add_constant(String::internValue(state, "toString")));
            chunk->emit_byte(OP_RETURN);
        }
        fiber->ensure_stack(function->max_slots + (int)(fiber->stack_top - fiber->stack));
        frame = &fiber->frames[fiber->frame_count++];
        frame->ip = function->chunk.m_code;
        frame->closure = nullptr;
        frame->function = function;
        frame->slots = fiber->stack_top;
        frame->result_ignored = false;
        frame->return_to_c = true;
        PUSH(function->asValue());
        PUSH(valobj);
        result = state->execFiber(fiber);
        if(result.type != LITRESULT_OK)
        {
            return String::intern(state, "null");
        }
        return Object::as<String>(result.result);
    }

    void Object::printArray(State* state, Writer* wr, Array* array, size_t size)
    {
        size_t i;
        wr->format("(%u) [", (unsigned int)size);
        if(size > 0)
        {
            wr->put(" ");
            for(i = 0; i < size; i++)
            {
                if(Object::isArray(array->at(i)) && (array == Object::as<Array>(array->at(i))))
                {
                    wr->put("(recursion)");
                }
                else
                {
                    Object::print(state, wr, array->at(i));
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

    void Object::printMap(State* state, Writer* wr, Map* map, size_t size)
    {
        bool had_before;
        size_t i;
        Table::Entry* entry;
        wr->format("(%u) {", (unsigned int)size);
        had_before = false;
        if(size > 0)
        {
            for(i = 0; i < (size_t)map->size(); i++)
            {
                entry = &map->m_values.m_inner.m_values[i];
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
                    wr->format("%s = ", entry->key->data());
                    if(Object::isMap(entry->value) && (map == Object::as<Map>(entry->value)))
                    {
                        wr->put("(recursion)");
                    }
                    else
                    {
                        Object::print(state, wr, entry->value);
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

    void Object::printObject(State* state, Writer* wr, Value value)
    {
        size_t size;
        Map* map;
        Array* array;
        Range* range;
        Value* slot;
        Object* obj;
        Upvalue* upvalue;
        obj = Object::asObject(value);
        if(obj != nullptr)
        {
            switch(obj->type)
            {
                case Object::Type::String:
                    {
                        wr->format("%s", Object::asString(value)->data());
                    }
                    break;
                case Object::Type::Function:
                    {
                        wr->format("function %s", Object::as<Function>(value)->name->data());
                    }
                    break;
                case Object::Type::Closure:
                    {
                        wr->format("closure %s", Object::as<Closure>(value)->function->name->data());
                    }
                    break;
                case Object::Type::NativePrimitive:
                    {
                        wr->format("function %s", Object::as<NativePrimFunction>(value)->name->data());
                    }
                    break;
                case Object::Type::NativeFunction:
                    {
                        wr->format("function %s", Object::as<NativeFunction>(value)->name->data());
                    }
                    break;
                case Object::Type::PrimitiveMethod:
                    {
                        wr->format("function %s", Object::as<PrimitiveMethod>(value)->name->data());
                    }
                    break;
                case Object::Type::NativeMethod:
                    {
                        wr->format("function %s", Object::as<NativeMethod>(value)->name->data());
                    }
                    break;
                case Object::Type::Fiber:
                    {
                        wr->format("fiber");
                    }
                    break;
                case Object::Type::Module:
                    {
                        wr->format("module %s", Object::as<Module>(value)->name->data());
                    }
                    break;

                case Object::Type::Upvalue:
                    {
                        upvalue = Object::as<Upvalue>(value);
                        if(upvalue->location == nullptr)
                        {
                            Object::print(state, wr, upvalue->closed);
                        }
                        else
                        {
                            printObject(state, wr, *upvalue->location);
                        }
                    }
                    break;
                case Object::Type::Class:
                    {
                        wr->format("class %s", Object::as<Class>(value)->name->data());
                    }
                    break;
                case Object::Type::Instance:
                    {
                        /*
                        if(Object::as<Instance>(value)->klass->type == Object::Type::Map)
                        {
                            fprintf(stderr, "instance is a map\n");
                        }
                        printf("%s instance", Object::as<Instance>(value)->klass->name->data());
                        */
                        wr->format("<instance '%s' ", Object::as<Instance>(value)->klass->name->data());
                        map = Object::as<Map>(value);
                        size = map->size();
                        printMap(state, wr, map, size);
                        wr->put(">");
                    }
                    break;
                case Object::Type::BoundMethod:
                    {
                        Object::print(state, wr, Object::as<BoundMethod>(value)->method);
                        return;
                    }
                    break;
                case Object::Type::Array:
                    {
                        #ifdef LIT_MINIMIZE_CONTAINERS
                            wr->put("array");
                        #else
                            array = Object::as<Array>(value);
                            size = array->size();
                            printArray(state, wr, array, size);
                        #endif
                    }
                    break;
                case Object::Type::Map:
                    {
                        #ifdef LIT_MINIMIZE_CONTAINERS
                            wr->format("map");
                        #else
                            map = Object::as<Map>(value);
                            size = map->size();
                            printMap(state, wr, map, size);
                        #endif
                    }
                    break;
                case Object::Type::Userdata:
                    {
                        wr->format("userdata");
                    }
                    break;
                case Object::Type::Range:
                    {
                        range = Object::as<Range>(value);
                        wr->format("%g .. %g", range->from, range->to);
                    }
                    break;
                case Object::Type::Field:
                    {
                        wr->format("field");
                    }
                    break;
                case Object::Type::Reference:
                    {
                        wr->format("reference => ");
                        slot = Object::as<Reference>(value)->slot;
                        if(slot == nullptr)
                        {
                            wr->put("null");
                        }
                        else
                        {
                            Object::print(state, wr, *slot);
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

    // impl::table 
    void Table::markForGC(VM* vm)
    {
        size_t i;
        Table::Entry* entry;
        for(i = 0; i <= this->m_inner.m_capacity; i++)
        {
            entry = &this->m_inner.m_values[i];
            if(entry != nullptr)
            {
                vm->markObject((Object*)entry->key);
                vm->markValue(entry->value);
            }
        }
    }

    Module* Module::getModule(State* state, String* name)
    {
        Value value;
        if(state->vm->modules->m_values.get(name, &value))
        {
            return Object::as<Module>(value);
        }
        return nullptr;
    }

    // impl::string
    bool String::stateGetGCAllowed(State* state)
    {
        return state->allow_gc;
    }

    void String::stateSetGCAllowed(State* state, bool v)
    {
        state->allow_gc = v;
    }

    String* String::stateFindInterned(State* state, const char* str, size_t length, uint32_t hs)
    {
        return state->vm->strings.find(str, length, hs);
    }

    void String::statePutInterned(State* state, String* string)
    {
        if(string->length() > 0)
        {
            state->pushRoot((Object*)string);
            state->vm->strings.set(string, Object::NullVal);
            state->popRoot();
        }
    }

    Closure* Closure::make(State* state, Function* function)
    {
        size_t i;
        Closure* closure;
        Upvalue** upvalues;
        closure = Object::make<Closure>(state, Object::Type::Closure);
        state->pushRoot((Object*)closure);
        upvalues = LIT_ALLOCATE(state, Upvalue*, function->upvalue_count);
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

    // impl::class
    Class* Class::getClassFor(State* state, Value value)
    {
        Value* slot;
        Upvalue* upvalue;
        if(Object::isObject(value))
        {
            switch(OBJECT_TYPE(value))
            {
                case Object::Type::String:
                    {
                        return state->stringvalue_class;
                    }
                    break;
                case Object::Type::Userdata:
                    {
                        return state->objectvalue_class;
                    }
                    break;
                case Object::Type::Field:
                case Object::Type::Function:
                case Object::Type::Closure:
                case Object::Type::NativeFunction:
                case Object::Type::NativePrimitive:
                case Object::Type::BoundMethod:
                case Object::Type::PrimitiveMethod:
                case Object::Type::NativeMethod:
                    {
                        return state->functionvalue_class;
                    }
                    break;
                case Object::Type::Fiber:
                    {
                        //fprintf(stderr, "should return fiber class ....\n");
                        return state->fibervalue_class;
                    }
                    break;
                case Object::Type::Module:
                    {
                        return state->modulevalue_class;
                    }
                    break;
                case Object::Type::Upvalue:
                    {
                        upvalue = Object::as<Upvalue>(value);
                        if(upvalue->location == nullptr)
                        {
                            return Class::getClassFor(state, upvalue->closed);
                        }
                        return Class::getClassFor(state, *upvalue->location);
                    }
                    break;
                case Object::Type::Instance:
                    {
                        return Object::as<Instance>(value)->klass;
                    }
                    break;
                case Object::Type::Class:
                    {
                        return state->classvalue_class;
                    }
                    break;
                case Object::Type::Array:
                    {
                        return state->arrayvalue_class;
                    }
                    break;
                case Object::Type::Map:
                    {
                        return state->mapvalue_class;
                    }
                    break;
                case Object::Type::Range:
                    {
                        return state->rangevalue_class;
                    }
                    break;
                case Object::Type::Reference:
                    {
                        slot = Object::as<Reference>(value)->slot;
                        if(slot != nullptr)
                        {
                            return Class::getClassFor(state, *slot);
                        }

                        return state->objectvalue_class;
                    }
                    break;
            }
        }
        else if(Object::isNumber(value))
        {
            return state->numbervalue_class;
        }
        else if(Object::isBool(value))
        {
            return state->boolvalue_class;
        }
        //fprintf(stderr, "failed to find class object!\n");
        return nullptr;
    }

    // impl::instance
    Result Instance::callMethod(State* state, Value callee, String* mthname, Value* argv, size_t argc)
    {
        Value mthval;
        mthval = Instance::getMethod(state, callee, mthname);
        if(!Object::isNull(mthval))
        {
            return state->call(mthval, argv, argc);
        }
        return INTERPRET_RUNTIME_FAIL;    
    }


    /**
    * impls
    */







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

    // otherfuncs
    double lit_check_number(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isNumber(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected a number as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }
        return Object::toNumber(args[id]);
    }

    double lit_get_number(VM* vm, Value* args, uint8_t arg_count, uint8_t id, double def)
    {
        (void)vm;
        if(arg_count <= id || !Object::isNumber(args[id]))
        {
            return def;
        }
        return Object::toNumber(args[id]);
    }

    bool lit_check_bool(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isBool(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected a boolean as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }

        return Object::asBool(args[id]);
    }

    bool lit_get_bool(VM* vm, Value* args, uint8_t arg_count, uint8_t id, bool def)
    {
        (void)vm;
        if(arg_count <= id || !Object::isBool(args[id]))
        {
            return def;
        }
        return Object::asBool(args[id]);
    }

    const char* lit_check_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isString(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }

        return Object::as<String>(args[id])->data();
    }

    const char* lit_get_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id, const char* def)
    {
        (void)vm;
        if(arg_count <= id || !Object::isString(args[id]))
        {
            return def;
        }

        return Object::as<String>(args[id])->data();
    }

    String* lit_check_object_string(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isString(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected a string as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }

        return Object::as<String>(args[id]);
    }

    Instance* lit_check_instance(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isInstance(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected an instance as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }

        return Object::as<Instance>(args[id]);
    }

    Value* lit_check_reference(VM* vm, Value* args, uint8_t arg_count, uint8_t id)
    {
        if(arg_count <= id || !Object::isReference(args[id]))
        {
            lit_runtime_error_exiting(vm, "expected a reference as argument #%i, got a %s", (int)id,
                                      id >= arg_count ? "null" : Object::valueName(args[id]));
        }

        return Object::as<Reference>(args[id])->slot;
    }

    void lit_ensure_bool(VM* vm, Value value, const char* error)
    {
        if(!Object::isBool(value))
        {
            lit_runtime_error_exiting(vm, error);
        }
    }

    void lit_ensure_string(VM* vm, Value value, const char* error)
    {
        if(!Object::isString(value))
        {
            lit_runtime_error_exiting(vm, error);
        }
    }

    void lit_ensure_number(VM* vm, Value value, const char* error)
    {
        if(!Object::isNumber(value))
        {
            lit_runtime_error_exiting(vm, error);
        }
    }

    void lit_ensure_object_type(VM* vm, Value value, Object::Type type, const char* error)
    {
        if(!Object::isObject(value) || OBJECT_TYPE(value) != type)
        {
            lit_runtime_error_exiting(vm, error);
        }
    }


    static size_t print_simple_op(State* state, Writer* wr, const char* name, size_t offset)
    {
        (void)state;
        wr->format("%s%s%s\n", COLOR_YELLOW, name, COLOR_RESET);
        return offset + 1;
    }

    static size_t print_constant_op(State* state, Writer* wr, const char* name, Chunk* chunk, size_t offset, bool big)
    {
        uint8_t constant;
        if(big)
        {
            constant = (uint16_t)(chunk->m_code[offset + 1] << 8);
            constant |= chunk->m_code[offset + 2];
        }
        else
        {
            constant = chunk->m_code[offset + 1];
        }
        wr->format("%s%-16s%s %4d '", COLOR_YELLOW, name, COLOR_RESET, constant);
        Object::print(state, wr, chunk->constants.m_values[constant]);
        wr->format("'\n");
        return offset + (big ? 3 : 2);
    }

    static size_t print_byte_op(State* state, Writer* wr, const char* name, Chunk* chunk, size_t offset)
    {
        uint8_t slot;
        (void)state;
        slot = chunk->m_code[offset + 1];
        wr->format("%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
        return offset + 2;
    }

    static size_t print_short_op(State* state, Writer* wr, const char* name, Chunk* chunk, size_t offset)
    {
        uint16_t slot;
        (void)state;
        slot = (uint16_t)(chunk->m_code[offset + 1] << 8);
        slot |= chunk->m_code[offset + 2];
        wr->format("%s%-16s%s %4d\n", COLOR_YELLOW, name, COLOR_RESET, slot);
        return offset + 2;
    }

    static size_t print_jump_op(State* state, Writer* wr, const char* name, int sign, Chunk* chunk, size_t offset)
    {
        uint16_t jump;
        (void)state;
        jump = (uint16_t)(chunk->m_code[offset + 1] << 8);
        jump |= chunk->m_code[offset + 2];
        wr->format("%s%-16s%s %4d -> %d\n", COLOR_YELLOW, name, COLOR_RESET, (int)offset, (int)(offset + 3 + sign * jump));
        return offset + 3;
    }

    static size_t print_invoke_op(State* state, Writer* wr, const char* name, Chunk* chunk, size_t offset)
    {
        uint8_t arg_count;
        uint8_t constant;
        (void)state;
        arg_count = chunk->m_code[offset + 1];
        constant = chunk->m_code[offset + 2];
        constant |= chunk->m_code[offset + 3];
        wr->format("%s%-16s%s (%d args) %4d '", COLOR_YELLOW, name, COLOR_RESET, arg_count, constant);
        Object::print(state, wr, chunk->constants.m_values[constant]);
        wr->format("'\n");
        return offset + 4;
    }

    size_t lit_disassemble_instruction(State* state, Chunk* chunk, size_t offset, const char* source)
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
        Writer* wr;
        Function* function;
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
        instruction = chunk->m_code[offset];
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
                    constant = (uint16_t)(chunk->m_code[offset] << 8);
                    offset++;
                    constant |= chunk->m_code[offset];
                    wr->format("%-16s %4d ", "OP_CLOSURE", constant);
                    Object::print(state, wr, chunk->constants.m_values[constant]);
                    wr->format("\n");
                    function = Object::as<Function>(chunk->constants.m_values[constant]);
                    for(j = 0; j < function->upvalue_count; j++)
                    {
                        is_local = chunk->m_code[offset++];
                        index = chunk->m_code[offset++];
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

    void lit_disassemble_chunk(Chunk* chunk, const char* name, const char* source)
    {
        size_t i;
        size_t offset;
        Value value;
        PCGenericArray<Value>* values;
        Function* function;
        values = &chunk->constants;

        for(i = 0; i < values->m_count; i++)
        {
            value = values->m_values[i];
            if(Object::isFunction(value))
            {
                function = Object::as<Function>(value);
                lit_disassemble_chunk(&function->chunk, function->name->data(), source);
            }
        }
        chunk->m_state->debugwriter.format("== %s ==\n", name);
        for(offset = 0; offset < chunk->m_count;)
        {
            offset = lit_disassemble_instruction(chunk->m_state, chunk, offset, source);
        }
    }

    void lit_disassemble_module(State* state, Module* module, const char* source)
    {
        (void)state;
        lit_disassemble_chunk(&module->main_function->chunk, module->main_function->name->data(), source);
    }

    void lit_trace_frame(Fiber* fiber, Writer* wr)
    {
        Fiber::CallFrame* frame;
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
               fiber->frame_count - 1, frame->function->name->data(), frame->function->arg_count, frame->function->max_slots,
               frame->function->max_slots + (int)(fiber->stack_top - fiber->stack), fiber->stack_capacity, frame->return_to_c);
    #endif
    }

    static const char* errorCodeToString(Error eid)
    {
        switch(eid)
        {
            case Error::LITERROR_UNCLOSED_MACRO:
                return "unclosed macro.";
            case Error::LITERROR_UNKNOWN_MACRO:
                return "unknown macro '%.*s'.";
            case Error::LITERROR_UNEXPECTED_CHAR:
                return "unexpected character '%c'";
            case Error::LITERROR_UNTERMINATED_STRING:
                return "unterminated string";
            case Error::LITERROR_INVALID_ESCAPE_CHAR:
                return "invalid escape character '%c'";
            case Error::LITERROR_INTERPOLATION_NESTING_TOO_DEEP:
                return "interpolation nesting is too deep, maximum is %i";
            case Error::LITERROR_NUMBER_IS_TOO_BIG:
                return "number is too big to be represented by a single literal";
            case Error::LITERROR_CHAR_EXPECTATION_UNMET:
                return "expected '%c' after '%c', got '%c'";
            case Error::LITERROR_EXPECTATION_UNMET:
                return "expected %s, got '%.*s'";
            case Error::LITERROR_INVALID_ASSIGMENT_TARGET:
                return "invalid assigment target";
            case Error::LITERROR_TOO_MANY_FUNCTION_ARGS:
                return "function cannot have more than 255 arguments, got %i";
            case Error::LITERROR_MULTIPLE_ELSE_BRANCHES:
                return "if-statement can have only one else-branch";
            case Error::LITERROR_VAR_MISSING_IN_FORIN:
                return "for-loops using in-iteration must declare a new variable";
            case Error::LITERROR_NO_GETTER_AND_SETTER:
                return "expected declaration of either getter or setter, got none";
            case Error::LITERROR_STATIC_OPERATOR:
                return "operator methods cannot be static or defined in static classes";
            case Error::LITERROR_SELF_INHERITED_CLASS:
                return "class cannot inherit itself";
            case Error::LITERROR_STATIC_FIELDS_AFTER_METHODS:
                return "all static fields must be defined before the methods";
            case Error::LITERROR_MISSING_STATEMENT:
                return "expected statement but got nothing";
            case Error::LITERROR_EXPECTED_EXPRESSION:
                return "expected expression after '%.*s', got '%.*s'";
            case Error::LITERROR_DEFAULT_ARG_CENTRED:
                return "default arguments must always be in the end of the argument list.";
            case Error::LITERROR_TOO_MANY_CONSTANTS:
                return "too many constants for one chunk";
            case Error::LITERROR_TOO_MANY_PRIVATES:
                return "too many private locals for one module";
            case Error::LITERROR_VAR_REDEFINED:
                return "variable '%.*s' was already declared in this scope";
            case Error::LITERROR_TOO_MANY_LOCALS:
                return "too many local variables for one function";
            case Error::LITERROR_TOO_MANY_UPVALUES:
                return "too many upvalues for one function";
            case Error::LITERROR_VARIABLE_USED_IN_INIT:
                return "variable '%.*s' cannot use itself in its initializer";
            case Error::LITERROR_JUMP_TOO_BIG:
                return "too much code to jump over";
            case Error::LITERROR_NO_SUPER:
                return "'super' cannot be used in class '%s', because it does not have a super class";
            case Error::LITERROR_THIS_MISSUSE:
                return "'this' cannot be used %s";
            case Error::LITERROR_SUPER_MISSUSE:
                return "'super' cannot be used %s";
            case Error::LITERROR_UNKNOWN_EXPRESSION:
                return "unknown expression with id '%i'";
            case Error::LITERROR_UNKNOWN_STATEMENT:
                return "unknown statement with id '%i'";
            case Error::LITERROR_LOOP_JUMP_MISSUSE:
                return "cannot use '%s' outside of loops";
            case Error::LITERROR_RETURN_FROM_CONSTRUCTOR:
                return "cannot use 'return' in constructors";
            case Error::LITERROR_STATIC_CONSTRUCTOR:
                return "constructors cannot be static (at least for now)";
            case Error::LITERROR_CONSTANT_MODIFIED:
                return "attempt to modify constant '%.*s'";
            case Error::LITERROR_INVALID_REFERENCE_TARGET:
                return "invalid refence target";
            default:
                break;
        }
        return "(nothing)";
    }

    String* lit_vformat_error(State* state, size_t line, Error error_id, va_list args)
    {
        size_t buffer_size;
        char* buffer;
        const char* error_message;
        String* rt;
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
            rt = String::format(state, "[err # line #]: $", (double)error_id, (double)line, (const char*)buffer);
        }
        else
        {
            rt = String::format(state, "[err #]: $", (double)error_id, (const char*)buffer);
        }
        free(buffer);
        return rt;
    }

    String* lit_format_error(State* state, size_t line, Error error, ...)
    {
        va_list args;
        String* result;
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

    bool lit_compile_and_save_files(State* state, char* files[], size_t num_files, const char* output_file)
    {
        size_t i;
        size_t length;
        char* file_name;
        char* source;
        FILE* file;
        String* module_name;
        Module* module;
        Module** compiled_modules;
        compiled_modules = LIT_ALLOCATE(state, Module*, num_files+1);
        AST::Optimizer::set_level(LITOPTLEVEL_EXTREME);
        for(i = 0; i < num_files; i++)
        {
            file_name = Util::copyString(files[i]);
            source = Util::readFile(file_name, &length);
            if(source == nullptr)
            {
                state->raiseError(COMPILE_ERROR, "failed to open file '%s' for reading", file_name);
                return false;
            }
            file_name = Util::patchFilename(file_name);
            module_name = String::copy(state, file_name, strlen(file_name));
            module = state->compileModule(module_name, source, length);
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
        FileIO::write_uint16_t(file, LIT_BYTECODE_MAGIC_NUMBER);
        FileIO::write_uint8_t(file, LIT_BYTECODE_VERSION);
        FileIO::write_uint16_t(file, num_files);
        for(i = 0; i < num_files; i++)
        {
            BinaryData::storeModule(compiled_modules[i], file);
        }
        FileIO::write_uint16_t(file, LIT_BYTECODE_END_NUMBER);
        LIT_FREE(state, Module, compiled_modules);
        fclose(file);
        return true;
    }


    Result lit_dump_file(State* state, const char* file)
    {
        size_t length;
        char* patched_file_name;
        char* source;
        Result result;
        String* module_name;
        Module* module;
        source = state->readSource(file, &length, &patched_file_name);
        if(source == nullptr)
        {
            return INTERPRET_RUNTIME_FAIL;
        }
        module_name = String::copy(state, patched_file_name, strlen(patched_file_name));
        module = state->compileModule(module_name, source, length);
        if(module == nullptr)
        {
            result = INTERPRET_RUNTIME_FAIL;
        }
        else
        {
            lit_disassemble_module(state, module, source);
            result = (Result){ LITRESULT_OK, Object::NullVal };
        }
        free((void*)source);
        free((void*)patched_file_name);
        return result;
    }



    void lit_printf(State* state, const char* message, ...)
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


    void lit_trace_vm_stack(VM* vm, Writer* wr)
    {
        Value* top;
        Value* slot;
        Fiber* fiber;
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
            Object::print(vm->m_state, wr, *slot);
            wr->format(" ]");
        }
        wr->format("%s", COLOR_RESET);
        for(slot = top; slot < fiber->stack_top; slot++)
        {
            wr->format("[ ");
            Object::print(vm->m_state, wr, *slot);
            wr->format(" ]");
        }
        wr->format("\n");
    }

    bool lit_handle_runtime_error(VM* vm, String* error_string)
    {
        int i;
        int count;
        size_t length;
        char* start;
        char* buffer;
        const char* name;
        Fiber::CallFrame* frame;
        Function* function;
        Chunk* chunk;
        Value error;
        Fiber* fiber;
        Fiber* caller;
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
        length = snprintf(nullptr, 0, "%s%s\n", COLOR_RED, error_string->data());
        for(i = count; i >= 0; i--)
        {
            frame = &fiber->frames[i];
            function = frame->function;
            chunk = &function->chunk;
            name = function->name == nullptr ? "unknown" : function->name->data();

            if(chunk->has_line_info)
            {
                length += snprintf(nullptr, 0, "[line %d] in %s()\n", (int)chunk->get_line(frame->ip - chunk->m_code - 1), name);
            }
            else
            {
                length += snprintf(nullptr, 0, "\tin %s()\n", name);
            }
        }
        length += snprintf(nullptr, 0, "%s", COLOR_RESET);
        buffer = (char*)malloc(length + 1);
        buffer[length] = '\0';
        start = buffer + sprintf(buffer, "%s%s\n", COLOR_RED, error_string->data());
        for(i = count; i >= 0; i--)
        {
            frame = &fiber->frames[i];
            function = frame->function;
            chunk = &function->chunk;
            name = function->name == nullptr ? "unknown" : function->name->data();
            if(chunk->has_line_info)
            {
                start += sprintf(start, "[line %d] in %s()\n", (int)chunk->get_line(frame->ip - chunk->m_code - 1), name);
            }
            else
            {
                start += sprintf(start, "\tin %s()\n", name);
            }
        }
        start += sprintf(start, "%s", COLOR_RESET);
        vm->m_state->raiseError(RUNTIME_ERROR, buffer);
        free(buffer);
        reset_stack(vm);
        return false;
    }

    bool lit_vruntime_error(VM* vm, const char* format, va_list args)
    {
        size_t buffer_size;
        char* buffer;
        va_list args_copy;
        va_copy(args_copy, args);
        buffer_size = vsnprintf(nullptr, 0, format, args_copy) + 1;
        va_end(args_copy);
        buffer = (char*)malloc(buffer_size+1);
        vsnprintf(buffer, buffer_size, format, args);
        return lit_handle_runtime_error(vm, String::take(vm->m_state, buffer, buffer_size));
    }

    bool lit_runtime_error(VM* vm, const char* format, ...)
    {
        bool result;
        va_list args;
        va_start(args, format);
        result = lit_vruntime_error(vm, format, args);
        va_end(args);
        return result;
    }

    bool lit_runtime_error_exiting(VM* vm, const char* format, ...)
    {
        bool result;
        va_list args;
        va_start(args, format);
        result = lit_vruntime_error(vm, format, args);
        va_end(args);
        vm->m_state->native_exit_jump();
        return result;
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



    #undef vm_pushgc
    #undef vm_popgc
    #undef LIT_TRACE_FRAME





    static Value objfn_array_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        size_t count;
        Array* made;
        Value cntval;
        Value fillval;
        (void)instance;
        (void)argc;
        (void)argv;
        made = Array::make(vm->m_state);
        if(argc > 0)
        {
            fillval = Object::NullVal;
            cntval = argv[0];
            if(!Object::isNumber(cntval))
            {
                lit_runtime_error_exiting(vm, "Array() expects either no arguments or (<number>[, <value>])");
                return Object::NullVal;
            }
            if(argc > 1)
            {
                fillval = argv[1];
            }
            count = Object::toNumber(cntval);
            for(i=0; i<count; i++)
            {
                made->push(fillval);
            }
        }
        return made->asValue();
    }

    static Value objfn_array_splice(VM* vm, Array* array, int from, int to)
    {
        size_t i;
        size_t length;
        Array* new_array;
        length = array->size();
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
        new_array = Array::make(vm->m_state);
        for(i = 0; i < length; i++)
        {
            new_array->push(array->at(from + i));
        }
        return new_array->asValue();
    }

    static Value objfn_array_slice(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int from;
        int to;
        from = lit_check_number(vm, argv, argc, 0);
        to = lit_check_number(vm, argv, argc, 1);
        return objfn_array_splice(vm, Object::as<Array>(instance), from, to);
    }

    static Value objfn_array_subscript(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int index;
        Range* range;
        PCGenericArray<Value>* values;
        if(argc == 2)
        {
            if(!Object::isNumber(argv[0]))
            {
                lit_runtime_error_exiting(vm, "array index must be a number");
            }
            values = &Object::as<Array>(instance)->m_actualarray;
            index = Object::toNumber(argv[0]);
            if(index < 0)
            {
                index = fmax(0, values->m_count + index);
            }
            values->reserve(index + 1, Object::NullVal);
            return values->m_values[index] = argv[1];
        }
        if(!Object::isNumber(argv[0]))
        {
            if(Object::isRange(argv[0]))
            {
                range = Object::as<Range>(argv[0]);
                return objfn_array_splice(vm, Object::as<Array>(instance), (int)range->from, (int)range->to);
            }
            lit_runtime_error_exiting(vm, "array index must be a number");
            return Object::NullVal;
        }
        values = &Object::as<Array>(instance)->m_actualarray;
        index = Object::toNumber(argv[0]);
        if(index < 0)
        {
            index = fmax(0, values->m_count + index);
        }
        if(values->m_capacity <= (size_t)index)
        {
            return Object::NullVal;
        }
        return values->m_values[index];
    }

    bool lit_compare_values(State* state, const Value a, const Value b)
    {
        Result inret;
        Value args[3];
        if(Object::isInstance(a))
        {
            args[0] = b;
            inret = Instance::callMethod(state, a, String::intern(state, "=="), args, 1);
            if(inret.type == LITRESULT_OK)
            {
                if(BOOL_VALUE(inret.result) == Object::TrueVal)
                {
                    return true;
                }
                return false;
            }
        }
        return (a == b);
    }

    static Value objfn_array_compare(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        Array* self;
        Array* other;
        (void)argc;
        fprintf(stderr, "call to objfn_array_compare\n");
        self = Object::as<Array>(instance);
        if(Object::isArray(argv[0]))
        {
            other = Object::as<Array>(argv[0]);
            if(self->m_actualarray.m_count == other->m_actualarray.m_count)
            {
                for(i=0; i<self->m_actualarray.m_count; i++)
                {
                    if(!lit_compare_values(vm->m_state, self->m_actualarray.m_values[i], other->m_actualarray.m_values[i]))
                    {
                        return Object::FalseVal;
                    }
                }
                return Object::TrueVal;
            }
            return Object::FalseVal;
        }
        lit_runtime_error_exiting(vm, "can only compare array to another array or null");
        return Object::FalseVal;
    }

    static Value objfn_array_add(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        Array* self;
        (void)vm;
        self = Object::as<Array>(instance);
        for(i=0; i<argc; i++)
        {
            self->push(argv[i]);
        }
        return Object::NullVal;
    }

    static Value objfn_array_insert(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int i;
        int index;
        Value value;
        PCGenericArray<Value>* values;
        LIT_ENSURE_ARGS(2);
        values = &Object::as<Array>(instance)->m_actualarray;
        index = lit_check_number(vm, argv, argc, 0);

        if(index < 0)
        {
            index = fmax(0, values->m_count + index);
        }
        value = argv[1];
        if((int)values->m_count <= index)
        {
            values->reserve(index + 1, Object::NullVal);
        }
        else
        {
            values->reserve(values->m_count + 1, Object::NullVal);
            for(i = values->m_count - 1; i > index; i--)
            {
                values->m_values[i] = values->m_values[i - 1];
            }
        }
        values->m_values[index] = value;
        return Object::NullVal;
    }

    static Value objfn_array_addall(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        Array* array;
        Array* toadd;
        LIT_ENSURE_ARGS(1);
        if(!Object::isArray(argv[0]))
        {
            lit_runtime_error_exiting(vm, "expected array as the argument");
        }
        array = Object::as<Array>(instance);
        toadd = Object::as<Array>(argv[0]);
        for(i = 0; i < toadd->m_actualarray.m_count; i++)
        {
            array->push(toadd->m_actualarray.m_values[i]);
        }
        return Object::NullVal;
    }

    static Value objfn_array_indexof(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1)

            int index = Object::as<Array>(instance)->indexOf(argv[0]);
        return index == -1 ? Object::NullVal : Object::toValue(index);
    }


    static Value objfn_array_remove(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int index;
        Array* array;
        LIT_ENSURE_ARGS(1);
        array = Object::as<Array>(instance);
        index = array->indexOf(argv[0]);
        if(index != -1)
        {
            return array->removeAt((size_t)index);
        }
        return Object::NullVal;
    }

    static Value objfn_array_removeat(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int index;
        index = lit_check_number(vm, argv, argc, 0);
        if(index < 0)
        {
            return Object::NullVal;
        }
        return Object::as<Array>(instance)->removeAt((size_t)index);
    }

    static Value objfn_array_contains(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1);
        return BOOL_VALUE(Object::as<Array>(instance)->indexOf(argv[0]) != -1);
    }

    static Value objfn_array_clear(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        Object::as<Array>(instance)->m_actualarray.m_count = 0;
        return Object::NullVal;
    }

    static Value objfn_array_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int number;
        Array* array;
        (void)vm;
        LIT_ENSURE_ARGS(1);
        array = Object::as<Array>(instance);
        number = 0;
        if(Object::isNumber(argv[0]))
        {
            number = Object::toNumber(argv[0]);
            if(number >= (int)array->m_actualarray.m_count - 1)
            {
                return Object::NullVal;
            }
            number++;
        }
        return array->m_actualarray.m_count == 0 ? Object::NullVal : Object::toValue(number);
    }

    static Value objfn_array_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t index;
        PCGenericArray<Value>* values;
        index = lit_check_number(vm, argv, argc, 0);
        values = &Object::as<Array>(instance)->m_actualarray;
        if(values->m_count <= index)
        {
            return Object::NullVal;
        }
        return values->m_values[index];
    }

    static Value objfn_array_join(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        size_t jlen;
        size_t index;
        size_t length;

        String* chars;
        PCGenericArray<Value>* values;
        String* string;
        String* joinee;
        String** strings;
        (void)argc;
        (void)argv;
        joinee = nullptr;
        length = 0;
        if(argc > 0)
        {
            joinee = Object::as<String>(argv[0]);
        }
        values = &Object::as<Array>(instance)->m_actualarray;
        //String* strings[values.m_count];
        strings = LIT_ALLOCATE(vm->m_state, String*, values->m_count+1);
        for(i = 0; i < values->m_count; i++)
        {
            string = Object::toString(vm->m_state, values->m_values[i]);
            strings[i] = string;
            length += string->length();
            if(joinee != nullptr)
            {
                length += joinee->length();
            }
        }
        jlen = 0;
        index = 0;
        //chars = sdsempty();
        chars = String::make(vm->m_state);
        //chars = sdsMakeRoomFor(chars, length + 1);
        if(joinee != nullptr)
        {
            jlen = joinee->length();
        }
        for(i = 0; i < values->m_count; i++)
        {
            string = strings[i];
            //memcpy(chars + index, string->data(), string->length());
            //chars = sdscatlen(chars, string->data(), string->length());
            chars->append(string);
            index += string->length();
            if(joinee != nullptr)
            {
                if((i+1) != values->m_count)
                {
                    //chars = sdscatlen(chars, joinee->data(), jlen);
                    chars->append(joinee);
                }
                index += jlen;
            }
        }
        LIT_FREE(vm->m_state, String*, strings);
        //return String::take(vm->m_state, chars, length)->asValue();
        return chars->asValue();
    }

    static Value objfn_array_sort(VM* vm, Value instance, size_t argc, Value* argv)
    {
        PCGenericArray<Value>* values;
        values = &Object::as<Array>(instance)->m_actualarray;
        if(argc == 1 && Object::isCallableFunction(argv[0]))
        {
            util_custom_quick_sort(vm, values->m_values, values->m_count, argv[0]);
        }
        else
        {
            util_basic_quick_sort(vm->m_state, values->m_values, values->m_count);
        }
        return instance;
    }

    static Value objfn_array_clone(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        size_t i;
        State* state;
        PCGenericArray<Value>* values;
        Array* array;
        PCGenericArray<Value>* new_values;
        state = vm->m_state;
        values = &Object::as<Array>(instance)->m_actualarray;
        array = Array::make(state);
        new_values = &array->m_actualarray;
        new_values->reserve(values->m_count, Object::NullVal);
        // lit_values_ensure_size sets the count to max of previous count (0 in this case) and new count, so we have to reset it
        new_values->m_count = 0;
        for(i = 0; i < values->m_count; i++)
        {
            new_values->push(values->m_values[i]);
        }
        return array->asValue();
    }

    // todo:
    // CONTAINER_OUTPUT_MAX is a terrible solution.
    // rather, implement lazy range, and then dynamically print (0 ... N)
    static Value objfn_array_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        bool has_more;
        size_t i;
        size_t value_amount;
        size_t olength;
        //char* buffer;
        String* buffer;
        Array* self;
        PCGenericArray<Value>* values;
        Value val;
        State* state;
        String* rt;
        String* part;
        String* stringified;
        String** values_converted;
        static const char* recstring = "(recursion)";
        self = Object::as<Array>(instance);
        values = &self->m_actualarray;
        state = vm->m_state;
        if(values->m_count == 0)
        {
            return String::internValue(state, "[]");
        }
        has_more = values->m_count > LIT_CONTAINER_OUTPUT_MAX;
        //value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->m_count;
        value_amount = values->m_count;
        values_converted = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
        // "[ ]"
        olength = 3;
        if(has_more)
        {
            olength += 3;
        }
        //buffer = sdsempty();
        buffer = String::make(vm->m_state);
        //buffer = sdsMakeRoomFor(buffer, olength+1);
        //buffer = sdscat(buffer, "[");
        buffer->append("[");
        for(i = 0; i < value_amount; i++)
        {
            val = values->m_values[(has_more && i == value_amount - 1) ? values->m_count - 1 : i];
            if(Object::isArray(val) && (Object::as<Array>(val) == self))
            {
                stringified = String::copy(state, recstring, strlen(recstring));
            }
            else
            {
                stringified = Object::toString(state, val);
            }
            part = stringified;
            //buffer = sdscatlen(buffer, part->data(), part->length());
            buffer->append(part);
            /*if(has_more && i == value_amount - 2)
            {
                //buffer = sdscat(buffer, " ... ");
                buffer->append(" ... ");
            }
            else
            */
            {
                if(i == (value_amount - 1))
                {
                    //buffer = sdscat(buffer, " ]");
                    buffer->append(" ]");
                }
                else
                {
                    //buffer = sdscat(buffer, ", ");
                    buffer->append(", ");
                }
            }
        }
        LIT_FREE(vm->m_state, String*, values_converted);
        // should be String::take, but it doesn't get picked up by the GC for some reason
        //rt = String::take(vm->m_state, buffer, olength);
        //rt = String::take(vm->m_state, buffer, olength);
        return buffer->asValue();
    }

    static Value objfn_array_length(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue(Object::as<Array>(instance)->m_actualarray.m_count);
    }

    void lit_open_array_library(State* state)
    {
        Class* klass = Class::make(state, "Array");
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
            klass->setGetter("length", objfn_array_length);
            state->arrayvalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }



    static Value objfn_class_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::format(vm->m_state, "<class @>", Object::as<Class>(instance)->name->asValue())->asValue();
    }

    static Value objfn_class_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        bool fields;
        int value;
        int index;
        int mthcap;
        Class* klass;
        (void)argc;
        LIT_ENSURE_ARGS(1);
        klass = Object::as<Class>(instance);
        index = argv[0] == Object::NullVal ? -1 : Object::toNumber(argv[0]);
        mthcap = (int)klass->methods.capacity();
        fields = index >= mthcap;
        value = (fields ? &klass->static_fields : &klass->methods)->iterator(fields ? index - mthcap : index);
        if(value == -1)
        {
            if(fields)
            {
                return Object::NullVal;
            }
            index++;
            fields = true;
            value = klass->static_fields.iterator(index - mthcap);
        }
        if(value == -1)
        {
            return Object::NullVal;
        }
        return Object::toValue(fields ? value + mthcap : value);
    }


    static Value objfn_class_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        bool fields;
        size_t index;
        size_t mthcap;
        Class* klass;
        index = lit_check_number(vm, argv, argc, 0);
        klass = Object::as<Class>(instance);
        mthcap = klass->methods.capacity();
        fields = index >= mthcap;
        return (fields ? &klass->static_fields : &klass->methods)->iterKey(fields ? index - mthcap : index);
    }


    static Value objfn_class_super(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Class* super;
        (void)vm;
        (void)argc;
        (void)argv;
        super = nullptr;
        if(Object::isInstance(instance))
        {
            super = Object::as<Instance>(instance)->klass->super;
        }
        else
        {
            super = Object::as<Class>(instance)->super;
        }
        if(super == nullptr)
        {
            return Object::NullVal;
        }
        return super->asValue();
    }

    static Value objfn_class_subscript(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Class* klass;    
        Value value;
        (void)argc;
        klass = Object::as<Class>(instance);
        if(argc == 2)
        {
            if(!Object::isString(argv[0]))
            {
                lit_runtime_error_exiting(vm, "class index must be a string");
            }

            klass->static_fields.set(Object::as<String>(argv[0]), argv[1]);
            return argv[1];
        }
        if(!Object::isString(argv[0]))
        {
            lit_runtime_error_exiting(vm, "class index must be a string");
        }
        if(klass->static_fields.get(Object::as<String>(argv[0]), &value))
        {
            return value;
        }
        if(klass->methods.get(Object::as<String>(argv[0]), &value))
        {
            return value;
        }
        return Object::NullVal;
    }

    static Value objfn_class_compare(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Class* selfclass;
        Class* otherclass;
        (void)vm;
        (void)argc;
        if(Object::isClass(argv[0]))
        {
            selfclass = Object::as<Class>(instance);
            otherclass = Object::as<Class>(argv[0]);
            if(String::equal(vm->m_state, selfclass->name, otherclass->name))
            {
                if(selfclass == otherclass)
                {
                    return Object::TrueVal;
                }
            }
        }
        return Object::FalseVal;
    }

    static Value objfn_class_name(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::as<Class>(instance)->name->asValue();
    }

    void lit_open_class_library(State* state)
    {
        Class* klass = Class::make(state, "Class");
        {
            klass->bindMethod("[]", objfn_class_subscript);
            klass->bindMethod("==", objfn_class_compare);
            klass->bindMethod("toString", objfn_class_tostring);
            klass->setGetter("super", objfn_class_super);
            #if 0
            klass->setGetter("methods", objfn_class_getmethods);
            klass->setGetter("fields", objfn_class_getfields);
            #endif
            klass->setStaticGetter("super", objfn_class_super);
            klass->setStaticGetter("name", objfn_class_name);
            klass->setStaticMethod("toString", objfn_class_tostring);
            klass->setStaticMethod("iterator", objfn_class_iterator);
            klass->setStaticMethod("iteratorValue", objfn_class_iteratorvalue);
            state->classvalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
    }


    #if defined(__unix__) || defined(__linux__)
    #endif

    static bool should_update_locals;


    void lit_open_libraries(State* state)
    {
        lit_open_math_library(state);
        lit_open_file_library(state);
        lit_open_gc_library(state);
    }

    #if 0
    #define COMPARE(state, callee, a, b) \
        ( \
        { \
            Value argv[2]; \
            argv[0] = a; \
            argv[1] = b; \
            Result r = state->call(callee, argv, 2); \
            if(r.type != LITRESULT_OK) \
            { \
                return; \
            } \
            !Object::isFalsey(r.result); \
        })
    #else
    static Result COMPARE_inl(State* state, Value callee, Value a, Value b)
    {
        Value argv[2];
        argv[0] = a;
        argv[1] = b;
        return state->call(callee, argv, 2);
    }

    #define COMPARE(state, callee, a, b) \
        COMPARE_inl(state, callee, a, b)
    #endif



    void util_custom_quick_sort(VM* vm, Value* l, int length, Value callee)
    {
        Result rt;
        State* state;
        if(length < 2)
        {
            return;
        }
        state = vm->m_state;
        int pivot_index = length / 2;
        int i;
        int j;
        Value pivot = l[pivot_index];
        for(i = 0, j = length - 1;; i++, j--)
        {
            //while(i < pivot_index && COMPARE(state, callee, l[i], pivot))
            while(i < pivot_index)
            {
                if((rt = COMPARE(state, callee, l[i], pivot)).type != LITRESULT_OK)
                {
                    return;
                }
                if(Object::isFalsey(rt.result))
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
                if(Object::isFalsey(rt.result))
                {
                    break;
                }
                j--;
            }
            if(i >= j)
            {
                break;
            }
            Value tmp = l[i];
            l[i] = l[j];
            l[j] = tmp;
        }
        util_custom_quick_sort(vm, l, i, callee);
        util_custom_quick_sort(vm, l + i, length - i, callee);
    }

    bool util_is_fiber_done(Fiber* fiber)
    {
        return fiber->frame_count == 0 || fiber->abort;
    }

    void util_run_fiber(VM* vm, Fiber* fiber, Value* argv, size_t argc, bool catcher)
    {
        bool vararg;
        int i;
        int to;
        int vararg_count;
        int objfn_function_arg_count;
        Array* array;
        Fiber::CallFrame* frame;
        if(util_is_fiber_done(fiber))
        {
            lit_runtime_error_exiting(vm, "Fiber already finished executing");
        }
        fiber->parent = vm->fiber;
        fiber->catcher = catcher;
        vm->fiber = fiber;
        frame = &fiber->frames[fiber->frame_count - 1];
        if(frame->ip == frame->function->chunk.m_code)
        {
            fiber->arg_count = argc;
            fiber->ensure_stack(frame->function->max_slots + 1 + (int)(fiber->stack_top - fiber->stack));
            frame->slots = fiber->stack_top;
            vm->push(frame->function->asValue());
            vararg = frame->function->vararg;
            objfn_function_arg_count = frame->function->arg_count;
            to = objfn_function_arg_count - (vararg ? 1 : 0);
            fiber->arg_count = objfn_function_arg_count;
            for(i = 0; i < to; i++)
            {
                vm->push(i < (int)argc ? argv[i] : Object::NullVal);
            }
            if(vararg)
            {
                array = Array::make(vm->m_state);
                vm->push(array->asValue());
                vararg_count = argc - objfn_function_arg_count + 1;
                if(vararg_count > 0)
                {
                    array->m_actualarray.reserve(vararg_count, Object::NullVal);
                    for(i = 0; i < vararg_count; i++)
                    {
                        array->m_actualarray.m_values[i] = argv[i + objfn_function_arg_count - 1];
                    }
                }
            }
        }
    }

    static inline bool compare(State* state, Value a, Value b)
    {
        Value argv[1];
        if(Object::isNumber(a) && Object::isNumber(b))
        {
            return Object::toNumber(a) < Object::toNumber(b);
        }
        argv[0] = b;
        return !Object::isFalsey(state->findAndCallMethod(a, String::intern(state, "<"), argv, 1).result);
    }

    void util_basic_quick_sort(State* state, Value* clist, int length)
    {
        int i;
        int j;
        int pivot_index;
        Value tmp;
        Value pivot;
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

    bool util_interpret(VM* vm, Module* module)
    {
        Function* function;
        Fiber* fiber;
        Fiber::CallFrame* frame;
        function = module->main_function;
        fiber = Fiber::make(vm->m_state, module, function);
        fiber->parent = vm->fiber;
        vm->fiber = fiber;
        frame = &fiber->frames[fiber->frame_count - 1];
        if(frame->ip == frame->function->chunk.m_code)
        {
            frame->slots = fiber->stack_top;
            vm->push(frame->function->asValue());
        }
        return true;
    }

    static bool compile_and_interpret(VM* vm, String* modname, const char* source, size_t length)
    {
        Module* module;
        module = vm->m_state->compileModule(modname, source, length);
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
    bool util_attempt_to_require(VM* vm, Value* argv, size_t argc, const char* path, bool ignore_previous, bool folders)
    {
        bool rt;
        bool found;
        size_t i;
        size_t srclength;
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
            dir_path = LIT_ALLOCATE(vm->m_state, char, length+1);
            dir_path[length - 2] = '\0';
            memcpy((void*)dir_path, path, length - 2);
            rt = util_attempt_to_require(vm, argv, argc, dir_path, ignore_previous, true);
            LIT_FREE(vm->m_state, char, dir_path);
            return rt;
        }
        //char modname[length + 5];
        modname = LIT_ALLOCATE(vm->m_state, char, length+5);
        //char modnamedotted[length + 5];
        modnamedotted = LIT_ALLOCATE(vm->m_state, char, length+5);
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
                                dir_path = LIT_ALLOCATE(vm->m_state, char, length + name_length - 2);
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
                                LIT_FREE(vm->m_state, char, dir_path);
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
                dir_name = LIT_ALLOCATE(vm->m_state, char, length + 6);
                dir_name[length + 5] = '\0';
                memcpy(dir_name, modname, length);
                memcpy(dir_name + length, ".init", 5);
                if(util_attempt_to_require(vm, argv, argc, dir_name, ignore_previous, false))
                {
                    success = true;
                }
                LIT_FREE(vm->m_state, char, dir_name);
                return success;
            }
        }
        else if(folders)
        {
            return false;
        }
        modname[length] = '.';
        String* name = String::copy(vm->m_state, modnamedotted, length);
        if(!ignore_previous)
        {
            Value existing_module;
            if(vm->modules->m_values.get(name, &existing_module))
            {
                Module* loaded_module = Object::as<Module>(existing_module);
                if(loaded_module->ran)
                {
                    vm->fiber->stack_top -= argc;
                    argv[-1] = Object::as<Module>(existing_module)->return_value;
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
        source = Util::readFile(modname, &srclength);
        if(source == nullptr)
        {
            return false;
        }
        if(compile_and_interpret(vm, name, source, srclength))
        {
            should_update_locals = true;
        }
        free(source);
        return true;
    }

    bool util_attempt_to_require_combined(VM* vm, Value* argv, size_t argc, const char* a, const char* b, bool ignore_previous)
    {
        bool rt;
        size_t a_length;
        size_t b_length;
        size_t total_length;
        char* path;
        a_length = strlen(a);
        b_length = strlen(b);
        total_length = a_length + b_length + 1;
        path = LIT_ALLOCATE(vm->m_state, char, total_length+1);
        memcpy(path, a, a_length);
        memcpy(path + a_length + 1, b, b_length);
        path[a_length] = '.';
        path[total_length] = '\0';
        rt = util_attempt_to_require(vm, argv, argc, (const char*)&path, ignore_previous, false);
        LIT_FREE(vm->m_state, char, path);
        return rt;
    }
    #endif

    Value util_invalid_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        lit_runtime_error_exiting(vm, "cannot create an instance of built-in type", Object::as<Instance>(instance)->klass->name);
        return Object::NullVal;
    }

    static Value objfn_number_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::stringNumberToString(vm->m_state, Object::toNumber(instance));
    }

    static Value objfn_number_tochar(VM* vm, Value instance, size_t argc, Value* argv)
    {
        char ch;
        (void)argc;
        (void)argv;
        ch = Object::toNumber(instance);
        return String::copy(vm->m_state, &ch, 1)->asValue();
    }


    static Value objfn_number_formatas(VM* vm, Value instance, size_t argc, Value* argv)
    {
        double nv;
        size_t flen;
        const char* fmtstr;
        char buf[50 + 1];
        (void)argc;
        (void)argv;
        nv = Object::toNumber(instance);
        fmtstr = lit_check_string(vm, argv, argc, 0);
        flen = snprintf(buf, 50, fmtstr, (int64_t)nv);        
        return String::copy(vm->m_state, buf, flen)->asValue();
    }

    static Value objfn_bool_compare(VM* vm, Value instance, size_t argc, Value* argv)
    {
        bool bv;
        (void)vm;
        (void)argc;
        bv = Object::asBool(instance);
        if(Object::isNull(argv[0]))
        {
            return BOOL_VALUE(false);
        }
        return BOOL_VALUE(Object::asBool(argv[0]) == bv);
    }

    static Value objfn_bool_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::internValue(vm->m_state, Object::asBool(instance) ? "true" : "false");
    }

    static Value cfn_time(VM* vm, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue((double)clock() / CLOCKS_PER_SEC);
    }

    static Value cfn_systemTime(VM* vm, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue(time(nullptr));
    }

    static Value cfn_print(VM* vm, size_t argc, Value* argv)
    {
        size_t i;
        size_t written = 0;
        String* sv;
        written = 0;
        if(argc == 0)
        {
            return Object::toValue(0);
        }
        for(i = 0; i < argc; i++)
        {
            sv = Object::toString(vm->m_state, argv[i]);
            written += fwrite(sv->data(), sizeof(char), sv->length(), stdout);
        }
        return Object::toValue(written);
    }

    static Value cfn_println(VM* vm, size_t argc, Value* argv)
    {
        size_t i;
        if(argc == 0)
        {
            return Object::NullVal;
        }
        for(i = 0; i < argc; i++)
        {
            lit_printf(vm->m_state, "%s", Object::toString(vm->m_state, argv[i])->data());
        }
        lit_printf(vm->m_state, "\n");
        return Object::NullVal;
    }

    static Value objfn_string_format(VM* vm, Value instance, size_t argc, Value* argv);

    static Value cfn_printf(VM* vm, size_t argc, Value* argv)
    {
        Value vres;
        Value firstarg;
        Value fwdargs[3]={Object::NullVal};
        String* res;
        firstarg = argv[0];
        vres = objfn_string_format(vm, firstarg, argc-1, argv+1);
        res = Object::as<String>(vres);
        fwdargs[0] = res->asValue();
        return cfn_print(vm, 1, fwdargs);
    }

    static bool cfn_eval(VM* vm, size_t argc, Value* argv)
    {
        char* code;
        (void)argc;
        (void)argv;
        code = (char*)lit_check_string(vm, argv, argc, 0);
        return compile_and_interpret(vm, vm->fiber->module->name, code, strlen(code));
    }

    #if 0
    static bool cfn_require(VM* vm, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        bool ignore_previous;
        size_t length;
        char* buffer;
        char* index;
        String* name;
        String* modname;
        name = lit_check_object_string(vm, argv, argc, 0);
        ignore_previous = argc > 1 && Object::isBool(argv[1]) && Object::asBool(argv[1]);
        // First check, if a file with this name exists in the local path
        if(util_attempt_to_require(vm, argv, argc, name->data(), ignore_previous, false))
        {
            return should_update_locals;
        }
        // If not, we join the path of the current module to it (the path goes all the way from the root)
        modname = vm->fiber->module->name;
        // We need to get rid of the module name (test.folder.module -> test.folder)
        index = strrchr(modname->data(), '.');
        if(index != nullptr)
        {
            length = index - modname->data();
            buffer = (char*)malloc(length + 1);
            //char buffer[length + 1];
            memcpy((void*)buffer, modname->data(), length);
            buffer[length] = '\0';
            if(util_attempt_to_require_combined(vm, argv, argc, (const char*)&buffer, name->data(), ignore_previous))
            {
                free(buffer);
                return should_update_locals;
            }
            else
            {
                free(buffer);
            }
        }
        lit_runtime_error_exiting(vm, "failed to require module '%s'", name->data());
        return false;
    }
    #endif

    void lit_open_string_library(State* state);
    void lit_open_array_library(State* state);
    void lit_open_map_library(State* state);
    void lit_open_range_library(State* state);
    void lit_open_fiber_library(State* state);
    void lit_open_module_library(State* state);
    void lit_open_function_library(State* state);
    void lit_open_class_library(State* state);
    void lit_open_object_library(State* state);


    void lit_open_core_library(State* state)
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
            Class* klass = Class::make(state, "Number");
            {
                klass->inheritFrom(state->objectvalue_class);
                klass->bindConstructor(util_invalid_constructor);
                klass->bindMethod("toString", objfn_number_tostring);
                klass->bindMethod("formatString", objfn_number_formatas);
                klass->bindMethod("toChar", objfn_number_tochar);
                klass->setGetter("chr", objfn_number_tochar);
                state->numbervalue_class = klass;
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }
        {
            Class* klass = Class::make(state, "Bool");
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
            state->defineNative("printf", cfn_printf);
            //state->defineNativePrimitive("require", cfn_require);
            state->defineNativePrimitive("eval", cfn_eval);
            state->setGlobal(String::intern(state, "globals"), state->vm->globals->asValue());
        }
    }

    static Value objfn_fiber_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        if(argc < 1 || !Object::isFunction(argv[0]))
        {
            lit_runtime_error_exiting(vm, "Fiber constructor expects a function as its argument");
        }

        Function* function = Object::as<Function>(argv[0]);
        Module* module = vm->fiber->module;
        Fiber* fiber = Fiber::make(vm->m_state, module, function);

        fiber->parent = vm->fiber;

        return fiber->asValue();
    }


    static Value objfn_fiber_done(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return BOOL_VALUE(util_is_fiber_done(Object::as<Fiber>(instance)));
    }


    static Value objfn_fiber_error(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::as<Fiber>(instance)->error;
    }


    static Value objfn_fiber_current(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        (void)argc;
        (void)argv;
        return vm->fiber->asValue();
    }


    static bool objfn_fiber_run(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        util_run_fiber(vm, Object::as<Fiber>(instance), argv, argc, false);
        return true;
    }


    static bool objfn_fiber_try(VM* vm, Value instance, size_t argc, Value* argv)
    {
        util_run_fiber(vm, Object::as<Fiber>(instance), argv, argc, true);
        return true;
    }


    static bool objfn_fiber_yield(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        if(vm->fiber->parent == nullptr)
        {
            lit_handle_runtime_error(vm, argc == 0 ? String::intern(vm->m_state, "Fiber was yielded") :
            Object::toString(vm->m_state, argv[0]));
            return true;
        }

        Fiber* fiber = vm->fiber;

        vm->fiber = vm->fiber->parent;
        vm->fiber->stack_top -= fiber->arg_count;
        vm->fiber->stack_top[-1] = argc == 0 ? Object::NullVal : Object::toString(vm->m_state, argv[0])->asValue();

        argv[-1] = Object::NullVal;
        return true;
    }


    static bool objfn_fiber_yeet(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        if(vm->fiber->parent == nullptr)
        {
            lit_handle_runtime_error(vm, argc == 0 ? String::intern(vm->m_state, "Fiber was yeeted") :
            Object::toString(vm->m_state, argv[0]));
            return true;
        }

        Fiber* fiber = vm->fiber;

        vm->fiber = vm->fiber->parent;
        vm->fiber->stack_top -= fiber->arg_count;
        vm->fiber->stack_top[-1] = argc == 0 ? Object::NullVal : Object::toString(vm->m_state, argv[0])->asValue();

        argv[-1] = Object::NullVal;
        return true;
    }


    static bool objfn_fiber_abort(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        lit_handle_runtime_error(vm, argc == 0 ? String::intern(vm->m_state, "Fiber was aborted") :
        Object::toString(vm->m_state, argv[0]));
        argv[-1] = Object::NullVal;
        return true;
    }

    static Value objfn_fiber_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::format(vm->m_state, "<fiber @>", instance)->asValue();

    }

    void lit_open_fiber_library(State* state)
    {
        Class* klass = Class::make(state, "Fiber");
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(objfn_fiber_constructor);
            klass->bindMethod("toString", objfn_fiber_tostring);
            klass->bindPrimitive("run", objfn_fiber_run);
            klass->bindPrimitive("try", objfn_fiber_try);
            klass->setGetter("done", objfn_fiber_done);
            klass->setGetter("error", objfn_fiber_error);
            klass->setStaticPrimitive("yield", objfn_fiber_yield);
            klass->setStaticPrimitive("yeet", objfn_fiber_yeet);
            klass->setStaticPrimitive("abort", objfn_fiber_abort);
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

    struct DirReader
    {
        void* handle;
    };

    struct DirItem
    {
        char name[LITDIR_PATHSIZE + 1];
        bool isdir;
        bool isfile;
    };

    bool lit_dir_open(DirReader* rd, const char* path)
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

    bool lit_dir_read(DirReader* rd, DirItem* itm)
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

    bool lit_dir_close(DirReader* rd)
    {
        #if defined(LITDIR_ISUNIX)
            closedir((DIR*)(rd->handle));
        #endif
        return false;
    }


    class FileData
    {
        public:
            char* path;
            FILE* handle;
            bool isopen;
    };

    class StdioHandle
    {
        public:
            FILE* handle;
            const char* name;
            bool canread;
            bool canwrite;
    };

    static void* LIT_INSERT_DATA(VM* vm, Value instance, size_t typsz, Userdata::CleanupFuncType cleanup)
    {
        Userdata* userdata = Userdata::make(vm->m_state, typsz, false);
        userdata->cleanup_fn = cleanup;
        Object::as<Instance>(instance)->fields.set(String::intern(vm->m_state, "_data"), userdata->asValue());
        return userdata->data;
    }

    static void* LIT_EXTRACT_DATA(VM* vm, Value instance)
    {
        Value _d;
        if(!Object::as<Instance>(instance)->fields.get(String::intern(vm->m_state, "_data"), &_d))
        {
            lit_runtime_error_exiting(vm, "failed to extract userdata");
            return nullptr;
        }
        return Object::as<Userdata>(_d)->data;
    }

    /*
     * File
     */
    void cleanup_file(State* state, Userdata* data, bool mark)
    {
        (void)state;
        FileData* fd;
        if(mark)
        {
            return;
        }
        if(data != nullptr)
        {
            fd = ((FileData*)data->data);
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

    static Value objmethod_file_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        const char* path;
        const char* mode;
        FILE* hnd;
        FileData* data;
        StdioHandle* hstd;    
        if(argc > 1)
        {
            if(Object::isUserdata(argv[0]))
            {
                hstd = (StdioHandle*)(Object::as<Userdata>(argv[0])->data);
                hnd = hstd->handle;
                //fprintf(stderr, "FILE: hnd=%p name=%s\n", hstd->handle, hstd->name);
                data = (FileData*)LIT_INSERT_DATA(vm, instance, sizeof(FileData), nullptr);
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
                data = (FileData*)LIT_INSERT_DATA(vm, instance, sizeof(FileData), cleanup_file);
                data->path = (char*)path;
                data->handle = hnd;
                data->isopen = true;
            }
        }
        else
        {
            lit_runtime_error_exiting(vm, "File() expects either string|string, or userdata|string");
            return Object::NullVal;
        }
        return instance;
    }


    static Value objmethod_file_close(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        FileData* data;
        data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        fclose(data->handle);
        data->handle = nullptr;
        data->isopen = false;
        return Object::NullVal;
    }

    static Value objmethod_file_exists(VM* vm, Value instance, size_t argc, Value* argv)
    {
        char* file_name;
        file_name = nullptr;
        if(Object::isInstance(instance))
        {
            file_name = ((FileData*)LIT_EXTRACT_DATA(vm, instance))->path;
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

    static Value objmethod_file_write(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1)
        size_t rt;
        String* value;
        value = Object::toString(vm->m_state, argv[0]);
        rt = fwrite(value->data(), value->length(), 1, ((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle);
        return Object::toValue(rt);
    }

    static Value objmethod_file_writebyte(VM* vm, Value instance, size_t argc, Value* argv)
    {
        uint8_t rt;
        uint8_t byte;
        byte = (uint8_t)lit_check_number(vm, argv, argc, 0);
        rt = FileIO::write_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, byte);
        return Object::toValue(rt);
    }

    static Value objmethod_file_writeshort(VM* vm, Value instance, size_t argc, Value* argv)
    {
        uint16_t rt;
        uint16_t shrt;
        shrt = (uint16_t)lit_check_number(vm, argv, argc, 0);
        rt = FileIO::write_uint16_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, shrt);
        return Object::toValue(rt);
    }

    static Value objmethod_file_writenumber(VM* vm, Value instance, size_t argc, Value* argv)
    {
        uint32_t rt;
        float num;
        num = (float)lit_check_number(vm, argv, argc, 0);
        rt = FileIO::write_uint32_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, num);
        return Object::toValue(rt);
    }

    static Value objmethod_file_writebool(VM* vm, Value instance, size_t argc, Value* argv)
    {
        bool value;
        uint8_t rt;
        value = lit_check_bool(vm, argv, argc, 0);
        rt = FileIO::write_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, (uint8_t)value ? '1' : '0');
        return Object::toValue(rt);
    }

    static Value objmethod_file_writestring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        String* string;
        FileData* data;
        if(lit_check_string(vm, argv, argc, 0) == nullptr)
        {
            return Object::NullVal;
        }
        string = Object::as<String>(argv[0]);
        data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        FileIO::write_string(data->handle, string);
        return Object::NullVal;
    }

    /*
     * ==
     * File reading
     */

    static Value objmethod_file_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        const char* path;
        auto data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        path = data->path;
        if(path == nullptr)
        {
            path = "stdio";
        }
        return String::format(vm->m_state, "<file @ ($)>", Object::as<Instance>(instance), data->path)->asValue();
    }

    static Value objmethod_file_readall(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        (void)argc;
        (void)argv;
        char c;
        long length;
        long actuallength;
        char* buffer;
        FileData* data;
        String* result;
        data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        if(fseek(data->handle, 0, SEEK_END) == -1)
        {
            /*
            * cannot seek, must read each byte.
            */
            result = String::allocEmpty(vm->m_state, 0);
            actuallength = 0;
            while((c = fgetc(data->handle)) != EOF)
            {
                //result->m_chars = sdscatlen(result->m_chars, &c, 1);
                result->append(c);
                actuallength++;
            }
        }
        else
        {
            result = String::make(vm->m_state);
            length = ftell(data->handle);
            fseek(data->handle, 0, SEEK_SET);
            buffer = LIT_ALLOCATE(vm->m_state, char, length+1);
            actuallength = fread(buffer, sizeof(char), length, data->handle);
            result->append(buffer, actuallength);
            LIT_FREE(vm->m_state, char, buffer);
            /*
            * after reading, THIS actually sets the correct length.
            * before that, it would be 0.
            */
            //sdsIncrLen(result->m_chars, actuallength);
        }
        
        result->m_hash = String::makeHash(*(result->m_chars));
        String::statePutInterned(vm->m_state, result);
        return result->asValue();
    }

    static Value objmethod_file_readline(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        size_t max_length;
        char* line;
        FileData* data;
        max_length = (size_t)lit_get_number(vm, argv, argc, 0, 128);
        data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        line = LIT_ALLOCATE(vm->m_state, char, max_length + 1);
        if(!fgets(line, max_length, data->handle))
        {
            LIT_FREE(vm->m_state, char, line);
            return Object::NullVal;
        }
        return String::take(vm->m_state, line, strlen(line) - 1)->asValue();
    }

    static Value objmethod_file_readbyte(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        return Object::toValue(FileIO::read_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
    }

    static Value objmethod_file_readshort(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        return Object::toValue(FileIO::read_uint16_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
    }

    static Value objmethod_file_readnumber(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        return Object::toValue(FileIO::read_uint32_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
    }

    static Value objmethod_file_readbool(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        return BOOL_VALUE((char)FileIO::read_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle) == '1');
    }

    static Value objmethod_file_readstring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        FileData* data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
        String* string = FileIO::read_string(vm->m_state, data->handle);

        return string == nullptr ? Object::NullVal : string->asValue();
    }

    static Value objmethod_file_getlastmodified(VM* vm, Value instance, size_t argc, Value* argv)
    {
        struct stat buffer;
        char* file_name = nullptr;
        (void)vm;
        (void)argc;
        (void)argv;
        if(Object::isInstance(instance))
        {
            file_name = ((FileData*)LIT_EXTRACT_DATA(vm, instance))->path;
        }
        else
        {
            file_name = (char*)lit_check_string(vm, argv, argc, 0);
        }

        if(stat(file_name, &buffer) != 0)
        {
            return Object::toValue(0);
        }
        #if defined(__unix__) || defined(__linux__)
        return Object::toValue(buffer.st_mtim.tv_sec);
        #else
            return Object::toValue(0);
        #endif
    }

    /*
    * Directory
    */
    static Value objfunction_directory_exists(VM* vm, Value instance, size_t argc, Value* argv)
    {
        const char* directory_name = lit_check_string(vm, argv, argc, 0);
        struct stat buffer;
        (void)vm;
        (void)instance;
        (void)argc;
        (void)argv;
        return BOOL_VALUE(stat(directory_name, &buffer) == 0 && S_ISDIR(buffer.st_mode));
    }

    static Value objfunction_directory_listfiles(VM* vm, Value instance, size_t argc, Value* argv)
    {
        State* state;
        Array* array;
        (void)instance;
        state = vm->m_state;
        array = Array::make(state);
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
                    array->push(String::internValue(state, ep->d_name));
                }
            }
            closedir(dir);
        }
        #endif
        return array->asValue();
    }

    static Value objfunction_directory_listdirs(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Array* array;
        State* state;
        DirReader rd;
        DirItem ent;
        (void)instance;
        state = vm->m_state;
        array = Array::make(state);

        if(lit_dir_open(&rd, lit_check_string(vm, argv, argc, 0)))
        {
            while(true)
            {
                if(lit_dir_read(&rd, &ent))
                {
                    if(ent.isdir && ((strcmp(ent.name, ".") != 0) || (strcmp(ent.name, "..") != 0)))
                    {
                        array->push(String::internValue(state, ent.name));
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

    static void free_handle(State* state, Userdata* userdata, bool mark)
    {
        StdioHandle* hstd;
        (void)mark;
        hstd = (StdioHandle*)(userdata->data);
        LIT_FREE(state, StdioHandle, hstd);
    }

    static void make_handle(State* state, Value fileval, const char* name, FILE* hnd, bool canread, bool canwrite)
    {
        Userdata* userhnd;
        Value args[5];
        String* varname;
        String* descname;
        Result res;
        Fiber* oldfiber;
        StdioHandle* hstd;
        oldfiber = state->vm->fiber;
        state->vm->fiber = Fiber::make(state, state->last_module, nullptr);
        {
            hstd = LIT_ALLOCATE(state, StdioHandle, 1);
            hstd->handle = hnd;
            hstd->name = name;
            hstd->canread = canread;
            hstd->canwrite = canwrite; 
            userhnd = Userdata::make(state, sizeof(StdioHandle), true);
            userhnd->data = hstd;
            userhnd->canfree = false;
            userhnd->cleanup_fn = free_handle;
            varname = String::intern(state, name);
            descname = String::intern(state, name);
            args[0] = userhnd->asValue();
            args[1] = descname->asValue();
            res = state->call(fileval, args, 2);
            //fprintf(stderr, "make_handle(%s, hnd=%p): res.type=%d, res.result=%s\n", name, hnd, res.type, Object::valueName(res.result));
            state->setGlobal(varname, res.result);
        }
        state->vm->fiber = oldfiber;
    }

    static void make_stdhandles(State* state)
    {
        Value fileval;
        fileval = state->getGlobal(String::intern(state, "File"));
        //fprintf(stderr, "fileval=%s\n", Object::valueName(fileval));
        {
            make_handle(state, fileval, "STDIN", stdin, true, false);
            make_handle(state, fileval, "STDOUT", stdout, false, true);
            make_handle(state, fileval, "STDERR", stderr, false, true);
        }
    }

    void lit_open_file_library(State* state)
    {
        {
            Class* klass = Class::make(state, "File");
            {
                klass->setStaticMethod("exists", objmethod_file_exists);
                klass->setStaticMethod("getLastModified", objmethod_file_getlastmodified);
                klass->bindConstructor(objmethod_file_constructor);
                klass->bindMethod("toString", objmethod_file_tostring);
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
                klass->setGetter("exists", objmethod_file_exists);
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }
        {
            Class* klass = Class::make(state, "Directory");
            {
                klass->setStaticMethod("exists", objfunction_directory_exists);
                klass->setStaticMethod("listFiles", objfunction_directory_listfiles);
                klass->setStaticMethod("listDirectories", objfunction_directory_listdirs);
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }

        make_stdhandles(state);
    }



    static Value objfn_function_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return Object::functionName(vm, instance)->asValue();
    }

    static Value objfn_function_name(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return Object::functionName(vm, instance)->asValue();
    }

    void lit_open_function_library(State* state)
    {
         Class* klass = Class::make(state, "Function");
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->bindMethod("toString", objfn_function_tostring);
            klass->setGetter("name", objfn_function_name);
            state->functionvalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

    static Value objfn_gc_memory_used(VM* vm, Value instance, size_t arg_count, Value* args)
    {
        (void)instance;
        (void)arg_count;
        (void)args;
        return Object::toValue(vm->m_state->bytes_allocated);
    }

    static Value objfn_gc_next_round(VM* vm, Value instance, size_t arg_count, Value* args)
    {
        (void)instance;
        (void)arg_count;
        (void)args;
        return Object::toValue(vm->m_state->next_gc);
    }

    static Value objfn_gc_trigger(VM* vm, Value instance, size_t arg_count, Value* args)
    {
        (void)instance;
        (void)arg_count;
        (void)args;
        int64_t collected;
        vm->m_state->allow_gc = true;
        collected = vm->collectGarbage();
        vm->m_state->allow_gc = false;

        return Object::toValue(collected);
    }

    void lit_open_gc_library(State* state)
    {
        Class* klass = Class::make(state, "GC");
        {
            klass->setStaticGetter("memoryUsed", objfn_gc_memory_used);
            klass->setStaticGetter("nextRound", objfn_gc_next_round);
            klass->setStaticMethod("trigger", objfn_gc_trigger);
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

    static Value objfn_map_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        (void)argc;
        (void)argv;
        return Map::make(vm->m_state)->asValue();
    }

    static Value objfn_map_subscript(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Value val;
        Value value;
        Map* map;
        String* index;
        if(!Object::isString(argv[0]))
        {
            lit_runtime_error_exiting(vm, "map index must be a string");
        }
        map = Object::as<Map>(instance);
        index = Object::as<String>(argv[0]);
        if(argc == 2)
        {
            val = argv[1];
            if(map->m_indexfn != nullptr)
            {
                return map->m_indexfn(vm, map, index, &val);
            }
            map->set(index, val);
            return val;
        }
        if(map->m_indexfn != nullptr)
        {
            return map->m_indexfn(vm, map, index, nullptr);
        }
        if(!map->m_values.get(index, &value))
        {
            return Object::NullVal;
        }
        return value;
    }

    static Value objfn_map_addall(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1);
        if(!Object::isMap(argv[0]))
        {
            lit_runtime_error_exiting(vm, "expected map as the argument");
        }
        Object::as<Map>(argv[0])->addAll(Object::as<Map>(instance));
        return Object::NullVal;
    }


    static Value objfn_map_clear(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argv;
        (void)argc;
        Object::as<Map>(instance)->m_values.m_inner .m_count = 0;
        return Object::NullVal;
    }

    static Value objfn_map_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1);
        (void)vm;
        int index;
        int value;
        index = argv[0] == Object::NullVal ? -1 : Object::toNumber(argv[0]);
        value = Object::as<Map>(instance)->m_values.iterator(index);
        return value == -1 ? Object::NullVal : Object::toValue(value);
    }

    static Value objfn_map_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t index;
        index = lit_check_number(vm, argv, argc, 0);
        return Object::as<Map>(instance)->m_values.iterKey(index);
    }

    static Value objfn_map_clone(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        State* state;
        Map* map;
        state = vm->m_state;
        map = Map::make(state);
        Object::as<Map>(instance)->m_values.addAll(&map->m_values);
        return map->asValue();
    }

    static Value objfn_map_tostring(VM* vm, Value instance, size_t argc, Value* argv)
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
        State* state;
        Map* map;
        Table* values;
        Table::Entry* entry;
        Value field;
        String* strobval;
        String* key;
        String* value;
        String** values_converted;
        String** keys;
        state = vm->m_state;
        map = Object::as<Map>(instance);
        values = &map->m_values;
        if(values->size() == 0)
        {
            return String::internValue(state, "{}");
        }
        has_wrapper = map->m_indexfn != nullptr;
        has_more = values->size() > LIT_CONTAINER_OUTPUT_MAX;
        value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->size();
        values_converted = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
        keys = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
        olength = 3;
        if(has_more)
        {
            olength += SINGLE_LINE_MAPS_ENABLED ? 5 : 6;
        }
        i = 0;
        index = 0;
        do
        {
            entry = &values->m_inner.m_values[index++];
            if(entry->key != nullptr)
            {
                // Special hidden key
                field = has_wrapper ? map->m_indexfn(vm, map, entry->key, nullptr) : entry->value;
                // This check is required to prevent infinite loops when playing with Module.privates and such
                strobval = (Object::isMap(field) && Object::as<Map>(field)->m_indexfn != nullptr) ? String::intern(state, "map") : Object::toString(state, field);
                state->pushRoot((Object*)strobval);
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
        buffer = LIT_ALLOCATE(vm->m_state, char, olength+1);
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
            memcpy(&buffer[buffer_index], key->data(), key->length());
            buffer_index += key->length();
            memcpy(&buffer[buffer_index], " = ", 3);
            buffer_index += 3;
            memcpy(&buffer[buffer_index], value->data(), value->length());
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
        LIT_FREE(vm->m_state, String*, keys);
        LIT_FREE(vm->m_state, String*, values_converted);
        return String::take(vm->m_state, buffer, olength)->asValue();
    }

    static Value objfn_map_length(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue(Object::as<Map>(instance)->size());
    }

    void lit_open_map_library(State* state)
    {
        Class* klass = Class::make(state, "Map");
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
            klass->setGetter("length", objfn_map_length);
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

    static Value math_abs(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(fabs(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_cos(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(cos(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_sin(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(sin(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_tan(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(tan(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_acos(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(acos(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_asin(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(asin(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_atan(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(atan(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_atan2(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(atan2(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
    }

    static Value math_floor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(floor(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_ceil(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(ceil(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_round(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int places;
        double value;
        (void)instance;
        value = lit_check_number(vm, argv, argc, 0);
        if(argc > 1)
        {
            places = (int)pow(10, lit_check_number(vm, argv, argc, 1));
            return Object::toValue(round(value * places) / places);
        }
        return Object::toValue(round(value));
    }

    static Value math_min(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(fmin(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
    }

    static Value math_max(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(fmax(lit_check_number(vm, argv, argc, 0), lit_check_number(vm, argv, argc, 1)));
    }

    static Value math_mid(VM* vm, Value instance, size_t argc, Value* argv)
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
            return Object::toValue(fmax(x, fmin(y, z)));
        }
        return Object::toValue(fmax(y, fmin(x, z)));
    }

    static Value math_toRadians(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(lit_check_number(vm, argv, argc, 0) * M_PI / 180.0);
    }

    static Value math_toDegrees(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(lit_check_number(vm, argv, argc, 0) * 180.0 / M_PI);
    }

    static Value math_sqrt(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(sqrt(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_log(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(exp(lit_check_number(vm, argv, argc, 0)));
    }

    static Value math_exp(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        return Object::toValue(exp(lit_check_number(vm, argv, argc, 0)));
    }

    /*
     * Random
     */

    static size_t static_random_data;

    static size_t* extract_random_data(State* state, Value instance)
    {
        if(Object::isClass(instance))
        {
            return &static_random_data;
        }

        Value data;

        if(!Object::as<Instance>(instance)->fields.get(String::intern(state, "_data"), &data))
        {
            return 0;
        }

        return (size_t*)Object::as<Userdata>(data)->data;
    }

    static Value random_constructor(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Userdata* userdata = Userdata::make(vm->m_state, sizeof(size_t), false);
        Object::as<Instance>(instance)->fields.set(String::intern(vm->m_state, "_data"), userdata->asValue());

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

    static Value random_setSeed(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t* data = extract_random_data(vm->m_state, instance);

        if(argc == 1)
        {
            size_t number = (size_t)lit_check_number(vm, argv, argc, 0);
            *data = number;
        }
        else
        {
            *data = time(nullptr);
        }

        return Object::NullVal;
    }

    static Value random_int(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t* data = extract_random_data(vm->m_state, instance);

        if(argc == 1)
        {
            int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
            return Object::toValue(rand_r((unsigned int*)data) % bound);
        }
        else if(argc == 2)
        {
            int min = (int)lit_get_number(vm, argv, argc, 0, 0);
            int max = (int)lit_get_number(vm, argv, argc, 1, 1);

            if(max - min == 0)
            {
                return Object::toValue(max);
            }

            return Object::toValue(min + rand_r((unsigned int*)data) % (max - min));
        }

        return Object::toValue(rand_r((unsigned int*)data));
    }

    static Value random_float(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t* data = extract_random_data(vm->m_state, instance);
        double value = (double)rand_r((unsigned int*)data) / RAND_MAX;

        if(argc == 1)
        {
            int bound = (int)lit_get_number(vm, argv, argc, 0, 0);
            return Object::toValue(value * bound);
        }
        else if(argc == 2)
        {
            int min = (int)lit_get_number(vm, argv, argc, 0, 0);
            int max = (int)lit_get_number(vm, argv, argc, 1, 1);

            if(max - min == 0)
            {
                return Object::toValue(max);
            }

            return Object::toValue(min + value * (max - min));
        }

        return Object::toValue(value);
    }

    static Value random_bool(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        (void)argc;
        (void)argv;
        return BOOL_VALUE(rand_r((unsigned int*)extract_random_data(vm->m_state, instance)) % 2);
    }

    static Value random_chance(VM* vm, Value instance, size_t argc, Value* argv)
    {
        float c;
        c = lit_get_number(vm, argv, argc, 0, 50);
        if((((float)rand_r((unsigned int*)extract_random_data(vm->m_state, instance))) / ((float)(RAND_MAX-1)) * 100) <= c)
        {
            return Object::TrueVal;
        }
        return Object::FalseVal;
    }

    static Value random_pick(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int value = rand_r((unsigned int*)extract_random_data(vm->m_state, instance));

        if(argc == 1)
        {
            if(Object::isArray(argv[0]))
            {
                Array* array = Object::as<Array>(argv[0]);

                if(array->m_actualarray.m_count == 0)
                {
                    return Object::NullVal;
                }

                return array->m_actualarray.m_values[value % array->m_actualarray.m_count];
            }
            else if(Object::isMap(argv[0]))
            {
                Map* map = Object::as<Map>(argv[0]);
                size_t length = map->size();
                size_t m_capacity = map->capacity();

                if(length == 0)
                {
                    return Object::NullVal;
                }

                size_t target = value % length;
                size_t index = 0;

                for(size_t i = 0; i < m_capacity; i++)
                {
                    if(map->at(i).key != nullptr)
                    {
                        if(index == target)
                        {
                            return map->m_values.m_inner.m_values[i].value;
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

        return Object::NullVal;
    }

    void lit_open_math_library(State* state)
    {
        {
            Class* klass = Class::make(state, "Math");
            {
                klass->static_fields.setField("Pi", Object::toValue(M_PI));
                klass->static_fields.setField("Tau", Object::toValue(M_PI * 2));
                klass->setStaticMethod("abs", math_abs);
                klass->setStaticMethod("sin", math_sin);
                klass->setStaticMethod("cos", math_cos);
                klass->setStaticMethod("tan", math_tan);
                klass->setStaticMethod("asin", math_asin);
                klass->setStaticMethod("acos", math_acos);
                klass->setStaticMethod("atan", math_atan);
                klass->setStaticMethod("atan2", math_atan2);
                klass->setStaticMethod("floor", math_floor);
                klass->setStaticMethod("ceil", math_ceil);
                klass->setStaticMethod("round", math_round);
                klass->setStaticMethod("min", math_min);
                klass->setStaticMethod("max", math_max);
                klass->setStaticMethod("mid", math_mid);
                klass->setStaticMethod("toRadians", math_toRadians);
                klass->setStaticMethod("toDegrees", math_toDegrees);
                klass->setStaticMethod("sqrt", math_sqrt);
                klass->setStaticMethod("log", math_log);
                klass->setStaticMethod("exp", math_exp);
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
            Class* klass = Class::make(state, "Random");
            {
                klass->bindConstructor(random_constructor);
                klass->bindMethod("setSeed", random_setSeed);
                klass->bindMethod("int", random_int);
                klass->bindMethod("float", random_float);
                klass->bindMethod("chance", random_chance);
                klass->bindMethod("pick", random_pick);
                klass->setStaticMethod("setSeed", random_setSeed);
                klass->setStaticMethod("int", random_int);
                klass->setStaticMethod("float", random_float);
                klass->setStaticMethod("bool", random_bool);
                klass->setStaticMethod("chance", random_chance);
                klass->setStaticMethod("pick", random_pick);
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }

    }


    static Value access_private(VM* vm, Map* map, String* name, Value* val)
    {
        int index;
        Value value;
        String* id;
        Module* module;
        id = String::intern(vm->m_state, "_module");
        if(!map->m_values.get(id, &value) || !Object::isModule(value))
        {
            return Object::NullVal;
        }
        module = Object::as<Module>(value);

        if(id == name)
        {
            return module->asValue();
        }

        if(module->private_names->m_values.get(name, &value))
        {
            index = (int)Object::toNumber(value);
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
        return Object::NullVal;
    }


    static Value objfn_module_privates(VM* vm, Value instance, size_t argc, Value* argv)
    {
        Module* module;
        Map* map;
        (void)argc;
        (void)argv;
        module = Object::isModule(instance) ? Object::as<Module>(instance) : vm->fiber->module;
        map = module->private_names;
        if(map->m_indexfn == nullptr)
        {
            map->m_indexfn = access_private;
            map->m_values.set(String::intern(vm->m_state, "_module"), module->asValue());
        }
        return map->asValue();
    }

    static Value objfn_module_current(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)instance;
        (void)argc;
        (void)argv;
        return vm->fiber->module->asValue();
    }

    static Value objfn_module_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::format(vm->m_state, "<module @>", Object::as<Module>(instance)->name->asValue())->asValue();
    }

    static Value objfn_module_name(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::as<Module>(instance)->name->asValue();
    }

    void lit_open_module_library(State* state)
    {
        Class* klass = Class::make(state, "Module");
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->static_fields.setField("loaded", state->vm->modules->asValue());
            klass->setStaticGetter("privates", objfn_module_privates);
            klass->setStaticGetter("current", objfn_module_current);
            klass->bindMethod("toString", objfn_module_tostring);
            klass->setGetter("name", objfn_module_name);
            klass->setGetter("privates", objfn_module_privates);
            state->modulevalue_class = klass;
        }
        state->setGlobal(klass->name, klass->asValue());
        if(klass->super == nullptr)
        {
            klass->inheritFrom(state->objectvalue_class);
        }
    }

    static Value objfn_object_class(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return Class::getClassFor(vm->m_state, instance)->asValue();
    }

    static Value objfn_object_super(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        Class* cl;
        cl = Class::getClassFor(vm->m_state, instance)->super;
        if(cl == nullptr)
        {
            return Object::NullVal;
        }
        return cl->asValue();
    }

    static Value objfn_object_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        return String::format(vm->m_state, "<instance @>", Class::getClassFor(vm->m_state, instance)->name->asValue())->asValue();
    }

    static void fillmap(State* state, Map* destmap, Table* fromtbl, bool includenullkeys)
    {
        size_t i;
        String* key;
        Value val;
        (void)state;
        (void)includenullkeys;
        for(i=0; i<fromtbl->size(); i++)
        {
            key = fromtbl->at(i).key;
            if(key != nullptr)
            {
                val = fromtbl->at(i).value;
                destmap->set(key, val);
            }
        }
    }

    static Value objfn_object_tomap(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        Map* map;
        Map* minst;
        Map* mclass;
        Map* mclstatics;
        Map* mclmethods;
        Instance* inst;
        mclass = nullptr;
        if(!Object::isInstance(instance))
        {
            lit_runtime_error_exiting(vm, "toMap() can only be used on instances");
        }
        inst = Object::as<Instance>(instance);
        map = Map::make(vm->m_state);
        {
            minst = Map::make(vm->m_state);
            fillmap(vm->m_state, minst, &(inst->fields), true);
        }
        {
            mclass = Map::make(vm->m_state);
            {
                mclstatics = Map::make(vm->m_state);
                fillmap(vm->m_state, mclstatics, &(inst->klass->static_fields), false);
            }
            {
                mclmethods = Map::make(vm->m_state);
                fillmap(vm->m_state, mclmethods, &(inst->klass->methods), false);
            }
            mclass->set(String::intern(vm->m_state, "statics"), mclstatics->asValue());
            mclass->set(String::intern(vm->m_state, "methods"), mclmethods->asValue());
        }
        map->set(String::intern(vm->m_state, "instance"), minst->asValue());
        map->set(String::intern(vm->m_state, "class"), mclass->asValue());
        return map->asValue();
    }

    static Value objfn_object_subscript(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        Value value;
        Instance* inst;
        if(!Object::isInstance(instance))
        {
            lit_runtime_error_exiting(vm, "cannot modify built-in types");
        }
        inst = Object::as<Instance>(instance);
        if(argc == 2)
        {
            if(!Object::isString(argv[0]))
            {
                lit_runtime_error_exiting(vm, "object index must be a string");
            }
            inst->fields.set(Object::as<String>(argv[0]), argv[1]);
            return argv[1];
        }
        if(!Object::isString(argv[0]))
        {
            lit_runtime_error_exiting(vm, "object index must be a string");
        }
        if(inst->fields.get(Object::as<String>(argv[0]), &value))
        {
            return value;
        }
        if(inst->klass->static_fields.get(Object::as<String>(argv[0]), &value))
        {
            return value;
        }
        if(inst->klass->methods.get(Object::as<String>(argv[0]), &value))
        {
            return value;
        }
        return Object::NullVal;
    }

    static Value objfn_object_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        int value;
        int index;
        Instance* self;
        LIT_ENSURE_ARGS(1);
        self = Object::as<Instance>(instance);
        index = argv[0] == Object::NullVal ? -1 : Object::toNumber(argv[0]);
        value = self->fields.iterator(index);
        return value == -1 ? Object::NullVal : Object::toValue(value);
    }


    static Value objfn_object_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t index;
        Instance* self;
        index = lit_check_number(vm, argv, argc, 0);
        self = Object::as<Instance>(instance);
        return self->fields.iterKey(index);
    }

    void lit_open_object_library(State* state)
    {
        Class* klass = Class::make(state, "Object");
        {
            klass->inheritFrom(state->classvalue_class);
            klass->setGetter("class", objfn_object_class);
            klass->setGetter("super", objfn_object_super);
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

    static Value objfn_range_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1);
        int number;
        Range* range;
        range = Object::as<Range>(instance);
        number = range->from;
        (void)vm;
        (void)argc;
        if(Object::isNumber(argv[0]))
        {
            number = Object::toNumber(argv[0]);
            if((range->to > range->from) ? (number >= range->to) : (number >= range->from))
            {
                return Object::NullVal;
            }
            number += (((range->from - range->to) > 0) ? -1 : 1);
        }
        return Object::toValue(number);
    }

    static Value objfn_range_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        LIT_ENSURE_ARGS(1);
        (void)vm;
        (void)instance;
        return argv[0];
    }

    static Value objfn_range_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)argc;
        (void)argv;
        Range* range;
        range = Object::as<Range>(instance);
        return String::format(vm->m_state, "<range (#, #)>", range->from, range->to)->asValue();
    }

    static Value objfn_range_from(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argv;
        (void)argc;
        return Object::toValue(Object::as<Range>(instance)->from);
    }

    static Value objfn_range_set_from(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        Object::as<Range>(instance)->from = Object::toNumber(argv[0]);
        return argv[0];
    }

    static Value objfn_range_to(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue(Object::as<Range>(instance)->to);
    }

    static Value objfn_range_set_to(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        Object::as<Range>(instance)->to = Object::toNumber(argv[0]);
        return argv[0];
    }

    static Value objfn_range_length(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        Range* range;
        range = Object::as<Range>(instance);
        return Object::toValue(range->to - range->from);
    }

    void lit_open_range_library(State* state)
    {
        Class* klass = Class::make(state, "Range");
        {
            klass->inheritFrom(state->objectvalue_class);
            klass->bindConstructor(util_invalid_constructor);
            klass->bindMethod("iterator", objfn_range_iterator);
            klass->bindMethod("iteratorValue", objfn_range_iteratorvalue);
            klass->bindMethod("toString", objfn_range_tostring);
            klass->bindField("from", objfn_range_from, objfn_range_set_from);
            klass->bindField("to", objfn_range_to, objfn_range_set_to);
            klass->setGetter("length", objfn_range_length);
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


    Value util_invalid_constructor(VM* vm, Value instance, size_t argc, Value* argv);

    static Value objfn_string_plus(VM* vm, Value instance, size_t argc, Value* argv)
    {
        String* selfstr;
        String* result;
        Value value;
        (void)argc;
        selfstr = Object::as<String>(instance);
        value = argv[0];
        String* strval = nullptr;
        if(Object::isString(value))
        {
            strval = Object::as<String>(value);
        }
        else
        {
            strval = Object::toString(vm->m_state, value);
        }
        result = String::allocEmpty(vm->m_state, selfstr->length() + strval->length());
        result->append(selfstr);
        result->append(strval);
        String::statePutInterned(vm->m_state, result);
        return result->asValue();
    }

    static Value objfn_string_splice(VM* vm, String* string, int64_t from, int64_t to)
    {
        size_t length;
        size_t actualto;
        size_t actualfrom;
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
            if(to == -1)
            {
                
            }
            //else
            {
                lit_runtime_error_exiting(vm, "String.splice argument 'from' (%d) is larger than argument 'to' (%d)", from, to);
            }
        }
        from = String::utfcharOffset(string->data(), from);
        to = String::utfcharOffset(string->data(), to);
        return String::fromRange(vm->m_state, string, from, to - from + 1)->asValue();
    }

    static Value objfn_string_subscript(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int index;
        String* c;
        String* string;
        if(Object::isRange(argv[0]))
        {
            Range* range = Object::as<Range>(argv[0]);
            return objfn_string_splice(vm, Object::as<String>(instance), range->from, range->to);
        }
        string = Object::as<String>(instance);
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
                return Object::NullVal;
            }
        }
        c = string->codePointAt(String::utfcharOffset(string->data(), index));
        if(c == nullptr)
        {
            return String::intern(vm->m_state, "")->asValue();
        }
        return c->asValue();
    }


    static Value objfn_string_compare(VM* vm, Value instance, size_t argc, Value* argv)
    {
        String* self;
        String* other;
        (void)argc;
        self = Object::as<String>(instance);
        if(Object::isString(argv[0]))
        {
            other = Object::as<String>(argv[0]);
            if(self->length() == other->length())
            {
                //fprintf(stderr, "string: same length(self=\"%s\" other=\"%s\")... strncmp=%d\n", self->data(), other->data(), strncmp(self->data(), other->data(), self->length));
                if(memcmp(self->data(), other->data(), self->length()) == 0)
                {
                    return Object::TrueVal;
                }
            }
            return Object::FalseVal;
        }
        else if(Object::isNull(argv[0]))
        {
            if((self == nullptr) || Object::isNull(instance))
            {
                return Object::TrueVal;
            }
            return Object::FalseVal;
        }
        lit_runtime_error_exiting(vm, "can only compare string to another string or null");
        return Object::FalseVal;
    }

    static Value objfn_string_append(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        String* self;
        String* other;
        (void)argc;
        self = Object::as<String>(instance);
        for(i=0; i<argc; i++)
        {
            other = Object::toString(vm->m_state, argv[i]);
            self->append(other);
        }
        return self->asValue();
    }

    static Value objfn_string_appendbyte(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int byte;
        size_t i;
        String* self;
        String* other;
        (void)argc;
        self = Object::as<String>(instance);
        for(i=0; i<argc; i++)
        {
            if(!Object::isNumber(argv[i]))
            {
                lit_runtime_error_exiting(vm, "appendByte() expects numbers");
                return Object::NullVal;
            }
            byte = Object::toNumber(argv[i]);
            self->append(byte);
        }
        return self->asValue();
    }

    static Value objfn_string_less(VM* vm, Value instance, size_t argc, Value* argv)
    {
        return BOOL_VALUE(strcmp(Object::as<String>(instance)->data(), lit_check_string(vm, argv, argc, 0)) < 0);
    }

    static Value objfn_string_greater(VM* vm, Value instance, size_t argc, Value* argv)
    {
        return BOOL_VALUE(strcmp(Object::as<String>(instance)->data(), lit_check_string(vm, argv, argc, 0)) > 0);
    }

    static Value objfn_string_tostring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return instance;
    }

    static Value objfn_string_tonumber(VM* vm, Value instance, size_t argc, Value* argv)
    {
        double result;
        (void)vm;
        (void)argc;
        (void)argv;
        result = strtod(Object::as<String>(instance)->data(), nullptr);
        if(errno == ERANGE)
        {
            errno = 0;
            return Object::NullVal;
        }
        return Object::toValue(result);
    }

    static Value objfn_string_touppercase(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        char* buffer;
        const char* sd;
        String* rt;
        String* string;
        string = Object::as<String>(instance);
        (void)argc;
        (void)argv;
        buffer = LIT_ALLOCATE(vm->m_state, char, string->length() + 1);
        sd = string->data();
        for(i = 0; i < string->length(); i++)
        {
            buffer[i] = (char)toupper(sd[i]);
        }
        buffer[i] = 0;
        rt = String::take(vm->m_state, buffer, string->length());
        return rt->asValue();
    }

    static Value objfn_string_tolowercase(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        char* buffer;
        const char* sd;
        String* rt;
        String* string;
        string = Object::as<String>(instance);
        (void)argc;
        (void)argv;
        buffer = LIT_ALLOCATE(vm->m_state, char, string->length() + 1);
        sd = string->data();
        for(i = 0; i < string->length(); i++)
        {
            buffer[i] = (char)tolower(sd[i]);
        }
        buffer[i] = 0;
        rt = String::take(vm->m_state, buffer, string->length());
        return rt->asValue();
    }

    static Value objfn_string_tobyte(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int iv;
        String* selfstr;
        (void)vm;
        (void)argc;
        (void)argv;
        selfstr = Object::as<String>(instance);
        iv = String::utfstringDecode((const uint8_t*)selfstr->data(), selfstr->length());
        return Object::toValue(iv);
    }

    static Value objfn_string_bytes(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int iv;
        size_t i;
        Array* arr;
        String* selfstr;
        (void)vm;
        (void)argc;
        (void)argv;
        selfstr = Object::as<String>(instance);
        arr = Array::make(vm->m_state);
        for(i=0; i<selfstr->size(); i++)
        {
            arr->push(Object::toValue(selfstr->at(i)));
        }
        return arr->asValue();
    }

    static Value objfn_string_byteat(VM* vm, Value instance, size_t argc, Value* argv)
    {
        char ch;
        size_t pos;
        size_t maxlen;
        String* self;
        self = Object::as<String>(instance);
        pos = lit_check_number(vm, argv, argc, 0);
        maxlen = self->size();
        if(pos >= maxlen)
        {
            return Object::NullVal;
        }
        return Object::toValue(self->at(pos));
    }

    static Value objfn_string_split(VM* vm, Value instance, size_t argc, Value* argv)
    {
        double result;
        (void)vm;
        (void)argc;
        (void)argv;
        String* self;
        String* sep;
        Array* rt;
        sep = String::intern(vm->m_state, "");
        self = Object::as<String>(instance);
        if(argc > 0)
        {
            if(!Object::isString(argv[0]))
            {
                lit_runtime_error_exiting(vm, "split() expects a string argument");
            }
            sep = Object::as<String>(argv[0]);
        }
        rt = self->split(sep, true);
        return rt->asValue();
    }

    static Value objfn_string_contains(VM* vm, Value instance, size_t argc, Value* argv)
    {
        bool icase;
        String* sub;
        String* selfstr;
        (void)vm;
        (void)argc;
        (void)argv;
        icase = false;
        selfstr = Object::as<String>(instance);
        sub = lit_check_object_string(vm, argv, argc, 0);
        if(argc > 1)
        {
            icase = lit_check_bool(vm, argv, argc, 1);
        }
        if(selfstr->contains(sub, icase))
        {
            return Object::TrueVal;
        }
        return Object::FalseVal;
    }

    static Value objfn_string_startswith(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        String* sub;
        String* string;
        string = Object::as<String>(instance);
        sub = lit_check_object_string(vm, argv, argc, 0);
        if(sub == string)
        {
            return Object::TrueVal;
        }
        if(sub->length() > string->length())
        {
            return Object::FalseVal;
        }
        for(i = 0; i < sub->length(); i++)
        {
            if(sub->at(i) != string->at(i))
            {
                return Object::FalseVal;
            }
        }
        return Object::TrueVal;
    }

    static Value objfn_string_endswith(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        size_t start;
        String* sub;
        String* string;
        string = Object::as<String>(instance);
        sub = lit_check_object_string(vm, argv, argc, 0);
        if(sub == string)
        {
            return Object::TrueVal;
        }
        if(sub->length() > string->length())
        {
            return Object::FalseVal;
        }
        start = string->length() - sub->length();
        for(i = 0; i < sub->length(); i++)
        {
            if(sub->at(i) != string->at(i + start))
            {
                return Object::FalseVal;
            }
        }
        return Object::TrueVal;
    }


    static Value objfn_string_replace(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t i;
        size_t buffer_length;
        size_t buffer_index;
        char* buffer;
        String* string;
        String* what;
        String* with;
        LIT_ENSURE_ARGS(2);
        if(!Object::isString(argv[0]) || !Object::isString(argv[1]))
        {
            lit_runtime_error_exiting(vm, "expected 2 string arguments");
        }
        string = Object::as<String>(instance);
        what = Object::as<String>(argv[0]);
        with = Object::as<String>(argv[1]);
        buffer_length = 0;
        for(i = 0; i < string->length(); i++)
        {
            if(strncmp(string->data() + i, what->data(), what->length()) == 0)
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
        buffer = LIT_ALLOCATE(vm->m_state, char, buffer_length+1);
        for(i = 0; i < string->length(); i++)
        {
            if(strncmp(string->data() + i, what->data(), what->length()) == 0)
            {
                memcpy(buffer + buffer_index, with->data(), with->length());
                buffer_index += with->length();
                i += what->length() - 1;
            }
            else
            {
                buffer[buffer_index] = string->at(i);
                buffer_index++;
            }
        }
        buffer[buffer_length] = '\0';
        return String::take(vm->m_state, buffer, buffer_length)->asValue();
    }

    static Value objfn_string_substring(VM* vm, Value instance, size_t argc, Value* argv)
    {
        size_t to;
        size_t from;
        String* self;
        self = Object::as<String>(instance);
        to = self->size();
        from = lit_check_number(vm, argv, argc, 0);
        if(argc > 1)
        {
            to = lit_check_number(vm, argv, argc, 1);
        }
        return objfn_string_splice(vm, self, from, to);
    }


    static Value objfn_string_length(VM* vm, Value instance, size_t argc, Value* argv)
    {
        (void)vm;
        (void)argc;
        (void)argv;
        return Object::toValue(Object::as<String>(instance)->utfLength());
    }

    static Value objfn_string_iterator(VM* vm, Value instance, size_t argc, Value* argv)
    {
        int index;
        String* string;
        string = Object::as<String>(instance);
        if(Object::isNull(argv[0]))
        {
            if(string->length() == 0)
            {
                return Object::NullVal;
            }
            return Object::toValue(0);
        }
        index = lit_check_number(vm, argv, argc, 0);
        if(index < 0)
        {
            return Object::NullVal;
        }
        do
        {
            index++;
            if(index >= (int)string->length())
            {
                return Object::NullVal;
            }
        } while((string->at(index) & 0xc0) == 0x80);
        return Object::toValue(index);
    }


    static Value objfn_string_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
    {
        uint32_t index;
        String* string;
        string = Object::as<String>(instance);
        index = lit_check_number(vm, argv, argc, 0);
        if(index == UINT32_MAX)
        {
            return false;
        }
        return string->codePointAt(index)->asValue();
    }


    bool check_fmt_arg(VM* vm, String* buf, size_t ai, size_t argc, Value* argv, const char* fmttext)
    {
        (void)argv;
        if(ai <= argc)
        {
            return true;
        }
        //sdsfree(buf);
        lit_runtime_error_exiting(vm, "too few arguments for format flag '%s' at position %d (argc=%d)", fmttext, ai, argc);
        return false;
    }

    static Value objfn_string_format(VM* vm, Value instance, size_t argc, Value* argv)
    {
        char ch;
        char pch;
        char nch;
        char tmpch;
        int iv;
        size_t i;
        size_t ai;
        size_t selflen;
        bool was_allowed;
        String* selfstr;
        Value rtv;
        //char* buf;
        String* buf;
        (void)pch;
        was_allowed = String::stateGetGCAllowed(vm->m_state);
        String::stateSetGCAllowed(vm->m_state, false);
        selfstr = Object::as<String>(instance);
        selflen = selfstr->length();
        //buf = sdsempty();
        //buf = sdsMakeRoomFor(buf, selflen + 10);
        buf = String::make(vm->m_state);
        ai = 0;
        ch = -1;
        pch = -1;
        nch = -1;
        for(i=0; i<selflen; i++)
        {
            pch = ch;
            ch = selfstr->at(i);
            if(i <= selflen)
            {
                nch = selfstr->at(i + 1);
            }
            if(ch == '%')
            {
                if(nch == '%')
                {
                    //buf = sdscatlen(buf, &ch, 1);
                    buf->append(ch);
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
                                    return Object::NullVal;
                                }
                                //buf = sdscatlen(buf, Object::as<String>(argv[ai])->data(), Object::as<String>(argv[ai])->length());
                                if(Object::isString(argv[ai]))
                                {
                                    buf->append(Object::as<String>(argv[ai]));
                                }
                                else
                                {
                                    buf->append(Object::toString(vm->m_state, argv[ai]));
                                    //lit_runtime_error_exiting(vm, "format 's' at position %d expected a string", ai);
                                }
                            }
                            break;
                        case 'd':
                        case 'i':
                            {
                                if(!check_fmt_arg(vm, buf, ai, argc, argv, "%d"))
                                {
                                    return Object::NullVal;
                                }
                                if(Object::isNumber(argv[ai]))
                                {
                                    iv = lit_check_number(vm, argv, argc, ai);
                                    //buf = sdscatfmt(buf, "%i", iv);
                                    buf->append(std::to_string(iv));
                                }
                                break;
                            }
                            break;
                        case 'c':
                            {
                                if(!check_fmt_arg(vm, buf, ai, argc, argv, "%c"))
                                {
                                    return Object::NullVal;
                                }
                                if(!Object::isNumber(argv[ai]))
                                {
                                    lit_runtime_error_exiting(vm, "flag 'c' expects a number");
                                }
                                iv = lit_check_number(vm, argv, argc, ai);
                                /* TODO: both of these use the same amount of memory. but which is faster? */
                                tmpch = iv;
                                buf->append(tmpch);
                            }
                            break;
                        case 'x':
                            {
                                size_t rlen;
                                char tmpbuf[25];
                                if(!check_fmt_arg(vm, buf, ai, argc, argv, "%c"))
                                {
                                    return Object::NullVal;
                                }
                                if(!Object::isNumber(argv[ai]))
                                {
                                    lit_runtime_error_exiting(vm, "flag 'x' expects a number");
                                }
                                iv = lit_check_number(vm, argv, argc, ai);
                                memset(tmpbuf, 0, 25);
                                rlen = sprintf(tmpbuf, "%02x", iv);
                                buf->append(tmpbuf, rlen);
                            }
                            break;
                        default:
                            {
                                //sdsfree(buf);
                                lit_runtime_error_exiting(vm, "unrecognized formatting flag '%c'", nch);
                                return Object::NullVal;
                            }
                            break;
                    }
                    ai++;
                }
            }
            else
            {
                //buf = sdscatlen(buf, &ch, 1);
                buf->append(ch);
            }
        }
        //rtv = String::copy(vm->m_state, buf, sdslen(buf))->asValue();
        //sdsfree(buf);
        //return rtv;
        String::stateSetGCAllowed(vm->m_state, was_allowed);

        return buf->asValue();
    }

    void lit_open_string_library(State* state)
    {
        {
            Class* klass = Class::make(state, "String");
            
                klass->inheritFrom(state->objectvalue_class);
                klass->bindConstructor(util_invalid_constructor);
                klass->bindMethod("+", objfn_string_plus);
                klass->bindMethod("[]", objfn_string_subscript);
                klass->bindMethod("<", objfn_string_less);
                klass->bindMethod(">", objfn_string_greater);
                klass->bindMethod("==", objfn_string_compare);
                klass->bindMethod("append", objfn_string_append);
                // this is technically redundant due to Number.chr, but lets add it anyway.
                klass->bindMethod("appendByte", objfn_string_appendbyte);
                klass->bindMethod("toString", objfn_string_tostring);
                {
                    klass->bindMethod("toNumber", objfn_string_tonumber);
                    klass->setGetter("to_i", objfn_string_tonumber);
                }
                /*
                *
                */
                {
                    klass->bindMethod("split", objfn_string_split);
                }
                /*
                * javascript shit (does what [] does)
                */
                {
                    klass->bindMethod("charCodeAt", objfn_string_byteat);
                }
                /*
                * String.toUpper()
                * turns string to uppercase.
                */
                {
                    klass->bindMethod("toUpperCase", objfn_string_touppercase);
                    klass->bindMethod("toUpper", objfn_string_touppercase);
                    klass->setGetter("upper", objfn_string_touppercase);
                }
                /*
                * String.toLower()
                * turns string to lowercase.
                */
                {
                    klass->bindMethod("toLowerCase", objfn_string_tolowercase);
                    klass->bindMethod("toLower", objfn_string_tolowercase);
                    klass->setGetter("lower", objfn_string_tolowercase);
                }
                /*
                * String.toByte
                * turns string into a byte integer.
                * if the string is multi character string, then $self[0] is used.
                * i.e., "foo".toByte == 102 (equiv: "foo"[0].toByte)
                */
                {
                    klass->setGetter("toByte", objfn_string_tobyte);
                    klass->bindMethod("byteAt", objfn_string_byteat);
                }
                //TODO: byteAt
                //LIT_BIND_METHOD(state, klass, "byteAt", objfn_string_byteat);
                /*
                * String.bytes
                * returns the string as an array of bytes
                */
                {
                    klass->setGetter("bytes", objfn_string_bytes);
                }
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
                klass->setGetter("length", objfn_string_length);
                klass->bindMethod("format", objfn_string_format);
                state->stringvalue_class = klass;
            
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }

    }
}


//endlit

// Used for clean up on Ctrl+C / Ctrl+Z
static lit::State* repl_state;

void interupt_handler(int signal_id)
{
    (void)signal_id;
    repl_state->release();
    printf("\nExiting.\n");
    exit(0);
}

static int run_repl(lit::State* state)
{
    //fprintf(stderr, "in repl...\n");
    #if defined(LIT_HAVE_READLINE)
    char* line;
    repl_state = state;
    signal(SIGINT, interupt_handler);
    //signal(SIGTSTP, interupt_handler);
    lit::AST::Optimizer::set_level(lit::LITOPTLEVEL_REPL);
    printf("lit v%s, developed by @egordorichev\n", LIT_VERSION_STRING);
    while(true)
    {
        line = readline("> ");
        if(line == nullptr)
        {
            return 0;
        }
        add_history(line);
        lit::Result result = state->interpretSource("repl", line, strlen(line));
        if(result.type == lit::LITRESULT_OK && result.result != lit::Object::NullVal)
        {
            printf("%s%s%s\n", COLOR_GREEN, lit::Object::toString(state, result.result)->data(), COLOR_RESET);
        }
    }
    #endif
    return 0;
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
    for(i = 0; i < lit::LITOPTSTATE_TOTAL; i++)
    {
        printf(" %s  %s\n", lit::AST::Optimizer::get_optimization_name((lit::Optimization)i),
               lit::AST::Optimizer::get_optimization_description((lit::Optimization)i));
    }
    printf("\nIf you want to use a predefined optimization level (recommended), run lit with argument -O[optimization level], for example -O1.\n\n");
    for(i = 0; i < lit::LITOPTLEVEL_TOTAL; i++)
    {
        printf("\t-O%i\t\t%s\n", i, lit::AST::Optimizer::get_optimization_level_description((lit::OptimizationLevel)i));
    }
}

static bool match_arg(const char* arg, const char* a, const char* b)
{
    return strcmp(arg, a) == 0 || strcmp(arg, b) == 0;
}

int exitstate(lit::State* state, lit::InterpretResultType result)
{
    int64_t amount;

    amount = state->release();
    if(result != lit::LITRESULT_COMPILE_ERROR && amount != 0)
    {
        fprintf(stderr, "gc: freed residual %i bytes\n", (int)amount);
        return LIT_EXIT_CODE_MEM_LEAK;
    }
    if(result != lit::LITRESULT_OK)
    {
        return result == lit::LITRESULT_RUNTIME_ERROR ? LIT_EXIT_CODE_RUNTIME_ERROR : LIT_EXIT_CODE_COMPILE_ERROR;
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
    lit::Module* module;
    lit::InterpretResultType result;
    lit::Array* arg_array;
    lit::State* state;
    state = lit::State::make();
    lit_open_libraries(state);
    num_files_to_run = 0;
    result = lit::LITRESULT_OK;
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
                    lit::AST::Optimizer::set_level((lit::OptimizationLevel)(c - '0'));
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
                lit::AST::Optimizer::set_all_enabled(enable_optimization);
            }
            else
            {
                found = false;
                // Yes I know, this is not the fastest way, and what now?
                for(j = 0; j < lit::LITOPTSTATE_TOTAL; j++)
                {
                    if(strcmp(lit::AST::Optimizer::get_optimization_name((lit::Optimization)j), optimization_name) == 0)
                    {
                        found = true;
                        lit::AST::Optimizer::set_enabled((lit::Optimization)j, enable_optimization);

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
                module = state->compileModule(lit::String::intern(state, module_name), source, length);
                if(module == nullptr)
                {
                    break;
                }
                lit_disassemble_module(state, module, source);
                free(source);
            }
            else
            {
                result = state->interpretSource(module_name, source, length).type;
                free(source);
                if(result != lit::LITRESULT_OK)
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
        else if(match_arg(arg, "-i", "--interactive"))
        {
            show_repl = true;
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
            lit::AST::Optimizer::set_level(lit::LITOPTLEVEL_EXTREME);
        }
        else if(match_arg(arg, "-p", "--pass"))
        {
            arg_array = lit::Array::make(state);

            for(j = 0; j < (size_t)args_left; j++)
            {
                arg_string = argv[i + j + 1];
                arg_array->push(lit::String::internValue(state, arg_string));
            }

            state->setGlobal(lit::String::intern(state, "args"), arg_array->asValue());
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
                result = lit::LITRESULT_COMPILE_ERROR;
            }
        }
        else
        {
            if(arg_array == nullptr)
            {
                arg_array = lit::Array::make(state);
            }
            state->setGlobal(lit::String::intern(state, "args"), arg_array->asValue());
            for(i = 0; i < num_files_to_run; i++)
            {
                file = files_to_run[i];
                result = (dump ? lit_dump_file(state, file) : state->interpretFile(file)).type;
                if(result != lit::LITRESULT_OK)
                {
                    return exitstate(state, result);
                }
            }
        }
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
