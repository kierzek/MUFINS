#include "GNI.h"
#include "Driver.h"


strvec GNI::genes;
gene_reactions GNI::geneR;

GNI::GNI(){
    Nsam = 33000;//gni/
    //    Nsam = 3000;//gni/
    bm="BIOMASS";
}
GNI::~GNI(){}


//gni/ gene to its knockout Reactions map
void GNI::gene_koreacs(Model* model){
    strvec reacs=model->get_reactions();
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=model->get_reac(Rn);
        strvec rules=reac->get_rules();
        if(!rules.empty()){ //if OR in Rule,then donot map all genes for this reac
            koGenes(rules, 0, rules.size(), Rn);
        }
    }
}

void GNI::koGenes(strvec &rules, int k1, int k2, string Rn){
    for(int i=k1;i<k2;i++){
        string rs=rules[i];
        if(rs=="("){
            int p=1;
            for(int j=i+1;j<k2;j++){
                if(rules[j]=="(") p++;
                else if(rules[j]==")"){
                    p--;
                    if(p!=0) p--;
                    else if(p==0) {
                        if(i>0?rules[i-1]==AND:1 && j+1<k2?rules[j+1]==AND:1)
                            koGenes(rules, i+1,j, Rn);
                        i=j+1;
                        break;
                    }
                }
            }
        }
        else if(rs!=AND && rs!=OR){
            if(i>k1?rules[i-1]==AND:1 && i+1<k2?rules[i+1]==AND:1){
                if(geneR.find(rs)==geneR.end()) genes.push_back(rs);
                if(find(geneR[rs].begin(),geneR[rs].end(),Rn)==geneR[rs].end()) //avoid repeated genes for a reaction
                    geneR[rs].push_back(Rn);
            }
        }
    }
}



//gni/ Reaction to nutrients map
//void GNI::reac_nts(){
//    for(int i=0;i<rnts.size();i++){
//        string Rn=rnts[i];
//        Reaction* reac=model->get_reac(Rn);
//        stomap* sto=reac->get_sto();
//        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
//            string met=it->first;
//            if(met.find("_xt")==string::npos) {
//                reacN[Rn]=met;
//                nts.push_back(met);
//            }
//        }
//    }
//}


//gni/ get biomass reaction and a set of nutrients of interest to test for GNI problem
void GNI::set_nts(string line, strvec externals, Model* model){
    if (line.empty()) {
        throw runtime_error("please specify the metabolites to test!");
        //        for(int i=0;i<externals.size();i++){
        //            if(externals[i].find("BIOMASS")==string::npos) nts.push_back(externals[i]);
        //        }
    }
    else{
        nts.clear();
        split(&line,&nts,SPACE);
        for(int i=0;i<nts.size();i++){
            string nt=nts[i];
            if(xtR.find(nt)==xtR.end()) throw runtime_error(nt+" : "+"invalid nutrient name!");
            else{
                stomap sto=xtR[nt];
                for(stomap::iterator it=sto.begin();it!=sto.end();it++){
                    string Rn=it->first;
                    //                    double coef=it->second;
                    //                    if(coef<0){//for forware exchange reaction
                    rnts.push_back(Rn);
                    reacN[Rn].push_back(nt);
                    //                    }else if(coef>0 && lb<0){//for reverse exchange reaction
                    //                        rnts.push_back(Rn);
                    //                        reacN[Rn].push_back(nt);
                    //                    }else {
                    ////                        nts.erase(nts.begin()+i);
                    //                        throw runtime_error("warning: nutrient "+nt+" cannot be uptaken!");
                    ////                        cout<<"warning: nutrient "+nt+" cannot be uptaken!";
                    //                    }
                }
            }
        }
    }
}

//gni/ get a set of nutrients which will be set present
void GNI::set_ntsp(string line){
    ntsp.clear();
    split(&line,&ntsp,SPACE);
}

//gni/ get a set of nutrients which will be set abesent
void GNI::set_ntsa(strvec externals){
    for(int i=0;i<externals.size();i++){//get absent nts excepting present nts from externals
        string nt=externals[i];
        //        if(nt.find(bm)!=string::npos) continue;
        if(find(ntsp.begin(),ntsp.end(),nt)==ntsp.end() && find(nts.begin(),nts.end(),nt)==nts.end()) ntsa.push_back(nt);
    }
}

////gni/ Nutrient to Reactions map
//void GNI::map_nt_reac(Model* model){
//    for(int i=0;i<nts.size();i++){
//        string nt=nts[i];
//        Metabolite* met=model->get_met(nt);
//        stomap* sto=met->get_sto();
//        ntR[nt]=*sto;
//
//        for(stomap::iterator it=ntR[nt].begin();it!=ntR[nt].end();it++){//t/
//            cout<<it->first<<':'<<it->second<<endl;
//        }
//    }
//}

//gni/ All external nutrients to Reactions map and set rnts and reacN
void GNI::map_xt_sto(Model* model, strvec externals){
    for(int i=0;i<externals.size();i++){
        string nt=externals[i];
        //        if(nt.find(bm)!=string::npos) continue;
        Metabolite* met=model->get_met(nt);
        stomap* sto=met->get_sto();
        xtR[nt]=*sto;
    }
}

//void GNI::set_nt_abs(string nt, Model* model, MtxLP* mlp, bool abs){//?
//    stomap sto=xtR[nt];
//    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
//        string Rn=it->first;
//        double coef=it->second;
//        if(mlp->find_col(Rn.data())!=0){//check if Rn deleted
//            Reaction* reac=model->get_reac(Rn);
//            double ub=reac->get_ub();
//            double lb=reac->get_lb();
//            if(abs && coef<0) mlp->setColBnds(Rn, lb<0?lb:0, ub>0?0:ub);
//            else if(abs && coef>0) mlp->setColBnds(Rn, lb<0?0:lb, ub>0?ub:0);
//            else mlp->setColBnds(Rn, lb,ub);
//        }
//    }
//}

void GNI::set_nt_abs(string nt, Model* model, MtxLP* mlp, bool abs){
    stomap sto=xtR[nt];
    reacbds Rbd;
    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
        string Rn=it->first;
        double coef=it->second;
        double lb, ub;
        Reaction* reac=model->get_reac(Rn);
        if(Rbd.find(Rn)==Rbd.end()){//multi nutrients in one reac
            ub=reac->get_ub();
            lb=reac->get_lb();
        }else{
            lb=Rbd[Rn].lb;
            ub=Rbd[Rn].ub;
        }
        if(abs && coef<0) {//allow secrete for nt at left hand of reac
            lb=lb<0?lb:0;
            ub=ub>0?0:ub;
            mlp->setColBnds(Rn, lb,ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
        else if(abs && coef>0){//allow secrete for nt at right hand of reac
            lb=lb<0?0:lb;
            ub=ub>0?ub:0;
            mlp->setColBnds(Rn, lb,ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
        else {
            mlp->setColBnds(Rn, lb,ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
    }
}


void GNI::set_nt_abs(string nt, Model* model, bool abs){
    stomap sto=xtR[nt];
    reacbds Rbd;
    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
        string Rn=it->first;
        double coef=it->second;
        double lb, ub;
        Reaction* reac=model->get_reac(Rn);
        if(Rbd.find(Rn)==Rbd.end()){//multi nutrients in one reac
            ub=reac->get_ub();
            lb=reac->get_lb();
        }else{
            lb=Rbd[Rn].lb;
            ub=Rbd[Rn].ub;
        }
        if(abs && coef<0) {//allow secrete for nt at left hand of reac
            lb=lb<0?lb:0;
            ub=ub>0?0:ub;
            reac->set_bounds(lb, ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
        else if(abs && coef>0){//allow secrete for nt at right hand of reac
            lb=lb<0?0:lb;
            ub=ub>0?ub:0;
            reac->set_bounds(lb, ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
        else {
            reac->set_bounds(lb, ub);
            Rbd[Rn].lb=lb;
            Rbd[Rn].ub=ub;
        }
    }
}

//void GNI::set_nt_abs(string nt, Model* model, bool abs){
//    stomap sto=xtR[nt];
//    reacbds Rbd;
//    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
//        string Rn=it->first;
//        if(model->find(Rn)!=model->end()){
//            double coef=it->second;
//            double lb, ub;
//            Reaction* reac=model->get_reac(Rn);
//            if(Rbd.find(Rn)==Rbd.end()){//multi nutrients in one reac
//                ub=reac->get_ub();
//                lb=reac->get_lb();
//            }else{
//                lb=Rbd[Rn].lb;
//                ub=Rbd[Rn].ub;
//            }
//            if(abs && coef<0) {//allow secrete for nt at left hand of reac
//                lb=lb<0?lb:0;
//                ub=ub>0?0:ub;
//                if(lb==0 & ub==0){
//                    model->del_element(Rn);
//                    model->erase(Rn);
//                    xtR[nt].erase(Rn);
//                }else{
//                    reac->set_bounds(lb, ub);
//                    Rbd[Rn].lb=lb;
//                    Rbd[Rn].ub=ub;
//                }
//            }
//            else if(abs && coef>0){//allow secrete for nt at right hand of reac
//                lb=lb<0?0:lb;
//                ub=ub>0?ub:0;
//                if(lb==0 & ub==0){
//                    model->del_element(Rn);
//                    model->erase(Rn);
//                    xtR[nt].erase(Rn);
//                }else{
//                    reac->set_bounds(lb, ub);
//                    Rbd[Rn].lb=lb;
//                    Rbd[Rn].ub=ub;
//                }
//            }
//            else {
//                if(lb==0 & ub==0){
//                    model->del_element(Rn);
//                    model->erase(Rn);
//                    xtR[nt].erase(Rn);
//                }else{
//                    reac->set_bounds(lb, ub);
//                    Rbd[Rn].lb=lb;
//                    Rbd[Rn].ub=ub;
//                }
//            }
//        }
//    }
//}

//void GNI::set_nt_abs_grb(string nt, Model* model, GRB_Mod *mod, bool abs){
//    stomap sto=xtR[nt];
//    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
//        string Rn=it->first;
//        double coef=it->second;
//        if(mod->hasVar(Rn)){//check if Rn deleted
//            Reaction* reac=model->get_reac(Rn);
//            double ub=reac->get_ub();
//            double lb=reac->get_lb();
//            if(abs && coef<0) mod->setVarBnds(Rn, lb<0?lb:0, ub>0?0:ub);//allow secrete for nt at left hand of reac
//            else if(abs && coef>0) mod->setVarBnds(Rn, lb<0?0:lb, ub>0?ub:0);//allow secrete for nt at right hand of reac
//            else mod->setVarBnds(Rn, lb, ub);
//        }
//    }
//}

void GNI::set_nt_abs_grb(string nt, Model* model, GRB_Mod *mod, bool abs){
    stomap sto=xtR[nt];
    reacbds Rbd;
    for(stomap::iterator it=sto.begin();it!=sto.end();it++){
        string Rn=it->first;
        double coef=it->second;
        double lb, ub;
        if(mod->hasVar(Rn)){//check if Rn deleted
            if(Rbd.find(Rn)==Rbd.end()){//multi nutrients in one reac
                Reaction* reac=model->get_reac(Rn);
                ub=reac->get_ub();
                lb=reac->get_lb();
            }else{
                lb=Rbd[Rn].lb;
                ub=Rbd[Rn].ub;
            }
            if(abs && coef<0) {//allow secrete for nt at left hand of reac
                lb=lb<0?lb:0;
                ub=ub>0?0:ub;
                mod->setVarBnds(Rn, lb, ub);
                Rbd[Rn].lb=lb;
                Rbd[Rn].ub=ub;
            }
            else if(abs && coef>0){//allow secrete for nt at right hand of reac
                lb=lb<0?0:lb;
                ub=ub>0?ub:0;
                mod->setVarBnds(Rn, lb, ub);
                Rbd[Rn].lb=lb;
                Rbd[Rn].ub=ub;
            }
            else {
                mod->setVarBnds(Rn, lb, ub);
                Rbd[Rn].lb=lb;
                Rbd[Rn].ub=ub;
            }
        }
    }
}


void GNI::fix_nts_grb(strvec externals, Model* model, GRB_Mod *grbmod){//absent or present, all allow secreting
    for(int i=0;i<externals.size();i++){
        string nt=externals[i];
        //        if(nt.find(bm)!=string::npos) continue;
        bool abs=(find(nts.begin(),nts.end(),nt)==nts.end())?0:1;
        set_nt_abs_grb(nt, model, grbmod, abs);
    }
}


void GNI::fix_absNts_grb(strvec externals, Model* model, GRB_Mod *grbmod){ //set all external nutrients to be absent
    for(int i=0;i<externals.size();i++){
        string nt=externals[i];
        set_nt_abs_grb(nt, model, grbmod);
    }
}


void GNI::fix_absNts_grb(Model* model, GRB_Mod *grbmod){ //set absent nutrients to be absent
    for(int i=0;i<ntsa.size();i++){
        string nt=ntsa[i];
        set_nt_abs_grb(nt, model, grbmod);
    }
}

//void GNI::fix_absNts(Model* model, MtxLP* lp){ //set all external nutrients to be absent
//     for(int i=0;i<ntsa.size();i++){
//        string nt=ntsa[i];
//        set_nt_abs(nt, model, lp);
//    }
//}

void GNI::fix_absNts(Model* model){ //set all external nutrients to be absent
    for(int i=0;i<ntsa.size();i++){
        string nt=ntsa[i];
        set_nt_abs(nt, model);
    }
}

////gni/ set a new row for model
//void GNI::set_new_col(string col, string row, double coef){
//    model->NewReaction(col);
//    Reaction* c1=model->get_reac(col);
//    stomap *sto_c1 = c1->get_sto();
//    (*sto_c1)[row]=coef;
//}

//gni/ set a new col for model
void GNI::set_new_col(Model* model, string col, string row, double coef, double lb, double ub, int kind){
    if(model->find(col)==model->end()) {//if col is new one
        model->NewReaction(col);
        Reaction* c1=model->get_reac(col);
        if(kind==CONT) c1->set_bounds(lb, ub);
        else c1->set_kind(kind);
        stomap *sto_c1 = c1->get_sto();
        (*sto_c1)[row]=coef;
    }else{
        Reaction* c1=model->get_reac(col);
        stomap *sto_c1 = c1->get_sto();
        (*sto_c1)[row]=coef;
    }

}

RC GNI::set_korc(MtxLP* lp, string name, bool c){
    RC rc;
    if(c==1){
        rc.col=1;
        rc.lb=lp->getColLB(name);
        rc.ub=lp->getColUB(name);
        rc.sto=lp->getColSto(name);
    }else if(c==0){
        rc.col=0;
        rc.lb=lp->getRowLB(name);
        rc.ub=lp->getRowUB(name);
        rc.sto=lp->getRowSto(name);
    }
    return rc;
}

//void GNI::ko_blp(MtxLP* blp, stomap *obj, strvec rkos, kobds &ko_rc, stomap &ko_obj){//gni/knockout blp model
////    strvec Rt;
//     for(int i=0;i<rkos.size();i++){//assume non-nts reactions for ko genes
//         string Rn=rkos[i];
////         if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
////             Rt.push_back(Rn);
//             string ro="ro__"+Rn;
//             ko_rc[ro]=set_korc(blp, ro, 0);
//             if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if not in nutrient reactions
//                string cx="cx__"+Rn;
//                string cm="cm__"+Rn;
//                ko_rc[cx]=set_korc(blp, cx, 1);
//                ko_rc[cm]=set_korc(blp, cm, 1);
//                ko_obj[cx]=(*obj)[cx];
//                ko_obj[cm]=(*obj)[cm];
//                blp->delRow(ro);
//                blp->delCol(cx);
//                blp->delCol(cm);
//                obj->erase(cx);
//                obj->erase(cm);
//             }else{
//                string ct="ct__"+Rn;
//                string ch="ch__"+Rn;
//                string rh1="rh1__"+Rn;
//                string rh2="rh2__"+Rn;
//                string rh3="rh3__"+Rn;
//                ko_rc[ct]=set_korc(blp, ct, 1);
//                ko_rc[ch]=set_korc(blp, ch, 1);
//                ko_rc[rh1]=set_korc(blp, rh1, 0);
//                ko_rc[rh2]=set_korc(blp, rh2, 0);
//                ko_rc[rh3]=set_korc(blp, rh3, 0);
//                ko_obj[ch]=(*obj)[ch];
//                blp->delRow(ro);
//                blp->delCol(ct);
//                blp->delCol(ch);
//                blp->delRow(rh1);
//                blp->delRow(rh2);
//                blp->delRow(rh3);
//                obj->erase(ch);
//             }
////         }
//    }
//}

void GNI::ko_blp(MtxLP* blp, stomap *obj, strvec rkos, stomap &ko_obj){//gni/knockout blp model
    //    strvec Rt;
    for(int i=0;i<rkos.size();i++){//assume non-nts reactions for ko genes
        string Rn=rkos[i];
        //         if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
        //             Rt.push_back(Rn);
        //            string ro="ro__"+Rn;
        string cx="cx__"+Rn;
        string cm="cm__"+Rn;
        ko_obj[cx]=(*obj)[cx];
        ko_obj[cm]=(*obj)[cm];
        obj->erase(cx);
        obj->erase(cm);
        if(reacN.find(Rn)!=reacN.end()){//if in nutrient reactions
            strvec nts=reacN[Rn];
            for(int i=0;i<nts.size();i++){//for multi nutrients in a reaction
                string nt=nts[i];
                string ch="ch__"+Rn+nt;
                ko_obj[ch]=(*obj)[ch];
                obj->erase(ch);
            }
        }
        //         }
    }
}

//void GNI::rko_blp(MtxLP* blp, stomap *obj, kobds &ko_rc, stomap &ko_obj){//gni/
//    for(kobds::iterator it=ko_rc.begin();it!=ko_rc.end();it++){
//        string name=it->first;
//        RC rc=it->second;
//        blp->addRC(name, rc);
//    }
//    for(stomap::iterator it=ko_obj.begin();it!=ko_obj.end();it++){
//        (*obj)[it->first]=it->second;
//    }
//}

void GNI::rko_blp(MtxLP* blp, stomap *obj, stomap &ko_obj){//gni/
    //    for(kobds::iterator it=ko_rc.begin();it!=ko_rc.end();it++){
    //        string name=it->first;
    //        RC rc=it->second;
    //        blp->addRC(name, rc);
    //    }
    for(stomap::iterator it=ko_obj.begin();it!=ko_obj.end();it++){
        (*obj)[it->first]=it->second;
    }
}

void GNI::ko_mlp(MtxLP* lp, strvec rkos, kobds &ko_rc){//gni/knockout mlp model
    //    strvec Rt;
    for(int i=0;i<rkos.size();i++){
        string Rn=rkos[i];
        //        if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
        //            Rt.push_back(Rn);
        ko_rc[Rn]=set_korc(lp, Rn, 1);
        lp->delCol(Rn);
        //        }
    }
}

void GNI::rko_mlp(MtxLP* lp, kobds &ko_rc){//gni/
    for(kobds::iterator it=ko_rc.begin();it!=ko_rc.end();it++){
        string name=it->first;
        RC rc=it->second;
        lp->addRC(name, rc);
    }
}


//gni/ set bilevel optimazation model for finding a growth medium
//void GNI::set_blp_model(Model* model, stomap *obj, string biom){//gni/
//    bm=biom;
//    (*obj)[biom]=1;
//    strvec reacs=model->get_reactions();
//    if(find(reacs.begin(),reacs.end(),biom)==reacs.end()) throw runtime_error(biom+" : "+"invalid biomass reaction!");
//    for(int i=0;i<reacs.size();i++){
//        string Rn=reacs[i];
//        Reaction* reac=model->get_reac(Rn);
//        stomap *sto=reac->get_sto();
//        double lb=reac->get_lb();
//        double ub=reac->get_ub();
//        string row="ro__"+Rn; //new row name for reac flux
//        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
//            string met=it->first; //metabolite
//            double coef=it->second; //coefficient of constraint matrix for this met and reac
//            string colz="cz__"+met; //new col for metabolite row constraint
//            set_new_col(model, colz,row,coef);
//
//        }
//        if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if reac is not in nutrient reacs
//            string colx="cx__"+Rn; //new col for <=w_max constraint
//            string colm="cm__"+Rn; //new col for <=-w_min constraint
//            set_new_col(model, colx,row,1,0,UB);
//            set_new_col(model, colm,row,-1,0,UB);
//            if(ub!=0) (*obj)[colx]=-ub;
//            if(lb!=0) (*obj)[colm]=lb;
//        }
//        else{
//            string colm="cm__"+Rn; //new col for <=-w_min constraint
//            set_new_col(model, colm,row,-1,0,UB);
//            if(lb!=0) (*obj)[colm]=lb;
//            string colt="ct__"+Rn; //new col for <=e constraint of nts
//            string colh="ch__"+Rn;
//            string cole="ce__"+reacN[Rn];
////            string cole="ce__"+Rn;
//            string rowe="re__"+Rn;
//            string rowh1="rh1__"+Rn;
//            string rowh2="rh2__"+Rn;
//            string rowh3="rh3__"+Rn;
//            set_new_col(model, colt,row,1,0,UB);//row
//            set_new_col(model, Rn,rowe,1,lb,ub);//rowe
//            set_new_col(model, cole,rowe,-ub,0,0,INT);
////            set_new_col(cole,rowe,-1,0,0,INT);
//            set_new_col(model, colh,rowh1,1,0,UB); //rowh1
//            set_new_col(model, cole,rowh1,-CTUB,0,0,INT);
//            set_new_col(model, colt,rowh2,1,0,UB);//rowh2
//            set_new_col(model, colh,rowh2,-1,0,UB);
//            set_new_col(model, cole,rowh2,CTUB,0,0,INT);
//            set_new_col(model, colt,rowh3,1,0,UB);//rowh3
//            set_new_col(model, colh,rowh3,-1,0,UB);
//            (*obj)[colh]=-ub;
//
////            set_new_col(colh,rowh1,1,0,UB); //rowh1
////            set_new_col(cole,rowh1,-CTUB*ub,0,0,INT);
////            set_new_col(colt,rowh2,ub,0,UB);//rowh2
////            set_new_col(colh,rowh2,-1,0,UB);
////            set_new_col(cole,rowh2,CTUB,0,0,INT);
////            set_new_col(colt,rowh3,ub,0,UB);//rowh3
////            set_new_col(colh,rowh3,-1,0,UB);
////            (*obj)[colh]=-1;
//        }
//    }
//}


//gni/ set bilevel optimazation model for finding a growth medium
void GNI::set_blp_model(Model* model, stomap *obj, string biom){//gni/
    bm=biom;
    (*obj)[biom]=1;
    strvec reacs=model->get_reactions();
    if(find(reacs.begin(),reacs.end(),biom)==reacs.end()) throw runtime_error(biom+" : "+"invalid biomass reaction!");
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=model->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        string row="ro__"+Rn; //new row name for reac flux
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string met=it->first; //metabolite
            double coef=it->second; //coefficient of constraint matrix for this met and reac
            string colz="cz__"+met; //new col for metabolite row constraint
            set_new_col(model, colz,row,coef);

        }
        //        if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if reac is not in nutrient reacs
        string colx="cx__"+Rn; //new col for <=w_max constraint
        string colm="cm__"+Rn; //new col for <=-w_min constraint
        set_new_col(model, colx,row,1,0,UB);
        set_new_col(model, colm,row,-1,0,UB);
        if(ub!=0) (*obj)[colx]=-ub;
        if(lb!=0) (*obj)[colm]=lb;
        //        }
        //        else{
        //            string colm="cm__"+Rn; //new col for <=-w_min constraint
        //            set_new_col(model, colm,row,-1,0,UB);
        //            if(lb!=0) (*obj)[colm]=lb;
        //            string colt="ct__"+Rn; //new col for <=e constraint of nts
        //            string colh="ch__"+Rn;
        //            string rowe="re__"+Rn;
        //            string rowh1="rh1__"+Rn;
        //            string rowh2="rh2__"+Rn;
        //            string rowh3="rh3__"+Rn;
        if(find(rnts.begin(),rnts.end(),Rn)!=rnts.end()){//if reac is in nutrient reacs
            strvec nts=reacN[Rn];
            for(int i=0;i<nts.size();i++){//for multi nutrients in a reaction
                string nt=nts[i];
                string colt="ct__"+Rn+nt; //new col for <=e constraint of nts
                string colh="ch__"+Rn+nt;
                string rowe="re__"+Rn+nt;
                string rowh1="rh1__"+Rn+nt;
                string rowh2="rh2__"+Rn+nt;
                string rowh3="rh3__"+Rn+nt;
                string cole="ce__"+nt;
                stomap ntsto=xtR[nt];
                if(ntsto[Rn]<0){//nt at left of exchange reaction
                    set_new_col(model, colt,row,1,0,UB);//row
                    set_new_col(model, Rn,rowe,1,lb,ub);//rowe  
                    set_new_col(model, cole,rowe,-ub,0,0,INT);//rowe
                    (*obj)[colh]=-ub;

                }else if(ntsto[Rn]>0){//nt at right of exchange reaction
                    set_new_col(model, colt,row,-1,0,UB);//row
                    set_new_col(model, Rn,rowe,-1,lb,ub);//rowe  
                    set_new_col(model, cole,rowe,lb,0,0,INT);//rowe
                    (*obj)[colh]=lb;
                }
                set_new_col(model, cole,rowh1,-CTUB,0,0,INT);//rowh1
                set_new_col(model, colh,rowh1,1,0,UB); 
                set_new_col(model, cole,rowh2,CTUB,0,0,INT);//rowh2                         
                set_new_col(model, colt,rowh2,1,0,UB);//rowh2
                set_new_col(model, colh,rowh2,-1,0,UB);
                set_new_col(model, colt,rowh3,1,0,UB);//rowh3
                set_new_col(model, colh,rowh3,-1,0,UB);
            }          

            //            set_new_col(cole,rowe,-1,0,0,INT);

            //            (*obj)[colh]=-ub;

            //            set_new_col(colh,rowh1,1,0,UB); //rowh1
            //            set_new_col(cole,rowh1,-CTUB*ub,0,0,INT);
            //            set_new_col(colt,rowh2,ub,0,UB);//rowh2
            //            set_new_col(colh,rowh2,-1,0,UB);
            //            set_new_col(cole,rowh2,CTUB,0,0,INT);
            //            set_new_col(colt,rowh3,ub,0,UB);//rowh3
            //            set_new_col(colh,rowh3,-1,0,UB);
            //            (*obj)[colh]=-1;
        }
        //        }//else{
    }
}

//stomap GNI::rand_med(){//gni/ generate a rand medium of ce vaule
//    stomap medr;
//    for(int i=0;i<rnts.size();i++){
//        string Rn=rnts[i];
//        medr[Rn]=rand() % 2;
//    }
//    return medr;
//}


void GNI::get_meds(int ns, int k, randmeds &rmeds, intvec &rmed){//gni/
    ns--;
    k++;
    for(int i=0;i<2;i++){
        rmed[k-1]=i;
        if(ns>0) {
            get_meds(ns,k,rmeds,rmed);
        }
        else rmeds.push_back(rmed);
    }
}

randmeds GNI::rand_meds(){//gni/ generate a rand medium of ce vaule
    int ns=nts.size();
    randmeds rmeds;
    intvec rmed;
    if(ns<=15){//if number of nts less than 15, then get all media
        for(int i=0;i<ns;i++) rmed.push_back(0);
        get_meds(ns, 0, rmeds, rmed);
    }
    else{//sampling with replacement
        for(int s=0;s<Nsam;s++){
            for(int i=0;i<ns;i++){
                rmed.push_back(rand() % 2);
            }
            rmeds.push_back(rmed);
            rmed.clear();
        }
    }
    return rmeds;
}
