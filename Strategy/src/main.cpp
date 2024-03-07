// Strategy.cpp : 定义 DLL 应用程序的导出函数。
#include "../header/stdafx.h"
#include "../header/platform.h"
#include "../header/adapter.h"
#include "../header/BaseRobot.h"
#include<iostream>
#include "xstring"
#include<string>
#include<typeinfo>
#include<sstream>
#include<locale>
#include<vector>

using namespace Simuro;
using namespace Adapter;
using namespace std;

double nearmy(int robot);
void nearoppest(void);
void nearopper(void);
void GK(void);
void CB(void);
void CDM(void);
void CM(void);
void ST(void);
void shoot(void);
void test(void);
void penaltys(void);
void newshoot(void);

int tick = 0;
int able = 1;
int unable = 1, unable1 = 1;

double di_op_0toball, di_op_1toball, di_op_2toball, di_op_3toball, di_op_4toball;
double di_se_toball, di_se_1toball, di_se_2toball, di_se_3toball, di_se_4toball;
int tickBeginPental = 0;
BaseRobot baseRobots[5];
BaseRobot oppRobots[5];
DataLoader dataloader;
Balls balls;


int race_state=-1;//处于何种定位球状态，0是开球，其他遵从JudgeType
int race_state_trigger=-1;//哪一方触发了定位球
double footBallNow_X, footBallNow_Y;

/**
* 打印比赛状态
*/
void OnEvent(EventType type, void* argument) {
	SendLog(L"V/Strategy:OnEvent()");
	if (type == EventType::JudgeResult)
	{
		JudgeResultEvent* judgeResult = static_cast<JudgeResultEvent*>(argument);
        race_state=judgeResult->type;
        race_state_trigger=judgeResult->actor;
		if (judgeResult->type == JudgeType::PlaceKick) 
		{
			SendLog(L"Place Kick");
		}
		switch (judgeResult->actor){
		    case Team::Self:
                SendLog(L"By self");
		        break;
		    case Team::Opponent:
                SendLog(L"By opp");
		        break;
		    case Team::None:
                SendLog(L"By both");
		        break;
		}
	}
}

/**
* 获得队伍信息
*/
void GetTeamInfo(TeamInfo* teamInfo) {
	SendLog(L"V/Strategy:GetTeamInfo()");
	static const wchar_t teamName[] = L"ST";
	static constexpr size_t len = sizeof(teamName);
	memcpy(teamInfo->teamName, teamName, len);
}

/**
* 摆位信息，进行定位球摆位
*/
void GetPlacement(Field* field) {
	SendLog(L"V/Strategy:GetPlacement()");
	auto robots = field->selfRobots;

	if (race_state == JudgeType::PlaceKick)//开球
	{
		SendLog(L"开球摆位");
		if (race_state_trigger == Team::Self)//开球进攻
		{
			robots[0].position.x = -100 ;
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = 42;
			robots[1].position.y = 42;
			robots[1].rotation = 180;
			robots[2].position.x = -30;
			robots[2].position.y = -10;
			robots[2].rotation = 0;
			robots[3].position.x = -50;
			robots[3].position.y = 10;
			robots[3].rotation = 0;
			robots[4].position.x = -70;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
		}
		else if (race_state_trigger == Team::Opponent)//开球防守
		{
			robots[0].position.x = -100;
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = -10;
			robots[1].position.y = 80;
			robots[1].rotation = -90;
			robots[2].position.x = -30;
			robots[2].position.y = -80;
			robots[2].rotation = -90;
			robots[3].position.x = -50;
			robots[3].position.y = 70;
			robots[3].rotation = -90;
			robots[4].position.x = -70;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
		}
		else//None人触发
		{

		}
	}
	if (race_state == PenaltyKick)
	{
		if (race_state_trigger == Team::Self)
		{
			robots[0].position.x = -103;//守门员位置看你
			robots[0].position.y = 0;
			robots[0].rotation = 90;
			robots[1].position.x = 69.9489441;//点球
			robots[1].position.y = -0.399356037;
			robots[1].rotation = 0;
			robots[2].position.x = -5;
			robots[2].position.y = -40;
			robots[2].rotation = 0;
			robots[3].position.x = -5;
			robots[3].position.y = 40;
			robots[3].rotation = 0;
			robots[4].position.x = -5;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
		}
		if (race_state_trigger == Team::Opponent)
		{
			robots[0].position.x = -91;//靠近球门线
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = 5;
			robots[1].position.y = 40;
			robots[1].rotation = 0;
			robots[2].position.x = 5;
			robots[2].position.y = -40;
			robots[2].rotation = 0;
			robots[3].position.x = 5;
			robots[3].position.y = 10;
			robots[3].rotation = 0;
			robots[4].position.x = 5;
			robots[4].position.y = -10;
			robots[4].rotation = 0;
		}
	}
	if (race_state == GoalKick)
	{
		if (race_state_trigger == Team::Self)
		{
			robots[0].position.x = -103;
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = 72;
			robots[1].position.y = 0;
			robots[1].rotation = 0;
			robots[2].position.x = -2;
			robots[2].position.y = -5;
			robots[2].rotation = 0;
			robots[3].position.x = -2;
			robots[3].position.y = 5;
			robots[3].rotation = 0;
			robots[4].position.x = -2;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
			field->ball.position.x = -110 + 15;
			field->ball.position.y = 0;
		}
		if (race_state_trigger == Team::Opponent)
		{
			robots[0].position.x = -105;
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = 2;
			robots[1].position.y = 6;
			robots[1].rotation = 0;
			robots[2].position.x = 2;
			robots[2].position.y = -2;
			robots[2].rotation = 0;
			robots[3].position.x = 2;
			robots[3].position.y = 2;
			robots[3].rotation = 0;
			robots[4].position.x = 2;
			robots[4].position.y = 6;
			robots[4].rotation = 0;
			field->ball.position.x = 0;
			field->ball.position.y = 0;
		}
	}
	if (race_state == FreeKickLeftBot || race_state == FreeKickLeftTop
		|| race_state == FreeKickRightBot || race_state == FreeKickRightTop)
	{
		if (race_state_trigger == Team::Self)
		{
			robots[0].position.x = -103;
			robots[0].position.y = 0;
			robots[0].rotation = 90;
			robots[1].position.x = 30;
			robots[1].position.y = 0;
			robots[1].rotation = 0;
			robots[2].position.x = -30;
			robots[2].position.y = -10;
			robots[2].rotation = 0;
			robots[3].position.x = -3;
			robots[3].position.y = 10;
			robots[3].rotation = 0;
			robots[4].position.x = -3;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
			field->ball.position.x = 0;
			field->ball.position.y = 0;
		}
		if (race_state_trigger == Team::Opponent)
		{
			robots[0].position.x = -105;
			robots[0].position.y = 0;
			robots[0].rotation = 0;
			robots[1].position.x = 30;
			robots[1].position.y = 0;
			robots[1].rotation = 0;
			robots[2].position.x = 10;
			robots[2].position.y = -10;
			robots[2].rotation = 0;
			robots[3].position.x = 10;
			robots[3].position.y = 10;
			robots[3].rotation = 0;
			robots[4].position.x = 10;
			robots[4].position.y = 0;
			robots[4].rotation = 0;
			field->ball.position.x = 0;
			field->ball.position.y = 0;
		}
	}

}


// 策略行为主函数，可将以下函数用策略模式封装
void strategy(double footBallNow_X, double footBallNow_Y)
{
	double distance, x4, y4, kbc, kds, xD, yD, yds, yab, kab;
	double E;
	double x1, y1, x2, y2, x3, y3, gapx, gapy; //上一周期足球的位置K1(x1,y1) 当前足球的位置K2(x2,y2) t个周期后足球的位置坐标K3(x3,y3)
	x1 = balls.getLastPos().x;
	y1 = balls.getLastPos().y;
	x2 = footBallNow_X;
	y2 = footBallNow_Y;
	gapx = x2 - x1;
	gapy = y2 - y1;
	x3 = x2 + 5 * gapx;
	y3 = y2 + 5 * gapy;
	penaltys();
	
	//存储信息
	for (int i = 0; i < 5; i++)
	{
		baseRobots[i].saveLastInformation(footBallNow_X, footBallNow_Y);
	}
	balls.saveLastInformation(footBallNow_X, footBallNow_Y);
}

/**
* 获得策略信息
*/
void GetInstruction(Field* field) {
	tick = field->tick;
	balls.update(&(field->ball));
	for (int i = 0; i < 5; i++) {
		baseRobots[i].update(&(field->selfRobots[i]));
	}
	for (int i = 0; i < 5; i++)
	{
		oppRobots[i].update(&(field->opponentRobots[i]));
	}

	 footBallNow_X = field->ball.position.x;
	 footBallNow_Y = field->ball.position.y;

	strategy(footBallNow_X, footBallNow_Y);
    dataloader.set_tick_state(tick, race_state);
}
void distance(Simuro::Robot* robot)
{
	di_op_0toball = 1;
}
/*机器人0号*/void GK(void)
{
	/*nearmy(0);
	if (fabs(footBallNow_Y) < 21)
	{
		if (footBallNow_X >0 )
		{
			
			baseRobots[0].move(-100, footBallNow_Y);
		}
		else
		{
			if(nearmy(0)>15)
			baseRobots[0].moveTo(footBallNow_X, footBallNow_Y);
			else if(nearmy(0) <=15)
				baseRobots[0].Velocity(125, 0);
		}

	}
	else
	{
		if (footBallNow_Y > 21)
		{
			baseRobots[0].move(-100, 21);
		}
		else if (footBallNow_Y <-21)
		{
			baseRobots[0].move(-100, -21);
		}
	}*/
	if (footBallNow_X >= 0)
	{
		baseRobots[0].move(-105, 0);
	}
	if (footBallNow_X < 0)
	{
		if(footBallNow_X>-105)
		{ 
		if (pow(footBallNow_X + 17.86436, 2) + pow(footBallNow_Y, 2) >= 8888.9748)
		{
			if (footBallNow_Y > 0)
			{
				//baseRobots[0].move(-105, 20);
				baseRobots[0].leftspin(-103, 20);
			}else if (footBallNow_Y <0)baseRobots[0].rightspin(-103, -20);

		}
		else if (pow(footBallNow_X + 17.86436, 2) + pow(footBallNow_Y, 2) < 8888.9748&& footBallNow_X)
		{
			if (pow(footBallNow_X + 60.51481, 2) + pow(footBallNow_Y, 2) >= 2848.7841)
			{
				if (footBallNow_Y > 0)
				{
					baseRobots[0].move(-105,10);
				}
				if (footBallNow_Y < 0)
				{
					baseRobots[0].move(-105, -10);
				}
			}
			if (pow(footBallNow_X + 60.51481, 2) + pow(footBallNow_Y, 2) < 2848.7841)
			{
				if (pow(footBallNow_X + 86.09334, 2) + pow(footBallNow_Y, 2) >= 971.52837)
				{
					if(footBallNow_Y>0)baseRobots[0].move(-103, 15);
					else if (footBallNow_Y <0)baseRobots[0].move(-103, -15);
				}
				if (pow(footBallNow_X +86.09334, 2) + pow(footBallNow_Y, 2) < 971.52837)
				{
					if (pow(footBallNow_X + 101.15508,2) + pow(footBallNow_Y, 2) >=568.58021)
					{
						if (footBallNow_Y > 0)baseRobots[0].move(-103, 10);
						else if (footBallNow_Y < 0)baseRobots[0].move(-103, -10);
					}
					if (pow(footBallNow_X + 101.15508, 2) + pow(footBallNow_Y, 2) < 568.58021)
					{
						baseRobots[0].moveTo(footBallNow_X, footBallNow_Y);

					}
				}

			}

		}
		}
		else if (footBallNow_X <= -105)
		{
			if (footBallNow_Y > 18)
			{
				//baseRobots[0].move(-105, 20);
				baseRobots[0].leftspin(-103, 20);
			}
			else if (footBallNow_Y < -18)baseRobots[0].rightspin(-103, -20);
			else if (footBallNow_Y >= -18 && footBallNow_Y <= 18)
			{
				baseRobots[0].moveTo(footBallNow_X, footBallNow_Y);
			}
		}
}



}
/*机器人4号*/void CB(void)
{
	if (footBallNow_X > 0&&fabs(footBallNow_Y)<=30)
	{
		baseRobots[4].moveTo(-70, 0);
	}
	else if (footBallNow_X>00 && footBallNow_Y > 30)
	{
		baseRobots[4].moveTo(-70, 45);
	}
	else if (footBallNow_X > 00 && footBallNow_Y <- 30)
	{
		baseRobots[4].moveTo(-70, -45);
	}
	else if (footBallNow_X <= 0)
	{
		if (footBallNow_X > baseRobots[4].getPos().x&&able==0)
		{
			baseRobots[4].moveTo(footBallNow_X, footBallNow_Y);
		}
		else if (footBallNow_X > -110 && baseRobots[4].getPos().x > footBallNow_X)
		{
			if (footBallNow_X > -90)
			{
				if (baseRobots[4].getPos().x < footBallNow_X + 15)
				{
					able = 0;
				}
				else if (baseRobots[4].getPos().x >= footBallNow_X + 15)
				{
					able = 1;
					if(baseRobots[4].getPos().y >= footBallNow_Y )baseRobots[4].move(footBallNow_X - 10, footBallNow_Y+10);
					else if(baseRobots[4].getPos().y < footBallNow_Y)baseRobots[4].move(footBallNow_X - 10, footBallNow_Y-10);
				}
			}
			else if (footBallNow_X <= -90)
			{
				if (fabs(footBallNow_Y)<=35&& fabs(footBallNow_X)>90)//球进入禁区，机器人去往最近的防守点
				{
					if (baseRobots[4].getPos().y > 0)
					{
						baseRobots[4].move(-80,30 );
					}

					if (baseRobots[4].getPos().y < 0)
					{
						baseRobots[4].move(-80, -30);
					}
				}
				else if (fabs(footBallNow_Y) > 25)
				{
					able = 1;
					baseRobots[4].move(-108, footBallNow_Y);
				}
			}
			
		}

	}
}
/*机器人1号*/ void ST(void)
{
	shoot();
	
	
	/*double x1, x2, y1, y2, x, y, d1, kk;

		x1 = footBallNow_X;
		x2 = 108;
		y1 = footBallNow_Y;
		y2 = 0;
		d1 = sqrt(pow(x1- baseRobots[1].getPos().x,2)+pow(fabs(y1)-fabs(baseRobots[1].getPos().y),2));
		y = baseRobots[1].getPos().x * (y2 - y1) / (x2 - x1) + (x2 * y1 - x1 * y2) / (x2 - x1);

		if (baseRobots[1].getPos().y > y + 1.5|| baseRobots[1].getPos().y < y - 1.5)
		{
			baseRobots[1].move(baseRobots[1].getPos().x, y);
		}
		if (baseRobots[1].getPos().y <= y + 1.5 && baseRobots[1].getPos().y >= y - 1.5)
		{

			baseRobots[1].moveTo(x1, y1);
		}
		*/


/*if (x1>70 && fabs(y1<40))
{
	if (baseRobots[1].getPos().x > 0)
	{
		unable = 0;
	}
	else if(baseRobots[1].getPos().x<0)baseRobots[1].move(50, 30);
	
	
	
	if (unable == 0)
	{
	
		if (baseRobots[1].getPos().y <= y + 1 && baseRobots[1].getPos().y >= y - 1)
		{
			//baseRobots[1].moveTo(x1, y1);
			unable1 = 0;
		}
		if(baseRobots[1].getPos().y > y + 1 && baseRobots[1].getPos().y < y - 1)
		
		{
			baseRobots[1].move(baseRobots[1].getPos().x, y);
		}


		if (unable1 == 0)
		{
			baseRobots[1].moveTo(x1, y1);
		}
	}

	/*double a1, b1, c1, a2, b2, c2, x1, x2, x3, y1, y2, y3;
	double set_x = -110, set_y = 0;
	x1 = footBallNow_X, y1 = footBallNow_Y;
	x2 = baseRobots[1].getPos().x, y2 = baseRobots[1].getPos().y;
	x3 = set_x, y3 = set_y;

	a1 = y2 - y1;
	b1 = x2-x1;
	c1 = 0.5 * (x1+x2)*(x1-x2)+0.5*(y2+y1)*(y2-y1);
	a2 = x3 - x1;
	b2 = -(y3 - y1);
	c2 = x1 * (y3 - y1) - y1;


	/*a1 = baseRobots[1].getPos().y - footBallNow_Y;
	b1 = baseRobots[1].getPos().x - footBallNow_X;
	c1 = 0.5 * (pow(footBallNow_X, 2) - pow(baseRobots[1].getPos().x, 2) + pow(baseRobots[1].getPos().y, 2) - pow(footBallNow_Y, 2));
	a2 = set_x - footBallNow_X;
	b2 = footBallNow_Y - set_y;
	c2 = footBallNow_X * (set_y - footBallNow_Y) - footBallNow_Y * (set_x - footBallNow_X);


	/*double chanshux = (b2 * c1 - b1 * c2) / (a2 * b1 - a1 * b2);
	double chanshuy = (a1*c2-a2*c1)/(a2*b1-a1*b2);
	baseRobots[1].move(chanshux, chanshuy);
	/*if (chanshux == 1 && chanshuy == 1)
	{
		baseRobots[1].moveTo(100, 0);

	}*/
/* }
else baseRobots[1].moveTo(40,y1);*/
}
/*机器人3号*/void CDM(void)
{
	nearoppest();
}
/*机器人2号*/void CM()
{

	if (footBallNow_X < 0)
	{
		nearopper();

	}
	if (footBallNow_X >= 0)
	{

		baseRobots[2].moveTo(50,30);
	}
}
double nearmy(int robot)
{
	double myrobot0,myrobot1, myrobot2, myrobot3, myrobot4;
	myrobot0= sqrt(pow(footBallNow_X - baseRobots[0].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(baseRobots[0].getPos().y), 2));
	myrobot1 = sqrt(pow(footBallNow_X - baseRobots[1].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(baseRobots[1].getPos().y), 2));
	myrobot2 = sqrt(pow(footBallNow_X - baseRobots[2].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(baseRobots[2].getPos().y), 2));
	myrobot3 = sqrt(pow(footBallNow_X - baseRobots[3].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(baseRobots[3].getPos().y), 2));
	myrobot4 = sqrt(pow(footBallNow_X - baseRobots[4].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(baseRobots[4].getPos().y), 2));

	switch (robot)
	{
	case 0:
		return myrobot0;
	case 1:
		return myrobot1;
	case 2:
		return myrobot2;
	case 3:
		return myrobot3;
	case 4:
		return myrobot4;

	}


	
}

void nearoppest(void)
{
	double opprobot1, opprobot2, opprobot3, opprobot4;

	opprobot1 = sqrt(pow(footBallNow_X - oppRobots[1].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[1].getPos().y), 2));
	opprobot2 = sqrt(pow(footBallNow_X - oppRobots[2].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[2].getPos().y), 2));
	opprobot3= sqrt(pow(footBallNow_X - oppRobots[3].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[3].getPos().y), 2));
	opprobot4 = sqrt(pow(footBallNow_X - oppRobots[4].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[4].getPos().y), 2));

	if (opprobot1 < opprobot2 && opprobot1 < opprobot3 && opprobot1 < opprobot4)
	{
		baseRobots[3].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
	}
	 if (opprobot2 < opprobot1 && opprobot2 < opprobot3 && opprobot2 < opprobot4)
	{
		baseRobots[3].moveTo(oppRobots[2].getPos().x, oppRobots[2].getPos().y);
	}
	 if (opprobot3 < opprobot4 && opprobot3 < opprobot2 && opprobot3 < opprobot1)
	{
		baseRobots[3].moveTo(oppRobots[3].getPos().x, oppRobots[3].getPos().y);
	}
	 if (opprobot4 < opprobot1 && opprobot4 < opprobot3 && opprobot4 < opprobot2)
	{
		baseRobots[3].moveTo(oppRobots[4].getPos().x, oppRobots[4].getPos().y);
	}
	
}
void nearopper(void)
{
	double opprobot1, opprobot2, opprobot3, opprobot4;

	opprobot1 = sqrt(pow(footBallNow_X - oppRobots[1].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[1].getPos().y), 2));
	opprobot2 = sqrt(pow(footBallNow_X - oppRobots[2].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[2].getPos().y), 2));
	opprobot3 = sqrt(pow(footBallNow_X - oppRobots[3].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[3].getPos().y), 2));
	opprobot4 = sqrt(pow(footBallNow_X - oppRobots[4].getPos().x, 2) + pow(fabs(footBallNow_Y) - fabs(oppRobots[4].getPos().y), 2));

	if (opprobot1 < opprobot2 && opprobot1 < opprobot3 && opprobot1 < opprobot4)
	{
		if (opprobot2 < opprobot3 && opprobot2 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[2].getPos().x, oppRobots[2].getPos().y);
		}
		if (opprobot3 < opprobot2 && opprobot3 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[3].getPos().x, oppRobots[3].getPos().y);
		}
		if (opprobot4 < opprobot2 && opprobot4< opprobot3)
		{
			baseRobots[2].moveTo(oppRobots[4].getPos().x, oppRobots[4].getPos().y);
		}
	}
	if (opprobot2 < opprobot1 && opprobot2 < opprobot3 && opprobot2 < opprobot4)
	{
		if (opprobot1 < opprobot3 && opprobot1 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		}
		if (opprobot3 < opprobot1 && opprobot3 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[3].getPos().x, oppRobots[3].getPos().y);
		}
		if (opprobot4 < opprobot1 && opprobot4 < opprobot3)
		{
			baseRobots[2].moveTo(oppRobots[4].getPos().x, oppRobots[4].getPos().y);
		}
	}
	if (opprobot3 < opprobot4 && opprobot3 < opprobot2 && opprobot3 < opprobot1)
	{
		if (opprobot1 < opprobot2 && opprobot1 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		}
		if (opprobot2 < opprobot1 && opprobot2 < opprobot4)
		{
			baseRobots[2].moveTo(oppRobots[2].getPos().x, oppRobots[2].getPos().y);
		}
		if (opprobot4 < opprobot1 && opprobot4 < opprobot2)
		{
			baseRobots[2].moveTo(oppRobots[4].getPos().x, oppRobots[4].getPos().y);
		}
	}
	if (opprobot4 < opprobot1 && opprobot4 < opprobot3 && opprobot4 < opprobot2)
	{
		if (opprobot1 < opprobot2 && opprobot1 < opprobot3)
		{
			baseRobots[2].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		}
		if (opprobot2 < opprobot1 && opprobot2 < opprobot3)
		{
			baseRobots[2].moveTo(oppRobots[2].getPos().x, oppRobots[2].getPos().y);
		}
		if (opprobot3 < opprobot1 && opprobot3 < opprobot2)
		{
			baseRobots[2].moveTo(oppRobots[3].getPos().x, oppRobots[3].getPos().y);
		}
	}

}
void shoot(void)
{
	double distance, x4, y4, kbc, kds, xD, yD, yds, yab, kab;
		double E;
		double x1, y1, x2, y2, x3, y3, gapx, gapy; //上一周期足球的位置K1(x1,y1) 当前足球的位置K2(x2,y2) t个周期后足球的位置坐标K3(x3,y3)
		x1 = balls.getLastPos().x;
		y1 = balls.getLastPos().y;
		x2 = footBallNow_X;
		y2 = footBallNow_Y;
		gapx = x2 - x1;
		gapy = y2 - y1;
		x3 = x2 + 5 * gapx;
		y3 = y2 + 5 * gapy;
	if (((footBallNow_X >= (baseRobots[1].getPos().x - 2)) && (footBallNow_X <= 110)) && ((footBallNow_Y >= (((footBallNow_X - 110) * (baseRobots[1].getPos().y + 40) / (baseRobots[1].getPos().x - 110) - 40) - 2)) && (footBallNow_Y <= ((footBallNow_X - 110) * (baseRobots[1].getPos().y - 40) / (baseRobots[1].getPos().x - 110) + 40) + 2)))
	{
		
		//if (((footBallNow_X >= (baseRobots[1].getPos().x - 2)) && (footBallNow_X <= 112)) && ((footBallNow_Y >= (((footBallNow_X - 110) * (baseRobots[1].getPos().y + 20) / (baseRobots[1].getPos().x - 110) - 20) - 2)) && (footBallNow_Y <= ((footBallNow_X - 110) * (baseRobots[1].getPos().y - 20) / (baseRobots[1].getPos().x - 110) + 20) + 2)))
		//确定目标运动点
		
		kbc = (baseRobots[1].getPos().y - y3) / (baseRobots[1].getPos().x - x3);
		kds = (-1) / kbc;
		kab = (110 - x3) / (16 - y3);
		xD = (0.5) * (x3 + baseRobots[1].getPos().x);
		yD = (0.5) * (y3 + baseRobots[1].getPos().y);
		//yds = kds * (x - xD) + yD;
		//yab = kab * (x - x3) + y3;
		E = kab - kds;  //求出目标点S(x4, y4)
		x4 = (yD - kds * xD - y3 + kab * x3) / E;
		y4 = (kab * (yD - kds * xD) - kds * (y3 - kab * x3)) / E;
		//判断机器人此时是否在线ab上,代入机器人坐标x求得方程中的Y;
		double Y;
		Y = kab * (baseRobots[1].getPos().x - x3) + y3;
		if ((Y <= (baseRobots[1].getPos().y - 2)) || (Y >= (baseRobots[1].getPos().y + 2)))
		{
			baseRobots[1].move(x4, y4);//不在则冲向目标点S
		}
		else
		{
			baseRobots[1].move(110, 16);//在则射门
		}
	}
	else
	{
		baseRobots[1].move(x3, y3);
	
	}
}
void test(void)
{
	baseRobots[4].getPos().x;
}

void newshoot(void)
{
	if (race_state == JudgeType::PlaceKick)//开球
	{
		SendLog(L"开球摆位");
		if (race_state_trigger == Team::Self)//开球进攻
		{
			shoot();
			baseRobots[2].move(x3, y3);//若不进，则进入进攻状态，按你的进攻来，初始位置也可调整
			baseRobots[3].move(x3, y3);
			baseRobots[4].move(x3, y3);

		}

		if (race_state_trigger == Team::Opponent)//开球防守
		{
			baseRobots[0].move(x3, y3);
			baseRobots[2].move(x3, y3);//其他按你的防守来，初始位置也可调整
			baseRobots[3].move(x3, y3);
			baseRobots[4].move(x3, y3);
		}
		
	}
	
}
void penaltys(void)
{
	if (race_state == JudgeType::PenaltyKick)//点球
	{
		if (race_state_trigger == Team::Self)//开球进攻
		{
			shoot();
			baseRobots[2].move(x3, y3);//若不进，则进入进攻状态，按你的进攻来，初始位置也可调整
			baseRobots[3].move(x3, y3);
			baseRobots[4].move(x3, y3);

		}

		if (race_state_trigger == Team::Opponent)//开球防守
		{
			baseRobots[0].move(x3, y3);
			baseRobots[2].move(x3, y3);//其他按你的防守来，初始位置也可调整
			baseRobots[3].move(x3, y3);
			baseRobots[4].move(x3, y3);
		}
	}
}