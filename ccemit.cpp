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

        void Emitter::init(State* state)
        {
            m_state = state;
            m_loopstart = 0;
            m_emitreference = 0;
            m_classname = nullptr;
            m_compiler = nullptr;
            m_chunk = nullptr;
            m_module = nullptr;
            m_prevwasexprstmt = false;
            m_havesuperclass = false;
            m_privates.init(state);
            m_breaks.init(state);
            m_continues.init(state);
        }

        void Emitter::release()
        {
            m_breaks.release();
            m_continues.release();
        }

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

        void Emitter::resolve_statements(Expression::List& statements)
        {
            size_t i;
            for(i = 0; i < statements.m_count; i++)
            {
                resolve_statement(statements.m_values[i]);
            }
        }

        void Emitter::emit_byte(uint16_t line, uint8_t byte)
        {
            if(line < m_lastline)
            {
                // Egor-fail proofing
                line = m_lastline;
            }
            m_chunk->putChunk(byte, line);
            m_lastline = line;
        }

        void Emitter::emit_bytes(uint16_t line, uint8_t a, uint8_t b)
        {
            if(line < m_lastline)
            {
                // Egor-fail proofing
                line = m_lastline;
            }
            m_chunk->putChunk(a, line);
            m_chunk->putChunk(b, line);
            m_lastline = line;
        }

        void Emitter::emit_op(uint16_t line, OpCode op)
        {
            auto compiler = m_compiler;
            emit_byte(line, (uint8_t)op);
            compiler->slots += getStackEffect((int)op);
            if(compiler->slots > (int)compiler->function->max_slots)
            {
                compiler->function->max_slots = (size_t)compiler->slots;
            }
        }

        void Emitter::emit_ops(uint16_t line, OpCode a, OpCode b)
        {
            auto compiler = m_compiler;
            emit_bytes(line, (uint8_t)a, (uint8_t)b);
            compiler->slots += getStackEffect((int)a) + getStackEffect((int)b);
            if(compiler->slots > (int)compiler->function->max_slots)
            {
                compiler->function->max_slots = (size_t)compiler->slots;
            }
        }

        void Emitter::emit_varying_op(uint16_t line, OpCode op, uint8_t arg)
        {
            auto compiler = m_compiler;
            emit_bytes(line, (uint8_t)op, arg);
            compiler->slots -= arg;
            if(compiler->slots > (int)compiler->function->max_slots)
            {
                compiler->function->max_slots = (size_t)compiler->slots;
            }
        }

        void Emitter::emit_arged_op(uint16_t line, OpCode op, uint8_t arg)
        {
            auto compiler = m_compiler;
            emit_bytes(line, (uint8_t)op, arg);
            compiler->slots += getStackEffect((int)op);
            if(compiler->slots > (int)compiler->function->max_slots)
            {
                compiler->function->max_slots = (size_t)compiler->slots;
            }
        }

        void Emitter::emit_short(uint16_t line, uint16_t value)
        {
            emit_bytes(line, (uint8_t)((value >> 8) & 0xff), (uint8_t)(value & 0xff));
        }

        void Emitter::emit_byte_or_short(uint16_t line, uint8_t a, uint8_t b, uint16_t index)
        {
            if(index > UINT8_MAX)
            {
                emit_op(line, (OpCode)b);
                emit_short(line, (uint16_t)index);
            }
            else
            {
                emit_arged_op(line, (OpCode)a, (uint8_t)index);
            }
        }

        void Emitter::init_compiler(Compiler* compiler, FunctionType type)
        {
            compiler->locals.init(m_state);
            compiler->type = type;
            compiler->scope_depth = 0;
            compiler->enclosing = (Compiler*)m_compiler;
            compiler->skip_return = false;
            compiler->function = Function::make(m_state, m_module);
            compiler->loop_depth = 0;
            m_compiler = compiler;
            const char* name = getStateScannerFilename();
            if(m_compiler == nullptr)
            {
                compiler->function->name = String::copy(m_state, name, strlen(name));
            }
            m_chunk = &compiler->function->chunk;
            if(Optimizer::is_enabled(LITOPTSTATE_LINE_INFO))
            {
                m_chunk->m_haslineinfo = false;
            }
            if(type == LITFUNC_METHOD || type == LITFUNC_STATIC_METHOD || type == LITFUNC_CONSTRUCTOR)
            {
                compiler->locals.push(Local{ "this", 4, -1, false, false });
            }
            else
            {
                compiler->locals.push(Local{ "", 0, -1, false, false });
            }
            compiler->slots = 1;
            compiler->max_slots = 1;
        }

        void Emitter::emit_return(size_t line)
        {
            if(m_compiler->type == LITFUNC_CONSTRUCTOR)
            {
                emit_arged_op(line, OP_GET_LOCAL, 0);
                emit_op(line, OP_RETURN);
            }
            else if(m_prevwasexprstmt && m_chunk->m_count > 0)
            {
                m_chunk->m_count--;// Remove the OP_POP
                emit_op(line, OP_RETURN);
            }
            else
            {
                emit_ops(line, OP_NULL, OP_RETURN);
            }
        }

        Function* Emitter::end_compiler(String* name)
        {
            if(!m_compiler->skip_return)
            {
                emit_return(m_lastline);
                m_compiler->skip_return = true;
            }
            auto function = m_compiler->function;
            m_compiler->locals.release();
            m_compiler = (Compiler*)m_compiler->enclosing;
            m_chunk = m_compiler == nullptr ? nullptr : &m_compiler->function->chunk;
            if(name != nullptr)
            {
                function->name = name;
            }
        #ifdef LIT_TRACE_CHUNK
            lit_disassemble_chunk(&function->chunk, function->name->data(), nullptr);
        #endif
            return function;
        }

        void Emitter::begin_scope()
        {
            m_compiler->scope_depth++;
        }

        void Emitter::end_scope(uint16_t line)
        {
            m_compiler->scope_depth--;
            auto compiler = m_compiler;
            PCGenericArray<Local>* locals = &compiler->locals;
            while(locals->m_count > 0 && locals->m_values[locals->m_count - 1].depth > compiler->scope_depth)
            {
                if(locals->m_values[locals->m_count - 1].captured)
                {
                    emit_op(line, OP_CLOSE_UPVALUE);
                }
                else
                {
                    emit_op(line, OP_POP);
                }

                locals->m_count--;
            }
        }

        uint16_t Emitter::addConstant(size_t line, Value value)
        {
            size_t constant = m_chunk->addConstant(value);
            if(constant >= UINT16_MAX)
            {
                error(line, Error::LITERROR_TOO_MANY_CONSTANTS);
            }
            return constant;
        }

        size_t Emitter::emit_constant(size_t line, Value value)
        {
            size_t constant = m_chunk->addConstant(value);
            if(constant < UINT8_MAX)
            {
                emit_arged_op(line, OP_CONSTANT, constant);
            }
            else if(constant < UINT16_MAX)
            {
                emit_op(line, OP_CONSTANT_LONG);
                emit_short(line, constant);
            }
            else
            {
                error(line, Error::LITERROR_TOO_MANY_CONSTANTS);
            }

            return constant;
        }

        int Emitter::add_private(const char* name, size_t length, size_t line, bool constant)
        {
            auto privates = &m_privates;
            if(privates->m_count == UINT16_MAX)
            {
                error(line, Error::LITERROR_TOO_MANY_PRIVATES);
            }
            auto private_names = &m_module->private_names->m_values;
            auto key = private_names->find(name, length, String::makeHash(name, length));
            if(key != nullptr)
            {
                error(line, Error::LITERROR_VAR_REDEFINED, length, name);
                Value index;
                if(private_names->get(key, &index))
                {
                    return Object::toNumber(index);
                }
            }
            auto state = m_state;
            int index = (int)privates->m_count;
            privates->push(Private{ false, constant });
            private_names->set(String::copy(state, name, length), Object::toValue(index));
            m_module->private_count++;
            return index;
        }

        int Emitter::resolve_private(const char* name, size_t length, size_t line)
        {
            auto private_names = &m_module->private_names->m_values;
            auto key = private_names->find(name, length, String::makeHash(name, length));
            if(key != nullptr)
            {
                Value index;
                if(private_names->get(key, &index))
                {
                    int number_index = Object::toNumber(index);
                    if(!m_privates.m_values[number_index].initialized)
                    {
                        error(line, Error::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                    }
                    return number_index;
                }
            }
            return -1;
        }

        int Emitter::add_local(const char* name, size_t length, size_t line, bool constant)
        {
            auto compiler = m_compiler;
            auto locals = &compiler->locals;
            if(locals->m_count == UINT16_MAX)
            {
                error(line, Error::LITERROR_TOO_MANY_LOCALS);
            }
            for(int i = (int)locals->m_count - 1; i >= 0; i--)
            {
                auto local = &locals->m_values[i];
                if(local->depth != UINT16_MAX && local->depth < compiler->scope_depth)
                {
                    break;
                }
                if(length == local->length && memcmp(local->name, name, length) == 0)
                {
                    error(line, Error::LITERROR_VAR_REDEFINED, length, name);
                }
            }
            locals->push(Local{ name, length, UINT16_MAX, false, constant });
            return (int)locals->m_count - 1;
        }

        int Emitter::resolve_local(Compiler* compiler, const char* name, size_t length, size_t line)
        {
            auto locals = &compiler->locals;
            for(int i = (int)locals->m_count - 1; i >= 0; i--)
            {
                auto local = &locals->m_values[i];
                if(local->length == length && memcmp(local->name, name, length) == 0)
                {
                    if(local->depth == UINT16_MAX)
                    {
                        error(line, Error::LITERROR_VARIABLE_USED_IN_INIT, length, name);
                    }
                    return i;
                }
            }
            return -1;
        }

        int Emitter::add_upvalue(Compiler* compiler, uint8_t index, size_t line, bool is_local)
        {
            size_t upvalue_count = compiler->function->upvalue_count;
            for(size_t i = 0; i < upvalue_count; i++)
            {
                auto upvalue = &compiler->upvalues[i];
                if(upvalue->index == index && upvalue->isLocal == is_local)
                {
                    return i;
                }
            }
            if(upvalue_count == UINT16_COUNT)
            {
                error(line, Error::LITERROR_TOO_MANY_UPVALUES);
                return 0;
            }
            compiler->upvalues[upvalue_count].isLocal = is_local;
            compiler->upvalues[upvalue_count].index = index;
            return compiler->function->upvalue_count++;
        }

        int Emitter::resolve_upvalue(Compiler* compiler, const char* name, size_t length, size_t line)
        {
            if(compiler->enclosing == nullptr)
            {
                return -1;
            }
            int local = resolve_local((Compiler*)compiler->enclosing, name, length, line);
            if(local != -1)
            {
                ((Compiler*)compiler->enclosing)->locals.m_values[local].captured = true;
                return add_upvalue(compiler, (uint8_t)local, line, true);
            }
            int upvalue = resolve_upvalue((Compiler*)compiler->enclosing, name, length, line);
            if(upvalue != -1)
            {
                return add_upvalue(compiler, (uint8_t)upvalue, line, false);
            }
            return -1;
        }

        void Emitter::mark_local_initialized(size_t index)
        {
            m_compiler->locals.m_values[index].depth = m_compiler->scope_depth;
        }

        void Emitter::mark_private_initialized(size_t index)
        {
            m_privates.m_values[index].initialized = true;
        }

        size_t Emitter::emit_jump(OpCode code, size_t line)
        {
            emit_op(line, code);
            emit_bytes(line, 0xff, 0xff);
            return m_chunk->m_count - 2;
        }

        void Emitter::patch_jump(size_t offset, size_t line)
        {
            size_t jump = m_chunk->m_count - offset - 2;
            if(jump > UINT16_MAX)
            {
                error(line, Error::LITERROR_JUMP_TOO_BIG);
            }
            m_chunk->m_code[offset] = (jump >> 8) & 0xff;
            m_chunk->m_code[offset + 1] = jump & 0xff;
        }

        void Emitter::emit_loop(size_t start, size_t line)
        {
            emit_op(line, OP_JUMP_BACK);
            size_t offset = m_chunk->m_count - start + 2;
            if(offset > UINT16_MAX)
            {
                error(line, Error::LITERROR_JUMP_TOO_BIG);
            }
            emit_short(line, offset);
        }

        void Emitter::patch_loop_jumps(PCGenericArray<size_t>* breaks, size_t line)
        {
            for(size_t i = 0; i < breaks->m_count; i++)
            {
                patch_jump(breaks->m_values[i], line);
            }
            breaks->release();
        }

        bool Emitter::emit_parameters(PCGenericArray<ExprFuncParam>* parameters, size_t line)
        {
            for(size_t i = 0; i < parameters->m_count; i++)
            {
                auto parameter = &parameters->m_values[i];
                int index = add_local(parameter->name, parameter->length, line, false);
                mark_local_initialized(index);
                // Vararg ...
                if(parameter->length == 3 && memcmp(parameter->name, "...", 3) == 0)
                {
                    return true;
                }

                if(parameter->default_value != nullptr)
                {
                    emit_byte_or_short(line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
                    size_t jump = emit_jump(OP_NULL_OR, line);

                    emit_expression(parameter->default_value);
                    patch_jump(jump, line);
                    emit_byte_or_short(line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                    emit_op(line, OP_POP);
                }
            }

            return false;
        }

        void Emitter::resolve_statement(Expression* statement)
        {
            if(statement == nullptr)
            {
                return;
            }
            switch(statement->type)
            {
                case Expression::Type::VarDecl:
                    {
                        auto stmt = (StmtVar*)statement;
                        mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, stmt->constant));
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        auto stmt = (StmtFunction*)statement;
                        if(!stmt->exported)
                        {
                            mark_private_initialized(add_private(stmt->name, stmt->length, statement->line, false));
                        }
                    }
                    break;
                case Expression::Type::ClassDecl:
                case Expression::Type::Block:
                case Expression::Type::ForLoop:
                case Expression::Type::WhileLoop:
                case Expression::Type::IfClause:
                case Expression::Type::ContinueClause:
                case Expression::Type::BreakClause:
                case Expression::Type::ReturnClause:
                case Expression::Type::MethodDecl:
                case Expression::Type::FieldDecl:
                case Expression::Type::Expression:
                    {
                    }
                    break;
                default:
                    break;
            }
        }

        void Emitter::emit_expression(Expression* expression)
        {
            switch(expression->type)
            {
                case Expression::Type::Literal:
                    {
                        Value value = ((ExprLiteral*)expression)->value;
                        if(Object::isNumber(value) || Object::isString(value))
                        {
                            emit_constant(expression->line, value);
                        }
                        else if(Object::isBool(value))
                        {
                            emit_op(expression->line, Object::asBool(value) ? OP_TRUE : OP_FALSE);
                        }
                        else if(Object::isNull(value))
                        {
                            emit_op(expression->line, OP_NULL);
                        }
                        else
                        {
                            /* UNREACHABLE */
                        }
                    }
                    break;
                case Expression::Type::Binary:
                    {
                        auto expr = (ExprBinary*)expression;
                        emit_expression(expr->left);
                        if(expr->right == nullptr)
                        {
                            break;
                        }
                        TokenType op = expr->op;
                        if(op == LITTOK_AMPERSAND_AMPERSAND || op == LITTOK_BAR_BAR || op == LITTOK_QUESTION_QUESTION)
                        {
                            size_t jump = emit_jump(op == LITTOK_BAR_BAR ? OP_OR : (op == LITTOK_QUESTION_QUESTION ? OP_NULL_OR : OP_AND),
                                                  m_lastline);
                            emit_expression(expr->right);
                            patch_jump(jump, m_lastline);
                            break;
                        }
                        emit_expression(expr->right);
                        switch(op)
                        {
                            case LITTOK_PLUS:
                                {
                                    emit_op(expression->line, OP_ADD);
                                }
                                break;
                            case LITTOK_MINUS:
                                {
                                    emit_op(expression->line, OP_SUBTRACT);
                                }
                                break;
                            case LITTOK_STAR:
                                {
                                    emit_op(expression->line, OP_MULTIPLY);
                                }
                                break;
                            case LITTOK_STAR_STAR:
                                {
                                    emit_op(expression->line, OP_POWER);
                                }
                                break;
                            case LITTOK_SLASH:
                                {
                                    emit_op(expression->line, OP_DIVIDE);
                                }
                                break;
                            case LITTOK_SHARP:
                                {
                                    emit_op(expression->line, OP_FLOOR_DIVIDE);
                                }
                                break;
                            case LITTOK_PERCENT:
                                {
                                    emit_op(expression->line, OP_MOD);
                                }
                                break;
                            case LITTOK_IS:
                                {
                                    emit_op(expression->line, OP_IS);
                                }
                                break;
                            case LITTOK_EQUAL_EQUAL:
                                {
                                    emit_op(expression->line, OP_EQUAL);
                                }
                                break;
                            case LITTOK_BANG_EQUAL:
                                {
                                    emit_ops(expression->line, OP_EQUAL, OP_NOT);
                                }
                                break;
                            case LITTOK_GREATER:
                                {
                                    emit_op(expression->line, OP_GREATER);
                                }
                                break;
                            case LITTOK_GREATER_EQUAL:
                                {
                                    emit_op(expression->line, OP_GREATER_EQUAL);
                                }
                                break;
                            case LITTOK_LESS:
                                {
                                    emit_op(expression->line, OP_LESS);
                                }
                                break;
                            case LITTOK_LESS_EQUAL:
                                {
                                    emit_op(expression->line, OP_LESS_EQUAL);
                                }
                                break;
                            case LITTOK_LESS_LESS:
                                {
                                    emit_op(expression->line, OP_LSHIFT);
                                }
                                break;
                            case LITTOK_GREATER_GREATER:
                                {
                                    emit_op(expression->line, OP_RSHIFT);
                                }
                                break;
                            case LITTOK_BAR:
                                {
                                    emit_op(expression->line, OP_BOR);
                                }
                                break;
                            case LITTOK_AMPERSAND:
                                {
                                    emit_op(expression->line, OP_BAND);
                                }
                                break;
                            case LITTOK_CARET:
                                {
                                    emit_op(expression->line, OP_BXOR);
                                }
                                break;
                            default:
                                {
                                    /* UNREACHABLE */
                                }
                            break;
                        }
                    }
                    break;
                case Expression::Type::Unary:
                    {
                        auto expr = (ExprUnary*)expression;
                        emit_expression(expr->right);
                        switch(expr->op)
                        {
                            case LITTOK_MINUS:
                                {
                                    emit_op(expression->line, OP_NEGATE);
                                }
                                break;
                            case LITTOK_BANG:
                                {
                                    emit_op(expression->line, OP_NOT);
                                }
                                break;
                            case LITTOK_TILDE:
                                {
                                    emit_op(expression->line, OP_BNOT);
                                }
                                break;
                            default:
                                {
                                    /* UNREACHABLE */
                                }
                                break;
                        }
                    }
                    break;
                case Expression::Type::Variable:
                    {
                        auto expr = (ExprVar*)expression;
                        bool ref = m_emitreference > 0;
                        if(ref)
                        {
                            m_emitreference--;
                        }
                        int index = resolve_local(m_compiler, expr->name, expr->length, expression->line);
                        if(index == -1)
                        {
                            index = resolve_upvalue(m_compiler, expr->name, expr->length, expression->line);
                            if(index == -1)
                            {
                                index = resolve_private(expr->name, expr->length, expression->line);
                                if(index == -1)
                                {
                                    emit_op(expression->line, ref ? OP_REFERENCE_GLOBAL : OP_GET_GLOBAL);
                                    emit_short(expression->line,
                                               addConstant(expression->line,
                                                            String::copy(m_state, expr->name, expr->length)->asValue()));
                                }
                                else
                                {
                                    if(ref)
                                    {
                                        emit_op(expression->line, OP_REFERENCE_PRIVATE);
                                        emit_short(expression->line, index);
                                    }
                                    else
                                    {
                                        emit_byte_or_short(expression->line, OP_GET_PRIVATE, OP_GET_PRIVATE_LONG, index);
                                    }
                                }
                            }
                            else
                            {
                                emit_arged_op(expression->line, ref ? OP_REFERENCE_UPVALUE : OP_GET_UPVALUE, (uint8_t)index);
                            }
                        }
                        else
                        {
                            if(ref)
                            {
                                emit_op(expression->line, OP_REFERENCE_LOCAL);
                                emit_short(expression->line, index);
                            }
                            else
                            {
                                emit_byte_or_short(expression->line, OP_GET_LOCAL, OP_GET_LOCAL_LONG, index);
                            }
                        }
                    }
                    break;
                case Expression::Type::Assign:
                    {
                        auto expr = (ExprAssign*)expression;
                        if(expr->to->type == Expression::Type::Variable)
                        {
                            emit_expression(expr->value);
                            auto e = (ExprVar*)expr->to;
                            int index = resolve_local(m_compiler, e->name, e->length, expr->to->line);
                            if(index == -1)
                            {
                                index = resolve_upvalue(m_compiler, e->name, e->length, expr->to->line);
                                if(index == -1)
                                {
                                    index = resolve_private(e->name, e->length, expr->to->line);
                                    if(index == -1)
                                    {
                                        emit_op(expression->line, OP_SET_GLOBAL);
                                        emit_short(expression->line,
                                                   addConstant(expression->line,
                                                                String::copy(m_state, e->name, e->length)->asValue()));
                                    }
                                    else
                                    {
                                        if(m_privates.m_values[index].constant)
                                        {
                                            error(expression->line, Error::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                        }
                                        emit_byte_or_short(expression->line, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                                    }
                                }
                                else
                                {
                                    emit_arged_op(expression->line, OP_SET_UPVALUE, (uint8_t)index);
                                }
                                break;
                            }
                            else
                            {
                                if(m_compiler->locals.m_values[index].constant)
                                {
                                    error(expression->line, Error::LITERROR_CONSTANT_MODIFIED, e->length, e->name);
                                }

                                emit_byte_or_short(expression->line, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                            }
                        }
                        else if(expr->to->type == Expression::Type::Get)
                        {
                            emit_expression(expr->value);
                            auto e = (ExprIndexGet*)expr->to;
                            emit_expression(e->where);
                            emit_expression(expr->value);
                            emit_constant(m_lastline, String::copy(m_state, e->name, e->length)->asValue());
                            emit_ops(m_lastline, OP_SET_FIELD, OP_POP);
                        }
                        else if(expr->to->type == Expression::Type::Subscript)
                        {
                            auto e = (ExprSubscript*)expr->to;
                            emit_expression(e->array);
                            emit_expression(e->index);
                            emit_expression(expr->value);
                            emit_op(m_lastline, OP_SUBSCRIPT_SET);
                        }
                        else if(expr->to->type == Expression::Type::Reference)
                        {
                            emit_expression(expr->value);
                            emit_expression(((ExprReference*)expr->to)->to);
                            emit_op(expression->line, OP_SET_REFERENCE);
                        }
                        else
                        {
                            error(expression->line, Error::LITERROR_INVALID_ASSIGMENT_TARGET);
                        }
                    }
                    break;
                case Expression::Type::Call:
                    {
                        auto expr = (ExprCall*)expression;
                        bool method = expr->callee->type == Expression::Type::Get;
                        bool super = expr->callee->type == Expression::Type::Super;
                        if(method)
                        {
                            ((ExprIndexGet*)expr->callee)->ignore_emit = true;
                        }
                        else if(super)
                        {
                            ((ExprSuper*)expr->callee)->ignore_emit = true;
                        }
                        emit_expression(expr->callee);
                        if(super)
                        {
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                        }
                        for(size_t i = 0; i < expr->args.m_count; i++)
                        {
                            auto e = expr->args.m_values[i];
                            if(e->type == Expression::Type::Variable)
                            {
                                auto ee = (ExprVar*)e;
                                // Vararg ...
                                if(ee->length == 3 && memcmp(ee->name, "...", 3) == 0)
                                {
                                    emit_arged_op(e->line, OP_VARARG,
                                                  resolve_local(m_compiler, "...", 3, expression->line));
                                    break;
                                }
                            }
                            emit_expression(e);
                        }
                        if(method || super)
                        {
                            if(method)
                            {
                                auto e = (ExprIndexGet*)expr->callee;
                                emit_varying_op(expression->line,
                                                ((ExprIndexGet*)expr->callee)->ignore_result ? OP_INVOKE_IGNORING : OP_INVOKE,
                                                (uint8_t)expr->args.m_count);
                                emit_short(m_lastline,
                                           addConstant(m_lastline,
                                                        String::copy(m_state, e->name, e->length)->asValue()));
                            }
                            else
                            {
                                auto e = (ExprSuper*)expr->callee;
                                uint8_t index = resolve_upvalue(m_compiler, "super", 5, m_lastline);
                                emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                                emit_varying_op(m_lastline,
                                                ((ExprSuper*)expr->callee)->ignore_result ? OP_INVOKE_SUPER_IGNORING : OP_INVOKE_SUPER,
                                                (uint8_t)expr->args.m_count);
                                emit_short(m_lastline, addConstant(m_lastline, e->method->asValue()));
                            }
                        }
                        else
                        {
                            emit_varying_op(expression->line, OP_CALL, (uint8_t)expr->args.m_count);
                        }
                        if(method)
                        {
                            auto get = expr->callee;
                            while(get != nullptr)
                            {
                                if(get->type == Expression::Type::Get)
                                {
                                    auto getter = (ExprIndexGet*)get;
                                    if(getter->jump > 0)
                                    {
                                        patch_jump(getter->jump, m_lastline);
                                    }
                                    get = getter->where;
                                }
                                else if(get->type == Expression::Type::Subscript)
                                {
                                    get = ((ExprSubscript*)get)->array;
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        if(expr->objexpr == nullptr)
                        {
                            return;
                        }
                        auto objinit = (ExprObject*)expr->objexpr;
                        for(size_t i = 0; i < objinit->values.m_count; i++)
                        {
                            auto e = objinit->values.m_values[i];
                            m_lastline = e->line;
                            emit_constant(m_lastline, objinit->keys.m_values[i]);
                            emit_expression(e);
                            emit_op(m_lastline, OP_PUSH_OBJECT_FIELD);
                        }
                    }
                    break;
                case Expression::Type::Get:
                    {
                        auto expr = (ExprIndexGet*)expression;
                        bool ref = m_emitreference > 0;
                        if(ref)
                        {
                            m_emitreference--;
                        }
                        emit_expression(expr->where);
                        if(expr->jump == 0)
                        {
                            expr->jump = emit_jump(OP_JUMP_IF_NULL, m_lastline);
                            if(!expr->ignore_emit)
                            {
                                emit_constant(m_lastline,
                                              String::copy(m_state, expr->name, expr->length)->asValue());
                                emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                            }
                            patch_jump(expr->jump, m_lastline);
                        }
                        else if(!expr->ignore_emit)
                        {
                            emit_constant(m_lastline, String::copy(m_state, expr->name, expr->length)->asValue());
                            emit_op(m_lastline, ref ? OP_REFERENCE_FIELD : OP_GET_FIELD);
                        }
                    }
                    break;
                case Expression::Type::Set:
                    {
                        auto expr = (ExprIndexSet*)expression;
                        emit_expression(expr->where);
                        emit_expression(expr->value);
                        emit_constant(m_lastline, String::copy(m_state, expr->name, expr->length)->asValue());
                        emit_op(m_lastline, OP_SET_FIELD);
                    }
                    break;
                case Expression::Type::Lambda:
                    {
                        auto expr = (ExprLambda*)expression;
                        auto name = String::format(m_state, "[lambda @:@]", m_module->name->asValue(),
                                                                      String::stringNumberToString(m_state, expression->line));
                        Compiler compiler;
                        init_compiler(&compiler, LITFUNC_REGULAR);
                        begin_scope();
                        bool vararg = emit_parameters(&expr->parameters, expression->line);
                        if(expr->body != nullptr)
                        {
                            bool single_expression = expr->body->type == Expression::Type::Expression;
                            if(single_expression)
                            {
                                compiler.skip_return = true;
                                ((ExprStatement*)expr->body)->pop = false;
                            }
                            emit_statement(expr->body);
                            if(single_expression)
                            {
                                emit_op(m_lastline, OP_RETURN);
                            }
                        }
                        end_scope(m_lastline);
                        auto function = end_compiler(name);
                        function->arg_count = expr->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(m_lastline, OP_CLOSURE);
                            emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                            for(size_t i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(m_lastline, function->asValue());
                        }
                    }
                    break;
                case Expression::Type::Array:
                    {
                        auto expr = (ExprArray*)expression;
                        emit_op(expression->line, OP_ARRAY);
                        for(size_t i = 0; i < expr->values.m_count; i++)
                        {
                            emit_expression(expr->values.m_values[i]);
                            emit_op(m_lastline, OP_PUSH_ARRAY_ELEMENT);
                        }
                    }
                    break;
                case Expression::Type::Object:
                    {
                        auto expr = (ExprObject*)expression;
                        emit_op(expression->line, OP_OBJECT);
                        for(size_t i = 0; i < expr->values.m_count; i++)
                        {
                            emit_constant(m_lastline, expr->keys.m_values[i]);
                            emit_expression(expr->values.m_values[i]);
                            emit_op(m_lastline, OP_PUSH_OBJECT_FIELD);
                        }
                    }
                    break;
                case Expression::Type::Subscript:
                    {
                        auto expr = (ExprSubscript*)expression;
                        emit_expression(expr->array);
                        emit_expression(expr->index);
                        emit_op(expression->line, OP_SUBSCRIPT_GET);
                    }
                    break;
                case Expression::Type::This:
                    {
                        FunctionType type = m_compiler->type;
                        if(type == LITFUNC_STATIC_METHOD)
                        {
                            error(expression->line, Error::LITERROR_THIS_MISSUSE, "in static methods");
                        }
                        if(type == LITFUNC_CONSTRUCTOR || type == LITFUNC_METHOD)
                        {
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                        }
                        else
                        {
                            if(m_compiler->enclosing == nullptr)
                            {
                                error(expression->line, Error::LITERROR_THIS_MISSUSE, "in functions outside of any class");
                            }
                            else
                            {
                                int local = resolve_local((Compiler*)m_compiler->enclosing, "this", 4, expression->line);
                                emit_arged_op(expression->line, OP_GET_UPVALUE,
                                              add_upvalue(m_compiler, local, expression->line, true));
                            }
                        }
                    }
                    break;
                case Expression::Type::Super:
                    {
                        if(m_compiler->type == LITFUNC_STATIC_METHOD)
                        {
                            error(expression->line, Error::LITERROR_SUPER_MISSUSE, "in static methods");
                        }
                        else if(!m_havesuperclass)
                        {
                            error(expression->line, Error::LITERROR_NO_SUPER, m_classname->data());
                        }
                        auto expr = (ExprSuper*)expression;
                        if(!expr->ignore_emit)
                        {
                            uint8_t index = resolve_upvalue(m_compiler, "super", 5, m_lastline);
                            emit_arged_op(expression->line, OP_GET_LOCAL, 0);
                            emit_arged_op(expression->line, OP_GET_UPVALUE, index);
                            emit_op(expression->line, OP_GET_SUPER_METHOD);
                            emit_short(expression->line, addConstant(expression->line, expr->method->asValue()));
                        }
                    }
                    break;
                case Expression::Type::Range:
                    {
                        auto expr = (ExprRange*)expression;
                        emit_expression(expr->to);
                        emit_expression(expr->from);
                        emit_op(expression->line, OP_RANGE);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        auto expr = (ExprIfClause*)expression;
                        emit_expression(expr->condition);
                        uint64_t else_jump = emit_jump(OP_JUMP_IF_FALSE, expression->line);
                        emit_expression(expr->if_branch);
                        uint64_t end_jump = emit_jump(OP_JUMP, m_lastline);
                        patch_jump(else_jump, expr->else_branch->line);
                        emit_expression(expr->else_branch);

                        patch_jump(end_jump, m_lastline);
                    }
                    break;
                case Expression::Type::Interpolation:
                    {
                        auto expr = (ExprInterpolation*)expression;
                        emit_op(expression->line, OP_ARRAY);
                        for(size_t i = 0; i < expr->expressions.m_count; i++)
                        {
                            emit_expression(expr->expressions.m_values[i]);
                            emit_op(m_lastline, OP_PUSH_ARRAY_ELEMENT);
                        }
                        emit_varying_op(m_lastline, OP_INVOKE, 0);
                        emit_short(m_lastline,
                                   addConstant(m_lastline, String::internValue(m_state, "join")));
                    }
                    break;
                case Expression::Type::Reference:
                    {
                        auto to = ((ExprReference*)expression)->to;
                        if(to->type != Expression::Type::Variable && to->type != Expression::Type::Get && to->type != Expression::Type::This && to->type != Expression::Type::Super)
                        {
                            error(expression->line, Error::LITERROR_INVALID_REFERENCE_TARGET);
                            break;
                        }
                        int old = m_emitreference;
                        m_emitreference++;
                        emit_expression(to);
                        m_emitreference = old;
                    }
                    break;
                default:
                    {
                        error(expression->line, Error::LITERROR_UNKNOWN_EXPRESSION, (int)expression->type);
                    }
                    break;
            }
        }

        bool Emitter::emit_statement(Expression* statement)
        {
            StmtClass* clstmt;
            Compiler compiler;
            Expression* expression;
            Expression* e;
            ExprStatement* expr;
            Field* field;
            StmtField* fieldstmt;
            Function* function;
            Function* getter;
            Function* setter;
            StmtFunction* funcstmt;
            Local* local;
            PCGenericArray<Local>* locals;
            StmtMethod* mthstmt;
            Expression* blockstmt;
            Expression* s;
            Expression::List* statements;
            String* name;
            StmtVar* var;
            StmtVar* varstmt;
            StmtForLoop* forstmt;
            StmtWhileLoop* whilestmt;
            StmtIfClause* ifstmt;
            bool constructor;
            bool isexport;
            bool isprivate;
            bool islocal;
            bool vararg;
            int depth;
            int ii;
            int index;
            size_t start;
            size_t else_jump;
            size_t exit_jump;
            size_t body_jump;
            size_t end_jump;
            size_t i;
            size_t increment_start;
            size_t localcnt;
            size_t iterator;
            size_t line;
            size_t sequence;
            uint16_t local_count;
            uint8_t super;
            uint64_t* end_jumps;
            if(statement == nullptr)
            {
                return false;
            }
            switch(statement->type)
            {
                case Expression::Type::Expression:
                    {
                        expr = (ExprStatement*)statement;
                        emit_expression(expr->expression);
                        if(expr->pop)
                        {
                            emit_op(statement->line, OP_POP);
                        }
                    }
                    break;
                case Expression::Type::Block:
                    {
                        statements = &((StmtBlock*)statement)->statements;
                        begin_scope();
                        {
                            for(i = 0; i < statements->m_count; i++)
                            {
                                blockstmt = statements->m_values[i];

                                if(emit_statement(blockstmt))
                                {
                                    break;
                                }
                            }
                        }
                        end_scope(m_lastline);
                    }
                    break;
                case Expression::Type::VarDecl:
                    {
                        varstmt = (StmtVar*)statement;
                        line = statement->line;
                        isprivate = m_compiler->enclosing == nullptr && m_compiler->scope_depth == 0;
                        index = isprivate ? resolve_private(varstmt->name, varstmt->length, statement->line) :
                                              add_local(varstmt->name, varstmt->length, statement->line, varstmt->constant);
                        if(varstmt->valexpr == nullptr)
                        {
                            emit_op(line, OP_NULL);
                        }
                        else
                        {
                            emit_expression(varstmt->valexpr);
                        }
                        if(isprivate)
                        {
                            mark_private_initialized(index);
                        }
                        else
                        {
                            mark_local_initialized(index);
                        }
                        emit_byte_or_short(statement->line, isprivate ? OP_SET_PRIVATE : OP_SET_LOCAL,
                                           isprivate ? OP_SET_PRIVATE_LONG : OP_SET_LOCAL_LONG, index);
                        if(isprivate)
                        {
                            // Privates don't live on stack, so we need to pop them manually
                            emit_op(statement->line, OP_POP);
                        }
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        ifstmt = (StmtIfClause*)statement;
                        else_jump = 0;
                        end_jump = 0;
                        if(ifstmt->condition == nullptr)
                        {
                            else_jump = emit_jump(OP_JUMP, statement->line);
                        }
                        else
                        {
                            emit_expression(ifstmt->condition);
                            else_jump = emit_jump(OP_JUMP_IF_FALSE, statement->line);
                            emit_statement(ifstmt->if_branch);
                            end_jump = emit_jump(OP_JUMP, m_lastline);
                        }
                        size_t jmpcnt = (ifstmt->elseif_branches == nullptr ? 1 : ifstmt->elseif_branches->m_count);
                        //uint64_t end_jumps[ifstmt->elseif_branches == nullptr ? 1 : ifstmt->elseif_branches->m_count];
                        end_jumps = (uint64_t*)calloc(jmpcnt + 1, sizeof(uint64_t));
                        memset(end_jumps, 0, jmpcnt);
                        if(ifstmt->elseif_branches != nullptr)
                        {
                            for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                            {
                                e = ifstmt->elseif_conditions->m_values[i];
                                if(e == nullptr)
                                {
                                    continue;
                                }
                                patch_jump(else_jump, e->line);
                                emit_expression(e);
                                else_jump = emit_jump(OP_JUMP_IF_FALSE, m_lastline);
                                emit_statement(ifstmt->elseif_branches->m_values[i]);
                                end_jumps[i] = emit_jump(OP_JUMP, m_lastline);
                            }
                        }
                        if(ifstmt->else_branch != nullptr)
                        {
                            patch_jump(else_jump, ifstmt->else_branch->line);
                            emit_statement(ifstmt->else_branch);
                        }
                        else
                        {
                            patch_jump(else_jump, m_lastline);
                        }
                        if(end_jump != 0)
                        {
                            patch_jump(end_jump, m_lastline);
                        }
                        if(ifstmt->elseif_branches != nullptr)
                        {
                            for(i = 0; i < ifstmt->elseif_branches->m_count; i++)
                            {
                                if(ifstmt->elseif_branches->m_values[i] == nullptr)
                                {
                                    continue;
                                }
                                patch_jump(end_jumps[i], ifstmt->elseif_branches->m_values[i]->line);
                            }
                        }
                        free(end_jumps);
                    }
                    break;
                case Expression::Type::WhileLoop:
                    {
                        whilestmt = (StmtWhileLoop*)statement;
                        start = m_chunk->m_count;
                        m_loopstart = start;
                        m_compiler->loop_depth++;
                        emit_expression(whilestmt->condition);
                        exit_jump = emit_jump(OP_JUMP_IF_FALSE, statement->line);
                        emit_statement(whilestmt->body);
                        patch_loop_jumps(&m_continues, m_lastline);
                        emit_loop(start, m_lastline);
                        patch_jump(exit_jump, m_lastline);
                        patch_loop_jumps(&m_breaks, m_lastline);
                        m_compiler->loop_depth--;
                    }
                    break;
                case Expression::Type::ForLoop:
                    {
                        forstmt = (StmtForLoop*)statement;
                        begin_scope();
                        m_compiler->loop_depth++;
                        if(forstmt->c_style)
                        {
                            if(forstmt->var != nullptr)
                            {
                                emit_statement(forstmt->var);
                            }
                            else if(forstmt->exprinit != nullptr)
                            {
                                emit_expression(forstmt->exprinit);
                            }
                            start = m_chunk->m_count;
                            exit_jump = 0;
                            if(forstmt->condition != nullptr)
                            {
                                emit_expression(forstmt->condition);
                                exit_jump = emit_jump(OP_JUMP_IF_FALSE, m_lastline);
                            }
                            if(forstmt->increment != nullptr)
                            {
                                body_jump = emit_jump(OP_JUMP, m_lastline);
                                increment_start = m_chunk->m_count;
                                emit_expression(forstmt->increment);
                                emit_op(m_lastline, OP_POP);
                                emit_loop(start, m_lastline);
                                start = increment_start;
                                patch_jump(body_jump, m_lastline);
                            }
                            m_loopstart = start;
                            begin_scope();
                            if(forstmt->body != nullptr)
                            {
                                if(forstmt->body->type == Expression::Type::Block)
                                {
                                    statements = &((StmtBlock*)forstmt->body)->statements;
                                    for(i = 0; i < statements->m_count; i++)
                                    {
                                        emit_statement(statements->m_values[i]);
                                    }
                                }
                                else
                                {
                                    emit_statement(forstmt->body);
                                }
                            }
                            patch_loop_jumps(&m_continues, m_lastline);
                            end_scope(m_lastline);
                            emit_loop(start, m_lastline);
                            if(forstmt->condition != nullptr)
                            {
                                patch_jump(exit_jump, m_lastline);
                            }
                        }
                        else
                        {
                            sequence = add_local("seq ", 4, statement->line, false);
                            mark_local_initialized(sequence);
                            emit_expression(forstmt->condition);
                            emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, sequence);
                            iterator = add_local("iter ", 5, statement->line, false);
                            mark_local_initialized(iterator);
                            emit_op(m_lastline, OP_NULL);
                            emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                            start = m_chunk->m_count;
                            m_loopstart = m_chunk->m_count;
                            // iter = seq.iterator(iter)
                            emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                            emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                            emit_varying_op(m_lastline, OP_INVOKE, 1);
                            emit_short(m_lastline,
                                       addConstant(m_lastline, String::internValue(m_state, "iterator")));
                            emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, iterator);
                            // If iter is null, just get out of the loop
                            exit_jump = emit_jump(OP_JUMP_IF_NULL_POPPING, m_lastline);
                            begin_scope();
                            // var i = seq.iteratorValue(iter)
                            var = (StmtVar*)forstmt->var;
                            localcnt = add_local(var->name, var->length, statement->line, false);
                            mark_local_initialized(localcnt);
                            emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, sequence);
                            emit_byte_or_short(m_lastline, OP_GET_LOCAL, OP_GET_LOCAL_LONG, iterator);
                            emit_varying_op(m_lastline, OP_INVOKE, 1);
                            emit_short(m_lastline,
                                       addConstant(m_lastline, String::internValue(m_state, "iteratorValue")));
                            emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, localcnt);
                            if(forstmt->body != nullptr)
                            {
                                if(forstmt->body->type == Expression::Type::Block)
                                {
                                    statements = &((StmtBlock*)forstmt->body)->statements;
                                    for(i = 0; i < statements->m_count; i++)
                                    {
                                        emit_statement(statements->m_values[i]);
                                    }
                                }
                                else
                                {
                                    emit_statement(forstmt->body);
                                }
                            }
                            patch_loop_jumps(&m_continues, m_lastline);
                            end_scope(m_lastline);
                            emit_loop(start, m_lastline);
                            patch_jump(exit_jump, m_lastline);
                        }
                        patch_loop_jumps(&m_breaks, m_lastline);
                        end_scope(m_lastline);
                        m_compiler->loop_depth--;
                    }
                    break;

                case Expression::Type::ContinueClause:
                {
                    if(m_compiler->loop_depth == 0)
                    {
                        error(statement->line, Error::LITERROR_LOOP_JUMP_MISSUSE, "continue");
                    }
                    m_continues.push(emit_jump(OP_JUMP, statement->line));
                    break;
                }

                case Expression::Type::BreakClause:
                    {
                        if(m_compiler->loop_depth == 0)
                        {
                            error(statement->line, Error::LITERROR_LOOP_JUMP_MISSUSE, "break");
                        }
                        emit_op(statement->line, OP_POP_LOCALS);
                        depth = m_compiler->scope_depth;
                        local_count = 0;
                        locals = &m_compiler->locals;
                        for(ii = locals->m_count - 1; ii >= 0; ii--)
                        {
                            local = &locals->m_values[ii];
                            if(local->depth < depth)
                            {
                                break;
                            }

                            if(!local->captured)
                            {
                                local_count++;
                            }
                        }
                        emit_short(statement->line, local_count);
                        m_breaks.push(emit_jump(OP_JUMP, statement->line));
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        funcstmt = (StmtFunction*)statement;
                        isexport = funcstmt->exported;
                        isprivate = !isexport && m_compiler->enclosing == nullptr && m_compiler->scope_depth == 0;
                        islocal = !(isexport || isprivate);
                        index = 0;
                        if(!isexport)
                        {
                            index = isprivate ? resolve_private(funcstmt->name, funcstmt->length, statement->line) :
                                              add_local(funcstmt->name, funcstmt->length, statement->line, false);
                        }
                        name = String::copy(m_state, funcstmt->name, funcstmt->length);
                        if(islocal)
                        {
                            mark_local_initialized(index);
                        }
                        else if(isprivate)
                        {
                            mark_private_initialized(index);
                        }
                        init_compiler(&compiler, LITFUNC_REGULAR);
                        begin_scope();
                        vararg = emit_parameters(&funcstmt->parameters, statement->line);
                        emit_statement(funcstmt->body);
                        end_scope(m_lastline);
                        function = end_compiler(name);
                        function->arg_count = funcstmt->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(m_lastline, OP_CLOSURE);
                            emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                            for(i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(m_lastline, function->asValue());
                        }
                        if(isexport)
                        {
                            emit_op(m_lastline, OP_SET_GLOBAL);
                            emit_short(m_lastline, addConstant(m_lastline, name->asValue()));
                        }
                        else if(isprivate)
                        {
                            emit_byte_or_short(m_lastline, OP_SET_PRIVATE, OP_SET_PRIVATE_LONG, index);
                        }
                        else
                        {
                            emit_byte_or_short(m_lastline, OP_SET_LOCAL, OP_SET_LOCAL_LONG, index);
                        }
                        emit_op(m_lastline, OP_POP);
                    }
                    break;
                case Expression::Type::ReturnClause:
                    {
                        if(m_compiler->type == LITFUNC_CONSTRUCTOR)
                        {
                            error(statement->line, Error::LITERROR_RETURN_FROM_CONSTRUCTOR);
                        }
                        expression = ((StmtReturn*)statement)->expression;
                        if(expression == nullptr)
                        {
                            emit_op(m_lastline, OP_NULL);
                        }
                        else
                        {
                            emit_expression(expression);
                        }
                        emit_op(m_lastline, OP_RETURN);
                        if(m_compiler->scope_depth == 0)
                        {
                            m_compiler->skip_return = true;
                        }
                        return true;
                    }
                    break;
                case Expression::Type::MethodDecl:
                    {
                        mthstmt = (StmtMethod*)statement;
                        constructor = mthstmt->name->length() == (sizeof(LIT_NAME_CONSTRUCTOR)-1) && memcmp(mthstmt->name->data(), LIT_NAME_CONSTRUCTOR, strlen(LIT_NAME_CONSTRUCTOR)-1) == 0;
                        if(constructor && mthstmt->is_static)
                        {
                            error(statement->line, Error::LITERROR_STATIC_CONSTRUCTOR);
                        }
                        init_compiler(&compiler,
                                      constructor ? LITFUNC_CONSTRUCTOR : (mthstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD));
                        begin_scope();
                        vararg = emit_parameters(&mthstmt->parameters, statement->line);
                        emit_statement(mthstmt->body);
                        end_scope(m_lastline);
                        function = end_compiler(String::format(m_state, "@:@", m_classname->asValue(), mthstmt->name->asValue()));
                        function->arg_count = mthstmt->parameters.m_count;
                        function->max_slots += function->arg_count;
                        function->vararg = vararg;
                        if(function->upvalue_count > 0)
                        {
                            emit_op(m_lastline, OP_CLOSURE);
                            emit_short(m_lastline, addConstant(m_lastline, function->asValue()));
                            for(i = 0; i < function->upvalue_count; i++)
                            {
                                emit_bytes(m_lastline, compiler.upvalues[i].isLocal ? 1 : 0, compiler.upvalues[i].index);
                            }
                        }
                        else
                        {
                            emit_constant(m_lastline, function->asValue());
                        }
                        emit_op(m_lastline, mthstmt->is_static ? OP_STATIC_FIELD : OP_METHOD);
                        emit_short(m_lastline, addConstant(statement->line, mthstmt->name->asValue()));

                    }
                    break;
                case Expression::Type::ClassDecl:
                    {
                        clstmt = (StmtClass*)statement;
                        m_classname = clstmt->name;
                        if(clstmt->parent != nullptr)
                        {
                            emit_op(m_lastline, OP_GET_GLOBAL);
                            emit_short(m_lastline, addConstant(m_lastline, clstmt->parent->asValue()));
                        }
                        emit_op(statement->line, OP_CLASS);
                        emit_short(m_lastline, addConstant(m_lastline, clstmt->name->asValue()));
                        if(clstmt->parent != nullptr)
                        {
                            emit_op(m_lastline, OP_INHERIT);
                            m_havesuperclass = true;
                            begin_scope();
                            super = add_local("super", 5, m_lastline, false);
                            
                            mark_local_initialized(super);
                        }
                        for(i = 0; i < clstmt->fields.m_count; i++)
                        {
                            s = clstmt->fields.m_values[i];
                            if(s->type == Expression::Type::VarDecl)
                            {
                                var = (StmtVar*)s;
                                emit_expression(var->valexpr);
                                emit_op(statement->line, OP_STATIC_FIELD);
                                emit_short(statement->line,
                                           addConstant(statement->line,
                                                        String::copy(m_state, var->name, var->length)->asValue()));
                            }
                            else
                            {
                                emit_statement(s);
                            }
                        }
                        emit_op(m_lastline, OP_POP);
                        if(clstmt->parent != nullptr)
                        {
                            end_scope(m_lastline);
                        }
                        m_classname = nullptr;
                        m_havesuperclass = false;
                    }
                    break;
                case Expression::Type::FieldDecl:
                    {
                        fieldstmt = (StmtField*)statement;
                        getter = nullptr;
                        setter = nullptr;
                        if(fieldstmt->getter != nullptr)
                        {
                            init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                            begin_scope();
                            emit_statement(fieldstmt->getter);
                            end_scope(m_lastline);
                            getter = end_compiler(String::format(m_state, "@:get @", m_classname->asValue(), fieldstmt->name));
                        }
                        if(fieldstmt->setter != nullptr)
                        {
                            init_compiler(&compiler, fieldstmt->is_static ? LITFUNC_STATIC_METHOD : LITFUNC_METHOD);
                            mark_local_initialized(add_local("value", 5, statement->line, false));
                            begin_scope();
                            emit_statement(fieldstmt->setter);
                            end_scope(m_lastline);
                            setter = end_compiler(String::format(m_state, "@:set @", m_classname->asValue(), fieldstmt->name));
                            setter->arg_count = 1;
                            setter->max_slots++;
                        }
                        field = Field::make(m_state, fieldstmt->name, (Object*)getter, (Object*)setter);
                        emit_constant(statement->line, field->asValue());
                        emit_op(statement->line, fieldstmt->is_static ? OP_STATIC_FIELD : OP_DEFINE_FIELD);
                        emit_short(statement->line, addConstant(statement->line, fieldstmt->name->asValue()));
                    }
                    break;
                default:
                    {
                        error(statement->line, Error::LITERROR_UNKNOWN_STATEMENT, (int)statement->type);
                    }
                    break;
            }
            m_prevwasexprstmt = statement->type == Expression::Type::Expression;
            return false;
        }


    }
}
