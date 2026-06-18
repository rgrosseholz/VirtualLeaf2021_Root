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
#include "triangle.h"
/**
 * @brief returns target Vector for Vector A
 * @details Adds target distance x to B_x if(B_x < A_x), else substract it. 
 *          Adds target distance y to average of A_y and B_y if(B_x < A_x),
 *             else substracts it. 
 */

Vector Triangle::getTargetA(Vector targetAB ){
    double targetA_x;
    double targetA_y;
    if(m_A->x > m_B->x){
        targetA_x = m_B->x + targetAB.x;
        targetA_y =  m_B->y + targetAB.y;
    }else{
        targetA_x = m_B->x - targetAB.x;
        targetA_y = m_B->y - targetAB.y;
    }
    Vector targetA { targetA_x, targetA_y };
    return targetA;
}
/* finis*/
