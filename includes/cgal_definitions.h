#pragma once

// CGAL
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/convex_hull_2.h>

#include "CustomConstrainedDelaunayTriangulation_2.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef K::Point_2 Point;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Polygon_2<K> Polygon;
typedef K::Line_2 Line;

typedef CGAL::Exact_predicates_tag Itag;
typedef CustomConstrainedDelaunayTriangulation_2<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge Edge;
typedef CDT::Face Face;

typedef CDT::Vertex_handle Vertex_handle;
