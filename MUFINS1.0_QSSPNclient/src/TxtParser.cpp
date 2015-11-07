#include "TxtParser.h"




void readfile(string& filename, string* s){
    char c;
    ifstream in(filename.data(), ios::in);
    if (!in) throw runtime_error(filename + ": file could not be opened!");
    while ((c = in.get()) != EOF)
        (*s) += c;
}
    

void readlines(string& filename, strvec* vec){
    string* str = new string;
    readfile(filename, str);
    split(str, vec, NEWLINE);
    delete str;
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
        parse_equation(toks[1], model, reaction, xtag);
        reaction->set_bounds(str2double(toks[2]), str2double(toks[3]));
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
    parse_eq(equation, sto);
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){
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


bool contains_ext_tag(string &name, string xtag){
    int xsize = xtag.size();
    if (xtag[xsize - 1] == '_' && name.find(xtag) == 0)
        return true;
    else if (xtag[0] == '_') {
        int d = name.size() - xtag.size();
        if (d >= 0 && name.find(xtag) == d)
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