
#include "DataMap.hh"

void DataMap::setMap (cv::Mat map, double res, int colsImg_)
{
  data = cv::Mat::zeros(map.size(), CV_8UC3);
  resolution = res;
  colsImg = colsImg_;
  set = true;
}


void DataMap::update(objectsV imageData, std::vector<contour> mapData, Eje eje)
{
  cv::Mat dataActual = Mat::zeros(data.size(), CV_8UC1);
  //std::cout<<"Tenemos que unir "<<imageData.size()<<" objetos que vemos en la imagen con "<<mapData.size()<<" objetos que vemos en el mapa"<<std::endl;
  double eL = 0.5;
  std::vector<int> indexFound;
  for(int i = 0; i < imageData.size(); i++)
  {
    cv::Rect rect = boundingRect(std::get<0>(imageData[i]));
    if(rect.width < MAXWIDTH)
    {
      //std::cout<<"Buscamos el "<<to_str(std::get<1>(imageData[i]))<<std::endl;
      Vector2D vec = vectorFromImgObj(std::get<0>(imageData[i]), colsImg);
      //eje.trasladar(&vec);
      double lon = 0.0;
      bool visible = true;
      contour matchCon;
      while(visible && lon<(LVEW/resolution))
      {
        lon += eL;
        Point2 p = vec.pAtL(lon);
        eje.trasladar(&p);

        for(int j = 0; j < mapData.size(); j++)
        {
          if(std::find(indexFound.begin(), indexFound.end() , j) == indexFound.end())
          {
            contour con = mapData[j];
            double resultOb = cv::pointPolygonTest(con, p.toCvF(), false);
            if(resultOb >= 0)
            {
              matchCon = con;
              visible = false;
              indexFound.push_back(j);
            }
          }
        }
      }


      cv::Point cen = getCen(matchCon);
      objectClass oclass = std::get<1>(imageData[i]);

      objectShape shape;
      objectColor color;
      divideClass(oclass, shape, color);
      //objects.push_back(std::make_pair(Point2(cen, vec.getBase()), std::get<1>(imageData[i])));
      if(!visible && (lon > MINDIST) && shape != INDEFINIDO)
      {
        filtrar(matchCon, oclass);
      }
      cv::circle(dataActual, cen, 3, (155), 2);
      putText(dataActual, to_str(std::get<1>(imageData[i])), cen, FONT_HERSHEY_SIMPLEX, 0.25, (255), 1);

      Point2 ot(0,0, eje.getActualRef());
      Point2 end = vec.pAtL(LVEW/resolution);
      eje.trasladar(&end);
      eje.trasladar(&ot);

      cv::line(dataActual, ot.toCv(), end.toCv(),(150), 1);
    }
  }

  Point2 origen = eje.getOrigen();
  cv::circle(dataActual, origen.toCv(), MINDIST/resolution, (155), 2);

  drawContours(dataActual, mapData, -1, (255, 0, 255), 1, 8);
  cv::imshow("Datos cruzados", dataActual);
  cv::waitKey(3);

  clean();
  draw();
};

void DataMap::draw()
{
  centerObjV::iterator it;
  for(it = objects.begin(); it != objects.end(); it++)
  {

    Point2 cen = it->first;
    objectShape shape;
    objectColor color;
    divideClass(it->second, shape, color);
    if(color == NEGRO) { color = BLANCO;}
    //cv::circle(data, cen.toCv(), 3, (155), 2);
    putText(data, to_str(it->second), cen.toCv(), FONT_HERSHEY_SIMPLEX, 0.25, Vec3b(255, 255, 255), 1);
    drawContours(data, vector<vector<Point> >(1, shapeObjects[cen]), -1, (getBGR(color) + getBGR(color)), 1);

    double area = cv::contourArea(shapeObjects[cen]);
    //putText(data, std::to_string(area), cv::Point(cen.getX(), cen.getY() + 10), FONT_HERSHEY_SIMPLEX, 0.3, Vec3b(255, 255, 255), 1);

    cv::RotatedRect rect = cv::fitEllipse (shapeObjects[cen]);
    double ratio = rect.size.width / rect.size.height;
    //putText(data, std::to_string(ratio), cv::Point(cen.getX(), cen.getY() + 20), FONT_HERSHEY_SIMPLEX, 0.3, Vec3b(255, 255, 255), 1);
  }
  cv::imshow("Datos historicos", data);
  cv::waitKey(3);
}

void DataMap::filtrar(contour con, objectClass cls)
{
  double area = cv::contourArea(con);
  if(!(area > 50 && area < 70 && cls == OBSTACULO))
  {
    itDatosHist it = allDatosHistL1.begin();
    bool found = false;
    for(itDatosHist it = allDatosHistL1.begin(); it!=allDatosHistL1.end(); it++)
    {
      Point2 p2 = it->first;
      double resultOb = cv::pointPolygonTest(con, p2.toCv(), false);
      if(resultOb >= 0)
      {
        shapeObjects[it->first] = con;
        objectColor col;
        objectShape shape;
        divideClass(cls, shape, col);
        found = true;
        //addDato(it->first, cls);
        if(area > 75)
        {
          addDato(it->first, OBSTACULO);
        }
        else if(area > 50 && area < 75)
        {

          addDato(it->first, getClass(AVION, col));
          addDato(it->first, cls);
        }

        else if( area > 50 && (cls == MONEDA || shape == BOTELLA)) ;
        else
        {
          addDato(it->first, cls);
        }
      }
    }

    if(!found)
    {
      Point2 p(getCen(con), MUNDO);
      std::vector<objectClass> vectorObE;
      std::vector<objectClass> vectorOb;
      allDatosHistL1[p] = vectorObE;
      vectorOb.push_back(cls);
      allDatosHistL2[p] = vectorOb;
      shapeObjects[p] = con;
    }
  }
};

void DataMap::addDato(Point2 p, objectClass ob)
{
  if(allDatosHistL2.find(p) != allDatosHistL2.end())
  {
    if(allDatosHistL2[p].size() <= 10)
    {
      allDatosHistL2[p].push_back(ob);
    }
    else
    {
        objectClass modaL2 = moda(allDatosHistL2[p]);
        allDatosHistL1[p].push_back(modaL2);
        allDatosHistL2[p].empty();

        objects[p] = moda(allDatosHistL1[p]);
    }
  }
  else
  {
    std::cout<<"Esto no deberia pasar...! tamano l1: "<<allDatosHistL1.size()<<" tamano l2: "<< allDatosHistL2.size()<<std::endl;

  }

}

objectClass DataMap::moda(std::vector<objectClass> vocls)
{
  objectClass obcls;
  int mayor = 0;
  std::map<objectClass, int> contMap;
  std::map<objectClass, int>::iterator it;
  for(int i = 0; i<vocls.size(); i++)
  {
    it = contMap.find(vocls[i]);
    if(it != contMap.end())
    {
      it->second += 1;
      if(it->second > mayor)
      {
        obcls = it->first;
        mayor = it->second;
      }
    }
    else
    {
      contMap[vocls[i]] = 1;
    }


  }

  return obcls;

}

void DataMap::clean()
{
  std::vector<Point2> vecDelete;
  itDatosHist it; // = allDatosHistL1.begin();
  for (it = allDatosHistL1.begin(); it!= allDatosHistL1.end(); it++)
  {
    contour act = shapeObjects[it->first];
    shapeObjV::iterator itc;
    for(itc = shapeObjects.begin(); itc != shapeObjects.end(); itc++)
    {
      if( shapeObjects.find(it->first) != itc)
      {
        Point2 p= itc->first;
        double result = pointPolygonTest(act, p.toCv(), false);
        if(result >= 0)
        {
          std::cout<<"Contorno repetido..."<<std::endl;
          vecDelete.push_back(p);
        }
      }
    }
  }

  for(int i = 0; i < vecDelete.size(); i++)
  {
    allDatosHistL1.erase(vecDelete[i]);
    allDatosHistL2.erase(vecDelete[i]);
    objects.erase(vecDelete[i]);
    shapeObjects.erase(vecDelete[i]);
  }

}
