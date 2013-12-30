#include "OgrExporter.h"
#include <GDAL/ogrsf_frmts.h>
#include "topologicalModel/TopologicalDataStructure.h"
#include <string>
#include <stdlib.h>
#include "CGALDefines.h"
#include "print.h"
#include <sys/stat.h>





OgrExporter::OgrExporter(void)
{
}


OgrExporter::~OgrExporter(void)
{
}


void OgrExporter::toShapeFile(TopologicalDataStructure * myTopologicalDataStructure)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	OGRDataSource *poDS;

	poDS = poDriver->CreateDataSource( "area_out.shp", NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "point_out", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "Edge_name", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;


	for (map<int,list<Point_3> >::iterator it=myTopologicalDataStructure->extrudedEdges.begin(); it!=myTopologicalDataStructure->extrudedEdges.end();++it)
	{

		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "Edge_name", (*it).first);

		OGRPolygon myPoly; OGRLinearRing myRing;

		list<Point_3> tmpPoints = (*it).second;

		for (list<Point_3>::iterator pointIt = tmpPoints.begin(); pointIt!= tmpPoints.end(); ++pointIt){
			x = CGAL::to_double((*pointIt).x());
			y = CGAL::to_double((*pointIt).y());

			myRing.addPoint(x,y);
		}

		myPoly.addRing(&myRing);
		poFeature->SetGeometry( &myPoly ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );

	}

	OGRDataSource::DestroyDataSource( poDS );
}

void OgrExporter::vertexToShapeFile(TopologicalDataStructure * myTopologicalDataStructure)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	OGRDataSource *poDS;

	poDS = poDriver->CreateDataSource( "point_out.shp", NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "point_out", NULL, wkbPoint, NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "Name", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;

	for (std::map<int,node>::iterator it=myTopologicalDataStructure->nodes.begin(); it!=myTopologicalDataStructure->nodes.end();++it)
	{
		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "Name", (*it).second.getId() );

		OGRPoint pt;

		x = CGAL::to_double((*it).second.geometry.x());
		y = CGAL::to_double((*it).second.geometry.y());
		pt.setX( x );
		pt.setY( y );

		poFeature->SetGeometry( &pt ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );
	}

	OGRDataSource::DestroyDataSource( poDS );
}


void OgrExporter::doorToShapeFile(TopologicalDataStructure * myTopologicalDataStructure)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	OGRDataSource *poDS;

	poDS = poDriver->CreateDataSource( "door_out.shp", NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "point_out", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "Door_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;

	for (map<int,list<Point_3> >::iterator it=myTopologicalDataStructure->extrudedDoors.begin(); it!=myTopologicalDataStructure->extrudedDoors.end();++it)
	{

		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "Door_id", (*it).first);

		OGRPolygon myPoly; OGRLinearRing myRing;

		list<Point_3> tmpPoints = (*it).second;

		for (list<Point_3>::iterator pointIt = tmpPoints.begin(); pointIt!= tmpPoints.end(); ++pointIt){
			x = CGAL::to_double((*pointIt).x());
			y = CGAL::to_double((*pointIt).y());

			myRing.addPoint(x,y);
		}

		myPoly.addRing(&myRing);
		poFeature->SetGeometry( &myPoly ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );

	}

	OGRDataSource::DestroyDataSource( poDS );
}

void OgrExporter::drawPolygonsModel(vector<AbstractSpace*> building)
{

	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("Building");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();
	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "rooms", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
	Point_2 tmPoint;
	for (vector<AbstractSpace*>::iterator it=building.begin(); it!=building.end(); ++it)
	{

		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "space_id", (*it)->id);

		OGRPolygon myPoly; OGRLinearRing myRing;


		for (VertexIterator vi = (*it)->geometry.vertices_begin(); vi!= (*it)->geometry.vertices_end(); ++vi){
			tmPoint = *vi;
			x = CGAL::to_double(tmPoint.x());
			y = CGAL::to_double(tmPoint.y());

			myRing.addPoint(x,y);
		}

		myPoly.addRing(&myRing);
		poFeature->SetGeometry( &myPoly ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );

	}

	OGRDataSource::DestroyDataSource( poDS );
}

void OgrExporter::printPoly(std::list<Point_2> tPoints, int polyId)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}
	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("poly");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName << polyId <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();
	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}
	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "polygon", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
	Point_2 tmPoint;


	OGRFeature *poFeature;

	poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
	poFeature->SetField( "space_id", 1);

	OGRPolygon myPoly; OGRLinearRing myRing;

	for (list<Point_2>::iterator it=tPoints.begin(); it!=tPoints.end(); ++it)
	{

		tmPoint = *it;
		x = CGAL::to_double(tmPoint.x());
		y = CGAL::to_double(tmPoint.y());

		myRing.addPoint(x,y);
	}

	myPoly.addRing(&myRing);
	poFeature->SetGeometry( &myPoly ); 

	if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
	{
		printf( "Failed to create feature in shapefile.\n" );
		//exit( 1 );
	}

	OGRFeature::DestroyFeature( poFeature );
	OGRDataSource::DestroyDataSource( poDS );
}

void OgrExporter::printPoly(Traits::Polygon_2 poly, int polyId)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}
	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("poly");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName << polyId <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();
	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}
	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "polygon", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
	Point_2 tmPoint;


	OGRFeature *poFeature;

	poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
	poFeature->SetField( "space_id", 1);

	OGRPolygon myPoly; OGRLinearRing myRing;

	for (Traits::Polygon_2::Vertex_iterator it=poly.vertices_begin(); it!=poly.vertices_end(); ++it)
	{

		tmPoint = *it;
		x = CGAL::to_double(tmPoint.x());
		y = CGAL::to_double(tmPoint.y());

		myRing.addPoint(x,y);
	}

	myPoly.addRing(&myRing);
	poFeature->SetGeometry( &myPoly ); 

	if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
	{
		printf( "Failed to create feature in shapefile.\n" );
		//exit( 1 );
	}

	OGRFeature::DestroyFeature( poFeature );
	OGRDataSource::DestroyDataSource( poDS );
}

bool OgrExporter::fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

void OgrExporter::printVertices(Polygon_2 tPoints, int polyId)
{
	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}
	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("boundedby");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName << polyId <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();
	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}
	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "polygon", NULL, wkbPoint , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
    for (VertexIterator Vit=tPoints.vertices_begin(); Vit!=tPoints.vertices_end();++Vit)
	{
		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "space_id", 1 );

		OGRPoint pt;

		x = CGAL::to_double((*Vit).x());
		y = CGAL::to_double((*Vit).y());
		pt.setX( x );
		pt.setY( y );

		poFeature->SetGeometry( &pt ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );
	}

	OGRDataSource::DestroyDataSource( poDS );
}


void OgrExporter::drawTriangulation(vector<list<Point_2> > triangles, int polyID,vector<int> numberTria)
{

	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("Triangulation_room_");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName << polyID <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();

	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "triangles", NULL, wkbPolygon , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
	Point_2 tmPoint;
	for (vector<list<Point_2> >::iterator it=triangles.begin(); it!=triangles.end(); ++it)
	{

		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "space_id", polyID);

		OGRPolygon myPoly; OGRLinearRing myRing;


		for (list<Point_2>::iterator pIt = (*it).begin(); pIt!= (*it).end();++pIt){
			tmPoint = *pIt;
			x = CGAL::to_double(tmPoint.x());
			y = CGAL::to_double(tmPoint.y());

			myRing.addPoint(x,y);
		}

		myPoly.addRing(&myRing);
		poFeature->SetGeometry( &myPoly ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );

	}

	OGRDataSource::DestroyDataSource( poDS );
}


void OgrExporter::drawTransitions(vector<Segment_2> transitions, int polyID)
{

	const char *pszDriverName = "ESRI Shapefile";
	OGRSFDriver *poDriver;

	OGRRegisterAll();

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
		pszDriverName );
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		//exit( 1 );
	}

	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("Transitions_room_");
		string extension(".shp");
		std::stringstream sstm;
		sstm << fileName << polyID <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));
	OGRDataSource *poDS;
	const char* filename = resultName.c_str();

	poDS = poDriver->CreateDataSource( filename, NULL );
	if( poDS == NULL )
	{
		printf( "Creation of output file failed.\n" );
		//exit( 1 );
	}

	OGRLayer *poLayer;

	poLayer = poDS->CreateLayer( "transitions", NULL, wkbLineString , NULL );
	if( poLayer == NULL )
	{
		printf( "Layer creation failed.\n" );
		//exit( 1 );
	}

	OGRFieldDefn oField( "space_id", OFTString );

	oField.SetWidth(32);

	if( poLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		printf( "Creating Name field failed.\n" );
		//exit( 1 );
	}

	double x, y;
	Point_2 tmPoint;
	for (vector<Segment_2>::iterator it=transitions.begin(); it!=transitions.end(); ++it)
	{

		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField( "space_id", polyID);

		OGRLineString myLine;

		myLine.addPoint(CGAL::to_double((*it).source().x()),CGAL::to_double((*it).source().y()));
		myLine.addPoint(CGAL::to_double((*it).target().x()),CGAL::to_double((*it).target().y()));
		poFeature->SetGeometry( &myLine ); 

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			printf( "Failed to create feature in shapefile.\n" );
			//exit( 1 );
		}

		OGRFeature::DestroyFeature( poFeature );

	}

	OGRDataSource::DestroyDataSource( poDS );
}