/* 
 * File:   Driver.h
 * Author: t00383
 *
 * Created on 15 February 2010, 16:36
 */

#ifndef _DRIVER_H
#define	_DRIVER_H

#include "TxtParser.h"
#include "MtxLP.h"
#include "SplitLP.h"
//#include "ThermoLP.h"
//#include "TfbaLP.h"
#include <iostream>//w/
#include <algorithm>
#include "GFA.h" //w/gfa/
#include "GNI.h" //w/gni/
#include "DPA.h" //w/dpa/
#include <cmath>//w/gni/


//default option values
#define PROBLEM         "objvalue"
#define DEF_MAX         true
#define WITHCOMMENTS    false
#define MAXROWS 5000 //t/
#define MAXCOLS 50000 //t/
#define TABEND "///" //w/
#define GURB 5//w/

//string arguments


class Driver{
//private:
protected:
    argmap ARGMAP;
	argmap GRBPROB; //w/grb/ set of problems using Gurobi
    argmap OTHPROB; //w/other common problems
    //driver parameters
    string infile;
    string expfile; //w/gfa/ and for metabolite file of gim3e
    string dpafile; //w/dpa/
    ostream* out;
    string bfile;
    string problem ;
    string target;
    bool withcomments;   //include comments into the output
    bool write;          //write the LP in a log-file
    string err_msg;

    //LP construction
    string ext;
    strvec externals;
    string arg;
    strvec args;
    stomap *obj;
    strvec *records;
    bool dosplit;
    bool dotrim;
    bool thermo;
    string xtag;

    //solver parameters
    bool max;    
    bool presolve;       //the presolver will be used
    int msg_lev;         //text output switched off
    int tm_lim;          //the number of iterations is unlimited
    double T;            //temperature in Kelvin (for TMFA)
    double cmin;         //minimal concentration
    double cmax;         //maximal concentration
    double mip_gap;      //used in MILP
    double vmax;         //
    double epsilon;      //tolerance for rounding errors and fva
    bool withzeroes;
    
    Model* model;
    MtxLP* lp;
    Matrix* N;
    

    //GFA construction-----------------------------
    GFA* gfa;//w/gfa/ GFA class
    flomap* Rstate;//w/gfa/ reaction state for expression log ratio
    string arr; //w/gfa/ current array name and for metabolite array of gim3e
//    double gfa_eps; //w/gfa/


    //GNI construction-----------------------------
    GNI* gni;//w/gni/
    //gene_sgni sgniOut;//w/gni/
    //gene_wgni wgniOut;//w/gni/
    //gene_wgni1 wgniOut1;//w/gni/
    ess_gene essgOut;//w/gni/

    //DPA construction-----------------------------
    DPA* dpa;//w/dpa/
    strvec arrs;//w/dpa/microarrays for test

    int get_arg(string);

protected:
    void init_args();
    void init_msg();
    void init_msg_grb();//w/grb
    void read_model();
    void read_bfile();

    virtual void fba();
    virtual void objval();
    virtual void objstat();
    virtual void ko();
    virtual void fva();
    void fva_gim3e(double value);//for gim3e
    void fsa();
    virtual void gfaa();//w/gfa/
    virtual void gfva();//w/gfa/
	virtual void gfva2();//w/gfa/
	void gfva3();//w/gfa/
    virtual void sgni();//w/gni/
    virtual void wgni(MtxLP* mlp);//w/gni/
    virtual void wgni2(MtxLP* mlp);//w/gni/
    virtual void dpaplot();//w/dpa/
    void dpasig();//w/dpa/
    //void fca();
    //void marg();
    void elmode();
    void plot();
    void plot3D();
    void uncons();
    void live();
    void essentials();
    void mss();
    void mps();
    void tmfa();
    void tfba();
    void bottlenecks();
    void tva();
    void tradeoff();// Zakrzewski's Changes
    void subset();
    void mxs(int dir);
//    void va();
    void test();
    void build();
    void build_gfa();//w/gfa/
    void parse_plot_args(PLOT_ARGS &plotargs, strvec::const_iterator &it);

public:
    Driver();
    virtual ~Driver();
    static int kind;   //solver kind: MILP;INTPT;GURB

	void set_problem(char *arg);//w/
    virtual void set_solver(char *arg);
    //void set_expfile(char *arg);//w/
    void set_msg_lev(char* arg);
    void set_output(char* arg);
    void set_objective(string);
    void set_args(string);
    void set_externals(string, bool);//ext- or internalise metabolites given in a line space-separated list
    void set_externals_gfa(string line);//w/gfa/
    void set_T(char *arg);
    void set_time(int lim);
    void set_conc_range(char *arg);
    void set_epsilon(char *arg);
    strvec get_args(strvec&, int type = CONT);
    void get_list_args(strlistvec&, strvec&, char = PLUS);
    strvec get_exts(){return externals;};//w/
    
    void initializeRun();//w/
    virtual void run();
    void Drive();

    friend int main(int, char**);
    friend bool read_record(Driver*, string);
    friend bool read_record_gfa(Driver*, string);//w/gfa/
    friend bool read_record_gni(Driver*, Model*, string);//w/gni/
    friend bool read_record_dpa(Driver*, string);//w/dpa/
    friend void read_constraint(Driver*, string, char);
    friend void read_constraint_mod(Driver*, string, char);//w/
    friend void read_sto(Driver*, string, double, double);
    friend void read_sto_mod(Driver*, string, double, double);//w/
    friend void read_objective(Driver*, string);

    static string get_msg(int nmsg);
    static string get_msg_grb(int nmsg);
    
    //---------------GFA-------------------------------------------------------
    void set_array();//w/gfa/
    void set_obj_arr();//w/gim3e/
    void get_args_gfa(strvec &reacs, strvec &genes);//w/gfa/
    strvec get_args_gfva(strvec& args);//w/gfa/
    
    //---------------GNI-------------------------------------------------------
    void del_exts();//w/gni/
    virtual void blp_fba(MtxLP* blp, BLPSOL &bsol, stomap &medium);//w/gni/
    virtual void blp_fba(MtxLP* blp, BLPSOL &bsol);//w/gni/
    virtual OBJSTAT fba_med(MtxLP* mlp, intvec rmed, stomap *mobj);//w/gni/

    //---------------DPA-------------------------------------------------------
    void set_arrays(string line);//w/dpa

};

//output
ostream &operator << (ostream &out, OBJSTAT objstat);
ostream &operator << (ostream &out, OBJSTAT_RANGE range);
ostream &operator << (ostream &out, rangemap &rm);
void print_fba_state(ostream* out, stomap* sto, flomap* Rstate, Model* model, bool withcomments);//w/gimme-fba
void print_rangemap(ostream *out, rangemap *rm, Model* model);
void print_rangemap_gfva(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments);//w/gfa/
void print_rangemap2(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments, bool state = true);//w/gfa/
void print_rangemap2_tab(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments, bool state = true);//w/gfa/
ostream &operator << (ostream &out, MtxLP &lp);
void print_rangemap_gfaa(ostream *out, reac_rangemap *Rrm, gene_rangemap *Grm, strvec reacs, Model* model, GFA* gfa, string arr, flomap* Rstate);//w/gfa/
void print_rangemap_gfaa_tab(ostream *out, reac_rangemap *Rrm, gene_rangemap *Grm, strvec reacs, Model* model, GFA* gfa, string arr, flomap* Rstate);//w/gfa/
void print_sgni(ostream *out, gene_sgni &sgniOut);//w/gni/
//void print_wgni(ostream *out, gene_wgni &wgniOut, strvec nts);//w/gni/
void print_wgni1(ostream *out, gene_wgni1 &wgniOut1, int Nsam);//w/gni/
void print_essg(ostream *out, ess_gene &essgOut, strvec nts);//w/gni/
bool myfunc_wgni (WGNI1 i, WGNI1 j);//w/gni/
void print_dpaplot(ostream *out, met_genes &metG, gene_mets &geneM, strstr2flo &plotMatrix, strvec &mets, strvec &genes);//w/dpa/
void print_dpasig(ostream *out, strvec &mets, strvec &arrs, metarr_sig &metsig);//w/dpa/


#endif	/* _DRIVER_H */







