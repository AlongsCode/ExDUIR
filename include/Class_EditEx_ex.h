#pragma once
#include "help_ex.h"

// ��չ�༭��_������ɫ_����
#define EEEP_CRBKGNORMAL	1
// ��չ�༭��_�߿���ɫ_����
#define EEEP_CRBORDERNORMAL	2
// ��չ�༭��_�߿���ɫ_��ȼ
#define EEEP_CRBORDERHOVER	3
// ��չ�༭��_�߿���ɫ_���»�ѡ��
#define EEEP_CRBORDERDOWNORCHECKED	4
// ��չ�༭��_ͼ����ɫ_����
#define EEEP_CRICONNORMAL	5
// ��չ�༭��_ͼ����ɫ_���»򽹵�
#define EEEP_CRICONDOWNORFOCUS	6
// ��չ�༭��_Բ�Ƕ�
#define EEEP_RADIUS	7
// ��չ�༭��_�߿�
#define EEEP_STORKEWIDTH	8
// ��չ�༭��_ͼ��λ��
#define EEEP_ICONPOSITION	9

void _editex_register();
LRESULT CALLBACK _editex_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam);