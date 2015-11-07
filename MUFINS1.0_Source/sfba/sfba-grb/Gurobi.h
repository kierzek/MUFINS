/* 
 * File:   Gurobi.h
 * Author: John
 *
 * Created on 10 May 2011, 10:38
 */

#ifndef GUROBI_H
#define	GUROBI_H

#include <gurobi_c++.h>
//#include "gurobi_c++.h"
#include "Model.h"
//#include "GNI.h"

//#include "Types.h"
//#include "MtxLP.h"
//#include "SplitLP.h"
//#include "TxtParser.h"


#define CTUB 100
#define GrbMax -1
#define GrbMin 1

//class GRB_Env{
//public:
//    const GRBEnv env;
//    GRB_Env();
//    ~GRB_Env();
//
//};


class GRB_Mod{
private:
//public:
    GRBModel* model;
    GRBvars grbvars;
    GRBcons grbcons;
	
public:
    double soltol; //the tolorence parameter for Gurobi solvoer
    int mipfoc; //The MIPFocus parameter allows you to modify your high-level solution strategy for MIP solver

    GRB_Mod(GRBEnv env);
    ~GRB_Mod();
//    void setModel(GRB_Env *Env);
//    GRB_Mod* copy(GRBEnv* env);
//    void copyTo(GRB_Mod* newMod);
//    void update(){model->update();};
//    GRBModel copy(){return GRBModel(*model);};
    void addConstr_sig(Model* netMod, string row, double ub);
    void build(Model* model);
    void build_gfa(Model* netMod);
    void build_gfa2(Model* netMod, stomap *obj);
    void build_sgni(Model* model, stomap *obj, string biom);
    void externalise(strvec& names, bool on);
    void externalise(string external, bool on);
    void setOptDir(int dir);
    void addConstraint(stomap* sto, double lb, double ub, string name);
    OBJSTAT optimize();
    OBJSTAT optimize(GRBModel model);// for fixedModel
    OBJSTAT optimize_gfva();
    OBJSTAT optimize_gfva2(stomap *allsol, stomap *obj);
    OBJSTAT optimize_fimat(stomap *allsol, stomap *obj);
    OBJSTAT optimize_gfva2x(stomap *allsol, stomap *obj, float eps);
    OBJSTAT optimize(stomap *allsol);
    OBJSTAT optimizeFix(stomap* obj);
    stomap getAllSol();
    FLUXACT getRange_gfaa(string reac, double bound, double eps);
    void getRanges(rangemap* rm, strvec cols);
    void getRanges_gfva2(OBJSTAT &objrange, stomap *allsol, rangemap* rm, strvec cols, stomap *obj);
    void getRanges(GRBModel model, rangemap* rm, strvec cols);
    void getOptRanges(rangemap* rm, OBJSTAT &objrange, stomap* obj, strvec cols);
    void getOptRanges_gfva(rangemap* rm, OBJSTAT &objrange, strvec cols);
    void getOptRanges_gfva2(rangemap* rm, OBJSTAT &objrange, stomap *allsol, strvec cols, stomap *obj);
    void getOptRanges_gfva2x(rangemap* rm, OBJSTAT &objrange, stomap *allsol, strvec cols, stomap *obj, float eps);
    void getOptRanges_gfva_fixmod(rangemap* rm, OBJSTAT &objrange, strvec cols);
    void delCon(string name);
    void delVar(string name);
    GRBConstr getCon(string name);
    GRBVar getVar(string name);
    double getVarUB(string name);
    double getVarLB(string name);
    void setVarBnds(string name, double lb, double ub);
    void setAllVarBnds(double lb, double ub);
    void setObjs(stomap* obj);
    void setObj(string name, double coef);
    void setObj(string name);
    void setMixedObj(stomap* sto, strvec &externals);
    bool hasVar(string name);
    void ko_blp(strvec rkos, strvec rnts, grbKoRow &kor, grbKoCol &koc);
//    void ko_blp(strvec rkos, strvec rnts, grbKos &kos);
    void ko_blp(strvec rkos, grbKos &kos, reac2nts &reacN);
    void ko_mlp(strvec rkos, grbKoCol &koc);
    grbRow set_kor(string name);
    grbCol set_koc(string name);
    void rko_blp(grbKoRow &kor, grbKoCol &koc);
    void rko_blp(grbKos &kos);
    void rko_mlp(grbKoCol &koc);
    void setPara();
    void write(string fname);
};


#endif	/* GUROBI_H */

