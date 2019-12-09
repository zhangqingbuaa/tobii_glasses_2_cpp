# TOBII GLASSES 2 API IN CPP

this API is written in UBUNTU16.04,ROS kinetic,it's building by "catkin_make" 

IN this project,I used some prominent open source project,such as CPR,and tobii glasses 2 api in python3 and python2.7. This project is altered besed on [CPR example](https://github.com/whoshuu/cpr-example/).

CPR: [C++ Requests](https://github.com/whoshuu/cpr) is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.



## Building
make sure you have owned ROS package
```
mkdir catkin_ws
cd catkin_ws
mkdir src
```
```
git clone https://github.com/zhangqingbuaa/tobiiglass2_cpp
```
then you may need rename this  Folder as tobiiglass
```
git submodule update --init --recursive
```
in /catkin_ws   to build by catkin_make
```
cd ..
catkin_make
```
```
source ./devel/setup.bash
rosrun tobiiglass  main 
```

#tobiiglass
# tobiiglass2_cpp
# tobiiglass
