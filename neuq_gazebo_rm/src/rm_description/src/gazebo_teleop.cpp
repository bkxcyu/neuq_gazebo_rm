
    #include <termios.h>
    #include <signal.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/poll.h>
    #include<std_msgs/Float64.h>
    #include <boost/thread/thread.hpp>
    #include <ros/ros.h>
    #include <geometry_msgs/Twist.h>
     
    #define KEYCODE_W 0x77
    #define KEYCODE_A 0x61
    #define KEYCODE_S 0x73
    #define KEYCODE_D 0x64
    #define KEYCODE_N 0x6D
    #define KEYCODE_M 0x6E

    #define KEYCODE_K 0x6B
    #define KEYCODE_I 0x69
    #define KEYCODE_J 0x6A
    #define KEYCODE_L 0x6C

    #define KEYCODE_A_CAP 0x41
    #define KEYCODE_D_CAP 0x44
    #define KEYCODE_S_CAP 0x53
    #define KEYCODE_W_CAP 0x57
     
    class SmartCarKeyboardTeleopNode
    {
        private:
            double walk_vel_;
            double run_vel_;
            double yaw_rate_;
            double yaw_rate_run_;
            
            geometry_msgs::Twist cmdvel_;
	    std_msgs::Float64 msg;
	std_msgs::Float64 yaw;
            ros::NodeHandle n_;
            ros::Publisher pub_;
	    ros::Publisher pub_2;
	    ros::Publisher pub_3;

     
        public:
            SmartCarKeyboardTeleopNode()
            {
                pub_ = n_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);

                pub_2 = n_.advertise<std_msgs::Float64>("/swivel/swivel/swiveljoint2_position_controller/command", 1);
		pub_3 = n_.advertise<std_msgs::Float64>("/swivel/swivel/swiveljoint1_position_controller/command", 1);

                ros::NodeHandle n_private("~");
                n_private.param("walk_vel", walk_vel_, 0.5);
                n_private.param("run_vel", run_vel_, 1.0);
                n_private.param("yaw_rate", yaw_rate_, 1.0);
                n_private.param("yaw_rate_run", yaw_rate_run_, 1.5);
            }
            
            ~SmartCarKeyboardTeleopNode() { }
            void keyboardLoop();
            
            void stopRobot()
            {
                cmdvel_.linear.x = 0.0;
                cmdvel_.angular.z = 0.0;

		msg.data=0.0;	
		yaw.data=0.0;
                pub_.publish(cmdvel_);
		pub_2.publish(msg);
		pub_3.publish(yaw);

            }
    };
     
    SmartCarKeyboardTeleopNode* tbk;
    int kfd = 0;
    struct termios cooked, raw;
    bool done;
     
    int main(int argc, char** argv)
    {
        ros::init(argc,argv,"tbk", ros::init_options::AnonymousName | ros::init_options::NoSigintHandler);
        SmartCarKeyboardTeleopNode tbk;
        
        boost::thread t = boost::thread(boost::bind(&SmartCarKeyboardTeleopNode::keyboardLoop, &tbk));
        
        ros::spin();
        
        t.interrupt();
        t.join();
        tbk.stopRobot();
        tcsetattr(kfd, TCSANOW, &cooked);
        
        return(0);
    }
     
    void SmartCarKeyboardTeleopNode::keyboardLoop()
    {
        char c;
        double max_tv = walk_vel_;
        double max_rv = yaw_rate_;
        bool dirty = false;
bool gim_pitch = false;
bool gim_yaw = false;
        int speed = 0;
        int speed2 = 1;
        int turn = 0;
	int turn2 = 1;
	float bian=0;
	float bian_yaw=0; 
        // get the console in raw mode
        tcgetattr(kfd, &cooked);
        memcpy(&raw, &cooked, sizeof(struct termios));
        raw.c_lflag &=~ (ICANON | ECHO);
        raw.c_cc[VEOL] = 1;
        raw.c_cc[VEOF] = 2;
        tcsetattr(kfd, TCSANOW, &raw);
        
        puts("Reading from keyboard");
        puts("Use WASD keys to control the robot");
        puts("Press Shift to move faster");
        
        struct pollfd ufd;
        ufd.fd = kfd;
        ufd.events = POLLIN;
        
        for(;;)
        {
            boost::this_thread::interruption_point();
            
            // get the next event from the keyboard
            int num;
            
            if ((num = poll(&ufd, 1, 250)) < 0)
            {
                perror("poll():");
                return;
            }
            else if(num > 0)
            {
                if(read(kfd, &c, 1) < 0)
                {
                    perror("read():");
                    return;
                }
            }
            else
            {
                if (dirty == true)
                {
                    stopRobot();
                    dirty = false;
                }
                
                continue;
            }
            
            switch(c)
            {
                case KEYCODE_W:
                    max_tv = walk_vel_;
                    speed = speed2;
                    turn = 0;
                    dirty = true;
                    break;
                case KEYCODE_S:
                    max_tv = walk_vel_;
                    speed = -speed2;
                    turn = 0;
                    dirty = true;
                    break;
                case KEYCODE_A:
                    max_rv = yaw_rate_;
                    speed = 0;
                    turn = -turn2;
                    dirty = true;
                    break;
                case KEYCODE_D:
                    max_rv = yaw_rate_;
                    speed = 0;
                    turn = turn2;
                    dirty = true;
                    break;

                case KEYCODE_W_CAP:
                    max_tv = run_vel_;
                    speed2 += 1;
                    turn = 0;
                    dirty = true;
	    std::cout<<"speed"<<speed2<<std::endl;
                    break;
                case KEYCODE_S_CAP:
                    max_tv = run_vel_;
                    speed2 += -1;
	    std::cout<<"speed"<<speed2<<std::endl;
                    turn2 = 0;
                    dirty = true;
                    break;
                case KEYCODE_A_CAP:
                    max_rv = yaw_rate_run_;
                    speed = 0;
                    turn2 += -1;
	    std::cout<<"turn"<<turn2<<std::endl;
                    dirty = true;
                    break;
                case KEYCODE_D_CAP:
                    max_rv = yaw_rate_run_;
                    speed = 0;
                    turn2 += 1;
	    std::cout<<"turn"<<turn2<<std::endl;
                    dirty = true;
                    break;
               case KEYCODE_I:

                    bian += 0.1;
                    gim_pitch = true;
                    break;
               case KEYCODE_K:

                    bian += -0.1;
                    gim_pitch = true;
                    break;
               case KEYCODE_J:

                    bian_yaw += 0.1;
                    gim_yaw = true;
                    break;
               case KEYCODE_L:

                    bian_yaw += -0.1;
                    gim_yaw = true;
                    break;
               case KEYCODE_N:

                    bian_yaw = 0;
		bian=0;
                    gim_yaw = true;
gim_pitch=true;
                    break;


                default:
                    max_tv = walk_vel_;
                    max_rv = yaw_rate_;
                    speed = 0;
                    turn = 0;
                    dirty = true;


            }
	if(dirty==true){
            cmdvel_.linear.x = speed * max_tv;
            cmdvel_.angular.z = turn * max_rv;
            pub_.publish(cmdvel_);
		dirty=false;
}
	if(gim_pitch==true){
		msg.data=bian;
		pub_2.publish(msg);
		gim_pitch=false;}
	if(gim_yaw==true){
		yaw.data=bian_yaw;
		pub_3.publish(yaw);
		gim_yaw=false;

		}
	
        }
    }

