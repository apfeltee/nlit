
#include "lit.h"

namespace lit
{
    int Array::indexOf(Value value)
    {
        size_t i;
        for(i = 0; i < m_actualarray.m_count; i++)
        {
            if(m_actualarray.m_values[i] == value)
            {
                return (int)i;
            }
        }
        return -1;
    }

    Value Array::removeAt(size_t index)
    {
        size_t i;
        size_t count;
        Value value;
        PCGenericArray<Value>* values;
        values = &m_actualarray;
        count = values->m_count;
        if(index >= count)
        {
            return Object::NullVal;
        }
        value = values->m_values[index];
        if(index == count - 1)
        {
            values->m_values[index] = Object::NullVal;
        }
        else
        {
            for(i = index; i < values->m_count - 1; i++)
            {
                values->m_values[i] = values->m_values[i + 1];
            }
            values->m_values[count - 1] = Object::NullVal;
        }
        values->m_count--;
        return value;
    }

    namespace Builtins
    {
        static Value objfn_array_constructor(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            size_t count;
            Array* made;
            Value cntval;
            Value fillval;
            (void)instance;
            (void)argc;
            (void)argv;
            made = Array::make(vm->m_state);
            if(argc > 0)
            {
                fillval = Object::NullVal;
                cntval = argv[0];
                if(!Object::isNumber(cntval))
                {
                    lit_runtime_error_exiting(vm, "Array() expects either no arguments or (<number>[, <value>])");
                    return Object::NullVal;
                }
                if(argc > 1)
                {
                    fillval = argv[1];
                }
                count = Object::toNumber(cntval);
                for(i=0; i<count; i++)
                {
                    made->push(fillval);
                }
            }
            return made->asValue();
        }

        static Value objfn_array_splice(VM* vm, Array* array, int from, int to)
        {
            size_t i;
            size_t length;
            Array* new_array;
            length = array->size();
            if(from < 0)
            {
                from = (int)length + from;
            }
            if(to < 0)
            {
                to = (int)length + to;
            }
            if(from > to)
            {
                lit_runtime_error_exiting(vm, "Array.splice argument 'from' is larger than argument 'to'");
            }
            from = fmax(from, 0);
            to = fmin(to, (int)length - 1);
            length = fmin(length, to - from + 1);
            new_array = Array::make(vm->m_state);
            for(i = 0; i < length; i++)
            {
                new_array->push(array->at(from + i));
            }
            return new_array->asValue();
        }

        static Value objfn_array_slice(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int from;
            int to;
            from = lit_check_number(vm, argv, argc, 0);
            to = lit_check_number(vm, argv, argc, 1);
            return objfn_array_splice(vm, Object::as<Array>(instance), from, to);
        }

        static Value objfn_array_subscript(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int64_t index;
            Array* self;
            Range* range;
            PCGenericArray<Value>* values;
            self = Object::as<Array>(instance);
            if(argc == 2)
            {
                if(!Object::isNumber(argv[0]))
                {
                    lit_runtime_error_exiting(vm, "array index must be a number");
                }
                values = &self->m_actualarray;
                index = Object::toNumber(argv[0]);
                if(index < 0)
                {
                    index = fmax(0, values->m_count + index);
                }
                values->reserve(index + 1, Object::NullVal);
                return values->m_values[index] = argv[1];
            }
            if(!Object::isNumber(argv[0]))
            {
                if(Object::isRange(argv[0]))
                {
                    range = Object::as<Range>(argv[0]);
                    return objfn_array_splice(vm, self, (int)range->from, (int)range->to);
                }
                lit_runtime_error_exiting(vm, "array index must be a number");
                return Object::NullVal;
            }
            values = &self->m_actualarray;
            index = Object::toNumber(argv[0]);
            if(index < 0)
            {
                index = fmax(0, values->m_count + index);
            }
            //if(values->m_capacity <= (size_t)index)
            if(index >= int64_t(values->size()))
            {
                lit_runtime_error_exiting(vm, "array index %d out of bounds", index);
                return Object::NullVal;
            }
            return values->m_values[index];
        }

        static Value objfn_array_compare(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            Array* self;
            Array* other;
            (void)argc;
            //fprintf(stderr, "call to objfn_array_compare\n");
            self = Object::as<Array>(instance);
            if(Object::isArray(argv[0]))
            {
                other = Object::as<Array>(argv[0]);
                if(self->m_actualarray.m_count == other->m_actualarray.m_count)
                {
                    for(i=0; i<self->m_actualarray.m_count; i++)
                    {
                        if(!Object::compare(vm->m_state, self->m_actualarray.m_values[i], other->m_actualarray.m_values[i]))
                        {
                            return Object::FalseVal;
                        }
                    }
                    return Object::TrueVal;
                }
                return Object::FalseVal;
            }
            lit_runtime_error_exiting(vm, "can only compare array to another array or null");
            return Object::FalseVal;
        }

        static Value objfn_array_add(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            Array* self;
            (void)vm;
            self = Object::as<Array>(instance);
            for(i=0; i<argc; i++)
            {
                self->push(argv[i]);
            }
            return Object::NullVal;
        }

        static Value objfn_array_insert(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int i;
            int index;
            Value value;
            PCGenericArray<Value>* values;
            LIT_ENSURE_ARGS(2);
            values = &Object::as<Array>(instance)->m_actualarray;
            index = lit_check_number(vm, argv, argc, 0);

            if(index < 0)
            {
                index = fmax(0, values->m_count + index);
            }
            value = argv[1];
            if((int)values->m_count <= index)
            {
                values->reserve(index + 1, Object::NullVal);
            }
            else
            {
                values->reserve(values->m_count + 1, Object::NullVal);
                for(i = values->m_count - 1; i > index; i--)
                {
                    values->m_values[i] = values->m_values[i - 1];
                }
            }
            values->m_values[index] = value;
            return Object::NullVal;
        }

        static Value objfn_array_addall(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            Array* array;
            Array* toadd;
            LIT_ENSURE_ARGS(1);
            if(!Object::isArray(argv[0]))
            {
                lit_runtime_error_exiting(vm, "expected array as the argument");
            }
            array = Object::as<Array>(instance);
            toadd = Object::as<Array>(argv[0]);
            for(i = 0; i < toadd->m_actualarray.m_count; i++)
            {
                array->push(toadd->m_actualarray.m_values[i]);
            }
            return Object::NullVal;
        }

        static Value objfn_array_indexof(VM* vm, Value instance, size_t argc, Value* argv)
        {
            LIT_ENSURE_ARGS(1)

                int index = Object::as<Array>(instance)->indexOf(argv[0]);
            return index == -1 ? Object::NullVal : Object::toValue(index);
        }


        static Value objfn_array_remove(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int index;
            Array* array;
            LIT_ENSURE_ARGS(1);
            array = Object::as<Array>(instance);
            index = array->indexOf(argv[0]);
            if(index != -1)
            {
                return array->removeAt((size_t)index);
            }
            return Object::NullVal;
        }

        static Value objfn_array_removeat(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int index;
            index = lit_check_number(vm, argv, argc, 0);
            if(index < 0)
            {
                return Object::NullVal;
            }
            return Object::as<Array>(instance)->removeAt((size_t)index);
        }

        static Value objfn_array_contains(VM* vm, Value instance, size_t argc, Value* argv)
        {
            LIT_ENSURE_ARGS(1);
            return Object::fromBool(Object::as<Array>(instance)->indexOf(argv[0]) != -1);
        }

        static Value objfn_array_clear(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argc;
            (void)argv;
            Object::as<Array>(instance)->m_actualarray.m_count = 0;
            return Object::NullVal;
        }

        static Value objfn_array_iterator(VM* vm, Value instance, size_t argc, Value* argv)
        {
            int number;
            Array* array;
            (void)vm;
            LIT_ENSURE_ARGS(1);
            array = Object::as<Array>(instance);
            number = 0;
            if(Object::isNumber(argv[0]))
            {
                number = Object::toNumber(argv[0]);
                if(number >= (int)array->m_actualarray.m_count - 1)
                {
                    return Object::NullVal;
                }
                number++;
            }
            return array->m_actualarray.m_count == 0 ? Object::NullVal : Object::toValue(number);
        }

        static Value objfn_array_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t index;
            PCGenericArray<Value>* values;
            index = lit_check_number(vm, argv, argc, 0);
            values = &Object::as<Array>(instance)->m_actualarray;
            if(values->m_count <= index)
            {
                return Object::NullVal;
            }
            return values->m_values[index];
        }

        static Value objfn_array_join(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t i;
            size_t jlen;
            size_t index;
            size_t length;

            String* chars;
            PCGenericArray<Value>* values;
            String* string;
            String* joinee;
            String** strings;
            (void)argc;
            (void)argv;
            joinee = nullptr;
            length = 0;
            if(argc > 0)
            {
                joinee = Object::as<String>(argv[0]);
            }
            values = &Object::as<Array>(instance)->m_actualarray;
            //String* strings[values.m_count];
            strings = LIT_ALLOCATE(vm->m_state, String*, values->m_count+1);
            for(i = 0; i < values->m_count; i++)
            {
                string = Object::toString(vm->m_state, values->m_values[i]);
                strings[i] = string;
                length += string->length();
                if(joinee != nullptr)
                {
                    length += joinee->length();
                }
            }
            jlen = 0;
            index = 0;
            //chars = sdsempty();
            chars = String::make(vm->m_state);
            //chars = sdsMakeRoomFor(chars, length + 1);
            if(joinee != nullptr)
            {
                jlen = joinee->length();
            }
            for(i = 0; i < values->m_count; i++)
            {
                string = strings[i];
                //memcpy(chars + index, string->data(), string->length());
                //chars = sdscatlen(chars, string->data(), string->length());
                chars->append(string);
                index += string->length();
                if(joinee != nullptr)
                {
                    if((i+1) != values->m_count)
                    {
                        //chars = sdscatlen(chars, joinee->data(), jlen);
                        chars->append(joinee);
                    }
                    index += jlen;
                }
            }
            LIT_FREE(vm->m_state, String*, strings);
            //return String::take(vm->m_state, chars, length)->asValue();
            return chars->asValue();
        }

        static Value objfn_array_sort(VM* vm, Value instance, size_t argc, Value* argv)
        {
            PCGenericArray<Value>* values;
            values = &Object::as<Array>(instance)->m_actualarray;
            if(argc == 1 && Object::isCallableFunction(argv[0]))
            {
                util_custom_quick_sort(vm, values->m_values, values->m_count, argv[0]);
            }
            else
            {
                util_basic_quick_sort(vm->m_state, values->m_values, values->m_count);
            }
            return instance;
        }

        static Value objfn_array_clone(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            size_t i;
            State* state;
            PCGenericArray<Value>* values;
            Array* array;
            PCGenericArray<Value>* new_values;
            state = vm->m_state;
            values = &Object::as<Array>(instance)->m_actualarray;
            array = Array::make(state);
            new_values = &array->m_actualarray;
            new_values->reserve(values->m_count, Object::NullVal);
            // lit_values_ensure_size sets the count to max of previous count (0 in this case) and new count, so we have to reset it
            new_values->m_count = 0;
            for(i = 0; i < values->m_count; i++)
            {
                new_values->push(values->m_values[i]);
            }
            return array->asValue();
        }

        // todo:
        // CONTAINER_OUTPUT_MAX is a terrible solution.
        // rather, implement lazy range, and then dynamically print (0 ... N)
        static Value objfn_array_tostring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            bool has_more;
            size_t i;
            size_t value_amount;
            size_t olength;
            //char* buffer;
            String* buffer;
            Array* self;
            PCGenericArray<Value>* values;
            Value val;
            State* state;
            String* part;
            String* stringified;
            String** values_converted;
            static const char* recstring = "(recursion)";
            self = Object::as<Array>(instance);
            values = &self->m_actualarray;
            state = vm->m_state;
            if(values->m_count == 0)
            {
                return String::internValue(state, "[]");
            }
            has_more = values->m_count > LIT_CONTAINER_OUTPUT_MAX;
            //value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->m_count;
            value_amount = values->m_count;
            values_converted = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
            // "[ ]"
            olength = 3;
            if(has_more)
            {
                olength += 3;
            }
            //buffer = sdsempty();
            buffer = String::make(vm->m_state);
            //buffer = sdsMakeRoomFor(buffer, olength+1);
            //buffer = sdscat(buffer, "[");
            buffer->append("[");
            for(i = 0; i < value_amount; i++)
            {
                val = values->m_values[(has_more && i == value_amount - 1) ? values->m_count - 1 : i];
                if(Object::isArray(val) && (Object::as<Array>(val) == self))
                {
                    stringified = String::copy(state, recstring, strlen(recstring));
                }
                else
                {
                    stringified = Object::toString(state, val);
                }
                part = stringified;
                //buffer = sdscatlen(buffer, part->data(), part->length());
                buffer->append(part);
                /*if(has_more && i == value_amount - 2)
                {
                    //buffer = sdscat(buffer, " ... ");
                    buffer->append(" ... ");
                }
                else
                */
                {
                    if(i == (value_amount - 1))
                    {
                        //buffer = sdscat(buffer, " ]");
                        buffer->append(" ]");
                    }
                    else
                    {
                        //buffer = sdscat(buffer, ", ");
                        buffer->append(", ");
                    }
                }
            }
            LIT_FREE(vm->m_state, String*, values_converted);
            // should be String::take, but it doesn't get picked up by the GC for some reason
            //rt = String::take(vm->m_state, buffer, olength);
            //rt = String::take(vm->m_state, buffer, olength);
            return buffer->asValue();
        }

        static Value objfn_array_length(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argc;
            (void)argv;
            return Object::toValue(Object::as<Array>(instance)->m_actualarray.m_count);
        }

        void lit_open_array_library(State* state)
        {
            //fprintf(stderr, "++ lit_open_array_libary()\n");
            Class* klass = Class::make(state, "Array");
            state->arrayvalue_class = klass;
            {
                klass->inheritFrom(state->objectvalue_class);
                klass->bindConstructor(objfn_array_constructor);
                klass->bindMethod("[]", objfn_array_subscript);
                klass->bindMethod("==", objfn_array_compare);
                klass->bindMethod("add", objfn_array_add);
                klass->bindMethod("push", objfn_array_add);
                klass->bindMethod("insert", objfn_array_insert);
                klass->bindMethod("slice", objfn_array_slice);
                klass->bindMethod("addAll", objfn_array_addall);
                klass->bindMethod("remove", objfn_array_remove);
                klass->bindMethod("removeAt", objfn_array_removeat);
                klass->bindMethod("indexOf", objfn_array_indexof);
                klass->bindMethod("contains", objfn_array_contains);
                klass->bindMethod("clear", objfn_array_clear);
                klass->bindMethod("iterator", objfn_array_iterator);
                klass->bindMethod("iteratorValue", objfn_array_iteratorvalue);
                klass->bindMethod("join", objfn_array_join);
                klass->bindMethod("sort", objfn_array_sort);
                klass->bindMethod("clone", objfn_array_clone);
                klass->bindMethod("toString", objfn_array_tostring);
                klass->setGetter("length", objfn_array_length);
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }
        }
    }
}

