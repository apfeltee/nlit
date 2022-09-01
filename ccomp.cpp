
#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        void Compiler::init(Parser* parser)
        {
            this->scope_depth = 0;
            this->function = nullptr;
            this->enclosing = (Compiler*)parser->m_compiler;
            parser->m_compiler = this;
        }
    }
}


