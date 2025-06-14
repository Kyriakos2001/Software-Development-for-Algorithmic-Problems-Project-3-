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

// Namespaces
using namespace std;

template <typename T>
class LocalSearchRandomization {
public:
    vector<Point> triangulate(vector<steiner_stategies::Strategy>& strategies, Graph& graph, JsonLoader& loader, Polygon& boundaryPolygon) {
        vector<Point> steinerPoints;
        vector<double> pn;
        CDT& cdt = *(graph.cdt);

        int MAX_ITERATIONS = loader.getL();
        int obtuse_triangles_initial = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));
        int obtuse_triangles_before = 0;
        int obtuse_triangles_after = 0;
        int convergence_iterations = 0;
        bool local_minimum_reached = false;

        cout << "# Max iterations: " << MAX_ITERATIONS << endl;

        for (int i = 1; i <= MAX_ITERATIONS; i++) {
            int conflicts = 0;

            obtuse_triangles_before = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));
            obtuse_triangles_after = 0;

            convergence_iterations++;

            std::vector<CDT::Face_handle> finite_faces;

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                finite_faces.push_back(fit);
            }

            //
            // Optimization algorithm
            //

            const unsigned int steiner_points_before_algorithm = steinerPoints.size();

            for (auto fit : finite_faces) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                bool result = utils::is_obtuse(a, b, c);

                cout << " - Iteration: " << i << " Checking triangle: " << a << "," << b << "," << c << ", obtuse:" << result << ", obtuse triangles: " << obtuse_triangles_before << endl;

                conflicts++;

                if (result) {                    
                    // ---------------------------------------------------------
                    map<steiner_stategies::Strategy, int> options;

                    for (steiner_stategies::Strategy& strategy : strategies) {
                        CDT cdt_copy = cdt;
                        Graph graph_copy;
                        graph_copy.cdt = &cdt_copy;
                        graph_copy.boundaryPolygon = graph.boundaryPolygon;

                        if (strategy == steiner_stategies::Strategy::PERICENTER) { // if max edge is constraint skip ...
                            int i = utils::find_obtuse_angle(a, b, c);             // 0:a, 1:b, 2:c
                            if (i == -1) {
                                cout << "CRITICAL ERROR: find_obtuse_angle failed " << endl;
                                exit(1);
                            }

                            std::tuple<int, int> edge_indices = utils::findOppositeEdge(i); // a:1,2 b:0,2 c:0,1

                            Point& p1 = fit->vertex(std::get<0>(edge_indices))->point();
                            Point& p2 = fit->vertex(std::get<1>(edge_indices))->point();

                            bool is_constraint = utils::checkConstraints(cdt, boundaryPolygon, p1, p2);

                            if (is_constraint) {
                                continue;
                            }
                        }

                        Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, strategy);

                        if (s != nullptr) {
                            if (utils::is_steiner_point_valid(boundaryPolygon, *s)) {
                                graph_copy.cdt->insertByStrategy(*s, strategy);
                                steiner_stategies::removeConflictPoints(graph_copy, a, b, c, strategy);
                            }

                            delete s;

                            int copy_obtuse_triangles_after = utils::countObtuseTriangles(cdt_copy, *(graph.boundaryPolygon)) ;

                            options[strategy] = copy_obtuse_triangles_after;

                            cout << "\t";
                            steiner_stategies::printStrategy(strategy);
                            cout << " - Method succeeded " << copy_obtuse_triangles_after << endl;
                        } else {
                            cout << "\t";
                            steiner_stategies::printStrategy(strategy);
                            cout << " - Method failed    " << endl;
                        }

                    }
                    // ---------------------------------------------------------
                    int min_value = std::numeric_limits<int>::max();
                    steiner_stategies::Strategy strategy = steiner_stategies::Strategy::NONE;

                    for (const auto& [key, value] : options) {
                        if (value < min_value) {
                            min_value = value;
                            strategy = key;
                        }
                    }

                    if (strategy != steiner_stategies::Strategy::NONE) {                        
                        if (min_value < obtuse_triangles_before) {
                            Point* s = steiner_stategies::generateSteinerPoint(graph, a, b, c, strategy);

                            cout << "*Best Strategy selected: " ;

                            steiner_stategies::printStrategy(strategy);

                            cout << endl;

                            if (utils::is_steiner_point_valid(boundaryPolygon, *s)) {
                                graph.cdt->insertByStrategy(*s, strategy);
                                steiner_stategies::removeConflictPoints(graph, a, b, c, strategy);

                                steinerPoints.emplace_back(*s);

                                pn.push_back(utils::calculate_p(steinerPoints.size(), 1, obtuse_triangles_before, min_value));
                            } else {
                                cout << "Steiner point ignored  - outside the boundaries " << endl;
                            }

                            delete s;

                            break;
                        } else {
                            cout << "*Best Strategy rejected: " ;

                            steiner_stategies::printStrategy(strategy);

                            cout << " as it does not improve the state" << " method min: " << min_value<< ", current:" << obtuse_triangles_before << endl;
                        }
                    }
                }
            }

            const unsigned int steiner_points_after_algorithm = steinerPoints.size();

            if (steiner_points_after_algorithm == steiner_points_before_algorithm) {
                local_minimum_reached = true;
            }

            obtuse_triangles_after = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));

            cout << " ### Initial: " << obtuse_triangles_initial << ", before: " << obtuse_triangles_before << ", after: " << obtuse_triangles_before << endl;
            // if (obtuse_triangles_after >= obtuse_triangles_before || conflicts == 0 || obtuse_triangles_after == 0) {
            //     break;
            // }

            if (obtuse_triangles_after == 0 || local_minimum_reached) {
                break;
            }
        }

        double p = utils::average(pn);

        cout << "***********************************************************************" << endl;
        cout << " - Initial obtuse triangles  : " << obtuse_triangles_initial << endl;
        cout << " - Total obtuse triangles    : " << obtuse_triangles_after << endl;
        cout << " - Total steiner points      : " << steinerPoints.size() << endl;
        cout << " - Local minimum reached     : " << local_minimum_reached << endl;
        cout << " - Iterations for convergence: " << convergence_iterations << " of " << MAX_ITERATIONS << endl;
        cout << " - Convergence rate metric   : " << p << endl;
        cout << "***********************************************************************" << endl;

        return steinerPoints;
    }
};
