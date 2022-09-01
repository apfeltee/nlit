
#include "lit.h"

namespace lit
{
    void Table::release()
    {
        size_t i;
        Entry* ent;
        for(i=0; i<size(); i++)
        {
            ent = m_inner.m_values[i];
            delete ent;
            m_inner.m_values[i] = nullptr;
        }
        m_inner.release();
    }

    void Table::markForGC(VM* vm)
    {
        size_t i;
        Table::Entry* entry;
        for(i = 0; i <= m_inner.m_capacity; i++)
        {
            entry = m_inner.m_values[i];
            if(entry != nullptr)
            {
                vmMarkObject(vm, (Object*)entry->key);
                vmMarkValue(vm, entry->value);
            }
        }
    }

    bool Table::set(String* key, Value value)
    {
        size_t i;
        if(m_inner.size() > 0)
        {
            for(i=0; i<m_inner.size(); i++)
            {
                if(m_inner.at(i) != nullptr)
                {
                    if(m_inner.at(i)->key != nullptr)
                    {
                        if((m_inner.at(i)->key->m_chars == key->m_chars))
                        {
                            m_inner.at(i)->value = value;
                            return false;
                        }
                    }
                }
            }
        }
        m_inner.push(new Entry{key, value});
        return true;
    }

    bool Table::get(String* key, Value* value) const
    {
        size_t i;
        for(i=0; i<m_inner.size(); i++)
        {
            if((m_inner.at(i)->key->m_chars == key->m_chars) && (m_inner.at(i)->key->m_hash == key->m_hash))
            {
                *value = m_inner.at(i)->value;
                return true;
            }
        }
        return false;
    }

    bool Table::remove(String* key)
    {
        size_t i;
        for(i=0; i<m_inner.size(); i++)
        {
            if(m_inner.at(i)->key && ((m_inner.at(i)->key->m_chars == key->m_chars) && (m_inner.at(i)->key->m_hash == key->m_hash)))
            {
                m_inner.at(i)->key = nullptr;
                return true;
            }
        }
        return false;
    }

    String* Table::find(const char* str, size_t length, uint32_t hs) const
    {
        size_t i;
        if(m_inner.size() > 0)
        {
            for(i=0; i<m_inner.size(); i++)
            {
                if(m_inner.at(i)->key)
                {
                    if(*(m_inner.at(i)->key->m_chars) == std::string_view(str, length))
                    {
                        if(m_inner.at(i)->key->m_hash == hs)
                        {
                            return m_inner.at(i)->key;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    void Table::addAll(const Table& from)
    {
        //fprintf(stderr, "addAll: from=%p (%d)\n", &from, from.size());
        size_t i;
        if(from.size() > 0)
        {
            for(i = 0; i < from.size(); i++)
            {
                const auto* entry = from.at(i);
                if(entry != nullptr)
                {
                    if(entry->key != nullptr)
                    {
                        //fprintf(stderr, "Table::addAll: %s\n", entry->key->data());
                        set(entry->key, entry->value);
                    }
                }
            }
        }
    }

    void Table::removeWhite()
    {
        size_t i;
        Entry* entry;
        for(i = 0; i <= m_inner.m_capacity; i++)
        {
            entry = m_inner.m_values[i];
            if(entry->key != nullptr && !entry->key->marked)
            {
                this->remove(entry->key);
            }
        }
    }

    void Table::vmMarkObject(VM* vm, Object* obj)
    {
        vm->markObject(obj);
    }

    void Table::vmMarkValue(VM* vm, Value val)
    {
        vm->markValue(val);
    }


    int64_t Table::iterator(int64_t number) const
    {
        if(m_inner.m_count == 0)
        {
            return -1;
        }
        if(number >= int64_t(m_inner.m_capacity))
        {
            return -1;
        }
        number++;
        for(; number < int64_t(m_inner.m_capacity); number++)
        {
            if(m_inner.m_values[number]->key != nullptr)
            {
                return number;
            }
        }
        return -1;
    }

    Value Table::iterKey(int64_t index) const
    {
        if(int64_t(m_inner.m_capacity) <= index)
        {
            return Object::NullVal;
        }
        return m_inner.m_values[index]->key->asValue();
    }

    namespace Builtins
    {
        static Value objfn_map_constructor(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)instance;
            (void)argc;
            (void)argv;
            return Map::make(vm->m_state)->asValue();
        }

        static Value objfn_map_subscript(VM* vm, Value instance, size_t argc, Value* argv)
        {
            Value val;
            Value value;
            Map* map;
            String* index;
            if(!Object::isString(argv[0]))
            {
                lit_runtime_error_exiting(vm, "map index must be a string");
            }
            map = Object::as<Map>(instance);
            index = Object::as<String>(argv[0]);
            if(argc == 2)
            {
                val = argv[1];
                if(map->m_indexfn != nullptr)
                {
                    return map->m_indexfn(vm, map, index, &val);
                }
                map->set(index, val);
                return val;
            }
            if(map->m_indexfn != nullptr)
            {
                return map->m_indexfn(vm, map, index, nullptr);
            }
            if(!map->m_values.get(index, &value))
            {
                return Object::NullVal;
            }
            return value;
        }

        static Value objfn_map_addall(VM* vm, Value instance, size_t argc, Value* argv)
        {
            LIT_ENSURE_ARGS(1);
            if(!Object::isMap(argv[0]))
            {
                lit_runtime_error_exiting(vm, "expected map as the argument");
            }
            Object::as<Map>(argv[0])->addAll(Object::as<Map>(instance));
            return Object::NullVal;
        }


        static Value objfn_map_clear(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argv;
            (void)argc;
            Object::as<Map>(instance)->m_values.m_inner.m_count = 0;
            return Object::NullVal;
        }

        static Value objfn_map_iterator(VM* vm, Value instance, size_t argc, Value* argv)
        {
            LIT_ENSURE_ARGS(1);
            (void)vm;
            int index;
            int value;
            index = argv[0] == Object::NullVal ? -1 : Object::toNumber(argv[0]);
            value = Object::as<Map>(instance)->m_values.iterator(index);
            return value == -1 ? Object::NullVal : Object::toValue(value);
        }

        static Value objfn_map_iteratorvalue(VM* vm, Value instance, size_t argc, Value* argv)
        {
            size_t index;
            index = lit_check_number(vm, argv, argc, 0);
            return Object::as<Map>(instance)->m_values.iterKey(index);
        }

        static Value objfn_map_clone(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            State* state;
            Map* map;
            state = vm->m_state;
            map = Map::make(state);
            Object::as<Map>(instance)->m_values.addAll(map->m_values);
            return map->asValue();
        }

        static Value objfn_map_tostring(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)argc;
            (void)argv;
            bool has_wrapper;
            bool has_more;
            size_t i;
            size_t index;
            size_t value_amount;
            size_t olength;
            size_t buffer_index;
            String* buffer;
            State* state;
            Map* map;
            Table* values;
            Table::Entry* entry;
            Value field;
            String* strobval;
            String* key;
            String* value;
            String** values_converted;
            String** keys;
            state = vm->m_state;
            map = Object::as<Map>(instance);
            values = &map->m_values;
            if(values->size() == 0)
            {
                return String::internValue(state, "{}");
            }
            has_wrapper = map->m_indexfn != nullptr;
            has_more = values->size() > LIT_CONTAINER_OUTPUT_MAX;
            value_amount = has_more ? LIT_CONTAINER_OUTPUT_MAX : values->size();
            values_converted = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
            keys = LIT_ALLOCATE(vm->m_state, String*, value_amount+1);
            olength = 3;
            if(has_more)
            {
                olength += SINGLE_LINE_MAPS_ENABLED ? 5 : 6;
            }
            i = 0;
            index = 0;
            do
            {
                entry = values->m_inner.m_values[index++];
                if(entry)
                {
                    if(entry->key != nullptr)
                    {
                        // Special hidden key
                        field = has_wrapper ? map->m_indexfn(vm, map, entry->key, nullptr) : entry->value;
                        // This check is required to prevent infinite loops when playing with Module.privates and such
                        strobval = (Object::isMap(field) && Object::as<Map>(field)->m_indexfn != nullptr) ? String::intern(state, "map") : Object::toString(state, field);
                        state->pushRoot((Object*)strobval);
                        values_converted[i] = strobval;
                        keys[i] = entry->key;
                        olength += (
                            entry->key->length() + 3 + strobval->length() +
                            #ifdef SINGLE_LINE_MAPS
                                (i == value_amount - 1 ? 1 : 2)
                            #else
                                (i == value_amount - 1 ? 2 : 3)
                            #endif
                        );
                    }
                }
                i++;
            } while(i < value_amount);
            buffer = String::allocEmpty(vm->m_state, olength+1);
            #ifdef SINGLE_LINE_MAPS
            buffer->append("{ ");
            #else
            buffer->append("{\n");
            #endif
            buffer_index = 2;
            for(i = 0; i < value_amount; i++)
            {
                key = keys[i];
                value = values_converted[i];
                #ifndef SINGLE_LINE_MAPS
                buffer->append('\t');
                #endif
                buffer->append(key->data(), key->length());
                buffer_index += key->length();
                buffer->append(" = ", 3);
                buffer_index += 3;
                buffer->append(value->data(), value->length());
                buffer_index += value->length();
                /*
                if(has_more && i == value_amount - 1)
                {
                    #ifdef SINGLE_LINE_MAPS
                    memcpy(&buffer->append(", ... }");
                    #else
                    buffer->append(",\n\t...\n}");
                    #endif
                    buffer_index += 8;
                }
                else
                */
                {
                    #ifdef SINGLE_LINE_MAPS
                    buffer->append((i == value_amount - 1) ? " }" : ", ");
                    #else
                    buffer->append((i == value_amount - 1) ? "\n}" : ",\n");
                    #endif
                    buffer_index += 2;
                }
                state->popRoot();
            }
            LIT_FREE(vm->m_state, String*, keys);
            LIT_FREE(vm->m_state, String*, values_converted);
            return buffer->asValue();
        }

        static Value objfn_map_length(VM* vm, Value instance, size_t argc, Value* argv)
        {
            (void)vm;
            (void)argc;
            (void)argv;
            return Object::toValue(Object::as<Map>(instance)->size());
        }

        void lit_open_map_library(State* state)
        {
            //fprintf(stderr, "++ lit_open_map_libary()\n");
            Class* klass = Class::make(state, "Map");
            state->mapvalue_class = klass;
            {
                klass->inheritFrom(state->objectvalue_class);
                klass->bindConstructor(objfn_map_constructor);
                klass->bindMethod("[]", objfn_map_subscript);
                klass->bindMethod("addAll", objfn_map_addall);
                klass->bindMethod("clear", objfn_map_clear);
                klass->bindMethod("iterator", objfn_map_iterator);
                klass->bindMethod("iteratorValue", objfn_map_iteratorvalue);
                klass->bindMethod("clone", objfn_map_clone);
                klass->bindMethod("toString", objfn_map_tostring);
                klass->setGetter("length", objfn_map_length);
                state->mapvalue_class = klass;
            }
            state->setGlobal(klass->name, klass->asValue());
            if(klass->super == nullptr)
            {
                klass->inheritFrom(state->objectvalue_class);
            }

        }
    }
}


