#pragma once

#include "cgal_definitions.h"
#include "graph_definitions.h"

using std::string;

namespace steiner_stategies {
    enum Strategy {
        POLYGON = 0,
        PERICENTER = 1,
        MAX_EDGE,
        PROJECTION,
        CENTROID,
        BISECTION, // NOT IMPLEMENTED
        ALTITUDE, // NOT IMPLEMENTED
        RANDOM, // NOT IMPLEMENTED
        NONE,
    };

    void printStrategy(Strategy strategy);

    //
    // Strategies
    //

    Point * generateSteinerPointFromMaxEdge(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointFromPericenter(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointInsideConvexHull(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointProjection(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointCentroid(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointRandom(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPoint(Graph & graph, Point & a, Point & b, Point &c, Strategy strategy);


    Point * generateSteinerPointBiSector(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointAltitude(Graph & graph, Point & a, Point & b, Point &c);

    //
    // Remove points if needed
    //
    void removeConflictPointsInsideConvexHull(Graph & graph, Point & a, Point & b, Point &c);

    void removeConflictPoints(Graph & graph, Point & a, Point & b, Point &c, Strategy strategy);

    void removeConflictPoints(Graph & graph, Point & a, Point & b, Point &c, int strategy);
   
    void removeConflictPoints(CDT & cdt, Point & a, Point & b, Point &c, int strategy);
}



