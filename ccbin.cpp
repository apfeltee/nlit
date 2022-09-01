
#include "lit.h"
#include "priv.h"

namespace lit
{
    size_t Chunk::addConstant(Value constant)
    {
        for(size_t i = 0; i < m_constants.m_count; i++)
        {
            if(m_constants.m_values[i] == constant)
            {
                return i;
            }
        }

        m_state->pushValueRoot(constant);
        m_constants.push(constant);
        m_state->popRoot();

        return m_constants.m_count - 1;
    }

    void BinaryData::storeModule(Module* module, FILE* file)
    {
        size_t i;
        bool disabled;
        Table* privates;
        disabled = AST::Optimizer::is_enabled(LITOPTSTATE_PRIVATE_NAMES);
        FileIO::binwrite_string(file, module->name);
        FileIO::binwrite_uint16_t(file, module->private_count);
        FileIO::binwrite_uint8_t(file, (uint8_t)disabled);
        if(!disabled)
        {
            privates = &module->private_names->m_values;
            for(i = 0; i < module->private_count; i++)
            {
                if(privates->at(i)->key != nullptr)
                {
                    FileIO::binwrite_string(file, privates->at(i)->key);
                    FileIO::binwrite_uint16_t(file, (uint16_t)Object::toNumber(privates->at(i)->value));
                }
            }
        }
        storeFunction(file, module->main_function);
    }
}



