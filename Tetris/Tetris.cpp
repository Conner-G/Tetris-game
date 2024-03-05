#include "Tetris.h"
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include "Block.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define MAX_LEVEL 5
#define RECORDER_FILE "recorder.txt"

//const int SPEED_NORMAL = 500;	//ms
const int SPEED_NORMAL[MAX_LEVEL] = { 500,400,300,200,100 };	//ms
const int SPEED_QUICK = 30;

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;

	for (int i = 0; i < rows; i++) {
		vector<int> mapRow;
		for (int j = 0; j < cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
	}
}

void Tetris::init()
{
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];

	//配置随机种子
	srand(time(NULL));

	//创建游戏窗口
	initgraph(938, 896);

	//加载背景图片
	loadimage(&imgBg, "res/bg2.png");

	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//初始化游戏区中的数据
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	score = 0;
	lineCount = 0;
	level = 1;

	//初始化最高分
	ifstream file(RECORDER_FILE);
	if (!file.is_open()) {
		cout << RECORDER_FILE << "打开失败" << endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close();	//关闭文件

	gameOver = false;
}

void Tetris::play()
{
	init();

	nextBlock = new Block;
	currBlock = nextBlock;
	nextBlock = new Block;
	
	int timer = 0;
	while (1) {		//所有游戏都是死循环
		//接受用户的输入
		keyEvent();

		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();

			//渲染游戏画面
			update = true;
		}
		if (update) {
			update = false;

			//更新游戏的画面
			updateWindow();

			//更新游戏的数据
			clearLine();
		}
		
		if (gameOver) {
			//保存分数
			saveScore();

			//更新游戏结束界面
			displayOver();

			system("pause");
			init();		//重新开局
		}
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;	//无符号char类型范围：0~255
	bool rotateFlag = false;
	int dx = 0;

	if (_kbhit()) {
		ch = _getch();

		//如果按下方向按键，会自动返回两个字符
		//如果按下↑方向键，会先后返回：224 72
		//如果按下↓方向键，会先后返回：224 80
		//如果按下←方向键，会先后返回：224 75
		//如果按下→方向键，会先后返回：224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}

	if (rotateFlag) {
		rotate();
		update = true;
	}

	if (dx != 0) {
		//实现左右移动
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);	//绘制背景图片
	BeginBatchDraw();	//解决画面闪烁问题

	IMAGE** imgs = Block::getImages();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) {
				continue;
			}
			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j] - 1]);
		}
	}

	currBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);

	drawScore();	//绘制分数

	EndBatchDraw();
}

//第一次调用，返回0
//以后返回距离上一次调用，间隔了多少ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;

	unsigned long long currentTime = GetTickCount();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	bakBlock = *currBlock;
	currBlock->drop();
	
	if (!currBlock->blockInMap(map)) {
		//把这个方块“固化”
		bakBlock.solidify(map);
		delete currBlock;
		currBlock = nextBlock;
		nextBlock = new Block;

		//检查游戏是否结束
		checkOver();
	}

	delay = SPEED_NORMAL[level - 1];
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1;	//存储数据的行数
	for (int i = rows - 1; i >= 0; i--) {
		//检查第i行是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];	//一边扫描一边存储
		}

		if (count < cols) {		//不是满行
			k--;
		}
		else {	//count == cols 满行
			lines++;
		}
	}

	if (lines > 0) {
		//计算得分
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//每100分一个级别 0-100分第1关 101-200分第2关
		level = (score + 99) / 100;
		if (level > MAX_LEVEL) {
			gameOver = true;
		}

		lineCount += lines;

	}
}

void Tetris::moveLeftRight(int offset)
{
	bakBlock = *currBlock;
	currBlock->moveLeftRight(offset);

	if (!currBlock->blockInMap(map)) {
		*currBlock = bakBlock;
	}
}

void Tetris::rotate()
{
	if (currBlock->getBlockType() == 7) return;	//“田”型方块则不用旋转
	
	bakBlock = *currBlock;
	currBlock->rotate();

	if (!currBlock->blockInMap(map)) {
		*currBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f);	//获取当前的字体
	f.lfHeight = 55;
	f.lfWidth = 25;
	f.lfQuality = ANTIALIASED_QUALITY;	//设置字体为“抗锯齿”效果
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Arial Black"));
	settextstyle(&f);

	setbkmode(TRANSPARENT);	//字体的背景设置为透明效果

	outtextxy(670, 727, scoreText);

	//绘制消除了多少行
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);

	//绘制当前是第几关
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(224 - 30, 727, scoreText);

	sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
	outtextxy(670, 817, scoreText);
}

void Tetris::checkOver()
{
	gameOver = (currBlock->blockInMap(map) == false);
}

void Tetris::saveScore()
{
	if (score > highestScore) {
		highestScore = score;

		ofstream file(RECORDER_FILE);
		file << highestScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop res/bg.mp3", 0, 0, 0);

	//判断胜利结束还是失败结束
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
