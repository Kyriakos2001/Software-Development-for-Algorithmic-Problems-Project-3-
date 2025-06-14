// Standard C++
#include <gmp.h>
#include <iostream>
#include <map>
#include <time.h>
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

#include "AntColonySearch.h"
#include "LocalSearch.h"
#include "SimpleTriangulationSearch.h"
#include "SimulatedAnnealingSearch.h"

// Namespaces
using namespace std;

#define DRAW false

string to_rational(const K::FT& coord) {
    const auto exact_coord = CGAL::exact(coord);

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

int main(int argc, char* argv[]) {
    srand(time(0));

    if (argc < 3) {
        cout << "Invalid arguments: syntax: ./polyg input.json output.json" << endl;
        cout << "argc: " << argc << endl;
        return 0;
    }

    cout << "Checking CGAL version ... " << CGAL_VERSION_STR << endl;

    const char* inputfile = argv[1];
    const char* outputfile = argv[2];

    cout << "Input file: " << inputfile << endl;
    cout << "Output file: " << outputfile << endl;

    // Load file:
    JsonLoader loader;

    bool load_parameters = true;

    for (int i = 1; i < argc; i = i + 2) {
        if (strcmp(argv[i], "-L") == 0) {
            loader.L = atoi(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-a") == 0) {
            loader.alpha = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-b") == 0) {
            loader.beta = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-x") == 0) {
            loader.xi = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-y") == 0) {
            loader.psi = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-l") == 0) {
            loader.lambda = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-k") == 0) {
            loader.kappa = atof(argv[i + 1]);
            load_parameters = false;
        }

        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "ls") == 0) {
            loader.method = "local";
        }

        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "sa") == 0) {
            loader.method = "sa";
        }

        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "ant") == 0) {
            loader.method = "ant";
        }

        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "sals") == 0) {
            loader.method = "sals";
        }

        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "acls") == 0) {
            loader.method = "acls";
        }


        if (strcmp(argv[i], "-m") == 0 && strcmp(argv[i + 1], "legacy") == 0) {
            loader.method = "legacy";
        }

        if (strcmp(argv[i], "-R") == 0 && strcmp(argv[i + 1], "true") == 0) {
            loader.randomize_on_deadend = true;
        }

        load_parameters = false;
    }

    cout << "Load paremters from JSON: " << ((load_parameters) ? "on" : "off ") << endl;

    loader.load(inputfile, load_parameters);

    JsonExporter exporter(loader.getInstance());

    loader.print();

    CDT cdt;

    //
    // Add vertices to graph
    //
    vector<Point> points = loader.getPoints();

    for (const Point& p : points) {
        cdt.insert(p);
    }

    //
    // Add edges to graph
    //
    std::vector<std::pair<int, int>> constraints = loader.getConstraints();

    for (const auto& constraint : constraints) {
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }

    //
    // Add boundaries as edges to graph
    //
    std::vector<int> boundary_constraints = loader.getRegionBoundaries();

    for (size_t i = 0, j = 1; i < boundary_constraints.size() && j < boundary_constraints.size(); i++, j++) {
        cdt.insert_constraint(points[boundary_constraints[i]], points[boundary_constraints[j]]);
    }

    cdt.insert_constraint(points[boundary_constraints[boundary_constraints.size() - 1]], points[boundary_constraints[0]]);

    //
    // Create a polygon for the boundary
    //
    Polygon boundaryPolygon;

    for (size_t i = 0; i < boundary_constraints.size(); i++) {
        boundaryPolygon.push_back(points[boundary_constraints[i]]);
    }

    if (DRAW) {
        // CGAL::draw(cdt);
    }

    //
    // Triangulation
    //

    Graph graph;
    graph.cdt = &cdt;
    graph.boundaryPolygon = &boundaryPolygon;

    vector<Point> steinerPoints;

    if (loader.getMethod() == "legacy") {
        SimpleTriangulationSearch<float> triangulator;

        steiner_stategies::Strategy strategy = steiner_stategies::Strategy::PROJECTION;
        steinerPoints = triangulator.triangulate(strategy, graph, loader, boundaryPolygon);
    } else if (loader.getMethod() == "local") {
        LocalSearch<float> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        strategies.push_back(steiner_stategies::Strategy::CENTROID);

        steinerPoints = triangulator.triangulate(strategies, graph, loader, boundaryPolygon);
    } else if (loader.getMethod() == "sa") {
        SimulatedAnnealingSearch<float> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        strategies.push_back(steiner_stategies::Strategy::CENTROID);

        float alpha = loader.alpha;
        float beta = loader.beta;

        cout << "Alpha: " << alpha << endl;
        cout << "Beta: " << beta << endl;

        steinerPoints = triangulator.triangulate(strategies, graph, loader, boundaryPolygon, alpha, beta);
    } else if (loader.getMethod() == "ant") {
        AntColonySearch<float> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        // strategies.push_back(steiner_stategies::Strategy::BISECTION);

        float alpha = loader.alpha;
        float beta = loader.beta;

        cout << "Alpha: " << alpha << endl;
        cout << "Beta: " << beta << endl;

        steinerPoints = triangulator.triangulate(strategies, graph, loader, boundaryPolygon, alpha, beta);
    } else if (loader.getMethod() == "sals") {
        SimulatedAnnealingSearch<float> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        strategies.push_back(steiner_stategies::Strategy::CENTROID);

        float alpha = loader.alpha;
        float beta = loader.beta;

        cout << "Alpha: " << alpha << endl;
        cout << "Beta: " << beta << endl;

        steinerPoints = triangulator.triangulate(strategies, graph, loader, boundaryPolygon, alpha, beta);

        LocalSearch<float> triangulator_ls;

        vector<Point> steinerPoints2 = triangulator_ls.triangulate(strategies, graph, loader, boundaryPolygon);

        for (Point& p : steinerPoints2) {
            steinerPoints.emplace_back(p);
        }
    } else if (loader.getMethod() == "acls") {
        AntColonySearch<float> triangulator;

        vector<steiner_stategies::Strategy> strategies;

        strategies.push_back(steiner_stategies::Strategy::MAX_EDGE);
        strategies.push_back(steiner_stategies::Strategy::PERICENTER);
        strategies.push_back(steiner_stategies::Strategy::POLYGON);
        strategies.push_back(steiner_stategies::Strategy::PROJECTION);
        // strategies.push_back(steiner_stategies::Strategy::BISECTION);

        float alpha = loader.alpha;
        float beta = loader.beta;

        cout << "Alpha: " << alpha << endl;
        cout << "Beta: " << beta << endl;

        steinerPoints = triangulator.triangulate(strategies, graph, loader, boundaryPolygon, alpha, beta);

        LocalSearch<float> triangulator_ls;

        vector<Point> steinerPoints2 = triangulator_ls.triangulate(strategies, graph, loader, boundaryPolygon);

        for (Point& p : steinerPoints2) {
            steinerPoints.emplace_back(p);
        }
    } else {
        cerr << "Unknown method of search \n";
        return -1;
    }

    //
    // Export
    //

    for (Point& p : steinerPoints) {
        string s1 = to_rational(p.x());
        string s2 = to_rational(p.y());
        exporter.steiner_points_x.emplace_back(s1);
        exporter.steiner_points_y.emplace_back(s2);
    }

    std::vector<Vertex_handle> vertices;

    for (auto vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit) {
        vertices.push_back(vit); // Store the vertex handle
    }

    for (auto edge = cdt.finite_edges_begin(); edge != cdt.finite_edges_end(); ++edge) {
        Vertex_handle v1 = edge->first->vertex(cdt.cw(edge->second));
        Vertex_handle v2 = edge->first->vertex(cdt.ccw(edge->second));

        if (utils::edge_inside_boundary(boundaryPolygon, v1, v2)) {
            int x1 = std::find(vertices.begin(), vertices.end(), v1) - vertices.begin(); // Index of vertex 1
            int x2 = std::find(vertices.begin(), vertices.end(), v2) - vertices.begin(); // Index of vertex 2

            exporter.edges.emplace_back(x1, x2);
        }
    }

    // exporter.print();

    cout << "****" << endl;
    for (auto vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit) { // map each vertex to a unique index
        cout << *vit << endl;
    }
    cout << "****" << endl;

    // Save JSON
    cout << "Saving to file ... " << outputfile << endl;
    exporter.save(outputfile);

    if (DRAW) {
        // CGAL::draw(cdt);
    }

    return 0;
}
