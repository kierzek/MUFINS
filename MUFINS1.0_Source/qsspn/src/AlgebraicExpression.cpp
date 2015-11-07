//
//  AlgebraicExpression.cpp
//  AlgebraicExpression
//
//  Created by Andrzej Kierzek on 24/12/2013.
//  Copyright (c) 2013 Andrzej Kierzek. All rights reserved.
//

#include "AlgebraicExpression.h"

double AlgebraicExpression::getVariable(int i){
    AlgebraicExpressionToken *token = new AlgebraicExpressionToken(vars[i]);
    return(token->value);
}

void AlgebraicExpression::tokenise(){
    size_t start,i,j;
    char buff[500];
    
    if(expression.size()==0) {
        cout << "Expected algebraic expression. Found empty string" << endl;
        exit(0);
    }
    
    j=0;
    for(i=0;i<expression.size();i++) {
        if(expression[i]==' ') continue;
        if(expression[i]=='\t') continue;
        buff[j]=expression[i];
        j++;
    }
    buff[j]=0;
    expression = string(buff);
    
    start = 0;
    for(i=0;i<expression.size();i++) {
        if(isOperator(expression[i]) || isBracket(expression[i])) {
            if(i!=start) tokenised_expression.push_back(*new string(expression,start,i-start));
            start = i+1;
            tokenised_expression.push_back(*new string(expression,i,1));
        }
    }
    if(!isBracket(expression[expression.size()-1]))tokenised_expression.push_back(*new string(expression,start,i-start));
    
    for(i=0;i<tokenised_expression.size();i++) {
        if(tokenised_expression[i] == "-") {
            if(isOperator(tokenised_expression[i-1][0]) && isDigit(tokenised_expression[i+1][0])) {
                tokenised_expression[i+1] = tokenised_expression[i] + tokenised_expression[i+1];
                for(j=i;j<tokenised_expression.size();j++) tokenised_expression[j] = tokenised_expression[j+1];
                tokenised_expression.pop_back();
            }
        }
    }
    
}


void AlgebraicExpression::parse() {
    double v;
    
    if(isOperator(tokenised_expression[0][0])) {
        cout << "Formula " << expression << " cannot start with operator" << endl;
        exit(0);
    }
    
    // Take in turn each input token.
    for(int i=0;i<tokenised_expression.size();i++) {
        
        
        // If token is number or variable pass it to output queue
        if(isNumber(tokenised_expression[i])) {
            v = atof(tokenised_expression[i].c_str());
            parsed_expression.push_back(*new AlgebraicExpressionToken(v));
            continue;
        }
        if(isVariable(tokenised_expression[i])) {
            bool found = false;
            for(int j=0;j<names.size();j++)
                if(names[j] == tokenised_expression[i])
                {
                    parsed_expression.push_back(*new AlgebraicExpressionToken(vars[j]));
                    found=true;
                }
            if(!found) {
                cout << "Unknown variable " << tokenised_expression[i] << " in " << expression << endl;
                exit(0);
            }
                
            continue;
        }
        
        //If token is operator o1
        if(isOperator(tokenised_expression[i][0])) {
            AlgebraicExpressionToken o1 = *new AlgebraicExpressionToken(tokenised_expression[i]);
            
            // While there is operator token o2 at the top of the stack
            while(operator_stack.size()>0) {
                size_t last = operator_stack.size()-1;
                AlgebraicExpressionToken o2 = operator_stack[last];  // look at the operator o2 on top of the stack
                // if o1 is left associative and its precedence equals o2 or o1 has precedence less than o2
                if((o1.o!=5 && o1.precedence == o2.precedence) || o1.precedence<o2.precedence) {                           parsed_expression.push_back(o2);
                    operator_stack.pop_back();
                } else {
                    break;
                }
            }
            // Push o1 onto stack
            operator_stack.push_back(o1);
            continue;
        }
        
        // If token is a barcket
        if(isBracket(tokenised_expression[i][0])) {
          AlgebraicExpressionToken token = *new AlgebraicExpressionToken(tokenised_expression[i]);
            
          //If the token is a left parenthesis, then push it onto the stack.
          if(token.isLeftBracket) {
                operator_stack.push_back(token);
                continue;
          }
            
          // If the token is a right parenthesis:
          if(token.isRightBracket) {
              
              //Until the token at the top of the stack is a left parenthesis, pop operators off the stack
              while(operator_stack.size()>0) {
                  int last = (int)operator_stack.size() - 1;
                  if(operator_stack[last].isLeftBracket) {
                      operator_stack.pop_back();
                      break;
                  }
                  parsed_expression.push_back(operator_stack[last]);
                  operator_stack.pop_back();
                  if(operator_stack.size()==0) {
                      cout << "Unmatched parentheses in: " << expression << endl;
                      exit(0);
                  }
              }
              
          }
        }
    }
    
    // While there are still operator tokens on the stack, pop them all into output.
    while(operator_stack.size()>0) {
      size_t last = operator_stack.size()-1;
        parsed_expression.push_back(operator_stack[last]);
        if(operator_stack[last].isLeftBracket || operator_stack[last].isRightBracket) {
            cout << "Unmatched parentheses in: " << expression << endl;
            exit(0);
        }
        operator_stack.pop_back();
    }
    
}

double AlgebraicExpression::evaluate() {
    double a, b, result;
    for(int i=0;i<parsed_expression.size();i++) {
        if(!parsed_expression[i].isOperator) {
            if(parsed_expression[i].isVariable) parsed_expression[i].dereference();
            stack.push_back(parsed_expression[i].value);
            continue;
        }
        b = stack[stack.size()-1]; stack.pop_back();
        a = stack[stack.size()-1]; stack.pop_back();
        switch (parsed_expression[i].o) {
            case 1:
                stack.push_back(a+b);
                break;
            case 2:
                stack.push_back(a-b);
                break;
            case 3:
                stack.push_back(a*b);
                break;
            case 4:
                stack.push_back(a/b);
                break;
            case 5:
                stack.push_back(pow(a,b));
                break;
        }
    }
    
    result = stack[0]; stack.pop_back();
    return(result);
}
