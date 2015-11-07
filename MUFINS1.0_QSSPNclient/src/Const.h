/* 
 * File:   Const.h
 * Author: Albert Gevorgyan
 *
 * Created on 05 February 2010, 10:52
 */

#ifndef _CONST_H
#define	_CONST_H

extern "C"
{
  #include "glpk/glpk.h"
}

#include "glpk/glpk.h"
#include "Round.h"

#define LB -MAX_NUM
#define UB MAX_NUM

#define MSG_LEV GLP_MSG_OFF


#define FIXRATE 0.001
#define VMAX    100000//000
#define CMIN_    (log(EPSILON))
#define CMAX    (log(UB))

#define DGMAX   1000000 //00
#define DG_EPSILON EPSILON_FLUX

#define T0_      (float(273.15))
#define TSt     298.15

#define R_       8.3143 //0.0083143//
#define F_       96.487

#define MIP_GAP 0.0001

#define PRESOLVE true
#define TM_LIM  INT_MAX
#define IT_LIM  INT_MAX


#endif	/* _CONST_H */

