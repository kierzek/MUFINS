/* 
 * File:   ThermoLP.h
 * Author: ag0022
 *
 * Created on 21 July 2010, 11:21
 */

#ifndef THERMOLP_H
#define	THERMOLP_H

#include "SplitLP.h"

#define DELTAG_TAG "dG_"
#define CONC_SUFX   "_conc"

#define DELTA_TYPE (INT+10)
#define LNCONC_TYPE  (DELTA_TYPE+1)
#define DELTAG0_TYPE  (DELTA_TYPE+2)

class ThermoLP : public SplitLP{
protected:
    float T;
    double RT;
    double K;
    double cmax;
    double cmin;
    intmap* types;
    Matrix* stomtx;
    
public:
    //initialisation, finalisation:
    ThermoLP(bool max = false, float T = TSt, int msg_lev = MSG_LEV, int tm_lim = TM_LIM, double cmin = CMIN_, double cmax = CMAX, \
            int it_lim = IT_LIM, string name = string(EMPTYSTR));
    ~ThermoLP();

    virtual int getColKind(int i)const;
    virtual double getColValue(string name)const;
    virtual void addMatrix(Matrix* N);
    void addConcentrations();
    void addThermoCols();
    void setDeltaG0(string rx, double lb, double ub);
    //virtual void addIntConstraints(strvec cols, bool tmp = false);
    virtual void addIntConstraints(string name, string deltaname, int deltacoef = 1);
    virtual void update();
    string concName(string metname){return "[" + metname + "]";}
    void setConcentration(string met, double lb, double ub);
    void setTemperature(double T);
    RANGE getDeltaGRange(string rx, Matrix* N);
    virtual void getColRange(OBJSTAT_RANGE &range, string cname, bool presolve = PRESOLVE);
    void addThermoConstraints(string rx, RANGE DeltaG0);
    virtual string getID(string colname);
};


#endif	/* THERMOLP_H */

