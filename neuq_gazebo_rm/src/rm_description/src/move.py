#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import Float64
import math
import random
T=random.randint(3,7)
A=1.5
PI=3.1415926
current_time=0
last_time=0
delta_time=0
theta=0
dafu_theta=0
def tuoluo_move_publisher():
    rospy.init_node('move_tuoluo', anonymous=True)
    tuoluo_move = rospy.Publisher('/single_2101t/single_2101t/swiveljoint1_position_controller/command', Float64, queue_size=10)
    dafu_move = rospy.Publisher('/single_2101t/single_2101t/swiveljoint2_position_controller/command', Float64, queue_size=10)
    rate = rospy.Rate(10) 

    while not rospy.is_shutdown():
	move=Float64()
	dafu=Float64()
	global last_time,current_time,delta_time,theta,dafu_theta
	last_time=current_time
	current_time = rospy.Time.now().to_sec()
	delta_time=current_time-last_time
	if(delta_time>10):
		delta_time=0
 	#w=A*math.sin((2*PI)/T*current_time)
        w=30
	theta=w*delta_time+theta
	#if theta>3.14159:
		#theta=theta-3.14159
	move.data=theta
	#print(move.data)
        tuoluo_move.publish(move)
        w2=0.785*math.sin(1.884*current_time)+1.305
	dafu_theta=w2*delta_time+dafu_theta
	dafu.data=dafu_theta
	dafu_move.publish(dafu.data)
	#print(dafu.data)

if __name__ == '__main__':
    try:
        tuoluo_move_publisher()
    except rospy.ROSInterruptException:
        pass

