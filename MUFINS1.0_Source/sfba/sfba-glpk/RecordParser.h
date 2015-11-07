/* 
 * File:   RecordParser.h
 * Author: ag0022
 *
 * Created on 01 October 2010, 11:50
 */

#ifndef RECORDPARSER_H
#define	RECORDPARSER_H

#include "Text.h"
#include "TxtParser.h"
#include "Driver.h"


#define OBJTAG      '!'
#define ARGTAG      '?'
#define EXTTAG      '\\'//externalise
#define INTTAG      '/' //internalise
#define CTAG        '[' //'$'
#define DGTAG       '>'
#define DG0TAG      '@'
#define TTAG        '*'
#define PLUSTAG     '+'
#define MINUSTAG    '^'
#define ALLTAG      '&'
#define VMAXTAG     '~'
#define DOLTAG     '$'//w/gni/

bool read_record(Driver* driver, string record);
bool read_record_gfa(Driver* driver, string record);//w/gfa/
bool read_record_gni(Driver* driver, Model* model, string record);//w/gni/
bool read_record_dpa(Driver* driver, string record);//w/dpa/
void read_constraint(Driver* driver, string line, char tag = 0);
void read_constraint_mod(Driver* driver, string line, char tag = 0);//w/
void read_sto(Driver* driver, string tok, double lb, double ub);
void read_sto_mod(Driver* driver, string tok, double lb, double ub);//w/
void read_objective(Driver* driver, string line);



#endif	/* RECORDPARSER_H */

