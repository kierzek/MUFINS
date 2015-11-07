#include "TxtParser.h"


bool fexists(const string filename)//w/ check if file exist
{
  ifstream ifile((char*)filename.c_str());
  return ifile;
}

void readfile(string& filename, string* s){
    char c;
    ifstream in(filename.data(), ios::in);
    if (in == NULL) throw runtime_error(filename + ": file could not be opened!");
    while ((c = in.get()) != EOF)
        (*s) += c;
}
    

void readlines(string& filename, strvec* vec){
    string* str = new string;
    readfile(filename, str);
    split(str, vec, NEWLINE);
    delete str;
}


//w/dpa/ parce array expression file (for float number)
void parse_arrayExpression(string &filename, strvec &arrName, strstr2int &arrexp){
//    strvec tisName;
    ifstream is(filename.data(), ios::in);
    string line;
    getline(is, line, NEWLINE);//get first line, array name
    if (line.size() > 0 && !isallspace(line)){
            try{
                split(&line, &arrName, TAB);
                arrName.erase(arrName.begin());
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(0), e.what());
            }
    }

    strvec toks;
    int Narr=arrName.size(); //number of arrays
    for (int i = 0; getline(is, line, NEWLINE); i++){
        if (line.size() > 0 && !isallspace(line)){
            try{
                split(&line, &toks, TAB);
                for(int j=1;j<Narr+1;j++){
                    string arr=arrName[j-1];
                    string Gn=toks[0];
                    if(toks[j]=="NA")  arrexp[Gn][arr]=100;//if "NA" then set number 100 for expression level
                    else arrexp[Gn][arr]=atoi(toks[j].data());
                }
                toks.clear();
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(i), e.what());
            }
        }
    }
}

//w/dpa/ parce array expression file (for float number)
void parse_arrayExpression_f(string &filename, strvec &arrName, strstr2flo &arrexp){
//    strvec tisName;
    ifstream is(filename.data(), ios::in);
    string line;
    getline(is, line, NEWLINE);//get first line, array name
    if (line.size() > 0 && !isallspace(line)){
            try{
                split(&line, &arrName, TAB);
                arrName.erase(arrName.begin());
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(0), e.what());
            }
    }

    strvec toks;
    int Narr=arrName.size(); //number of arrays
    for (int i = 0; getline(is, line, NEWLINE); i++){
        if (line.size() > 0 && !isallspace(line)){
            try{
                split(&line, &toks, TAB);
                for(int j=1;j<Narr+1;j++){
                    string arr=arrName[j-1];
                    string Gn=toks[0];
                    if(toks[j]=="NA")  arrexp[Gn][arr]=1e5;//if "NA" then set number 100 for expression level
                    else arrexp[Gn][arr]=atof(toks[j].data());
                }
                toks.clear();
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(i), e.what());
            }
        }
    }
}

//w/dpa/ parse dapplot output to get map of metabolites to genes
void parse_dpaplot(string &filename, strvec &mets, met_genes &metG){
    ifstream is(filename.data(), ios::in);
    string line;
    for (int i=0; i<2;i++) getline(is, line, NEWLINE);//from third line to read

    strvec toks;
    for (int i = 0; getline(is, line, NEWLINE); i++){
        if (line.size() > 0 && !isallspace(line)){
            try{
                split(&line, &toks, TAB);
                string met=toks[0];
                if(met=="///") break;
                strvec genes;
                mets.push_back(met);
                genes.assign(toks.begin()+1,toks.end());
                metG[met]=genes;
                genes.clear();
                toks.clear();
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(i), e.what());
            }
        }
    }

}


void parse_line_file(string &filename, Model* model, _parse_line parse_line, string xtag){
    ifstream is(filename.data(), ios::in);
    string line;
    for (int i = 0; getline(is, line, NEWLINE); i++){
        if (line.size() > 0 && line[0] != COMMENT && !isallspace(line)){
            try{
                parse_line(line, model, xtag);
            }
            catch(exception &e) {
                throw ParsingException(filename, num2str(i), e.what());
            }
        }
    }
}


void parse_model(string &filename, Model *model, string xtag){
    parse_line_file(filename, model, &parse_model_line, xtag);
    set_sto_sig(model);//w/signalling
}
void parse_model_line(string &line, Model *model, string xtag){
    istringstream is(line, ios::in);
    string content, comment;
    getline(is, content, COMMENT);
    getline(is, comment);       
    if (!isallspace(content)){
        int len;
        strvec toks;
        split(&content, &toks, DELIM);
        if ((len = toks.size()) < MIN_TOK_LEN) throw runtime_error(line);
        Reaction *reaction = model->NewReaction(toks[0]);
        reaction->set_bounds(str2double(toks[2]), str2double(toks[3]));
        parse_equation(toks[1], model, reaction, xtag);
        //reaction->set_bounds(str2double(toks[2]), str2double(toks[3]));
        string rule = (len > MIN_TOK_LEN) ? toks[MIN_TOK_LEN] : string("");
        parse_rule(rule, model, reaction);

        double deltaG0min = (len > MIN_TOK_LEN + 1) ? str2double(toks[MIN_TOK_LEN + 1]) : LB;
        double deltaG0max = (len > MIN_TOK_LEN + 2) ? str2double(toks[MIN_TOK_LEN + 2]) : UB;
        reaction->set_deltaG0_range(deltaG0min, deltaG0max);

        reaction->set_comment(comment);
    }
}
void parse_equation(string &equation, Model *model, Reaction *reaction, string xtag){
    reaction->set_equation(equation);
    stomap *sto = reaction->get_sto();
    if (equation.find(" & ")!= string::npos || equation.find(" ~ ")!= string::npos){ //w/if there is activator/inhibitor in equ
        string Rn=reaction->get_name();
        double ub=reaction->get_ub();
        parse_eq_s(equation, sto, model, Rn, ub);
    }else parse_eq(equation, sto);//w/
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){ //add mets and mets_sto
        string metname = it->first;
        double coef = it->second;
        Element* metabolite = model->find_element(metname);
        if (metabolite == NULL){
            bool external = contains_ext_tag(metname, xtag);
            metabolite = model->NewMetabolite(metname, external);
        }
        metabolite->add_sto(reaction->get_name(), coef);
    }
}
void parse_rule(string &rule, Model *model, Reaction *reaction){
    strvec toks;
    split(&rule, &toks, SPACE);
    reaction->set_rules(toks);//w/ get all rule statement
    strvec *genes = reaction->get_genes();
    for (strvec::iterator it = toks.begin(); it != toks.end(); it ++){
        if (!is_operator(*it))
            genes->push_back(*it);
    }
    for (int i = 0; i < genes->size(); i++){
        string genename = (*genes)[i];
        Element* gene = model->find_element(genename);
        if (gene == NULL)
            gene = model->NewGene(genename);
        gene->add_sto(reaction->get_name(), 1);
    }

    strvec *ruletoks = new strvec;
    dijkstra(toks, *ruletoks);
    reaction->set_rule(ruletoks);
}


void parse_bfile(string &filename, Model *model){
    parse_line_file(filename, model, &parse_bounds);
}
void parse_bounds(string &line, Model *model, string xtag){
    strvec toks;
    split(&line, &toks, DELIM);
    int ntoks = toks.size();
    if (ntoks < 1) throw runtime_error(line);
    string reacname = toks[0];
    double lb = (ntoks > 1) ? str2double(toks[1]) : LB;
    double ub = (ntoks > 2) ? str2double(toks[2]) : lb;
    Reaction* reaction = model->get_reac(reacname);
    if (reaction == NULL) throw runtime_error(reacname + ": no such column!");
    reaction->set_bounds(lb, ub);
}


void parse_eq(string &eq, stomap *sto){
    strvec left_right;
    split(&eq, &left_right, EQUAL);
    if (left_right.size() < 2) throw exception();
    parse_sto(left_right[0], sto, -1);
    parse_sto(left_right[1], sto);
}
void parse_sto(string &side, stomap *sto, int sign){
    strvec toks;
    split(&side, &toks, PLUS);
    for (int i = 0; i < toks.size(); i++){
        strvec coef_met;
        string scoef = "1", met = "";
        split(&toks[i], &coef_met, SPACE);
        if (coef_met.size() > 0){
            met = coef_met[0];
            if (coef_met.size() > 1){
                scoef = met;
                met = coef_met[1];
            }
        }
        double coef = str2double(scoef) * sign;
        set_sto(met, coef, sto);
    }
}

void parse_eq_s(string &eq, stomap *sto, Model *model, string Rn, double ub){//w/for signalling net
    strvec left_right;
    split(&eq, &left_right, EQUAL);
    if (left_right.size() < 2) throw exception();
    parse_sto_s(model, Rn, ub, left_right[0], sto, -1);
    parse_sto_s(model, Rn, ub, left_right[1], sto);
}
void parse_sto_s(Model *model, string Rn, double ub, string &side, stomap *sto, int sign){//w/for signalling net
    strvec toks;
    split(&side, &toks, PLUS);
    for (int i = 0; i < toks.size(); i++){
        strvec coef_met;
        string scoef = "1";
        string mid = "";
        split(&toks[i], &coef_met, SPACE);
        if(coef_met[0]=="&"){
            mid=coef_met[1];
            model->rxnsig[Rn].activators.push_back(mid);
            }
        else if(coef_met[0]=="~"){
            double coef=ub; //inhibitor intensity (alpha)
            if(coef_met.size()==3){
                coef=str2double(coef_met[1]);
                mid=coef_met[2];
                model->rxnsig[Rn].inhibitors[mid]=coef;
            }else{
                mid=coef_met[1];
                model->rxnsig[Rn].inhibitors[mid]=1.0;
            }
            }
        else if(coef_met.size() > 0){
            mid = coef_met[0];
            if (coef_met.size() > 1){
                scoef = mid;
                mid = coef_met[1];
            }
            double coef = str2double(scoef) * sign;
            set_sto(mid, coef, sto);
        }
    }
}
void set_sto_sig(Model *model){//w/for signalling model
    for(RxnSig::iterator it=model->rxnsig.begin();it!=model->rxnsig.end();it++){
        string Rn = it->first;
        SigSet sigset = it->second;
        Reaction *rxn=model->get_reac(Rn);
        stomap *sto = rxn->get_sto();
        double ub=rxn->get_ub();
        string rowA=Rn+"__rowA";//constrain row for activator of the rxn
        string rowI1=Rn+"__rowI1";//constrain row for inhibitor of the rxn
        string rowI2=Rn+"__rowI2";//constrain row2 for inhibitor of the rxn
        strvec acts = sigset.activators;
        stomap inhs = sigset.inhibitors;
        for (int i = 0; i < acts.size(); i++){
            string mid = acts[i];
            string col=mid+"__col";
            if(!model->ifexist_element(col)){
                model->NewReaction(col);
                Reaction *rxn=model->get_reac(col);
                rxn->set_bounds(0, UB);
                stomap *sto_a = rxn->get_sto();
                (*sto_a)[mid]=-1;
                (*sto_a)[rowA]=-1;
            }else{
                Reaction *rxn=model->get_reac(col);
                stomap *sto_a = rxn->get_sto();
                (*sto_a)[rowA]=-1;
            }
            (*sto)[rowA]=1;
            if(!model->ifexist_element(rowA)){
                Element* met = model->NewMetabolite(rowA,0);
                met->set_bounds(LB, 0);//set constraint bounds
            }
        }
        for(stomap::iterator it=inhs.begin();it!=inhs.end();it++){
                string mid = it->first;
                double coef = it->second;

                Metabolite* met=model->get_met(mid); //set all rxns (produce met) for constrains of rowI1, rowI2
                stomap* stoM=met->get_sto();
                for(stomap::iterator it=stoM->begin();it!=stoM->end();it++){
                    string Rn=it->first;
                    double Rc = it->second;
                    Reaction* rxn=model->get_reac(Rn);
                    stomap *stoR = rxn->get_sto();
                    double lb = rxn->get_lb();
                    double ub = rxn->get_ub();
                    if(lb<0 && ub<=0 && Rc<0){ //reverse rxn, Rc<0, produce met
                        (*stoR)[rowI1]=coef*abs(Rc);
                        (*stoR)[rowI2]=coef*abs(Rc);
                    }else if(lb>=0 && ub>0 && Rc>0){//forward rxn, Rc>0, produce met
                        (*stoR)[rowI1]=coef*Rc;
                        (*stoR)[rowI2]=coef*Rc;
                    }
                }
                (*sto)[rowI1]=1;
                if(!model->ifexist_element(rowI1)){
                    Element* met1 = model->NewMetabolite(rowI1,0);
                    met1->set_bounds(0, ub);//set constraint bounds
                }
                if(!model->ifexist_element(rowI2)){
                    Element* met2 = model->NewMetabolite(rowI2,0);
                    met2->set_bounds(0, ub);//set constraint bounds
                }
            }
    }
}

bool contains_ext_tag(string &name, string xtag){
    int xsize = xtag.size();
    if (xtag[xsize - 1] == '_' && name.find(xtag) == 0)
        return true;
    else if (xtag[0] == '_') {
        int d = name.size() - xtag.size();
        if (d >= 0 && name.rfind(xtag) == d) //w/The position of the last occurrence in the string of the searched content.
            return true;
    }
    return false;
}

//    if (name.find(EXT_TAG_2) == 0)
//        return true;
//    if (name.find(EXT_TAG_3) == 0)
//        return true;
//
//    int len = name.size();
//
//    int d = len - strlen(EXT_TAG_0);
//    if (d >= 0 && name.find(EXT_TAG_0) == d)
//        return true;
//    d = len - strlen(EXT_TAG_1);
//    if (d >= 0 && name.find(EXT_TAG_1) == d)
//        return true;