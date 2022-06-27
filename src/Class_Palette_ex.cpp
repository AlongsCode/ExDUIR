#include "Class_Palette_ex.h"

void _palette_register()
{
	DWORD cbObjExtra = 5 * sizeof(size_t);
	Ex_ObjRegister(L"Palette", EOS_VISIBLE, EOS_EX_FOCUSABLE, DT_LEFT, cbObjExtra, LoadCursor(0, IDC_HAND), ECVF_CANVASANTIALIAS, _palette_proc);
}

LRESULT CALLBACK _palette_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		Ex_ObjSetLong(hObj, PTL_DOWN, 0);
		Ex_ObjSetLong(hObj, PTL_BEGINX, 5);
		Ex_ObjSetLong(hObj, PTL_BEGINY, 5);
		HEXBRUSH brush = _brush_create(ExRGB2ARGB(0, 255));
		Ex_ObjSetLong(hObj, PTL_PEN, (LONG_PTR)brush);
		_palette_genimage(hObj);
		Ex_ObjInvalidateRect(hObj, 0);
	}
	else if (uMsg == WM_PAINT)
	{
		_palette_paint(hObj);
	}
	else if (uMsg == WM_DESTROY)
	{
		HEXIMAGE image = (HEXIMAGE)Ex_ObjGetLong(hObj, PTL_IMAGE);
		if (image)
		{
			_img_destroy(image);
		}
		HEXBRUSH brush = (HEXBRUSH)Ex_ObjGetLong(hObj, PTL_PEN);
		if (brush)
		{
			_brush_destroy(brush);
		}
	}
	else if (uMsg == WM_LBUTTONDOWN)
	{
		Ex_ObjSetLong(hObj, PTL_DOWN, 1);
		Ex_ObjSendMessage(hObj, WM_MOUSEMOVE, 0, lParam);
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		Ex_ObjSetLong(hObj, PTL_DOWN, 0);
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		auto down = Ex_ObjGetLong(hObj, PTL_DOWN);
		if (down)
		{
			RECT rc;
			Ex_ObjGetRectEx(hObj, &rc, 1);
			if (LOWORD(lParam) >= rc.left && LOWORD(lParam) <= rc.right)
			{
				if (HIWORD(lParam) >= rc.top && HIWORD(lParam) <= rc.bottom)
				{
					Ex_ObjSetLong(hObj, PTL_BEGINX, LOWORD(lParam));
					Ex_ObjSetLong(hObj, PTL_BEGINY, HIWORD(lParam));
					Ex_ObjInvalidateRect(hObj, 0);
					HEXIMAGE img = (HEXIMAGE)Ex_ObjGetLong(hObj, PTL_IMAGE);
					EXARGB pixel;
					_img_getpixel(img, LOWORD(lParam), HIWORD(lParam), &pixel);
					Ex_ObjDispatchNotify(hObj, PTN_MOUSEMOVE, pixel, 0);
				}
			}
		}	
	}
	return Ex_ObjDefProc(hWnd, hObj, uMsg, wParam, lParam);
}

void _palette_genimage(HEXOBJ hObj)
{
	INT color[8] = {255, 33023, 65535, 65280, 16776960, 16711680, 16711808, 255};
	INT nError = 0;
	obj_s* pObj = nullptr;
	if (_handle_validate(hObj, HT_OBJECT, (LPVOID*)&pObj, &nError))
	{
		auto canvas = _canvas_createfromobj(hObj, pObj->right_, pObj->bottom_, 0);
		_canvas_begindraw(canvas);
		auto dc = _canvas_getdc(canvas);
		auto GridWidth = (pObj->right_ - pObj->left_) / 7 + 1;
		auto GridHeight = (pObj->bottom_ - pObj->top_) / 2;
		for (int i = 0; i < 7; i++)
		{
			_palette_drawgradientrect(dc, GridWidth * i, GridHeight, GridWidth * i + GridWidth, GridHeight + 1, color[i], color[i + 1], TRUE);
		}
		for (int i = 0; i < (pObj->right_ - pObj->left_); i++)
		{
			_palette_drawgradientrect(dc, i, 0, i + 1, GridHeight, ExRGB2ARGB(16777125, 0), GetPixel(dc, i, GridHeight), FALSE);
			_palette_drawgradientrect(dc, i, GridHeight, i + 1, GridHeight * 2,  GetPixel(dc, i, GridHeight), ExRGB2ARGB(0, 0), FALSE);
		}
		_canvas_releasedc(canvas);
		_canvas_enddraw(canvas);
		HEXIMAGE hImg;
		_img_createfromcanvas(canvas, &hImg);
		Ex_ObjSetLong(hObj, PTL_IMAGE, hImg);
		_canvas_destroy(canvas);
	}
}

void _palette_drawgradientrect(HDC hdc, INT left, INT top, INT right, INT bottom, EXARGB startColor, EXARGB endColor, BOOL horizontalGradient)
{
	TRIVERTEX tr[2];
	GRADIENT_RECT gradrc;
	
	tr[0].x = left;
	tr[0].y = top;

	tr[0].Red = (USHORT)(ExGetR(startColor) << 8);
	tr[0].Green = (USHORT)(ExGetG(startColor) << 8);
	tr[0].Blue = (USHORT)(ExGetB(startColor) << 8);
	tr[0].Alpha = 255;
	tr[1].x = right;
	tr[1].y = bottom;
	tr[1].Red = (USHORT)(ExGetR(endColor) << 8);
	tr[1].Green = (USHORT)(ExGetG(endColor) << 8);
	tr[1].Blue = (USHORT)(ExGetB(endColor) << 8);
	tr[1].Alpha = 255;
	gradrc.UpperLeft = 0;
	gradrc.LowerRight = 1;
	INT hv = 0;
	if (horizontalGradient)
	{
		hv = GRADIENT_FILL_RECT_H;
	}
	else {
		hv = GRADIENT_FILL_RECT_V;
	}
	GradientFill(hdc, (PTRIVERTEX)&tr, 2, &gradrc, 1, hv);
}

void _palette_paint(HEXOBJ hObj)
{
	EX_PAINTSTRUCT ps{ 0 };
	if (Ex_ObjBeginPaint(hObj, &ps))
	{
		HEXIMAGE img = (HEXIMAGE)Ex_ObjGetLong(hObj, PTL_IMAGE);
		_canvas_clear(ps.hCanvas, ExRGB2ARGB(0, 255));
		_canvas_drawimagerectrect(ps.hCanvas, img, 0, 0, ps.uWidth, ps.uHeight, 0, 0, ps.uWidth, ps.uHeight, 255);
		HEXBRUSH brush = (HEXBRUSH)Ex_ObjGetLong(hObj, PTL_PEN);
		INT beginX = Ex_ObjGetLong(hObj, PTL_BEGINX);
		INT beginY = Ex_ObjGetLong(hObj, PTL_BEGINY);
		_canvas_drawellipse(ps.hCanvas, brush, beginX - 5/2, beginY-5/2, 5, 5, 2, 0);
		Ex_ObjEndPaint(hObj, &ps);
	}
}