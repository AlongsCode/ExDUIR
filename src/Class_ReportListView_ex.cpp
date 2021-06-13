#include "Class_ReportListView_ex.h"

ClsPROC m_pfnListView;

LRESULT CALLBACK _rlv_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT nError = 0;

	if (uMsg == WM_CREATE)
	{
		_rlv_init(hObj);
	}
	else if (uMsg == WM_DESTROY)
	{
		_rlv_uninit(hObj);
	}
	else if (uMsg == WM_NCCALCSIZE)
	{
		if ((Ex_ObjGetLong(hObj, EOL_STYLE) & ERLS_NOHEAD) == 0)
		{
			((NCCALCSIZE_PARAMS*)lParam)->rgrc[2].top = ((NCCALCSIZE_PARAMS*)lParam)->rgrc[2].top + Ex_ObjGetLong(hObj, _rlv_nHeadHeight);//rcClient.Top
		}
	}
	else if (uMsg == WM_NOTIFY)
	{
		if (((EX_NMHDR*)lParam)->hObjFrom == hObj)
		{
			if (_rlv_notify_proc(hObj, (EX_NMHDR*)lParam))
			{
				return 0;
			}
		}
	}
	else if (uMsg == WM_HSCROLL)
	{
		size_t ret = Ex_ObjCallProc(m_pfnListView, hWnd, hObj, uMsg, wParam, lParam);
		Ex_ObjInvalidateRect(Ex_ObjGetLong(hObj, _rlv_hObjHead), 0);
		return ret;
	}
	else if (uMsg == WM_SIZE)
	{
		Ex_ObjMove(Ex_ObjGetLong(hObj, _rlv_hObjHead), 0, 0, LOWORD(lParam), Ex_ObjGetLong(hObj, _rlv_nHeadHeight), FALSE);
	}
	else if (uMsg == LVM_GETITEM)
	{
		return _rlv_li_get(hObj, (EX_REPORTLIST_ITEMINFO*)lParam, FALSE) ? 1 : 0;
	}
	else if (uMsg == LVM_SETITEM)
	{
		return _rlv_li_set(hObj, (EX_REPORTLIST_ITEMINFO*)lParam, FALSE) ? 1 : 0;
	}
	else if (uMsg == LVM_GETITEMTEXT)
	{
		if (wParam != 0)
		{
			((EX_REPORTLIST_ITEMINFO*)lParam)->iRow = wParam;
		}
		return _rlv_li_get(hObj, (EX_REPORTLIST_ITEMINFO*)lParam, TRUE) ? 1 : 0;
	}
	else if (uMsg == LVM_SETITEMTEXT)
	{
		if (wParam != 0)
		{
			((EX_REPORTLIST_ITEMINFO*)lParam)->iRow = wParam;
		}
		return _rlv_li_set(hObj, (EX_REPORTLIST_ITEMINFO*)lParam, TRUE) ? 1 : 0;
	}
	else if (uMsg == LVM_INSERTITEM)
	{
		size_t ret = _rlv_tr_ins(hObj, (EX_REPORTLIST_ROWINFO*)lParam);
		if (ret != 0 && wParam != 0)
		{
			_rlv_tr_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_DELETEITEM)
	{
		size_t ret = _rlv_tr_del(hObj, lParam) ? 1 : 0;
		if (ret != 0 && wParam != 0)
		{
			_rlv_tr_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_DELETEALLITEMS)
	{
		_rlv_tr_clear(hObj);
		_rlv_tr_update(hObj);
		return 0;
	}
	else if (uMsg == LVM_INSERTCOLUMN)
	{
		size_t ret = _rlv_tc_ins(hObj, (EX_REPORTLIST_COLUMNINFO*)lParam);
		if (ret != 0 && wParam != 0)
		{
			_rlv_tc_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_DELETECOLUMN)
	{
		size_t ret = _rlv_tc_del(hObj, lParam) ? 1 : 0;
		if (ret != 0 && wParam != 0)
		{
			_rlv_tc_update(hObj);
			_rlv_tr_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_DELETEALLCOLUMN)
	{
		_rlv_tc_clear(hObj);
		_rlv_tc_update(hObj);
		_rlv_tr_update(hObj);
		return 0;
	}
	else if (uMsg == LVM_SORTITEMS)
	{
		size_t ret = 0;
		if (lParam != 0)
		{
			INT iCol = ((EX_REPORTLIST_SORTINFO*)lParam)->iCol;
			array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
			if (hArr != 0 && iCol >= 0 && iCol <= Ex_ObjGetLong(hObj, _rlv_cTCs))
			{
				Array_SetType(hArr, lParam);
				Array_Sort(hArr, ((EX_REPORTLIST_SORTINFO*)lParam)->fDesc != FALSE);
				Array_SetType(hArr, 0);
				Ex_ObjSetLong(hObj, _rlv_nTCIdxSorted, ((EX_REPORTLIST_SORTINFO*)lParam)->iCol);
				Ex_ObjSetLong(hObj, _rlv_fTCSortedDesc, ((EX_REPORTLIST_SORTINFO*)lParam)->fDesc);
				Ex_ObjInvalidateRect(hObj, 0);
				ret = 1;
			}
		}
		return ret;
	}
	else if (uMsg == LVM_UPDATE)
	{
		_rlv_tc_update(hObj);
		_rlv_tr_update(hObj);
		return 0;
	}
	else if (uMsg == LVM_GETCOLUMNCOUNT)
	{
		return Ex_ObjGetLong(hObj, _rlv_cTCs);
	}
	else if (uMsg == LVM_GETCOLUMN)
	{
		LPVOID ptr = __ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), wParam, sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			RtlMoveMemory((LPVOID)lParam, ptr, sizeof(EX_REPORTLIST_COLUMNINFO));
			ret = 1;
		}
		return ret;
	}
	else if (uMsg == LVM_SETCOLUMN)
	{
		EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)__ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), LOWORD(wParam), sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			RtlMoveMemory(ptr, (LPVOID)lParam, sizeof(EX_REPORTLIST_COLUMNINFO));
			LPCWSTR old = ptr->wzText;
			ptr->wzText = copytstr(((EX_REPORTLIST_COLUMNINFO*)lParam)->wzText, lstrlenW(((EX_REPORTLIST_COLUMNINFO*)lParam)->wzText));
			Ex_MemFree((LPVOID)old);
			ret = 1;
		}
		if (ret != 0 && HIWORD(wParam) != 0)
		{
			_rlv_tc_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_SETCOLUMNTEXT)
	{
		EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)__ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), LOWORD(wParam), sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			LPCWSTR old = ptr->wzText;
			ptr->wzText = copytstr((LPCWSTR)lParam, lstrlenW((LPCWSTR)lParam));
			Ex_MemFree((LPVOID)old);
			ret = 1;
		}
		if (ret != 0 && HIWORD(wParam) != 0)
		{
			_rlv_tc_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_GETCOLUMNTEXT)
	{
		EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)__ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), wParam, sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			ret = (size_t)ptr->wzText;
		}
		if (ret != 0 && HIWORD(wParam) != 0)
		{
			_rlv_tc_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_GETCOLUMNWIDTH)
	{
		EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)__ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), wParam, sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			ret = (size_t)ptr->nWidth;
		}
		return ret;
	}
	else if (uMsg == LVM_SETCOLUMNWIDTH)
	{
		EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)__ptr_index((LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo), Ex_ObjGetLong(hObj, _rlv_cTCs), LOWORD(wParam), sizeof(EX_REPORTLIST_COLUMNINFO));
		size_t ret = 0;
		if (ptr != 0)
		{
			ptr->nWidth = lParam;
		}
		if (ret != 0 && HIWORD(wParam) != 0)
		{
			_rlv_tc_update(hObj);
		}
		return ret;
	}
	else if (uMsg == LVM_GETITEMHEIGHT)
	{
		return Ex_ObjGetLong(hObj, _rlv_nItemHeight);
	}
	else if (uMsg == LVM_SETITEMHEIGHT)
	{
		Ex_ObjSetLong(hObj, _rlv_nItemHeight, lParam);
		_rlv_tc_update(hObj);
		return 0;
	}
	else if (uMsg == RLVM_GETHITCOL)
	{
		return _rlv_getHitCol(hObj, lParam);
	}

	return Ex_ObjCallProc(m_pfnListView, hWnd, hObj, uMsg, wParam, lParam);
}

LRESULT CALLBACK _rlv_head_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT nError = 0;

	if (uMsg == WM_PAINT)
	{
		_rlv_head_paint(hObj);
	}
	else if (uMsg == WM_MOUSELEAVE)//当离开表头
	{
		if ((Ex_ObjGetUIState(hObj) & STATE_ALLOWDRAG) == 0)//并且未拖动时
		{
			Ex_ObjSetLong(hObj, _rlv_head_nIndexHit, 0);//设置为未命中任何列
			Ex_ObjInvalidateRect(hObj, 0);
		}
	}
	else if (uMsg == WM_EX_LCLICK)//当单击表头
	{
		INT nHitBlock = 0;
		size_t nIndex = _rlv_head_hittest(hObj, LOWORD(lParam), HIWORD(lParam), TRUE, &nHitBlock);
		HEXOBJ hObjList = Ex_ObjGetLong(hObj, _rlv_head_hListView);
		if (hObjList == 0)
		{
			hObjList = Ex_ObjGetParent(hObj);
		}
		if (hObjList != 0 && nIndex != 0 && nHitBlock == 0)//若命中某一列
		{
			EX_REPORTLIST_COLUMNINFO* pTC = (EX_REPORTLIST_COLUMNINFO*)(Ex_ObjGetLong(hObjList, _rlv_pTCInfo) + (nIndex - 1) * sizeof(EX_REPORTLIST_COLUMNINFO));
			if ((pTC->dwStyle & ERLV_CS_CLICKABLE) == ERLV_CS_CLICKABLE)//如果它是可点击的
			{
				if (Ex_ObjDispatchNotify(hObjList, RLVN_COLUMNCLICK, nIndex, lParam) == 0)//分发事件
				{
					if ((pTC->dwStyle & ERLV_CS_SORTABLE) == ERLV_CS_SORTABLE)//如果是可排序的,则按文本自动排序
					{
						EX_REPORTLIST_SORTINFO* p = (EX_REPORTLIST_SORTINFO*)Ex_MemAlloc(sizeof(EX_REPORTLIST_SORTINFO));
						p->iCol = nIndex;
						if (Ex_ObjGetLong(hObjList, _rlv_nTCIdxSorted) == nIndex)
						{
							p->fDesc = Ex_ObjGetLong(hObjList, _rlv_fTCSortedDesc) != 0 ? FALSE : TRUE;
						}
						Ex_ObjSendMessage(hObjList, LVM_SORTITEMS, 0, (size_t)p);
						Ex_MemFree(p);
					}
				}
			}
		}
	}
	else if (uMsg == WM_LBUTTONDOWN)//当按下左键
	{
		Ex_ObjSetUIState(hObj, STATE_DOWN, FALSE, 0, FALSE);
		INT rHitBlock = 0;
		size_t nIndex = _rlv_head_hittest(hObj, LOWORD(lParam), HIWORD(lParam), FALSE, &rHitBlock);
		if (rHitBlock == 2)//如果命中位置是分割线,则开始拖动
		{
			Ex_ObjSetUIState(hObj, STATE_ALLOWDRAG, FALSE, 0, FALSE);
		}
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		Ex_ObjSetUIState(hObj, STATE_DOWN, TRUE, 0, FALSE);
		Ex_ObjSetUIState(hObj, STATE_ALLOWDRAG, TRUE, 0, FALSE);
		INT rHitBlock = 0;
		_rlv_head_hittest(hObj, LOWORD(lParam), HIWORD(lParam), FALSE, &rHitBlock);
		HEXOBJ hObjList = Ex_ObjGetLong(hObj, _rlv_head_hListView);
		if (hObjList == 0)
		{
			hObjList = Ex_ObjGetParent(hObj);
		}
		if (hObjList != 0)
		{
			_rlv_tc_update(hObjList);
			listview_s* pOwner = (listview_s*)Ex_ObjGetLong(hObjList, EOL_OWNER);
			Ex_ObjDispatchMessage(hObjList, LVM_REDRAWITEMS, pOwner->index_start_, pOwner->index_end_);
		}
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		INT rHitBlock = 0;
		INT nIndex = _rlv_head_hittest(hObj, LOWORD(lParam), HIWORD(lParam), TRUE, &rHitBlock);

		if (nIndex != 0)
		{
			if (rHitBlock == 2 || (Ex_ObjGetUIState(hObj) & STATE_ALLOWDRAG) != 0)//如果是正在拖动,则更新拖动列的宽度,并更新列表
			{

				SetCursor(LoadCursorW(0, IDC_SIZEWE));
				if ((Ex_ObjGetUIState(hObj) & STATE_DOWN) != 0)
				{
					HEXOBJ hObjList = Ex_ObjGetLong(hObj, _rlv_head_hListView);
					if (hObjList == 0)
					{
						hObjList = Ex_ObjGetParent(hObj);
					}
					if (hObjList != 0)
					{
						INT w = -Ex_ObjScrollGetPos(hObjList, SB_HORZ);

						LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObjList, _rlv_pTCInfo);
						for (INT i = 0; i < nIndex; i++)
						{
							EX_REPORTLIST_COLUMNINFO* p = (EX_REPORTLIST_COLUMNINFO*)__ptr_index(pTCs, nIndex, i + 1, sizeof(EX_REPORTLIST_COLUMNINFO));
							if (nIndex == i + 1)
							{
								INT x = LOWORD(lParam);
								RECT rc;
								Ex_ObjGetRect(hObj, &rc);
								if (x > 1 && x < rc.right - 1)
								{
									w = x - w;
									if (w < 1)
									{
										w = 1;
									}
									p->nWidth = w;
									Ex_ObjInvalidateRect(hObjList, 0);
									Ex_ObjInvalidateRect(hObj, 0);
									break;
								}
							}
							w = w + p->nWidth;
						}
						return 0;
					}
				}
			}
			else {
				SetCursor(LoadCursorW(0, IDC_ARROW));
				INT nInexHit = Ex_ObjSetLong(hObj, _rlv_head_nIndexHit, nIndex);
				if (nInexHit != nIndex)
				{
					Ex_ObjInvalidateRect(hObj, 0);
				}
			}
		}
		/*else {
			SetCursor(LoadCursorW(0, IDC_ARROW));
			INT nInexHit = Ex_ObjSetLong(hObj, _rlv_head_nIndexHit, nIndex);
			if (nInexHit != nIndex)
			{
				Ex_ObjInvalidateRect(hObj, 0);
			}
		}*/
	}
	else if (uMsg == WM_SETCURSOR)//禁控件自身再次设置光标产生闪烁
	{
		return 1;
	}

	return Ex_ObjDefProc(hWnd, hObj, uMsg, wParam, lParam);
}

size_t _rlv_head_hittest(HEXOBJ hObj, INT x, INT y, BOOL fJustHit, INT* rHitBlock)
{
	if ((Ex_ObjGetUIState(hObj) & STATE_ALLOWDRAG) == 0)
	{
		HEXOBJ hObjList = Ex_ObjGetLong(hObj, _rlv_head_hListView);
		if (hObjList == 0)
		{
			hObjList = Ex_ObjGetParent(hObj);
		}
		if (hObjList != 0)
		{
			LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObjList, _rlv_pTCInfo);
			INT cTCs = Ex_ObjGetLong(hObjList, _rlv_cTCs);
			if (pTCs != 0 && cTCs > 0)
			{
				RECT rc{ 0 };
				Ex_ObjGetClientRect(hObj, &rc);
				rc.left = -Ex_ObjScrollGetPos(hObjList, SB_HORZ);

				for (INT i = 0; i < cTCs; i++)//循环检测列
				{
					EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + sizeof(EX_REPORTLIST_COLUMNINFO) * i);
					INT nColWidth = ptr->nWidth;

					if (nColWidth > 0)
					{
						rc.right = rc.left + nColWidth;

						if ((ptr->dwStyle & ERLV_CS_LOCKWIDTH) != ERLV_CS_LOCKWIDTH)//如果为非锁定宽度列
						{
							rc.right = rc.right + 5;
						}

						POINT pt;
						pt.x = x;
						pt.y = y;
						if (PtInRect(&rc, pt))//命中了某列
						{
							if (fJustHit == FALSE)
							{
								INT nIndexHit = Ex_ObjSetLong(hObj, _rlv_head_nIndexHit, i + 1);
								if (nIndexHit != i + 1)
								{
									Ex_ObjInvalidateRect(hObj, 0);
								}

							}

							if (rHitBlock != 0)
							{
								*rHitBlock = 0;
								if (*rHitBlock == 0 && ((ptr->dwStyle & ERLV_CS_LOCKWIDTH) != ERLV_CS_LOCKWIDTH))//线
								{
									RECT rcBlock = rc;
									rcBlock.right = rc.right;
									rcBlock.left = rcBlock.right - 10;
									POINT pt2;
									pt2.x = x;
									pt2.y = y;
									if (PtInRect(&rcBlock, pt2))
									{
										*rHitBlock = 2;
									}
								}
							}
							return i + 1;
						}
						rc.left = rc.right;
						if ((ptr->dwStyle & ERLV_CS_LOCKWIDTH) != ERLV_CS_LOCKWIDTH)//
						{
							rc.left = rc.left - 5;
						}
					}
				}
				if (fJustHit == FALSE)
				{
					INT nIndexHit = Ex_ObjSetLong(hObj, _rlv_head_nIndexHit, 0);
					if (nIndexHit != 0)
					{
						Ex_ObjInvalidateRect(hObj, 0);
					}
				}
			}
		}
	}
	else {
		*rHitBlock = 2;
		return Ex_ObjGetLong(hObj, _rlv_head_nIndexHit);
	}
	return 0;
}

void _rlv_head_paint(HEXOBJ hObj)
{
	EX_PAINTSTRUCT2 ps{ 0 };
	if (Ex_ObjBeginPaint(hObj, &ps))
	{

		HEXOBJ hObjList = Ex_ObjGetLong(hObj, _rlv_head_hListView);
		INT nIndexHit = Ex_ObjGetLong(hObj, _rlv_head_nIndexHit);
		if (hObjList == 0)
		{
			hObjList = Ex_ObjGetParent(hObj);
		}
		if (hObjList != 0)
		{
			LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObjList, _rlv_pTCInfo);
			INT cTCs = Ex_ObjGetLong(hObjList, _rlv_cTCs);
			_canvas_clear(ps.hCanvas, Ex_ObjGetColor(hObjList, COLOR_EX_TEXT_HOT));
			if (pTCs != 0 && cTCs > 0)
			{
				HEXBRUSH hBrush = _brush_create(Ex_ObjGetColor(hObjList, COLOR_EX_BORDER));
				INT nOffsetX = -Ex_ObjScrollGetPos(hObjList, SB_HORZ);
				for (INT i = 0; i < cTCs; i++)
				{
					EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + sizeof(EX_REPORTLIST_COLUMNINFO) * i);
					INT nColWidth = ptr->nWidth;
					if (nColWidth > 0)
					{
						if (nIndexHit == i + 1 && (ptr->dwStyle & ERLV_CS_CLICKABLE) == ERLV_CS_CLICKABLE)
						{
							_brush_setcolor(hBrush, Ex_ObjGetColor(hObjList, COLOR_EX_TEXT_HOVER));
							_canvas_fillrect(ps.hCanvas, hBrush, nOffsetX, 0, nOffsetX + nColWidth, ps.height);
							_brush_setcolor(hBrush, Ex_ObjGetColor(hObjList, COLOR_EX_BORDER));
						}
						_canvas_drawtext(ps.hCanvas, Ex_ObjGetFont(hObjList), Ex_ObjGetColor(hObjList, COLOR_EX_TEXT_NORMAL), ptr->wzText, -1, DT_SINGLELINE | ptr->dwTextFormat, nOffsetX + 3, 0, nOffsetX + nColWidth - 3, ps.height);
						_canvas_drawline(ps.hCanvas, hBrush, nOffsetX + nColWidth, 0, nOffsetX + nColWidth, ps.height, 1.5, D2D1_DASH_STYLE_SOLID);
					}
					nOffsetX = nOffsetX + nColWidth;
				}
				_brush_destroy(hBrush);
			}
		}
		Ex_ObjEndPaint(hObj, &ps);
	}
}

INT _rlv_getHitCol(HEXOBJ hObj, INT x)
{
	LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
	INT cTCs = Ex_ObjGetLong(hObj, _rlv_cTCs);
	if (pTCs != 0 && cTCs > 0)
	{
		INT nOffsetX = -Ex_ObjScrollGetPos(hObj, SB_HORZ);
		for (INT i = 0; i < cTCs; i++)
		{
			EX_REPORTLIST_COLUMNINFO* ptr = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + sizeof(EX_REPORTLIST_COLUMNINFO) * i);
			INT nColWidth = ptr->nWidth;
			if (nColWidth > 0)
			{
				if (x >= nOffsetX && x < nOffsetX + nColWidth)
				{
					return i + 1;
				}
			}
			nOffsetX = nOffsetX + nColWidth;
		}
	}
	return 0;
}

BOOL _rlv_notify_proc(HEXOBJ hObj, EX_NMHDR* pNotifyInfo)
{
	INT nCode = pNotifyInfo->nCode;
	LPARAM lParam = pNotifyInfo->lParam;
	if (nCode == NM_CUSTOMDRAW)
	{
		_rlv_draw_tr(hObj, (EX_CUSTOMDRAW*)lParam);
	}
	else if (nCode == NM_CALCSIZE)
	{
		if (Ex_ObjGetLong(hObj, _rlv_nItemWidth) > __get_int((LPVOID)lParam, 0))
		{
			__set_int((LPVOID)lParam, 0, Ex_ObjGetLong(hObj, _rlv_nItemWidth));
		}
		if (Ex_ObjGetLong(hObj, _rlv_nItemHeight) > __get_int((LPVOID)lParam, 4))
		{
			__set_int((LPVOID)lParam, 4, Ex_ObjGetLong(hObj, _rlv_nItemHeight));
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

void _rlv_init(HEXOBJ hObj)
{
	Ex_ObjSetLong(hObj, _rlv_pTCInfo, 0);
	Ex_ObjSetLong(hObj, _rlv_cTCs, 0);
	Ex_ObjSetLong(hObj, _rlv_nItemWidth, 0);
	Ex_ObjSetLong(hObj, _rlv_nItemHeight, 20);
	Ex_ObjSetLong(hObj, _rlv_nHeadHeight, 25);
	HEXOBJ head = Ex_ObjCreate((LPCWSTR)ATOM_REPORTLISTVIEW_HEAD, 0, -1, 0, 0, 0, 0, hObj);
	Ex_ObjSetLong(hObj, _rlv_hObjHead, head);
	Ex_ObjShow(head, (Ex_ObjGetLong(hObj, EOL_STYLE) & ERLS_NOHEAD) == 0);
	Ex_ObjSetLong(hObj, _rlv_nTCIdxSorted, 0);
	array_s* hArr = Array_Create(0);
	if (hArr)
	{
		Array_SetExtra(hArr, hObj);
		Array_BindEvent(hArr, eae_delmember, _rlv_arr_del);
		Array_BindEvent(hArr, eae_comparemember, _rlv_arr_order);
		Ex_ObjSetLong(hObj, _rlv_arrTRInfo, (size_t)hArr);
	}
}


void _rlv_arr_del(array_s* hArr, INT nIndex, reportlistview_tr_s* pvData, INT nType)
{
	LPVOID pTDs = pvData->pTDInfo_;
	if (pTDs != 0)
	{
		INT nCount = Ex_ObjGetLong(Array_GetExtra(hArr), _rlv_cTCs);
		Ex_ObjDispatchNotify(Array_GetExtra(hArr), RLVN_DELETE_ITEM, nIndex, (size_t)pvData);
		for (INT i = 0; i < nCount; i++)
		{
			reportlistview_td_s* pTD = (reportlistview_td_s*)((size_t)pTDs + i * sizeof(reportlistview_td_s));
			Ex_MemFree((LPVOID)pTD->wzText_);
		}
		Ex_MemFree(pTDs);
	}
	Ex_MemFree(pvData);
}


size_t _rlv_arr_order(array_s* hArr, INT nIndex1, LPVOID pvData1, INT nIndex2, LPVOID pvData2, EX_REPORTLIST_SORTINFO* pSortInfo, INT nReason)
{
	LPVOID lpfnCmp = pSortInfo->lpfnCmp;
	INT nIndexTC = pSortInfo->iCol;
	if (nIndexTC != 0)
	{
		reportlistview_td_s* ptr1 = (reportlistview_td_s*)((size_t)(((reportlistview_tr_s*)pvData1)->pTDInfo_) + sizeof(reportlistview_td_s) * (nIndexTC - 1));
		pvData1 = (LPVOID)ptr1->wzText_;
		reportlistview_td_s* ptr2 = (reportlistview_td_s*)((size_t)(((reportlistview_tr_s*)pvData2)->pTDInfo_) + sizeof(reportlistview_td_s) * (nIndexTC - 1));
		pvData2 = (LPVOID)ptr2->wzText_;
	}
	if (lpfnCmp == 0)
	{
		if (nIndexTC == 0)//整行按lParam排序
		{
			return ((reportlistview_tr_s*)pvData2)->lParam_ - ((reportlistview_tr_s*)pvData1)->lParam_;
		}
		else if (pSortInfo->nType == 0)
		{
			return wstr_compare((LPCWSTR)pvData2, (LPCWSTR)pvData1, TRUE);
		}
		else
		{
			pvData1 = _fmt_int(pvData1, 0);
			pvData2 = _fmt_int(pvData2, 0);
			return (size_t)pvData2 - (size_t)pvData1;
		}
	}
	else {
		return ((ReportListViewOrderPROC)lpfnCmp)((HEXOBJ)Array_GetExtra(hArr), nIndex1, pvData1, nIndex2, pvData2, nIndexTC, pSortInfo->nType, (size_t)pSortInfo->lParam);
	}
	return 0;
}

void _rlv_uninit(HEXOBJ hObj)
{
	Array_Destroy((array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo));
	INT nCount = Ex_ObjSetLong(hObj, _rlv_cTCs, 0);
	LPVOID pTCs = (LPVOID)Ex_ObjSetLong(hObj, _rlv_pTCInfo, 0);
	EX_REPORTLIST_COLUMNINFO* ptc = nullptr;
	if (pTCs != 0)
	{
		for (INT i = 0; i < nCount; i++)
		{
			ptc = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + i * sizeof(EX_REPORTLIST_COLUMNINFO));
			Ex_MemFree((LPVOID)ptc->wzText);
		}
		Ex_MemFree(pTCs);
	}
}

void _rlv_draw_tr(HEXOBJ hObj, EX_CUSTOMDRAW* pDrawInfo)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	reportlistview_tr_s* pTR = (reportlistview_tr_s*)Array_GetMember((array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo), pDrawInfo->iItem);
	RECT rcTD{ 0 };
	if (pTR != 0)
	{
		rcTD.left = pDrawInfo->rcPaint.left;
		rcTD.top = pDrawInfo->rcPaint.top;
		rcTD.bottom = pDrawInfo->rcPaint.bottom;

		if (rcTD.top < Ex_ObjGetLong(hObj, _rlv_nHeadHeight) && (Ex_ObjGetLong(hObj, EOL_STYLE) & ERLS_NOHEAD) == 0)
		{
			_canvas_cliprect(pDrawInfo->hCanvas, pDrawInfo->rcPaint.left, Ex_ObjGetLong(hObj, _rlv_nHeadHeight), pDrawInfo->rcPaint.right, pDrawInfo->rcPaint.bottom);
		}
		if (Ex_ObjDispatchNotify(hObj, RLVN_DRAW_TR, (size_t)pTR, (size_t)pDrawInfo) == 0)
		{
			HEXIMAGE hImage = pTR->hImage_;
			if (hImage != 0)
			{
				INT index = pDrawInfo->rcPaint.bottom - pDrawInfo->rcPaint.top;
				_canvas_drawimagerect(pDrawInfo->hCanvas, hImage, rcTD.left, rcTD.top, rcTD.left + index, rcTD.bottom, 255);
				rcTD.left = rcTD.left + index;
			}
			EX_REPORTLIST_COLUMNINFO* pTC = (EX_REPORTLIST_COLUMNINFO*)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
			for (INT i = 1; i <= nCount; i++)
			{
				rcTD.right = rcTD.left + pTC->nWidth;
				if (i == 1 && hImage != 0)
				{
					rcTD.right = rcTD.right - (pDrawInfo->rcPaint.bottom - pDrawInfo->rcPaint.top);
				}
				_rlv_draw_td(hObj, pDrawInfo, pDrawInfo->iItem, i, pTC, &rcTD);
				rcTD.left = rcTD.right;
				pTC = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTC + sizeof(EX_REPORTLIST_COLUMNINFO));
			}
			if ((pDrawInfo->dwStyle & ERLS_DRAWHORIZONTALLINE) != 0)
			{
				HEXBRUSH hBrush = _brush_create(Ex_ObjGetColor(hObj, COLOR_EX_BORDER));
				_canvas_drawline(pDrawInfo->hCanvas, hBrush, pDrawInfo->rcPaint.left, pDrawInfo->rcPaint.bottom, pDrawInfo->rcPaint.right, pDrawInfo->rcPaint.bottom, 1.5, D2D1_DASH_STYLE_SOLID);
				_brush_destroy(hBrush);
			}
		}
		_canvas_resetclip(pDrawInfo->hCanvas);
	}
}

void _rlv_draw_td(HEXOBJ hObj, EX_CUSTOMDRAW* cd, INT nIndexTR, INT nIndexTC, EX_REPORTLIST_COLUMNINFO* pTC, RECT* rcTD)
{
	reportlistview_td_s* pTD = _rlv_td_get(hObj, nIndexTR, nIndexTC);
	if (pTD != 0)
	{
		if (Ex_ObjDispatchNotify(hObj, RLVN_DRAW_TD, nIndexTC, (size_t)cd) == 0)
		{
			LPCWSTR wzText = pTD->wzText_;
			if (wzText != 0)
			{
				INT crText = pTC->crText;
				if (crText == 0) crText = Ex_ObjGetColor(hObj, COLOR_EX_TEXT_NORMAL);
				_canvas_drawtext(cd->hCanvas, Ex_ObjGetFont(hObj), crText, wzText, -1, pTC->dwTextFormat, rcTD->left + 1, rcTD->top, rcTD->right - 3, rcTD->bottom);
			}
			if ((cd->dwStyle & ERLS_DRAWVERTICALLINE) != 0)
			{
				HEXBRUSH hBrush = _brush_create(Ex_ObjGetColor(hObj, COLOR_EX_BORDER));
				_canvas_drawline(cd->hCanvas, hBrush, rcTD->right, rcTD->top, rcTD->right, rcTD->bottom, 1.5, D2D1_DASH_STYLE_SOLID);
				_brush_destroy(hBrush);
			}
		}
	}
}

INT _rlv_tc_ins(HEXOBJ hObj, EX_REPORTLIST_COLUMNINFO* pInsertInfo)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
	INT nIndexInsert = pInsertInfo->nInsertIndex;
	LPVOID pNew = __ptr_ins(&pTCs, nCount, &nIndexInsert, sizeof(EX_REPORTLIST_COLUMNINFO), pInsertInfo);
	EX_REPORTLIST_COLUMNINFO* ptc = (EX_REPORTLIST_COLUMNINFO*)((size_t)pNew + (nIndexInsert - 1) * sizeof(EX_REPORTLIST_COLUMNINFO));
	ptc->wzText = copytstr(pInsertInfo->wzText, lstrlenW(pInsertInfo->wzText));
	ptc->nInsertIndex = 0;
	if (ptc->dwTextFormat == 0)
	{
		ptc->dwTextFormat = DT_SINGLELINE | DT_LEFT | DT_VCENTER;
	}
	Ex_ObjSetLong(hObj, _rlv_pTCInfo, (size_t)pNew);
	nCount = nCount + 1;
	Ex_ObjSetLong(hObj, _rlv_cTCs, nCount);
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	for (INT i = 0; i < Array_GetCount(hArr); i++)
	{
		reportlistview_tr_s* ptr = (reportlistview_tr_s*)Array_GetMember(hArr, i + 1);
		LPVOID pTDs = ptr->pTDInfo_;
		LPVOID p2 = __ptr_ins(&pTDs, nCount - 1, &nIndexInsert, sizeof(reportlistview_td_s), 0);
		ptr->pTDInfo_ = p2;
	}
	Ex_ObjSetLong(hObj, _rlv_nTCIdxSorted, 0);
	Ex_ObjSetLong(hObj, _rlv_fTCSortedDesc, 0);
	return nIndexInsert;
}

BOOL _rlv_tc_del(HEXOBJ hObj, INT nIndex)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	if (nIndex <= 0 || nIndex > nCount)
	{
		return FALSE;
	}
	LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
	EX_REPORTLIST_COLUMNINFO* ptc = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + (nIndex - 1) * sizeof(EX_REPORTLIST_COLUMNINFO));
	Ex_MemFree((LPVOID)ptc->wzText);
	__ptr_del(&pTCs, nCount, nIndex, sizeof(EX_REPORTLIST_COLUMNINFO));
	Ex_ObjSetLong(hObj, _rlv_pTCInfo, (size_t)pTCs);
	nCount = nCount - 1;
	Ex_ObjSetLong(hObj, _rlv_cTCs, nCount);
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	for (INT i = 0; i < Array_GetCount(hArr); i++)
	{
		reportlistview_tr_s* ptr = (reportlistview_tr_s*)Array_GetMember(hArr, i + 1);
		LPVOID pTDs = ptr->pTDInfo_;
		Ex_MemFree((LPVOID)((reportlistview_td_s*)((size_t)pTDs + (nIndex - 1) * sizeof(reportlistview_td_s)))->wzText_);
		__ptr_del(&pTDs, nCount + 1, nIndex, sizeof(reportlistview_td_s));
		ptr->pTDInfo_ = pTDs;
	}
	Ex_ObjSetLong(hObj, _rlv_nTCIdxSorted, 0);
	Ex_ObjSetLong(hObj, _rlv_fTCSortedDesc, 0);
	return TRUE;
}

void _rlv_tc_clear(HEXOBJ hObj)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
	EX_REPORTLIST_COLUMNINFO* ptc = nullptr;
	for (INT i = 0; i < nCount; i++)
	{
		ptc = (EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + i * sizeof(EX_REPORTLIST_COLUMNINFO));
		Ex_MemFree((LPVOID)ptc->wzText);
	}
	Ex_MemFree(pTCs);
	Ex_ObjSetLong(hObj, _rlv_pTCInfo, 0);
	Ex_ObjSetLong(hObj, _rlv_cTCs, 0);
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	reportlistview_tr_s* ptr = nullptr;
	LPVOID pTDs = nullptr;
	for (INT i = 0; i < Array_GetCount(hArr); i++)
	{
		ptr = (reportlistview_tr_s*)Array_GetMember(hArr, i + 1);
		pTDs = ptr->pTDInfo_;
		for (INT j = 0; j < nCount; j++)
		{
			Ex_MemFree((LPVOID)((reportlistview_td_s*)((size_t)pTDs + j * sizeof(reportlistview_td_s)))->wzText_);
		}
		Ex_MemFree(pTDs);
		ptr->pTDInfo_ = 0;
	}
	Ex_ObjSetLong(hObj, _rlv_nTCIdxSorted, 0);
	Ex_ObjSetLong(hObj, _rlv_fTCSortedDesc, 0);
}

void _rlv_tc_update(HEXOBJ hObj)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	LPVOID pTCs = (LPVOID)Ex_ObjGetLong(hObj, _rlv_pTCInfo);
	INT nWidth = 0;
	for (INT i = 0; i < nCount; i++)
	{
		nWidth = nWidth + ((EX_REPORTLIST_COLUMNINFO*)((size_t)pTCs + i * sizeof(EX_REPORTLIST_COLUMNINFO)))->nWidth;
	}
	Ex_ObjSetLong(hObj, _rlv_nItemWidth, nWidth);
	RECT rc{ 0 };
	Ex_ObjGetRect(hObj, &rc);
	Ex_ObjSendMessage(hObj, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));
	//Ex_ObjSendMessage(hObj, WM_HSCROLL, MAKELONG(nScroll, 0), 0);
}

INT _rlv_tr_ins(HEXOBJ hObj, EX_REPORTLIST_ROWINFO* pInsertInfo)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	reportlistview_tr_s* pTR = (reportlistview_tr_s*)Ex_MemAlloc(sizeof(reportlistview_tr_s));
	RtlMoveMemory(pTR, pInsertInfo, sizeof(EX_REPORTLIST_ROWINFO));
	LPVOID pTDs = nullptr;
	if (nCount > 0)
	{
		pTDs = Ex_MemAlloc(sizeof(reportlistview_td_s) * nCount);
	}
	pTR->pTDInfo_ = pTDs;
	nCount = Array_AddMember(hArr, (size_t)pTR, (size_t)pInsertInfo->nInsertIndex);
	if (nCount == 0)
	{
		if (pTDs != 0)
		{
			Ex_MemFree(pTDs);
		}
		Ex_MemFree(pTR);
	}
	return nCount;
}

BOOL _rlv_tr_del(HEXOBJ hObj, INT nIndex)
{
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	BOOL ret = FALSE;
	if (Array_DelMember(hArr, nIndex))
	{
		ret = TRUE;
	}
	return ret;
}

void _rlv_tr_clear(HEXOBJ hObj)
{
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	Array_Clear(hArr);
}

void _rlv_tr_update(HEXOBJ hObj)
{
	INT nCount = Array_GetCount((array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo));
	Ex_ObjSendMessage(hObj, LVM_SETITEMCOUNT, nCount, LVSICF_NOSCROLL);
	Ex_ObjSendMessage(hObj, LVM_REDRAWITEMS, 1, nCount);
}

reportlistview_td_s* _rlv_td_get(HEXOBJ hObj, INT nIndexTR, INT nIndexTC)
{
	INT nCount = Ex_ObjGetLong(hObj, _rlv_cTCs);
	reportlistview_td_s* pTD = nullptr;
	if (nIndexTC <= 0 || nIndexTC > nCount)
	{
		return 0;
	}
	array_s* hArr = (array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo);
	reportlistview_tr_s* pTR = (reportlistview_tr_s*)Array_GetMember(hArr, nIndexTR);
	if (pTR != 0)
	{
		LPVOID pTDs = pTR->pTDInfo_;
		if (pTDs != 0)
		{
			pTD = (reportlistview_td_s*)((size_t)pTDs + (nIndexTC - 1) * sizeof(reportlistview_td_s));
		}
	}
	return pTD;
}

void _rlv_td_setText(HEXOBJ hObj, INT nIndexTR, INT nIndexTC, LPCWSTR wzText)
{
	reportlistview_td_s* pTD = _rlv_td_get(hObj, nIndexTR, nIndexTC);
	if (pTD != 0)
	{
		LPCWSTR wzText_ = pTD->wzText_;
		pTD->wzText_ = copytstr(wzText, lstrlenW(wzText));
		Ex_MemFree((LPVOID)wzText_);
	}
}

BOOL _rlv_li_get(HEXOBJ hObj, EX_REPORTLIST_ITEMINFO* lParam, BOOL fJustText)
{
	INT nIndexTR = lParam->iRow;
	INT nIndexTC = lParam->iCol;
	BOOL ret = FALSE;
	reportlistview_tr_s* pTR = (reportlistview_tr_s*)Array_GetMember((array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo), nIndexTR);
	if (pTR != 0)
	{
		if (fJustText == FALSE)
		{
			lParam->hImage = pTR->hImage_;
			lParam->dwStyle = pTR->dwStyle_;
			lParam->lParam = pTR->lParam_;
			lParam->wzText = ((reportlistview_td_s*)((size_t)pTR->pTDInfo_ + (nIndexTC - 1) * sizeof(reportlistview_td_s)))->wzText_;
		}
		INT nError = 0;
		obj_s* pObj = nullptr;
		if (_handle_validate(hObj, HT_OBJECT, (LPVOID*)&pObj, &nError))
		{
			LPVOID pOwner = pObj->dwOwnerData_;
			LPVOID lpItems = ((listview_s*)pOwner)->lpItems_;
			lParam->dwState = _lv_getitemstate(lpItems, nIndexTR);
			ret = TRUE;
		}
	}
	return ret;
}



BOOL _rlv_li_set(HEXOBJ hObj, EX_REPORTLIST_ITEMINFO* lParam, BOOL fJustText)
{
	INT nIndexTR = lParam->iRow;
	INT nIndexTC = lParam->iCol;
	BOOL ret = FALSE;
	reportlistview_tr_s* pTR = (reportlistview_tr_s*)Array_GetMember((array_s*)Ex_ObjGetLong(hObj, _rlv_arrTRInfo), nIndexTR);
	if (pTR != 0)
	{
		if (fJustText == FALSE)
		{
			pTR->hImage_ = lParam->hImage;
			pTR->dwStyle_ = lParam->dwStyle;
			pTR->lParam_ = lParam->lParam;
		}
		_rlv_td_setText(hObj, nIndexTR, nIndexTC, lParam->wzText);
		ret = TRUE;
	}
	return ret;
}

void _ReportListView_regsiter()
{
	EX_CLASSINFO clsInfo{ 0 };
	Ex_ObjGetClassInfoEx(L"listview", &clsInfo);
	m_pfnListView = clsInfo.pfnClsProc;
	Ex_ObjRegister(L"ReportListView", EOS_VSCROLL | EOS_HSCROLL | EOS_VISIBLE, clsInfo.dwStyleEx, clsInfo.dwTextFormat, 9 * sizeof(size_t), clsInfo.hCursor, clsInfo.dwFlags, _rlv_proc);
	Ex_ObjRegister(L"ReportListView.Header", EOS_VISIBLE, EOS_EX_FOCUSABLE, 0, _rlv_head_sizeof, 0, 0, _rlv_head_proc);
}