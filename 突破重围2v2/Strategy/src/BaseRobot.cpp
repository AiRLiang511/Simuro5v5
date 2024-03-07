#include "../header/BaseRobot.h"

#include <cmath>

#include "../header/constant.h"
#include "../header/globalVariable.h"
#include "fstream"
#include "iostream"

// 构造函数
BaseRobot::BaseRobot()
{

}

// 析构函数
BaseRobot::~BaseRobot()
{

}

/*用于更新BaseRobot数据
*因为BaseRobot是我们自己的定义的类型，平台每拍只会给Robot类型传数据，所以需要我们自己把Robot类的数据传给BaseRobot
*另外当重新摆位后许多历史数据失效，需要清空，通过设置reset实现。
*/
void BaseRobot::update(Simuro::Robot* robot, bool resetHistoryRecord)
{
	this->robot = robot;	// 从平台传的robot数据获取baseRobots的基础信息
	this->reset = resetHistoryRecord;	// 摆位后重新记录历史数据
	if (reset == true)	// 如果需要重新记录历史数据，所以历史数据设置为现在数据
	{
		for (int i = 0; i < 8; i++)
		{
			HistoryInformation[i] = *robot;
		}
	}
	else
	{
		HistoryInformation[0] = *robot;	// 当前拍历史数据即为现在数据
	}
}

// 用于读取当前状态的位置
Simuro::Vector2 BaseRobot::getPos()
{
	return robot->position;
}

// 用于读取上一拍位置
Simuro::Vector2 BaseRobot::getLastPos()
{
	Simuro::Vector2 lastPos;
	lastPos.x = lastRobotX;
	lastPos.y = lastRobotY;
	return lastPos;
}

// 用于获取机器人旋转角
double BaseRobot::getRotation()
{
	return robot->rotation;
}

// 用于获取机器人左轮速
double BaseRobot::getLeftWheelVelocity()
{
	return robot->wheel.leftSpeed;
}

// 用于获取机器人右轮速
double BaseRobot::getRightWheelVelocity()
{
	return robot->wheel.rightSpeed;
}



// 获取机器人跑位目标点x坐标
double BaseRobot::getTarX()
{
	return lastTargetX;	//尚无维护，用户可根据需要自行维护
}

// 获取机器人跑位目标点y坐标
double BaseRobot::getTarY()
{
	return lastTargetY;	//尚无维护，用户可根据需要自行维护
}


//赋轮速
void BaseRobot::Velocity(double vl, double vr)
{
	robot->wheel.leftSpeed = vl;
	robot->wheel.rightSpeed = vr;
}


// 重载运算符 - ，用于位置间的减法，直接计算出距离
double operator-(Simuro::Vector2 a, Simuro::Vector2 b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	double distance = sqrt(dx * dx + dy * dy);
	return distance;
}

/*获取前后time拍的信息
*time > 0 预测后time拍
*time < 0, 历史0-time拍
*/
Simuro::Robot BaseRobot::GetRobotInformation(int time)
{
	if (time >= 0)
	{
		return PredictInformation[time];
	}
	else
	{
		return HistoryInformation[-time];
	}
}

void BaseRobot::PredictRobotInformation(int tick_delay)	// 预测自己n拍后的状态
{
	double delta_t = 1.0;	// 两次调用间隔一拍

	PredictInformation[0] = *robot;	// 预测0拍后的信息应为本拍信息
	AngularSpeed[0] = AngularSpeed[1];	//当前的预测角速度应为对于上一拍而言的下一拍角速度
	for (int i = 1; i <= tick_delay; i++)	// 预测第i拍数据
	{
		double PredictedlastRotation = GetRobotInformation(i - 1).rotation;	//i-1拍旋转角
		Simuro::Vector2 PredictedLastPos = GetRobotInformation(i - 1).position;	// i-1拍位置
		double PredictedLineSpeed = (PredictedLastPos - GetRobotInformation(i - 2).position) / delta_t;// i-1拍线速度
		double angle_logic = atan((PredictedLastPos.y - GetRobotInformation(i - 2).position.y) / (PredictedLastPos.x - GetRobotInformation(i - 2).position.x)) * 180 / M_PI;	//逻辑角度
		if (PredictedLastPos.x < GetRobotInformation(i - 2).position.x)
		{
			if (angle_logic > 0)
			{
				angle_logic = 180 - angle_logic;
			}
			else
			{
				angle_logic = -180 - angle_logic;
			}

		}
		double delta_angle = angle_logic - GetRobotInformation(i - 1).rotation;
		if (delta_angle > 180) delta_angle -= 360;
		if (delta_angle < -180) delta_angle += 360;
		if (fabs(delta_angle) > 90)
		{
			PredictedLineSpeed = -PredictedLineSpeed;	// 根据逻辑角度算出带方向的速度
		}

		double PiedictedAngularSpeed;	// 计算推测的角速度
		if (AngularSpeed[i - 1] > 0)
		{
			if (PredictedlastRotation > GetRobotInformation(i - 2).rotation)
			{
				PiedictedAngularSpeed = (PredictedlastRotation - GetRobotInformation(i - 2).rotation) / delta_t;	//i-1 拍角速度
			}
			else
			{
				PiedictedAngularSpeed = (360 + PredictedlastRotation - GetRobotInformation(i - 2).rotation) / delta_t;
			}
		}
		else
		{
			if (PredictedlastRotation > GetRobotInformation(i - 2).rotation)
			{
				PiedictedAngularSpeed = (PredictedlastRotation - GetRobotInformation(i - 2).rotation - 360) / delta_t;	//i-1 拍角速度
			}
			else
			{
				PiedictedAngularSpeed = (PredictedlastRotation - GetRobotInformation(i - 2).rotation) / delta_t;
			}
		}


		Simuro::Wheel settedWheelSpeed = HistoryInformation[tick_delay + 1 - i].wheel;	// 延迟到第i拍会获得的轮速
		if (settedWheelSpeed.leftSpeed > 125)
		{
			settedWheelSpeed.leftSpeed = 125;
		}
		else if (settedWheelSpeed.leftSpeed < -125)
		{
			settedWheelSpeed.leftSpeed = -125;
		}
		if (settedWheelSpeed.rightSpeed > 125)
		{
			settedWheelSpeed.rightSpeed = 125;
		}
		else if (settedWheelSpeed.rightSpeed < -125)
		{
			settedWheelSpeed.rightSpeed = -125;
		}

		// 预测模型的一些参数
		double K1 = 0.002362192;
		double K2 = exp(-1 / 0.9231);
		double K3 = 1 - exp(-1 / 3.096);
		double K4 = 0.53461992;
		double nextLineSpeed;	// 在第i-1拍预测到的i拍线速度

		if (settedWheelSpeed.leftSpeed != 0 && settedWheelSpeed.rightSpeed != 0)
		{
			nextLineSpeed = PredictedLineSpeed * 0.939534127623834133 + (settedWheelSpeed.leftSpeed + settedWheelSpeed.rightSpeed) / 2 * K1;
		}
		if (settedWheelSpeed.leftSpeed == 0 || settedWheelSpeed.rightSpeed == 0)
		{
			nextLineSpeed = PredictedLineSpeed * K2;
		}
		double nextAngularSpeed = PiedictedAngularSpeed + ((settedWheelSpeed.rightSpeed - settedWheelSpeed.leftSpeed) / 2 * K4 - PiedictedAngularSpeed) * K3;
		AngularSpeed[i] = nextAngularSpeed;

		double newLineSpeed = nextLineSpeed;//先算线速度然后根据线速度算出位置？
		double newAngularSpeed = nextAngularSpeed;// 先算角速度然后根据角速度算出角度？（假设角速度ni时针为正）

		PredictInformation[i].position.x = PredictInformation[i - 1].position.x + newLineSpeed * cos(PredictInformation[i - 1].rotation / 180 * M_PI);
		PredictInformation[i].position.y = PredictInformation[i - 1].position.y + newLineSpeed * sin(PredictInformation[i - 1].rotation / 180 * M_PI);
		PredictInformation[i].rotation = PredictInformation[i - 1].rotation + newAngularSpeed;
		while (PredictInformation[i].rotation > 180)
			PredictInformation[i].rotation -= 360;
		while (PredictInformation[i].rotation < -180)
			PredictInformation[i].rotation += 360;

		// 其中历史信息需要单独维护，在main getinstruction每次函数结束前更新历史信息
		// 如果进入摆位函数，则历史信息清零。（reset = true）

	}
}




/**
double BaseRobot::move(double x, double y, int z)
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
	angleTo = 180. / M_PI * atan2(dY, dX);
	double distance = sqrt(dX * dX + dY * dY);
	angleDiff = robot->rotation - angleTo;
	//正则化夹角
	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;
	//lastU = lastU + getPwm(angleDiff);
	if (fabs(angleDiff) < 85)
	{
		lastU = lastU + pidCal1(angleDiff); //lastU是上一次的控制量
		vR = vMax + lastU;
		vL = vMax - lastU;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		lastU = lastU + pidCal1(angleDiff);
		vR = -vMax + lastU;
		vL = -vMax - lastU;
	}
	else
	{
		vR = 80;
		vL = -80;
	}
	if (z == 1)

	{
		if (distance < 3)
		{
			Velocity(0, 0);
			return 1;
		}
		else
		{

			Velocity(vL, vR);
			return 0;
		}
	}
	else if (z == 0)
	{
		Velocity(vL, vR);
		return 0;
	}
	else if (z == 2)
	{
		if (fabs(angleDiff) < 3)
		{
			Velocity(0, 0);
		}
		Velocity(vL, vR);
	}

}
*/
// 方法2使用PID进行调参
// 这里用pid方法进行底层运动函数控制，具体pid算法请参考免费课程，如果想优化底层可以重新写此函数。

void BaseRobot::new_new_move(double x, double y)
{
	if (tick == 1 || tick == 2 || tick % 100 == 0 || reset == true)
	{
		pid_new.resetPID();
		pid_angle_new.resetPID();

	}
	/* 用自己现在tick+0的位置和tick-7到tick-1的轮速，去预测7拍之后的位置(和旋转角）
	* tick+0拍的位置和速度，tick-7拍赋的轮速 -> tick+1拍的位置
	* 最终tick-1拍的轮速，获得tick+7拍的位置
	* 需要存储历史10tick的左右轮速，位置，在每一拍结束时维护，如果发生摆位则清除历史轮速（用reset一起搞了）。
	*/
	double vMax = 125; //最大速度,方便后边用sigmoid函数调整速度大小
	double vl, vr;
	double angleTo = 0, angleDiff = 0; //angleTo:足球机器人和小球的角度

	double dX = x - PredictInformation[tick_delay].position.x;
	double dY = y - PredictInformation[tick_delay].position.y;
	angleTo = 180. / M_PI * atan2(dY, dX);
	angleDiff = angleTo - this->PredictInformation[tick_delay].rotation;

	double distance = sqrt(dX * dX + dY * dY);

	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;
	double delta_vvv = pid_new.pidCal(distance);

	

	if (fabs(angleDiff) < 90)
	{
		double delta_v = pid_angle_new.pidCal(angleDiff); //lastU是上一次的控制量
		vr = delta_vvv + delta_v;
		vl = delta_vvv - delta_v;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		double delta_v = pid_angle_new.pidCal(angleDiff); //lastU是上一次的控制量
		vr = -delta_vvv + delta_v;
		vl = -delta_vvv - delta_v;
	}
	else
	{
		vr = 80;
		vl = -80;
	}



	
}

void BaseRobot::new_move(double x, double y)
{
	if (tick == 1 || tick == 2 || tick % 100 == 0 || reset == true)
	{
		pid_new.resetPID();
		pid_angle_new.resetPID();

	}
	/* 用自己现在tick+0的位置和tick-7到tick-1的轮速，去预测7拍之后的位置(和旋转角）
	* tick+0拍的位置和速度，tick-7拍赋的轮速 -> tick+1拍的位置
	* 最终tick-1拍的轮速，获得tick+7拍的位置
	* 需要存储历史10tick的左右轮速，位置，在每一拍结束时维护，如果发生摆位则清除历史轮速（用reset一起搞了）。
	*/
	double vMax = 125; //最大速度,方便后边用sigmoid函数调整速度大小
	double vl, vr;
	double angleTo = 0, angleDiff = 0; //angleTo:足球机器人和小球的角度

	double dX = x - PredictInformation[tick_delay].position.x;
	double dY = y - PredictInformation[tick_delay].position.y;
	angleTo = 180. / M_PI * atan2(dY, dX);
	angleDiff = angleTo - this->PredictInformation[tick_delay].rotation;

	double distance = sqrt(dX * dX + dY * dY);

	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;
	double delta_vvv = pid_new.pidCal(distance);
	if (delta_vvv<80&& delta_vvv>80)
	{
		delta_vvv = 80;
	}
	



	if (fabs(angleDiff) < 90)
	{
		double delta_v = pid_angle_new.pidCal(angleDiff); //lastU是上一次的控制量
		vr = delta_vvv + delta_v;
		vl = delta_vvv - delta_v;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		double delta_v = pid_angle_new.pidCal(angleDiff); //lastU是上一次的控制量
		vr = -delta_vvv + delta_v;
		vl = -delta_vvv - delta_v;
	}
	else
	{
		vr = 80;
		vl = -80;
	}



	if (distance < 3)
	{
		turnToAngle(90);
	}
	else
	{
		Velocity(vl, vr);
	}

	/*
	if (fabs(angleDiff) > 2)
	{
		double delta_vv = pid_angle_new.pidCal(angleDiff);
		vl = -delta_vv;
		vr = delta_vv;
		Velocity(vl, vr);
	}
	else if (fabs(angleDiff) < 2)
	{


		double delta_v = pid_new.pidCal(distance);

		if (distance < 2)
		{
			Velocity(0, 0);
		}

		else
		{
			vl = delta_v;
			vr = delta_v;
			Velocity(vl, vr);
		}
	}*/



}

void BaseRobot::move(double x, double y)
{

	if (tick == 1 || tick == 2 || tick % 100 == 0 || reset == true)
	{
		pid_moveTo.resetPID();
	}
	/* 用自己现在tick+0的位置和tick-7到tick-1的轮速，去预测7拍之后的位置(和旋转角）
	* tick+0拍的位置和速度，tick-7拍赋的轮速 -> tick+1拍的位置
	* 最终tick-1拍的轮速，获得tick+7拍的位置
	* 需要存储历史10tick的左右轮速，位置，在每一拍结束时维护，如果发生摆位则清除历史轮速（用reset一起搞了）。
	*/

	double vMax = 125; //最大速度,方便后边用sigmoid函数调整速度大小
	double vL, vR;
	double angleTo = 0, angleDiff = 0; //angleTo:足球机器人和小球的角度

	double dX = x - PredictInformation[tick_delay].position.x;
	double dY = y - PredictInformation[tick_delay].position.y;
	angleTo = 180. / M_PI * atan2(dY, dX);
	angleDiff = angleTo - this->PredictInformation[tick_delay].rotation;

	double distance = sqrt(dX * dX + dY * dY);
	//正则化夹角
	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;

	if (fabs(angleDiff) < 90)
	{
		double delta_v = pid_moveTo.pidCal(angleDiff); //lastU是上一次的控制量
		vR = vMax + delta_v;
		vL = vMax - delta_v;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		double delta_v = pid_moveTo.pidCal(angleDiff); //lastU是上一次的控制量
		vR = -vMax + delta_v;
		vL = -vMax - delta_v;
	}
	else
	{
		vR = 80;
		vL = -80;
	}

	if (distance < 2)
	{
		Velocity(0, 0);
	}
	else
		Velocity(vL, vR);


}













void BaseRobot::moveTo(double x, double y)
{
	if (tick == 1 || tick == 2 || tick % 100 == 0 || reset == true)
	{
		pid_moveTo.resetPID();
	}
	/* 用自己现在tick+0的位置和tick-7到tick-1的轮速，去预测7拍之后的位置(和旋转角）
	* tick+0拍的位置和速度，tick-7拍赋的轮速 -> tick+1拍的位置
	* 最终tick-1拍的轮速，获得tick+7拍的位置
	* 需要存储历史10tick的左右轮速，位置，在每一拍结束时维护，如果发生摆位则清除历史轮速（用reset一起搞了）。
	*/

	double vMax = 125; //最大速度,方便后边用sigmoid函数调整速度大小
	double vL, vR;
	double angleTo = 0, angleDiff = 0; //angleTo:足球机器人和小球的角度

	double dX = x - PredictInformation[tick_delay].position.x;
	double dY = y - PredictInformation[tick_delay].position.y;
	angleTo = 180. / M_PI * atan2(dY, dX);
	angleDiff = angleTo - this->PredictInformation[tick_delay].rotation;

	double distance = sqrt(dX * dX + dY * dY);
	//正则化夹角
	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;

	if (fabs(angleDiff) < 90)
	{
		double delta_v = pid_moveTo.pidCal(angleDiff); //lastU是上一次的控制量
		vR = vMax + delta_v;
		vL = vMax - delta_v;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		double delta_v = pid_moveTo.pidCal(angleDiff); //lastU是上一次的控制量
		vR = -vMax + delta_v;
		vL = -vMax - delta_v;
	}
	else
	{
		vR = 80;
		vL = -80;
	}



	Velocity(vL, vR);




	//double delta_v = pid_moveTo.pidCal(angleDiff); //lastU是上一次的控制量
	//vR = vMax + delta_v;
	//vL = vMax - delta_v;

}



// 保存信息，给下一拍用
void BaseRobot::saveLastInformation(double footBallNow_X, double footBallNow_Y)
{
	lastRotation = robot->rotation;
	lastRobotX = robot->position.x;
	lastRobotY = robot->position.y;
	lastBallx = footBallNow_X;
	lastBally = footBallNow_Y;

	for (int i = 7; i >= 1; i--)
	{
		HistoryInformation[i] = HistoryInformation[i - 1];
	}
	HistoryInformation[1] = *robot;

	pid_moveTo.updatePID();
	pid_angle.updatePID();
	pid_new.updatePID();
	pid_angle_new.updatePID();
}
// 读取某一时刻事件状态
int DataLoader::get_event(int tick)
{
	return this->event_states[tick];
}

// 保存事件及发生时间
void DataLoader::set_tick_state(int tick, int event_state)
{
	this->tick = tick;
	this->event_states[tick] = event_state;
}


void BaseRobot::turnToAngle(double tar_angle)
{
	if (tick == 1 || tick == 2 || tick % 100 || reset == true)
	{
		pid_angle.resetPID();
	}

	double parameter = 0.5;//参数，调整速度用
	double vMax = 0; //最大速度,方便后边用sigmoid函数调整速度大小
	double vL, vR;
	double angleDiff = tar_angle - PredictInformation[tick_delay].rotation;
	//正则化夹角
	while (angleDiff > 180) angleDiff -= 360;
	while (angleDiff < -180) angleDiff += 360;

	if (fabs(angleDiff) < 90)
	{
		double delta_v = pid_angle.pidCal(angleDiff); //lastU是上一次的控制量
		vR = vMax + delta_v;
		vL = vMax - delta_v;
	}
	else if (fabs(angleDiff) >= 90)
	{
		angleDiff += 180;
		if (angleDiff > 180)
			angleDiff -= 360;
		double delta_v = pid_angle.pidCal(angleDiff); //lastU是上一次的控制量
		vR = -vMax + delta_v;
		vL = -vMax - delta_v;
	}
	else
	{
		vR = 80;
		vL = -80;
	}

	//double delta_v = pid_angle.pidCal(angleDiff); //lastU是上一次的控制量
	//vR = vMax + delta_v;
	//vL = vMax - delta_v;
	Velocity(vL, vR);
	if ((fabs(angleDiff) < 3))
	{
		Velocity(0, 0);
	}
}


void BaseRobot::moveWithAngle(double tarX, double tarY, double tar_angle)	// 机器人跑位到某点，并固定在某个角度
{
	double dX = tarX - PredictInformation[tick_delay].position.x;
	double dY = tarY - PredictInformation[tick_delay].position.y;
	double distance = sqrt(dX * dX + dY * dY);
	if (distance > 2)
	{
		move(tarX, tarY);
	}
	else
	{
		turnToAngle(tar_angle);
	}
}

void BaseRobot::throwBall(double ballx, double bally)
{
	double dX = ballx - PredictInformation[tick_delay].position.x;
	double dY = bally - PredictInformation[tick_delay].position.y;
	double distance = sqrt(dX * dX + dY * dY);
	if (distance > 10 || dX < 0)
	{
		moveWithAngle(ballx - 1, bally, 0);
	}
	else
	{
		if (bally < 0)
			Velocity(125, -125);
		else
			Velocity(-125, 125);
	}
}

void BaseRobot::shoot(double ballx, double bally)
{
	double dX = ballx - PredictInformation[tick_delay].position.x;
	double dY = bally - PredictInformation[tick_delay].position.y;
	if (ballx < -75)
	{
		moveTo(-60, bally);
	}
	else if (ballx < 72.5)	// 如果不在射门范围内，先跑到射门点
	{
		moveTo(ballx, bally);
	}
	else	// 在射门范围内调整位置射门。
	{
		throwBall(ballx, bally);
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

void BaseRobot::breakThrough(BaseRobot oppRobots[5], double tarx, double tary)
{
	moveTo(tarx, tary);
	double future_time = 14;	// 以匀速直线运动预测14拍之后
	double len = 5;	//碰撞检测用到的参数，表示长度。
	double deltay = 4;	// 增大此值可使碰撞减少更明显，但也会使机器人更敏感，更漂浮不定
	for (int i = 0; i < 5; i++)
	{
		double futureOppY = future_time * oppRobots[i].getPos().y - (future_time - 1) * oppRobots[i].getLastPos().y;	// 未来对方0号机器人坐标x
		if (oppRobots[i].getPos().x > robot->position.x && oppRobots[i].getPos().x < tarx
			|| oppRobots[i].getPos().x < robot->position.x && oppRobots[i].getPos().x > tarx)	// 如果我方0号与对方0号x的关系存在产生碰撞可能
		{
			if (robot->position.y > 0)	// 如果在上半场
			{
				if (futureOppY + len > robot->position.y)	// 如果我方0号与对方0号y的关系存在产生碰撞可能
				{
					moveTo(tarx, tary + deltay);	// 转换目标点
				}
			}
			if (robot->position.y < 0)	// 如果在下半场
			{
				if (futureOppY - len < robot->position.y)	// 如果我方0号与对方0号y的关系存在产生碰撞可能
				{
					moveTo(tarx, tary - deltay);	// 转换目标点
				}
			}
		}
	}
}

/*进攻函数，keepY表示保证机器人横坐标不会超过某一个给定值keepY*/
void BaseRobot::keepYPushBall(double keepY1, double keepY2, double footBallNow_X, double footBallNow_Y)
{
	if (getPos().x < keepY1)
	{
		moveTo(keepY1, footBallNow_Y);
	}
	else
	{
		if (footBallNow_X < keepY1)
		{
			moveTo(keepY1, footBallNow_Y);	// 若机器人目标点横坐标超出给定范围则目标点y不变，目标点x变成keepY
		}
		else {
			moveTo(footBallNow_X, footBallNow_Y);	// 否则正常运行
		}
	}
	if (getPos().x > keepY2)
	{
		moveTo(keepY2, footBallNow_Y);	// 若机器人横坐标超出给定范围则y不变，x退回到keepY
	}
	else
	{
		if (footBallNow_X > keepY2)
		{
			moveTo(keepY2, footBallNow_Y);	// 若机器人目标点横坐标超出给定范围则目标点y不变，目标点x变成keepY

		}
		else
		{
			moveTo(footBallNow_X, footBallNow_Y);	// 否则正常运行
		}
	}
}