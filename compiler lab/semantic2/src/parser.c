/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "debug.h"
#include "token.h"

Token *currentToken;
Token *lookAhead;

extern Type *intType;
extern Type *charType;
extern SymTab *symtab;
<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
Object *obj;

void scan(void) {
    Token *tmp = currentToken;
    currentToken = lookAhead;
    lookAhead = getValidToken();
    free(tmp);
}

void eat(TokenType tokenType) {
    if (lookAhead->tokenType == tokenType) {
        scan();
    } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
    eat(KW_PROGRAM);
    eat(TK_IDENT);
    obj = createProgramObject(currentToken->string);
    enterBlock(obj->progAttrs->scope);
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_PERIOD);
    exitBlock();
}

void compileBlock(void) {

    if (lookAhead->tokenType == KW_CONST) {
        eat(KW_CONST);

        do {
            eat(TK_IDENT);

            obj = createConstantObject(currentToken->string);

            eat(SB_EQ);

            obj->constAttrs->value = compileConstant();
            declareObject(obj);
            eat(SB_SEMICOLON);


        } while (lookAhead->tokenType == TK_IDENT);

        compileBlock2();
    } else compileBlock2();
}

void compileBlock2(void) {
    if (lookAhead->tokenType == KW_TYPE) {
        eat(KW_TYPE);

        do {
            eat(TK_IDENT);

            obj = createTypeObject(currentToken->string);

            eat(SB_EQ);

            obj->typeAttrs->actualType = compileType();
            declareObject(obj);
            eat(SB_SEMICOLON);


        } while (lookAhead->tokenType == TK_IDENT);

        compileBlock3();
    } else compileBlock3();
}

void compileBlock3(void) {
    if (lookAhead->tokenType == KW_VAR) {
        eat(KW_VAR);

        do {
            eat(TK_IDENT);

            obj = createVariableObject(currentToken->string);

            eat(SB_COLON);

            obj->varAttrs->type = compileType();
            declareObject(obj);
            eat(SB_SEMICOLON);


        } while (lookAhead->tokenType == TK_IDENT);

        compileBlock4();
    } else compileBlock4();
}

void compileBlock4(void) {
    compileSubDecls();
    compileBlock5();
}

void compileBlock5(void) {
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
}

void compileSubDecls(void) {
    while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
        if (lookAhead->tokenType == KW_FUNCTION)
            compileFuncDecl();
        else compileProcDecl();
    }
}

void compileFuncDecl(void) {
    Object *funcObj;
    eat(KW_FUNCTION);
    eat(TK_IDENT);

    obj = createFunctionObject(currentToken->string);
    declareObject(obj);

    funcObj = obj;

    enterBlock(obj->funcAttrs->scope);
    compileParams();

    eat(SB_COLON);
    funcObj->funcAttrs->returnType = compileType();

    eat(SB_SEMICOLON);

    compileBlock();
    eat(SB_SEMICOLON);
    exitBlock();

}

void compileProcDecl(void) {

    eat(KW_PROCEDURE);
    eat(TK_IDENT);
    obj = createProcedureObject(currentToken->string);
    declareObject(obj);

    enterBlock(obj->procAttrs->scope);
    compileParams();

    eat(SB_SEMICOLON);

    compileBlock();
    eat(SB_SEMICOLON);
    exitBlock();

}

ConstantValue *compileUnsignedConstant(void) {
    ConstantValue *constValue;

    switch (lookAhead->tokenType) {
        case TK_NUMBER:
            eat(TK_NUMBER);
            constValue = makeIntConstant(currentToken->value);
            break;
        case TK_IDENT:
            eat(TK_IDENT);

            Object *found = lookupObject(currentToken->string);
            if (found == NULL)
                return NULL;
            constValue = duplicateConstantValue(found->constAttrs->value);

            break;
        case TK_CHAR:
            eat(TK_CHAR);
            constValue = makeCharConstant(currentToken->string[0]);
            break;
        default:
            error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
    return constValue;
}

ConstantValue *compileConstant(void) {
    ConstantValue *constValue;

    switch (lookAhead->tokenType) {
        case SB_PLUS:
            eat(SB_PLUS);
            constValue = compileConstant2();
            break;
        case SB_MINUS:
            eat(SB_MINUS);
            constValue = compileConstant2();
            break;
        case TK_CHAR:
            eat(TK_CHAR);
            constValue = makeCharConstant(currentToken->string[0]);
            break;
        default:
            constValue = compileConstant2();
            break;
    }
    return constValue;
}

ConstantValue *compileConstant2(void) {
    ConstantValue *constValue;

    switch (lookAhead->tokenType) {
        case TK_NUMBER:
            eat(TK_NUMBER);
            constValue = makeIntConstant(currentToken->value);
            break;
        case TK_IDENT:
            eat(TK_IDENT);

            Object *found = findObject(symtab->globalObjectList, currentToken->string);
            if (found == NULL)
                return NULL;
            constValue = duplicateConstantValue(found->constAttrs->value);

            break;
        default:
            error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
    return constValue;
}

Type *compileType(void) {
    Type *type;

    switch (lookAhead->tokenType) {
        case KW_INTEGER:
            eat(KW_INTEGER);
            type = makeIntType();
            break;
        case KW_CHAR:
            eat(KW_CHAR);
            type = makeCharType();
            break;
        case KW_ARRAY:
            eat(KW_ARRAY);
            eat(SB_LSEL);
            eat(TK_NUMBER);
            int size = currentToken->value;
            eat(SB_RSEL);
            eat(KW_OF);
            type = makeArrayType(size, compileType());
            break;
        case TK_IDENT:
            eat(TK_IDENT);

            Object *found = lookupObject(currentToken->string);
            if (found == NULL)
                return NULL;
            type = duplicateType(found->typeAttrs->actualType);

            break;
        default:
            error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
    return type;
}

Type *compileBasicType(void) {
    Type *type;

    switch (lookAhead->tokenType) {
        case KW_INTEGER:
            eat(KW_INTEGER);
            type = makeIntType();
            break;
        case KW_CHAR:
            eat(KW_CHAR);
            type = makeCharType();
            break;
        default:
            error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
    return type;
}

void compileParams(void) {
    if (lookAhead->tokenType == SB_LPAR) {
        eat(SB_LPAR);
        compileParam();
        while (lookAhead->tokenType == SB_SEMICOLON) {
            eat(SB_SEMICOLON);
            compileParam();
        }
        eat(SB_RPAR);
=======

void scan(void)
{
  Token *tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType)
{
  if (lookAhead->tokenType == tokenType)
  {
    scan();
  }
  else
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void)
{
  // TODO: create, enter, and exit program block
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
}

void compileBlock(void)
{
  // TODO: create and declare constant objects
  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);

    do
    {
      eat(TK_IDENT);
      eat(SB_EQ);
      compileConstant();
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  }
  else
    compileBlock2();
}

void compileBlock2(void)
{
  // TODO: create and declare type objects
  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);

    do
    {
      eat(TK_IDENT);
      eat(SB_EQ);
      compileType();
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  }
  else
    compileBlock3();
}

void compileBlock3(void)
{
  // TODO: create and declare variable objects
  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);

    do
    {
      eat(TK_IDENT);
      eat(SB_COLON);
      compileType();
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock4();
  }
  else
    compileBlock4();
}

void compileBlock4(void)
{
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void)
{
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE))
  {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else
      compileProcDecl();
  }
}

void compileFuncDecl(void)
{
  // TODO: create and declare a function object
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
}

void compileProcDecl(void)
{
  // TODO: create and declare a procedure object
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
}

ConstantValue *compileUnsignedConstant(void)
{
  // TODO: create and return an unsigned constant value
  ConstantValue *constValue;

  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue *compileConstant(void)
{
  // TODO: create and return a constant
  ConstantValue *constValue;

  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileConstant2();
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue *compileConstant2(void)
{
  // TODO: create and return a constant value
  ConstantValue *constValue;

  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type *compileType(void)
{
  // TODO: create and return a type
  Type *type;

  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type *compileBasicType(void)
{
  Type *type;

  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void)
{
  if (lookAhead->tokenType == SB_LPAR)
  {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON)
    {
      eat(SB_SEMICOLON);
      compileParam();
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c
    }
}

<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
void compileParam(void) {

    switch (lookAhead->tokenType) {
        case TK_IDENT:
            eat(TK_IDENT);
            obj = createParameterObject(currentToken->string, PARAM_VALUE, symtab->currentScope->owner);
            eat(SB_COLON);
            obj->paramAttrs->type = compileBasicType();
            declareObject(obj);
            break;
        case KW_VAR:
            eat(KW_VAR);
            eat(TK_IDENT);
            obj = createParameterObject(currentToken->string, PARAM_REFERENCE, symtab->currentScope->owner);
            eat(SB_COLON);
            obj->paramAttrs->type = compileBasicType();
            declareObject(obj);
            break;
        default:
            error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
}

void compileStatements(void) {
=======
void compileParam(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  case KW_VAR:
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileStatements(void)
{
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c
    compileStatement();
    while (lookAhead->tokenType == SB_SEMICOLON) {
        eat(SB_SEMICOLON);
        compileStatement();
    }
}

<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
void compileStatement(void) {
    switch (lookAhead->tokenType) {
        case TK_IDENT:
            compileAssignSt();
            break;
        case KW_CALL:
            compileCallSt();
            break;
        case KW_BEGIN:
            compileGroupSt();
            break;
        case KW_IF:
            compileIfSt();
            break;
        case KW_WHILE:
            compileWhileSt();
            break;
        case KW_FOR:
            compileForSt();
            break;
            // EmptySt needs to check FOLLOW tokens
        case SB_SEMICOLON:
        case KW_END:
        case KW_ELSE:
            break;
            // Error occurs
        default:
            error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
}

void compileLValue(void) {
    eat(TK_IDENT);
    compileIndexes();
}

void compileAssignSt(void) {
    compileLValue();
    eat(SB_ASSIGN);
    compileExpression();
}

void compileCallSt(void) {
    eat(KW_CALL);
    eat(TK_IDENT);
    compileArguments();
}

void compileGroupSt(void) {
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
}

void compileIfSt(void) {
    eat(KW_IF);
    compileCondition();
    eat(KW_THEN);
    compileStatement();
    if (lookAhead->tokenType == KW_ELSE)
        compileElseSt();
}

void compileElseSt(void) {
    eat(KW_ELSE);
    compileStatement();
}

void compileWhileSt(void) {
    eat(KW_WHILE);
    compileCondition();
    eat(KW_DO);
    compileStatement();
}

void compileForSt(void) {
    eat(KW_FOR);
    eat(TK_IDENT);
    eat(SB_ASSIGN);
    compileExpression();
    eat(KW_TO);
    compileExpression();
    eat(KW_DO);
    compileStatement();
}

void compileArgument(void) {
    compileExpression();
}

void compileArguments(void) {
    switch (lookAhead->tokenType) {
        case SB_LPAR:
            eat(SB_LPAR);
            compileArgument();

            while (lookAhead->tokenType == SB_COMMA) {
                eat(SB_COMMA);
                compileArgument();
            }

            eat(SB_RPAR);
            break;
            // Check FOLLOW set
        case SB_TIMES:
        case SB_SLASH:
        case SB_PLUS:
        case SB_MINUS:
        case KW_TO:
        case KW_DO:
        case SB_RPAR:
        case SB_COMMA:
        case SB_EQ:
        case SB_NEQ:
        case SB_LE:
        case SB_LT:
        case SB_GE:
        case SB_GT:
        case SB_RSEL:
        case SB_SEMICOLON:
        case KW_END:
        case KW_ELSE:
        case KW_THEN:
            break;
        default:
            error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileCondition(void) {
    compileExpression();
    switch (lookAhead->tokenType) {
        case SB_EQ:
            eat(SB_EQ);
            break;
        case SB_NEQ:
            eat(SB_NEQ);
            break;
        case SB_LE:
            eat(SB_LE);
            break;
        case SB_LT:
            eat(SB_LT);
            break;
        case SB_GE:
            eat(SB_GE);
            break;
        case SB_GT:
            eat(SB_GT);
            break;
        default:
            error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    }

    compileExpression();
}

void compileExpression(void) {
    switch (lookAhead->tokenType) {
        case SB_PLUS:
            eat(SB_PLUS);
            compileExpression2();
            break;
        case SB_MINUS:
            eat(SB_MINUS);
            compileExpression2();
            break;
        default:
            compileExpression2();
    }
}

void compileExpression2(void) {
    compileTerm();
    compileExpression3();
}


void compileExpression3(void) {
    switch (lookAhead->tokenType) {
        case SB_PLUS:
            eat(SB_PLUS);
            compileTerm();
            compileExpression3();
            break;
        case SB_MINUS:
            eat(SB_MINUS);
            compileTerm();
            compileExpression3();
            break;
            // check the FOLLOW set
        case KW_TO:
        case KW_DO:
        case SB_RPAR:
        case SB_COMMA:
        case SB_EQ:
        case SB_NEQ:
        case SB_LE:
        case SB_LT:
        case SB_GE:
        case SB_GT:
        case SB_RSEL:
        case SB_SEMICOLON:
        case KW_END:
        case KW_ELSE:
        case KW_THEN:
            break;
        default:
            error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileTerm(void) {
    compileFactor();
    compileTerm2();
}

void compileTerm2(void) {
    switch (lookAhead->tokenType) {
        case SB_TIMES:
            eat(SB_TIMES);
            compileFactor();
            compileTerm2();
            break;
        case SB_SLASH:
            eat(SB_SLASH);
            compileFactor();
            compileTerm2();
            break;
            // check the FOLLOW set
        case SB_PLUS:
        case SB_MINUS:
        case KW_TO:
        case KW_DO:
        case SB_RPAR:
        case SB_COMMA:
        case SB_EQ:
        case SB_NEQ:
        case SB_LE:
        case SB_LT:
        case SB_GE:
        case SB_GT:
        case SB_RSEL:
        case SB_SEMICOLON:
        case KW_END:
        case KW_ELSE:
        case KW_THEN:
            break;
        default:
            error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileFactor(void) {
    switch (lookAhead->tokenType) {
        case TK_NUMBER:
            eat(TK_NUMBER);
            break;
        case TK_CHAR:
            eat(TK_CHAR);
            break;
        case TK_IDENT:
            eat(TK_IDENT);
            switch (lookAhead->tokenType) {
                case SB_LPAR:
                    compileArguments();
                    break;
                case SB_LSEL:
                    compileIndexes();
                    break;
                default:
                    break;
            }
            break;
        default:
            error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
=======
void compileStatement(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileLValue(void)
{
  eat(TK_IDENT);
  compileIndexes();
}

void compileAssignSt(void)
{
  compileLValue();
  eat(SB_ASSIGN);
  compileExpression();
}

void compileCallSt(void)
{
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
}

void compileGroupSt(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void)
{
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt();
}

void compileElseSt(void)
{
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void)
{
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void)
{
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
}

void compileArgument(void)
{
  compileExpression();
}

void compileArguments(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument();

    while (lookAhead->tokenType == SB_COMMA)
    {
      eat(SB_COMMA);
      compileArgument();
    }

    eat(SB_RPAR);
    break;
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void)
{
  compileExpression();
  switch (lookAhead->tokenType)
  {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  compileExpression();
}

void compileExpression(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
  }
}

void compileExpression2(void)
{
  compileTerm();
  compileExpression3();
}

void compileExpression3(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileTerm(void)
{
  compileFactor();
  compileTerm2();
}

void compileTerm2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileFactor(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType)
    {
    case SB_LPAR:
      compileArguments();
      break;
    case SB_LSEL:
      compileIndexes();
      break;
    default:
      break;
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c
    }
}

<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
void compileIndexes(void) {
    while (lookAhead->tokenType == SB_LSEL) {
        eat(SB_LSEL);
        compileExpression();
        eat(SB_RSEL);
    }
}

int compile(char *fileName) {
    if (openInputStream(fileName) == IO_ERROR)
        return IO_ERROR;
=======
void compileIndexes(void)
{
  while (lookAhead->tokenType == SB_LSEL)
  {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
}

int compile(char *fileName)
{
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c

    currentToken = NULL;
    lookAhead = getValidToken();

    initSymTab();

    compileProgram();

<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
    printObject(symtab->program, 0);
=======
  printObject(symtab->program, 0);
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c

    cleanSymTab();

<<<<<<< Updated upstream:compiler-construction-lab/sematic2/src/parser.c
    free(currentToken);
    free(lookAhead);
    closeInputStream();
    return IO_SUCCESS;

=======
  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
>>>>>>> Stashed changes:compiler-construction-lab/solution/PhanTichNguNghia2/day2/incompleted/parser.c
}
