/****************************************************************************
* compute scl in free products of cyclic groups (finite and infinite)
* By Alden Walker
* Implements a generalization of the algorithm described in "scl"
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <ctype.h>

#include "scylla_classes.h"
#include "rational.h"
#include "scylla_lp.h"










/*****************************************************************************
 * Make the list of group polygons and rectangles. 
 * ***************************************************************************/
void compute_group_polygons_and_rectangles(Chain &C, 
                                           InterfaceEdgeList &IEL,
                                           std::vector<GroupEdgeList> &GEL,
                                           std::vector<std::vector<GroupPolygon> > &GP,
                                           std::vector<std::vector<GroupRectangle> > &GR) {
  int i,j,k,m,n;
  int num_groups = (C.G)->num_groups();
  Multiset letter_selection;
  std::vector<Multiarc> regular_multiarcs;
  std::vector<Multiarc> inverse_multiarcs;
  Multiarc temp_marc;
  GroupRectangle temp_group_rect;
  GroupPolygon temp_group_poly;
  
  GP.resize(num_groups);
  GR.resize(num_groups);
  
  for (i=0; i<num_groups; i++) {
    
    regular_multiarcs.resize(0);
    inverse_multiarcs.resize(0);
    
    //get the regulars (non-inverse)
    if (C.regular_letters[i].size() > 0 && (C.G)->orders[i] > 0) {    
      temp_marc.letters.resize((C.G)->orders[i]-1);
      letter_selection = Multiset((C.G)->orders[i]-1, 0, C.regular_letters[i].size());
      do {
        for (j=0; j<(C.G)->orders[i]-1; j++) {
          temp_marc.letters[j] = C.regular_letters[i][letter_selection[j]];
        }
        regular_multiarcs.push_back(temp_marc);
      } while (1 != letter_selection.next());
    }
    
    //get the inverse multiarcs 
    if (C.inverse_letters[i].size() > 0 && (C.G)->orders[i] > 0) {
      temp_marc.letters.resize((C.G)->orders[i]-1);
      letter_selection = Multiset((C.G)->orders[i]-1, 0, C.inverse_letters[i].size());
      do {
        for (j=0; j<(C.G)->orders[i]-1; j++) {
          temp_marc.letters[j] = C.inverse_letters[i][letter_selection[j]];
        }
        inverse_multiarcs.push_back(temp_marc);
      } while (1 != letter_selection.next());    
    }
    
    //now assemble the group rectangles and group polygons
    //first, let's do the rectangles
    GR[i].resize(0);
    for (j=0; j<(int)C.regular_letters[i].size(); j++) {
      for (k=0; k<(int)C.inverse_letters[i].size(); k++) {
        temp_group_rect.first = IEL.get_index_from_group_side(C.regular_letters[i][j], 
                                                              C.inverse_letters[i][k]);
        temp_group_rect.last = IEL.get_index_from_group_side(C.inverse_letters[i][k], 
                                                             C.regular_letters[i][j]);
        GR[i].push_back(temp_group_rect);
      }
    }

    //now we do the group polygons 
    //these either have one or two multiarc sides.  first, the ones with a 
    //single multiarc side
    GP[i].resize(0);
    temp_group_poly.group = i;
    temp_group_poly.sides.resize(1);
    temp_group_poly.edges.resize(1);
    for (j=0; j<(int)regular_multiarcs.size(); j++) {
      temp_group_poly.sides[0] = regular_multiarcs[j];
      for (k=0; k<(int)GEL[i].regular_edges.size(); k++) {
        temp_group_poly.edges[0] = GEL[i].regular_edges[k];
        GP[i].push_back(temp_group_poly);
      }
    }
    for (j=0; j<(int)inverse_multiarcs.size(); j++) {
      temp_group_poly.sides[0] = inverse_multiarcs[j];
      for (k=0; k<(int)GEL[i].inverse_edges.size(); k++) {
        temp_group_poly.edges[0] = GEL[i].inverse_edges[k];
        GP[i].push_back(temp_group_poly);
      }
    }
    
    //now the ones with two multiarc sides
    temp_group_poly.sides.resize(2);
    temp_group_poly.edges.resize(2);
    for (j=0; j<(int)regular_multiarcs.size(); j++) { //first multiarc
      temp_group_poly.sides[0] = regular_multiarcs[j];
      for (k=0; k<(int)regular_multiarcs.size(); k++) { //second multiarc
        temp_group_poly.sides[1] = regular_multiarcs[k];
        for (m=0; m<(int)GEL[i].regular_edges.size(); m++) { //first side (between multiarc 0 and 1)
          temp_group_poly.edges[0] = GEL[i].regular_edges[m];
          for (n=0; n<(int)GEL[i].regular_edges.size(); n++) {
            temp_group_poly.edges[1] = GEL[i].regular_edges[n];
            GP[i].push_back(temp_group_poly);
          }
        }
      }
    }
    for (j=0; j<(int)inverse_multiarcs.size(); j++) { //first multiarc
      temp_group_poly.sides[0] = inverse_multiarcs[j];
      for (k=0; k<(int)inverse_multiarcs.size(); k++) { //second multiarc
        temp_group_poly.sides[1] = inverse_multiarcs[k];
        for (m=0; m<(int)GEL[i].inverse_edges.size(); m++) { //first side (between multiarc 0 and 1)
          temp_group_poly.edges[0] = GEL[i].inverse_edges[m];
          for (n=0; n<(int)GEL[i].inverse_edges.size(); n++) {
            temp_group_poly.edges[1] = GEL[i].inverse_edges[n];
            GP[i].push_back(temp_group_poly);
          }
        }
      }
    }
    
  }  
  
}




/*****************************************************************************
 * compute the list of polys. 
 *****************************************************************************/
void compute_central_polys(Chain &C, 
                           InterfaceEdgeList &IEL, 
                           CentralEdgeList &CEL,
                           std::vector<CentralPolygon> &CP) {
  int i,j,k;
  
  CentralPolygon temp_central_poly;
  
  CP.resize(0);
  temp_central_poly.edges.resize(4);
  temp_central_poly.interface.resize(4);
  
  //first, we go through everything with two central edges
  //for this, all we have to do is enumerate everything with 
  //2 interface edges, and that's it
  for (i=0; i<4; i++) {
    temp_central_poly.interface[i] = (i%2 == 0 ? true : false);
  }
  for (i=0; i<(int)IEL.edges.size(); i++) {
    temp_central_poly.edges[0] = i;
    for (j=0; j<(int)IEL.edges.size(); j++) {
      temp_central_poly.edges[2] = j;
      temp_central_poly.edges[1] = CEL.get_index( IEL[i].last, IEL[j].first );
      temp_central_poly.edges[3] = CEL.get_index( IEL[j].last, IEL[i].first );
      CP.push_back(temp_central_poly);
    }
  }
  
  //now, we go through all guys with 1 central edge
  //build chains of interface edges; whenever the length is 3, we tack on 
  //a central edge, and we're done
  std::vector<int> current_beginning_letters(3);    //this records the first letters of the interface edge choices
  std::vector<int> current_edges(3);                //this records where we are in the lists real_edges_beginning_with
  int current_len;                                  //this records the current length
  int temp_CE_1, temp_CE_2, temp_CE_3, temp_letter;
  current_len = 1;
  current_beginning_letters[0] = 0;
  current_edges[0] = 0;
  temp_central_poly.interface[0] = true;
  temp_central_poly.interface[1] = true;
  temp_central_poly.interface[2] = true;
  temp_central_poly.interface[3] = false;
  while (true) {   
    //for (i=0; i<current_len; i++) {
    //  std::cout << "(" << current_beginning_letters[i] << ", " << current_edges[i] << ") ";
    //}
    //std::cout << "\n";
    if (current_len == 3) {
      temp_CE_1 = IEL.edges_beginning_with[current_beginning_letters[current_len-1]][current_edges[current_len-1]];
      temp_CE_2 = IEL.edges_beginning_with[current_beginning_letters[0]][current_edges[0]];
      for (i=0; i<current_len; i++) {
        temp_central_poly.edges[i] = IEL.edges_beginning_with[current_beginning_letters[i]]
                                                             [current_edges[i]];
      }
      temp_central_poly.edges[3] = CEL.get_index(IEL[temp_CE_1].last, IEL[temp_CE_2].first);
      CP.push_back(temp_central_poly);
    }
    //now we advance it: if the list is shorter than maxmal (3), then add
    //one on.  otherwise, step back and leave it short
    if (current_len < 3) { 
      temp_CE_1 = IEL.edges_beginning_with[current_beginning_letters[current_len-1]]
                                          [current_edges[current_len-1]];
      temp_letter = C.next_letter( IEL[temp_CE_1].last );
      current_beginning_letters[current_len] = temp_letter;
      current_edges[current_len] = 0;
      current_len++;
      continue;
    }
    //if we're here, then the length is 3, so advance to the next one
    i = current_len-1;
    while (i>=0 && current_edges[i] == (int)IEL.edges_beginning_with[current_beginning_letters[i]].size()-1) {
      i--;
    }
    if (i==-1) {
      if (current_beginning_letters[0] == (int)C.chain_letters.size()-1) {
        break;
      } else {
        current_beginning_letters[0]++;
        current_edges[0] = 0;
        current_len = 1;
        continue;
      }
    } 
    current_edges[i]++;
    current_len = i+1;
  }
  
  
  //now we search for all interface-edge polygons
  current_len = 1;
  current_beginning_letters.resize(3);
  current_edges.resize(3);
  current_beginning_letters[0] = 0;
  current_edges[0] = 0;
  temp_central_poly.interface[0] = true;
  temp_central_poly.interface[1] = true;
  temp_central_poly.interface[2] = true;
  temp_central_poly.interface[3] = true;
  while (true) {     
    //for (i=0; i<current_len; i++) {
    //  std::cout << "(" << current_beginning_letters[i] << ", " << current_edges[i] << ") ";
    //}
    //std::cout << "\n";
    if (current_len == 3) {
      temp_CE_1 = IEL.edges_beginning_with[current_beginning_letters[current_len-1]]
                                          [current_edges[current_len-1]];
      temp_CE_2 = IEL.edges_beginning_with[current_beginning_letters[0]]
                                          [current_edges[0]];
      temp_CE_3 = IEL.get_index_from_poly_side(C.next_letter( IEL[temp_CE_1].last ),
                                               C.prev_letter( IEL[temp_CE_2].first ) );
      if (temp_CE_3 != -1) {
        for (i=0; i<current_len; i++) {
          temp_central_poly.edges[i] = IEL.edges_beginning_with[current_beginning_letters[i]]
                                                               [current_edges[i]];
        }
        temp_central_poly.edges[3] = temp_CE_3;
        CP.push_back(temp_central_poly);
      }
    }
    //now we advance it: if the list is shorter than maxmal (4), then add
    //one on.  otherwise, step back and leave it short
    if (current_len < 3) { 
      for (k=current_edges[current_len-1];
           k<(int)IEL.edges_beginning_with[current_beginning_letters[current_len-1]].size();
           k++) {
        temp_CE_1 = IEL.edges_beginning_with[current_beginning_letters[current_len-1]][k];
        temp_letter = C.next_letter( IEL[temp_CE_1].last );
        if (temp_letter > current_beginning_letters[0]) {
          break;
        }
      }
      if (k<(int)IEL.edges_beginning_with[current_beginning_letters[current_len-1]].size()) {
        current_beginning_letters[current_len] = temp_letter;
        current_edges[current_len] = 0;
        current_edges[current_len-1] = k;
        current_len++;
        continue;
      } else {
        current_edges[current_len-1] = k-1;
        //we need to advance to the next letter; the following code will do that
        //because we just set current_edges[current_len-1] to the last one
      }
    }
    //if we're here, then the length is 3, so advance to the next one
    i = current_len-1;
    while (i>=0 && current_edges[i] == (int)IEL.edges_beginning_with[current_beginning_letters[i]].size()-1) {
      i--;
    }
    if (i==-1) {
      if (current_beginning_letters[0] == (int)C.chain_letters.size()-1) {
        break;
      } else {
        current_beginning_letters[0]++;
        current_edges[0] = 0;
        current_len = 1;
        continue;
      }
    } 
    current_edges[i]++;
    current_len = i+1;
  }
  
}



void print_central_polys(std::vector<CentralPolygon> &CP, std::ostream &os) {
  int i,j;
  os << "Central polygons: (" << CP.size() << "):\n"; 
  for (i=0; i<(int)CP.size(); i++) {
    os << i << ": ";
    for (j=0; j<(int)CP[i].edges.size(); j++) {
      os << CP[i].edges[j];
      if (CP[i].interface[j]) {
        os << "i";
      } else {
        os << "c";
      }
      os << " ";
    }
    os << "\n";
  }
}

void print_group_rectangles_and_polys(std::vector<std::vector<GroupRectangle> > &GR,
                                      std::vector<std::vector<GroupPolygon> > &GP,
                                      std::ostream &os) {
  int i,j,k,l;
  for (i=0; i<(int)GP.size(); i++) {
    os << "Group " << i << " rectangles:\n";
    for (j=0; j<(int)GR[i].size(); j++) {
      os << j << ": " << GR[i][j].first << " " << GR[i][j].last << "\n";
    }
    os << "Group " << i << " polygons:\n";
    for (j=0; j<(int)GP[i].size(); j++) {
      os << j << ": ";
      for (k=0; k<(int)GP[i][j].sides.size(); k++) {
        os << "(";
        for (l=0; l<(int)GP[i][j].sides[k].letters.size(); l++) {
          os << GP[i][j].sides[k].letters[l] << " ";
        }
        os << ") " << GP[i][j].edges[k] << " ";
      }
      os << "\n";
    }
  }
}
      




int main(int argc, char* argv[]) {
  int current_arg = 1;
  int i;
  if (argc < 3 || std::string(argv[1]) == "-h") {
    std::cout << "usage: ./scyllop [-h] <gen string> <chain>\n";
    std::cout << "\twhere <gen string> is of the form <gen1><order1><gen2><order2>...\n";
    std::cout << "\te.g. a5b0 computes in Z/5Z * Z\n";
    std::cout << "\tand <chain> is an integer linear combination of words in the generators\n";
    std::cout << "\te.g. ./scyllop a5b0 aabaaaB\n";
    std::cout << "\t-h: print this message\n";
    exit(0);
  }
  while (argv[current_arg][0] == '-') {
    current_arg++;
    //handle arguments (none yet)
  }
  
  std::string G_in = std::string(argv[current_arg]);
  CyclicProduct G(G_in);                                                         //create the group
  current_arg++;
  
  Chain C(&G, &argv[current_arg], argc-current_arg);                              //process the chain argument

  std::cout << "Group: " << G << "\n";
  std::cout << "Chain: " << C << "\n";
  std::cout << "Letters:\n";
  C.print_letters(std::cout);
  
  std::cout << "Group letters:\n";
  C.print_group_letters(std::cout);
  
  CentralEdgeList CEL(C);
  CEL.print(std::cout);
  
  InterfaceEdgeList IEL(C);
  IEL.print(std::cout);
  
  std::vector<GroupEdgeList> GEL((C.G)->num_groups());
  for (i=0; i<(C.G)->num_groups(); i++) {
    GEL[i] = GroupEdgeList(C, i);
    GEL[i].print(std::cout);
  }
  
  std::vector<CentralPolygon> CP;
  compute_central_polys(C, IEL, CEL, CP);
  print_central_polys(CP, std::cout);
  
  std::cout << "printed\n";
  std::cout.flush();
  
  std::vector<std::vector<GroupRectangle> > GR;
  std::vector<std::vector<GroupPolygon> > GP;
  compute_group_polygons_and_rectangles(C, IEL, GEL, GP, GR);
  std::cout << "computed\n"; std::cout.flush();
  print_group_rectangles_and_polys(GR, GP, std::cout);
  
   
  rational scl;
  std::vector<rational> solution_vector(0);                           //run the LP
  scyllop_lp(C, GEL, IEL, CEL, GP, GR, CP, &scl, &solution_vector, GLPK_DOUBLE, true); 
  
  std::cout << "scl( " << C << ") = " << scl << " = " << scl.get_d() << "\n";    //output the answer
  
  return 0;
}
