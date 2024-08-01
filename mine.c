#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#undef main
#define map_col 20
#define bricks 34 // 每一个地雷块的大小
#define cheat_bricks 15
#define Map_position_x 420
#define Map_position_y 250
#define Map_position__cheat_x 1307
#define Map_position_cheat_y 700
#define mouse_generate_speed 200
#define boomb_num 40
#define heart_num 3
#define mouse_jump_frequency 7000
#define max_mouse_jump_frequency 3000
#define mouse_killed_standard 120
SDL_Window *win = NULL;
SDL_Renderer *render1 = NULL;
SDL_Texture *map_texture = NULL;
const int mouseleftbutton[] = {464, 240, 16, 16};
const int mouserightbutton[] = {464 + 16, 240, 16, 16};
const int spade[] = {672, 80, 16, 16};   // 左键
const int hammer[] = {592, 112, 16, 16}; // 右键
const int stop[] = {592, 336, 16, 16};
const int king[] = {448, 48, 16, 16};
const int normal_bricks[] = {400, 240, 16, 16};
const int heart[] = {672, 160, 16, 16};
const int Empty_heart[] = {640, 160, 16, 16};
const int timer[] = {656, 192, 16, 16};
const int heart_marker[] = {624, 160, 16, 16};
const int drug[] = {656, 176, 16, 16};
const int boomb[] = {720, 144, 16, 16};
const int mouse[] = {368, 128, 16, 16};
const int danger_mouse[] = {336, 126, 16, 16};
const int book[] = {544, 240, 16, 16};
const int blank[] = {576, 192, 16, 16};
const int number0[4] = {560, 272, 16, 16};
const int danger_sign[] = {224, 160, 16, 16};
const int warning[] = {592, 320, 16, 16}; // 592, 320
const int score_mark[] = {640, 256, 16, 16};
const int restart[] = {352, 320, 16, 16};
const int record_mark[] = {688, 32, 16, 16};
typedef struct logic_map
{
    int status;        // 0 未显露，1 显露
    int boomb;         // 0 无炸弹 1，有炸弹
    int mouse;         // 0 没有 1 有老鼠
    int mark;          // 0 没有空白标记 1 有标记
    int number_bricks; //-1 没有数字 其他就是这个格子的数字
    int booommm;       // 0 没爆炸 1 爆炸了
} logic_map;
int mouse_instinction = 0; // 老鼠的存活状态
int boomb_num_now = boomb_num;
int heart_num_now = heart_num;
int score = 0;
int max_score = 0;
int cheat_open = 0;
logic_map LOGIC_MAP[map_col][map_col];
SDL_Color font_color2 = {255, 204, 0};
SDL_Color background_main = {77, 62, 77, 255};
SDL_Rect map_position_rect = {Map_position_x, Map_position_y, map_col *bricks, bricks *map_col};
int mouse_kill = 0, mouse_jump_x = 0, mouse_jump_y = 0;
int judge_coodinationx[] = {0, 1, 1, 1};
int judge_coodinationy[] = {-1, -1, 0, 1};
int judge_coodinationx2[] = {0, 1, 1, 2, 2, 2, 2, 2};
int judge_coodinationy2[] = {-2, -2, 2, -2, -1, 0, 1, 2};

void init_logic_map()
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < map_col; i++)
    {
        for (int j = 0; j < map_col; j++)
        {

            LOGIC_MAP[i][j].status = 0;
            LOGIC_MAP[i][j].boomb = 0;
            LOGIC_MAP[i][j].mouse = 0;
            LOGIC_MAP[i][j].mark = 0;
            LOGIC_MAP[i][j].number_bricks = -1;
            LOGIC_MAP[i][j].booommm = 0;
        }
    }
    for (int i = 0; i < boomb_num;)
    {
        int x1 = rand() % map_col, y1 = rand() % map_col;
        if (LOGIC_MAP[x1][y1].boomb == 0)
        {
            LOGIC_MAP[x1][y1].boomb = 1;
            i++;
        }
    }
}
SDL_Rect *select_rect(const int pixels[])
{
    SDL_Rect *srcrect = malloc(sizeof(SDL_Rect));
    srcrect->x = pixels[0];
    srcrect->y = pixels[1];
    srcrect->w = pixels[2];
    srcrect->h = pixels[3];
    return srcrect;
}
void draw_smallpixel(int x, int y, const int pixels[]) // 画每个小的像素,不带renderprsent
{
    SDL_Rect *map_rect = malloc(sizeof(SDL_Rect));
    map_rect->x = x;
    map_rect->y = y;
    map_rect->w = bricks;
    map_rect->h = bricks;
    SDL_Rect *srcrect = select_rect(pixels);
    SDL_RenderCopy(render1, map_texture, srcrect, map_rect);
    free(srcrect);
    free(map_rect);
}
void draw_smallpixel_F(int x, int y, const int pixels[]) // 画每个小的像素,不带renderprsent
{
    SDL_Rect *map_rect = malloc(sizeof(SDL_Rect));
    map_rect->x = x;
    map_rect->y = y;
    map_rect->w = cheat_bricks;
    map_rect->h = cheat_bricks;
    SDL_Rect *srcrect = select_rect(pixels);
    SDL_RenderCopy(render1, map_texture, srcrect, map_rect);
    free(srcrect);
    free(map_rect);
}
void init_map_pixels() // 初始化小的像素
{

    for (int i = 0; i < map_col * bricks; i += bricks)
    {
        for (int j = 0; j < map_col * bricks; j += bricks)
        {
            draw_smallpixel(Map_position_x + j, Map_position_y + i, normal_bricks);
        }
    }
    for (int i = 0; i < 75 * 3; i += 75)
    {
        draw_smallpixel(80 + i, 20, heart);
    }

    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    draw_smallpixel(10, 20, heart_marker);
    draw_smallpixel(10, 20 + 60, timer);
    draw_smallpixel(10, 20 + 60 * 2, drug);
    draw_smallpixel(10, 20 + 60 * 3, boomb);
    draw_smallpixel(10, 20 + 60 * 4, mouse);
    draw_smallpixel(10, 20 + 60 * 5, score_mark);
    draw_smallpixel(10, 20 + 60 * 6, record_mark);
    draw_smallpixel(displayMode.w - 300, 205, king);
    draw_smallpixel(displayMode.w - 300, 305, stop);
    draw_smallpixel(displayMode.w - 300, 405, book);
    draw_smallpixel(displayMode.w - 300, 505, restart);
    draw_smallpixel(60, 505, spade);
    draw_smallpixel(60, 605, hammer);
    draw_smallpixel(10, 505, mouseleftbutton);
    draw_smallpixel(10, 605, mouserightbutton);
}
void draw_map_init() // 初始化地图
{
    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderClear(render1); // 地图的背景颜色

    init_map_pixels();
    SDL_RenderPresent(render1);
}
int scan_rect(int x, int y, SDL_Rect *rect1) // 检查是不是在矩形里面
{
    int rect_x = rect1->x, rect_x1 = rect_x + rect1->w;
    int rect_y = rect1->y, rect_y1 = rect_y + rect1->h;
    if (x >= rect_x && x <= rect_x1 && y >= rect_y && y <= rect_y1)
    {
        return 1;
    }
    return 0;
}
SDL_Rect *write_word(char words[], SDL_Color font_color, TTF_Font *title_font, int x, int y) // 写文字
{
    SDL_Surface *font_surface = TTF_RenderText_Blended(title_font, words, font_color);
    SDL_Texture *font_Texture = SDL_CreateTextureFromSurface(render1, font_surface);
    SDL_Rect *text_rect = malloc(sizeof(SDL_Rect));
    text_rect->x = x;
    text_rect->y = y;
    text_rect->w = font_surface->w;
    text_rect->h = font_surface->h;
    SDL_RenderCopy(render1, font_Texture, NULL, text_rect);
    SDL_RenderPresent(render1);
    SDL_FreeSurface(font_surface);
    SDL_DestroyTexture(font_Texture);
    return text_rect;
}
SDL_Rect *write_word_F(char words[], SDL_Color font_color, TTF_Font *title_font, int x, int y) // 写文字不会带上pesent(书写多段文字)
{
    SDL_Surface *font_surface = TTF_RenderText_Blended(title_font, words, font_color);
    SDL_Texture *font_Texture = SDL_CreateTextureFromSurface(render1, font_surface);
    SDL_Rect *text_rect = malloc(sizeof(SDL_Rect));
    text_rect->x = x;
    text_rect->y = y;
    text_rect->w = font_surface->w;
    text_rect->h = font_surface->h;
    SDL_RenderCopy(render1, font_Texture, NULL, text_rect);
    SDL_FreeSurface(font_surface);
    SDL_DestroyTexture(font_Texture);
    return text_rect;
}
void write_word_Free(char words[], SDL_Color font_color, TTF_Font *title_font, int x, int y) // 写文字不会带上pesent(书写多段文字)
{
    SDL_Surface *font_surface = TTF_RenderText_Blended(title_font, words, font_color);
    SDL_Texture *font_Texture = SDL_CreateTextureFromSurface(render1, font_surface);
    SDL_Rect *text_rect = malloc(sizeof(SDL_Rect));
    text_rect->x = x;
    text_rect->y = y;
    text_rect->w = font_surface->w;
    text_rect->h = font_surface->h;
    SDL_RenderCopy(render1, font_Texture, NULL, text_rect);
    SDL_FreeSurface(font_surface);
    SDL_DestroyTexture(font_Texture);
    free(text_rect);
}
void init_menu(TTF_Font *title_font) // 初始化菜单
{
    SDL_Surface *background_surface = IMG_Load("image/4.png");
    SDL_Texture *menu_background = SDL_CreateTextureFromSurface(render1, background_surface);
    SDL_RenderCopy(render1, menu_background, NULL, NULL);
    SDL_RenderPresent(render1);
    if (title_font == NULL)
    {
        SDL_Log("open font file failed\n");
    }
    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    SDL_Color font_color = {255, 255, 255};
    SDL_Color font_color2 = {255, 204, 0};
    TTF_SetFontSize(title_font, 175);
    write_word_Free("MINESWEEPER", font_color2, title_font, displayMode.w / 12, displayMode.h / 13);
    TTF_SetFontSize(title_font, 100);
    write_word_Free("START", font_color2, title_font, displayMode.w / 7, displayMode.h / 3 + 100);
    write_word_Free("TIPS", font_color2, title_font, displayMode.w / 2 + 270, displayMode.h / 3 + 100);
    write_word_Free("INTRO", font_color2, title_font, displayMode.w / 2 + 270, displayMode.h / 2 + 270);
    SDL_RenderPresent(render1);
    SDL_FreeSurface(background_surface);
    SDL_DestroyTexture(menu_background);
}
void generate_mouse()
{
    srand((unsigned)time(NULL));
    mouse_jump_x = rand() % map_col;
    mouse_jump_y = rand() % map_col;
    printf("%d %d\n", mouse_jump_x, mouse_jump_y);
}
void draw_mouse()
{
    SDL_Rect blank_rect = {mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, bricks, bricks};
    LOGIC_MAP[mouse_jump_x][mouse_jump_y].mouse = 1;
    SDL_RenderFillRect(render1, &blank_rect);
    draw_smallpixel(mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, mouse);
}
void clear_mouse()
{
    LOGIC_MAP[mouse_jump_x][mouse_jump_y].mouse = 0;

    SDL_Rect blank_rect = {mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, bricks, bricks};
    SDL_RenderFillRect(render1, &blank_rect);
    if (LOGIC_MAP[mouse_jump_x][mouse_jump_y].status == 0)
    {
        if (LOGIC_MAP[mouse_jump_x][mouse_jump_y].number_bricks == -1)
        {
            draw_smallpixel(mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, normal_bricks);
        }
        else
        {
            int numbers = LOGIC_MAP[mouse_jump_x][mouse_jump_y].number_bricks;
            const int number_now[] = {number0[0] + numbers * 16, number0[1], 16, 16};
            draw_smallpixel(mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, number_now);
        }
    }
    else
    {
        if (LOGIC_MAP[mouse_jump_x][mouse_jump_y].boomb == 0)
        {
            draw_smallpixel(mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, blank);
        }
        else
        {
            draw_smallpixel(mouse_jump_x * bricks + Map_position_x, mouse_jump_y * bricks + Map_position_y, danger_sign);
        }
    }
}
void PRINT(int start, TTF_Font *title_font)
{
    TTF_SetFontSize(title_font, 30);
    char record_array[20000];
    itoa(max_score, record_array, 10);
    SDL_Rect *record_rect = write_word_F(record_array, font_color2, title_font, 80, 20 + 60 * 6);
    char score_array[20000];
    itoa(score, score_array, 10);
    SDL_Rect *score_rect = write_word_F(score_array, font_color2, title_font, 80, 20 + 60 * 5);
    char boom[20000];
    itoa(boomb_num_now, boom, 10);
    SDL_Rect *boom_rect = write_word_F(boom, font_color2, title_font, 80, 20 + 60 * 3);
    int time_now = SDL_GetTicks64();
    int time_passed = (time_now - start) / 1000;
    char mouse_killed[20000];
    itoa(mouse_kill, mouse_killed, 10);
    SDL_Rect *mouse_kill_rect = write_word_F(mouse_killed, font_color2, title_font, 80, 20 + 60 * 4);
    char time[20000];
    itoa(time_passed, time, 10);
    SDL_Rect *time_rect = write_word(time, font_color2, title_font, 80, 20 + 60);

    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderFillRect(render1, mouse_kill_rect);
    SDL_RenderFillRect(render1, time_rect);
    SDL_RenderFillRect(render1, record_rect);
    SDL_RenderFillRect(render1, boom_rect);
    SDL_RenderFillRect(render1, score_rect);
    free(mouse_kill_rect);
    free(time_rect);
    free(record_rect);
    free(boom_rect);
    free(score_rect);
}
void PRINT_MAP()
{
    for (int yy = 0; yy < map_col; yy++)
    {
        for (int xx = 0; xx < map_col; xx++)
        {
            if (LOGIC_MAP[xx][yy].boomb == 1)
            {
                printf("%d ", 1);
            }
            else
            {
                printf("%d ", 0);
            }
        }
        printf("\n");
    }
}
SDL_Rect *Get_font_rect(int x, int y, char words[], SDL_Color font_color, TTF_Font *title_font)
{
    SDL_Surface *font_surface = TTF_RenderText_Blended(title_font, words, font_color);
    SDL_Rect *text_rect = malloc(sizeof(SDL_Rect));
    text_rect->x = x;
    text_rect->y = y;
    text_rect->w = font_surface->w;
    text_rect->h = font_surface->h;
    SDL_FreeSurface(font_surface);
    return text_rect;
}
void PRINT_MESSAGE(int start, TTF_Font *title_font, int *flag1, int *time1, int *flag2, int *time2)
{
    TTF_SetFontSize(title_font, 30);
    int time_now = SDL_GetTicks64();
    int time_passed = (time_now - start) / 1000;

    if (boomb_num_now == 10 && time_passed <= 900 && *flag1 == 0)
    {
        *flag1 = 1;
        *time1 = SDL_GetTicks64();
        write_word_Free("Bomb Disposal Expert!!!   +20", font_color2, title_font, 600, 100);
        score += 20;
        return;
    }
    else if (time_now - *time1 >= 4000 && *flag1 == 1)
    {
        SDL_Rect *boomb_rect1 = Get_font_rect(600, 100, "Bomb Disposal Expert!!!   +20", font_color2, title_font);
        SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
        SDL_RenderFillRect(render1, boomb_rect1);
        free(boomb_rect1);
    }

    if (mouse_kill == 200 && *flag2 == 0)
    {
        *flag2 = 1;
        *time2 = SDL_GetTicks64();
        write_word_Free("MOUSE KILLER   +10", font_color2, title_font, 600, 150);
        score += 10;
        return;
    }
    else if (time_now - *time2 >= 4000 && *flag2 == 1)
    {
        SDL_Rect *mouse_rect1 = Get_font_rect(600, 150, "MOUSE KILLER   +10", font_color2, title_font);
        SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
        SDL_RenderFillRect(render1, mouse_rect1);
        free(mouse_rect1);
    }
}
void PRINT_MAP_cheat()
{
    cheat_open = 1;
    for (int i = 0, y1 = 0; i < map_col * cheat_bricks; i += cheat_bricks, y1++)
    {
        for (int j = 0, x1 = 0; j < map_col * cheat_bricks; j += cheat_bricks, x1++)
        {

            if (LOGIC_MAP[x1][y1].boomb != 1)
            {
                draw_smallpixel_F(Map_position__cheat_x + j, Map_position_cheat_y + i, normal_bricks);
            }
            else if (LOGIC_MAP[x1][y1].boomb == 1)
            {
                draw_smallpixel_F(Map_position__cheat_x + j, Map_position_cheat_y + i, boomb);
            }
        }
    }
}
void PRINT_CLEAR()
{
    cheat_open = 0;
    SDL_Rect rect9 = {Map_position__cheat_x, Map_position_cheat_y, map_col * cheat_bricks, map_col * cheat_bricks};
    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderFillRect(render1, &rect9);
}

int valid(int x, int y) // xy是logicmap的坐标
{
    if (x >= map_col || x < 0 || y >= map_col || y < 0)
    {
        return -1;
    }
    return 1;
}
int scan_surrounding(int x, int y) // 搜索这附近的炸弹个数
{
    int num = 0;
    for (int i = 0; i < 4; i++) // 代表的是相对的坐标
    {
        if (valid(x + judge_coodinationx[i], y + judge_coodinationy[i]) > 0 && LOGIC_MAP[x + judge_coodinationx[i]][y + judge_coodinationy[i]].boomb == 1)
        {
            num++;
        }
        if (valid(x - judge_coodinationx[i], y - judge_coodinationy[i]) > 0 && LOGIC_MAP[x - judge_coodinationx[i]][y - judge_coodinationy[i]].boomb == 1)
        {
            num++;
        }
    }

    return num;
}
int scan_surrounding_status(int x, int y)
{
    int num = 0;
    for (int i = 0; i < 4; i++)
    {
        int newx1 = mouse_jump_x + judge_coodinationx[i], newy1 = mouse_jump_y + judge_coodinationy[i];

        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0 && LOGIC_MAP[newx1][newy1].boomb == 1)
        {
            num++;
        }
        newx1 = mouse_jump_x - judge_coodinationx[i], newy1 = mouse_jump_y - judge_coodinationy[i];

        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0 && LOGIC_MAP[newx1][newy1].boomb == 1)
        {
            num++;
        }
    }
    return num;
}
void release(int x, int y, const int command[]) // 方块打开后
{
    SDL_Rect rect0 = {x, y, bricks, bricks};
    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderFillRect(render1, &rect0);
    draw_smallpixel(x, y, command);
    SDL_RenderPresent(render1);
}
void release_F(int x, int y, const int command[]) // 方块打开后,没有present
{
    SDL_Rect rect0 = {x, y, bricks, bricks};
    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderFillRect(render1, &rect0);
    draw_smallpixel(x, y, command);
}
void release_R(int x, int y, const int command[])
{
    SDL_Rect rect0 = {x, y, bricks, bricks};
    SDL_SetRenderDrawColor(render1, 56, 44, 99, 255);
    SDL_RenderFillRect(render1, &rect0);
    draw_smallpixel(x, y, command);
}
void release_number(int x, int y) // 打开这个位置的数字个数
{
    if (LOGIC_MAP[x][y].status == 1)
    {
        return;
    }
    int numbers = scan_surrounding(x, y);
    int x0 = x * bricks + Map_position_x, y0 = y * bricks + Map_position_y;
    if (x < 0 || x >= map_col || y < 0 || y >= map_col)
    {
        return;
    }
    LOGIC_MAP[x][y].number_bricks = numbers;
    const int number_now[] = {number0[0] + numbers * 16, number0[1], 16, 16};

    if (LOGIC_MAP[x][y].status == 0 && LOGIC_MAP[x][y].mark == 1)
    {
        release_R(x0, y0, number_now);
        draw_smallpixel(x0, y0, warning);
        return;
    }

    release_F(x0, y0, number_now);
}
void release_surrounding(int x, int y) // 打开这位置周围的数字
{

    for (int i = 0; i < 4; i++)
    {
        int newx = x + judge_coodinationx[i], newy = y + judge_coodinationy[i];
        release_number(newx, newy);
        newx = x - judge_coodinationx[i];
        newy = y - judge_coodinationy[i];
        release_number(newx, newy);
    }
}
void minus_heart() // 减少心，未刷新
{
    SDL_Rect rect_heart = {80 + heart_num_now * 75, 20, bricks, bricks};
    SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
    SDL_RenderFillRect(render1, &rect_heart);
    draw_smallpixel(80 + heart_num_now * 75, 20, Empty_heart);
}
void scan_spade_down(int x, int y) // 未刷新
{
    int x0 = ((x - Map_position_x) / bricks);
    int y0 = ((y - Map_position_y) / bricks);
    if (LOGIC_MAP[x0][y0].mouse == 1)
    {
        return;
    }

    if (LOGIC_MAP[x0][y0].boomb == 1 && LOGIC_MAP[x0][y0].status == 0 && LOGIC_MAP[x0][y0].booommm == 0)
    {
        score += 4;
        LOGIC_MAP[x0][y0].status = 1;
        LOGIC_MAP[x0][y0].number_bricks = -1;
        LOGIC_MAP[x0][y0].booommm = 1;
        release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, danger_sign);
        (boomb_num_now)--;
    }
    else if (LOGIC_MAP[x0][y0].boomb == 0)
    {
        /*扣血*/
        score--;
        (heart_num_now)--;
        minus_heart();
    }
    else if (LOGIC_MAP[x0][y0].boomb == 1 && LOGIC_MAP[x0][y0].status == 1 && LOGIC_MAP[x0][y0].booommm == 0)
    {
        score += 4;
        LOGIC_MAP[x0][y0].booommm = 1;
        assert(LOGIC_MAP[x0][y0].number_bricks == -1); // 如果前一个地鼠打开了一个格子，但是相邻的格子又被地鼠到过，那么打后者的时候就会导致前面的打开的格子附上数字
        release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, danger_sign);
        (boomb_num_now)--;
    }

    /*检查心和炸弹是否足够胜利*/
}
void scan_hammer_dowm(int x, int y) // 没刷新
{
    int x0 = ((x - Map_position_x) / bricks);
    int y0 = ((y - Map_position_y) / bricks);
    if (LOGIC_MAP[x0][y0].mouse == 1 && LOGIC_MAP[x0][y0].status == 0) // 打到地鼠
    {
        score += 2;
        mouse_kill++;
        mouse_instinction = 0;
        LOGIC_MAP[x0][y0].mouse = 0;
        LOGIC_MAP[x0][y0].status = 1;
        LOGIC_MAP[x0][y0].number_bricks = -1;
        if (LOGIC_MAP[x0][y0].boomb == 1) // 显露有雷
        {
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, boomb);
        }
        else // 显露没有雷
        {
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, blank);
        }
        release_surrounding(x0, y0);
    }
    else if (LOGIC_MAP[x0][y0].mouse == 1 && LOGIC_MAP[x0][y0].status == 1)
    {
        score += 2;
        mouse_kill++;
        assert(LOGIC_MAP[x0][y0].number_bricks == -1);
        mouse_instinction = 0;
        if (LOGIC_MAP[x0][y0].boomb == 1) // 显露有雷
        {
            LOGIC_MAP[x0][y0].booommm = 1;
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, danger_sign);
        }
        else // 显露没有雷
        {
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, blank);
        }
    }
}
void mark_blank(int x, int y)
{
    int x0 = ((x - Map_position_x) / bricks);
    int y0 = ((y - Map_position_y) / bricks);
    if (LOGIC_MAP[x0][y0].status == 0)
    {
        LOGIC_MAP[x0][y0].mark = 1;
        if (LOGIC_MAP[x0][y0].number_bricks != -1)
        {
            int numbers2 = LOGIC_MAP[x0][y0].number_bricks;
            const int number_now2[] = {number0[0] + numbers2 * 16, number0[1], 16, 16};
            release_R(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, number_now2);
            draw_smallpixel(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, warning);
        }
        else
        {

            release_R(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, warning);
        }
    }
}
void clear_mark(int x, int y)
{
    int x0 = ((x - Map_position_x) / bricks);
    int y0 = ((y - Map_position_y) / bricks);
    LOGIC_MAP[x0][y0].mark = 0;
    if (LOGIC_MAP[x0][y0].status == 0)
    {
        if (LOGIC_MAP[x0][y0].number_bricks == -1)
        {
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, normal_bricks);
        }
        else
        {
            int numbers = LOGIC_MAP[x0][y0].number_bricks;
            const int number_now[] = {number0[0] + numbers * 16, number0[1], 16, 16};
            release_F(x0 * bricks + Map_position_x, y0 * bricks + Map_position_y, number_now);
        }
    }
}
void rand_mouse_again() // 还没有验证正确性
{
    int boom_surround = scan_surrounding_status(mouse_jump_x, mouse_jump_y);
    srand((unsigned)time(NULL));
    for (int i = 0; i < 4; i++) // 把要修改的位置的炸弹数字都变成0
    {
        int newx1 = mouse_jump_x + judge_coodinationx[i], newy1 = mouse_jump_y + judge_coodinationy[i];

        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0)
        {
            LOGIC_MAP[newx1][newy1].boomb = 0;
        }
        newx1 = mouse_jump_x - judge_coodinationx[i], newy1 = mouse_jump_y - judge_coodinationy[i];

        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0)
        {
            LOGIC_MAP[newx1][newy1].boomb = 0;
        }
    }

    for (int i = 0; i < boom_surround;) // 随机选择可修改的位置分配地雷
    {
        int j = rand() % 4;
        int k = rand() % 2;
        int l = -1;
        for (int t = 0; t < k; t++)
        {
            l *= l;
        }
        int new2x = mouse_jump_x + l * judge_coodinationx[j], new2y = mouse_jump_y + l * judge_coodinationy[j];
        if (valid(new2x, new2y) > 0 && LOGIC_MAP[new2x][new2y].boomb == 0 && LOGIC_MAP[new2x][new2y].status == 0)
        {
            i++;
            LOGIC_MAP[new2x][new2y].boomb = 1;
        }
    }
    for (int i = 0; i < 4; i++) // 修改有数字的方块的数字
    {
        int newx1 = mouse_jump_x + judge_coodinationx[i], newy1 = mouse_jump_y + judge_coodinationy[i];
        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0)
        {
            if (LOGIC_MAP[newx1][newy1].number_bricks != -1)
            {
                release_number(newx1, newy1);
            }
        }
        newx1 = mouse_jump_x - judge_coodinationx[i], newy1 = mouse_jump_y - judge_coodinationy[i];
        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0)
        {
            if (LOGIC_MAP[newx1][newy1].number_bricks != -1)
            {
                release_number(newx1, newy1);
            }
        }
    }
    for (int i = 0; i < 8; i++) // 修改有数字的方块的数字
    {
        int newx1 = mouse_jump_x + judge_coodinationx2[i], newy1 = mouse_jump_y + judge_coodinationy2[i];
        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0 && LOGIC_MAP[newx1][newy1].number_bricks != -1)
        {

            release_number(newx1, newy1);
        }
        newx1 = mouse_jump_x - judge_coodinationx2[i], newy1 = mouse_jump_y - judge_coodinationy2[i];
        if (valid(newx1, newy1) > 0 && LOGIC_MAP[newx1][newy1].status == 0 && LOGIC_MAP[newx1][newy1].number_bricks != -1)
        {

            release_number(newx1, newy1);
        }
    }
}
void mouse_timerevent()
{
    if (mouse_instinction == 1)
    {
        score--;
        clear_mouse();
        heart_num_now--;
        minus_heart();
        rand_mouse_again();
        if (cheat_open == 1)
        {
            PRINT_CLEAR();
            PRINT_MAP_cheat();
        }
    }

    mouse_instinction = 1;
    generate_mouse();
    PRINT_MAP();
}
unsigned int Callback(unsigned interval, void *param)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.data1 = (void *)mouse_timerevent;
    SDL_PushEvent(&event);
    if (score > 50 && score <= 100)
    {
        interval = 6000;
    }
    else if (score <= 150)
    {
        interval = 5000;
    }
    else if (score <= 175)
    {
        interval = 4000;
    }
    else if (score <= 500)
    {
        interval = 4000;
    }
    else
    {
        interval -= 10;
    }

    return interval;
}
int scan__win_judge() // 1 是成功 0 是失败
{
    int flag = 1;
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (LOGIC_MAP[x][y].status == 0 && LOGIC_MAP[x][y].mark == 0)
            {
                flag = 0;
            }
        }
    }
    return flag;
}
int scan_judge_map()
{
    int marked = 0;
    int released = 0;
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (LOGIC_MAP[x][y].status == 1)
            {
                released++;
            }
            else if (LOGIC_MAP[x][y].mark == 1) // 可能有bug
            {
                marked++;
            }
        }
    }
    if (released + marked == map_col * map_col)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void saveScore()
{
    if (max_score < score)
    {
        max_score = score;
    }

    FILE *file = fopen("leaderboard.txt", "w");
    if (file != NULL)
    {
        fprintf(file, "%d", max_score);
        fclose(file);
    }
}
void loadLeaderboard()
{
    FILE *file = fopen("leaderboard.txt", "r");
    if (file != NULL)
    {

        if (fscanf(file, "%d", &max_score) == EOF)
        {
            max_score = 0;
        }

        fclose(file);
    }
}

int main()
{
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("init wrong!\n");
    }
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
    {
        fprintf(stderr, "SDL_GetDesktopDisplayMode Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    win = SDL_CreateWindow("mine", 0, 0, displayMode.w, displayMode.h,
                           SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Surface *icon = IMG_Load("image/boom.png");
    SDL_SetWindowIcon(win, icon);
    SDL_FreeSurface(icon);
    render1 = SDL_CreateRenderer(win, -1, 0);
    if (render1 == NULL)
    {
        SDL_Log("render wrong!\n");
    }
    TTF_Font *title_font = TTF_OpenFont("font\\VerticalSans.ttf", 250);
    init_menu(title_font);
    SDL_Rect text_start_rect = {displayMode.w / 7, displayMode.h / 3 + 100, 520, 150};       // 有潜在风险426, 650, 525, 150
    SDL_Rect text_tips_rect = {displayMode.w / 2 + 270, displayMode.h / 3 + 100, 420, 150};  // 420, 150
    SDL_Rect text_intro_rect = {displayMode.w / 2 + 270, displayMode.h / 2 + 270, 540, 150}; // 540, 150
    SDL_Surface *map_surface = IMG_Load("image/colored-transparent_packed.png");
    map_texture = SDL_CreateTextureFromSurface(render1, map_surface);

    SDL_Event event;
    int isquit = 0;
    while (isquit == 0)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    isquit = 1;
                }
                break;
            case SDL_QUIT:
                isquit = 1;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (scan_rect(event.button.x, event.button.y, &text_start_rect) && event.button.clicks == 1)
                {
                RESTARTT:
                    draw_map_init();
                    init_logic_map();
                    TTF_SetFontSize(title_font, 40);
                    write_word_Free("READ TIPS PLEASE", font_color2, title_font, 500, 1000);
                    SDL_Rect *rect_start2 = write_word_F("START", font_color2, title_font, displayMode.w - 250, 200);
                    SDL_Rect *rect_stop = write_word_F("STOP", font_color2, title_font, displayMode.w - 250, 300);
                    SDL_Rect *rect_menu = write_word("MENU", font_color2, title_font, displayMode.w - 250, 400);
                    SDL_Rect *rect_restart = write_word("RESTART", font_color2, title_font, displayMode.w - 250, 500);
                    SDL_Rect *rect_cheat = write_word("CHEAT", font_color2, title_font, displayMode.w - 325, 600);

                    boomb_num_now = boomb_num;
                    heart_num_now = heart_num;
                    int flag1 = 0;
                    int flag2 = 0;
                    int time1 = 0;
                    int time2 = 0;
                    int game_qiut1 = 0;
                    SDL_Event start_play_event;
                    while (game_qiut1 == 0)
                    {
                        if (SDL_PollEvent(&start_play_event))
                        {
                            switch (start_play_event.type)
                            {
                            case SDL_MOUSEBUTTONDOWN:
                                if (scan_rect(start_play_event.button.x, start_play_event.button.y, rect_start2) && start_play_event.button.clicks == 1)
                                {
                                    /*时间开始跑*/
                                    loadLeaderboard();
                                    int time_start = SDL_GetTicks64();
                                    SDL_Event event_now;
                                    int gamestop = 0;
                                    mouse_instinction = 1;
                                    score = 0;
                                    SDL_TimerID id = SDL_AddTimer(mouse_jump_frequency, Callback, NULL);
                                    generate_mouse();
                                    draw_mouse();

                                    while (gamestop == 0)
                                    {

                                        if (heart_num_now == 0)
                                        {
                                            gamestop = 1;
                                            SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
                                            SDL_RenderFillRect(render1, &map_position_rect);
                                            TTF_SetFontSize(title_font, 100);
                                            write_word_Free("YOU LOSE", font_color2, title_font, Map_position_x + 100, Map_position_y + 200);
                                            SDL_RenderPresent(render1);
                                            saveScore();
                                        }
                                        else if (boomb_num_now == 0 && mouse_kill >= mouse_killed_standard)
                                        {
                                            if (scan_judge_map() > 0)
                                            {
                                                gamestop = 1;
                                                SDL_SetRenderDrawColor(render1, 77, 62, 77, 255);
                                                SDL_RenderFillRect(render1, &map_position_rect);
                                                int win = scan__win_judge();
                                                if (win == 1)
                                                {
                                                    score += 20;
                                                    TTF_SetFontSize(title_font, 100);
                                                    write_word_Free("YOU WIN", font_color2, title_font, Map_position_x + 100, Map_position_y + 200);
                                                    write_word_Free("+20", font_color2, title_font, Map_position_x + 150, Map_position_y + 350);
                                                    SDL_RenderPresent(render1);
                                                }
                                                else
                                                {
                                                    TTF_SetFontSize(title_font, 100);
                                                    write_word_Free("YOU LOSE", font_color2, title_font, Map_position_x + 100, Map_position_y + 200);
                                                    SDL_RenderPresent(render1);
                                                }
                                                saveScore();
                                            }
                                        }
                                        PRINT_MESSAGE(time_start, title_font, &flag1, &time1, &flag2, &time2);
                                        PRINT(time_start, title_font); // 单位是毫秒

                                        if (SDL_PollEvent(&event_now))
                                        {
                                            switch (event_now.type)
                                            {
                                            case SDL_MOUSEBUTTONDOWN:
                                                if (scan_rect(event_now.button.x, event_now.button.y, rect_stop) && event_now.button.clicks == 1)
                                                {
                                                    gamestop = 1;
                                                }
                                                else if (scan_rect(event_now.button.x, event_now.button.y, &map_position_rect) && event_now.button.clicks == 1 && event_now.button.button == SDL_BUTTON_LEFT)
                                                {
                                                    scan_spade_down(event_now.button.x, event_now.button.y);
                                                    SDL_RenderPresent(render1);
                                                }
                                                else if (scan_rect(event_now.button.x, event_now.button.y, &map_position_rect) && event_now.button.clicks == 1 && event_now.button.button == SDL_BUTTON_RIGHT)
                                                {
                                                    scan_hammer_dowm(event_now.button.x, event_now.button.y);
                                                    SDL_RenderPresent(render1);
                                                }
                                                else if (scan_rect(event_now.button.x, event_now.button.y, &map_position_rect) && event_now.button.clicks == 2 && event_now.button.button == SDL_BUTTON_RIGHT)
                                                {
                                                    int x0 = ((event_now.button.x - Map_position_x) / bricks);
                                                    int y0 = ((event_now.button.y - Map_position_y) / bricks);
                                                    if (LOGIC_MAP[x0][y0].mark == 0)
                                                    {
                                                        mark_blank(event_now.button.x, event_now.button.y);
                                                    }
                                                    else
                                                    {
                                                        clear_mark(event_now.button.x, event_now.button.y);
                                                    }
                                                    SDL_RenderPresent(render1);
                                                }
                                                break;
                                            case SDL_USEREVENT:
                                                ((void (*)(void))event_now.user.data1)();
                                                if (mouse_instinction == 1)
                                                {
                                                    draw_mouse();
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    SDL_RemoveTimer(id);
                                }

                                else if (scan_rect(start_play_event.button.x, start_play_event.button.y, rect_menu) && start_play_event.button.clicks == 1)
                                {
                                    game_qiut1 = 1;
                                    init_menu(title_font);
                                }
                                else if (scan_rect(start_play_event.button.x, start_play_event.button.y, rect_restart) && start_play_event.button.clicks == 1)
                                {
                                    saveScore();
                                    goto RESTARTT;
                                }
                                else if (scan_rect(start_play_event.button.x, start_play_event.button.y, rect_cheat) && start_play_event.button.clicks == 1)
                                {
                                    if (cheat_open == 0)
                                    {
                                        PRINT_MAP_cheat();
                                    }
                                    else
                                    {
                                        PRINT_CLEAR();
                                    }
                                    SDL_RenderPresent(render1);
                                }

                                break;
                            case SDL_KEYDOWN:
                                if (start_play_event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                                {
                                    saveScore();
                                    goto END;
                                }
                                break;
                            case SDL_QUIT:
                                saveScore();
                                goto END;
                                break;
                            }
                        }
                    }
                    saveScore();
                    free(rect_menu);
                    free(rect_restart);
                    free(rect_start2);
                    free(rect_stop);
                    free(rect_cheat);
                }
                else if (scan_rect(event.button.x, event.button.y, &text_intro_rect) && event.button.clicks == 1)
                {
                    SDL_SetRenderDrawColor(render1, 54, 59, 64, 255);
                    SDL_RenderClear(render1);
                    SDL_Surface *intro_surface = IMG_Load("image\\intro.png");
                    SDL_Texture *intro_texture = SDL_CreateTextureFromSurface(render1, intro_surface);
                    SDL_Rect intro_rect = {0, 0, intro_surface->w, intro_surface->h};
                    SDL_RenderCopy(render1, intro_texture, NULL, &intro_rect);
                    SDL_RenderPresent(render1);
                    SDL_FreeSurface(intro_surface);
                    SDL_DestroyTexture(intro_texture);
                    SDL_Event event4;
                    int return_to_menu = 0;
                    while (return_to_menu == 0)
                    {
                        if (SDL_PollEvent(&event4))
                        {
                            switch (event4.type)
                            {
                            case SDL_MOUSEBUTTONDOWN:
                                if (event4.button.clicks == 1)
                                {
                                    init_menu(title_font);
                                    return_to_menu = 1;
                                }
                                break;

                            default:
                                break;
                            }
                        }
                    }
                }
                else if (scan_rect(event.button.x, event.button.y, &text_tips_rect) && event.button.clicks == 1)
                {
                    SDL_SetRenderDrawColor(render1, 54, 59, 64, 255);
                    SDL_RenderClear(render1);
                    SDL_Surface *intro_surface = IMG_Load("image\\tips.png");
                    SDL_Texture *intro_texture = SDL_CreateTextureFromSurface(render1, intro_surface);
                    SDL_Rect intro_rect = {0, 0, intro_surface->w - 816, intro_surface->h - 500};
                    SDL_RenderCopy(render1, intro_texture, NULL, &intro_rect);
                    SDL_RenderPresent(render1);
                    SDL_FreeSurface(intro_surface);
                    SDL_DestroyTexture(intro_texture);
                    SDL_Event event4;
                    int return_to_menu = 0;
                    while (return_to_menu == 0)
                    {
                        if (SDL_PollEvent(&event4))
                        {
                            switch (event4.type)
                            {
                            case SDL_MOUSEBUTTONDOWN:
                                if (event4.button.clicks == 1)
                                {
                                    init_menu(title_font);
                                    return_to_menu = 1;
                                }
                                break;

                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
END:
    TTF_CloseFont(title_font);
    SDL_DestroyRenderer(render1);
    SDL_DestroyWindow(win);
    SDL_FreeSurface(map_surface);
    SDL_DestroyTexture(map_texture);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    return 0;
}