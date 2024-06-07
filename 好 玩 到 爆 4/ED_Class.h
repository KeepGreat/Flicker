#ifndef ED_CLASS_H
#define ED_CLASS_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <vector>
#include <string>

#include "ED_const.h"

class Map;
class BaseTower;
class Turret;
class Enermy;
class Bullet;
class Text;
class Image;
class Page;

extern Map map;
extern std::vector<BaseTower*> towers;
extern std::vector<Enermy*> enermys;
extern std::vector<Text*> txts;
extern std::vector<Image*> imgs;
extern std::vector<Page*> pages;

class Map
{
private:
    enum ground_state {Vacant, Chosed, Occupied, Turret1, Turret2};
    typedef struct {
        SDL_Surface *surf;
        SDL_Texture *text;
        SDL_Rect rect;
        enum ground_state state;
    } Ground;
    typedef struct {
        SDL_Surface *surf;
        SDL_Texture *text;
        SDL_Rect rect;
        SDL_Rect route;
        int orientationX;
        int orientationY;
    } Path;
    int grounum;
    int pathnum;
    Ground **grou;
    Path **path;
    SDL_Point start;
    SDL_Point end;
public:
    friend class BaseTower;
    friend class Turret;
    friend class Enermy;
    Map(){}; //不在此初始化是因为参数不确定
    ~Map();
    void InitElement(int g, int p);
    void SetElement(int grouindex, int x, int y);
    void SetElement(int pathindex, SDL_Rect pathrect, SDL_Rect pathroute, int oX, int oY);
    void SetElement(SDL_Point s, SDL_Point e);
    void Draw();
    void DetectChosed(SDL_Event *event);
    int GetGrouNum(){return grounum;};
    void Clear();
};

void Map::InitElement(int g, int p)
{
    grounum = g;
    pathnum = p;
    grou = new Ground* [grounum];
    path = new Path* [pathnum];
    
    for (int i = 0; i < grounum; i++)
    {
        grou[i] = new Ground;
        grou[i]->surf = SDL_LoadBMP("./resource/ground.bmp");
        grou[i]->text = SDL_CreateTextureFromSurface(renderer, grou[i]->surf);
        grou[i]->state = Vacant;
    }
    for (int i = 0; i < pathnum; i++)
    {
        path[i] = new Path;
        path[i]->surf = SDL_LoadBMP("./resource/path.bmp");
        path[i]->text = SDL_CreateTextureFromSurface(renderer, path[i]->surf);
    }
}

void Map::SetElement(int grouindex, int x, int y)
{
    grou[grouindex]->rect.x = x;
    grou[grouindex]->rect.y = y;
    grou[grouindex]->rect.w = ENTITY_WIDTH;
    grou[grouindex]->rect.h = ENTITY_HEIGHT;
}

void Map::SetElement(int pathindex, SDL_Rect pathrect, SDL_Rect pathroute, int oX, int oY)
{
    path[pathindex]->rect = pathrect;
    path[pathindex]->route = pathroute;
    path[pathindex]->orientationX = oX;
    path[pathindex]->orientationY = oY;
}

void Map::SetElement(SDL_Point s, SDL_Point e)
{
    start = s;
    end = e;
}

Map::~Map()
{
    /*for (int i = 0; i < grounum; i++)
    {
        if (grou[i]->state == Vacant) //建立炮塔后图层和纹理由炮塔类管理
        {
            SDL_DestroyTexture(grou[i]->text);
            SDL_FreeSurface(grou[i]->surf);
        }
        delete grou[i];
    }
    for (int i = 0; i < pathnum; i++)
    {
        SDL_DestroyTexture(path[i]->text);
        SDL_FreeSurface(path[i]->surf);
        delete path[i];
    }*/
    delete [] grou;
    delete [] path;
}

void Map::Draw()
{
    for (int i = 0; i < grounum; i++)
    {
        if (grou[i]->state != Occupied)
        {
            SDL_RenderCopy(renderer, grou[i]->text, NULL, &grou[i]->rect);
        }
    }
    for (int i = 0; i < pathnum; i++)
    {
        SDL_RenderCopy(renderer, path[i]->text, NULL, &path[i]->rect);
    }
}

void Map::DetectChosed(SDL_Event *event)
{
    SDL_Point point = {event->button.x, event->button.y};
    for (int i = 0; i < grounum; i++)
    {
        if (SDL_PointInRect(&point, &grou[i]->rect))
        {
            if (grou[i]->state == Vacant)
            {
                grou[i]->state = Chosed;
                grou[i]->surf = SDL_LoadBMP("./resource/GroundChosed.bmp");
                grou[i]->text = SDL_CreateTextureFromSurface(renderer, grou[i]->surf);
                continue;
            }
            if (grou[i]->state == Chosed)
            {
                grou[i]->state = Vacant;
                grou[i]->surf = SDL_LoadBMP("./resource/ground.bmp");
                grou[i]->text = SDL_CreateTextureFromSurface(renderer, grou[i]->surf);
                continue;
            }
        }
    }
}

void Map::Clear()
{
    for (int i = 0; i < grounum; i++)
    {
        SDL_DestroyTexture(grou[i]->text);
        SDL_FreeSurface(grou[i]->surf);
        delete grou[i];
    }
    for (int i = 0; i < pathnum; i++)
    {
        SDL_DestroyTexture(path[i]->text);
        SDL_FreeSurface(path[i]->surf);
        delete path[i];
    }
}

class Enermy
{
private:
    SDL_Surface *surf;
    SDL_Texture *text;
    SDL_Rect rect;
    SDL_FRect healthbar;
    int health;
    int dehealth;
    int speed;
    enum state {Default, Frozen};
    enum state state;
public:
    Enermy(int h, int s);
    ~Enermy();
    void SetElement(int dh, int ds);
    void Summon(Map &map);
    void Operate(Map &map);
    void ReactToClick(SDL_Event *event);
    int ArriveEnd(Map &map);
    void Draw();
    int GetHealth(){return health;};
    SDL_Rect GetRect(){return rect;};
    void GetHurt(int d){health -= d;};
    void SetState(int i);
    friend void DetectCollide(Enermy *ener, Bullet &bull); //为啥修改私有成员就会报错“非静态成员引用必须与特定对象相对”呀QAQ --> friend的作用是能让你访问形参对象中的私有成员
};

Enermy::Enermy(int h, int s)
{
    surf = SDL_LoadBMP("./resource/enermy.bmp");
    text = SDL_CreateTextureFromSurface(renderer, surf);
    health = h;
    dehealth = h;
    speed = s;
    state = Default;
}

Enermy::~Enermy()
{
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surf);
}

void Enermy::SetElement(int dh, int ds)
{
    health += dh;
    speed += ds;
}

void Enermy::SetState(int i)
{
    if (i == 1)
    {
        state = Frozen;
    }
}

void Enermy::Summon(Map &map)
{
    rect = {map.start.x, map.start.y, ENTITY_WIDTH, ENTITY_HEIGHT};
}

void Enermy::Operate(Map &map)
{
    int index = -1;
    SDL_Point detect = {rect.x, rect.y};
    for (int i = 0; i < map.pathnum; i++)
    {
        if (SDL_PointInRect(&detect, &map.path[i]->route))
        {
            index = i;
            break;
        }
    }
    if (index > -1 && health > 0)
    {
        if (state == Default)
        {
            rect.x += speed * map.path[index]->orientationX;
            rect.y += speed * map.path[index]->orientationY;
        }
        else if (state == Frozen)
        {
            rect.x += speed * map.path[index]->orientationX * 0.5;
            rect.y += speed * map.path[index]->orientationY * 0.5;
        }
    }
    healthbar.x = (float) rect.x;
    healthbar.y = (float) rect.y - 20;
    healthbar.w = (float) 100 * (float(health) / float(dehealth));
    healthbar.h = (float) 20;
}

void Enermy::ReactToClick(SDL_Event *event)
{
    SDL_Point detect = {event->button.x, event->button.y};
    if (SDL_PointInRect(&detect, &rect) && health > 0)
    {
        health--;
        if (speed < 10)
        {
            speed++;    
        } //嘿嘿~
    }
}

int Enermy::ArriveEnd(Map &map)
{
    int flag = 0;
    if (SDL_PointInRect(&map.end, &rect))
    {
        flag = 1;
    }

    return flag;
}

void Enermy::Draw()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
    SDL_RenderFillRectF(renderer, &healthbar);
    SDL_RenderCopy(renderer, text, NULL, &rect);
    if (state == Frozen)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawColor(renderer, de_color.r, de_color.g, de_color.b, de_color.a);
}

class BaseTower //将自身纹理覆盖在Map类上
{
private:
    int cost;
    int realm;
    int damage;
    int grade;
protected:
    SDL_Surface *surf;
    SDL_Texture *text;
    SDL_Rect rect;
public:
    enum tower_state {Default, Chosed};
    enum tower_state state;
    BaseTower(const char * bmproute, int scost, int srealm, int sdamage);
    ~BaseTower();
    void SetElement(int dr, int dd, int dg, int dc = 0);
    void SetElement(SDL_Surface *s, SDL_Texture *t);
    int BuildTower(Map &map);
    SDL_Rect GetRect(){return rect;};
    int GetRealm(){return realm;};
    int GetGrade(){return grade;};
    int GetDamage(){return damage;};
    virtual void Draw() = 0;
    virtual void Operate() = 0;
    virtual void Upgrade() = 0;
    virtual void DetectChosed(SDL_Event *event) = 0;
};

BaseTower::BaseTower(const char * bmproute, int scost, int srealm, int sdamage)
{
    surf = SDL_LoadBMP(bmproute);
    text = SDL_CreateTextureFromSurface(renderer, surf);
    cost = scost;
    realm = srealm;
    damage = sdamage;
    state = Default;
    grade = 1;
}

BaseTower::~BaseTower()
{
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surf);
}

void BaseTower::SetElement(int dr, int dd, int dg, int dc)
{
    realm += dr;
    damage += dd;
    grade += dg;
    cost += dc;
}

void BaseTower::SetElement(SDL_Surface *s, SDL_Texture *t)
{
    surf = s;
    text = t;
}

int BaseTower::BuildTower(Map &map)
{   
    int flag = 0;
    for (int i = 0; i < map.grounum; i++)
    {
        if (map.grou[i]->state == Map::Chosed)
        {
            rect = map.grou[i]->rect;
            map.grou[i]->state = Map::Occupied;
            flag = 1;
            break;
        }
    }

    return flag;
}

class Bullet
{
private:
    SDL_Surface *surf;
    SDL_Texture *text;
    SDL_Rect rect;
    SDL_Rect target;
    int damage;
    int speed;
    enum bull_state {Default, Valid, Invalid};
    enum bull_state state;
public:
    friend class Turret;
    Bullet();
    ~Bullet();
    void SetRect(int rx, int ry, int tx, int ty);
    void Operate();
    void Draw();
    void Eliminate();
    friend void DetectCollide(Enermy *ener, Bullet &bull); //为啥修改私有成员就会报错“非静态成员引用必须与特定对象相对”呀QAQ --> friend的作用是能让你访问形参对象中的私有成员
};

Bullet::Bullet()
{
    surf = SDL_LoadBMP("./resource/Bullet.bmp");
    text = SDL_CreateTextureFromSurface(renderer, surf);
    rect.w = BULLET_WIDTH;
    rect.h = BULLET_HEIGHT;
    damage = BULLET_DAMAGE;
    speed = BULLET_SPEED;
    state = Default;
}

Bullet::~Bullet()
{
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surf);
}

void Bullet::Operate()
{
    if (state == Valid)
    {
        if (target.x < rect.x)
        {
            rect.x -= speed;
        }
        if (target.x > rect.x)
        {
            rect.x += speed;
        }
        if (target.y < rect.y)
        {
            rect.y -= speed;
        }
        if (target.y > rect.y)
        {
            rect.y += speed;
        }
        if (rect.x >= target.x - 20 && rect.x <= target.x + 20 && rect.y >= target.y - 20 && rect.y <= target.y + 20)
        {
            state = Invalid;
        }
    }
    
    std::vector<Enermy*>::iterator ehead = enermys.begin();
    std::vector<Enermy*>::iterator etail = enermys.end();
    for (; ehead != etail; ehead++)
    {
        DetectCollide((*ehead), (*this));
    }
}

void Bullet::SetRect(int rx, int ry, int tx, int ty)
{
    rect.x = rx;
    rect.y = ry;
    target.x = tx;
    target.y = ty;
}

void Bullet::Draw()
{
    if (state == Valid)
    {
        SDL_RenderCopy(renderer, text, NULL, &rect);
    }
}

void Bullet::Eliminate()
{
    if (state == Invalid)
    {
        rect.x = WINDOW_WIDTH;
        rect.y = WINDOW_HEIGHT;
    }
}

class Turret : public BaseTower
{
private:
    Bullet bull; //必须把类声明前置否则会报错"incomplete type"
public:
    friend class Enermy; //为了适应DetectCollide()函数
    Turret(const char * br, int c, int re, int d) : BaseTower(br, c, re, d){};
    ~Turret(){};
    virtual void Draw();
    virtual void Operate();
    virtual void Upgrade();
    virtual void DetectChosed(SDL_Event *event);
};

void Turret::Operate() //记得时刻重置子弹目标坐标
{   
    long int distance;
    if (bull.state != Bullet::Valid)
    {
        std::vector<Enermy*>::iterator ehead = enermys.begin();
        std::vector<Enermy*>::iterator etail = enermys.end();
        for (; ehead != etail; ehead++)
        {
            distance =((*ehead)->GetRect().x - GetRect().x) * ((*ehead)->GetRect().x - GetRect().x) + ((*ehead)->GetRect().y - GetRect().y) * ((*ehead)->GetRect().y - GetRect().y);
            if (distance <= GetRealm() * GetRealm() && (*ehead)->GetHealth() > 0)
            {
                bull.SetRect(GetRect().x + ENTITY_WIDTH / 2, GetRect().y + ENTITY_HEIGHT / 2, (*ehead)->GetRect().x + ENTITY_WIDTH / 2, (*ehead)->GetRect().y + ENTITY_HEIGHT / 2);
                bull.state = Bullet::Valid;
                break;
            }
        }
    }
    bull.Operate();
    bull.Draw();
    bull.Eliminate();
}

void Turret::Draw()
{
    SDL_RenderCopy(renderer, text, NULL, &rect);
    bull.Draw();
}

void Turret::DetectChosed(SDL_Event *event)
{
    SDL_Point detect = {event->button.x, event->button.y};
    if (SDL_PointInRect(&detect, &rect))
    {
        if (GetGrade() == 1)
        {
            if (state == Default)
            {
                surf = SDL_LoadBMP("./resource/TurretChosed.bmp");
                text = SDL_CreateTextureFromSurface(renderer, surf);
                state = Chosed;
                return;
            }
            if (state == Chosed)
            {
                surf = SDL_LoadBMP("./resource/Turret.bmp");
                text = SDL_CreateTextureFromSurface(renderer, surf);
                state = Default;
                return;
            }
        }
    }
}

void Turret::Upgrade()
{
    if (state == Chosed && GetGrade() == 1 && Coin >= 100)
    {
        SetElement(200, 1, 1); //调范围和等级
        bull.damage += 1; //调伤害
        surf = SDL_LoadBMP("./resource/Turret2.bmp");
        text = SDL_CreateTextureFromSurface(renderer, surf);
        Coin -= 100;
    }
}

void DetectCollide(Enermy *ener, Bullet &bull)
{
    if (SDL_HasIntersection(&ener->rect, &bull.rect))
    {
        ener->health -= bull.damage;
        bull.state = Bullet::Invalid;
    }
}

class Frost : public BaseTower
{
private:
    SDL_Rect Ice;
    clock_t tick;
    clock_t timer;
    int elapse;
public:
    Frost(const char *br, int c, int re, int d) : BaseTower(br, c, re, d){
        elapse = 1 * CLOCKS_PER_SEC;
        timer = clock();
    };
    ~Frost(){};
    virtual void Draw();
    virtual void Operate();
    virtual void Upgrade();
    virtual void DetectChosed(SDL_Event *event);
};

void Frost::Operate()
{
    long int distance;
    tick = clock();
    if (tick - timer >= elapse)
    {
        for (int i = 0; i < enermys.size(); i++)
        {
            distance = (enermys[i]->GetRect().x - rect.x) * (enermys[i]->GetRect().x - rect.x) + (enermys[i]->GetRect().y - rect.y) * (enermys[i]->GetRect().y - rect.y);
            if (distance <= GetRealm() * GetRealm() && enermys[i]->GetHealth() > 0)
            {
                enermys[i]->SetState(1);
                enermys[i]->GetHurt(GetDamage());
            }
        }
        timer = tick;
    }

}

void Frost::Draw()
{
    SDL_RenderCopy(renderer, text, NULL, &rect);
}

void Frost::Upgrade()
{
    if (state == Chosed && GetGrade() == 1 && Coin >= 50)
    {
        SetElement(200, 1, 1);
        surf = SDL_LoadBMP("./resource/Frost2.bmp");
        text = SDL_CreateTextureFromSurface(renderer, surf);
        Coin -= 50;
    }
}

void Frost::DetectChosed(SDL_Event *event)
{
    SDL_Point detect = {event->button.x, event->button.y};
    if (SDL_PointInRect(&detect, &rect))
    {
        if (GetGrade() == 1)
        {
            if (state == Default)
            {
                surf = SDL_LoadBMP("./resource/FrostChosed.bmp");
                text = SDL_CreateTextureFromSurface(renderer, surf);
                state = Chosed;
                return;
            }
            if (state == Chosed)
            {
                surf = SDL_LoadBMP("./resource/Frost.bmp");
                text = SDL_CreateTextureFromSurface(renderer, surf);
                state = Default;
                return;
            }
        }
    }
}

class Text
{
private:
    SDL_Surface *surf;
    SDL_Texture *text;
    SDL_Rect rect;
    std::string txt;
    std::string temp;
    SDL_Color color;
public:
    friend class Page;
    Text(){};
    ~Text();
    void SetElement(const char *cons, int x, int y);
    void Operate(int v); //字幕变化
    void Draw();
};

Text::~Text()
{
    txt.clear();
    temp.clear();
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surf);
}

void Text::SetElement(const char *cons, int x, int y)
{
    txt = cons;
    color = {0, 0, 0, 255};
    surf = TTF_RenderUTF8_Blended(font, txt.c_str(), color);
    text = SDL_CreateTextureFromSurface(renderer, surf);
    rect.x = x;
    rect.y = y;
    rect.w = surf->w;
    rect.h = surf->h;
}

void Text::Operate(int v)
{
    temp = txt + std::to_string(v);
    surf = TTF_RenderUTF8_Blended(font, temp.c_str(), color);
    text = SDL_CreateTextureFromSurface(renderer, surf);
    rect.w = surf->w;
    rect.h = surf->h;
}

void Text::Draw()
{
    SDL_RenderCopy(renderer, text, NULL, &rect);
}

class Image
{
private:
    SDL_Surface *surf;
    SDL_Texture *text;
    SDL_Rect rect;
    SDL_Color color;
    int value; //默认为-1,存储pageindex
public:
    friend class Page;
    Image(const char *s, SDL_Rect r, int flag, int v = -1);
    Image() = default;
    ~Image();
    void SetElement(const char *s, SDL_Rect r, int flag, int v = -1);
    void SetElement(SDL_Color c){color = c;};
    void SetElement(int v){value = v;};
    void Draw();
    int GetValue(){return value;};
};

/** \param flag include 4 varieties of photo, use the initiation of IMG, 
 *              3 refers to .BMP format and 4 refers to .GIF format
 */
Image::Image(const char *s, SDL_Rect r, int flag, int v)
{
    if (flag == IMG_INIT_JPG)
    {
        surf = IMG_LoadJPG_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == IMG_INIT_PNG)
    {
        surf = IMG_LoadPNG_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == 3)
    {
        surf = SDL_LoadBMP(s);
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == 4)
    {
        surf = IMG_LoadGIF_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    value = v;
    rect = r;
}

Image::~Image() //存在surf和text未被初始化的情况
{
    if (text != NULL)
    {
        SDL_DestroyTexture(text);
    }
    if (surf != NULL)
    {
        SDL_FreeSurface(surf);
    }
}

/** \param flag include 4 varieties of photo, use the initiation of IMG, 
 *              3 refers to .BMP format and 4 refers to .GIF format
 */
void Image::SetElement(const char *s, SDL_Rect r, int flag, int v)
{
    if (flag == IMG_INIT_JPG)
    {
        surf = IMG_LoadJPG_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == IMG_INIT_PNG)
    {
        surf = IMG_LoadPNG_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == 3)
    {
        surf = SDL_LoadBMP(s);
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    if (flag == 4)
    {
        surf = IMG_LoadGIF_RW(SDL_RWFromFile(s, "rb"));
        text = SDL_CreateTextureFromSurface(renderer, surf);
    }
    value = v;
    rect = r;
}

void Image::Draw()
{
    SDL_RenderCopy(renderer, text, NULL, &rect);
}

class Page
{
private:
    std::vector<Text*> txts;
    std::vector<Image*> imgs;
    std::vector<Image*> buttons;
    int index;
public:
    Page(int i) : index(i){};
    ~Page();
    void AddElement(Text *txt);
    void AddElement(Image *img);
    void AddButton(Image *but);
    int SwitchTo(SDL_Event *event);
    void Draw();
    void Clear();
};

Page::~Page()
{
    for (int i = 0; i < txts.size(); i++)
    {
        delete txts[i];
    }
    txts.clear();

    for (int i = 0; i < imgs.size(); i++)
    {
        delete imgs[i];
    }
    imgs.clear();

    for (int i = 0; i < buttons.size(); i++)
    {
        delete buttons[i];
    }
    buttons.clear();
}

void Page::AddElement(Text *txt)
{
    txts.push_back(txt);
}

void Page::AddElement(Image *img)
{
    imgs.push_back(img);
}

void Page::AddButton(Image *but)
{
    buttons.push_back(but);
}

int Page::SwitchTo(SDL_Event *event)
{
    int flag = index;
    SDL_Point detect = {event->button.x, event->button.y};
    for (int i = 0; i < buttons.size(); i++)
    {
        if (SDL_PointInRect(&detect, &buttons[i]->rect))
        {
            flag = buttons[i]->value;
            break;
        }
    }
    return flag;
}

void Page::Draw()
{
    for (int i = 0; i < txts.size(); i++)
    {
        txts[i]->Draw();
    }
    for (int i = 0; i < imgs.size(); i++)
    {
        imgs[i]->Draw();
    }
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->Draw();
    }
}

void Page::Clear()
{
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

#endif