# TOBII GLASSES 2 API IN CPP
[![Travis Build Status](https://travis-ci.org/whoshuu/cpr-example.svg?branch=master)](https://travis-ci.org/whoshuu/cpr-example) [![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/l7c4yti84r9i87ys?svg=true)](https://ci.appveyor.com/project/whoshuu/cpr-example)
this API is written in UBUNTU16.04,it's building by "catkin_make" ,so it can be used on ROS,and it will be updated.
IN this project,I used some prominent open source project,such as CPR,and tobii glasses 2 api in python3 and python2.7. This project is altered besed on [CPR example](https://github.com/whoshuu/cpr-example/).

CPR: [C++ Requests](https://github.com/whoshuu/cpr) is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.

This is a forkable repository that handles the boilerplate of building and integrating this library into a networked application.

## Building

This project and C++ Requests both use CMake. The first step is to make sure all of the submodules are initialized:

```
git submodule update --init --recursive
```
you should make a ROS project and used catkin_make to build.

#tobiiglass
# tobiiglass2_cpp
