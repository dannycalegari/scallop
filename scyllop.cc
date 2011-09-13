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

#include "scyllop_classes.h"
#include "rational.h"
#include "scyllop_lp.h"

/*****************************************************************************
 * make the list of multiarcs -- each multiarc is for a different group, and 
 * each one is made up of a collection (possibly including duplicates) of 
 * chunks, where the total number of letters is the order of the group.  
 * only the cyclic order in the multiarc matters.  
 * ***************************************************************************/
void compute_multiarcs(CyclicProduct &G, Chain &C, std::vector<std::vector<Multiarc> > &arcs) {
  std::vector<char> gens = G.gen_list();
  std::vector<int> orders = G.order_list();
  int i,j,k;
  int num_groups = gens.size();
  std::vector<std::vector<ChainChunk> > chain_chunks(0);
  std::vector<ChainChunk> group_chunks(0);
  std::string word;
  Multiset chunk_selection;
  ChainChunk temp;
  int first_index;
  
  arcs.resize(num_groups);
  
  for (i=0; i<num_groups; i++) {
    
    //create a list of chunks in that group
    //note that we already have a list of maximal chunks -- we only need to 
    //go through and get all the possible chunks
    group_chunks.resize(0);
    for (j=0; j<chain_chunks.size(); j++) {
      for (k=0; k<chain_chunks[j].size(); k++) {
        //
    
    //now go through and create all possible multiarcs; we may assume that the
    //smallest index is first
    arcs[i].resize(0);
    for (first_index=0; first_index<(int)group_chunks.size(); first_index++) {
      chunk_selection = Multiset(orders[i], first_index, group_chunks.size());
      do {
        //try this chunk slection
        
      } while (1 != chunk_selection.next());
    }
    
  }

} 




void compute_polys(CyclicProduct &G, 
                   Chain &C, 
                   std::vector<std::vector<Multiarc> > &arcs, 
                   std::vector<Polygon> &polys) {
  polys.resize(0);
  
}


int main(int argc, char* argv[]) {
  int current_arg = 1;
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
  
  Chain C(&G, &argv[current_arg], argc-current_arg);                              //process the chain arguments
  
  std::vector<std::vector<Multiarc> > arcs(0);
  std::vector<Polygon> polys(0);

  std::cout << "Group: " << G << "\n";
  std::cout << "Chain: " << C << "\n";
  C.print_chunks(std::cout);
  std::cout.flush();
  
  //compute_multiarcs(G, C, arcs);                                                 //calls arcs and polys by reference
  compute_polys(G, C, arcs, polys);
  
  rational scl;
  std::vector<rational> solution_vector(polys.size());                           //run the LP
  scyllop_lp(G, C, arcs, polys, &scl, &solution_vector, GLPK_DOUBLE, 0); 
  
  std::cout << "scl( " << C << ") = " << scl << " = " << scl.get_d() << "\n";    //output the answer
  
  return 0;
}
