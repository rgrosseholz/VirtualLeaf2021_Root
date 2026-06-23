/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#include <QObject>
#include <QtGui>

#include "simplugin.h"

#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "root.h"

static const std::string _module_id("$Id$");

QString TwoCells::ModelID(void)
{
  // specify the name of your model here
  return QString("Root");
}

// return the number of chemicals your model uses
int TwoCells::NChem(void) { return 0; }

// To be executed after cell division
void TwoCells::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2)
{
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void TwoCells::SetCellColor(CellBase *c, QColor *color)
{
  // add cell coloring rules here
}

void TwoCells::CellHouseKeeping(CellBase *c)
{
  // add cell behavioral rules here
  
  // cellulose spring rules
  c->EnlargeTargetArea(par->cell_expansion_rate);

  if(!(c->isTrianglePlaced()) && c->Index() != -1 && c->CellType() == 0 ) // instead of celltype use k 
  {
    double width {0};
    c->Length(NULL, &width);
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width - 2.5, 3);
    // set mechanical properties
    c->setStiffnessMatrix( par->d * Matrix { Vector { par->e,par->f,0},
                   Vector { par->f,par->c,0}, Vector {0,0,par->mu }});
  }

  if(!(c->isTrianglePlaced()) && c->CellType() == 1 ) // instead of celltype use k 
  {
    double width {0};
    c->Length(NULL, &width);
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width - 2.5, 4);
    // set mechanical properties
    c->setStiffnessMatrix( par->k[0] * Matrix { Vector { par->k[1],par->k[2],0},
                   Vector { par->k[2],par->k[3],0}, Vector {0,0,par->k[4] }});
  }

  if(!(c->isTrianglePlaced()) && c->CellType() == 2 ) // instead of celltype use k 
  {
    double width {0};
    c->Length(NULL, &width);
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width - 2.5, 4);
    // set mechanical properties
    c->setStiffnessMatrix( par->k[5] * Matrix { Vector { par->k[6],par->k[7],0},
                   Vector { par->k[7],par->k[8],0}, Vector {0,0,par->k[9] }});
  }

  if(!(c->isTrianglePlaced()) && c->CellType() == 3 ) // instead of celltype use k 
  {
    double width {0};
    c->Length(NULL, &width);
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width - 2.5, 4);
    // set mechanical properties
    c->setStiffnessMatrix( par->k1 * Matrix { Vector { par->k2,par->r,0},
                   Vector { par->r,par->kr,0}, Vector {0,0,par->km }});
  }

  if ( ( c->Index() == 1 || c->Index() == 2 || c->Index() == 7 ||
        c->Index() == 12 || c->Index() == 13 || c->Index() == 14 ||
        c->Index() == 15 || c->Index() == 16 || c->Index() == 36 ||
        c->Index() == 41 || c->Index() == 35 ) && 
      c->Area() > par->rel_cell_div_threshold * c->BaseArea() 
      )
  {
		  c->Divide();
	}

    //cell wall weakening happens here
  if(par->k[0] == 0){

    c->LoopWallElements([](auto wallElementInfo){
      Vector from { *(wallElementInfo->getFrom()) };
      Vector to { *(wallElementInfo->getTo()) };
      Vector wallVector { to - from };
      Vector growthDirection { 0, 1};
      // if angle is between 75 - 105 degree return true
      if ( 1.3 < wallVector.Angle(growthDirection) &&  1.9 > wallVector.Angle(growthDirection) ) 
      { 
        wallElementInfo->getWallElement()->setStiffness(1.5);
      } else { 
        wallElementInfo->getWallElement()->setStiffness(1);
      }
    });
  }



}

void TwoCells::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2)
{
  // add biochemical transport rules here
}
void TwoCells::WallDynamics(Wall *w, double *dw1, double *dw2)
{
  // add biochemical networks for reactions occuring at walls here
}
void TwoCells::CellDynamics(CellBase *c, double *dchem)
{
  // add biochemical networks for intracellular reactions here
}

// Q_EXPORT_PLUGIN2(root, root)
