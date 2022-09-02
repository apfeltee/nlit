
#include "lit.h"
#include "priv.h"

#define LIT_DEBUG_OPTIMIZER

#define optc_do_binary_op(op) \
    if(Object::isNumber(a) && Object::isNumber(b)) \
    { \
        optdbg("translating constant binary expression of '" # op "' to constant value"); \
        return Object::toValue(Object::toNumber(a) op Object::toNumber(b)); \
    } \
    return Object::NullVal;

#define optc_do_bitwise_op(op, typ) \
    if(Object::isNumber(a) && Object::isNumber(b)) \
    { \
        optdbg("translating constant bitwise expression of '" #op "' to constant value"); \
        return Object::toValue(typ(Object::toNumber(a)) op typ(Object::toNumber(b))); \
    } \
    return Object::NullVal;

#define optc_do_fn_op(fn, tokstr) \
    if(Object::isNumber(a) && Object::isNumber(b)) \
    { \
        optdbg("translating constant expression of '" tokstr "' to constant value via call to '" #fn "'"); \
        return Object::toValue(fn(Object::toNumber(a), Object::toNumber(b))); \
    } \
    return Object::NullVal;

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

        void Optimizer::set_level(OptimizationLevel level)
        {
            switch(level)
            {
                case LITOPTLEVEL_NONE:
                    {
                        Optimizer::set_all_enabled(false);
                    }
                    break;
                case LITOPTLEVEL_REPL:
                    {
                        Optimizer::set_all_enabled(true);
                        Optimizer::set_enabled(LITOPTSTATE_UNUSED_VAR, false);
                        Optimizer::set_enabled(LITOPTSTATE_UNREACHABLE_CODE, false);
                        Optimizer::set_enabled(LITOPTSTATE_EMPTY_BODY, false);
                        Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                        Optimizer::set_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
                    }
                    break;
                case LITOPTLEVEL_DEBUG:
                    {
                        Optimizer::set_all_enabled(true);
                        Optimizer::set_enabled(LITOPTSTATE_UNUSED_VAR, false);
                        Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                        Optimizer::set_enabled(LITOPTSTATE_PRIVATE_NAMES, false);
                    }
                    break;
                case LITOPTLEVEL_RELEASE:
                    {
                        Optimizer::set_all_enabled(true);
                        Optimizer::set_enabled(LITOPTSTATE_LINE_INFO, false);
                    }
                    break;
                case LITOPTLEVEL_EXTREME:
                    {
                        Optimizer::set_all_enabled(true);
                    }
                    break;
                case LITOPTLEVEL_TOTAL:
                    {
                    }
                    break;

            }
        }

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

        void Optimizer::init(State* state)
        {
            m_state = state;
            m_depth = -1;
            m_markused = false;
            m_variables.init(state);
        }

        bool Optimizer::is_empty(Expression* statement)
        {
            return statement == nullptr || (statement->type == Expression::Type::Block && ((StmtBlock*)statement)->statements.m_count == 0);
        }

        Value Optimizer::evaluate_unary_op(Value value, TokenType op)
        {
            switch(op)
            {
                case LITTOK_MINUS:
                    {
                        if(Object::isNumber(value))
                        {
                            m_optcount++;
                            optdbg("translating constant unary minus on number to literal value");
                            return Object::toValue(-Object::toNumber(value));
                        }
                    }
                    break;
                case LITTOK_BANG:
                    {
                        m_optcount++;
                        optdbg("translating constant expression of '=' to literal value");
                        return Object::fromBool(Object::isFalsey(value));
                    }
                    break;
                case LITTOK_TILDE:
                    {
                        if(Object::isNumber(value))
                        {
                            m_optcount++;
                            optdbg("translating unary tile (~) on number to literal value");
                            return Object::toValue(~((int)Object::toNumber(value)));
                        }
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
            return Object::NullVal;
        }

        Value Optimizer::evaluate_binary_op(Value a, Value b, TokenType op)
        {
            switch(op)
            {
                case LITTOK_PLUS:
                    {
                        optc_do_binary_op(+);
                    }
                    break;
                case LITTOK_MINUS:
                    {
                        optc_do_binary_op(-);
                    }
                    break;
                case LITTOK_STAR:
                    {
                        optc_do_binary_op(*);
                    }
                    break;
                case LITTOK_SLASH:
                    {
                        optc_do_binary_op(/);
                    }
                    break;
                case LITTOK_STAR_STAR:
                    {
                        optc_do_fn_op(pow, "**");
                    }
                    break;
                case LITTOK_PERCENT:
                    {
                        optc_do_fn_op(fmod, "%");
                    }
                    break;
                case LITTOK_GREATER:
                    {
                        optc_do_binary_op(>);
                    }
                    break;
                case LITTOK_GREATER_EQUAL:
                    {
                        optc_do_binary_op(>=);
                    }
                    break;
                case LITTOK_LESS:
                    {
                        optc_do_binary_op(<);
                    }
                    break;
                case LITTOK_LESS_EQUAL:
                    {
                        optc_do_binary_op(<=);
                    }
                    break;
                case LITTOK_LESS_LESS:
                    {
                        optc_do_bitwise_op(<<, int64_t);
                    }
                    break;
                case LITTOK_GREATER_GREATER:
                    {
                        optc_do_bitwise_op(>>, int64_t);
                    }
                    break;
                case LITTOK_BAR:
                    {
                        optc_do_bitwise_op(|, int64_t);
                    }
                    break;
                case LITTOK_AMPERSAND:
                    {
                        optc_do_bitwise_op(&, int64_t);
                    }
                    break;
                case LITTOK_CARET:
                    {
                        optc_do_bitwise_op(^, int64_t);
                    }
                    break;
                case LITTOK_SHARP:
                    {
                        if(Object::isNumber(a) && Object::isNumber(b))
                        {
                            return Object::toValue(floor(Object::toNumber(a) / Object::toNumber(b)));
                        }
                        return Object::NullVal;
                    }
                    break;
                case LITTOK_EQUAL_EQUAL:
                    {
                        return Object::fromBool(a == b);
                    }
                    break;
                case LITTOK_BANG_EQUAL:
                    {
                        return Object::fromBool(a != b);
                    }
                    break;
                case LITTOK_IS:
                default:
                    {
                    }
                    break;
            }
            return Object::NullVal;
        }

        void Optimizer::optdbg(const char* fmt, ...)
        {
            #if defined(LIT_DEBUG_OPTIMIZER)
                va_list va;
                va_start(va, fmt);
                fprintf(stderr, "optimizer: ");
                vfprintf(stderr, fmt, va);
                fprintf(stderr, "\n");
                va_end(va);
            #endif
        }

        void Optimizer::opt_begin_scope()
        {
            m_depth++;
        }

        void Optimizer::opt_end_scope()
        {
            bool remove_unused;
            Variable* variable;
            PCGenericArray<Variable>* variables;
            m_depth--;
            variables = &m_variables;
            remove_unused = Optimizer::is_enabled(LITOPTSTATE_UNUSED_VAR);
            while(variables->m_count > 0 && variables->m_values[variables->m_count - 1].depth > m_depth)
            {
                if(remove_unused && !variables->m_values[variables->m_count - 1].used)
                {
                    variable = &variables->m_values[variables->m_count - 1];
                    Expression::releaseStatement(m_state, *variable->declaration);
                    *variable->declaration = nullptr;
                }
                variables->m_count--;
            }
        }

        Variable* Optimizer::add_variable(const char* name, size_t length, bool constant, Expression** declaration)
        {
            m_variables.push(Variable{ name, length, m_depth, constant, m_markused, Object::NullVal, declaration });
            return &m_variables.m_values[m_variables.m_count - 1];
        }

        Variable* Optimizer::resolve_variable(const char* name, size_t length)
        {
            int i;
            PCGenericArray<Variable>* variables;
            Variable* variable;
            variables = &m_variables;
            for(i = variables->m_count - 1; i >= 0; i--)
            {
                variable = &variables->m_values[i];
                if(length == variable->length && memcmp(variable->name, name, length) == 0)
                {
                    return variable;
                }
            }
            return nullptr;
        }

        Value Optimizer::attempt_to_optimize_binary(ExprBinary* expression, Value value, bool left)
        {
            double number;
            TokenType op;
            op = expression->op;
            Expression* branch;
            branch = left ? expression->left : expression->right;
            if(Object::isNumber(value))
            {
                number = Object::toNumber(value);
                if(op == LITTOK_STAR)
                {
                    if(number == 0)
                    {
                        optdbg("reducing expression to literal '0'");
                        return Object::toValue(0);
                    }
                    else if(number == 1)
                    {
                        optdbg("reducing expression to literal '1'");
                        Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                        expression->left = branch;
                        expression->right = nullptr;
                    }
                }
                else if((op == LITTOK_PLUS || op == LITTOK_MINUS) && number == 0)
                {
                    optdbg("reducing expression that would result in '0' to literal '0'");
                    Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                    expression->left = branch;
                    expression->right = nullptr;
                }
                else if(((left && op == LITTOK_SLASH) || op == LITTOK_STAR_STAR) && number == 1)
                {
                    optdbg("reducing expression that would result in '1' to literal '1'");
                    Expression::releaseExpression(m_state, left ? expression->right : expression->left);
                    expression->left = branch;
                    expression->right = nullptr;
                }
            }
            return Object::NullVal;
        }

        Value Optimizer::evaluate_expression(Expression* expression)
        {
            ExprUnary* uexpr;
            ExprBinary* bexpr;
            Value a;
            Value b;
            Value branch;
            if(expression == nullptr)
            {
                return Object::NullVal;
            }
            switch(expression->type)
            {
                case Expression::Type::Literal:
                    {
                        return ((ExprLiteral*)expression)->value;
                    }
                    break;
                case Expression::Type::Unary:
                    {
                        uexpr = (ExprUnary*)expression;
                        branch = evaluate_expression(uexpr->right);
                        if(branch != Object::NullVal)
                        {
                            return evaluate_unary_op(branch, uexpr->op);
                        }
                    }
                    break;
                case Expression::Type::Binary:
                    {
                        bexpr = (ExprBinary*)expression;
                        a = evaluate_expression(bexpr->left);
                        b = evaluate_expression(bexpr->right);
                        if(a != Object::NullVal && b != Object::NullVal)
                        {
                            return evaluate_binary_op(a, b, bexpr->op);
                        }
                        else if(a != Object::NullVal)
                        {
                            return attempt_to_optimize_binary(bexpr, a, false);
                        }
                        else if(b != Object::NullVal)
                        {
                            return attempt_to_optimize_binary(bexpr, b, true);
                        }
                    }
                    break;
                default:
                    {
                        return Object::NullVal;
                    }
                    break;
            }
            return Object::NullVal;
        }

        void Optimizer::optimize_expression(Expression** slot)
        {
            auto expression = *slot;
            if(expression == nullptr)
            {
                return;
            }
            auto state = m_state;
            switch(expression->type)
            {
                case Expression::Type::Unary:
                case Expression::Type::Binary:
                    {
                        if(Optimizer::is_enabled(LITOPTSTATE_LITERAL_FOLDING))
                        {
                            Value optimized = evaluate_expression(expression);
                            if(optimized != Object::NullVal)
                            {
                                *slot = (Expression*)ExprLiteral::make(state, expression->line, optimized);
                                Expression::releaseExpression(state, expression);
                                break;
                            }
                        }
                        switch(expression->type)
                        {
                            case Expression::Type::Unary:
                                {
                                    optimize_expression(&((ExprUnary*)expression)->right);
                                }
                                break;
                            case Expression::Type::Binary:
                                {
                                    auto expr = (ExprBinary*)expression;
                                    optimize_expression(&expr->left);
                                    optimize_expression(&expr->right);
                                }
                                break;
                            default:
                                {
                                }
                                break;
                        }
                    }
                    break;
                case Expression::Type::Assign:
                    {
                        auto expr = (ExprAssign*)expression;
                        optimize_expression(&expr->to);
                        optimize_expression(&expr->value);
                    }
                    break;
                case Expression::Type::Call:
                    {
                        auto expr = (ExprCall*)expression;
                        optimize_expression(&expr->callee);
                        optimize_expressions(&expr->args);
                    }
                    break;
                case Expression::Type::Set:
                    {
                        auto expr = (ExprIndexSet*)expression;
                        optimize_expression(&expr->where);
                        optimize_expression(&expr->value);
                    }
                    break;
                case Expression::Type::Get:
                    {
                        optimize_expression(&((ExprIndexGet*)expression)->where);
                    }
                    break;
                case Expression::Type::Lambda:
                    {
                        opt_begin_scope();
                        optimize_statement(&((ExprLambda*)expression)->body);
                        opt_end_scope();
                    }
                    break;
                case Expression::Type::Array:
                    {
                        optimize_expressions(&((ExprArray*)expression)->values);
                    }
                    break;
                case Expression::Type::Object:
                    {
                        optimize_expressions(&((ExprObject*)expression)->values);
                    }
                    break;
                case Expression::Type::Subscript:
                    {
                        auto expr = (ExprSubscript*)expression;
                        optimize_expression(&expr->array);
                        optimize_expression(&expr->index);
                    }
                    break;
                case Expression::Type::Range:
                    {
                        auto expr = (ExprRange*)expression;
                        optimize_expression(&expr->from);
                        optimize_expression(&expr->to);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        auto expr = (ExprIfClause*)expression;
                        Value optimized = evaluate_expression(expr->condition);
                        if(optimized != Object::NullVal)
                        {
                            if(Object::isFalsey(optimized))
                            {
                                *slot = expr->else_branch;
                                expr->else_branch = nullptr;// So that it doesn't get freed
                            }
                            else
                            {
                                *slot = expr->if_branch;
                                expr->if_branch = nullptr;// So that it doesn't get freed
                            }
                            optimize_expression(slot);
                            Expression::releaseExpression(state, expression);
                        }
                        else
                        {
                            optimize_expression(&expr->if_branch);
                            optimize_expression(&expr->else_branch);
                        }
                    }
                    break;
                case Expression::Type::Interpolation:
                    {
                        optimize_expressions(&((ExprInterpolation*)expression)->expressions);
                    }
                    break;
                case Expression::Type::Variable:
                    {
                        auto expr = (ExprVar*)expression;
                        auto variable = resolve_variable(expr->name, expr->length);
                        if(variable != nullptr)
                        {
                            variable->used = true;

                            // Not checking here for the enable-ness of constant-folding, since if its off
                            // the constant_value would be Object::NullVal anyway (:thinkaboutit:)
                            if(variable->constant && variable->constant_value != Object::NullVal)
                            {
                                *slot = (Expression*)ExprLiteral::make(state, expression->line, variable->constant_value);
                                Expression::releaseExpression(state, expression);
                            }
                        }
                    }
                    break;
                case Expression::Type::Reference:
                    {
                        optimize_expression(&((ExprReference*)expression)->to);
                    }
                    break;
                case Expression::Type::Literal:
                case Expression::Type::This:
                case Expression::Type::Super:
                    {
                        // nothing to do here
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
        }

        void Optimizer::optimize_expressions(Expression::List* expressions)
        {
            size_t i;
            for(i = 0; i < expressions->m_count; i++)
            {
                optimize_expression(&expressions->m_values[i]);
            }
        }

        void Optimizer::optimize_statement(Expression** slot)
        {
            size_t i;
            size_t j;
            State* state;
            Expression* statement;
            statement = *slot;
            if(statement == nullptr)
            {
                return;
            }
            state = m_state;
            switch(statement->type)
            {
                case Expression::Type::Unspecified:
                    {
                    }
                    break;
                case Expression::Type::Expression:
                    {
                        optimize_expression(&((ExprStatement*)statement)->expression);
                    }
                    break;
                case Expression::Type::Block:
                    {
                        auto stmt = (StmtBlock*)statement;
                        if(stmt->statements.m_count == 0)
                        {
                            Expression::releaseStatement(state, statement);
                            *slot = nullptr;
                            break;
                        }
                        opt_begin_scope();
                        optimize_statements(&stmt->statements);
                        opt_end_scope();
                        bool found = false;
                        for(i = 0; i < stmt->statements.m_count; i++)
                        {
                            auto step = stmt->statements.m_values[i];
                            if(!is_empty(step))
                            {
                                found = true;
                                if(step->type == Expression::Type::ReturnClause)
                                {
                                    // Remove all the statements post return
                                    for(j = i + 1; j < stmt->statements.m_count; j++)
                                    {
                                        step = stmt->statements.m_values[j];
                                        if(step != nullptr)
                                        {
                                            Expression::releaseStatement(state, step);
                                            stmt->statements.m_values[j] = nullptr;
                                        }
                                    }
                                    stmt->statements.m_count = i + 1;
                                    break;
                                }
                            }
                        }
                        if(!found && Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY))
                        {
                            Expression::releaseStatement(m_state, statement);
                            *slot = nullptr;
                        }
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        auto stmt = (StmtIfClause*)statement;
                        optimize_expression(&stmt->condition);
                        optimize_statement(&stmt->if_branch);
                        bool empty = Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY);
                        bool dead = Optimizer::is_enabled(LITOPTSTATE_UNREACHABLE_CODE);
                        Value optimized = empty ? evaluate_expression(stmt->condition) : Object::NullVal;
                        if((optimized != Object::NullVal && Object::isFalsey(optimized)) || (dead && is_empty(stmt->if_branch)))
                        {
                            Expression::releaseExpression(state, stmt->condition);
                            stmt->condition = nullptr;
                            Expression::releaseStatement(state, stmt->if_branch);
                            stmt->if_branch = nullptr;
                        }
                        if(stmt->elseif_conditions != nullptr)
                        {
                            optimize_expressions(stmt->elseif_conditions);
                            optimize_statements(stmt->elseif_branches);
                            if(dead || empty)
                            {
                                for(i = 0; i < stmt->elseif_conditions->m_count; i++)
                                {
                                    if(empty && is_empty(stmt->elseif_branches->m_values[i]))
                                    {
                                        Expression::releaseExpression(state, stmt->elseif_conditions->m_values[i]);
                                        stmt->elseif_conditions->m_values[i] = nullptr;
                                        Expression::releaseStatement(state, stmt->elseif_branches->m_values[i]);
                                        stmt->elseif_branches->m_values[i] = nullptr;
                                        continue;
                                    }
                                    if(dead)
                                    {
                                        Value value = evaluate_expression(stmt->elseif_conditions->m_values[i]);
                                        if(value != Object::NullVal && Object::isFalsey(value))
                                        {
                                            Expression::releaseExpression(state, stmt->elseif_conditions->m_values[i]);
                                            stmt->elseif_conditions->m_values[i] = nullptr;
                                            Expression::releaseStatement(state, stmt->elseif_branches->m_values[i]);
                                            stmt->elseif_branches->m_values[i] = nullptr;
                                        }
                                    }
                                }
                            }
                        }
                        optimize_statement(&stmt->else_branch);
                    }
                    break;
                case Expression::Type::WhileLoop:
                    {
                        auto stmt = (StmtWhileLoop*)statement;
                        optimize_expression(&stmt->condition);
                        if(Optimizer::is_enabled(LITOPTSTATE_UNREACHABLE_CODE))
                        {
                            Value optimized = evaluate_expression(stmt->condition);
                            if(optimized != Object::NullVal && Object::isFalsey(optimized))
                            {
                                Expression::releaseStatement(m_state, statement);
                                *slot = nullptr;
                                break;
                            }
                        }
                        optimize_statement(&stmt->body);
                        if(Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                        {
                            Expression::releaseStatement(m_state, statement);
                            *slot = nullptr;
                        }
                    }
                    break;
                case Expression::Type::ForLoop:
                    {
                        auto stmt = (StmtForLoop*)statement;
                        opt_begin_scope();
                        // This is required, so that this doesn't optimize out our i variable (and such)
                        m_markused = true;
                        optimize_expression(&stmt->exprinit);
                        optimize_expression(&stmt->condition);
                        optimize_expression(&stmt->increment);
                        optimize_statement(&stmt->var);
                        m_markused = false;
                        optimize_statement(&stmt->body);
                        opt_end_scope();
                        if(Optimizer::is_enabled(LITOPTSTATE_EMPTY_BODY) && is_empty(stmt->body))
                        {
                            Expression::releaseStatement(m_state, statement);
                            *slot = nullptr;
                            break;
                        }
                        if(stmt->c_style || !Optimizer::is_enabled(LITOPTSTATE_C_FOR) || stmt->condition->type != Expression::Type::Range)
                        {
                            break;
                        }
                        auto range = (ExprRange*)stmt->condition;
                        Value from = evaluate_expression(range->from);
                        Value to = evaluate_expression(range->to);
                        if(!Object::isNumber(from) || !Object::isNumber(to))
                        {
                            break;
                        }
                        bool reverse = Object::toNumber(from) > Object::toNumber(to);
                        auto var = (StmtVar*)stmt->var;
                        size_t line = range->line;
                        // var i = from
                        var->valexpr = range->from;
                        // i <= to
                        stmt->condition = (Expression*)ExprBinary::make(
                        state, line, (Expression*)ExprVar::make(state, line, var->name, var->length), range->to, LITTOK_LESS_EQUAL);
                        // i++ (or i--)
                        auto var_get = (Expression*)ExprVar::make(state, line, var->name, var->length);
                        auto assign_value = ExprBinary::make(state, line, var_get, (Expression*)ExprLiteral::make(state, line, Object::toValue(1)),
                        reverse ? LITTOK_MINUS_MINUS : LITTOK_PLUS);
                        assign_value->ignore_left = true;
                        auto increment = (Expression*)ExprAssign::make(state, line, var_get, (Expression*)assign_value);
                        stmt->increment = (Expression*)increment;
                        range->from = nullptr;
                        range->to = nullptr;
                        stmt->c_style = true;
                        Expression::releaseExpression(state, (Expression*)range);
                    }
                    break;
                case Expression::Type::VarDecl:
                    {
                        auto stmt = (StmtVar*)statement;
                        auto variable = add_variable(stmt->name, stmt->length, stmt->constant, slot);
                        optimize_expression(&stmt->valexpr);
                        if(stmt->constant && Optimizer::is_enabled(LITOPTSTATE_CONSTANT_FOLDING))
                        {
                            Value value = evaluate_expression(stmt->valexpr);
                            if(value != Object::NullVal)
                            {
                                variable->constant_value = value;
                            }
                        }
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        auto stmt = (StmtFunction*)statement;
                        auto variable = add_variable(stmt->name, stmt->length, false, slot);
                        if(stmt->exported)
                        {
                            // Otherwise it will get optimized-out with a big chance
                            variable->used = true;
                        }
                        opt_begin_scope();
                        optimize_statement(&stmt->body);
                        opt_end_scope();
                    }
                    break;
                case Expression::Type::ReturnClause:
                    {
                        optimize_expression(&((StmtReturn*)statement)->expression);
                    }
                    break;
                case Expression::Type::MethodDecl:
                    {
                        opt_begin_scope();
                        optimize_statement(&((StmtMethod*)statement)->body);
                        opt_end_scope();
                    }
                    break;
                case Expression::Type::ClassDecl:
                    {
                        optimize_statements(&((StmtClass*)statement)->fields);
                    }
                    break;
                case Expression::Type::FieldDecl:
                    {
                        auto stmt = (StmtField*)statement;
                        if(stmt->getter != nullptr)
                        {
                            opt_begin_scope();
                            optimize_statement(&stmt->getter);
                            opt_end_scope();
                        }
                        if(stmt->setter != nullptr)
                        {
                            opt_begin_scope();
                            optimize_statement(&stmt->setter);
                            opt_end_scope();
                        }
                    }
                    break;
                // Nothing to optimize there
                case Expression::Type::ContinueClause:
                case Expression::Type::BreakClause:
                    {
                    }
                    break;
                default:
                    break;
            }
        }

        void Optimizer::optimize_statements(Expression::List* statements)
        {
            size_t i;
            for(i = 0; i < statements->m_count; i++)
            {
                optimize_statement(&statements->m_values[i]);
            }
        }

    }
}
