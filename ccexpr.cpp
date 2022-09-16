

#include "lit.h"
#include "priv.h"

namespace lit
{
    namespace AST
    {
        void Expression::releaseExpressionList(State* state, Expression::List* expressions)
        {
            if(expressions == nullptr)
            {
                return;
            }
            for(size_t i = 0; i < expressions->m_count; i++)
            {
                Expression::releaseExpression(state, expressions->m_values[i]);
            }
            expressions->release();
        }

        void Expression::releaseStatementList(State* state, Expression::List* statements)
        {
            size_t i;
            for(i = 0; i < statements->m_count; i++)
            {
                Expression::releaseStatement(state, statements->m_values[i]);
            }
            statements->release();
        }

        void Expression::internalReleaseStatementList(State* state, Expression::List* statements)
        {
            if(statements == nullptr)
            {
                return;
            }
            for(size_t i = 0; i < statements->m_count; i++)
            {
                Expression::releaseStatement(state, statements->m_values[i]);
            }
            statements->release();
        }

        void Expression::releaseAllocatedExpressionList(State* state, Expression::List* expressions)
        {
            size_t i;
            if(expressions == nullptr)
            {
                return;
            }
            for(i = 0; i < expressions->m_count; i++)
            {
                Expression::releaseExpression(state, expressions->m_values[i]);
            }
            expressions->release();
            Memory::reallocate(state, expressions, sizeof(Expression::List), 0);
        }

        void Expression::releaseAllocatedStatementList(State* state, Expression::List* statements)
        {
            size_t i;
            if(statements == nullptr)
            {
                return;
            }
            for(i = 0; i < statements->m_count; i++)
            {
                Expression::releaseStatement(state, statements->m_values[i]);
            }
            statements->release();
            Memory::reallocate(state, statements, sizeof(Expression::List), 0);
        }

        void Expression::releaseExpression(State* state, Expression* expression)
        {
            if(expression == nullptr)
            {
                return;
            }

            switch(expression->type)
            {
                case Expression::Type::Literal:
                {
                    Memory::reallocate(state, expression, sizeof(ExprLiteral), 0);
                    break;
                }

                case Expression::Type::Binary:
                {
                    ExprBinary* expr = (ExprBinary*)expression;

                    if(!expr->ignore_left)
                    {
                        Expression::releaseExpression(state, expr->left);
                    }

                    Expression::releaseExpression(state, expr->right);

                    Memory::reallocate(state, expression, sizeof(ExprBinary), 0);
                    break;
                }

                case Expression::Type::Unary:
                {
                    Expression::releaseExpression(state, ((ExprUnary*)expression)->right);
                    Memory::reallocate(state, expression, sizeof(ExprUnary), 0);

                    break;
                }

                case Expression::Type::Variable:
                {
                    Memory::reallocate(state, expression, sizeof(ExprVar), 0);
                    break;
                }

                case Expression::Type::Assign:
                {
                    ExprAssign* expr = (ExprAssign*)expression;

                    Expression::releaseExpression(state, expr->to);
                    Expression::releaseExpression(state, expr->value);

                    Memory::reallocate(state, expression, sizeof(ExprAssign), 0);
                    break;
                }

                case Expression::Type::Call:
                {
                    ExprCall* expr = (ExprCall*)expression;

                    Expression::releaseExpression(state, expr->callee);
                    Expression::releaseExpression(state, expr->objexpr);

                    Expression::releaseExpressionList(state, &expr->args);

                    Memory::reallocate(state, expression, sizeof(ExprCall), 0);
                    break;
                }

                case Expression::Type::Get:
                {
                    Expression::releaseExpression(state, ((ExprIndexGet*)expression)->where);
                    Memory::reallocate(state, expression, sizeof(ExprIndexGet), 0);
                    break;
                }

                case Expression::Type::Set:
                {
                    ExprIndexSet* expr = (ExprIndexSet*)expression;

                    Expression::releaseExpression(state, expr->where);
                    Expression::releaseExpression(state, expr->value);

                    Memory::reallocate(state, expression, sizeof(ExprIndexSet), 0);
                    break;
                }

                case Expression::Type::Lambda:
                    {
                        ExprLambda* expr = (ExprLambda*)expression;
                        Expression::releaseParameters(state, &expr->parameters);
                        Expression::releaseStatement(state, expr->body);
                        Memory::reallocate(state, expression, sizeof(ExprLambda), 0);
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        auto expr = (StmtFunction*)expression;
                        Expression::releaseParameters(state, &expr->parameters);
                        Expression::releaseStatement(state, expr->body);
                        Memory::reallocate(state, expression, sizeof(ExprLambda), 0);                        
                    }
                    break;
                case Expression::Type::Array:
                    {
                        Expression::releaseExpressionList(state, &((ExprArray*)expression)->values);
                        Memory::reallocate(state, expression, sizeof(ExprArray), 0);
                    }
                    break;
                case Expression::Type::Object:
                    {
                        ExprObject* map = (ExprObject*)expression;
                        map->keys.release();
                        Expression::releaseExpressionList(state, &map->values);
                        Memory::reallocate(state, expression, sizeof(ExprObject), 0);
                    }
                    break;
                case Expression::Type::Subscript:
                    {
                        ExprSubscript* expr = (ExprSubscript*)expression;
                        Expression::releaseExpression(state, expr->array);
                        Expression::releaseExpression(state, expr->index);
                        Memory::reallocate(state, expression, sizeof(ExprSubscript), 0);
                    }
                    break;
                case Expression::Type::This:
                    {
                        Memory::reallocate(state, expression, sizeof(ExprThis), 0);
                    }
                    break;
                case Expression::Type::Super:
                    {
                        Memory::reallocate(state, expression, sizeof(ExprSuper), 0);
                    }
                    break;
                case Expression::Type::Range:
                    {
                        ExprRange* expr = (ExprRange*)expression;
                        Expression::releaseExpression(state, expr->from);
                        Expression::releaseExpression(state, expr->to);
                        Memory::reallocate(state, expression, sizeof(ExprRange), 0);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        ExprIfClause* expr = (ExprIfClause*)expression;
                        Expression::releaseExpression(state, expr->condition);
                        Expression::releaseExpression(state, expr->if_branch);
                        Expression::releaseExpression(state, expr->else_branch);
                        Memory::reallocate(state, expression, sizeof(ExprIfClause), 0);
                    }
                    break;
                case Expression::Type::Interpolation:
                    {
                        Expression::releaseExpressionList(state, &((ExprInterpolation*)expression)->expressions);
                        Memory::reallocate(state, expression, sizeof(ExprInterpolation), 0);
                    }
                    break;
                case Expression::Type::Reference:
                    {
                        Expression::releaseExpression(state, ((ExprReference*)expression)->to);
                        Memory::reallocate(state, expression, sizeof(ExprReference), 0);
                    }
                    break;
                default:
                    {
                        state->raiseError(COMPILE_ERROR, "Unknown expression type %d", (int)expression->type);
                    }
                    break;
            }
        }

        void Expression::releaseStatement(State* state, Expression* statement)
        {
            if(statement == nullptr)
            {
                return;
            }
            switch(statement->type)
            {
                case Expression::Type::Expression:
                    {
                        Expression::releaseExpression(state, ((ExprStatement*)statement)->expression);
                        Memory::reallocate(state, statement, sizeof(ExprStatement), 0);
                    }
                    break;
                case Expression::Type::Block:
                    {
                        Expression::internalReleaseStatementList(state, &((StmtBlock*)statement)->statements);
                        Memory::reallocate(state, statement, sizeof(StmtBlock), 0);
                    }
                    break;
                case Expression::Type::VarDecl:
                    {
                        Expression::releaseExpression(state, ((StmtVar*)statement)->valexpr);
                        Memory::reallocate(state, statement, sizeof(StmtVar), 0);
                    }
                    break;
                case Expression::Type::IfClause:
                    {
                        StmtIfClause* stmt = (StmtIfClause*)statement;
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseStatement(state, stmt->if_branch);
                        Expression::releaseAllocatedExpressionList(state, stmt->elseif_conditions);
                        Expression::releaseAllocatedStatementList(state, stmt->elseif_branches);
                        Expression::releaseStatement(state, stmt->else_branch);
                        Memory::reallocate(state, statement, sizeof(StmtIfClause), 0);
                    }
                    break;
                case Expression::Type::WhileLoop:
                    {
                        StmtWhileLoop* stmt = (StmtWhileLoop*)statement;
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtWhileLoop), 0);
                    }
                    break;
                case Expression::Type::ForLoop:
                    {
                        StmtForLoop* stmt = (StmtForLoop*)statement;
                        Expression::releaseExpression(state, stmt->increment);
                        Expression::releaseExpression(state, stmt->condition);
                        Expression::releaseExpression(state, stmt->exprinit);

                        Expression::releaseStatement(state, stmt->var);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtForLoop), 0);
                    }
                    break;
                case Expression::Type::ContinueClause:
                    {
                        Memory::reallocate(state, statement, sizeof(StmtContinue), 0);
                    }
                    break;
                case Expression::Type::BreakClause:
                    {
                        Memory::reallocate(state, statement, sizeof(StmtBreak), 0);
                    }
                    break;
                case Expression::Type::FunctionDecl:
                    {
                        StmtFunction* stmt = (StmtFunction*)statement;
                        Expression::releaseStatement(state, stmt->body);
                        Expression::releaseParameters(state, &stmt->parameters);
                        Memory::reallocate(state, statement, sizeof(StmtFunction), 0);
                    }
                    break;
                case Expression::Type::ReturnClause:
                    {
                        Expression::releaseExpression(state, ((StmtReturn*)statement)->expression);
                        Memory::reallocate(state, statement, sizeof(StmtReturn), 0);
                    }
                    break;
                case Expression::Type::MethodDecl:
                    {
                        StmtMethod* stmt = (StmtMethod*)statement;
                        Expression::releaseParameters(state, &stmt->parameters);
                        Expression::releaseStatement(state, stmt->body);
                        Memory::reallocate(state, statement, sizeof(StmtMethod), 0);
                    }
                    break;
                case Expression::Type::ClassDecl:
                    {
                        Expression::internalReleaseStatementList(state, &((StmtClass*)statement)->fields);
                        Memory::reallocate(state, statement, sizeof(StmtClass), 0);
                    }
                    break;
                case Expression::Type::FieldDecl:
                    {
                        StmtField* stmt = (StmtField*)statement;
                        Expression::releaseStatement(state, stmt->getter);
                        Expression::releaseStatement(state, stmt->setter);
                        Memory::reallocate(state, statement, sizeof(StmtField), 0);
                    }
                    break;
                default:
                    {
                        state->raiseError(COMPILE_ERROR, "Unknown statement type %d", (int)statement->type);
                    }
                    break;
            }
        }

        ExprLiteral* ExprLiteral::make(State* state, size_t line, Value value)
        {
            auto rt = Expression::make<ExprLiteral>(state, line, Expression::Type::Literal);
            rt->value = value;
            return rt;
        }

        ExprBinary* ExprBinary::make(State* state, size_t line, Expression* left, Expression* right, TokenType op)
        {
            auto expression = Expression::make<ExprBinary>(state, line, Expression::Type::Binary);
            expression->left = left;
            expression->right = right;
            expression->op = op;
            expression->ignore_left = false;
            return expression;
        }

        ExprUnary* ExprUnary::make(State* state, size_t line, Expression* right, TokenType op)
        {
            auto expression = Expression::make<ExprUnary>(state, line, Expression::Type::Unary);
            expression->right = right;
            expression->op = op;
            return expression;
        }

        ExprVar* ExprVar::make(State* state, size_t line, const char* name, size_t length)
        {
            auto expression = Expression::make<ExprVar>(state, line, Expression::Type::Variable);
            expression->name = name;
            expression->length = length;
            return expression;
        }

        ExprAssign* ExprAssign::make(State* state, size_t line, Expression* to, Expression* value)
        {
            auto expression = Expression::make<ExprAssign>(state, line, Expression::Type::Assign);
            expression->to = to;
            expression->value = value;
            return expression;
        }

        ExprCall* ExprCall::make(State* state, size_t line, Expression* callee)
        {
            auto expression = Expression::make<ExprCall>(state, line, Expression::Type::Call);
            expression->callee = callee;
            expression->objexpr = nullptr;
            expression->args.init(state);
            return expression;
        }

        ExprIndexGet* ExprIndexGet::make(State* state, size_t line, Expression* where, const char* name, size_t length, bool questionable, bool ignore_result)
        {
            auto expression = Expression::make<ExprIndexGet>(state, line, Expression::Type::Get);
            expression->where = where;
            expression->name = name;
            expression->length = length;
            expression->ignore_emit = false;
            expression->jump = questionable ? 0 : -1;
            expression->ignore_result = ignore_result;
            return expression;
        }

        ExprIndexSet* ExprIndexSet::make(State* state, size_t line, Expression* where, const char* name, size_t length, Expression* value)
        {
            auto expression = Expression::make<ExprIndexSet>(state, line, Expression::Type::Set);
            expression->where = where;
            expression->name = name;
            expression->length = length;
            expression->value = value;
            return expression;
        }

        ExprLambda* ExprLambda::make(State* state, size_t line)
        {
            auto expression = Expression::make<ExprLambda>(state, line, Expression::Type::Lambda);
            expression->body = nullptr;
            expression->parameters.init(state);
            return expression;
        }

        ExprArray* ExprArray::make(State* state, size_t line)
        {
            auto expression = Expression::make<ExprArray>(state, line, Expression::Type::Array);
            expression->values.init(state);
            return expression;
        }

        ExprObject* ExprObject::make(State* state, size_t line)
        {
            auto expression = Expression::make<ExprObject>(state, line, Expression::Type::Object);
            expression->keys.init(state);
            expression->values.init(state);
            return expression;
        }

        ExprSubscript* ExprSubscript::make(State* state, size_t line, Expression* array, Expression* index)
        {
            auto expression = Expression::make<ExprSubscript>(state, line, Expression::Type::Subscript);
            expression->array = array;
            expression->index = index;
            return expression;
        }

        ExprSuper* ExprSuper::make(State* state, size_t line, String* method, bool ignore_result)
        {
            auto expression = Expression::make<ExprSuper>(state, line, Expression::Type::Super);
            expression->method = method;
            expression->ignore_emit = false;
            expression->ignore_result = ignore_result;
            return expression;
        }

        ExprRange* ExprRange::make(State* state, size_t line, Expression* from, Expression* to)
        {
            auto expression = Expression::make<ExprRange>(state, line, Expression::Type::Range);
            expression->from = from;
            expression->to = to;
            return expression;
        }

        ExprIfClause* ExprIfClause::make(State* state, size_t line, Expression* condition, Expression* if_branch, Expression* else_branch)
        {
            auto expression = Expression::make<ExprIfClause>(state, line, Expression::Type::IfClause);
            expression->condition = condition;
            expression->if_branch = if_branch;
            expression->else_branch = else_branch;
            return expression;
        }

        ExprInterpolation* ExprInterpolation::make(State* state, size_t line)
        {
            auto expression = Expression::make<ExprInterpolation>(state, line, Expression::Type::Interpolation);
            expression->expressions.init(state);
            return expression;
        }

        ExprReference* ExprReference::make(State* state, size_t line, Expression* to)
        {
            auto expr = Expression::make<ExprReference>(state, line, Expression::Type::Reference);
            expr->to = to;
            return expr;
        }

        ExprStatement* ExprStatement::make(State* state, size_t line, Expression* expression)
        {
            auto statement = Expression::make<ExprStatement>(state, line, Expression::Type::Expression);
            statement->expression = expression;
            statement->pop = true;
            return statement;
        }

        StmtBlock* StmtBlock::make(State* state, size_t line)
        {
            auto statement = Expression::make<StmtBlock>(state, line, Expression::Type::Block);
            statement->statements.init(state);
            return statement;
        }

        StmtVar* StmtVar::make(State* state, size_t line, const char* name, size_t length, Expression* exprinit, bool constant)
        {
            auto statement = Expression::make<StmtVar>(state, line, Expression::Type::VarDecl);
            statement->name = name;
            statement->length = length;
            statement->valexpr = exprinit;
            statement->constant = constant;
            return statement;
        }

        StmtIfClause* StmtIfClause::make(State* state,
                                                size_t line,
                                                Expression* condition,
                                                Expression* if_branch,
                                                Expression* else_branch,
                                                Expression::List* elseif_conditions,
                                                Expression::List* elseif_branches)
        {
            auto statement = Expression::make<StmtIfClause>(state, line, Expression::Type::IfClause);
            statement->condition = condition;
            statement->if_branch = if_branch;
            statement->else_branch = else_branch;
            statement->elseif_conditions = elseif_conditions;
            statement->elseif_branches = elseif_branches;
            return statement;
        }

        StmtWhileLoop* StmtWhileLoop::make(State* state, size_t line, Expression* condition, Expression* body)
        {
            auto statement = Expression::make<StmtWhileLoop>(state, line, Expression::Type::WhileLoop);
            statement->condition = condition;
            statement->body = body;
            return statement;
        }

        StmtForLoop* StmtForLoop::make(State* state,
                                                  size_t line,
                                                  Expression* exprinit,
                                                  Expression* var,
                                                  Expression* condition,
                                                  Expression* increment,
                                                  Expression* body,
                                                  bool c_style)
        {
            auto statement = Expression::make<StmtForLoop>(state, line, Expression::Type::ForLoop);
            statement->exprinit = exprinit;
            statement->var = var;
            statement->condition = condition;
            statement->increment = increment;
            statement->body = body;
            statement->c_style = c_style;
            return statement;
        }

        StmtContinue* StmtContinue::make(State* state, size_t line)
        {
            return Expression::make<StmtContinue>(state, line, Expression::Type::ContinueClause);
        }

        StmtBreak* StmtBreak::make(State* state, size_t line)
        {
            return Expression::make<StmtBreak>(state, line, Expression::Type::BreakClause);
        }

        StmtFunction* StmtFunction::make(State* state, size_t line, const char* name, size_t length)
        {
            auto function = Expression::make<StmtFunction>(state, line, Expression::Type::FunctionDecl);
            function->name = name;
            function->length = length;
            function->body = nullptr;
            function->parameters.init(state);
            return function;
        }

        StmtMethod* StmtMethod::make(State* state, size_t line, String* name, bool is_static)
        {
            auto statement = Expression::make<StmtMethod>(state, line, Expression::Type::MethodDecl);
            statement->name = name;
            statement->body = nullptr;
            statement->is_static = is_static;
            statement->parameters.init(state);
            return statement;
        }

        StmtClass* StmtClass::make(State* state, size_t line, String* name, String* parent)
        {
            auto statement = Expression::make<StmtClass>(state, line, Expression::Type::ClassDecl);
            statement->name = name;
            statement->parent = parent;
            statement->fields.init(state);
            return statement;
        }

        StmtField* StmtField::make(State* state, size_t line, String* name, Expression* getter, Expression* setter, bool is_static)
        {
            auto statement = Expression::make<StmtField>(state, line, Expression::Type::FieldDecl);
            statement->name = name;
            statement->getter = getter;
            statement->setter = setter;
            statement->is_static = is_static;
            return statement;
        }

    }
}



