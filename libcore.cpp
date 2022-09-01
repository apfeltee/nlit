
#include <sys/stat.h>
#include "lit.h"

namespace lit
{
    static bool should_update_locals;

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
                    vm->fiber->m_stacktop -= argc;
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


    namespace Builtins
    {
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
                return Object::fromBool(false);
            }
            return Object::fromBool(Object::asBool(argv[0]) == bv);
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
                written += FileIO::write_string(stdout, sv->data(), sv->length());
            }
            return Object::toValue(written);
        }

        static Value cfn_println(VM* vm, size_t argc, Value* argv)
        {
            size_t i;
            size_t wr;
            String* stringified;
            wr = 0;
            if(argc == 0)
            {
                return Object::toValue(wr);
            }
            for(i = 0; i < argc; i++)
            {
                stringified = Object::toString(vm->m_state, argv[i]);
                wr += FileIO::write_string(stdout, stringified->data(), stringified->size());
            }
            wr += FileIO::write_string(stdout, "\n");
            return Object::toValue(wr);
        }

        Value objfn_string_format(VM* vm, Value instance, size_t argc, Value* argv);

        static Value cfn_printf(VM* vm, size_t argc, Value* argv)
        {
            Value vres;
            Value firstarg;
            String* res;
            (void)res;
            firstarg = argv[0];
            vres = objfn_string_format(vm, firstarg, argc-1, argv+1);
            res = Object::as<String>(vres);
            
            lit_runtime_error_exiting(vm, "printf() is currently broken");
            return Object::NullVal;
        }

        static bool cfn_eval(VM* vm, size_t argc, Value* argv)
        {
            char* code;
            (void)argc;
            (void)argv;
            code = (char*)lit_check_string(vm, argv, argc, 0);
            return compile_and_interpret(vm, vm->fiber->m_module->name, code, strlen(code));
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
            modname = vm->fiber->m_module->name;
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


        void lit_open_core_library(State* state)
        {
            //fprintf(stderr, "++ lit_open_core_libary()\n");
            /*
            * the order here is important: class must be declared first, and object second,
            * since object derives class, and everything else derives object.
            */
            {
                lit_open_kernel_library(state);
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
                state->numbervalue_class = klass;
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
                state->boolvalue_class = klass;
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
    }
}


