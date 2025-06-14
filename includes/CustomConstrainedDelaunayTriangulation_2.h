#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>

template <class Gt, class Tds = CGAL::Default, class Itag = CGAL::Default>
class CustomConstrainedDelaunayTriangulation_2 : public CGAL::Constrained_Delaunay_triangulation_2<Gt, Tds, Itag> {
public:
    using Base = CGAL::Constrained_Delaunay_triangulation_2<Gt, Tds, Itag>;

    using typename Base::Face_handle;
    using typename Base::Point;
    using typename Base::Vertex_handle;
    using typename Base::Locate_type;



    // Constructors
    CustomConstrainedDelaunayTriangulation_2(const Gt& gt = Gt()) : Base(gt) {

    }

    CustomConstrainedDelaunayTriangulation_2(typename Base::List_constraints& lc, const Gt& gt = Gt()) : Base(lc, gt) {}



    template <class InputIterator>
    CustomConstrainedDelaunayTriangulation_2(InputIterator it, InputIterator last, const Gt& gt = Gt()) : Base(it, last, gt) {}

    // New insert method without flips

    Vertex_handle insert_no_flip(const Point& a, Face_handle start = Face_handle()) {
        // Call Ctr::insert without flip_around
        Vertex_handle va = this->Base::Ctr::insert(a, start); // Directly call Ctr::insert from the base
        return va;
    }

    Vertex_handle insert_no_flip(const Point& a, Locate_type lt, Face_handle loc, int li) {
        Vertex_handle va = this->Base::Ctr::insert(a, lt, loc, li); // Directly call Ctr::insert from the base
        return va;
    }

    void remove_no_flip(Vertex_handle v) {
        this->Base::Ctr::remove(v);
    }

    void insertByStrategy(const Point & p, int strategy) {
        if (strategy <= 0) {
            CGAL::Constrained_Delaunay_triangulation_2<Gt, Tds, Itag>::insert(p);
        } else {
            this->insert_no_flip(p);
        }
    }
};

