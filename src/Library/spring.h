

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
    //using personalized default initializer should in the end become some par variable
    double m_spring_base_length { 7 };
    double m_spring_stiffness { 20 };

public:
    Spring(Node* node1, Node* node2, CellBase* cell, double s_b_length = double {7} , double s_stiffness = 20)
        : m_n1 { node1 }, m_n2 { node2 }, m_c { cell },
         m_spring_base_length { s_b_length }, m_spring_stiffness { s_stiffness } 
    {
    }
    Spring(const Spring& src)
        {

            m_n1 = src.m_n1;
            m_n2 = src.m_n2;
            m_c = src.m_c;
            m_spring_base_length = src.m_spring_base_length;
            m_spring_stiffness = src.m_spring_stiffness;
        }

    Node* getNode1() { return m_n1; };
    Node* getNode2() { return m_n2; };
    
    void setSpringStiffness(double value) { m_spring_stiffness = value; }
    void setSpringBaseLength(double length = 8) 
    {
        m_spring_base_length = length; 
    }
    double getSpringStiffness() const { return m_spring_stiffness; }
    double getSpringBaseLength() const { return m_spring_base_length; }
    Vector getSpringBaseLengthVector() const { return m_spring_base_length; }
    Vector getSpringVector() // vector points from node 2 to node 1 
    {
        return Vector { m_n1->x - m_n2->x, m_n1->y - m_n2->y, m_n1->z - m_n2->z};
    }
    Vector getSpringVector(Vector rN1, Vector rN2) // vector points from node 2 to node 1 
    {
        return Vector { (m_n1->x + rN1.x) - (m_n2->x + rN2.x) ,
                        (m_n1->y + rN1.y) - (m_n2->y + rN2.y) ,
                        (m_n1->z + rN1.z) - (m_n2->z + rN2.z)};
    }
    /**
     * @brief Calculates the norm of the vector between the two nodes
     */
    double getSpringLength()
    {
        return sqrt(DSQR(m_n1->x - m_n2->x)+DSQR(m_n1->y - m_n2->y)+DSQR(m_n1->z - m_n2->z));
    }
    


    void updateSpringBaseLength( double elastic_limit = 1.15)
    {
        if( getSpringLength()/getSpringBaseLength() > elastic_limit )
        {
            setSpringBaseLength( getSpringBaseLength() * 1.1 );
        } else {
            setSpringBaseLength( 12 );
        }
    }

    Matrix getCelluloseStrainMatrix(double rx, double ry);

    bool checkSpringOrientation(double lowerAngleBound, double higherAngleBound,
                                 Vector ref_vec = Vector { 0, 1, 0});

};
#endif

/* finis*/
