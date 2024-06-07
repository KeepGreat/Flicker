#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <vector>

#include "ED_const.h"
#include "ED_Class.h"
#include "ED_File.h"


extern Map map;
extern std::vector<BaseTower*> towers;
extern std::vector<Enermy*> enermys;
extern std::vector<Text*> txts;
/** 固定格式：
 *  0：Player Left: %d || 剩余生命值
 *  1：Level: %d || 关卡
 *  2：Coins: %d || 金币
 *  3：Phases Left: %d || 剩余波数
*/
extern std::vector<Image*> imgs;
/** 固定格式：
 *  100+ || 0: 游戏界面，不属于Page类管理范围内
 *  1: 初始界面，能够跳转100、2、3
 *  2: 选关界面，能够跳转100+并修改Level
 *  3: 帮助界面，能够跳转1
 *  4: 胜利界面，能够跳转2并修改Level
 *  5: 胜利界面，能够跳转2并修改Level
 *  6: 很6界面，犯病专用
 *  100: 过关界面，能够跳转2、100+并修改Level, 不属于Page类管理范围内
 *  200: 进行下一关，别问为什么还要设置这么一个index，问就是参数是变量
*/
extern std::vector<Page*> pages;

int Init();
void Destroy();

int Init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Init failed\n");
        return -1;
    }

    win = SDL_CreateWindow("好 玩 到 爆 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (win == NULL)
    {
        SDL_Log("Create Window failed: %s", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(win, -1, 0);
    if (renderer == NULL)
    {
        SDL_Log("Create Renderer failed: %s", SDL_GetError());
        return -1;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, de_color.r, de_color.g, de_color.b, de_color.a);

    DetectLevel();

    TTF_Init();
    font = TTF_OpenFont("./resource/comic.ttf", 30);

    IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG);

    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    bgm = Mix_LoadMUS("./resource/bgm.ogg");
    Mix_PlayMusic(bgm, -1);
    //Text组的初始化, 在Page[0]中
    {
        Text *t1 = new Text;
        t1->SetElement("Player Left: ", 10, 10);
        txts.push_back(t1);
        Text *t2 = new Text;
        t2->SetElement("Level: ", 450, 10);
        txts.push_back(t2);
        Text *t3 = new Text;
        t3->SetElement("Coins: ", 700, 10);
        txts.push_back(t3);
        Text *t4 = new Text;
        t4->SetElement("Phase Left: ", 10, 40);
        txts.push_back(t4);
    }

    //Image组的初始化, 在Page[0]中
    {
        Image *t1 = new Image;
        t1->SetElement("./resource/PauseButton.bmp", {1000, 0, 100, 100}, 3);
        imgs.push_back(t1);
        Image *t2 = new Image;
        t2->SetElement("./resource/PauseButton2.bmp", {1000, 0, 100 ,100}, 3);
        imgs.push_back(t2);
    }

    //Page组的初始化
    {
        Page *nullhead = nullptr;
        pages.push_back(nullhead);
        
        Page *p1 = new Page(1);//初始界面
        Image *p1b1 = new Image("./resource/Button_Start.bmp", {500, 350, 200, 100}, 3, 0);
        p1->AddButton(p1b1);
        Text *p1t1 = new Text;
        p1t1->SetElement("————KeepGreat", 600, 250);
        p1->AddElement(p1t1);
        Text *p1t2 = new Text;
        p1t2->SetElement("v 1.0.0 ", 1100, 750);
        p1->AddElement(p1t2);
        Image *p1i1 = new Image("./resource/Image_Title1.bmp", {300, 100, 500, 100}, 3);
        p1->AddElement(p1i1);
        Image *p1i2 = new Image("./resource/Image_Title2.bmp", {800, 150, 100, 100}, 3);
        p1->AddElement(p1i2);
        Image *p1b2 = new Image("./resource/Button_ChooseLevel.bmp", {500, 500, 200, 100}, 3, 2);
        p1->AddButton(p1b2);
        Image *p1b3 = new Image("./resource/Button_Help.bmp", {500, 650, 200, 100}, 3, 3);
        p1->AddButton(p1b3);
        Image *p1b4 = new Image("./resource/WhatIsThis.jpg", {100, 600, 100, 100}, IMG_INIT_JPG, 666);
        p1->AddButton(p1b4);
        pages.push_back(p1);

        Page *p2 = new Page(2);//选关界面
        Image *p2b1 = new Image("./resource/Button_Back.bmp", {900, 600, 200, 100}, 3, 1);
        p2->AddButton(p2b1);
        Image *p2b2 = new Image("./resource/Button_0.bmp", {100, 100, 100, 100}, 3, 0);
        p2->AddButton(p2b2);
        Image *p2b3 = new Image("./resource/Button_1.bmp", {250, 100, 100, 100}, 3, 101);
        p2->AddButton(p2b3);
        Image *p2b4 = new Image("./resource/Button_2.bmp", {400, 100, 100, 100}, 3, 102);
        p2->AddButton(p2b4);
        Image *p2b5 = new Image("./resource/Button_3.bmp", {550, 100, 100, 100}, 3, 103);
        p2->AddButton(p2b5);
        Image *p2b6 = new Image("./resource/Button_4.bmp", {700, 100, 100, 100}, 3, 104);
        p2->AddButton(p2b6);
        Image *p2b7 = new Image("./resource/Button_5.bmp", {850, 100, 100, 100}, 3, 105);
        p2->AddButton(p2b7);
        Image *p2b8 = new Image("./resource/Button_6.bmp", {1000, 100, 100, 100}, 3, 106);
        p2->AddButton(p2b8);
        Image *p2b9 = new Image("./resource/Button_7.bmp", {100, 250, 100, 100}, 3, 107);
        p2->AddButton(p2b9);
        Image *p2b10 = new Image("./resource/Button_8.bmp", {250, 250, 100, 100}, 3, 108);
        p2->AddButton(p2b10);
        Image *p2b11 = new Image("./resource/Button_9.bmp", {400, 250, 100, 100}, 3, 109);
        p2->AddButton(p2b11);
        Image *billcipher = new Image("./resource/Button_10.jpg", {550, 250, 100, 110}, IMG_INIT_JPG, 110);
        p2->AddButton(billcipher);
        pages.push_back(p2);

        Page *p3 = new Page(3);//帮助界面
        Image *p3i1 = new Image("./resource/Image_Help.bmp", {0, 0, 1200, 500}, 3);
        p3->AddElement(p3i1);
        Image *p3b1 = new Image("./resource/Button_Back.bmp", {900, 600, 200, 100}, 3, 1);
        p3->AddButton(p3b1);
        pages.push_back(p3);

        Page *p4 = new Page(4);//胜利界面
        Image *p4i1 = new Image("./resource/Image_Congra.bmp", {0, 0, 1200, 600}, 3);
        p4->AddElement(p4i1);
        Image *p4b1 = new Image("./resource/Button_NextLevel.bmp", {450, 650, 100, 100}, 3, 200);
        p4->AddButton(p4b1);
        Image *p4b2 = new Image("./resource/Button_BackTo2.bmp", {600, 650, 100, 100}, 3, 2);
        p4->AddButton(p4b2);
        pages.push_back(p4);

        Page *p5 = new Page(5);//失败界面
        Image *p5i1 = new Image("./resource/Image_Encour.bmp", {0, 0, 1200, 600}, 3);
        p5->AddElement(p5i1);
        Image *p5b2 = new Image("./resource/Button_BackTo2.bmp", {600, 650, 100, 100}, 3, 2);
        p5->AddButton(p5b2);
        pages.push_back(p5);
    }
    return 0;
}

void Destroy()
{
    for (int i = 0; i < pages.size(); i++)
    {
        delete pages[i];
    }
    pages.clear();
    for (int i = 0; i < 2; i++)
    {
        delete imgs[i];
    }
    imgs.clear();
    for (int i = 0; i < TEXT_NUMBER; i++)
    {
        delete txts[i];
    }
    txts.clear();

    std::vector<BaseTower*>::iterator thead = towers.begin();
    std::vector<BaseTower*>::iterator trear = towers.end();
    for (; thead != trear; thead++)
    {
        delete *thead;
    }
    towers.clear();

    std::vector<Enermy*>::iterator ehead = enermys.begin();
    std::vector<Enermy*>::iterator erear = enermys.end();
    for (; ehead != erear; ehead++)
    {
        delete *ehead;
    }
    enermys.clear();

    Mix_FreeMusic(bgm);
    Mix_CloseAudio();
    Mix_Quit();
    
    IMG_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}