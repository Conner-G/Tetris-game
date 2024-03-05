#pragma once
#include <graphics.h>	//需要先安装easyx图形库
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
	void drop();	//下落
	void moveLeftRight(int offset);	//左右移
	void rotate();	//旋转
	void draw(int leftMargin, int topMargin);	//绘制
	static IMAGE** getImages();
	Block& operator=(const Block& other);

	bool blockInMap(const vector<vector<int>> &map);
	//Point* getSmallBlocks();

	void solidify(vector<vector<int>>& map);
	int getBlockType();

private:
	int blockType;	//方块的类型
	Point smallBlocks[4];
	IMAGE *img;

	static IMAGE* imgs[7];
	static int size;
};

