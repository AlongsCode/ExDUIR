#pragma once
#include "help_ex.h"

// ��չ��ѡ��_������ɫ_���»�ѡ��
#define ERBEP_CRBKGDOWNORCHECKED	1
// ��չ��ѡ��_�߿���ɫ_����
#define ERBEP_CRBORDERNORMAL	2
// ��չ��ѡ��_�߿���ɫ_��ȼ
#define ERBEP_CRBORDERHOVER	3
// ��չ��ѡ��_�߿���ɫ_���»�ѡ��
#define ERBEP_CRBORDERDOWNORCHECKED	4

void _radiobuttonex_register();
LRESULT CALLBACK _radiobuttonex_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam);
INT _radiobuttonex_paint(HEXOBJ hObj);