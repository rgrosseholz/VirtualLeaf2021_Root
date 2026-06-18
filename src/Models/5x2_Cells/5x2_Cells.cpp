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
#include "5x2_Cells.h"
#include "node.h"

static const std::string _module_id("$Id$");

QString five_x_two_Cells::ModelID(void)
{
  // specify the name of your model here
  return QString("5x2_Cells");
}

// return the number of chemicals your model uses
int five_x_two_Cells::NChem(void) { return 0; }

// To be executed after cell division
void five_x_two_Cells::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2)
{
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void five_x_two_Cells::SetCellColor(CellBase *c, QColor *color)
{
  // add cell coloring rules here
  if( c->CellType() == 0 ){
    color->setRgb(0,0,0.8);
  }
  
  if( c->CellType() == 1 ){
    color->setRgb(0,0.8,0);
  }

  if( c->CellType() == 2 ){ 
    color->setRgb(0.8,0,0);
  }

} 

void five_x_two_Cells::CellHouseKeeping(CellBase *c)
{
   // add cell behavioral rules here
  if( c->CellType() == 1 ){
    double yCoordinate { (c->Centroid()).y };
    if ( yCoordinate < 120 ){
      c->SetCellType( 2 );
    }
  }

  if( c->CellType() == 0){
    c->EnlargeTargetArea(par->cell_expansion_rate * 0.14);
  }

  if( c->CellType() == 1){
    c->EnlargeTargetArea(par->cell_expansion_rate);
  } 

  if( c->CellType() == 2){
    c->EnlargeTargetArea(1.2 * par->cell_expansion_rate);
  }
  
  c->LoopWallElements([](auto wallElementInfo){
    Vector from { *(wallElementInfo->getFrom()) };
    Vector to { *(wallElementInfo->getTo()) };
    Vector wallVector { to - from };
    Vector growthDirection { 0, 1};
    // if angle is between 75 - 105 degree return true
    if ( 1.3 < wallVector.Angle(growthDirection) &&  1.9 > wallVector.Angle(growthDirection) ) 
    { 
      wallElementInfo->getWallElement()->setStiffness(1);
    } else { 
      wallElementInfo->getWallElement()->setStiffness(0.65);
    }
  });

    // cellulose activation
  if(!(c->isTrianglePlaced()) && c->Index() != -1)
  {
    double width {0};
    c->Length(NULL, &width);
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle(Vector { width - 1, 0 });
    c->setTriangles();
  } 
  
  //division
  if (c->Area() > par->rel_cell_div_threshold * c->BaseArea() && c->CellType() == 1) {
		c->Divide();
	}
}

void five_x_two_Cells::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2)
{
  // add biochemical transport rules here
}
void five_x_two_Cells::WallDynamics(Wall *w, double *dw1, double *dw2)
{
  // add biochemical networks for reactions occuring at walls here
}
void five_x_two_Cells::CellDynamics(CellBase *c, double *dchem)
{
  // add biochemical networks for intracellular reactions here
}

// Q_EXPORT_PLUGIN2(5x2_Cells, 5x2_Cells)
