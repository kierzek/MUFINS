#include "SplitLP.h"
#include <time.h>//w/

////////////////////////////////////////////////////////////////////////////////
//initialise, finalise

SplitLP::SplitLP(bool max, int kind, int msg_lev, int tm_lim, int it_lim, string name):MtxLP(max, kind, msg_lev, tm_lim, it_lim, name){
}


string SplitLP::get_opp_name(string name)const{
    return is_pos(name) ? NEG_TAG + name : name.substr(1);
}

void SplitLP::addStoich(stomap* sto, double lb, double ub, string name){
    string opp_name = get_opp_name(name);
    double ub_pos = (ub > 0) ? ub : 0;
    double ub_neg = (lb < 0) ? -lb : 0;

    MtxLP::addStoich(sto, 0, ub_pos, name);
    mul(sto, -1);
    MtxLP::addStoich(sto, 0, ub_neg, opp_name);    
}

void SplitLP::fix(string name, double value, bool tmp, string constraintname){
    MtxLP::fix(name, value, tmp, constraintname);
//    name = get_opp_name(name);
//    MtxLP::fix(name, 0, tmp);
}

void SplitLP::getSolution(stomap* sol, strvec q, int kind, bool withzeroes) const{
    MtxLP::getSolution(sol, q, kind, (kind == CONT) ? true : withzeroes);
    if (kind == CONT){
        strvec del(2 * sol->size());
        strvec pos(sol->size());
        strvec::iterator pt = pos.begin(), dt = del.begin();
        string name;
        int d = 0;
        for (stomap::iterator it = sol->begin(); it != sol->end(); ++it){
            name = it->first;
            if (is_pos(name))
                *pt++ = name;
            else {
                *pt++ = get_opp_name(name);
                *dt++ = name;
                d++;
            }
        }
        striset posset(pos.begin(), pos.end());
        for (striset::iterator pt = posset.begin(); pt != posset.end(); ++pt){
            name = *pt;
            double val = getColValue(name) - getColValue(get_opp_name(name));
            if (withzeroes || !is_zero(val))
                (*sol)[name] = val;
            else {
                *dt++ = name;
                d++;
            }
        }
        dt = del.begin();
        for (int i = 0; i <= d; i++)
            sol->erase(*dt++);
    }
 }

void SplitLP::elModes(solvec& solutions, int num, strvec& fixcols, bool withzeroes){
    srand (unsigned(time(NULL)));
    double fixrate = 1/this->vmax;
    for (strvec::iterator it = fixcols.begin(); it != fixcols.end(); ++it){
        fix(*it, fixrate, true);
        fix(get_opp_name(*it), 0, true);
    }
    int kind = getKind() == MILP ? INT : CONT;
    void (MtxLP::*optimise)(stomap*, bool, strvec, bool, bool) = \
        (kind == INT) ? &MtxLP::optLenSol : &MtxLP::optSumSol;
    int i;
    for (i = 0; i < num; i++){
        stomap sol;
        (this->*optimise)(&sol, false, getColNames(), false, true);
        if (sol.empty())
            break;
        if (fixrate != 0) mul(&sol, vmax);
        solutions.push_back(sol);
        if (i < num - 1){
            sol.clear();
            MtxLP::getSolution(&sol, getColNames(kind), kind, withzeroes);
            if (kind == INT) cutSolution(&sol, true);
            else {
                for (strvec::iterator it = fixcols.begin();  it != fixcols.end(); ++it)
                     sol.erase(*it);
                int nblock = rand() % sol.size();
                stomap::iterator itblock = sol.begin();
                for (int i = 0; i < nblock; i++)itblock++;
                string sblock = itblock->first;
                block(sblock, true);
            }
        }
    }
    cleanTmpRows();//fixcols.size() * 2 + i - 1);
}


string SplitLP::getTransporterName(string xname, int dir) const{
    stomap* rsto = getRowSto(xname);
    string txname = getTransporterName(rsto, dir);
    delete rsto;
    return txname;
}

string SplitLP::getTransporterName(stomap *rsto, int dir) const{
    stomap::iterator jt = rsto->begin();
    if ((jt->second / dir) > 0) ++jt;
    if (jt != rsto->end())
        return jt->first;
    return "";
}

void SplitLP::getConversion(stomap* comp, bool withzeroes) const{
    strvec externals = getExternals();
    for (strvec::iterator xi = externals.begin(); xi != externals.end(); ++xi){
        string xname = *xi;
        stomap *rsto = getRowSto(xname);
        string txname = rsto->begin()->first;   //getTransporterName(rsto, dir);
        string txopp = get_opp_name(txname);
        if(!is_pos(txname)){
            string mid = txname;
            txname = txopp;
            txopp = mid;
        }
        double coef = (*rsto)[txname];
        double flux = getColValue(txname) - getColValue(txopp);
        double conv = coef * flux * vmax;
        if (withzeroes || !is_zero(conv))
            (*comp)[xname] = conv;
        delete rsto;
    }
}

void SplitLP::minExchangeSets(solvec& solutions, stomap* obj, int dir, long nmax){
    strvec externals = getExternals();
    strvec txs(externals.size());// - obj->size());
    strvec::iterator ti = txs.begin();
    stomap::iterator oend = obj->end();
    for (strvec::iterator it = externals.begin(); it != externals.end(); ++it){
        string xname = *it;
        string txname = getTransporterName(xname, dir);
        stomap::iterator jt = obj->find(xname);
        if (jt != oend){
            double coef = jt->second;
            coef /= this->vmax;
            fix(txname, coef, true, txname + "_mxs");
        }
        else if (dir > 0){
                fix(txname, 0, true);
        }
        *ti++ = get_opp_name(txname);
    }
    setObjDir(false);
    setLenObjective(txs, true);

    strvec itxs = get_int_names(txs);

    long i = 0;
    while(i < nmax){
        stomap conv;
        stomap sol;

        Solve(true);
        getConversion(&conv);

        if (!isSolved())
            break;

        getSolution(&sol, itxs, INT, false);

        if (!conv.empty()){
            i++;
            if (dir < 0){
                minimiseInput(&conv, txs);
                setLenObjective(txs, true);
            }
            solutions.push_back(conv);
        }

        if (i < nmax)
            cutSolution(&sol, true);
    }

//    cleanTmpRows();
}


void SplitLP::minimiseInput(stomap* conv, strvec& txs){
    strvec externals = getExternals();
    stomap obj;
    int nblock = 0;

    stomap::iterator cend = conv->end();
    for (strvec::iterator it = externals.begin(); it != externals.end(); ++it){
        string tx = getTransporterName(*it, 1);
        if (tx.size() > 0){
            stomap::iterator ct = conv->find(*it);
            if (ct != cend && ct->second < 0){
                obj[tx] = 1;
            }
            else{
                block(tx, true);
                nblock++;
            }
        }
    }
    setObjective(&obj);
    Solve(true);
    conv->clear();
    getConversion(conv);
    cleanTmpRows(nblock);
}
