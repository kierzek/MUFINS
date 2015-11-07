/* 
 * File:   Cache.h
 * Author: Jacek Sroka
 */

#ifndef _CACHE_H
#define	_CACHE_H

class Cache {
private:
    unsigned long int actSize;
    unsigned long int sizeTreshold;
    std::map<std::string, double> map;
    
    void clearIfTooBig();
public:
    Cache(unsigned long int);
    Cache();
    virtual ~Cache();

    bool isPresent(std::string);
    double getValue(std::string);
    void setValue(std::string, double);
};

#endif	/* _DRIVER_H */
