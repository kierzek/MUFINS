/*
 *  Trajectory.h
 *  pnfba_xcode
 *
 *  Created by Andrzej Kierzek on 06/01/2011.
 *  Copyright 2011 Andrzej Kierzek. All rights reserved.
 *
 */

#include "Driver.h"

class State {
private:
	double *state;

public:
	State(int dim) {
		state = new double[dim];
	}
	void setState(int i,double st) {
		state[i] = st;	
	}
	double getState(int i) {
	    return state[i];
	}
	
};

class Trajectory {
private:	
	map<string,vector<double> > fluxmap;
	//map<string,vector<int> > statemap;
    map<string,vector<double> > statemap;
	map<string,vector<double> > amountmap;
	vector<double> t;
	vector<int> iteration;
	vector<int> gsmnIteration;
	vector<double> gsmnTime;
	vector<string> events;
	
public:
	Trajectory() {
	}
	void pushFlux(string nm,double f) {
	   	(fluxmap[nm]).push_back(f);
	}
	void pushState(string nm,double s) {
	    (statemap[nm]).push_back(s);	
	}
	void pushAmount(string nm, double a) {
	    (amountmap[nm]).push_back(a);	
	}
	void pushTime(double tm) {
		t.push_back(tm);
	}
	void pushIteration(int it) {
	    iteration.push_back(it);	
	}
	void pushGsmnTime(double tm) {
		gsmnTime.push_back(tm);
	}
	void pushGsmnIteration(double it) {
	    gsmnIteration.push_back(it);	
	}
	void pushEvent(string e) {
		events.push_back(e);
	}
	void printTableWithTime();
	void printTableWithIteration();
	void printTableWithIteration(string of);
	void printStateTableWithIteration();
	void printEventTableWithIteration();
	void printEventTableWithIteration(string of,int tn);
	void printEventTableWithTime(string of,int tn);
	void printEventAmountTableWithTime(string of,int tn);
	void printGsmnTableWithIteration();
};