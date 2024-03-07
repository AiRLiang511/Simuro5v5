该项目为轮式足球机器人仿真赛所用代码
![b30b46a075218ea7e2901ce9fa5c921](https://github.com/AiRLiang511/Simuro5v5/assets/115909351/8a7afbe5-917b-4a57-a469-08225fcffa9c)

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
