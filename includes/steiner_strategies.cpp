#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include "cgal_definitions.h"
#include "steiner_strategies.h"
#include "utils.hpp"


using namespace std;

Point * steiner_stategies::generateSteinerPointFromMaxEdge(Graph & graph, Point& a, Point& b, Point& c) {
    K::FT length12_sq = CGAL::squared_distance(a, b);

    K::FT length23_sq = CGAL::squared_distance(b, c);

    K::FT length31_sq = CGAL::squared_distance(c, a);

    if (length12_sq >= length23_sq && length12_sq >= length31_sq) {
        return new Point((a.x() + b.x()) / 2, (a.y() + b.y()) / 2);
    } else if (length23_sq >= length12_sq && length23_sq >= length31_sq) {
        return new Point((b.x() + c.x()) / 2, (b.y() + c.y()) / 2);
    } else {
        return new Point((c.x() + a.x()) / 2, (c.y() + a.y()) / 2);
    }
}

Point * steiner_stategies::generateSteinerPointFromPericenter(Graph & graph, Point& a, Point& b, Point& c) {
    Point circumcenter = CGAL::circumcenter(a, b, c);

    Point * p = new Point(circumcenter);

    return p;
}

Point * steiner_stategies::generateSteinerPointInsideConvexHull(Graph & graph, Point& a, Point& b, Point& c) {
    CDT & cdt = *(graph.cdt);

    // cout << "Examining triangle " << a << " " << b << " " << c << endl;

    vector<Point> boundary;

    boundary.emplace_back(a);
    boundary.emplace_back(b);
    boundary.emplace_back(c);

    for (int vertex_index=0;vertex_index<3;vertex_index++) {
        int neighbor_vertex_index = 0;

        Face * face = utils::findNeighbor(graph, a, b, c, vertex_index, neighbor_vertex_index);

        if (face == nullptr) {
            // cout << "\tDirection " << vertex_index << ", No neighbor found " << endl;
        } else {
            Point aa = face->vertex(0)->point(); 
            Point bb = face->vertex(1)->point();
            Point cc = face->vertex(2)->point();

            bool is_obtuse = utils::find_obtuse_angle(aa,bb,cc) >= 0;

            // cout << "\tDirection " << vertex_index << ", Face: " << face->vertex(0)->point() << " " << face->vertex(1)->point() << " " << face->vertex(2)->point() << ", obtuse: " << is_obtuse << endl;

            if (!is_obtuse) {                
                continue;
            }

            

            if (neighbor_vertex_index == 0) {
                // cout << "Adding vertex to boundary (case 0): " << aa << endl;
                boundary.emplace_back(aa);
            } else if (neighbor_vertex_index == 1) {
                // cout << "Adding vertex to boundary (case 1): " << bb << endl;
                boundary.emplace_back(bb);
            } else if (neighbor_vertex_index == 2) {
                // cout << "Adding vertex to boundary (case 2): " << cc << endl;
                boundary.emplace_back(cc);
            } else {
                // cout << "Invalid index" << neighbor_vertex_index << endl;
                exit(1);
            }

            if (!utils::is_convex(boundary)) {
                boundary.pop_back();
                // cout << "Polygon not convex \n";
            }
        }
    }

    if (boundary.size() > 3) {
        for (unsigned int i=0;i<boundary.size() - 1;i++) {
            Point & p = boundary[i];
            Point & q = boundary[i+1];
            // cout << "Adding boundary: " << p << " to " << q << endl;
            cdt.insert_constraint(p,q);
        }

        Point & p = boundary[boundary.size()-1];
        Point & q = boundary[0];

        // cout << "Adding boundary: " << p << " to " << q << endl;

        cdt.insert_constraint(p,q);


        Point centroid = utils::centroid(boundary);
        return new Point(centroid);
    } else {
        Point centroid = utils::centroid(boundary);
        return new Point(centroid);
    }
}

Point * steiner_stategies::generateSteinerPointProjection(Graph & graph, Point& a, Point& b, Point& c) {
    int i = utils::find_obtuse_angle(a,b,c);

    if (i == 0) { // A
        Line templine(b, c);
        Point projection = templine.projection(a);
        return new Point(projection);
    } else if (i == 1) { // B
        Line templine(a, c);
        Point projection = templine.projection(b);
        return new Point(projection); 
    } else if (i == 2) { // C
         Line templine(a, b);
        Point projection = templine.projection(c);
        return new Point(projection);
    }
     
    return nullptr;
}

Point * steiner_stategies::generateSteinerPointBiSector(Graph & graph, Point& a, Point& b, Point& c) {
    // TODO
    return nullptr;
}

Point * steiner_stategies::generateSteinerPointAltitude(Graph & graph, Point & a, Point & b, Point &c) {
    // TODO
    return nullptr;
}


Point* steiner_stategies::generateSteinerPointRandom(Graph & graph, Point & a, Point & b, Point & c) {
    // Calculate the barycenter of the triangle
    double barycenter_x = CGAL::to_double(a.x() + b.x() + c.x()) / 3.0;
    double barycenter_y = CGAL::to_double(a.y() + b.y() + c.y()) / 3.0;

    // Define the Gaussian distribution parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist_x(barycenter_x, std::abs(CGAL::to_double((b.x() - a.x()) + (c.x() - a.x()))) / 6.0);
    std::normal_distribution<double> dist_y(barycenter_y, std::abs(CGAL::to_double((b.y() - a.y()) + (c.y() - a.y()))) / 6.0);

    Point* random_point = nullptr;

    // Generate a random point inside the triangle using rejection sampling
    for (int i = 0; i < 100; ++i) { // Limit to 100 iterations to avoid infinite loops
        double x = dist_x(gen);
        double y = dist_y(gen);
    
        // Check if the point is inside the triangle using barycentric coordinates
        double denominator = CGAL::to_double((b.y() - c.y()) * (a.x() - c.x()) + (c.x() - b.x()) * (a.y() - c.y()));
        double alpha = CGAL::to_double((b.y() - c.y()) * (x - c.x()) + (c.x() - b.x()) * (y - c.y())) / denominator;
        double beta = CGAL::to_double((c.y() - a.y()) * (x - c.x()) + (a.x() - c.x()) * (y - c.y())) / denominator;
        double gamma = 1.0 - alpha - beta;

        if (alpha >= 0 && beta >= 0 && gamma >= 0) {
            random_point = new Point(x, y);
            break;
        }
    }

    return random_point;
}

Point * steiner_stategies::generateSteinerPoint(Graph & graph, Point& a, Point& b, Point& c, Strategy strategy) {
    if (strategy == MAX_EDGE) {
        return generateSteinerPointFromMaxEdge(graph, a, b, c);
    }

    if (strategy == PERICENTER) {
        return generateSteinerPointFromPericenter(graph, a, b, c);
    }

    if (strategy == POLYGON) {
        return generateSteinerPointInsideConvexHull(graph, a, b, c);
    }

    if (strategy == BISECTION) {
        return generateSteinerPointBiSector(graph, a, b, c);
    }

    if (strategy == ALTITUDE) {
        return generateSteinerPointAltitude(graph, a, b, c);
    }

    if (strategy == CENTROID) {
        return generateSteinerPointCentroid(graph, a, b, c);
    }

    if (strategy == RANDOM) {
        return generateSteinerPointRandom(graph, a, b, c);
    }

    if (strategy != PROJECTION) {
        cerr << "Warning: strategy not defined - using as default projectin method" << endl;
    }

    return generateSteinerPointProjection(graph, a, b, c);
}




void steiner_stategies::removeConflictPointsInsideConvexHull(Graph & graph, Point & a, Point & b, Point &c) {
    CDT & cdt = *(graph.cdt);

    vector<Point> boundary;

    boundary.emplace_back(a);
    boundary.emplace_back(b);
    boundary.emplace_back(c);

    for (int vertex_index=0;vertex_index<3;vertex_index++) {
        int neighbor_vertex_index = 0;

        Face * face = utils::findNeighbor(graph, a, b, c, vertex_index, neighbor_vertex_index);

        if (face == nullptr) {
            // cout << "\tDirection " << vertex_index << ", No neighbor found " << endl;
        } else {
            Point aa = face->vertex(0)->point(); 
            Point bb = face->vertex(1)->point();
            Point cc = face->vertex(2)->point();

            bool is_obtuse = utils::find_obtuse_angle(aa,bb,cc) >= 0;

            // cout << "\tDirection " << vertex_index << ", Face: " << face->vertex(0)->point() << " " << face->vertex(1)->point() << " " << face->vertex(2)->point() << ", obtuse: " << is_obtuse << endl;

            if (!is_obtuse) {                
                continue;
            }

            if (neighbor_vertex_index == 0) {
                // cout << "Adding vertex to boundary (case 0): " << aa << endl;
                boundary.emplace_back(aa);
            } else if (neighbor_vertex_index == 1) {
                // cout << "Adding vertex to boundary (case 1): " << bb << endl;
                boundary.emplace_back(bb);
            } else if (neighbor_vertex_index == 2) {
                // cout << "Adding vertex to boundary (case 2): " << cc << endl;
                boundary.emplace_back(cc);
            } else {
                // cout << "Invalid index" << neighbor_vertex_index << endl;
                exit(1);
            }

            if (!utils::is_convex(boundary)) {
                boundary.pop_back();
                // cout << "Polygon not convex \n";
            }
        }
    }

    if (boundary.size() > 3) {
        // remove all points within the boundary

        std::vector<Vertex_handle> vertices_to_remove;

        Polygon_2 polygon(boundary.begin(), boundary.end());

        for (auto vh = cdt.finite_vertices_begin(); vh != cdt.finite_vertices_end(); ++vh) {
            if (utils::is_point_inside_polygon(polygon, vh->point())) {
                vertices_to_remove.push_back(vh);
            }
        }

        // Remove points inside the boundary
        for (auto vh : vertices_to_remove) {
            cdt.remove(vh);
        }

        // Output remaining points
        for (auto vh = cdt.finite_vertices_begin(); vh != cdt.finite_vertices_end(); ++vh) {
            std::cout << vh->point() << std::endl;
        }
        
        return;
    }
}



void steiner_stategies::removeConflictPoints(Graph & graph, Point & a, Point & b, Point &c, Strategy strategy) {
    if (strategy == POLYGON) {
        return removeConflictPointsInsideConvexHull(graph, a, b, c);
    } else {
        return;
    }
}

void steiner_stategies::removeConflictPoints(Graph & graph, Point & a, Point & b, Point &c, int strategy) {
    steiner_stategies::removeConflictPoints(graph, a, b,c , (Strategy)strategy);
}

void steiner_stategies::removeConflictPoints(CDT & cdt, Point & a, Point & b, Point &c, int strategy) {
    Graph graph;
    graph.cdt = &cdt;

    steiner_stategies::removeConflictPoints(graph, a, b, c, strategy);    
}

Point * steiner_stategies::generateSteinerPointCentroid(Graph & graph, Point & a, Point & b, Point &c) {
    std::vector<Point> points;

    points.emplace_back(a);
    points.emplace_back(b);
    points.emplace_back(c);

    Point centroid = utils::centroid(points);
    return new Point(centroid);
}

void steiner_stategies::printStrategy(Strategy strategy) {
    if (strategy == MAX_EDGE) {
        cout << "MAX_EDGE  ";
    } else if (strategy == PERICENTER) {
        cout << "PERICENTER";
    } else if (strategy == POLYGON) {
        cout << "POLYGON   ";
    } else if (strategy == BISECTION) {
        cout << "BISECTION ";
    } else if (strategy == ALTITUDE) {
        cout << "ALTITUDE  ";
    } else if (strategy == PROJECTION) {
        cout << "PROJECTION";
    } else if (strategy == CENTROID) {
        cout << "CENTROID";
    } else {
        cout << "UNKNOWN STRATEGY";
    }
}