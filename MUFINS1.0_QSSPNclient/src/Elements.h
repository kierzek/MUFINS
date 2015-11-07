/* 
 * File:   Element.h
 * Author: t00383
 *
 * Created on 20 January 2010, 10:53
 */

#ifndef _ELEMENT_H
#define	_ELEMENT_H

//#include <cmath>

#include "Text.h"
#include "Round.h"
#include "RPN.h"


class Element: public stomap {
protected:
    string Name;
    stomap *Sto;
    double lb, ub;
    string Comment;
    
public:
    Element(string name);
    Element(Element *other);
    virtual ~Element();

    virtual string get_name() const;
    virtual void print(ostream* out = &cout) const;
    virtual string get_info()const{return Comment;};
    virtual string get_comment()const;

    virtual stomap* get_sto() const;
    virtual double get_lb()const;
    virtual double get_ub()const;
    
    virtual void set_name(string name);
    virtual void add_sto(string name, double coef);
    virtual void del_sto(string name);
    virtual void set_bounds(double lb, double ub);

    virtual void operator *= (double coef);
};

ostream &operator << (ostream &out, const Element &element);

class Reaction: public Element {
protected:
    string Equation;
    strvec *Genes;
    strvec *Rule;
    RANGE DeltaG0Range;

public:
    Reaction(string name);
    Reaction(Reaction *other);
    virtual ~Reaction();

    virtual strvec* get_genes() const;
    virtual strvec* get_rule()const;
    virtual string get_equation()const;
    virtual RANGE get_deltaG0_range()const;
    virtual void print(ostream* out = &cout) const;
//    virtual string get_info()const{return get_equation();}

    virtual void set_comment(string comment);
    virtual void set_equation(string comment);
    virtual void set_rule(strvec *rule);
    virtual void set_deltaG0_range(double lb, double ub);

    virtual bool is_blocked(strvec* genes) const;
};


class Metabolite: public Element {
protected:
    bool External;
//    double lnConc;
public:
    Metabolite(string name, bool external);
    Metabolite(Metabolite *other);

    virtual void print(ostream* out = &cout) const;
//    virtual string get_info()const{return Name;};
    virtual bool is_external() const;
//    virtual double get_lnconc() const;
//    virtual double get_conc() const;
    virtual void set_external(bool on);
 //   virtual void set_conc(double val);
};

typedef Element Gene;

/*
class Gene: public Element {
protected:
    bool

};*/

#endif	/* _ELEMENT_H */

