#include "../header/BaseRobot.h"

#include <cmath>

#include "../header/constant.h"
#include "../header/globalVariable.h"
#include "fstream"
#include "iostream"

BaseRobot::BaseRobot() 
{
	sptr = new PID;
	initPid();
}

BaseRobot::~BaseRobot()
{
	delete sptr;
}

void BaseRobot::update(Simuro::Robot* robot)
{
	this->robot = robot;
}

void Balls::update(Simuro::Ball* ball)
{
	this-> ball= ball;
}

Simuro::Vector2 BaseRobot::getPos() 
{
	return robot->position;
}

Simuro::Vector2 BaseRobot::getLastPos()
{
	Simuro::Vector2 lastPos;
	lastPos.x = lastRobotX;
	lastPos.y = lastRobotY;
	return lastPos;
}

Simuro::Vector2 Balls::getLastPos()
{
	Simuro::Vector2 lastPos;
	lastPos.x = lastBallX;
	lastPos.y = lastBallY;
	return lastPos;
}

Simuro::Vector2 BaseRobot::togetLastPos()
{
	Simuro::Vector2 lastPoss;
	lastPoss.x = lastBallX;
	lastPoss.y = lastBallY;
	return lastPoss;
}

double BaseRobot::getRotation()
{
	return robot->rotation;
}

double BaseRobot::getLeftWheelVelocity()
{
	return robot->wheel.leftSpeed;
}

double BaseRobot::getRightWheelVelocity()
{
	return robot->wheel.rightSpeed;
}

double BaseRobot::getTarX()
{
	return lastTargetX;
}

double BaseRobot::getTarY()
{
	return lastTargetY;
}

void BaseRobot::initPid()
{
	sptr->lastError = 0;
	sptr->preError = 0;
	sptr->proportion = 2;
	sptr->integral = 0;
	sptr->derivative = 4.2;
}

//赋轮速
void BaseRobot::Velocity(double vl, double vr) {

	robot->wheel.leftSpeed = vl;
	robot->wheel.rightSpeed = vr;
}

//获得增量
double BaseRobot::pidCal(double nowPoint)
{
	int nowError,nowPid;
	nowError = 0 - nowPoint;
	nowPid = sptr->proportion * (nowError - sptr->lastError) +  sptr->integral * nowError + sptr->derivative * (nowError - 2 * sptr->lastError + sptr->preError);
	sptr->preError = sptr->lastError;
	sptr->lastError = nowError;
	return nowPid;
}

void BaseRobot::move(double x,double y)
{
	 BaseRobot::moveTo(x, y);
	 if (BaseRobot::getPos().x >= x - 2 && BaseRobot::getPos().x <= x + 2 && BaseRobot::getPos().y >= y - 2 && BaseRobot::getPos().y <= y + 2)
	 {
		 BaseRobot::Velocity(0, 0);

	 }
	
}
void BaseRobot::leftspin(double x, double y)
{
	BaseRobot::moveTo(x, y);
	if (BaseRobot::getPos().x >= x - 2 && BaseRobot::getPos().x <= x + 2 && BaseRobot::getPos().y >= y - 2 && BaseRobot::getPos().y <= y + 2)
	{
		BaseRobot::Velocity(125, -125);

	}

}
void BaseRobot::rightspin(double x, double y)
{
	BaseRobot::moveTo(x, y);
	if (BaseRobot::getPos().x >= x - 2 && BaseRobot::getPos().x <= x + 2 && BaseRobot::getPos().y >= y - 2 && BaseRobot::getPos().y <= y + 2)
	{
		BaseRobot::Velocity(-125, 125);

	}

}

 void BaseRobot::stop(void)
 {
	 BaseRobot::Velocity(0, 0);
 }


//方法2使用PID经行调参
void BaseRobot::moveTo(double x, double y)
{
	if (tick == 1 || tick == 2 || tick % 100 == 0)
	{
		initPid();
		lastU = 0;
		lastU1 = 0;
	}
	double parameter = 0.5;//参数，调整速度用
	double vMax = 125; //最大速度,方便后边用sigmoid函数调整速度大小
	double vL, vR;
	double angleTo = 0, angleDiff = 0; //angleTo:足球机器人和小球的角度
	double dX = x - robot->position.x;
	double dY = y - robot->position.y;
	double dR2RX = robot->position.x - lastRobotX;
	double dR2RY = robot->position.y - lastRobotY;
	angleTo = 180. / M_PI * atan2(dY,dX);
	double distance = sqrt(dX * dX + dY * dY);
	angleDiff = robot->rotation - angleTo;
	//正则化夹角
	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;
	//lastU = lastU + getPwm(angleDiff);
	if (fabs(angleDiff) < 85)
	{
		lastU = lastU + pidCal(angleDiff); //lastU是上一次的控制量
		vR = vMax + lastU;
		vL = vMax - lastU;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		lastU = lastU + pidCal(angleDiff);
		vR = -vMax + lastU;
		vL = -vMax - lastU;
	}
	else
	{
		vR = 80;
		vL = -80;
	}

	Velocity(vL, vR);
}



void BaseRobot::gogo(double x, double y)
{

}

void BaseRobot::moveto_within_x_limits(double x_limits, double tar_x, double tar_y)
{
	if (this->getPos().x > x_limits)
	{
		this->moveTo(x_limits, tar_y);
	}
	else
	{
		if (tar_x > x_limits)
		{
			this->moveTo(x_limits, tar_y);
		}
		else
		{
			this->moveTo(tar_x, tar_y);
		}
	}
}


void BaseRobot::saveLastInformation(double footBallNow_X, double footBallNow_Y)
{
	lastRotation = robot->rotation;
	if (tick % 10 == 0)
	{
		lastRobotX = robot->position.x;
		lastRobotY = robot->position.y;
	}

}

void Balls::saveLastInformation(double footBallNow_X, double footBallNow_Y)
{
	
	if (tick % 10 == 0)
	{
		lastBallX = ball->position.x;
		lastBallY = ball->position.y;
	}

}

int DataLoader::get_event(int tick)
{
    return this->event_states[tick];
}

void DataLoader::set_tick_state(int tick, int event_state)
{
    this->tick=tick;
    this->event_states[tick]=event_state;
}

