
#include "Ejes.hh"

void Eje::trasladar(BasePV *pv)
{

  //std::cout<<" antes y: "<<pv->getY()<<std::endl;
  checkBase(actual, pv->getBase());
  double x = pv->getX();
  double y = pv->getY();
  pv->setX(M[1][0]*pv->getZ() + x * M[1][1] + y *M[1][2]);
  pv->setY(M[2][0]*pv->getZ() + x * M[2][1] + y *M[2][2]);

  pv->setBase(base);

}

void Eje::trasladar(contour &con, EjesTipos base)
{
  //std::vector<Point2> vec;
  for (int i = 0; i< con.size(); i++)
  {
    Point2 p(con[i], base);
    trasladar(&p);
    con[i] = p.toCv();

  }

}

/*void trasladar(std::vector<Point2> &vec)
{
  for(int i = 0; i < vec.size(); i++)
  {
    trasladar(&vec[1]);
  }
}*/
void Eje::actualizarQ(Point2 origen_, double angulo_)
{
  //origen = origen_;
  //angulo = angulo;

  M[0][0] = 1;
  M[0][1] = 0;
  M[0][2] = 0;
  M[1][0] = origen_.getX();
  M[2][0] = origen_.getY();

  M[1][1] = cos(angulo_);
  M[1][2] = sin(angulo_);
  M[2][1] = -sin(angulo_);
  M[2][2] = cos(angulo_);
}
void Eje::draw(cv::Mat *src)
{
  int longitud = src->rows / 20;

  Point2 ot(0,0, actual);
  Point2 ejex(longitud, 0, actual);
  Point2 ejey(0, longitud, actual);

  trasladar(&ot);
  trasladar(&ejex);
  trasladar(&ejey);

  //Point2 ot = trasladar(Point2(0,0, actual));
  //Point2 ejex = trasladar(Point2(longitud, 0, actual));
  //Point2 ejey = trasladar(Point2(0, longitud, actual));

  line(*src, cv::Point(ot.getX(), ot.getY()), cv::Point(ejex.getX(), ejex.getY()), (255), 1);
  line(*src, cv::Point(ot.getX(), ot.getY()), cv::Point(ejey.getX(), ejey.getY()), (255), 1);

  putText(*src, "x",cv::Point(ejex.getX(), ejex.getY()), cv::FONT_HERSHEY_SIMPLEX, 0.5, (255), 1 );
  putText(*src, "y",cv::Point(ejey.getX(), ejey.getY()), cv::FONT_HERSHEY_SIMPLEX, 0.5, (255), 1 );
}

void checkBase(EjesTipos base1, EjesTipos base2)
{
  if(base1 != base2)
  {
    std::cout<<"Warning! comparando dos objetos es bases distintas: "<< base1<<" y "<< base2<<std::endl;
  }
}

void Vector2D::normalize()
{
  double l = sqrt(getX()*getX() + getY()*getY());

  setX(getX() / l);
  setY(getY() / l);

  unitario = true;
}

double Vector2D::norm()
{
  double l = sqrt(getX()*getX() + getY()*getY());
  return l;
}

std::vector<contour> findMapContours(cv::Mat &src)
{
  //cv::Mat mParcial = src.clone();
  cv::Mat mContornos = src.clone();
  //cv::cvtColor(mContornos, mContornos, CV_BGR2GRAY);
  cv::blur(mContornos, mContornos, cv::Size(3,3));

  std::vector< std::vector<cv::Point> > con;
  //std::vector< std::vector< cv::Vec4i> hi;
  cv::findContours(mContornos, con, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  std::cout<<"Encontrados "<<con.size()<<" contornos en el mapa"<<std::endl;

  if(con.size() == 1)
  {
    //ya esta el rectangulo exterior completo. Hay que ver que hay dentro
    std::cout<<"Contorno exterior completo"<<std::endl;
    int dif = 10;
    cv::Rect bigRect = boundingRect(con[0]);
    cv::Rect notSoBigRect(bigRect.x + dif, bigRect.y + dif, bigRect.height - 2*dif, bigRect.width - 2*dif);

    cv::Mat smallerMat = mContornos(notSoBigRect);
    std::vector< std::vector<cv::Point> > inCon;
    cv::findContours(smallerMat, inCon, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(bigRect.x + dif, bigRect.y + dif));

    for(int i = 0; i < inCon.size(); i++)
    {
      con.push_back(inCon[i]);
    }

  }

  std::cout<<"Encontrados "<<con.size()<<" contornos en el mapa"<<std::endl;
  cv::Mat drawing = cv::Mat::zeros(mContornos.size(), CV_8UC3);
  for (int i = 0; i< con.size(); i++)
  {
    drawContours(drawing, con, i, (255, 0, 255), 2, 8);
    //cv::imshow("Contornos", drawing);
    //cv::waitKey(0);
  }


  return con;
  //cv::imshow("Contornos", drawing);
  //cv::waitKey(3);
}

void toEulerAngle( geometry_msgs::Quaternion q, double *roll, double *pitch, double *yaw)
{
  // roll (x-axis)
  double sinr = 2*(q.w * q.x * q.y * q.z);
  double cosr = 1 - 2*(q.x*q.x + q.y*q.y);
  *roll = atan2(sinr, cosr);

  //pitch (y-axis)
  double sinp = 2*(q.w * q.y - q.z*q.x);
  if(fabs(sinp)>=1)
  {
    *pitch = copysign(M_PI / 2, sinp);
  }
  else
  {
    *pitch = asin(sinp);
  }

  //yax (z-axis)
  double siny = 2*(q.w*q.z + q.x*q.y);
  double cosy = 1 - 2*(q.y*q.y + q.z*q.z);
  *yaw = atan2(siny, cosy);

}

void upsideDownMat(cv::Mat &src)
{
  cv::Mat map_x, map_y;
  map_x.create(src.size(), CV_32FC1);
  map_y.create(src.size(), CV_32FC1);

  for( int j = 0; j< src.rows; j++)
  {
    for(int i = 0; i< src.cols; i++)
    {
      map_x.at<float>(j,i) = i;
      map_y.at<float>(j,i) = src.rows - j;
    }
  }

  cv::remap(src, src, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
}

contour drawCameraView(cv::Mat &src, Eje robot, double resolution)
{
  contour con;
  //double desfx = 0.19/resolution;

  Point2 o(0,0, robot.getActualRef());
  Point2 limP((LVEW/resolution) * cos(FOV-PCAM), (LVEW/resolution) * sin(FOV-PCAM), robot.getActualRef());
  Point2 limN((LVEW/resolution) * cos(FOV+PCAM), -(LVEW/resolution) * sin(FOV+PCAM), robot.getActualRef());
  Point2 cen((LVEW/resolution),0, robot.getActualRef());

  robot.trasladar(&o);
  robot.trasladar(&limP);
  robot.trasladar(&limN);
  robot.trasladar(&cen);


  line(src, cv::Point(o.getX(), o.getY()), cv::Point(limP.getX(), limP.getY()), (150), 1);
  line(src, cv::Point(o.getX(), o.getY()), cv::Point(limN.getX(), limN.getY()), (150), 1);
  line(src, cv::Point(limP.getX(), limP.getY()), cv::Point(cen.getX(), cen.getY()), (150), 1);
  line(src, cv::Point(limN.getX(), limN.getY()), cv::Point(cen.getX(), cen.getY()), (150), 1);
  //circle(src, cv::Point(o.getX(), o.getY()), LVEW/resolution, (150));

  con.push_back(cv::Point(o.getX(), o.getY()));
  con.push_back(cv::Point(limP.getX(), limP.getY()));
  con.push_back(cv::Point(cen.getX(), cen.getY()));
  con.push_back(cv::Point(limN.getX(), limN.getY()));
  con.push_back(cv::Point(o.getX(), o.getY()));


  return con;

}

contour getVisibleObjs(contour camCon, std::vector<contour> objects, float resolution, Eje eje)
{

  contour newContour;
  Point2 cen(0,0,ROBOT);
  eje.trasladar(&cen);
  newContour.push_back(cen.toCv());
  double eA = -0.01;
  double eL = 0.5;

  double angulo = FOV+PCAM;
  while(angulo > -(FOV-PCAM))
  {
    double lon = 0.0;
    bool visible = true;
    Vector2D v(angulo, ROBOT);
    while(visible && lon<LVEW/resolution)
    {
      lon += eL;
      Point2 p = v.pAtL(lon);
      eje.trasladar(&p);

      for(int i = 0; i < objects.size(); i++)
      {
        double resultOb = cv::pointPolygonTest(objects[i], p.toCv(), false);
        if(resultOb >= 0)
        {
          visible = false;
        }
      }
    }
    Point2 p = v.pAtL(lon);
    eje.trasladar(&p);
    newContour.push_back(p.toCv());
    angulo += eA;

  }

  newContour.push_back(cen.toCv());
  return newContour;
}

Vector2D vectorFromImgObj(contour con, int xsize)
{
  cv::Point cen = getCen(con);
  double angle = (-FOV) + (((2.0*FOV)) / (double)xsize) * (double)cen.x;
  //std::cout<<"ANGULO: "<<angle<<std::endl;
  return Vector2D(angle, ROBOT);
}
