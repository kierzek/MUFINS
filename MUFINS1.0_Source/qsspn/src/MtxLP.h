/* 
 * File:   MtxLP.h
 * Author: ag0022
 *
 * Created on 01 July 2010, 10:06
 */

#ifndef MTXLP_H
#define	MTXLP_H


#include "Model.h"



#define CONT GLP_CV  //continuous
#define INT GLP_BV //binary, since we'll hardly need other integer variables

#define EXACT   0
#define SMPLX   1
#define INTPT   2
#define MILP    3
#define DFLT    SMPLX



#define INT_TAG "int_"
#define INT_CONSTR_TAG "_constr"
#define ROW_TAG "row_"
#define COL_TAG "col_"




class MtxLP{
private:
    //function pointers
    int (MtxLP::*ptr_Solve)(bool presolve);
    double (MtxLP::*ptr_getObjValue)()const;
    double (MtxLP::*ptr_getColValue)(int)const;
    int (MtxLP::*ptr_getStatus)()const;

    ////////////////////////////////////////////
    //delegated to glpk:
    LPX *lp;
    void create_prob(){
        lp = glp_create_prob();
    }
    void delete_prob(){
        glp_delete_prob(lp);
    }
    void set_prob_name(const char *name){
	glp_set_prob_name(lp, name);
    }
    const char* get_prob_name()const{
        return glp_get_prob_name(lp);
    }
    void create_index(){
        glp_create_index(lp);
    }
    void delete_index(){
        glp_delete_index(lp);
    }
    void set_obj_dir(int dir){
        glp_set_obj_dir(lp, dir);
    }
    int get_obj_dir()const{
        return glp_get_obj_dir(lp);
    }
    void set_obj_coef(int i, double coef){
        glp_set_obj_coef(lp, i, coef);
    }
    double get_obj_coef(int i)const{
        return glp_get_obj_coef(lp, i);
    }
    int add_rows(int i){
        return(glp_add_rows(lp, i));
    }
    int add_cols(int i){
        return(glp_add_cols(lp, i));
    }
    int get_num_rows()const{
        return glp_get_num_rows(lp);
    }
    int get_num_cols()const{
        return glp_get_num_cols(lp);
    }
    void set_row_name(int i, const char *name){
        glp_set_row_name(lp, i, name);
    }
    void set_col_name(int i, const char *name){
        glp_set_col_name(lp, i, name);
    }
    const char* get_row_name(int i)const{
        return glp_get_row_name(lp, i);
    }
    const char* get_col_name(int i)const{
        return glp_get_col_name(lp, i);
    }
    void set_row_bnds(int i, int type, double lb, double ub){
        glp_set_row_bnds(lp, i, type, lb, ub);
    }
    void set_col_bnds(int i, int type, double lb, double ub){
        glp_set_col_bnds(lp, i, type, lb, ub);
    }
    int get_row_type(int i)const{
        return glp_get_row_type(lp, i);
    }
    int get_col_type(int i)const{
        return glp_get_col_type(lp, i);
    }
    double get_row_lb(int i)const{
        return glp_get_row_lb(lp, i);
    }
    double get_col_lb(int i)const{
        return glp_get_col_lb(lp, i);
    }
    double get_row_ub(int i)const{
        return glp_get_row_ub(lp, i);
    }
    double get_col_ub(int i)const{
        return glp_get_col_ub(lp, i);
    }
    void set_col_kind(int j, int kind){
        glp_set_col_kind(lp, j, kind);
    }
    int get_col_kind(int j)const{
        return glp_get_col_kind(lp, j);
    }
    int find_row(const char* name)const{
        return glp_find_row(lp, name);
    }
    int find_col(const char* name)const{
        return glp_find_col(lp, name);
    }

    void set_mat_row(int i, int len, const int ind[], const double val[]){
        glp_set_mat_row(lp, i, len, ind, val);
    }
    void set_mat_col(int i, int len, const int ind[], const double val[]){
        glp_set_mat_col(lp, i, len, ind, val);
    }
    void load_matrix(int ne, const  int ia[], const int ja[], const double ar[]){
        glp_load_matrix(lp, ne, ia, ja, ar);
    }
    void del_rows(int nrs, const int num[]){
        glp_del_rows(lp, nrs, num);
    }
    void del_cols(int ncs, const int num[]){
        glp_del_cols(lp, ncs, num);
    }
    int get_mat_row(int i, int ind[], double val[])const{
        return(glp_get_mat_row(lp, i, ind, val));
    }
    int get_mat_col(int i, int ind[], double val[])const{
        return(glp_get_mat_col(lp, i, ind, val));
    }

    //solvers
    int simplex(const glp_smcp *parm = NULL){
        return glp_simplex(lp, parm);
    }
    int interior(const glp_iptcp *parm = NULL){
        return glp_interior(lp, parm);
    }
    int intopt(const glp_iocp *parm = NULL){
        return glp_intopt(lp, parm);
    }
    int exact(const glp_smcp *parm = NULL){
        return glp_exact(lp, parm);
    }

    //initialising parameters
    void init_smcp(glp_smcp *parm){
        glp_init_smcp(parm);
    }
    void init_iptcp(glp_iptcp* parm){
        glp_init_iptcp(parm);
    }
    void init_iocp(glp_iocp *parm){
        glp_init_iocp(parm);
    }

    double get_obj_val()const{
        return glp_get_obj_val(lp);
    }
    double mip_obj_val()const{
        return glp_mip_obj_val(lp);
    }
    double get_col_prim(int i)const{
        return glp_get_col_prim(lp, i);
    }
    double get_col_dual(int i)const{
        return glp_get_col_dual(lp, i);
    }
    double mip_col_val(int j)const{
        return glp_mip_col_val(lp, j);
    }
    int get_status()const{
        return glp_get_status(lp);
    }
    int mip_status()const{
        return glp_mip_status(lp);
    }

    int warm_up(){
        return glp_warm_up(lp);
    }
    void adv_basis(int flags){
        glp_adv_basis(lp, flags);
    }
    void std_basis(){
        glp_std_basis(lp);
    }
    void set_int_parm(int p, int val){
        lpx_set_int_parm(lp, p, val);
    }
    int ipt_status()const{
        return glp_ipt_status(lp);
    }
    double ipt_obj_val()const{
        return glp_ipt_obj_val(lp);
    }
    double ipt_col_prim(int i)const{
        return glp_ipt_col_prim(lp, i);
    }

    int write_lp(const char *fname){
        return glp_write_lp(lp, NULL, fname);
    }
    int write_mps(const char *fname){
        return glp_write_mps(lp, GLP_MPS_FILE, NULL, fname);
    }

protected:
    /////////////////////////////////////////////////
    //result:
    int ret_cod;
    /////////////////////////////////////////////////
    //settings:
    int mtxlen;
    double vmax;
    int kind;
    int msg_lev;
    int tm_lim;
    int it_lim;
    double mip_gap;
    //////////////////////////////////////////////
    //temporary constraints and bounds
    strstack tmp_rows;
    strvec tmp_externals;
    /////////////////////////////////////////////////
    //inline methods
    bool is_zero(double val)const{
        return (kind == EXACT) ? (val == 0) : IS_ZERO(val);
    }
    int nrow(string name)const{
        return find_row(name.data());
    }
    int ncol(string name)const{
        return find_col(name.data());
    }
    int bounds2type(double lb, double ub)const{
        int type;
        bool free_lb = IS_INF(lb);
        bool free_ub = IS_INF(ub);
        if (free_lb && free_ub)
            type = GLP_FR;//free
        else if (free_lb)
            type = GLP_UP;//upper bound
        else if (free_ub)
            type = GLP_LO;//lower bound
        else if EQ(lb, ub)
            type = GLP_FX;//fixed
        else
            type = GLP_DB;//double bounded
        return type;
    }
    ///////////////////////////////////////////////
    //problem-specific
    string get_int_name(string name)const{
        return string(INT_TAG) + name;
    }
    strvec get_int_names(strvec names);

public:
    //initialisation, finalisation:
    MtxLP(bool max = false, int kind = DFLT, int msg_lev = MSG_LEV, int tm_lim = TM_LIM, int it_lim = IT_LIM, string name = string(EMPTYSTR));
    ~MtxLP();

    virtual void update();

    void createProb();
    void deleteProb();

    void setName(string name);
    string getName()const;

    void setKind(int kind);
    int getKind()const;

    void setMipGap(double mip_gap){
        this->mip_gap = mip_gap;
    }

    //objective
    void setObjDir(bool max);
    bool isMaximising()const;
    double getObjCoef(string name)const;
    void getObjective(stomap* obj, bool withzeroes = false)const;

    void setObjCoef(string col, double coef);
    void emptyObjective();

    void setObjective(double coef = 1);
    void setObjective(stomap* sto);
    void setObjective(strvec cols, double coef = 1);

    void updateObjCoef(string col, double coef);
    void updateObjective(stomap *sto, double coef = 1);
    void setRowObjCoef(string row, double coef = 1);
    void setMixedObjective(stomap* sto);

    void setLenObjective(bool tmp = false);
    virtual void setLenObjective(strvec cols, bool tmp = false);
    void setVMax(double vmax);

    //retrieving rows, columns
    virtual int getColKind(int i)const;

    bool hasRow(string name)const{
        return nrow(name) != 0;
    }
    bool hasCol(string name)const{
        return ncol(name) != 0;
    }
    virtual string getID(string colname){
        return colname;
    }
    int getNumRows()const;
    int getNumCols()const;
    strvec getRowNames()const;
    strvec getColNames(int kind = CONT)const;
    string newRowName(string name = "")const;
    string newColName(string name = "")const;
    strstack getTmpRows()const;

    int getRowType(string name)const;
    int getColType(string name)const;
    double getRowLB(string name)const;
    double getRowUB(string name)const;
    double getColLB(string name)const;
    double getColUB(string name)const;

    stomap* getRowSto(string name)const;
    stomap* getColSto(string name)const;

    bool isExternal(string)const;//whether the metabolite met is external 
    strvec getExternals()const;//all externals
    strvec getTransporters()const;
    strvec getImporters()const;
    strvec getExporters()const;

    //modifying rows, columns
    int addRow(string name, double lb = 0, double ub = 0);
    int addCol(string name, double lb = LB, double ub = UB);
    void attachIntVar(string name);

    void delRow(string name);
    void delCol(string name);

    void setRowBnds(string name, double lb = 0, double ub = 0);//steady state!
    void setColBnds(string name, double lb = LB, double ub = UB);//>= UB means unbounded!
    void externalise(strvec& names, bool on = true);

    void addConstraint(stomap* sto, double lb = LB, double ub = UB, bool tmp = false, string name = "");
    void setColRange(string name, double lb, double ub, bool tmp = false);
    virtual void fix(string name, double value, bool tmp = false, string constraintname = "");
    void fix(stomap *sto, bool tmp = false);
    void block(string name, bool tmp = false, string constraintname = "");
    void block(strvec *names, bool tmp = false);
    int fixSolution(int kind = CONT, bool tmp = false);
    int fixSupport(int kind = CONT, bool tmp = false);
    void cutSolution(stomap *sol, bool tmp = false);

    virtual void addStoich(stomap* sto, double lb = LB, double ub = UB, string name = "");
    virtual void addMatrix(Matrix* N);

    void cleanTmpRows(int n = -1);

    ///////////////////////////////////////////////////////////////////////////////////////
    //solving: wrappers and to-be-assigned-to-the-pointer functions:
    void Solve(bool presolve = PRESOLVE);
    int Simplex(bool presolve);
    int Interior(bool presolve);
    int Intopt(bool presolve);
    int Exact(bool presolve);


    ///////////////////////////////////////////////////////////////////////////////////////
    //retrieving solution
    double getObjValue()const;
    virtual double getColValue(string name)const{
        return (*this.*ptr_getColValue)(ncol(name));
    }
    int getStatus()const{
        return (*this.*ptr_getStatus)();
    }
    bool isStatusOptimal()const{
        return getStatus() == GLP_OPT;
    }
    bool isSolved()const;
    void getSolution(stomap* sol, int kind = CONT, bool withzeroes = false)const;
    virtual void getSolution(stomap* sol, strvec queries, int kind = CONT, bool withzeroes = false)const;
    void getSupport(strvec& support, strvec queries, int kind = CONT)const;
    void getSupport(strvec& support, int kind = CONT)const;
    void getObjStat(OBJSTAT &objstat);

    ///////////////////////////////////////////////////////////////////////////////////////
    //specific problems
    void optimise(stomap* obj, bool max = true, bool presolve = PRESOLVE);
    void optimiseSum(bool max, strvec queries, bool presolve = PRESOLVE);
    void optSumSol(stomap* sol, bool max, strvec queries, bool withzeroes = false, bool presolve = PRESOLVE);
    void optimiseLen(bool max, strvec queries, bool presolve = PRESOLVE);
    void optLenSol(stomap* sol, bool max, strvec queries, bool withzeroes = false, bool presolve = PRESOLVE);
    //objvalue
    double getOptVal(stomap* obj, bool max = true, bool presolve = PRESOLVE);
    //objstat
    void getOptObjStat(OBJSTAT &objstat, stomap* obj, bool max = true, bool presolve = PRESOLVE);
    //FBA
    void getOptSol(stomap* sol, stomap* obj, bool max = true, int kind = CONT, bool withzeroes = false, bool presolve = PRESOLVE);
    //FVA
    void getRange(OBJSTAT_RANGE &range, stomap* obj, bool presolve = PRESOLVE);
    virtual void getColRange(OBJSTAT_RANGE &range, string cname, bool presolve = PRESOLVE);
    void getRanges(rangemap* rm, strvec queries, bool presolve = PRESOLVE);
    void fixSolution(stomap* obj, bool max = true, bool tmp = false, bool withzeroes = false, bool presolve = PRESOLVE, double scale = 1);
    void getOptRanges(rangemap* rm, OBJSTAT &obrange, stomap* obj, strvec queries, bool presolve = PRESOLVE);
    void getMarginalCost(OBJSTAT& objstat, string name, stomap* obj, double point, double difratio = FIXRATE, bool max = true, bool presolve = PRESOLVE);
   //liveness
    strvec getUnblockedVars(strvec queries, bool presolve = PRESOLVE);
    bool isUnBlocked(string name, bool presolve = PRESOLVE);
    //plot
    void plot(pointvec* plot, stomap *obj, PLOT_ARGS plotargs, bool max = true, bool presolve = PRESOLVE);
    void plot3D(pointvec* plot, stomap *obj, PLOT_ARGS xargs, PLOT_ARGS yargs, bool max = true, bool presolve = PRESOLVE);

    //essentiality
    bool isEssential(stomap *obj, string col, bool max = true, bool presolve = PRESOLVE);
    bool isCutSet(stomap *obj, strvec set, bool max = true, bool presolve = PRESOLVE);
    void ko(OBJSTAT& rv, stomap *obj, strvec blockvec, bool max = true, bool presolve = PRESOLVE);
    void ko(objstatvec& rv, stomap *obj, strlistvec blockvecs, bool max = true, bool presolve = PRESOLVE);
    strvec getEssentials(stomap *obj, strvec queries, bool max = true, bool presolve = PRESOLVE);
    bool isBottleNeck(stomap *obj, string col, bool max = true, bool presolve = PRESOLVE);
    strvec getBottleNecks(stomap *obj, strvec queries, bool max = true, bool presolve = PRESOLVE);
    //unconserved metabolites
    strvec unconsRows(bool presolve = PRESOLVE);
    ///////////////////////////////////////////////////////////////////////////////////////
    //IO
    int writeLP(string filename);
    int writeMPS(string filename);


};




#endif	/* MTXLP_H */

