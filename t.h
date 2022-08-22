
#define LIT_BEGIN_CLASS(name) \
    LitClass* klass = LitClass::make(state, name);


#define LIT_END_CLASS_IGNORING() \
    lit_set_global(state, klass->name, klass->asValue()); 


#define LIT_END_CLASS() \
    lit_set_global(state, klass->name, klass->asValue()); \
    if(klass->super == nullptr) \
    { \
        klass->inheritFrom(state->objectvalue_class); \
    } \

#define LIT_BIND_CONSTRUCTOR(state, klass, method) \
    klass->bindConstructor(method)

#define LIT_BIND_METHOD(state, klass, name, method)\
    klass->bindMethod(name, method)

#define LIT_BIND_PRIMITIVE(state, klass, name, method) \
    klass->bindPrimitive(name, method)

#define LIT_BIND_STATIC_METHOD(name, method) \
    klass->setStaticMethod(name, method);

#define LIT_BIND_STATIC_PRIMITIVE(name, method) \
    klass->setStaticPrimitive(name, method);

#define LIT_SET_STATIC_FIELD(name, field) \
    klass->static_fields.setField(name, field);

#define LIT_BIND_SETTER(name, setter) \
    klass->setSetter(name, setter);

#define LIT_BIND_GETTER(name, getter) \
    klass->setGetter(name, getter);

#define LIT_BIND_FIELD(name, getter, setter) \
    klass->bindField(name, getter, setter)

#define LIT_BIND_STATIC_SETTER(name, setter) \
    klass->setStaticSetter(name, setter)
    
#define LIT_BIND_STATIC_GETTER(name, getter) \
    klass->setStaticGetter(name, getter)

#define LIT_BIND_STATIC_FIELD(name, getter, setter) \
    klass->setStaticField(name, getter, setter)



    {
        LIT_BEGIN_CLASS("String");
        {
            klass->inheritFrom(state->objectvalue_class);
            LIT_BIND_CONSTRUCTOR(state, klass, util_invalid_constructor);
            LIT_BIND_METHOD(state, klass, "+", objfn_string_plus);
            LIT_BIND_METHOD(state, klass, "[]", objfn_string_subscript);
            LIT_BIND_METHOD(state, klass, "<", objfn_string_less);
            LIT_BIND_METHOD(state, klass, ">", objfn_string_greater);
            LIT_BIND_METHOD(state, klass, "==", objfn_string_compare);
            LIT_BIND_METHOD(state, klass, "toString", objfn_string_tostring);
            {
                LIT_BIND_METHOD(state, klass, "toNumber", objfn_string_tonumber);
                LIT_BIND_GETTER("to_i", objfn_string_tonumber);
            }
            /*
            * String.toUpper()
            * turns string to uppercase.
            */
            {
                LIT_BIND_METHOD(state, klass, "toUpperCase", objfn_string_touppercase);
                LIT_BIND_METHOD(state, klass, "toUpper", objfn_string_touppercase);
                LIT_BIND_GETTER("upper", objfn_string_touppercase);
            }
            /*
            * String.toLower()
            * turns string to lowercase.
            */
            {
                LIT_BIND_METHOD(state, klass, "toLowerCase", objfn_string_tolowercase);
                LIT_BIND_METHOD(state, klass, "toLower", objfn_string_tolowercase);
                LIT_BIND_GETTER("lower", objfn_string_tolowercase);
            }
            /*
            * String.toByte
            * turns string into a byte integer.
            * if the string is multi character string, then $self[0] is used.
            * i.e., "foo".toByte == 102 (equiv: "foo"[0].toByte)
            */
            {
                LIT_BIND_GETTER("toByte", objfn_string_tobyte);
            }

            //TODO: byteAt
            //LIT_BIND_METHOD(state, klass, "byteAt", objfn_string_byteat);

            /*
            * String.contains(String str[, bool icase])
            * returns true if $self contains $str.
            * if optional param $icase is true, then search is case-insensitive.
            * this is faster than, eg, "FOO".lower.contains("BLAH".lower), since
            * only the characters searched are tolower()'d.
            */
            LIT_BIND_METHOD(state, klass, "contains", objfn_string_contains);

            /*
            * String.startsWith(String str)
            * returns true if $self starts with $str
            */
            LIT_BIND_METHOD(state, klass, "startsWith", objfn_string_startswith);

            /*
            * String.endsWith(String str)
            * returns true if $self ends with $str
            */
            LIT_BIND_METHOD(state, klass, "endsWith", objfn_string_endswith);

            /*
            * String.replace(String find, String rep)
            * replaces every $find with $rep
            */
            LIT_BIND_METHOD(state, klass, "replace", objfn_string_replace);
            {
                LIT_BIND_METHOD(state, klass, "substring", objfn_string_substring);
                LIT_BIND_METHOD(state, klass, "substr", objfn_string_substring);
            }
            LIT_BIND_METHOD(state, klass, "iterator", objfn_string_iterator);
            LIT_BIND_METHOD(state, klass, "iteratorValue", objfn_string_iteratorvalue);
            LIT_BIND_GETTER("length", objfn_string_length);
            LIT_BIND_METHOD(state, klass, "format", objfn_string_format);
            state->stringvalue_class = klass;
        }
        LIT_END_CLASS();
    }



