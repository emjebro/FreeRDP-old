/*
   Copyright (c) 2009 Jay Sorg
   Copyright (c) 2010 Vic Lee

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

#include <windows.h>
#include <stdio.h>
#include "freerdp.h"
#include "wf_event.h"

#define X_POS (lParam & 0xffff)
#define Y_POS ((lParam >> 16) & 0xffff)
#define SCANCODE ((lParam >> 16) & 0xff)

LRESULT CALLBACK
wf_event_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	wfInfo * wfi;
	HDC hdc;
	PAINTSTRUCT ps;

	wfi = (wfInfo *) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch(Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			wfi->backstore->hdc, ps.rcPaint.left, ps.rcPaint.top,
			SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_LBUTTONDOWN:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_MOUSE,
			PTRFLAGS_DOWN | PTRFLAGS_BUTTON1, X_POS, Y_POS);
		break;

	case WM_LBUTTONUP:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_MOUSE,
			PTRFLAGS_BUTTON1, X_POS, Y_POS);
		break;

	case WM_RBUTTONDOWN:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_MOUSE,
			PTRFLAGS_DOWN | PTRFLAGS_BUTTON2, X_POS, Y_POS);
		break;

	case WM_RBUTTONUP:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_MOUSE,
			PTRFLAGS_BUTTON2, X_POS, Y_POS);
		break;

	case WM_MOUSEMOVE:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_MOUSE,
			PTRFLAGS_MOVE, X_POS, Y_POS);
		break;

	case WM_KEYDOWN:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_SCANCODE,
			RDP_KEYPRESS, SCANCODE, 0);
		break;

	case WM_KEYUP:
		wfi->inst->rdp_send_input(wfi->inst, RDP_INPUT_SCANCODE,
			RDP_KEYRELEASE, SCANCODE, 0);
		break;

	default:
		// Process the left-over messages
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	// If something was not done, let it go
	return 0;
}