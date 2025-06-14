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
class SimpleTriangulationSearch {
public:
    vector<Point> triangulate(steiner_stategies::Strategy strategy, Graph& graph, JsonLoader& loader,  Polygon & boundaryPolygon) {
        int MAX_ITERATIONS = loader.getL();
        int obtuse_triangles_initial = 0;
        int obtuse_triangles_before = 0;
        int obtuse_triangles_after = 0;
        
        vector<Point> steinerPoints;
        CDT & cdt = *(graph.cdt);

        cout << "# Max iterations: " << MAX_ITERATIONS << endl;

        for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
            Point a = fit->vertex(0)->point();
            Point b = fit->vertex(1)->point();
            Point c = fit->vertex(2)->point();

            if (utils::is_obtuse(a, b, c)) {
                obtuse_triangles_initial++;
            }
        }

        for (int i = 1; i <= MAX_ITERATIONS; i++) {
            int conflicts = 0;

            obtuse_triangles_before = 0;
            obtuse_triangles_after = 0;

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                if (utils::is_obtuse(a, b, c)) {
                    obtuse_triangles_before++;
                }
            }

            std::vector<CDT::Face_handle> finite_faces;

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                finite_faces.push_back(fit);
            }

            //
            // Optimization algorithm
            //

            for (auto fit : finite_faces) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                bool result = utils::is_obtuse(a, b, c);

                cout << "Checking triangle: " << a << "," << b << "," << c << ", obtuse:" << result << endl;

                conflicts++;

                if (result) {
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

                    Point* s = steiner_stategies::generateSteinerPoint(graph, a, b, c, strategy);

                    if (s != nullptr) {
                        if (utils::is_steiner_point_valid(boundaryPolygon, *s)) {
                            cdt.insertByStrategy(*s, strategy);
                            steiner_stategies::removeConflictPoints(graph, a, b, c, strategy);
                            
                            steinerPoints.emplace_back(*s);
                        }

                        delete s;
                    }
                }
            }

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                if (utils::is_obtuse(a, b, c)) {
                    obtuse_triangles_after++;
                }
            }

            cout << " ### Initial: " << obtuse_triangles_initial << ", before: " << obtuse_triangles_before << ", after: " << obtuse_triangles_before << endl;
            if (obtuse_triangles_after >= obtuse_triangles_before || conflicts == 0) {
                break;
            }
        }

        cout << "Initial obtuse triangles: " << obtuse_triangles_initial << endl;
        cout << "Total obtuse triangles: " << obtuse_triangles_after << endl;

        return steinerPoints;
    }
};
