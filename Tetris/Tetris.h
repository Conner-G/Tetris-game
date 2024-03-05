#pragma once
#include <vector>
#include <graphics.h>
using namespace std;
#include "Block.h"

class Tetris
{
public:
	//���캯��
	Tetris(int rows, int cols, int left, int top, int blockSize);
	//rows ������ cols ������
	//left ������߽絽��Ϸ������߽�ľ��룻 top �����ϱ߽絽��Ϸ�����ϱ߽�ľ��룻 blockSize �����С		����λ�����أ�
	void init();	//��ʼ��
	void play();	//��ʼ��Ϸ

private:
	void keyEvent();
	void updateWindow();

	//���ؾ�����һ�ε��øú���������˶���ʱ��	����λ�����룩
	//��һ�ε��øú���������0
	int getDelay();	
	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();	//��ת
	void drawScore();	//���Ƶ�ǰ����
	void checkOver();	//�����Ϸ�Ƿ����
	void saveScore();	//������߷�
	void displayOver();	//������Ϸ��������

private:
	int delay;
	bool update;	//�Ƿ����

	//int map[20][10];
	//0���հף�û���κη���
	//5���ǵ�5�ֶ���˹����
	vector<vector<int>> map;	//��ά����
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;	//����ͼƬ
	IMAGE imgOver;
	IMAGE imgWin;
	Block* currBlock;	//��ǰ����
	Block* nextBlock;	//Ԥ�淽��
	Block bakBlock;		//��ǰ���齵������У�����������һ���Ϸ�λ��

	int score;	//��ǰ�ķ���
	int highestScore;	//��߷�
	int level;	//��ǰ�Ĺؿ�
	int lineCount;	//��ǰ�Ѿ������˶�����
	bool gameOver;	//��Ϸ�Ƿ��Ѿ�����


};

