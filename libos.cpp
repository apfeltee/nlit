
#include <sys/stat.h>
#include "lit.h"

namespace lit
{
    namespace Builtins
    {
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
            return Object::fromBool(lit_file_exists(file_name));
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
            rt = FileIO::binwrite_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, byte);
            return Object::toValue(rt);
        }

        static Value objmethod_file_writeshort(VM* vm, Value instance, size_t argc, Value* argv)
        {
            uint16_t rt;
            uint16_t shrt;
            shrt = (uint16_t)lit_check_number(vm, argv, argc, 0);
            rt = FileIO::binwrite_uint16_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, shrt);
            return Object::toValue(rt);
        }

        static Value objmethod_file_writenumber(VM* vm, Value instance, size_t argc, Value* argv)
        {
            uint32_t rt;
            float num;
            num = (float)lit_check_number(vm, argv, argc, 0);
            rt = FileIO::binwrite_uint32_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, num);
            return Object::toValue(rt);
        }

        static Value objmethod_file_writebool(VM* vm, Value instance, size_t argc, Value* argv)
        {
            bool value;
            uint8_t rt;
            value = lit_check_bool(vm, argv, argc, 0);
            rt = FileIO::binwrite_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle, (uint8_t)value ? '1' : '0');
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
            FileIO::binwrite_string(data->handle, string);
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
            return String::format(vm->m_state, "[file @ ($)]", Object::as<Instance>(instance), data->path)->asValue();
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
            return Object::toValue(FileIO::binread_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
        }

        static Value objmethod_file_readshort(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)instance;
            (void)argc;
            (void)argv;
            return Object::toValue(FileIO::binread_uint16_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
        }

        static Value objmethod_file_readnumber(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)instance;
            (void)argc;
            (void)argv;
            return Object::toValue(FileIO::binread_uint32_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle));
        }

        static Value objmethod_file_readbool(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)instance;
            (void)argc;
            (void)argv;
            return Object::fromBool((char)FileIO::binread_uint8_t(((FileData*)LIT_EXTRACT_DATA(vm, instance))->handle) == '1');
        }

        static Value objmethod_file_readstring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)instance;
            (void)argc;
            (void)argv;
            FileData* data = (FileData*)LIT_EXTRACT_DATA(vm, instance);
            String* string = FileIO::binread_string(vm->m_state, data->handle);

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
            return Object::fromBool(stat(directory_name, &buffer) == 0 && S_ISDIR(buffer.st_mode));
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
            //fprintf(stderr, "++ lit_open_file_libary()\n");
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
    }
}
