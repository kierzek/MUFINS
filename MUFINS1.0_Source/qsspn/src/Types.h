/* 
 * File:   Types.h
 * Author: Albert Gevorgyan
 *
 * Created on 20 January 2010, 11:07
 */

#ifndef _TYPES_H
#define	_TYPES_H

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <cstdio>

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iterator>
#include <set>

#include "Const.h"

using namespace std;

//container types; may be redefined
typedef vector <string> strvec;
typedef map<string, double> stomap; //reaction stoichiometry, mapping metabolite names to coefs.
typedef map<string, int> argmap;
typedef map<int, string> constmap;
typedef map<int, int>intmap;

typedef bool t_val;
typedef t_val (*op)(t_val, t_val);
typedef t_val (*t_translate)(string);
typedef map<string, op> opmap;//mapping names to binary operators

typedef stack<string> strstack;
typedef stack<t_val> tstack;

typedef ostream_iterator<string>  ostrit;
typedef istream_iterator<string>  istrit;

typedef struct{
    double min;
    double max;
}RANGE;
typedef struct{
    double objval;
    int status;
}OBJSTAT;
typedef struct{
    OBJSTAT min;
    OBJSTAT max;
}OBJSTAT_RANGE;
typedef vector<OBJSTAT> objstatvec;
typedef map<string, OBJSTAT_RANGE> rangemap;

typedef vector<double> POINT;
typedef vector<POINT> pointvec;

typedef struct{
    string query;
    double from;
    double to;
    double step;
}PLOT_ARGS;


typedef struct{
    strvec rows;
    strvec cols;
}MATRIX;
typedef vector<MATRIX> mtxvec;


typedef vector<stomap> solvec;
typedef set<string> striset;
typedef map<string, string> strmap;
typedef vector<strvec> strlistvec;

#endif	/* _TYPES_H */



