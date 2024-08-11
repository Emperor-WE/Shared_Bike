CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#project name
PROJECT(shared_bike)

#用来显示的定义变量
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -g3 -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations -fpermissive")

#将指定目录添加到编译器的头文件搜索路径之下
INCLUDE_DIRECTORIES(../third/include)
INCLUDE_DIRECTORIES(./common)

#将指定目录添加到需要链接的库文件目录之下
LINK_DIRECTORIES(../third/lib/iniparser)
LINK_DIRECTORIES(../third/lib/log4cpp)
LINK_DIRECTORIES(./common)

#搜集所有在指定路径下的源文件的文件名，将输出结果列表存储在指定的变量中
#n内置变量：CMAKE_SOURCE_DIR 定义了顶级CmakeLists.txt所在的文件夹，PROJECT_SOURCE_DIR 定义了包含最近的 project()命令的CmakeLists.txt所在的文件夹
aux_source_directory(${PROJECT_SOURCE_DIR} SOURCE_FILES)

#使用给定的源文件，为工程引入一个可执行文件
ADD_EXECUTABLE(shared_bike ${SOURCE_FILES})

#该指令的作用为将目标文件与库文件进行链接
TARGET_LINK_LIBRARIES(shared_bike iniparser)
TARGET_LINK_LIBRARIES(shared_bike log4cpp)
TARGET_LINK_LIBRARIES(shared_bike common)
#PROJECT_NAME就相当于 prject_name -- shared_bike
TARGET_LINK_LIBRARIES(${PROJECT_NAME} liblog4cpp.a)
#增加子目录
ADD_SUBDIRECTORY(common)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
INSTALL (TARGETS shared_bike DESTINATION bin)