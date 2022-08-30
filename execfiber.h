
    //#define LIT_TRACE_EXECUTION

    /*
    * visual studio doesn't support computed gotos, so
    * instead a switch-case is used. 
    */
    #if !defined(_MSC_VER)
        #define LIT_USE_COMPUTEDGOTO
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
        return Result{ LITRESULT_RUNTIME_ERROR, Object::NullVal };

    #define vm_recoverstate(fiber, frame, ip, current_chunk, slots, privates, upvalues) \
        vm_writeframe(frame, ip); \
        fiber = vm->fiber; \
        if(fiber == nullptr) \
        { \
            return Result{ LITRESULT_OK, vm_pop(fiber) }; \
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

    #define vm_invokemethod(opname, instance, method_name, arg_count) \
        Class* klass = Class::getClassFor(this, instance); \
        if(klass == nullptr) \
        { \
            vm_rterrorvarg("invokemethod(%s -> %s): cannot get class object for a '%s'", opname, method_name, Object::valueName(instance)); \
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
            vm_invokemethod("vm_binaryop", a, op_string, 1); \
        }

    /*
    * for operators where both float and fixed work (+, -, etc)
    */
    #define vm_bitwiseop_float(op, op_string) \
        Value a = vm_peek(fiber, 1); \
        Value b = vm_peek(fiber, 0); \
        if(!Object::isNumber(a) || !Object::isNumber(b)) \
        { \
            vm_rterrorvarg("Operands of bitwise op %s must be two numbers, got %s and %s", op_string, \
                               Object::valueName(a), Object::valueName(b)); \
        } \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = (Object::toValue(Object::toNumber(a) op Object::toNumber(b)));

    /*
    * for operators that require fixed numbers
    */
    #define vm_bitwiseop_fixed(op, op_string) \
        Value a = vm_peek(fiber, 1); \
        Value b = vm_peek(fiber, 0); \
        if(!Object::isNumber(a) || !Object::isNumber(b)) \
        { \
            vm_rterrorvarg("Operands of bitwise op %s must be two numbers, got %s and %s", op_string, \
                               Object::valueName(a), Object::valueName(b)); \
        } \
        vm_drop(fiber); \
        *(fiber->stack_top - 1) = (Object::toValue(int64_t(Object::toNumber(a)) op int64_t(Object::toNumber(b))));

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
                        return Result{ LITRESULT_OK, result };
                    }
                    if(fiber->frame_count == 0)
                    {
                        fiber->module->return_value = result;
                        if(fiber->parent == nullptr)
                        {
                            vm_drop(fiber);
                            this->allow_gc = was_allowed;
                            return Result{ LITRESULT_OK, result };
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
                    tmpval = Object::fromBool(Object::isFalsey(vm_pop(fiber)));
                    vm_push(fiber, tmpval);
                    continue;
                }
                op_case(BNOT)
                {
                    if(!Object::isNumber(vm_peek(fiber, 0)))
                    {
                        vm_rterror("Operand must be a number");
                    }
                    auto num = Object::toNumber(vm_pop(fiber));
                    tmpval = Object::toValue(~int64_t(num));
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
                // todo: this is broken, methinks
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
                    vm_invokemethod("POWER", a, "**", 1);
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

                    vm_invokemethod("FLOOR_DIVIDE", a, "#", 1);
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
                    vm_invokemethod("MOD", a, "%", 1);
                    continue;
                }
                op_case(BAND)
                {
                    vm_bitwiseop_fixed(&, "&");
                    continue;
                }
                op_case(BOR)
                {
                    vm_bitwiseop_fixed(|, "|");
                    continue;
                }
                op_case(BXOR)
                {
                    vm_bitwiseop_fixed(^, "^");
                    continue;
                }
                op_case(LSHIFT)
                {
                    vm_bitwiseop_fixed(<<, "<<");
                    continue;
                }
                op_case(RSHIFT)
                {
                    vm_bitwiseop_fixed(>>, ">>");
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
                    vm_push(fiber, Object::fromBool(a == b));
                    */
                    vm_binaryop(Object::toValue, ==, "==");
                    continue;
                }

                op_case(GREATER)
                {
                    vm_binaryop(Object::fromBool, >, ">");
                    continue;
                }
                op_case(GREATER_EQUAL)
                {
                    vm_binaryop(Object::fromBool, >=, ">=");
                    continue;
                }
                op_case(LESS)
                {
                    vm_binaryop(Object::fromBool, <, "<");
                    continue;
                }
                op_case(LESS_EQUAL)
                {
                    vm_binaryop(Object::fromBool, <=, "<=");
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
                    Value tmp = vm_peek(fiber, 0);
                    if(Object::isNull(tmp) || Object::isFalsey(tmp))
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
                    Value tmp = vm_peek(fiber, 1);
                    vm_invokemethod("SUBSCRIPT_GET", tmp, "[]", 1);
                    continue;
                }
                op_case(SUBSCRIPT_SET)
                {
                    Value tmp = vm_peek(fiber, 2);
                    vm_invokemethod("SUBSCRIPT_SET", tmp, "[]", 2);
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
                    vm_push(fiber, Object::fromBool(found));
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
                    /*
                    if(vm->globals->m_values.getSlot(name, &pval))
                    {
                        vm_push(fiber, Reference::make(this, pval)->asValue());
                    }
                    else
                    */
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
                        //if(!Object::as<Instance>(vobj)->fields.getSlot(name, &pval))
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

