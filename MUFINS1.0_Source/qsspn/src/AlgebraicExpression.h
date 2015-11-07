//
//  AlgebraicExpression.h
//  AlgebraicExpression
//
//  Created by Andrzej Kierzek on 24/12/2013.
//  Copyright (c) 2013 Andrzej Kierzek. All rights reserved.
//

using namespace std;
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

class AlgebraicExpressionToken {
public:
    bool isOperator;
    bool isConstant;
    bool isVariable;
    bool isLeftBracket;
    bool isRightBracket;
    double value;
    int precedence;
    double *reference;
    int o;
    AlgebraicExpressionToken(double *r) {
        isOperator = false;
        isConstant = false;
        isVariable = true;
        isLeftBracket = false;
        isRightBracket = false;
        reference = r;
        precedence = 0;
        o = -1;
        dereference();
    }
    AlgebraicExpressionToken(double v) {
        isOperator = false;
        isConstant = true;
        isVariable = false;
        isLeftBracket = false;
        isRightBracket = false;
        reference = NULL;
        precedence = 0;
        o = -1;
        value = v;
    }
    AlgebraicExpressionToken(string s) {
        isOperator = true;
        isConstant = false;
        isVariable = false;
        isLeftBracket = false;
        isRightBracket = false;
        value = 0;
        if(s == "+") {
            o = 1;
            precedence = 1;
        } else if (s == "-") {
            o = 2;
            precedence = 1;
        } else if (s == "*") {
            o = 3;
            precedence = 5;
        } else if (s == "/") {
            o = 4;
            precedence = 5;
        } else if (s == "^") {
            o = 5;
            precedence = 10;
        } else if (s=="(") {
            isLeftBracket = true;
            isOperator = false;
            precedence = 0;
        } else if (s==")") {
            isRightBracket = true;
            isOperator = false;
            precedence = 0;
        }
    }
    void dereference() {
        value = *reference;
    }
};

class AlgebraicExpression {
private:
    string expression;
    vector<double*> vars;
    vector<string> names;
    vector<string> tokenised_expression;
    vector<AlgebraicExpressionToken> operator_stack;
    vector<AlgebraicExpressionToken> parsed_expression;
    vector<double> stack;
    
    void tokenise();
    bool isOperator(char c) {
        if(c=='+') return(true);
        if(c=='-') return(true);
        if(c=='*') return(true);
        if(c=='/') return(true);
        if(c=='^') return(true);
        return(false);
    }

    bool isDigit(char c) {
        if(c == '0') return(true);
        if(c == '1') return(true);
        if(c == '2') return(true);
        if(c == '3') return(true);
        if(c == '4') return(true);
        if(c == '5') return(true);
        if(c == '6') return(true);
        if(c == '7') return(true);
        if(c == '8') return(true);
        if(c == '9') return(true);
        return(false);
    }
    bool isBracket(char c) {
        if(c == '(') return(true);
        if(c == ')') return(true);
        return(false);
    }
    bool isNumber(string s) {
        size_t i;
        for(i=0;i<s.size();i++) {
            if(s[i]=='-' && i == 0 && s.size()>1) continue;
            if(s[i]=='.') continue;
            if(!isDigit(s[i])) {
                return(false);
            }
        }
        return(true);
    }
    bool isVariable(string s) {
        if(isNumber(s)) return(false);
        if(s.size()==1 && isOperator(s[0])) return(false);
        if(s.size()==1 && isBracket(s[0])) return(false);
        return(true);
    }
    
public:
    AlgebraicExpression(string s, vector<string> n, vector<double*> v) {
        expression = s;
        names = n;
        vars = v;
        tokenise();
    }
    AlgebraicExpression() {
        
    }
    string getExpression() {
        return expression;
    }
    size_t getNumberOfTokens() {
        return(tokenised_expression.size());
    }
    string getToken(size_t i) {
        return(tokenised_expression[i]);
    }
    int size() {
        return((int)vars.size());
    }
    double getVariable(int i);
    
    void parse();
    double evaluate();
    
    void printTokenisedExpression() {
        for(vector<AlgebraicExpressionToken>::iterator it = parsed_expression.begin();it != parsed_expression.end();++it) {
            if(it->isConstant) cout << it->value << endl;
            if(it->isVariable) cout << it->value << endl;
            if(it->isOperator) {
                if(it->o == 1) cout << "+" << endl;
                if(it->o == 2) cout << "-" << endl;
                if(it->o == 3) cout << "*" << endl;
                if(it->o == 4) cout << "/" << endl;
                if(it->o == 5) cout << "^" << endl;
            }
            if(it->isLeftBracket) cout << "(" << endl;
            if(it->isRightBracket) cout << ")" << endl;
        }
    }
    void printOperatorStack() {
        for(vector<AlgebraicExpressionToken>::iterator it = operator_stack.begin();it != operator_stack.end();++it) {
            if(it->isConstant) cout << it->value << endl;
            if(it->isVariable) cout << it->value << endl;
            if(it->isOperator) {
                if(it->o == 1) cout << "+" << endl;
                if(it->o == 2) cout << "-" << endl;
                if(it->o == 3) cout << "*" << endl;
                if(it->o == 4) cout << "/" << endl;
                if(it->o == 5) cout << "^" << endl;
            }
            if(it->isLeftBracket) cout << "(" << endl;
            if(it->isRightBracket) cout << ")" << endl;
        }
    }
};