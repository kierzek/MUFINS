/* 
 * File:   TfbaLP.h
 * Author: ag0022
 *
 * Created on 13 August 2010, 10:40
 */

#ifndef TFBALP_H
#define	TFBALP_H

#include "SplitLP.h"

#define DELTAG0_TAG "dG0_"
#define CONC_SUFX  "_conc"

#define DELTA_TYPE (INT+10)
#define LNCONC_TYPE  (DELTA_TYPE+1)
#define DELTAG0_TYPE  (DELTA_TYPE+2)

class TfbaLP : public SplitLP{
protected:
    float T;
    double RT;
    double K;
    double cmax;
    double cmin;
    intmap* types;
    Matrix* stomtx;
    int nblocked;

public:
    //initialisation, finalisation:
    TfbaLP(bool max = false, float T = TSt, int msg_lev = MSG_LEV, int tm_lim = TM_LIM, double cmin = CMIN, double cmax = CMAX, \
            int it_lim = IT_LIM, string name = string(EMPTYSTR));
    ~TfbaLP();

    virtual void addMatrix(Matrix* N);
    void addConcentrations();
    RANGE getDeltaG0(string rx)const;
    RANGE getDeltaG(string rx)const;
    void setFeasibleDirs(string rx);
    void setDeltaG0(string rx, double lb, double ub);
    void setConcentration(string met, double lb, double ub);
    void setTemperature(double T);
    virtual void update();
    void thermoBlock(string rx);
    
    virtual int getColKind(int i)const;
    virtual double getColValue(string name)const;

    float getTemperature()const;
    
    strvec getBottleNecks(stomap *obj, strvec queries, bool max = true, bool presolve = PRESOLVE);
    bool isBottleNeck(stomap *obj, string col, bool max = true, bool presolve = PRESOLVE);

    void addThermoConstraint(string rx);
    int addThermoConstraints();
    void TFBA(stomap *obj, bool max, bool presolve);


};


#endif	/* TFBALP_H */

