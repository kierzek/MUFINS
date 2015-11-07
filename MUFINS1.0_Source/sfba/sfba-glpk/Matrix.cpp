#include "Matrix.h"


Matrix::~Matrix(){
    delete map;
}

void Matrix::setup(const strvec* rows, const strvec* cols, elmap* map){
    this->rows.insert(this->rows.begin(), rows->begin(), rows->end());
    this->cols.insert(this->cols.begin(), cols->begin(), cols->end());
    this->map = map;
}

void Matrix::involved_with(strvec& result, string name) const{
    stomap* sto = (*this)[name]->get_sto();
    for (stomap::iterator jt = sto->begin(); jt != sto->end(); ++jt){
        string name2 = jt->first;
        if (std::find(result.begin(), result.end(), name2) == result.end())
            result.push_back(name2);
    }
}

void Matrix::involved_with(strvec& result, strvec names) const{
    for (strvec::const_iterator it = names.begin(); it != names.end(); ++it)
        involved_with(result, *it);
    
}

void Matrix::achievable_from(strvec &nodes, strvec from) const{
    int was = nodes.size();
    involved_with(nodes, from);
    if (nodes.size() == was)
        return;
    achievable_from(nodes, nodes);
}

void Matrix::connected_component(MATRIX* component, string name){
    strvec adjnodes, from;
    from.push_back(name);
    achievable_from(adjnodes, from);
    select_rows_and_cols(component->rows, component->cols, adjnodes);
}

void Matrix::connected_components(mtxvec& result){
    strvec ccols(cols);
    while (!ccols.empty()){
        MATRIX component;
        string name = ccols.at(0);
        connected_component(&component, name);
        result.push_back(component);
        for (strvec::iterator it = component.cols.begin(); it != component.cols.end(); ++it)
            std::remove(ccols.begin(), ccols.end(), *it);
        ccols.resize(ccols.size() - component.cols.size());
    }
}


bool Matrix::is_row(string name) const{
    return std::find(rows.begin(), rows.end(), name) != rows.end();
}

bool Matrix::is_col(string name) const{
    return std::find(cols.begin(), cols.end(), name) != cols.end();
}

bool Matrix::is_orphan(string name) const{
    return (*this)[name]->get_sto()->size() < 2;
}

bool Matrix::is_isolate(string name) const{
    return (*this)[name]->get_sto()->size() == 0;
}

void Matrix::select(strvec &rv, strvec &names, bool(Matrix::*fun)(string)const, strvec &ignore)const {
    if (names.empty())
        names = rows;
    elmap::iterator end = map->end();
    string name;
    strvec::iterator igbeg = ignore.begin(), igend = ignore.end();
    for (strvec::iterator it = names.begin(); it != names.end(); ++it){        
        if (std::find(igbeg, igend, *it) == igend){
            name = *it;
            if (map->find(*it) == end)
                throw runtime_error(string("No such row or column: ") + name);
            if ((*this.*fun)(name))
                rv.push_back(name);
        }
    }
}

void Matrix::select(strvec& result, strvec& names, bool(Matrix::*fun)(string)const) const{
    strvec empty;
    select(result, names, fun, empty);
}

void Matrix::get_orphans(strvec &rv, strvec &names, strvec &ignore) const{
    select(rv, names, &Matrix::is_orphan, ignore);
}

void Matrix::get_isolates(strvec &rv, strvec &names) const{
    select(rv, names, &Matrix::is_isolate);
}

void Matrix::select_rows_and_cols(strvec &rows, strvec &cols, strvec &names) const{
    select(rows, names, &Matrix::is_row);
    select(cols, names, &Matrix::is_col);
}


void Matrix::del(string name){
    elmap::iterator it = map->find(name);
    Element* el = it->second;
    stomap* sto = el->get_sto();
    for (stomap::iterator jt = sto->begin(); jt != sto->end(); ++jt){
        Element* adj = (*this)[jt->first];
        adj->del_sto(name);
    }
    map->erase(it);
}

void Matrix::del_rows(strvec &names){
    for (strvec::iterator it = names.begin(); it != names.end(); ++it){
        del(*it);
        std::remove(rows.begin(), rows.end(), *it);
    }
    rows.resize(rows.size() - names.size());
}

void Matrix::del_cols(strvec &names){
    for (strvec::iterator it = names.begin(); it != names.end(); ++it){
        del(*it);
        std::remove(cols.begin(), cols.end(), *it);
    }
    cols.resize(cols.size() - names.size());
}

void Matrix::trim(strvec &ignore){
    strvec orphans;
    get_orphans(orphans, rows, ignore);
//cout << "O" << endl << orphans << endl;
    while (!orphans.empty()){
        strvec involve_orphans;
        involved_with(involve_orphans, orphans);
//cout << "R" << endl << involve_orphans << endl;
        del_cols(involve_orphans);
        strvec isolates;
        get_isolates(isolates, rows);
//cout << "I" << endl << isolates << endl;
        del_rows(isolates);
        orphans.clear();
        get_orphans(orphans, rows, ignore);
//cout << "DONE" << endl;
//cout << "O" << endl << orphans << endl;
    }
}
