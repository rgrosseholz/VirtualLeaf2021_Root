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

Vector Triangle::getTargetA(double targetDistanceAB){
    double targetA_x;
    if(m_A->x-m_B->x>0){
        targetA_x = m_B->x + targetDistanceAB;
    }else{
        targetA_x = m_B->x - targetDistanceAB;
    }
    Vector targetA { targetA_x, 0.5*(m_A->y + m_B->y) };
    return targetA;
}
/* finis*/
