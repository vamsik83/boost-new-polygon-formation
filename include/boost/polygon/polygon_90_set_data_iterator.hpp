/**
 * polygon_90_set_data_iterator: infrastructure to support memory efficient polygon formation
 * on the polygon_90_set_data.
 *
 * History:
 *
 * Name                     : Date       : Description
 *
 * vamsi.krishnak@gmail.com : 05/25/2015 : created
 */

#ifndef BOOST_POLYGON_SET_DATA_ITERATOR_HPP
#define BOOST_POLYGON_SET_DATA_ITERATOR_HPP

#include "isotropy.hpp"
/**
 * polygon_90_set_data_iterator: an iterator interface to the Manhattan 
 * polygon formation code.
 */
namespace boost { namespace polygon {
 /** A vertical or horizonal oriented interval */
 template<typename T>
 class orthogonal_half_edge {
    private:
     T m_iBegin;
     T m_iEnd;
     T m_iLocation;
     bool m_bOrient; /** +ve or -ve (if ver/hor +ve == UP/LEFT and -ve == DOWN/RIGHT) */

    public:

    inline orthogonal_half_edge() : m_iBegin(std::numeric_limits<T>::max()),
        m_iEnd(std::numeric_limits<T>::min()), m_iLocation(std::numeric_limits<T>::max()),
        m_bOrient() {} 

    inline  bool valid(void) const {
        return m_iBegin <= m_iEnd;
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

    inline T end(void) const {
        return m_iEnd;
    }

    inline T location(void) const {
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
 };



 
}}
#endif
