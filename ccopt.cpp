
#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        static const char* optimization_level_descriptions[LITOPTLEVEL_TOTAL]
        = { "No optimizations (same as -Ono-all)", "Super light optimizations, sepcific to interactive shell.",
            "(default) Recommended optimization level for the development.", "Medium optimization, recommended for the release.",
            "(default for bytecode) Extreme optimization, throws out most of the variable/function names, used for bytecode compilation." };

        static const char* optimization_names[LITOPTSTATE_TOTAL]
        = { "constant-folding", "literal-folding", "unused-var",    "unreachable-code",
            "empty-body",       "line-info",       "private-names", "c-for" };

        static const char* optimization_descriptions[LITOPTSTATE_TOTAL]
        = { "Replaces constants in code with their values.",
            "Precalculates literal expressions (3 + 4 is replaced with 7).",
            "Removes user-declared all variables, that were not used.",
            "Removes code that will never be reached.",
            "Removes loops with empty bodies.",
            "Removes line information from chunks to save on space.",
            "Removes names of the private locals from modules (they are indexed by id at runtime).",
            "Replaces for-in loops with c-style for loops where it can." };

        static bool optimization_states[LITOPTSTATE_TOTAL];

        static bool optimization_states_setup = false;
        static bool any_optimization_enabled = false;



        bool Optimizer::is_enabled(Optimization optimization)
        {
            if(!optimization_states_setup)
            {
                setup_optimization_states();
            }
            return optimization_states[(int)optimization];
        }

        void Optimizer::set_enabled(Optimization optimization, bool enabled)
        {
            size_t i;
            if(!optimization_states_setup)
            {
                setup_optimization_states();
            }
            optimization_states[(int)optimization] = enabled;
            if(enabled)
            {
                any_optimization_enabled = true;
            }
            else
            {
                for(i = 0; i < LITOPTSTATE_TOTAL; i++)
                {
                    if(optimization_states[i])
                    {
                        return;
                    }
                }
                any_optimization_enabled = false;
            }
        }

        void Optimizer::set_all_enabled(bool enabled)
        {
            size_t i;
            optimization_states_setup = true;
            any_optimization_enabled = enabled;
            for(i = 0; i < LITOPTSTATE_TOTAL; i++)
            {
                optimization_states[i] = enabled;
            }
        }

        const char* Optimizer::get_optimization_name(Optimization optimization)
        {
            return optimization_names[(int)optimization];
        }

        const char* Optimizer::get_optimization_description(Optimization optimization)
        {
            return optimization_descriptions[(int)optimization];
        }

        const char* Optimizer::get_optimization_level_description(OptimizationLevel level)
        {
            return optimization_level_descriptions[(int)level];
        }

        void Optimizer::optimize(Expression::List* statements)
        {
            return;
            if(!optimization_states_setup)
            {
                setup_optimization_states();
            }
            if(!any_optimization_enabled)
            {
                return;
            }
            opt_begin_scope();
            optimize_statements(statements);
            opt_end_scope();
            m_variables.release();
        }
    }
}
