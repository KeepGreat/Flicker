#include "ED_Init.h"
#include "ED_Loop.h"
#include "ED_Class.h"
#include <iostream>
//#include <list> list不支持数组表示法与随机访问
#include <vector>

#undef main

Map map;
std::vector<BaseTower*> towers;
std::vector<Enermy*> enermys;
std::vector<Text*> txts;
std::vector<Image*> imgs;
std::vector<Page*> pages;

int main(void)
{
    if (Init() < 0)
    {
        return -1;
    }
    EventLoop();
    Destroy();
    return 0;
}