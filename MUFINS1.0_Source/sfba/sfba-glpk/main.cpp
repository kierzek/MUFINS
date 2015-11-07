/*
* File:   main.cpp
* Author: Albert Gevorgyan
*
* Created on 14 January 2010, 11:32
*/

#include <iostream>
#include <stdlib.h>//w/gni/
#include <time.h>//w/gni/
#include "Driver.h"
//#include "DriverGrb.h"//w/

//options
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "getopt.h"
struct option longopts[] = {
    {"infile", 1, NULL, 'i'},
    {"expfile", 1, NULL, 'j'}, //w/gfa/
    {"dpafile", 1, NULL, 'u'}, //w/dpa/
    //{"gfathr", 1, NULL, 'h'}, //w/gfa/ positive threshold for active flux
    //{"nsam", 1, NULL, 'n'}, //w/gni/ sample size for wgni
    {"file", 1, NULL, 'f'},
    {"bfile", 1, NULL, 'b'},
    {"problem", 1, NULL, 'p'},
    {"objective", 1, NULL, 'o'},
    {"msg", 1, NULL, 'g'},
    {"limit", 1, NULL, 'l'},
    {"arguments", 1, NULL, 'a'},
    {"solver", 1, NULL, 's'},
    {"comments", 0, NULL, 'c'},
    {"minimise", 0, NULL, 'm'},
    {"write", 0, NULL, 'w'},
    {"trim", 0, NULL, 't'},
    {"externals", 1, NULL, 'x'},
    //    {"deltag", 0, NULL, 'd'},
    //    {"temperature", 1, NULL, 'k'},
    //    {"concentrations", 1, NULL, 'q'},
    //{"mip_gap", 1, NULL, 'y'},
    {"Vmax", 1, NULL, 'V'},
    {"Xtag", 1, NULL, 'X'},
    {"epsilon", 1, NULL, 'E'},
    {"zeroes", 0, NULL, 'z'},
    {0, 0, 0, 0}
};
#define OPTSTRING   ":i:j:u:b:p:a:o:g:l:k:f:s:x:V:X:E:mctw"//w/

struct optionargs{
    char* infile;
    char* outfile;
    char* expfile;
    char* dpafile;
    char* bfile;
    char* problem;
    char* objective;
    char* solver;
    char* msglev;
    char* limit;
    char* arguments;
    char* externals;
    char* Vmax;
    char* Xtag;
    char* epsilon;
    bool comments;
    bool minimise;
    bool write;
    bool trim;
    bool zeroes;
};
optionargs optargs={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

void print_options(char**);


int main(int argc, char** argv) {

    if (argc < 2)
        print_options(argv);

    //Driver* driver = new Driver;

    try{
        int opt;
        while ((opt = getopt_long_only(argc, argv, OPTSTRING, longopts, NULL)) != EOF){
            switch (opt){
            case 'i':
                optargs.infile = optarg;
                break;
            case 'j': //w/gfa/
                optargs.expfile = optarg;
                break;
            case 'u': //dpa/
                optargs.dpafile = optarg;
                break;
            case 'f':
                optargs.outfile = optarg;
                break;
            case 'b':
                optargs.bfile = optarg;
                break;
            case 'p':
                optargs.problem = optarg;//w/
                break;
            case 'o':
                optargs.objective = optarg;
                break;
            case 'g':
                optargs.msglev = optarg;
                break;
            case 'l':
                optargs.limit = optarg;
                break;
            case 'm':
                optargs.minimise = false;
                break;
            case 'c':
                optargs.comments = true;
                break;
            case 'a':
                optargs.arguments = optarg;
                break;
            case 's':
                optargs.solver = optarg;
                break;                     
            case 'w':
                optargs.write = true;
                break;
            case 't':
                optargs.trim = true;
                break;
            case 'x':
                optargs.externals = optarg;
                break;          
            case 'V':
                optargs.Vmax = optarg;
                cout << "VMAX:" << ' ' << optargs.Vmax << endl;
                break;
            case 'X':
                optargs.Xtag = optarg;
                break;
            case 'E':
                optargs.epsilon = optarg;
                break;
            case 'z':
                optargs.zeroes = false;
                break;
            /*case 'd':
                optargs.thermo = true;
                break;
            case 'k':
                optargs.set_T(optarg);
                break;
            case 'q':
                optargs.set_conc_range(optarg);
                break;
            case 'y':
                optargs.mip_gap = str2double(optarg);
                break;*/
            }
        }
    }
    catch (exception &e){
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    srand ( time(NULL) );//w/gni/
    //srand ( 11 );//w/gni/
    try {
    //    if(optargs.solver==NULL || string(optargs.solver).find("grb")==string::npos){
            Driver* driver = new Driver;
            if(optargs.infile!=NULL) driver->infile = optargs.infile;
            if(optargs.outfile!=NULL) driver->set_output(optargs.outfile);
            if(optargs.expfile!=NULL) driver->expfile = optargs.expfile;
            if(optargs.dpafile!=NULL) driver->dpafile = optargs.dpafile;
            if(optargs.bfile!=NULL) driver->bfile = optargs.bfile;
            if(optargs.problem!=NULL) driver->set_problem(optargs.problem);
            if(optargs.objective!=NULL) driver->target = optargs.objective;
            if(optargs.solver!=NULL) driver->set_solver(optargs.solver);
            if(optargs.msglev!=NULL) driver->set_msg_lev(optargs.msglev);
            if(optargs.limit!=NULL) driver->set_time(atoi(optargs.limit));
            if(optargs.arguments!=NULL) driver->arg = optargs.arguments;
            if(optargs.externals!=NULL) driver->ext = optargs.externals;
            if(optargs.Vmax!=NULL) driver->vmax = str2double(optargs.Vmax);
            if(optargs.Xtag!=NULL) driver->xtag = optargs.Xtag;
            if(optargs.epsilon!=NULL) driver->set_epsilon(optargs.epsilon);
            if(optargs.comments!=NULL) driver->withcomments = optargs.comments;
            if(optargs.minimise!=NULL) driver->max = optargs.minimise;
            if(optargs.write!=NULL) driver->write = optargs.write;
            if(optargs.trim!=NULL) driver->dotrim = optargs.trim;
            if(optargs.zeroes!=NULL) driver->withzeroes = optargs.zeroes;
            driver->run();
            delete driver;
    //    }
    //    else{
    //        DriverGrb* driverGrb = new DriverGrb;
    //        if(optargs.infile!=NULL) driverGrb->infile = optargs.infile;
    //        if(optargs.outfile!=NULL) driverGrb->set_output(optargs.outfile);
    //        if(optargs.expfile!=NULL) driverGrb->expfile = optargs.expfile;
    //        if(optargs.dpafile!=NULL) driverGrb->dpafile = optargs.dpafile;
    //        if(optargs.bfile!=NULL) driverGrb->bfile = optargs.bfile;
    //        if(optargs.problem!=NULL) driverGrb->set_problem(optargs.problem);
    //        if(optargs.objective!=NULL) driverGrb->target = optargs.objective;
    //        if(optargs.solver!=NULL) driverGrb->set_solver(optargs.solver);
    //        if(optargs.msglev!=NULL) driverGrb->set_msg_lev(optargs.msglev);
    //        if(optargs.limit!=NULL) driverGrb->set_time(atoi(optargs.limit));
    //        if(optargs.arguments!=NULL) driverGrb->arg = optargs.arguments;
    //        if(optargs.externals!=NULL) driverGrb->ext = optargs.externals;
    //        if(optargs.Vmax!=NULL) driverGrb->vmax = str2double(optargs.Vmax);
    //        if(optargs.Xtag!=NULL) driverGrb->xtag = optargs.Xtag;
    //        if(optargs.epsilon!=NULL) driverGrb->set_epsilon(optargs.epsilon);
    //        if(optargs.comments!=NULL) driverGrb->withcomments = optargs.comments;
    //        if(optargs.minimise!=NULL) driverGrb->max = optargs.minimise;
    //        if(optargs.write!=NULL) driverGrb->write = optargs.write;
    //        if(optargs.trim!=NULL) driverGrb->dotrim = optargs.trim;
    //        if(optargs.zeroes!=NULL) driverGrb->withzeroes = optargs.zeroes;
    //        driverGrb->run();
    //        delete driverGrb;
    //    }        
    //} catch(GRBException e) {
    //    cout << "Error code = " << e.getErrorCode() << endl;
    //    cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }
    exit(EXIT_SUCCESS);

}


void print_options(char** argv){
    char* program = argv[0];
    printf("%s\n", program);
    printf("USAGE:%s -i model_file [-j expression_file][-p problem_type] [-o obj_fun] [-a arg_list] [-s solver_algorithm] [-b problem_file]\
[-u dpaplot_file] [-f output_file] [-x ext_list] [-X exteranl_tag] [-g msg_level] [-l time_limit]\
[-m] [-t] [-c] [-w] [-V max_rate] [-k temperature] [-q conc_range] [-d read_deltag]\n", program);
    printf("-infile    (-i) The name of the model file in flux analyser like format.\n");
    printf("-expfile   (-j) The name of the expression file\n"); //w/gfa/
    printf("-dpafile   (-u) The name of the dpaplot output file\n"); //w/dpa/
    printf("-problem   (-p) The analysis program:\n");
    printf("    show         - show the model contents\n");
    printf("    objvalue     - objective function value\n");
    printf("    objstat      - objective function value and status\n");
    printf("    fba          - FBA\n");
    printf("    fva          - FVA\n");
    //printf("    fsa          - FSA\n");
    printf("    em           - elementary modes\n");    
    printf("    ko           - knockout genes/enzymes or reactions\n");
    printf("    ess          - essential reactions\n");
    printf("    live         - live reactions\n");
    printf("    plot         - plot objective function against another flux\n");
    printf("    plot3d       - plot against 2 fluxes\n");
    printf("    mss          - minimal substrate sets\n");
    printf("    mps          - minimal product sets\n");
    printf("    imat[,thr] - Flux activity analysis using iMAT method, thr: threshold for active flux (defualt:1)\n"); //w/gfa/old name: gfaa
    //    printf("    gfva        - genome-scale FVA\n"); //w/gfa/
    printf("    fimat      - an alternative method to iMAT method which is much faster\n"); //w/gfa/old name: gfva2
    //printf("    limat      - an alternative method to iMAT method only using linear programming\n"); //w/gfa/old name: gfva3
    printf("    gimme[,thr]  - Gene Inactivity Moderated by Metabolism and Expression using FBA, thr: threshold for active flux (defualt:12)\n");
    printf("    gimmefva[,thr]  - Gene Inactivity Moderated by Metabolism and Expression using FVA, thr: threshold for active flux (defualt:12)\n");
    printf("    gim3e        - FVA under metabolic constraints based on GIM3E method\n");//w/gni/
    printf("    sgni         - strong GNI analysis\n");//w/gni/
    printf("    wgni[,sam]   - weak GNI analysis, sam: growth media sample size (defualt:33000)\n");//w/gni/
    printf("    dpaplot      - DPA analysis of producibility plot\n");//w/dpa/
    printf("    dpasig       - DPA analysis of metabolite signals\n");//w/dpa/
    //    printf("    bottlenecks - directional or thermodynamic bottlenecks\n");
    printf("    uncons      - unconserved metabolites\n");
    printf("    orphans     - orphan metabolites\n");
    printf("    cc          - connected components\n");
    printf("    troff       - tradeoff between two objectives\n");// Zakrzewski's change
    printf("-objective (-o) Objective function, defined as a linear expression including reaction and/or metabolite names, or expression array name.\n");
    printf("-arguments (-a) Space-delimited list of problem-specific arguments.\n");
    printf("-solver    (-s) For GLPK solver you can choose simplex, exact or milp[,mip_gap] with optional mip gap tolerance of MILP.\n");
    printf("-bfile     (-b) Problem file with definitions of additional constraints, objective and problem-specific arguments\n");
    printf("-file      (-f) Output file (stdout by default)\n");
    printf("-externals (-x) List of external metabolites.\n");
    printf("-msg       (-g) Messages during solution.\n");
    printf("-limit     (-l) Time limit in milliseconds.\n");
    printf("-comments  (-c) Comments in the output on/off.\n");
    printf("-minimise  (-m) Define optimisation direction as minimisation, on/off (maximisation by default).\n");
    printf("-trim      (-t) Reduce the stoichiometry matrix before solving LP.\n");
    printf("-write     (-w) Write the LP to the file 'GLPKmodel.LP'or 'GRBmodel.LP', on/off.\n");
    printf("-Vmax      (-V) maximal flux rate in the model (for MILP).\n");
    printf("-Xtag      (-X) external tag indicating external reactions.\n");//w/gni/
    printf("-epsilon   (-E) round-off error tolerance threshold (default:1e-5).\n");
    //printf("-mip_gap   (-y) relative mip gap tolerance (for MILP).\n");
    //printf("-gfathr    (-h) positive threshold for active flux.\n");//w/gfa/
    //printf("-nsam      (-n) media sample size for wgni.\n");//w/gni/

    exit(EXIT_SUCCESS);
}


/*
printf("AMKFBA upgraded with GLPK-4.41.\n");
printf("USAGE:%s -i input_file -p program -obj opt_target further options\n",argc[0]);
printf("-i The name of the file in flux analyser like format\n");
printf("-f The file containing the list of command line options (one option per line)\n");
printf("-p The analysis program:\n");
printf("    fba         - single FBA run\n");
printf("    objvalue    - Compute objective function value and print it\n");
printf("                  as a single number to stdout\n");
printf("    objstat     - compute objective function value and status\n");
printf("                  and print as one line output\n");
printf("    metabolites - print metabolite list\n");
printf("    genes       - print gene list\n");
printf("    reactions4gene - print input file lines of reactions which \n");
printf("                     are inactivated by the gene\n");
printf("    fametlist   - print metabolite list in FluxAnalyser format\n");
printf("    kogene      - knockout gene and run FBA\n");
printf("    genes2reactions - print gene/reaction associations\n");
printf("    g2rMatrix - print gene protein associations in sparse matrix format\n");
printf("-obj Optimisation target name. It may be reaction or metabolite name\n");
printf("-bfile flux (column) bounds file\n");
printf("-rbfile rate (row) bounds file\n");
printf("-ko gene name - the gene to be inactivated\n");
printf("-min - compute minimal value of the objective function\n");
printf("-metext - set metabolite as external");
printf("\n");
*/
