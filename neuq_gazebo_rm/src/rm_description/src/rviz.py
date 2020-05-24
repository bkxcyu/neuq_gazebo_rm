#!/usr/bin/env python 
import roslib
#roslib.load_manifest('learning_tf')
import rospy
import tf
import math
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Point
from visualization_msgs.msg import *
from rm_description.msg import pw
from nav_msgs.msg import Odometry
#from sympy import *
current_time=0
last_time=0
delta_time=0
x=0
y=0
z=0
thetachassis=0
angular_z=0
twist_x=0
yaw=0
pitch1=0
def callback(position):
	global x,y,z
	x=position.x 
	y=position.y
	z=position.z

def twcallback(twist):
    global twist_x
    global angular_z
    twist_x = twist.linear.x
    angular_z = twist.angular.z
def anglecallback(angle):
	global yaw
	global pitch1
	yaw=angle.yaw
	pitch1=-angle.pitch
	print("rviz.py")
	print(pitch1)
	print(yaw)

def linecallback(square):
	global zs_x
	global zs_y
	global ys_x
	global ys_y
	global yx_x
	global yx_y
	global zx_x
	global zx_y
	zs_x=square.zs_x
	zs_y=square.zs_y
	zx_x=square.zx_x
	zx_y=square.zx_y
	yx_x=square.yx_x
	yx_y=square.yx_y
	ys_x=square.ys_x
	ys_y=square.ys_y




if __name__=="__main__":
	rospy.init_node('my_tf_broadcaster')
	rospy.Subscriber('getpoint',Point, callback)
	rospy.Subscriber('cmd_vel', Twist, twcallback)
	rospy.Subscriber("control", pw, anglecallback)
	topic = 'visualization_marker_array'
	#topic2 = 'visualization_marker_line'
	topic3 = 'draw_bullet_tracks'
	topic4 = 'draw_target_points'
	publisher = rospy.Publisher(topic, MarkerArray,queue_size = 100)
	markerArray = MarkerArray()
	#publisherline = rospy.Publisher(topic2, Marker,queue_size = 100)
	publisherpoint = rospy.Publisher(topic4, Marker,queue_size = 100)
	count = 0
	MARKERS_MAX = 100
	while not rospy.is_shutdown():
	
		last_time=current_time
		current_time = rospy.Time.now().to_sec()
		delta_time=current_time-last_time
		br1 = tf.TransformBroadcaster()
		br2 = tf.TransformBroadcaster()
		thetachassis=thetachassis+angular_z*delta_time
		br1.sendTransform((x,y,z+0.7),
						tf.transformations.quaternion_from_euler(0, pitch1,yaw+thetachassis),
						rospy.Time.now(),
						"gimbal",
						"base_link")
		br2.sendTransform((x,y,z+0.2),
						tf.transformations.quaternion_from_euler(0, 0,thetachassis),
						rospy.Time.now(),
						"chassis",
						"base_link")

		marker = Marker()
  		marker.header.frame_id = "/base_link"
		marker.type = marker.SPHERE
		marker.action = marker.ADD
		marker.scale.x = 0.01
		marker.scale.y = 0.01
		marker.scale.z = 0.01
		marker.color.a = 1.0
		marker.color.r = 1.0
		marker.color.g = 1.0
		marker.color.b = 0.0
		marker.pose.orientation.w = 1.0
		if(count>1):
			count=0
		#if(count==0):
		v=10
		marker.pose.position.x = v*math.cos(yaw)*(1-math.pow(2.71828,-count))*math.cos(pitch1)+x
		marker.pose.position.y = v*math.cos(yaw)*(1-math.pow(2.71828,-count))*math.sin(pitch1)+y
		marker.pose.position.z = (9.8+v*math.sin(-yaw))*(1-math.pow(2.71828,-count))-9.8*count+0.35
			#a=x
			#b=y
		#elif(count!=0):
			#marker.pose.position.x = v*math.cos(yaw)*(1-math.pow(2.71828,-count))*math.cos(pitch1)+a
			#marker.pose.position.y = v*math.cos(yaw)*(1-math.pow(2.71828,-count))*math.sin(pitch1)+b
			#marker.pose.position.z = (9.8+v*math.sin(-yaw))*(1-math.pow(2.71828,-count))-9.8*count+1
		if(count > 0.1):
			markerArray.markers.pop(0)

		markerArray.markers.append(marker)
		id = 1
		for m in markerArray.markers:
			m.id = id
			id += 1
		publisher.publish(markerArray)
		count += 0.01
		rospy.sleep(0.01)

