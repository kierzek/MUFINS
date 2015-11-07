#include "MtxLP.h"
#include <iostream>
#include <cstdio>

#include "time.h"

////////////////////////////////////////////////////////////////////////////////
//initialise, finalise, uodate
MtxLP::MtxLP(bool max, int kind, int msg_lev, int tm_lim, int it_lim, string name){
    createProb();
    setObjDir(max);
    setKind(kind);
    setName(name);
    this->ret_cod = 0;
    this->vmax = 1;
    this->msg_lev = msg_lev;
    this->tm_lim = tm_lim;
    this->it_lim = it_lim;
    this->mip_gap = MIP_GAP;
}
MtxLP::~MtxLP(){
    deleteProb();
}
void MtxLP::update(){
  externalise(tmp_externals, false);//internalise temporary externals
  tmp_externals.clear();//clear their list
}

////////////////////////////////////////////////////////////////////////////////
//create, delete problem
void MtxLP::createProb(){
    create_prob();
    create_index();
}
void MtxLP::deleteProb(){
    delete_index();
    delete_prob();
}

////////////////////////////////////////////////////////////////////////////////
//naming
void MtxLP::setName(string name){
    set_prob_name(name.data());
}
string MtxLP::getName() const{
    return get_prob_name();
}

////////////////////////////////////////////////////////////////////////////////
//setting the kind
void MtxLP::setKind(int kind){
    this->kind = kind;
    switch (kind){
        case EXACT:
        case SMPLX:{
            ptr_Solve = &MtxLP::Simplex;
            ptr_getObjValue = &MtxLP::get_obj_val;
            ptr_getColValue = &MtxLP::get_col_prim;
            ptr_getStatus = &MtxLP::get_status;
            break;
        }
        case INTPT:{
            ptr_Solve = &MtxLP::Interior;
            ptr_getObjValue = &MtxLP::ipt_obj_val;
            ptr_getColValue = &MtxLP::ipt_col_prim;
            ptr_getStatus = &MtxLP::ipt_status;
            break;
        }
        case MILP:{
            ptr_Solve = &MtxLP::Intopt;
            ptr_getObjValue = &MtxLP::mip_obj_val;
            ptr_getColValue = &MtxLP::mip_col_val;
            ptr_getStatus = &MtxLP::mip_status;
            break;
        }
    }
}

int MtxLP::getKind() const{
    return kind;
}

strvec MtxLP::get_int_names(strvec names){
    strvec rv(names.size());
    strvec::iterator it = names.begin(), it2 = rv.begin();
    while (it != names.end())
        *it2++ = get_int_name(*it++);
    return rv;
}


////////////////////////////////////////////////////////////////////////////////
//objective

void MtxLP::setObjDir(bool max){
    int ndir = max ? GLP_MAX : GLP_MIN;
    set_obj_dir(ndir);
}
bool MtxLP::isMaximising() const{
    int ndir = get_obj_dir();
    return ndir == GLP_MAX ? true : false;
}

double MtxLP::getObjCoef(string name) const{
    return get_obj_coef(ncol(name));
}

void MtxLP::getObjective(stomap* rv, bool withzeroes) const{
    for (int i = 1; i <= get_num_cols(); i++){
        double coef = get_obj_coef(i);
        if (withzeroes || coef != 0)
            (*rv)[get_col_name(i)] = coef;
    }
}

void MtxLP::setObjCoef(string name, double coef){
    int n = ncol(name);
    if (n == 0)
        throw runtime_error(string("no such column ") + name);
    set_obj_coef(ncol(name), coef);
}

void MtxLP::emptyObjective(){
    int ncols = get_num_cols();
    for (int i = 1; i <= ncols; i++)
        set_obj_coef(i, 0.0);
}



void MtxLP::setObjective(double coef){
    int ncols = get_num_cols();
    for (int i = 1; i <= ncols; i++)
        set_obj_coef(i, coef);
}

void MtxLP::setObjective(stomap* sto){
    for (stomap::iterator i = sto->begin(); i != sto->end(); ++i)
        setObjCoef(i->first, i->second);
}

void MtxLP::setObjective(strvec names, double coef){
    emptyObjective();
    for (strvec::iterator it = names.begin(); it != names.end(); ++it)
        setObjCoef(*it, coef);
}

void MtxLP::updateObjCoef(string col, double coef){
    coef += getObjCoef(col);
    setObjCoef(col, coef);
}

void MtxLP::updateObjective(stomap* sto, double coef){
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it)
        updateObjCoef(it->first, it->second * coef);
}

void MtxLP::setRowObjCoef(string row, double coef){
    stomap* rsto = getRowSto(row);
    this->updateObjective(rsto, coef);
    delete rsto;
}

void MtxLP::setMixedObjective(stomap* sto){
    emptyObjective();
    for (stomap::iterator i = sto->begin(); i != sto->end(); ++i){
        string name = i->first;
        if (nrow(name) != 0){
            setRowObjCoef(name, i->second);
            //if the metabolite is internal, externalise it and add to the list of temp. externals.
            if (!isExternal(name)){
                strvec buff (1, name);                          // Added by Andrzej Kierzek
                externalise(buff);                             // Added by Andrzej Kierzek
                tmp_externals.push_back(name);
            }
        }
        else
            updateObjCoef(i->first, i->second);
    }
}

void MtxLP::setLenObjective(strvec cols, bool tmp){
    if (kind != MILP)
        throw runtime_error("trying to optimise length using a non-MILP solver!");
    emptyObjective();
    int contcoef = isMaximising() ? 1 : -1;
    int intcoef = -contcoef;
    for (strvec::iterator it = cols.begin(); it != cols.end(); ++it){
        string name = *it;
        string intname = get_int_name(name);
        stomap *sto = new stomap;
        (*sto)[name] = contcoef;
        (*sto)[intname] = intcoef * vmax;
        addConstraint(sto, 0, UB, tmp, name + INT_CONSTR_TAG);
        setObjCoef(intname, 1);
        delete sto;
    }
}

void MtxLP::setLenObjective(bool tmp){
    strvec cols = getColNames();
    setLenObjective(cols, tmp);
}

void MtxLP::setVMax(double vmax){
    this->vmax = vmax;
}
////////////////////////////////////////////////////////////////////////////////
//adding rows and columns

int MtxLP::addRow(string name, double lb, double ub){
    int i = nrow(name);
    if (i == 0)
        i = add_rows(1);
    set_row_name(i, name.data());
    setRowBnds(name, lb, ub);
    return i;
}
int MtxLP::addCol(string name, double lb, double ub){
    int i = add_cols(1);
    set_col_name(i, name.data());
    setColBnds(name, lb, ub);
    return i;
}

void MtxLP::attachIntVar(string name){
    string intname = get_int_name(name);
    int i = addCol(intname);
    set_col_kind(i, INT);
}

void MtxLP::delRow(string name){
    int num[2];
    num[1] = nrow(name);
    del_rows(1, num);
}

void MtxLP::delCol(string name){
    int num[2];
    num[1] = ncol(name);
    del_cols(1, num);
}

int MtxLP::getNumRows() const{
    return get_num_rows();
}
int MtxLP::getNumCols() const{
    return get_num_cols();
}
strvec MtxLP::getRowNames() const{
    strvec rv;
    int nrows = get_num_rows();
    for (int i = 1; i <= nrows; i++)
        rv.push_back(get_row_name(i));
    return rv;
}
int MtxLP::getColKind(int i) const{
    return get_col_kind(i);
}
strvec MtxLP::getColNames(int kind) const{
    strvec rv;
    int ncols = get_num_cols();
    for (int i = 1; i <= ncols; i++){
        if (kind == getColKind(i))
            rv.push_back(get_col_name(i));
    }
    return rv;
}
strstack MtxLP::getTmpRows() const{
    return tmp_rows;
}
string MtxLP::newRowName(string name) const{
    return name.empty() ? string(ROW_TAG) + num2str(getNumRows() + 1) : name;
}
string MtxLP::newColName(string name) const{
    return name.empty() ? string(COL_TAG) + num2str(getNumCols() + 1) : name;
}

void MtxLP::setRowBnds(string name, double lb, double ub){
    int type = bounds2type(lb, ub);
    set_row_bnds(nrow(name), type, lb, ub);
}
void MtxLP::setColBnds(string name, double lb, double ub){
    //std::ostringstream strs;
    //strs << lb << " " << ub;
    //cout << "setColBnds(" << name << "," << strs.str() << ")" << endl;
    int type = bounds2type(lb, ub);
    set_col_bnds(ncol(name), type, lb, ub);
}

void MtxLP::externalise(strvec& names, bool on){
    double lb = on ? -UB : 0;
    double ub = on ? UB : 0;
    for (strvec::iterator it = names.begin(); it != names.end(); ++it){
        if (nrow(*it) != 0)
            setRowBnds(*it, lb, ub);
    }
}

int MtxLP::getRowType(string name) const{
    return get_row_type(nrow(name));
}
int MtxLP::getColType(string name) const{
    return get_col_type(ncol(name));
}
double MtxLP::getRowLB(string name)const{
    return get_row_lb(nrow(name));
}
double MtxLP::getRowUB(string name)const{
    return get_row_ub(nrow(name));
}
double MtxLP::getColLB(string name)const{
    return get_col_lb(ncol(name));
}
double MtxLP::getColUB(string name)const{
    return get_col_ub(ncol(name));
}

stomap* MtxLP::getRowSto(string name) const{
    stomap* rv = new stomap;
    int n = nrow(name);
    if (n == 0) throw runtime_error(name + ": no such row!");
    int len = get_num_cols();
    int *ind = new int[len + 1];
    double *val = new double[len + 1];
    len = get_mat_row(n, ind, val);

    for (int i = 1; i <= len; i++)
        (*rv)[get_col_name(ind[i])] = val[i];

    delete ind;
    delete val;

    return rv;
}

stomap* MtxLP::getColSto(string name) const{
    stomap* rv = new stomap;
    int n = ncol(name);
    if (n == 0) throw runtime_error(name + ": no such row!");
    int len = get_num_rows();
    int *inds = new int[len + 1];
    double *vals = new double[len + 1];
    len = get_mat_col(n, inds, vals);

    for (int i = 1; i <= len; i++){
        if (inds[i] <= mtxlen)
            (*rv)[get_row_name(inds[i])] = vals[i];
    }

    delete inds;
    delete vals;

    return rv;
}

bool MtxLP::isExternal(string met) const{
//whether the metabolite met is external  
    return getRowType(met) == GLP_FR;
}

strvec MtxLP::getExternals()const{
    strvec rv;
    strvec rows = getRowNames();
    for (strvec::iterator it = rows.begin(); it != rows.end(); ++it)
        if (isExternal(*it))
            rv.push_back(*it);
    return rv;
}



strvec MtxLP::getTransporters()const{
    strvec rv;
    strvec ext = getExternals();
    for (strvec::iterator it = ext.begin(); it != ext.end(); ++it){
        stomap *sto = getRowSto(*it);
        string tx = sto->begin()->first;
        rv.push_back(tx);
        delete sto;
    }
    return rv;
}

strvec MtxLP::getImporters()const{
    strvec rv;
    strvec ext = getExternals();
    for (strvec::iterator it = ext.begin(); it != ext.end(); ++it){
        stomap *sto = getRowSto(*it);
        for (stomap::iterator jt = sto->begin(); jt != sto->end(); ++jt)
            if (jt->second < 0)
                rv.push_back(jt->first);
        delete sto;
    }
    return rv;
}

strvec MtxLP::getExporters()const{
    strvec rv;
    strvec ext = getExternals();
    for (strvec::iterator it = ext.begin(); it != ext.end(); ++it){
        stomap *sto = getRowSto(*it);
        stomap::iterator first = sto->begin();
        for (stomap::iterator jt = sto->begin(); jt != sto->end(); ++jt)
            if (jt->second > 0)
                rv.push_back(jt->first);
        delete sto;
    }
    return rv;
}

void MtxLP::addConstraint(stomap* sto, double lb, double ub, bool tmp, string name){
    //cout << *sto << ' ' << lb << ' ' << ub << endl;
    name = newRowName(name);
    int len = sto->size();
    int* ind = new int[len + 1];
    double* val = new double[len + 1];
    int i = 1;
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){
        string col = it->first;
        int n = ncol(col);
        if (n == 0)
            throw runtime_error(string("no column ") + col);
        ind[i] = n;
        val[i] = it->second;
        i++;
    }
    int j = addRow(name, lb, ub);
    set_mat_row(j, len, ind, val);
    delete [] ind;
    delete [] val;
    if (tmp) tmp_rows.push(name);
}

void MtxLP::setColRange(string name, double lb, double ub, bool tmp){
    stomap sto;
    sto[name] = 1;
    addConstraint(&sto, lb, ub, tmp);
}

void MtxLP::fix(string name, double value, bool tmp, string constraintname){
    stomap sto;
    sto[name] = 1;
    addConstraint(&sto, value, value, tmp, constraintname);
}
void MtxLP::fix(stomap* sto, bool tmp){
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it)
        fix(it->first, it->second, tmp);
}

void MtxLP::block(string name, bool tmp, string constraintname){
    fix(name, 0, tmp, constraintname);
}
void MtxLP::block(strvec* names, bool tmp){
    for (strvec::iterator it = names->begin(); it != names->end(); ++it)
        block(*it, tmp);
}

int MtxLP::fixSolution(int kind, bool tmp){
    if (!isSolved()) return 0;
    strvec cnames = getColNames(kind);
    for (strvec::iterator i = cnames.begin(); i != cnames.end(); i++)
        fix(*i, getColValue(*i), tmp);
    return cnames.size();
}

int MtxLP::fixSupport(int kind, bool tmp){
    int rv = 0;
    if (!isSolved()) return 0;
    strvec cnames = getColNames(kind);
    for (strvec::iterator i = cnames.begin(); i != cnames.end(); i++){
        if (is_zero(getColValue(*i))){
            block(*i, tmp);
            rv++;
        }
    }
    return rv;
}

void MtxLP::cutSolution(stomap *sol, bool tmp){
    double colsum = 0;
    stomap sto;
    for (stomap::iterator it = sol->begin(); it != sol->end(); ++it){
        colsum += it->second;
        sto[it->first] = 1;
    }
    double dif = getKind() == MILP ? 1 : 0.1;
    addConstraint(&sto, 0, colsum - dif, tmp);
}

void MtxLP::addStoich(stomap* sto, double lb, double ub, string name){
    name = newColName(name);
    int len = sto->size();
    int* ind = new int[len + 1];
    double* val = new double[len + 1];
    int j = addCol(name, lb, ub);
    int i = 1;
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){
        string row = it->first;
        int n = nrow(row);
        if (n == 0) n = addRow(row);
        ind[i] = n;
        val[i] = it->second;
        i++;
    }
    set_mat_col(j, len, ind, val);
    delete [] ind;
    delete [] val;

    if (getKind() == MILP)
        attachIntVar(name);
}

void MtxLP::addMatrix(Matrix* N){
    for (strvec::const_iterator it = N->cols.begin(); it != N->cols.end(); ++it){
        string name = *it;
        Element* col = (*N)[name];
        addStoich(col->get_sto(), col->get_lb(), col->get_ub(), name);
    }
    this->mtxlen = N->rows.size();
}

void MtxLP::cleanTmpRows(int n){
    int size = tmp_rows.size();
    if (n < 0) n = size;
    else if (n > size) throw runtime_error("trying to delete too many temp. rows!");
    for (int i = 0; i < n; i++){
        delRow(tmp_rows.top());
        tmp_rows.pop();
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//solving
void MtxLP::Solve(bool presolve){
    this->ret_cod = (*this.*ptr_Solve)(presolve);
}

int MtxLP::Simplex(bool presolve){
    int rv;
    glp_smcp parm;
    init_smcp(&parm);
    parm.presolve = presolve;
    parm.msg_lev = msg_lev;
    parm.it_lim = it_lim;
    parm.tm_lim = tm_lim;
    rv = simplex(&parm);
    if (kind == EXACT)
        rv = exact(&parm);
    return rv;
}

int MtxLP::Interior(bool presolve){
    glp_iptcp parm;
    init_iptcp(&parm);
    parm.msg_lev = msg_lev;
    return interior(&parm);
}

int MtxLP::Intopt(bool presolve){
    int rv;
    glp_iocp parm;
    init_iocp(&parm);
    parm.presolve = presolve;
    parm.msg_lev = msg_lev;
    parm.tm_lim = tm_lim;
    parm.mip_gap = mip_gap;
    parm.fp_heur = true;
    parm.gmi_cuts = true;
    parm.mir_cuts = true;
    parm.clq_cuts = true;
    parm.cov_cuts = true;
    rv = intopt(&parm);
    return rv;
}

//int MtxLP::Exact(bool presolve){
//    glp_smcp parm;
//    init_smcp(&parm);
//    parm.presolve = presolve;
//    parm.msg_lev = msg_lev;
//    parm.it_lim = it_lim;
//    parm.tm_lim = tm_lim;
//    simplex(&parm);
//    return exact(&parm);
//}

///////////////////////////////////////////////////////////////////////////////
//retrieving solution
bool MtxLP::isSolved() const{
    return ret_cod == GLP_EMIPGAP || isStatusOptimal();
}

void MtxLP::getSolution(stomap* sol, strvec q, int kind, bool withzeroes) const{
    if (!isSolved()) return;
    for (strvec::iterator i = q.begin(); i != q.end(); i++){
        string name = *i;
        double coef = getColValue(name);
        if (withzeroes || !is_zero(coef))
            (*sol)[name] = coef;
    }
}

void MtxLP::getSolution(stomap* sol, int kind, bool withzeroes) const{
    strvec cnames = getColNames(kind);
    getSolution(sol, cnames, kind, withzeroes);
}

void MtxLP::getSupport(strvec& support, strvec queries, int kind) const{
    stomap sol;
    getSolution(&sol, queries, kind, false);
    support.resize(sol.size());
    strvec::iterator jt = support.begin();
    for (stomap::iterator it = sol.begin(); it != sol.end(); ++it)
        *jt++ = it->first;
}

void MtxLP::getSupport(strvec& support, int kind) const{
    strvec cnames = getColNames(kind);
    getSupport(support, cnames, kind);
}

double MtxLP::getObjValue() const{
    double rv = (*this.*ptr_getObjValue)();
    return is_zero(rv) ? 0.0 : rv;
}

void MtxLP::getObjStat(OBJSTAT& objstat){
    objstat.objval = getObjValue();
    objstat.status = getStatus();
}
///////////////////////////////////////////////////////////////////////////////////////
//specific problems

void MtxLP::optimise(stomap* targ, bool max, bool presolve){
    setMixedObjective(targ);
    setObjDir(max);
    Solve(presolve);
}


void MtxLP::optimiseSum(bool max, strvec q, bool presolve){
    setObjDir(max);
    if (q.empty())
        q = getColNames();
    setObjective(q);
    Solve(presolve);
}

void MtxLP::optSumSol(stomap* sol, bool max, strvec queries, bool withzeroes, bool presolve){
    optimiseSum(max, queries, presolve);
    getSolution(sol, queries, CONT, withzeroes);
}

void MtxLP::optimiseLen(bool max, strvec q, bool presolve){
    if (q.empty())
        q = getColNames();
    setObjDir(max);
    setLenObjective(q, true);
    Solve(presolve);
    cleanTmpRows(q.size());
}

void MtxLP::optLenSol(stomap* sol, bool max, strvec queries, bool withzeroes, bool presolve){
    optimiseLen(max, queries, presolve);
    getSolution(sol, queries, CONT, withzeroes);
}


double MtxLP::getOptVal(stomap* targ, bool max, bool presolve){
    optimise(targ, max, presolve);
    return getObjValue();
}

void MtxLP::getOptObjStat(OBJSTAT& objstat, stomap* targ, bool max, bool presolve){
    objstat.objval = getOptVal(targ, max, presolve);
    objstat.status = getStatus();
}

void MtxLP::getOptSol(stomap* sol, stomap* targ, bool max, int kind, bool withzeroes, bool presolve){
    optimise(targ, max, presolve);
    getSolution(sol, kind, withzeroes);
}



void MtxLP::getRange(OBJSTAT_RANGE& range, stomap* targ, bool presolve){
    getOptObjStat(range.min, targ, false, presolve);
    getOptObjStat(range.max, targ, true, presolve);
}

void MtxLP::getColRange(OBJSTAT_RANGE& range, string cname, bool presolve){
    stomap targsto;
    targsto[cname] = 1;
    getRange(range, &targsto, presolve);
}

void MtxLP::getRanges(rangemap* rm, strvec q, bool presolve){
    OBJSTAT_RANGE range;
    stomap targsto;
    for (strvec::iterator it = q.begin(); it != q.end(); ++it){
        string name = *it;
        getColRange(range, name, presolve);
        (*rm)[name] = range;
    }
}

strvec MtxLP::getUnblockedVars(strvec q, bool presolve){
    strvec rv;
    for (strvec::iterator it = q.begin(); it != q.end(); ++it){
        if (isUnBlocked(*it, presolve))
            rv.push_back(*it);
    }
    return rv;
}

bool MtxLP::isUnBlocked(string name, bool presolve){
    bool rv = false;
    stomap obj;
    obj[name] = 1;
    OBJSTAT objstat;
    getOptObjStat(objstat, &obj, true, presolve);
    rv = objstat.status == GLP_OPT  && !is_zero(objstat.objval);
    if (!rv){
        getOptObjStat(objstat, &obj, false, presolve);
        rv = objstat.status == GLP_OPT  && !is_zero(objstat.objval);
    }
    return rv;
}

void MtxLP::fixSolution(stomap* targ, bool max, bool tmp, bool withzeroes, bool presolve, double scale){
    optimise(targ, max, presolve);
    for (stomap::iterator i = targ->begin(); i != targ->end(); i++){
        string name = i->first;
        if (ncol(name) != 0){
            double coef = getColValue(name);
            if (withzeroes || !is_zero(coef))
                fix(name, coef * scale, tmp);
        }
        else{
            stomap* rsto = getRowSto(name);
            for (stomap::iterator j = rsto->begin(); j != rsto->end(); j++){
                string name = j->first;
                double coef = getColValue(name);
                if (withzeroes || !is_zero(coef))
                    fix(name, coef * scale, tmp);
            }
            delete rsto;
        }
    }
}

void MtxLP::getOptRanges(rangemap* rm, OBJSTAT &objrange, stomap* obj, strvec q, bool presolve){
    fixSolution(obj, true, true, true, presolve);
    getObjStat(objrange);
    getRanges(rm, q, presolve);
    cleanTmpRows(obj->size());
}


void MtxLP::plot(pointvec* plot, stomap* obj, PLOT_ARGS yargs, bool max, bool presolve){
    POINT point(2);
    for (double y = yargs.from; y <= yargs.to; y += yargs.step){
        fix(yargs.query, y, true);
        point[0] = y;
        point[1] = getOptVal(obj, max, presolve);
        plot->push_back(point);
        cleanTmpRows(1);
    }
}

void MtxLP::plot3D(pointvec* plot, stomap* obj, PLOT_ARGS xargs, PLOT_ARGS yargs, bool max, bool presolve){
    POINT point(3);
    for (double x = xargs.from; x <= xargs.to; x += xargs.step){
        //fix(xargs.query, x, true);
        setColRange(xargs.query, x - xargs.step, x, true);
        point[0] = x;
        for (double y = yargs.from; y <= yargs.to; y += yargs.step){
            //fix(yargs.query, y, true);
            setColRange(yargs.query, y - yargs.step, y, true);
            point[1] = y;
            point[2] = getOptVal(obj, max, presolve);
            plot->push_back(point);
            cleanTmpRows(1);
        }
        cleanTmpRows(1);
    }
}

bool MtxLP::isEssential(stomap* obj, string col, bool max, bool presolve){
    block(col, true);
    optimise(obj, max, presolve);
    cleanTmpRows(1);
    return is_zero(getObjValue());
}

bool MtxLP::isCutSet(stomap* obj, strvec set, bool max, bool presolve){
    block(&set, true);
    optimise(obj, max, presolve);
    cleanTmpRows(set.size());
    return is_zero(getObjValue());
}

void MtxLP::ko(OBJSTAT& rv, stomap* obj, strvec blockvec, bool max, bool presolve){
    block(&blockvec, true);
    getOptObjStat(rv, obj, max, presolve);
    cleanTmpRows(blockvec.size());
}

void MtxLP::ko(objstatvec& rv, stomap* obj, strlistvec blockvecs, bool max, bool presolve){
    OBJSTAT objstat;
    for (strlistvec::iterator it = blockvecs.begin(); it != blockvecs.end(); ++it){
        ko(objstat, obj, *it, max, presolve);
        rv.push_back(objstat);
    }
}

strvec MtxLP::getEssentials(stomap* obj, strvec q, bool max, bool presolve){
    strvec rv;
    for (strvec::iterator it = q.begin(); it != q.end(); ++it){
        if (isEssential(obj, *it, max, presolve))
            rv.push_back(*it);
    }
    return rv;
}


bool MtxLP::isBottleNeck(stomap* obj, string rx, bool max, bool presolve){
    double lb = getColLB(rx), ub = getColUB(rx);
    setColBnds(rx, LB, UB);
    optimise(obj, max, presolve);
    bool rv = !is_zero(getObjValue());
    setColBnds(rx, lb, ub);
    return rv;
}

strvec MtxLP::getBottleNecks(stomap* obj, strvec q, bool max, bool presolve){
    strvec rv;
    for (strvec::iterator it = q.begin(); it != q.end(); ++it){
        if (isBottleNeck(obj, *it, max, presolve))
            rv.push_back(*it);
    }
    return rv;
}

strvec MtxLP::unconsRows(bool presolve){
    strvec uncons;
    stomap* sol = new stomap;
    optLenSol(sol, true, getColNames(), true, presolve);
    for (stomap::iterator it = sol->begin(); it != sol->end(); ++it)
        if (it->second <= 0)
            uncons.push_back(it->first);
    delete sol;
    return uncons;
}

//////////////////////////////////////////////////////////////////////////////
//IO
int MtxLP::writeLP(string filename){
    return write_lp(filename.data());
}
int MtxLP::writeMPS(string filename){
    return write_mps(filename.data());
}


