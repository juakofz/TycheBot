#include <iostream>

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>

#define source_frameid "/tychebot_map"
#define target_frameid "/tychebot_base_link"



int main(int argc, char **argv){
	ros::init(argc, argv, "base_to_map_tf");

	ros::NodeHandle nh;;
	tf::TransformListener listener;
	tf::StampedTransform transform;
	geometry_msgs::Pose pose;

	ros::Publisher publisher;
	publisher = nh.advertise<geometry_msgs::Pose>("/tychebot/base_to_map", 1);

	std::cout << "Base to map listener started." << std::endl;

	bool rec = false;
	while(!rec)
	{
		std::cout << "Waiting for transform..." << std::endl;
		rec = listener.waitForTransform(source_frameid, target_frameid, ros::Time(0), ros::Duration(3.0));	
	}

	std::cout << "Transform received!" << std::endl;

	ros::Rate rate(10.0);
	while(ros::ok())
	{
		
	    try
	    {
	      listener.lookupTransform(source_frameid, target_frameid, ros::Time(0), transform);
	    }

	    catch (tf::TransformException &ex)
	    {
	      ROS_ERROR("%s",ex.what());
	      ros::Duration(1.0).sleep();
	      continue;
    	}

    	tf::Quaternion q = transform.getRotation();
  		tf::Vector3 v = 	transform.getOrigin();

  		pose.position.x = v.getX();
  		pose.position.y = v.getY();
  		pose.position.z = v.getZ();

  		pose.orientation.x = q.getX();
  		pose.orientation.y = q.getY();
  		pose.orientation.z = q.getZ();
  		pose.orientation.w = q.getW();

  		publisher.publish(pose);

  		rate.sleep();

	}

	return 0;
}