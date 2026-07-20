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
#include "rootLayout.h"

static const std::string _module_id("$Id$");

QString TwoCells::ModelID(void)
{
  // specify the name of your model here
  return QString("RootLayout");
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
  double targetArea = c->getTargetArea();
  double currentArea = c->Area();
  double maxLogDeviation = log10(par->mu);
  double areaDiff = log10(currentArea/targetArea);

  areaDiff = clamp(
    areaDiff,
    -maxLogDeviation,
    maxLogDeviation
  );
  double intensity = abs(areaDiff) / maxLogDeviation;

  int deviationChannel = static_cast<int>(255.0 * intensity);

  int greenChannel = 255 - deviationChannel;

  if (areaDiff < 0){
    color->setRgb(deviationChannel, greenChannel, 0);
  }
  else
  {
    color->setRgb(0, greenChannel, deviationChannel);
  }
}

void TwoCells::initializeTriangles(CellBase *c){
  // initialize cell properties depending on cell type
  if( c->Index() != -1 && c->CellType() == 0 ) 
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

    // depending on cell position, set growth rate parameter ( TargetLength used for this )
    double centroidY { c->Centroid().y };
    if ( centroidY < 115){
      c->SetTargetLength( 2 );
    }else if ( centroidY < 130){
      c->SetTargetLength( 1 );
    } else {
      c->SetTargetLength( 0 );
    }
  }

  if( c->CellType() == 1 ) 
  {
    double width {6};
    
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width * par->rho1, par->c0);
    // set mechanical properties
   c->setStiffnessMatrix( par->k[0] * Matrix { Vector { par->k[1],par->k[2],0},
                   Vector { par->k[2],par->k[3],0}, Vector {0,0,par->k[4] }});
                  
    c->SetCellVeto(true);
    double centroidY { c->Centroid().y };
    if ( centroidY < 115){
      c->SetTargetLength( 2 );
    }else if ( centroidY < 130){
      c->SetTargetLength( 1 );
    } else {
      c->SetTargetLength( 0 );
    }
  }

  if( c->CellType() == 2 ) 
  {
    double width {10};
    
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width * par->rho1, par->c0);
    // set mechanical properties
    c->setStiffnessMatrix( par->k[0] * Matrix { Vector { par->k[1],par->k[2],0},
                   Vector { par->k[2],par->k[3],0}, Vector {0,0,par->k[4] }});
                  
    c->SetCellVeto(true);
    double centroidY { c->Centroid().y };
    if ( centroidY < 115){
      c->SetTargetLength( 2 );
    }else if ( centroidY < 130){
      c->SetTargetLength( 1 );
    } else {
      c->SetTargetLength( 0 );
    }
  }


  if( c->CellType() == 3 ) 
  {
    double width {10};
    c->PlaceTriangles();
    c->setTargetVectorABofTriangle( Vector{width,0,0} );
    c->setTriangles(width * par->rho1, par->c0);
    // set mechanical properties
    c->setStiffnessMatrix( par->k[0] * Matrix { Vector { par->k[1],par->k[2],0},
                   Vector { par->k[2],par->k[3],0}, Vector {0,0,par->k[4] }});

    c->SetCellVeto(true);
    double centroidY { c->Centroid().y };
    if ( centroidY < 115){
      c->SetTargetLength( 2 );
    }else if ( centroidY < 130){
      c->SetTargetLength( 1 );
    } else {
      c->SetTargetLength( 0 );
    }
    
  }
}

void TwoCells::CellHouseKeeping(CellBase *c)
{
    
  // growth rates are calculated 
  /*if( (c->Index() == 47 || c->Index() == 30 || c->Index() == 37 || c->Index() == 45
    || c->Index() == 10 || c->Index() == 11 || c->Index() == 12 || c->Index() == 19 || c->Index() == 27 || c->Index() == 0
    || c->Index() == 29) && c->getTargetLength() != 3 )
  { c->EnlargeTargetArea(par->d);
    c->SetTargetLength(3);
  }
  */
  if(c->getTargetLength() == 0 && c->TargetArea() < par->f * c->Area()){
    c->EnlargeTargetArea(par->cell_expansion_rate * c->Area() );
  }
  if(c->getTargetLength() == 1 && c->TargetArea() < par->c * c->Area()){
    c->EnlargeTargetArea(par->cell_expansion_rate * c->Area() );
  }
  if(c->getTargetLength() == 2 && c->TargetArea() < par->mu * c->Area()){
    c->EnlargeTargetArea(par->cell_expansion_rate * c->Area() );
  }
  // Allow only lowest row  of cells to devide. The parameter pin_fixed is used therefore.
  if (false && c->getPin_fixed()  && 
      c->Area() > par->rel_cell_div_threshold * c->BaseArea() 
      )
  {
		  c->DivideOverAxis(Vector {1,0,0});
	}

    //cell wall weakening happens here
  if(true){

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
