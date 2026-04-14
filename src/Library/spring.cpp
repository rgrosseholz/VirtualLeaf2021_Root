
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

#include "cellbase.h"
#include "mesh.h"
#include "random.h"
#include "pi.h"
#include "spring.h"

Vector Spring::getSpringVector() // vector points from node 2 to node 1 
    {
        return Vector { m_n1->x - m_n2->x, m_n1->y - m_n2->y, m_n1->z - m_n2->z};
    }

Vector Spring::getSpringVector(Vector rN1, Vector rN2) // vector points from node 2 to node 1 
    {
        return Vector { (m_n1->x + rN1.x) - (m_n2->x + rN2.x) ,
                        (m_n1->y + rN1.y) - (m_n2->y + rN2.y) ,
                        (m_n1->z + rN1.z) - (m_n2->z + rN2.z)};
    }

/**
 * @brief Calculates the norm of the vector between the two nodes
 */
double Spring::getSpringLength()
    {
        return sqrt(DSQR(m_n1->x - m_n2->x)+DSQR(m_n1->y - m_n2->y)+DSQR(m_n1->z - m_n2->z));
    }


/**
 * @brief checks if the spring vector lies within an angle bound relativ to a reference vector
 *         
 * @details the calculated angle lies within [0,pi]
 */
bool Spring::checkSpringOrientation(double lowerAngleBound, double higherAngleBound,
                                         Vector ref_vec)
{
    Vector springVector { getSpringVector() };
    double angle { ref_vec.Angle(getSpringVector()) };
    return (angle > lowerAngleBound) && (angle < higherAngleBound);
}

/* finis*/
