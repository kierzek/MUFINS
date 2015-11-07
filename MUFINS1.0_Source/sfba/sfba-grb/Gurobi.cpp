#include "Gurobi.h"



//******** class GRB_Mod *******************************************************

//GRB_Mod::GRB_Mod(GRB_Env *Env){
//    *model=GRBModel(Env->env);
//}

GRB_Mod::GRB_Mod(GRBEnv env){
    model = new GRBModel(env);
	soltol = 1e-6;
    mipfoc = 0;
}
GRB_Mod::~GRB_Mod(){
    delete model;
}


//void GRB_Mod::setModel(GRB_Env *Env){
//    model=GRBModel(Env->env);
//}

//GRB_Mod* GRB_Mod::copy(GRBEnv* env){
//    model->update();
//    GRB_Mod* newMod=new GRB_Mod(*env);
////    newMod->model= new GRBModel(*model);
//    *(newMod->model)=GRBModel(*model);
//    newMod->grbcons=grbcons;
//    newMod->grbvars=grbvars;
//    return newMod;
//}

//void GRB_Mod::copyTo(GRB_Mod* newMod){
//    model->update();
//    newMod->model= new GRBModel(*model);
////    *(newMod->model)=GRBModel(*model);
//    newMod->grbcons=grbcons;
//    newMod->grbvars=grbvars;
//}


void GRB_Mod::addConstr_sig(Model* netMod, string row, double ub){
    if(row.find("__rowA")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,0,row);
    else if(row.find("__rowI1")!=string::npos || row.find("__rowI2")!=string::npos){
        Element* met = netMod->find_element(row);
        double cub=met->get_ub();
        grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,cub,row);
    }else grbcons[row]=model->addConstr(0,GRB_EQUAL,0,row);
}


void GRB_Mod::build(Model* netMod){
    strvec reacs=netMod->get_reactions();
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=netMod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        GRBColumn col;
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string row=it->first;
            double coef=it->second;
            if(grbcons.find(row)==grbcons.end()){
                //grbcons[row]=model->addConstr(0,GRB_EQUAL,0,row);
                addConstr_sig(netMod, row, ub);
            }
            col.addTerm(coef,grbcons[row]);
        }
        model->update();
        char type=(reac->get_kind()==CONT)?GRB_CONTINUOUS:GRB_BINARY;
        if(type==GRB_BINARY) {lb=0; ub=1;}
        grbvars[Rn]=model->addVar(lb,ub,0,type,col,Rn);
    }
    model->update();
}

void GRB_Mod::build_gfa(Model* netMod){//gfa/
    strvec reacs=netMod->get_reactions();
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=netMod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        GRBColumn col;
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string row=it->first;
            double coef=it->second;
            if(grbcons.find(row)==grbcons.end()){
                if(row.find("__row1")!=string::npos) grbcons[row]=model->addConstr(0,GRB_GREATER_EQUAL,lb,row);
                else if(row.find("__row2")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,ub,row);
                else grbcons[row]=model->addConstr(0,GRB_EQUAL,0,row);
            }
            col.addTerm(coef,grbcons[row]);
        }
        model->update();
        char type=(reac->get_kind()==CONT)?GRB_CONTINUOUS:GRB_BINARY;
        if(type==GRB_BINARY) {lb=0; ub=1;}
        double obj=(type==GRB_BINARY)?1:0;
        grbvars[Rn]=model->addVar(lb,ub,obj,type,col,Rn);
    }
    model->update();
}

void GRB_Mod::build_gfa2(Model* netMod, stomap *obj){//gfa/
    strvec reacs=netMod->get_reactions();
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=netMod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        GRBColumn col;
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string row=it->first;
            double coef=it->second;
            if(grbcons.find(row)==grbcons.end()){
                if(row.find("__r1")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,ub,row);//forward reac
                else if(row.find("__r2")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,ub,row);//reverse reac
                else if(row.find("__r3")!=string::npos) grbcons[row]=model->addConstr(0,GRB_EQUAL,1,row);//y1+y2=1
                else if(row.find("__b1")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,ub,row);//gim3e,fimat
                else if(row.find("__b2")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,0,row);//gim3e,fimat
                else grbcons[row]=model->addConstr(0,GRB_EQUAL,0,row);
            }
            col.addTerm(coef,grbcons[row]);
        }
        model->update();
        char type=(reac->get_kind()==CONT)?GRB_CONTINUOUS:GRB_BINARY;
        if(type==GRB_BINARY) {lb=0; ub=1;}
        double objc=(obj->find(Rn)!=obj->end())?(*obj)[Rn]:0;
        grbvars[Rn]=model->addVar(lb,ub,objc,type,col,Rn);
    }
    model->update();
}

void GRB_Mod::build_sgni(Model* netMod, stomap *obj, string biom){
    strvec reacs=netMod->get_reactions();
    for(int i=0;i<reacs.size();i++){
        string Rn=reacs[i];
        Reaction* reac=netMod->get_reac(Rn);
        stomap *sto=reac->get_sto();
        double lb=reac->get_lb();
        double ub=reac->get_ub();
        GRBColumn col;
        for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string row=it->first;
            double coef=it->second;
            if(grbcons.find(row)==grbcons.end()){
                if(row.find("ro__"+biom)!=string::npos) grbcons[row]=model->addConstr(0,GRB_EQUAL,1,row);
                else if(row.find("re__")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,0,row);
                else if(row.find("rh1__")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,0,row);
                else if(row.find("rh2__")!=string::npos) grbcons[row]=model->addConstr(0,GRB_LESS_EQUAL,CTUB,row);
                else if(row.find("rh3__")!=string::npos) grbcons[row]=model->addConstr(0,GRB_GREATER_EQUAL,0,row);
                else grbcons[row]=model->addConstr(0,GRB_EQUAL,0,row);
            }
            col.addTerm(coef,grbcons[row]);
        }
//        if(Rn.find("BIOMASS")!=string::npos) objCoef=1;
//        else if(Rn.find("cx_")!=string::npos) objCoef=ub;
//        else if(Rn.find("cm_")!=string::npos) objCoef=-lb;//assume nutrient reac lb=0
//        else if(Rn.find("ch_")!=string::npos) objCoef=ub;
//        else objCoef=0;
        double objc=(obj->find(Rn)!=obj->end())?(*obj)[Rn]:0;
        char type=(reac->get_kind()==CONT)?GRB_CONTINUOUS:GRB_BINARY;
        model->update();
        grbvars[Rn]=model->addVar(lb,ub,objc,type,col,Rn);
    }
    model->update();
}


void GRB_Mod::externalise(strvec& externals, bool on){
    if(on){
        for(int i=0;i<externals.size();i++){
            grbcons[externals[i]].set(GRB_CharAttr_Sense, GRB_LESS_EQUAL);
            grbcons[externals[i]].set(GRB_DoubleAttr_RHS, GRB_INFINITY);
        }
    }
    else if(!on){
        for(int i=0;i<externals.size();i++){
            grbcons[externals[i]].set(GRB_CharAttr_Sense, GRB_EQUAL);
            grbcons[externals[i]].set(GRB_DoubleAttr_RHS, 0);
        }
    }
    model->update();
}

void GRB_Mod::externalise(string external, bool on){
    if(on){
            grbcons[external].set(GRB_CharAttr_Sense, GRB_LESS_EQUAL);
            grbcons[external].set(GRB_DoubleAttr_RHS, GRB_INFINITY);
    }
    else if(!on){
            grbcons[external].set(GRB_CharAttr_Sense, GRB_EQUAL);
            grbcons[external].set(GRB_DoubleAttr_RHS, 0);
    }
    model->update();
}

void GRB_Mod::setOptDir(int dir){
    model->set(GRB_IntAttr_ModelSense, dir);
}

void GRB_Mod::addConstraint(stomap* sto, double lb, double ub, string name){
	if(grbcons.find(name)==grbcons.end()){
		GRBLinExpr expr=0;
		for(stomap::iterator it=sto->begin();it!=sto->end();it++){
            string reac=it->first;
            double coef=it->second;
            if(grbvars.find(reac)!=grbvars.end()) expr += coef * grbvars[reac];
			else throw runtime_error(string("no column ") + reac);  
        }
		grbcons[name]=model->addRange(expr,lb,ub,name);
        model->update();
	}
	else throw runtime_error(string("no row ") + name); 
		
}

OBJSTAT GRB_Mod::optimize(){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);
	else sol.objval=0;
    return sol;
}

OBJSTAT GRB_Mod::optimize(GRBModel model){// for fixedModel
    OBJSTAT sol;
    model.update();
//    model.write("X_fixmodel_update.LP");//t/
//    model->presolve();
    model.optimize();
    sol.status=model.get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model.get(GRB_DoubleAttr_ObjVal);
    return sol;
}


OBJSTAT GRB_Mod::optimize(stomap *allsol){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);

    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar v = vars[j];
        (*allsol)[v.get(GRB_StringAttr_VarName)]=v.get(GRB_DoubleAttr_X);
    }
	return sol;
}

OBJSTAT GRB_Mod::optimizeFix(stomap* obj){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);
    else  throw runtime_error(sol.status + ": no optimal value");

    for (stomap::iterator it = obj->begin(); it != obj->end(); ++it){
        string name=it->first;
        double bd=grbvars[name].get(GRB_DoubleAttr_X);
        setVarBnds(name,bd,bd);
    }
//    write("X_e1260_gfvaGrb.LP");//t/
    return sol;
}

OBJSTAT GRB_Mod::optimize_gfva(){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);
    
    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar var = vars[j];
        if(var.get(GRB_CharAttr_VType)=='B'){
            string name=var.get(GRB_StringAttr_VarName);
            int bd=var.get(GRB_DoubleAttr_X);
//            int bd=0;//t/
//            double bd=(bd>0.8)?bd:0;
//            setVarBnds(name,bd,bd);
//            var.set(GRB_CharAttr_VType, 'C');
            var.set(GRB_DoubleAttr_LB, bd);
            var.set(GRB_DoubleAttr_UB, bd);
        }
    }
//    write("X_e1260_gfvaGrb.LP");//t/
    return sol;
}

OBJSTAT GRB_Mod::optimize_gfva2(stomap *allsol, stomap *obj){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);

    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar var = vars[j];
        if(var.get(GRB_CharAttr_VType)=='C'){
            string Rn=var.get(GRB_StringAttr_VarName);
            double bd=var.get(GRB_DoubleAttr_X);
            (*allsol)[Rn]=bd;
            if(obj->find(Rn)!=obj->end()) setVarBnds(Rn,bd,bd);
        }
    }

   // for(stomap::iterator it=obj->begin();it!=obj->end();it++){
   //     string Rn=it->first;
   //     GRBVar var = model->getVarByName(Rn);
   //         double bd=var.get(GRB_DoubleAttr_X);
			////cout<<Rn<<" : "<<bd<<endl;//t/
   //         setVarBnds(Rn,bd,bd);
   // }

	//GRBConstr *cons=model->getConstrs();//remove constraints: row1,2,3
	//int numcons=model->get(GRB_IntAttr_NumConstrs);
 //   for (int j = 0; j < numcons; j++) {
 //       GRBConstr cons = cons[j];
 //       string name=cons.get(GRB_StringAttr_ConstrName);
	//	if(name.find("__r1")!=string::npos) model->remove(cons);
	//	else if(name.find("__r2")!=string::npos) model->remove(cons);
	//	else if(name.find("__r3")!=string::npos) model->remove(cons);
	//}

    for(GRBcons::iterator it=grbcons.begin();it!=grbcons.end();it++){
        string row=it->first;
        GRBConstr cons=it->second;
        string name=cons.get(GRB_StringAttr_ConstrName);
        if(name.find("__r1")!=string::npos) model->remove(cons);
        else if(name.find("__r2")!=string::npos) model->remove(cons);
        else if(name.find("__r3")!=string::npos) model->remove(cons);
    }
    for(GRBvars::iterator it=grbvars.begin();it!=grbvars.end();it++){
        string col=it->first;
        GRBVar var=it->second;
        //string name=var.get(GRB_StringAttr_VarName);
        if(var.get(GRB_CharAttr_VType)=='B') model->remove(var);
        /*if(name.find("__c1")!=string::npos) model->remove(var);
        else if(name.find("__c2")!=string::npos) model->remove(var);*/
    }
    return sol;
}

OBJSTAT GRB_Mod::optimize_fimat(stomap *allsol, stomap *obj){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);

    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar var = vars[j];
        if(var.get(GRB_CharAttr_VType)=='C'){
            string Rn=var.get(GRB_StringAttr_VarName);
            double bd=var.get(GRB_DoubleAttr_X);
            (*allsol)[Rn]=bd;
            if(obj->find(Rn)!=obj->end()) setVarBnds(Rn,bd,bd);
        }
    }

    for(GRBcons::iterator it=grbcons.begin();it!=grbcons.end();it++){
        string row=it->first;
        GRBConstr cons=it->second;
        string name=cons.get(GRB_StringAttr_ConstrName);
        if(name.find("__b1")!=string::npos) model->remove(cons);
        else if(name.find("__b2")!=string::npos) model->remove(cons);
    }
    for(GRBvars::iterator it=grbvars.begin();it!=grbvars.end();it++){
        string col=it->first;
        GRBVar var=it->second;
        if(var.get(GRB_CharAttr_VType)=='B') model->remove(var);
    }
    return sol;
}

OBJSTAT GRB_Mod::optimize_gfva2x(stomap *allsol, stomap *obj, float eps){
    OBJSTAT sol;
    model->update();
//    model->presolve();
    model->optimize();
    sol.status=model->get(GRB_IntAttr_Status);
    if(sol.status==GRB_OPTIMAL)
        sol.objval=model->get(GRB_DoubleAttr_ObjVal);
        //cout<<"objval = "<<sol.objval<<endl;//t/
    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar var = vars[j];
        if(var.get(GRB_CharAttr_VType)=='C'){
            string Rn=var.get(GRB_StringAttr_VarName);
            double bd=var.get(GRB_DoubleAttr_X);
            (*allsol)[Rn]=bd;
            if(obj->find(Rn)!=obj->end()){
                if(bd>=eps) setVarBnds(Rn,eps,1000);
                else if(bd<=-eps) setVarBnds(Rn,-1000,-eps);
                else setVarBnds(Rn,0,0);
            }
        }
    }

    for(GRBcons::iterator it=grbcons.begin();it!=grbcons.end();it++){
        string row=it->first;
        GRBConstr cons=it->second;
        string name=cons.get(GRB_StringAttr_ConstrName);
        if(name.find("__r1")!=string::npos) model->remove(cons);
        else if(name.find("__r2")!=string::npos) model->remove(cons);
        else if(name.find("__r3")!=string::npos) model->remove(cons);
    }
    for(GRBvars::iterator it=grbvars.begin();it!=grbvars.end();it++){
        string col=it->first;
        GRBVar var=it->second;
        //string name=var.get(GRB_StringAttr_VarName);
        if(var.get(GRB_CharAttr_VType)=='B') model->remove(var);
        /*if(name.find("__c1")!=string::npos) model->remove(var);
        else if(name.find("__c2")!=string::npos) model->remove(var);*/
    }
    return sol;
}

stomap GRB_Mod::getAllSol(){
    stomap allsol;
    GRBVar *vars=model->getVars();
    int numvars=model->get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        GRBVar v = vars[j];
        allsol[v.get(GRB_StringAttr_VarName)]=v.get(GRB_DoubleAttr_X);
    }
    return allsol;
}

FLUXACT GRB_Mod::getRange_gfaa(string reac, double bound, double eps){//gfa/
    FLUXACT activity;
    if(bound>eps){
//        setColBnds(reac, eps, UB);
    	setVarBnds(reac, eps, bound);
        activity.act=optimize();
        activity.act.objval=(int)activity.act.objval;
        if(activity.act.status!=GRB_OPTIMAL) activity.act.objval=0;
        setVarBnds(reac, 0, eps);
        activity.inact=optimize();
        activity.inact.objval=(int)activity.inact.objval;
        if(activity.inact.status!=GRB_OPTIMAL) activity.inact.objval=0;
        activity.level=activity.act.objval-activity.inact.objval;
    }
    else if(bound<-eps){
//        setColBnds(reac, LB, -eps);
    	setVarBnds(reac, bound, -eps);
        activity.act=optimize();
        activity.act.objval=(int)activity.act.objval;
        if(activity.act.status!=GRB_OPTIMAL) activity.act.objval=0;
        setVarBnds(reac, -eps, 0);
        activity.inact=optimize();
        activity.inact.objval=(int)activity.inact.objval;
        if(activity.inact.status!=GRB_OPTIMAL) activity.inact.objval=0;
        activity.level=activity.act.objval-activity.inact.objval;
    }
    return activity;
}

void GRB_Mod::getRanges(rangemap* rm, strvec cols){
    for(int i=0;i<cols.size();i++){
//        write("X_gfvaGrb_getRanges.LP");//t/
        string col=cols[i];
        GRBVar var=grbvars[col];
        model->setObjective(var + 0);
        setOptDir(GrbMax);
        (*rm)[col].max=optimize();
        setOptDir(GrbMin);
        (*rm)[col].min=optimize();
        //cout<<col<<endl;//t/
    }
}

void GRB_Mod::getRanges_gfva2(OBJSTAT &objrange, stomap *allsol,rangemap* rm, strvec cols, stomap *obj){
    for(int i=0;i<cols.size();i++){
        string col=cols[i];
        if(obj->find(col)==obj->end()){
                GRBVar var=grbvars[col];
                model->setObjective(var + 0);
                setOptDir(GrbMax);
                (*rm)[col].max=optimize();
                setOptDir(GrbMin);
                (*rm)[col].min=optimize();
        }
        else{
                (*rm)[col].max.objval=(*allsol)[col];
                (*rm)[col].max.status=objrange.status;
                (*rm)[col].min.objval=(*allsol)[col];
                (*rm)[col].min.status=objrange.status;
        }
    }
}

void GRB_Mod::getRanges(GRBModel model, rangemap* rm, strvec cols){
    GRBvars modvars;
    GRBVar *vars=model.getVars();
    int numvars=model.get(GRB_IntAttr_NumVars);
    for (int j = 0; j < numvars; j++) {
        string name=vars[j].get(GRB_StringAttr_VarName);//t/
        modvars[name] = vars[j];
    }
        
    for(int i=0;i<cols.size();i++){
        string col=cols[i];
        GRBVar var=modvars[col];
        model.setObjective(var + 0);
        model.set(GRB_IntAttr_ModelSense, GrbMax);
        model.update();
        (*rm)[col].max=optimize(model);
        model.set(GRB_IntAttr_ModelSense, GrbMin);
        model.update();
        (*rm)[col].min=optimize(model);
    }
}


void GRB_Mod::getOptRanges(rangemap* rm, OBJSTAT &objrange, stomap* obj, strvec cols){//fvaGrb
    objrange=optimizeFix(obj);

//    model->getEnv().set(GRB_IntParam_Method, 0);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
//    model->getEnv().set(GRB_DoubleParam_BarConvTol, 1e-10);
//    model->getEnv().set(GRB_DoubleParam_OptimalityTol, 1e-9);
//    model->getEnv().set(GRB_DoubleParam_FeasibilityTol, 1e-3);
//    model->getEnv().set(GRB_DoubleParam_MarkowitzTol, 0.01);
//    model->getEnv().set(GRB_DoubleParam_PerturbValue, 0.00001);
//    model->getEnv().set(GRB_IntParam_Quad, 1);//t/
//    model->getEnv().set(GRB_IntParam_ScaleFlag, 0);//t/
//    model->getEnv().set(GRB_IntParam_Presolve, 1);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
    
    getRanges(rm, cols);
}

void GRB_Mod::getOptRanges_gfva(rangemap* rm, OBJSTAT &objrange, strvec cols){//w/
//    objrange=optimize_gfva();
//    model->getEnv().set(GRB_IntParam_MIPFocus, 1);//t/
	
	objrange=optimize_gfva();

//    model->getEnv().set(GRB_IntParam_Presolve, 0);//t/
//    model->getEnv().set(GRB_IntParam_Method, 1);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
//    model->getEnv().set(GRB_DoubleParam_OptimalityTol, 1e-9);
//    model->getEnv().set(GRB_DoubleParam_FeasibilityTol, 1e-3);
//    model->getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-3);

	write("X_gfva2Grb_getRanges.LP");//t/
    getRanges(rm, cols);
}

void GRB_Mod::getOptRanges_gfva2(rangemap* rm, OBJSTAT &objrange, stomap *allsol, strvec cols, stomap *obj){//w/
//    objrange=optimize_gfva();
//    model->getEnv().set(GRB_IntParam_MIPFocus, 1);//t/
	
    //objrange=optimize_gfva2(allsol, obj);
    objrange=optimize_fimat(allsol, obj);

//    model->getEnv().set(GRB_IntParam_Presolve, 0);//t/
//    model->getEnv().set(GRB_IntParam_Method, 1);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
//    model->getEnv().set(GRB_DoubleParam_OptimalityTol, 1e-9);
//    model->getEnv().set(GRB_DoubleParam_FeasibilityTol, 1e-3);
//    model->getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-3);

	//write("X_gfva2Grb_getRanges.LP");//t/
    getRanges_gfva2(objrange, allsol, rm, cols, obj);
}

void GRB_Mod::getOptRanges_gfva2x(rangemap* rm, OBJSTAT &objrange, stomap *allsol, strvec cols, stomap *obj, float eps){//w/fimat2
//    objrange=optimize_gfva();
//    model->getEnv().set(GRB_IntParam_MIPFocus, 1);//t/
	
    objrange=optimize_gfva2x(allsol, obj, eps);
    getRanges(rm, cols);
    //getRanges_gfva2(objrange, allsol, rm, cols, obj);
}

void GRB_Mod::getOptRanges_gfva_fixmod(rangemap* rm, OBJSTAT &objrange, strvec cols){//w/
    objrange=optimize();
    GRBModel fixmod = model->fixedModel();
    fixmod.getEnv().set(GRB_IntParam_Presolve, 0);
    fixmod.getEnv().set(GRB_IntParam_Method, 0);
    fixmod.getEnv().set(GRB_DoubleParam_FeasibilityTol, 1e-3);
//    fixmod.write("X_fixmod.LP");//t/
    getRanges(fixmod, rm, cols);
}


void GRB_Mod::delCon(string name){
    model->remove(grbcons[name]);
}

void GRB_Mod::delVar(string name){
    model->remove(grbvars[name]);
}

GRBConstr GRB_Mod::getCon(string name){
    return grbcons[name];
}

GRBVar GRB_Mod::getVar(string name){
    return grbvars[name];
}

double GRB_Mod::getVarUB(string name){
    return grbvars[name].get(GRB_DoubleAttr_UB);
}

double GRB_Mod::getVarLB(string name){
    return grbvars[name].get(GRB_DoubleAttr_LB);
}

void GRB_Mod::setVarBnds(string name, double lb, double ub){
    GRBVar var=grbvars[name];
    var.set(GRB_DoubleAttr_LB, lb);
    var.set(GRB_DoubleAttr_UB, ub);
}

void GRB_Mod::setAllVarBnds(double lb, double ub){
    GRBVar *vars = 0;
    vars = model->getVars();
    for (int j = 0; j < model->get(GRB_IntAttr_NumVars); j++) {
        GRBVar var = vars[j];
        var.set(GRB_DoubleAttr_LB, lb);
        var.set(GRB_DoubleAttr_UB, ub);
    }
}

void GRB_Mod::setObj(string name, double coef){//for reactions of variables
        GRBVar var=grbvars[name];
        var.set(GRB_DoubleAttr_Obj, coef);
}

void GRB_Mod::setObjs(stomap* obj){//for stomap obj
    for (stomap::iterator it = obj->begin(); it != obj->end(); ++it){
        string name=it->first;
        double coef=it->second;
        setObj(name, coef);
    }
}

void GRB_Mod::setObj(string name){//for metabolites of constraints
    GRBConstr row=grbcons[name];
    GRBLinExpr expr=model->getRow(row);
    model->setObjective(expr);
    externalise(name,true);
}

void GRB_Mod::setMixedObj(stomap* sto, strvec &externals){
	GRBLinExpr expr=0;
	for (stomap::iterator it = sto->begin(); it!= sto->end(); ++it){
        string name = it->first;
		double coef=it->second;
		if(grbvars.find(name)!=grbvars.end()) expr += coef * grbvars[name];
		else if(grbcons.find(name)!=grbcons.end()){
			expr += coef * model->getRow(grbcons[name]);
			externals.push_back(name);
		}
		else throw runtime_error(string("no scuh column or row: ") + name);
	}
	model->setObjective(expr);
}

bool GRB_Mod::hasVar(string name){
    return grbvars.find(name)!=grbvars.end();
}


void GRB_Mod::ko_blp(strvec rkos, strvec rnts, grbKoRow &kor, grbKoCol &koc){//knockout blp model
     strvec Rt;
     for(int i=0;i<rkos.size();i++){//assume non-nts reactions for ko genes
         string Rn=rkos[i];
         if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
             Rt.push_back(Rn);
             string ro="ro__"+Rn;
             kor[ro]=set_kor(ro);
             if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if not in nutrient reactions
                string cx="cx__"+Rn;
                string cm="cm__"+Rn;
                koc[cx]=set_koc(cx);
                koc[cm]=set_koc(cm);
                model->remove(grbcons[ro]);
                model->remove(grbvars[cx]);
                model->remove(grbvars[cm]);
                grbcons.erase(ro);
                grbvars.erase(cx);
                grbvars.erase(cm);
             }else{
                string ct="ct__"+Rn;
                string ch="ch__"+Rn;
                string rh1="rh1__"+Rn;
                string rh2="rh2__"+Rn;
                string rh3="rh3__"+Rn;
                koc[ct]=set_koc(ct);
                koc[ch]=set_koc(ch);
                kor[rh1]=set_kor(rh1);
                kor[rh2]=set_kor(rh2);
                kor[rh3]=set_kor(rh3);
                model->remove(grbcons[ro]);
                model->remove(grbvars[ct]);
                model->remove(grbvars[ch]);
                model->remove(grbcons[rh1]);
                model->remove(grbcons[rh2]);
                model->remove(grbcons[rh3]);
                grbcons.erase(ro);
                grbcons.erase(rh1);
                grbcons.erase(rh2);
                grbcons.erase(rh3);
                grbvars.erase(ct);
                grbvars.erase(ch);
             }
         }
    }
//    model->update();
}

//void GRB_Mod::ko_blp(strvec rkos, strvec rnts, grbKos &kos){//knockout blp model
//     strvec Rt;
//     for(int i=0;i<rkos.size();i++){//assume non-nts reactions for ko genes
//         string Rn=rkos[i];
//         if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
//             Rt.push_back(Rn);
//             string ro="ro__"+Rn;
//             GRBLinExpr ROW=model->getRow(grbcons[ro]);
//             for(int i=0;i<ROW.size();i++){
//                 string coln=ROW.getVar(i).get(GRB_StringAttr_VarName);
//                 kos[ro].sto[coln]=ROW.getCoeff(i);
//                 model->chgCoeff(grbcons[ro], grbvars[coln], 0);
//             }
//             if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if not in nutrient reactions
//                string cx="cx__"+Rn;
//                string cm="cm__"+Rn;
//                kos[ro].obj[cx]=grbvars[cx].get(GRB_DoubleAttr_Obj);
//                kos[ro].obj[cm]=grbvars[cm].get(GRB_DoubleAttr_Obj);
//                grbvars[cx].set(GRB_DoubleAttr_Obj, 0);
//                grbvars[cm].set(GRB_DoubleAttr_Obj, 0);
//             }else{
//                string ch="ch__"+Rn;
//                kos[ro].obj[ch]=grbvars[ch].get(GRB_DoubleAttr_Obj);
//                grbvars[ch].set(GRB_DoubleAttr_Obj, 0);
//             }
//         }
//    }
////    model->update();
//}

void GRB_Mod::ko_blp(strvec rkos, grbKos &kos, reac2nts &reacN){//knockout blp model
     strvec Rt;
     for(int i=0;i<rkos.size();i++){//assume non-nts reactions for ko genes
         string Rn=rkos[i];
         if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
             Rt.push_back(Rn);
             string ro="ro__"+Rn;
//             GRBLinExpr ROW=model->getRow(grbcons[ro]);
//             for(int i=0;i<ROW.size();i++){
//                 string coln=ROW.getVar(i).get(GRB_StringAttr_VarName);
//                 kos[ro].sto[coln]=ROW.getCoeff(i);
//                 model->chgCoeff(grbcons[ro], grbvars[coln], 0);
//             }
//             if(find(rnts.begin(),rnts.end(),Rn)==rnts.end()){//if not in nutrient reactions
                string cx="cx__"+Rn;
                string cm="cm__"+Rn;
                kos[ro].obj[cx]=grbvars[cx].get(GRB_DoubleAttr_Obj);
                kos[ro].obj[cm]=grbvars[cm].get(GRB_DoubleAttr_Obj);
                grbvars[cx].set(GRB_DoubleAttr_Obj, 0);
                grbvars[cm].set(GRB_DoubleAttr_Obj, 0);
//             }else{
             if(reacN.find(Rn)!=reacN.end()){//if in nutrient reactions
                strvec nts=reacN[Rn];
                for(int i=0;i<nts.size();i++){//for multi nutrients in a reaction
                    string nt=nts[i];
                    string ch="ch__"+Rn+nt;
                    kos[ro].obj[ch]=grbvars[ch].get(GRB_DoubleAttr_Obj);
                    grbvars[ch].set(GRB_DoubleAttr_Obj, 0);
                }
             }
         }
    }
    model->update();
}

void GRB_Mod::ko_mlp(strvec rkos, grbKoCol &koc){//gni/knockout mlp model
    strvec Rt;
     for(int i=0;i<rkos.size();i++){
        string Rn=rkos[i];
        if(find(Rt.begin(),Rt.end(),Rn)==Rt.end()){//avoid the problem of reapeat reactions for a gene
            Rt.push_back(Rn);
            koc[Rn]=set_koc(Rn);
            model->remove(grbvars[Rn]);
            grbvars.erase(Rn);
        }
    }
}

grbRow GRB_Mod::set_kor(string name){
    grbRow row;
    GRBConstr con=grbcons[name];
    row.sense=con.get(GRB_CharAttr_Sense);
    row.rhs=con.get(GRB_DoubleAttr_RHS);
    GRBLinExpr ROW=model->getRow(con);
    for(int i=0;i<ROW.size();i++){
         row.sto[ROW.getVar(i).get(GRB_StringAttr_VarName)]=ROW.getCoeff(i);
    }
    return row;
}

grbCol GRB_Mod::set_koc(string name){
    grbCol col;
    GRBVar var=grbvars[name];
    col.lb=var.get(GRB_DoubleAttr_LB);
    col.ub=var.get(GRB_DoubleAttr_UB);
    col.obj=var.get(GRB_DoubleAttr_Obj);
    col.COL=model->getCol(var);
//    GRBColumn COL=model->getCol(var);
//    for(int i=0;i<COL.size();i++){
//         col.sto[COL.getConstr(i).get(GRB_StringAttr_ConstrName)]=COL.getCoeff(i);
//    }
    return col;
}

void GRB_Mod::rko_blp(grbKoRow &kor, grbKoCol &koc){
     for(grbKoCol::iterator it=koc.begin();it!=koc.end();it++){
        string name=it->first;
        grbCol col=it->second;
        grbvars[name]=model->addVar(col.lb, col.ub, col.obj, GRB_CONTINUOUS, name);
        model->update();
     }
     for(grbKoRow::iterator it=kor.begin();it!=kor.end();it++){
        string name=it->first;
        grbRow row=it->second;
        grbcons[name]=model->addConstr(0, row.sense, row.rhs, name);
        model->update();
        for (stomap::iterator it =row.sto.begin(); it != row.sto.end(); ++it){
                string coln=it->first;
                double coef=it->second;
                model->chgCoeff(grbcons[name], grbvars[coln], coef);
        }
     }
     model->update();
}

//void GRB_Mod::rko_blp(grbKos &kos){
//     for(grbKos::iterator it=kos.begin();it!=kos.end();it++){
//        string name=it->first;
//        grbKo ko=it->second;
//
//        for (stomap::iterator it =ko.sto.begin(); it != ko.sto.end(); ++it){
//                string coln=it->first;
//                double coef=it->second;
//                model->chgCoeff(grbcons[name], grbvars[coln], coef);
//        }
//        for (stomap::iterator it =ko.obj.begin(); it != ko.obj.end(); ++it){
//                string coln=it->first;
//                double coef=it->second;
//                grbvars[coln].set(GRB_DoubleAttr_Obj,coef);
//        }
//     }
//     model->update();
//}

void GRB_Mod::rko_blp(grbKos &kos){
     for(grbKos::iterator it=kos.begin();it!=kos.end();it++){
        string name=it->first;
        grbKo ko=it->second;

//        for (stomap::iterator it =ko.sto.begin(); it != ko.sto.end(); ++it){
//                string coln=it->first;
//                double coef=it->second;
//                model->chgCoeff(grbcons[name], grbvars[coln], coef);
//        }
        for (stomap::iterator it =ko.obj.begin(); it != ko.obj.end(); ++it){
                string coln=it->first;
                double coef=it->second;
                grbvars[coln].set(GRB_DoubleAttr_Obj,coef);
        }
     }
     model->update();
}

void GRB_Mod::rko_mlp(grbKoCol &koc){
     for(grbKoCol::iterator it=koc.begin();it!=koc.end();it++){
        string name=it->first;
        grbCol col=it->second;
        grbvars[name]=model->addVar(col.lb, col.ub, col.obj, GRB_CONTINUOUS, col.COL, name);
     }
//     model->update();
}


void GRB_Mod::write(string fname){
    model->update();
    model->write(fname.data());
}


void GRB_Mod::setPara(){
    model->getEnv().set(GRB_IntParam_OutputFlag, 0);//0: no output
    model->getEnv().set(GRB_IntParam_MIPFocus, mipfoc); //1:finding feasible quickly; 2:proving optimality; 3:help bound moving for best objective; 0: auto
//    model->getEnv().set(GRB_IntParam_NodeMethod, 2);
//    model->getEnv().set(GRB_DoubleParam_Heuristics, 0.5);
//    model->getEnv().set(GRB_DoubleParam_MIPGap, 1e-6);
//    model->getEnv().set(GRB_IntParam_PreDual, 2);
//    model->getEnv().set(GRB_IntParam_Presolve, 2);

//    model->getEnv().set(GRB_DoubleParam_IterationLimit, 3000000);

//    model->getEnv().set(GRB_IntParam_Method, 4);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
//    model->getEnv().set(GRB_DoubleParam_BarConvTol, 1e-10);
    model->getEnv().set(GRB_DoubleParam_OptimalityTol, soltol); //simplex, the less the more optimal 
    model->getEnv().set(GRB_DoubleParam_FeasibilityTol, soltol); //simplex, the less the more optimal, big effect 
//	model->getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-3);
//    model->getEnv().set(GRB_DoubleParam_MarkowitzTol, 1e-3); //simplex, the less the more optimal 
//    model->getEnv().set(GRB_DoubleParam_PerturbValue, 1e-6);
//    model->getEnv().set(GRB_IntParam_Quad, 1);//t/
//    model->getEnv().set(GRB_IntParam_ScaleFlag, 0);//t/
//    model->getEnv().set(GRB_IntParam_Presolve, 1);//t/
//    model->getEnv().set(GRB_IntParam_Aggregate, 0);//t/
//    model->getEnv().set(GRB_IntParam_SimplexPricing, 1);//t/

}





