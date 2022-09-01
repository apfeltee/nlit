
#include "lit.h"
#include "priv.h"

namespace lit
{
    void VM::markRoots()
    {
        size_t i;
        State* state;
        state = m_state;
        for(i = 0; i < state->root_count; i++)
        {
            this->markValue(state->roots[i]);
        }
        this->markObject((Object*)this->fiber);
        this->markObject((Object*)state->kernelvalue_class);
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


    void VM::closeUpvalues(const Value* last)
    {
        Fiber* fiber;
        Upvalue* upvalue;
        fiber = this->fiber;
        while(fiber->m_openupvalues != nullptr && fiber->m_openupvalues->location >= last)
        {
            upvalue = fiber->m_openupvalues;
            upvalue->closed = *upvalue->location;
            upvalue->location = &upvalue->closed;
            fiber->m_openupvalues = upvalue->next;
        }
    }

    bool VM::dispatchCall(Function* function, Closure* closure, uint8_t arg_count)
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
        //if(fiber->m_framecount == LIT_CALL_FRAMES_MAX)
        //{
            //lit_runtime_error(this, "call stack overflow");
            //return true;
        //}
        #endif
        if(fiber->m_framecount + 1 > fiber->m_framecapacity)
        {
            //newcapacity = fmin(LIT_CALL_FRAMES_MAX, fiber->m_framecapacity * 2);
            newcapacity = (fiber->m_framecapacity * 2);
            newsize = (sizeof(Fiber::CallFrame) * newcapacity);
            osize = (sizeof(Fiber::CallFrame) * fiber->m_framecapacity);
            fiber->m_allframes = (Fiber::CallFrame*)Memory::reallocate(m_state, fiber->m_allframes, osize, newsize);
            fiber->m_framecapacity = newcapacity;
        }

        function_arg_count = function->arg_count;
        fiber->ensure_stack(function->max_slots + (int)(fiber->m_stacktop - fiber->m_stackdata));
        frame = &fiber->m_allframes[fiber->m_framecount++];
        frame->function = function;
        frame->closure = closure;
        frame->ip = function->chunk.m_code;
        frame->slots = fiber->m_stacktop - arg_count - 1;
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
                    this->push(Array::make(m_state)->asValue());
                }
            }
            else if(function->vararg)
            {
                array = Array::make(m_state);
                vararg_count = arg_count - function_arg_count + 1;
                m_state->pushRoot((Object*)array);
                array->m_actualarray.reserve(vararg_count, Object::NullVal);
                m_state->popRoot();
                for(i = 0; i < vararg_count; i++)
                {
                    array->m_actualarray.m_values[i] = this->fiber->m_stacktop[(int)i - (int)vararg_count];
                }
                this->fiber->m_stacktop -= vararg_count;
                this->push(array->asValue());
            }
            else
            {
                this->fiber->m_stacktop -= (arg_count - function_arg_count);
            }
        }
        else if(function->vararg)
        {
            array = Array::make(m_state);
            vararg_count = arg_count - function_arg_count + 1;
            m_state->pushRoot((Object*)array);
            array->push(*(fiber->m_stacktop - 1));
            *(fiber->m_stacktop - 1) = array->asValue();
            m_state->popRoot();
        }
        return true;
    }

    void VM::markObject(Object* obj)
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

    void VM::markValue(Value value)
    {
        if(Object::isObject(value))
        {
            markObject(Object::asObject(value));
        }
    }

    void VM::markArray(PCGenericArray<Value>* array)
    {
        size_t i;
        for(i = 0; i < array->m_count; i++)
        {
            this->markValue(array->m_values[i]);
        }
    }

    void VM::blackenObject(Object* obj)
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
                        data->cleanup_fn(m_state, data, true);
                    }
                }
                break;
            case Object::Type::Function:
                {
                    function = (Function*)obj;
                    this->markObject((Object*)function->name);
                    this->markArray(&function->chunk.m_constants);
                }
                break;
            case Object::Type::Fiber:
                {
                    fiber = (Fiber*)obj;
                    for(Value* slot = fiber->m_stackdata; slot < fiber->m_stacktop; slot++)
                    {
                        this->markValue(*slot);
                    }
                    for(i = 0; i < fiber->m_framecount; i++)
                    {
                        frame = &fiber->m_allframes[i];
                        if(frame->closure != nullptr)
                        {
                            this->markObject((Object*)frame->closure);
                        }
                        else
                        {
                            this->markObject((Object*)frame->function);
                        }
                    }
                    for(upvalue = fiber->m_openupvalues; upvalue != nullptr; upvalue = upvalue->next)
                    {
                        this->markObject((Object*)upvalue);
                    }
                    this->markValue(fiber->m_error);
                    this->markObject((Object*)fiber->m_module);
                    this->markObject((Object*)fiber->m_parent);
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

    void VM::traceReferences()
    {
        Object* obj;
        while(this->gray_count > 0)
        {
            obj = this->gray_stack[--this->gray_count];
            this->blackenObject(obj);
        }
    }

    void VM::sweep()
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
                Object::releaseObject(m_state, unreached);
            }
        }
    }

    uint64_t VM::collectGarbage()
    {
        clock_t t;
        uint64_t before;
        uint64_t collected;
        (void)t;
        if(!m_state->allow_gc)
        {
            return 0;
        }

        m_state->allow_gc = false;
        before = m_state->bytes_allocated;

    #ifdef LIT_LOG_GC
        printf("-- gc begin\n");
        t = clock();
    #endif

        markRoots();
        this->traceReferences();
        this->strings.removeWhite();
        this->sweep();
        m_state->next_gc = m_state->bytes_allocated * LIT_GC_HEAP_GROW_FACTOR;
        m_state->allow_gc = true;
        collected = before - m_state->bytes_allocated;

    #ifdef LIT_LOG_GC
        printf("-- gc end. Collected %imb in %gms\n", ((int)((collected / 1024.0 + 0.5) / 10)) * 10,
               (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
    #endif
        return collected;
    }
}

