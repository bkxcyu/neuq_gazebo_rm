################################################################  
# 功能包结构：  
rm_description包括步兵的模型文件和配置文件。  
sentry_gazebo包括哨兵的模型文件和配置文件。  
PK包括2v2比赛场地和步兵竞速与智能射击比赛场地的模型文件和一些配置文件。  

# 下载编译：  
#### 1.拷贝整个目录到你本地。  
#### 2.下载依赖项：  
如果不是ros自带的gazebo，要`sudo apt-get install ros-kinetic-gazebo-ros-pkgs ros-kinetic-gazebo-ros-control`安装ros和gazebo的接口  
`$> sudo apt-get update`  
`$> rospack profile`  
`$> sudo apt-get install ros-kinetic-position-controllers`  
`$> sudo apt-get install ros-kinetic-effort-controllers`  
`$> sudo apt_get install ros-kinetic-joint-state-controller`  
#### 3.加执行权限并编译：
`$> cd neuq_gazebo_rm`  
`$> catkin_make`  
`$> cd neuq_gazebo_rm/src/rm_description/src`  
`$> chmod u+x move.py`  
`$> chmod u+x rviz.py`  


################################################################  
# 步兵仿真：  
步兵仿真目前仅可用于测试自瞄，由于gazebo插件bug导致的漂移问题（转弯速度过快或下坡上坡会导致步兵倾斜）尚未解决，所以还不能测试飞坡。  

## 一、通信结构：  
`gazebo_teleop.cpp`	gazebo中步兵模型运动控制文件  
`getpoint.cpp`		获取gazebo中步兵实际位置传输给rviz用于更新rviz坐标轴  
`move.py`		控制gazebo中的能量机关和小陀螺按指定规律运动  
`rviz.py`			rviz通信文件，炮弹和tf关系更新  


## 二、使用方式：
#### 使用gazebo仿真：
1.`roslaunch rm_description test.launch`（在gazebo中加载场地模型和步兵模型）
2.`roslaunch rm_description gazebo_node.launch`(加载gazebo文件，在此终端可键盘操控)
#### 使用rviz仿真：(关掉了gazebo仿真图像界面，接受gazebo步兵位置发送给rviz用于更新坐标，gazebo运行太卡可以用这个，rviz中能量机关和陀螺是静止的只是用于参考相对位置,建议imshow显示gazebo摄像头画面，这样更加准确)
1.roslaunch rm_description rviz.launch（gazebo加载步兵文件）
2.roslaunch single_2101t display.launch（rviz加载单项赛场地）
3.roslaunch rm_description rviz_node.launch（加载rviz文件，在此终端可键盘操控）


## 三、接口说明：
总的来说就是接收gazebo虚拟世界的图像信息进行识别，计算出打角再输入到gazebo环境中进行测试。
1.通过订阅gazebo中仿真相机的画面用于测试识别，订阅话题名为/swivel/rrbot/camera1/image_raw。（消息类型为图像）
2.通过向gazebo发送计算出的云台角度更新gazebo画面用于测试自瞄，发送话题名为"/swivel/swivel/swiveljoint2_position_controller/command"（pitch）和
"/swivel/swivel/swiveljoint1_position_controller/command"（yaw）。（消息类型为std_msgs中的Float64）
3.要实时修改rviz坐标轴转角的话需要将计算出的pitch和yaw，通过control的消息名发送给rviz.py，自定义消息类型已给出,在rm_description里面的msg。


##四、键盘操控：
键盘节点在gazebo和rviz的launch中都有，在终端中输入字符就能控制。
wsad分别对应底盘前后左右，大写WSAD是增加对应方向的速度。
ikjl是云台的上下左右。


##五、其他补充
黄色小块是相机。红色小块不用在意。
为了方便测试自瞄我们把相机放在了炮筒口，想要修改相机位置在步兵的urdf文件里修改即可。

################################################################
# 哨兵仿真：
## 一、文件结构：
aTRACK - 哨兵轨道sdf模型
gimbal_urdf - 哨兵云台相关文件
mbot_description - 哨兵底盘相关文件
sample - 控制哨兵在轨道上运动、获取gazebo中某个模型的位置的代码示例


## 二、使用方式：
1.打开一个世界 例如roslaunch mbot_description world.launch
2.把模型放到轨道上（可以在左栏输入mrobot的位置或者停住时间手动拖到轨道上，详见图片示例）
3.启动测试的代码 例如 rosrun sample key_ch

#### 提示：
如果崩了缓一缓重新开一次
用虚拟机开gazebo记得把显示器-3D图形-加速3D图形关闭


## 三、接口说明：
#### get_pos.cpp：
功能：获取gazebo里‘mrobot’模型的位置
话题名称：/gazebo/model_states
话题类型：gazebo_msgs/ModelStates

#### key_ch.cpp：
功能：控制哨兵底盘（差分轮）的速度
话题名：/chassis/cmd_vel
话题类型：geometry_msgs/Twist
使用说明：按上/下键停止移动，按左键沿x轴逆方向0.5m/s移动，按右键沿x轴正方向0.5m/s移动，q键结束


## 四、其他补充：
==========================
#### sentry_gazebo/mbot_description/launch
==========================
world.launch（true.world）： 轨道+底盘（未放置，需要手动拖到轨道）
albot.launch（true.world）： 轨道+底盘带云台（未放置，需要手动拖到轨道）

world文件夹里fake.world，是轨道+底盘已放置在轨道上，可以在world.launch中替换true.world使用
（可能开的时候会有报错，可以使用）
##### *放置技巧
可以在左栏的pose属性里调整
在最下有个时间可以暂停，这样放的时候物体就不会乱动了

=============================
#### sentry_gazebo/mbot_description/urdf/xacro
=============================
robot前缀的是只有底盘的模型
albot前缀的是底盘连云台的模型（这版云台还没有实际功能）
################################################################
