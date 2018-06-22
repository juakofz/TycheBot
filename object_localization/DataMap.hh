#ifndef DATAMAP_HH
#define DATAMAP_HH

#include "Ejes.hh"

#define MINDIST 2
#define MAXWIDTH 300

typedef std::pair<Point2, objectClass> centerObj;
typedef std::map<Point2, objectClass> centerObjV;
typedef std::map<Point2, contour> shapeObjV;
typedef std::pair<Point2, std::vector<objectClass> > pDatosHist;
typedef std::map<Point2, std::vector<objectClass> > datosHist;
typedef std::map<Point2, std::vector<objectClass> >::iterator itDatosHist;
class DataMap
{
  cv::Mat data;
  centerObjV objects;
  shapeObjV shapeObjects;
  double resolution;
  int colsImg;

  datosHist allDatosHistL1;
  datosHist allDatosHistL2;
  bool set;

public:
  DataMap(): set(false) {};

  void setMap (cv::Mat map, double res, int colsImg);
  void update(objectsV imageData, std::vector<contour> mapData, Eje eje );
  void updateMap(std::vector<contour> mapData);
  bool isSet(){ return set;}

  void draw();
  void filtrar(contour con, objectClass cls);
  void addDato(Point2 p, objectClass ob);
  objectClass moda(std::vector<objectClass> ocls);
  void clean();

  centerObjV getData() { return objects;}
};


#endif //DATAMAP_HH
