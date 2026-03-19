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

#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "tutorial1B.h"

static const std::string _module_id("$Id$");

QString Tutorial1B::ModelID(void) {
  // specify the name of your model here
  return QString( "1B: Cell growth and cell division" );
}

// return the number of chemicals your model uses
int Tutorial1B::NChem(void) { return 0; }

// To be executed after cell division
void Tutorial1B::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void Tutorial1B::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here

}

void Tutorial1B::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here
	c->EnlargeTargetArea(par->cell_expansion_rate/5);
	
  // cellulose spring activation
  if(par->k[0] == 0 && !(c->isSpringPlaced()) && c->Index()!=-1 ) // instead of celltype use k for no
  {
      c->PlaceSprings();
      c->SetSigmaSprings( 0.1 ); 
      c->SetSpringDistributionMean( 0 );
      c->SetSpringsNormalDistributedExcludeTopBottom();
      c->setSpringBaseLength(9);
      par->bend_lambda = 1;
      
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
        wallElementInfo->getWallElement()->setStiffness(1);
      } else { 
        wallElementInfo->getWallElement()->setStiffness(0.65);
      }
    });
  }

  //division
  if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
		c->Divide();
	}
}

void Tutorial1B::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void Tutorial1B::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void Tutorial1B::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(tutorial1B, Tutorial1B)
