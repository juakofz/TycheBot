
#include "funciones.hh"



Vec3b getBGR(objectColor color)
{
  Vec3b rgb(0,0,0);
  //std::string str(color);
  //std::cout<<"El color que buscamos es el: "<< str <<std::endl;

  if(color == ROJO) {rgb[2] = 102;}
  if(color == AZUL) {rgb[0] = 102;}
  if(color == VERDE) {rgb[1] = 102;}
  if(color == AMARILLO) {rgb[2] = 102; rgb[1] = 102;}
  if(color == BLANCO) {rgb[0] = 102; rgb[1] = 102; rgb[2] = 102;}
  if(color == NEGRO);

  return rgb;
};

std::vector<objectI> findObjects(objectColor colorName, Mat *src)
{

  std::vector<objectI> allObjects;


  Vec3b color = getBGR(colorName);
  for( int x = 0; x < src->rows; x++ )
  {
    for( int y = 0; y < src->cols; y++ )
    {
        if(acceptColor(src->at<Vec3b>(x, y), color, 150 ) )
        {
          src->at<Vec3b>(x, y)[0] = 255;
          src->at<Vec3b>(x, y)[1] = 255;
          src->at<Vec3b>(x, y)[2] = 255;
        }

        else
        {
          src->at<Vec3b>(x, y)[0] = 0;
          src->at<Vec3b>(x, y)[1] = 0;
          src->at<Vec3b>(x, y)[2] = 0;
        }
      }
  }
  Mat bw;
  cvtColor(*src, bw, CV_BGR2GRAY);
  threshold(bw, bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  //imshow("Binary Image", bw);

  Mat closed;
  int kernel_size = 10;
  Mat kernel = Mat::ones(kernel_size, kernel_size, CV_32F);
  morphologyEx(bw, closed, MORPH_CLOSE, kernel);
  //imshow("Closed Image", closed);

  Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
  Mat dist;
  dilate(closed, dist, kernel1);
  //imshow("Peaks", dist);
  // Create the CV_8U version of the distance image
  // It is needed for findContours()

  Mat dist_8u;
  dist.convertTo(dist_8u, CV_8U);
  // Find total markers
  vector<vector<Point> > contours;
  findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  for(int i = contours.size() - 1; i >= 0; i--)
  {
    if(cv::contourArea(contours[i]) < MINAREA)
    {
      contours.erase(contours.begin() + i);
    }
  }
  // Create the marker image for the watershed algorithm
  //*con = contours;
  //RNG rng;
  //Mat drawing = Mat::zeros(dist.size(), CV_8UC3);
  Mat drawing = Mat::zeros(dist.size(), CV_8UC3);
  //drawing = cv::Scalar(255, 255, 255);
  //std::cout<<"Hemos encontrado "<<contours.size()<<" objetos"<<std::endl;
  for( int i = 0; i< contours.size(); i++)
  {
    //Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    //Scalar scolor = Scalar(color[0], color[1], color[2]);
    if(color == getBGR(NEGRO))
    {
      color = getBGR(BLANCO);
    }

    drawContours(drawing, contours, i, color, 2, 8);
    Moments m = moments(contours[i]);

    int cx = m.m10 / m.m00;
    int cy = m.m01 / m.m00;
    circle(drawing, Point(cx, cy), 2, color, 3);

    double area = contourArea(contours[i]);
    std::ostringstream areaStr;
    areaStr << "Area: "<< area;

    Vec3b colorN = color;
    objectShape result = identify(contours[i]);

    if(area < 400)
    {
      colorN = getBGR(BLANCO);
      result = INDEFINIDO;
    }


    if (color == getBGR(AMARILLO))
    {
      result = S_MONEDA;
    }

    if (color == getBGR(BLANCO))
    {
      result = S_OBSTACULO;
    }

    Rect rect = boundingRect(contours[i]);
    rectangle(drawing, rect, colorN);

    putText(drawing, areaStr.str(), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.3, color);

    std::ostringstream relStr;
    relStr << "Relacion h/w: "<<((double)rect.height / (double)rect.width);
    putText(drawing, relStr.str(), Point(cx, cy + 10), FONT_HERSHEY_SIMPLEX, 0.3, color);

    double areaR = ((double)rect.width*(double)rect.height);
    std::ostringstream areaRStr;
    areaRStr << "diff: "<< (((abs(areaR - area))/area)*100) <<"%";
    putText(drawing, areaRStr.str(), Point(cx, cy + 20), FONT_HERSHEY_SIMPLEX, 0.3, color);


    //putText(drawing, to_str(result), Point(cx, cy + 30), FONT_HERSHEY_SIMPLEX, 0.3, color);

    objectI ob = std::make_pair(contours[i], getClass(result, colorName));
    allObjects.push_back(ob);
  }

  //imshow("Contours", drawing);
  //cv::waitKey(0);
  *src = drawing;
  //return contours.size();

  return allObjects;

}

bool acceptColor(Vec3b color1, Vec3b color2, int margen)
{
  bool flag = true;
  for(int i = 0 ; i<3; i++)
  {
    if(color1[i] == 0 && (color2[i] != 0)) { flag = false;}
    if(color2[i] == 0 && (color1[i] != 0)) { flag = false;}

    if( (color2[i] != 0 && (color1[i] != 0)) && ((abs(color1[i]) - color2[i]) > margen) ) { flag = false;}

  }

  if( ((color1[0] == color1[1]) && (color1[1] == color1[2])) &&
      ((color2[0] == color2[1]) && (color2[1] == color2[2])) &&
     ((abs(color1[0]) - color2[0]) < 50) &&
     ((abs(color1[1]) - color2[1]) < 50) &&
     ((abs(color1[2]) - color2[2]) < 50) )
  {
    flag = true;
  }


  return flag;


}

objectShape identify(vector<Point> contour)
{

  objectShape result = INDEFINIDO;
  double area = contourArea(contour);
  Rect rect = boundingRect(contour);

  double rel = ((double)rect.height / (double)rect.width);
  double areaR = ((double)rect.width*(double)rect.height);
  double areaP = (((abs(areaR - area))/area)*100);

  if(areaP < 20)
  {
    result = S_OBSTACULO;
  }

  if(rel < 0.4)
  {
    result = AVION;
  }

  return result;
};

std::string to_str(objectClass o)
{
  std::string str;
  objectShape shape;
  objectColor color;
  divideClass(o, shape, color);
  str = to_str(shape);


  return str;
};

std::string to_str(objectShape shape)
{
  std::string str;
  switch(shape)
  {
    case S_OBSTACULO :
      str = "obstaculo";
      break;
    case S_MONEDA :
      str = "moneda";
      break;
    case INDEFINIDO:
      str = "indefinido";
      break;
    case AVION :
      str = "avion";
      break;
    case BARCO :
      str = "barco";
      break;
    case BOTELLA :
      str = "botella";
      break;
    case BICI :
      str = "bicicleta";
      break;
  };

  return str;
}
void drawCon(Mat *mat, std::vector<Point> con, bool sq, Vec3b color)
{
  if(sq)
  {
    Rect rect = boundingRect(con);
    rectangle(*mat, rect, color);
  }
  else
  {
    drawContours(*mat, vector<vector<Point> >(1, con), -1, color, 2, 8);
  }
}

objectsV processImg(Mat src, Mat *result)
{
  /*if (!src.data)
  {
    std::cout<<"No image. Returning"<<std::endl;
    return Mat();
  }*/

  // vector con todos los posibles colores.
  std::vector<objectColor> colores;
  colores.push_back(ROJO);
  colores.push_back(AZUL);
  colores.push_back(VERDE);
  colores.push_back(AMARILLO);
  colores.push_back(NEGRO);

  // Show source image
  //&imshow("Source Image", src);

  std::vector<Mat> contornos;
  Mat total = Mat::zeros(src.size(), CV_8UC3);
  Mat nogo = Mat::zeros(src.size(), CV_8UC3);
  Mat yesgo = Mat::zeros(src.size(), CV_8UC3);

  vector<vector<Point> > contoursT;
  vector<vector<Point> > contoursY;
  vector<vector<Point> > contoursN;
  vector<vector<Point> > contoursU;

  objectsV objects;

  for (int i = 0; i<colores.size(); i++)
  {
    objectsV ob;
    Mat buffer = src.clone();
    cv::Rect ROI(0,0, src.cols, 430);
    cv::Mat recortada = buffer(ROI);
    //vector<vector<Point> > con;
    ob = findObjects(colores[i], &recortada);
    contornos.push_back(buffer);
    total = total + buffer;
    /*((colores[i].compare(argv[1]) == 0) || (colores[i].compare("amarillo") == 0))?
      yesgo = yesgo + buffer: nogo = nogo + buffer;*/
    objects.insert(objects.end(), ob.begin(), ob.end());
    //waitKey(0);
  }

  Mat rMat = src.clone();
  //Mat uMat = src.clone();
  //Mat iMat = src.clone();


  for(int i = 0; i < objects.size(); i++)
  {

    /*if (std::get<1>(objects[i]) == INDEFINIDO)
    {

      drawCon(&rMat, std::get<0>(objects[i]));
      drawCon(&uMat, std::get<0>(objects[i]));
      Moments m = moments(std::get<0>(objects[i]));
      int cx = m.m10 / m.m00;
      int cy = m.m01 / m.m00;

      //putText(rMat, to_str(std::get<1>(objects[i])), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.75, Vec3b(0, 0, 0), 2);
      //putText(uMat, to_str(std::get<1>(objects[i])), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.75, Vec3b(255, 255, 255), 2);

    }
    else
    {*/
      drawCon(&rMat, std::get<0>(objects[i]));
      //drawCon(&iMat, std::get<0>(objects[i]));
      Moments m = moments(std::get<0>(objects[i]));
      int cx = m.m10 / m.m00;
      int cy = m.m01 / m.m00;

      if(std::get<1>(objects[i]) == MONEDA || std::get<1>(objects[i]) == OBSTACULO)
      {
        putText(rMat, to_str(std::get<1>(objects[i])), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.75, Vec3b(255, 255, 255), 2);
      }//putText(iMat, to_str(std::get<1>(objects[i])), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.75, Vec3b(255, 255, 255), 2);

    //}
  }

  //imshow("mat I", iMat);

  //waitKey(3);
  *result = rMat;
  return objects;
}

objectShape getObjectFromH(int h, int hs)
{
  static classRegressionV regressionData = initRegression();

  objectShape result = INDEFINIDO;

  double minDiff = 20;

  for(int i=0; i<regressionData.size(); i++)
  {
    classRegression iter = regressionData[i];
    int margenIter[2] = {get<1>(iter)[0], get<1>(iter)[1]};
    if( h>=margenIter[0] && h<= margenIter[1])
    {
      //std::cout<<"Esta dentro de los margenes del "<<to_str(get<0>(iter))<<std::endl;
      double coefIter[4] = {get<2>(iter)[0], get<2>(iter)[1], get<2>(iter)[2], get<2>(iter)[3]};
      double hsAprox = coefIter[0] + coefIter[1]*h  + coefIter[2]*h*h + coefIter[3]*h*h*h;
      //std::cout<<"hsAProx:" << hsAprox<<", h:"<< hs<<std::endl;
      double diff = abs(hsAprox - (double)hs);

      //std::cout<<"diff: "<<diff<<"mindiff: "<<minDiff<<std::endl;

      if(diff < minDiff)
      {
        minDiff = diff;
        result =  get<0>(iter);
        //std::cout<<"Es un "<< to_string(result)<<std::endl;
      }
    }
  }

  return result;
}

classRegressionV initRegression()
{
  classRegressionV result;
  std::vector<int> margen;
  std::vector<double> coef;

  //barco
  /*margen.clear();
  coef.clear();

  margen.push_back(40);
  margen.push_back(135);

  coef.push_back(250);
  coef.push_back(0.72);
  coef.push_back(-0.011);
  coef.push_back(0.000036);

  result.push_back(std::make_tuple(BOAT, margen, coef));*/

  margen.clear();
  coef.clear();

  margen.push_back(50);
  margen.push_back(380);

  coef.push_back(240);
  coef.push_back(-0.35);
  coef.push_back(-0.0000013);
  coef.push_back(0);

  result.push_back(std::make_tuple(BARCO, margen, coef));

  //botella
  /*margen.clear();
  coef.clear();

  margen.push_back(154);
  margen.push_back(312);

  coef.push_back(280);
  coef.push_back(-0.9);
  coef.push_back(0.0028);
  coef.push_back(-0.0000041);

  result.push_back(std::make_tuple(BOTTLE, margen, coef));*/

  margen.clear();
  coef.clear();

  margen.push_back(50);
  margen.push_back(400);

  coef.push_back(240);
  coef.push_back(-0.36);
  coef.push_back(-0.0001);
  coef.push_back(0.000000094);

  result.push_back(std::make_tuple(BOTELLA, margen, coef));


  //avion
  margen.clear();
  coef.clear();

  margen.push_back(15);
  margen.push_back(85);

  coef.push_back(380);
  coef.push_back(0.2);
  coef.push_back(-0.0037);
  coef.push_back(0.000025);

  result.push_back(std::make_tuple(AVION, margen, coef));

  margen.clear();
  coef.clear();

  margen.push_back(85);
  margen.push_back(100);

  coef.push_back(480);
  coef.push_back(-1);
  coef.push_back(0);
  coef.push_back(0);

  result.push_back(std::make_tuple(AVION, margen, coef));

  margen.clear();
  coef.clear();

  margen.push_back(50);
  margen.push_back(310);

  coef.push_back(240);
  coef.push_back(-0.27);
  coef.push_back(-0.000026);
  coef.push_back(0);

  result.push_back(std::make_tuple(BICI, margen, coef));

  return result;

};

void drawClassName(Mat *src, objectI ob)
{

  Moments m = moments(std::get<0>(ob));

  int cx = m.m10 / m.m00;
  int cy = m.m01 / m.m00;
  putText(*src, to_str(std::get<1>(ob)), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.75, Vec3b(255, 255, 255), 2);
  drawCon(src, std::get<0>(ob));
};

cv::Point getCen(contour con)
{
  Moments m = cv::moments(con);

  int cx = m.m10 / m.m00;
  int cy = m.m01 / m.m00;
  return Point(cx, cy);
};

objectClass getClass(objectShape shape, objectColor color)
{
  objectClass cls = INDEFINIDO_K;
  if((int)shape < 5)
  {
    cls = (objectClass)((int)color + 3*(int)shape);
  }
  else if(shape == S_MONEDA)
  {
    cls =  MONEDA;
  }
  else if(shape == S_OBSTACULO)
  {
    cls =  OBSTACULO;
  }


  std::cout<<"Clase obtenida es: "<<cls<<", "<<to_str(cls)<<std::endl;
  return cls;
}

void divideClass(objectClass cls, objectShape &shape, objectColor &color)
{
  if(cls == MONEDA)
  {
    shape =  S_MONEDA;
    color = AMARILLO;
  }
  else if(cls == OBSTACULO)
  {
    shape =  S_OBSTACULO;
    color = NEGRO;
  }

  else if(cls == INDEFINIDO_K)
  {
    shape =  INDEFINIDO;
    color = NEGRO;
  }

  else
  {
    shape = (objectShape)((int)(cls / 3));
    color = (objectColor)((int)(cls % 3));
  }
  //std::cout<<"De "<<cls<<"obtenemos "<<shape<<" y "<< color<<std::endl;

}
