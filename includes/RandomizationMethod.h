#pragma once

// Standard C++
#include <gmp.h>
#include <iostream>
#include <map>
#include <vector>

// Macros and headers for CGAL
#include "cgal_definitions.h"

// Macros and headers for boost
#include "boost_definitions.h"

// Macros and headers for QT
#include "qt_definitions.h"

// Configuration
#include "triangulation_configuration.h"

// Support classes
#include "JsonExporter.h"
#include "JsonLoader.h"
#include "graph_definitions.h"
#include "steiner_strategies.h"
#include "utils.hpp"
#include "LocalSearchRandomization.h"

// Namespaces
using namespace std;

template <typename T>
class RandomizationMethod {
public:
    static int tryMethod(CDT& cdt, Polygon& boundaryPolygon, JsonLoader& loader, vector<double> & pn, int steiner_points_before, int MAX_ITERATIONS) {
        vector<Point> steinerPoints;
        
        int obtuse_triangles_before = utils::countObtuseTriangles(cdt, boundaryPolygon);        

        CDT cdt_copy = cdt;
        Graph graph_copy;
        graph_copy.cdt = &cdt_copy;
        graph_copy.boundaryPolygon = &boundaryPolygon;

        for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end() && MAX_ITERATIONS > 0; ++fit) {
            Point a = fit->vertex(0)->point();
            Point b = fit->vertex(1)->point();
            Point c = fit->vertex(2)->point();

            bool result = utils::is_obtuse(a, b, c);

            if (result) {
                Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, steiner_stategies::Strategy::RANDOM);

                if (s != nullptr) {
                    if (utils::is_steiner_point_valid(boundaryPolygon, *s)) {
                        graph_copy.cdt->insertByStrategy(*s, steiner_stategies::Strategy::RANDOM);

                        MAX_ITERATIONS--;                        
                    }
                }               
            }
        }      

        LocalSearchRandomization<T> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        strategies.push_back(steiner_stategies::Strategy::CENTROID);

        steinerPoints = triangulator.triangulate(strategies, graph_copy, loader, boundaryPolygon);

        int step = steinerPoints.size() - steiner_points_before;

        int copy_obtuse_triangles_after = utils::countObtuseTriangles(cdt_copy, boundaryPolygon) ;

        if (copy_obtuse_triangles_after < obtuse_triangles_before) {
            cout << "Local minimum break! " << endl;

            for (Point & p : steinerPoints) {
                cdt.insertByStrategy(p, steiner_stategies::Strategy::RANDOM); // flip???
            }


            pn.push_back(utils::calculate_p(steinerPoints.size(), step, obtuse_triangles_before, obtuse_triangles_before));

            return copy_obtuse_triangles_after;
        }      

        return obtuse_triangles_before;
    }
};