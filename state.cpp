
#include "lit.h"
#include "priv.h"

namespace lit
{
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
        state->kernelvalue_class = nullptr;
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
        Builtins::lit_open_core_library(state);
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
            total_t = t = clock();
            /*
            if(!lit_preprocess(this->preprocessor, code))
            {
                return nullptr;
            }
            1*/
            //printf("-----------------------\nPreprocessing:  %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            //t = clock();
            statements.init(this);
            if(this->parser->parse(module_name->data(), code, length, statements))
            {
                AST::Expression::releaseStatementList(this, &statements);
                return nullptr;
            }
            //printf("Parsing:        %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            //t = clock();
            this->optimizer->optimize(&statements);
            //printf("Optimization:   %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            //t = clock();
            module = this->emitter->run_emitter(statements, module_name);
            AST::Expression::releaseStatementList(this, &statements);
            //printf("Emitting:       %gms\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
            //printf("\nTotal:          %gms\n-----------------------\n", (double)(clock() - total_t) / CLOCKS_PER_SEC * 1000);
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
        upvalue = this->vm->fiber->m_openupvalues;
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
            this->vm->fiber->m_openupvalues = created_upvalue;
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
        fiber->ensure_stack(callee->max_slots + (int)(fiber->m_stacktop - fiber->m_stackdata));
        frame = &fiber->m_allframes[fiber->m_framecount++];
        frame->slots = fiber->m_stacktop;
        fiber->push(callee->asValue());
        for(i = 0; i < argc; i++)
        {
            fiber->push(argv[i]);
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
                    fiber->push(Object::NullVal);
                }
                if(vararg)
                {
                    fiber->push(Array::make(vm->m_state)->asValue());
                }
            }
            else if(callee->vararg)
            {
                array = Array::make(vm->m_state);
                varargc = argc - function_arg_count + 1;
                array->m_actualarray.reserve(varargc + 1, Object::NullVal);
                for(i = 0; i < varargc; i++)
                {
                    array->m_actualarray.m_values[i] = fiber->m_stacktop[(int)i - (int)varargc];
                }

                fiber->m_stacktop -= varargc;
                vm->push(array->asValue());
            }
            else
            {
                fiber->m_stacktop -= (argc - function_arg_count);
            }
        }
        else if(callee->vararg)
        {
            array = Array::make(vm->m_state);
            varargc = argc - function_arg_count + 1;
            array->push(*(fiber->m_stacktop - 1));
            *(fiber->m_stacktop - 1) = array->asValue();
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
            return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
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
            return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
        }
        fiber = this->vm->fiber;
        result = this->execFiber(fiber);
        if(fiber->m_error != Object::NullVal)
        {
            result.result = fiber->m_error;
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
                return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
            }
            type = Object::asObject(callee)->type;

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
                return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
            }
            fiber->ensure_stack(3 + argc + (int)(fiber->m_stacktop - fiber->m_stackdata));
            slot = fiber->m_stacktop;
            fiber->push(instance);
            if(type != Object::Type::Class)
            {
                for(i = 0; i < argc; i++)
                {
                    fiber->push(argv[i]);
                }
            }
            switch(type)
            {
                case Object::Type::NativeFunction:
                    {
                        Value result = Object::as<NativeFunction>(callee)->function(vm, argc, fiber->m_stacktop - argc);
                        fiber->m_stacktop = slot;
                        return Result{LITRESULT_OK, result};
                    }
                    break;
                case Object::Type::NativePrimitive:
                    {
                        Object::as<NativePrimFunction>(callee)->function(vm, argc, fiber->m_stacktop - argc);
                        fiber->m_stacktop = slot;
                        Result{LITRESULT_OK, Object::NullVal};
                    }
                    break;
                case Object::Type::NativeMethod:
                    {
                        natmethod = Object::as<NativeMethod>(callee);
                        result = natmethod->method(vm, *(fiber->m_stacktop - argc - 1), argc, fiber->m_stacktop - argc);
                        fiber->m_stacktop = slot;
                        return Result{LITRESULT_OK, result};
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
                        fiber->m_stacktop = slot;
                        //return Result{LITRESULT_OK, *slot};
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
                            result = Object::as<NativeMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->m_stacktop - argc);
                            fiber->m_stacktop = slot;
                            return Result{LITRESULT_OK, result};
                        }
                        else if(Object::isPrimitiveMethod(mthval))
                        {
                            Object::as<PrimitiveMethod>(mthval)->method(vm, bound_method->receiver, argc, fiber->m_stacktop - argc);

                            fiber->m_stacktop = slot;
                            return Result{LITRESULT_OK, Object::NullVal};
                        }
                        else
                        {
                            fiber->m_stacktop = slot;
                            return this->callFunction(Object::as<Function>(mthval), argv, argc);
                        }
                    }
                    break;
                case Object::Type::PrimitiveMethod:
                    {
                        Object::as<PrimitiveMethod>(callee)->method(vm, *(fiber->m_stacktop - argc - 1), argc, fiber->m_stacktop - argc);
                        fiber->m_stacktop = slot;
                        return Result{LITRESULT_OK, Object::NullVal};
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
        return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
    }

    /*
    void lit_disassemble_module(State* state, Module* module, const char* source);
    void lit_disassemble_chunk(Chunk* chunk, const char* name, const char* source);
    size_t lit_disassemble_instruction(State* state, Chunk* chunk, size_t offset, const char* source);
    */
    void State::showDecompiled()
    {
        
    }

    void State::raiseError(ErrorType type, const char* message, ...)
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

    void State::releaseAPI()
    {
        this->api_name = nullptr;
        this->api_function = nullptr;
        this->api_fiber = nullptr;
    }

    void State::pushRoot(Object* obj)
    {
        pushValueRoot(obj->asValue());
    }

    void State::pushValueRoot(Value value)
    {
        if(this->root_count + 1 >= this->root_capacity)
        {
            this->root_capacity = LIT_GROW_CAPACITY(this->root_capacity);
            this->roots = (Value*)realloc(this->roots, this->root_capacity * sizeof(Value));
        }
        this->roots[this->root_count++] = value;
    }

    Value State::peekRoot(uint8_t distance)
    {
        assert(this->root_count - distance + 1 > 0);
        return this->roots[this->root_count - distance - 1];
    }

    void State::popRoot()
    {
        this->root_count--;
    }

    void State::popRoots(uint8_t amount)
    {
        this->root_count -= amount;
    }

    Value State::getGlobal(String* name)
    {
        Value global;
        if(!this->getVMGlobal(name, &global))
        {
            return Object::NullVal;
        }
        return global;
    }

    Function* State::getGlobalFunction(String* name)
    {
        Value function = getGlobal(name);
        if(Object::isFunction(function))
        {
            return Object::as<Function>(function);
        }
        return nullptr;
    }

    void State::setGlobal(String* name, Value value)
    {
        this->pushRoot((Object*)name);
        this->pushValueRoot(value);
        this->setVMGlobal(name, value);
        this->popRoots(2);
    }

    bool State::globalExists(String* name)
    {
        Value global;
        return this->getVMGlobal(name, &global);
    }

    void State::defineNative(const char* name, NativeFunction::FuncType native)
    {
        this->pushRoot((Object*)String::intern(this, name));
        this->pushRoot((Object*)NativeFunction::make(this, native, Object::as<String>(this->peekRoot(0))));
        this->setVMGlobal(Object::as<String>(this->peekRoot(1)), this->peekRoot(0));
        this->popRoots(2);
    }

    void State::defineNativePrimitive(const char* name, NativePrimFunction::FuncType native)
    {
        this->pushRoot((Object*)String::intern(this, name));
        this->pushRoot((Object*)NativePrimFunction::make(this, native, Object::as<String>(this->peekRoot(0))));
        this->setVMGlobal(Object::as<String>(this->peekRoot(1)), this->peekRoot(0));
        this->popRoots(2);
    }

    Result State::callClosure(Closure* callee, Value* argv, size_t argc)
    {
        Fiber::CallFrame* frame;
        frame = this->setupCall(callee->function, argv, argc);
        if(frame == nullptr)
        {
            return Result{LITRESULT_RUNTIME_ERROR, Object::NullVal};
        }
        frame->closure = callee;
        return this->execCall(frame);
    }

    char* State::readSource(std::string_view path, size_t* destlen, char** patched_file_name)
    {
        clock_t t;
        char* fname;
        char* source;
        t = 0;
        t = clock();
        fname = Util::copyString(path);
        source = Util::readFile(fname, destlen);
        if(source == nullptr)
        {
            this->raiseError(RUNTIME_ERROR, "failed to open file '%s' for reading", fname);
        }
        fname = Util::patchFilename(fname);
        printf("reading source: %gms\n",  (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
        *patched_file_name = fname;
        return source;
    }

    Result State::interpretFile(std::string_view path)
    {
        size_t srclen;
        char* source;
        char* patchedpath;
        Result result;
        source = this->readSource(path.data(), &srclen, &patchedpath);
        if(source == nullptr)
        {
            return Result{LITRESULT_INVALID, Object::NullVal};
        }
        result = this->interpretSource(patchedpath, source, srclen);
        free((void*)source);
        free(patchedpath);
        return result;
    }

    Result State::internalInterpret(String* module_name, const char* code, size_t length)
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
        if(!m_haderror && !fiber->m_isaborting && fiber->m_stacktop != fiber->m_stackdata)
        {
            istack = (intptr_t)(fiber->m_stackdata);
            itop = (intptr_t)(fiber->m_stacktop);
            idif = (intptr_t)(fiber->m_stackdata - fiber->m_stacktop);
            /* me fail english. how do i put this better? */
            this->raiseError(RUNTIME_ERROR, "stack should be same as stack top", idif, istack, istack, itop, itop);
        }
        this->last_module = module;
        return result;
    }
}


