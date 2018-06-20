#include <stdio.h>

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <geometry_msgs/Pose.h>
#include <std_msgs/Float64.h>
#include <tychebot_control/objectArray.h>

using namespace std;

//Subscribers
ros::Subscriber sub_pose; //Pose
ros::Subscriber sub_explore_lite_goal; //Exploration goal
ros::Subscriber sub_obj_list;//Subscribe to object list

//Publishers
ros::Publisher pub_move_base_goal; //Navigation goal
ros::Publisher pub_move_base_cancel; //Stop navigation
ros::Publisher pub_camera_pan; //Camera pan

//Messages
geometry_msgs::Pose msg_pose; //Current position and orientation
move_base_msgs::MoveBaseActionGoal msg_explore_lite_goal; //Goal provided by modified explore_lite node
std_msgs::Float64 msg_camera_pan; //Camera pan
tychebot_control::objectArray test; //Behaves as a std::vector

//Callbacks
void callback_pose(const geometry_msgs::Pose::ConstPtr&);
void callback_explore(const move_base_msgs::MoveBaseActionGoal::ConstPtr&);



int main(int argc, char **argv)
{
	ros::init(argc, argv, "tychebot_control_test");
	ros::NodeHandle nh;

	cout << "tychebot_control_test started..." << endl;

	//Subscribers
	sub_pose = nh.subscribe<geometry_msgs::Pose>("/tychebot/base_to_map", 10, &callback_pose);
	sub_explore_lite_goal = nh.subscribe<move_base_msgs::MoveBaseActionGoal>("/explore/goal", 10, &callback_explore);
	//sub_obj_list = nh.subscribe<move_base_msgs::MoveBaseActionGoal>("/explore/goal", 10, &callback_explore);

	//Publishers
	pub_move_base_goal = nh.advertise<move_base_msgs::MoveBaseActionGoal>("/tychebot/movebase/goal", 1);
	pub_camera_pan = nh.advertise<std_msgs::Float64>("/tychebot/joint_pan_position_controller/command", 1);
	
	//Main loop
	while(ros::ok())
	{
		ros::spinOnce();

		//1 - Navigation loop

		//1.0 - Read inputs: via callbacks

		//1.1 - Evaluate objectives:
		/*1st aruco, then coins if nearby and available,
		then explore untill recognised >>half of the objects
		*/

		//1.2 - Send objective: coin, explore, or mark: publish to move_base


		//2 - Identification loop

		/*Aim camera to nearest unidentified obstacle in camera range.
		Wait for identication/timeout before objective change*/


		//Goal test with modified explore_lite
		pub_move_base_goal.publish(msg_explore_lite_goal);

	}

}

void callback_pose(const geometry_msgs::Pose::ConstPtr& _msg_pose)
{
	msg_pose = *_msg_pose;
}

void callback_explore(const move_base_msgs::MoveBaseActionGoal::ConstPtr& _msg_explore_lite_goal)
{

	msg_explore_lite_goal = *_msg_explore_lite_goal;

	//Time
	int t = _msg_explore_lite_goal->header.stamp.sec;
	cout << "Goal recieved at " << t << endl;

	//Position
	float px = _msg_explore_lite_goal->goal.target_pose.pose.position.x;
	float py = _msg_explore_lite_goal->goal.target_pose.pose.position.y;
	float pz = _msg_explore_lite_goal->goal.target_pose.pose.position.z;
	
	//Orientation
	float qx = _msg_explore_lite_goal->goal.target_pose.pose.orientation.x;
	float qy = _msg_explore_lite_goal->goal.target_pose.pose.orientation.y;
	float qz = _msg_explore_lite_goal->goal.target_pose.pose.orientation.z;
	float qw = _msg_explore_lite_goal->goal.target_pose.pose.orientation.w;

	cout << "Position: [" << px << ", " << py << ", " << pz << "]" << endl;
	cout << "Orientation: [" << qx << ", " << qy << ", " << qz << ", " << qw << "]" << endl;

}
