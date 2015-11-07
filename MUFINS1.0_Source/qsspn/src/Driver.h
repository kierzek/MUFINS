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
#include "ThermoLP.h"
#include "TfbaLP.h"


//default option values
#define PROBLEM         "objvalue"
#define DEF_MAX         true
#define WITHCOMMENTS    false


//string arguments


class Driver{
private:
    argmap ARGMAP;
    //driver parameters
    string infile;
    ostream* out;
    string bfile;
    string problem ;
    string target;
    bool withcomments;   //include comments into the output
    bool write;         //write the LP in a log-file
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
    bool updateRequired;
    bool max;
    int kind;   //MILP;//INTPT;//                //the solver algorithm is MILP
    bool presolve;            //the presolver will be used
    int msg_lev;         //text output switched off
    int tm_lim;           //the number of iterations is unlimited
    double T;             //temperature in Kelvin (for TMFA)
    double cmin;          //minimal concentration
    double cmax;           //maximal concentration
    double mip_gap;
    double vmax;
    
    Model* model;
    MtxLP* lp;
    Matrix* N;


    int get_arg(string);

protected:
    void init_args();
    void init_msg();

    void read_model();

    void read_bfile();

    void fba();
    void objval();
    void objstat();
    void ko();
    void fva();
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

    void mxs(int dir);
    void va();
    void test();
    

    void build();
    void parse_plot_args(PLOT_ARGS &plotargs, strvec::const_iterator &it);

public:
    Driver();
    virtual ~Driver();

    void set_solver(char *arg);
    void set_msg_lev(char* arg);
    void set_output(char* arg);
    void set_objective(string);
    void set_args(string);
    void set_externals(string, bool);//ext- or internalise metabolites given in a line space-separated list
    void set_T(char *arg);
    void set_conc_range(char *arg);
    strvec get_args(strvec&, int type = CONT);
    void get_list_args(strlistvec&, strvec&, char = PLUS);
	
    void run();
    void Drive();

    friend int main(int, char**);
    friend bool read_record(Driver*, string);
    friend void read_constraint(Driver*, string, char);
    friend void read_sto(Driver*, string, double, double);
    friend void read_objective(Driver*, string);

    static string get_msg(int nmsg);    
	
	/***** Added by Andrzej Kierzek *****/
	void setMax(bool m) {
		max = m;	
	}
	
    void setUpdateRequired(bool b) {
        updateRequired = b;
    }
    
    bool isUpdateRequired() {
        return updateRequired;
    }
	
	void setInfile(string s) {
	    infile = s;	
	}
	void setXtag(string s) {
	    xtag = s;	
	}
	void setObjective(string s) {
	    set_objective(s);	
	}
	MtxLP *getLP() {
		return lp;
	}
	void updateLP() {
		lp->update();
	}
    void buildLP();
	double calcObjValue();
	
};
ostream &operator << (ostream &out, OBJSTAT objstat);
ostream &operator << (ostream &out, OBJSTAT_RANGE range);
ostream &operator << (ostream &out, rangemap &rm);
void print_rangemap(ostream *out, rangemap *rm, Model* model);
ostream &operator << (ostream &out, MtxLP &lp);

#endif	/* _DRIVER_H */







