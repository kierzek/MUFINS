#include <map>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>

#include "Cache.h"

///////////////////////////////////////////////////////////////////////////////////////////////

Cache::Cache(unsigned long int tr) {
    actSize = 0;
    sizeTreshold = tr;
}

Cache::Cache() {
    actSize = 0;
    sizeTreshold = 10000000;
}

Cache::~Cache() {
    //delete map;
}

void Cache::clearIfTooBig() {
    //remove every third element
    int dummy;
    if (actSize > sizeTreshold) {
    std::cout << "clear at=" << actSize << " treshold=" << sizeTreshold << " size=" << map.size() << "\n";
        for (std::map<std::string, double>::iterator it = map.begin(); it != map.end(); ++it) {
            dummy = rand() % 3;
            if (dummy == 0) {
                actSize -= it->first.size();
                map.erase(it->first);
            }
        }
    std::cout << "after at=" << actSize << " treshold=" << sizeTreshold << " size=" << map.size() << "\n";
    }
}

bool Cache::isPresent(std::string s) {
    return (map.count(s) == 1);
}

double Cache::getValue(std::string s) {
    return map[s];
}

void Cache::setValue(std::string s, double d) {
    //if (isPresent(s)) {
    //    map[s] = d;
    //} else {
        actSize += s.size();
        clearIfTooBig();
        map[s] = d;
    //}
}
