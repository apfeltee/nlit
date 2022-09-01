

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

    }
}



