/*
 *  PetriNet.cpp
 *  pnfba_xcode
 *
 *  Created by Andrzej Kierzek on 01/01/2011.
 *  Copyright 2011 Andrzej Kierzek. All rights reserved.
 *
 */

#include <fstream>
#include "PetriNet.h"

double MetaboliteActivity::activity(double a) {
	double result = 0;
	for (int i = 0; i < size; i++) {
		result = (a >= thresholds[i]) ? activities[i] : result;
	}
	return result;	
}

void EnzymeActivity::activity(double t, double &lb, double &ub) {
    lb=0;
	ub=0;
	for(int i=0;i<size;i++) {
		if(t>=thresholds[i]) {
		  	lb = lowerbounds[i];
			ub = upperbounds[i];
		}
	}
}

Interaction::Interaction(string nm,int ns,int np,int nc) {
	name = nm;
	Ns = ns;
	Np = np;
	Nc = nc;
	substrates = new Substance*[Ns];
	products = new Substance*[Np];
	consumed = new Substance*[Nc];
	activities = new Activity[Ns];
    activity_values = new double[Ns];
    stoich = new double[Ns];
    pstoich = new double[Np];
	a=0;
	c=0;
    propensity_expression = false;
    reset_expression = false;
}

void Interaction::setPropensityFunction(string formula) {
    vector<double*> vars;
    vector<string> names;
    
    for(int i=0;i<Ns;i++) {
        names.push_back(substrates[i]->getName());
        vars.push_back(&activity_values[i]);
    }
    
    propensityFunction = *new AlgebraicExpression(formula, names, vars);
    propensityFunction.parse();
}

void Interaction::setResetFunction(string formula) {
    vector<double*> vars;
    vector<string> names;
    
    for(int i=0;i<Ns;i++) {
        names.push_back(substrates[i]->getName());
        vars.push_back(&activity_values[i]);
    }
    
    resetFunction = *new AlgebraicExpression(formula, names, vars);
    resetFunction.parse();
}

PetriNet::PetriNet(int ns, int ni) {
    Ns = ns;
	Ni = ni;
	substances = new Substance[Ns];
	interactions = new Interaction[Ni];
	fbaRequired = false;
	nutrientUpdateTime = 0;
	Time = 0;
	deltaTime = 0;
	timeSinceNutrientUpdate = 0;
	msg_flag=false;
    maxChange = DEFAULT_MAX_CHANGE;
    has_fba = false;
}

void PetriNet::readPetriNet(string path) {
	fstream infile;
	string word;
	
	infile.open(path.c_str());
    if (!infile) {
		cerr << "Could not open file:" << path << endl;
		exit(0);
	}
	while(infile) {
		infile >> word;
		if(word=="SUBSTANCES") {
			readSubstances(infile);
		}
		if(word=="REACTIONS") {
			readReactions(infile);
		}
		if(word=="GSMN") {
			read_gsmn_info(infile);
		}
		word="";
	}
}

void PetriNet::readMonitors(fstream &f) {
	string word;
	while(f) {
		f >> word;
		if(word=="END") break;
		stateMonitors.push_back(word);
	}
}

void PetriNet::readGsmnMonitors(fstream &f) {
	string word;
	while(f) {
		f >> word;
		if(word=="END") break;
		gsmnMonitors.push_back(word);
	}	
}



void PetriNet::read_gsmn_info(fstream &f) {
	string word, word1;
	int idx;
	int objidx;
    bool objflag = false;
	vector<int> metidx;
	vector<int> enzidx;
	vector<int> nutridx;
	
	while(f) {
		f>>word;
		if(word=="END") break;
		if(word=="MODEL") {
            has_fba = true;
		    f >> word;
			setSfbaFile(word);
		}
		if(word=="EXT_TAG") {
            has_fba = true;
			f >> word;
			setXtTag(word);
		}
        if(word=="FLUX") {
            has_fba=true;
            f >> word;
            f >> word1;
            for(int i=0;i<Ns;i++) substances[i].addFluxRequest(word,word1);
        }
		if(word=="METABOLITE") {
            has_fba = true;
			f >> word;	
			idx = nameIndex[word];
			metidx.push_back(idx);
			f >> word;
			substances[idx].setGsmnMetabolite(word);
			f >> word;
			if(word=="ACTIVITY") {
				int as;
				f >> as;
				MetaboliteActivity ma = *(new MetaboliteActivity(as));
				for(int i=0;i<ma.size;i++) {
					double thr;
					double a;
					f >> thr;
					f>> a;
					ma.setThreshold(thr, i);
					ma.setActivity(a, i);
				}
				substances[idx].setActivity(ma);
				f>>word;
			}
			f>>word;
		}
		if(word=="OBJECTIVE") {
            has_fba = true;
            objflag=true;
			f >> word;	
			objidx = nameIndex[word];
			f >> word;
			substances[objidx].setGsmnReaction(word);
			f >> word;
			if(word=="ACTIVITY") {
				int as;
				f >> as;
				MetaboliteActivity ma = *(new MetaboliteActivity(as));
				for(int i=0;i<ma.size;i++) {
					double thr;
					double a;
					f >> thr;
					f>> a;
					ma.setThreshold(thr, i);
					ma.setActivity(a, i);
				}
				substances[objidx].setActivity(ma);
				f>>word;
			}
			f>>word;
		}
		if(word=="ENZYME") {
            has_fba = true;
		    bool listflag = true;
            f >> word;	
			idx = nameIndex[word];
			enzidx.push_back(idx);
			f >> word;
            if(word!="list") {
                listflag = false;
			    substances[idx].setGsmnReaction(word);
                substances[idx].allocateRWeights(1);
                substances[idx].setRWeight(0, 1.0);
            }
			f >> word;
			if(word=="ACTIVITY") {
				int as;
				f >> as;
				EnzymeActivity ea = *(new EnzymeActivity(as));
				for(int i=0;i<ea.size;i++) {
					double thr;
					double lb;
					double ub;
					f >> thr;
					f >> lb;
					f >> ub;
					ea.setThreshold(thr, i);
					ea.setLowerBound(lb, i);
					ea.setUpperBound(ub, i);
				}
				substances[idx].setEnzymeActivity(ea);
				f>>word;
			}
            if(listflag) f >> word;
            if(word=="LIST") {
                int items;
                f >> items;
                substances[idx].allocateRWeights(items);
                for(int i=0;i<items;i++) {
                    double wght;
                    f >> word;
                    substances[idx].setGsmnReaction(word);
                    f >> wght;
                    substances[idx].setRWeight(i, wght);
                }
                f>>word;
            }
			f>>word;
		}
		if(word=="NUTRIENT") {
            has_fba = true;
		    f >> word;	
			idx = nameIndex[word];
			nutridx.push_back(idx);
			f >> word;
			substances[idx].setGsmnReaction(word);
			double u;
			f >> u;
			substances[idx].setNutrientUnit(u);
			f >> u;
			substances[idx].setNutrientMax(u);
			f >> word;
			if(word=="ACTIVITY") {
				int as;
				f >> as;
				EnzymeActivity ea = *(new EnzymeActivity(as));
				for(int i=0;i<ea.size;i++) {
					double thr;
					double lb;
					double ub;
					f >> thr;
					f >> lb;
					f >> ub;
					ea.setThreshold(thr, i);
					ea.setLowerBound(lb, i);
					ea.setUpperBound(ub, i);
				}
				substances[idx].setEnzymeActivity(ea);
				f>>word;
			}
			f>>word;
		}
	}
	metabolites = new Substance*[metidx.size()];
	Nm = (int)metidx.size();
	for(int i=0;i<metidx.size();i++) metabolites[i] = &substances[metidx[i]];
	enzymes = new Substance*[enzidx.size()];
	Ne = (int)enzidx.size();
	for(int i=0;i<enzidx.size();i++) enzymes[i] = &substances[enzidx[i]];
	nutrients = new Substance*[nutridx.size()];
	Nn = (int)nutridx.size();
	for(int i=0;i<nutridx.size();i++) nutrients[i] = &substances[nutridx[i]];
	if(objflag) {
        objective = &substances[objidx];
    } else {
        objective = NULL;
    }
	metidx.clear();
	enzidx.clear();
	nutridx.clear();
}

void PetriNet::readSubstances(fstream &f) {
	string word;
	double v1, v2;
    int v3;
	vector<Substance> sv;
	while(f) {
		f >> word;
		if(word=="END") break;
		f >> v1;
		f >> v2;
		f >> v3;
		Substance s = *(new Substance(word,v1,v2,v3));
		sv.push_back(s);		
	}
	Ns = (int)sv.size();
	substances = new Substance[Ns];
	for(int i=0;i<sv.size();i++) {
		substances[i] = sv[i];
	    nameIndex[substances[i].getName()] =i;	
	}	
	sv.clear();
}

void PetriNet::readReactions(fstream &f) {
    string word;
	vector<Interaction> iv;
	Interaction in;
	while(f) {
		f >> word;
		if(word=="END") break;
		if(word=="INTERACTION") {
			in = readInteraction(f);
			iv.push_back(in);
		}
	}
	Ni = (int)iv.size();
	interactions = new Interaction[Ni];
	for(int i=0;i<iv.size();i++) interactions[i] = iv[i];
	iv.clear();
}

Interaction PetriNet::readInteraction(fstream &f) {
	string word;
	string name;
	double c;
	double delay;
	bool slow;
	bool instant;
    bool reset_flag;
    bool fba_flux;
	Interaction in;
	double tr;
    double st;
    double pst;
	double act;
	vector<int> sv;
	vector<int> pv;
	vector<int> cv;
	vector<int> mv;
    vector<double> stv;
    vector<double> pstv;
	vector<Activity> av;
	
	f >> name;
	f >> c;
	f >> word;
    slow = false; reset_flag = false; instant = false; fba_flux = false;
    if(word=="SLOW") {
		slow=true;
	} else if(word=="INSTANT") {
		instant=true;
    } else if(word=="RESET") {
        reset_flag=true;
    } else if(word=="FLUX") {
        fba_flux=true;
    }
	f >> delay;
	while(f) {
	  	f >> word;
		if(word=="END") break;
		if(word=="SUBSTRATE") {
			f >> word;	
			sv.push_back(nameIndex[word]);
			if(substances[nameIndex[word]].getType()==METABOLITE_NODE) mv.push_back(nameIndex[word]);
            stv.push_back(1);
			f >> word;
            if(word=="STOICHIOMETRY") {
                f >> st;
                stv.pop_back();
                stv.push_back(st);
                f >> word;
            }
			if(word=="ACTIVITY") {
				int actnum;
				f >> actnum;
				Activity af = *(new Activity(actnum));
				for(int i=0;i<actnum;i++) {
				    f >> tr;
					f >> act;
					af.setThreshold(tr, i);
					af.setActivity(act,i);
				}
			    av.push_back(af);
				f >> word;
				f >> word;
			}
		}
		if(word=="PRODUCT") {
			f >> word;
			pv.push_back(nameIndex[word]);
            pstv.push_back(1);
		}
        if(word=="PRODUCT_STOICHIOMETRY") {
            f >> word;
            pv.push_back(nameIndex[word]);
            f >> pst;
            pstv.push_back(pst);
        }
		if(word=="CONSUMED") {
			f >> word;
			cv.push_back(nameIndex[word]);
		}
		
	}
	in = *(new Interaction(name,int(sv.size()),int(pv.size()),int(cv.size())));
	in.initMetabolites(int(mv.size()));
	in.setRateConstant(c);
	in.delay = delay;
	in.slow = slow;
	in.instant = instant;
    in.reset = reset_flag;
    in.fba_flux = fba_flux;
	for(int i=0;i<sv.size();i++) {
		in.setSubstrate(&substances[sv[i]], i);
		in.setActivity(av[i],i);
        in.setStoichiometry(stv[i], i);
	}
	for(int i=0;i<pv.size();i++) in.setProduct(&substances[pv[i]], i);
    for(int i=0;i<pv.size();i++) in.setProductStoichiometry(pstv[i], i);
	for(int i=0;i<cv.size();i++) in.setConsumed(&substances[cv[i]], i);
	for(int i=0;i<mv.size();i++) in.setMetabolite(&substances[mv[i]], i);
	sv.clear();
	pv.clear();
	av.clear();
	cv.clear();
	mv.clear();
    stv.clear();
    pstv.clear();
	return in;
}

void PetriNet::buildGsmnDriver() {
	gsmnDriver = new Driver;
	gsmnDriver->setInfile(sfba_file);
	gsmnDriver->setXtag(xt_tag);
	gsmnDriver->buildLP();
    gsmnDriver->setUpdateRequired(true);
}

void PetriNet::runGsmnMonitors() {
	double result;
    for(int i=0;i<gsmnMonitors.size();i++) {
	  gsmnDriver->setObjective(gsmnMonitors[i]);	
	  gsmnDriver->updateLP();	
	  result = gsmnDriver->calcObjValue();
	  trajectory->pushFlux(gsmnMonitors[i],result);
	}
	
}

void PetriNet::runGsmnMonitors(int trj) {
	double result;
    for(int i=0;i<gsmnMonitors.size();i++) {
		gsmnDriver->setObjective(gsmnMonitors[i]);	
		gsmnDriver->updateLP();	
		result = gsmnDriver->calcObjValue();
		trajectories[trj]->pushFlux(gsmnMonitors[i],result);
	}
	
}

void PetriNet::runStateMonitors() {
    int s;
	for(int i=0;i<stateMonitors.size();i++) {
		s = substances[nameIndex[stateMonitors[i]]].getState();
		trajectory->pushState(stateMonitors[i], s);
	}
}

void PetriNet::runStateMonitors(int trj) {
    double s;
	double a;
	for(int i=0;i<stateMonitors.size();i++) {
		if(substances[nameIndex[stateMonitors[i]]].getType()==SUBSTANCE_NODE) {
		   s = substances[nameIndex[stateMonitors[i]]].getState();
		   trajectories[trj]->pushState(stateMonitors[i], s);
		} else if(substances[nameIndex[stateMonitors[i]]].getType()==METABOLITE_NODE) {
			s = substances[nameIndex[stateMonitors[i]]].getState();
			trajectories[trj]->pushState(stateMonitors[i], s);
		} else if(substances[nameIndex[stateMonitors[i]]].getType()==ENZYME_NODE) {
			s = substances[nameIndex[stateMonitors[i]]].getState();
			trajectories[trj]->pushState(stateMonitors[i], s);
		} else if(substances[nameIndex[stateMonitors[i]]].getType()==NUTRIENT_NODE) {
			a = substances[nameIndex[stateMonitors[i]]].getAmount();
			trajectories[trj]->pushAmount(stateMonitors[i], a);
		} else if(substances[nameIndex[stateMonitors[i]]].getType()==OBJECTIVE_NODE) {
			a = substances[nameIndex[stateMonitors[i]]].getAmount();
			trajectories[trj]->pushAmount(stateMonitors[i], a);
		} else  if(substances[nameIndex[stateMonitors[i]]].getType()==FLUX_NODE) {
			a = substances[nameIndex[stateMonitors[i]]].getState();
			trajectories[trj]->pushState(stateMonitors[i], a);
		}
	}
}

State *PetriNet::getState() {
	State *result;
	result = new State(Ns);
	for(int i=0;i<Ns;i++) result->setState(i, substances[i].getState());
	return result;
}

void PetriNet::run() {
	trajectory = new Trajectory();
	buildGsmnDriver();
	
	int zero = 0;
	runMonitors(zero);
	for(int i=1;i<Nmax;i++) {
	   synchronousUpdate();
	   runMonitors(i);
	}
	trajectory->printTableWithIteration();
}

void Substance::metaboliteAction(Driver *gsmn) {
	if(flag1) {
	   gsmn->setObjective(gsmn_metabolite);
	   gsmn->updateLP();
	   flux = gsmn->calcObjValue();
	   flag1 = false;
	}
	state = activity.activity(flux);
	if(msg_flag) cout << this->getName() << "\t" << flux << "\t" << state << endl;
}

/* void Substance::producibilityState(Driver *gsmn) {
	if(gsmn->isUpdateRequired()) {
       gsmn->setObjective(gsmn_metabolite);
	   gsmn->updateLP();
	   flux = gsmn->calcObjValue();
    }
    state = activity.activity(flux);
	if(msg_flag) cout << this->getName() << "\t" << flux << "\t" << state << endl;
} */

void Substance::producibilityState(Driver *gsmn) {
	if(gsmn->isUpdateRequired()) {
        gsmn->setObjective(gsmn_metabolite);
        gsmn->updateLP();
        flux = gsmn->calcObjValue();
        for(int i=0;i<flux_requests.size();i++) flux_requests[i].flux(gsmn);
    }
    state = activity.activity(flux);
	if(msg_flag) cout << this->getName() << "\t" << flux << "\t" << state << endl;
}

void Substance::objectiveAction(Driver *gsmn) {	
	double lb,ub;
	gsmn->getLP()->setColBnds(gsmn_reaction,LB_INF,UB_INF);
	gsmn->setObjective(gsmn_reaction);
	gsmn->updateLP();
	flux = gsmn->calcObjValue();
	state = activity.activity(flux);
	lb = flux - OBJECTIVE_TOLERANCE;
	ub = flux;
	gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	if(msg_flag) cout << gsmn_reaction << "\t" << lb << "\t" << ub << endl;
}

void Substance::addFluxRequest(Interaction i) {
    flux_requests.push_back(i);
}

void Substance::printFluxRequests() {
    for(int i=0;i<flux_requests.size();i++) {
        cout << "  FLUX " << flux_requests[i].getName() << " " << flux_requests[i].flux_request << endl;
    }
}

void Substance::addFluxRequest(string in, string r) {
    for(int i=0;i<flux_requests.size();i++)
        if(flux_requests[i].getName()==in) flux_requests[i].flux_request = r;
}

string Substance::getFluxRequestTransition(int i) {
    return flux_requests[i].getName();
}

string Substance::getFluxRequest(int i) {
    return flux_requests[i].flux_request;
}

// DYNAMIC FBA FUNCTIONS

void Substance::objectiveAction(Driver *gsmn, double dt) {
	double lb,ub;
    if(gsmn->isUpdateRequired()) {
	   gsmn->getLP()->setColBnds(gsmn_reaction,LB_INF,UB_INF);
       gsmn->setObjective(gsmn_reaction);
	   gsmn->updateLP();
	   flux = gsmn->calcObjValue();
    }
	amount += flux*amount*dt;  
	state = rint(amount*BIOMASS_UNIT);
    if(gsmn->isUpdateRequired()) {
	  lb = flux - OBJECTIVE_TOLERANCE;
	  ub = flux;
	  gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
      if(msg_flag) cout << gsmn_reaction << "\t" << state << "\t" << lb << "\t" << ub << endl;
    } else {
      if(msg_flag) cout << gsmn_reaction << "\t" << state << "\t" << "unchanged" << "\t" << "unchanged" << endl;
    }
	
}

void Substance::nutrientState(Driver *gsmn, double dt, double biomass) {
	flux = gsmn->getLP()->getColValue(gsmn_reaction);
	amount += flux*biomass*dt;
	if(amount<0) amount=0;
	state = rint(amount*nutrientUnit);
	
	if(msg_flag) cout << this->getName() <<  ": Unit: " << nutrientUnit << " Flux:" << flux << " dt:" << dt << " Change:" << flux*dt*biomass << " Amount:" << amount << endl;
}

void Substance::setNutrientBounds(Driver *gsmn, double dt, double biomass) {
	double lb, ub;
	lb = (-1)*amount/(biomass*dt);
	if(lb<nutrientMax) lb=nutrientMax;
	ub = UB_INF;
    if(lb!=lbound) gsmn->setUpdateRequired(true);
    if(ub!=ubound) gsmn->setUpdateRequired(true);
	gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	if(msg_flag) cout << "Nutrient bound: " << gsmn_reaction << "\t" << lb << "\t" << ub << endl;
    lbound = lb;
    ubound = ub;
}
/////////////////////////////////

void Substance::nutrientState(Driver *gsmn, double dt) {
	double lb, ub;
	enzActivity.activity(state, lb, ub);
	gsmn->getLP()->setColBnds(gsmn_reaction,LB_INF,UB_INF);
	gsmn->setObjective(gsmn_reaction);
	gsmn->updateLP();
	flux = gsmn->calcObjValue();
	state += floor(nutrientUnit*flux*dt);
	if(state<0) state=0;
	gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	if(msg_flag) cout << this->getName() <<  ": Unit: " << nutrientUnit << " Flux:" << flux << " dt:" << dt << " Change:" << nutrientUnit*flux*dt << " State:" << state << endl;
}

void Substance::setNutrientBounds(Driver *gsmn) {
	double lb, ub;
	enzActivity.activity(state, lb, ub);
	gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	if(msg_flag) cout << gsmn_reaction << "\t" << lb << "\t" << ub << endl;
}

bool Substance::enzymeAction(Driver *gsmn) {
	double lb,ub;
	enzActivity.activity(state, lb, ub);
	if(lb!=lbound || ub!=ubound) {
	   gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	   lbound = lb;
	   ubound = ub;
	   if(msg_flag) cout << gsmn_reaction << "\t" << lbound << "\t" << ubound << endl;
	   return true;
	}
	if(msg_flag) cout << gsmn_reaction << "\t" << lbound << "\t" << ubound << endl;
	return false;
}

void Substance::initialEnzymeAction(Driver *gsmn) {
	double lb,ub;
	enzActivity.activity(state, lb, ub);
	gsmn->getLP()->setColBnds(gsmn_reaction,lb,ub);
	lbound = lb;
	ubound = ub;
	if(msg_flag) cout << gsmn_reaction << "\t" << lbound << "\t" << ubound << endl;
}

void Substance::setEnzymeBounds(Driver *gsmn) {
	double lb,ub;
	enzActivity.activity(state, lb, ub);
    if(lb!=lbound) {
        gsmn->setUpdateRequired(true);
        if(msg_flag) cout << "FBA TRIGGERED BY: " << name << "\t" << lb << endl;
        if(print_fba_trigger) cout << "PRINT_FBA_TRIGGER " << name << "\t lower bound:" << lb << endl;
    }
    if(ub!=ubound) {
        gsmn->setUpdateRequired(true);
        if(msg_flag) cout << "FBA TRIGGERED BY: " << name << "\t" << ub << endl;
        if(print_fba_trigger) cout << "PRINT_FBA_TRIGGER " << name << "\t upper bound:" << ub << endl;
    }
    for(int i=0;i<gsmn_reactions.size();i++) {
	  gsmn->getLP()->setColBnds(gsmn_reactions[i],lb*rweights[i],ub*rweights[i]);
      if(msg_flag) cout << name << "\t" << gsmn_reactions[i] << "\t" << lb*rweights[i] << "\t" << ub*rweights[i] << endl;
    }
	lbound = lb;
	ubound = ub;	
}

void PetriNet::readModel(string path) {
    fstream infile;
    Json::Value root;
    vector<int> mv;
    
    infile.open(path.c_str());
	if (!infile) {
		cerr << "Could not open file:" << path << endl;
		exit(0);
	}
    has_fba = false;
    
    infile >> root;
    
    Json::Value places = root[SUBSTANCES_TAG];
    Ns = places.size();
    substances = new Substance[Ns];
    for(int i=0; i<Ns;i++) {
        Json::Value place = places[i][SUBSTANCE_TAG];
        string name = place[NAME_TAG].asString();
        double state = place[STATE_TAG].asDouble();
        double max = place[MAX_TAG].asDouble();
        int type = place[TYPE_TAG].asInt();
        substances[i] = *(new Substance(name,state,max,type));
        nameIndex[substances[i].getName()] = i;
    }
    Json::Value transitions = root[INTERACTIONS_TAG];
    Ni = transitions.size();
    interactions = new Interaction[Ni];
    for(int i=0;i<Ni;i++) {
        Json::Value transition = transitions[i][INTERACTION_TAG];
        string name = transition[NAME_TAG].asString();
        Json::Value preplaces = transition[SUBSTRATES_TAG];
        Json::Value postplaces = transition[PRODUCTS_TAG];
        Json::Value consumed = transition[CONSUMED_TAG];
        
        interactions[i]= *(new Interaction(name,preplaces.size(),postplaces.size(),consumed.size()));
        
        interactions[i].setRateConstant(transition[RATE_CONSTANT_TAG].asDouble());
        interactions[i].delay = transition[DELAY_TAG].asDouble();
        interactions[i].slow = false; interactions[i].reset = false; interactions[i].instant = false;
        string type = transition[TYPE_TAG].asString();
        interactions[i].fba_flux = false;
        if(type==SLOW_TRANSITION_TAG) {
            interactions[i].slow=true;
        } else if(type==INSTANT_TRANSITION_TAG) {
            interactions[i].instant=true;
        } else if(type==RESET_TRANSITION_TAG) {
            interactions[i].reset=true;
        } else if(type==FLUX_TRANSITION_TAG) {
            interactions[i].fba_flux=true;
        }
        
        
        for(int j=0;j<preplaces.size();j++) {
            Json::Value preplace = preplaces[j][SUBSTRATE_TAG];
            interactions[i].setSubstrate(&substances[nameIndex[preplace[NAME_TAG].asString()]],j);
            if(substances[nameIndex[preplace[NAME_TAG].asString()]].getType()==METABOLITE_NODE)
                                            mv.push_back(nameIndex[preplace[NAME_TAG].asString()]);
            interactions[i].setStoichiometry(preplace[STOICHIOMETRY_TAG].asDouble(), j);
            Json::Value act = preplace[ACTIVITY_TAG];
            Activity af = *(new Activity(act.size()));
            for(int k=0;k<af.size;k++) {
               af.setThreshold(act[k][THRESHOLD_TAG].asDouble(), k);
               af.setActivity(act[k][ACTIVITY_ACTIVITY_TAG].asDouble(), k);
            }
            interactions[i].setActivity(af, j);
        }
        
        for(int j=0;j<postplaces.size();j++) {
            Json::Value postplace = postplaces[j][PRODUCT_TAG];
            interactions[i].setProduct(&substances[nameIndex[postplace[NAME_TAG].asString()]], j);
            interactions[i].setProductStoichiometry(postplace[STOICHIOMETRY_TAG].asDouble(), j);
        }
        
        for(int j=0;j<consumed.size();j++) {
            Json::Value cons = consumed[j][CONSUMED_SUBSTRATE_TAG];
            interactions[i].setConsumed(&substances[nameIndex[cons[NAME_TAG].asString()]], j);
        }
        
        interactions[i].initMetabolites((int)mv.size());
        for(int j=0;j<mv.size();j++) interactions[i].setMetabolite(&substances[mv[j]], j);
        mv.clear();
        if(!transition[PROPENSITY_FUNCTION_TAG].empty()) {
            interactions[i].setPropensityFunction(transition[PROPENSITY_FUNCTION_TAG].asString());
            interactions[i].propensity_expression = true;
        }
        if(!transition[RESET_FUNCTION_TAG].empty()) {
            interactions[i].setResetFunction(transition[RESET_FUNCTION_TAG].asString());
            interactions[i].reset_expression = true;
        }
    }
    
    Json::Value qssf = root[GSMN_TAG];
    setSfbaFile(qssf[MODEL_TAG].asString());
    setXtTag(qssf[EXT_TAG_TAG].asString());
    Json::Value objectives = qssf[METABOLITES_TAG];
    Nm = objectives.size();
    metabolites = new Substance*[Nm];
    for(int j=0;j<objectives.size();j++) {
        has_fba = true;
        Json::Value objective = objectives[j][METABOLITE_TAG];
        metabolites[j] = &substances[nameIndex[objective[NAME_TAG].asString()]];
        metabolites[j]->setGsmnMetabolite(objective[GSMN_METABOLITE_TAG].asString());
        Json::Value metact = objective[ACTIVITY_TAG];
        MetaboliteActivity ma = *(new MetaboliteActivity(metact.size()));
        for(int k=0;k<metact.size();k++) {
            ma.setThreshold(metact[k][THRESHOLD_TAG].asDouble(), k);
            ma.setActivity(metact[k][ACTIVITY_ACTIVITY_TAG].asDouble(), k);
        }
        metabolites[j]->setActivity(ma);
    }
    Json::Value constraints = qssf[ENZYMES_TAG];
    Ne = constraints.size();
    enzymes = new Substance*[Ne];
    for(int j=0;j<constraints.size();j++) {
        has_fba = true;
        Json::Value constraint = constraints[j][ENZYME_TAG];
        enzymes[j] = &substances[nameIndex[constraint[NAME_TAG].asString()]];
        Json::Value enz_activity = constraint[ACTIVITY_TAG];
        EnzymeActivity ea = *(new EnzymeActivity(enz_activity.size()));
        for(int k=0;k<enz_activity.size();k++) {
            ea.setThreshold(enz_activity[k][THRESHOLD_TAG].asDouble(), k);
            ea.setLowerBound(enz_activity[k][LOWER_BOUND_TAG].asDouble(), k);
            ea.setUpperBound(enz_activity[k][UPPER_BOUND_TAG].asDouble(), k);
        }
        enzymes[j]->setEnzymeActivity(ea);
        Json::Value flux_list = constraint[ENZYME_LIST_TAG];
        enzymes[j]->allocateRWeights(flux_list.size());
        for(int k=0; k<flux_list.size(); k++) {
            enzymes[j]->setGsmnReaction(flux_list[k].asString());
            enzymes[j]->setRWeight(k, 1.0);
        }
        
    }
    Json::Value requests = qssf[FLUX_REQUESTS_TAG];
    if(!requests.empty()) {
        for(int j=0;j<requests.size();j++) {
            Json::Value rq = requests[j][FLUX_REQUEST_TAG];
            for(int k=0;k<Ns;k++)
                substances[k].addFluxRequest(rq[FLUX_REQUEST_TRANSITION_TAG].asString(),rq[FLUX_REQUEST_FLUX_TAG].asString());
        }
    }
    
}

void PetriNet::runWorkflow(string path) {
	int cursor;
   	fstream infile;
	string word;
	vector<string> text;
	
	infile.open(path.c_str());
	if (!infile) {
		cerr << "Could not open file:" << path << endl;
		exit(0);
	}
	
    
	while(infile>>word) text.push_back(word);
	infile.close();
	
	cursor = 0;
	while(cursor<text.size()) {
        
        if(text[cursor] == "/*") {
            cursor++;
            while(text[cursor]!="*/" && cursor<text.size()) cursor++;
            if(cursor==text.size()) {
                cout << "Unmatched comment" << endl;
                exit(0);
            }
        }
		if(text[cursor]=="GSMN_MONITORS") {
			cursor++;
			while(text[cursor]!="END") {
				gsmnMonitors.push_back(text[cursor]);
				cursor++;
			}
		}
		if(text[cursor]=="PETRI_NET_MONITORS") {
			cursor++;
			while(text[cursor]!="END") {
			    stateMonitors.push_back(text[cursor]);	
				cursor++;
			}
		}
        if(text[cursor]=="INITIAL_STATE") {
            cursor++;
            while(text[cursor]!="END") {
                word = text[cursor];
                cursor++;
                for(int i=0;i<Ns;i++)
                    if(word==substances[i].getName())
                        substances[i].setState(atof(text[cursor].c_str()));
            }
        }
        if(text[cursor]=="PROPENSITY_FUNCTION") {
            string formula = "";
            cursor++;
            int iid = (-1);
            for(int i=0;i<Ni;i++) if(interactions[i].getName()==text[cursor]) { iid = i; break; }
            if(iid == (-1)) { cout << "Unknown transition " << text[cursor] << " in propensity function" << endl; exit(0); }
            cursor++;
            string propensity_equation = "";
            while(text[cursor]!="END") {
                propensity_equation += text[cursor];
                cursor++;
			}
            interactions[iid].setPropensityFunction(propensity_equation);
            interactions[iid].propensity_expression = true;
        }
        if(text[cursor]=="RESET_FUNCTION") {
            string formula = "";
            cursor++;
            int iid = (-1);
            for(int i=0;i<Ni;i++) if(interactions[i].getName()==text[cursor]) { iid = i; break; }
            if(iid == (-1)) { cout << "Unknown transition " << text[cursor] << " in propensity function" << endl; exit(0); }
            cursor++;
            string reset_equation = "";
            while(text[cursor]!="END") {
                reset_equation += text[cursor];
                cursor++;
			}
            interactions[iid].setResetFunction(reset_equation);
            interactions[iid].reset_expression = true;
        }
        if(text[cursor]=="MODEL") {
            cursor++;
			setSfbaFile(text[cursor]);
		}
		if(text[cursor]=="SEED") {
		   cursor++;
		   Seed = atoi(text[cursor].c_str());
		}
		if(text[cursor]=="TIME_MAX") {
			cursor++;
			TimeMax = atof(text[cursor].c_str());
		}
		if(text[cursor]=="MAXIMAL_TIMESTEP") {
		    cursor++;
			maxTimestep = atof(text[cursor].c_str());
		}
		if(text[cursor]=="NUTRIENT_UPDATE_TIME") {
			cursor++;
			nutrientUpdateTime = atof(text[cursor].c_str());
		}
		if(text[cursor]=="NUMBER_OF_ITERATIONS") {
		    cursor++;
			Nmax = atoi(text[cursor].c_str());
		}
		if(text[cursor]=="NUMBER_OF_SAMPLES") {
			cursor++;
			Nsample = atoi(text[cursor].c_str());
		}
		if(text[cursor]=="OUTPUT") {
		    cursor++;
			outfile = text[cursor];
		}
		if(text[cursor]=="LOG") {
		    cursor++;
			logfile = text[cursor];
		}
		if(text[cursor]=="MONITOR") {
			cursor++;
			monitor_fq = atoi(text[cursor].c_str());
		}
        if(text[cursor]=="PROGRESS") {
            cursor++;
            progress_flag = true;
            progress_fq = atoi(text[cursor].c_str());
        }
        if(text[cursor]=="MAX_CHANGE") {
            cursor++;
            maxChange = atof(text[cursor].c_str());
        }
		if(text[cursor]=="TARGET") {
			cursor++;
			targetIdx = nameIndex[text[cursor]];
			cursor++;
			targetThreshold = atof(text[cursor].c_str());
            target_flag = true;
		}
		if(text[cursor]=="HYBRID_SIMULATION1") hybridSimulation1();
		if(text[cursor]=="HYBRID_SIMULATION2") hybridSimulation2();
        if(text[cursor]=="HYBRID_SIMULATION3") {
            if(has_fba) hybridSimulation3();
            if(!has_fba) pnSimulation();
        }
        if(text[cursor]=="SIMULATION") {
            if(has_fba) hybridSimulation3();
            if(!has_fba) pnSimulation();
        }
        if(text[cursor]=="HYBRID_SIMULATION4") hybridSimulation4();
        if(text[cursor]=="TOKEN_GAME1") tokenGame1(); 
		if(text[cursor]=="PRINT_MESSAGES") {
			 msg_flag = true;
			 for(int i=0;i<Ns;i++) substances[i].setMessageFlag(true);
		}
        if(text[cursor]=="PRINT_FBA_TRIGGER") for(int i=0;i<Ns;i++) substances[i].setPrintFbaTrigger(true);
        if(text[cursor]=="PRINT_MAX_CHANGE") setPrintMaxChangeFlag(true);
		if(text[cursor]=="PRINT_SYSTEM") printSystem();
        if(text[cursor]=="PRINT_JSON") printJson(false);
        if(text[cursor]=="PRINT_COMPACT_JSON") printJson(true);
		cursor++;
	}
	
}

/* Function to save the Petri Net in Json */
void PetriNet::printJson(bool fast) {
    Json::Value pn;
    Json::Value place,transition,substrate,product,pair,qssf,metabolite,enzyme,flux_request,consumed;
    Json::Value activity(Json::arrayValue);
    Json::Value flux_list(Json::arrayValue);
    Json::StyledWriter writer;
    Json::FastWriter fast_writer;
    string output;
    
    for(int i=0;i<Ns;i++) {
        place[SUBSTANCE_TAG][NAME_TAG] = substances[i].getName();
        place[SUBSTANCE_TAG][STATE_TAG] = substances[i].getState();
        place[SUBSTANCE_TAG][MAX_TAG] = substances[i].getMax();
        place[SUBSTANCE_TAG][TYPE_TAG] = substances[i].getType();
        pn[SUBSTANCES_TAG].append(place);
    }
    
    for(int i=0;i<Ni;i++) {
        transition[INTERACTION_TAG][NAME_TAG] = interactions[i].getName();
        transition[INTERACTION_TAG][RATE_CONSTANT_TAG] = interactions[i].getRateConstant();
        if(interactions[i].propensity_expression)
            transition[INTERACTION_TAG][PROPENSITY_FUNCTION_TAG] = interactions[i].getPropensityFunction().getExpression();
        if(interactions[i].reset_expression)
            transition[INTERACTION_TAG][RESET_FUNCTION_TAG] = interactions[i].getResetFunction().getExpression();
        if(interactions[i].slow) { transition[INTERACTION_TAG][TYPE_TAG] = SLOW_TRANSITION_TAG; }
        else if(interactions[i].instant)  { transition[INTERACTION_TAG][TYPE_TAG] = INSTANT_TRANSITION_TAG; }
        else if(interactions[i].reset) { transition[INTERACTION_TAG][TYPE_TAG] = RESET_TRANSITION_TAG; }
        else if(interactions[i].fba_flux) { transition[INTERACTION_TAG][TYPE_TAG] = FLUX_TRANSITION_TAG; }
        else { transition[INTERACTION_TAG][TYPE_TAG] = FAST_TRANSITION_TAG; }
        transition[INTERACTION_TAG][DELAY_TAG] = interactions[i].delay;
        for(int j=0;j<interactions[i].numberOfSubstrates();j++) {
            substrate[SUBSTRATE_TAG][NAME_TAG] = interactions[i].getSubstrate(j)->getName();
            substrate[SUBSTRATE_TAG][STOICHIOMETRY_TAG] = interactions[i].getStoichiometry(j);
            Activity *af = interactions[i].getActivity(j);
            for(int k=0; k<af->size;k++) {
                pair[THRESHOLD_TAG] = af->getThreshold(k);
                pair[ACTIVITY_ACTIVITY_TAG] = af->getActivity(k);
                activity.append(pair);
            }
            substrate[SUBSTRATE_TAG][ACTIVITY_TAG] = activity;
            transition[INTERACTION_TAG][SUBSTRATES_TAG].append(substrate);
            activity.clear();
        }
        for(int j=0;j<interactions[i].numberOfProducts();j++) {
            product[PRODUCT_TAG][NAME_TAG] = interactions[i].getProduct(j)->getName();
            product[PRODUCT_TAG][STOICHIOMETRY_TAG] = interactions[i].getProductStoichiometry(j);
            transition[INTERACTION_TAG][PRODUCTS_TAG].append(product);
        }
        for(int j=0;j<interactions[i].numberOfConsumed();j++) {
            consumed[CONSUMED_SUBSTRATE_TAG][NAME_TAG] = interactions[i].getConsumed(j)->getName();
            transition[INTERACTION_TAG][CONSUMED_TAG].append(consumed);
        }
        pn[INTERACTIONS_TAG].append(transition);
        transition.clear();
    }
    
    qssf[MODEL_TAG] = sfba_file;
    qssf[EXT_TAG_TAG] = xt_tag;
    for(int i=0;i<Nm;i++) {
        for(int j=0;j<metabolites[i]->numberOfFluxRequests();j++) {
            flux_request[FLUX_REQUEST_TAG][FLUX_REQUEST_TRANSITION_TAG] = metabolites[i]->getFluxRequestTransition(j);
            flux_request[FLUX_REQUEST_TAG][FLUX_REQUEST_FLUX_TAG] = metabolites[i]->getFluxRequest(j);
            qssf[FLUX_REQUESTS_TAG].append(flux_request);
        }
    }
    for(int i=0;i<Nm;i++) {
        metabolite[METABOLITE_TAG][NAME_TAG] = metabolites[i]->getName();
        metabolite[METABOLITE_TAG][GSMN_METABOLITE_TAG] = metabolites[i]->getGsmnMetabolite();
        for(int j=0; j<metabolites[i]->getActivity().size; j++) {
            pair[THRESHOLD_TAG] = metabolites[i]->getActivity().getThreshold(j);
            pair[ACTIVITY_ACTIVITY_TAG] = metabolites[i]->getActivity().getActivity(j);
            activity.append(pair);
        }
        metabolite[METABOLITE_TAG][ACTIVITY_TAG] = activity;
        qssf[METABOLITES_TAG].append(metabolite);
        activity.clear();
    }
    pair.clear();
    for(int i=0;i<Ne;i++) {
        enzyme[ENZYME_TAG][NAME_TAG] = enzymes[i]->getName();
        //enzyme[ENZYME_TAG][ENZYME_FLUX_TAG] = enzymes[i]->getGsmnReaction();
        for(int j=0;j<enzymes[i]->getEnzymeActivity().size;j++) {
            pair[THRESHOLD_TAG] = enzymes[i]->getEnzymeActivity().getThreshold(j);
            pair[LOWER_BOUND_TAG] = enzymes[i]->getEnzymeActivity().getLowerBound(j);
            pair[UPPER_BOUND_TAG] = enzymes[i]->getEnzymeActivity().getUpperBound(j);
            activity.append(pair);
        }
        enzyme[ENZYME_TAG][ACTIVITY_TAG] = activity;
        for(int j=0;j<enzymes[i]->numberOfGsmnReactions();j++)
                  flux_list.append(enzymes[i]->getGsmnReaction(j));
        enzyme[ENZYME_TAG][ENZYME_LIST_TAG] = flux_list;
        flux_list.clear();
        qssf[ENZYMES_TAG].append(enzyme);
        activity.clear();
    }
    pn[GSMN_TAG] = qssf;
    
    if(fast) {
       cout << fast_writer.write(pn);
    } else {
       cout << writer.write(pn);
    }
    pn.clear();
}

void PetriNet::printSystem() {
	cout << "SUBSTANCES" << endl;
	for(int i=0;i<Ns;i++)
		cout << "   " << substances[i].getName() << " " << substances[i].getState() << " " << substances[i].getMax() << " " << substances[i].getType() << endl;
	cout << "END" << endl << endl;
	cout << "REACTIONS" << endl;
	for(int i=0;i<Ni;i++) {
		cout << "   INTERACTION" << " " << interactions[i].getName() << " " << interactions[i].getRateConstant() << " ";
		if(interactions[i].slow) { cout << "SLOW"; 
		} else { 
			if(interactions[i].instant) {cout << "INSTANT";} else { cout << "FAST"; }
		}
		cout << " " << interactions[i].delay;
		cout << endl;	
		for(int j=0; j<interactions[i].numberOfSubstrates();j++) {
			Substance *s = interactions[i].getSubstrate(j);	
			cout << "      SUBSTRATE " << s->getName()  << endl;
            cout << "         STOICHIOMETRY " << interactions[i].getStoichiometry(j) << endl;
			cout << "         ACTIVITY" << endl;
			Activity *af = interactions[i].getActivity(j);
			for(int k=0; k<af->size;k++) cout << "            " << af->getThreshold(k) << " " << af->getActivity(k) << endl;	
            cout << "         END" << endl;
			cout << "      END" << endl;
		}
		for(int j=0;j<interactions[i].numberOfProducts();j++) {
            if(interactions[i].getProductStoichiometry(j)==1)
              cout << "      PRODUCT " << interactions[i].getProduct(j)->getName() << endl;
            if(interactions[i].getProductStoichiometry(j)>1) {
                cout << "      PRODUCT_STOICHIOMETRY " << interactions[i].getProduct(j)->getName() << " ";
                cout<< interactions[i].getProductStoichiometry(j) << endl;
            }
        }
		for(int j=0;j<interactions[i].numberOfConsumed();j++) cout << "      CONSUMED " << interactions[i].getConsumed(j)->getName() << endl;
		//for(int j=0;j<interactions[i].numberOfMetabolites();j++) cout << "      METABOLITES " << interactions[i].getMetabolite(j)->getName() << endl;
		cout << "   END" << endl;
	}
	cout << "END" << endl;
	cout << endl;
	cout << "GSMN" << endl;
	cout << "   MODEL " << sfba_file << endl;
	cout << "   EXT_TAG " << xt_tag << endl;
    for(int q=0;q<Nm;q++) metabolites[q]->printFluxRequests();
    if(!objective==NULL) {
	  cout << "   OBJECTIVE " << objective->getName() << " " << objective->getGsmnReaction() << endl;
	  cout << "      ACTIVITY " << objective->getActivity().size << endl;
	  for(int j=0; j<objective->getActivity().size; j++)
	    	cout << "         " << objective->getActivity().getThreshold(j)  << " " << objective->getActivity().getActivity(j) << endl;
      cout << "      END" << endl;
	  cout << "   END" << endl;
    }
	for(int i=0;i<Nm;i++) { 
		    cout << "   METABOLITE " << metabolites[i]->getName() << " " << metabolites[i]->getGsmnMetabolite() << endl;
			cout << "      ACTIVITY " << metabolites[i]->getActivity().size << endl;
			for(int j=0; j<metabolites[i]->getActivity().size; j++)
			    cout << "         " << metabolites[i]->getActivity().getThreshold(j)  << " " << metabolites[i]->getActivity().getActivity(j) << endl;
			cout << "      END" << endl;
			cout << "   END" << endl;
	}
	for(int i=0;i<Ne;i++) {
			cout << "   ENZYME " << enzymes[i]->getName() << " " << enzymes[i]->getGsmnReaction() << endl;
			cout << "      ACTIVITY " << enzymes[i]->getEnzymeActivity().size << endl;
			for(int j=0;j<enzymes[i]->getEnzymeActivity().size;j++) 
				cout << "         " << enzymes[i]->getEnzymeActivity().getThreshold(j) << " " << enzymes[i]->getEnzymeActivity().getLowerBound(j) << " " << enzymes[i]->getEnzymeActivity().getUpperBound(j) << endl;
			cout << "      END" << endl;
            if(enzymes[i]->numberOfGsmnReactions()>1) {
                cout << "      LIST " << enzymes[i]->numberOfGsmnReactions() << endl;
                for(int j=0;j<enzymes[i]->numberOfGsmnReactions();j++) {
                    cout << "        " << enzymes[i]->getGsmnReaction(j) << " " << enzymes[i]->getRWeight(j) << endl;
                }
                cout << "      END" << endl;
            }
			cout << "   END" << endl;
	}
	for(int i=0;i<Nn;i++) {
		cout << "   NUTRIENT " << nutrients[i]->getName() << " " << nutrients[i]->getGsmnReaction() << " " << nutrients[i]->getNutrientUnit() << " " << nutrients[i]->getNutrientMax() << endl;
		cout << "      ACTIVITY " << nutrients[i]->getEnzymeActivity().size << endl;
		for(int j=0;j<nutrients[i]->getEnzymeActivity().size;j++) 
			cout << "         " << nutrients[i]->getEnzymeActivity().getThreshold(j) << " " << nutrients[i]->getEnzymeActivity().getLowerBound(j) << " " << nutrients[i]->getEnzymeActivity().getUpperBound(j) << endl;
		cout << "      END" << endl;
		cout << "   END" << endl;
	}
	cout << "END" << endl;
}
	
	
