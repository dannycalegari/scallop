#ifndef SCYLLA_LP_H
#define SCYLLA_LP_H

#include <vector>

#include "rational.h"
#include "scylla_classes.h"

enum scyllop_lp_solver {GLPK_DOUBLE, GLPK_EXACT, QSOPT_EXACT, EXLP};


void scyllop_lp(Chain& C, 
                std::vector<GroupEdgeList> &GEL, 
                InterfaceEdgeList &IEL ,
                CentralEdgeList &CEL, 
                std::vector<std::vector<GroupPolygon> > &GP,
                std::vector<std::vector<GroupRectangle> > &GR,
                std::vector<CentralPolygon> &CP,
                rational* scl, 
                std::vector<rational>* solution_vector, 
                scyllop_lp_solver solver, 
                bool VERBOSE); 

#endif 
