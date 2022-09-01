
#include "lit.h"

namespace lit
{
    Class* Class::fromInstance(Value instance)
    {
        return Object::as<Instance>(instance)->klass;
    }

    Class* Class::getClassFor(State* state, Value value)
    {
        Value* slot;
        Upvalue* upvalue;
        if(Object::isObject(value))
        {
            switch(Object::asObject(value)->type)
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
        return Result{LITRESULT_INVALID, Object::NullVal};    
    }

    namespace Builtins
    {
        static Value objfn_class_tostring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            const char* name;
            Class* selfclass;
            name = "(unnamed)";
            (void)argc;
            (void)argv;
            if(Object::isClass(instance))
            {
                selfclass = Object::as<Class>(instance);
                if(selfclass && selfclass->name)
                {
                    name = selfclass->name->data();
                }
            }
            return String::format(vm->m_state, "[class $]", name)->asValue();
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
            String* name;
            Value setval;
            Value value;
            (void)argc;
            klass = Object::as<Class>(instance);
            if(argc == 2)
            {
                if(!Object::isString(argv[0]))
                {
                    lit_runtime_error_exiting(vm, "class index must be a string");
                }
                name = Object::as<String>(argv[0]);
                setval = argv[1];
                if(Object::isFunction(setval) || Object::isClosure(setval))
                {
                    fprintf(stderr, "setting method ...\n");
                    klass->methods.set(name, setval);
                }
                else
                {
                    klass->static_fields.set(name, setval);
                }
                return setval;
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
            //fprintf(stderr, "++ lit_open_class_libary()\n");
            Class* klass = Class::make(state, "Class");
            state->classvalue_class = klass;
            {
                klass->inheritFrom(state->kernelvalue_class);
                klass->bindMethod("[]", objfn_class_subscript);
                klass->bindMethod("==", objfn_class_compare);
                klass->bindMethod("toString", objfn_class_tostring);
                //klass->setGetter("super", objfn_class_super);

                klass->setStaticGetter("super", objfn_class_super);
                klass->setStaticGetter("name", objfn_class_name);
                klass->setStaticMethod("toString", objfn_class_tostring);
                klass->setStaticMethod("iterator", objfn_class_iterator);
                klass->setStaticMethod("iteratorValue", objfn_class_iteratorvalue);
            }
            state->classvalue_class->super = state->kernelvalue_class;
            state->setGlobal(klass->name, klass->asValue());
        }
    }
}
