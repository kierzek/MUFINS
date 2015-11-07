/* 
 * File:   Round.h
 * Author: Albert Gevorgyan
 *
 * Created on 26 January 2010, 13:22
 */

#ifndef _ROUND_H
#define	_ROUND_H

#include <cfloat>
#include <climits>
#include <cmath>
///////////////////////////////
//precision
#define EPSILON DBL_EPSILON
#define EPSILON_FLUX 1e-5
#define IS_ZERO(val) ((abs(val)) <= EPSILON_FLUX)
#define EQ(a, b) (IS_ZERO((a) - (b)))


//////////////////////////////
//infinity
#define MAX_NUM DBL_MAX
#define IS_INF(val) (abs(val) >= MAX_NUM)



/*
def IsAllZero(seq):
    bools = map(lambda x: IsZero(x), seq)
    return not False in bools

def RoundFloat(val, places = 3):
    return float('%.*f' % (places, val))

def RoundSto(sto):
    for k in sto:
        sto[k] = RoundFloat(sto[k])
*/







#endif	/* _ROUND_H */

