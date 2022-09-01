
#include "lit.h"

namespace lit
{
    bool Fiber::ensureFiber(VM* vm, Fiber* fiber)
    {
        return ensureFiber(vm->m_state, vm, fiber);
    }

    namespace Builtins
    {
        static Value objfn_fiber_constructor(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)instance;
            if(argc < 1 || !Object::isFunction(argv[0]))
            {
                lit_runtime_error_exiting(vm, "Fiber constructor expects a function as its argument");
            }

            Function* function = Object::as<Function>(argv[0]);
            Module* module = vm->fiber->m_module;
            Fiber* fiber = Fiber::make(vm->m_state, module, function);

            fiber->m_parent = vm->fiber;

            return fiber->asValue();
        }


        static Value objfn_fiber_done(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argc;
            (void)argv;
            return Object::fromBool(util_is_fiber_done(Object::as<Fiber>(instance)));
        }


        static Value objfn_fiber_error(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argc;
            (void)argv;
            return Object::as<Fiber>(instance)->m_error;
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
            if(vm->fiber->m_parent == nullptr)
            {
                lit_handle_runtime_error(vm, argc == 0 ? String::intern(vm->m_state, "Fiber was yielded") :
                Object::toString(vm->m_state, argv[0]));
                return true;
            }

            Fiber* fiber = vm->fiber;

            vm->fiber = vm->fiber->m_parent;
            vm->fiber->m_stacktop -= fiber->m_argcount;
            vm->fiber->m_stacktop[-1] = argc == 0 ? Object::NullVal : Object::toString(vm->m_state, argv[0])->asValue();

            argv[-1] = Object::NullVal;
            return true;
        }

        static bool objfn_fiber_yeet(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)instance;
            if(vm->fiber->m_parent == nullptr)
            {
                lit_handle_runtime_error(vm, argc == 0 ? String::intern(vm->m_state, "Fiber was yeeted") :
                Object::toString(vm->m_state, argv[0]));
                return true;
            }

            Fiber* fiber = vm->fiber;

            vm->fiber = vm->fiber->m_parent;
            vm->fiber->m_stacktop -= fiber->m_argcount;
            vm->fiber->m_stacktop[-1] = argc == 0 ? Object::NullVal : Object::toString(vm->m_state, argv[0])->asValue();

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
            return String::format(vm->m_state, "[fiber @]", instance)->asValue();

        }

        void lit_open_fiber_library(State* state)
        {
            //fprintf(stderr, "++ lit_open_fiber_libary()\n");
            Class* klass = Class::make(state, "Fiber");
            state->fibervalue_class = klass;
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
    }
}
