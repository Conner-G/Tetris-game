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

	//�����������
	srand(time(NULL));

	//������Ϸ����
	initgraph(938, 896);

	//���ر���ͼƬ
	loadimage(&imgBg, "res/bg2.png");

	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//��ʼ����Ϸ���е�����
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	score = 0;
	lineCount = 0;
	level = 1;

	//��ʼ����߷�
	ifstream file(RECORDER_FILE);
	if (!file.is_open()) {
		cout << RECORDER_FILE << "��ʧ��" << endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close();	//�ر��ļ�

	gameOver = false;
}

void Tetris::play()
{
	init();

	nextBlock = new Block;
	currBlock = nextBlock;
	nextBlock = new Block;
	
	int timer = 0;
	while (1) {		//������Ϸ������ѭ��
		//�����û�������
		keyEvent();

		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();

			//��Ⱦ��Ϸ����
			update = true;
		}
		if (update) {
			update = false;

			//������Ϸ�Ļ���
			updateWindow();

			//������Ϸ������
			clearLine();
		}
		
		if (gameOver) {
			//�������
			saveScore();

			//������Ϸ��������
			displayOver();

			system("pause");
			init();		//���¿���
		}
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;	//�޷���char���ͷ�Χ��0~255
	bool rotateFlag = false;
	int dx = 0;

	if (_kbhit()) {
		ch = _getch();

		//������·��򰴼������Զ����������ַ�
		//������¡�����������Ⱥ󷵻أ�224 72
		//������¡�����������Ⱥ󷵻أ�224 80
		//������¡�����������Ⱥ󷵻أ�224 75
		//������¡�����������Ⱥ󷵻أ�224 77
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
		//ʵ�������ƶ�
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);	//���Ʊ���ͼƬ
	BeginBatchDraw();	//���������˸����

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

	drawScore();	//���Ʒ���

	EndBatchDraw();
}

//��һ�ε��ã�����0
//�Ժ󷵻ؾ�����һ�ε��ã�����˶���ms
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
		//��������顰�̻���
		bakBlock.solidify(map);
		delete currBlock;
		currBlock = nextBlock;
		nextBlock = new Block;

		//�����Ϸ�Ƿ����
		checkOver();
	}

	delay = SPEED_NORMAL[level - 1];
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1;	//�洢���ݵ�����
	for (int i = rows - 1; i >= 0; i--) {
		//����i���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];	//һ��ɨ��һ�ߴ洢
		}

		if (count < cols) {		//��������
			k--;
		}
		else {	//count == cols ����
			lines++;
		}
	}

	if (lines > 0) {
		//����÷�
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//ÿ100��һ������ 0-100�ֵ�1�� 101-200�ֵ�2��
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
	if (currBlock->getBlockType() == 7) return;	//����ͷ���������ת
	
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
	gettextstyle(&f);	//��ȡ��ǰ������
	f.lfHeight = 55;
	f.lfWidth = 25;
	f.lfQuality = ANTIALIASED_QUALITY;	//��������Ϊ������ݡ�Ч��
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Arial Black"));
	settextstyle(&f);

	setbkmode(TRANSPARENT);	//����ı�������Ϊ͸��Ч��

	outtextxy(670, 727, scoreText);

	//���������˶�����
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);

	//���Ƶ�ǰ�ǵڼ���
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

	//�ж�ʤ����������ʧ�ܽ���
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
