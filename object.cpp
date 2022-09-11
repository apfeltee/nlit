
#include "priv.h"

namespace lit
{
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
                    LIT_FREE_ARRAY(state, Fiber::CallFrame, fiber->m_allframes, fiber->m_framecapacity);
                    LIT_FREE_ARRAY(state, Value, fiber->m_stackdata, fiber->m_stackcapacity);
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

    bool Object::compare(State* state, const Value a, const Value b)
    {
        Result inret;
        Value args[3];
        if(Object::isInstance(a))
        {
            args[0] = b;
            inret = Instance::callMethod(state, a, String::intern(state, "=="), args, 1);
            if(inret.type == LITRESULT_OK)
            {
                if(Object::fromBool(inret.result) == Object::TrueVal)
                {
                    return true;
                }
                return false;
            }
        }
        return (a == b);
    }

    State* Object::asState(VM* vm)
    {
        return vm->m_state;
    }

    String* Object::functionName(VM* vm, Value instance)
    {
        String* name;
        Field* field;
        name = nullptr;
        switch(Object::asObject(instance)->type)
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
            return String::format(vm->m_state, "[function #]", *((double*)Object::asObject(instance)));
        }
        return String::format(vm->m_state, "[function $]", name->data());
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
            function = state->api_function = Function::make(state, fiber->m_module);
            function->chunk.m_haslineinfo = false;
            function->name = state->api_name;
            chunk = &function->chunk;
            chunk->m_count = 0;
            chunk->m_constants.m_count = 0;
            function->max_slots = 3;
            chunk->putChunk(OP_INVOKE, 1);
            chunk->emit_byte(0);
            chunk->emit_short(chunk->addConstant(String::internValue(state, "toString")));
            chunk->emit_byte(OP_RETURN);
        }
        fiber->ensure_stack(function->max_slots + (int)(fiber->m_stacktop - fiber->m_stackdata));
        frame = &fiber->m_allframes[fiber->m_framecount++];
        frame->ip = function->chunk.m_code;
        frame->closure = nullptr;
        frame->function = function;
        frame->slots = fiber->m_stacktop;
        frame->result_ignored = false;
        frame->return_to_c = true;
        fiber->push(function->asValue());
        fiber->push(valobj);
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
                entry = map->at(i);
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
                        wr->format("[instance '%s' ", Object::as<Instance>(value)->klass->name->data());
                        map = Object::as<Map>(value);
                        size = map->size();
                        printMap(state, wr, map, size);
                        wr->put("]");
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

    Function* Function::make(State* state, Module* mod)
    {
        Function* function;
        function = Object::make<Function>(state, Object::Type::Function);
        function->chunk.init(state);
        function->name = nullptr;
        function->arg_count = 0;
        function->upvalue_count = 0;
        function->max_slots = 0;
        function->module = mod;
        function->vararg = false;
        return function;
    }

    Upvalue* Upvalue::make(State* state, Value* slot)
    {
        Upvalue* upvalue;
        upvalue = Object::make<Upvalue>(state, Object::Type::Upvalue);
        upvalue->location = slot;
        upvalue->closed = Object::NullVal;
        upvalue->next = nullptr;
        return upvalue;
    }

    NativeFunction* NativeFunction::make(State* state, NativeFunction::FuncType function, String* name)
    {
        NativeFunction* native;
        native = Object::make<NativeFunction>(state, Object::Type::NativeFunction);
        native->function = function;
        native->name = name;
        return native;
    }

    NativePrimFunction* NativePrimFunction::make(State* state, NativePrimFunction::FuncType function, String* name)
    {
        NativePrimFunction* native;
        native = Object::make<NativePrimFunction>(state, Object::Type::NativePrimitive);
        native->function = function;
        native->name = name;
        return native;
    }

    NativeMethod* NativeMethod::make(State* state, FuncType method, String* name)
    {
        NativeMethod* native;
        native = Object::make<NativeMethod>(state, Object::Type::NativeMethod);
        native->method = method;
        native->name = name;
        return native;
    }

    NativeMethod* NativeMethod::make(State* state, FuncType method, std::string_view name)
    {
        return NativeMethod::make(state, method, String::copy(state, name.data(), name.size()));
    }

    PrimitiveMethod* PrimitiveMethod::make(State* state, FuncType method, String* name)
    {
        PrimitiveMethod* native;
        native = Object::make<PrimitiveMethod>(state, Object::Type::PrimitiveMethod);
        native->method = method;
        native->name = name;
        return native;
    }

    PrimitiveMethod* PrimitiveMethod::make(State* state, FuncType method, std::string_view sv)
    {
        return make(state, method, String::copy(state, sv.data(), sv.size()));
    }

}

