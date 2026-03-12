
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
 * @brief Calculates the strain matrix of a cellulose spring
 * 
 * @details The strain tensor is assuming small deformation where we map the
 *          actual node movement on spring movement relative to the baselength.
 *          In node notation we would habe smth like 
 *          strain_tensor = { Vector { dx, 1/2 (dx + dy)}, Vector { 1/2 (dx + dy)} }
 * 
 * @param rx used for entering -rx in displace node to get the strain before the
 *           node movement otherwise enter 0
 * 
 * @param ry same
 * 
 * @return returns strain matrix
 */
Matrix Spring::getCelluloseStrainMatrix(double rx, double ry)
{
    double springX { abs(getSpringVector().x + rx) }; // taking abs bc i want to consider only length not direction
    double springY { abs(getSpringVector().y + ry) };
    Vector baseLength { getSpringBaseLengthVector() };
    return Matrix { Vector { (springX - baseLength.x)/baseLength.Norm(),
                        0.5 * ( (springX - baseLength.x) + 0 /*(springY - baseLength.y)*/) / baseLength.Norm()  
                        , 0}, 
             Vector { 0.5 * ( (springX - baseLength.x) + 0 /*(springY - baseLength.y)*/) / baseLength.Norm() , 
                    0 /*(springY - baseLength.y)/baseLength.Norm()*/, 0},
             Vector {0, 0, 0} };
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
