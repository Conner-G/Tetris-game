#pragma once
#include <graphics.h>	//��Ҫ�Ȱ�װeasyxͼ�ο�
#include <vector>
using namespace std;

struct Point {
	int row;
	int col;
};

class Block
{
public:
	Block();
	void drop();	//����
	void moveLeftRight(int offset);	//������
	void rotate();	//��ת
	void draw(int leftMargin, int topMargin);	//����
	static IMAGE** getImages();
	Block& operator=(const Block& other);

	bool blockInMap(const vector<vector<int>> &map);
	//Point* getSmallBlocks();

	void solidify(vector<vector<int>>& map);
	int getBlockType();

private:
	int blockType;	//���������
	Point smallBlocks[4];
	IMAGE *img;

	static IMAGE* imgs[7];
	static int size;
};

