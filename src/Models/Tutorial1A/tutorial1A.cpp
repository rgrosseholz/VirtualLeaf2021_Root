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
#include "matrix.h"
#include "wallbase.h"
#include "cellbase.h"
#include "tutorial1A.h"

static const std::string _module_id("$Id$");

QString Tutorial1A::ModelID(void) {
  // specify the name of your model here
  return QString( "1A: Cell growth" );
}

// return the number of chemicals your model uses
int Tutorial1A::NChem(void) { return 0; }

// To be executed after cell division
void Tutorial1A::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void Tutorial1A::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
  // add cell coloring rules here
  double targetArea = c->getTargetArea();
  double currentArea = c->Area();
  double maxLogDeviation = log10(par->mu);
  double areaDiff = log10(currentArea/targetArea);

  areaDiff = clamp(
    areaDiff,
    -maxLogDeviation,
    maxLogDeviation
  );

  double deviationRatio = 0.0;
  if (maxLogDeviation > 0) {
    deviationRatio = areaDiff / maxLogDeviation;
  }
  deviationRatio = clamp(deviationRatio, -1.0, 1.0);

  double intensity = abs(deviationRatio);
  int saturation = static_cast<int>(150 + 105.0 * intensity);
  int value = static_cast<int>(255.0 - 120.0 * intensity);

  if (deviationRatio < 0) {
    *color = QColor::fromHsv(30, saturation, value); // orange-ish for smaller-than-target
  }
  else {
    *color = QColor::fromHsv(210, saturation, value); //  blue-ish for larger-than-target
  }
}

void Tutorial1A::initializeTriangles(CellBase *c){
  // initialize cell properties depending on cell type
  if( c->Index() != -1 ) 
  {
    double width {6}; //  width of the cell 
    // set triangles
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width * par->rho1, par->c0);
    // set mechanical properties
    c->setStiffnessMatrix( par->k[0] * Matrix { Vector { par->k[1],par->k[2],0},
                   Vector { par->k[2],par->k[3],0}, Vector {0,0,par->k[4] }});
    // set cell wall remodelling veto              
    c->SetCellVeto(true);

  }
}

void Tutorial1A::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

// enlarge target area, par f controls the maximum pressure difference 
  if(c->TargetArea() < par->f * c->Area()){
    c->EnlargeTargetArea(par->cell_expansion_rate  );
  }

  double base_element_length = 25;
  c->LoopWallElements([base_element_length](auto wallElementInfo)
                      {
        if(std::isnan(wallElementInfo->getWallElement()->getBaseLength())){
        wallElementInfo->getWallElement()->setBaseLength(base_element_length);
        } });


  //cell wall weakening happens here
  if(c->CellType() == 0){

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
}

void Tutorial1A::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void Tutorial1A::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void Tutorial1A::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(tutorial1A, Tutorial1A)
