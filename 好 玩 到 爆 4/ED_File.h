#ifndef ED_READFILE_H
#define ED_READFILE_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <fstream>
#include <string>

#include "ED_Class.h"
#include "ED_const.h"
using namespace std;

void DetectLevel();
int ReadLevel(int Level, Map &map);

void DetectLevel()
{
    //检测所有Level
    sprintf(filename, "./resource/data/Level%d.txt", MaxLevel);
    ifstream fdet;
    fdet.open(filename);
    while (fdet.is_open())
    {
        MaxLevel++;
        sprintf(filename, "./resource/data/Level%d.txt", MaxLevel);
        fdet.close();
        fdet.clear();
        fdet.open(filename);
    }
    MaxLevel--;
    SDL_Log("Level Detection Completed, %d Levels are detected, The MaxLevel Is %d\n", MaxLevel + 1, MaxLevel);
}

int ReadLevel(int level, Map &map)
{
    sprintf(filename, "./resource/data/Level%d.txt", level);
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open())
    {
        SDL_Log("Can't open file!\n");
        return -1;
    }
    int g, p, x, y, w, h, px, py, pw, ph, oX, oY;
    fin >> g >> p;
    map.InitElement(g, p);
    for (int i = 0; i < g; i++)
    {
        fin >> x >> y;
        map.SetElement(i, x, y);
    }
    for (int i = 0; i < p; i++)
    {
        fin >> x >> y >> w >> h >> px >> py >> pw >> ph >> oX >> oY;
        SDL_Rect pathrect = {x, y, w, h};
        SDL_Rect pathroute = {px, py, pw, ph};
        map.SetElement(i, pathrect, pathroute, oX, oY);
    }
    SDL_Point s, e;
    fin >> x >> y;
    s = {x, y};
    fin >> x >> y;
    e = {x, y};
    map.SetElement(s, e);
    fin.close();
    fin.clear();
    return 1;
}

int Write()
{
    return 1;
}


#endif