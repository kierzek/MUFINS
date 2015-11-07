/*
 *  Trajectory.cpp
 *  pnfba_xcode
 *
 *  Created by Andrzej Kierzek on 06/01/2011.
 *  Copyright 2011 Andrzej Kierzek. All rights reserved.
 *
 */

#include "Trajectory.h"

void Trajectory::printTableWithTime() {
	map<string, vector<double> >::iterator it;
	map<string, vector<double> >::iterator it1;
	strvec headers;
	strvec headers1;
	for(it = fluxmap.begin(); it!=fluxmap.end(); it++) 
		headers.push_back((*it).first);
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	cout << "Time";
	for(int i=0;i<headers.size();i++) cout << "\t" << headers[i];
	for(int i=0;i<headers1.size();i++) cout << "\t" << headers1[i];
	cout << endl;
	for(int i=0;i<t.size();i++) {
		cout << t[i];
		for(int j=0;j<headers.size();j++) cout << "\t" << (fluxmap[headers[j]])[i];
		for(int j=0;j<headers1.size();j++) cout << "\t" << (statemap[headers1[j]])[i];
		cout << endl;
	}
}

void Trajectory::printTableWithIteration() {
	map<string, vector<double> >::iterator it;
	map<string, vector<double> >::iterator it1;
	strvec headers;
	strvec headers1;
	for(it = fluxmap.begin(); it!=fluxmap.end(); it++) 
		headers.push_back((*it).first);
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	cout << "Time";
	for(int i=0;i<headers.size();i++) cout << "\t" << headers[i];
	for(int i=0;i<headers1.size();i++) cout << "\t" << headers1[i];
	cout << endl;
	for(int i=0;i<iteration.size();i++) {
		cout << iteration[i];
		for(int j=0;j<headers.size();j++) cout << "\t" << (fluxmap[headers[j]])[i];
		for(int j=0;j<headers1.size();j++) cout << "\t" << (statemap[headers1[j]])[i];
		cout << endl;
	}

}

void Trajectory::printTableWithIteration(string of) {
	ofstream outfile;
	outfile.open(of.c_str(),ofstream::out);
	map<string, vector<double> >::iterator it;
	map<string, vector<double> >::iterator it1;
	strvec headers;
	strvec headers1;
	for(it = fluxmap.begin(); it!=fluxmap.end(); it++) 
		headers.push_back((*it).first);
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	outfile << "Time";
	for(int i=0;i<headers.size();i++) outfile << "\t" << headers[i];
	for(int i=0;i<headers1.size();i++) outfile << "\t" << headers1[i];
	outfile << endl;
	for(int i=0;i<iteration.size();i++) {
		outfile << iteration[i];
		for(int j=0;j<headers.size();j++) outfile << "\t" << (fluxmap[headers[j]])[i];
		for(int j=0;j<headers1.size();j++) outfile << "\t" << (statemap[headers1[j]])[i];
		outfile << endl;
	}
	outfile.close();
	
}

void Trajectory::printStateTableWithIteration() {
	map<string, vector<double> >::iterator it1;
	strvec headers1;
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	cout << "Time";
	for(int i=0;i<headers1.size();i++) cout << "\t" << headers1[i];
	cout << endl;
	for(int i=0;i<iteration.size();i++) {
		cout << iteration[i];
		for(int j=0;j<headers1.size();j++) cout << "\t" << (statemap[headers1[j]])[i];
		cout << endl;
	}	
	
}

void Trajectory::printEventTableWithIteration() {
	map<string, vector<double> >::iterator it1;
	strvec headers1;
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	cout << "Time\tEvent";
	for(int i=0;i<headers1.size();i++) cout << "\t" << headers1[i];
	cout << endl;
	for(int i=0;i<iteration.size();i++) {
		cout << iteration[i];
		cout << "\t";
		cout << events[i];
		for(int j=0;j<headers1.size();j++) cout << "\t" << (statemap[headers1[j]])[i];
		cout << endl;
	}	
}

void Trajectory::printEventTableWithIteration(string of, int tn) {
	ofstream outfile;
	outfile.open(of.c_str(),ofstream::app);
	map<string, vector<double> >::iterator it1;
	strvec headers1;
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	outfile << "Trajectory\tTime\tEvent";
	for(int i=0;i<headers1.size();i++) outfile << "\t" << headers1[i];
	outfile << endl;
	for(int i=0;i<iteration.size();i++) {
		outfile << tn << "\t";
		outfile << iteration[i];
		outfile << "\t";
		outfile << events[i];
		for(int j=0;j<headers1.size();j++) outfile << "\t" << (statemap[headers1[j]])[i];
		outfile << endl;
	}	
	outfile.close();
}

void Trajectory::printEventTableWithTime(string of, int tn) {
	ofstream outfile;
	outfile.open(of.c_str(),ofstream::app);
	map<string, vector<double> >::iterator it1;
	strvec headers1;
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	outfile << "Trajectory\tTime\tEvent";
	for(int i=0;i<headers1.size();i++) outfile << "\t" << headers1[i];
	outfile << endl;
	for(int i=0;i<t.size();i++) {
		outfile << tn << "\t";
		outfile << t[i];
		outfile << "\t";
		outfile << events[i];
		for(int j=0;j<headers1.size();j++) outfile << "\t" << (statemap[headers1[j]])[i];
		outfile << endl;
	}	
	outfile.close();
}

void Trajectory::printEventAmountTableWithTime(string of, int tn) {
	ofstream outfile;
	outfile.open(of.c_str(),ofstream::app);
	map<string, vector<double> >::iterator it1;
	map<string, vector<double> >::iterator it2;
	strvec headers1, headers2;
	for(it1 = statemap.begin(); it1!=statemap.end(); it1++) 
		headers1.push_back((*it1).first);
	for(it2 = amountmap.begin(); it2!=amountmap.end(); it2++) 
		headers2.push_back((*it2).first);
	outfile << "Trajectory\tTime\tEvent";
	for(int i=0;i<headers2.size();i++) outfile << "\t" << headers2[i];
	for(int i=0;i<headers1.size();i++) outfile << "\t" << headers1[i];
	outfile << endl;
	for(int i=0;i<t.size();i++) {
		outfile << tn << "\t";
		outfile << t[i];
		outfile << "\t";
		outfile << events[i];
		for(int j=0;j<headers2.size();j++) outfile << "\t" << (amountmap[headers2[j]])[i];
		for(int j=0;j<headers1.size();j++) outfile << "\t" << (statemap[headers1[j]])[i];
		outfile << endl;
	}	
	outfile.close();
}

void Trajectory::printGsmnTableWithIteration() {
	map<string, vector<double> >::iterator it;
	strvec headers;
	for(it = fluxmap.begin(); it!=fluxmap.end(); it++) 
		headers.push_back((*it).first);
	cout << "Time";
	for(int i=0;i<headers.size();i++) cout << "\t" << headers[i];
	cout << endl;
	for(int i=0;i<gsmnIteration.size();i++) {
		cout << gsmnIteration[i];
		for(int j=0;j<headers.size();j++) cout << "\t" << (fluxmap[headers[j]])[i];
		cout << endl;
	}
}
