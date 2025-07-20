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

D2D1_RECT_F b1Rect{ 50.0f, 5.0f, scr_width / 3 - 50.0f, 45.0f };
D2D1_RECT_F b2Rect{ scr_width / 3 + 50.0f, 5.0f, scr_width * 2 / 3 - 50.0f, 45.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3 + 50.0f, 5.0f, scr_width - 50.0f, 45.0f };

D2D1_RECT_F b1TxtRect{ 90.0f, 10.0f, scr_width / 3 - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3 + 90.0f, 10.0f, scr_width * 2 / 3 - 50.0f, 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2 / 3 + 90.0f, 10.0f, scr_width - 50.0f, 40.0f };

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

bool first_player_turn = true;

int score1 = 0;
int score2 = 0;

bool min_selected = false;
bool small_selected = false;
bool mid_selected = false;
bool big_selected = false;

int pl1_min_deployed = 0;
int pl1_small_deployed = 0;
int pl1_mid_deployed = 0;
int pl1_big_deployed = 0;

int pl2_min_deployed = 0;
int pl2_small_deployed = 0;
int pl2_mid_deployed = 0;
int pl2_big_deployed = 0;

struct FRAMEBMP
{
    int delay = 0;
    int max_delay = 0;

    int frame = 0;
    int max_frames = 0;

    bool ended = false;

    int GetFrame()
    {
        delay--;
        ended = false;
        if (delay < 0)
        {
            delay = max_delay;
            ++frame;
            if (frame > max_frames)
            {
                frame = 0;
                ended = true;
            }
        }
        return frame;
    }
};

FRAMEBMP FieldFrame;
FRAMEBMP ExplosionFrame;
FRAMEBMP IntroFrame;

dll::GRID* grid1{ nullptr };
dll::GRID* grid2{ nullptr };

std::vector<dll::Ship>vPl1Ships;
std::vector<dll::Ship>vPl2Ships;

std::vector<dll::TILE> build_ship;

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
}
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


bool IsNear(dll::TILE myTile, dll::TILE checkTile)
{
    if (abs(myTile.col - checkTile.col) > 1 || abs(myTile.row - checkTile.row) > 1)return false;

    return true;
}
void GameOver()
{
    PlaySound(NULL, NULL, NULL);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    wcscpy_s(player1, L"МЪртинчУ");
    wcscpy_s(player2, L"ДаниелчУ");
    
    name1_set = false;
    name2_set = false;

    player1_set = false;
    player2_set = false;

    first_player_turn = true;

    score1 = 0;
    score2 = 0;

    min_selected = false;
    small_selected = false;
    mid_selected = false;
    big_selected = false;

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

    ExplosionFrame.delay = 3;
    ExplosionFrame.max_delay = 3;
    ExplosionFrame.max_frames = 25;

    //////////////////////////////////////////

    if (grid1)grid1->Release();
    grid1 = new dll::GRID();

    if (grid2)grid2->Release();
    grid2 = new dll::GRID();

    if (!vPl1Ships.empty())
        for (int i = 0; i < vPl1Ships.size(); ++i)vPl1Ships[i]->Release();
    vPl1Ships.clear();

    if (!vPl2Ships.empty())
        for (int i = 0; i < vPl2Ships.size(); ++i)vPl2Ships[i]->Release();
    vPl2Ships.clear();

    build_ship.clear();
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
        if (MessageBox(hwnd, L"Ако излезете, губите тази игра !\n\nНаистина ли излизате !",
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
    {
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако рестартирате, губите тази игра !\n\nНаистина ли рестартирате !",
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
    }
    break;

    case WM_KEYDOWN:
        if (min_selected || small_selected || mid_selected || big_selected)
        {
            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
            MessageBox(hwnd, L"Довърши кораба,който си избрал !", L"Вече има избран кораб !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
            break;
        }
        switch (wParam)
        {
        case key1:
            if (first_player_turn)
            {
                if (pl1_min_deployed >= 1)
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
                if (pl2_min_deployed >= 1)
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
                if (pl1_small_deployed >= 2)
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
                if (pl2_small_deployed >= 2)
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
                if (pl1_mid_deployed >= 3)
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
                if (pl2_mid_deployed >= 3)
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
                if (pl1_big_deployed >= 4)
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
                if (pl2_big_deployed >= 4)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    MessageBox(hwnd, L"Големият кораб вече е поставен !", L"Избери друго !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
                    break;
                }
                big_selected = true;
                break;
            }
            break;
        }
        break;


    case WM_LBUTTONDOWN:
        if (HIWORD(lParam) <= 50)
        {

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

                    dll::FPOINT f_cursor{ (float)(LOWORD(lParam)), (float)(HIWORD(lParam))};

                    dll::TILE current_tile{ grid1->GetTileDims(grid1->GetTileNumber(f_cursor)) };

                    if (current_tile.state != dll::content::free)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        break;
                    }
                    
                    if (min_selected)
                    {
                        vPl1Ships.push_back(dll::ShipFactory(dll::ships::min_ship,&current_tile,1,dll::dirs::hor,*grid1));
                        pl1_min_deployed = 1;
                        min_selected = false;
                        break;
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



                }
            }
            else
            {

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

        D2D1_RECT_F upRect{ 150.0f, 0, scr_width, 50.0f };
        D2D1_RECT_F downRect{ 200.0f, scr_height, scr_width, scr_height + 50.0f };

        bool up_ok = false;
        bool down_ok = false;

        mciSendString(L"play .\\res\\snd\\entry.wav", NULL, NULL, NULL);

        while (!up_ok)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawText(up_txt, 15, bigText, upRect, hgltBrush);
            Draw->EndDraw();

            upRect.top += 2.0f;
            upRect.bottom += 2.0f;
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

            downRect.top -= 2.0f;
            downRect.bottom -= 2.0f;
            if (downRect.top <= scr_height / 2 + 100.0f) down_ok = true;

        }

        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawText(up_txt, 15, bigText, upRect, hgltBrush);
        Draw->DrawText(down_txt, 14, bigText, downRect, hgltBrush);
        Draw->EndDraw();

        PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_SYNC);
    }
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
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[IntroFrame.GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
            if (hgltBrush && bigText)
                Draw->DrawTextW(L"ПАУЗА", 6, bigText,
                    D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, scr_height), hgltBrush);
            Draw->EndDraw();
            continue;
        }
    
    /////////////////////////////////////////////







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


        if (first_player_turn)
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
        }
        else
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
                        Draw->DrawBitmap(bmpSmallHShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    else
                        Draw->DrawBitmap(bmpSmallVShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    break;

                case dll::ships::mid_ship1:
                    if ((*ship)->dir == dll::dirs::hor)
                        Draw->DrawBitmap(bmpMid1HShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    else
                        Draw->DrawBitmap(bmpMid1VShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    break;

                case dll::ships::mid_ship2:
                    if ((*ship)->dir == dll::dirs::hor)
                        Draw->DrawBitmap(bmpMid2HShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    else
                        Draw->DrawBitmap(bmpMid2VShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    break;

                case dll::ships::big_ship1:
                    if ((*ship)->dir == dll::dirs::hor)
                        Draw->DrawBitmap(bmpBig1HShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    else
                        Draw->DrawBitmap(bmpBig1VShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    break;

                case dll::ships::big_ship2:
                    if ((*ship)->dir == dll::dirs::hor)
                        Draw->DrawBitmap(bmpBig2HShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    else
                        Draw->DrawBitmap(bmpBig2VShip, D2D1::RectF((*ship)->ship_tile->start.x, (*ship)->ship_tile->start.y,
                            (*ship)->ship_tile->end.x, (*ship)->ship_tile->end.y));
                    break;

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