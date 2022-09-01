
#include "lit.h"

namespace lit
{
    Module* Module::getModule(State* state, String* name)
    {
        Value value;
        if(state->vm->modules->m_values.get(name, &value))
        {
            return Object::as<Module>(value);
        }
        return nullptr;
    }

    namespace Builtins
    {
        
    }
}


