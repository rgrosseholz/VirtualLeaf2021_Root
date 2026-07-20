/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
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


#ifndef _NODESET_H_
#define _NODESET_H_

#include <algorithm>
#include <numeric>
#include <list>
#include <iterator>
#include "node.h"
#include "mesh.h"

class Mesh;
class NodeSet : public list<Node *> {

 public:
  NodeSet(void) {
    done = false;
  }

  inline bool DoneP(void) { return done; }
  inline void ResetDone(void) { done = false; }

  void AddNode(Node * n) {
    push_back(n);
    n->node_set = this;
  }

  list <Cell *> getCells(void) {

    list<Cell *> cellset;

    for (list<Node *>::const_iterator i=begin();
	 i!=end();
	 i++) {
      transform ( (*i)->owners.begin(), (*i)->owners.end(), back_inserter( cellset ) , mem_fn ( &Neighbor::getCell ));
    }

    cellset.sort();

    // remove all non-unique elements
    // (unique moves all unique elements to the front and returns an iterator to the end of the unique elements;
    // so we simply erase all remaining elements.
    cellset.erase(::unique(cellset.begin(), cellset.end() ), cellset.end() );

    // remove boundary_polygon
    cellset.erase( find_if ( cellset.begin(), cellset.end(), mem_fn( &Cell::BoundaryPolP  ) ) );
    return cellset;
  }

  void CleanUp(void) {

    // remove double Nodes from the set
    sort();
    erase(::unique(begin(), end() ), 
	  end() );


  }
  void print(ostream &os) const {
    transform( begin(), end(), ostream_iterator<int>( os, " " ), mem_fn( &Node::Index ) );
  }

  

  /*! Attempt a move over (rx, ry)
    reject if energetically unfavourable.
  */
  void AttemptMove(Mesh* m, double rx, double ry) {

    done = true;
    // 1. Collect list of all attached to the nodes in the set
    list<Cell *> celllist = getCells();

    // 2. Sum the current energy of these cells
    // implemented erngies: area energy, 1D and 2D cell wall energy
    double old_energy=0.;
    for ( list<Cell *>::const_iterator i = celllist.begin(); 
	  i!=celllist.end();
	  ++i ) {
      double length_energy {0};
      double cellulose_energy { 0 };
      for(auto node : (*i)->getNodesList()){
        //1D Cell Wall energy
        for( auto owner : node->owners){
          if( owner.CellEquals( (*i)->Index() ) ){
            length_energy += m->cellWallEnergy1D( *(*i), *node, *owner.get_nb1(), *owner.get_nb2() );
          }
        }
        //2D Cell Wall energy
        cellulose_energy += m->wallEnergy2D(*(*i), *node);
      }
      old_energy += (*i)->AreaEnergy() +  length_energy + cellulose_energy;
    }

    // 3. (Temporarily) move the set's nodes.
    for ( list<Node *>::iterator n = begin();
	  n!=end();
	  ++n ) {

      (*n)->x+=rx;
      (*n)->y+=ry;
      // (*n)->z += rz;
    }

    // 4. Recalculate the energy
    double new_energy=0.;
    for ( list<Cell *>::const_iterator i = celllist.begin(); 
	  i!=celllist.end();
	  ++i ) {
      double length_energy {0};
      double cellulose_energy { 0 };
      for(auto node : (*i)->getNodesList()){
        //1D Cell Wall energy
        for( auto owner : node->owners){
          if( owner.CellEquals( (*i)->Index() ) ){
            length_energy += m->cellWallEnergy1D( *(*i), *node, *owner.get_nb1(), *owner.get_nb2() );
          }
        }
        //2D Cell Wall energy
        cellulose_energy += m->wallEnergy2D(*(*i), *node);
      }
      new_energy += (*i)->AreaEnergy() +  length_energy + cellulose_energy;
    }


    // 5. Accept or Reject DeltaH
    double dh = new_energy - old_energy;

    list<int> new_areas;

    // cerr << "Nodeset says: dh = " << dh << " ...";
    if (RANDOM()<exp((-dh)/par.T) ) {

      // ACCEPT
      // recalculate areas of cells
      for_each ( celllist.begin(), celllist.end(), mem_fn ( &Cell::RecalcArea ) );

      // recalculate integrals of cells
      for_each ( celllist.begin(), celllist.end(), mem_fn ( &Cell::SetIntegrals ) );


    } else {
      // REJECT

      // Move the set's nodes back to their original position
      for ( list<Node *>::iterator n = begin();
	    n!= end();
	    ++n ) {

	(*n)->x-=rx;
	(*n)->y-=ry;
      }
    }
  }

  void XMLAdd(QDomDocument &doc, QDomElement &root) const;
  void XMLRead(QDomElement &root, Mesh *m);
 private:
  bool done;
};

ostream &operator<<(ostream &os, const NodeSet &ns);
#endif

/* finis */
