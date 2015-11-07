#include <map>

#include "Driver.h"
#include "Types.h"
#include <time.h>
#include "MetabolismClient.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////
//initialisation
constmap MSG_DICT;

void Driver::init_msg() {
    MSG_DICT[GLP_OPT] = "OPTIMAL";
    MSG_DICT[GLP_FEAS] = "FEASIBLE";
    MSG_DICT[GLP_INFEAS] = "INFEASIBLE";
    MSG_DICT[GLP_NOFEAS] = "NON-FEASIBLE";
    MSG_DICT[GLP_UNBND] = "UNBOUNDED";
    MSG_DICT[GLP_UNDEF] = "UNDEFINED";
}

string Driver::get_msg(int nmsg) {
    return MSG_DICT[nmsg];
}

void Driver::init_args() {
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
}

int Driver::get_arg(string arg) {
    //    *out << COMMENT << "get_arg()" << endl;
    argmap::const_iterator it = ARGMAP.find(arg);
    if (it == ARGMAP.end())
        throw runtime_error(arg + ": unknown argument!");
    return it->second;
}

Driver::Driver() {
    out = &cout;
    err_msg = "Error: ";
    withcomments = WITHCOMMENTS; //include comments into the output
    write = false;

    problem = PROBLEM;
    max = DEF_MAX;
    kind = SMPLX; //MILP;//INTPT;//                //the solver algorithm is MILP
    presolve = true; //the presolver will be used
    msg_lev = MSG_LEV; //text output switched off
    tm_lim = TM_LIM; //the number of iterations is unlimited

    model = NULL;
    lp = NULL;
    N = new Matrix;
    records = new strvec;
    obj = new stomap;

    dosplit = false;
    dotrim = false;
    thermo = false;
    T = TSt;
    cmin = CMIN_;
    cmax = CMAX;
    mip_gap = MIP_GAP;
    vmax = VMAX;
    xtag = EXT_TAG_3;

    init_args();
    init_msg();
    init_operators();
}

Driver::~Driver() {
    if (model != NULL) delete model;
    if (lp != NULL) delete lp;
    if (out != &cout) delete out;
    if (records != NULL) delete records;
    if (obj != NULL) delete obj;
    if (client != NULL) delete client;
}


//void Driver::set_msg_lev(char* optarg){
//    msg_lev = get_arg(string(optarg));
//}
//
//void Driver::set_solver(char *arg){
//    kind = get_arg(string(arg));
//}
//
//void Driver::set_output(char* arg){
//    out = new ofstream(arg);
//}
//
//void Driver::set_T(char* optarg){
//    T = T0 + atof(optarg);
//    if (T < 0) throw runtime_error(err_msg + "temperature must be >= 0!");
//}
//
//void Driver::set_conc_range(char* arg){
//    strvec sminmax;
//    string is(arg);
//    split(&is, &sminmax, SPACE);
//    cmin = log(str2double(sminmax[0]));
//    if (sminmax.size() > 1)
//        cmax = log(str2double(sminmax[1]));
//}
///////////////////////////////////////////////////////////////////////////////////////////////
//reading text files

void Driver::read_model() {
    //    *out << COMMENT << "read_model()" << endl;

    //parsing the input
    if (infile.empty())
        throw runtime_error(err_msg + "no input file specified!");
    model = new Model;
    parse_model(infile, model, xtag);
}

void Driver::read_bfile() {
    //    *out << COMMENT << "read_bfile()" << endl;
    if (bfile.empty())
        records->push_back(CONTINUE);
    else {
        ifstream is(bfile.data());
        split(is, records, ENDOFRECORD);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//building LP

void Driver::build() {
    //    *out << COMMENT << "build()" << endl;
    if (problem == "tfba")lp = new TfbaLP(max, T, msg_lev, tm_lim, cmin, cmax);
    else if (thermo)lp = new ThermoLP(max, T, msg_lev, tm_lim, cmin, cmax);
    else if (dosplit)lp = new SplitLP(max, kind, msg_lev, tm_lim);
    else lp = new MtxLP(max, kind, msg_lev, tm_lim);
    lp->setMipGap(mip_gap);
    lp->setVMax(vmax);

    if (dotrim) {
        N->trim(externals);
        //cout << N->cols << endl;
    }
    lp->addMatrix(N);
    lp->externalise(externals);
    set_objective(target);
}

void Driver::set_objective(string line) {
    //    *out << COMMENT << "set_objective(" << line << ")" << endl;
    if (line.empty()) return;
    objectiveString = line;
    obj->clear();
    parse_sto(line, obj);

    //*out << COMMENT << "updateLP()\n";
    lp->update();
}

void Driver::set_args(string line) {
    //    *out << COMMENT << "set_args(" << line << ")" << endl;
    //    if (line.empty()) return;
    args.clear();
    split(&line, &args, SPACE);
}

//void Driver::set_externals(string line, bool on){
////input: line of space-separated metabolite names, boolean
////externalise if on=true, internalise if on=false
//    strvec vec;    
//    line.erase(line.find_last_not_of(" \n\r\t")+1);//trim the whitespaces
//    split(&line, &vec, SPACE);//parse the list
//    if (vec.empty())vec = lp->getRowNames();//if the list is empty, apply to all metabolites
//    lp->externalise(vec, on);//done
//}
//
//strvec Driver::get_args(strvec& args, int type){
//    if (args.empty())
//        args = lp->getColNames(type);
//    else if (dotrim){
//        strvec rv;
//        elmap *map = N->map;
//        elmap::const_iterator end = map->end();
//        for (strvec::iterator it = args.begin(); it != args.end(); ++it){
//            if (map->find(lp->getID(*it)) != end)
//                rv.push_back(*it);
//        }
//        args = rv;
//    }
//    return args;
//}
//
//void Driver::get_list_args(strlistvec& rv, strvec& args, char delim){
//    if (args.empty()){
//        strvec genes = model->get_genes();
//        for (strvec::iterator it = genes.begin(); it != genes.end(); ++it){
//            strvec vec(1);
//            *vec.begin() = *it;
//            rv.push_back(vec);
//        }
//        strvec reacs = lp->getColNames(CONT);
//        for (strvec::iterator it = reacs.begin(); it != reacs.end(); ++it){
//            strvec vec(1);
//            *vec.begin() = *it;
//            rv.push_back(vec);
//        }
//    }
//    else{
//        for (strvec::iterator it = args.begin(); it != args.end(); ++it){
//            strvec arglist;
//            split(&*it, &arglist, delim);
//            rv.push_back(arglist);
//        }
//    }
//}
//
//void Driver::parse_plot_args(PLOT_ARGS &pa, strvec::const_iterator &it){
//    pa.query = *it++;
//    pa.from = str2double(*it++);
//    pa.to = str2double(*it++);
//    pa.step = str2double(*it++);
//    if ((pa.from > pa.to && pa.step > 0)) throw runtime_error("invalid range, from > to!");
//    if ((pa.from < pa.to && pa.step < 0)) throw runtime_error("invalid range, from < to!");
//}

///////////////////////////////////////////////////////////////////////////////////////////////
//running

//void Driver::run(){
//    try{
//        void (Driver::*ptr_solve)();
//
//        read_model();
//        read_bfile();
//        set_args(arg);
//
//        //non LP problems
//        if (problem == "show"){
//            *out << *model;
//            return;
//        }
//        else if (problem == "orphans"){
//            model->get_N(N);
//            strvec orphans;
//            N->get_orphans(orphans, args, externals);
//            if (withcomments) *out << COMMENT << orphans.size() << " orphans." << endl;
//            model->print_vec(orphans, out, withcomments);
//            *out << ENDOFRECORD << endl << endl;
//            return;
//        }
//        else if (problem == "cc"){
//            model->get_Nx(N);
//            strvec adj;
//            mtxvec comps;
//            N->connected_components(comps);
//            for (mtxvec::iterator it = comps.begin(); it != comps.end(); ++it){
//                MATRIX comp = *it;
//                if (withcomments) *out << COMMENT << comp.cols.size() << " reactions. " << endl;
//                model->print_vec(comp.cols, out, withcomments);
//                *out << endl;
////                if (withcomments) *out << COMMENT << comp.rows.size() << " metabolites." << endl;
////                model->print_vec(comp.rows, out, withcomments);
////                *out << endl;
//            }
//            *out << ENDOFRECORD << endl << endl;
//            return;
//        }
//        //LP problems
//        if (problem == "uncons"){
////            split = false;
//            kind = MILP;
//            model->get_Nt(N);
//            ptr_solve = &Driver::uncons;
//        }
//        else {
//            model->get_Nx(N);
//
//            if (ext.empty()) model->get_externals(externals);
//            else split(&ext, &externals, SPACE);//parse the list of externals supplied with the command line
//
//            if (problem == "objvalue")
//                ptr_solve = &Driver::objval;
//            else if (problem == "objstat")
//                ptr_solve = &Driver::objstat;
//            else if (problem == "fba")
//                ptr_solve = &Driver::fba;
//            else if (problem == "ko")
//                ptr_solve = &Driver::ko;
//            else if (problem == "fva")
//                ptr_solve = &Driver::fva;
//            else if (problem == "plot")
//                ptr_solve = &Driver::plot;
//            else if (problem == "plot3d")
//                ptr_solve = &Driver::plot3D;
//            else if (problem == "em"){
//                dosplit = true;
//                ptr_solve = &Driver::elmode;
////                if (kind == SMPLX) kind = EXACT;
//            }
//            else if (problem == "live")
//                ptr_solve = &Driver::live;
//            else if (problem == "ess")
//                ptr_solve = &Driver::essentials;
//            else if (problem == "mss"){
//                dosplit = true;
//                ptr_solve = &Driver::mss;
//                kind = MILP;
//            }
//            else if (problem == "mps"){
//                dosplit = true;
//                ptr_solve = &Driver::mps;
//                kind = MILP;
//            }
//
////            else if (problem == "tfba"){
////                thermo = true;
////                ptr_solve = &Driver::tfba;
////            }
//            else if (problem == "bottlenecks")
//                ptr_solve = &Driver::bottlenecks;
//            else if (problem == "tmfa"){
//                thermo = true;
//                ptr_solve = &Driver::tmfa;
//            }
//            else if (problem == "tva"){
//                thermo = true;
//                ptr_solve = &Driver::tva;
//            }
//            else if (problem == "test"){
//                //thermo = true;
//                ptr_solve = &Driver::test;
//            }
//            else
//                throw runtime_error(string("No such program: ") + problem);
//        }
//
//
//        build();
//
//        //if (withcomments) *out << COMMENT << problem << endl << endl;
//        time_t t0 = time(NULL);
//        for (strvec::iterator it = records->begin(); it != records->end(); ++it){
//            if (read_record(this, *it)){
//                //if (withcomments) *out << COMMENT << *obj << TAB << args << endl;
//                lp->update();
//                (*this.*ptr_solve)();
//                *out << ENDOFRECORD << endl << endl;
//                if (write)
//                    lp->writeLP("LP.txt");
//            }
//        }
//        if (withcomments) *out << COMMENT << "time spent: " << time(NULL) - t0  << endl;
//        out->flush();
//    }
//    catch (exception &e){
//        cerr << e.what() << endl;
//    }
//}

//////////////////////////////////////////////////
// Added by Andrzej Kierzek

void Driver::buildLP() {
    //    *out << COMMENT << "buildLP()" << endl;
    read_model();
    read_bfile();
    set_args(arg);
    model->get_Nx(N);
    if (ext.empty()) model->get_externals(externals);
    else split(&ext, &externals, SPACE); //parse the list of externals supplied with the command line
    build();
    lp->update();
}

double Driver::calcObjValue() {
    //    *out << COMMENT << "calcObjValue()" << endl;
    double res = lp->getOptVal(obj, max, presolve);
    //    *out << COMMENT << "calcObjValue()=" << res << endl;
    return res;
    //    return lp->getOptVal(obj, max, presolve);
}

void Driver::set_server_address(std::string adr, std::string port) {
    is_client = true;
    serverAddress = adr;
    serverPort = port;
    client = new PNFBA::MetabolismClient(serverAddress, serverPort);
}

bool Driver::isClient() {
    return is_client;
}

void Driver::add_change(std::string name, double lo, double hi) {
    if (isClient()) {
        std::ostringstream strs;
        strs << lo << " " << hi;
        //*out << COMMENT << "add_change(" << name << "," << strs.str() << ")" << endl;
        changesMap[name] = strs.str();
    } else {
        resetMapLo[name] = lp->getColLB(name);
        resetMapHi[name] = lp->getColUB(name);
    }
    lp->setColBnds(name, lo, hi);
}

void Driver::reset_changes() {
    for (std::map<std::string, double>::iterator it = resetMapLo.begin(); it != resetMapLo.end(); ++it) {
        lp->setColBnds(it->first, it->second, resetMapHi[it->first]);
    }
    if (isClient()) {
        //should never execute
        std::cerr << "warning executing reset_changes on client\n";
        changesMap.clear();
    } else {
        resetMapLo.clear();
        resetMapHi.clear();
    }
}

std::string Driver::changesMapToString() {
    std::ostringstream strs;
    strs << objectiveString << endl;
    strs << changesMap.size() << endl;

    for (std::map<std::string, std::string>::iterator it = changesMap.begin(); it != changesMap.end(); ++it) {
        strs << it->first << TAB << it->second << endl;
    }

    return strs.str();
}

void Driver::restoreChangesMapAndUpdateLP(std::string str) {
    changesMap.clear();
    std::istringstream strs(str);
    int len;
    std::string line;
    std::string name;
    std::string value;
    std::string lo;
    std::string hi;
    int tabPos;

    getline(strs, objectiveString);
    set_objective(objectiveString);
    strs >> len;
    getline(strs, line); //read the new line

    while (len-- > 0) {
        getline(strs, line);
        tabPos = line.find(TAB);
        name = line.substr(0, tabPos);
        value = line.substr(tabPos + 1);
        changesMap[name] = value;
        tabPos = value.find(' ');
        lo = value.substr(0, tabPos);
        hi = value.substr(tabPos + 1);
        //*out << COMMENT << "name=" << name << " value="  << value << " lo="<< lo << " hi=" << hi << endl;
        //*out << COMMENT << "name=" << name << " value="  << value << " lo="<< std::stod(lo, 0) << " hi=" << std::stod(hi, 0) << endl;
        lp->setColBnds(name, std::stod(lo, 0), std::stod(hi, 0));
    }
}

double Driver::calcObjValueBuff() {
    //    *out << COMMENT << "calcObjValueBuff()" << endl;

    //TODO comunicate with serverAddress
    //std::string m1 = changesMapToString();
    //restoreChangesMap(m1);
    //std::string m2 = changesMapToString();
    //*out << endl;
    //*out << m1 << endl;
    //*out << m2 << endl;
    //*out << endl;

    //return client->runMetabolism(changesMapToString());
    double res = client->runMetabolism(changesMapToString());
    //    *out << COMMENT << "calcObjValueBuff()" << res << endl;
    return res;
}

///////////////////////////////////////////////////////////////////////////////////
//solvers
//void Driver::objval(){
//    *out << lp->getOptVal(obj, max, presolve);
//}
//
//void Driver::objstat(){
//    OBJSTAT objstat;
//    lp->getOptObjStat(objstat, obj, max, presolve);
//    *out << objstat;
//}
//
//void Driver::fba(){
//    stomap* sol = new stomap;
//    lp->getOptSol(sol, obj, max, (kind == MILP) ? INT : CONT, false, presolve);
//    if (withcomments) *out << COMMENT << *lp << ' ' << '(' << *obj << ')' << endl;
//    model->print_sto(sol, out, withcomments);
//    delete sol;    
//}
//
//void Driver::ko(){
//    objstatvec ovec;
//    strlistvec qlists, blocklists;
//    get_list_args(qlists, args);
//    model->reaclists_to_block(blocklists, qlists);
//    lp->ko(ovec, obj, blocklists, max, presolve);
//    for (int i = 0; i < ovec.size(); i++){
//        *out << qlists[i] << TAB << ovec[i];
//        if (withcomments) *out << TAB << COMMENT << blocklists[i];
//        *out << endl;
//    }
//}
//
//void Driver::fva(){
//    args = get_args(args);
//    va();
//}
//
//void Driver::va(){
//    rangemap *rm = new rangemap;
//    OBJSTAT objrange;
//    if (obj->size() > 0)
//        lp->getOptRanges(rm, objrange, obj, args, presolve);
//    else
//        lp->getRanges(rm, args, presolve);
//    if (withcomments) *out << COMMENT << objrange << " (" << *obj << ')' << endl;
//    print_rangemap(out, rm, model);
//    delete rm;
//}
//
//void Driver::plot(){
//    if (args.size() != 4) throw runtime_error("invalid list of arguments, must be 'query from to step'!");
//    pointvec *plot = new pointvec;
//    strvec::const_iterator it = args.begin();
//    PLOT_ARGS yargs;
//    parse_plot_args(yargs, it);
//    lp->plot(plot, obj, yargs, max, presolve);
//    if (withcomments) *out << COMMENT << yargs.query << SPACE << TAB << *obj << endl;
//    *out << *plot;
//    delete plot;
//}
//
//void Driver::plot3D(){
//    if (args.size() != 8) throw runtime_error("invalid list of arguments, must be 'query from to step query from to step'!");
//    pointvec *plot = new pointvec;
//    strvec::const_iterator it = args.begin();
//    PLOT_ARGS xargs;
//    parse_plot_args(xargs, it);
//    PLOT_ARGS yargs;
//    parse_plot_args(yargs, it);
//    lp->plot3D(plot, obj, xargs, yargs, max, presolve);
//    if (withcomments) *out << COMMENT << xargs.query << SPACE << TAB << yargs.query << SPACE << TAB << *obj << endl;
//    *out << *plot;
//    delete plot;
//}
//
//void Driver::uncons(){
//    strvec uncons = lp->unconsRows(presolve);
//    if (withcomments) *out << COMMENT << uncons.size() << " unconserved metabolites." << endl;
//    model->print_vec(uncons, out, withcomments);
//}
//
//void Driver::elmode(){
//    if (kind == MILP) vmax = VMAX;
//    long num = INT_MAX;
//    switch(args.size()){
//        case 0: case 1: break;
//        case 3:
//            vmax = str2double(args[2]);
//            if (vmax < 1)
//                throw runtime_error(err_msg + "vmax must be >= 1");
//            lp->setVMax(vmax);
//            args.pop_back();
//        case 2:
//            num = (int)str2double(args[1]);
//            if (num < 0) num = INT_MAX;
//            args.pop_back();
//            break;
//        default:
//            throw runtime_error(err_msg + "invalid number of arguments, must be 0, 1 or 2!");
//    }
//    solvec solutions;
//    int n = 1;
//    ((SplitLP*)lp)->elModes(solutions, num, args);
//    for (solvec::iterator it = solutions.begin(); it != solutions.end(); ++it){
//        stomap sol = *it;
//        if (withcomments) *out << COMMENT << n++ << ": " << sol.size() << " reactions" << " (" << args << "):" << endl;
//        model->print_sto(&sol, out, withcomments);
//        *out << endl;
//    }
//}
//
//void Driver::live(){
//    args = get_args(args);
//    strvec lives = lp->getUnblockedVars(args, presolve);
//    if (withcomments) *out << COMMENT << lives.size() << " live reactions." << endl;
//    model->print_vec(lives, out, withcomments);
//}
//
//void Driver::essentials(){
//    args = get_args(args);
//    strvec essentials = lp->getEssentials(obj, args, max, presolve);
//    if (withcomments) *out << COMMENT << essentials.size() << " essential reactions." << endl;
//    model->print_vec(essentials, out, withcomments);
//}
//
//void Driver::mss(){
//    mxs(-1);
//}
//
//void Driver::mps(){
//    mxs(1);
//}
//
//void Driver::mxs(int dir){
//    solvec solutions;
//    double vmax = 1;
//    long nmax = LONG_MAX;
//    if (args.size() > 1)
//        vmax = str2double(args[1]);
//    if (args.size() > 0){
//        nmax = (long)str2double(args[0]);
//        if (nmax < 0) nmax = LONG_MAX;
//    }
//    lp->setVMax(vmax);
//    ((SplitLP*)lp)->minExchangeSets(solutions, obj, dir, nmax);
//    int n = 1;
//    string sres = dir < 0 ? "substrates" : "products";
//    for (solvec::iterator it = solutions.begin(); it != solutions.end(); ++it){
//        stomap sol = *it;
//        int npos = 0;
//        for (stomap::iterator jt = sol.begin(); jt != sol.end(); ++jt )
//            if (jt->second / dir > 0)
//                npos++;
//        if (withcomments) *out << COMMENT << n++ << ": " << npos << " " << sres << " (" << *obj << "):" << endl;
//        model->print_conv(&sol, out);
//        *out << endl;
//    }
//}
//
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
//
//void Driver::bottlenecks(){
//    lp->optimise(obj, max, presolve);
//    double objval = lp->getObjValue();
//    if (withcomments) *out << COMMENT << "result: " << objval << endl;
//    strvec bottlenecks;
//    if (objval == 0){
//        args = get_args(args);
//        bottlenecks = lp->getBottleNecks(obj, args, max, presolve);
//    }
//    if (withcomments){
//        string s = thermo ? " thermodynamic " : " directional ";
//        *out << COMMENT << bottlenecks.size() << s << "bottlenecks." << endl;
//    }       
//    model->print_vec(bottlenecks, out, withcomments);
//}
//
//void Driver::tva(){
//    args = get_args(args, LNCONC_TYPE);
//    va();
//}
//
//void Driver::test(){
//
//}


///////////////////////////////////////////////////////////////////////////////////
//output

ostream &operator<<(ostream &out, OBJSTAT objstat) {
    out << objstat.objval << COLON << SPACE << Driver::get_msg(objstat.status);
    return out;
}

ostream &operator<<(ostream &out, OBJSTAT_RANGE range) {
    out << range.min << TAB << range.max;
    return out;
}

ostream &operator<<(ostream &out, rangemap &rm) {
    for (rangemap::iterator it = rm.begin(); it != rm.end(); ++it)
        out << it->first << TAB << it->second << endl;
    return out;
}

void print_rangemap(ostream *out, rangemap *rm, Model* model) {
    for (rangemap::iterator it = rm->begin(); it != rm->end(); ++it) {
        string name = it->first;
        if (name[0] == '-')name = name.substr(1);
        if (name[0] == '[')name = name.substr(1, name.size() - 2);
        *out << it->first << TAB << it->second << TAB << COMMENT << model->find_element(name)->get_info() << endl;
    }
}

ostream &operator<<(ostream &out, MtxLP &lp) {
    OBJSTAT objstat;
    lp.getObjStat(objstat);
    out << objstat;
    return out;
}






//void Driver::fca(){
//    args = get_args(args);
////    rangemap *rm = new rangemap;
////    if (obj->size() > 0)
////        lp->getOptRanges(rm, obj, args, presolve);
////    else
////        lp->getRanges(rm, args, presolve);
////    print_rangemap(out, rm, model);
////    delete rm;
//}

//void Driver::marg(){
//    OBJSTAT objstat;
//    string name = args[0];
//    double point = str2double(args[1]);
//    lp->getMarginalCost(objstat, name, obj, point, FIXRATE, max, presolve);
//    *out << name << TAB << objstat << endl;
//}





//    fva();

//    strvec bottlenecks;
//    if (objval == 0){
//        args = get_args(args);
//        //bottlenecks = tlp->getBottleNecks(obj, args, max, presolve);
//    }

//    if (withcomments) *out << bottlenecks.size() << " thermodynamic bottlenecks." << endl;
//    for (strvec::iterator it = bottlenecks.begin(); it != bottlenecks.end(); ++it){
//        string name = *it;
//        cout << name << ' ';
//        if (name[0] == '-')name = name.substr(1);
//        cout << tlp->getDeltaG(name) << endl;
//    }
//model->print_vec(bottlenecks, out, withcomments);
//
//    stomap sol;
//    lp->getSolution(&sol);
//    model->print_sto(&sol, out, true);



//void Driver::tfba(){
//    TfbaLP *tlp = (TfbaLP*)lp;
//    tlp->TFBA(obj, max, presolve);
//    stomap *sol = new stomap;
//    lp->getObjective(sol, false);
//    cout << *sol << endl;
//    sol->empty();
//
//    if (withcomments)
//        *out << COMMENT << tlp->getTemperature() << "\t" << *lp << endl;
//    lp->getSolution(sol, CONT, false);
//    if (withcomments)
//        *out << COMMENT << "fluxes:" << endl;
//    model->print_sto(sol, out, true);
//    *out << endl;
//    sol->clear();
//    lp->getSolution(sol, LNCONC_TYPE, true);
//    if (withcomments)
//        *out << COMMENT << "concentrations:" << endl;
//    model->print_sto(sol, out, true);
//    delete sol;
//}


//    for (strvec::iterator it = bottlenecks.begin(); it != bottlenecks.end(); ++it){
//        string name = *it;
//        cout << name << ' ';
//        if (name[0] == '-')name = name.substr(1);
//        cout << tlp->getDeltaG(name) << endl;
//    }
//    TfbaLP *tlp = (TfbaLP*)lp;
//    bottlenecks = tlp->getBottleNecks(obj, args, max, presolve);
//    tlp->TFBA(obj, max, presolve);
//
//    stomap sol;
//    lp->getSolution(&sol);
//    model->print_sto(&sol, out, true);


//    strvec reacs = model->reacs_to_block(&args);
//    if (withcomments) *out << COMMENT << '(' << *obj << ") \\ " << args << endl;
//    lp->isCutSet(obj, reacs, presolve);
//    *out << *lp;
