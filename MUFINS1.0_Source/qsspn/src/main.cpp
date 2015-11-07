/*
 * File:   main.cpp
 * Author: Andrzej Kierzek
 *
 * Created on 28 December 2014, 12:56
 */

#include <iostream>

#include "PetriNet.h"

bool isJason(string path) {
    int l = (int)path.size();
    if((char)path[l-1] == 'n')
        if((char)path[l-2] == 'o')
            if((char)path[l-3] == 's')
                 if((char)path[l-4] == 'j')
                     if((char)path[l-5] == '.')
                                      return true;
    return false;
}
int main(int argc, char** argv) {
    
    if (argc != 3) {
			cout << VERSION << endl;
			cout << "USAGE: qsspn qsspn_file ctrl_file" << endl;
            cout << "The qsspn_file file with extension .json is read as Json file" << endl;
            cout << "The qsspn file without .json extension is assumed to be in old qsspn format" << endl;
			exit(0);
		}
    
    PetriNet *pn = new PetriNet();
    string path(argv[1]);
    string path1(argv[2]);
    if(isJason(path)) {
        pn->readModel(path);
    } else {
        pn->readPetriNet(path);
    }
    pn->runWorkflow(path1);
    exit(0);
}




