#ifndef Funciones_H
#define Funciones_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <utility>
using namespace std;
using namespace cv;

#define MINAREA 800

enum objectShape{
  AVION = 0,
  BICI,
  BARCO,
  BOTELLA,
  INDEFINIDO,

  S_MONEDA,
  S_OBSTACULO

};

enum objectClass{
  AVION_A = 1,
  AVION_V,
  AVION_R,

  BICI_A,
  BICI_V,
  BICI_R,

  BARCO_A,
  BARCO_V,
  BARCO_R,

  BOTELLA_A,
  BOTELLA_V,
  BOTELLA_R,

  INDEFINIDO_A,
  INDEFINIDO_V,
  INDEFINIDO_R,
  INDEFINIDO_K,

  MONEDA,
  OBSTACULO,

};

enum objectColor{
  ROJO = 0,
  AZUL,
  VERDE,
  AMARILLO,
  NEGRO,
  BLANCO
};

typedef std::vector<Point> contour;
typedef std::pair< contour, objectClass> objectI;
typedef std::vector<objectI> objectsV;
typedef std::tuple< objectShape, std::vector<int>, std::vector<double> > classRegression;
typedef std::vector< classRegression> classRegressionV;

//typedef std::pair< objectClass, double> regressionResult;

Vec3b getBGR(objectColor color);
std::vector<objectI> findObjects(objectColor colorName, Mat *src);
bool acceptColor(Vec3b color1, Vec3b color2, int margen);

objectShape identify(vector<Point> contoursT);

std::string to_str(objectClass);
std::string to_str(objectShape);

void drawCon(Mat *mat, std::vector<Point> con, bool sq = false, Vec3b color = Vec3b(255, 255, 255));

objectsV processImg(Mat src, Mat *result);
objectShape getObjectFromH(int h, int hs);

classRegressionV initRegression();
void drawClassName(Mat *src, objectI ob);

cv::Point getCen(contour con);

objectClass getClass(objectShape shape, objectColor color);

void divideClass(objectClass cls, objectShape &shape, objectColor &color);

#endif
