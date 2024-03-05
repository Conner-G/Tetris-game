/*
1. 创建项目
2. 先导入素材（找美工）
3. C++开发
4. 设计C++项目的模块
	即设计类: Block 表示方块； Tetris 表示俄罗斯方块游戏
5. 设计各个模块的主要接口
6. 启动游戏
*/

#include "Tetris.h"

int main(void) {
	Tetris game(20, 10, 263, 133, 36);
	game.play();
	return 0;

}