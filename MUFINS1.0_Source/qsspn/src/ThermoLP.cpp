
#include "ThermoLP.h"



ThermoLP::ThermoLP(bool max, float T,  int msg_lev, int tm_lim, double cmin, double cmax, int it_lim, string name):\
SplitLP(max, MILP, msg_lev, tm_lim, it_lim, name)
{
    this->vmax = VMAX;
    this->cmin = cmin;
    this->cmax = cmax;
    this->K = DGMAX;
    this->types = new intmap;
    this->setTemperature(T);
}

ThermoLP::~ThermoLP(){
    delete types;
}

int ThermoLP::getColKind(int i) const{
    int rv = SplitLP::getColKind(i);
    intmap::iterator it = types->find(i);
    if (it != types->end())
        rv = it->second;
    return rv;
}

double ThermoLP::getColValue(string name) const{
    double rv = SplitLP::getColValue(name);
    if (getColKind(ncol(name)) == LNCONC_TYPE){
        rv = exp(rv);
    }
    return rv;
}

void ThermoLP::setDeltaG0(string rx, double lb, double ub){
    Reaction* reac = (Reaction*)(*stomtx)[rx];
    reac->set_deltaG0_range(lb, ub);
}

void ThermoLP::addMatrix(Matrix* N){
    SplitLP::addMatrix(N);
    this->stomtx = N;
    addConcentrations();
    addThermoCols();
}

void ThermoLP::addConcentrations(){
    for (strvec::const_iterator it = stomtx->rows.begin(); it != stomtx->rows.end(); it++){
        int j = addCol(concName(*it), cmin, cmax);
        (*types)[j] = LNCONC_TYPE;
    }
}

void ThermoLP::addThermoCols(){
    strvec reacs = getColNames(CONT);
    for (strvec::iterator it = reacs.begin(); it != reacs.end(); it+=2){
        string rx = *it;
        string deltaname = DELTAG_TAG + rx;
        int i = addCol(deltaname);
        (*types)[i] = DELTA_TYPE;
        addIntConstraints(rx, deltaname, 1);
        addIntConstraints(*(it + 1), deltaname, -1);
    }
}

void ThermoLP::update(){
    strvec reacs = getColNames(CONT);
    for (strvec::iterator it = reacs.begin(); it != reacs.end(); it+=2){
        string rx = *it;
        RANGE DeltaG0 = getDeltaGRange(rx, stomtx);
        addThermoConstraints(rx, DeltaG0);
    }
}

void ThermoLP::addThermoConstraints(string rx, RANGE DeltaG0){
    string deltaname = DELTAG_TAG + rx;
    stomap sto;
    sto[deltaname] = 1;
    stomap* iw = getColSto(rx);
    for (stomap::iterator it = iw->begin(); it != iw->end(); ++it)
        sto[concName(it->first)] = - it->second * RT;
    delete iw;
    addConstraint(&sto, DeltaG0.min, DeltaG0.max, false, deltaname + CONC_SUFX);
}


void ThermoLP::addIntConstraints(string name, string deltaname, int deltacoef){
    string intname = get_int_name(name);
    stomap sto;
    sto[name] = -1;
    sto[intname] = K;
    addConstraint(&sto, 0, UB, false, name + INT_CONSTR_TAG);

    sto.erase(name);
    sto[deltaname] = deltacoef;
    addConstraint(&sto, LB, K, false, DELTAG_TAG + name + "_int");
}



RANGE ThermoLP::getDeltaGRange(string rx, Matrix* N){
    Reaction* reac = (Reaction*)(*N)[rx];
    return reac->get_deltaG0_range();
}

void ThermoLP::setConcentration(string met, double lb, double ub){
    lb = log(lb);
    ub = log(ub);
    if (met == "[]"){
        strvec cnames = getColNames(LNCONC_TYPE);
        for (strvec::iterator it = cnames.begin(); it != cnames.end(); ++it)
            setColBnds(*it, lb, ub);
    }
    else if (this->ncol(met))
        setColBnds(met, lb, ub);
}

void ThermoLP::setTemperature(double T){
    this->T = T;
    RT = R * this->T;
}


void ThermoLP::getColRange(OBJSTAT_RANGE& range, string cname, bool presolve){
    MtxLP::getColRange(range, cname, presolve);
    if (getColKind(ncol(cname)) == LNCONC_TYPE){
        range.min.objval = exp(range.min.objval);
        range.max.objval = exp(range.max.objval);
    }
}

string ThermoLP::getID(string colname){
    string rv = MtxLP::getID(colname);
    int kind = getColKind(ncol(colname));
    switch (kind){
        case LNCONC_TYPE:
            rv = rv.substr(1, rv.size() - 2);
            break;
        case DELTA_TYPE:
            rv = rv.substr(3);
            break;

    }
    return rv;
}











//void ThermoLP::addMatrix(Matrix* N){
//    SplitLP::addMatrix(N);
//    strvec reacs = getColNames(CONT);
//    addIntConstraints(reacs);
//    strvec::iterator it = reacs.begin(), rend = reacs.end();
//    while (it != rend){
//        string rx = *it;
//        double DeltaG_LB = getDeltaG(rx, N);
//        double DeltaG_UB = (DeltaG_LB == LB) ? UB : DeltaG_LB;
//        string dGplus = addThermoConstraints(rx, DeltaG_LB, DeltaG_UB);
//        string dGminus = addThermoConstraints(*(it + 1), -DeltaG_LB, -DeltaG_UB);
////        stomap sto;
////        sto[dGplus] = 1;
////        sto[dGminus] = 1;
////        addConstraint(&sto, 0, 0, false, rx + "_block");
//        it += 2;
//    }
//}
//
//void ThermoLP::addIntConstraints(strvec cols, bool tmp){
//    int contcoef = -1;
//    int intcoef = -contcoef;
//    for (strvec::iterator it = cols.begin(); it != cols.end(); ++it){
//        string name = *it;
//        string intname = get_int_name(name);
//        stomap sto;
//        sto[name] = contcoef;
//        sto[intname] = intcoef * vmax;
//        addConstraint(&sto, 0, UB, tmp, name + INT_CONSTR_TAG);
//    }
//}
//
//string ThermoLP::addThermoConstraints(string rx, double DeltaG_LB, double DeltaG_UB){
////    cout << rx << ": " << DeltaG0 << endl;
//    string intname = get_int_name(rx);
//    string deltaname = DELTAG_TAG + rx;
//    int i = addCol(deltaname);
//    (*types)[i] = DELTA_TYPE;
//    stomap sto;
//    sto[intname] = K;
//    sto[deltaname] = 1;
//    addConstraint(&sto, LB, K - DG_EPSILON, false, deltaname + "_1");
//    sto.erase(intname);
//
//    stomap* iw = getColSto(rx);
//    for (stomap::iterator it = iw->begin(); it != iw->end(); ++it){
//        string met = it->first;
//        string lncname = met;
//        if (ncol(lncname) == 0){
//            int j = addCol(lncname, cmin, cmax);
//            (*types)[j] = LNCONC_TYPE;
//        }
//        sto[lncname] = - it->second * RT;
//    }
//    delete iw;
//    addConstraint(&sto, DeltaG_LB, DeltaG_UB, false, deltaname + "_2");
//    return deltaname;
//}

//void ThermoLP::addMatrix(Matrix* N){
//    SplitLP::addMatrix(N);
//    strvec reacs = getColNames(CONT);
//
//    strvec::iterator it = reacs.begin(), rend = reacs.end();
//    while (it != rend){
//        string rx = *it;
//        RANGE DeltaG0 = getDeltaGRange(rx, N);
//
//        //if (DeltaG_LB != LB){
//            string deltaname = addThermoConstraints(rx, DeltaG0);
//            addIntConstraints(rx, deltaname, 1);
//            addIntConstraints(*(it + 1), deltaname, -1);
//        //}
//        it += 2;
//    }
//}