#include "framework.h"
#include "BattleShips.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include <D2BMPLOADER.h>
#include <ErrH.h>
#include <FCheck.h>
#include <battle.h>
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "battle.lib")

constexpr wchar_t bWinClassName[]{ L"New_BattleShips" };

constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t snd_file[]{ L".\\res\\snd\\main.wav" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };

constexpr int mNew{ 1001 };
constexpr int mExit{ 1002 };
constexpr int mSave{ 1003 };
constexpr int mLoad{ 1004 };
constexpr int mHoF{ 1005 };

constexpr int no_record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int record{ 2003 };

constexpr int key1{ 49 };
constexpr int key2{ 50 };
constexpr int key3{ 51 };
constexpr int key4{ 52 };

////////////////////////////////////////////////

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HICON mainIcon{ nullptr };
HCURSOR mainCur{ nullptr };
HCURSOR outCur{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};
MSG bMsg{};
BOOL bRet = 0;
POINT cur_pos{ 0,0 };

float render_target_x_scale{ 1.0f };
float render_target_y_scale{ 1.0f };

D2D1_RECT_F b1Rect{ 50.0f, 5.0f, scr_width / 3 - 50.0f, 45.0f };
D2D1_RECT_F b2Rect{ scr_width / 3 + 50.0f, 5.0f, scr_width * 2 / 3 - 50.0f, 45.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3 + 50.0f, 5.0f, scr_width - 50.0f, 45.0f };

D2D1_RECT_F b1TxtRect{ 100.0f, 10.0f, scr_width / 3 - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3 + 100.0f, 10.0f, scr_width * 2 / 3 - 50.0f, 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2 / 3 + 100.0f, 10.0f, scr_width - 50.0f, 40.0f };

bool pause = false;
bool next_turn = false;
bool in_client = true;
bool sound = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

wchar_t player1[16]{ L"МЪртинчУ" };
wchar_t player2[16]{ L"ДаниелчУ" };

bool name1_set = false;
bool name2_set = false;

bool player1_set = false;
bool player2_set = false;

bool show_help = false;

bool show_grid1 = false;
bool show_grid2 = false;

bool first_player_turn = true;

bool first_player_shoot = false;
bool second_player_shoot = false;

bool first_player_win = false;
bool second_player_win = false;

int score1 = 0;
int score2 = 0;

int turn_count = 0;

bool min_selected = false;
bool small_selected = false;
bool mid_selected = false;
bool big_selected = false;

bool erase_current_ship = false;

bool pl1_min_deployed = false;
bool pl1_small_deployed = false;
bool pl1_mid_deployed = false;
bool pl1_big_deployed = false;

bool pl2_min_deployed = false;
bool pl2_small_deployed = false;
bool pl2_mid_deployed = false;
bool pl2_big_deployed = false;

struct FRAMEBMP
{
    int delay = 0;
    int max_delay = 0;

    float sx{ 0 };
    float sy{ 0 };

    float ex{ 0 };
    float ey{ 0 };

    int frame = 0;
    int max_frames = 0;
    int GetFrame()
    {
        delay--;
        if (delay < 0)
        {
            delay = max_delay;
            ++frame;
            if (frame > max_frames)frame = 0;
        }
        return frame;
    }
};

FRAMEBMP FieldFrame;
FRAMEBMP IntroFrame;

dll::RANDIT Randerer{};

dll::GRID* grid1{ nullptr };
dll::GRID* grid2{ nullptr };

dll::GRID* attack_grid1{ nullptr };
dll::GRID* attack_grid2{ nullptr };

std::vector<dll::Ship>vPl1Ships;
std::vector<dll::Ship>vPl2Ships;

std::vector<FRAMEBMP>vExplosions1;
std::vector<FRAMEBMP>vExplosions2;

//////////////////////////////////////////////////

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* statBckgBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };

ID2D1SolidColorBrush* GreenBoundBrush{ nullptr };
ID2D1SolidColorBrush* RedBoundBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmText{ nullptr };
IDWriteTextFormat* midText{ nullptr };
IDWriteTextFormat* bigText{ nullptr };

ID2D1Bitmap* bmpMinShip{ nullptr };

ID2D1Bitmap* bmpSmallHShip{ nullptr };
ID2D1Bitmap* bmpSmallVShip{ nullptr };

ID2D1Bitmap* bmpMid1HShip{ nullptr };
ID2D1Bitmap* bmpMid1VShip{ nullptr };

ID2D1Bitmap* bmpMid2HShip{ nullptr };
ID2D1Bitmap* bmpMid2VShip{ nullptr };

ID2D1Bitmap* bmpBig1HShip{ nullptr };
ID2D1Bitmap* bmpBig1VShip{ nullptr };

ID2D1Bitmap* bmpBig2HShip{ nullptr };
ID2D1Bitmap* bmpBig2VShip{ nullptr };

ID2D1Bitmap* bmpOnTarget{ nullptr };
ID2D1Bitmap* bmpMissed{ nullptr };

ID2D1Bitmap* bmpField[5]{ nullptr };
ID2D1Bitmap* bmpExplosion[26]{ nullptr };
ID2D1Bitmap* bmpIntro[19]{ nullptr };

///////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
    check.Release();
};
template<HasRelease T>bool ClrHeap(T** what)
{
    if (*what)
    {
        (*what)->Release();
        (*what) = nullptr;
        return true;
    }
    return false;
};

void LogError(LPCWSTR what)
{
    std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
    err << what << L" Time of occurrence: " << std::chrono::system_clock::now() << std::endl;
    err.close();
}
void ReleaseResources()
{
    if (!ClrHeap(&iFactory))LogError(L"Error releasing D2D1 Factory !");
    if (!ClrHeap(&Draw))LogError(L"Error releasing D2D1 RenderTarget !");
    if (!ClrHeap(&b1BckgBrush))LogError(L"Error releasing D2D1 b1BckgBrush !");
    if (!ClrHeap(&b2BckgBrush))LogError(L"Error releasing D2D1 b2BckgBrush !");
    if (!ClrHeap(&b3BckgBrush))LogError(L"Error releasing D2D1 b3BckgBrush !");

    if (!ClrHeap(&statBckgBrush))LogError(L"Error releasing D2D1 statBckgBrush !");
    if (!ClrHeap(&hgltBrush))LogError(L"Error releasing D2D1 hgltBrush !");
    if (!ClrHeap(&txtBrush))LogError(L"Error releasing D2D1 txtBrush !");
    if (!ClrHeap(&inactBrush))LogError(L"Error releasing D2D1 inactBrush !");

    if (!ClrHeap(&GreenBoundBrush))LogError(L"Error releasing D2D1 GreenBoundBrush !");
    if (!ClrHeap(&RedBoundBrush))LogError(L"Error releasing D2D1 RedBoundBrush !");

    if (!ClrHeap(&iWriteFactory))LogError(L"Error releasing D2D1 WriteFactory !");
    if (!ClrHeap(&nrmText))LogError(L"Error releasing D2D1 nrmText !");
    if (!ClrHeap(&midText))LogError(L"Error releasing D2D1 midText !");
    if (!ClrHeap(&bigText))LogError(L"Error releasing D2D1 bigText !");

    if (!ClrHeap(&bmpMinShip))LogError(L"Error releasing D2D1 bmpMinShip !");
    if (!ClrHeap(&bmpSmallHShip))LogError(L"Error releasing D2D1 bmpSmallHShip !");
    if (!ClrHeap(&bmpSmallVShip))LogError(L"Error releasing D2D1 bmpSmallVShip !");
    if (!ClrHeap(&bmpMid1HShip))LogError(L"Error releasing D2D1 bmpMid1HShip !");
    if (!ClrHeap(&bmpMid1VShip))LogError(L"Error releasing D2D1 bmpMid1VShip !");
    if (!ClrHeap(&bmpMid2HShip))LogError(L"Error releasing D2D1 bmpMid2HShip !");
    if (!ClrHeap(&bmpMid2VShip))LogError(L"Error releasing D2D1 bmpMid2VShip !");
    if (!ClrHeap(&bmpBig1HShip))LogError(L"Error releasing D2D1 bmpBig1HShip !");
    if (!ClrHeap(&bmpBig1VShip))LogError(L"Error releasing D2D1 bmpBig1VShip !");
    if (!ClrHeap(&bmpBig2HShip))LogError(L"Error releasing D2D1 bmpBig2HShip !");
    if (!ClrHeap(&bmpBig2VShip))LogError(L"Error releasing D2D1 bmpBig2VShip !");

    if (!ClrHeap(&bmpOnTarget))LogError(L"Error releasing D2D1 bmpOnTarget !");
    if (!ClrHeap(&bmpMissed))LogError(L"Error releasing D2D1 bmpMissed !");

    for (int i = 0; i < 5; ++i)if (!ClrHeap(&bmpField[i]))LogError(L"Error releasing D2D1 bmpField !");
    for (int i = 0; i < 26; ++i)if (!ClrHeap(&bmpExplosion[i]))LogError(L"Error releasing D2D1 bmpExplosion !");
    for (int i = 0; i < 19; ++i)if (!ClrHeap(&bmpIntro[i]))LogError(L"Error releasing D2D1 bmpIntro !");
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    std::remove(tmp_file);
    ReleaseResources();
    exit(1);
}
BOOL CheckRecord()
{
    if (score1 < 1 && score2 < 1)return no_record;

    if (first_player_win)
    {
        int result = 0;
        CheckFile(record_file, &result);

        if (result == FILE_NOT_EXIST)
        {
            std::wofstream rec(record_file);
            rec << score1 << std::endl;
            for (int i = 0; i < 16; ++i)rec << static_cast<int>(player1[i]) << std::endl;
            rec.close();
            return first_record;
        }
        else
        {
            std::wifstream check(record_file);
            check >> result;
            check.close();

            if (result < score1)
            {
                std::wofstream rec(record_file);
                rec << score1 << std::endl;
                for (int i = 0; i < 16; ++i)rec << static_cast<int>(player1[i]) << std::endl;
                rec.close();
                return record;
            }
        }
    }
    else if (second_player_win)
    {
        int result = 0;
        CheckFile(record_file, &result);

        if (result == FILE_NOT_EXIST)
        {
            std::wofstream rec(record_file);
            rec << score2 << std::endl;
            for (int i = 0; i < 16; ++i)rec << static_cast<int>(player2[i]) << std::endl;
            rec.close();
            return first_record;
        }
        else
        {
            std::wifstream check(record_file);
            check >> result;
            check.close();

            if (result < score2)
            {
                std::wofstream rec(record_file);
                rec << score2 << std::endl;
                for (int i = 0; i < 16; ++i)rec << static_cast<int>(player2[i]) << std::endl;
                rec.close();
                return record;
            }
        }
    }

    return no_record;
}
bool IsNear(dll::TILE myTile, dll::TILE checkTile)
{
    if (abs(myTile.col - checkTile.col) > 1 || abs(myTile.row - checkTile.row) > 1)return false;

    return true;
}
void GameOver()
{
    PlaySound(NULL, NULL, NULL);

    if (bigText && hgltBrush && (first_player_win || second_player_win))
    {
        wchar_t win_txt[30]{ L"ПОБЕДА ЗА " };
        int win_size = 0;

        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        if (first_player_win)
        {
            wcscat_s(win_txt, player1);
            for (int i = 0; i < 30; ++i)
            {
                if (win_txt[i] != '\0')++win_size;
                else break;
            }

            Draw->DrawTextW(win_txt, win_size, bigText,
                D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            PlaySound(L".\\res\\snd\\win_player1.wav", NULL, SND_SYNC);
            Sleep(2000);
        }
        else
        {
            wcscat_s(win_txt, player2);
            for (int i = 0; i < 30; ++i)
            {
                if (win_txt[i] != '\0')++win_size;
                else break;
            }

            Draw->DrawTextW(win_txt, win_size, bigText,
                D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            PlaySound(L".\\res\\snd\\win_player2.wav", NULL, SND_SYNC);
            Sleep(2000);
        }

        if (turn_count <= 30)
        {
            if (turn_count <= 15)
            {
                if (first_player_win)score1 += 200;
                else score2 += 200;

                Draw->BeginDraw();
                Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
                Draw->DrawTextW(L"МАКСИМАЛЕН БОНУС !", 19, bigText,
                    D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
                Draw->EndDraw();
                PlaySound(L".\\res\\snd\\bonus.wav", NULL, SND_SYNC);
                Sleep(2000);
            }
            else if (turn_count <= 20)
            {
                if (first_player_win)score1 += 150;
                else score2 += 150;
                Draw->BeginDraw();
                Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
                Draw->DrawTextW(L"ПОЛУЧИ БОНУС !", 15, bigText,
                    D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
                Draw->EndDraw();
                PlaySound(L".\\res\\snd\\bonus.wav", NULL, SND_SYNC);
                Sleep(2000);
            }
            else if (turn_count <= 25)
            {
                if (first_player_win)score1 += 100;
                else score2 += 100;
                Draw->BeginDraw();
                Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
                Draw->DrawTextW(L"ПОЛУЧИ БОНУС !", 15, bigText,
                    D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
                Draw->EndDraw();
                PlaySound(L".\\res\\snd\\bonus.wav", NULL, SND_SYNC);
                Sleep(2000);
            }
            else if (turn_count <= 30)
            {
                if (first_player_win)score1 += 50;
                else score2 += 50;
                Draw->BeginDraw();
                Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
                Draw->DrawTextW(L"МИНИМАЛЕН БОНУС !", 18, bigText,
                    D2D1::RectF(300.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
                Draw->EndDraw();
                PlaySound(L".\\res\\snd\\bonus.wav", NULL, SND_SYNC);
                Sleep(2000);
            }
        }
    }

    if (bigText && hgltBrush)
    {
        switch (CheckRecord())
        {
        case no_record:
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawTextW(L"НЯМА РЕКОРД НА ИГРАТА !", 24, bigText,
                D2D1::RectF(100.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_SYNC);
            Sleep(1500);
            break;

        case first_record:
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawTextW(L"ПЪРВИ РЕКОРД НА ИГРАТА !", 25, bigText,
                D2D1::RectF(100.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            PlaySound(L".\\res\\snd\\record.wav", NULL, SND_SYNC);
            Sleep(1500);
            break;

        case record:
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawTextW(L"СВЕТОВЕН РЕКОРД НА ИГРАТА !", 28, bigText,
                D2D1::RectF(50.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            PlaySound(L".\\res\\snd\\record.wav", NULL, SND_SYNC);
            Sleep(1500);
            break;
        }
    }

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    wcscpy_s(player1, L"МЪртинчУ");
    wcscpy_s(player2, L"ДаниелчУ");
    
    name1_set = false;
    name2_set = false;

    show_help = false;

    player1_set = false;
    player2_set = false;

    first_player_turn = true;
    first_player_win = false;
    second_player_win = false;

    show_grid1 = false;
    show_grid2 = false;

    score1 = 0;
    score2 = 0;

    turn_count = 1;

    min_selected = false;
    small_selected = false;
    mid_selected = false;
    big_selected = false;

    erase_current_ship = false;

    first_player_shoot = false;
    second_player_shoot = false;

    pl1_min_deployed = 0;
    pl1_small_deployed = 0;
    pl1_mid_deployed = 0;
    pl1_big_deployed = 0;

    pl2_min_deployed = 0;
    pl2_small_deployed = 0;
    pl2_mid_deployed = 0;
    pl2_big_deployed = 0;

    FieldFrame.delay = 12;
    FieldFrame.max_delay = 12;
    FieldFrame.max_frames = 4;

    IntroFrame.delay = 3;
    IntroFrame.max_delay = 3;
    IntroFrame.max_frames = 18;

    //////////////////////////////////////////

    if (grid1)grid1->Release();
    grid1 = new dll::GRID();
    
    if (grid2)grid2->Release();
    grid2 = new dll::GRID();

    if (attack_grid1)attack_grid1->Release();
    attack_grid1 = new dll::GRID();

    if (attack_grid2)attack_grid2->Release();
    attack_grid2 = new dll::GRID();

    if (!vPl1Ships.empty())
        for (int i = 0; i < vPl1Ships.size(); ++i)vPl1Ships[i]->Release();
    vPl1Ships.clear();

    if (!vPl2Ships.empty())
        for (int i = 0; i < vPl2Ships.size(); ++i)vPl2Ships[i]->Release();
    vPl2Ships.clear();

    vExplosions1.clear();
    vExplosions2.clear();
}
void ShowRecord()
{
    int result = 0;
    CheckFile(record_file, &result);

    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма рекорд на играта !\n\nПостарай се повече !", L"Липсва файл !",
            MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        return;
    }

    wchar_t rec_txt[75]{ L"КАПИТАН: " };
    wchar_t add[5]{ L"\0" };
    int rec_size = 0;

    wchar_t saved_player[16]{ L"\0" };

    std::wifstream rec(record_file);
    rec >> result;
    for (int i = 0; i < 16; ++i)
    {
        int letter{ 0 };
        rec >> letter;
        saved_player[i] = static_cast<wchar_t>(letter);
    }
    rec.close();

    wcscat_s(rec_txt, saved_player);
    wcscat_s(rec_txt, L"\n\nСВЕТОВЕН РЕКОРД: ");
    wsprintf(add, L"%d", result);
    wcscat_s(rec_txt, add);

    for (int i = 0; i < 75; ++i)
    {
        if (rec_txt[i] != '\0')rec_size++;
        else break;
    }

    if (bigText && hgltBrush)
    {
        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawTextW(rec_txt, rec_size, bigText,
            D2D1::RectF(100.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
        Draw->EndDraw();
        if (sound)mciSendString(L"play .\\res\\snd\\showrec.wav", NULL, NULL, NULL);
        Sleep(4000);
    }
}
void ShowHelp()
{
    int result = 0;
    CheckFile(help_file, &result);

    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Липсва помощна информация за играта !\n\nСвържете се с разработчика !", L"Липсва файл !",
            MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        return;
    }

    wchar_t help_txt[500];
    std::wifstream help(help_file);
    help >> result;
    for (int i = 0; i < result; ++i)
    {
        int letter = 0;
        help >> letter;
        help_txt[i] = static_cast<wchar_t>(letter);
    }
    help.close();

    if (midText && RedBoundBrush)
    {
        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawTextW(help_txt, result, midText, D2D1::RectF(100.0f, 100.0f, scr_width, scr_height), RedBoundBrush);
        Draw->EndDraw();
        if (sound)mciSendString(L"play .\\res\\snd\\showhelp.wav", NULL, NULL, NULL);
    }
}
void SaveGame()
{
    int result{ 0 };
    CheckFile(save_file, &result);

    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма записана игра !\n\nПостарай се повече !", L"Липсва файл !",
            MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        return;
    }
    else
    {
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(bHwnd, L"Ако презаредиш, губиш тази игра !\n\nНаистина ли презареждаш ?",
            L"Презареждане", MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDNO) return;
    }

    std::wofstream save(save_file);

    save << score1 << std::endl;
    save << score2 << std::endl;
    for (int i = 0; i < 16; ++i)save << static_cast<wchar_t>(player1[i]) << std::endl;
    for (int i = 0; i < 16; ++i)save << static_cast<wchar_t>(player2[i]) << std::endl;
    save << player1_set << std::endl;
    save << player2_set << std::endl;

    save << first_player_turn << std::endl;
    save << first_player_win << std::endl;
    save << second_player_win << std::endl;

    save << show_grid1 << std::endl;
    save << show_grid2 << std::endl;

    save << turn_count << std::endl;

    save << min_selected << std::endl;
    save << small_selected << std::endl;
    save << mid_selected << std::endl;
    save << big_selected << std::endl;

    save << erase_current_ship << std::endl;
    save << first_player_shoot << std::endl;
    save << second_player_shoot << std::endl;

    save << pl1_min_deployed << std::endl;
    save << pl1_small_deployed << std::endl;
    save << pl1_mid_deployed << std::endl;
    save << pl1_min_deployed << std::endl;

    save << pl2_min_deployed << std::endl;
    save << pl2_small_deployed << std::endl;
    save << pl2_mid_deployed << std::endl;
    save << pl2_min_deployed << std::endl;

    for (int i = 0; i < MAX_COLS; i++)
    {
        for (int k = 0; k < MAX_ROWS; ++k)
            save << static_cast<int>(grid1->grid[i][k].state) << std::endl;
    }
    for (int i = 0; i < MAX_COLS; i++)
    {
        for (int k = 0; k < MAX_ROWS; ++k)
            save << static_cast<int>(grid2->grid[i][k].state) << std::endl;
    }

    for (int i = 0; i < MAX_COLS; i++)
    {
        for (int k = 0; k < MAX_ROWS; ++k)
            save << static_cast<int>(attack_grid1->grid[i][k].state) << std::endl;
    }
    for (int i = 0; i < MAX_COLS; i++)
    {
        for (int k = 0; k < MAX_ROWS; ++k)
            save << static_cast<int>(attack_grid2->grid[i][k].state) << std::endl;
    }

    save << vPl1Ships.size() << std::endl;
    if (!vPl1Ships.empty())
    {
        for (int i = 0; i < vPl1Ships.size(); ++i)
        {
            save << static_cast<int>(vPl1Ships[i]->get_type()) << std::endl;
        }
    }

}

D2D1_RECT_F RectBound(dll::TILE what)
{
    D2D1_RECT_F ret{};

    ret.left = what.start.x + 1.0f;
    ret.right = what.end.x - 1.0f;
    ret.top = what.start.y + 1.0f;
    ret.bottom = what.end.y - 1.0f;

    return ret;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return true;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        if (first_player_turn)
        {
            if (GetDlgItemText(hwnd, IDC_NAME, player1, 16) < 1)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                wcscpy_s(player1, L"МЪртинчУ");
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            name1_set = true;
            EndDialog(hwnd, IDOK);
            break;
        }
        else
        {
            if (GetDlgItemText(hwnd, IDC_NAME, player2, 16) < 1)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                wcscpy_s(player1, L"МЪртинчУ");
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            name2_set = true;
            EndDialog(hwnd, IDOK);
            break;
        }
        break;
    }

    return(INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch(ReceivedMsg)
    {
    case WM_CREATE:
        if (bIns)
        {
            bBar = CreateMenu();
            bMain = CreateMenu();
            bStore = CreateMenu();

            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

            AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
            AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bMain, MF_STRING, mExit, L"Изход");

            AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
            AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
            AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

            SetMenu(hwnd, bBar);
            InitGame();
        }

        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако излезете, губите тази игра !\n\nНаистина ли излизате ?",
            L"Изход", MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(10, 10, 10)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }
            
            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = true;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                        b3Hglt = true;
                    }
                }

                SetCursor(outCur);
                return true;
            }
            else if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }
        
            SetCursor(mainCur);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(LoadCursorW(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND: 
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако рестартирате, губите тази игра !\n\nНаистина ли рестартирате ?",
                L"Рестарт", MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        }
        break;

    case WM_KEYDOWN:
        if ((first_player_turn && player1_set)|| (!first_player_turn && player2_set))
        {
            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
            MessageBox(hwnd, L"Флотът вече е поставен !", L"Играта почна !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
            break;
        }  
        if (wParam != VK_ESCAPE
            && (min_selected || small_selected || mid_selected || big_selected))
        {
            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
            MessageBox(hwnd, L"Довърши кораба,който си избрал !", L"Вече има избран кораб !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
            break;
        }
        else
        {
            switch (wParam)
            {
            case key1:
                if (first_player_turn)
                {
                    if (pl1_min_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Лодката вече е поставена !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    min_selected = true;
                    break;
                }
                else
                {
                    if (pl2_min_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Лодката вече е поставена !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    min_selected = true;
                    break;
                }
                break;

            case key2:
                if (first_player_turn)
                {
                    if (pl1_small_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Малкият кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    small_selected = true;
                    break;
                }
                else
                {
                    if (pl2_small_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Малкият кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    small_selected = true;
                    break;
                }
                break;

            case key3:
                if (first_player_turn)
                {
                    if (pl1_mid_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Средният кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    mid_selected = true;
                    break;
                }
                else
                {
                    if (pl2_mid_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Средният кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    mid_selected = true;
                    break;
                }
                break;

            case key4:
                if (first_player_turn)
                {
                    if (pl1_big_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Големият кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    big_selected = true;
                    break;
                }
                else
                {
                    if (pl2_big_deployed)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Големият кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }
                    big_selected = true;
                    break;
                }
                break;

            case VK_F1:
                if (!show_help)
                {
                    show_help = true;
                    pause = true;
                    ShowHelp();
                    break;
                }
                else
                {
                    show_help = false;
                    pause = false;
                    break;
                }
                break;

            case VK_F2:
                if (!first_player_turn)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    show_grid1 = false;
                    break;
                }
                if (!show_grid1)show_grid1 = true;
                else show_grid1 = false;
                break;

            case VK_F3:
                if (first_player_turn)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    show_grid2 = false;
                    break;
                }
                if (!show_grid2)show_grid2 = true;
                else show_grid2 = false;
                break;

            case VK_ESCAPE:
                erase_current_ship = true;
                break;
            }
        }
        break;

    case WM_LBUTTONDOWN:
        if (HIWORD(lParam) <= 50)
        {
            if (LOWORD(lParam) >= b1Rect.left && LOWORD(lParam) <= b1Rect.right)
            {
                if (first_player_turn)
                {
                    if (name1_set)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Името вече е зададено !", L"Грешка !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                        break;
                    }
                    if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                    if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &DlgProc) == IDOK)name1_set = true;
                    break;
                }
                else
                {
                    if (name2_set)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Името вече е зададено !", L"Грешка !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                        break;
                    }
                    if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                    if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &DlgProc) == IDOK)name2_set = true;
                    break;
                }
            }
            if (LOWORD(lParam) >= b2Rect.left && LOWORD(lParam) <= b2Rect.right)
            {
                mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);

                if (sound)
                {
                    sound = false;
                    PlaySound(NULL, NULL, NULL);
                    break;
                }
                else
                {
                    sound = true;
                    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);
                    break;
                }
            }
            if (LOWORD(lParam) >= b3Rect.left && LOWORD(lParam) <= b3Rect.right)
            {
                Draw->BeginDraw();
                Draw->DrawBitmap(bmpField[FieldFrame.GetFrame()], D2D1::RectF(0, 50.0f, scr_width, scr_height));
                if (bigText && hgltBrush)Draw->DrawTextW(L"СЛЕДВАЩ ХОД !", 14, bigText, D2D1::RectF(scr_width / 2 - 200.0f,
                    scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
                Draw->EndDraw();
                if (first_player_turn)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\playertwo.wav", NULL, NULL, NULL);
                    first_player_turn = false;
                    show_grid1 = false;
                }
                else
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\playerone.wav", NULL, NULL, NULL);
                    first_player_turn = true;
                    show_grid2 = false;
                }
                Sleep(2000);
            }
        }
        else
        {
            if (!player1_set || !player2_set)
            {
                if (first_player_turn)
                {
                    if (player1_set)
                    {
                        if (sound)mciSendString(L"play.\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Вече си поставил своя флот !", L"Смяна на играча !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                        break;
                    }

                    if (min_selected)
                    {
                        
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale), 
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid1->GetTileDims(grid1->GetTileNumber(f_cursor)) };
                        
                        if (grid1->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        
                        vPl1Ships.push_back(dll::ShipFactory(dll::ships::min_ship,&current_tile,1,dll::dirs::hor,*grid1));
                        pl1_min_deployed = true;
                        min_selected = false;

                        if (pl1_min_deployed && pl1_small_deployed && pl1_mid_deployed && pl1_big_deployed)
                        {
                            player1_set = true;
                            break;
                        }
                        break;
                    }
                    else if (small_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid1->GetTileDims(grid1->GetTileNumber(f_cursor)) };

                        if (grid1->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[2]{};

                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 2; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !", 
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl1Ships.push_back(dll::ShipFactory(dll::ships::small_ship, temp_ship, 2, dll::dirs::hor, *grid1));
                            else 
                                vPl1Ships.push_back(dll::ShipFactory(dll::ships::small_ship, temp_ship, 2, dll::dirs::vert, *grid1));
                            small_selected = false;
                            pl1_small_deployed = true;
                            if (pl1_min_deployed && pl1_small_deployed && pl1_mid_deployed && pl1_big_deployed)
                            {
                                player1_set = true;
                                break;
                            }
                            break;
                        }
                    }
                    else if (mid_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid1->GetTileDims(grid1->GetTileNumber(f_cursor)) };

                        if (grid1->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[3]{};
                        
                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 3; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[1].state == dll::content::free)
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }
                            
                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[2].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[1].row == current_tile.row)
                            {
                                if (temp_ship[1].col == current_tile.col || abs(temp_ship[1].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[1].col == current_tile.col)
                            {
                                if (temp_ship[1].row == current_tile.row || abs(temp_ship[1].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[2] = current_tile;
                            temp_ship[2].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;

                            int rand_type = Randerer(1, 2);

                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl1Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 3, dll::dirs::hor, *grid1));
                            else 
                                vPl1Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type), 
                                    temp_ship, 3, dll::dirs::vert, *grid1));
                            mid_selected = false;
                            pl1_mid_deployed = true;
                            if (pl1_min_deployed && pl1_small_deployed && pl1_mid_deployed && pl1_big_deployed)
                            {
                                player1_set = true;
                                break;
                            }
                            break;
                        }
                    }
                    else if (big_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid1->GetTileDims(grid1->GetTileNumber(f_cursor)) };

                        if (grid1->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[4]{};

                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 4; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[1].state == dll::content::free)
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[2].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[1].row == current_tile.row)
                            {
                                if (temp_ship[1].col == current_tile.col || abs(temp_ship[1].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[1].col == current_tile.col)
                            {
                                if (temp_ship[1].row == current_tile.row || abs(temp_ship[1].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            
                            if (temp_ship[1].row != current_tile.row && temp_ship[1].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[2] = current_tile;
                            temp_ship[2].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;
                        }
                        else if (temp_ship[3].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[2].row == current_tile.row)
                            {
                                if (temp_ship[2].col == current_tile.col || abs(temp_ship[2].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[2].col == current_tile.col)
                            {
                                if (temp_ship[2].row == current_tile.row || abs(temp_ship[2].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            if (temp_ship[2].row != current_tile.row && temp_ship[2].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[3] = current_tile;
                            temp_ship[3].state = dll::content::used;
                            grid1->grid[current_tile.col][current_tile.row].state = dll::content::used;

                            int rand_type = Randerer(3, 4);

                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl1Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 4, dll::dirs::hor, *grid1));
                            else
                                vPl1Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 4, dll::dirs::vert, *grid1));
                            big_selected = false;
                            pl1_big_deployed = true;
                            if (pl1_min_deployed && pl1_small_deployed && pl1_mid_deployed && pl1_big_deployed)
                            {
                                player1_set = true;
                                break;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    if (player2_set)
                    {
                        if (sound)mciSendString(L"play.\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Вече си поставил своя флот !", L"Смяна на играча !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                        break;
                    }

                    if (min_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                        if (grid2->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }

                        vPl2Ships.push_back(dll::ShipFactory(dll::ships::min_ship, &current_tile, 1, dll::dirs::hor, *grid2));
                        pl2_min_deployed = 1;
                        min_selected = false;
                        if (pl2_min_deployed && pl2_small_deployed && pl2_mid_deployed && pl2_big_deployed)
                        {
                            player2_set = true;
                            break;
                        }
                        break;
                    }
                    else if (small_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                        if (grid2->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[2]{};

                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 2; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl2Ships.push_back(dll::ShipFactory(dll::ships::small_ship, temp_ship, 2, dll::dirs::hor, *grid2));
                            else
                                vPl2Ships.push_back(dll::ShipFactory(dll::ships::small_ship, temp_ship, 2, dll::dirs::vert, *grid2));
                            small_selected = false;
                            pl2_small_deployed = true;
                            if (pl2_min_deployed && pl2_small_deployed && pl2_mid_deployed && pl2_big_deployed)
                            {
                                player2_set = true;
                                break;
                            }
                            break;
                        }
                    }
                    else if (mid_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                        if (grid2->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[3]{};

                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 3; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[1].state == dll::content::free)
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[2].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[1].row == current_tile.row)
                            {
                                if (temp_ship[1].col == current_tile.col || abs(temp_ship[1].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[1].col == current_tile.col)
                            {
                                if (temp_ship[1].row == current_tile.row || abs(temp_ship[1].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[1].row != current_tile.row && temp_ship[1].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[2] = current_tile;
                            temp_ship[2].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;

                            int rand_type = Randerer(1, 2);

                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl2Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 3, dll::dirs::hor, *grid2));
                            else
                                vPl2Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 3, dll::dirs::vert, *grid2));
                            mid_selected = false;
                            pl2_mid_deployed = true;
                            if (pl2_min_deployed && pl2_small_deployed && pl2_mid_deployed && pl2_big_deployed)
                            {
                                player2_set = true;
                                break;
                            }
                            break;
                        }
                    }
                    else if (big_selected)
                    {
                        dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                        dll::TILE current_tile{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                        if (grid2->grid[current_tile.col][current_tile.row].state != dll::content::free)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            MessageBox(hwnd, L"Неправилно избрана позиция !",
                                L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                            break;
                        }
                        static dll::TILE temp_ship[4]{};

                        if (erase_current_ship)
                        {
                            erase_current_ship = false;
                            for (int i = 0; i < 4; ++i)
                            {
                                if (temp_ship[i].state != dll::content::free)
                                    grid1->grid[temp_ship[i].col][temp_ship[i].row].state = dll::content::free;
                                temp_ship[i].state = dll::content::free;
                            }
                        }

                        if (temp_ship[0].state == dll::content::free)
                        {
                            temp_ship[0] = current_tile;
                            temp_ship[0].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[1].state == dll::content::free)
                        {
                            if (temp_ship[0].row == current_tile.row)
                            {
                                if (temp_ship[0].col == current_tile.col || abs(temp_ship[0].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[0].col == current_tile.col)
                            {
                                if (temp_ship[0].row == current_tile.row || abs(temp_ship[0].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }

                            if (temp_ship[0].row != current_tile.row && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }
                            
                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }


                            temp_ship[1] = current_tile;
                            temp_ship[1].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                            break;
                        }
                        else if (temp_ship[2].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[1].row == current_tile.row)
                            {
                                if (temp_ship[1].col == current_tile.col || abs(temp_ship[1].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[1].col == current_tile.col)
                            {
                                if (temp_ship[1].row == current_tile.row || abs(temp_ship[1].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            if (temp_ship[1].row != current_tile.row && temp_ship[1].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[2] = current_tile;
                            temp_ship[2].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;
                        }
                        else if (temp_ship[3].state == dll::content::free)
                        {
                            if (temp_ship[0].row == temp_ship[1].row && temp_ship[0].row != current_tile.row)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }
                            else if (temp_ship[0].col == temp_ship[1].col && temp_ship[0].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;

                            }

                            if (temp_ship[2].row == current_tile.row)
                            {
                                if (temp_ship[2].col == current_tile.col || abs(temp_ship[2].col - current_tile.col) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            else if (temp_ship[2].col == current_tile.col)
                            {
                                if (temp_ship[2].row == current_tile.row || abs(temp_ship[2].row - current_tile.row) > 1)
                                {
                                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Неправилно избрана позиция !",
                                        L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                    break;
                                }
                            }
                            
                            if (temp_ship[2].row != current_tile.row && temp_ship[2].col != current_tile.col)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            if (current_tile.state != dll::content::free)
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                                MessageBox(hwnd, L"Неправилно избрана позиция !",
                                    L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                                break;
                            }

                            temp_ship[3] = current_tile;
                            temp_ship[3].state = dll::content::used;
                            grid2->grid[current_tile.col][current_tile.row].state = dll::content::used;

                            int rand_type = Randerer(3, 4);

                            if (temp_ship[0].row == temp_ship[1].row)
                                vPl2Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 4, dll::dirs::hor, *grid2));
                            else
                                vPl2Ships.push_back(dll::ShipFactory(static_cast<dll::ships>(rand_type),
                                    temp_ship, 4, dll::dirs::vert, *grid2));
                            big_selected = false;
                            pl2_big_deployed = true;
                            if (pl2_min_deployed && pl2_small_deployed && pl2_mid_deployed && pl2_big_deployed)
                            {
                                player2_set = true;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                if (first_player_turn)
                {
                    if (first_player_shoot)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Вече стреля през този ход !", L"Край на хода !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }

                    dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                    dll::TILE target{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                    first_player_shoot = true;
                    second_player_shoot = false;

                    ++turn_count;

                    if (!vPl2Ships.empty() &&
                        grid2->grid[target.col][target.row].state != dll::content::fire
                        && grid2->grid[target.col][target.row].state != dll::content::explosion)
                    {
                        attack_grid1->grid[target.col][target.row].state = dll::content::fire;

                        for (int i = 0; i < vPl2Ships.size(); ++i)
                        {
                            if (vPl2Ships[i]->get_type() == dll::ships::min_ship)
                            {
                                if (vPl2Ships[i]->ship_tile->number == target.number)
                                {
                                    vPl2Ships[i]->hit_ship(0);
                                    vPl2Ships[i]->ship_tile->state = dll::content::explosion;
                                    grid2->grid[target.col][target.row].state = dll::content::explosion;

                                    score1 += 50;

                                    attack_grid1->grid[target.col][target.row].state = dll::content::explosion;

                                    FRAMEBMP an_explosion{};
                                    an_explosion.max_delay = 3;
                                    an_explosion.max_frames = 25;
                                    an_explosion.sx = attack_grid1->grid[target.col][target.row].start.x;
                                    an_explosion.sy = attack_grid1->grid[target.col][target.row].start.y;
                                    an_explosion.ex = attack_grid1->grid[target.col][target.row].end.x;
                                    an_explosion.ey = attack_grid1->grid[target.col][target.row].end.y;
                                    vExplosions1.push_back(an_explosion);

                                    vPl2Ships.erase(vPl2Ships.begin() + i);

                                    if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                        MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);

                                    break;
                                }
                            }
                            else if (vPl2Ships[i]->get_type() == dll::ships::small_ship)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 2; ++k)
                                {
                                    if (vPl2Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl2Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl2Ships[i]->hit_ship(k);
                                        vPl2Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid2->grid[target.col][target.row].state = dll::content::explosion;

                                        if (vPl2Ships[i]->ship_healt() <= 0)
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl2Ships.erase(vPl2Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid1->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid1->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid1->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid1->grid[target.col][target.row].end.y;
                                        vExplosions1.push_back(an_explosion);

                                        score1 += 50;

                                        on_target = true;

                                        attack_grid1->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                            else if (vPl2Ships[i]->get_type() == dll::ships::mid_ship1
                                || vPl2Ships[i]->get_type() == dll::ships::mid_ship2)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 3; ++k)
                                {
                                    if (vPl2Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl2Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl2Ships[i]->hit_ship(k);
                                        vPl2Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid2->grid[target.col][target.row].state = dll::content::explosion;


                                        if (vPl2Ships[i]->ship_healt() <= 0) 
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl2Ships.erase(vPl2Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid1->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid1->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid1->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid1->grid[target.col][target.row].end.y;
                                        vExplosions1.push_back(an_explosion);

                                        score1 += 50;

                                        on_target = true;

                                        attack_grid1->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                            else if (vPl2Ships[i]->get_type() == dll::ships::big_ship1
                                || vPl2Ships[i]->get_type() == dll::ships::big_ship2)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 4; ++k)
                                {
                                    if (vPl2Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl2Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl2Ships[i]->hit_ship(k);
                                        vPl2Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid2->grid[target.col][target.row].state = dll::content::explosion;

                                        if (vPl2Ships[i]->ship_healt() <= 0) 
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl2Ships.erase(vPl2Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid1->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid1->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid1->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid1->grid[target.col][target.row].end.y;
                                        vExplosions1.push_back(an_explosion);

                                        score1 += 50;

                                        on_target = true;

                                        attack_grid1->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                        }
                    }
                    
                    if (sound)mciSendString(L"play .\\res\\snd\\missed.wav", NULL, NULL, NULL);
                }
                else
                {
                    if (second_player_shoot)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        MessageBox(hwnd, L"Вече стреля през този ход !", L"Край на хода !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                        break;
                    }

                    dll::FPOINT f_cursor{ (float)(LOWORD(lParam) * render_target_x_scale),
                            (float)(HIWORD(lParam) * render_target_y_scale) };

                    dll::TILE target{ grid2->GetTileDims(grid2->GetTileNumber(f_cursor)) };

                    first_player_shoot = false;
                    second_player_shoot = true;

                    if (!vPl1Ships.empty() &&
                        grid1->grid[target.col][target.row].state != dll::content::fire
                        && grid1->grid[target.col][target.row].state != dll::content::explosion)
                    {
                        attack_grid2->grid[target.col][target.row].state = dll::content::fire;

                        for (int i = 0; i < vPl1Ships.size(); ++i)
                        {
                            if (vPl1Ships[i]->get_type() == dll::ships::min_ship)
                            {
                                if (vPl1Ships[i]->ship_tile->number == target.number)
                                {
                                    vPl1Ships[i]->hit_ship(0);
                                    vPl1Ships[i]->ship_tile->state = dll::content::explosion;
                                    grid1->grid[target.col][target.row].state = dll::content::explosion;

                                    score2 += 50;

                                    attack_grid2->grid[target.col][target.row].state = dll::content::explosion;

                                    FRAMEBMP an_explosion{};
                                    an_explosion.max_delay = 3;
                                    an_explosion.max_frames = 25;
                                    an_explosion.sx = attack_grid2->grid[target.col][target.row].start.x;
                                    an_explosion.sy = attack_grid2->grid[target.col][target.row].start.y;
                                    an_explosion.ex = attack_grid2->grid[target.col][target.row].end.x;
                                    an_explosion.ey = attack_grid2->grid[target.col][target.row].end.y;
                                    vExplosions2.push_back(an_explosion);

                                    vPl1Ships.erase(vPl1Ships.begin() + i);

                                    if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                    MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                        MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);

                                    break;
                                }
                            }
                            else if (vPl1Ships[i]->get_type() == dll::ships::small_ship)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 2; ++k)
                                {
                                    if (vPl1Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl1Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl1Ships[i]->hit_ship(k);
                                        vPl1Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid1->grid[target.col][target.row].state = dll::content::explosion;

                                        if (vPl1Ships[i]->ship_healt() <= 0) 
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl1Ships.erase(vPl1Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid2->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid2->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid2->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid2->grid[target.col][target.row].end.y;
                                        vExplosions2.push_back(an_explosion);

                                        score2 += 50;

                                        on_target = true;

                                        attack_grid2->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                            else if (vPl1Ships[i]->get_type() == dll::ships::mid_ship1
                                || vPl1Ships[i]->get_type() == dll::ships::mid_ship2)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 3; ++k)
                                {
                                    if (vPl1Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl1Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl1Ships[i]->hit_ship(k);
                                        vPl1Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid1->grid[target.col][target.row].state = dll::content::explosion;


                                        if (vPl1Ships[i]->ship_healt() <= 0)
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl1Ships.erase(vPl1Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid2->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid2->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid2->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid2->grid[target.col][target.row].end.y;
                                        vExplosions2.push_back(an_explosion);

                                        score2 += 50;

                                        on_target = true;

                                        attack_grid2->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                            else if (vPl1Ships[i]->get_type() == dll::ships::big_ship1
                                || vPl1Ships[i]->get_type() == dll::ships::big_ship2)
                            {
                                bool on_target = false;

                                for (int k = 0; k < 4; ++k)
                                {
                                    if (vPl1Ships[i]->ship_tile[k].number == target.number)
                                    {
                                        if (vPl1Ships[i]->ship_tile[k].state != dll::content::used)break;
                                        vPl1Ships[i]->hit_ship(k);
                                        vPl1Ships[i]->ship_tile[k].state = dll::content::explosion;
                                        grid1->grid[target.col][target.row].state = dll::content::explosion;

                                        if (vPl1Ships[i]->ship_healt() <= 0)
                                        {
                                            if (sound)mciSendString(L"play .\\res\\snd\\destroyed.wav", NULL, NULL, NULL);
                                            MessageBox(hwnd, L"Потопи този кораб !", L"Точен мерник !",
                                                MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
                                            vPl1Ships.erase(vPl1Ships.begin() + i);
                                        }
                                        else if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

                                        FRAMEBMP an_explosion{};
                                        an_explosion.max_delay = 3;
                                        an_explosion.max_frames = 25;
                                        an_explosion.sx = attack_grid2->grid[target.col][target.row].start.x;
                                        an_explosion.sy = attack_grid2->grid[target.col][target.row].start.y;
                                        an_explosion.ex = attack_grid2->grid[target.col][target.row].end.x;
                                        an_explosion.ey = attack_grid2->grid[target.col][target.row].end.y;
                                        vExplosions2.push_back(an_explosion);

                                        score2 += 50;

                                        on_target = true;

                                        attack_grid2->grid[target.col][target.row].state = dll::content::explosion;

                                        break;
                                    }
                                }

                                if (on_target)break;
                            }
                        }
                    }
                    
                    if (sound)mciSendString(L"play .\\res\\snd\\missed.wav", NULL, NULL, NULL);
                }
            }
        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream strt(Ltmp_file);
        strt << L"Game started at: " << std::chrono::system_clock::now();
        strt.close();
    }

    int winx = GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f);

    if (GetSystemMetrics(SM_CXSCREEN) < winx + (int)(scr_width) ||
        GetSystemMetrics(SM_CYSCREEN) < 10 + (int)(scr_height))ErrExit(eScreen);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);
    mainCur = LoadCursorFromFile(L".\\res\\main.ani");
    outCur = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCur || !outCur)ErrExit(eCursor);

    
    bWinClass.lpszClassName = bWinClassName;
    bWinClass.hInstance = bIns;
    bWinClass.lpfnWndProc = &WinProc;
    bWinClass.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
    bWinClass.hCursor = mainCur;
    bWinClass.hIcon = mainIcon;
    bWinClass.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWinClass))ErrExit(eClass);

    bHwnd = CreateWindow(bWinClassName, L"МОРСКА БИТКА !", WS_CAPTION | WS_SYSMENU, winx, 10, (int)(scr_width), (int)(scr_height),
        NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1 Factory !");
            ErrExit(eD2D);
        }

        if (iFactory)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
                D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1 HwndRenderTarget !");
                ErrExit(eD2D);
            }

            if (Draw)
            {
                RECT clRect{};
                GetClientRect(bHwnd, &clRect);

                D2D1_SIZE_F DrawSize{ Draw->GetSize() };

                render_target_x_scale = DrawSize.width / (clRect.right - clRect.left);
                render_target_y_scale = DrawSize.height / (clRect.bottom - clRect.top);

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Moccasin), &statBckgBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating statBckgBrush !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &txtBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating hgltBrush !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &hgltBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating hgltBrush !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateGray), &inactBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating inactBrush !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGreen), &GreenBoundBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating GreenBoundBrush !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &RedBoundBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating redBoundBrush !");
                    ErrExit(eD2D);
                }

                D2D1_GRADIENT_STOP gSt[2]{};
                ID2D1GradientStopCollection* gColl{ nullptr };

                gSt[0].position = 0;
                gSt[0].color = D2D1::ColorF(D2D1::ColorF::DarkKhaki);
                gSt[1].position = 1.0f;
                gSt[1].color = D2D1::ColorF(D2D1::ColorF::OliveDrab);

                hr = Draw->CreateGradientStopCollection(gSt, 2, &gColl);
                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 GradientStopCollection !");
                    ErrExit(eD2D);
                }

                if (gColl)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left +
                        (b1Rect.right - b1Rect.left) / 2), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2, 20.0f),
                        gColl, &b1BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left +
                        (b2Rect.right - b2Rect.left) / 2), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2, 20.0f),
                        gColl, &b2BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left +
                        (b3Rect.right - b3Rect.left) / 2), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2, 20.0f),
                        gColl, &b3BckgBrush);

                    if (hr != S_OK)
                    {
                        LogError(L"Error creating D2D1 buttons background brushes !");
                        ErrExit(eD2D);
                    }

                    ClrHeap(&gColl);
                }

                //////////////////////////////////////////////////////////////////

                bmpMinShip = Load(L".\\res\\img\\ships\\min.png", Draw);
                if (!bmpMinShip)
                {
                    LogError(L"Error loading bmpMinShip !");
                    ErrExit(eD2D);
                }

                bmpSmallHShip = Load(L".\\res\\img\\ships\\small_h.png", Draw);
                if (!bmpSmallHShip)
                {
                    LogError(L"Error loading bmpSmallHShip !");
                    ErrExit(eD2D);
                }
                bmpSmallVShip = Load(L".\\res\\img\\ships\\small_v.png", Draw);
                if (!bmpSmallVShip)
                {
                    LogError(L"Error loading bmpSmallVShip !");
                    ErrExit(eD2D);
                }

                bmpMid1HShip = Load(L".\\res\\img\\ships\\mid1_h.png", Draw);
                if (!bmpMid1HShip)
                {
                    LogError(L"Error loading bmpMid1HShip !");
                    ErrExit(eD2D);
                }
                bmpMid1VShip = Load(L".\\res\\img\\ships\\mid1_v.png", Draw);
                if (!bmpMid1VShip)
                {
                    LogError(L"Error loading bmpMid1VShip !");
                    ErrExit(eD2D);
                }

                bmpMid2HShip = Load(L".\\res\\img\\ships\\mid2_h.png", Draw);
                if (!bmpMid2HShip)
                {
                    LogError(L"Error loading bmpMid2HShip !");
                    ErrExit(eD2D);
                }
                bmpMid2VShip = Load(L".\\res\\img\\ships\\mid2_v.png", Draw);
                if (!bmpMid2VShip)
                {
                    LogError(L"Error loading bmpMid2VShip !");
                    ErrExit(eD2D);
                }

                bmpBig1HShip = Load(L".\\res\\img\\ships\\big1_h.png", Draw);
                if (!bmpBig1HShip)
                {
                    LogError(L"Error loading bmpBig1HShip !");
                    ErrExit(eD2D);
                }
                bmpBig1VShip = Load(L".\\res\\img\\ships\\big1_v.png", Draw);
                if (!bmpBig1VShip)
                {
                    LogError(L"Error loading bmpBig1VShip !");
                    ErrExit(eD2D);
                }

                bmpBig2HShip = Load(L".\\res\\img\\ships\\big2_h.png", Draw);
                if (!bmpBig2HShip)
                {
                    LogError(L"Error loading bmpBig2HShip !");
                    ErrExit(eD2D);
                }
                bmpBig2VShip = Load(L".\\res\\img\\ships\\big2_v.png", Draw);
                if (!bmpBig2VShip)
                {
                    LogError(L"Error loading bmpBig2VShip !");
                    ErrExit(eD2D);
                }

                bmpOnTarget = Load(L".\\res\\img\\ontarget.png", Draw);
                if (!bmpOnTarget)
                {
                    LogError(L"Error loading bmpOnTarget !");
                    ErrExit(eD2D);
                }

                bmpMissed = Load(L".\\res\\img\\missed.png", Draw);
                if (!bmpMissed)
                {
                    LogError(L"Error loading bmpMissed !");
                    ErrExit(eD2D);
                }

                for (int i = 0; i < 5; ++i)
                {
                    wchar_t name[75] = L".\\res\\img\\field\\";
                    wchar_t add[5] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpField[i] = Load(name, Draw);

                    if (!bmpField[i])
                    {
                        LogError(L"Error loading bmpField !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 26; ++i)
                {
                    wchar_t name[75] = L".\\res\\img\\explosion\\";
                    wchar_t add[5] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpExplosion[i] = Load(name, Draw);

                    if (!bmpExplosion[i])
                    {
                        LogError(L"Error loading bmpExplosion !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 19; ++i)
                {
                    wchar_t name[75] = L".\\res\\img\\intro\\";
                    wchar_t add[5] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpIntro[i] = Load(name, Draw);

                    if (!bmpIntro[i])
                    {
                        LogError(L"Error loading bmpIntro !");
                        ErrExit(eD2D);
                    }
                }
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1 iWriteFactory !");
            ErrExit(eD2D);
        }

        if (iWriteFactory)
        {
            hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 16, L"", &nrmText);
            hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 24, L"", &midText);
            hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 72, L"", &bigText);

            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1 TextFormats !");
                ErrExit(eD2D);
            }
        }
    }

    if (Draw && bigText && hgltBrush)
    {
        wchar_t up_txt[15]{ L"МОРСКА БИТКА !" };
        wchar_t down_txt[14]{ L"dev. Daniel !" };

        D2D1_RECT_F upRect{ 250.0f, 0, scr_width, 50.0f };
        D2D1_RECT_F downRect{ 350.0f, scr_height, scr_width, scr_height + 50.0f };

        bool up_ok = false;
        bool down_ok = false;

        mciSendString(L"play .\\res\\snd\\entry.wav", NULL, NULL, NULL);

        while (!up_ok)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawText(up_txt, 15, bigText, upRect, hgltBrush);
            Draw->EndDraw();

            upRect.top += 2.5f;
            upRect.bottom += 2.5f;
            if (upRect.top >= scr_height / 2 - 100.0f)
            {
                mciSendString(L"play .\\res\\snd\\entry.wav", NULL, NULL, NULL);
                up_ok = true;
            }
        }
        while (!down_ok)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawText(up_txt, 15, bigText, upRect, hgltBrush);
            Draw->DrawText(down_txt, 14, bigText, downRect, hgltBrush);
            Draw->EndDraw();

            downRect.top -= 2.5f;
            downRect.bottom -= 2.5f;
            if (downRect.top <= scr_height / 2 + 100.0f) down_ok = true;

        }

        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawText(up_txt, 15, bigText, upRect, hgltBrush);
        Draw->DrawText(down_txt, 14, bigText, downRect, hgltBrush);
        Draw->EndDraw();

        PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_SYNC);
    }

    mciSendString(L"play .\\res\\snd\\playerone.wav", NULL, NULL, NULL);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);

            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            if (hgltBrush && bigText)
                Draw->DrawTextW(L"ПАУЗА", 6, bigText,
                    D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            continue;
        }
    
    /////////////////////////////////////////////

        if (player1_set && player2_set)
        {
            if (vPl1Ships.empty())second_player_win = true;
            if (vPl2Ships.empty())first_player_win = true;

            if (first_player_win || second_player_win)
            {
                Draw->EndDraw();
                GameOver();
            }
        }


    // DRAW THINGS ******************************

        Draw->BeginDraw();

        if (nrmText && statBckgBrush && b1BckgBrush && b2BckgBrush && b3BckgBrush && txtBrush && hgltBrush && inactBrush)
        {
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, scr_height), statBckgBrush);
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, (b1Rect.right - b1Rect.left) / 2, 20.0f), b1BckgBrush);
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, (b2Rect.right - b2Rect.left) / 2, 20.0f), b2BckgBrush);
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, (b3Rect.right - b3Rect.left) / 2, 20.0f), b3BckgBrush);

            if (first_player_turn)
            {
                if (name1_set)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, inactBrush);
                else
                {
                    if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, txtBrush);
                    else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, hgltBrush);
                }
            }
            else
            {
                if (name2_set)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, inactBrush);
                else
                {
                    if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, txtBrush);
                    else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, hgltBrush);
                }
            }

            if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, txtBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, hgltBrush);

            if (!b3Hglt)Draw->DrawTextW(L"СЛЕДВАЩ ХОД", 12, nrmText, b3TxtRect, txtBrush);
            else Draw->DrawTextW(L"СЛЕДВАЩ ХОД", 12, nrmText, b3TxtRect, hgltBrush);
        }

        Draw->DrawBitmap(bmpField[FieldFrame.GetFrame()], D2D1::RectF(0, 50.0f, scr_width, scr_height));

        /////////////////////////////////////////

        if ((!player1_set || !player2_set) && nrmText && txtBrush)
            Draw->DrawTextW(L"ПОСТАВИ КОРАБ: ", 16, nrmText, D2D1::RectF(905.0f, scr_height / 2 - 200.0f, scr_width,
                scr_height / 2 - 100.0f), txtBrush);
        else if (nrmText && txtBrush)
        {
            wchar_t play_txt[30]{ L"ХОД: " };
            wchar_t add[5]{ L"\0" };
            wsprintf(add, L"%d", turn_count);

            wcscat_s(play_txt, add);
            int pl_size = 0;

            for (int i = 0; i < 30; ++i)
            {
                if (play_txt[i] != '\0')++pl_size;
                else break;
            }

            Draw->DrawTextW(play_txt, pl_size, nrmText, D2D1::RectF(940.0f, 300.0f, scr_width, scr_height), txtBrush);
        }

        if (first_player_turn)
        {
            wchar_t play_txt[30]{ L"КОМАНДИР: " };
            wcscat_s(play_txt, player1);
            int pl_size = 0;
            
            for (int i = 0; i < 30; ++i)
            {
                if (play_txt[i] != '\0')++pl_size;
                else break;
            }

            Draw->DrawTextW(play_txt, pl_size, nrmText, D2D1::RectF(905.0f, 60.0f, scr_width, 200.0f), txtBrush);
        }
        else
        {
            wchar_t play_txt[30]{ L"КОМАНДИР: " };
            wcscat_s(play_txt, player2);
            int pl_size = 0;

            for (int i = 0; i < 30; ++i)
            {
                if (play_txt[i] != '\0')++pl_size;
                else break;
            }

            if (nrmText && txtBrush)
                Draw->DrawTextW(play_txt, pl_size, nrmText, D2D1::RectF(905.0f, 60.0f, scr_width, 200.0f), txtBrush);
        }

        
        if (erase_current_ship)
        {
            D2D1_SIZE_F ship_size = bmpMissed->GetSize();
            Draw->DrawBitmap(bmpMissed, D2D1::RectF(950.0f, scr_height / 2 - 25.0f, 950.0f + ship_size.width,
                scr_height / 2 - 25.0f + ship_size.height));
        }
        else
        {
            if (min_selected)
            {
                D2D1_SIZE_F ship_size = bmpMinShip->GetSize();
                Draw->DrawBitmap(bmpMinShip, D2D1::RectF(950.0f, scr_height / 2 - 25.0f, 950.0f + ship_size.width,
                    scr_height / 2 - 25.0f + ship_size.height));
            }
            if (small_selected)
            {
                D2D1_SIZE_F ship_size = bmpSmallVShip->GetSize();
                Draw->DrawBitmap(bmpSmallVShip, D2D1::RectF(950.0f, scr_height / 2 - 25.0f, 950.0f + ship_size.width,
                    scr_height / 2 - 25.0f + ship_size.height));
            }
            if (mid_selected)
            {
                D2D1_SIZE_F ship_size = bmpMid1VShip->GetSize();
                Draw->DrawBitmap(bmpMid1VShip, D2D1::RectF(950.0f, scr_height / 2 - 25.0f, 950.0f + ship_size.width,
                    scr_height / 2 - 25.0f + ship_size.height));
            }
            if (big_selected)
            {
                D2D1_SIZE_F ship_size = bmpBig1VShip->GetSize();
                Draw->DrawBitmap(bmpMinShip, D2D1::RectF(950.0f, scr_height / 2 - 25.0f, 950.0f + ship_size.width,
                    scr_height / 2 - 25.0f + ship_size.height));
            }
        }

        if (first_player_turn)
        {
            if (!player1_set || show_grid1)
            {
                for (int cols = 0; cols < MAX_COLS; ++cols)
                {
                    for (int rows = 0; rows < MAX_ROWS; ++rows)
                    {
                        if (grid1->grid[cols][rows].state == dll::content::free)
                            Draw->DrawRectangle(RectBound(grid1->grid[cols][rows]), GreenBoundBrush);
                        else Draw->DrawRectangle(RectBound(grid1->grid[cols][rows]), RedBoundBrush);
                    }
                }

                if (!vPl1Ships.empty())
                {
                    for (std::vector<dll::Ship>::iterator ship = vPl1Ships.begin(); ship < vPl1Ships.end(); ++ship)
                    {
                        switch ((*ship)->get_type())
                        {
                        case dll::ships::min_ship:
                            Draw->DrawBitmap(bmpMinShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                                (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                            break;

                        case dll::ships::small_ship:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpSmallHShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[1].end.x, (*ship)->ship_tile[1].end.y));
                            else
                                Draw->DrawBitmap(bmpSmallVShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[1].end.x, (*ship)->ship_tile[1].end.y));
                            break;

                        case dll::ships::mid_ship1:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpMid1HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            else
                                Draw->DrawBitmap(bmpMid1VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            break;

                        case dll::ships::mid_ship2:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpMid2HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            else
                                Draw->DrawBitmap(bmpMid2VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            break;

                        case dll::ships::big_ship1:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpBig1HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            else
                                Draw->DrawBitmap(bmpBig1VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            break;

                        case dll::ships::big_ship2:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpBig2HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            else
                                Draw->DrawBitmap(bmpBig2VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            break;

                        }
                    }
                }
            }
        }
        else 
        {
            if (!player2_set || show_grid2)
            {
                for (int cols = 0; cols < MAX_COLS; ++cols)
                {
                    for (int rows = 0; rows < MAX_ROWS; ++rows)
                    {
                        if (grid2->grid[cols][rows].state == dll::content::free)
                            Draw->DrawRectangle(RectBound(grid2->grid[cols][rows]), GreenBoundBrush);
                        else Draw->DrawRectangle(RectBound(grid2->grid[cols][rows]), RedBoundBrush);
                    }
                }

                if (!vPl2Ships.empty())
                {
                    for (std::vector<dll::Ship>::iterator ship = vPl2Ships.begin(); ship < vPl2Ships.end(); ++ship)
                    {
                        switch ((*ship)->get_type())
                        {
                        case dll::ships::min_ship:
                            Draw->DrawBitmap(bmpMinShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                                (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                            break;

                        case dll::ships::small_ship:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpSmallHShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[1].end.x, (*ship)->ship_tile[1].end.y));
                            else
                                Draw->DrawBitmap(bmpSmallVShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[1].end.x, (*ship)->ship_tile[1].end.y));
                            break;

                        case dll::ships::mid_ship1:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpMid1HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            else
                                Draw->DrawBitmap(bmpMid1VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            break;

                        case dll::ships::mid_ship2:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpMid2HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            else
                                Draw->DrawBitmap(bmpMid2VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[2].end.x, (*ship)->ship_tile[2].end.y));
                            break;

                        case dll::ships::big_ship1:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpBig1HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            else
                                Draw->DrawBitmap(bmpBig1VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            break;

                        case dll::ships::big_ship2:
                            if ((*ship)->dir == dll::dirs::hor)
                                Draw->DrawBitmap(bmpBig2HShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            else
                                Draw->DrawBitmap(bmpBig2VShip, D2D1::RectF((*ship)->ship_tile[0].start.x, (*ship)->ship_tile[0].start.y,
                                    (*ship)->ship_tile[3].end.x, (*ship)->ship_tile[3].end.y));
                            break;

                        }
                    }
                }
            }
        }
        
        if (player1_set && player2_set && !show_grid1 && !show_grid2)
        {
            if (first_player_turn)
            {
                for (int cols = 0; cols < MAX_COLS; ++cols)
                {
                    for (int rows = 0; rows < MAX_ROWS; ++rows)
                    {
                        Draw->DrawRectangle(RectBound(attack_grid1->grid[cols][rows]), GreenBoundBrush);
                        
                        if (attack_grid1->grid[cols][rows].state == dll::content::fire)
                            Draw->DrawBitmap(bmpMissed, RectBound(attack_grid1->grid[cols][rows]));
                        
                        if (!vExplosions1.empty())
                        {
                            for (int i = 0; i < vExplosions1.size(); ++i)
                                Draw->DrawBitmap(bmpExplosion[vExplosions1[i].GetFrame()], D2D1::RectF(vExplosions1[i].sx,
                                    vExplosions1[i].sy, vExplosions1[i].ex, vExplosions1[i].ey));
                        }
                    }
                }

            }
            else
            {
                for (int cols = 0; cols < MAX_COLS; ++cols)
                {
                    for (int rows = 0; rows < MAX_ROWS; ++rows)
                    {
                        Draw->DrawRectangle(RectBound(attack_grid2->grid[cols][rows]), GreenBoundBrush);

                        if (attack_grid2->grid[cols][rows].state == dll::content::fire)
                            Draw->DrawBitmap(bmpMissed, RectBound(attack_grid2->grid[cols][rows]));

                        if (!vExplosions2.empty())
                        {
                            for (int i = 0; i < vExplosions2.size(); ++i)
                                Draw->DrawBitmap(bmpExplosion[vExplosions2[i].GetFrame()], D2D1::RectF(vExplosions2[i].sx,
                                    vExplosions2[i].sy, vExplosions2[i].ex, vExplosions2[i].ey));
                        }
                    }
                }
            }
        }

        /////////////////////////////////////////
        
        Draw->EndDraw();
    }

    std::remove(tmp_file);
    ReleaseResources();
    return (int) bMsg.wParam;
}