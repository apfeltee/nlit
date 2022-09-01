#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        static const int8_t stack_effects[] = {
            #define OPCODE(_, effect) effect,
            #include "opcode.inc"
            #undef OPCODE
        };

        const char* Emitter::getStateScannerFilename()
        {
            return m_state->scanner->m_filename;
        }

        void Emitter::error(size_t line, Error error, ...)
        {
            va_list args;
            va_start(args, error);
            m_state->raiseError(COMPILE_ERROR, lit_vformat_error(m_state, line, error, args)->data());
            va_end(args);
        }

        int8_t Emitter::getStackEffect(int idx)
        {
            return stack_effects[idx];
        }

        Module* Emitter::run_emitter(Expression::List& statements, String* module_name)
        {
            size_t i;
            size_t total;
            size_t old_privates_count;
            bool isnew;
            State* state;        
            Value module_value;
            Module* module;
            PCGenericArray<Private>* privates;
            Compiler compiler;
            Expression* stmt;
            m_lastline = 1;
            m_emitreference = 0;
            state = m_state;
            isnew = false;
            if(m_state->vm->modules->m_values.get(module_name, &module_value))
            {
                module = Object::as<Module>(module_value);
            }
            else
            {
                module = Module::make(m_state, module_name);
                isnew = true;
            }
            m_module = module;
            old_privates_count = module->private_count;
            if(old_privates_count > 0)
            {
                privates = &m_privates;
                privates->m_count = old_privates_count - 1;
                privates->push(Private{ true, false });
                for(i = 0; i < old_privates_count; i++)
                {
                    privates->m_values[i].initialized = true;
                }
            }
            init_compiler(&compiler, LITFUNC_SCRIPT);
            m_chunk = &compiler.function->chunk;
            this->resolve_statements(statements);
            for(i = 0; i < statements.m_count; i++)
            {
                stmt = statements.m_values[i];
                if(stmt != nullptr)
                {
                    if(emit_statement(stmt))
                    {
                        break;
                    }
                }
            }
            end_scope(m_lastline);
            module->main_function = end_compiler(module_name);
            if(isnew)
            {
                total = m_privates.m_count;
                module->privates = LIT_ALLOCATE(m_state, Value, total);
                for(i = 0; i < total; i++)
                {
                    module->privates[i] = Object::NullVal;
                }
            }
            else
            {
                module->privates = LIT_GROW_ARRAY(m_state, module->privates, Value, old_privates_count, module->private_count);
                for(i = old_privates_count; i < module->private_count; i++)
                {
                    module->privates[i] = Object::NullVal;
                }
            }
            m_privates.release();
            if(Optimizer::is_enabled(LITOPTSTATE_PRIVATE_NAMES))
            {
                m_module->private_names->m_values.release();
            }
            if(isnew && !state->m_haderror)
            {
                state->vm->modules->m_values.set(module_name, module->asValue());
            }
            module->ran = true;
            return module;
        }
    }
}
