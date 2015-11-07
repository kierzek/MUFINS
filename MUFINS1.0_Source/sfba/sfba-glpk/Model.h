/* 
 * File:   Model.h
 * Author: Albert Gevorgyan
 *
 * Created on 20 January 2010, 10:53
 */

#ifndef _MODEL_H
#define	_MODEL_H

#include "Matrix.h"


class Model: public elmap{
protected:
    strvec reactions, metabolites, genes;
    
public:
    ~Model();
    RxnSig rxnsig;
    
    //void print(ostream* out = &cout)const;
    string get_comment(string name)const{return find_element(name)->get_name();};
    void print_sto(stomap* sto, ostream* out = &cout, bool withcomments = false, const char* indent = EMPTYSTR)const;
    void print_conv(stomap* sto, ostream* out = &cout, const char* indent = EMPTYSTR)const;
    void print_vec(strvec& vec, ostream* out = &cout, bool withcomments = false, const char* indent = EMPTYSTR)const;

    Element* find_element(string name)const;
    Metabolite *get_met(string name)const;
    Reaction *get_reac(string name)const;
    Gene *get_gene(string name)const;
    void del_element(string name);//w/
    bool ifexist_element(string name);//w/

    Reaction* NewReaction(string name);
    Metabolite* NewMetabolite(string name, bool external);
    Gene* NewGene(string name);

    void get_externals(strvec&)const;
    void get_internals(strvec&)const;
    strvec get_reactions(){return reactions;}//w/
    strvec get_metabolites(){return metabolites;}//w/


    strvec reacs_to_block(strvec* els)const;
    void reaclists_to_block(strlistvec& rv, strlistvec& qlists)const;

    void get_Nx(Matrix*)const;
    void get_N(Matrix*)const;
    void get_Nt(Matrix*)const;
    strvec get_genes()const;

    friend ostream &operator << (ostream &out, const Model &model);
   
};



#endif	/* _MODEL_H */

