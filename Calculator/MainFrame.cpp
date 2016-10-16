#include <windows.h>
#include <stdio.h>
#include "Calculate.h"
#include "resource.h"

//计算器的皮肤，在程序初始化时必须进行赋值

typedef struct SkinStruct {
	//窗口边界颜色
	COLORREF WINDOW_BORDER_COLOR;

	//计算器面板区域
	COLORREF BUTTON_AREA_BG_NOR;
	COLORREF BUTTON_AREA_BG_HOVER;
	COLORREF BUTTON_AREA_BG_DOWN;

	//显示器区域
	COLORREF DISPLAY_AREA_BG;

	//标题栏区域
	COLORREF BUTTON_CONTROL_COLOR_NOR;
	COLORREF BUTTON_CONTROL_COLOR_HOVER;
	COLORREF BUTTON_CONTROL_COLOR_DOWN;
	COLORREF BUTTON_CONTROL_CLOSE_COLOR_DOWN;
	COLORREF BUTTON_CONTROL_CLOSE_COLOR_HOVER;
}SkinStruct;
//一共有三套皮肤
#define SKIN_COUNT 3
SkinStruct Skin;


//窗口整体
#define WINDOW_WIDTH		320
#define WINDOW_HEIGHT		550

//计算器面板区域
#define BUTTON_ROWS		6
#define BUTTON_COLUMNS		5
#define BUTTON_HEIGHT		55//每个按钮高度

//显示器区域
#define DISPLAY_NUM_MAX_LEN	9	 //数字的最大长度
#define DISPLAY_EXP_MAX_LEN	40	//表达式的最大长度

//标题栏区域
#define WINDOW_TITLE_HEIGHT		    32//窗口标题栏高度
#define WINDOWS_TITLE			    TEXT("计算器")//窗口标题
#define BUTTON_CONTROL_NUM		    3 //控制按钮数目
#define BUTTON_CONTROL_WIDTH		    50 //控制按钮宽度

//按键在button_caption的位置
#define	KEY_LN		0
#define	KEY_LOG		1
#define	KEY_SIN		2
#define	KEY_COS		3
#define	KEY_TAN		4
#define	KEY_1X		5
#define	KEY_CE		6
#define	KEY_C		7
#define	KEY_BACK	8
#define	KEY_DIV		9
#define	KEY_POW		10
#define	KEY_7		11
#define	KEY_8		12
#define	KEY_9		13
#define	KEY_X		14
#define	KEY_X2		15
#define	KEY_4		16
#define	KEY_5		17
#define	KEY_6		18
#define	KEY_MINUS	19
#define	KEY_SQRT	20
#define	KEY_1		21
#define	KEY_2		22
#define	KEY_3		23
#define	KEY_ADD		24
#define	KEY_LEFT	25
#define	KEY_RIGHT	26
#define	KEY_0		27
#define	KEY_DOT		28
#define	KEY_EQUAL	29

TCHAR *button_caption[BUTTON_ROWS*BUTTON_COLUMNS] = {
	L"ln"	,	L"log"	,	L"sin"	,	L"cos"	,	L"tan",
	L"1/x"	,	L"CE"	,	L"C"	,	L"<-"	,	L"÷",
	L"xⁿ"	,	L"7"	,	L"8"	,	L"9"	,	L"×",
	L"x²"	,	L"4"	,	L"5"	,	L"6"	,	L"-",
	L"√"	,	L"1"	,	L"2"	,	L"3"	,	L"+",
	L"("	,	L")"	,	L"0"	,	L"."	,	L"="
};

//函数申明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int get_button_index(int x, int y);
int get_control_button_index(int x, int y);
void KeyHandle(int index);
HFONT CreateFont_(int nHeight, int fnWeight);
BOOL LoadSkin(int i);

//计算器面板全局变量
int client_height, client_width;
int key_height, key_width;	//每个按钮的高度和宽度
int button_area_top;		//按钮区域顶部位置
int button_down_index	= -1;	//当前被按下的按钮
int button_hover_index	= -1;	//当前激活的按钮

//标题栏控制按钮全局变量
int button_control_down	    = -1;
int button_control_hover    = -1;
TCHAR *button_control_caption[BUTTON_CONTROL_NUM] = { TEXT("×"),TEXT("-") ,TEXT("★")};

//显示屏上的内容
int need_brackets = 0;			//需要自动补全的括号数目

TCHAR display_stack[DISPLAY_EXP_MAX_LEN +1] =	TEXT("");   //待计算的表达式
TCHAR display[DISPLAY_NUM_MAX_LEN+1]	    =	TEXT("0");  //当前显示的数字

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	//装载默认皮肤
	LoadSkin(1);

	static TCHAR szAppName[] = WINDOWS_TITLE;
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	int screen_w, screen_h;

	//设置窗口阴影
	wndclass.style = CS_HREDRAW | CS_VREDRAW| CS_DROPSHADOW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	//wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hbrBackground = CreateSolidBrush(Skin.WINDOW_BORDER_COLOR);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass)){
		MessageBox(NULL, TEXT("这个程序需要在 Windows NT 才能执行！"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	screen_w=GetSystemMetrics(SM_CXSCREEN);
	screen_h=GetSystemMetrics(SM_CYSCREEN);

	hwnd = CreateWindow(
		szAppName,
		WINDOWS_TITLE,
		WS_POPUP | WS_MINIMIZEBOX,
		screen_w/2- WINDOW_WIDTH/2,
		screen_h/2- WINDOW_HEIGHT/2,
		WINDOW_WIDTH+2,
		WINDOW_HEIGHT+2,//留下边界位置
		NULL,
		NULL,
		hInstance,
		NULL);


	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
BOOL LoadSkin(int i) {
	//装载第i套皮肤，即为skin结构体变量赋值
	if (i > SKIN_COUNT)return FALSE;
	switch (i)
	{
	case 1:
		//窗口边界颜色
		Skin.WINDOW_BORDER_COLOR =	RGB(24, 112, 181);

		//计算器面板区域
		Skin.BUTTON_AREA_BG_NOR =	RGB(230, 230, 230);
		Skin.BUTTON_AREA_BG_HOVER =	RGB(207, 207, 207);
		Skin.BUTTON_AREA_BG_DOWN =	RGB(184, 184, 184);

		//显示器区域
		Skin.DISPLAY_AREA_BG =		RGB(242, 242, 242);

		//标题栏区域
		Skin.BUTTON_CONTROL_COLOR_NOR =		RGB(242, 242, 242);
		Skin.BUTTON_CONTROL_COLOR_HOVER =	RGB(25, 114, 184);
		Skin.BUTTON_CONTROL_COLOR_DOWN =	RGB(51, 130, 192);
		Skin.BUTTON_CONTROL_CLOSE_COLOR_DOWN =	RGB(241, 112, 122);
		Skin.BUTTON_CONTROL_CLOSE_COLOR_HOVER = RGB(232, 17, 35);
		break;
	case 2:
		//计算器面板区域
		Skin.BUTTON_AREA_BG_NOR = RGB(0, 200, 200);
		Skin.BUTTON_AREA_BG_HOVER = RGB(0, 178, 178);
		Skin.BUTTON_AREA_BG_DOWN = RGB(0, 153, 153);

		//显示器区域
		Skin.DISPLAY_AREA_BG = RGB(0, 220, 220);

		//标题栏区域
		Skin.BUTTON_CONTROL_COLOR_NOR = RGB(0, 220, 220);

		break;
	case 3:
		//计算器面板区域
		Skin.BUTTON_AREA_BG_NOR = RGB(230, 230, 0);
		Skin.BUTTON_AREA_BG_HOVER = RGB(207, 207, 0);
		Skin.BUTTON_AREA_BG_DOWN = RGB(184, 184, 0);

		//显示器区域
		Skin.DISPLAY_AREA_BG = RGB(242, 242, 0);

		//标题栏区域
		Skin.BUTTON_CONTROL_COLOR_NOR = RGB(242, 242, 0);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc, buffer_hdc;
	HBITMAP bmp;
	static HFONT font_title;	//标题栏字体
	static HFONT font_title_button;	//标题栏控制按钮字体
	static HFONT font_display;	//显示屏字体
	static HFONT font_display_stack;//显示频表达式字体

	PAINTSTRUCT ps;
	RECT rect;
	int x, y;
	int index = 0;			//指示当前是第几个按钮


	switch (message){
	case WM_CREATE:
		//计算每个按钮的尺寸
		GetClientRect(hwnd, &rect);
		client_height = rect.bottom;
		client_width = rect.right;
		key_height = BUTTON_HEIGHT;
		key_width = client_width / BUTTON_COLUMNS;
		button_area_top = client_height - key_height*BUTTON_ROWS;

		//创建字体，只需要创建一次
		//设置标题栏字体
		font_title = CreateFont_((int)(WINDOW_TITLE_HEIGHT*0.6), FW_HEAVY);

		//设置标题栏控制按钮字体
		font_title_button = CreateFont_((int)(WINDOW_TITLE_HEIGHT*0.8), FW_HEAVY);

		//设置显示屏字体，大字体
		font_display = CreateFont_((int)(button_area_top*0.3), FW_HEAVY);

		//设置显示频字体，以及计算器面板按钮字体，小字体
		font_display_stack = CreateFont_((int)((key_height)*0.5), FW_DEMIBOLD);

		return 0;
	case WM_LBUTTONDOWN:
		 x = LOWORD(lParam);
		 y = HIWORD(lParam);
		//重绘标题栏，实现标题栏的控制
		if (y <= WINDOW_TITLE_HEIGHT){
			if (x > client_width - BUTTON_CONTROL_WIDTH*BUTTON_CONTROL_NUM&&x<client_width){
				/*标题栏控制按钮*/
				button_control_hover = -1;
				button_control_down = get_control_button_index(x, y);
				InvalidateRect(hwnd, NULL, FALSE);
			}else{
				/*响应鼠标在标题栏拖动效果*/
				SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
				SendMessage(hwnd, WM_LBUTTONUP, NULL, NULL);
			}
			break;
		}
		//计算器面板按钮的操作
		button_hover_index = -1;
		button_down_index = get_button_index(x, y);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONUP:
		 x = LOWORD(lParam);
		 y = HIWORD(lParam);
		 //重绘标题栏，实现标题栏的控制
		if (y <= WINDOW_TITLE_HEIGHT){
			if (x > client_width - BUTTON_CONTROL_WIDTH*BUTTON_CONTROL_NUM&&x<client_width){
				/*标题栏控制按钮*/
				button_control_down = -1;
				button_control_hover = -1;
				index = get_control_button_index(x, y);
				if (index == 0){
					/*关闭程序*/
					PostQuitMessage(0);
					break;
				}else if (index == 1){
					/*最小化*/
					ShowWindow(hwnd, SW_MINIMIZE);
					break;
				}
				else if (index == 2) {
					/*换肤按钮*/
					static int SkinID = 1;
					SkinID++;
					if (SkinID > SKIN_COUNT)SkinID = 1;
					LoadSkin(SkinID);
					InvalidateRect(hwnd, NULL, FALSE);
				}
				//InvalidateRect(hwnd, NULL, FALSE);
			}
			break;
		}
		//计算器面板按钮的操作
		button_down_index = -1;
		index = get_button_index(x, y);
		button_hover_index = index; //放开之后还有焦点
		KeyHandle(index); //处理按键
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_MOUSEMOVE:
		 x = LOWORD(lParam);
		 y = HIWORD(lParam);
		 //重绘标题栏，实现标题栏的控制
		if (y <= WINDOW_TITLE_HEIGHT){
			if (x > client_width - BUTTON_CONTROL_WIDTH*BUTTON_CONTROL_NUM&&x<client_width){
				/*标题栏控制按钮*/
				button_control_hover = get_control_button_index(x, y);
				InvalidateRect(hwnd, NULL, FALSE);
			}
			break;
		}else{
			button_control_hover = -1;
			button_control_down = -1;
		}
		//计算器面板按钮的操作
		button_hover_index = get_button_index(x, y);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//使用双缓冲，防止GDI闪烁
		buffer_hdc= CreateCompatibleDC(NULL);
		bmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH,WINDOW_HEIGHT);
		SelectObject(buffer_hdc, bmp);

		//选入画刷和画笔
		SelectObject(buffer_hdc, GetStockObject(DC_BRUSH));
		SelectObject(buffer_hdc, GetStockObject(DC_PEN));
		SetBkMode(buffer_hdc, TRANSPARENT);//设置字体背景

		//绘制显示屏
		SetDCBrushColor(buffer_hdc, Skin.DISPLAY_AREA_BG);
		SetDCPenColor(buffer_hdc, Skin.DISPLAY_AREA_BG);
		Rectangle(buffer_hdc, 0, 0, client_width, button_area_top);

		//设置显示屏字体，大字体
		SelectObject(buffer_hdc, font_display);

		//绘制当前显示屏数字
		rect.left = 10;//左右留一点空隙
		rect.top = (int)(button_area_top*0.4);
		rect.right = client_width - 10;
		rect.bottom = button_area_top;
		DrawText(buffer_hdc, display, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
				
		//设置显示频字体，以及计算器面板按钮字体，小字体
		SelectObject(buffer_hdc, font_display_stack);

		//绘制当前显示屏公式
		SetDCPenColor(buffer_hdc, RGB(192, 192, 192));
		rect.left = 10;//左右留一点空隙
		rect.top = 0;
		rect.right = client_width - 10;
		rect.bottom = (int)(button_area_top*0.6);
		DrawText(buffer_hdc, display_stack, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

		//绘制计算器面板按钮区域背景
		SetDCBrushColor(buffer_hdc, Skin.BUTTON_AREA_BG_NOR);
		SetDCPenColor(buffer_hdc, Skin.BUTTON_AREA_BG_NOR);
		Rectangle(buffer_hdc, 0, button_area_top-1, client_width, client_height);
		
		//第一排按钮不同的背景颜色
		SetDCBrushColor(buffer_hdc, Skin.DISPLAY_AREA_BG);
		SetDCPenColor(buffer_hdc, Skin.DISPLAY_AREA_BG);
		Rectangle(buffer_hdc, 0, button_area_top , client_width, button_area_top+ key_height);

		//绘制计算器面板按钮
		index = 0;
		for (int i = 0; i < BUTTON_ROWS; i++){
			for (int j = 0; j < BUTTON_COLUMNS; j++){
				rect.left = j*key_width;
				rect.top = button_area_top + i*key_height;
				rect.right = rect.left + key_width;
				rect.bottom = rect.top + key_height;
				
				if (index == button_hover_index){
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_AREA_BG_HOVER);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_AREA_BG_HOVER);
					Rectangle(buffer_hdc, rect.left, rect.top, rect.right, rect.bottom);
				}else if (index == button_down_index){
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_AREA_BG_DOWN);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_AREA_BG_DOWN);
					Rectangle(buffer_hdc, rect.left, rect.top, rect.right, rect.bottom);
				}
				DrawText(buffer_hdc, button_caption[index], -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				index++;
			}
		}

		//设置标题栏字体
		SelectObject(buffer_hdc, font_title);

		//绘制标题栏
		rect.left = 10;//左右留一点空隙
		rect.top = 0;
		rect.right = client_width;
		rect.bottom = WINDOW_TITLE_HEIGHT;
		DrawText(buffer_hdc, WINDOWS_TITLE, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

		//设置标题栏控制按钮字体
		SelectObject(buffer_hdc, font_title_button);

		//绘制标题栏控制按钮
		for (int i = 0; i < BUTTON_CONTROL_NUM; i++){
			rect.left = client_width - BUTTON_CONTROL_WIDTH*(i+1);
			rect.top = 0;
			rect.right= client_width - BUTTON_CONTROL_WIDTH*i;
			rect.bottom = WINDOW_TITLE_HEIGHT;

			if (i == button_control_hover){
				if (i == 0){
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_CONTROL_CLOSE_COLOR_HOVER);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_CONTROL_CLOSE_COLOR_HOVER);
				}else{
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_HOVER);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_HOVER);
				}
			}
			else if (i == button_control_down){
				if (i == 0){
					 //关闭按钮特殊一点
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_CONTROL_CLOSE_COLOR_DOWN);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_CONTROL_CLOSE_COLOR_DOWN);
				}else{
					SetDCBrushColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_DOWN);
					SetDCPenColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_DOWN);
				}
			}else{
				SetDCBrushColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_NOR);
				SetDCPenColor(buffer_hdc, Skin.BUTTON_CONTROL_COLOR_NOR);
			}

			Rectangle(buffer_hdc, rect.left, rect.top, rect.right, rect.bottom);
			DrawText(buffer_hdc, button_control_caption[i], -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}

		BitBlt(hdc, 1, 1, WINDOW_WIDTH, WINDOW_HEIGHT,buffer_hdc, 0, 0, SRCCOPY);
		DeleteObject(bmp);
		DeleteObject(buffer_hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_KEYDOWN:
	    switch (wParam)
	    {
		case '1':
		case VK_NUMPAD1:
		    KeyHandle(KEY_1);
		    break;
		case '2':
		case VK_NUMPAD2:
		    KeyHandle(KEY_2);
		    break;
		case '3':
		case VK_NUMPAD3:
		    KeyHandle(KEY_3);
		    break;
		case '4':
		case VK_NUMPAD4:
		    KeyHandle(KEY_4);
		    break;
		case '5':
		case VK_NUMPAD5:
		    KeyHandle(KEY_5);
		    break;
		case '6':
		case VK_NUMPAD6:
		    KeyHandle(KEY_6);
		    break;
		case '7':
		case VK_NUMPAD7:
		    KeyHandle(KEY_7);
		    break;
		case '8':
		case VK_NUMPAD8:
		    KeyHandle(KEY_8);
		    break;
		case '9':
		case VK_NUMPAD9:
		    KeyHandle(KEY_9);
		    break;
		case '0':
		case VK_NUMPAD0:
		    KeyHandle(KEY_0);
		    break;
		case VK_DECIMAL:
		    KeyHandle(KEY_DOT);
		    break;
		case VK_ADD:
		    KeyHandle(KEY_ADD);
		    break;
		case VK_SUBTRACT:
		    KeyHandle(KEY_MINUS);
		    break;
		case VK_MULTIPLY:
		    KeyHandle(KEY_X);
		    break;
		case VK_DIVIDE:
		    KeyHandle(KEY_DIV);
		    break;
		case VK_BACK:
		    KeyHandle(KEY_BACK);
		    break;
		case VK_RETURN:
		    KeyHandle(KEY_EQUAL);
		    break;
	    }
	    InvalidateRect(hwnd, NULL, FALSE);
	    return 0;
	case WM_DESTROY:
	    PostQuitMessage(0);
	    return 0;

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
int get_button_index(int x,int y){
	/*获取鼠标在哪个计算器面板的哪个按钮上*/
	RECT rect;
	int index = 0;
	for (int i = 0; i < BUTTON_ROWS; i++){
		for (int j = 0; j < BUTTON_COLUMNS; j++){
			rect.left = j*key_width;
			rect.right = rect.left + key_width;
			rect.top = button_area_top + i*key_height;
			rect.bottom = rect.top + key_height;
			if (x >= rect.left&&x <= rect.right&&y >= rect.top&&y <= rect.bottom){
				return index;
			}
			index++;
		}
	}
	return -1;
}
int get_control_button_index(int x, int y){
	/*判断鼠标在标题控制按钮的哪一个上*/
	RECT rect;
	for (int i = 0; i < BUTTON_CONTROL_NUM; i++){
		rect.left = client_width - BUTTON_CONTROL_WIDTH*(i + 1);
		rect.top = 0;
		rect.right = client_width - BUTTON_CONTROL_WIDTH*i;
		rect.bottom = WINDOW_TITLE_HEIGHT;

		if (x >= rect.left&&x <= rect.right&&y >= rect.top&&y <= rect.bottom){
			return i;
		}
	}
	return -1;
}
HFONT CreateFont_(int nHeight, int fnWeight){
	HFONT hFont = CreateFont(
		nHeight,			// int     nHeight,
		0,				// int     nWidth,
		0,				// int     nEscapement,
		0,				// int     nOrientation,
		fnWeight,			// int     fnWeight,
		0,				// DWORD   fdwItalic,
		0,				// DWORD   fdwUnderline,
		0,				// DWORD   fdwStrikeOut,
		0,				// DWORD   fdwCharSet,
		OUT_DEFAULT_PRECIS,		// DWORD   fdwOutputPrecision,
		CLIP_DEFAULT_PRECIS,		// DWORD   fdwClipPrecision,
		ANTIALIASED_QUALITY,		// DWORD   fdwQuality,
		FF_DONTCARE,			// DWORD   fdwPitchAndFamily,
		TEXT("微软雅黑")		// LPCTSTR lpszFace
		);
	return hFont;
}
int is_first_keyup() {
    /*判断是不是第一次按下按键，即显示屏上只有0*/
    return (wcscmp(display, TEXT("0")) == 0);
}
void push_chr_to_expression(TCHAR *str){
	/*将str压入表达式，并检查表达式是否满足要求*/
	//检查表达式长度是否满足要求
	if (wcslen(display_stack) >= DISPLAY_EXP_MAX_LEN) {
		return;
	}
	wcscat(display_stack, str);
}

void KeyHandle(int index){
	int n = 0;
	double result;
	int error_code;
	static BOOL new_operate = 1;	//是否是新的表达式，每次计算完一个表达式就将此置为1

	/*每次计算完一个表达式，先将屏幕清空*/
	if (new_operate == 1) {
		new_operate = 0;
		KeyHandle(KEY_C);
       }
    
	/*处理按键*/
	TCHAR *key = button_caption[index];

	switch (index){
	/*计算器功能键*/
		case KEY_CE://清理当前操作数
			wcscpy(display, TEXT("0"));
			break;
		case KEY_C://清除所有
			wcscpy(display_stack, TEXT(""));
			wcscpy(display, TEXT("0"));
			break;
		case KEY_BACK://退位键
			if (wcscmp(display, L"0") == 0|| wcslen(display)==0) {
				if (wcslen(display_stack) > 0) {
					display_stack[wcslen(display_stack) - 1] = '\0';
				}
				else {
					wcscpy(display, L"0");
				}
			}
			else {
				display[wcslen(display) - 1] = '\0';
				if (wcslen(display) == 0)wcscpy(display, L"0");
			}
			
			break;

	/*数字键*/
		case KEY_DOT:
			//不允许按下多次小数点
			for (int i = 0; i < wcslen(display); i++) {
				if (display[i] == '.')n++;
			}
			if (n >= 1)break;
			wcscat(display, key);
			break;
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			//数字要设置一个最大长度
			if (wcslen(display) >= DISPLAY_NUM_MAX_LEN)
				break;

			//第一位不为0
			if (is_first_keyup()) {
				wcscpy(display, TEXT(""));  
			}
			wcscat(display, key);
			break;

	/*基本运算键*/
		case KEY_RIGHT:	//)
			//第一位不能为后括号
			if (is_first_keyup()) {
				break;	
			}
			//需要补齐的后括号数目减一
			need_brackets--;
		case KEY_LEFT:	//(
			//第一位不为0
			if (is_first_keyup()) {
				wcscpy(display, TEXT(""));  
			}
			if(index==KEY_LEFT)need_brackets++;//记录还差一个后括号，之后补齐
		case KEY_ADD:	//+
		case KEY_MINUS:	//-
		case KEY_X:	//X
		case KEY_DIV:	//÷
		case KEY_POW:	//n次方
			if (index == KEY_POW)key = L"^";

			//数字最后一位不为小数点
			if (display[wcslen(display) - 1] == '.')display[wcslen(display) - 1] = '\0';
			push_chr_to_expression(display);
			push_chr_to_expression(key);

			wcscpy(display, TEXT(""));
			break;

	/*拓展功能键*/
		case KEY_1X:		// 1/x
			key = L"1÷";
		case KEY_X2:		//平方
			if (index == KEY_X2)key = L"sqr";
		case KEY_SQRT:		//开根号
		case KEY_LN:	    
		case KEY_LOG:
		case KEY_SIN:
		case KEY_COS:
		case KEY_TAN:
			if (is_first_keyup()) {
				wcscpy(display, L"");
			}

			push_chr_to_expression(key);

			//自动加上前括号
			push_chr_to_expression(button_caption[KEY_LEFT]);

			//数字最后一位不为小数点
			if (display[wcslen(display) - 1] == '.')display[wcslen(display) - 1] = '\0';

			//数字加入表达式
			push_chr_to_expression(display);

			//如果有数字就自动添加右括号，如果没数字就记录差一个右括号
			if (wcscmp(display, L"") != 0) {
				push_chr_to_expression( button_caption[KEY_RIGHT]);
			}
			else {
				//记录还差一个后括号，之后补齐
				need_brackets++;
			}
			wcscpy(display, TEXT(""));
			break;

	/*等于键*/
		case KEY_EQUAL://等于
			//数字最后一位不为小数点
			if (display[wcslen(display) - 1] == '.')display[wcslen(display) - 1] = '\0';
			push_chr_to_expression(display);

			//自动补齐后括号
			for (int i = 0; i < need_brackets; i++) {
				push_chr_to_expression(button_caption[KEY_RIGHT]);
			}
			push_chr_to_expression(key);

			if (!EvaluateExpression(display_stack, result, error_code)) {
				//计算错误
				switch (error_code)
				{
				case ERROR_CODE_NOZERO	:
					wcscpy(display, L"除数不能为0");
					break;
				case ERROR_CODE_WORRYNUM:
					wcscpy(display, L"数字无效");
					break;
				case ERROR_CODE_WORRYEXP:
					wcscpy(display, L"错误的表达式");
					break;
				}
			}else{
				swprintf(display, 10, TEXT("%8lg"), result);
			}
			new_operate = 1;
			need_brackets = 0;
			break;
	}
}