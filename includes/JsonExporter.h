#pragma once

// boost
#include "boost_definitions.h"

// Macros for CGAL
#include "cgal_definitions.h"

// Standard C++
#include <iostream>
#include <vector>
#include <string>

// Namespaces
using namespace std;



class JsonExporter {
private:
    boost::property_tree::ptree pt_root;
    boost::property_tree::ptree pt_steiner_points_x;
    boost::property_tree::ptree pt_steiner_points_y;
    boost::property_tree::ptree pt_edges;
public:

    string content_type;
    string instance_uid;
    vector<string> steiner_points_x;
    vector<string> steiner_points_y;
    vector<std::pair<int, int>> edges;

    JsonExporter(string instance_uid);

    void save(const char* inputfile);
    void print() const;
};