#include <cmath>
#include <gmp.h>
#include <string>
#include <vector>

#include "cgal_definitions.h"
#include "triangulation_configuration.h"
#include "utils.hpp"

using namespace std;

string utils::simple_or_not(const Polygon_2& p) {
    if (p.is_simple())
        return "Simple Polygon";
    else
        return "Not Simple Polygon";
}

bool utils::is_obtuse(Point& a, Point& b, Point& c) {
    if (CGAL::angle(a, b, c) == CGAL::OBTUSE) {
        return true;
    }

    if (CGAL::angle(b, a, c) == CGAL::OBTUSE) {
        return true;
    }

    if (CGAL::angle(a, c, b) == CGAL::OBTUSE) {
        return true;
    }

    return false;
}

int utils::find_obtuse_angle(Point& a, Point& b, Point& c) {
    if (CGAL::angle(a, b, c) == CGAL::OBTUSE) {
        return 1;
    }

    if (CGAL::angle(b, a, c) == CGAL::OBTUSE) {
        return 0;
    }

    if (CGAL::angle(a, c, b) == CGAL::OBTUSE) {
        return 2;
    }

    return -1;
}

bool utils::checkConstraints(const CDT& cdt, const Polygon_2& boundaryPolygon, const Point& p1, const Point& p2) {
    CDT::Segment seg(p1, p2);

    for (auto edge = cdt.finite_edges_begin(); edge != cdt.finite_edges_end(); ++edge) {
        CDT::Edge edge_info = *edge;

        Vertex_handle v1 = edge->first->vertex(cdt.cw(edge->second));
        Vertex_handle v2 = edge->first->vertex(cdt.ccw(edge->second));

        if (utils::edge_inside_boundary(boundaryPolygon, v1, v2)) {

            if (cdt.is_constrained(edge_info)) {

                auto segment = cdt.segment(edge);

                if (segment.source() == p1 && segment.target() == p2) {
                    return true;
                }
                if (segment.source() == p2 && segment.target() == p1) {
                    return true;
                }
            }
        }
    }
    return false; // The pair is not in the constraints
}

std::tuple<int, int> utils::findOppositeEdge(int vertexIndex) {
    switch (vertexIndex) {
    case 0:                           // Vertex a
        return std::make_tuple(1, 2); // Edge bc
    case 1:                           // Vertex b
        return std::make_tuple(2, 0); // Edge ca
    case 2:                           // Vertex c
        return std::make_tuple(0, 1); // Edge ab
    default:
        cout << "CRITICAL ERROR: Invalid vertex index. Must be 0, 1, or 2. \n";
        exit(1);
    }
}

string utils::coordinate_to_rational(const K::FT& coord) {
    const auto exact_coord = CGAL::exact(coord);

    cout << exact_coord << endl;

    const mpq_t* gmpq_ptr = reinterpret_cast<const mpq_t*>(&exact_coord);

    mpz_t num, den;
    mpz_init(num);
    mpz_init(den);

    mpq_get_num(num, *gmpq_ptr); // Get the numerator
    mpq_get_den(den, *gmpq_ptr); // Get the denominator

    char* num_str = mpz_get_str(nullptr, 10, num);
    char* den_str = mpz_get_str(nullptr, 10, den);

    std::string result = std::string(num_str) + "/" + std::string(den_str);

    mpz_clear(num);
    mpz_clear(den);

    return result;
}

Point utils::findGeometricalMean(Polygon& polygon) {
    K::FT x_sum = 0;
    K::FT y_sum = 0;
    K::FT n = polygon.size();

    for (auto vertex = polygon.vertices_begin(); vertex != polygon.vertices_end(); ++vertex) {
        x_sum += vertex->x();
        y_sum += vertex->y();
    }

    return Point(x_sum / n, y_sum / n);
}

bool utils::isConvex(Polygon& polygon) {
    if (polygon.size() < 3) {
        return false;
    }

    auto orientation = polygon.orientation();

    for (size_t i = 0; i < polygon.size(); ++i) {
        const Point& p0 = polygon[i];
        const Point& p1 = polygon[(i + 1) % polygon.size()];
        const Point& p2 = polygon[(i + 2) % polygon.size()];

        if (CGAL::orientation(p0, p1, p2) != orientation) {
            return false;
        }
    }
    return true;
}

Face* utils::findNeighbor(Graph& graph, Point& a, Point& b, Point& c, int vertex_index, int& neighbor_vertex_index) {
    CDT& cdt = *(graph.cdt);
    bool no_neighbor_because_of_constraint = false;

    if (vertex_index == 0) {
        no_neighbor_because_of_constraint = checkConstraints(cdt, *(graph.boundaryPolygon), b, c);
    }
    if (vertex_index == 1) {
        no_neighbor_because_of_constraint = checkConstraints(cdt, *(graph.boundaryPolygon), a, c);
    }
    if (vertex_index == 2) {
        no_neighbor_because_of_constraint = checkConstraints(cdt, *(graph.boundaryPolygon), a, b);
    }

    if (no_neighbor_because_of_constraint) {
        return nullptr;
    }

    Point x, y;

    if (vertex_index == 0) {
        x = b;
        y = c;
    }
    if (vertex_index == 1) {
        x = a;
        y = c;
    }
    if (vertex_index == 2) {
        x = a;
        y = b;
    }

    for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
        Point aa = fit->vertex(0)->point();
        Point bb = fit->vertex(1)->point();
        Point cc = fit->vertex(2)->point();

        if (aa == a && bb == b && cc == c) {
            continue;
        }

        if ((aa == x && bb == y) || (aa == y && bb == x)) {
            neighbor_vertex_index = 2;
            return &(*fit);
        }

        if ((bb == x && cc == y) || (bb == y && cc == x)) {
            neighbor_vertex_index = 0;
            return &(*fit);
        }

        if ((cc == x && aa == y) || (cc == y && aa == x)) {
            neighbor_vertex_index = 1;
            return &(*fit);
        }
    }

    return nullptr;
}

bool utils::is_point_inside_polygon(const Polygon_2& polygon, const Point& point) {
    return polygon.bounded_side(point) == CGAL::ON_BOUNDED_SIDE;
}

bool utils::is_steiner_point_valid(const Polygon_2& polygon, const Point& point) {
    return polygon.bounded_side(point) == CGAL::ON_BOUNDED_SIDE || (ALLOW_POINTS_ON_BOUNDARY && polygon.bounded_side(point) == CGAL::ON_BOUNDARY);
}

Point utils::centroid(std::vector<Point>& points) {
    K::FT sumX = 0;
    K::FT sumY = 0;

    for (const auto& point : points) {
        sumX += point.x();
        sumY += point.y();
    }

    K::FT centerX = sumX / K::FT(points.size());
    K::FT centerY = sumY / K::FT(points.size());

    return Point(centerX, centerY);
}

bool utils::is_convex(const std::vector<Point>& boundary) {
    int n = boundary.size();
    if (n < 3) {
        return false;
    }

    bool got_positive = false;
    bool got_negative = false;

    for (int i = 0; i < n; ++i) {
        const Point& p0 = boundary[i];
        const Point& p1 = boundary[(i + 1) % n];
        const Point& p2 = boundary[(i + 2) % n];

        K::FT cross_product = (p1.x() - p0.x()) * (p2.y() - p1.y()) - (p1.y() - p0.y()) * (p2.x() - p1.x());

        if (cross_product > 0) {
            got_positive = true;
        } else if (cross_product < 0) {
            got_negative = true;
        }

        if (got_positive && got_negative) {
            return false;
        }
    }

    return true; // All cross products had the same sign, so the polygon is convex
}

int utils::countObtuseTriangles(CDT& cdt, const Polygon_2& boundaryPolygon) {
    int counter = 0;

    for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
        Point a = fit->vertex(0)->point();
        Point b = fit->vertex(1)->point();
        Point c = fit->vertex(2)->point();

        if (utils::is_obtuse(a, b, c)) {
            counter++;
        }
    }

    return counter;
}

double utils::average(vector<double>& values) {
    double sum = 0;

    if (values.size() <= 1) {
        return 0;
    }

    for (unsigned int i = 0; i < values.size() - 1; i++) {
        sum = sum + values[i];
    }

    return sum / values.size();
}

double utils::calculate_p(int steiner_points, int step, int obtuse_triangles_before, int obtuse_triangles_after) {
    double a = obtuse_triangles_after;
    double b = obtuse_triangles_before;
    double c = steiner_points;
    double d = steiner_points - step;

    return log(a / b) / log(c / d);
}

bool utils::edge_inside_boundary(const Polygon_2& boundaryPolygon, Point& p1, Point& p2) {
    Point midpoint((p1.x() + p2.x()) / 2.0, (p1.y() + p2.y()) / 2.0);

    return boundaryPolygon.bounded_side(midpoint) != CGAL::ON_UNBOUNDED_SIDE;
}

bool utils::edge_inside_boundary(const Polygon_2& boundaryPolygon, Vertex_handle v1, Vertex_handle v2) {
    Point p1 = v1->point();
    Point p2 = v2->point();

    return utils::edge_inside_boundary(boundaryPolygon, p1, p2);
}

bool utils::face_inside_boundary(const Polygon_2& boundaryPolygon, CDT::Face_handle& face) {
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    bool x1 = utils::edge_inside_boundary(boundaryPolygon, p1, p2);
    bool x2 = utils::edge_inside_boundary(boundaryPolygon, p2, p3);
    bool x3 = utils::edge_inside_boundary(boundaryPolygon, p3, p1);

    return x1 && x2 && x3;
}