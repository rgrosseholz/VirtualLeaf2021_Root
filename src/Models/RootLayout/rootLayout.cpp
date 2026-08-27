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
    
  }
}

void TwoCells::CellHouseKeeping(CellBase *c)
{

  //fix cell if final size is reached and belongs to the upper part of the tissue ( = targetLength > 0)
  if(c->CellType() == 0 &&  c->TargetArea() > 170 * par->nu && c->getTargetLength() > 0){
    //c->SetTargetLength(4);//stop growth
    c->Fix();
  }

  if(c->CellType() == 1 &&  c->TargetArea() > 115 * par->nu && c->getTargetLength() > 0){
    c->SetTargetLength(4);//stop growth
  }

  if(c->CellType() == 2 &&  c->TargetArea() > 190 * par->nu && c->getTargetLength() > 0){
    //c->SetTargetLength(4);//stop growth
    c->Fix();
  }

  if(c->CellType() == 3 && c->TargetArea() > 280 * par->nu && c->getTargetLength() > 0){
    //c->SetTargetLength(4);//stop growth
    c->Fix();
  }


  //update min and max cellcenter of tissue layout
  double centerY { c->Centroid().y };  
  // par->gamma = minimal cell center
  if( centerY < par->gamma ){
    par->gamma = centerY; 
  }
   // par->eps = maximal cell center
  if(centerY > par->eps){
    par->eps = centerY; 
  }
  double tissueDistance { par->eps - par->gamma };
  // set cell targetLength to adjust maximal allowed pressure in the cell 
  if( centerY > par->gamma + 0.8 * tissueDistance && c->getTargetLength() < 4){
    c->SetTargetLength(0);
  }
  if( centerY > par->gamma + 0.4 * tissueDistance && centerY < par->gamma + 0.8 * tissueDistance && c->getTargetLength() < 4){
    c->SetTargetLength(1);
  }
  if( centerY < par->gamma + 0.4 * tissueDistance && c->getTargetLength() < 4){
    c->SetTargetLength(2);
  }

  // enlarge target area depending on tissue location ( cell target length )
  // and only up to a certain pressure
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
  // Allow Cell division if area is x times larger then initial cell area
  if ( c->getPin_fixed()  && c->CellType() == 0 && 
      c->Area() > par->rel_cell_div_threshold * 72
      )
  {
		  c->DivideOverAxis(Vector {1,0,0});
	}

  if ( c->getPin_fixed()  && c->CellType() == 1 && 
      c->Area() > par->rel_cell_div_threshold * 50
      )
  {
		  c->DivideOverAxis(Vector {1,0,0});
	}

  if ( c->getPin_fixed()  && c->CellType() == 2 && 
      c->Area() > par->rel_cell_div_threshold * 80
      )
  {
		  c->DivideOverAxis(Vector {1,0,0});
	}

  if ( c->getPin_fixed()  && c->CellType() == 3 && 
      c->Area() > par->rel_cell_div_threshold * 80
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
