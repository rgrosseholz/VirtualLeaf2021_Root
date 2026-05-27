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
    

public:
    Triangle(Node* nodeA, Node* nodeB,  Node* nodeC, CellBase* cell)
        : m_A { nodeA }, m_B { nodeB }, m_C { nodeC }, m_cell { cell }
    {
    }
    Triangle(const Triangle& src)
        {

            m_A = src.m_A;
            m_B = src.m_B;
            m_C = src.m_C;
            m_cell = src.m_cell;
        }

    Node* getNodeA() { return m_A; };
    Node* getNodeB() { return m_B; };
    Node* getNodeC() { return m_C; };
    CellBase* getCell() { return m_cell; };

    Vector getTargetA(double targetDistanceAB);

    void setNodeA(Node* A) { m_A = A; };
    void setNodeB(Node* B) { m_B = B; };
    void setNodeC(Node* C) { m_C = C; };
};
#endif

/* finis*/
