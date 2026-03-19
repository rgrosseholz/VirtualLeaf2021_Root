
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
