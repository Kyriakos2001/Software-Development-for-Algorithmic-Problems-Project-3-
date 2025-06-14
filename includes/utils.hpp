#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <tuple>
#include <vector>

#include "cgal_definitions.h"
#include "graph_definitions.h"

using std::string;
using std::vector;

namespace utils {
    string simple_or_not(const Polygon_2 & p);

    bool is_obtuse(Point & a, Point & b, Point &c);

    int find_obtuse_angle(Point & a, Point & b, Point &c);

    string coordinate_to_rational(const K::FT& coord);    

    std::tuple<int, int> findOppositeEdge(int vertexIndex);

    bool checkConstraints(const CDT& cdt, const Polygon_2& boundaryPolygon, const Point& p1, const Point& p2);

    Point findGeometricalMean(Polygon & polygon);

    bool isConvex(Polygon & polygon);

    Point centroid(std::vector<Point> & points);

    int countObtuseTriangles(CDT& cdt, const Polygon_2& boundaryPolygon);

    bool is_point_inside_polygon(const Polygon_2& polygon, const Point& point);

    bool is_steiner_point_valid(const Polygon_2& polygon, const Point& point);

    // index 0: vertex a: edge: bc
    // index 1: vertex b: edge: ac
    // index 2: vertex c: edge: ab
    Face * findNeighbor(Graph & graph, Point& a, Point& b, Point& c, int vertex_index, int & neighbor_vertex_index);


    bool is_convex(const std::vector<Point>& boundary);

    double average(vector<double> & values);

    double calculate_p(int steiner_points, int step, int obtuse_triangles_before, int obtuse_triangles_after);

    bool edge_inside_boundary(const Polygon_2& boundaryPolygon, Point & p1, Point & p2);
    
    bool edge_inside_boundary(const Polygon_2& boundaryPolygon, Vertex_handle v1, Vertex_handle v2);

    bool face_inside_boundary(const Polygon_2& boundaryPolygon, CDT::Face_handle& face);
}

string to_rational(const K::FT& coord);

#endif
