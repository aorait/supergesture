#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include <geometry_msgs/PoseStamped.h>
//#include <cmath>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float64MultiArray.h"
#include <iostream>  
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <ctime>
#include <sys/time.h> 
#include <iomanip>
#include <math.h>

const int dimension=3;
double normVal;
int classID = 0;
double quat_diff[4];
std::ofstream out;
using namespace std;

class FormFeatureVector{
	ros::NodeHandle nh;
	ros::Publisher pub;
	ros::Subscriber sub1,sub2,sub3,sub4,sub5,sub6,sub7;
	std_msgs::Float64MultiArray featureVec;//feature vector publishes array each having 4 values
	double x_vals[dimension], y_vals[dimension], z_vals[dimension], q0_vals[dimension], q1_vals[dimension], q2_vals[dimension], q3_vals[dimension];
public:
	void callBack1(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[0]= msg.pose.position.x;
  		y_vals[0]= msg.pose.position.y;
  		z_vals[0]= msg.pose.position.z;
		q1_vals[0]= msg.pose.orientation.x;
		q2_vals[0]= msg.pose.orientation.y;
		q3_vals[0]= msg.pose.orientation.z;
		q0_vals[0]= msg.pose.orientation.w;

	}
	void callBack2(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[1]= msg.pose.position.x;
  		y_vals[1]= msg.pose.position.y;
  		z_vals[1]= msg.pose.position.z;
		q1_vals[1]= msg.pose.orientation.x;
		q2_vals[1]= msg.pose.orientation.y;
		q3_vals[1]= msg.pose.orientation.z;
		q0_vals[1]= msg.pose.orientation.w;
  	}
  	
  	void callBack3(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[2]= msg.pose.position.x;
  		y_vals[2]= msg.pose.position.y;
  		z_vals[2]= msg.pose.position.z;
		q1_vals[2]= msg.pose.orientation.x;
		q2_vals[2]= msg.pose.orientation.y;
		q3_vals[2]= msg.pose.orientation.z;
		q0_vals[2]= msg.pose.orientation.w;
		featureVec.data.clear();
  		calcRelVec();
  		pub.publish(featureVec);
  	}

  	void calcRelVec(){
		double dist_hand_upperArm;
		double *quat_diff_lowerArm_upperArm= quatDiff(q0_vals[1],q1_vals[1],q2_vals[1],q3_vals[1],q0_vals[2],q1_vals[2],q2_vals[2],q3_vals[2]);
		dist_hand_upperArm = sqrt(pow((x_vals[0]-x_vals[2]),2)+pow((y_vals[0]-y_vals[2]),2)+pow((z_vals[0]-z_vals[2]),2))/normVal;
		featureVec.data.push_back(dist_hand_upperArm);
  		for(int i=1;i<4;i++){ //Storing only first 3 values of the quaterion because the last value remains mostly constant
  			featureVec.data.push_back(quat_diff_lowerArm_upperArm[i]);
  		
  		}
  	}
	double* quatDiff(double a1,double a2, double a3, double a4, double b1,double b2, double b3, double b4){	
		//This function calculates the difference between 2 quaternions	
		double norm_a=pow(a1,2)+pow(a2,2)+pow(a3,2)+pow(a4,2);
		double inv_a1=a1/norm_a;
		double inv_a2=-a2/norm_a;
		double inv_a3=-a3/norm_a;
		double inv_a4=-a4/norm_a;
		quat_diff[0]=inv_a1*b1 - inv_a2*b2 - inv_a3*b3 - inv_a4*b4;
		quat_diff[1]=inv_a1*b2 + inv_a2*b1 + inv_a3*b4 - inv_a4*b3;
		quat_diff[2]=inv_a1*b3 - inv_a2*b4 + inv_a3*b1 + inv_a4*b2;
		quat_diff[3]=inv_a1*b4 + inv_a2*b3 - inv_a3*b2 + inv_a4*b1;
		return quat_diff;
	}


	FormFeatureVector(){
		sub3 = nh.subscribe("/UpperArm/pose", 100, &FormFeatureVector::callBack3, this);
		sub2 = nh.subscribe("/LowerArm/pose", 100, &FormFeatureVector::callBack2, this);
		sub1 = nh.subscribe("/Hand/pose", 100, &FormFeatureVector::callBack1, this);
		pub = nh.advertise<std_msgs::Float64MultiArray>("staticFeatureVector", 100);
	}
	
};

int main(int argc, char **argv)
{  
    normVal = 1.0;
    ros::init(argc, argv, "staticFeatureNode");
    FormFeatureVector obj;
    ros::spin();
    return 0;
}
