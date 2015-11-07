
#include "TfbaLP.h"

TfbaLP::TfbaLP(bool max, float T,  int msg_lev, int tm_lim, double cmin, double cmax, int it_lim, string name):\
SplitLP(max, SMPLX, msg_lev, tm_lim, it_lim, name)
{
    this->vmax = VMAX;
    this->cmin = cmin;
    this->cmax = cmax;
    this->K = DGMAX;
    this->types = new intmap;
    this->setTemperature(T);
    nblocked = 0;
}

TfbaLP::~TfbaLP(){
    delete types;
}

void TfbaLP::setTemperature(double T){
    this->T = T0_ + T;
    RT = R_ * this->T;
}

float TfbaLP::getTemperature() const{
    return T - T0_;
}


void TfbaLP::addMatrix(Matrix* N){
    SplitLP::addMatrix(N);
    this->stomtx = N;
    addConcentrations();    
}

void TfbaLP::addConcentrations(){
    for (strvec::const_iterator it = stomtx->rows.begin(); it != stomtx->rows.end(); it++){
        int j = addCol(*it, cmin, cmax);
        (*types)[j] = LNCONC_TYPE;
    }
}

RANGE TfbaLP::getDeltaG0(string rx)const{
    bool isneg = rx[0] == '-';
    if (isneg)rx = rx.substr(1);
    Reaction* reac = (Reaction*)(*stomtx)[rx];
    RANGE rv = reac->get_deltaG0_range();
    if (isneg){
        RANGE neg;
        neg.min = -rv.max;
        neg.max = -rv.min;
        rv = neg;
    }
    return rv;
}

RANGE TfbaLP::getDeltaG(string rx)const{
    RANGE rv = getDeltaG0(rx);
    if (rv.min == LB)
        return rv;
    stomap* iw = getColSto(rx);
    for (stomap::iterator it = iw->begin(); it != iw->end(); ++it){
        string met = it->first;
        double coef = it->second;
        double lb = getColLB(met);
        double ub = getColUB(met);
        double RTcoef = RT * coef;
        //cout << met << ' ' << coef << ' ' << lb << ' ' << ub << endl;
        rv.min += RTcoef * (coef < 0 ? ub : lb);
        rv.max += RTcoef * (coef < 0 ? lb : ub);
    }
    delete iw;
    return rv;
}

void TfbaLP::setFeasibleDirs(string rx){
    RANGE dG = this->getDeltaG(rx);
    if (dG.min == LB)
        return;
    if (dG.min >= 0)thermoBlock(rx);
    if (dG.max <= 0)thermoBlock(get_opp_name(rx));
}

void TfbaLP::thermoBlock(string rx){
    string constraint_name = rx + "_tblock";
    cout << rx << endl;
    block(rx, true, constraint_name);
    nblocked++;
}

void TfbaLP::setDeltaG0(string rx, double lb, double ub){
    Reaction* reac = (Reaction*)(*stomtx)[rx];
    reac->set_deltaG0_range(lb, ub);
}

void TfbaLP::setConcentration(string met, double lb, double ub){
    lb = log(lb);
    ub = log(ub);
    if (met.size() == 0){
//        cout << "default: " << lb << '\t' << ub << endl;
        strvec cnames = getColNames(LNCONC_TYPE);
        for (strvec::iterator it = cnames.begin(); it != cnames.end(); ++it)
            setColBnds(*it, lb, ub);
    }
    else setColBnds(met, lb, ub);
}

void TfbaLP::update(){
    cleanTmpRows(nblocked);
    nblocked = 0;
    strvec reacs = getColNames(CONT);
    for (strvec::iterator it = reacs.begin(); it != reacs.end(); it+= 2)
        setFeasibleDirs(*it);

}


strvec TfbaLP::getBottleNecks(stomap* obj, strvec q, bool max, bool presolve){
    strvec rv;
    for (strvec::iterator it = q.begin(); it != q.end(); ++it){
        if (isBottleNeck(obj, *it, max, presolve))
            rv.push_back(*it);
    }
    return rv;  
}

bool TfbaLP::isBottleNeck(stomap* obj, string rx, bool max, bool presolve){
    bool rv = false;
    string constraint_name = rx + "_tblock";
    if (nrow(constraint_name)) {
        delRow(constraint_name);
        optimise(obj, max, presolve);
        rv = !is_zero(getObjValue());
        block(rx, true, constraint_name);
    }
    return rv;
}


int TfbaLP::getColKind(int i) const{
    int rv = SplitLP::getColKind(i);
    intmap::iterator it = types->find(i);
    if (it != types->end())
        rv = it->second;
    return rv;
}

double TfbaLP::getColValue(string name) const{
    double rv = SplitLP::getColValue(name);
    if (getColKind(ncol(name)) == LNCONC_TYPE){
        rv = exp(rv);
    }
    return rv;
}



void TfbaLP::addThermoConstraint(string rx){
    RANGE DeltaG0 = getDeltaG0(rx);
    string deltaname = DELTAG0_TAG + rx;
    int i = this->addCol(deltaname, DeltaG0.min, DeltaG0.max);
    (*types)[i] = DELTAG0_TYPE;
    stomap sto;
    sto[deltaname] = 1;
    stomap* iw = getColSto(rx);
    for (stomap::iterator it = iw->begin(); it != iw->end(); ++it)
        sto[it->first] = it->second * RT;
    delete iw;
    addConstraint(&sto, LB, 0, true, rx + CONC_SUFX);
}

int TfbaLP::addThermoConstraints(){
    int rv = 0;
    strvec reacs = getColNames(CONT);
    for (strvec::iterator it = reacs.begin(); it != reacs.end(); ++it){
        if (getColValue(*it) > 0){
            addThermoConstraint(*it);
            rv++;
        }
    }
    return rv;
}

void TfbaLP::TFBA(stomap* obj, bool max, bool presolve){
    optimise(obj, max, presolve);
//    int ntmp = fixSolution(CONT, true);
//    ntmp += addThermoConstraints();
//    Solve(presolve);
//    cleanTmpRows(ntmp);
//    strvec dGvars = getColNames(DELTAG0_TYPE);
//    for (strvec::iterator it = dGvars.begin(); it != dGvars.end(); ++it)
//        delCol(*it);
}
