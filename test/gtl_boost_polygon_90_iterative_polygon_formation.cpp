#include<cstdio>
#define BOOST_POLYGON_NO_DEPS
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/polygon_90_set_data_iterator.hpp>
#include "gtl_boost_polygon_90_unit_test_utils.h"

namespace gtl = boost::polygon;
using namespace gtl;
using namespace gtl::operators;

/**
 * test_half_edge_count: the test plan is to take a polygon_90_set_data 
 * and run a half edge iterator both VERTICAL and HORIZONTAL and 
 * count the number of half edges produced.
 */
static int test_half_edge_count() {

  int test_data[] = {2,6, 2,9,
   /*begin-hole*/ 4,9, 4,7, 6,7, 6,9, 2,9, /*end-hole*/
    2,10, 5,10, 5,12, 7,12, 7,7, 10,7, 10,3, 7,3, 7,6, 2,6};
  size_t tcount = sizeof(test_data)/(2*sizeof(int));

  polygon_90_set_data<int> pset(boost::polygon::VERTICAL);
  polygon_90_data<int> poly;

  CreateGTLPolygon(test_data, tcount, poly);
  pset.insert(poly);

  /** First check the VERTICAL orientation. */
  orthogonal_half_edge_iterator<int> hBegin = orthogonal_half_edge_iterator<int>::begin(pset);
  orthogonal_half_edge_iterator<int> hEnd = orthogonal_half_edge_iterator<int>::end(pset);
  orthogonal_half_edge_iterator<int> hItr;

  size_t hcount = 0, gold_count = 7;
  for (hItr = hBegin; hItr != hEnd; ++hItr) {
    ++hcount;
  }

  if (hcount != gold_count) {
    std::cerr << "[FAILED]" << std::endl;
  }

  std::cout << "[SUB-TEST] VERTICAL orientation passed\n" << std::endl;

  /** Check the iterator over HORIZONTAL orientation. */
  polygon_90_set_data<int> pset_h(boost::polygon::HORIZONTAL);
  pset_h.insert( pset );

  hItr.reset(pset_h);
  hBegin = orthogonal_half_edge_iterator<int>::begin(pset_h);
  hEnd = orthogonal_half_edge_iterator<int>::end(pset_h);

  hcount = 0;
  for (hItr = hBegin; hItr != hEnd; ++hItr) {
    ++hcount;
  }

  if (hcount != gold_count) {
    std::cerr << "[FAILED]" << std::endl;
  }

  std::cout << "[SUB-TEST] HORIZONTAL orientation passed\n" << std::endl;

  return !(hcount == gold_count);
} 

/**
 * test_half_edge_xor : the test plan is to
 * take a polygon_90_set_data and run a half edge iterator
 * both VERTICAL and HORIZONTAL and create a NEW
 * polygon_90_set_data and ensure that they are XOR clean.
 */
static int test_half_edge_xor() {
  int test_data[] = {2,6, 2,9,
   /*begin-hole*/ 4,9, 4,7, 6,7, 6,9, 2,9, /*end-hole*/
    2,10, 5,10, 5,12, 7,12, 7,7, 10,7, 10,3, 7,3, 7,6, 2,6};
  size_t tcount = sizeof(test_data)/(2*sizeof(int));

  polygon_90_set_data<int> pset(boost::polygon::VERTICAL);
  polygon_90_data<int> poly;

  CreateGTLPolygon(test_data, tcount, poly);
  pset.insert(poly);

  orthogonal_half_edge_iterator<int> hBegin = orthogonal_half_edge_iterator<int>::begin(pset);
  orthogonal_half_edge_iterator<int> hEnd = orthogonal_half_edge_iterator<int>::end(pset);
  orthogonal_half_edge_iterator<int> hItr;

  std::vector<orthogonal_half_edge<int> > hedges;
  for (hItr=hBegin; hItr != hEnd; ++hItr) {
    hedges.push_back( *hItr );
  }

  /** Now insert these edges into a new polygon_90_set_data. */
  polygon_90_set_data<int> pset_h(boost::polygon::HORIZONTAL);

  hItr.reset(pset);

  for (hItr = hBegin; hItr != hEnd; ++hItr) {
    orthogonal_half_edge<int>::insert(*hItr, pset_h, boost::polygon::VERTICAL);
  }

  pset_h ^= pset;
  polygon_90_set_data<int> xor_result = pset_h;

  if (!boost::polygon::empty(xor_result)) {
    std::cerr << "[XOR-SUBTEST] FAILED\n" << std::endl;
    return 1;
  }

  std::cout << "[XOR-SUBTEST] PASSED\n" << std::endl;
  return 0;
}


/** Main routine to run the tests in this module */
int test_orthogonal_half_edge_iterator() {
  int status = 0;

  status += test_half_edge_count();

  status += test_half_edge_xor();

  return status;
}
