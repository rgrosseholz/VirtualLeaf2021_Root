#ifndef _TRIANGLES_H_
#define _TRIANGLES_H_

#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include <QDebug>

#include "assert.h"
#include "sqr.h"
#include "tiny.h"
#include "vector.h"
#include "wall.h"
#include "warning.h"


class CellBase;
class Node;
class Triangle{
    friend class Mesh;
    // m stands for class member
    Node* m_A;  // first node A of the triangle. Node A is the moving Node.
    Node* m_B;  // first node B of the triangle. Node B is opposing Node of Node A.
    Node* m_C;  // first node C of the triangle. Node C is Neighbor of Node B. 
    CellBase* m_cell;
    // defines target/reference triangle which is tried to be reached
    Vector m_target_BA; // target vector from node B to Node A
    Vector m_target_BC; // target vector from node B to Node C
    double m_target_theta; // target angle between BA and BC

public:
    Triangle(Node* nodeA, Node* nodeB,  Node* nodeC, CellBase* cell, Vector target_BA, Vector target_BC, double target_theta)
        : m_A { nodeA }, m_B { nodeB }, m_C { nodeC }, m_cell { cell }
            , m_target_BA { target_BA } , m_target_BC { target_BC}, m_target_theta { target_theta }
    {
    }
    Triangle(const Triangle& src)
        {

            m_A = src.m_A;
            m_B = src.m_B;
            m_C = src.m_C;
            m_cell = src.m_cell;
            m_target_BA = src.m_target_BA;
            m_target_BC = src.m_target_BC;
            m_target_theta = src.m_target_theta;
        }

    Node* getNodeA() { return m_A; };
    Node* getNodeB() { return m_B; };
    Node* getNodeC() { return m_C; };
    CellBase* getCell() { return m_cell; };

    Vector getTargetBA() { return m_target_BA;}
    Vector getTargetBC() { return m_target_BC;}
    double getTargetTheta() { return m_target_theta;}

    void setNodeA(Node* A) { m_A = A; };
    void setNodeB(Node* B) { m_B = B; };
    void setNodeC(Node* C) { m_C = C; };

    void setTargetBA(Vector BA) { m_target_BA = BA; };
    void setTargetBC(Vector BC) { m_target_BC = BC; };
    void setTargetTheta(double theta) {m_target_theta = theta; };
};
#endif

/* finis*/
