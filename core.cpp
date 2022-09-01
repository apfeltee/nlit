
#include <sys/stat.h>
#include "lit.h"
#include "priv.h"

namespace lit
{

    // typimpls 


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
        if(!Object::isObject(value) || Object::asObject(value)->type != type)
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
        Object::print(state, wr, chunk->m_constants.m_values[constant]);
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
        Object::print(state, wr, chunk->m_constants.m_values[constant]);
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
        same = !chunk->m_haslineinfo || (offset > 0 && line == chunk->get_line(offset - 1));
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
                    Object::print(state, wr, chunk->m_constants.m_values[constant]);
                    wr->format("\n");
                    function = Object::as<Function>(chunk->m_constants.m_values[constant]);
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
        values = &chunk->m_constants;

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
        frame = &fiber->m_allframes[fiber->m_framecount - 1];
        wr->format("== fiber %p f%i %s (expects %i, max %i, added %i, current %i, exits %i) ==\n", fiber,
               fiber->m_framecount - 1, frame->function->name->data(), frame->function->arg_count, frame->function->max_slots,
               frame->function->max_slots + (int)(fiber->m_stacktop - fiber->m_stackdata), fiber->m_stackcapacity, frame->return_to_c);
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
        FileIO::binwrite_uint16_t(file, LIT_BYTECODE_MAGIC_NUMBER);
        FileIO::binwrite_uint8_t(file, LIT_BYTECODE_VERSION);
        FileIO::binwrite_uint16_t(file, num_files);
        for(i = 0; i < num_files; i++)
        {
            BinaryData::storeModule(compiled_modules[i], file);
        }
        FileIO::binwrite_uint16_t(file, LIT_BYTECODE_END_NUMBER);
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
            return Result{LITRESULT_INVALID, Object::NullVal};
        }
        module_name = String::copy(state, patched_file_name, strlen(patched_file_name));
        module = state->compileModule(module_name, source, length);
        if(module == nullptr)
        {
            result = Result{LITRESULT_INVALID, Object::NullVal};
        }
        else
        {
            lit_disassemble_module(state, module, source);
            result = Result{ LITRESULT_OK, Object::NullVal };
        }
        free((void*)source);
        free((void*)patched_file_name);
        return result;
    }

    void lit_trace_vm_stack(VM* vm, Writer* wr)
    {
        Value* top;
        Value* slot;
        Fiber* fiber;
        fiber = vm->fiber;
        if(fiber->m_stacktop == fiber->m_stackdata || fiber->m_framecount == 0)
        {
            return;
        }
        top = fiber->m_allframes[fiber->m_framecount - 1].slots;
        wr->format("        | %s", COLOR_GREEN);
        for(slot = fiber->m_stackdata; slot < top; slot++)
        {
            wr->format("[ ");
            Object::print(vm->m_state, wr, *slot);
            wr->format(" ]");
        }
        wr->format("%s", COLOR_RESET);
        for(slot = top; slot < fiber->m_stacktop; slot++)
        {
            wr->format("[ ");
            Object::print(vm->m_state, wr, *slot);
            wr->format(" ]");
        }
        wr->format("\n");
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

    #if defined(__unix__) || defined(__linux__)
    #endif

    namespace Builtins
    {
        void lit_open_libraries(State* state)
        {
            lit_open_math_library(state);
            lit_open_file_library(state);
            lit_open_gc_library(state);
        }
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
        return fiber->m_framecount == 0 || fiber->m_isaborting;
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
        fiber->m_parent = vm->fiber;
        fiber->m_havecatcher = catcher;
        vm->fiber = fiber;
        frame = &fiber->m_allframes[fiber->m_framecount - 1];
        if(frame->ip == frame->function->chunk.m_code)
        {
            fiber->m_argcount = argc;
            fiber->ensure_stack(frame->function->max_slots + 1 + (int)(fiber->m_stacktop - fiber->m_stackdata));
            frame->slots = fiber->m_stacktop;
            vm->push(frame->function->asValue());
            vararg = frame->function->vararg;
            objfn_function_arg_count = frame->function->arg_count;
            to = objfn_function_arg_count - (vararg ? 1 : 0);
            fiber->m_argcount = objfn_function_arg_count;
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
        fiber->m_parent = vm->fiber;
        vm->fiber = fiber;
        frame = &fiber->m_allframes[fiber->m_framecount - 1];
        if(frame->ip == frame->function->chunk.m_code)
        {
            frame->slots = fiber->m_stacktop;
            vm->push(frame->function->asValue());
        }
        return true;
    }

    namespace Builtins
    {
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
            //fprintf(stderr, "++ lit_open_function_libary()\n");
            Class* klass = Class::make(state, "Function");
            state->functionvalue_class = klass; 
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
            //fprintf(stderr, "++ lit_open_gc_libary()\n");
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
            return Object::fromBool(rand_r((unsigned int*)extract_random_data(vm->m_state, instance)) % 2);
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
                        if(map->at(i)->key != nullptr)
                        {
                            if(index == target)
                            {
                                return map->m_values.m_inner.m_values[i]->value;
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
            //fprintf(stderr, "++ lit_open_math_libary()\n");
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
            module = Object::isModule(instance) ? Object::as<Module>(instance) : vm->fiber->m_module;
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
            return vm->fiber->m_module->asValue();
        }

        static Value objfn_module_tostring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            return String::format(vm->m_state, "[module @]", Object::as<Module>(instance)->name->asValue())->asValue();
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
            //fprintf(stderr, "++ lit_open_module_libary()\n");
            Class* klass = Class::make(state, "Module");
            state->modulevalue_class = klass;
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


        /*
        String* name;
        Object* init_method;
        Table methods;
        Table static_fields;
        Class* super;
        */
        static Value objfn_kernel_getmethods(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            Map* m;
            String* key;
            Class* selfclass;
            (void)argc;
            (void)argv;
            selfclass = nullptr;
            if(Object::isClass(instance))
            {
                selfclass = Object::as<Class>(instance);
            }
            else if(Object::isInstance(instance))
            {
                selfclass = Object::as<Instance>(instance)->klass;
            }
            else
            {
                selfclass = Class::getClassFor(vm->m_state, instance);
            }
            if(selfclass == nullptr)
            {
                lit_runtime_error_exiting(vm, "__methods: cannot find class of object");
                return Object::NullVal;
            }
            m = Map::make(vm->m_state);
            for(i=0; i<selfclass->methods.size(); i++)
            {
                key = selfclass->methods.at(i)->key;
                if(key != nullptr)
                {
                    m->set(key, selfclass->methods.at(i)->value);
                }
            }
            return m->asValue();
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
                key = fromtbl->at(i)->key;
                if(key != nullptr)
                {
                    val = fromtbl->at(i)->value;
                    destmap->set(key, val);
                }
            }
        }

        static Value objfn_kernel_tomap(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            Map* map;
            Map* minst;
            Map* mclass;
            Map* mclstatics;
            Map* mclmethods;
            Class* klass;
            Instance* inst;
            klass = nullptr;
            minst = nullptr;
            inst = nullptr;
            mclass = nullptr;
            map = Map::make(vm->m_state);
            if(Object::isInstance(instance))
            {
                inst = Object::as<Instance>(instance);
            }
            if(inst != nullptr)
            {
                klass = inst->klass;
            }
            if(klass == nullptr)
            {
                klass = Class::getClassFor(vm->m_state, instance);
            }
            if(inst != nullptr)
            {
                minst = Map::make(vm->m_state);
                fillmap(vm->m_state, minst, &(inst->fields), true);
            }
            if(klass != nullptr)
            {
                mclass = Map::make(vm->m_state);
                {
                    mclstatics = Map::make(vm->m_state);
                    fillmap(vm->m_state, mclstatics, &(klass->static_fields), false);
                }
                {
                    mclmethods = Map::make(vm->m_state);
                    fillmap(vm->m_state, mclmethods, &(klass->methods), false);
                }
                mclass->set(String::intern(vm->m_state, "statics"), mclstatics->asValue());
                mclass->set(String::intern(vm->m_state, "methods"), mclmethods->asValue());
            }
            if(minst != nullptr)
            {
                map->set(String::intern(vm->m_state, "instance"), minst->asValue());
            }
            if(mclass != nullptr)
            {
                map->set(String::intern(vm->m_state, "class"), mclass->asValue());
            }
            return map->asValue();
        }

        static Value objfn_kernel_tostring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)instance;
            (void)argc;
            (void)argv;
            return String::intern(vm->m_state, "[kernel]")->asValue();
        }

        void lit_open_kernel_library(State* state)
        {
            //fprintf(stderr, "++ lit_open_kernel_libary()\n");
            Class* klass = Class::make(state, "Kernel");
            state->kernelvalue_class = klass;
            {
                klass->setStaticMethod("toString", objfn_kernel_tostring);
                klass->setGetter("__map", objfn_kernel_tomap);
                klass->setStaticGetter("__map", objfn_kernel_tomap);
                klass->setGetter("__methods", objfn_kernel_getmethods);
                klass->setStaticGetter("__methods", objfn_kernel_getmethods);
                #if 0
                klass->setGetter("__fields", objfn_kernel_getfields);
                //klass->setStaticGetter("__fields", objfn_kernel_getfields);
                #endif
            }
            state->setGlobal(klass->name, klass->asValue());
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
            return String::format(vm->m_state, "[instance @]", Class::getClassFor(vm->m_state, instance)->name->asValue())->asValue();
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
            //fprintf(stderr, "++ lit_open_object_libary()\n");
            Class* klass = Class::make(state, "Object");
            {
                klass->inheritFrom(state->classvalue_class);
                klass->setGetter("class", objfn_object_class);
                klass->setGetter("super", objfn_object_super);
                klass->bindMethod("[]", objfn_object_subscript);
                klass->bindMethod("toString", objfn_object_tostring);
                klass->bindMethod("iterator", objfn_object_iterator);
                klass->bindMethod("iteratorValue", objfn_object_iteratorvalue);
            }
            state->objectvalue_class = klass;
            state->objectvalue_class->super = state->classvalue_class;
            state->setGlobal(klass->name, klass->asValue());
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
            return String::format(vm->m_state, "[range (#, #)]", range->from, range->to)->asValue();
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
            //fprintf(stderr, "++ lit_open_range_libary()\n");
            Class* klass = Class::make(state, "Range");
            state->rangevalue_class = klass;
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

    }
}
