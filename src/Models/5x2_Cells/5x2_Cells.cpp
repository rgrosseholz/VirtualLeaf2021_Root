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
}

void five_x_two_Cells::CellHouseKeeping(CellBase *c)
{
  // add cell behavioral rules here
  
  // cellulose spring rules
  c->EnlargeTargetArea(par->cell_expansion_rate/5);

  double base_element_length = 25;
  c->LoopWallElements([base_element_length](auto wallElementInfo)
                      {
        if(std::isnan(wallElementInfo->getWallElement()->getBaseLength())){
        wallElementInfo->getWallElement()->setBaseLength(base_element_length);
        } });


    //cell wall weakening happens here
    double patho_chem_level = c->Chemical(0) / (0.5);
    if (patho_chem_level > 1.2) {
        patho_chem_level = 1.2;
    }
    double stiffness_inf = 2.5;
    if(patho_chem_level>0.1 && c->CellType()!=2){
        c->SetCellVeto(false);
        stiffness_inf = 2.5 - (patho_chem_level);
    c->LoopWallElements([stiffness_inf](auto wallElementInfo){
        wallElementInfo->getWallElement()->setStiffness(stiffness_inf);
    });
    }
    else{
        c->LoopWallElements([stiffness_inf](auto wallElementInfo){
        wallElementInfo->getWallElement()->setStiffness(stiffness_inf);
        });
        c->SetCellVeto(true);
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
