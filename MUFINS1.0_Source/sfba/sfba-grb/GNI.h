/* 
* File:   GNI.h
* Author: john
*
* Created on 26 May 2011, 14:33
*/

#ifndef GNI_H
#define	GNI_H

#include "MtxLP.h"
#include "SplitLP.h"
#include "TxtParser.h"
#include "Gurobi.h"

class GNI {

    //private:
public:
    static strvec genes;//gni/all genes of model
    static gene_reactions geneR; //gni/ gene to reactions for current model
    strvec rnts;//gni/reactions of nutrients
    strvec nts;//gni/a set of nutrients for investigation
    strvec ntsp;//gni/a set of nutrients to be set present
    strvec ntsa;//gni/a set of nutrients to be set absent
    reac2nts reacN;//gni/ exchange reaction to nutrients/x/
    //    strstr2flo ntR;//gni/
    strstr2flo xtR;//gni/ for all external nutrients
    int Nsam;//gni/
    string bm;//biomass reaction name

public:
    GNI();
    ~GNI();


    void gene_koreacs(Model* model);//gni/
    void koGenes(strvec &rules, int k1, int k2, string Rn);
    //    strvec getGenes(){return genes;};
    strvec getRkos(string Gn){return geneR[Gn];};
    strvec getRnts(string Rn){return reacN[Rn];};
    //    stomap getNtR(string nt){return ntR[nt];};
    //    void reac_nts();//gni/
    //    void map_nt_reac(Model* model);//gni/
    void map_xt_sto(Model* model, strvec externals);//gni/
    void set_nt_abs(string nt, Model* model, bool abs=true);
    void set_nt_abs(string nt, Model* model, MtxLP* mlp, bool abs=true);
    void set_nt_abs_grb(string nt, Model* model, GRB_Mod *mod, bool abs=true);
    void set_nts(string line, strvec externals, Model* model);//gni/
    void set_ntsp(string line);
    void set_ntsa(strvec externals);
    void fix_nts_grb(strvec externals, Model* model, GRB_Mod *grbmod);
    void fix_absNts_grb(strvec externals, Model* model, GRB_Mod *grbmod);
    void fix_absNts_grb(Model* model, GRB_Mod *grbmod);
    void fix_absNts(Model* model);
    //    void fix_absNts(Model* model, MtxLP* lp);
    void set_new_col(Model* model, string col, string row, double coef, double lb=LB, double ub=UB, int kind=CONT);//gni/
    RC set_korc(MtxLP* lp, string name, bool c);//gni
    //    void ko_blp(MtxLP* blp, stomap *obj, strvec rkos, kobds &ko_rc, stomap &ko_obj);//gni/
    void ko_blp(MtxLP* blp, stomap *obj, strvec rkos, stomap &ko_obj);//gni/
    void ko_mlp(MtxLP* blp, strvec rkos, kobds &ko_rc);//gni/
    //    void rko_blp(MtxLP* blp, stomap *obj, kobds &ko_rc, stomap &ko_obj);//gni/
    void rko_blp(MtxLP* blp, stomap *obj, stomap &ko_obj);//gni/
    void rko_mlp(MtxLP* mlp, kobds &ko_rc);//gni/

    void set_blp_model(Model* model, stomap *obj, string biom);//gni/
    void set_blp_model_dual2();//gni/
    //    stomap rand_med();//gni/
    void get_meds(int ns, int k, randmeds &rmeds, intvec &rmed);//gni/
    randmeds rand_meds();//gni/

};

#endif	/* GNI_H */

