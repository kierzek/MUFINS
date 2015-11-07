/* 
 * File:   SplitLP.h
 * Author: ag0022
 *
 * Created on 01 July 2010, 10:10
 */

#ifndef SPLITLP_H
#define	SPLITLP_H


#include "MtxLP.h"

#define NEG_TAG "-"

class SplitLP : public MtxLP{
protected:
    bool is_pos(string name)const{
        return (name.substr(0, 1) != NEG_TAG);
    }


public:
    //initialisation, finalisation:
    SplitLP(bool max = false, int kind = DFLT, int msg_lev = MSG_LEV, int tm_lim = TM_LIM, int it_lim = IT_LIM, string name = string(EMPTYSTR));


    virtual void addStoich(stomap* sto, double lb = LB, double ub = UB, string name = "");
    string get_opp_name(string name)const;
    
    virtual void fix(string name, double value, bool tmp = false, string constraintname = "");

    virtual void getSolution(stomap* sol, strvec queries, int kind = CONT, bool withzeroes = false)const;

    //elmode
    void elModes(solvec& solutions, int num, strvec &fixcols);
    //min substrate sets

    string getTransporterName(string xname, int dir)const;
    string getTransporterName(stomap *rsto, int dir)const;
    void getConversion(stomap* conv, bool withzeroes = false)const;
    void minExchangeSets(solvec& solutions, stomap* obj, int Dir = -1, long nmax = LONG_MAX);
    void minimiseInput(stomap* conv, strvec& txs);
 

};


#endif	/* SPLITLP_H */

