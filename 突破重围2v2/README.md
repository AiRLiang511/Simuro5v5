# DLLStrategy

DLL策略项目

## 说明

该模板适用于Simuro5v5平台的[DLLAdapter](https://github.com/npuv5pp/V5DLLAdapter)项目。

新的接口函数包括：

- `void GetTeamInfo(TeamInfo* teaminfo)`

  用于指定策略信息，目前包含队名字段。

  参数`TeamInfo* teaminfo`**需要策略填充自身的信息**，会返回给平台。

- `void GetInstruction(Field* field)`

  比赛中的每拍被调用，**需要策略指定轮速**，相当于旧接口的Strategy。

  参数`Field* field`为`In/Out`参数，存储当前赛场信息，并允许策略修改己方轮速。

- `void GetPlacement(Field* field)`

  每次自动摆位时被调用，**需要策略指定摆位信息**。

  参数`Field* field`为`In/Out`参数，存储当前赛场信息，并允许策略修改己方位置（和球的位置）。

- `void OnEvent(EventType type, void* argument)`

  事件发生时被调用。

  参数`EventType type`表示事件类型；

  参数`void* argument`表示该事件的参数，如果不含参数，则为NULL。

## 其他

新的比赛接口函数所接受的参数类型为`Field`，里面包含了赛场信息，类似旧接口中的`Environment`，但是`field`中并不保存`gamestate`和`whosball`，因为这些变量只在回合开始时改变。

## 项目介绍

- 工程构建方法：cmack
  - cmake：构建的项目文件 **CMakeLists.txt**
- 项目打开方式：在文件夹右键，使用visual studio打开，等待cmake分析结束
- 项目文件介绍：
  1. out：项目构建生成的文件，包括最后生成的动态链接库
  2. Strategy：代码文件
     - src：源文件保存目录（.cpp）
       - dllmain：dll入口函数
       - stdafx.cpp：对应stdafx.h的实现文件
       - BaseRobot.cpp：机器人基类的实现源文件
       - main.cpp：策略主入口
     - header：头文件保存目标（.h）
       - targetver.h、stdafx.h、platform.h、adapter.h：平台文件
       - BaseRobot.h：机器人基类
       - constant.h：常量定义
       - globalVariable.h：全局变量声明
     - DLLStrategy.def：DLL导出函数名声明文件
  3. .gitattributes：git参数设置
  4. .gitignore：git版本控制忽略的文件
  5. CmakeLists.txt：项目构建文件
  6. CMakeSettings.json：cmake项目构建参数
  7. READEME.md：项目描述文件，markdown语法
