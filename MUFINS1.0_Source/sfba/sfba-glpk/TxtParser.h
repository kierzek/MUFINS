/* 
 * File:   TxtParser.h
 * Author: Albert Gevorgyan
 *
 * Created on 20 January 2010, 10:52
 */

#ifndef _TXTPARSER_H
#define	_TXTPARSER_H

#include <fstream>

#include "Model.h"


#define TOK_LEN     5
#define MIN_TOK_LEN 4

#define EXT_TAG_0 "_xt"
#define EXT_TAG_1 "_ext"
#define EXT_TAG_2 "X_"
#define EXT_TAG_3 "x_"


typedef struct{
    stomap target;
    strvec query;
}TARGET_QUERY;

typedef void (*_parse_line)(string&, Model*, string);

class ParsingException: public runtime_error{
public:
    ParsingException(string filename, string linenum, string msg = "(no message)")
            : runtime_error(string("cannot parse ") + filename + " line " + linenum + ": " + msg + "."){}

};

//reading a file
bool fexists(const string filename);//w/
void readfile(string &filename, string* s);
void readlines(string &filename, strvec* vec);

//abstract text parser
void parse_line_file(string &filename, Model* model, _parse_line parse_line, string xtag = "");

//parsing a text(csv) model file
void parse_model(string &filename, Model* model, string xtag);
void parse_model_line(string &line, Model*, string xtag);
void parse_equation(string &eq, Model* model, Reaction* reaction, string xtag);
void parse_rule(string &rule, Model* model, Reaction* reaction);

//parsing a bounds file
void parse_bfile(string &filename, Model* model);
void parse_bounds(string &bounds, Model* model, string xtag = "");

//convert an equation string into a stoichiometry
void parse_eq(string &eq, stomap* sto);
void parse_sto(string &side, stomap* sto, int coef = 1);
void parse_eq_s(string &eq, stomap *sto, Model *model, string Rn, double ub);
void parse_sto_s(Model *model, string Rn, double ub, string &side, stomap *sto, int coef = 1);
void set_sto_sig(Model *model);//w/for signalling model


bool contains_ext_tag(string &name, string xtag);

void parse_dpaplot(string &filename, strvec &mets, met_genes &metG);//w/dpa/
void parse_arrayExpression(string &filename, strvec &arrName, strstr2int &arrexp);//w/gfa/
void parse_arrayExpression_f(string &filename, strvec &arrName, strstr2flo &arrexp);//w/dpa/


#endif	/* _TXTPARSER_H */

