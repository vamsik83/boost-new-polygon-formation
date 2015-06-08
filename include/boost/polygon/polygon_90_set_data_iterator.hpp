/**
 * polygon_90_set_data_iterator: infrastructure to support memory efficient polygon formation
 * on the polygon_90_set_data.
 *
 * History:
 * Name                     : Date       : Description
 * vamsi.krishnak@gmail.com : 05/25/2015 : created
 */

#ifndef BOOST_POLYGON_SET_DATA_ITERATOR_HPP
#define BOOST_POLYGON_SET_DATA_ITERATOR_HPP

#include "isotropy.hpp"
/**
 * polygon_90_set_data_iterator: an iterator interface to the Manhattan 
 * polygon formation code.
 *
 * <p>
 * polygon_90_set_data internal representation:
 * --------------------------------------------
 * internally a Manhattan polygon 'P' inside the 'polygon_90_set_data' is stored as a 
 * sequence of '(x_i, y_i, {+1,-1})' tuples. If 'P' has 'n' vertices the total space
 * to store the data is '4*n + 2*n*sizeof(Unit)'. E.g. in the case of 
 * 'polygon_90_set_data<int>' we need a total of '12*n' bytes. This seems space
 * inefficient -- which rightfully is. However the only reason on why this represenation
 * is chosen is to give a choice to the user on the orientation (horizonal/vertical) 
 * for polygon formation. The following example makes it clear.
 *
 * Consider a unit-square at origin is stored inside polygon_90_set_data, it consists 
 * of the following four vertices. 
 *
 *    [b] (0,1,-1)           [c] (1,1,+1)
 *
 *
 *    [a] (0,0,+1)           [d] (1,0,-1)
 *
 * Now we could view the data as a set of VERTICAL halfedges or HORIZONTAL halfedges.
 * In the case of VERTICAL halfedges { (a-->b) (c-->d)} we view the data as follows:
 *
 *    [b] (0,1,-1)           [c] (1,1,+1)
 *     ^                      |
 *     |                      |
 *     |                      v
 *    [a] (0,0,+1)           [d] (1,0,-1)
 *
 * In the case of HORIZONTAL halfedges {(d-->a) (b-->c)}. Notice that we switch between
 * choosing VERTICAL or HORIZONTAL by simply changing our sorting key (either X-coordinate
 * or Y-coordinate).
 * </p>
 */
namespace boost { namespace polygon {
 /** orthogonal_half_edge: A vertical or horizonal oriented interval. */
 template<typename T>
 class orthogonal_half_edge {
    private:
     T m_iBegin;
     T m_iEnd;
     T m_iLocation;
     /** +ve or -ve (if ver/hor +ve == UP/LEFT and -ve == DOWN/RIGHT). */
     bool m_bOrient; 

    public:

    inline orthogonal_half_edge() : m_iBegin(std::numeric_limits<T>::max()),
        m_iEnd(std::numeric_limits<T>::min()), m_iLocation(std::numeric_limits<T>::max()),
        m_bOrient() {} 

    inline  bool valid(void) const {
        return m_iBegin <= m_iEnd;
    }

    inline void invalidate() {
      m_iEnd = std::numeric_limits<T>::min();
      m_iBegin = std::numeric_limits<T>::max();
    }

    inline const orthogonal_half_edge<T>& set(T begin, T end, T location, 
            bool dir=true) {
        m_iBegin = begin; m_iEnd = end;
        m_iLocation = location; m_bOrient = dir;
        return *this;
    }

    inline T begin(void) const {
      return m_iBegin;
    }

    inline T& begin(void) {
      return m_iBegin;
    }

    inline T& end(void) {
      return m_iEnd;
    }

    inline T end(void) const {
      return m_iEnd;
    }

    inline bool orientation() const {
      return m_bOrient;
    }

    inline void set_orientation(bool o) {
      m_bOrient = o;
    }

    inline T location(void) const {
        return m_iLocation;
    }

    inline T& location(void) {
      return m_iLocation;
    }

    inline bool operator==(const orthogonal_half_edge& o) {
        return m_iBegin == o.m_iBegin && m_iEnd == o.m_iEnd &&
            m_iLocation == o.m_iLocation && m_bOrient == o.m_bOrient;
    }

    inline bool operator<(const orthogonal_half_edge<T>& o) const {
        return m_iLocation == o.m_iLocation ? m_iBegin < o.m_iBegin :
            m_iLocation < o.m_iLocation;
    }

    inline void print(void) const {
        std::cout << "[ " << m_iBegin << "," << m_iEnd << "]@" << m_iLocation << std::endl; ;
    }

    /**
     * Static method to insert an orthogonal half edge into a polygon_90_set_data. The
     * orient tells if this half edge is VERTICAL or HORIZONTAL.
     */

    static inline void insert(const orthogonal_half_edge<T>& edge,
        polygon_90_set_data<T>& pset, orientation_2d orient) {
      int sign = edge.orientation() ? 1 : -1;
      std::pair<T, std::pair<T, int> > vertex;
      vertex.first = edge.location();
      vertex.second.first = edge.begin();
      vertex.second.second = sign;

      pset.insert(vertex, false, orient);

      vertex.second.first = edge.end();
      vertex.second.second = -sign;

      pset.insert(vertex, false, orient);
    }
 };

 /**
  * orthogonal_half_edge_iterator: is a VIEW over polygon_90_set_data which gives us
  * an half_edge view of the underlying data inside polygon_90_set_data.
  *
  * NOTE: using this view can clean the underlying polygon_90_set_data.
  */
 template<typename T>
 class orthogonal_half_edge_iterator {
   typedef typename polygon_90_set_data<T>::iterator_type pset_iterator_type;

   private:
   polygon_90_set_data<T> *m_pPolygonSet;
   pset_iterator_type m_clBegin;
   pset_iterator_type m_clEnd;
   orthogonal_half_edge<T> m_clTempEdge;

   /** Fill the next valid half edge from the pset. */
   inline void fill_next_edge(void) {
     m_clTempEdge.invalidate();
     if (m_clBegin != m_clEnd) {

       m_clTempEdge.location() = (*m_clBegin).first;
       m_clTempEdge.begin() = (*m_clBegin).second.first;
       int sign = (*m_clBegin).second.second;

       ++m_clBegin;

       /** polygon_90_set_data is always even in size. */
       assert( m_clBegin != m_clEnd ); 
       assert( m_clTempEdge.location() == (*m_clBegin).first );
       assert( sign == -1*((*m_clBegin).second.second) );

       m_clTempEdge.end() = (*m_clBegin).second.first; 
       m_clTempEdge.set_orientation(sign == 1);

       ++m_clBegin;
     }
   }

   inline void init() {
      if (m_pPolygonSet) {
        m_pPolygonSet->clean();
        m_clBegin = m_pPolygonSet->begin();
        m_clEnd = m_pPolygonSet->end();
      }
   }

   public:

   inline orthogonal_half_edge_iterator(polygon_90_set_data<T> &pset)
       : m_pPolygonSet(&pset), m_clBegin(), m_clEnd()  { init(); }

   inline orthogonal_half_edge_iterator(polygon_90_set_data<T> &pset,
       pset_iterator_type beg, pset_iterator_type end) : m_pPolygonSet(&pset),
          m_clBegin(beg) , m_clEnd(end) {}

   inline orthogonal_half_edge_iterator() : m_pPolygonSet(NULL), m_clBegin(), m_clEnd(), m_clTempEdge() {}

   inline orthogonal_half_edge_iterator(const orthogonal_half_edge_iterator& o) :
     m_pPolygonSet(o.m_pPolygonSet), m_clBegin(o.m_clBegin), m_clEnd(o.m_clEnd), m_clTempEdge() {}

   inline orthogonal_half_edge_iterator& operator=(const orthogonal_half_edge_iterator& o) {
     if (this != &o) {
       m_pPolygonSet = o.m_pPolygonSet;
       m_clBegin = o.m_clBegin;
       m_clEnd = o.m_clEnd;
     }
     return *this;
   }

   inline bool operator==(const orthogonal_half_edge_iterator& o) {
     return (m_pPolygonSet == o.m_pPolygonSet) && (m_clBegin == o.m_clBegin)
       && (m_clEnd == o.m_clEnd);
   }

   inline bool operator!=(const orthogonal_half_edge_iterator& o) {
     return !(*this == o);
   }

   inline orthogonal_half_edge_iterator& operator++(void) {
     this->fill_next_edge();
     return *this;
   }

   inline const orthogonal_half_edge<T>& operator*(void) {
     return m_clTempEdge;
   }

   inline void reset(polygon_90_set_data<T> &pset) {
       m_pPolygonSet = &pset;
       init();
   }

   /*** Static methods to construct an half edge iterator from polygon_90_set_data. */
   inline static orthogonal_half_edge_iterator<T> begin(polygon_90_set_data<T>& input) {
     return orthogonal_half_edge_iterator<T>(input);
   }

   inline static orthogonal_half_edge_iterator<T> end(polygon_90_set_data<T>& input) {
     return orthogonal_half_edge_iterator<T>(input, input.end(), input.end());
   }
 };

}}
#endif
