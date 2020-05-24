/*    
功能：
控制哨兵底盘（差分轮）的速度
话题名：/chassis/cmd_vel
话题类型：geometry_msgs/Twist
------------------------------------
*/
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43 //R：方向键right   
#define KEYCODE_L 0x44 //L：方向键left     
#define KEYCODE_U 0x41 //U：方向键up      
#define KEYCODE_D 0x42 //D：方向键down
#define KEYCODE_Q 0x71 //q

#define VEL1 0.5  //加速
#define VEL2 -0.5 // 减速

class TeleopTurtle//定义了一个类
{
public:
  TeleopTurtle();
  void keyLoop();

private:

  
  ros::NodeHandle nh_;//含有句柄
  double linear_, angular_, l_scale_, a_scale_;//线速度等变量
  ros::Publisher twist_pub_;//含有发布者
  
};

TeleopTurtle::TeleopTurtle(): 
  linear_(0),
  angular_(0),
  l_scale_(1.0),//线速度
  a_scale_(1.0)//角速度
{
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);

  twist_pub_ = nh_.advertise<geometry_msgs::Twist>("/chassis/cmd_vel", 1);//发布的消息 固定格式
}

int kfd = 0;//定义 kfd 作用：文件描述符
struct termios cooked, raw;//定义 接口cooked 接口raw 
//termios 函数族提供了一个常规的终端接口，用于控制非同步通信端口 
//这个变量被用来提供一个健全的线路设置集合

void quit(int sig)//定义 quit函数 作用是处理SIGINT信号
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);
                                 
  
  ros::shutdown();//关闭节点
  exit(0);//终止进程
}

//主函数
int main(int argc, char** argv)
{
  ros::init(argc, argv, "keys");
  TeleopTurtle teleop_turtle;

  signal(SIGINT,quit);//相当于规定Ctrl+C的退出方式
                      //前一个参数表示要处理的信号 后一个参数表示信号的处理方式
                      //SIGINT /* interrupt */程序终止(interrupt)信号, 通常是Ctrl+C，用于通知前台进程组终止进程。
                      //当收到 SIGINT 信号 就调用quit（）函数

  teleop_turtle.keyLoop();//调用 keyLoop()函数
  quit(0);//调用quit函数 结束进程
  
  return(0);
}
//

void TeleopTurtle::keyLoop()//定义 用于键盘控制的函数 以下全是
{
  char c;//
  bool dirty=false;//定义 布尔型dirty 作用：控制在对应按键被按下后发布消息


                                                          
  tcgetattr(kfd, &cooked);//tcgetattr()与tcsetattr()一起使用，以确定实际数据传送速率
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;//c_cc control characters VEOL 11
  raw.c_cc[VEOF] = 2;//VEOF 4
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");//puts()向标准输出设备（屏幕）输出字符串并换行 
  puts("---------------------------");
  puts("Use arrow keys to move.'left'/'right' to move back/forward.'up'/'down' to stop. 'q' to quit.");


  for(;;)//无条件循环 
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)//如果已经读取到末尾，报错并退出
                            //read（）：函数从打开的设备或文件中读取数据。
                            //#include <unistd.h>    
                            //ssize_t read(int fd, void *buf, size_t count);
                            //count（这里是1）是请求读取的字节数，读上来的数据保存在缓冲区buf中，同时文件的当前读写位置向后移。     
                            //返回值：成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0
    {
      perror("read():");//perror（）：输出括号内的内容并且输出系统错误信息 <stdio.h>
      exit(-1);//退出程序 exit(0)表示程序正常退出，非0表示非正常退出
    }

    linear_=angular_=0;//线速度和角速度变量的初始化
    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)//如果是规定的按键 就开启dirty 使发布者发布消息
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        linear_ = VEL2;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        linear_ = VEL1;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        linear_ =0.01;
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        linear_ = 0.01;
        dirty = true;
        break;
      case KEYCODE_Q:
        ROS_DEBUG("quit");
        return;
    }
   

    geometry_msgs::Twist twist;//定义速度消息
    twist.angular.z = a_scale_*angular_;//设置 角速度信息
    twist.linear.x = l_scale_*linear_;//设置 线速度信息
    
    if(dirty ==true)//dirty开启则 发布信息 然后dirty关闭
    {
      twist_pub_.publish(twist); 
      dirty=false;
    }
  }


  return;
}



