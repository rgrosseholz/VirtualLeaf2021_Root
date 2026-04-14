

#ifndef _SPRINGS_H_
#define _SPRINGS_H_

#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include <QDebug>

#include "assert.h"
#include "matrix.h"
#include "parameter.h"
#include "sqr.h"
#include "tiny.h"
#include "vector.h"
#include "wall.h"
#include "warning.h"
#include "node.h"

class Spring{

    friend class Cell;
    friend class CellBase;
    friend class Mesh;
    // m stands for class member
    Node* m_n1;  // first node of the spring
    Node* m_n2;  // second node of the spring
    CellBase* m_c;

public:
    Spring(Node* node1, Node* node2, CellBase* cell)
        : m_n1 { node1 }, m_n2 { node2 }, m_c { cell }
    {
    }
    Spring(const Spring& src)
        {

            m_n1 = src.m_n1;
            m_n2 = src.m_n2;
            m_c = src.m_c;
        }

    Node* getNode1() { return m_n1; };
    Node* getNode2() { return m_n2; };

    Vector getSpringVector();

    Vector getSpringVector(Vector rN1, Vector rN2);
    
    /**
     * @brief Calculates the norm of the vector between the two nodes
     */
    double getSpringLength();

    bool checkSpringOrientation(double lowerAngleBound, double higherAngleBound,
                                 Vector ref_vec = Vector { 0, 1, 0});

};
#endif

/* finis*/
