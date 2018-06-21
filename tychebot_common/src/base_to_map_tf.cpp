#include <iostream>

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>

#define source_frameid "/tychebot_map"
#define target_frameid_base "/tychebot_base_link"
#define target_frameid_cam "/tychebot_front_ptz_camera_optical_frame_link"


int main(int argc, char **argv){
	ros::init(argc, argv, "base_to_map_tf");

	ros::NodeHandle nh;;
	tf::TransformListener listener;

	tf::StampedTransform transform_base;
	geometry_msgs::Pose pose_base;
	ros::Publisher publisher_base;
	publisher_base = nh.advertise<geometry_msgs::Pose>("/tychebot/base_to_map", 1);

	tf::StampedTransform transform_cam;
	geometry_msgs::Pose pose_cam;
	ros::Publisher publisher_cam;
	publisher_cam = nh.advertise<geometry_msgs::Pose>("/tychebot/camera_to_map", 1);

	std::cout << "TF listener started." << std::endl;

	bool rec = false;
	while(!rec)
	{
		std::cout << "Waiting for transforms..." << std::endl;
		bool rec1 = listener.waitForTransform(source_frameid, target_frameid_base, ros::Time(0), ros::Duration(3.0));
		bool rec2 = listener.waitForTransform(source_frameid, target_frameid_cam, ros::Time(0), ros::Duration(3.0));
		rec = (rec1 && rec2);
	}

	std::cout << "Transform received!" << std::endl;

	ros::Rate rate(10.0);
	while(ros::ok())
	{
		
	    try
	    {
	      listener.lookupTransform(source_frameid, target_frameid_base, ros::Time(0), transform_base);
	      listener.lookupTransform(source_frameid, target_frameid_cam, ros::Time(0), transform_cam);
	    }

	    catch (tf::TransformException &ex)
	    {
	      ROS_ERROR("%s",ex.what());
	      ros::Duration(1.0).sleep();
	      continue;
    	}

    	//Base
    	tf::Quaternion q = transform_base.getRotation();
  		tf::Vector3 v = transform_base.getOrigin();

  		pose_base.position.x = v.getX();
  		pose_base.position.y = v.getY();
  		pose_base.position.z = v.getZ();

  		pose_base.orientation.x = q.getX();
  		pose_base.orientation.y = q.getY();
  		pose_base.orientation.z = q.getZ();
  		pose_base.orientation.w = q.getW();

  		publisher_base.publish(pose_base);

  		//Camera
    	q = transform_cam.getRotation();
  		v = transform_cam.getOrigin();

  		pose_cam.position.x = v.getX();
  		pose_cam.position.y = v.getY();
  		pose_cam.position.z = v.getZ();

  		pose_cam.orientation.x = q.getX();
  		pose_cam.orientation.y = q.getY();
  		pose_cam.orientation.z = q.getZ();
  		pose_cam.orientation.w = q.getW();

  		publisher_cam.publish(pose_cam);

  		rate.sleep();

	}

	return 0;
}