#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <list>
#include <map>
#include <string>
#include "OgrExporter.h"

template<class K>
void print_point ( CGAL::Point_2<K> const& p )
{
    std::cout << "(" << p.x() << "," << p.y() << ")" ;
}

template<class K>
void print_polygon ( CGAL::Polygon_2<K> const& poly)
{
    typedef CGAL::Polygon_2<K> Polygon ;
    
    std::cout << "Polygon with " << poly.size() << " vertices" << std::endl ;
    
    for( typename Polygon::Vertex_const_iterator vi = poly.vertices_begin() ; vi != poly.vertices_end() ; ++ vi )
    {
		print_point(*vi); std::cout << std::endl ;
    }
}

template<class K>
void print_polygon_on_list ( CGAL::Polygon_2<K> const& poly, std::list<CGAL::Point_2<K> >* lista )
{
    typedef CGAL::Polygon_2<K> Polygon ;
    
    for( typename Polygon::Vertex_const_iterator vi = poly.vertices_begin() ; vi != poly.vertices_end() ; ++ vi )
    {
		print_point(*vi); std::cout << std::endl ;
		lista->push_back((*vi));
    }
}

template<class K>
void print_polygons ( std::vector< boost::shared_ptr< CGAL::Polygon_2<K> > > const& polies)
{
    typedef std::vector< boost::shared_ptr< CGAL::Polygon_2<K> > > PolygonVector ;
    
    std::cout << "Polygon list with " << polies.size() << " polygons" << std::endl ;
    
    for( typename PolygonVector::const_iterator pi = polies.begin() ; pi != polies.end() ; ++ pi ){
		
		print_polygon((**pi));
		
	}
    
    
}

template<class K>
void print_polygon_with_holes ( CGAL::Polygon_with_holes_2<K> const& polywh )
{
    typedef CGAL::Polygon_with_holes_2<K> PolygonWithHoles ;
    
    std::cout << "Polygon_with_holes having " << polywh.number_of_holes() << " holes" << std::endl ;
    
    print_polygon(polywh.outer_boundary());
    
    for( typename PolygonWithHoles::Hole_const_iterator hi = polywh.holes_begin() ; hi != polywh.holes_end() ; ++ hi )
		print_polygon(*hi);
}

template<class K>
void print_polygons_with_holes ( std::vector< boost::shared_ptr< CGAL::Polygon_with_holes_2<K> > > const& polies )
{
    typedef std::vector< boost::shared_ptr< CGAL::Polygon_with_holes_2<K> > > PolygonWithHolesVector ;
    
    std::cout << "Polygon_with_holes list with " << polies.size() << " element" << std::endl ;
    
    for( typename PolygonWithHolesVector::const_iterator pi = polies.begin() ; pi != polies.end() ; ++ pi )
		print_polygon_with_holes(**pi);
}

template<class K>
void print_straight_skeleton( CGAL::Straight_skeleton_2<K> const& ss )
{
    typedef CGAL::Straight_skeleton_2<K> Ss ;
    
    typedef typename Ss::Vertex_const_handle     Vertex_const_handle ;
    typedef typename Ss::Halfedge_const_handle   Halfedge_const_handle ;
    typedef typename Ss::Halfedge_const_iterator Halfedge_const_iterator ;
    
    Halfedge_const_handle null_halfedge ;
    Vertex_const_handle   null_vertex ;
    
    std::cout << "Straight skeleton with " << ss.size_of_vertices()
    << " vertices, " << ss.size_of_halfedges()
    << " halfedges and " << ss.size_of_faces()
    << " faces" << std::endl ;
    
    for ( Halfedge_const_iterator i = ss.halfedges_begin(); i != ss.halfedges_end(); ++i )
    {
		print_point(i->opposite()->vertex()->point()) ;
		std::cout << "->" ;
		print_point(i->vertex()->point());
		std::cout << " " << ( i->is_bisector() ? "bisector" : "contour" ) << std::endl;
    }
}

template<class K>
void print_polygon_on_file ( CGAL::Polygon_2<K> const& poly)
{
    //NODES
    string resultName;
    do{
        int random = rand() % 100;
        string fileName ("polygon_output");
        string extension(".txt");
        std::stringstream sstm;
        sstm << fileName <<"_"<< random << extension;
        resultName = sstm.str();
    }
    while(OgrExporter::fileExists(resultName));
    
    ofstream filePoly;
	filePoly.precision(16);
	filePoly.open (resultName.c_str());
    filePoly<<"x, y"<<endl;
    
    typedef CGAL::Polygon_2<K> Polygon ;
    
    std::cout << "Polygon with " << poly.size() << " vertices. File named " <<resultName<<"will be output"<<std::endl ;
    
    for( typename Polygon::Vertex_const_iterator vi = poly.vertices_begin() ; vi != poly.vertices_end() ; ++ vi )
    {
		print_point(*vi); std::cout << std::endl ;
        filePoly<<CGAL::to_double((*vi).x())<<", "<<CGAL::to_double((*vi).y())<<endl;
        
    }
    
    filePoly.close();
}