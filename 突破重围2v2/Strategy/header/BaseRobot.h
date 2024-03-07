#include "platform.h"
#include "string"



class PID
{
public:
	PID(double proportion, double integral, double derivative)
	{
		this->proportion = proportion;
		this->integral = integral;
		this->derivative = derivative;
	}
	//获得增量
	double pidCal(double nowError)
	{
		this->nowError = nowError;
		double delta_v = lastU + this->proportion * (nowError - this->lastError) + this->integral * nowError + this->derivative * (nowError - 2 * this->lastError + this->preError);
		return delta_v;
	}
	void updatePID()
	{
		this->lastU += this->proportion * (nowError - this->lastError) + this->integral * nowError + this->derivative * (nowError - 2 * this->lastError + this->preError);
		this->preError = this->lastError;
		this->lastError = this->nowError;
	}
	void resetPID()
	{
		this->lastError = 0;
		this->preError = 0;
		this->lastU = 0;
	}
private:
	double proportion; //比例常数
	double integral; //积分常数 
	double derivative; //微分常数
	double lastError = 0;	// 1拍前误差
	double preError = 0;	// 2拍前误差
	double nowError;	// 现在误差
public:
	double lastU = 0;	//增量式pid计算的值
};


/*
机器人基类
维护机器人信息和基本动作（跑、转、甩）
子类建议只在基本动作基本上封装简单动作，更复杂的动作作为单人策略
信息复杂计算使用计算类
*/

class BaseRobot
{
public:
	BaseRobot();	// 构造函数
	virtual ~BaseRobot();	// 析构函数

	void update(Simuro::Robot* robot, bool reset);	// 将平台提供的环境信息赋给BaseRobot，每拍更新，reset=1表示重新摆位，需要清空历史数据
	void new_move(double x, double y);
	void new_new_move(double x, double y);
	void saveLastInformation(double footBallNow_X, double footBallNow_Y);	// 保存机器人本拍信息，留作下一拍使用，可用于拓展保存更多信息，每拍结束前必须调用，以维护pid等数据

	Simuro::Vector2 getPos();	// 获取机器人位置
	void move(double x,double y);
	Simuro::Vector2 getLastPos();	// 获取机器人上一拍位置

	double getRotation();	// 获取机器人旋转角

	double getLeftWheelVelocity();	// 获取机器人左轮速

	double getRightWheelVelocity();	// 获取机器人右轮速

	double getTarX();	// 获取机器人跑位目标点x坐标

	double getTarY();	// 获取机器人跑位目标点y坐标

	void Velocity(double vl, double vr);	// 直接赋左右轮速

	void moveTo(double tarX, double tarY);	// 跑位函数，跑位到(tarX,tarY)

	void turnToAngle(double tar_angle);	// 机器人原地旋转，固定到某个角度

	void throwBall(double ballx, double bally);	// 机器人跑位到球左边把球甩出去

	void shoot(double ballx, double bally);	// 带球到禁区内，并甩球射门

	void breakThrough(BaseRobot oppRobots[5], double tarx, double tary);	// 机器人运动到目标点，同时尽量避免碰撞地方机器人

	void moveWithAngle(double tarX, double tarY, double tar_angle);	// 机器人跑位到某点，并固定在某个角度

	void keepYPushBall(double keepY1, double keepY2, double footBallNow_X, double footBallNow_Y);	// 进攻函数，keepY表示保证机器人横坐标不会超过某一个给定值keepY

	void PredictRobotInformation(int tick_delay);	// 预测自己tick_delay拍后的位置和状态
	void leftspin(double x, double y);
	void rightspin(double x, double y);
	Simuro::Robot GetRobotInformation(int time);	// 获得机器人近time拍的状态，time < 0获得历史状态，time > 0获得预测状态
private:
	Simuro::Robot* robot = nullptr;

	double lastTargetX = 0, lastTargetY = 0;	// 机器人上一拍目标点
	double lastRobotX = 0, lastRobotY = 0;	// 机器人上一拍位置

	double lastRotation = 0;	// 机器人上一拍旋转角
public:
	Simuro::Robot HistoryInformation[8] = { {{0,0},0,{0,0}} };	// 存储机器人的历史8拍的信息
	Simuro::Robot PredictInformation[8] = { {{0,0},0,{0,0}} };	// 存储预测出的机器人未来8拍的信息

	//Simuro::Robot PdtInformation[40000] = { {{0,0},0,{0,0}} };	// 记录机器人在tick拍下的信息，用于打印数据，平时注释掉即可
	//Simuro::Robot TrueInformation[40000] = { {{0,0},0,{0,0}} };

	double AngularSpeed[8] = { 0 };	// 存储预测出的机器人近8拍的角速度信息

	//初始化PID参数
public:
	bool reset = false;	// 如果机器人重新摆位，则需要重置
	PID pid_moveTo = PID(2, 0,4.2);// PID(1.9, 0, 2.2);// PID(0.52, 0, 2.2);
	PID pid_angle = PID(2.0, 0, 4.2);//PID(2, 0, 4.2);
	PID pid_new = PID(4, 0,15);//PID(2.4, 0, 4);//2.4 0 1   4 0 10
	PID pid_angle_new = PID(0.4, 0, 0.15);//PID(2, 0, 4.2);0.45 0 0.1 
};

class DataLoader
{
public:
	int get_event(int tick);//获得tick时刻的比赛状态
	void set_tick_state(int tick, int event_state);//设置此时的信息
private:
	int tick;	// 定义当前所在的拍数
	int event_states[100000];	// 用于存储事件状态

};