#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include <geometry_msgs/PoseStamped.h>
#include <cmath>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float64MultiArray.h"

#define dimension 4
double normVal;

int tms = 0;

using namespace std;

class FormFeatureVector
{
	ros::NodeHandle nh;
	ros::Publisher pub;
	ros::Subscriber sub1,sub2,sub3,sub4,sub5,sub6,sub7;
	std_msgs::Float64MultiArray featureVec;
	double x_vals[dimension], y_vals[dimension], z_vals[dimension];
public:
	void callBack1(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[0]= msg.pose.position.x;
  		y_vals[0]= msg.pose.position.y;
  		z_vals[0]= msg.pose.position.z;

	}
	void callBack2(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[1]= msg.pose.position.x;
  		y_vals[1]= msg.pose.position.y;
  		z_vals[1]= msg.pose.position.z;
  	}
  	
  	void callBack3(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[2]= msg.pose.position.x;
  		y_vals[2]= msg.pose.position.y;
  		z_vals[2]= msg.pose.position.z;
  	}
  	void callBack4(const geometry_msgs::PoseStamped msg)
	{
  		x_vals[3]= msg.pose.position.x;
  		y_vals[3]= msg.pose.position.y;
  		z_vals[3]= msg.pose.position.z;
		featureVec.data.clear();
  		calcDist();
		tms++;
  		pub.publish(featureVec);
  	}

  	void calcDist()
	{
  		double dist;
		int cnt = 0;
  		for(int i=0;i<dimension;i++)
		{
  			for(int j=i+1;j<dimension;j++)
			{
				if (cnt != 0 && /*cnt != 3 &&*/ cnt != 5)
				/*order: 	shoulder - upper arm    deleted
						shoulder - lower arm
						shoulder - hand
						upper arm - lower arm
						upper arm - hand	change to elbow angle
						lower arm - hand        deleted
				*/
				{
  					dist=sqrt(pow((x_vals[i]-x_vals[j]),2)+pow((y_vals[i]-y_vals[j]),2)+pow((z_vals[i]-z_vals[j]),2));
  					dist=dist/normVal;
  					featureVec.data.push_back(dist);
				}
				cnt++;
  			}
  		}
  	}

	FormFeatureVector()
	{
		printf("Initialization...\n");
		sub1 = nh.subscribe("/Shoulder/pose", 100, &FormFeatureVector::callBack1, this);
		sub2 = nh.subscribe("/UpperArm/pose", 100, &FormFeatureVector::callBack2, this);
		sub3 = nh.subscribe("/LowerArm/pose", 100, &FormFeatureVector::callBack3, this);
		sub4 = nh.subscribe("/Hand/pose", 100, &FormFeatureVector::callBack4, this);
		pub = nh.advertise<std_msgs::Float64MultiArray>("classifierFeatureVector", 100);
		printf("Done.\n"); 
	}
	
};



int main(int argc, char **argv)
{
  normVal = 1.0;
  ros::init(argc, argv, "classifierFeatureVector");
  FormFeatureVector obj;
  ros::spin();
  return 0;
}
