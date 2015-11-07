#include <map>
#include "Driver.h"
#include "Types.h"
#include <time.h>

//w/initialize static variables
int Driver::kind;//w/


//initialisation
constmap MSG_DICT;
void Driver::init_msg(){
    MSG_DICT[GLP_OPT] = "OPTIMAL";
    MSG_DICT[GLP_FEAS] = "FEASIBLE";
    MSG_DICT[GLP_INFEAS] = "INFEASIBLE";
    MSG_DICT[GLP_NOFEAS] = "NON-FEASIBLE";
    MSG_DICT[GLP_UNBND] = "UNBOUNDED";
    MSG_DICT[GLP_UNDEF] = "UNDEFINED";
}
string Driver::get_msg(int nmsg){
    return MSG_DICT[nmsg];
}


void Driver::init_args(){
    //message levels
    ARGMAP["off"] = GLP_MSG_OFF;
    ARGMAP["on"] = GLP_MSG_ON;
    ARGMAP["err"] = GLP_MSG_ERR;
    ARGMAP["all"] = GLP_MSG_ALL;
    ARGMAP["max"] = true;
    ARGMAP["min"] = false;
    //solvers
    ARGMAP["simplex"] = SMPLX;
    ARGMAP["milp"] = MILP;
    ARGMAP["exact"] = EXACT;
    //    ARGMAP["intopt"] = INTPT;
    ARGMAP["grb"] = GURB;//w/grb/

    GRBPROB["objvalue"]=1; //w/--
    GRBPROB["objstat"]=1;
    GRBPROB["fba"]=1;
    GRBPROB["fva"]=1;
    GRBPROB["ko"]=1;
    GRBPROB["imat"]=1;
    GRBPROB["gfva"]=1;
    GRBPROB["fimat"]=1;
    GRBPROB["fimat2"]=1;
    GRBPROB["limat"]=1;
    GRBPROB["sgni"]=1;
    GRBPROB["wgni"]=1;
    GRBPROB["dpaplot"]=1;//w/--
    GRBPROB["gim3e"]=1;

    OTHPROB["objvalue"]=1;
    OTHPROB["objstat"]=1;
    OTHPROB["fba"]=1;
    OTHPROB["fva"]=1;
    OTHPROB["fsa"]=1;
    OTHPROB["ko"]=1;
    OTHPROB["plot"]=1;
    OTHPROB["plot3d"]=1;
    OTHPROB["em"]=1;
    OTHPROB["live"]=1;
    OTHPROB["ess"]=1;
    OTHPROB["mss"]=1;
    OTHPROB["mps"]=1;
    OTHPROB["troff"]=1;
    OTHPROB["bottlenecks"]=1;
    OTHPROB["test"]=1;
    OTHPROB["uncons"]=1;
}
int Driver::get_arg(string arg){
    argmap::const_iterator it = ARGMAP.find(arg);
    if (it == ARGMAP.end())
        throw runtime_error(arg + ": unknown argument!");
    return it->second;
}

Driver::Driver(){
    out          = &cout;
    err_msg      = "Error: ";
    withcomments = WITHCOMMENTS;   //include comments into the output
    write        = false;

    problem      = PROBLEM;
    max          = DEF_MAX;
    kind         = SMPLX;             //MILP;//INTPT;//                //the solver algorithm is MILP
    presolve     = true;              //the presolver will be used
    msg_lev      = MSG_LEV;           //text output switched off
    tm_lim       = TM_LIM;            //the number of iterations is unlimited

    model = NULL;
    lp = NULL;
    N = new Matrix;
    records = new strvec;
    obj = new stomap;
    gfa = new GFA;//w/gfa/
    Rstate = new flomap;//w/gfa/
    dosplit = false;
    dotrim = false;
    thermo = false;
    T = TSt;
    cmin = CMIN;
    cmax = CMAX;
    mip_gap = MIP_GAP;
    vmax = VMAX;
    xtag = EXT_TAG_3;
    epsilon = EPSILON_FLUX;
    withzeroes = true;
    gni=new GNI();//w/gni/
    dpa = new DPA;//w/dpa/

    init_args();
    init_msg();
    init_operators();
}

Driver::~Driver(){
    if (model != NULL) delete model;
    if (lp != NULL) delete lp;
    if (out != &cout) delete out;
    if (records != NULL) delete records;
    if (obj != NULL) delete obj;
    if (gfa != NULL) delete gfa;//w/gfa/
    if (Rstate != NULL) delete Rstate;//w/gfa/
    if (gni != NULL) delete gni;//w/gni/
    if (dpa != NULL) delete dpa;//w/dpa/  
}

void Driver::set_msg_lev(char* optarg){
    msg_lev = get_arg(string(optarg));
}

void Driver::set_problem(char *arg){ //w/set problem and related parameters
    //kind = get_arg(string(arg));
    strvec para; //w/---
    string tt(arg);
    split(&tt, &para, COMMA);

    problem=para[0];
    if(para.size() == 2) {
        if(problem=="imat" || problem=="fimat2" || problem=="gfva") gfa->eps =str2double(para[1]);
        else if(problem.find("gimme")!=string::npos) gfa->eps =str2double(para[1]);
        else if(problem=="wgni") gni->Nsam = str2int(para[1]);
    }
    else{
        if(problem=="imat" || problem=="fimat2" || problem=="gfva") gfa->eps =1;
        else if(problem.find("gimme")!=string::npos) gfa->eps =12;
    }
}

void Driver::set_solver(char *arg){
    //kind = get_arg(string(arg));
    strvec solp; //w/solver parameters
    string tt(arg);
    split2(&tt, &solp, COMMA);

    argmap::const_iterator it = ARGMAP.find(solp[0]);
    if (it == ARGMAP.end())
        throw runtime_error(solp[0] + ": unknown argument!");
    else kind=it->second;
    if(solp.size() > 1) {
        if(kind==MILP) mip_gap=str2double(solp[1]);
    }
}

void Driver::set_output(char* arg){
    out = new ofstream(arg);
}

void Driver::set_T(char* optarg){
    T = T0 + atof(optarg);
    if (T < 0) throw runtime_error(err_msg + "temperature must be >= 0!");
}

void Driver::set_time(int lim){
    if (lim < 0) throw runtime_error(err_msg + "time limit must be >= 0!");
    this->tm_lim = lim;
}

void Driver::set_conc_range(char* arg){
    strvec sminmax;
    string is(arg);
    split(&is, &sminmax, SPACE);
    cmin = log(str2double(sminmax[0]));
    if (sminmax.size() > 1)
        cmax = log(str2double(sminmax[1]));
}

void Driver::set_epsilon(char* arg){
    this->epsilon = str2double(arg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//reading text files
void Driver::read_model(){
    //parsing the input
    if (!fexists(infile))
        throw runtime_error(err_msg + "no model file specified!");
    model = new Model;
    parse_model(infile, model, xtag);
}

void Driver::read_bfile(){
    if (bfile.empty())
        records->push_back(CONTINUE);
    else if (!fexists(bfile))
        throw runtime_error(err_msg + "no problem file specified!");
    else{
        ifstream is(bfile.data());
        split(is, records, ENDOFRECORD);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//building LP
void Driver::build(){
    /*    if (problem == "tfba")lp = new TfbaLP(max, T, msg_lev, tm_lim, cmin, cmax);
    else if (thermo)lp = new ThermoLP(max, T, msg_lev, tm_lim, cmin, cmax);
    else*/
    if (dosplit)lp = new SplitLP(max, kind, msg_lev, tm_lim);
    else lp = new MtxLP(max, kind, msg_lev, tm_lim);
    lp->setMipGap(mip_gap);
    lp->setVMax(vmax);
    lp->setEpsilon(epsilon);

    if (dotrim) {
        N->trim(externals);
        //cout << N->cols << endl;
    }
    if(problem=="mss"|| problem=="mps" || problem == "uncons" || problem == "em"|| problem == "live") lp->addMatrix(N);//w/
    else lp->addMatrix_sig(N);
    //lp->addMatrix(N, model);//w/
    //lp->writeLP("GLPKmodel.LP");//t/
    lp->externalise(externals);
    //    set_objective(target);
}

void Driver::build_gfa(){ //w/gfa/ build gfa model
    if (dosplit)lp = new SplitLP(max, kind=MILP, msg_lev, tm_lim);
    else lp = new MtxLP(max, kind, msg_lev, tm_lim);
    if (dotrim) N->trim(externals);
    lp->addMatrix(N, problem, target);
    //    lp->externalise(externals);
}

void Driver::set_objective(string line){
    if (line.empty()) return;
    obj->clear();
    parse_sto(line, obj);
}

//w/gni/ get array name for a problem; one problem has one array
void Driver::set_array(){
    arr.clear();
    //    for(argmap::iterator it=gfa->tisIdx.begin();it!=gfa->tisIdx.end();it++){//t/
    //        cout<<it->first<<':'<<it->second<<endl;
    //    }
    if (target.empty()) arr=gfa->arrName[0];
    else arr=target;
    if(find(gfa->arrName.begin(),gfa->arrName.end(),arr)==gfa->arrName.end()) throw runtime_error("argument error: "+arr+" is not found in expression table!");
}

//w/gim3e/ set objective and metabolite arr for gim3e, override problem file
void Driver::set_obj_arr(){
    if (target.empty()) throw runtime_error("error: please specify the objective function!");
    strvec arrs; //w/solver parameters
    string tt(target);
    split2(&tt, &arrs, COMMA);

    if(arrs.size() > 0) {
        if(!arrs[0].empty()){
            set_objective(arrs[0]);
        }else throw runtime_error("error: please specify the objective function!");
        if(arrs.size() > 1) {
            if(!arrs[1].empty()) arr=arrs[1];
        }else arr=gfa->arrName[0];
    }
    if(find(gfa->arrName.begin(),gfa->arrName.end(),arr)==gfa->arrName.end()) throw runtime_error("argument error: "+arr+" is not found in gene expression table!");

}

//w/dpa/ get array names for a problem, if not set all arraies well be used;
void Driver::set_arrays(string line){
    arrs.clear();
    if (!line.empty()) {
        split(&line, &arrs, SPACE);
        for(int i=0;i<arrs.size();i++){
            if(find(dpa->arrName.begin(),dpa->arrName.end(),arrs[i])==dpa->arrName.end()) throw runtime_error("argument error: "+arrs[i]+" is not found in expression table!");
        }
    }
    else arrs=dpa->arrName;
}

void Driver::set_args(string line){
    //    if (line.empty()) return;
    args.clear();
    split(&line, &args, SPACE);
}

void Driver::set_externals(string line, bool on){
    //input: line of space-separated metabolite names, boolean
    //externalise if on=true, internalise if on=false
    strvec vec;    
    line.erase(line.find_last_not_of(" \n\r\t")+1);//trim the whitespaces
    split(&line, &vec, SPACE);//parse the list
    if (vec.empty())vec = lp->getRowNames();//if the list is empty, apply to all metabolites
    lp->externalise(vec, on);//done
}

void Driver::set_externals_gfa(string line){//w/gfa/
    externals.clear();
    split(&line, &externals, SPACE);
}

strvec Driver::get_args(strvec& args, int type){
    if (args.empty())
        args = lp->getColNames(type);
    else if (dotrim){
        strvec rv;
        elmap *map = N->map;
        elmap::const_iterator end = map->end();
        for (strvec::iterator it = args.begin(); it != args.end(); ++it){
            if (map->find(lp->getID(*it)) != end)
                rv.push_back(*it);
        }
        args = rv;
    }
    return args;
}

strvec Driver::get_args_gfva(strvec& args){//w/gfa/only for continual variables
    strvec cols;
    strvec reacs=model->get_reactions();
    if (args.empty())
        for(int i=0;i<reacs.size();i++){
            string Rn=reacs[i];
            Reaction *reac=model->get_reac(Rn);
            if(reac->get_kind()==CONT) cols.push_back(Rn);
        }
    else{
        for(int i=0;i<args.size();i++){
            if(find(reacs.begin(),reacs.end(),args[i])!=reacs.end()) cols.push_back(args[i]);
        }
    }
    return cols;
}

void Driver::get_list_args(strlistvec& rv, strvec& args, char delim){
    if (args.empty()){
        strvec genes = model->get_genes();
        for (strvec::iterator it = genes.begin(); it != genes.end(); ++it)
            rv.push_back(strvec(1, *it));
        //        strvec reacs = lp->getColNames(CONT);
        strvec reacs = model->get_reactions();//w/for koGrb
        for (strvec::iterator it = reacs.begin(); it != reacs.end(); ++it)
            rv.push_back(strvec(1, *it));
    }
    else{
        for (strvec::iterator it = args.begin(); it != args.end(); ++it){
            strvec arglist;
            split(&*it, &arglist, delim);
            rv.push_back(arglist);
        }
    }
}

void Driver::parse_plot_args(PLOT_ARGS &pa, strvec::const_iterator &it){
    pa.query = *it++;
    pa.from = str2double(*it++);
    pa.to = str2double(*it++);
    pa.step = str2double(*it++);
    if ((pa.from > pa.to && pa.step > 0)) throw runtime_error("invalid range, from > to!");
    if ((pa.from < pa.to && pa.step < 0)) throw runtime_error("invalid range, from < to!");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//running
void Driver::initializeRun(){//w/
    if(!infile.empty()) read_model();//w/
    read_bfile();
    set_args(arg);
    if(problem.find("imat")==string::npos && problem.find("gim")==string::npos)//w/
        set_objective(target);//w/when problem file set objective, this will be overwrite
}

void Driver::run(){
    try{
        initializeRun();
        time_t t0 = time(NULL);
        void (Driver::*ptr_solve)();

        //non LP problems------------------------------------------------------------
        if(problem == "dpasig"){ //w/dpa/ <DPA metatoblite signal> 
            dpa->read_arrayExpression(expfile, err_msg);
            dpa->read_dpaplot(dpafile, err_msg);
            for (strvec::iterator it = records->begin(); it != records->end(); ++it){
                if (read_record_dpa(this, *it)){//set assigned externals
                    dpasig();
                    *out << ENDOFRECORD << endl << endl;
                }
            }
            if (withcomments) *out << COMMENT << "time spent: " << time(NULL) - t0  << endl;
            out->flush();
            return;
        }
        else if (problem == "show"){
            *out << *model;
            return;
        }
        else if (problem == "orphans"){
            model->get_N(N);
            strvec orphans;
            N->get_orphans(orphans, args, externals);
            if (withcomments) *out << COMMENT << orphans.size() << " orphans." << endl;
            model->print_vec(orphans, out, withcomments);
            *out << ENDOFRECORD << endl << endl;
            return;
        }
        else if (problem == "cc"){
            model->get_Nx(N);
            strvec adj;
            mtxvec comps;
            N->connected_components(comps);
            for (mtxvec::iterator it = comps.begin(); it != comps.end(); ++it){
                MATRIX comp = *it;
                if (withcomments) *out << COMMENT << comp.cols.size() << " reactions. " << endl;
                model->print_vec(comp.cols, out, withcomments);
                *out << endl;
                //if (withcomments) *out << COMMENT << comp.rows.size() << " metabolites." << endl;
                //model->print_vec(comp.rows, out, withcomments);
                //*out << endl;
            }
            *out << ENDOFRECORD << endl << endl;
            return;
        }

        //LP problems--------------------------------------------------------------------
        if (ext.empty()) model->get_externals(externals);
        else split(&ext, &externals, SPACE);//parse the list of externals supplied with the command line

        if (problem == "uncons") ptr_solve = &Driver::uncons;
        else if (problem == "objvalue") ptr_solve = &Driver::objval;
        else if (problem == "objstat") ptr_solve = &Driver::objstat;
        else if (problem == "fba") ptr_solve = &Driver::fba;            
        else if (problem == "ko") ptr_solve = &Driver::ko;           
        else if (problem == "fva") ptr_solve = &Driver::fva;
        else if (problem == "fsa") ptr_solve = &Driver::fsa;
        else if (problem == "plot") ptr_solve = &Driver::plot;
        else if (problem == "plot3d") ptr_solve = &Driver::plot3D;
        else if (problem == "em"){
            dosplit = true;
            ptr_solve = &Driver::elmode;
            //if (kind == SMPLX) kind = EXACT;
        }
        else if (problem == "live") ptr_solve = &Driver::live;
        else if (problem == "ess") ptr_solve = &Driver::essentials;
        else if (problem == "mss"){
            dosplit = true;
            ptr_solve = &Driver::mss;
            kind = MILP;
        }
        else if (problem == "mps"){
            dosplit = true;
            ptr_solve = &Driver::mps;
            kind = MILP;
        }
        else if(problem =="troff") ptr_solve = &Driver::tradeoff; //Zakrzewski's change
        else if (problem == "bottlenecks") ptr_solve = &Driver::bottlenecks;
        //else if (problem == "tfba"){
        //    thermo = true;
        //    ptr_solve = &Driver::tfba;
        //}
        //else if (problem == "tmfa"){
        //    thermo = true;
        //    ptr_solve = &Driver::tmfa;
        //}
        //else if (problem == "tva"){
        //    thermo = true;
        //    ptr_solve = &Driver::tva;
        //}
        else if (problem == "test"){
            //thermo = true;
            ptr_solve = &Driver::test;
        }
        else if (problem == "gimme") ptr_solve = &Driver::fba; //w/
        else if (problem == "gimmefva") ptr_solve = &Driver::gfva3;//w/                         

        //if (withcomments) *out << COMMENT << problem << endl << endl;

        if (problem == "imat"){//w/gfa/------ gfaa problem -----
            max=1; kind=MILP;
            gfa->read_expression(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model(arr, model, obj, Rstate, externals); //set array model
                model->get_Nx(N);
                build_gfa();
                lp->externalise(externals, true);
                gfaa();
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        //w/gfa/ iMAT's method, get flux variability after fixing MILP binary
        else if(problem == "gfva"){
            max=1; kind=MILP;
            gfa->read_expression(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gfva(arr, model, obj, Rstate); //set array model
                model->get_Nx(N);
                build_gfa();
                lp->externalise(externals, true);
                gfva();
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        //w/gfa/ objective is nonzero state reactions with state as coefs, get flux variability after fixing the reactions flux, eversible reac can be select only in one direction
        else if(problem == "fimat"){
            max=1; kind=MILP;
            gfa->read_expression_f(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gfva2(arr, model, obj, Rstate); //set array model
                model->get_Nx(N);
                build_gfa();
                lp->externalise(externals, true);
                gfva2();
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        else if(problem == "limat"){
            max=1; kind=SMPLX;
            gfa->read_expression_f(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gfva3(arr, model, obj, Rstate); //set array model
                model->get_Nx(N);
                build_gfa();         
                lp->externalise(externals, true);
                gfva3();
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        else if(problem == "gimme" || problem == "gimmefva"){//w/
            max=0; kind=SMPLX;
            gfa->read_expression_f(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_array();
                gfa->set_gfa_model_gimme(arr, model, obj, Rstate); //set array model
                model->get_Nx(N);
                build_gfa();

                lp->externalise(externals, true);
                (*this.*ptr_solve)();
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        else if(problem == "gim3e"){ //w/GIM3E/ 
            gfa->read_expression_met(expfile, err_msg);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gfa(this, (*records)[0])){
                set_obj_arr();//use target to set obj and arr, override problem file

                model->get_Nx(N);//build raw model to get objective value
                build_gfa();
                lp->externalise(externals, true);
                OBJSTAT objstat;
                lp->getOptObjStat(objstat, obj, max, presolve);
                if(objstat.status != GLP_OPT) throw runtime_error("error: can't find optimal solution for objective function!");
                //lp->externalise(externals, false);
                if (withcomments) *out << COMMENT << objstat << " (" << *obj << ')' << endl;

                gfa->set_gim3e_model_split(model);
                gfa->set_gim3e_model_TS(model, arr, 1e-6);

                model->get_Nx(N);
                build_gfa();
                lp->externalise(externals, true);
                fva_gim3e(objstat.objval);
                lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");

            }
            //}
        }
        else if(problem == "sgni"){ //w/gni/ <SrongGNI> 
            max=1; kind=MILP;
            gni->gene_koreacs(model);
            gni->map_xt_sto(model, externals);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gni(this, model, (*records)[0])){//set nts and ntsp
                del_exts();
                gni->set_ntsa(externals);
                gni->fix_absNts(model);
                gni->set_blp_model(model, obj, target);//set blp model
                model->get_Nx(N);
                build_gfa();
                //gni->fix_absNts(model,lp);
                //lp->externalise(externals, true);
                sgni();
                //lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        else if(problem == "wgni"){ //w/gni/ <WeakGNI> 
            max=1;
            gni->gene_koreacs(model);
            gni->map_xt_sto(model, externals);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (read_record_gni(this, model, (*records)[0])){
                del_exts();
                gni->set_ntsa(externals);
                gni->fix_absNts(model);

                Matrix* N1 =  new Matrix;
                model->get_Nx(N1);
                MtxLP* mlp = new MtxLP(max, kind, msg_lev, tm_lim);
                mlp->addMatrix(N1);

                gni->set_blp_model(model, obj, target);//set blp model
                model->get_Nx(N);
                build_gfa();
                //lp->externalise(externals, true);
                //(*this.*ptr_solve)(MtxLP* mlp);
                //wgni(mlp);
                wgni2(mlp);
                //lp->externalise(externals, false);
                *out << ENDOFRECORD << endl << endl;
                if (write) lp->writeLP("GLPKmodel.LP");
            }
            //}
        }
        else if(problem == "dpaplot"){ //w/dpa/<DPA producibility plot>
            max=1;
            dpa->gene_koreacs(model);
            //for (strvec::iterator it = records->begin(); it != records->end(); ++it){
            if (!records->empty())
                read_record_gfa(this, (*records)[0]);//set assigned externals
            del_exts();
            model->get_Nx(N);
            build_gfa();
            //lp->externalise(externals, true);
            dpaplot();
            //lp->externalise(externals, false);
            *out << ENDOFRECORD << endl << endl;
            if (write) lp->writeLP("GLPKmodel.LP");
            //}
        }
        else if(OTHPROB.find(problem)!=OTHPROB.end()){ //----------------other problem----------------------------------------
            model->get_Nx(N);
            build();            
            for (strvec::iterator it = records->begin(); it != records->end(); ++it){
                if (read_record(this, *it)){
                    //if (withcomments) *out << COMMENT << *obj << TAB << args << endl;
                    lp->update(); //get back tmp_externals             
                    (*this.*ptr_solve)();
                    *out << ENDOFRECORD << endl << endl;
                    if (write)
                        lp->writeLP("GLPKmodel.LP");
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
void Driver::objval(){
    *out << lp->getOptVal(obj, max, presolve);
}

void Driver::objstat(){
    OBJSTAT objstat;
    lp->getOptObjStat(objstat, obj, max, presolve);
    *out << objstat;
}

void Driver::fba(){
    stomap* sol = new stomap;
    lp->getOptSol(sol, obj, max, (kind == MILP) ? INT : CONT, withzeroes, presolve);
    if (withcomments) *out << COMMENT << *lp << ' ' << '(' << *obj << ')' << endl;
    if(problem=="gimme")//w/gimme
        print_fba_state(out, sol, Rstate, model, withcomments);
    else
        model->print_sto(sol, out, withcomments);
    delete sol;    
}

void Driver::ko(){
    objstatvec ovec;
    strlistvec qlists, blocklists;
    get_list_args(qlists, args);
    //    for(int i=0;i<qlists.size();i++) {//t/
    //        strvec list=qlists[i];
    //        for(int i=0;i<list.size();i++) cout<<list[i]<<endl;//t/
    //        cout<<endl;
    //    }//t/
    model->reaclists_to_block(blocklists, qlists);
    //    for(int i=0;i<blocklists.size();i++) {//t/
    //        strvec list=blocklists[i];
    //        for(int i=0;i<list.size();i++) cout<<list[i]<<endl;//t/
    //        cout<<endl;
    //    }//t/
    //    for (stomap::iterator it = obj->begin(); it != obj->end(); ++it){//t/
    //        cout<<it->first<<':'<<it->second<<endl;
    //    }
    lp->ko(ovec, obj, blocklists, max, presolve);
    for (int i = 0; i < ovec.size(); i++){
        *out << qlists[i] << TAB << ovec[i];
        if (withcomments) *out << TAB << COMMENT << blocklists[i];
        *out << endl;
    }
}

//void Driver::fva(){
//    args = get_args(args);
//    va();
//}

void Driver::fva(){//w/va->fva
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    args = get_args(args);
    if (obj->size() > 0)
        lp->getOptRanges(rm, objrange, obj, max, args, presolve);
    else
        lp->getRanges(rm, args, presolve);
    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    //print_rangemap(out, rm, model);
    print_rangemap2(out, rm, model, Rstate, withcomments, false);
    delete rm;
}

void Driver::fva_gim3e(double value){//w/gim3e

    rangemap *rm = new rangemap;
    args = get_args_gfva(args);
    lp->getOptRanges_gim3e(rm, obj, value, args, presolve);

    //if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    //print_rangemap(out, rm, model);
    print_rangemap2(out, rm, model, Rstate, withcomments, false);
    //print_rangemap2_tab(out, rm, model, Rstate, withcomments, false);
    delete rm;
}

void Driver::fsa(){
    double const1, const2;
    stomap *rv = new stomap;

    if (args.size() < 2) throw runtime_error("invalid number of arguments, must be at least two!");
    strvec::iterator it = args.begin();
    const1 = str2double(*it);
    it = args.erase(it);
    const2 = str2double(*it);
    it = args.erase(it);
    args = get_args(args);

    lp->getShifts(rv, obj, const1, const2, args, presolve);
    if (withcomments) *out << COMMENT << *obj << endl;
    model->print_sto(rv, out, withcomments);
    delete rv;
}

void Driver::plot(){
    if (args.size() != 4) throw runtime_error("invalid list of arguments, must be 'query from to step'!");
    pointvec *plot = new pointvec;
    strvec::const_iterator it = args.begin();
    PLOT_ARGS yargs;
    parse_plot_args(yargs, it);
    lp->plot(plot, obj, yargs, max, presolve);
    if (withcomments) *out << COMMENT << yargs.query << SPACE << TAB << *obj << endl;
    *out << *plot;
    delete plot;
}

void Driver::plot3D(){
    if (args.size() != 8) throw runtime_error("invalid list of arguments, must be 'query from to step query from to step'!");
    pointvec *plot = new pointvec;
    strvec::const_iterator it = args.begin();
    PLOT_ARGS xargs;
    parse_plot_args(xargs, it);
    PLOT_ARGS yargs;
    parse_plot_args(yargs, it);
    lp->plot3D(plot, obj, xargs, yargs, max, presolve);
    if (withcomments) *out << COMMENT << xargs.query << SPACE << TAB << yargs.query << SPACE << TAB << *obj << endl;
    *out << *plot;
    delete plot;
}

void Driver::uncons(){
    strvec uncons = lp->unconsRows(presolve);
    if (withcomments) *out << COMMENT << uncons.size() << " unconserved metabolites." << endl;
    model->print_vec(uncons, out, withcomments);
}

void Driver::elmode(){
    if (kind == MILP) vmax = VMAX;
    long num = INT_MAX;
    switch(args.size()){
    case 0: case 1: break;
    case 3:
        vmax = str2double(args[2]);
        if (vmax < 1)
            throw runtime_error(err_msg + "vmax must be >= 1");
        lp->setVMax(vmax);
        args.pop_back();
    case 2:
        num = (int)str2double(args[1]);
        if (num < 0) num = INT_MAX;
        args.pop_back();
        break;
    default:
        throw runtime_error(err_msg + "invalid number of arguments, must be 0, 1 or 2!");
    }
    solvec solutions;
    int n = 1;
    ((SplitLP*)lp)->elModes(solutions, num, args);
    for (solvec::iterator it = solutions.begin(); it != solutions.end(); ++it){
        stomap sol = *it;
        if (withcomments) *out << COMMENT << n++ << ": " << sol.size() << " reactions" << " (" << args << "):" << endl;
        model->print_sto(&sol, out, withcomments);
        *out << endl;
    }
}

void Driver::live(){
    args = get_args(args);
    strvec lives = lp->getUnblockedVars(args, presolve);
    if (withcomments) *out << COMMENT << lives.size() << " live reactions." << endl;
    model->print_vec(lives, out, withcomments);
}

void Driver::essentials(){
    args = get_args(args);
    strvec essentials = lp->getEssentials(obj, args, max, presolve);
    if (withcomments) *out << COMMENT << essentials.size() << " essential reactions." << endl;
    model->print_vec(essentials, out, withcomments);
}

void Driver::mss(){
    mxs(-1);
}

void Driver::mps(){
    mxs(1);
}

void Driver::mxs(int dir){
    solvec solutions;
    double vmax = 1;
    long nmax = LONG_MAX;
    if (args.size() > 1)
        vmax = str2double(args[1]);
    if (args.size() > 0){
        nmax = (long)str2double(args[0]);
        if (nmax < 0) nmax = LONG_MAX;
    }
    lp->setVMax(vmax);
    ((SplitLP*)lp)->minExchangeSets(solutions, obj, dir, nmax);
    int n = 1;
    string sres = dir < 0 ? "substrates" : "products";
    for (solvec::iterator it = solutions.begin(); it != solutions.end(); ++it){
        stomap sol = *it;
        int npos = 0;
        for (stomap::iterator jt = sol.begin(); jt != sol.end(); ++jt )
            if (jt->second / dir > 0)
                npos++;
        if (withcomments) *out << COMMENT << n++ << ": " << npos << " " << sres << " (" << *obj << "):" << endl;
        model->print_conv(&sol, out);
        *out << endl;
    }
}

void Driver::bottlenecks(){
    lp->optimise(obj, max, presolve);
    double objval = lp->getObjValue();
    if (withcomments) *out << COMMENT << "result: " << objval << endl;
    strvec bottlenecks;
    if (objval == 0){
        args = get_args(args);
        bottlenecks = lp->getBottleNecks(obj, args, max, presolve);
    }
    if (withcomments){
        string s = thermo ? " thermodynamic " : " directional ";
        *out << COMMENT << bottlenecks.size() << s << "bottlenecks." << endl;
    }       
    model->print_vec(bottlenecks, out, withcomments);
}

void Driver::test(){
}

void Driver::tradeoff(){ //Method added by Zakrzewski
    if(args.size() < 1 )throw runtime_error("invalid list of arguments. At least one arguments required (the second objective)");
    if(args.size() > 2 )throw runtime_error("Too many arguments provided");
    string secondObj  =  args[0];
    //cout << "seoncdObj: " << secondObj;
    double resolution = 100;
    if(args.size() ==2) resolution = str2double(args[1]);
    if(resolution <= 0)throw runtime_error("Resolution must be higher than zero");
    stomap* sol = new stomap;
    lp->getOptSol(sol, obj, max, (kind == MILP) ? INT : CONT, withzeroes, presolve);
    double maximalFirstObj =lp->getObjValue();
    //string firstObjName = target;// Provisional - it will not parse weighted objective correctly
    string firstObjName = obj->begin()->first;
    set_objective(secondObj);    
    delete sol;
    //cout << "\n" << "maxFirst Obj: " << maximalFirstObj;
    pointvec *plot = new pointvec;
    PLOT_ARGS yargs;
    yargs.query = firstObjName;
    yargs.from  =  0;
    yargs.to    = maximalFirstObj;
    yargs.step  = maximalFirstObj/resolution;
    lp->plot(plot, obj, yargs, max, presolve);
    if (withcomments) *out << COMMENT << yargs.query << SPACE << TAB << *obj << endl;
    *out << *plot;
    delete plot;
}

//void Driver::tmfa(){
//    stomap* sol = new stomap;
//    const int ntypes = 4;
//    int types[ntypes] = {CONT, LNCONC_TYPE, DELTA_TYPE, INT};
//    string typenames[ntypes] = {"fluxes", "concentrations", "delta G-s", "integers"};
//    lp->optimise(obj, max, presolve);
//    if (withcomments) *out << COMMENT << *lp << ' ' << '(' << *obj << ')' << endl;
//    for (int i = 0; i < ntypes - 1; ++i){
//        lp->getSolution(sol, types[i], bool(i));
//        if (withcomments) *out << COMMENT << sol->size() << " " << typenames[i]  << " (" << *obj << "):" << endl;
//        model->print_sto(sol, out, !bool(i));
//        *out << endl;
//        sol->clear();
//    }
//    delete sol;
//}

//void Driver::tva(){
//    args = get_args(args, LNCONC_TYPE);
//    va();
//}

void Driver::gfaa(){//w/imat method
    strvec reacs, genes;
    //    get_args_gfa(reacs, genes);
    gfa->get_args_gfa(model, args, reacs, genes);
    //---------------------flux activity for reactions-----------------------
    reac_rangemap *Rrm = new reac_rangemap;
    for(int i=0;i<reacs.size();i++){// compute flux activity and direction for reactions
        string Rn=reacs[i];
        (*Rrm)[Rn]=gfa->compute_reac_fluxact(Rn, model, lp, obj, presolve);
    }

    //---------------------flux activity for genes---------------------------
    gene_rangemap *Grm = new gene_rangemap;
    for(int i=0;i<genes.size();i++){
        string Gn=genes[i];
        strvec Rs=gfa->get_geneReacs(Gn);
        (*Grm)[Gn]=gfa->compute_gene_fluxact(Rrm, Rs, model, lp, obj, presolve);
    }
    //    print_rangemap_gfaa(out, Rrm, Grm, reacs, model, gfa, arr, Rstate);
    print_rangemap_gfaa_tab(out, Rrm, Grm, reacs, model, gfa, arr, Rstate);
}

void Driver::gfva(){//w/gfa/
    max=GLP_MAX;
    args = get_args_gfva(args);
    //    for(int i=0;i<args.size();i++) cout<<args[i]<<endl;//tst/
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    if (obj->size() > 0){
        lp->getOptRanges_gfa(rm, objrange, obj, args, presolve);
    }else
        lp->getRanges(rm, args, presolve);
    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    print_rangemap_gfva(out, rm, model, Rstate, withcomments);
    delete rm;
}

void Driver::gfva2(){//w/
    max=GLP_MAX;
    args = get_args_gfva(args);
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    stomap* sol = new stomap;
    if (obj->size() > 0)
        lp->getOptRanges_gfa2(sol, rm, objrange, obj, args, presolve);
    else
        lp->getRanges(rm, args, presolve);
    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    print_rangemap2_tab(out, rm, model, Rstate, withcomments);
    delete rm;
    delete sol;
}

void Driver::gfva3(){//w/ split reversible reacs and maximize the objective func using SIMPLEX solver
    args = get_args(args);
    rangemap *rm = new rangemap;
    OBJSTAT objrange;
    stomap* sol = new stomap;
    if (obj->size() > 0)
        lp->getOptRanges_gfa3(sol, rm, objrange, obj, args, max, presolve);
    else
        lp->getRanges(rm, args, presolve);
    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
    print_rangemap2_tab(out, rm, model, Rstate, withcomments);
    delete rm;
    delete sol;
}

void Driver::sgni(){//w/gni/ strong gni
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
        //        kobds ko_rc;
        stomap ko_obj;
        //        printSystem(lp);//t/
        //        gni->ko_blp(lp, obj, rkos, ko_rc, ko_obj);
        gni->ko_blp(lp, obj, rkos, ko_obj);
        //        printSystem(lp);//t/
        BLPSOL bsol;
        stomap medium;//growth medium
        blp_fba(lp, bsol, medium);
        sgniOut[Gn].bsol=bsol;
        sgniOut[Gn].medium=medium;        
        //-----------------------------------------------------
        if(bsol.status=="OPTIMAL" && bsol.level>0.2){//if the gene is essential under the growth medium
            for (stomap::iterator it = medium.begin(); it != medium.end(); ++it){
                string nt=it->first;
                string ce="ce__"+nt;
                int ceval=it->second;
                if(ceval==0){//judging if GNI-SN (Strong Negative)
                    lp->setColBnds(ce, 1,1);
                    blp_fba(lp, bsol);
                    if(bsol.status!="OPTIMAL" || bsol.level<0.2)
                        sgniOut[Gn].sn.push_back(nt);
                    //                    blp->setColBnds(ce, 0,1);
                    lp->setColKind(ce, INT);
                }
                else if(ceval==1){//judging if GNI-SP (Strong Positive)
                    lp->setColBnds(ce, 0,0);
                    blp_fba(lp, bsol);
                    if(bsol.status!="OPTIMAL" || bsol.level<0.2)
                        sgniOut[Gn].sp.push_back(nt);
                    //                    blp->setColBnds(ce, 0,1);
                    lp->setColKind(ce, INT);
                }
            }
        }
        //        gni->rko_blp(lp, obj, ko_rc, ko_obj);
        gni->rko_blp(lp, obj, ko_obj);
    }
    print_sgni(out, sgniOut);
}

void Driver::wgni(MtxLP* mlp){//w/gni/ weak gni using glpk
    gene_wgni1 wgniOut1;
    stomap *mobj = new stomap;
    (*mobj)[target]=1;
    objstatvec wtbm;//BIOMASS fba for wild type over all rmeds
    randmeds rmeds=gni->rand_meds();
    for(int s=0;s<rmeds.size();s++){
        intvec rmed=rmeds[s];
        OBJSTAT solwt=fba_med(mlp,rmed,mobj);
        wtbm.push_back(solwt);
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
        //        kobds ko_rc;
        stomap ko_obj;
        //        printSystem(lp);//t/
        //        gni->ko_blp(lp, obj, rkos, ko_rc, ko_obj);
        gni->ko_blp(lp, obj, rkos, ko_obj);

        BLPSOL bsol;
        stomap medium;//growth medium
        kind=MILP;
        blp_fba(lp, bsol, medium);
        if(bsol.status=="OPTIMAL" && bsol.level>0.2){//if the gene is essential under a growth medium
            WGNI1 wgni;
            wgni.Gn=Gn;
            wgni.rkos=rkos;
            wgni.Ness=0;

            kobds mko_rc;
            gni->ko_mlp(mlp, rkos, mko_rc);

            for(int s=0;s<rmeds.size();s++){
                intvec rmed=rmeds[s];
                OBJSTAT solko=fba_med(mlp,rmed,mobj);
                if(wtbm[s].status==GLP_OPT && solko.status==GLP_OPT){
                    double level;
                    if(wtbm[s].objval-solko.objval<1e-7) level=0;
                    else level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
                    if(level>=0.2){
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
            gni->rko_mlp(mlp, mko_rc);

        }
        //        gni->rko_blp(lp, obj, ko_rc, ko_obj);
        gni->rko_blp(lp, obj, ko_obj);
    }//-------------------------------------------------------------------------------
    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
    print_wgni1(out, wgniOut1, rmeds.size());
}

void Driver::wgni2(MtxLP* mlp){//w/gni/ weak gni using glpk
    gene_wgni1 wgniOut1;
    stomap *mobj = new stomap;
    (*mobj)[target]=1;
    objstatvec wtbm;//BIOMASS fba for wild type over all rmeds
    cout<<"Sampling media: ";
    randmeds rmeds=gni->rand_meds();
    cout<<rmeds.size()<<endl;
    cout<<"Computing wild type biomass"<<endl;
    for(int s=0;s<rmeds.size();s++){
        intvec rmed=rmeds[s];
        OBJSTAT solwt=fba_med(mlp,rmed,mobj);
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

        kobds mko_rc;
        gni->ko_mlp(mlp, rkos, mko_rc);

        for(int s=0;s<rmeds.size();s++){//BIOMASS fba for ko type of gene Gn over all rmeds
            intvec rmed=rmeds[s];
            OBJSTAT solko=fba_med(mlp,rmed,mobj);
            if(wtbm[s].status==GLP_OPT && solko.status==GLP_OPT){
                double level;
                if(wtbm[s].objval-solko.objval<1e-7) level=0;
                else level=(wtbm[s].objval-solko.objval)/wtbm[s].objval;
                if(level>=0.2){
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
        gni->rko_mlp(mlp, mko_rc);
        cout<<"\r"<<i+1<<flush;
    }
    //-------------------------------------------------------------------------------
    sort(wgniOut1.begin(), wgniOut1.end(), myfunc_wgni);
    print_wgni1(out, wgniOut1, rmeds.size());
}


void Driver::dpaplot(){//w/dpa/compute producibility plot using glpk
    strstr2flo plotMatrix;//w/dpa/ metabolite and gene to producibility of wildtype and knockout
    objstatvec solwt;//fba for wild type over all metabolites
    strvec mets;
    if(args.size()>0){
        mets=args;
    }else mets=lp->getRowNames();
    for(int m=0;m<mets.size();m++){
        string met=mets[m];
        OBJSTAT stat;
        obj->clear();
        (*obj)[met]=1;
        lp->getOptObjStat(stat, obj, max, presolve); //where met will be externalise
        solwt.push_back(stat);
        plotMatrix[met]["WildType"]=stat.objval;
        lp->update();//internalise the met
    }
    //--------------------------------------------------------
    met_genes metG;
    gene_mets geneM;
    strvec gns=dpa->genes;
    for(int i=0;i<gns.size();i++){
        string Gn=gns[i];
        strvec rkos=dpa->getRkos(Gn);
        if(rkos.size()<1) continue;
        geneM[Gn].rkos=rkos;
        kobds mko_rc;
        dpa->ko_mlp(lp, rkos, mko_rc);

        for(int m=0;m<mets.size();m++){
            string met=mets[m];
            OBJSTAT solko;
            obj->clear();
            (*obj)[met]=1;
            lp->getOptObjStat(solko, obj, max, presolve);
            if(solwt[m].status==GLP_OPT && solko.status==GLP_OPT){
                double Df;
                Df=abs(solwt[m].objval-solko.objval);
                plotMatrix[met][Gn]=solko.objval;
                if(Df>0.001) {
                    metG[met].push_back(Gn);
                    geneM[Gn].mets.push_back(met);
                }
            }
            lp->update();
        }
        dpa->rko_mlp(lp, mko_rc);
    }//--------------------------------------------------------

    print_dpaplot(out, metG, geneM, plotMatrix, mets, gns);
}

void Driver::dpasig(){//w/dpa/compute metabolite signals using glpk
    metarr_sig metsig;
    dpa->compute_signal(arrs, metsig);
    print_dpasig(out, dpa->mets, arrs, metsig);
}

void Driver::blp_fba(MtxLP* blp, BLPSOL &bsol, stomap &medium){//w/gni/
    stomap* sol = new stomap;//-------------------------------------
    blp->getOptSol(sol, obj, max, ALLKIND, true, presolve);//w/withzeroes=true;
    bsol.drop=blp->getObjValue();
    bsol.status=get_msg(blp->getStatus());
    bsol.bm=(*sol)[target];
    if(bsol.drop<1e-7) bsol.level=0;
    else bsol.level=bsol.drop/bsol.bm;//growth drop percentage over BIOMASS of wild
    for(int i=0;i<gni->nts.size();i++){
        string nt=gni->nts[i];
        string ce="ce__"+nt;
        medium[nt]=(*sol)[ce];
    }
    //    model->print_sto(sol, out, withcommegni->nts);//t/
    delete sol;
}

void Driver::blp_fba(MtxLP* blp, BLPSOL &bsol){//w/gni/
    stomap* sol = new stomap;//-------------------------------------
    blp->getOptSol(sol, obj, max, ALLKIND, true, presolve);//w/withzeroes=true;
    bsol.drop=blp->getObjValue();
    bsol.status=get_msg(blp->getStatus());
    bsol.bm=(*sol)[target];
    if(bsol.drop<1e-7) bsol.level=0;
    else bsol.level=bsol.drop/bsol.bm;//growth drop percentage over BIOMASS of wild type
    delete sol;
}

OBJSTAT Driver::fba_med(MtxLP* mlp, intvec rmed, stomap *mobj){//w/gni/
    for(int i=0;i<rmed.size();i++){
        string nt=gni->nts[i];
        bool abs=(rmed[i]==0)?true:false;
        gni->set_nt_abs(nt, model, mlp, abs);
    }
    OBJSTAT msol;
    kind=SMPLX;
    stomap* sol = new stomap;//-------------------------------------
    mlp->getOptSol(sol, mobj, max, (kind == MILP) ? INT : CONT, true, presolve);//w/withzeroes=true;
    msol.objval=mlp->getObjValue();
    msol.status=mlp->getStatus();
    //    model->print_sto(sol, out, withcommegni->nts);//t/
    delete sol;
    return msol;
}

//w/gfa/ get reactions and genes of current model (after trim)
void Driver::get_args_gfa(strvec &reacs, strvec &genes){
    //    strvec Reactions = lp->getColNames(); //get reactions of current model
    strvec Reactions = model->get_reactions(); //get reactions of current model
    argmap Rmap,Gmap;
    for(int i=0;i<Reactions.size();i++)
        Rmap[Reactions[i]]=1;

    strvec Genes; //get genes which associate current reactions

    for(int i=0;i<Reactions.size();i++){
        string Rn=Reactions[i];
        Reaction* reac=model->get_reac(Rn);
        strvec* Rule=reac->get_rule();
        for(int j=0;j<Rule->size();j++){
            string Gn=(*Rule)[j];
            if(Gn!=AND && Gn!=OR){
                if(gfa->exist_gene(Gn)){
                    gfa->get_geneReacs(Gn).push_back(Rn);
                    if(Gmap.find(Gn)==Gmap.end()){
                        Gmap[Gn]=1;
                        Genes.push_back(Gn);
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
        reacs=Reactions;
        genes=Genes;
    }
}

//w/gni/ delete externals from stos of reacs
void Driver::del_exts(){
    for(int i=0;i<externals.size();i++){
        string En=externals[i];
        Element* ext=model->find_element(En);
        stomap* sto=ext->get_sto();
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string Rn=it->first;
            Reaction* reac=model->get_reac(Rn);
            //            Element* reac=model->find_element(Rn);
            stomap* stoR=reac->get_sto();
            stoR->erase(En);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//output
ostream &operator << (ostream &out, OBJSTAT objstat){
    double objval=(abs(objstat.objval)<1e-7)?0:objstat.objval;//w/
    out << objval << COLON << SPACE << Driver::get_msg(objstat.status);//w/
    return out;
}

ostream &operator << (ostream &out, OBJSTAT_RANGE range){
    out << range.min << TAB << range.max;
    return out;
}

ostream &operator << (ostream &out, rangemap &rm){
    for (rangemap::iterator it = rm.begin(); it != rm.end(); ++it)
        out << it->first << TAB << it->second << endl;
    return out;
}

void print_rangemap(ostream *out, rangemap *rm, Model* model){
    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it){
        string name = it->first;
        if (name[0] == '-') name = name.substr(1);
        if (name[0] == '[') name = name.substr(1, name.size() -2);
        *out << it->first << TAB << it->second << TAB << COMMENT << model->find_element(name)->get_info() << endl;
    }
}

void print_rangemap_gfva(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments){//w/gfa/
    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it){
        string name = it->first;
        double state=(*Rstate)[name];
        if (name[0] == '-') name = name.substr(1);
        if (name[0] == '[') name = name.substr(1, name.size() -2);
        *out << it->first << TAB << it->second << TAB <<"STATE = "<<state;
        if (withcomments)
            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
        *out << endl;
    }
}

//void print_rangemap_gfva2(ostream *out, rangemap *rm, stomap *allsol, Model* model, flomap* Rstate, bool withcomments){//w/gfa/
//    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it){
//        string name = it->first;
//        double state=(*Rstate)[name];
//        if (name[0] == '-')name = name.substr(1);
//        if (name[0] == '[')name = name.substr(1, name.size() -2);
//        *out << it->first << TAB << it->second << TAB <<"STATE = "<<state;
//		if (withcomments)
//            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
//        *out << endl;
//    }
//	*out <<TABEND<<endl<<endl;
//	for (stomap::const_iterator it = allsol->begin(); it != allsol->end(); ++it){
//        *out << it->first << TAB << it->second;
//        if (withcomments)
//            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
//        *out << endl;
//    }
//}

void print_fba_state(ostream* out, stomap* sto, flomap* Rstate, Model* model, bool withcomments){
    for (stomap::const_iterator it = sto->begin(); it != sto->end(); ++it){
        string name = it->first;
        double state=(*Rstate)[name];
        (*out) << it->first << TAB << it->second<< TAB <<state;;
        if (withcomments)
            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
        (*out) << endl;
    }
}

void print_rangemap2(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments, bool state){//w/gfa/
    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it){
        string activity="UND";
        string name = it->first;
        OBJSTAT_RANGE range=it->second;
        double min=range.min.objval;
        double max=range.max.objval;
        if(min>1e-7 || max<-1e-7) activity="ACTIVE";
        else if(min>-1e-7 && max<1e-7) activity="INACT";
        double state=(*Rstate)[name];
        if (name[0] == '-') name = name.substr(1);
        if (name[0] == '[') name = name.substr(1, name.size() -2);
        *out << it->first << TAB << it->second << TAB <<activity;
        if(state)  *out << TAB <<state;
        if (withcomments)
            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
        *out << endl;
    }
}

void print_rangemap2_tab(ostream *out, rangemap *rm, Model* model, flomap* Rstate, bool withcomments, bool state){//w/gfa/
    if(state) *out <<"Reac(direc)"<<TAB<<"Minimum"<<TAB<<"Maximum"<<TAB<<"Activity"<<TAB<<"State"<<TAB<<"Comment"<<endl;
    else *out <<"Reac(direc)"<<TAB<<"Minimum"<<TAB<<"Maximum"<<TAB<<"Activity"<<TAB<<"Comment"<<endl;
    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it){
        int activity=0;
        string name = it->first;
        OBJSTAT_RANGE range=it->second;
        double min=range.min.objval;
        double max=range.max.objval;
        if(min>1e-7 || max<-1e-7) activity=1;
        else if(min>-1e-7 && max<1e-7) activity=-1;
        double rstate=(*Rstate)[name];
        if (name[0] == '-') name = name.substr(1);
        if (name[0] == '[') name = name.substr(1, name.size() -2);
        *out << it->first << TAB << it->second << TAB <<activity;
        if(state)  *out << TAB <<rstate;
        if (withcomments)
            (*out) << TAB << COMMENT << model->find_element(it->first)->get_info();
        *out << endl;
    }
}

ostream &operator << (ostream &out, MtxLP &lp){
    OBJSTAT objstat;
    lp.getObjStat(objstat);
    out << objstat;
    return out;
}

ostream &operator << (ostream &out, FLUXACT activity){//w/gfa/
    out <<"ACT = "<< activity.act << TAB <<"INACT = "<< activity.inact<< TAB <<"ACT_LEV = "<<activity.level;
    return out;
}

//w/gfa/ print reaction activity, its directionality, and flux activity for genes
void print_rangemap_gfaa(ostream *out, reac_rangemap *Rrm, gene_rangemap *Grm, strvec reacs, Model* model, GFA* gfa, string arr, flomap* Rstate){
    *out <<"Flux-activity state for reactions:"<<endl;
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        reac_rangemap::iterator it=Rrm->find(Rn);
        if(it!=(*Rrm).end()){
            double state=(*Rstate)[Rn];
            FLUXDIR flux=it->second;
            if(flux.direc==0){
                *out << Rn <<" (+)"<< TAB << flux.dirp << TAB <<"STATE = "<<state<< TAB << COMMENT << model->find_element(Rn)->get_info() << endl;
                *out << Rn <<" (-)"<< TAB << flux.dirm << TAB <<"STATE = "<<state<< TAB << COMMENT << model->find_element(Rn)->get_info() << endl;
            }
            else if(flux.direc==1){
                *out << Rn <<" (+)"<< TAB << flux.dirp << TAB <<"STATE = "<<state<< TAB << COMMENT << model->find_element(Rn)->get_info() << endl;
            }
            else if(flux.direc==-1){
                *out << Rn <<" (-)"<< TAB << flux.dirm << TAB <<"STATE = "<<state<< TAB << COMMENT << model->find_element(Rn)->get_info() << endl;
            }
        }
    }
    *out <<endl<<"Flux-activity state for genes:"<<endl;
    for (gene_rangemap::iterator it = Grm->begin(); it != Grm->end(); ++it){
        string Gn = it->first;
        FLUXACT gact=it->second;
        int gexp=gfa->get_gene_exp(Gn, arr);
        *out << Gn << TAB << gact << TAB <<"EXP_LEV = "<<gexp<< TAB <<"POST_REGULATION = ";
        if(gact.level>0 && gexp<=0) *out<<"up"<<endl;
        else if(gact.level<0 && gexp>=0) *out<<"down"<<endl;
        else *out<<"no"<<endl;
    }
}

//w/gfa/ print reaction activity, its directionality, and flux activity for genes
void print_rangemap_gfaa_tab(ostream *out, reac_rangemap *Rrm, gene_rangemap *Grm, strvec reacs, Model* model, GFA* gfa, string arr, flomap* Rstate){
    *out <<"Flux-activity analysis results for expression profile of "<<arr<<endl<<endl;
    *out <<"Flux-activity state for reactions:"<<endl;
    *out <<"Reac(direc)"<<TAB<<"Active"<<TAB<<"Inactive"<<TAB<<"ACT_LEV"<<TAB<<"State"<<TAB<<"Comment"<<endl;
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        reac_rangemap::iterator it=Rrm->find(Rn);
        if(it!=(*Rrm).end()){
            double state=(*Rstate)[Rn];
            FLUXDIR flux=it->second;
            if(flux.direc==0){
                *out << Rn <<" (+)"<<TAB<<flux.dirp.act<<TAB<<flux.dirp.inact<<TAB<<flux.dirp.level<<TAB<<state<< TAB<< COMMENT <<model->find_element(Rn)->get_info() << endl;
                *out << Rn <<" (-)"<<TAB<<flux.dirm.act<<TAB<<flux.dirm.inact<<TAB<<flux.dirm.level<<TAB<<state<< TAB<< COMMENT <<model->find_element(Rn)->get_info() << endl;
            }
            else if(flux.direc==1){
                *out << Rn <<" (+)"<<TAB<<flux.dirp.act<<TAB<<flux.dirp.inact<<TAB<<flux.dirp.level<<TAB<<state<< TAB<< COMMENT <<model->find_element(Rn)->get_info() << endl;
            }
            else if(flux.direc==-1){
                *out << Rn <<" (-)"<<TAB<<flux.dirm.act<<TAB<<flux.dirm.inact<<TAB<<flux.dirm.level<<TAB<<state<< TAB<< COMMENT <<model->find_element(Rn)->get_info() << endl;
            }
        }
    }
    *out <<TABEND<<endl<<endl;
    *out <<"Flux-activity state for genes:"<<endl;
    //    *out <<"Number of genes: "<<Grm->size()<<endl;//t/
    *out <<"Gene"<<TAB<<"Active"<<TAB<<"Inactive"<<TAB<<"ACT_LEV"<<TAB<<"EXP_LEV"<<TAB<<"Postregulation"<<endl;

    for (gene_rangemap::iterator it = Grm->begin(); it != Grm->end(); ++it){
        string Gn = it->first;
        FLUXACT gact=it->second;
        int gexp=gfa->get_gene_exp(Gn, arr);
        *out <<Gn<<TAB<<gact.act<<TAB<<gact.inact<<TAB<<gact.level<<TAB<<gexp<<TAB;
        if(gact.level>0 && gexp<=0) *out<<"up"<<endl;
        else if(gact.level<0 && gexp>=0) *out<<"down"<<endl;
        else *out<<"no"<<endl;
    }
}

void print_sgni(ostream *out, gene_sgni &sgniOut){//w/gni/
    for (gene_sgni::iterator it = sgniOut.begin(); it != sgniOut.end(); ++it){
        string Gn=it->first;
        SGNI gni=it->second;
        BLPSOL bsol=gni.bsol;
        stomap medium=gni.medium;
        *out <<"Gene:"<<TAB<<Gn<<endl;
        *out <<"KO Reactions:";
        for(int i=0;i<gni.rkos.size();i++) *out <<TAB<<gni.rkos[i];
        *out <<endl;
        *out <<"BLP Result:"<<TAB<<"Drop = "<<bsol.drop<<":"<<bsol.status<<TAB<<"Level = "<<bsol.level<<endl;
        *out <<"Growth Medium:"<<endl;
        for (stomap::iterator it = medium.begin(); it != medium.end(); ++it){
            *out <<it->first<<" : "<<it->second<<endl;
        }
        if(!gni.sp.empty()){
            *out <<"Nutrients of GNI-SP:";
            for(int i=0;i<gni.sp.size();i++) *out <<TAB<<gni.sp[i];
            *out <<endl;
        }
        if(!gni.sn.empty()){
            *out <<"Nutrients of GNI-SN:";
            for(int i=0;i<gni.sn.size();i++) *out <<TAB<<gni.sn[i];
            *out <<endl;
        }
        *out <<endl;
    }
}

//void print_wgni(ostream *out, gene_wgni &wgniOut, strvec nts){//w/gni/
//    for (gene_wgni::iterator it = wgniOut.begin(); it != wgniOut.end(); ++it){
//         string Gn=it->first;
//         WGNI gni=it->second;
//         nt_pval ntp=gni.nts;
//         *out <<"Gene:"<<TAB<<Gn<<endl;
//         *out <<"KO Reactions:";
//         for(int i=0;i<gni.rkos.size();i++) *out <<TAB<<gni.rkos[i];
//         *out <<endl;
//         *out <<"Nutrients:"<<endl;
//         int size=nts.size();
//         unsigned int M=pow(2,size);
//         for(int i=0;i<size;i++){
//              string nt=nts[i];
//              int N=ntp[nt].Np+ntp[nt].Nn;
////              double Pp=pval_bin(N,0.5,ntp[nt].Np);
////              double Pn=pval_bin(N,0.5,ntp[nt].Nn);
////              double Pp=alglib::binomialcdistribution(ntp[nt].Np-1, N, 0.5);
////              double Pn=alglib::binomialcdistribution(ntp[nt].Nn-1, N, 0.5);
//              double Pp=hypergeometric_cdf(ntp[nt].Np, M/2, M/2, N);
//              double Pn=hypergeometric_cdf(ntp[nt].Nn, M/2, M/2, N);
//              *out <<nt<<TAB<<"GNI-WP: "<<"Number = "<<ntp[nt].Np<<", "<<"Pvalue = "<<Pp<<TAB<<"GNI-WN: "<<"Number = "<<ntp[nt].Nn<<", "<<"Pvalue = "<<Pn<<endl;
//         }
//         *out <<endl;
//    }
//}


void print_wgni1(ostream *out, gene_wgni1 &wgniOut1, int Nsam){//w/gni/
    *out <<"The analysis results of weak GNI with sample size of "<<Nsam<<endl<<endl;
    //    for (gene_wgni1::iterator it = wgniOut1.begin(); it != wgniOut1.end(); ++it){
    for(int i=0;i<wgniOut1.size();i++){
        WGNI1 wgni=wgniOut1[i];
        if(wgni.Ness==0) continue;//donot output if Ness==0
        nt_pval ntout=wgni.nts;
        *out <<"Gene:"<<TAB<<wgni.Gn<<endl;
        *out <<"KO Reactions:";
        for(int i=0;i<wgni.rkos.size();i++) *out <<TAB<<wgni.rkos[i];
        *out <<endl;
        *out <<"Number of Essential medium = "<<wgni.Ness<<endl;
        *out <<"Nutrients:"<<endl;
        for (nt_pval::iterator it = ntout.begin(); it != ntout.end(); ++it){
            //              string nt=nts[i];
            string nt=it->first;
            NTPVAL stat=it->second;
            *out <<nt<<TAB<<"GNI-WP: "<<"Number = "<<stat.Np<<TAB<<"GNI-WN: "<<"Number = "<<stat.Nn<<endl;
        }
        *out <<endl;
    }

}

void print_essg(ostream *out, ess_gene &essgOut, strvec nts){//w/gni/
    *out <<"The analysis results of essential genes with following nutrients: "<<endl;
    for(int i=0;i<nts.size();i++)  *out <<nts[i]<<TAB;
    *out <<endl<<endl;
    for(int i=0;i<essgOut.size();i++){
        ESSG essg=essgOut[i];
        *out <<"Gene:"<<TAB<<essg.Gn<<endl;
        *out <<"KO Reactions:";
        for(int i=0;i<essg.rkos.size();i++) *out <<TAB<<essg.rkos[i];
        *out <<endl;
        *out <<"Result:"<<TAB<<"Drop = "<<essg.drop<<TAB<<"Level = "<<essg.level<<endl;
        *out <<endl;
    }
    *out <<"List of essential genes: "<<endl;
    for(int i=0;i<essgOut.size();i++){
        ESSG essg=essgOut[i];
        *out <<essg.Gn<<TAB;
    }
    *out <<endl;
}

//bool myfunc_wgni (WGNI1 i, WGNI1 j) { return (i.pval<j.pval); }//w/gni/
bool myfunc_wgni (WGNI1 i, WGNI1 j) { return (i.Ness>j.Ness); }//w/gni/

void print_dpaplot(ostream *out, met_genes &metG, gene_mets &geneM, strstr2flo &plotMatrix, strvec &mets, strvec &gns){//w/dpa/
    *out <<"Metabolites to genes: "<<endl<<endl;
    for (met_genes::iterator it = metG.begin(); it != metG.end(); ++it){
        string met=it->first;
        strvec genes=it->second;
        *out <<met;
        for(int i=0;i<genes.size();i++) *out <<TAB<<genes[i];
        *out <<endl;
    }
    *out <<TABEND<<endl<<endl;

    *out <<"Genes to metabolites: "<<endl;
    for (gene_mets::iterator it = geneM.begin(); it != geneM.end(); ++it){
        *out <<endl;
        string Gn=it->first;
        DPAGene dpag=it->second;
        *out <<"Gene:"<<TAB<<Gn<<endl;
        *out <<"KO Reactions:";
        for(int i=0;i<dpag.rkos.size();i++) *out <<TAB<<dpag.rkos[i];
        *out <<endl;
        *out <<"Metabolites:";
        for(int i=0;i<dpag.mets.size();i++) *out <<TAB<<dpag.mets[i];
        *out <<endl;
    }
    *out <<TABEND<<endl<<endl;

    *out <<"Matrix of producibility of wildtype and knockouts: "<<endl<<endl;
    *out <<TAB<<"WildType";
    for(int i=0;i<gns.size();i++) *out <<TAB<<gns[i];
    *out <<endl;
    for(int i=0;i<mets.size();i++){
        string met=mets[i];
        *out <<met<<TAB<<plotMatrix[met]["WildType"];
        for(int j=0;j<gns.size();j++){
            string Gn=gns[j];
            *out <<TAB<<plotMatrix[met][Gn];
        }
        *out <<endl;
    }
}

void print_dpasig(ostream *out, strvec &mets, strvec &arrs, metarr_sig &metsig){
    strstr2flo upsig=metsig.upsig;
    strstr2flo dwsig=metsig.dwsig;
    *out <<"Metabolite signals from microarray data: ";
    for(int i=0;i<arrs.size();i++) *out <<TAB<<arrs[i];
    *out <<endl<<endl;

    *out <<"Signals for up-regulated genes: "<<endl<<endl;
    for(int i=0;i<arrs.size();i++) *out <<TAB<<arrs[i];
    *out <<endl;
    for(int i=0;i<mets.size();i++){
        string met=mets[i];
        *out <<met;
        for(int j=0;j<arrs.size();j++){
            string arr=arrs[j];
            *out <<TAB<<upsig[met][arr];
        }
        *out <<endl;
    }
    *out <<TABEND<<endl<<endl;

    *out <<"Signals for down-regulated genes: "<<endl<<endl;
    for(int i=0;i<arrs.size();i++) *out <<TAB<<arrs[i];
    *out <<endl;
    for(int i=0;i<mets.size();i++){
        string met=mets[i];
        *out <<met;
        for(int j=0;j<arrs.size();j++){
            string arr=arrs[j];
            *out <<TAB<<dwsig[met][arr];
        }
        *out <<endl;
    }
}

