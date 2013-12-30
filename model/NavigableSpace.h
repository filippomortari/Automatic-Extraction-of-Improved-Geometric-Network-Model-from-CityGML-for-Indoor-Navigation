#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __NavigableSpace_h__
#define __NavigableSpace_h__

#include "NonNavigableSpace.h"
#include "AbstractSpace.h"
#include "../CGALDefines.h"

enum FaceType {CROSSING, HOLENEIGHBOUR, DOORADJACENT, DEADEND};
class NavigableSpace;
struct FaceInfo2
{
  FaceInfo2(){}
  int nesting_level;
  list<FaceType> faceType;
  int id;
  NavigableSpace* mySpace;

  bool in_domain(){ 
    return nesting_level%2 == 1;
  }
};

typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;
typedef CDT::Face_circulator Face_circulator; 
typedef CDT::Vertex_iterator Vertex_iterator; 

class NonNavigableSpace;


class NavigableSpace: public AbstractSpace
{
public: 

	std::vector<NavigableSpace*> _subspaces;
	std::vector<NavigableSpace*> _neighbours;
	std::vector<NonNavigableSpace*> _touchedNonNavigableSpaces;

	CDT triangulatedSpaces;
	
	~NavigableSpace(void);
	void addNeighbour(NavigableSpace*);
	void foo(void);

};

#endif
