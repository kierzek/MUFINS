/* 
 * File:   RPN.h
 * Author: Albert Gevorgyan
 *
 * Created on 02 February 2010, 13:11
 */

#ifndef _RPN_H
#define	_RPN_H

#include "Text.h"

#define RDELIM SPACE



t_val _and(t_val a, t_val b);
t_val _or(t_val a, t_val b);
t_val _translate(string tok);

void init_operators();

bool is_lower(string op1, string op2);
bool is_binop(string s);


void dijkstra(strvec &in, strvec &out);
//void dijkstra(string in, strvec &out);

t_val evaluate(strvec toks, t_translate translate = &_translate);
//t_val evaluate(string s);

bool evaluate_ko(strvec* rule, strvec* ko);


//void rpn_test();


#endif	/* _RPN_H */

