#include "DriverGrb.h"
#include <map>
#include "Types.h"
#include <time.h>


DriverGrb::DriverGrb():Driver(){
    env=0;
    env=new GRBEnv();//w/grb/
    grbmod=new GRB_Mod(*env);//w/grb/
    init_msg_grb();//w/grb/
}

DriverGrb::~DriverGrb(){
    //if (model != NULL) delete model;
    //if (lp != NULL) delete lp;
    //if (out != &cout) delete out;
    //if (records != NULL) delete records;
    //if (obj != NULL) delete obj;
    //if (gfa != NULL) delete gfa;//w/gfa/
    //if (Rstate != NULL) delete Rstate;//w/gfa/
    //if (gni != NULL) delete gni;//w/gni/
    //if (dpa != NULL) delete dpa;//w/dpa/
    if (grbmod != NULL) delete grbmod;//w/grb/
    if (env != NULL) delete env;//w/grb/
}

void DriverGrb::set_solver(char *arg){
    //kind = get_arg(string(arg));
    strvec solp; //w/solver parameters
    string tt(arg);
    split2(&tt, &solp, COMMA);

    argmap::const_iterator it = ARGMAP.find(solp[0]);
    if (it == ARGMAP.end())
        throw runtime_error(solp[0] + ": unknown argument!");
    else kind=it->second;
    if(solp.size() > 1)
        if(!solp[1].empty()) grbmod->soltol=str2double(solp[1]);
    if(solp.size() > 2)
        if(!solp[2].empty()) grbmod->mipfoc=str2int(solp[2]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//running
void DriverGrb::run(){
    try{
        //env=0;
        //env=new GRBEnv();//w/grb/
        //grbmod=new GRB_Mod(*env);//w/grb/
        initializeRun();
        time_t t0 = time(NULL);
        void (DriverGrb::*ptr_solve)();
        if (GRBPROB.find(problem)==GRBPROB.end()) //w/grb/
            throw runtime_error(string("Gurobi solover does not support for this problem: ") + problem);
        grbmod->setOptDir(GrbMax);
        grbmod->setPara();

        //Gurobi problems----------------------------------
        if (ext.empty()) model->get_externals(externals);
        else split(&ext, &externals, SPACE);//parse the list of externals supplied with the command line

        if (problem == "uncons") ptr_solve = &DriverGrb::uncons;
        else if (problem == "objvalue") ptr_solve = &DriverGrb::objval;
        else if (problem == "objstat") ptr_solve = &DriverGrb::objstat;
        else if (problem == "fba") ptr_solve = &DriverGrb::fba;                    
        else if (problem == "fva") ptr_solve = &DriverGrb::fva;
        else if (problem == "fsa") ptr_solve = &DriverGrb::fsa;
        else if (problem == "plot") ptr_solve = &DriverGrb::plot;
        else if (problem == "plot3d") ptr_solve = &DriverGrb::plot3D;
        else if (problem == "em"){
            dosplit = true;
            ptr_solve = &DriverGrb::elmode;
            //if (kind == SMPLX) kind = EXACT;
        }
        else if (problem == "mss"){
            dosplit = true;
            ptr_solve = &DriverGrb::mss;
            kind = MILP;
        }
        else if (problem == "mps"){
            dosplit = true;
            ptr_solve = &DriverGrb::mps;
            kind = MILP;
        }
        else if(problem =="troff") ptr_solve = &DriverGrb::tradeoff; //Zakrzewski's change
        //else if (problem == "tfba"){
        //    thermo = true;
        //    ptr_solve = &DriverGrb::tfba;
        //}
        else if (problem == "bottlenecks") ptr_solve = &DriverGrb::bottlenecks;
        //else if (problem == "tmfa"){
        //    thermo = true;
        //    ptr_solve = &DriverGrb::tmfa;
        //}
        //else if (problem == "tva"){
        //    thermo = true;
        //    ptr_solve = &DriverGrb::tva;
        //}
        else if (problem == "test"){
            //thermo = true;
            ptr_solve = &DriverGrb::test;
        }
        //else if (problem == "gimme") ptr_solve = &DriverGrb::fba; //w/
        //else if (problem == "gimmefva") ptr_solve = &DriverGrb::gfva3;//w/                 

        //if (withcomments) *out << COMMENT << problem << endl << endl;

        if (problem == "imat"){//w/gfa/--------- gfaa problem ------------
            gfa->read_expression(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model(arr, model, obj, Rstate, externals); //set array model
                grbmod->build_gfa(model);
                grbmod->externalise(externals, true);
                gfaa();
                grbmod->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        //w/gfa/ iMAT's method, get flux variability after fixing MILP binary
        else if(problem == "gfva"){
            gfa->read_expression(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gfva(arr, model, obj, Rstate); //set array model
                grbmod->build_gfa(model);
                grbmod->externalise(externals, true);
                //grbmod->write("GRBmodel.LP");//t/
                gfva();
                grbmod->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        //w/gfa/ objective is nonzero state reactions with state as coefs, get flux variability after fixing the reactions flux, eversible reac can be select only in one direction
        else if(problem == "fimat"){
            gfa->read_expression_f(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                //gfa->set_gfa_model_gfva2(arr, model, obj, Rstate); //set array model
                gfa->set_gfa_model_fimat(arr, model, obj, Rstate); //set array model
                grbmod->build_gfa2(model, obj);
                grbmod->externalise(externals, true);
                //grbmod->write("GRBmodel.LP");//t/
                gfva2();
                grbmod->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        else if(problem == "fimat2"){//same as fimat, but fixing the nonzero state rxn with bound range by active or inactive state
            gfa->read_expression_f(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gfva2(arr, model, obj, Rstate); //set array model
                grbmod->build_gfa2(model, obj);
                grbmod->externalise(externals, true);
                //grbmod->write("GRBmodel.LP");//t/
                gfva2x();
                grbmod->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }       
        else if(problem == "gim3e"){ //w/GIM3E/ 
            if(!expfile.empty()) gfa->read_expression_met(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_obj_arr();//use target to set obj and arr, override problem file

                GRB_Mod* mod=new GRB_Mod(*env);//build raw model to get objective value
                mod->build(model);
                mod->setOptDir(GrbMax);
                mod->setPara();
                OBJSTAT objstat;
                mod->setMixedObj(obj, externals);
                mod->externalise(externals, true);
                objstat=mod->optimize();
                if(objstat.status != GRB_OPTIMAL) throw runtime_error("error: can't find optimal solution for objective function!");
                if (withcomments) *out << COMMENT << objstat << " (" << *obj << ')' << endl;
                delete mod;

                gfa->set_gim3e_model_split(model);
                gfa->set_gim3e_model_TS(model, arr, grbmod->soltol);
                grbmod->build_gfa2(model, obj);
                grbmod->externalise(externals, true);
                fva_gim3e(objstat.objval);
                grbmod->externalise(externals, false);

                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        else if(problem == "sgni"){ //w/gni/ <SrongGNI> 
            gni->gene_koreacs(model);
            gni->map_xt_sto(model, externals);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gni(this, model, (*records)[0])){//set nts and ntsp
                del_exts();
                gni->set_ntsa(externals);
                gni->fix_absNts(model);
                gni->set_blp_model(model, obj, target);//set blp model

                grbmod->build_sgni(model, obj, target);
                sgni();
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        else if(problem == "wgni"){ //w/gni/ <WeakGNI with Gurobi> 
            gni->gene_koreacs(model);
            gni->map_xt_sto(model, externals);

            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gni(this, model, (*records)[0])){//set nts and ntsp
                del_exts();
                gni->set_ntsa(externals);
                gni->fix_absNts(model);

                GRB_Mod* mod=new GRB_Mod(*env);//mod for wgni
                mod->build(model);
                mod->setOptDir(GrbMax);
                mod->setPara();

                gni->set_blp_model(model, obj, target);//set blp model
                grbmod->build_sgni(model, obj, target);
                //gni->fix_absNts_grb(model, grbmod);
                //gni->fix_absNts_grb(model, mod);
                //wgniGrb(mod);
                wgni2(mod);
                *out << ENDOFRECORD << endl << endl;
                if (write) grbmod->write("GRBmodel.LP");
            }
            //}
        }
        else if(problem == "ess"){ //w/gni/ <essGrb> 
            gni->gene_koreacs(model);       
            for (strvec::iterator it = records->begin(); it != records->end(); ++it){
                if (read_record_gni(this, model, *it)){
                    del_exts();
                    grbmod->build(model);
                    gni->map_xt_sto(model, externals);

                    ess();

                    *out << ENDOFRECORD << endl << endl;
                    if (write) grbmod->write("GRBmodel.LP");
                }
            }
        }
        else if(problem == "ko"){ //w/ <koGrb> 
            for (strvec::iterator it = records->begin(); it != records->end(); ++it){
                del_exts();
                grbmod->build(model);
                if (read_record_Grb(this, *it)){
                    //for (stomap::iterator it = obj->begin(); it != obj->end(); ++it){//t/
                    //         cout<<it->first<<':'<<it->second<<endl;
                    //}
                    grbmod->setObjs(obj);
                    ko();

                    *out << ENDOFRECORD << endl << endl;
                    if (write) grbmod->write("GRBmodel.LP");
                }
            }
        }
        else if(problem == "dpaplot"){ //w/dpa/ <DPA producibility plot> 
            dpa->gene_koreacs(model);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (!records->empty())
                read_record_gfa(this, (*records)[0]);//set assigned externals
            del_exts();
            grbmod->build(model);

            dpaplot();

            *out << ENDOFRECORD << endl << endl;
            if (write) grbmod->write("GRBmodel.LP");
            //}
        }
        else if(OTHPROB.find(problem)!=OTHPROB.end()){ //w/grb/ for problems: objvalue, objstat, fba, fva
            max=GLP_MAX;
            for (strvec::iterator it = records->begin(); it != records->end(); ++it){
                //del_exts();
                grbmod->build(model);
                if (read_record_Grb(this, *it)){
                    //for (stomap::iterator it = obj->begin(); it != obj->end(); ++it){//t/
                    //    cout<<it->first<<':'<<it->second<<endl;
                    //}
                    grbmod->setMixedObj(obj, externals);
                    grbmod->externalise(externals, true);
                    (*this.*ptr_solve)();
                    grbmod->externalise(externals, false);

                    *out << ENDOFRECORD << endl << endl;
                    if (write) grbmod->write("GRBmodel.LP");
                }
            }
        }
        else throw runtime_error(string("No such program: ") + problem);

        if (withcomments) *out << COMMENT << "time spent: " << time(NULL) - t0  << endl;
        out->flush();
        
    }
    catch (exception &e){
        cerr << e.what() << endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//analysis functions
void DriverGrb::objval(){
    *out <<grbmod->optimize().objval;//w/grb/
}

void DriverGrb::objstat(){
    OBJSTAT objstat;
    objstat=grbmod->optimize();//w/grb/
    *out << objstat;
}

void DriverGrb::fba(){
    stomap* sol = new stomap;
    OBJSTAT objstat;
    objstat=grbmod->optimize(sol);
    if (withcomments) *out << COMMENT << objstat << " (" << *obj << ')' << endl;

    if(problem=="gimme")//w/gimme
        print_fba_state(out, sol, Rstate, model, withcomments);
    else
        model->print_sto(sol, out, withcomments);
    delete sol;    
}

void DriverGrb::fva(){//w/va->fva
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    args = get_args_gfva(args);
    grbmod->getOptRanges(rm, objrange, obj, args);

    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    //print_rangemap(out, rm, model);
    print_rangemap2(out, rm, model, Rstate, withcomments, false);
    delete rm;
}

void DriverGrb::fva_gim3e(double value){//w/gim3e
    grbmod->addConstraint(obj, 0.99*value, value, "OBJECTIVE");
    rangemap *rm = new rangemap;
    args = get_args_gfva(args);
    grbmod->getRanges(rm, args);

    //if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    //print_rangemap(out, rm, model);
    print_rangemap2(out, rm, model, Rstate, withcomments, false);
    //print_rangemap2_tab(out, rm, model, Rstate, withcomments, false);
    delete rm;
}

//void Driver::fvaGrb(){//w/
//    if (args.empty()) args = model->get_reactions();
//    rangemap *rm = new rangemap;
//    OBJSTAT objrange;
//    grbmod->getOptRanges(rm, objrange, obj, args);
//    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
//    print_rangemap(out, rm, model);
////    print_rangemap_gfva(out, rm, model, Rstate);
//    delete rm;
//}

void DriverGrb::gfaa(){//w/gfa/
    //    grbmod->write("X_grbmod_gfa.LP");//t/
    strvec reacs, genes;
    gfa->get_args_gfa(model, args, reacs, genes);
    //---------------------flux activity for reactions-----------------------
    reac_rangemap *Rrm = new reac_rangemap;
    for(int i=0;i<reacs.size();i++){// compute flux activity and direction for reactions
        string Rn=reacs[i];
        (*Rrm)[Rn]=gfa->compute_reac_fluxact_grb(Rn, model, grbmod);
    }

    //---------------------flux activity for genes---------------------------
    gene_rangemap *Grm = new gene_rangemap;
    for(int i=0;i<genes.size();i++){
        string Gn=genes[i];
        strvec Rs=gfa->get_geneReacs(Gn);
        (*Grm)[Gn]=gfa->compute_gene_fluxact_grb(Rrm, Rs, model, grbmod);
    }
    //    print_rangemap_gfaa(out, Rrm, Grm, reacs, model, gfa, arr, Rstate);
    print_rangemap_gfaa_tab(out, Rrm, Grm, reacs, model, gfa,  arr, Rstate);
    delete Rrm;
    delete Grm;
}

void DriverGrb::gfva(){//w/gfa/
    args = get_args_gfva(args);
    //    for(int i=0;i<args.size();i++) cout<<args[i]<<endl;//t/
    rangemap *rm = new rangemap;
    OBJSTAT objrange;

    grbmod->getOptRanges_gfva(rm, objrange, args);
    //    grbmod->getOptRanges_gfva_fixmod(rm, objrange, args);//use fixedmodel very strengent

    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    //print_rangemap_gfva(out, rm, model, Rstate, withcomments);
    print_rangemap2_tab(out, rm, model, Rstate, withcomments);
    delete rm;
}

void DriverGrb::gfva2(){//w/fimat
    args = get_args_gfva(args);
    //    for(int i=0;i<args.size();i++) cout<<args[i]<<endl;//t/
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    stomap *allsol=new stomap;
    grbmod->getOptRanges_gfva2(rm, objrange, allsol, args, obj);
    //    grbmod->getOptRanges_gfva_fixmod(rm, objrange, args);//use fixedmodel very strengent

    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    print_rangemap2_tab(out, rm, model, Rstate, withcomments);
    //print_rangemap2_tab(out, rm, allsol, model, Rstate, withcomments);
    delete rm;
    delete allsol;
}

void DriverGrb::gfva2x(){//w/fimat2
    args = get_args_gfva(args);
    //    for(int i=0;i<args.size();i++) cout<<args[i]<<endl;//t/
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    stomap *allsol=new stomap;
    grbmod->getOptRanges_gfva2x(rm, objrange, allsol, args, obj, gfa->eps);
    //    grbmod->getOptRanges_gfva_fixmod(rm, objrange, args);//use fixedmodel very strengent

    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    print_rangemap2_tab(out, rm, model, Rstate, withcomments);
    //print_rangemap2_tab(out, rm, allsol, model, Rstate, withcomments);
    delete rm;
    delete allsol;
}

void DriverGrb::sgni(){//w/gni/ strong gni using Gurobi
    //    grbmod->write("X_sgni_grbmod.LP");//t/
    gene_sgni sgniOut;
    strvec gns;
    if(args.size()>0){
        gns=args;
    }else gns=gni->genes;
    for(int i=0;i<gns.size();i++){
        string Gn=gns[i];
        strvec rkos=gni->getRkos(Gn);
        if(rkos.size()<1) continue;
        sgniOut[Gn].rkos=rkos;
        //        grbKoRow kor;
        //        grbKoCol koc;
        //        grbmod->ko_blp(rkos, rnts, kor, koc);
        grbKos kos;
        grbmod->ko_blp(rkos, kos, gni->reacN);

        //        char di = (char)(((int)'0')+i);//t/
        //        string dig=string(1,di);//t/
        //        grbmod->write("X_sgni_ko_"+Gn+".LP");//t/

        BLPSOL bsol;
        stomap medium;//growth medium
        blp_fba(grbmod, bsol, medium);
        sgniOut[Gn].bsol=bsol;
        sgniOut[Gn].medium=medium;
        //-----------------------------------------------------
        if(bsol.status=="OPTIMAL" && bsol.level>0.2){//if the gene is essential under the growth medium
            for (stomap::iterator it = medium.begin(); it != medium.end(); ++it){
                string nt=it->first;
                string ce="ce__"+nt;
                int ceval=it->second;
                if(ceval==0){//judging if GNI-SN (Strong Negative)
                    grbmod->setVarBnds(ce, 1, 1);
                    blp_fba(grbmod, bsol);
                    if(bsol.status!="OPTIMAL" || bsol.level<=0.2)
                        sgniOut[Gn].sn.push_back(nt);
                    grbmod->setVarBnds(ce, 0, 1);
                }
                else if(ceval==1){//judging if GNI-SP (Strong Positive)
                    grbmod->setVarBnds(ce, 0, 0);
                    blp_fba(grbmod, bsol);
                    if(bsol.status!="OPTIMAL" || bsol.level<=0.2)
                        sgniOut[Gn].sp.push_back(nt);
                    grbmod->setVarBnds(ce, 0, 1);
                }
            }
        }
        //        grbmod->rko_blp(kor, koc);
        grbmod->rko_blp(kos);
        //        grbmod->write("X_sgni_rko"+Gn+".LP");//t/
    }
    print_sgni(out, sgniOut);
}

void DriverGrb::wgni(GRB_Mod* mod){//w/gni/ weak gni using Gurobi
    //    grbmod->write("X_grbmod.LP");//t/
    //    mod->write("X_mod.LP");//t/
    gene_wgni1 wgniOut1;
    mod->setObj(target, 1);
    objstatvec wtbm;//BIOMASS fba for wild type over all rmeds
    randmeds rmeds=gni->rand_meds();
    for(int s=0;s<rmeds.size();s++){
        intvec rmed=rmeds[s];
        OBJSTAT solwt=fba_med(mod,rmed);
        wtbm.push_back(solwt);
        //        for(int i=0;i<rmed.size();i++){//t/
        //            cout<<rmed[i]<<',';
        //        }
        //        cout<<endl;//t/
    }

    strvec gns;
    if(args.size()>0){
        gns=args;
    }else gns=gni->genes;
    //    for (gene_reactions::iterator it = geneR.begin(); it != geneR.end(); ++it){
    for(int i=0;i<gns.size();i++){
        string Gn=gns[i];
        strvec rkos=gni->getRkos(Gn);

        if(rkos.size()<1) continue;
        //        grbKoRow kor;
        //        grbKoCol koc;
        //        grbmod->ko_blp(rkos, rnts, kor, koc);
        grbKos kos;
        grbmod->ko_blp(rkos, kos, gni->reacN);
        //        char di = (char)(((int)'0')+i);//t/
        //        string dig=string(1,di);//t/
        //        grbmod->write("X_grbmod_ko"+dig+".LP");//t/

        BLPSOL bsol;
        stomap medium;//growth medium
        blp_fba(grbmod, bsol, medium);
        if(bsol.status=="OPTIMAL" && bsol.level>0.2){//if the gene is essential under a growth medium
            //            for(stomap::iterator it=medium.begin();it!=medium.end();it++) cout<<it->first<<':'<<it->second<<endl;//t/
            WGNI1 wgni;
            wgni.Gn=Gn;
            wgni.rkos=rkos;
            wgni.Ness=0;
            //            grbKoCol mkoc;
            //            mod->ko_mlp(rkos, mkoc);
            //            mod->write("X_mod_ko"+dig+".LP");//t/

            for(int i=0;i<rkos.size();i++){//ko_mlp
                string Rn=rkos[i];
                mod->setVarBnds(Rn, 0, 0);
            }


            for(int s=0;s<rmeds.size();s++){
                intvec rmed=rmeds[s];
                OBJSTAT solko=fba_med(mod,rmed);
                if(wtbm[s].status==GRB_OPTIMAL && solko.status==GRB_OPTIMAL){
                    double level;
                    if(wtbm[s].objval-solko.objval<1e-7) level=0;
                    else level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
                    //                    level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
                    if(level>0.2){
                        wgni.Ness++;
                        for(int i=0;i<rmed.size();i++){
                            string nt=gni->nts[i];
                            if(rmed[i]==1) wgni.nts[nt].Np++;
                            else wgni.nts[nt].Nn++;
                        }
                    }
                }
            }
            //            wgni.pval=wgni.Ness==0?1:gsl_cdf_binomial_Q(wgni.Ness-1, 0.01, rmeds.size());
            wgniOut1.push_back(wgni);
            //            mod->rko_mlp(mkoc);

            for(int i=0;i<rkos.size();i++){//rko_mlp
                string Rn=rkos[i];
                Reaction* reac=model->get_reac(Rn);
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                mod->setVarBnds(Rn, lb, ub);
            }

        }
        //        mod->write("X_wgni_ko"+Gn+".LP");//t/
        //        grbmod->rko_blp(kor, koc);
        grbmod->rko_blp(kos);
        //        mod->write("X_wgni_rko"+Gn+".LP");//t/
    }//-------------------------------------------------------------------------------
    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
    print_wgni1(out, wgniOut1, rmeds.size());
}                     

void DriverGrb::wgni2(GRB_Mod* mod){//w/gni/ weak gni using Gurobi
    gene_wgni1 wgniOut1;
    mod->setObj(target, 1);
    objstatvec wtbm;//BIOMASS fba for wild type over all rmeds
    cout<<"Sampling media: ";
    randmeds rmeds=gni->rand_meds();
    cout<<rmeds.size()<<endl;

    //for(int s=0;s<rmeds.size();s++){//t/
    //    intvec rmed=rmeds[s];
    //    for(int i=0;i<rmed.size();i++) cout<<rmed[i]<<',';
    //    cout<<endl;//t/
    //}//t/

    cout<<"Computing wild type biomass"<<endl;
    for(int s=0;s<rmeds.size();s++){
        intvec rmed=rmeds[s];
        OBJSTAT solwt=fba_med(mod,rmed);
        wtbm.push_back(solwt);
    }

    strvec gns;
    if(args.size()>0){
        gns=args;
    }else gns=gni->genes;
    cout<<"Computing WGNIs of "<<gns.size()<<" genes: "<<endl;
    for(int i=0;i<gns.size();i++){
        string Gn=gns[i];
        strvec rkos=gni->getRkos(Gn);
        if(rkos.size()<1) continue;
        WGNI1 wgni;
        wgni.Gn=Gn;
        wgni.rkos=rkos;
        wgni.Ness=0;

        for(int i=0;i<rkos.size();i++){//ko_mlp
            string Rn=rkos[i];
            mod->setVarBnds(Rn, 0, 0);
        }

        for(int s=0;s<rmeds.size();s++){//BIOMASS fba for ko type of gene Gn over all rmeds
            intvec rmed=rmeds[s];
            OBJSTAT solko=fba_med(mod,rmed);
            if(wtbm[s].status==GRB_OPTIMAL && solko.status==GRB_OPTIMAL){
                double level;
                if(wtbm[s].objval-solko.objval<1e-7) level=0;
                else level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
                if(level>0.2){
                    wgni.Ness++;
                    for(int i=0;i<rmed.size();i++){
                        string nt=gni->nts[i];
                        if(rmed[i]==1) wgni.nts[nt].Np++;
                        else wgni.nts[nt].Nn++;
                    }
                }
            }
        }
        wgniOut1.push_back(wgni);
        cout<<"\r"<<i+1<<flush;

        for(int i=0;i<rkos.size();i++){//rko_mlp
            string Rn=rkos[i];
            Reaction* reac=model->get_reac(Rn);
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            mod->setVarBnds(Rn, lb, ub);
        }
    }
    //-------------------------------------------------------------------------------
    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
    print_wgni1(out, wgniOut1, rmeds.size());
}

//void DriverGrb::wgniGrb1(GRB_Mod* mod){//w/gni/ weak gni using Gurobi without judging if the gene is essential under a growth medium
////    strvec gns1;//t/test number of essential genes which have koreactions
////    gns1=args;
////    for(int i=0;i<gns1.size();i++){
////        string Gn=gns1[i];
////        strvec rkos=gni->getRkos(Gn);
////        if(rkos.size()>0) *out<<Gn<<endl;
////        rkos.clear();
////    }//t/
//
////    grbmod->write("X_grbmod.LP");//t/
////    mod->write("X_mod.LP");//t/
//    gene_wgni1 wgniOut1;
//    gni->map_xt_sto(model, externals);
//    
//    string nts_p[6]={"MO2_xt", "O2_xt", "CO2_xt", "PI_xt", "FE3_xt", "CO-II_xt"};//set these nts to be present
//    strvec ntsp, ntsa;
//    ntsp.assign(nts_p,nts_p+6);
////    for(int i=0;i<ntsp.size();i++) cout<<ntsp[i]<<endl;//t/
////    cout<<endl;//t/
//    for(int i=0;i<externals.size();i++){//get absent nts excepting present nts from externals
//        string nt=externals[i];
//        if(find(ntsp.begin(),ntsp.end(),nt)==ntsp.end()) ntsa.push_back(nt);
//    }
////    for(int i=0;i<ntsa.size();i++) cout<<ntsa[i]<<endl;//t/
//    gni->fix_absNts_grb(ntsa, model, mod);
////    gni->fix_xt_abs(externals, mod);//set all external nutrients to be absent
//    
////    mod->write("X_mod_ntsa.LP");//t/
//
//    mod->setObj(target, 1);
//    objstatvec wtbm;//BIOMASS fba for wild type over all rmeds
//    randmeds rmeds=gni->rand_meds();
//    for(int s=0;s<rmeds.size();s++){
//        intvec rmed=rmeds[s];
//        OBJSTAT solwt=fba_med(mod,rmed);
//        wtbm.push_back(solwt);
////        for(int i=0;i<rmed.size();i++){//t/
////            cout<<rmed[i]<<',';
////        }
////        cout<<endl;//t/
//    }
//
//    strvec gns;
//    if(args.size()>0){
//        gns=args;
//    }else gns=gni->genes;
////    for (gene_reactions::iterator it = geneR.begin(); it != geneR.end(); ++it){
//    for(int i=0;i<gns.size();i++){
//        string Gn=gns[i];
//        strvec rkos=gni->getRkos(Gn);
//        
//        if(rkos.size()<1) continue;
//
//        WGNI1 wgni;
//        wgni.Gn=Gn;
//        wgni.rkos=rkos;
//        wgni.Ness=0;
////            grbKoCol mkoc;
////            mod->ko_mlp(rkos, mkoc);
////            mod->write("X_mod_ko"+dig+".LP");//t/
//
//        for(int i=0;i<rkos.size();i++){//ko_mlp
//            string Rn=rkos[i];
//            mod->setVarBnds(Rn, 0, 0);
//        }
//
//
//        for(int s=0;s<rmeds.size();s++){
//            intvec rmed=rmeds[s];
//            OBJSTAT solko=fba_med(mod,rmed);
//            if(wtbm[s].status==GRB_OPTIMAL && solko.status==GRB_OPTIMAL){
//                double level;
//                if(wtbm[s].objval-solko.objval<1e-10) level=0;
//                else level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
//                if(level>=0.2){
//                     wgni.Ness++;
//                    for(int i=0;i<rmed.size();i++){
//                        string nt=gni->nts[i];
//                        if(rmed[i]==1) wgni.nts[nt].Np++;
//                        else wgni.nts[nt].Nn++;
//                    }
//                }
//            }
//        }
////        wgni.pval=wgni.Ness==0?1:gsl_cdf_binomial_Q(wgni.Ness-1, 0.01, rmeds.size());
//        wgniOut1.push_back(wgni);
////            mod->rko_mlp(mkoc);
////            mod->write("X_mod_rko"+dig+".LP");//t/
//
//        for(int i=0;i<rkos.size();i++){//rko_mlp
//            string Rn=rkos[i];
//            Reaction* reac=model->get_reac(Rn);
//            double lb=reac->get_lb();
//            double ub=reac->get_ub();
//            mod->setVarBnds(Rn, lb, ub);
//        }
//
//    }//-------------------------------------------------------------------------------
////    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
//    print_wgni1(out, wgniOut1, rmeds.size());
//}

void DriverGrb::ess(){//w/gni/ get essential genes under defined growth medium
    //    grbmod->write("X_grbmod.LP");//t/
    gni->fix_nts_grb(externals, model, grbmod);
    grbmod->setObj(target, 1);
    //    grbmod->write("X_TB-Viv_fixnts.LP");//t/
    OBJSTAT solwt=grbmod->optimize();//BIOMASS fba for wild type

    strvec gns;
    if(args.size()>0){
        gns=args;
    }else gns=gni->genes;
    //    for (gene_reactions::iterator it = geneR.begin(); it != geneR.end(); ++it){
    for(int i=0;i<gns.size();i++){
        string Gn=gns[i];
        strvec rkos=gni->getRkos(Gn);

        ESSG essg;
        essg.Gn=Gn;
        essg.rkos=rkos;

        //            grbKoCol mkoc;
        //            grbmod->ko_mlp(rkos, mkoc);
        //            grbmod->write("X_mod_ko"+dig+".LP");//t/

        for(int i=0;i<rkos.size();i++){//ko_mlp
            string Rn=rkos[i];
            grbmod->setVarBnds(Rn, 0, 0);
        }


        OBJSTAT solko=grbmod->optimize();
        if(solwt.status==GRB_OPTIMAL && solko.status==GRB_OPTIMAL){
            double drop=solwt.objval-solko.objval;
            double level;
            if(drop<1e-10){drop=0; level=0;}
            else level=(solwt.objval-solko.objval)/solwt.objval;
            if(level>=0.2){
                essg.drop=drop;
                essg.level=level;
                essgOut.push_back(essg);
            }
        }

        //            grbmod->rko_mlp(mkoc);
        //            grbmod->write("X_mod_rko"+dig+".LP");//t/

        for(int i=0;i<rkos.size();i++){//rko_mlp
            string Rn=rkos[i];
            Reaction* reac=model->get_reac(Rn);
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            grbmod->setVarBnds(Rn, lb, ub);
        }

    }//-------------------------------------------------------------------------------
    //    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
    print_essg(out, essgOut, gni->nts);
}

void DriverGrb::ko(){//w/gni/ get essential genes under defined growth medium

    strlistvec qlists, blocklists;//qlists: currently only able to use one gene or double genes
    get_list_args(qlists, args);
    model->reaclists_to_block(blocklists, qlists);

    OBJSTAT solwt=grbmod->optimize();//wild type, no ko
    for(int k=0;k<blocklists.size();k++) {//t/
        strvec rkos=blocklists[k];//knoctout reactions
        OBJSTAT solko;
        //        strvec rkos; //knoctout reactions
        if (!rkos.empty()){
            //            for(int i=0;i<list.size();i++) rkos.push_back(list[i]);//t/

            //            grbKoCol mkoc;
            //            grbmod->ko_mlp(rkos, mkoc);
            //            grbmod->write("X_mod_ko"+dig+".LP");//t/

            for(int i=0;i<rkos.size();i++){//ko_mlp
                string Rn=rkos[i];
                grbmod->setVarBnds(Rn, 0, 0);
            }
            solko=grbmod->optimize();

            //            grbmod->rko_mlp(mkoc);
            //            grbmod->write("X_mod_rko"+dig+".LP");//t/

            for(int i=0;i<rkos.size();i++){//rko_mlp
                string Rn=rkos[i];
                Reaction* reac=model->get_reac(Rn);
                double lb=reac->get_lb();
                double ub=reac->get_ub();
                grbmod->setVarBnds(Rn, lb, ub);
            }
            *out << qlists[k] << TAB << solko;
            if (withcomments) *out << TAB << COMMENT << rkos;
            *out << endl;
        }
        else{
            //            *out << qlists[k] << TAB << "NA: NA";
            *out << qlists[k] << TAB << solwt;
            if (withcomments) *out << TAB << COMMENT << rkos;
            *out << endl;
        }
        rkos.clear();
    }
}

void DriverGrb::dpaplot(){//w/dpa/compute producibility plot using glpk
    strstr2flo plotMatrix;//w/dpa/ metabolite and gene to producibility of wildtype and knockout
    objstatvec solwt;//fba for wild type over all metabolites
    strvec metsAll=model->get_metabolites();
    strvec mets;
    if(args.size()>0){
        mets=args;
    }
    else{
        for(int m=0;m<metsAll.size();m++)
            if(find(externals.begin(),externals.end(),metsAll[m])==externals.end()) mets.push_back(metsAll[m]);
    }
    for(int m=0;m<mets.size();m++){
        string met=mets[m];
        grbmod->setObj(met); //externalise(met,true)
        OBJSTAT stat=grbmod->optimize();
        solwt.push_back(stat);
        plotMatrix[met]["WildType"]=stat.objval;
        grbmod->externalise(met,false);
    }
    //--------------------------------------------------------
    met_genes metG;
    gene_mets geneM;
    strvec gns=dpa->genes;
    for(int g=0;g<gns.size();g++){
        string Gn=gns[g];
        strvec rkos=dpa->getRkos(Gn);
        if(rkos.size()<1) continue;
        geneM[Gn].rkos=rkos;

        for(int i=0;i<rkos.size();i++){//ko_mlp
            string Rn=rkos[i];
            grbmod->setVarBnds(Rn, 0, 0);
        }

        for(int m=0;m<mets.size();m++){
            string met=mets[m];
            grbmod->setObj(met);
            OBJSTAT solko=grbmod->optimize();
            if(solwt[m].status==GRB_OPTIMAL && solko.status==GRB_OPTIMAL){
                double Df;
                Df=abs(solwt[m].objval-solko.objval);
                plotMatrix[met][Gn]=solko.objval;
                if(Df>0.001) {
                    metG[met].push_back(Gn);
                    geneM[Gn].mets.push_back(met);
                }
            }
            grbmod->externalise(met,false);
        }
        for(int i=0;i<rkos.size();i++){//rko_mlp
            string Rn=rkos[i];
            Reaction* reac=model->get_reac(Rn);
            double lb=reac->get_lb();
            double ub=reac->get_ub();
            grbmod->setVarBnds(Rn, lb, ub);
        }
        rkos.clear();
    }//--------------------------------------------------------

    print_dpaplot(out, metG, geneM, plotMatrix, mets, gns);
}

void DriverGrb::blp_fba(GRB_Mod *bmod, BLPSOL &bsol, stomap &medium){//w/gni/
    OBJSTAT sol=bmod->optimize();
    bsol.drop=sol.objval;
    bsol.status=get_msg_grb(sol.status);
    GRBVar bm=bmod->getVar(target);
    bsol.bm=bm.get(GRB_DoubleAttr_X);
    if(bsol.drop<1e-7) bsol.level=0;
    //    if(bsol.drop<=0) bsol.level=0;
    else bsol.level=bsol.drop/bsol.bm;//growth drop percentage over BIOMASS of wild
    for(int i=0;i<gni->nts.size();i++){
        string nt=gni->nts[i];
        string ce="ce__"+nt;
        GRBVar var=bmod->getVar(ce);
        medium[nt]=var.get(GRB_DoubleAttr_X);
    }
}

void DriverGrb::blp_fba(GRB_Mod *bmod, BLPSOL &bsol){//w/gni/
    OBJSTAT sol=bmod->optimize();
    bsol.drop=sol.objval;
    bsol.status=get_msg_grb(sol.status);
    GRBVar bm=bmod->getVar(target);
    bsol.bm=bm.get(GRB_DoubleAttr_X);
    if(bsol.drop<1e-7) bsol.level=0;
    //    if(bsol.drop<=0) bsol.level=0;
    else bsol.level=bsol.drop/bsol.bm;//growth drop percentage over BIOMASS of wild
}

OBJSTAT DriverGrb::fba_med(GRB_Mod *mod, intvec rmed){//w/gni/
    //    mod->write("X_mod.LP");//t/
    //    GRBVar var=mod->getVar("R932");//t/
    //    cout<<var.get(GRB_StringAttr_VarName)<<endl;//t/
    //    int S=rmed.size();//t/
    for(int i=0;i<rmed.size();i++){
        string nt=gni->nts[i];
        bool abs=(rmed[i]==0)?true:false;
        gni->set_nt_abs_grb(nt, model, mod, abs);
    }
    //-------------------------------------
    OBJSTAT msol;
    msol=mod->optimize();
    return msol;
}



