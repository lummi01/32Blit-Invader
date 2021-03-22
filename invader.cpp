#include "invader.hpp"
#include "assets.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


using namespace blit;

Font font(font8x8);

struct SaveData 
{
    int score;
};

struct Player 
{
    int type;
    int ani;
    int x;
    int y;
    bool shot;
    int shot_x;
    int shot_y;
    int live;
    int score;
    int wave;
    int dT;
    float deadTimer;
};

struct Invader
{
    int x;
    int y;
    int dT;
    int ani;
    int left;
    int right;
    int bottom;
    int dx;
    int pic[11][5];
    int sum;
};

struct Alien_Shot
{
    int row[11];
    int x[11];
    int y[11];
};

struct Mother_Ship
{
    int x;
    int dx;
    int dt;
    int dT;
    int sT;
};

struct Explosion
{
    int x;
    int y;
    int type;
    int dT;
};

SaveData saveData;
Player p;
Invader inv;
Alien_Shot bomb;
Mother_Ship ufo;
Explosion ex;

bool shield;
int wall[4][6][3];
int state;
int dT;
int ani;
uint32_t lastTime = 0;
int localHighscore = 0;

void new_ufo()
{
    if (rand() % 2 > 0)
    {
        ufo.x = -16;
        ufo.dx = 1;
    }
    else 
    {
        ufo.x = 320;
        ufo.dx = -1;
    }
    ufo.dT = 0;
    ufo.sT = 500 + rand() % 1000;
}

void new_wave()
{
    if (p.wave < 7)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    wall[i][j][k] = 1;
                }
            }
            wall[i][2][2] = 0;
            wall[i][3][2] = 0;
        }
        shield = true;
    }
    else
    {
        shield = false;
        if (p.wave > 8)
        {
            p.wave = 8;
        }
    }

    inv.x = 0;
    inv.y = 32 + (p.wave * 12);
    inv.left = 0;
    inv.right = 10; //216;
    inv.bottom = 4; //60;
    inv.dx = 4;
    inv.ani = 0;
    inv.sum = 55;
    for (int i = 0; i < 11; i++)
    {
        inv.pic[i][0] = 10;
        inv.pic[i][1] = 6;
        inv.pic[i][2] = 6;
        inv.pic[i][3] = 2;
        inv.pic[i][4] = 2;

        bomb.row[i] = 5;
    }
}

void start_game() 
{
    state = 1;
    p.type = 0;
    p.ani = 0;
    p.live = 2;
    p.x = 300 - (p.live * 15);
    p.shot = false;
    p.deadTimer = 0;
    p.score = 0;
    p.wave = 0;

    new_wave();
    new_ufo();
}

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() 
{
    set_screen_mode(ScreenMode::hires);

    screen.sprites = Surface::load(asset_sprites);

    // Attempt to load the first save slot.
    if (read_save(saveData)) {
        // Loaded sucessfully!
        localHighscore = saveData.score;
    }
    else {
        // No save file or it failed to load, set up some defaults.
        saveData.score = 0;

    start_game();
    }
}


///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) 
{

    // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
    screen.clear();

    // draw some text at the top of the screen
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(255, 255, 255);

    if (state == 0) 
    {
        screen.text("Invader", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1 / 3), true, TextAlign::center_center);
        screen.text("Press A to Start", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3), true, TextAlign::center_center);
    }
    else if (state == 1 || state == 2) 
    {
        screen.text("Score: " + std::to_string(p.score), font, Point(6, 227));

        if (shield)
        {
            for (int i = 0; i < 4; i ++)
            {
                screen.sprite(Rect(0, 3, 3, 2), Point(50 + (i * 64), 172));
                for (int j = 0; j < 6; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        if (wall[i][j][k] > 1)
                        {
                            screen.sprite(34 + wall[i][j][k], Point(48 + (i * 64) + (j * 4), 171 + (k * 6)));
                        }                   
                    }
                }
            }
        }


        for (int i = 0; i < 11; i++)
        { 
            for (int j = 0; j < 5; j++)
            {
                if (inv.pic[i][j] > 0)
                {
                    screen.sprite(inv.pic[i][j] - inv.ani, Point(inv.x + (i * 20), inv.y + (j * 12)));
                    screen.sprite(inv.pic[i][j] - inv.ani + 1, Point(inv.x + (i * 20) + 8, inv.y + (j * 12)));
                }
            }
            if (bomb.y[i] > 0)
            {
                screen.sprite(32 + ani, Point(bomb.x[i], bomb.y[i]));
            }
        }

        if (ufo.dT > ufo.sT)
        {
            screen.sprite(51, Point(ufo.x, 8));
            screen.sprite(52, Point(ufo.x + 8, 8));
        }

        if (p.shot)
        {
            screen.pen = Pen(255, 255, 255);
            screen.line(Point(p.shot_x, p.shot_y), Point(p.shot_x, p.shot_y + 5));
        }

        if (ex.dT > 0)
        {
            ex.dT--;
            screen.sprite(ex.type, Point(ex.x, ex.y));
            screen.sprite(ex.type + 1, Point(ex.x + 8, ex.y));
        }
        
        screen.sprite(16 + p.ani, Point(p.x, 210));
        screen.sprite(17 + p.ani, Point(p.x + 8, 210));
        
        for (int i = 0; i < p.live; i++)
        {
            screen.sprite(16, Point(300 - (i * 15), 227));
            screen.sprite(17, Point(308 - (i * 15), 227));            
        }
        
        screen.pen = Pen(255, 0, 255);
        screen.line(Point(0, 220), Point(SCREEN_WIDTH, 220));

        screen.pen = Pen(0, 0, 0);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) 
{
    if (state == 0)
    {

    }
    else if (state == 1)
    {
        inv.dT++;
        if (inv.dT > 50)
        {
            inv.dT = 0;
            inv.x = 0;
            inv.dx--;
            if (inv.dx < 0)
            {
                inv.dx = 2;
                state = 2;
            }
        }
        else if (inv.dT == 25)
        {
            inv.x = 320;
        }
    }
    else if (state == 2)
    {
        inv.dT++;
        if (inv.dT > 1 + ((inv.sum) * .5))
        {
            inv.dT = 0;
            inv.ani += 2;
            if (inv.ani > 2)
            {
                inv.ani = 0;
            }

            if (p.type == 0)
            {
                inv.x += inv.dx;
                if (inv.x + (inv.left * 20) < 0 || inv.x + (inv.right * 20 +16) > SCREEN_WIDTH + 2)
                {
                    inv.dx = -inv.dx;
                    inv.x += inv.dx;
                    inv.y += 6;
                    if (inv.y + (inv.bottom * 12) > 202)
                    {
                        p.live = 0;
                        p.type = 1;
                    }
                    else if (inv.y + (inv.bottom * 12) > 162)
                    {
                        shield = false;
                    }
                }
            
                if (std::rand() % 9 == 0)
                {
                    int i = inv.left + std::rand() %(inv.right - inv.left + 1);
                    while (bomb.row[i] == 0)
                    {
                        i++;
                    }
                    if (bomb.y[i] == 0)
                    {
                        bomb.x[i] = inv.x + (i * 20) + 5;
                        bomb.y[i] = inv.y + (bomb.row[i] * 12) - 4;
                    }
                }
            }
        }

        if (p.shot)
        {
            p.shot_y -= 2; 
            if (p.shot_y < 0)
            {
                p.shot = false;
            }
            else if (shield && p.shot_y < 188 && p.shot_y > 172)
            {
                for (int i = 0; i < 4; i ++)
                {
                    if (p.shot_x < 74 + (i * 64) && p.shot_x > 50 + (i * 64))
                    {
                        int xx = (p.shot_x - (50 + (i * 64))) * .25;
                        int yy = (p.shot_y - 173) * .2;
                        if (wall[i][xx][yy] == 1)
                        {
                            wall[i][xx][yy] = 2 + rand() % 3;
                            p.shot = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                for (int i = 0; i < 11; i++)
                { 
                    for (int j = 0; j < 5; j++)
                    {
                        if (inv.pic[i][j] > 0)
                        {
                            if (p.shot_x < (i * 20 + 13 + inv.x) && p.shot_x > (i * 20 + inv.x) && p.shot_y < (j * 12 + 8 + inv.y) && p.shot_y +3 > (j * 12 + inv.y))
                            {
                                p.shot = false;
                                p.score += (inv.pic[i][j] + 2) * 2.5;
                                inv.pic[i][j] = 0;
    
                                ex.x = inv.x + (i * 20);
                                ex.y = inv.y + (j * 12);
                                ex.type = 39;
                                ex.dT = 10;
    
                                inv.sum--;
                                if (inv.sum == 0)
                                {
                                    p.wave++;
                                    new_wave();
                                    state = 1;
                                }
                                else
                                {
                                    while (inv.pic[i][bomb.row[i] - 1] == 0 && bomb.row[i] > 0)
                                    {
                                        bomb.row[i]--;
                                    }
                    
                                    for (int k = inv.left; k < inv.right + 1; k++)
                                    {
                                        int sum = 0;
                                        for (int l = 0; l < inv.bottom + 1; l++)
                                        {
                                            sum += inv.pic[k][l];
                                        }
                                        if (sum > 0)
                                        {
                                            inv.left = k;
                                            break;
                                        }
                                    }
                                    for (int k = inv.right; k > inv.left - 1; k--)
                                    {
                                        int sum = 0;
                                        for (int l = 0; l < 5; l++)
                                        {
                                            sum += inv.pic[k][l];
                                        }
                                        if (sum > 0)
                                        {
                                            inv.right = k;
                                            break;
                                        }
                                    }
                                    for (int k = inv.bottom; k > -1; k--)
                                    {
                                        int sum = 0;
                                        for (int l = inv.left; l < inv.right + 1; l++)
                                        {
                                            sum += inv.pic[l][k];
                                        }
                                        if (sum > 0)
                                        {
                                            inv.bottom = k;
                                            break;
                                        }
                                    }
    
                                    break;
                                }
                            }
                        }
                    }
                    if (p.shot == false)
                    {
                        break;
                    }
                }
            }
        }
    }
    
    ufo.dT++;
    if (ufo.dT > ufo.sT)
    {
        ufo.dt++;
        if (ufo.dt > 1)
        {
            ufo.dt = 0;
            ufo.x += ufo.dx;
            if (ufo.x < -16 || ufo.x > 320)
            {
                new_ufo();
            }
        }
        if (p.shot && p.shot_x < ufo.x + 16 && p.shot_x > ufo.x && p.shot_y < 16 && p.shot_y > 4)
        {
            ex.x = ufo.x;
            ex.y = 8;
            ex.type = 53;
            ex.dT = 25;

            new_ufo();
            p.shot = false;
            p.score += 100;
        }
    }


    for (int i = 0; i < 11; i++)
    { 
        if (bomb.y[i] > 0)
        {
            bomb.y[i]++;
            if (bomb.y[i] > 212)
            {   
                bomb.y[i] = 0;
            }
            else if (bomb.y[i] < 218 && bomb.y[i] > 204 && bomb.x[i] < p.x + 12 && bomb.x[i] + 3 > p.x)
            {
                bomb.y[i] = 0;
                p.type = 1;
            }
            else if (shield && bomb.y[i] < 181 && bomb.y[i] > 165)
            {
                for (int j = 0; j < 4; j ++)
                {
                    if (bomb.x[i] < 73 + (j * 64) && bomb.x[i] > 46 + (j * 64))
                    {
                        int xx = (bomb.x[i] - 48 - (j * 64)) * .25;
                        int yy = (bomb.y[i] - 166) * .2;
                        if (wall[j][xx][yy] == 1)
                        {
                            wall[j][xx][yy] = 2 + rand() % 3;
                            bomb.y[i] = 0;
                            break;
                        }
                    }
                }
            }
            else if (p.shot && p.shot_x < bomb.x[i] + 4 && p.shot_x > bomb.x[i] && p.shot_y < bomb.y[i] + 6 && p.shot_y + 6 > bomb.y[i])
            {
                ex.x = bomb.x[i] - 1;
                ex.y = bomb.y[i] - 1;
                ex.type = 41;
                ex.dT = 10;

                bomb.y[i] = 0;
                p.shot = false;
                p.score += 5;
            }
        }
    }

    ani++;
    if (ani > 3)
    {
        ani = 0;
    }

    if (p.type == 0)
    {
        if (p.shot == false && buttons & Button::A && state == 2)
        {
            p.shot = true;
            p.shot_x = p.x + 6;
            p.shot_y = 206;
        }
        if (buttons & Button::DPAD_RIGHT && p.x < SCREEN_WIDTH - 13)
        {
            p.x++;
        }
        if (buttons & Button::DPAD_LEFT && p.x > 0)
        {
            p.x--;
        }
    }
    else if (p.type == 1)
    {
        p.ani = 2 * (rand() % 3) + 2;
        p.dT++;
        if (p.dT > 150)
        {
            p.dT = 0;
            if (p.live == 0)
            {
                // Game Over
                start_game();
            }
            else
            {
                p.live--;
                p.x = 300 - (p.live * 15);
                p.type = 0;
                p.ani = 0;
            }
        }
    }
}

