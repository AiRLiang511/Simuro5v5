cmake_minimum_required(VERSION 3.16)
project(Strategy)

set(CMAKE_CXX_STANDARD 17)

add_library(P5v5ARL SHARED
        Strategy/header/platform.h
        Strategy/header/stdafx.h
        Strategy/header/adapter.h
        Strategy/header/targetver.h
        Strategy/header/constant.h
        Strategy/header/globalVariable.h
        Strategy/header/BaseRobot.h

        Strategy/src/main.cpp
        Strategy/src/dllmain.cpp
        Strategy/src/stdafx.cpp
        Strategy/src/BaseRobot.cpp

        Strategy/DLLStrategy.def
        )