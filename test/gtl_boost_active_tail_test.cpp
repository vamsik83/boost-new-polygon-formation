#include <cstdio>
#define BOOST_POLYGON_NO_DEPS
#include <boost/polygon/polygon.hpp>
#include "gtl_boost_polygon_90_unit_test_utils.h"
using namespace boost::polygon::polygon_formation;
using namespace boost::polygon;
/**
 * Unit test for ActiveTail:
 *
 *    +----+
 *    |    |
 *    ^    v 
 *    | +--+
 *  +-+ |
 *  |   |
 *  ^   v 
 *  |   |
 *  +-+ +--+
 *    ^    v 
 *    +----+
 * ActiveTail is a compact coordinate representation of Manhattan
 * polygons. If the underlying polyline represents a solid boundary
 * then the ActiveTail should have VERTICAL orientation on the other
 * hand if its a hole boundary then it should have HORIZONTAL 
 * orientation.
 *
 * The test simply tries to create the polygon directly using active
 * tail infrastructure of pushCoordinate and does an XOR check with
 * the existing polygon formation code.
 */
int test_active_tail_formation() {
  typedef std::pair<ActiveTail<int>*, ActiveTail<int>*> 
    TailPair;

  int test_data[] = {2,4, 2,7, 3,7, 3,10, 
    7,10, 7,8, 5,8, 5,4, 7,4, 7,3, 3,3, 3,4};

  size_t tcount = sizeof(test_data)/(2*sizeof(int));
  polygon_90_data<int> gold;
  CreateGTLPolygon(test_data, tcount, gold);

  /** Sweepline at x=2 */
  TailPair p = createActiveTailsAsPair<int>(
      test_data[0], test_data[1], true, NULL, true); 

  ActiveTail<int>* head = p.first;
  ActiveTail<int>* tail = p.second;


  /********************************************************
   * SubTest1: construct polygon by just growing the tail.
   ********************************************************/
  tail->pushCoordinate(3); //x
  tail->pushCoordinate(3); //y
  tail->pushCoordinate(7);
  tail->pushCoordinate(4);
  tail->pushCoordinate(5);
  tail->pushCoordinate(8);
  tail->pushCoordinate(7);
  tail->pushCoordinate(10);
  tail->pushCoordinate(3);
  tail->pushCoordinate(7);
  (tail->getTail())->joinTailToTail(*(head->getTail()));

  PolyLineHoleData<true, int> figure(head);
  PolyLineHoleData<true, int>::iterator_type itr;

  for(itr = figure.begin(); itr != figure.end(); ++itr) {
    point_data<int> pt = *itr;
    printf("(%d,%d) \n", pt.x(), pt.y());
  }

  polygon_90_data<int> result;
  assign(result, figure);

  if (!CheckManhattanPolygonEquality(result, gold)) {
    fprintf(stderr, "[FAILED] SubTest1 failed\n");
    return 1;
  } else {
    printf("[PASSED] SubTest1\n");
  }
  tail->destroyContents();
  /********************************************************/


  /********************************************************
   * SubTest2: construct polygon by joining two ActiveTail
   * pairs.
   *
   * 1. The first pair which bounds a solid starts growing
   * at (2,4).
   * 2. The second pair which bounds a hole starts growing
   * at (5,4).
   *
   * Both these pairs get joined at x=7.
   *
   * TODO(vamsi.krishnak@gmail.com): fix this test!.
   ********************************************************/

  TailPair p1 = createActiveTailsAsPair<int>(2,4, true, 
      NULL, true);
  TailPair p2 = createActiveTailsAsPair<int>(5,4, false,
      NULL, true);
  ActiveTail<int>* head1 = p1.first;
  ActiveTail<int>* tail1 = p1.second;
  ActiveTail<int>* head2 = p2.first;
  ActiveTail<int>* tail2 = p2.second;

  tail1->pushCoordinate(3);
  head1->pushCoordinate(7);
  head1->pushCoordinate(3);
  tail1->pushCoordinate(3);
  tail1->pushCoordinate(7);
  head1->pushCoordinate(10);
  head1->pushCoordinate(7);


  head2->pushCoordinate(8);
  tail2->pushCoordinate(7);
  head2->pushCoordinate(7);

  (head1->getTail())->joinTailToHead(*(tail2->getTail()));

  PolyLineHoleData<true, int> figure1(head2);

  for(itr = figure1.begin(); itr != figure1.end(); ++itr) {
    point_data<int> pt = *itr;
    printf("(%d,%d) \n", pt.x(), pt.y());
  }
  //TODO(fix this test).
  return 0;
}

