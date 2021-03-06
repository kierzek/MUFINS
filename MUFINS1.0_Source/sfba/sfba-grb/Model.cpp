#include "Model.h"

////////////////////////////////////////////////////////////
//Model
Model::~Model(){
    //cout << "Destructing" << endl;
    for(iterator i = begin(); i != end(); ++i)
        delete i->second;
}


void Model::print_sto(stomap* sto, ostream* out, bool withcomments, const char* indent) const{
    for (stomap::const_iterator it = sto->begin(); it != sto->end(); ++it){
        (*out) << indent << it->first << TAB << it->second;
        if (withcomments)
            (*out) << TAB << COMMENT << find_element(it->first)->get_info();
        (*out) << endl;
    }
}

void Model::print_conv(stomap* sto, ostream* out, const char* indent) const{
    for (stomap::const_iterator it = sto->begin(); it != sto->end(); ++it){
        double val = it->second;
        string t1 = "", t2 = "\t";
        if (val > 0){
            t1 = "\t";
            t2 = "";
        }
        (*out) << indent << it->first << TAB << t1 << val << t2 << endl;
    }
}


void Model::print_vec(strvec& vec, ostream* out, bool withcomments, const char* indent) const{
    for (strvec::iterator it = vec.begin(); it != vec.end(); ++it){
        string name = *it;
        (*out) << indent << name;
        if (name[0] == '-')
            name = name.substr(1);
        if (withcomments)
            (*out) << TAB << COMMENT << find_element(name)->get_info();
        (*out) << endl;
    }
}


Element* Model::find_element(string name) const{
    const_iterator it = find(name);
    return (it == end() ? NULL : it->second);
//    if(it == end()) throw runtime_error("Error: "+name+" -no such element in the model!");//w/
//    else it->second;
    //return dynamic_cast<Element*>(find_element(name));
}

bool Model::ifexist_element(string name){
    const_iterator it = find(name);
    return (it == end() ? false : true);
}

Metabolite *Model::get_met(string name) const{
//    return dynamic_cast<Metabolite*>(find_element(name));
    const_iterator it = find(name);//w/
    if(it == end()) throw runtime_error("cannot find this metabolite in the model: "+name);//w/
    else return dynamic_cast<Metabolite*>(it->second);
}

Reaction *Model::get_reac(string name) const{
    const_iterator it = find(name);//w/
    if(it == end()) throw runtime_error("cannot find this reaction in the model: "+name);//w/
    else return dynamic_cast<Reaction*>(it->second);
//    return dynamic_cast<Reaction*>(find_element(name));
}

Gene *Model::get_gene(string name) const{
    const_iterator it = find(name);//w/
    if(it == end()) throw runtime_error("cannot find this gene in the model: "+name);//w/
    else return dynamic_cast<Gene*>(it->second);
//    return dynamic_cast<Gene*>(find_element(name));
}

void Model::del_element(string name){//w/
    this->erase(name);
    if(std::find(reactions.begin(),reactions.end(),name)!=reactions.end()) reactions.erase(std::find(reactions.begin(),reactions.end(),name));
    else if(std::find(metabolites.begin(),metabolites.end(),name)!=metabolites.end()) metabolites.erase(std::find(metabolites.begin(),metabolites.end(),name));
    else if(std::find(genes.begin(),genes.end(),name)!=genes.end()) genes.erase(std::find(genes.begin(),genes.end(),name));
}

Reaction* Model::NewReaction(string name){
    Reaction *reaction = new Reaction(name = name);
    (*this)[name] = reaction;
    reactions.push_back(name);
    return reaction;
}

Metabolite* Model::NewMetabolite(string name, bool external = false){
    Metabolite *metabolite = new Metabolite(name = name, external = external);
    (*this)[name] = metabolite;
    metabolites.push_back(name);
    return metabolite;
}

Gene* Model::NewGene(string name){
    Gene *gene = new Gene(name = name);
    (*this)[name] = gene;
    genes.push_back(name);
    return gene;
}

void Model::get_externals(strvec& rv) const{
    for (int i = 0; i < metabolites.size(); i++){
        string m = metabolites[i];
        if (get_met(m)->is_external())
            rv.push_back(m);
    }
}

void Model::get_internals(strvec& rv) const{
    for (strvec::const_iterator it = metabolites.begin(); it != metabolites.end(); ++it){
        string m = *it;
        if (!get_met(m)->is_external())
            rv.push_back(m);
    }
}

//strvec Model::involved_with(strvec elements) const{
//    strvec rv;
//    for (strvec::iterator it = elements.begin(); it != elements.end(); ++it){
//        stomap* sto = find_element(*it)->get_sto();
//        for (stomap::iterator jt = sto->begin(); jt != sto->end(); ++jt){
//            string name2 = jt->first;
//            if (std::find(rv.begin(), rv.end(), name2) == rv.end())
//                rv.push_back(name2);
//        }
//    }
//    return rv;
//}
//
//bool Model::is_orphan(string metabolite) const{
//    return find_element(metabolite)->get_sto()->size() < 2;
//}
//
//strvec Model::get_orphans() const{
//    strvec rv;
//    strvec internals = get_internals();
//    for (strvec::iterator it = internals.begin(); it != internals.end(); ++it){
//        if (is_orphan(*it))
//            rv.push_back(*it);
//    }
//    return rv;
//}
//
////void Model::extract_core() const{
////    strvec reactions = this->reactions;
////    strvec orphans = get_orphans();
////    while (!orphans.empty()){
////        strvec involve_orphans = involved_with(orphans);
////        for (strvec::iterator it = involve_orphans.begin(); it != involve_orphans.end(); ++it){
////            //cout << *it << endl;
////            std::remove(reactions.begin(), reactions.end(), *it);
////        }
////        orphans.clear();
////    }
////
////}


strvec Model::reacs_to_block(strvec* els) const{
    strvec rv;
    strvec kogenes;
    strvec::const_iterator it;
    for (it = els->begin(); it != els->end(); it++){
        string name = *it;
        if (std::find(reactions.begin(), reactions.end(), name) != reactions.end())
                rv.push_back(name);
        else if (std::find(genes.begin(), genes.end(), name) != genes.end())
                kogenes.push_back(name);
        else
//            rv.push_back("NA");//w/name not in model
            throw runtime_error(name + " is neither a reaction nor a gene.");//w/
    }
    if (!kogenes.empty()){
        for(it = reactions.begin(); it != reactions.end(); ++it){
            string name = *it;
            Reaction* reac = get_reac(name);
            if (reac->is_blocked(&kogenes))
                rv.push_back(name);
        }
    }
    return rv;
}

void Model::reaclists_to_block(strlistvec& rv, strlistvec& qlists) const{
    for (strlistvec::iterator it = qlists.begin(); it != qlists.end(); it++){
        strvec qlist = *it;
        strvec toblock = reacs_to_block(&qlist);
        rv.push_back(toblock);
    }
}

void Model::get_Nx(Matrix* N) const{
    elmap *map = new elmap(*this);
    N->setup(&metabolites, &reactions, map);
}

void Model::get_N(Matrix* N) const{
    strvec internals;
    get_internals(internals);
    elmap *map = new elmap;

    for (strvec::const_iterator it = reactions.begin(); it != reactions.end(); ++it){
        string rname = *it;
        Reaction* reaction = get_reac(rname);
        Reaction *reaction2 = new Reaction(reaction);
        stomap* sto = reaction->get_sto();
        for (stomap::iterator it = sto->begin(); it != sto->end(); ++it){
            string mname = it->first;
            Metabolite *met = get_met(mname);
            if (met->is_external())
                reaction2->del_sto(mname);
            else
                (*map)[mname] = met;
        }
        (*map)[rname] = reaction2;
    }
    N->setup(&internals, &reactions, map);
}
/*
void Model::get_Nsplit(Matrix* N) const{
    get_N(N);
    strvec reacs = N->cols;
    for (strvec::iterator it = reacs.begin(); it != reacs.end(); ++it){
        string reacname = *it;
        Reaction* reaction = dynamic_cast<Reaction*>((*N)[reacname]);
        if (reaction->get_lb() < 0){
            string backname = reacname + BACK_TAG;
            N->cols.push_back(backname);
            Reaction* back = new Element(reaction);
            back->set_name(backname);
            *back *= -1;
            (*(N->map))[backname] = back;
        }
    }
}
*/
void Model::get_Nt(Matrix* N) const{
    elmap *map = new elmap(*this);
    N->setup(&reactions, &metabolites, map);
}

strvec Model::get_genes() const{
    return genes;
}



ostream &operator << (ostream &out, const Model &model){
    strvec::const_iterator it;
    for (it = model.reactions.begin(); it != model.reactions.end(); ++it)
        out << *model.find_element(*it) << endl;
    for (it = model.metabolites.begin(); it != model.metabolites.end(); ++it)
        out << *model.find_element(*it) << endl;
    for (it = model.genes.begin(); it != model.genes.end(); ++it)
        out << *model.find_element(*it) << endl;
    return out;
}

