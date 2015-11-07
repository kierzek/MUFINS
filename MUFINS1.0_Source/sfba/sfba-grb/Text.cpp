#include "Text.h"
#include "Round.h"

void  split(string* str, strvec* vec, char delim){ //ignore the repeated delims
    istringstream is(*str);
    split(is, vec, delim);
}
void split(istream &is, strvec* vec, char delim){ //ignore the repeated delims
    string field;
    while (getline(is, field, delim))
        if (!field.empty())
            vec->push_back(field);
}

void  split2(string* str, strvec* vec, char delim){ //w/not ignore the repeated delims
    istringstream is(*str);
    split2(is, vec, delim);
}
void split2(istream &is, strvec* vec, char delim){ //w/not ignore the repeated delims
    string field;
    while (getline(is, field, delim))     
        vec->push_back(field);
}

string num2str(double num){
    stringstream ss;
    ss << num;
    return ss.str();
}

double str2double(string s){
    if (s == "INF")
        return MAX_NUM;
    if (s == "-INF")
        return -MAX_NUM;
    return atof(s.data());
}

double str2int(string s){//w/
    if (s == "INF")
        return MAX_NUM;
    if (s == "-INF")
        return -MAX_NUM;
    return atoi(s.data());
}



bool isallspace(string s){
    bool rv = true;
    for (int i = 0; rv && i < s.size(); ++i)
        rv = isspace(s[i]);
    return rv;
}

ostream &operator << (ostream &out, const stomap &sto){
    if (sto.size() > 0){
        strvec items;
        for (stomap::const_iterator it = sto.begin(); it != sto.end(); ++it){
            double coef = it->second;
            string item = (coef == 1) ? "" : num2str(coef) + SPACE;
            item += it->first;
            items.push_back(item);
        }
        int i;
        for (i = 0; i < items.size() - 1; i++)
            out << items[i] << SPACE << PLUS << SPACE;
        out << items[i];
    }
    return out;
}

ostream &operator << (ostream &out, const strvec &vec){
    std::copy(vec.begin(), vec.end(), ostrit(out, LISTDELIM));
    return out;
}

ostream &operator << (ostream &out, const POINT &point){
    std::copy(point.begin(), point.end(), ostream_iterator<double>(out, TAB1));
    return out;    
}

ostream &operator << (ostream &out, const pointvec &pv){
    for (pointvec::const_iterator it = pv.begin(); it != pv.end(); ++it)
        out << *it << endl;
    return out;    
}


/*
stomap operator * (stomap &sto, double coef){
    stomap rv;
    for (stomap::iterator it = sto.begin(); it != sto.end(); ++it)
        rv[it->first] = it->second * coef;
    return rv;
}
*/
void mul(stomap *sto, double coef){
    for (stomap::iterator it = sto->begin(); it != sto->end(); ++it)
        it->second *= coef;
}


void set_sto(string name, double coef, stomap* sto){
    stomap::iterator it = sto->find(name);
    if (it == sto->end())
        (*sto)[name] = 0;
    (*sto)[name] += coef;
}

ostream &operator << (ostream &out, const RANGE &range){
    out << range.min << '\t' << range.max;
    return out;
}


bool is_operator(string s){
    return (s == AND) || (s == OR) || (s == OPEN_PAR) || (s == CLOS_PAR);
}

