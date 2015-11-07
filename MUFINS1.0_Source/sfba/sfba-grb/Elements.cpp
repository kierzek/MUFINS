#include "Elements.h"

Element::Element(string name){
    set_name(name);
    lb = LB;
    ub = UB;
    Sto = new stomap;
    kind=CONT; //w/ default is continual col variable
}
Element::Element(Element* other){
    set_name(other->get_name());
    set_bounds(other->get_lb(), other->get_ub());
    Sto = new stomap(*(other->get_sto()));
}

Element::~Element(){
    delete Sto;
}

string Element::get_name() const{
    return (Name);
}

stomap* Element::get_sto() const{
    return (Sto);
}


double Element::get_lb() const{
    return lb;
}

double Element::get_ub() const{
    return ub;
}

string Element::get_comment() const{
    return Comment;
}


void Element::set_name(string name){
    Name = name;
}

void Element::add_sto(string name, double coef){
    set_sto(name, coef, Sto);
}

void Element::del_sto(string name){
    stomap::iterator it = Sto->find(name);
    Sto->erase(it);
}
void Element::set_bounds(double lb, double ub){
    this->lb = lb;
    this->ub = ub;
}

void Element::print(ostream* out) const{
    *out << get_name() << endl;
    *out << TAB << "stoichiometry: " << *Sto << endl;
}

void Element::operator *=(double coef){
    for (stomap::iterator it = Sto->begin(); it != Sto->end(); ++it)
        it->second *= coef;
}

ostream &operator << (ostream &out, const Element &element){
    element.print(&out);
    return out;
}

void Element::set_kind(int KIND){//w/
    kind = KIND;
}

int Element::get_kind(){//w/
    return kind;
}

////////////////////////////////////////////////////////////
//Reaction
Reaction::Reaction(string name):Element(name){
    Genes = new strvec();
    Rule = new strvec();    
}
Reaction::Reaction(Reaction* other):Element(other){
    Genes = new strvec(*(other->get_genes()));
    Rule = new strvec(*(other->get_rule()));
    set_comment(other->get_comment());
    set_equation(other->get_equation());
}
Reaction::~Reaction(){
    delete Genes;
    delete Rule;
}
strvec* Reaction::get_genes() const{
    return (Genes);
}
void Reaction::set_equation(string equation){
    Equation = equation;
}
void Reaction::set_comment(string comment){
    Comment = comment;
}
void Reaction::set_rule(strvec *rule){
    delete Rule;
    Rule = rule;
}

void Reaction::set_rules(strvec Rules){ //w/ get all rule statement
    //delete rules;
	rules.clear();
    rules = Rules;
}

void Reaction::set_deltaG0_range(double lb, double ub){
    DeltaG0Range.min = lb;
    DeltaG0Range.max = ub;
}

void Reaction::print(ostream* out) const{
    Element::print(out);
    (*out) <<  "\tequation:\t" << Equation  << endl;
    (*out) <<  "\tlower bound:\t" << lb << endl;
    (*out) <<  "\tupper bound:\t" << ub << endl;
    (*out) <<  "\trule:\t" << *Rule << endl;
    (*out) <<  "\tcomment:\t" << Comment << endl;
    (*out) <<  "\tgenes/enzymes:\t" << *Genes << endl;
    (*out) <<  "\tdelta G0:\t" << DeltaG0Range << endl;
}


strvec* Reaction::get_rule() const{
    return Rule;
}

string Reaction::get_equation() const{
    return Equation;
}
RANGE Reaction::get_deltaG0_range() const{
    RANGE rv;
    rv.min = DeltaG0Range.min;
    rv.max = DeltaG0Range.max;
    return rv;
}

bool Reaction::is_blocked(strvec* genes) const{
#ifdef DEBUG
    cout << Name << ": ";
    bool rv = evaluate_ko(Rule, genes);
    cout << " = " << rv << endl;
    return !rv;
#endif
    return !evaluate_ko(Rule, genes);
}

////////////////////////////////////////////////////////////
//Metabolite
Metabolite::Metabolite(string name, bool external):Element(name){
    set_external(external);
    lb = 0;
//    lnConc = 0;
}
Metabolite::Metabolite(Metabolite* other):Element(other){
    set_external(other->is_external());
//   set_conc(other->get_conc());
}

void Metabolite::print(ostream* out) const{
    Element::print(out);
    (*out) << "\texternal:\t" << is_external() << endl;
//    (*out) << "\tconcentration:\t" << get_conc() << endl;
}

bool Metabolite::is_external() const{
    return External;
}
//double Metabolite::get_lnconc() const{
//    return lnConc;
//}
//double Metabolite::get_conc() const{
//    return exp(lnConc);
//}
void Metabolite::set_external(bool external){
    External = external;
}
//void Metabolite::set_conc(double val){
//    lnConc = log(val);
//}