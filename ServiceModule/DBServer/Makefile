#++++++++++++++++++++++++++++++++++++++++++#
#平台定义
ifeq ($(PLATFORM),)
PLATFORM=x64-linux
endif

#++++++++++++++++++++++++++++++++++++++++++#
#得到顶层目录【makefile.rules目录的父目录】绝对路径
TOPDIR=$(shell sh -c '\
        dir=`pwd|sed s/[/]$$//`; \
        while test $$dir; do \
                if test -d $$dir/rules; then \
                        echo $$dir; \
                        exit; \
                fi; \
                dir=`echo $$dir|sed s/[/][^/]*$$//`; \
        done \
')

# 用户设定

#++++++++++++++++++++++++++++++++++++++++++#
#EXECUTABLE 是最终目标文件名，自己设定
#eg : EXECUTABLE := test.exe

NO_GB2312=1

#MAKE_STATIC_LIB := 1
#MAKE_SHARED_LIB := 1
EXECUTABLE := DBServer

#++++++++++++++++++++++++++++++++++++++++++#
#INCLUDE_PATH 是用户创建include 文件的搜索附加路径
INCLUDE_PATH := $(SYS_INCLUDE_PATH) \
		-I./ \
		-I./include \
		-I../../SGPub \
		-I../../SGPub/ext \
		-I../../SGPub/ext/jsoncpp \
		-I../../SGPub/roselib/include \
		-I../../SGPub/rosecore/include \
		-I../../SGPub/rosetcp/include \
		-I../SGSHeader \
		-I../../SGPub/roselog/include

#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#DEFINES 用户自定义宏
DEFINES := -DXP_UNIX -DOS_LINUX
#++++++++++++++++++++++++++++++++++++++++++#

#SYS_LIBRARY_PATH为系统lib文件路径的环境变量(在这里添加so文件路径,eg : -L../ -L../../)
LIBRARY_PATH := $(SYS_LIBRARY_PATH) -L../../SGPub/ext/mysql/linux/mysql-connector-c-6.0.2-linux-glibc2.3-x86-64bit/lib

#需要对DEBUG和RELEASE版本link不同的动态库(.so)的情况

#}}}}连接库包含设定++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#LIBS 是本目标程序需要连接的动态库(.so)列表
#带-l前缀，只写该动态库的link名，例如stdc++表示会link在LIBRARY_PATH路径下的 libstdc++.so
LIBS := -lstdc++ -lpthread -lrt -ldl -lmysql


#不带-l前缀，且需要写出该静态库的全路径，例如 ../../lib/xxxlib.a
USERLIBS := 
#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
# 默认SOURCE为makefile目录的./source下的所有C/C++源文件
SOURCE := $(wildcard ./source/*.cpp) 
_SOURCE := 

# 源文件最终集合 = [SOURCE集] - [_SOURCE集]
SOURCE := $(filter-out $(_SOURCE),$(SOURCE))

#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
# C++预编译设置，C语言不支持预编译
#PRECOMPILE_HEADER := ./stdafx.h

#++++++++++++++++++++++++++++++++++++++++++#

-include local.$(PLATFORM).defs
include $(TOPDIR)/rules/rules.$(PLATFORM)
