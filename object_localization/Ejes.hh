#ifndef EJE_H
#define EJE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include "geometry_msgs/Quaternion.h"
#include "funciones.hh"


#define FOV 0.5236
//#define FOV 0.49
#define LVEW 10
#define PCAM 0

enum EjesTipos
{
  MUNDO = 0,
  ROBOT,
  CAMARA,
  MAPA
};
class Eje;
class Vector2D;

void checkBase(EjesTipos base1, EjesTipos base2);
std::vector<contour> findMapContours(cv::Mat &src);

void toEulerAngle(geometry_msgs::Quaternion q, double *roll, double *pitch, double *yaw);
void upsideDownMat(cv::Mat &src);

contour drawCameraView(cv::Mat &src, Eje robot, double resolution);

contour getVisibleObjs(contour camCon, std::vector<contour> objects, float resolution, Eje eje);

Vector2D vectorFromImgObj(contour con, int xsize);

class BasePV
{
  const int z;
  double x;
  double y;
  EjesTipos base;

  protected:



  public:

  BasePV(int z_, double x_, double y_, EjesTipos base_): z(z_), x(x_), y(y_), base(base_) {}
  BasePV(int z_): z(z_){};
  double getX() const{return x;}
  double getY() const{return y;}
  int getZ(){ return z;}
  EjesTipos getBase(){return base;}
  void setX(double x_) {x = x_;}
  void setY(double y_) {y = y_;}
  void setBase(EjesTipos base_) {base = base_;}

  bool operator==(BasePV *other)
  {
    bool flag = false;
    std::cout<<"bases: "<<getBase()<<", "<<other->getBase()<<" zts: "<<getZ()<<", "<<other->getZ()<<std::endl;
    if(getZ() == other->getZ() && getBase() == other->getBase())
    {
      std::cout<<"X: "<<getX()<<", "<<other->getX()<<" Y: "<<getY()<<", "<<other->getY()<<std::endl;
      flag = ( (abs(getX() - other->getX()) < 0.5) && (abs(getY() - other->getY()) < 0.5));
    }
    return flag;
  }
  bool operator<(const BasePV other)const
  {
    //return (!( *(this) == other ));
    double m1 = (int)(sqrt(getX()*getX() + getY()*getY())) + cos(getX());
    double m2 = (int)(sqrt(other.getX()*other.getX() + other.getY()*other.getY())) + cos(other.getX());

    return m1<m2;
  }

};


class Point2: public BasePV
{
  /*double x;
  double y;
  EjesTipos base;*/

public:
  Point2(double x_, double y_, EjesTipos base_): BasePV(1, x_, y_, base_ ) {}
  Point2(cv::Point p, EjesTipos base_): BasePV(1, p.x, p.y, base_ ) {}
  //double getX(){return x;}
  //double getY(){return y;}
  //EjesTipos getBase(){return base;}

  /*Point2 operator+ (int inc)
  {
    return Point2(this->getX() + inc, this->getY() + inc, getBase());
  }*/

  cv::Point toCv(){ return cv::Point(getX(), getY()); }
  cv::Point2f toCvF(){ return cv::Point2f((float)getX(), (float)getY()); }
  bool operator<(const Point2 other)const
  {
    //return (!( *(this) == other ));
    double m1 = (int)(sqrt(getX()*getX() + getY()*getY())) + cos(getX());
    double m2 = (int)(sqrt(other.getX()*other.getX() + other.getY()*other.getY())) + cos(other.getX());

    return m1<m2;
  }

};

class Vector2D: public BasePV
{
  bool unitario;
  public:
  Vector2D(double x_, double y_, EjesTipos base_): BasePV(0, x_, y_, base_ ), unitario(false) {}
  Vector2D(Point2 p1_, Point2 p2_ ): BasePV(0 ), unitario(false)
   {
    checkBase(p1_.getBase(), p2_.getBase());
    setX(p2_.getX() - p1_.getX());
    setY(p2_.getY() -p1_.getY());
    setBase(p1_.getBase());

  }

  Vector2D(Point2 p1_): BasePV(0, p1_.getX(), p1_.getY(), p1_.getBase() ), unitario(false) {}
  Vector2D(double angle, EjesTipos base_): BasePV(0, cos(angle), sin(angle), base_ ), unitario(true) {}

  Vector2D operator+ (Vector2D other)
  {
    checkBase(getBase(), other.getBase());
    return Vector2D(this->getX() + other.getX(), this->getY() + other.getY(), getBase());
  }

  Vector2D operator* (double l)
  {
    return Vector2D(this->getX() * l, this->getY() * l, getBase());
  }

  Vector2D getPLength (double l)
  {
    if(!isUnitario())
    {
      normalize();
    }

    return *(this)*l;
  }

  Point2 pAtL(double l)
  {
    if(isUnitario())
    {
      normalize();
    }

    Vector2D buf = *(this)*l;
    Point2 p(buf.getX(), buf.getY(), buf.getBase());
    return p;
  }

  bool isUnitario() {return unitario;}
  void normalize();

  double norm();
};

class Eje
{
  //posicion respecto a otro eje.
  EjesTipos base;
  EjesTipos actual;
  //Point2 origen;
  //double angulo;

  double M[3][3];
public:


  Eje(EjesTipos base_, EjesTipos actual_, Point2 origen_, double angulo_): base(base_), actual(actual_)
  {
    checkBase(base_, origen_.getBase());
    actualizarQ(origen_, angulo_);
  }

  Point2 getOrigen(){return Point2(M[1][0], M[2][0], base);}
  EjesTipos getActualRef(){return actual;}

  void trasladar(BasePV *pv);
  void trasladar(contour &con, EjesTipos base);
  //void trasladar(std::vector<Point2> &vec);

  void actualizarQ(Point2 origen_, double angulo_);

  void draw(cv::Mat *src);


};

#endif //EJE_H
