#pragma once

#include <iostream>
#include <vector>

using namespace std;


class Pheromones {
public:
    unsigned int total_methods;
    vector<float> values;
    
    Pheromones(unsigned int total_methods):total_methods(total_methods) {
        for (unsigned int i=0;i<total_methods;i++) {
            values.push_back(10);
        }
    }

    friend ostream & operator<<(ostream & os, Pheromones& p);    
};

ostream & operator<<(ostream & os, Pheromones& p) {
    os << " ";
    for (unsigned int i=0;i<p.total_methods;i++) {
        os << p.values[i] << " ";
    }

    return os;
}


