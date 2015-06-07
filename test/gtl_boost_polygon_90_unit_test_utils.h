#ifndef GTL_BOOST_POLYGON_90_UNIT_TEST_UTILS_H
#define GTL_BOOST_POLYGON_90_UNIT_TEST_UTILS_H

#include <iostream>
#define BOOST_POLYGON_NO_DEPS
#include <boost/polygon/polygon.hpp>

typedef int intDC;
typedef boost::polygon::polygon_90_with_holes_data<intDC> GTLPolygon;
typedef boost::polygon::polygon_90_set_data<intDC> GTLPolygonSet;
typedef boost::polygon::polygon_90_concept GTLPolygonConcept;
typedef boost::polygon::point_data<intDC> GTLPoint;

/** Utility functions which can be used across all unit-tests. */
inline void PrintPolygon(const GTLPolygon& p){
   //get an iterator of the point_data<int>
   boost::polygon::point_data<int> pt;
   boost::polygon::polygon_90_data<int>::iterator_type itr;

   size_t vertex_id = 0;
   for(itr = p.begin(); itr != p.end(); ++itr){
      pt = *itr;
      std::cout << "Vertex-" << ++vertex_id << "(" << pt.x() <<
         "," << pt.y() << ")" << std::endl;
   }
}

// size: is the number of vertices //
template<typename Polygon>
inline void CreateGTLPolygon(const int *coords, size_t size, Polygon& r){
   std::vector<GTLPoint> pts;

   for(size_t i=0; i<size; i++){
      pts.push_back( GTLPoint(coords[2*i], coords[2*i+1]) );
   }
   boost::polygon::set_points(r, pts.begin(), pts.end());
}

#endif
