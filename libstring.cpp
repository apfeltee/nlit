
#include "lit.h"

namespace lit
{
    String* String::allocEmpty(State* state, size_t length)
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

    String* String::allocate(State* state, const char* chars, size_t length, uint32_t hs, bool mustfree)
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

    String* String::take(State* state, char* chars, size_t length)
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

    String* String::copy(State* state, const char* chars, size_t length)
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

    Value String::stringNumberToString(State* state, double value)
    {
        if(std::isnan(value))
        {
            return String::internValue(state, "nan");
        }

        if(std::isinf(value))
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

    int String::decodeNumBytes(uint8_t byte)
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

    int String::encodeNumBytes(int value)
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

    String* String::fromCodePoint(State* state, int value)
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

    String* String::fromRange(State* state, String* source, int start, uint32_t count)
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

    int String::utfstringEncode(int value, uint8_t* bytes)
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

    int String::utfstringDecode(const uint8_t* bytes, uint32_t length)
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

    int String::utfcharOffset(const char* str, int index)
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

    String* String::format(State* state, const char* format, ...)
    {
        String* rt;
        va_list va;
        va_start(va, format);
        rt = String::vformat(state, format, va);
        va_end(va);
        return rt;
    }

    String* String::vformat(State* state, const char* format, va_list arg_list)
    {
        char ch;
        size_t length;
        size_t total_length;
        bool was_allowed;
        const char* c;
        const char* strval;
        Value val;
        String* string;
        String* result;
        was_allowed = stateGetGCAllowed(state);
        stateSetGCAllowed(state, false);
        total_length = strlen(format);
        result = String::allocEmpty(state, total_length + 1);
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
        result->m_hash = String::makeHash(*(result->m_chars));
        String::statePutInterned(state, result);
        stateSetGCAllowed(state, was_allowed);
        return result;
    }

    bool String::equal(State* state, String* a, String* b)
    {
        (void)state;
        if((a == nullptr) || (b == nullptr))
        {
            return false;
        }
        return (*(a->m_chars) == *(b->m_chars));
    }

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

    bool String::contains(const char* findme, size_t fmlen, bool icase) const
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

    Array* String::split(std::string_view sep, bool keepblanc) const
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

    size_t String::utfLength() const
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

    String* String::codePointAt(uint32_t index) const
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

    namespace Builtins
    {
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
            return Object::fromBool(strcmp(Object::as<String>(instance)->data(), lit_check_string(vm, argv, argc, 0)) < 0);
        }

        static Value objfn_string_greater(VM* vm, Value instance, size_t argc, Value* argv)
        {
            return Object::fromBool(strcmp(Object::as<String>(instance)->data(), lit_check_string(vm, argv, argc, 0)) > 0);
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

        static Value objfn_string_indexof(VM* vm, Value instance, size_t argc, Value* argv)
        {
            char want;
            const char* swant;
            size_t pos;
            size_t maxlen;
            String* self;
            self = Object::as<String>(instance);
            swant = lit_check_string(vm, argv, argc, 0);
            want = swant[0];
            maxlen = self->size();
            for(pos=0; pos<maxlen; pos++)
            {
                if(self->at(pos) == want)
                {
                    return Object::toValue(pos);
                }
            }
            return Object::toValue(-1);

        }


        static Value objfn_string_split(VM* vm, Value instance, size_t argc, Value* argv)
        {
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
            (void)buf;
            if(ai <= argc)
            {
                return true;
            }
            //sdsfree(buf);
            lit_runtime_error_exiting(vm, "too few arguments for format flag '%s' at position %d (argc=%d)", fmttext, ai, argc);
            return false;
        }

        Value objfn_string_format(VM* vm, Value instance, size_t argc, Value* argv)
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
            //fprintf(stderr, "++ lit_open_string_libary()\n");
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
            * javascript shit (does what byteAt does)
            */
            {
                klass->bindMethod("charCodeAt", objfn_string_byteat);
            }
            {
                klass->bindMethod("indexOf", objfn_string_indexof);
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

