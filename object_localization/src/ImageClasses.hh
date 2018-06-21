
#ifndef ImageClasses_H
#define ImageClasses_H



#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>

#include "std_msgs/String.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/Odometry.h"

#include "gazebo_msgs/GetModelState.h"
#include "gazebo_msgs/ModelState.h"

#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Point.h"

#include "tychebot_control/object.h"
#include "tychebot_control/objectArray.h"

#include "DataMap.hh"

using namespace std;
using namespace cv;

//static const std::string OPENCV_WINDOW = "Image window";
static const std::string MAP_WINDOW = "map window";
static const std::string H_WINDOW = "alturas";
static const std::string P_WINDOW = "processed window";
static const std::string R_WINDOW = "regression window";


class ImageRGB
{
  // for the image
  ros::NodeHandle nh;
  image_transport::ImageTransport it_RGB;
  image_transport::Subscriber image_sub_RGB;

  //image_transport::Publisher image_pub_;
  Mat matRGB;
  //Mat matSrcRGB;

  objectsV objects;


  ros::Publisher chatter_pub;
  // for the map




  ros::Subscriber subMap;// = nMap.subscribe("/tychebot/map", 10, mapCallback);
  //ros::Subscriber subTf;// = n.subscribe("/tychebot/dometry/odomety_filtered", 10, tfCallback);
  ros::Subscriber subCamTf;

  //ros::ServiceClient clientTf;
  //image_transport::Publisher image_pub_;
  cv::Mat mapC; //contours
  cv::Mat mapRaw; // raw map

  std::vector<contour> contornos;
  float resMap;

  //Data
  DataMap dataMap;
public:
  ImageRGB() : it_RGB(nh)
  {
    //for the image
    //Subscribe to input video feed and pucblish output video feed
    image_sub_RGB = it_RGB.subscribe("/tychebot/tychebot_front_ptz_camera/image_raw", 1, &ImageRGB::imageCbRGB, this);
      //image_pub_ = it_.advertise("image_converter/output_video", 1);

    //cv::namedWindow(H_WINDOW);
    cv::namedWindow(P_WINDOW);
    //cv::namedWindow(R_WINDOW);

    //for the map

    //Subscribe to input video feed and pucblish output video feed
    subMap = nh.subscribe("/tychebot/map", 2, &ImageRGB::mapCallback, this);
    //subTf = nh.subscribe("/tychebot/base_to_map", 1, &ImageRGB::tfCallback, this);
    subCamTf = nh.subscribe("/tychebot/camera_to_map", 1, &ImageRGB::tfCallback, this);
    //image_pub_ = it_.advertise("image_converter/output_video", 1);
    //clientTf = nsTf.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");

    chatter_pub = nh.advertise<tychebot_control::objectArray>("/tychebot/objects_position", 1);

    cv::namedWindow(MAP_WINDOW);
    contornos.empty();
  }
  ~ImageRGB()
  {
    cv::destroyWindow(MAP_WINDOW);
    //cv::destroyWindow(H_WINDOW);
    cv::destroyWindow(P_WINDOW);
    //cv::destroyWindow(R_WINDOW);

  }

  //for the image
  void imageCbRGB(const sensor_msgs::ImageConstPtr& msg);
  Mat extractHeight(vector<Point> contour, int* h = NULL, int *hs = NULL, int *w = NULL);

  //for the map
  void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& map);
  void tfCallback(const geometry_msgs::Pose::ConstPtr& tf);

  //void updateContours(std::vector<contour> contornosN);
  std::vector<contour> drawViewContours(cv::Mat &src, contour camCon, Eje eje);
};

#endif
