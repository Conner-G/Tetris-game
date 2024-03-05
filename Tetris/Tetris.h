#pragma once
#include <vector>
#include <graphics.h>
using namespace std;
#include "Block.h"

class Tetris
{
public:
	//构造函数
	Tetris(int rows, int cols, int left, int top, int blockSize);
	//rows 行数； cols 列数；
	//left 背景左边界到游戏区域左边界的距离； top 背景上边界到游戏区域上边界的距离； blockSize 方块大小		（单位：像素）
	void init();	//初始化
	void play();	//开始游戏

private:
	void keyEvent();
	void updateWindow();

	//返回距离上一次调用该函数，间隔了多少时间	（单位：毫秒）
	//第一次调用该函数，返回0
	int getDelay();	
	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();	//旋转
	void drawScore();	//绘制当前分数
	void checkOver();	//检查游戏是否结束
	void saveScore();	//保存最高分
	void displayOver();	//更新游戏结束画面

private:
	int delay;
	bool update;	//是否更新

	//int map[20][10];
	//0：空白，没有任何方块
	//5：是第5种俄罗斯方块
	vector<vector<int>> map;	//二维数组
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;	//背景图片
	IMAGE imgOver;
	IMAGE imgWin;
	Block* currBlock;	//当前方块
	Block* nextBlock;	//预告方块
	Block bakBlock;		//当前方块降落过程中，用来备份上一个合法位置

	int score;	//当前的分数
	int highestScore;	//最高分
	int level;	//当前的关卡
	int lineCount;	//当前已经消除了多少行
	bool gameOver;	//游戏是否已经结束


};

