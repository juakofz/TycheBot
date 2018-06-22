
#include "ImageClasses.hh"


    void ImageRGB::imageCbRGB(const sensor_msgs::ImageConstPtr& msg)
      {
        //std::cout<<"imageCbRGB:: start"<<std::endl;
        //ROS_INFO("Callback");
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
          cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        }
        catch (cv_bridge::Exception& e)
        {
          ROS_ERROR("cv_bridge exception: %s", e.what());
          return;
        }

        //imageCallbackRGB(cv_ptr->image);

        objects = processImg(cv_ptr->image, &matRGB);
        Mat alturas = Mat::zeros(matRGB.rows, matRGB.cols, CV_8UC1);

        Mat regression = (cv_ptr->image).clone();
        Mat total = matRGB.clone();
        if(matRGB.rows>0 && matRGB.cols>0)
        {

          for(int i=0; i<objects.size(); i++)
          {
            if(get<1>(objects[i]) != OBSTACULO && get<1>(objects[i]) != MONEDA)
            {
              int h = 0;
              int hs = 0;
              int w = 0;

              objectShape shape;
              objectColor color;
              divideClass(get<1>(objects[i]), shape, color);
              alturas = alturas + extractHeight(get<0>(objects[i]), &h, &hs, &w);

              if(shape == INDEFINIDO )
              {
                get<1>(objects[i]) = getClass(getObjectFromH(h, hs), color);
                divideClass(get<1>(objects[i]), shape, color);
                  if(shape == BOTELLA )//)|| get<1>(objects[i]) == BOAT)
                  {
                    if(((double)h/(double)w) >= 2.6)
                    {
                      get<1>(objects[i]) = getClass(BOTELLA, color);
                    }
                    else if(((double)h/(double)w) >= 1)
                    {
                      get<1>(objects[i]) = getClass(BARCO, color);
                    }
                    else
                    {
                      get<1>(objects[i]) = getClass(BICI, color);
                    }
                  }
              }




              //drawClassName(&regression, objects[i]);
              drawClassName(&total, objects[i]);
            }

          }

          cv::imshow(H_WINDOW, alturas);
        }
        //cv::Mat total = matRGB + regression;
        //cv::imshow(P_WINDOW, matRGB);
        //cv::imshow(R_WINDOW, regression);
        cv::imshow(P_WINDOW, total);
        cv::waitKey(3);

        //static int image_count = 0;
        //std::stringstream sstream;
        //sstream <<"my_image"<< image_count<<".png";
        //ROS_ASSERT( cv::imwrite(sstream.str(), cv_ptr->image) );
        //image_count++;

        //Output modified video sstream
        //image_pub_.publish(cv_ptr->toImageMsg());
      };

Mat ImageRGB::extractHeight(vector<Point> contour, int* h_, int *hs_, int* w)
{

  double meanDist = 0;
  int cont = 0;
  Mat filledCont = Mat::zeros(matRGB.rows, matRGB.cols, CV_8UC1);
  drawContours(filledCont, vector<vector<Point> >(1, contour), -1, (255), CV_FILLED);

  Rect rect = boundingRect(contour);

  Moments m = moments(contour);

  int cx = m.m10 / m.m00;
  int cy = m.m01 / m.m00;

  std::ostringstream hStr;
  hStr << "h: "<< rect.height;

  std::ostringstream hsStr;
  hsStr << "h suelo: "<< filledCont.rows - (rect.height + rect.y);

  std::ostringstream hw;
  hw << "ratio: "<< ((double)rect.height / (double)rect.width);

  //putText(filledCont, meanDistStr.str(), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.5, Vec3b(200, 155, 155));
  putText(filledCont, hStr.str(), Point(cx, cy+15), FONT_HERSHEY_SIMPLEX, 0.5, Vec3b(200, 155, 155));
  putText(filledCont, hsStr.str(), Point(cx, cy+30), FONT_HERSHEY_SIMPLEX, 0.5, Vec3b(200, 155, 155));
  putText(filledCont, hw.str(), Point(cx, cy+45), FONT_HERSHEY_SIMPLEX, 0.5, Vec3b(200, 155, 155));

  if(h_ != NULL && hs_!= NULL )
  {
    *h_ = rect.height;
    *hs_ = (filledCont.rows - (rect.height + rect.y));

    if(w != NULL)
    {
      *w = rect.width;
    }
  }
  return filledCont;
}

void ImageRGB::mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& map)
{
  static int i=0;
  cv::Mat mapa(map->info.height, map->info.width, CV_8U);

  resMap = map->info.resolution;

  for (int y = 0; y < mapa.rows; y++)
  {
    for (int x = 0; x < mapa.cols; x++)
    {
      if((map->data[(y*mapa.rows + x)]) == -1)
      {
        mapa.at<uchar>(y, x) = 0;
      }
      else
      {
        mapa.at<uchar>(y, x) = (map->data[(y*mapa.rows + x)]) ;
      }
    }
  }

  upsideDownMat(mapa);
  cv::Point center(mapa.cols/2, mapa.rows/2);
  //xInitRobot = 8;
  //yInitRobot = -8;
  xInitRobot =0;
  yInitRobot = 0;
  //cv::Point center(mapa.cols/2 + xInitRobot/resMap, mapa.rows/2 + yInitRobot/resMap);
  int size = 750;
  cv::Rect myROI(center.x - size/2, center.y - size/2, size, size);
  cv::Mat croppedMap = mapa(myROI);

  //cv::resize(croppedMap, croppedMap, cv::Size(), 0.75, 0.75, cv::INTER_AREA);

  mapRaw = croppedMap.clone();
  /*cv::Mat littleMap;
  cv::resize(mapa, littleMap, cv::Size(), 0.25, 0.25, cv::INTER_AREA);
  //ROS_INFO("Size = [%d, %d]", map->info.height, map->info.width);
  cv::imshow("all map", littleMap);*/

  //cv::imwrite("mapaCompleto.png", mapa);
  //cv::imwrite("mapaParcial.png", croppedMap);
  contornos = findMapContours(croppedMap);
  //updateContours(detectedCon);

  mapC = croppedMap;
  if(!dataMap.isSet()) {dataMap.setMap(mapRaw, resMap, matRGB.cols);}
  dataMap.updateMap(contornos);
  //cv::waitKey(30);
}

void ImageRGB::tfCallback(const geometry_msgs::Pose::ConstPtr& tf)
{
  //std::cout<<"He oido: "<< tf->pose.pose.position.x<<std::endl;
  if(mapRaw.rows>0 && mapRaw.cols>0)
  {
    //gazebo_msgs::GetModelState modelstate;
    //gazebo_msgs::ModelState modelstate;

    //getmodelstate.model_name = "tychebot";
    //modelstate.request.model_name = "tychebot";

    //if(clientTf.call(modelstate))
    //{
      //ROS_INFO("SERVICE CALL SUCCESS");

      double yaw, pitch, roll;
      double xP = tf->position.x;
      double yP = tf->position.y;
      //double zP = modelstate.pose.position.z;

      //toEulerAngle(tf->pose.pose.orientation, &roll, &pitch, &yaw);
      toEulerAngle(tf->orientation, &roll, &pitch, &yaw);

      //int x = (int)(((tf->pose.pose.position.x/resMap) +0.5) + mapRaw.cols/2) ;
      //int y = (int)(-((tf->pose.pose.position.y/resMap) +0.5) + mapRaw.cols/2) ;

      //int x = (int)(((xP/resMap)) + mapa.cols/2 - xInitRobot/resMap) ;
      //int y = (int)(-((yP/resMap)) + mapa.rows/2 - yInitRobot/resMap ) ;
      int x = (int)(((xP/resMap)) + mapRaw.cols/2 ) ;
      int y = (int)(-((yP/resMap)) + mapRaw.rows/2 ) ;
      Eje mapa1(MUNDO, MAPA, Point2(mapRaw.cols/2,  mapRaw.rows/2, MUNDO), 0);
      Eje robot(MUNDO, ROBOT, Point2(x, y, MUNDO), yaw + M_PI/2 );
      Eje mundo(MUNDO, MUNDO, Point2(0, 0, MUNDO), 0);
      Eje mapa(MAPA, MUNDO, Point2(-mapRaw.cols/2, -mapRaw.rows/2, MAPA), 0);

      /*Point2 pPrueba(0,0,MAPA);
      std::cout<<"Punto iriginal: "<<pPrueba.getX()<<", "<<pPrueba.getY()<<std::endl;
      mapa1.trasladar(&pPrueba);
      std::cout<<"tas1: "<<pPrueba.getX()<<", "<<pPrueba.getY()<<std::endl;
      mapa.trasladar(&pPrueba);
      std::cout<<"tas2: "<<pPrueba.getX()<<", "<<pPrueba.getY()<<std::endl;*/

      cv::Mat mapN = mapRaw.clone();

      robot.draw(&mapN);
      mundo.draw(&mapN);

      contour camCon = drawCameraView(mapN, robot, resMap);

      std::vector<contour> vContour = drawViewContours(mapN, camCon, robot);

      cv::imshow(MAP_WINDOW, mapN);
      cv::waitKey(3);

      if(dataMap.isSet())
      {
        dataMap.update(objects, vContour, robot);
      }

      tychebot_control::objectArray msg;
      tychebot_control::object data;


      centerObjV allObjects = dataMap.getData();
      centerObjV::iterator it;
      for(it = allObjects.begin(); it != allObjects.end(); it++)
      {
        Point2 p = it->first;
        mapa.trasladar(&p);
        data.pose.x = p.getX() * resMap;
        data.pose.y = p.getY() * resMap;
        data.pose.z = 0;

        int type = (int)(it->second);
        if(type<= 18)
        {
          data.type = (uint8_t)(type);
          msg.objects.push_back(data);
        }
      }


      chatter_pub.publish(msg);

    //}
  }

}


std::vector<contour> ImageRGB::drawViewContours(cv::Mat &src, contour camCon, Eje eje)
{

  contour visibleContours = getVisibleObjs(camCon, contornos, resMap, eje);
  std::vector<contour> contourV;
  for(int i = 0; i < contornos.size(); i++)
  {
    bool flag = false;
    for (int j = 0; j < contornos[i].size(); j++)
    {

      double result = cv::pointPolygonTest(visibleContours, contornos[i][j], false);
      if(result >= 0)
      {
        flag = true;
      }
    }

    if(flag)
    {
      drawContours(src, vector<vector<Point> >(1, contornos[i]), -1, (255), 3);
      contourV.push_back(contornos[i]);
    }
    drawContours(src, vector<vector<Point> >(1, contornos[i]), -1, (255), 1);
  }


  //contour visibleContours = getVisibleObjs(camCon, contornos, resMap, eje);

  //eje.trasladar(visibleContours, ROBOT);
  drawContours(src, vector<vector<Point> >(1, visibleContours), -1, (255), 1);
  //drawContours(src, vector<vector<Point> >(1, camCon), -1, (255), 2);

  return contourV;
}
