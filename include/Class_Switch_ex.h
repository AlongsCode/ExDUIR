#pragma once
#include "help_ex.h"

// ��������_������ɫ_��ͨ
#define ESP_CRBKGNORMAL	1
// ��������_������ɫ_��ȼ
#define ESP_CRBKGHOVER	2
// ��������_������ɫ_���»�ѡ��
#define ESP_CRBKGDOWNORCHECKED	3
// ��������_�߿���ɫ_��ͨ
#define ESP_CRBORDERNORMAL	4
// ��������_�߿���ɫ_��ȼ
#define ESP_CRBORDERHOVER	5
// ��������_�߿���ɫ_���»�ѡ��
#define ESP_CRBORDERDOWNORCHECKED	6
// ��������_Բ�Ƕ�
#define ESP_RADIUS 7
// ��������_�߿�
#define ESP_STROKEWIDTH 8


void _switch_register();
LRESULT CALLBACK _switch_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam);
INT	_switch_paint(HEXOBJ hObj);