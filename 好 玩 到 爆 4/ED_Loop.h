#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <vector>
#include <ctime>

#include "ED_const.h"
#include "ED_Class.h"


extern Map map;
extern std::vector<BaseTower*> towers;
extern std::vector<Enermy*> enermys;
extern std::vector<Text*> txts;
extern std::vector<Image*> imgs;
extern std::vector<Page*> pages;

void EventLoop();
void Draw();
void BuildTower(SDL_Event *event);
void UpgradeTower(SDL_Event *event);
void SummonEnermy(SDL_Event *event); //test func
void SummonEnermy();
void Detect(Map &map);
void DetectChosed(SDL_Event *event);
void DetectClick(SDL_Event *event);
void Operate();
void Eliminate();
void InfoQuery(SDL_Event *event);
void StatChange(SDL_Event *event);
void Pause(SDL_Event *event);
void PageInteract(SDL_Event *event);
void EndLevel();
void LoadLevel();
void ExitLevel(SDL_Event *event);

void EventLoop()
{
    SDL_Event event;
    Uint64 start, end;
    int elapse;
    while (GameState)
    {
        start = SDL_GetTicks64();
        if (!PageIndex)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT :
                    {
                        GameState = 0;
                        return;
                    }
                    case SDL_MOUSEBUTTONDOWN :
                    {
                        DetectChosed(&event);
                        DetectClick(&event);
                    }
                    case SDL_KEYDOWN :
                    {
                        BuildTower(&event);
                        UpgradeTower(&event);
                        InfoQuery(&event);
                        if (ConsoleMode)
                        {
                            StatChange(&event);
                            SummonEnermy(&event);
                        }
                        Pause(&event);
                        ExitLevel(&event);
                    }
                }
            }
            if (!IsPause)
            {
                Operate();
                Detect(map);
                SummonEnermy();
                Eliminate();
                EndLevel();
                Draw();
            }
            else
            {
                StartTick = clock();
            }
            end = SDL_GetTicks64();
            elapse = mSPF - (end - start);
            if (elapse > 0)
            {
                SDL_Delay(elapse);
            }
        }
        else
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                    {
                        GameState = 0;
                        return;
                    }
                    case SDL_MOUSEBUTTONDOWN:
                    {
                        PageInteract(&event);
                    }
                }
            }
            Draw();
            end = SDL_GetTicks64();
            elapse = mSPF - (end - start);
            if (elapse > 0)
            {
                SDL_Delay(elapse);
            }
        }
    }
}

void Draw()
{
    SDL_RenderClear(renderer);

    if (!PageIndex)
    {
        map.Draw();

        std::vector<Enermy*>::iterator ehead = enermys.begin();
        std::vector<Enermy*>::iterator etail = enermys.end();
        for (; ehead != etail; ehead++)
        {
            (*ehead)->Draw();
        }

        std::vector<BaseTower*>::iterator thead = towers.begin();
        std::vector<BaseTower*>::iterator trear = towers.end();
        for (; thead != trear; thead++)
        {
            (*thead)->Draw();
        }

        for (int i = 0; i < TEXT_NUMBER; i++)
        {
            txts[i]->Draw();
        }

        for (int i = 0; i < 1; i++)
        {
            imgs[i]->Draw();
        }

    }
    else
    {
        int index = PageIndex;
        pages[index]->Draw();
    }

    SDL_RenderPresent(renderer);
}

void BuildTower(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_1)
    {
        if (Coin >= TURRET_COST)
        {
            BaseTower *temp = new Turret("./resource/Turret.bmp", TURRET_COST, TURRET_REALM, TURRET_DAMAGE);
            if (temp->BuildTower(map))
            {
                towers.push_back(temp);
                Coin -= TURRET_COST;
            }
            else
            {
                delete temp;
            }
        }
    }
    if (event->key.keysym.sym == SDLK_2)
    {
        if (Coin >= FROST_COST)
        {
            BaseTower *temp = new Frost("./resource/Frost.bmp", FROST_COST, FROST_REALM, FROST_DAMAGE);
            if (temp->BuildTower(map))
            {
                towers.push_back(temp);
                Coin -= FROST_COST;
            }
            else
            {
                delete temp;
            }
        }
    }
}

void UpgradeTower(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_e)
    {
        for (int i = 0; i < towers.size(); i++)
        {
            towers[i]->Upgrade();
        }
    }
}

void SummonEnermy(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_s)
    {
        Enermy *temp = new Enermy(ENERMY_DE_HEALTH, ENERMY_DE_SPEED);
        temp->Summon(map);
        enermys.push_back(temp);
    }
}

void SummonEnermy()
{
    clock_t tick = clock();
    clock_t elapse;
    if (enermys.size() < 5 && Phase > 0)
    {
        elapse = clock_t(CLOCKS_PER_SEC * float(2 - Level * 0.1 + Phase * 0.2));
        if (tick - StartTick >= elapse)
        {
            Enermy *temp = new Enermy(ENERMY_DE_HEALTH + Level * 2, ENERMY_DE_SPEED);
            temp->Summon(map);
            enermys.push_back(temp);
            StartTick = tick;
        }
    }
}

void Detect(Map &map)
{
    std::vector<Enermy*>::iterator head = enermys.begin();
    std::vector<Enermy*>::iterator tail = enermys.end();
    for (; head != tail; head++)
    {
        if ((*head)->ArriveEnd(map))
        {
            delete *head;
            enermys.erase(head);
            Player--;
            ElimiCount++;
        }
    }
}

void DetectChosed(SDL_Event *event)
{
    map.DetectChosed(event);
    for (int i = 0; i < towers.size(); i++)
    {
        towers[i]->DetectChosed(event);
    }
}

void DetectClick(SDL_Event *event)
{
    if (IsPause)
    {
        IsPause = 0; //偷鸡 is not allowed!
    }
    for (int i = 0; i < enermys.size(); i++)
    {
        enermys[i]->ReactToClick(event);
    }
}

void Operate()
{
    std::vector<Enermy*>::iterator ehead = enermys.begin();
    std::vector<Enermy*>::iterator etail = enermys.end();
    for (; ehead != etail; ehead++)
    {
        (*ehead)->Operate(map);
    }

    std::vector<BaseTower*>::iterator thead = towers.begin();
    std::vector<BaseTower*>::iterator ttail = towers.end();
    for (; thead != ttail; thead++)
    {
        (*thead)->Operate();
    }

    //Text类的运行
    {
        txts[0]->Operate(Player);
        txts[1]->Operate(Level);
        txts[2]->Operate(Coin);
        txts[3]->Operate(Phase);
    }

    if (ElimiCount * 100 >= 500 + (Level * 50))
    {
        Phase--;
        ElimiCount = 0;
    }
}

void Eliminate()
{
    std::vector<Enermy*>::iterator ehead = enermys.begin();
    std::vector<Enermy*>::iterator etail = enermys.end();
    for (; ehead != etail; ehead++)
    {
        if ((*ehead)->GetHealth() <= 0)
        {
            delete *ehead;
            enermys.erase(ehead);
            ElimiCount++;
            Coin += 25;
        }
    }
}

void InfoQuery(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_i)
    {
        SDL_Log("The amount of Grounds: %d", map.GetGrouNum());
        SDL_Log("The amount of Enermys: %d", enermys.size());
        SDL_Log("The amount of Towers: %d", towers.size());
        SDL_Log("Level : %d\n", Level);
        SDL_Log("PageIndex : %d\n", PageIndex);
    }
}

void StatChange(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_p)
    {
        Player++;
        Coin += 100;
    }
    if (event->key.keysym.sym == SDLK_m)
    {
        Phase--;
    }
}

void Pause(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_SPACE)
    {
        if (!IsPause)
        {
            imgs[1]->Draw();
            SDL_RenderPresent(renderer);
            IsPause = 1;
            return;
        }
        if (IsPause)
        {
            imgs[0]->Draw();
            SDL_RenderPresent(renderer);
            IsPause = 0;
            return;
        }
    }
}

void PageInteract(SDL_Event *event)
{
    int temp = pages[PageIndex]->SwitchTo(event);
    if (temp != PageIndex)
    {
        if (temp > 100 && temp != 200 && temp != 666)
        {
            temp -= 100;
            Level = temp;
            if (ReadLevel(Level, map) < 0)
            {
                SDL_Log("failed to read data\n");
                exit(1);
            }
            PageIndex = 0;
            return;
        }
        else if (temp == 200)
        {
            if (Level < MaxLevel)
            {
                Level++;
                SDL_Log("Now Level is : %d\n", Level);
                if (ReadLevel(Level, map) < 0)
                {
                    SDL_Log("failed to read data\n");
                    exit(1);
                }
            }
            else
            {
                PageIndex = 1;
                return;
            }
            PageIndex = 0;
            return;
        }
        else if (temp == 666)
        {
            ConsoleMode = 1;
            return;
        }
        else if (temp == 0)
        {
            Level = temp;
            if (ReadLevel(Level, map) < 0)
            {
                SDL_Log("failed to read data\n");
                exit(1);
            }
            PageIndex = 0;
            return;
        }
        pages[PageIndex]->Clear();
        PageIndex = temp;
    }
}

void EndLevel()
{
    //失败情况
    if (Player < 0 && Phase >= 0 && enermys.size() > 0)
    {
        SDL_Log("Oops! You lose!\n");
        Player = PLAYER;
        Phase = PHASE;
        Coin = COIN;
        map.Clear();
        for (int i = 0; i < enermys.size(); i++)
        {
            delete enermys[i];
        }
        enermys.clear();
        for (int i = 0; i < towers.size(); i++)
        {
            delete towers[i];
        }
        towers.clear();
        //剩下的文本内容通过覆盖即可
        PageIndex = 5;
    }
    else if (Player >= 0 && Phase <= 0 && enermys.size() == 0)
    {
        SDL_Log("Congratulation! You win this Level!\n");
        Player = PLAYER;
        Phase = PHASE;
        Coin = COIN;
        map.Clear();
        for (int i = 0; i < enermys.size(); i++)
        {
            delete enermys[i];
        }
        enermys.clear();
        for (int i = 0; i < towers.size(); i++)
        {
            delete towers[i];
        }
        towers.clear();
        //剩下的文本内容通过覆盖即可
        PageIndex = 4;
    }
}

void LoadLevel() //指完成关卡后跳入下一关
{
    //清理上一关场景
    map.Clear();
    for (int i = 0; i < enermys.size(); i++)
    {
        delete enermys[i];
    }
    enermys.clear();
    for (int i = 0; i < towers.size(); i++)
    {
        delete towers[i];
    }
    towers.clear();

    Player = PLAYER;
    Phase = PHASE;
    Coin = COIN;
    //加载下一关
    Level++;
    txts[0]->Operate(Player);
    txts[1]->Operate(Level);
    txts[2]->Operate(Coin);
    txts[3]->Operate(Phase);
    if (Level <= MaxLevel && ReadLevel(Level, map) < 0)
    {
        SDL_Log("failed to read data\n");
        exit(1);
    }
}

void ExitLevel(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_ESCAPE)
    {
        map.Clear();
        for (int i = 0; i < enermys.size(); i++)
        {
            delete enermys[i];
        }
        enermys.clear();
        for (int i = 0; i < towers.size(); i++)
        {
            delete towers[i];
        }
        towers.clear();
        Player = PLAYER;
        Phase = PHASE;
        Coin = COIN;
        //剩下的文本内容通过覆盖即可
        PageIndex = 1;
    }
}