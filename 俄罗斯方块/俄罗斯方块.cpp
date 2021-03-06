// 俄罗斯方块.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <graphics.h>      // 引用图形库头文件
#include <conio.h>
#include <math.h>
#include <time.h>

#pragma warning(disable : 4996)


#define WB 12 //宽的块数
#define HB 18 //高的块数
#define BLOCK_SIZE 20

DWORD interval = 500; //自动下落时间间隔 默认500ms
int GAMEOVER = 0;

class Border
{
public:
	int TopLeft[2] = { 5,5 }, TopRight[2] = { 5,-1 }, BottomLeft[2] = { -1,5 }, BottomRight[2] = { -1,-1 };
	// 此中保存的是索引 相当于(y,x)而不是(x,y)
};


enum CMD
{
	CMD_ROTATE, // 方块旋转
	CMD_LEFT, CMD_RIGHT, CMD_DOWN, // 方块左、右、下移动
	CMD_QUIT // 退出游戏
};
class map
{
public:
	int x;
	int y;

	int maps[18][12] = { 0 };

	int first = 1; //第一次产生新的块
	int blockTemp[7][7] = { 0 };
	int block[7][7] = { 0 };
	int blockNext[7][7] = { 0 };

	void _GetNewBlock();
	void GetNewBlock();
	Border* GetBorder();
	void OnRotate();										// 旋转方块
	void OnLeft();											// 左移方块
	void OnRight();											// 右移方块
	void OnDown();											// 下移方块
	void Curing();											// 固化

	map();
	~map();

private:

};
map::map()
{
	map::GetNewBlock();

}

map::~map()
{
}
void map::_GetNewBlock()
{
	/*

	生成一个新的 放入blockTemp
	从(3,3)初始化,随机像周围扩展
	一共获得四个块 结束

	*/
	int i = 0;
	int temp[7][7] = { 0 };
	int StartX = 3; int ThisX = 3;
	int StartY = 3; int ThisY = 3;
	temp[StartY][StartX] = 1;

	switch (rand() % 2) {
	case 0:
		// 多初始化一次
		switch (rand() % 4)
		{
		case 0: //向上
			StartY -= 1; break;
		case 1: //向下
			StartY += 1; break;
		case 2: //向左
			StartX -= 1; break;
		case 3: //向右
			StartX += 1; break;
		}
		temp[StartY][StartX] = 1;
		i = 1;
		switch (rand() % 2) {
		case 0:
			ThisX = StartX;
			ThisY = StartY;
		}

	case 1:
		for (; i < 3; i++) {
			do
			{
				switch (rand() % 4)
				{
				case 0: //向上
				{
					if (temp[ThisY - 1][ThisX] != 1) {
						ThisY -= 1;
						temp[ThisY][ThisX] = 1;
						goto label;
					}
					break;
				}
				case 1: //向下
				{
					if (temp[ThisY + 1][ThisX] != 1) {
						ThisY += 1;
						temp[ThisY][ThisX] = 1;
						goto label;
					}
					break;
				}
				case 2: //向左
				{
					if (temp[ThisY][ThisX - 1] != 1) {
						ThisX -= 1;
						temp[ThisY][ThisX] = 1;
						goto label;
					}
					break;
				}
				case 3: //向右
				{
					if (temp[ThisY][ThisX + 1] != 1) {
						ThisX += 1;
						temp[ThisY][ThisX] = 1;
						goto label;
					}
					break;
				}
				}
			} while (true);
			i -= 1;
		label: {}
		}
	}

	memcpy(map::blockTemp, temp, sizeof(temp));

}

void map::GetNewBlock()
{
	/*
	从(3,3)初始化,随机像周围扩展
	一共获得四个块 结束

	*/
	int i = 0;
	int temp[7][7] = { 0 };
	int StartX = 3; int ThisX = 3;
	int StartY = 3; int ThisY = 3;
	temp[StartY][StartX] = 1;
	if (first)
	{
		_GetNewBlock();
		memcpy(map::block, map::blockTemp, sizeof(map::blockTemp));
		_GetNewBlock();
		memcpy(map::blockNext, map::blockTemp, sizeof(map::blockTemp));
		first = 0;
	}
	else
	{
		memcpy(map::block, map::blockNext, sizeof(map::blockNext));
		_GetNewBlock();
		memcpy(map::blockNext, map::blockTemp, sizeof(map::blockTemp));
	}

	int flag = 1;
	map::x = 5;
	map::y = 0;

	// 判断游戏结束
	for (int j = 0; flag && (j < 7); j++) {
		for (int i = 0; flag && (i < 7); i++) {
			if (map::block[j][i] == 1) {
				if (map::block[j + 1][i] == 0 && map::maps[map::y + j + 1][map::x + i] == 1)
				{
					closegraph();
					GAMEOVER = 1;
				}
			}
		}
	}

}

Border * map::GetBorder()
{
	//获取边界的块,
	Border *border = new Border;

	int left = 0, top = 0, right = 0;

	for (int j = 0; j < 7; j++) {
		for (int i = 0; i < 7; i++) {
			if (maps[j][i] == 1) {
				if (j <= border->TopLeft[0] && i <= border->TopLeft[1]) {
					border->TopLeft[1] = i; border->TopLeft[0] = j;
				}
				if (j <= border->TopRight[0] && i >= border->TopRight[1]) {
					border->TopRight[1] = i; border->TopRight[0] = j;
				}
				if (j >= border->BottomLeft[0] && i <= border->BottomLeft[1]) {
					border->BottomLeft[1] = i; border->BottomLeft[0] = j;
				}
				if (j >= border->BottomRight[0] && i >= border->BottomRight[1]) {
					border->BottomRight[1] = i; border->BottomRight[0] = j;
				}
			}

		}
	}
	return border;
}

void map::OnRotate()
{
	int temp[7][7] = { 0 };
	int flag = 0; //是否异常跳出
	int CenterX = 3, CenterY = 3;
	for (int j = 0; (j < 7); j++) {
		for (int i = 0; (i < 7); i++) {
			if (map::block[j][i] == 1) {
				//double rho = sqrt((i- CenterX)*(i- CenterX) + (j- CenterY) * (j-CenterY));
				//double theta = asin(1.0*(j- CenterY) / rho);
				//int NewX = rho * sin(theta) + CenterX;
				//int NewY = rho * cos(theta) + CenterY;
				int NewX = -(j - CenterX)+3;
				int NewY = (i - CenterY)+3;
				if (map::maps[map::y + NewY][map::x + NewX] == 1) {
					memset(temp, 0, sizeof(temp));
					return;
				}
				else
				{
					temp[NewY][NewX] = 1;
				}
			}
		}
	}


	memcpy(map::block, temp, sizeof(temp));


}

void map::OnLeft()
{
	//auto Border = map::GetBorder();
	//边界判断

	//其他块判断
	int flag = 1;
	for (int j = 0; flag && (j < 7); j++) {
		for (int i = 0; flag && (i < 7); i++) {
			if (map::block[j][i] == 1) {
				if (map::block[j][i - 1] == 0 && map::maps[map::y + j][map::x + i - 1] == 1)
				{
					flag = 0;
				}
				else if (map::x + i - 1 < 0) {
					flag = 0;
				}
			}
		}
	}
	if (flag)
	{
		x -= 1;
		return;
	}
	return;

}

void map::OnRight()
{
	int flag = 1;
	for (int j = 0; flag && (j < 7); j++) {
		for (int i = 0; flag && (i < 7); i++) {
			if (map::block[j][i] == 1) {
				if (map::block[j][i + 1] == 0 && map::maps[map::y + j][map::x + i + 1] == 1)
				{
					flag = 0;
				}
				else if (map::x + i + 1 > WB - 1) {
					flag = 0;
				}
			}
		}
	}
	if (flag)
	{
		x += 1;
		return;
	}
	return;


}

void map::OnDown()
{
	int flag = 1;
	for (int j = 0; flag && (j < 7); j++) {
		for (int i = 0; flag && (i < 7); i++) {
			if (map::block[j][i] == 1) {
				if (map::block[j + 1][i] == 0 && map::maps[map::y + j + 1][map::x + i] == 1)
				{
					map::Curing();
					flag = 0;
				}
				else if (map::y + j + 1 > HB - 1) {
					map::Curing();
					flag = 0;
				}
			}
		}
	}
	if (flag)
	{
		y += 1;
		return;
	}
	return;

}

void map::Curing()
{
	// 固化
	for (int j = 0; j < 7; j++) {
		for (int i = 0; i < 7; i++) {
			if (map::block[j][i] == 1) {
				map::maps[map::y + j][map::x + i] = 1;
			}
		}
	}

	//消除
	for (int j = 0; j < HB; j++) {
		int n = 0;
		for (int i = 0; i < WB; i++) {
			if (map::maps[j][i] == 1) {
				n += 1;
			}
		}
		if (n == WB) {
			for (int x = 0; x < WB; x++) {
				map::maps[j][x] = 0;
			}

			for (int y = j; y > 0; y--) {
				for (int x = 0; x < WB; x++) {
					map::maps[y][x] = map::maps[y - 1][x];
				}
			}

		}
	}

	map::GetNewBlock();
}




class Game
{
public:
	Game();
	~Game();
	int W; //宽
	int H; //高
	map map;
	CMD GetCommand();
	void Draw();

private:

};

Game::Game()
{
	W = 240;
	H = 360;
	initgraph(555, 555); //初始化窗口

	setbkmode(TRANSPARENT);
	solidrectangle(0, 0, W, H);

	outtextxy(10, H + 20, _T("操作说明"));
	outtextxy(10, H + 40, _T("上：旋转"));
	outtextxy(10, H + 60, _T("左：左移"));
	outtextxy(10, H + 80, _T("右：右移"));
	outtextxy(10, H + 100, _T("下：下移"));


	srand((unsigned)time(0));
	srand((unsigned)rand());
}

Game::~Game()
{

}
CMD Game::GetCommand()				//获取方向
{
	auto startTime = GetTickCount();
	while (true)
	{
		// 超时相当于下一个
		DWORD newtime = GetTickCount();
		if (newtime - startTime >= interval)
		{
			startTime = newtime;
			return CMD_QUIT;
		}

		// 如果有按键，返回按键对应的功能
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'W':	return CMD_ROTATE;
			case 'a':
			case 'A':	return CMD_LEFT;
			case 'd':
			case 'D':	return CMD_RIGHT;
			case 's':
			case 'S':	return CMD_DOWN;
			case 27:	return CMD_QUIT;

			}
		}
		Sleep(10);
	}

}



void Game::Draw() {
	cleardevice();
	setfillcolor(WHITE);
	solidrectangle(0, 0, W, H);

	outtextxy(10, H + 20, _T("操作说明"));
	outtextxy(10, H + 40, _T("上：旋转"));
	outtextxy(10, H + 60, _T("左：左移"));
	outtextxy(10, H + 80, _T("右：右移"));
	outtextxy(10, H + 100, _T("下：下移"));


	setfillcolor(BLUE);
	setlinecolor(WHITE);
	auto maps = Game::map.maps;


	// 画出游戏盘
	for (int j = 0; j < HB; j++) {
		for (int i = 0; i < WB; i++) {
			if (maps[j][i] == 1)
				fillrectangle(i*BLOCK_SIZE, j*BLOCK_SIZE, (i + 1) * BLOCK_SIZE, (j + 1)*BLOCK_SIZE);
		}
	}
	// 画出下一个
	setfillcolor(YELLOW);
	setlinecolor(WHITE);
	auto block = Game::map.blockNext;

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (block[j][i])
				fillrectangle((i + WB)*BLOCK_SIZE, (j + 5)*BLOCK_SIZE, (i + 1 + WB) * BLOCK_SIZE, (j + 1 + 5)*BLOCK_SIZE);
		}
	}



	// 画出当前块
	setfillcolor(RED);
	setlinecolor(RED);
	block = Game::map.block;

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (block[j][i])
				fillrectangle((Game::map.x + i)*BLOCK_SIZE, (Game::map.y + j)*BLOCK_SIZE, (Game::map.x + i + 1) * BLOCK_SIZE, (Game::map.y + j + 1)*BLOCK_SIZE);
		}
	}

}

int main()
{

	while (true)
	{
		Game Game;
		GAMEOVER = 0;
		auto startTime = GetTickCount();
		while (!GAMEOVER)
		{
			CMD c = Game.GetCommand();

			switch (c)
			{
			case CMD_ROTATE:	Game.map.OnRotate(); break;
			case CMD_LEFT:		Game.map.OnLeft();  break;
			case CMD_RIGHT:		Game.map.OnRight(); break;
			case CMD_DOWN:		Game.map.OnDown();	break;
			case CMD_QUIT:		break;
			}

			Game.Draw();


			// 500ms执行一次下降
			DWORD newtime = GetTickCount();
			if (newtime - startTime >= interval)
			{
				startTime = newtime;
				Game.map.OnDown();
			}

			Sleep(50);
		}



		printf("你死了\n还玩吗? (Y/N) \n");
		char c;
		scanf("%c", &c);
		if (c == 'y' || c == 'Y') {
			printf("少侠");
			system("pause");
			Game.~Game();
		}
		else
			break;

		

	}





}
