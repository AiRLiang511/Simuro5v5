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

int a, b;
double D1 = 0;

int tick = 0;
int tickBeginPental = 0;
BaseRobot baseRobots[5];
BaseRobot oppRobots[5];
DataLoader dataloader;
int race_state=-1;//处于何种定位球状态，0是开球，其他遵从JudgeType
int race_state_trigger=-1;//哪一方触发了定位球
int zeroopp; //辨认0号的对手位
int oneopp;  //辨认1号的对手位
double zeromovex; //辨认0号靠近对手时目标点的x坐标
double onemovex; //辨认1号靠近对手时目标点的x坐标
double zeromovey; //辨认0号靠近对手时目标点的y坐标
double onemovey; //辨认1号靠近对手时目标点的y坐标


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
	static const wchar_t teamName[] = L"manniu1";
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
			robots[0].position.x = -106;
			robots[0].position.y = -24;
			robots[0].rotation = 0;
			robots[1].position.x = -106;
			robots[1].position.y = 24;//0.523456
			robots[1].rotation = 0;
		}
		else if (race_state_trigger == Team::Opponent)//开球防守
		{
			robots[0].position.x = -75;
			robots[0].position.y = 8;
			robots[0].rotation = 0;
			robots[1].position.x = -75;
			robots[1].position.y = -8;
			robots[1].rotation = 0;
		}
		else//None人触发
		{

		}
	}
}


// 策略行为主函数，可将以下函数用策略模式封装
void strategy(double footBallNow_X, double footBallNow_Y)
{
	if (race_state == JudgeType::PlaceKick)//开球
	
		if (race_state_trigger == Team::Self)
		{
			double d00s, d10s, d01s, d11s, d30, d60, d111, d222, d00, d10, d01, d11;
			d00s = sqrt((pow((oppRobots[0].getPos().x - (-106)), 2) + pow((oppRobots[0].getPos().y - (-24)), 2))); //0号初始位置与对方0号距离
			d11s = sqrt((pow((oppRobots[1].getPos().x - (-106)), 2) + pow((oppRobots[1].getPos().y - 24), 2))); //1号初始位置与对方1号距离
			d10s = sqrt((pow((oppRobots[1].getPos().x - (-106)), 2) + pow((oppRobots[1].getPos().y - (-24)), 2))); //0号初始位置与对方1号距离
			d01s = sqrt((pow((oppRobots[0].getPos().x - (-106)), 2) + pow((oppRobots[0].getPos().y - 24), 2))); //1号初始位置与对方0号距离

			d00 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[0].getPos().y), 2)));
			d11 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[1].getPos().y), 2)));
			d10 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[0].getPos().y), 2)));
		    d01 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[1].getPos().y), 2)));

			double x10, y10, x20, y20;

			double pre0, pre1, avert0, avert1;
			double x1, y1, x2, y2, x3, y3, gapx, gapy;
			x1 = oppRobots[0].getLastPos().x;
			y1 = oppRobots[0].getLastPos().y;
			x2 = oppRobots[0].getPos().x;
			y2 = oppRobots[0].getPos().y;

			gapx = x2 - x1;

			gapy = y2 - y1;
			x3 = x2 + 2 * gapx;
			y3 = x3 + 2 * gapy;
			d30 = sqrt((pow((x3 - baseRobots[0].getPos().x), 2) + pow((y3 - baseRobots[0].getPos().y), 2)));

			avert0 = baseRobots[0].getPos().y + 2 * gapy;

			double x4, y4, x5, y5, x6, y6, gapsx, gapsy;
			x4 = oppRobots[1].getLastPos().x;
			y4 = oppRobots[1].getLastPos().y;
			x5 = oppRobots[1].getPos().x;
			y5 = oppRobots[1].getPos().y;
			gapsx = x5 - x4;
			gapsy = y5 - y4;
			x6 = x5 + 2 * gapsx;
			y6 = x5 + 2 * gapsy;
			d60 = sqrt((pow((x6 - baseRobots[0].getPos().x), 2) + pow((y6 - baseRobots[0].getPos().y), 2)));
			avert1 = baseRobots[0].getPos().y + 2 * gapsy;

			/*if (d00 >= 40.55578959)
				{
					baseRobots[0].moveTo(110, 15);
					baseRobots[1].moveTo(110, -15);
				}
			if (d00 < 40.55578959)
				{
					baseRobots[0].moveTo(x3, y3 + 8.15128495);
					baseRobots[1].moveTo(x3, y3 - 8.15128495);

				}*/
			//zeroopp = 0;//对方1号与0号的距离大于对方0号与0号的距离则 0号对0号， 1号对1号
			//oneopp = 1;
			if (d10s >= d00s)//通过距离判断对手位是谁
			{
				zeroopp = 0;//对方1号与0号的距离大于对方0号与0号的距离则 0号对0号， 1号对1号
				oneopp = 1;
			}
			else
			{
				zeroopp = 1;//对方1号与0号的距离小于对方0号与0号的距离则 0号对对方1号， 1号对对方0号
				oneopp = 0;
			}

			//if (baseRobots[0].getPos().x  < -55)//小于-55时直走
			//{
			//	baseRobots[0].moveTo(-55, -24);
			//	
			//}
			/*baseRobots[0].moveTo(110, -17.5123456);
			baseRobots[1].moveTo(110, 17.5123456);*/
			//else
			//{
				if (zeroopp == 0)//0号对位0号
				{
					if (d00 >= 20.01234567)
					{
						baseRobots[0].moveTo(112, -16.5123456);
					}
					else if (d00 < 20.01234567 && d00 >= 15.01234567)//靠近后记录此时对手位的位置，作为闪避时目标点
					{
						zeromovex = oppRobots[0].getPos().x;
						zeromovey = oppRobots[0].getPos().y - 5;
						baseRobots[0].moveTo(112, -16.5123456);
					}
					else
					{
						baseRobots[0].moveTo(zeromovex, zeromovey); //距离小于15则目标点为闪避点躲避
					}
				}
				
				if (zeroopp == 1)//0号对位1号
				{
					if (d10 >= 20.01234567)
					{
						baseRobots[0].moveTo(112, -16.5123456);
					}
					else if (d10 < 20.01234567 && d10 >= 15.01234567)
					{
						zeromovex = oppRobots[1].getPos().x;
						zeromovey = oppRobots[1].getPos().y - 5;
						baseRobots[0].moveTo(112, -16.5123456);
					}
					else
					{
						baseRobots[0].moveTo(zeromovex, zeromovey);
					}
				}
			//}
			

			//if (baseRobots[1].getPos().x < -55)//通过距离判断对手位是谁
			//{
			//	baseRobots[1].moveTo(-55, 24);
			//}
			/*else
			{*/
				if (oneopp == 0)//1号对位0号
				{
					if (d01 >= 20.01234567)
					{
						baseRobots[1].moveTo(112, 16.5123456);
					}
					else if (d01 < 20.01234567 && d01 >= 15.01234567)
					{
						onemovex = oppRobots[0].getPos().x;
						onemovey = oppRobots[0].getPos().y + 5;
						baseRobots[1].moveTo(112, 16.5123456);
					}
					else
					{
						baseRobots[1].moveTo(onemovex, onemovey);
					}
				}

				if (oneopp == 1)//1号对位1号
				{
					if (d11 >= 20.01234567)
					{
						baseRobots[1].moveTo(112, 16.5123456);
					}
					else if (d11 < 20.01234567 && d11 >= 15.01234567)
					{
						onemovex = oppRobots[1].getPos().x;
						onemovey = oppRobots[1].getPos().y + 5;
						baseRobots[1].moveTo(112, 16.5123456);
					}
					else
					{
						baseRobots[1].moveTo(onemovex, onemovey);
					}
				}
			//}
			
		}
		//if (race_state_trigger == Team::Self)//开球进攻
		//{	
		//	
		//	double d00, d10, d01, d11, d30, d60, d111, d222;
		//	d00 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[0].getPos().y), 2)));
		//	d11 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[1].getPos().y), 2)));
		//	d10 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[0].getPos().y), 2)));
		//	d01 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[1].getPos().y), 2)));

		//	double x10, y10, x20, y20;

		//	double pre0, pre1, avert0, avert1;
		//	double x1, y1, x2, y2, x3, y3, gapx, gapy;
		//	x1 = oppRobots[0].getLastPos().x;
		//	y1 = oppRobots[0].getLastPos().y;
		//	x2 = oppRobots[0].getPos().x;
		//	y2 = oppRobots[0].getPos().y;
		//	gapx = x2 - x1;
		//	
		//	gapy = y2 - y1;
		//	x3 = x2 + 2 * gapx;
		//	y3 = x3 + 2 * gapy;
		//	d30 = sqrt((pow((x3 - baseRobots[0].getPos().x), 2) + pow((y3 - baseRobots[0].getPos().y), 2)));
		//	
		//	avert0 = baseRobots[0].getPos().y + 2 * gapy;

		//	double x4, y4, x5, y5, x6, y6, gapsx, gapsy;
		//	x4 = oppRobots[1].getLastPos().x;
		//	y4 = oppRobots[1].getLastPos().y;
		//	x5 = oppRobots[1].getPos().x;
		//	y5 = oppRobots[1].getPos().y;
		//	gapsx = x5 - x4;
		//	gapsy = y5 - y4;
		//	x6 = x5 + 2 * gapsx;
		//	y6 = x5 + 2 * gapsy;
		//	d60 = sqrt((pow((x6 - baseRobots[0].getPos().x), 2) + pow((y6 - baseRobots[0].getPos().y), 2)));
		//	avert1 = baseRobots[0].getPos().y + 2 * gapsy;

		//	
		//	if (d00 <= d10)
		//	{
		//		if (baseRobots[0].getPos().x <= 0 && baseRobots[1].getPos().x <= 0)
		//		{
		//			baseRobots[0].moveTo(110, 10);
		//			baseRobots[1].moveTo(110, 10);
		//		}

		//		if (baseRobots[0].getPos().x > 0 && baseRobots[1].getPos().x <= 0)
		//		{
		//			baseRobots[1].moveTo(110, 10);
		//			if (baseRobots[0].getPos().x > 0)
		//			{
		//				if (baseRobots[0].getPos().x <= oppRobots[0].getPos().x)
		//				{
		//					if (d00 >= 15.55578959)
		//					{
		//						baseRobots[0].moveTo(110, 10);
		//					}
		//					if (d00 < 15.55578959)
		//					{
		//						if (d00 >= 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x3, y3 - 15.15128495);
		//						}
		//						if (d00 < 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x6, y6);
		//						}
		//					}
		//				}
		//				if (baseRobots[0].getPos().x > oppRobots[0].getPos().x)
		//				{
		//					if (d30 >= 10.55578959)
		//					{
		//						baseRobots[0].moveTo(110, -12);
		//					}
		//					if (d30 < 10.55578959)
		//					{
		//						if (avert0 >= -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, avert0);
		//						}
		//						if (avert0 < -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, -15);
		//						}
		//					}
		//				}
		//			}
		//		}

		//		if (baseRobots[0].getPos().x > 0 && baseRobots[1].getPos().x > 0)
		//		{
		//			if (baseRobots[0].getPos().x <= 80.5153158)
		//			{
		//				if (baseRobots[0].getPos().x < oppRobots[0].getPos().x)
		//				{
		//					if (d00 >= 15.55578959)
		//					{
		//						baseRobots[0].moveTo(110, 12);
		//						baseRobots[1].moveTo(110, 12);
		//					}
		//					if (d00 < 15.55578959)
		//					{
		//						if (d00 >= 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x3, y3 - 15.15128495);
		//							baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y + 10.15128495);
		//						}
		//						if (d00 < 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x6, y6);
		//							baseRobots[1].moveTo(110, -12);
		//						}
		//					}
		//				}
		//				if (baseRobots[0].getPos().x >= oppRobots[0].getPos().x)
		//				{
		//					if (d30 >= 10.55578959)
		//					{
		//						baseRobots[0].moveTo(110, -12);
		//						baseRobots[1].moveTo(110, -12);
		//					}
		//					if (d30 < 10.55578959)
		//					{
		//						if (avert0 >= -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, avert0);
		//							baseRobots[1].moveTo(110, 0);
		//						}
		//						if (avert0 < -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, -15);
		//							baseRobots[1].moveTo(110, 0);
		//						}
		//					}
		//				}
		//			}
		//			if (baseRobots[0].getPos().x >80.5153158)
		//			{
		//				baseRobots[0].moveTo(110, 16);
		//				baseRobots[1].moveTo(110, -16);
		//			}
		//			
		//		}
		//	}
		//	if (d00 > d10)
		//	{
		//		if (baseRobots[0].getPos().x <= 0 && baseRobots[1].getPos().x <= 0)
		//		{
		//			baseRobots[0].moveTo(110, 12);
		//			baseRobots[1].moveTo(110, 12);
		//		}

		//		if (baseRobots[0].getPos().x > 0 && baseRobots[1].getPos().x <= 0)
		//		{
		//			baseRobots[1].moveTo(110, 10);
		//			if (baseRobots[0].getPos().x > 0)
		//			{
		//				if (baseRobots[0].getPos().x <= oppRobots[1].getPos().x)
		//				{
		//					if (d10 >= 15.55578959)
		//					{
		//						baseRobots[0].moveTo(110, 10);
		//					}
		//					if (d10 < 15.55578959)
		//					{
		//						if (d10 >= 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x6, y6 - 15.15128495);
		//						}
		//						if (d10 < 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x3, y3);
		//						}
		//					}
		//				}
		//				if (baseRobots[0].getPos().x > oppRobots[1].getPos().x)
		//				{
		//					if (d60 >= 10.55578959)
		//					{
		//						baseRobots[0].moveTo(110, -12);
		//					}
		//					if (d60 < 10.55578959)
		//					{
		//						if (avert1 >= -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, avert1);
		//						}
		//						if (avert1 < -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, -15);
		//						}
		//					}
		//				}
		//			}
		//		}

		//		if (baseRobots[0].getPos().x > 0 && baseRobots[1].getPos().x > 0)
		//		{
		//			if (baseRobots[0].getPos().x <= 80.5153158)
		//			{
		//				if (baseRobots[0].getPos().x < oppRobots[1].getPos().x)
		//				{
		//					if (d10 >= 15.55578959)
		//					{
		//						baseRobots[0].moveTo(110, 12);
		//						baseRobots[1].moveTo(110, 12);
		//					}
		//					if (d10 < 15.55578959)
		//					{
		//						if (d10 >= 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x6, y6 - 15.15128495);
		//							baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y + 10.15128495);
		//						}
		//						if (d10 < 10.55168498)
		//						{
		//							baseRobots[0].moveTo(x6, y6);
		//							baseRobots[1].moveTo(110, -12);
		//						}
		//					}
		//				}
		//				if (baseRobots[0].getPos().x >= oppRobots[1].getPos().x)
		//				{
		//					if (d60 >= 10.55578959)
		//					{
		//						baseRobots[0].moveTo(110, -12);
		//						baseRobots[1].moveTo(110, -12);
		//					}
		//					if (d60 < 10.55578959)
		//					{
		//						if (avert1 >= -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, avert1);
		//							baseRobots[1].moveTo(110, 0);
		//						}
		//						if (avert1 < -12.64891315)
		//						{
		//							baseRobots[0].moveTo(110, -15);
		//							baseRobots[1].moveTo(110, 0);
		//						}
		//					}
		//				}
		//			}
		//			if (baseRobots[0].getPos().x > 80.5153158)
		//			{
		//				baseRobots[0].moveTo(110, 16);
		//				baseRobots[1].moveTo(110, -16);
		//			}

		//		}
		//	}
		//	
		//	
		//}
		if (race_state_trigger == Team::Opponent)
		{
			double d00, d10, d01, d11, d30, d60, d111, d222;
			d00 = sqrt((pow((oppRobots[0].getPos().x + 67), 2) + pow((oppRobots[0].getPos().y - 8), 2)));
			d11 = sqrt((pow((oppRobots[1].getPos().x + 67), 2) + pow((oppRobots[1].getPos().y - (-8)), 2)));
			d10 = sqrt((pow((oppRobots[1].getPos().x + 67), 2) + pow((oppRobots[1].getPos().y - 8), 2)));
			d01 = sqrt((pow((oppRobots[0].getPos().x + 67), 2) + pow((oppRobots[0].getPos().y - (-8)), 2)));

			//baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);//直接moveto
			//baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
			
			if (d10 >= d00)//通过距离判断对手位是谁
			{
				zeroopp = 0;//对方1号与0号的距离大于对方0号与0号的距离则 0号对0号， 1号对1号
				oneopp = 1;
			}
			else
			{
				zeroopp = 1;//对方1号与0号的距离小于对方0号与0号的距离则 0号对对方1号， 1号对对方0号
				oneopp = 0;
			}

			if (zeroopp == 0)
			{
				baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);

			}
			else if (zeroopp == 1)
			{
				baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);

			}
			if (oneopp == 0)
			{

				baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
			}
			else if (oneopp == 1)
			{

				baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
			}
			/*if (d10 >= d00)//这个不成功
			{
				zeroopp = 0;
				oneopp = 1;
			}
			else
			{
				zeroopp = 1;
				oneopp = 0;
			}

			if (zeroopp == 0)
			{
				baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
			
			}
			else if (zeroopp == 1)
			{
				baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
				
			}
			if (oneopp == 0)
			{
			
				baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
			}
			else if (oneopp == 1)
			{
				
				baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
			}*/
			/*if (tick == 10)
			{
				if (d111 <= d222)
				{
					if
					baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
					baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
				}
				if (d111 > d222)
				{
					baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
					baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
				}
			}*/

		}
		//if (race_state_trigger == Team::Opponent)//开球防守
		//{
		//	double d00, d10, d01, d11, d30, d60;
		//	d00 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[0].getPos().y), 2)));
		//	d11 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[1].getPos().y), 2)));
		//	d10 = sqrt((pow((oppRobots[1].getPos().x - baseRobots[0].getPos().x), 2) + pow((oppRobots[1].getPos().y - baseRobots[0].getPos().y), 2)));
		//	d01 = sqrt((pow((oppRobots[0].getPos().x - baseRobots[1].getPos().x), 2) + pow((oppRobots[0].getPos().y - baseRobots[1].getPos().y), 2)));


		//	double pre0, pre1, avert0, avert1;
		//	double x1, y1, x2, y2, x3, y3, gapx, gapy;
		//	x1 = oppRobots[0].getLastPos().x;
		//	y1 = oppRobots[0].getLastPos().y;
		//	x2 = oppRobots[0].getPos().x;
		//	y2 = oppRobots[0].getPos().y;
		//	gapx = x2 - x1;
		//	gapy = y2 - y1;
		//	x3 = x2 + 2 * gapx;
		//	y3 = x3 + 2 * gapy;
		//	d30 = sqrt((pow((x3 - baseRobots[0].getPos().x), 2) + pow((y3 - baseRobots[0].getPos().y), 2)));

		//	avert0 = baseRobots[0].getPos().y + 2 * gapy;

		//	double x4, y4, x5, y5, x6, y6, gapsx, gapsy;
		//	x4 = oppRobots[1].getLastPos().x;
		//	y4 = oppRobots[1].getLastPos().y;
		//	x5 = oppRobots[1].getPos().x;
		//	y5 = oppRobots[1].getPos().y;
		//	gapsx = x5 - x4;
		//	gapsy = y5 - y4;
		//	x6 = x5 + 2 * gapsx;
		//	y6 = x5 + 2 * gapsy;
		//	d60 = sqrt((pow((x6 - baseRobots[0].getPos().x), 2) + pow((y6 - baseRobots[0].getPos().y), 2)));
		//	avert1 = baseRobots[0].getPos().y + 2 * gapsy;

		//	if (d00 <= d10)
		//	{
		//		if (oppRobots[0].getPos().x <= baseRobots[0].getPos().x && oppRobots[1].getPos().x <= baseRobots[1].getPos().x)
		//		{
		//			baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//			baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//		}
		//		if (oppRobots[0].getPos().x > baseRobots[0].getPos().x && oppRobots[1].getPos().x <= baseRobots[1].getPos().x)
		//		{
		//			if (d00 <= d01)
		//			{
		//				baseRobots[0].moveTo(x3, y3);
		//				baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				if (oppRobots[0].getPos().x <= baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				}
		//				if (oppRobots[1].getPos().x > baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(x6, y6);
		//				}
		//			}

		//			if (d00 > d01)
		//			{
		//				baseRobots[1].moveTo(x3, y3);
		//				baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				if (oppRobots[0].getPos().x <= baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				}
		//				if (oppRobots[1].getPos().x > baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(x6, y6);
		//				}
		//			}
		//		}
		//		if (oppRobots[0].getPos().x <= baseRobots[0].getPos().x && oppRobots[1].getPos().x > baseRobots[1].getPos().x)
		//		{
		//			if (d10 <= d11)
		//			{
		//				baseRobots[0].moveTo(x6, y6);
		//				baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				if (oppRobots[1].getPos().x <= baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				}
		//				if (oppRobots[0].getPos().x > baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(x3, y3);
		//				}
		//			}

		//			if (d10 > d11)
		//			{
		//				baseRobots[1].moveTo(x6, y6);
		//				baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				if (oppRobots[1].getPos().x <= baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				}
		//				if (oppRobots[0].getPos().x > baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(x3, y3);
		//				}
		//			}
		//		}
		//	}
		//	
		//	if (d00 > d10)
		//	{
		//		if (oppRobots[1].getPos().x <= baseRobots[0].getPos().x && oppRobots[0].getPos().x <= baseRobots[1].getPos().x)
		//		{
		//			baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//			baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//		}
		//		if (oppRobots[1].getPos().x > baseRobots[0].getPos().x && oppRobots[0].getPos().x <= baseRobots[1].getPos().x)
		//		{
		//			if (d00 <= d01)
		//			{
		//				baseRobots[0].moveTo(x6, y6);
		//				baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				if (oppRobots[1].getPos().x <= baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				}
		//				if (oppRobots[0].getPos().x > baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(x3, y3);
		//				}
		//			}

		//			if (d00 > d01)
		//			{
		//				baseRobots[1].moveTo(x6, y6);
		//				baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				if (oppRobots[1].getPos().x <= baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				}
		//				if (oppRobots[0].getPos().x > baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(x3, y3);
		//				}
		//			}
		//		}
		//		if (oppRobots[1].getPos().x <= baseRobots[0].getPos().x && oppRobots[0].getPos().x > baseRobots[1].getPos().x)
		//		{
		//			if (d10 <= d11)
		//			{
		//				baseRobots[0].moveTo(x3, y3);
		//				baseRobots[1].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				if (oppRobots[0].getPos().x <= baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				}
		//				if (oppRobots[1].getPos().x > baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(x6, y6);
		//				}
		//			}

		//			if (d10 > d11)
		//			{
		//				baseRobots[1].moveTo(x3, y3);
		//				baseRobots[0].moveTo(oppRobots[1].getPos().x, oppRobots[1].getPos().y);
		//				if (oppRobots[0].getPos().x <= baseRobots[1].getPos().x)
		//				{
		//					baseRobots[1].moveTo(oppRobots[0].getPos().x, oppRobots[0].getPos().y);
		//				}
		//				if (oppRobots[1].getPos().x > baseRobots[0].getPos().x)
		//				{
		//					baseRobots[0].moveTo(x6, y6);
		//				}
		//			}
		//		}
		//	}
		//
		//}
	/*}*/
	//存储信息
	for (int i = 0; i < 5; i++)
	{
		baseRobots[i].saveLastInformation(footBallNow_X, footBallNow_Y);
	}
}

/**
* 获得策略信息
*/
void GetInstruction(Field* field) {
	tick = field->tick;

	for (int i = 0; i < 5; i++) {
		baseRobots[i].update(&(field->selfRobots[i]));
	}
	for (int i = 0; i < 5; i++)
	{
		oppRobots[i].update(&(field->opponentRobots[i]));
	}

	double footBallNow_X = field->ball.position.x;
	double footBallNow_Y = field->ball.position.y;

	strategy(footBallNow_X, footBallNow_Y);
	dataloader.set_tick_state(tick, race_state);
}

//void GetInstruction(Field* field) {
//	
//	tick = field->tick;
//
//	for (int i = 0; i < 5; i++) {
//		baseRobots[i].update(&(field->selfRobots[i]), resetHistoryRecord);	// 每一拍更新己方机器人信息给BaseRobot
//		oppRobots[i].update(&(field->opponentRobots[i]), resetHistoryRecord);	// 每一拍更新对方机器人信息给BaseRobot
//		
//		baseRobots[i].PredictRobotInformation(tick_delay);	// 计算预测的信息 PredictInformation[]
//	}
//
//	double footBallNow_X = field->ball.position.x;
//	double footBallNow_Y = field->ball.position.y;
//	
//
//}