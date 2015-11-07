/* 
 * File:   GFA.h
 * Author: John
 *
 * Created on 06 January 2011, 09:57
 */

#ifndef GFA_H
#define	GFA_H

#include "MtxLP.h"
#include "SplitLP.h"
#include "TxtParser.h"



class GFA {

//private:
public:
    strvec arrName;// array names and metabolite array names of gim3e
    strstr2int arrexp; //[gene][arr] for discrete expression: -1, 0, 1
    strstr2flo arrexp_f;//w/ for float ratio expression
    strstr2int metexp; //gim3e/ [met][arr] for discrete trunover metabolite expression: 0, 1(detected)

    double eps; //w/ threshold for judging positive flux
    gene_reactions geneR; //w/ gene to reactions for current model


public:
    GFA();
    ~GFA();

//    void set_gfa_eps(double gfa_eps);
    void read_expression(string expfile, string err_msg); //w/
    void read_expression_f(string expfile, string err_msg);//w/
    void read_expression_met(string metfile, string err_msg);//w/gim3e/
//    void ini_gmodel(string arr, Model* model);
//    inline void ini_gmodel(string arr, Model* model){gmodel[arr].model=*model;};
    
    inline int get_eps(){return eps;};
    inline bool exist_gene(string Gn){return arrexp.find(Gn)!=arrexp.end();};
    inline int get_gene_exp(string Gn, string arr){return arrexp[Gn][arr];};
    inline strvec get_geneReacs(string Gn){return geneR[Gn];};

    void split_model_rev(Model* tmod);//w/
    void set_gfa_model(string arr, Model* tmod, stomap *obj, flomap* Rstate, strvec &externals); //w/
    void set_gfa_model_gfva(string arr, Model* tmod, stomap *obj, flomap* Rstate); //w/
    void set_gfa_model_gfva2(string arr, Model* tmod, stomap *obj, flomap* Rstate);
    void set_gfa_model_fimat(string arr, Model* tmod, stomap *obj, flomap* Rstate);
    void set_gfa_model_gfva3(string arr, Model* tmod, stomap *obj, flomap* Rstate);
    void set_gfa_model_gimme(string arr, Model* tmod, stomap *obj, flomap* Rstate);
    int eval_reaction_state(Reaction* reac, string arr, flomap* Rstate);//w/
    double eval_reaction_state_f(Reaction* reac, string arr, flomap* Rstate);//w/
    int rule2state(strvec rules, int *k, string arr, int size);//w/
    double rule2state_f(strvec rules, int *k, string arr, int size);//w/
    int tri_bool_logic(int a, int b, string op);//w/
    double tri_bool_logic_f(double a, double b, string op);//w/

    FLUXDIR compute_reac_fluxact(string Rn, Model* model, MtxLP* lp, stomap *obj, bool presolve);//w/
    FLUXACT compute_gene_fluxact(reac_rangemap *Rrm, strvec Rs, Model* model, MtxLP* lp, stomap *obj, bool presolve);//w/
    FLUXACT get_fluxact_dir(reac_rangemap *Rrm, string Rn, Model* model, MtxLP* lp, stomap *obj, bool presolve);//w/
    int get_expvalue(string genename, string arr);//w/
    double get_expvalue_f(string genename, string arr);//w/

    void get_args_gfa(Model* model, strvec &args, strvec &reacs, strvec &genes);

    //------------------------GIM3E------------------------
    void set_gim3e_model_split(Model* tmod);
    void set_gim3e_model_TS(Model* tmod, string mar, double tol);
    void set_gim3e_model_gene(string arr, Model* tmod, stomap *obj, flomap* Rstate);
};

#endif	/* GFA_H */



