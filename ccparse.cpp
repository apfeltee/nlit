
#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        static bool did_setup_rules;

        ParseRule Parser::rules[LITTOK_EOF + 1];

        void Parser::init(State* state)
        {
            if(!did_setup_rules)
            {
                did_setup_rules = true;
                Parser::setup_rules();
            }
            m_state = state;
            m_haderror = false;
            m_panicmode = false;
        }

        void Parser::stringError(Token* token, const char* message)
        {
            (void)token;
            if(m_panicmode)
            {
                return;
            }
            m_state->raiseError(COMPILE_ERROR, message);
            m_haderror = true;
            this->sync();
        }

        Scanner* Parser::getStateScanner(State* state)
        {
            return state->scanner;
        }
    }
}


