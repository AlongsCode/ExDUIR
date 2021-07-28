#pragma once
#include "help_ex.h"

// �б�����_����
#define LB_TYPE 0
// �б�����_������
#define LB_CTCS 1
// �б�����_������Ϣ
#define LB_ITEMINFO 2
// �б�����_�ȵ���Ŀ
#define LB_INDEX 3
// �б�����_ͼƬ����
#define LB_HIMAGELIST 4
// �б�����_ͼƬ����
#define LB_HIMAGWIDTH 5
// �б�����_ͼƬ��߶�
#define LB_HIMAGHEIGHT 6

void _listbuttonex_register();
LRESULT CALLBACK _listbuttonex_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam);
void _listbuttonex_arr_del(array_s* hArr, INT nIndex, EX_LISTBUTTON_ITEMINFO* pvData, INT nType);
void _listbuttonex_paint(HEXOBJ hObj);
void _listbuttonex_mousemove(HEXOBJ hObj, INT nType, LPARAM lParam);
void _listbuttonex_mousedown(HEXOBJ hObj, INT nType, LPARAM lParam);
void _listbuttonex_mouseup(HEXOBJ hObj, INT nType, LPARAM lParam);
void _listbuttonex_mouseleave(HEXOBJ hObj, INT nType);
void _listbuttonex_update(HEXOBJ hObj);
INT _listbuttonex_hittest(HEXOBJ hObj, array_s* hArr, POINT pt);
void _listbuttonex_recovery(HEXOBJ hObj, INT nType, array_s* hArr, INT Index, BOOL Update);
INT _listbuttonex_itemWidth(HEXOBJ hObj, INT nType, UINT nImage, LPCWSTR wzText);
LRESULT CALLBACK _listbuttonex_menu_proc(HWND hWnd, HEXDUI hExDUI, INT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lpResult);