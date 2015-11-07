#include "GFA.h"

GFA::GFA(){
//   eps=1e-7;
   eps=1;
}
GFA::~GFA(){}


//void GFA::set_gfa_eps(double gfa_eps){
//    eps=gfa_eps!=NULL?gfa_eps:1;
//}


void GFA::read_expression(string expfile, string err_msg){ //w/ read gene/enzyme expression file
//parsing the input
    if (!fexists(expfile))
        throw runtime_error(err_msg + "wrong expression file name!");
    parse_arrayExpression(expfile, arrName, arrexp);
}

void GFA::read_expression_f(string expfile, string err_msg){ //w/ read gene/enzyme expression file
//parsing the input
    if (!fexists(expfile))
        throw runtime_error(err_msg + "wrong expression file name!");
    parse_arrayExpression_f(expfile, arrName, arrexp_f);
}

void GFA::read_expression_met(string metfile, string err_msg){ //w/ read gene/enzyme expression file
//parsing the input
    if (!fexists(metfile))
        throw runtime_error(err_msg + "wrong expression file name!");
    parse_arrayExpression(metfile, arrName, metexp);
}

//w/ get rule from reaction, set state of reaction by boolean logic, set geneReaction
void GFA::set_gfa_model(string arr, Model* tmod, stomap *obj, flomap* Rstate, strvec &externals){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state(reac, arr, Rstate);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        if(lb==0 && ub==0){
			tmod->del_element(Rn);
			for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){
				string name=it->first;
				strvec::iterator it_ext=std::find(externals.begin(),externals.end(),name);
				if(it_ext!=externals.end()) externals.erase(it_ext);
			}
		}
        else{
            if(state==1){ //highly expressed reaction
                stomap *sto=reac->get_sto();
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                string row1=Rn+"__row1";
                string row2=Rn+"__row2";
                string col1=Rn+"__col1";
                string col2=Rn+"__col2";
                if(ub>=eps){ //only flux>eps, it make sense for gfa
                    (*sto)[row1]=1;
                    tmod->NewReaction(col1);//y+
                    Reaction *c1=tmod->get_reac(col1);
                    c1->set_kind(INT);
                    stomap *sto_c1 = c1->get_sto();
                    (*sto_c1)[row1]=lb-eps;
                    c1->set_bounds(lb, ub); //set the same bounds as the reaction
                    (*obj)[col1]=1;
                }
                if(lb<=-eps){
                    (*sto)[row2]=1;
                    tmod->NewReaction(col2);//y-
                    Reaction *c2=tmod->get_reac(col2);
                    c2->set_kind(INT);
                    stomap *sto_c2 = c2->get_sto();
                    (*sto_c2)[row2]=ub+eps;
                    c2->set_bounds(lb, ub);
                    (*obj)[col2]=1;
                }
            }
            else if(state==-1){ //lowly expressed reaction
                stomap *sto=reac->get_sto();
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                string row1=Rn+"__row1";
                string row2=Rn+"__row2";
                string col1=Rn+"__col1";
                tmod->NewReaction(col1);//y+
                Reaction *c1=tmod->get_reac(col1);
                c1->set_kind(INT);
                c1->set_bounds(lb, ub); //set the same bounds as the reaction
    //            c1->set_bounds(0, 1); //
                stomap *sto_c1 = c1->get_sto();
    //            if(lb<=0){
                    (*sto)[row1]=1;
                    (*sto_c1)[row1]=lb;
    //                (*sto_c1)[row1]=lb+eps;
    //            	(*obj)[col1]=1;
    //            }
    //            if(ub>=0){
                    (*sto)[row2]=1;
                    (*sto_c1)[row2]=ub;
    //                (*sto_c1)[row2]=ub-eps;
                    (*obj)[col1]=1;
    //            }
            }
        }
    }
}

////w/ get rule from reaction, set state of reaction by boolean logic, set geneReaction
//void GFA::set_gfa_model(string arr, Model* tmod, stomap *obj, flomap* Rstate, strvec &externals){
//    double state;
//    strvec reacs=tmod->get_reactions();
//    for(int i=0;i<reacs.size();i++){
//    	string Rn=reacs[i];
//        Reaction* reac=tmod->get_reac(Rn);
//        state=eval_reaction_state(reac, arr, Rstate);
//        stomap *sto=reac->get_sto();
//        double lb=reac->get_lb();
//        double ub=reac->get_ub();
//		if(state==1){ //highly expressed reaction
//			stomap *sto=reac->get_sto();
//			double lb=reac->get_lb();
//			double ub=reac->get_ub();
//			string row1=Rn+"__row1";
//			string row2=Rn+"__row2";
//			string col1=Rn+"__col1";
//			string col2=Rn+"__col2";
//			if(ub>=eps){ //only flux>eps, it make sense for gfa
//				(*sto)[row1]=1;
//				tmod->NewReaction(col1);//y+
//				Reaction *c1=tmod->get_reac(col1);
//				c1->set_kind(INT);
//				stomap *sto_c1 = c1->get_sto();
//				(*sto_c1)[row1]=lb-eps;
//				c1->set_bounds(lb, ub); //set the same bounds as the reaction
//				(*obj)[col1]=1;
//			}
//			if(lb<=-eps){
//				(*sto)[row2]=1;
//				tmod->NewReaction(col2);//y-
//				Reaction *c2=tmod->get_reac(col2);
//				c2->set_kind(INT);
//				stomap *sto_c2 = c2->get_sto();
//				(*sto_c2)[row2]=ub+eps;
//				c2->set_bounds(lb, ub);
//				(*obj)[col2]=1;
//			}
//		}
//		else if(state==-1){ //lowly expressed reaction
//			stomap *sto=reac->get_sto();
//			double lb=reac->get_lb();
//			double ub=reac->get_ub();
//			string row1=Rn+"__row1";
//			string row2=Rn+"__row2";
//			string col1=Rn+"__col1";
//			tmod->NewReaction(col1);//y+
//			Reaction *c1=tmod->get_reac(col1);
//			c1->set_kind(INT);
//			c1->set_bounds(lb, ub); //set the same bounds as the reaction
////            c1->set_bounds(0, 1); //
//			stomap *sto_c1 = c1->get_sto();
////            if(lb<=0){
//				(*sto)[row1]=1;
//				(*sto_c1)[row1]=lb;
////                (*sto_c1)[row1]=lb+eps;
////            	(*obj)[col1]=1;
////            }
////            if(ub>=0){
//				(*sto)[row2]=1;
//				(*sto_c1)[row2]=ub;
////                (*sto_c1)[row2]=ub-eps;
//				(*obj)[col1]=1;
////            }
//		}
//    }
//}

//w/ get rule from reaction, set state of reaction by boolean logic, set geneReaction
void GFA::set_gfa_model_gfva(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state(reac, arr, Rstate);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        if(lb==0 && ub==0) tmod->del_element(Rn);
//        if(lb!=0 && ub!=0){
        else{
            if(state==1){ //highly expressed reaction
                stomap *sto=reac->get_sto();
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                string row1=Rn+"__row1";
                string row2=Rn+"__row2";
                string col1=Rn+"__col1";
                string col2=Rn+"__col2";
                if(ub>=eps){ //only flux>eps, it make sense for gfa
                    (*sto)[row1]=1;
                    tmod->NewReaction(col1);//y+
                    Reaction *c1=tmod->get_reac(col1);
                    c1->set_kind(INT);
                    stomap *sto_c1 = c1->get_sto();
                    (*sto_c1)[row1]=lb-eps;
                    c1->set_bounds(lb, ub); //set the same bounds as the reaction
                    (*obj)[col1]=1;
                }
                if(lb<=-eps){
                    (*sto)[row2]=1;
                    tmod->NewReaction(col2);//y-
                    Reaction *c2=tmod->get_reac(col2);
                    c2->set_kind(INT);
                    stomap *sto_c2 = c2->get_sto();
                    (*sto_c2)[row2]=ub+eps;
                    c2->set_bounds(lb, ub);
                    (*obj)[col2]=1;
                }
            }
            else if(state==-1){ //lowly expressed reaction
                stomap *sto=reac->get_sto();
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                string row1=Rn+"__row1";
                string row2=Rn+"__row2";
                string col1=Rn+"__col1";
                tmod->NewReaction(col1);//y+
                Reaction *c1=tmod->get_reac(col1);
                c1->set_kind(INT);
                c1->set_bounds(lb, ub); //set the same bounds as the reaction
    //            c1->set_bounds(0, 1); //
                stomap *sto_c1 = c1->get_sto();
    //            if(lb<=0){
                    (*sto)[row1]=1;
//                    (*sto_c1)[row1]=lb;//eps=0
                    (*sto_c1)[row1]=lb+(eps-1e-7);
    //            	(*obj)[col1]=1;
    //            }
    //            if(ub>=0){
                    (*sto)[row2]=1;
//                    (*sto_c1)[row2]=ub;//eps=0
                    (*sto_c1)[row2]=ub-(eps-1e-7);
                    (*obj)[col1]=1;
    //            }
            }
        }
    }
}

//w/ fimat: get rule from reaction, set state of reaction by boolean logic, set geneReaction
void GFA::set_gfa_model_gfva2(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state_f(reac, arr, Rstate);
        if(state!=0){ //expressed reaction    
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            if(ub>0 && lb<0){ //split reversible reaction into two reactions
                stomap *sto=reac->get_sto();
                string Rev=Rn+"__r";
                string row1=Rn+"__r1";//forward reac
                string row2=Rn+"__r2";//reverse reac
                string row3=Rn+"__r3";//y1+y2=1
                string col1=Rn+"__c1";//binary indicator for selecting forward flux or reverse flux for a reversible reac
                string col2=Rn+"__c2";

                reac->set_bounds(0, ub);//set forward reaction bounds
                (*Rstate)[Rev]=state;
                tmod->NewReaction(Rev);//add reverse reaction with new column
                Reaction *reacR=tmod->get_reac(Rev);
                reacR->set_comment(Rn+":Reverse");
                stomap *stoR = reacR->get_sto();
                *stoR=*sto;
                for ( stomap::iterator it=stoR->begin(); it!= stoR->end(); it++ )
                    it->second=-(it->second);
                reacR->set_bounds(0, -lb);

                (*sto)[row1]=1;
                (*stoR)[row2]=1;

                tmod->NewReaction(col1);//y1
                Reaction *c1=tmod->get_reac(col1);
                c1->set_kind(INT);
                stomap *sto_c1 = c1->get_sto();
                (*sto_c1)[row1]=ub;
                (*sto_c1)[row3]=1;
                c1->set_bounds(lb, ub); //set the same bounds as the reaction
                tmod->NewReaction(col2);//y2
                Reaction *c2=tmod->get_reac(col2);
                c2->set_kind(INT);
                stomap *sto_c2 = c2->get_sto();
                (*sto_c2)[row2]=-lb;
                (*sto_c2)[row3]=1;
                c2->set_bounds(lb, ub); //set the same bounds as the reaction
                
                (*obj)[Rn]=state;
                (*obj)[Rev]=state;
            }
            else if(lb<0) (*obj)[Rn]=-state;
            else (*obj)[Rn]=state;
        }
    }
}

//w/ fimat: get rule from reaction, set state of reaction by boolean logic, set geneReaction
void GFA::set_gfa_model_fimat(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state_f(reac, arr, Rstate);
        if(state!=0){ //expressed reaction    
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            if(ub>0 && lb<0){ //split reversible reaction into two reactions
                stomap *sto=reac->get_sto();
                string Rnr=Rn+"__r";
                string row1=Rn+"__b1";//forward reac: Vf + ub*i <= ub
                string row2=Rn+"__b2";//reverse reac: Vr + lb*i <= 0
                string coli=Rn+"__i";//binary indicator for selecting forward flux or reverse flux for a reversible reac

                reac->set_bounds(0, ub);//set forward reaction bounds

                (*Rstate)[Rnr]=state;
                tmod->NewReaction(Rnr);//add reverse reaction with new column
                Reaction *reacR=tmod->get_reac(Rnr);
                reacR->set_comment(Rn+":Reverse");
                stomap *stoR = reacR->get_sto();
                *stoR=*sto;
                for ( stomap::iterator it=stoR->begin(); it!= stoR->end(); it++ )
                    it->second=-(it->second);
                reacR->set_bounds(0, -lb);

                (*sto)[row1]=1;
                (*stoR)[row2]=1;

                tmod->NewReaction(coli);//add binary column
                Reaction *ci=tmod->get_reac(coli);
                ci->set_kind(INT);
                stomap *stoI = ci->get_sto();
                (*stoI)[row1]=ub;
                (*stoI)[row2]=lb;
                ci->set_bounds(lb, ub); //set the same bounds as the reaction
                
                (*obj)[Rn]=state;
                (*obj)[Rnr]=state;
            }
            else if(lb<0) (*obj)[Rn]=-state;
            else (*obj)[Rn]=state;
        }
    }
}


//w/ limat: set objective function based on reaction state which is computed from gene expression and rules; split reversible reaction into two reactions
void GFA::set_gfa_model_gfva3(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state_f(reac, arr, Rstate);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        string Rev=Rn+"__r";
        if(ub>0 && lb<0){ //split reversible reaction into two forward reactions
            reac->set_bounds(0, ub);
            (*Rstate)[Rn]=state; //!
            (*Rstate)[Rev]=state;
            tmod->NewReaction(Rev);//add reverse reaction with new column
            Reaction *c2=tmod->get_reac(Rev);
            c2->set_comment(Rn+":Reverse");
            stomap *sto_c2 = c2->get_sto();
            *sto_c2=*sto;
            for ( stomap::iterator it=sto_c2->begin(); it!= sto_c2->end(); it++ )
                it->second=-(it->second);
            c2->set_bounds(0, -lb);
            if(state!=0){
                (*obj)[Rn]=state;
                (*obj)[Rev]=state;
            }
        }
        else{
            if(state!=0) {
                if(lb<0) (*obj)[Rn]=-state;
                else (*obj)[Rn]=state;
            }
        }

    }
}

//w/ set objective function based on reaction state which is computed from gene expression and rules; split reversible reaction into two reactions
void GFA::set_gfa_model_gimme(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    double diff; //eps-state
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state_f(reac, arr, Rstate);
	    diff=eps-state;
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        string Rev=Rn+"__r";
        if(ub>0 && lb<0){ //split reversible reaction into two forward reactions
            reac->set_bounds(0, ub);
            (*Rstate)[Rn]=state; //!
            (*Rstate)[Rev]=state;
            tmod->NewReaction(Rev);//add reverse reaction with new column
            Reaction *c2=tmod->get_reac(Rev);
            c2->set_comment(Rn+":Reverse");
            stomap *sto_c2 = c2->get_sto();
            *sto_c2=*sto;
            for ( stomap::iterator it=sto_c2->begin(); it!= sto_c2->end(); it++ )
                it->second=-(it->second);
            c2->set_bounds(0, -lb);
            if(diff>0 && state!=0){
                (*obj)[Rn]=diff;//for minimize sum(diff*vi)
                (*obj)[Rev]=diff;
            }
        }
        else{
            if(diff>0 && state!=0) {
                if(lb<0) (*obj)[Rn]=-diff;
                else (*obj)[Rn]=diff;
            }
        }

    }
}

//w/GIM3E: split reversible rxn; make all rxn in forward direction: left -> right
void GFA::set_gim3e_model_split(Model* tmod){
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        stomap *sto=reac->get_sto();
        if(ub>0 && lb<0){ //split reversible reaction into two reactions            
            string Rnr=Rn+"__r";
            string row1=Rn+"__b1";//forward reac: Vf + ub*i <= ub
            string row2=Rn+"__b2";//reverse reac: Vr + lb*i <= 0
            string coli=Rn+"__i";//binary indicator for selecting forward flux or reverse flux for a reversible reac

            reac->set_bounds(0, ub);//set forward reaction bounds

            tmod->NewReaction(Rnr);//add reverse reaction with new column
            Reaction *reacR=tmod->get_reac(Rnr);
            reacR->set_comment(Rn+":Reverse");
            stomap *stoR = reacR->get_sto();
            *stoR=*sto;
            for ( stomap::iterator it=stoR->begin(); it!= stoR->end(); it++ )
                it->second=-(it->second);
            reacR->set_bounds(0, -lb);

            (*sto)[row1]=1;
            (*stoR)[row2]=1;

            tmod->NewReaction(coli);//add binary column
            Reaction *ci=tmod->get_reac(coli);
            ci->set_kind(INT);
            stomap *stoI = ci->get_sto();
            (*stoI)[row1]=ub;
            (*stoI)[row2]=lb;
            ci->set_bounds(lb, ub); //set the same bounds as the reaction              
        }
        else if(lb<0){//make reverse direction to forward direction
            for ( stomap::iterator it=sto->begin(); it!= sto->end(); it++ )
                it->second=-(it->second);
            reac->set_bounds(0, -lb);
        }

    }
}

//w/GIM3E: add turnover met to each rxn and its turnover sink (TS) rxn
void GFA::set_gim3e_model_TS(Model* tmod, string mar, double tol){
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        for ( stomap::iterator it=sto->begin(); it!= sto->end(); it++ ){
            string met = it->first;     
            if(metexp.find(met)!=metexp.end()) (*sto)["t_"+met]=1;
        }
    }
    for ( strstr2int::iterator it=metexp.begin(); it!= metexp.end(); it++ ){
        string met = it->first; 
        int lev = metexp[met][mar];
        string TS = "TS_"+met;
        tmod->NewReaction(TS);
        Reaction *ts=tmod->get_reac(TS);
        ts->set_comment("Turnover sink: "+met);
        stomap *sto = ts->get_sto();
        (*sto)["t_"+met]=-2;
        if(lev == 1) ts->set_bounds(1.01*tol, UB);
        else ts->set_bounds(0, UB);
    }

}

//w/GIM3E: map gene's panelty to rxn state and set obj
void GFA::set_gim3e_model_gene(string arr, Model* tmod, stomap *obj, flomap* Rstate){
    double state;
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        if(Rn.find("__r")!=string::npos) continue;//no need to get split reverse rxn state which is same as its forward rxn
        Reaction* reac=tmod->get_reac(Rn);
        state=eval_reaction_state_f(reac, arr, Rstate);
        if(state!=0){ //expressed reaction
            (*obj)[Rn]=state;
            string Rnr=Rn+"__r"; //for split reverse rxn
            if(find(reacs.begin(),reacs.end(),Rnr)!=reacs.end())
                (*obj)[Rnr]=state;
            (*Rstate)[Rnr]=state;

        }
    }
}


//w/ get rule from reaction, set state of reaction by boolean logic, set geneReaction
//void GFA::set_gfa_model(string arr, Model* tmod, stomap *obj, flomap* Rstate){
//    double state;
//    strvec reacs=tmod->get_reactions();
//    for(int i=0;i<reacs.size();i++){
//    	string Rn=reacs[i];
//        Reaction* reac=tmod->get_reac(Rn);
//        state=eval_reaction_state(reac, arr, Rstate);
//        if(state==1){ //highly expressed reaction
//            stomap *sto=reac->get_sto();
//            double lb=reac->get_lb();
//            double ub=reac->get_ub();
//            string row1=Rn+"_r1";
//            string row2=Rn+"_r2";
//            string col1=Rn+"_c1";
//            string col2=Rn+"_c2";
//            if(ub>=eps){ //only flux>eps, it make sense for gfa
//            	(*sto)[row1]=1;
//            	tmod->NewReaction(col1);//y+
//                Reaction *c1=tmod->get_reac(col1);
//                c1->set_kind(INT);
//                stomap *sto_c1 = c1->get_sto();
//                (*sto_c1)[row1]=lb-eps;
//                c1->set_bounds(lb, ub); //set the same bounds as the reaction
//                (*obj)[col1]=1;
//            }
//            if(lb<=-eps){
//            	(*sto)[row2]=1;
//            	tmod->NewReaction(col2);//y-
//                Reaction *c2=tmod->get_reac(col2);
//                c2->set_kind(INT);
//                stomap *sto_c2 = c2->get_sto();
//                (*sto_c2)[row2]=ub+eps;
//                c2->set_bounds(lb, ub);
//                (*obj)[col2]=1;
//            }
//        }
//        else if(state==-1){ //lowly expressed reaction
//            stomap *sto=reac->get_sto();
//            double lb=reac->get_lb();
//            double ub=reac->get_ub();
//            string row1=Rn+"_r1";
//            string row2=Rn+"_r2";
//            string col1=Rn+"_c1";
//            tmod->NewReaction(col1);//y+
//            Reaction *c1=tmod->get_reac(col1);
//            c1->set_kind(INT);
//            c1->set_bounds(lb, ub); //set the same bounds as the reaction
////            c1->set_bounds(0, 1); //
//            stomap *sto_c1 = c1->get_sto();
////            if(lb<=0){
//            	(*sto)[row1]=1;
//            	(*sto_c1)[row1]=lb;
////                (*sto_c1)[row1]=lb+eps;
////            	(*obj)[col1]=1;
////            }
////            if(ub>=0){
//            	(*sto)[row2]=1;
//            	(*sto_c1)[row2]=ub;
////                (*sto_c1)[row2]=ub-eps;
//            	(*obj)[col1]=1;
////            }
//        }
//    }
//}



//w/ split the reversible reactions
void GFA::split_model_rev(Model* tmod){
    strvec reacs=tmod->get_reactions();
    for(int i=0;i<reacs.size();i++){
    	string Rn=reacs[i];
        Reaction* reac=tmod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        strvec rules=reac->get_rules();
        string Rev=Rn+"_r";
        if(ub>0 && lb<0){ //split reversible reaction into two reactions
            reac->set_bounds(0, ub);
            tmod->NewReaction(Rev);//add reverse reaction with new column
            Reaction *c2=tmod->get_reac(Rev);
            c2->set_comment(Rn+":Reverse");
            c2->set_rules(rules);
            stomap *sto_c2 = c2->get_sto();
            *sto_c2=*sto;
            for ( stomap::iterator it=sto_c2->begin(); it!= sto_c2->end(); it++ )
                it->second=-(it->second);
            c2->set_bounds(0, -lb);
        }
    }
}


int GFA::eval_reaction_state(Reaction* reac, string arr, flomap* Rstate){//w/
    strvec rules=reac->get_rules();
    string Rname=reac->get_name();
    strvec::iterator it=rules.begin();
    int size=rules.size();
    if(size==1) return (*Rstate)[Rname]=get_expvalue(*it, arr); //set reaction state
    else if(size>1){
        int* k=new int(0);
        return (*Rstate)[Rname]=rule2state(rules, k, arr, size);
        delete k;
    }
    else return 0;
}


double GFA::eval_reaction_state_f(Reaction* reac, string arr, flomap* Rstate){//w/
    strvec rules=reac->get_rules();
    string Rname=reac->get_name();
    strvec::iterator it=rules.begin();
    int size=rules.size();
    if(size==1) return (*Rstate)[Rname]=get_expvalue_f(*it, arr); //set reaction state
    else if(size>1){
        int* k=new int(0);
        return (*Rstate)[Rname]=rule2state_f(rules, k, arr, size);
        delete k;
    }
    else return 0;
}

//w/ evaluate reaction state from nesting rules
int GFA::rule2state(strvec rules, int *k, string arr, int size){
    int FIRST=100;
    int state=FIRST;
    string op;
	while(*k!=size){
        if(rules[*k]==OPEN_PAR){ //for rule segment within parentheses
            (*k)++;
            int b=rule2state(rules, k, arr, size);
            if(state==FIRST) state=b;
            else state=tri_bool_logic(state, b, op);
        }
		else if(rules[*k]==CLOS_PAR){
        	(*k)++;
        	break;
        }
        else if(rules[*k]==AND || rules[*k]==OR) { //get next rule segment
            op=rules[*k];
            (*k)++;
        }
        else{ //rule segement without parentheses
            while((*k!=size) && (rules[*k]!=OPEN_PAR) && (rules[*k]!=CLOS_PAR)){
            	if(state==FIRST) state=get_expvalue(rules[*k], arr);
            	else if(rules[*k]==AND || rules[*k]==OR) op=rules[*k];
                else{
                    int b=get_expvalue(rules[*k], arr);
                    state=tri_bool_logic(state, b, op);
                }
                (*k)++;
            }
        }
    }
    return state;
}

//w/ evaluate reaction state from nesting rules
double GFA::rule2state_f(strvec rules, int *k, string arr, int size){
    double FIRST=1e5;
    double state=FIRST;
    string op;
	while(*k!=size){
        if(rules[*k]==OPEN_PAR){ //for rule segment within parentheses
            (*k)++;
            double b=rule2state_f(rules, k, arr, size);
            if(state==FIRST) state=b;
            else state=tri_bool_logic_f(state, b, op);
        }
		else if(rules[*k]==CLOS_PAR){
        	(*k)++;
        	break;
        }
        else if(rules[*k]==AND || rules[*k]==OR) { //get next rule segment
            op=rules[*k];
            (*k)++;
        }
        else{ //rule segement without parentheses
            while((*k!=size) && (rules[*k]!=OPEN_PAR) && (rules[*k]!=CLOS_PAR)){
            	if(state==FIRST) state=get_expvalue_f(rules[*k], arr);
            	else if(rules[*k]==AND || rules[*k]==OR) op=rules[*k];
                else{
                    double b=get_expvalue_f(rules[*k], arr);
                    state=tri_bool_logic_f(state, b, op);
                }
                (*k)++;
            }
        }
    }
    return state;
}


int GFA::get_expvalue(string Gn, string arr){ //w/
    if(arrexp.find(Gn)!=arrexp.end() && arrexp[Gn][arr]!=100){return arrexp[Gn][arr];}
    else return 0;
}

double GFA::get_expvalue_f(string Gn, string arr){ //w/
    if(arrexp_f.find(Gn)!=arrexp_f.end() && arrexp_f[Gn][arr]!=1e5){return arrexp_f[Gn][arr];}
    else return 0;
}

int GFA::tri_bool_logic(int a, int b, string op){ //w/
    if(op==OR){ return a>=b?a:b;}
    else if(op==AND){return a<=b?a:b;}
}

double GFA::tri_bool_logic_f(double a, double b, string op){ //w/
    if(op==OR){ return a>=b?a:b;}
    else if(op==AND){return a<=b?a:b;}
}


FLUXDIR GFA::compute_reac_fluxact(string Rn, Model* model, MtxLP* lp, stomap *obj, bool presolve){//w/
	FLUXDIR flux;
	Reaction* reac=model->get_reac(Rn);
	double lb=reac->get_lb();
	double ub=reac->get_ub();
        if(ub>eps && lb<-eps) {
	    flux.direc=0;
//	    lp->getRange_gfaa(flux.dirp, obj, presolve, Rn, ub, eps);
//	    lp->getRange_gfaa(flux.dirm, obj, presolve, Rn, lb, eps);

        flux.dirp=lp->getRange_gfaa(obj, presolve, Rn, ub, eps);
	    flux.dirm=lp->getRange_gfaa(obj, presolve, Rn, lb, eps);

        if(flux.dirp.act.status==5 && flux.dirm.act.status==5)//judge direction
            flux.dirlev=flux.dirp.act.objval-flux.dirm.act.objval;
        else if(flux.dirp.act.status==5) flux.dirlev=1;
        else if(flux.dirm.act.status==5) flux.dirlev=-1;
        else if(flux.dirp.inact.status==5 && flux.dirm.inact.status==5)
            flux.dirlev=flux.dirp.inact.objval-flux.dirm.inact.objval;
        else if(flux.dirp.inact.status==5) flux.dirlev=1;
        else if(flux.dirm.inact.status==5) flux.dirlev=-1;
        else flux.dirlev=1;
	}
	else if(ub>eps) {
	    flux.direc=1;
//            lp->getRange_gfaa(flux.dirp, obj, presolve, Rn, ub, eps);
        flux.dirp=lp->getRange_gfaa(obj, presolve, Rn, ub, eps);
	    flux.dirlev=1;
	}
	else if(lb<-eps) {
	    flux.direc=-1;
//            lp->getRange_gfaa(flux.dirm, obj, presolve, Rn, lb, eps);
        flux.dirm=lp->getRange_gfaa(obj, presolve, Rn, lb, eps);
	    flux.dirlev=-1;
	}        
	lp->setColBnds(Rn, lb, ub);//recover to the original bounds of model
	return flux;
}


FLUXACT GFA::compute_gene_fluxact(reac_rangemap *Rrm, strvec Rs, Model* model, MtxLP* lp, stomap *obj, bool presolve){//w/
   FLUXACT gact;
   if(Rs.size()==1)
      gact=get_fluxact_dir(Rrm, Rs[0], model, lp, obj, presolve);
   else if(Rs.size()>1){
      FLUXACT max=get_fluxact_dir(Rrm, Rs[0], model, lp, obj, presolve); //x: get max activity of reacs
      for(int j=1;j<Rs.size();j++){
      	FLUXACT temp=get_fluxact_dir(Rrm, Rs[j], model, lp, obj, presolve);
        max=temp.act.objval>max.act.objval?temp:max;
      }
      gact.act=max.act;

      for(int j=0;j<Rs.size();j++){//y: get optval when all reacs inactive
          FLUXDIR greac=(*Rrm).find(Rs[j])->second;
          if(greac.direc==1) lp->setColBnds(Rs[j], 0, eps);//judge direction
          else if(greac.direc==-1) lp->setColBnds(Rs[j], -eps, 0);
          else if(greac.dirlev>=0) lp->setColBnds(Rs[j], 0, eps);
          else if(greac.dirlev<0) lp->setColBnds(Rs[j], -eps, 0);
      }
      lp->getOptObjStat(gact.inact, obj, true, presolve);
      gact.level=gact.act.objval-gact.inact.objval;
    }
    for(int j=0;j<Rs.size();j++){ //recover to the original bounds of model
            Reaction* reac=model->get_reac(Rs[j]);
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            lp->setColBnds(Rs[j], lb, ub);
    }
    return gact;
}


FLUXACT GFA::get_fluxact_dir(reac_rangemap *Rrm, string Rn, Model* model, MtxLP* lp, stomap *obj, bool presolve){//w/
    FLUXDIR flux;
    if(Rrm->find(Rn)!=Rrm->end()){
    	flux=(*Rrm).find(Rn)->second;
    	if(flux.dirlev>=0) return flux.dirp;
    	else return flux.dirm;
    }
    else{
            flux=compute_reac_fluxact(Rn, model, lp, obj, presolve);
            (*Rrm)[Rn]=flux;
            if(flux.dirlev>=0) return flux.dirp;
            else return flux.dirm;
    }
}


//gfa/ get reactions and genes of current model (after trim)
void GFA::get_args_gfa(Model* model, strvec &args, strvec &reacs, strvec &genes){
    strvec allreacs =model->get_reactions();//get reactions of current model
    argmap Rmap,Gmap;
    strvec Reacs;
    strvec Genes; //get genes which associate current reactions

    for(int i=0;i<allreacs.size();i++){
    	string Rn=allreacs[i];
    	Reaction* reac=model->get_reac(Rn);
        if(reac->get_kind()==CONT){
            Rmap[Rn]=1;
            Reacs.push_back(Rn);
            strvec* Rule=reac->get_rule();
            for(int j=0;j<Rule->size();j++){
                string Gn=(*Rule)[j];
                if(Gn!=AND && Gn!=OR){
                    if(exist_gene(Gn)){
                        geneR[Gn].push_back(Rn);
                        if(Gmap.find(Gn)==Gmap.end()){
                            Gmap[Gn]=1;
                            Genes.push_back(Gn);
                        }
                    }
                }
            }
        }
    }
    if(args.size()>0){
        for(int i=0;i<args.size();i++){
            argmap::iterator Rit=Rmap.find(args[i]);
            argmap::iterator Git=Gmap.find(args[i]);
            if(Rit!=Rmap.end())
                reacs.push_back(Rit->first);
            else if(Git!=Gmap.end())
                genes.push_back(Git->first);
            else throw runtime_error("argument error: "+args[i]+"is not found in current model!");
        }
    }
    else{
        reacs=Reacs;
        genes=Genes;
    }
}



