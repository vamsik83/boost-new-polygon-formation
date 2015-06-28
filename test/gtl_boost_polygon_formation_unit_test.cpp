/*
  Copyright 2008 Intel Corporation

  Use, modification and distribution are subject to the Boost Software License,
  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt).
*/
#include <iostream>
#define BOOST_POLYGON_NO_DEPS
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/polygon_90_set_data_iterator.hpp>
#include "gtl_boost_polygon_90_unit_test_utils.h"

namespace gtl = boost::polygon;
using namespace boost::polygon::operators;
#include <time.h>
#include <stdlib.h>
#include <algorithm>

void assert_s(bool c, std::string msg) {
  if(!c) {
    std::cout << msg << std::endl;
    exit( 1);
  }
}

/*************New Polygon Formation Tests********************/
/*
 *
 * Test Input:
 * +--------------------+
 * |        +-------+   |
 * |        |       |   |
 * |        |       |   |
 * +-----+  |       |   |
 *       |  |       |   |
 *       |  |       |   |
 * +-----+  |       |   |
 * |        |       |   |
 * |        |       |   |
 * |        +-------+   |
 * +--------+           |
 *          |           |
 *          |           |
 * +--------+           |
 * |                    |
 * |                    |
 * +--------+           |
 *          |           |
 *          |           |
 * +--------+           |
 * |                    |
 * |                    |
 * +--------------------+
 *
 * Test Plan: 
 * a. call 'get(out, param)' , param >= 4 
 * b. check if each polygon in the container is <= param
 * c. check the area of all the pieces sum up to original piece
 */

int test_new_polygon_formation(int argc, char** argv){
   //                                               //
   // Sub-Test-1: do a Boolean and call the new get //
   //                                               //
   int coordinates[] = {0,0, 10,0, 10,10, 0,10};
   int coordinates1[] = {9,1, 20,1, 20,10, 9,10};
   std::vector<GTLPoint> pts;
   size_t count = sizeof(coordinates)/(2*sizeof(intDC)); 
   size_t count1 = sizeof(coordinates1)/(2*sizeof(intDC));
   GTLPolygon poly, poly1;
   GTLPolygonSet polySet;
   
   CreateGTLPolygon(coordinates, count, poly);
   CreateGTLPolygon(coordinates1, count1, poly1);

   polySet.insert(poly);
   polySet.insert(poly1);

   std::vector<GTLPolygon> result;
   polySet.get(result, 100);

   if(result.size() > 1){
      std::cerr << "FAILED: expecting only one polygon because the"
         " threshold is 100" << std::endl;
      return 1;
   }

   if(result[0].size() != 6){
      std::cerr << "FAILED: expecting only 6 vertices" << std::endl;
      return 1;
   }

   if(area(result[0]) != 190){
      std::cerr <<"FAILED: expecting only 6 vertices" << std::endl;
      return 1;
   }

   //expect no more than 1 polygon
   std::cout << "Found " << result.size() << "polygons after union" 
      << std::endl;
   for(size_t i=0; i<result.size(); i++){
      PrintPolygon(result[i]);
   }

   intDC shell_coords[] = {0,0, 10,0, 10,21, 0,21, 0,15, 3,15, 3,13,
      0,13, 0,10, 5,10, 5,8, 0,8, 0,5, 5,5, 5,3, 0,3};
   intDC hole_coords[] = {4,11, 7,11, 7,19, 4,19};
   GTLPolygon slice_polygon, slice_hole;
   count = sizeof(shell_coords)/(2*sizeof(intDC));
   count1 = sizeof(hole_coords)/(2*sizeof(intDC));

   CreateGTLPolygon(shell_coords, count, slice_polygon);
   CreateGTLPolygon(hole_coords, count1, slice_hole);

   result.clear();
   polySet.clear();
   polySet.insert(slice_polygon);
   polySet.insert(slice_hole, true);

   polySet.get(result);
   double gold_area = 0;
   std::cout << "Found " << result.size() << " slices" << std::endl;
   for(size_t i=0; i<result.size(); i++){
      PrintPolygon(result[i]);
      gold_area += area(result[i]); 
   }

   result.clear();
   polySet.get(result, 6);
   double platinum_area = 0;
   std::cout << "Found " << result.size() << " slices" << std::endl;
   for(size_t i=0; i<result.size(); i++){
      PrintPolygon(result[i]);
      platinum_area += area(result[i]); 
      if(result[i].size() > 6){
         std::cerr << "FAILED: expecting size to be less than 6" << std::endl;
         return 1;
      }
   }

   std::cout << "platinum_area = " << platinum_area << " , gold_area=" 
      << gold_area << std::endl;
   if( platinum_area != gold_area){
      std::cerr << "FAILED: Area mismatch" << std::endl;
      return 1;
   }
   std::cout << "[SUB-TEST-1] PASSED\n";

   result.clear();
   polySet.get(result, 4);
   platinum_area = 0;
   std::cout << "Found " << result.size() << " slices" << std::endl;
   for(size_t i=0; i<result.size(); i++){
      PrintPolygon(result[i]);
      platinum_area += area(result[i]); 
      if(result[i].size() > 4){ 
         std::cerr << "FAILED: expecting size to be < 4" << std::endl;
         return 1;
      }
   }

   std::cout << "platinum_area=" << platinum_area << ", gold_area=" 
      << gold_area << std::endl;

   if( platinum_area != gold_area){
      std::cerr << "FAILED: Area mismatch" << std::endl;
      return 1;
   }

   std::cout << "[SUB-TEST-1] PASSED" << std::endl;
   return 0;
}

/* 
 * INPUT:
 *   +--------+
 *   |        |
 *   |        |
 *   |        +---+
 *   |            |
 *   |        +---+
 *   |        |
 *   +--------+
 *            X 
 *            
 * TEST PLAN: as the sweepline moves and reaches
 * X the number of vertices in the solid jumps by 4
 * instead of 2. So make sure we don't get a 6 vertex
 * polygon when the threshold is 4 and 6.
 */
int test_new_polygon_formation_marginal_threshold(int argc, char**){
   std::vector<GTLPoint> pts;
   GTLPolygon polygon;
   GTLPolygonSet pset;
   std::vector<GTLPolygon> result;
   intDC coords[] = {0,0, 15,0, 15,10, 10,10, 10,15, 5,15, 5,10, 0,10};
   size_t count = sizeof(coords)/(2*sizeof(intDC));

   CreateGTLPolygon(coords, count, polygon);
   pset.insert(polygon);

   for(size_t i=0; i<1; i++){
      pset.get(result, i ? 4 : 6);
      double gold_area = 175, plat_area = 0;
      for(size_t i=0; i<result.size(); i++){
         if(result[i].size() > (i ? 4 : 6) ){
            size_t expected = i ? 4 : 6;
            std::cerr << "FAILED: Expecting no more than " <<
               expected << " vertices" << std::endl;
            return 1;
         }
         PrintPolygon(result[i]);
         plat_area += area(result[i]); 
      }

      if(plat_area != gold_area){
         std::cerr << "FAILED area mismatch gold=" << gold_area <<
            " plat=" << plat_area << std::endl;
         return 1;
      }
   }
   std::cout << "Test Passed" << std::endl;
   return 0;
}

/*******************Test Iterator Support*******************/

int test_orthogonal_half_edge(void) {
    typedef boost::polygon::orthogonal_half_edge<int> HalfEdge;
    std::vector<HalfEdge> test_input;
    HalfEdge tmp;

    test_input.push_back(tmp.set(6, 9, 9));
    test_input.push_back(tmp.set(-2, 5, 7, false));
    test_input.push_back(tmp.set(5, 7, 7, false));
    test_input.push_back(tmp.set(6, 9, 5));
    test_input.push_back(tmp.set(5, 6, 3, false));
    test_input.push_back(tmp.set(2, 3, 5, false));
    test_input.push_back(tmp.set(-1, 1, 5, false));
    test_input.push_back(tmp.set(-1, 1, 3, false));

    std::sort(test_input.begin(), test_input.end());

    for (size_t i=1; i<test_input.size(); i++) {
        bool lorder = test_input[i-1].location() <= test_input[i].location();
        bool iorder = test_input[i-1].begin() <= test_input[i].begin();

        if (!lorder) {
            std::cerr << "[test_orthogonal_half_edge]: failed (see below)" << std::endl;
            test_input[i-1].print();
            test_input[i].print();
            return 1;
        }
    }

    std::cerr << "[test_orthogonal_half_edge]: PASSED" << std::endl;
    return 0;
}

extern int test_orthogonal_half_edge_iterator();
extern int test_active_tail_formation();
/************************************************************/

int main(int argc, char** argv) {
  //this test fails and I'd like to get it to pass
  //if(!test_colinear_duplicate_points()) return 1;
  //

  /** Iterative polygon formation tests. */
  if (test_orthogonal_half_edge()) {
     std::cerr << "[test_orthogonal_half_edge]: failed" << std::endl;
      return 1;
  }

  if (test_orthogonal_half_edge_iterator()) {
    std::cerr << "[test_orthogonal_half_edge_iterator]: failed" 
      << std::endl;
    return 1;
  }

  /*New polygon_formation tests*/ 
  if(test_new_polygon_formation(0,NULL)){
     std::cerr << "[test_new_polygon_formation] failed" << std::endl;
     return 1;
  }

  if(test_new_polygon_formation_marginal_threshold(0,NULL)){
     std::cerr << "[test_new_polygon_formation_marginal_threshold] failed" 
         << std::endl;
     return 1;
  }

  if (test_active_tail_formation()) {
    std::cerr << "[test_active_tail_formation] failed" << std::endl;
    return 1;
  }

  std::cout << "ALL TESTS COMPLETE\n";
  return 0;
}
