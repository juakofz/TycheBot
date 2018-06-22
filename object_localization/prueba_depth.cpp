#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>

#include "funciones.hh"

static const std::string OPENCV_WINDOW_DEPTH = "Image window depth";

class ImageDepth
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  Mat mat;

public:
  ImageDepth() : it_(nh_)
  {
    //Subscribe to input video feed and pucblish output video feed
    image_sub_ = it_.subscribe("/tychebot/front_rgbd_camera/depth/image_raw", 1, &ImageDepth::imageCb, this);
    //image_pub_ = it_.advertise("image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW_DEPTH);
  }
  ~ImageDepth()
  {
    cv::destroyWindow(OPENCV_WINDOW_DEPTH);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    ROS_INFO("Depth");
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    //Draw an example circle on the video sstream
    /*if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
    {
      cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255, 0, 0));
    }*/
    Mat tmp;
    (cv_ptr->image).convertTo(tmp, CV_8UC1, 255.0);
    //Update GUI window
    //cv::imshow(OPENCV_WINDOW_DEPTH, cv_ptr->image);
    cv::imshow(OPENCV_WINDOW_DEPTH, cv_ptr->image);
    cv::waitKey(3);
    /*mat = processImg(cv_ptr->image);
    cv::imshow("Processed image", mat);
    cv::waitKey(3);

    static int image_count = 0;
    std::stringstream sstream;
    sstream <<"my_image"<< image_count<<".png";
    ROS_ASSERT( cv::imwrite(sstream.str(), cv_ptr->image) );
    image_count++;

    //Output modified video sstream
    image_pub_.publish(cv_ptr->toImageMsg());*/
  }
};

int main (int argc, char** argv)
{
  std::cout<<"Ejecutando"<<std::endl;
  ros::init(argc, argv, "image_converter");
  ImageDepth ic;
  ros::spin();
  return 0;
}
