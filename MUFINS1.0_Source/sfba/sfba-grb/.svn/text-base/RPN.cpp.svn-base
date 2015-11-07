#include <stack>

#include "RPN.h"

static strvec PREC;
static opmap OPMAP;
static strvec::iterator pstart, pend;

void init_operators(){
    if (OPMAP.size() > 0)return;
    OPMAP[OR] = &_or;
    PREC.push_back(OR);
    OPMAP[AND] = &_and;
    PREC.push_back(AND);
    pstart = PREC.begin();
    pend = PREC.end();    
}

inline t_val _and(t_val a, t_val b){return a && b;}
inline t_val _or(t_val a, t_val b){return a || b;}
inline t_val _translate(string tok){return atoi(tok.data());}



inline bool is_lower(string op1, string op2){
    return find(pstart, pend, op1) <= find(pstart, pend, op2);
}

inline bool is_binop(string s){
    return OPMAP.find(s) != OPMAP.end();
}

void dijkstra(strvec &in, strvec &out){
    if (in.empty())
        return;
    strstack stak;
    string op;
    for (strvec::iterator it = in.begin(); it != in.end(); ++it){
        string tok = *it;
        if (tok == OPEN_PAR)
            stak.push(tok);
        else if(tok == CLOS_PAR){
            while (!stak.empty()){
                op = stak.top();
                stak.pop();
                if (op == OPEN_PAR)break;
                else out.push_back(op);
            }
        }
        else if (is_operator(tok)){
            while (!stak.empty() && is_binop(op = stak.top()) && is_lower(tok, op)){
                stak.pop();
                out.push_back(op);
            }
            stak.push(tok);
        }
        else
            out.push_back(tok);
    }
    while (!stak.empty()){
        op = stak.top();
        out.push_back(op);
        stak.pop();
    }
}


t_val evaluate(strvec toks, t_translate translate){
    if (toks.empty())
        return 1;
    tstack stak;
    for (strvec::iterator it = toks.begin(); it != toks.end(); ++it){
        string tok = *it;
        if (is_operator(tok)){
            t_val val2 = stak.top();
            stak.pop();
            t_val val1 = stak.top();
            stak.pop();
            t_val res = OPMAP[tok](val1, val2);
            stak.push(res);
        }
        else{
            t_val value = translate(tok);
            stak.push(value);
        }
    }
    return stak.top();
}





bool evaluate_ko(strvec* rule, strvec* ko){
    strvec toks;
    for (strvec::iterator it = rule->begin(); it != rule->end(); ++it){
        string tok = *it;
        if (is_binop(tok));
        else if (std::find(ko->begin(), ko->end(), tok) != ko->end())
            tok = "0";
        else
            tok = "1";
        toks.push_back(tok);
    }
    bool rv = evaluate(toks);
    return rv;
}





////////////////////////////////////////////////////
/*
void dijkstra(string in, strvec &out){
    strvec toks = split(in, RDELIM);
    dijkstra(toks, out);
}
t_val evaluate(string s){
    strvec out;
    dijkstra(s, out);
    //cout << join(out, SPACE) << endl;
    return evaluate(out);
}
void rpn_test(){
    init_operators();
    string s = "0 AND ( 0 OR 1 ) OR ( 1 AND 0 ) OR ( ( 1 OR 0 ) AND 0 )";
    //cout << s << " = " << evaluate(s) << endl;
}
*/
