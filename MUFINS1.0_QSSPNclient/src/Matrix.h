/* 
 * File:   Matrix.h
 * Author: t00383
 *
 * Created on 14 March 2010, 14:52
 */

#ifndef _MATRIX_H
#define	_MATRIX_H

#include "Elements.h"


typedef map<string, Element*> elmap;


class Matrix:public MATRIX{
public:
//    strvec rows;
//    strvec cols;
    elmap *map;
    
    ~Matrix();
    void setup(const strvec* rows, const strvec* cols, elmap* map);

    Element* operator [](string name)const {return map->find(name)->second;}

    void involved_with(strvec &result, string name)const;
    void involved_with(strvec &result, strvec names)const;

    void achievable_from(strvec &nodes, strvec from)const;
    void connected_component(MATRIX* component, string name);
    void connected_components(mtxvec &result);
    

    bool is_row(string name)const;
    bool is_col(string name)const;
    bool is_isolate(string name)const;
    bool is_orphan(string name)const;

    void select(strvec &result, strvec &names, bool (Matrix::*fun)(string name)const, strvec &ignore)const;
    void select(strvec &result, strvec &names, bool (Matrix::*fun)(string name)const)const;
    void get_isolates(strvec &result, strvec &names)const;
    void get_orphans(strvec &result, strvec &names, strvec &ignore)const;
    void select_rows_and_cols(strvec &rows, strvec &cols, strvec &names)const;


    void del(string name);
    void del_cols(strvec &names);
    void del_rows(strvec &names);


    void trim(strvec &ignore);
};





#endif	/* _MATRIX_H */

