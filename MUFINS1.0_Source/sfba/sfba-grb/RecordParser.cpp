#include "RecordParser.h"


bool read_record(Driver* driver, string record){
    if (record == CONTINUE)
        return true;
    if (isallspace(record))
        return false;
    istringstream is(record, ios::in);
    string line;
    int i = 0;
    do{
        ++i;
        line = line.substr(0, line.find(COMMENT));
        if (!line.empty() && !isallspace(line)){
            char tag = line[0];
            string line2 = line.substr(1);
            try{
                switch(tag){
                    case OBJTAG:
                        if (driver->target.empty())
                            read_objective(driver, line2);
                        break;
                    case ARGTAG:
                        if (driver->arg.empty())
                            driver->set_args(line2);
                        break;
                    //case EXTTAG:
                    //    driver->set_externals(line2, true);//externalise the list provided in line2
                    //    break;
                    //case INTTAG:
                    //    driver->set_externals(line2, false);//internalise the list provided in line2
                    //    break;
                    case EXTTAG://w/ simply set externals with '\'
                        driver->set_externals_gfa(line2);
                        break;
                    case VMAXTAG:
                        driver->lp->setVMax(str2double(line2));
                        break;
//                    case TTAG:
//                        if (driver->thermo) ((ThermoLP*)driver->lp)->setTemperature(T0 + str2double(line2));
//                        break;
                    default:
                        try{
                            read_constraint(driver, line, tag);
                        }
                        catch(exception &e){;}
                }
            }
            catch(exception &e) {
                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
            }
        }
    }
    while(getline(is, line, NEWLINE));
    return true;
}

bool read_record_Grb(DriverGrb* driver, string record){
    if (record == CONTINUE)
        return true;
    if (isallspace(record))
        return false;
    istringstream is(record, ios::in);
    string line;
    int i = 0;
    do{
        ++i;
        line = line.substr(0, line.find(COMMENT));
        if (!line.empty() && !isallspace(line)){
            char tag = line[0];
            string line2 = line.substr(1);
            try{
                switch(tag){
                    case OBJTAG:
                        if (driver->target.empty())
                            read_objective_Grb(driver, line2);
                        break;
                    case ARGTAG:
                        if (driver->arg.empty())
                            driver->set_args(line2);
                        break;
                    //case EXTTAG:
                    //    driver->set_externals(line2, true);//externalise the list provided in line2
                    //    break;
                    //case INTTAG:
                    //    driver->set_externals(line2, false);//internalise the list provided in line2
                    //    break;
                    case EXTTAG:
                        driver->set_externals_gfa(line2);
                        break;
                    //case VMAXTAG:
                    //    driver->lp->setVMax(str2double(line2));
                    //    break;
//                    case TTAG:
//                        if (driver->thermo) ((ThermoLP*)driver->lp)->setTemperature(T0 + str2double(line2));
//                        break;
                    default:
                        try{
                            read_constraint_grb(driver, line, tag);
                        }
                        catch(exception &e){;}
                }
            }
            catch(exception &e) {
                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
            }
        }
    }
    while(getline(is, line, NEWLINE));
    return true;
}

//w/gfa/ read record for genome-wide flux analysis(gfa)
bool read_record_gfa(Driver* driver, string record){
    if (record == CONTINUE)
        return true;
    if (isallspace(record))
        return false;
    istringstream is(record, ios::in);
    string line;
    int i = 0;
    do{
        ++i;
        line = line.substr(0, line.find(COMMENT));
        if (!line.empty() && !isallspace(line)){
            char tag = line[0];
            string line2 = line.substr(1);
            try{
                switch(tag){
                    case OBJTAG:
                        if (driver->target.empty())
                            driver->target=line.substr(1);
//                            driver->set_array(line.substr(1));
                        break;
                    case ARGTAG:
                        if (driver->arg.empty())
                            driver->set_args(line.substr(1));
                        break;
//                    case EXTTAG:
//                        driver->set_externals(line2, true);//externalise the list provided in line2
//                        break;
//                    case INTTAG:
//                        driver->set_externals(line2, false);//internalise the list provided in line2
//                        break;
                    case EXTTAG:
                        driver->set_externals_gfa(line2);
                        break;
                    default:
                        try{
                            read_constraint_mod(driver, line, tag);
                        }
                        catch(exception &e){;}
                }
            }
            catch(exception &e) {
                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
            }
        }        
    }
    while(getline(is, line, NEWLINE));
    return true;
}


//w/gni/ read record for Gene nutrient interaction(gni)
bool read_record_gni(Driver* driver, Model* model, string record){
    if (record == CONTINUE)
        return true;
    if (isallspace(record))
        return false;
    istringstream is(record, ios::in);
    string line;
    int i = 0;
    do{
        ++i;
        line = line.substr(0, line.find(COMMENT));
        if (!line.empty() && !isallspace(line)){
            char tag = line[0];
            string line2 = line.substr(1);
            try{
                switch(tag){
                    case OBJTAG:
                        if (driver->target.empty()) throw runtime_error("please specify a biomass reaction!");
                        driver->gni->set_nts(line2, driver->externals, driver->model);
//                        driver->gni->map_nt_reac(model);
                        break;
                    case DOLTAG:
                        driver->gni->set_ntsp(line2);
                        break;
                    case ARGTAG:
                        if (driver->arg.empty())
                            driver->set_args(line.substr(1));
                        break;
//                    case EXTTAG:
//                        driver->set_externals(line2, true);//externalise the list provided in line2
//                        break;
//                    case INTTAG:
//                        driver->set_externals(line2, false);//internalise the list provided in line2
//                        break;
                    case EXTTAG:
                        driver->set_externals_gfa(line2);
                        break;
                    default:
                        try{
                            read_constraint_mod(driver, line, tag);
                        }
                        catch(exception &e){;}
                }
            }
            catch(exception &e) {
                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
            }
        }
    }
    while(getline(is, line, NEWLINE));
    return true;
}

//w/dpa/ read record for DPA analysis
bool read_record_dpa(Driver* driver, string record){
    if (record == CONTINUE)
        return true;
    if (isallspace(record))
        return false;
    istringstream is(record, ios::in);
    string line;
    int i = 0;
    do{
        ++i;
        line = line.substr(0, line.find(COMMENT));
        if (!line.empty() && !isallspace(line)){
            char tag = line[0];
            string line2 = line.substr(1);
            try{
                switch(tag){
                    case OBJTAG:
                        if (driver->target.empty())
                            driver->set_arrays(line.substr(1));
                        break;
                    case ARGTAG:
                        if (driver->arg.empty())
                            driver->set_args(line.substr(1));
                        break;
//                    case EXTTAG:
//                        driver->set_externals(line2, true);//externalise the list provided in line2
//                        break;
//                    case INTTAG:
//                        driver->set_externals(line2, false);//internalise the list provided in line2
//                        break;
                    case EXTTAG:
                        driver->set_externals_gfa(line2);
                        break;
                    default:
                        try{
                            read_constraint_mod(driver, line, tag);
                        }
                        catch(exception &e){;}
                }
            }
            catch(exception &e) {
                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
            }
        }
    }
    while(getline(is, line, NEWLINE));
    return true;
}

void read_constraint(Driver* driver, string line, char tag){
    double lb = LB, ub = UB;
    strvec toks;
    split(&line, &toks, DELIM);
    int ntoks = toks.size();
    if (ntoks < 1) throw runtime_error(line);
    string tok0 = toks[0];
    if ((ntoks) > 1){
            lb = str2double(toks[1]);
            ub = lb;
    }
    if ((ntoks > 2)) ub = str2double(toks[2]);

    switch(tag){
        case PLUSTAG:
            read_sto(driver, tok0, lb, ub);
            break;
        case MINUSTAG:
            driver->lp->delRow(tok0.substr(1));
            break;
        case ALLTAG:{
            strvec cols = driver->lp->getColNames();
            for (strvec::iterator it = cols.begin(); it != cols.end(); ++it)
                driver->lp->setColBnds(*it, lb, ub);
            break;
        }
//        case CTAG:
//            if (driver->thermo)
//                ((ThermoLP*)driver->lp)->setConcentration(tok0, lb, ub);
//            break;
//        case DG0TAG:
//            if (driver->thermo)
//                ((ThermoLP*)driver->lp)->setDeltaG0(tok0.substr(1), lb, ub);
//            break;
//        case DGTAG:
//            tok0 = DELTAG_TAG + tok0.substr(1);
        default:
            if (driver->lp->hasCol(tok0))
                driver->lp->setColBnds(tok0, lb, ub);
  }
}

void read_constraint_mod(Driver* driver, string line, char tag){//w/grb/
    double lb = LB, ub = UB;
    strvec toks;
    split(&line, &toks, DELIM);
    int ntoks = toks.size();
    if (ntoks < 1) throw runtime_error(line);
    string tok0 = toks[0];
    if ((ntoks) > 1){
            lb = str2double(toks[1]);
            ub = lb;
    }
    if ((ntoks > 2)) ub = str2double(toks[2]);
    strvec reacs=driver->model->get_reactions();
    switch(tag){
        case PLUSTAG:
            read_sto_mod(driver, tok0, lb, ub);
            break;
        case MINUSTAG:
            driver->model->del_element(tok0.substr(1));
            break;
        case ALLTAG:{
            for(int i=0;i<reacs.size();i++){
                string Rn=reacs[i];
                Reaction* reac=driver->model->get_reac(tok0);
                reac->set_bounds(lb, ub);
            }
            break;
        }
//        case CTAG:
//            if (driver->thermo)
//                ((ThermoLP*)driver->lp)->setConcentration(tok0, lb, ub);
//            break;
//        case DG0TAG:
//            if (driver->thermo)
//                ((ThermoLP*)driver->lp)->setDeltaG0(tok0.substr(1), lb, ub);
//            break;
//        case DGTAG:
//            tok0 = DELTAG_TAG + tok0.substr(1);
        default:
            if(find(reacs.begin(),reacs.end(),tok0)==reacs.end())
                throw runtime_error(string("no column ") + tok0);
            else{
                 Reaction* reac=driver->model->get_reac(tok0);
                 reac->set_bounds(lb, ub);
            }
      }
}


void read_constraint_grb(DriverGrb* driver, string line, char tag){//w/grb/
		double lb = LB, ub = UB;
		strvec toks;
		split(&line, &toks, DELIM);
		int ntoks = toks.size();
		if (ntoks < 1) throw runtime_error(line);
		string tok0 = toks[0];
		if ((ntoks) > 1){
				lb = str2double(toks[1]);
				ub = lb;
		}
		if ((ntoks > 2)) ub = str2double(toks[2]);

        switch(tag){
            case PLUSTAG://add row
                read_sto_grb(driver, tok0, lb, ub);
                break;
            case MINUSTAG://del row
                driver->grbmod->delCon(tok0.substr(1));
                break;
            case ALLTAG:{
                driver->grbmod->setAllVarBnds(lb, ub);
                break;
            }
    //        case CTAG:
    //            if (driver->thermo)
    //                ((ThermoLP*)driver->lp)->setConcentration(tok0, lb, ub);
    //            break;
    //        case DG0TAG:
    //            if (driver->thermo)
    //                ((ThermoLP*)driver->lp)->setDeltaG0(tok0.substr(1), lb, ub);
    //            break;
    //        case DGTAG:
    //            tok0 = DELTAG_TAG + tok0.substr(1);
            default:
                if (driver->grbmod->hasVar(tok0))
                    driver->grbmod->setVarBnds(tok0, lb, ub);
      }
}

void read_sto(Driver* driver, string tok, double lb, double ub){
    strvec name_sto;
    stomap sto;
    split(&tok, &name_sto, COLON);
    string name = name_sto.front().substr(1);
    parse_sto(name_sto.back(), &sto);
    driver->lp->addConstraint(&sto, lb, ub, false, name);
}

void read_sto_grb(DriverGrb* driver, string tok, double lb, double ub){
    strvec name_sto;
    stomap sto;
    split(&tok, &name_sto, COLON);
    string name = name_sto.front().substr(1);
    parse_sto(name_sto.back(), &sto);
	driver->grbmod->addConstraint(&sto, lb, ub, name);//w/grb/
}

void read_sto_mod(Driver* driver, string tok, double lb, double ub){//w/
    strvec name_sto;
    stomap sto;
    split(&tok, &name_sto, COLON);
    string name = name_sto.front().substr(1);
    parse_sto(name_sto.back(), &sto);
    strvec reacs=driver->model->get_reactions();
    for (stomap::iterator it = sto.begin(); it != sto.end(); ++it){
        string col = it->first;
        if(find(reacs.begin(),reacs.end(),col)==reacs.end())
            throw runtime_error(string("no column ") + col);
        else{
            Reaction *reac=driver->model->get_reac(col);
            stomap *sto_reac = reac->get_sto();
            (*sto_reac)[name]=it->second;
        }
    }
}

void read_objective(Driver* driver, string line){
    istringstream is(line.data());
    string tok;

    std::getline(is, tok, COLON);
    driver->max = driver->get_arg(tok);
    driver->lp->setObjDir(driver->max);

    std::getline(is, tok, COLON);
    driver->set_objective(tok);
}

void read_objective_Grb(DriverGrb* driver, string line){ //w/
    istringstream is(line.data());
    string tok;

    std::getline(is, tok, COLON);
    driver->max = driver->get_arg(tok);
    if(driver->max) driver->grbmod->setOptDir(GrbMax);
    else driver->grbmod->setOptDir(GrbMin);

    std::getline(is, tok, COLON);
    driver->set_objective(tok);
//	driver->grbmod->setObjs(driver->obj);
}



//    parse_sto(toks[0], &sto);
            //driver->lp->addConstraint(&sto, lb, ub, false);

//case CTAG:
//try{
//    if (driver->thermo) read_constraint(driver, line.substr(1), LNCONC_TYPE);
//}
//catch(exception &e){;}
//break;
//case DGTAG:
//try{
//    if (driver->thermo) read_constraint(driver, line.substr(1), DELTA_TYPE);
//}
//catch(exception &e){;}
//break;
//case DG0TAG:
//try{
//   if (driver->thermo) read_constraint(driver, line.substr(1), DELTAG0_TYPE);
//}
//catch(exception &e){;}
//break;


//        case CTAG:
//            if (driver->problem == "tfba" || driver->problem == "bottlenecks"  || driver->problem == "test"){
//                string name = sto.begin()->first;
//                if (name.size() == 0 || driver->lp->hasCol(name))
//                    ((TfbaLP*)driver->lp)->setConcentration(name, lb, ub);
//            }
//            else{
//                lb = log(lb);
//                ub = log(ub);
//                driver->lp->addConstraint(&sto, lb, ub, false);
//                cout << sto << " " << lb << " " << ub << endl;
//            }
//            break;
//        case DGTAG:
//            for (stomap::iterator it = sto.begin(); it != sto.end(); ++it){
//                string name = it->first;
//                sto[DELTAG_TAG + name] = it->second;
//                sto.erase(it);
//            }
//            driver->lp->addConstraint(&sto, lb, ub, true);
//            break;
//        case DG0TAG:{
//            if (driver->problem == "tfba" || driver->problem == "bottlenecks" || driver->problem == "test"){
//                ((TfbaLP*)driver->lp)->setDeltaG0(sto.begin()->first, lb, ub);
//            }
//            else{
//            if (sto.size() > 1) throw runtime_error(string("dG0 bounds for more than one flux: ") + line);
//            string name = DELTAG_TAG + sto.begin()->first + CONC_SUFX;
//            if (driver->lp->hasRow(name))driver->lp->setRowBnds(name, lb, ub);
//            //else throw runtime_error(string("reaction not found: " + name));
//            }
//            break;
//        }




//    bool read_record(string);
//    void read_objective(string);
//    void read_constraint(string, int type = CONT);

//bool Driver::read_record(string record){
//    if (record == CONTINUE)
//        return true;
//    if (isallspace(record))
//        return false;
//    istringstream is(record, ios::in);
//    string line;
//    int i = 0;
//    do{
//        ++i;
//        line = line.substr(0, line.find(COMMENT));
//        if (!line.empty() && !isallspace(line)){
//            try{
//                switch(line[0]){
//                    case OBJTAG:
//                        if (target.empty())
//                            read_objective(line.substr(1));
//                        break;
//                    case ARGTAG:
//                        if (arg.empty())
//                            set_args(line.substr(1));
//                        break;
//                    case EXTTAG:
//                        lp->externalise(externals, false);
//                        set_externals(line.substr(1));
//                        lp->externalise(externals, true);
//                        break;
//                    case CTAG:
//                        try{
//                            if (thermo) read_constraint(line.substr(1), LNCONC_TYPE);
//                        }
//                        catch(exception &e){;}
//                        break;
//                    case DGTAG:
//                        try{
//                            if (thermo) read_constraint(line.substr(1), DELTA_TYPE);
//                        }
//                        catch(exception &e){;}
//                        break;
//                    case DG0TAG:
//                        try{
//                           if (thermo) read_constraint(line.substr(1), DELTAG0_TYPE);
//                        }
//                        catch(exception &e){;}
//                        break;
//                    case TTAG:
//                        if (thermo) ((TfbaLP*)lp)->setTemperature(str2double(line.substr(1)));
//                    default:
//                        try{
//                            read_constraint(line);
//                        }
//                        catch(exception &e){;}
//                }
//            }
//            catch(exception &e) {
//                throw runtime_error(string("error in line ") + num2str(i) + ": " + e.what());
//            }
//        }
//    }
//    while(getline(is, line, NEWLINE));
//    return true;
//}
//
//void Driver::read_constraint(string line, int type){
//    strvec toks;
//    stomap sto;
//    double lb = LB, ub = UB;
//    int ntoks;
//
//    split(&line, &toks, DELIM);
//    ntoks = toks.size();
//    if (ntoks < 1) throw runtime_error(line);
//    parse_sto(toks[0], &sto);
//    if ((ntoks) > 1){
//            lb = str2double(toks[1]);
//            ub = lb;
//    }
//    if ((ntoks > 2)) ub = str2double(toks[2]);
//
//    switch(type){
//        case LNCONC_TYPE:
//            if (problem == "tfba" || problem == "bottlenecks"  || problem == "test"){
//                string name = sto.begin()->first;
//                if (name.size() == 0 || lp->hasCol(name))
//                    ((TfbaLP*)lp)->setConcentration(name, lb, ub);
//            }
//            else{
//            lb = log(lb);
//            ub = log(ub);
//            lp->addConstraint(&sto, lb, ub, false);
//            }
//            break;
//        case DELTA_TYPE:
//            for (stomap::iterator it = sto.begin(); it != sto.end(); ++it){
//                string name = it->first;
//                sto[DELTAG_TAG + name] = it->second;
//                sto.erase(it);
//            }
//            lp->addConstraint(&sto, lb, ub, true);
//            break;
//        case DELTAG0_TYPE:{
//            if (problem == "tfba" || problem == "bottlenecks" || problem == "test"){
//                ((TfbaLP*)lp)->setDeltaG0(sto.begin()->first, lb, ub);
//            }
//            else{
//            if (sto.size() > 1) throw runtime_error(string("dG0 bounds for more than one flux: ") + line);
//            string name = DELTAG_TAG + sto.begin()->first + CONC_SUFX;
//            if (lp->hasRow(name))lp->setRowBnds(name, lb, ub);
//            //else throw runtime_error(string("reaction not found: " + name));
//            }
//            break;
//        }
//        default:
//            lp->addConstraint(&sto, lb, ub, false);
//    }
//}
//
//void Driver::read_objective(string line){
//    istringstream is(line.data());
//    string tok;
//
//    std::getline(is, tok, COLON);
//    max = get_arg(tok);
//    lp->setObjDir(max);
//
//    std::getline(is, tok, COLON);
//    set_objective(tok);
//}
