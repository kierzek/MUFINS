/*
 *  PetriNet.h
 *  pnfba_xcode
 *
 *  Created by Andrzej Kierzek on 31/12/2010.
 *  Copyright 2010 Andrzej Kierzek. All rights reserved.
 *
 */

#include "Trajectory.h"
#include "AlgebraicExpression.h"
#include "json.h"

#define SUBSTANCE_NODE 1
#define METABOLITE_NODE 2
#define ENZYME_NODE 3
#define NUTRIENT_NODE 4
#define OBJECTIVE_NODE 5
#define FLUX_NODE 6
#define OBJECTIVE_TOLERANCE 1e-3
#define LB_INF -999999.0
#define UB_INF 999999.0
#define TIME_INF 1e30
#define STATE_INF 1e30
#define MIN_DT 1e-9
#define DEFAULT_MAX_CHANGE 0.01
#define BIOMASS_UNIT 1000
#define NOEVENT "none"
#define VERSION "qsspn version 3.0 compiled on 18-01-2015; 08:35."

#define SUBSTANCE_TAG "place"
#define SUBSTANCES_TAG "places"
#define INTERACTION_TAG "transition"
#define INTERACTIONS_TAG "transitions"
#define SUBSTRATE_TAG "preplace"
#define SUBSTRATES_TAG "preplaces"
#define PRODUCT_TAG "postplace"
#define PRODUCTS_TAG "postplaces"
#define ACTIVITY_TAG "activity"
#define THRESHOLD_TAG "t"
#define ACTIVITY_ACTIVITY_TAG "a"
#define STOICHIOMETRY_TAG "stoichiometry"
#define NAME_TAG "name"
#define MAX_TAG "max"
#define STATE_TAG "state"
#define TYPE_TAG "type"
#define RATE_CONSTANT_TAG "c"
#define SLOW_TRANSITION_TAG "SLOW"
#define FAST_TRANSITION_TAG "FAST"
#define INSTANT_TRANSITION_TAG "DETERMINISTIC"
#define RESET_TRANSITION_TAG "RESET"
#define FLUX_TRANSITION_TAG "FLUX"
#define DELAY_TAG "delay"
#define CONSUMED_TAG "consumed"
#define CONSUMED_SUBSTRATE_TAG "consumed_preplace"
#define GSMN_TAG "qssf"
#define METABOLITE_TAG "objective"
#define METABOLITES_TAG "objectives"
#define ENZYME_TAG "constraint"
#define ENZYMES_TAG "constraints"
#define FLUX_REQUEST_TAG "flux_request"
#define FLUX_REQUEST_TRANSITION_TAG "transition"
#define FLUX_REQUEST_FLUX_TAG "flux"
#define FLUX_REQUESTS_TAG "flux_requests"
#define EXT_TAG_TAG "externality_tag"
#define MODEL_TAG "sfba_file"
#define GSMN_METABOLITE_TAG "objective"
#define ENZYME_FLUX_TAG "flux"
#define LOWER_BOUND_TAG "lb"
#define UPPER_BOUND_TAG "ub"
#define ENZYME_LIST_TAG "flux_list"
#define PROPENSITY_FUNCTION_TAG "propensity_function"
#define RESET_FUNCTION_TAG "reset_function"

class Interaction;

class Activity {
private:
	double *thresholds;
	double *activities;
    bool mass_action;
	
	
public:
	int size;
	Activity() {
	}
	Activity(int s) {
		thresholds = new double[s];
		activities = new double[s];
		size=s;
        if(s==0) {
            mass_action = true;
        } else {
            mass_action=false;
        }
	}
	double getThreshold(int i) {
		return thresholds[i];
	}
	double getActivity(int i) {
	    return activities[i];	
	}
	void setThreshold(double t, int i) {
		thresholds[i] = t;
	}
	void setActivity(double a, int i) {
		activities[i] = a;
	}
    void setMassAction(bool b) {
        mass_action = b;
    }
    bool isMassAction() {
        return mass_action;
    }
};

class MetaboliteActivity {
private:
    double *thresholds;
	double *activities;
	
public:
	int size;
	MetaboliteActivity() {
	}
	MetaboliteActivity(int s) {
		thresholds = new double[s];
		activities = new double[s];
		size = s;
	}
	double getThreshold(int i) {
		return thresholds[i];
	}
	double getActivity(int i) {
	    return activities[i];	
	}
	void setThreshold(double n, int i) {
		thresholds[i] = n;
	}
	void setActivity(double a, int i) {
		activities[i] = a;
	}
	double activity(double a);
};

class EnzymeActivity {
private:
	double *thresholds;
	double *lowerbounds;
	double *upperbounds;

public:
	int size;
	EnzymeActivity() {
	}
	EnzymeActivity(int s) {
		thresholds = new double[s];
		lowerbounds = new double[s];
		upperbounds = new double[s];
		size = s;
	}
	double getThreshold(int i) {
		return thresholds[i];
	}
	void setThreshold(double t,int i) {
		thresholds[i] = t;
	}
	double getLowerBound(int i) {
		return lowerbounds[i];
	}
	void setLowerBound(double l,int i) {
		lowerbounds[i] = l;
	}
	double getUpperBound(int i) {
		return upperbounds[i];
	}
	void setUpperBound(double u, int i) {
	    upperbounds[i] = u;	
	}
	void activity(double t, double &lb, double &ub);
};

// Substance/Place node of the Petri Net layer.
class Substance {
private:
	string name;        // The name and ID
	double state;       // Current state
	double flux;        // Maximal flux towards this metabolite
	double amount;      // Amount of substance as continous variable
	double max;         // Maximal number of states 0..max
	int type;           // Type of the node. 1 - substance node, 2 - metabolite node, 3 - enzyme node
	MetaboliteActivity activity;     // Metabolite activity if the node is METABOLITE_NODE
	EnzymeActivity enzActivity;      // Enzyme activity if the node is ENZYME_NODE
	string gsmn_metabolite;          // Associated GSMN metabolite if the node is METABOLITE_NODE
	string gsmn_reaction;            // Associated GSMN reaction if node is ENZYME_NODE
    strvec gsmn_reactions;           // List of associated GSMN reactions if node is ENZYME_NODE
    double *rweights;                // Weights of associated GSMN reactions if node is ENZYME_NODE
	bool flag1;                      // If true, metabolite node requires update
	double lbound, ubound;           // lower and upper bound of an ENZYME_NODE
	double nutrientUnit;             // Scalling factor for external metabolite dynamics
	double nutrientMax;              // MAximal external metabolite uptake rate
	bool msg_flag;                   // Print messages if true
    bool print_fba_trigger;          // Print enzyme that triggered FBA when true
    vector<Interaction> flux_requests;  // Vector of interaction requesting fba solution fluxes

public:
	double stateChange;    // Change of state in synchronous update algorithm
    double resetValue;     // Variable used by reset function
    bool resetDone;        // The state of this place has been already reset in the same time slice
	Substance() {
        resetDone = false;
	}
	Substance(string nm,double st,double mx,int t) {
		name = nm;
		state = st;
		max = mx;
		type = t;
		stateChange = 0;
		flag1 = false;
		flux = 0.00;
		msg_flag = false;
        resetDone = false;
	}
	void update() {
		state += stateChange;	
		state = (state>max) ? max : state;
		state = (state<0) ? 0 : state;
		stateChange = 0;
	}
	void update(int s) {
		state += s;	
		state = (state>max) ? max : state;
		state = (state<0) ? 0 : state;
	}
	string getName() {
		return name;
	}
	void setName(string nm) {
		name = nm;
	}
    double getState() {
		return state;
	}
	void setState(double s) {
		state = s;
	}
    double *getStateReference() {
        return &state;
    }
	double getAmount() {
		return amount;
	}
	void setAmount(double a) {
		amount = a;
	}
	double getFlux() {
	    return flux;	
	}
	void setFlux(double f) {
	    flux = f;	
	}
	int getMax() {
		return max;
	}
	void setMax(int m) {
		max = m;
	}
	int getType() {
		return type;
	}
	void setType(int t) {
		type = t;
	}
	void setGsmnMetabolite(string nm) {
	    gsmn_metabolite = nm;	
	}
	string getGsmnMetabolite() {
		return gsmn_metabolite;
	}
	MetaboliteActivity getActivity() {
	    return activity;	
	}
	void setActivity(MetaboliteActivity af) {
	    activity = af;	
	}
	EnzymeActivity getEnzymeActivity() {
	    return enzActivity;	
	}
	void setEnzymeActivity(EnzymeActivity enz) {
	    enzActivity = enz;	
	}
	void setNutrientUnit(double u) {
	    nutrientUnit = u;	
	}
	double getNutrientUnit() {
		return nutrientUnit;	
	}
	void setNutrientMax(double u) {
		nutrientMax = u;
	}
	double getNutrientMax() {
		return(nutrientMax);
	}
	string getGsmnReaction() {
	    return gsmn_reaction;	
	}
    string getGsmnReaction(int n) {
        return gsmn_reactions[n];
    }
	void setGsmnReaction(string rn) {
	    gsmn_reaction = rn;	
        gsmn_reactions.push_back(rn);
	}
    int numberOfGsmnReactions() {
        return (int)gsmn_reactions.size();
    }
    int numberOfFluxRequests() {
        return (int)flux_requests.size();
    }
    string getFluxRequestTransition(int i);
    string getFluxRequest(int i);
	void fbaRequired(bool f) {
		flag1 = f;
	}
	bool isFbaRequired() {
		return flag1;
	}
	void setMessageFlag(bool f) {
		msg_flag = f;
	}
    void setPrintFbaTrigger(bool f) {
        print_fba_trigger = f;
    }
    void allocateRWeights(int n) {
        rweights = new double[n];
    }
    void setRWeight(int n, double w) {
        rweights[n] = w;
    }
    double getRWeight(int n) {
        return rweights[n];
    }
    void addFluxRequest(Interaction i);
	void metaboliteAction(Driver *gsmn);
	void producibilityState(Driver *gsmn);
	bool enzymeAction(Driver *gsmn);
	void initialEnzymeAction(Driver *gsmn);
	void setEnzymeBounds(Driver *gsmn);
	void objectiveAction(Driver *gsmn);
	void objectiveAction(Driver *gsmn, double dt);
	void nutrientState(Driver *gsmn, double dt);
	void nutrientState(Driver *gsmn, double dt, double biomass);
	void setNutrientBounds(Driver *gsmn);
	void setNutrientBounds(Driver *gsmn, double dt, double biomass);
    void printFluxRequests();
    void addFluxRequest(string in, string r);
};

//Interaction/Transition node of the Petri Net layer
class Interaction {
private:
	string name;
	Substance **substrates;                    // Reaction substrates
	Substance **products;                      // Reaction products
	Substance **consumed;                      // Substrates which are consumed
	Substance **metabolites;                   // Substrates that require call to GSMN.
	Activity  *activities;                     // Activity functions of substrates
    double *activity_values;                   // Activity function values for substrates
    double *stoich;                               // Stoichiometric coefficients of substrates.
    double *pstoich;                              // Stoichiometric cefficients of products.
	int Ns;                                    // Number of substrates equal to number of activities.
	int Np;                                    // Number of products
	int Nc;                                    // Number of consumed
	int Nm;                                    // Number of metabolites
    AlgebraicExpression propensityFunction;    // Propensity function.
    AlgebraicExpression resetFunction;         // Function that calculates states of products in reset transition.
	
public:
	double a;        // Propensity
	double c;        // Rate constant
	double delay;    // Delay time
	bool slow;       // Slow, transient reaction if TRUE
	bool instant;    // Fast reaction which is instant if TRUE
    bool reset;      // Reset reaction if True
    bool fba_flux;       // Flux reaction
    bool propensity_expression; // Propensity calculated by algebraic expression if TRUE
    bool reset_expression;      // Reset value calculated by algebraic expression if TRUE
    string flux_request;        // Name of flux variable to request from FBA

	void stateChange();             // Calculate state change for each of the substrate and product nodes
    void elementaryChange();        // Fire one elementary reaction and update state change of the substrates and products
	void execute();                 // Execute one interaction event
	void fire();                    // Fire one elementary transition.
    void set();                     // Reset states of product nodes
    void flux(Driver *gsmn);        // Request solution flux from gsmn and set product node state to this flux
	double activity(int i);         // Calculate activity of substrate i
	double propensity(Driver *gsmn);  // Calculate propenisty together with caclulations of metabolite producibility
	double propensity();            // Calculate propensity function and return its value
	
	Interaction() {
	} 
	Interaction(string nm,int ns,int np,int nc);
	Substance *getSubstrate(int i) {
		return substrates[i];
	}
	Substance *getProduct(int i) {
		return products[i];
	}
	Substance *getConsumed(int i) {
		return consumed[i];
	}
	Substance *getMetabolite(int i) {
	    return metabolites[i];	
	}
	Activity *getActivity(int i) {
	    return &activities[i];	
	}
    double getStoichiometry(int i) {
        return stoich[i];
    }
    double getProductStoichiometry(int i) {
        return pstoich[i];
    }
	void setSubstrate(Substance *s, int i) {
	    substrates[i] = s;
	}
    void setStoichiometry(double s, int i) {
        stoich[i] = s;
    }
    void setProductStoichiometry(double s, int i) {
        pstoich[i] = s;
    }
	void setProduct(Substance *s,int i) {
	    products[i] = s;
	}
	void setConsumed(Substance *s, int i) {
		consumed[i] = s;
	}
	void initMetabolites(int i) {
	    Nm = i;
		metabolites = new Substance*[Nm];	
	}
	void setMetabolite(Substance *s, int i) {
        if(fba_flux) s->addFluxRequest(*this);
	    metabolites[i] = s;	
	}
	void setActivity(Activity af, int i) {
	    activities[i] = af;	
	}
	int numberOfSubstrates() {
		return Ns;
	}
	int numberOfProducts() {
		return Np;
	}
	int numberOfConsumed() {
		return Nc;
	}
	int numberOfMetabolites() {
		return Nm;
	}
	string getName() {
		return name;	
	}
	double getRateConstant() {
	    return c;	
	}
	void setRateConstant(double rc) {
	    c = rc;
	}
    void setFluxProducts() {
        for(int i=0;i<Np;i++) products[i]->setType(FLUX_NODE);
    }
    void setPropensityFunction(string formula);
    void setResetFunction(string formula);
    AlgebraicExpression getPropensityFunction() {
        return propensityFunction;
    }
    AlgebraicExpression getResetFunction() {
        return resetFunction;
    }
};

class RandomNumberGenerator {
private:
	int i96,j96;
	float u[97],uni,c,cd,cm;
public:
	RandomNumberGenerator() {
	}
	RandomNumberGenerator(int sd) {
		rmarzam(sd);	
	}
	void rmarzam( int newseed );
	double marzam();
	int uniform_index(int max) {
		return floor(marzam()*(max));
	}
};

class PetriNet {
private:
	Substance *substances;        // Petri Net nodes
	Interaction *interactions;    // Petri Net 
	Substance **enzymes;          // Pointers to the nodes of the type ENZYME_NODE
	Substance **metabolites;      // Pointers to the nodes of the type METABOLITE_NODE
	Substance **nutrients;        // Pointers to the nodes of the type NUTRIENT_NODE
	Substance *objective;         // Pointer to the OBJECTIVE_NODE
	Interaction **active;         // Pointers to interactions that can happen in given interaction
	Interaction **slow;           // Pointers to slow interactions
	Interaction **fast;           // Pointers to fast interactions
    Interaction **instant;        // Pointers to instant interactions
    Interaction **reset;          // Pointers to reset interactions
	map <string, int> nameIndex;  // Name-index mapping of node names. Each name is associated with the index in substances
	int Ns;                       // Number of substances
	int Ni;                       // Number of interactions
	int Nm;                       // Number of metabolite nodes
	int Ne;                       // Number of enzyme nodes
	int Nn;                       // Number of nutrient nodes
	int Nia;                      // Number of active interactions
	int Nslow;                    // Number of slow reactions
	int Nfast;                    // Number of fast reactions
    int Ninstant;                 // Number of instant interactions
    int Nreset;                   // Number of resest interactions
	vector<int> delayed;          // Indexes of delayed reactions
	vector<double> delays;        // Delay times
	Driver *gsmnDriver;           // Driver class of the GSMN model
	string sfba_file;             // Path to GSMN file in sfba format
	string outfile;               // Path to the output file.
	string logfile;               // Path to progress monitor file
	int monitor_fq;               // Frequency of saving states into trajectory.
    int iteration_counter;        // Simulation iteration counter
    int progress_fq;              // Frequency of progress reports.
	string xt_tag;                // External metabolite tag
	strvec gsmnMonitors;          // Objective functions to explore GSMN after at the given iteration.
	strvec stateMonitors;         // Names of nodes for which state will be stored in trajectory
	Trajectory *trajectory;        // Simulation results
	Trajectory **trajectories;     // Simulation results
	int Nmax;                     // Maximal number of simulation steps
	int Nsample;                  // Sample size
	int Seed;                     // Seed of Random number generator
	double Time;				  // Absolute simulation time
	double deltaTime;             // Time increment in the particular iteration.
	double maxTimestep;           // Maximal timestep parameter for hybrid simulation.
    double maxChange;             // Maximal relative substance state change for adaptive Euler timestep calculation.
	double nutrientUpdateTime;    // Time after which nutrients are to be updated.
	double timeSinceNutrientUpdate;  // Time that passed since last nutrient update.
	double TimeMax;               // Maximal time of the simulation
	RandomNumberGenerator *rnd;     // Random Number Generator
	State *initialState;           // Initial state of the Petri net
	int targetIdx;                // The index of the target node in reachability sampling
	double targetThreshold;          // Threshold state of the target node in reachability sampling//
	bool fbaRequired;             //  Metabolite producibilities need to be updated
	bool msg_flag;                // Print output if true
    bool progress_flag;           // Print progress report
    bool target_flag;             // Check whether target has been reached
    bool has_fba;                 // Equals true if system contains FBA part.
    bool print_max_change;        // Print max state change when true

public:
	PetriNet() {
        maxChange = DEFAULT_MAX_CHANGE;
        progress_flag = false;
        msg_flag = false;
        monitor_fq = 1;
        target_flag = false;
        targetThreshold = STATE_INF;
        has_fba=false;
        print_max_change = false;
	}
	PetriNet(int ns, int ni);
    
    void setPrintMaxChangeFlag(bool f) {
        print_max_change = f;
    }
    void setSfbaFile(string f) {
		sfba_file = f;
	}
	void setXtTag(string t) {
		xt_tag = t;
	}
	void setTimeMax(double t) {
		TimeMax = t;
	}
	void setMaximalTimestep(double t) {
		maxTimestep = t;
	}
    void setMaximalChange(double c) {
        maxChange = c;
    }
    bool hasFBA() {
        return(has_fba);
    }
    void setHasFBA(bool f) {
        has_fba = f;
    }
	void readModel(string path);
    void readPetriNet(string path);
	void readSubstances(fstream &f);
	void readReactions(fstream &f);
	void readMonitors(fstream &f);
	void readGsmnMonitors(fstream &f);
	Interaction readInteraction(fstream &f);
	void read_gsmn_info(fstream &f);
	void printSystem();
    void printJson(bool fast);
	void run();
	void runWorkflow(string path);
	void buildGsmnDriver();
	State *getState();
	void setState(State *st) {
	  for(int i=0;i<Ns;i++) substances[i].setState(st->getState(i));	
	}
	void runGsmnMonitors();
	void runStateMonitors();
	void runGsmnMonitors(int trj);
	void runGsmnMonitors(int trj,int it) {
	    trajectories[trj]->pushGsmnIteration(it);
		runGsmnMonitors(trj);
	}
	void runStateMonitors(int trj);
	void runStateMonitors(int trj, int it) {
	   	trajectories[trj]->pushIteration(it);
		runStateMonitors(trj);
	}
	void runStateMonitors(int trj, int it, string e) {
	   	trajectories[trj]->pushIteration(it);
		trajectories[trj]->pushEvent(e);
		runStateMonitors(trj);
	}
	void runStateMonitors(int trj, int it, string e,double t) {
	   	trajectories[trj]->pushIteration(it);
		trajectories[trj]->pushEvent(e);
		trajectories[trj]->pushTime(t);
		runStateMonitors(trj);
	}
	void runStateMonitors(int trj, string e,double t) {
		trajectories[trj]->pushEvent(e);
		trajectories[trj]->pushTime(t);
		runStateMonitors(trj);
	}
	void runMonitors(double t) {
		trajectory->pushTime(t);
		runGsmnMonitors();
		runStateMonitors();
	}
	void runMonitors(double t,int trj) {
		trajectories[trj]->pushTime(t);
		runGsmnMonitors(trj);
		runStateMonitors(trj);
	}
	void runMonitors(int it) {
		trajectory->pushIteration(it);
		runGsmnMonitors();
		runStateMonitors();
	}
	void runMonitors(int it,int trj) {
		trajectories[trj]->pushIteration(it);
		runGsmnMonitors(trj);
		runStateMonitors(trj);
	}
	void synchronousUpdate();                    // Run one step of synchronous update algorithm.
	string asynchronousUpdate();                 // Execute one step of asynchronous update and return the name of interaction that happened
	string ssaUpdate();                          // Execute one step of asynchronous update and return the name of interaction that happened
	void NutrientDynamics(double dt, int max);
	void prepareForSampling(int ns,int sd);
	void splitReactions();                       // Assigning transition pointers to reaction type arrays
	void updateFastReactions1();                 // First version of fast reaction update for full hybrid simulation algorithm
    void updateFastReactions2();                 // Fast reaction update with naive adaptive timestep based on relative change
	void updateFastReactions3(double maxdt);     // updateFastReactions2 with maximal timestep as argument
    void updateInstantReactions();               // Instant interaction update
    void updateResetReactions();                 // Execute reset interactions
    string hybridUpdate1();                      // First version of event update for full hybrid simulation algorithm
    string hybridUpdate2();                      // Hybrid simulation routine compatible with updateFastReactions2()
    string hybridUpdate3();                      // hybridUpdate2 with corrected execution of updateFastReactions2()
	void hybridSimulation1();                    // First version of full hybrid simulation algorithm.
	void hybridSimulation2();                    // Second version of hybrid simulation algorithm
    void hybridSimulation3();                    // Hybrid simulation without the notion of BIOMASS and NUTRIENT
    void hybridSimulation4();                    // Third version of hybrid simulation algorithm with dFBA.
    void tokenGame1();                           // Simple token game simulation without GSMN
    void pnSimulation();                         // Hybrid simulation without metabolism (Petri Net only)
	int fireDelayedReaction();
    
};

