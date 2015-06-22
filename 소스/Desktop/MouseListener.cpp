#include"Header.h"

#include "afxwin.h"
#include "stdafx.h"
#include "TrayIconMng.h"
#include "FingerKeyboard.h"
#include "FingerKeyboardDlg.h"

Mouse* MouseListener::mouse;// = new Mouse();
CvRect MouseListener::mouseDragArea;
CvPoint MouseListener::originPoint;


HWND hWnd;
#define IDH_TRAYICON 0x01


MouseListener::~MouseListener(){
	delete this->mouse;
}
void MouseListener::resetMouseDragArea(){
	mouseDragArea.width = 0;
	mouseDragArea.height = 0;
	mouseDragArea.x = 0;
	mouseDragArea.y = 0;

}
MouseListener::MouseListener(){
	mouse = new Mouse();
	this->mouse->setLButtonUp();
	this->mouse->setRButtonUp();
}
bool MouseListener::isMouseDragSize(){
	return this->mouseDragArea.width >0 && this->mouseDragArea.height > 0;
}
Mouse* MouseListener::getMouse(){
	return this->mouse;
}
CvRect MouseListener::getMouseDragArea(){
	return this->mouseDragArea;
}
void MouseListener::mouseClickEvent(int mEvent, int x, int y, int flags, void* param){
	IplImage* targetImage = (IplImage*) param;
	static int cornerIndex;
	static CvPoint2D32f adjustPoint;

	if(!targetImage)
		return;
	if((CurrentMode::state == SET_ROI && mouse->getLButtonDownState())||
		CurrentMode::state == ADJUST_CORNER && mouse->getRButtonDownState()){
			mouseDragArea.x = MIN(x, originPoint.x);
			mouseDragArea.y = MIN(y, originPoint.y);
			mouseDragArea.width = mouseDragArea.x + CV_IABS(x - originPoint.x);
			mouseDragArea.height = mouseDragArea.y + CV_IABS(y- originPoint.y);

			mouseDragArea.x = MAX(mouseDragArea.x, 0);
			mouseDragArea.y = MAX(mouseDragArea.y, 0);
			mouseDragArea.width = MIN(mouseDragArea.width, targetImage->width);
			mouseDragArea.height = MIN(mouseDragArea.height, targetImage->height);
			mouseDragArea.width -= mouseDragArea.x;
			mouseDragArea.height -= mouseDragArea.y;
	}
	switch(mEvent){
	case CV_EVENT_LBUTTONDOWN:
		if(CurrentMode::state  == SET_ROI){
			originPoint = cvPoint(x, y);
			mouseDragArea = cvRect(x, y, 0, 0);
			mouse->setLButtonDown();
		}
		else if(CurrentMode::state == ROI_CONFIRM){
			//cout<<"MESSAGE : Setting ROI"<<endl;
			//AfxMessageBox(_T("Setting ROI"), MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			//MessageBox(NULL,_T("Setting ROI"),_T("Setting ROI"), MB_OK);

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("���콺 �巡�׸� �̿��Ͽ� A4������ ������ �����ϼ���."));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

			CurrentMode::state = SET_CORNER;
		}
		else if(CurrentMode::state == CORNER_CONFIRM){
			CurrentMode::state = CALC_SPOT;
			//cout<<"MESSAGE : CORNER CONFIRM"<<endl;

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("������� �Ǻλ��� ����մϴ�. �ʷϻ� �� �ȿ� ���� �÷���������."));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

		}
		else if(CurrentMode::state  == CATCH_CORNER){
			mouse->setLButtonDown();
			if(mouseDragArea.x < x && mouseDragArea.x + mouseDragArea.width > x){
				if(mouseDragArea.y < y && mouseDragArea.y + mouseDragArea.height > y){
					//cout<<"MESSAGE : Corner Position Adjusting"<<endl;

					NOTIFYICONDATA nid;    //�������������Ͽ�����
					ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
					nid.cbSize = sizeof(NOTIFYICONDATA);
					nid.hWnd = hWnd;
					nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
					nid.uCallbackMessage = WM_TRAYICON;
					nid.uID = IDH_TRAYICON;
					nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
					_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
					_tcscpy(nid.szInfo, TEXT("���� ���õǾ����ϴ�. �ʿ��� ��ġ�� �ű⼼��."));
					nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
					if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
					{
						::Shell_NotifyIcon(NIM_ADD, &nid);
					}

					CurrentMode::state = CORNER_MOVE;
				}
			}
		}
		else if(CurrentMode::state == CATCH_SKIN_COLOR){
			//cout<<"MESSAGE : CALCUALATE SKIN COLOR"<<endl;

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("Ÿ�����ϼ���"));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

			CurrentMode::state = CALC_HIST;
		}
		break;

	case CV_EVENT_MOUSEMOVE:
		if(CurrentMode::state == CORNER_MOVE && mouse->getLButtonDownState()){
			adjustPoint.x = (float)(x - FingerKeyboard::selectedPaperArea.x);
			adjustPoint.y = (float)(y - FingerKeyboard::selectedPaperArea.y);
			PaperKeyboard::keyboardCorner[cornerIndex] = adjustPoint;
		}
		break;
	case CV_EVENT_LBUTTONUP:
		if(CurrentMode::state  == SET_ROI){
			mouse->setLButtonUp();
			if(mouseDragArea.width > 0 && mouseDragArea.height >0){
				CurrentMode::state  = ROI_CONFIRM;
				// cout<<"MESSAGE : ROI Confirm"<<endl;

				NOTIFYICONDATA nid;    //�������������Ͽ�����
				ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
				nid.cbSize = sizeof(NOTIFYICONDATA);
				nid.hWnd = hWnd;
				nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
				nid.uCallbackMessage = WM_TRAYICON;
				nid.uID = IDH_TRAYICON;
				nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
				_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
				_tcscpy(nid.szInfo, TEXT("������ ������ Ȯ���մϱ�? �ƴ϶�� ���콺 ������ Ŭ��, �����Ϸ��� ���콺 ���� Ŭ���� �ϼ���."));
				nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
				if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
				{
					::Shell_NotifyIcon(NIM_ADD, &nid);
				}

			}
		}
		else if(CurrentMode::state  == CORNER_MOVE){
			mouse->setLButtonUp();
			// cout<<"MESSAGE : Corner Confirm"<<endl;

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("A4���� ������ �����Ǿ����ϴ�."));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

			CurrentMode::state  = CORNER_CONFIRM;
		}
		break;
	case CV_EVENT_RBUTTONDOWN: 
		if(CurrentMode::state == ROI_CONFIRM){
			//cout<<"MESSAGE : Reset ROI"<<endl;

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("A4���� ������ �缳�� �մϴ�."));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

			mouseDragArea.width = 0;
			mouseDragArea.height = 0;
			mouseDragArea.x = 0;
			mouseDragArea.y = 0;
			CurrentMode::state = SET_ROI;
		}
		else if(CurrentMode::state == CORNER_CONFIRM){
			//cout<<"MESSAGE : Adjust Paper Coner"<<endl;

			NOTIFYICONDATA nid;    //�������������Ͽ�����
			ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
			nid.uCallbackMessage = WM_TRAYICON;
			nid.uID = IDH_TRAYICON;
			nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
			_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
			_tcscpy(nid.szInfo, TEXT("���콺 �巡���Ͽ� �����Ϸ��� ���� �����ϼ���."));
			nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
			if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
			{
				::Shell_NotifyIcon(NIM_ADD, &nid);
			}

			originPoint = cvPoint(x, y);
			mouseDragArea = cvRect(x, y, 0, 0);
			mouse->setRButtonDown();
			CurrentMode::state = ADJUST_CORNER;
		}
		else if(CurrentMode::state == ADJUST_CORNER){
			originPoint = cvPoint(x, y);
			mouseDragArea = cvRect(x, y, 0, 0);
			mouse->setRButtonDown();
		}

		break;
	case CV_EVENT_RBUTTONUP:
		if(CurrentMode::state == ADJUST_CORNER){
			mouse->setRButtonUp();
			for(int i = 0 ; i < 4 ; i++){
				if(PaperKeyboard::keyboardCorner[i].x + FingerKeyboard::selectedPaperArea.x > (mouseDragArea.x)
					&& PaperKeyboard::keyboardCorner[i].x + FingerKeyboard::selectedPaperArea.x < (mouseDragArea.x + mouseDragArea.width))
					if(PaperKeyboard::keyboardCorner[i].y + FingerKeyboard::selectedPaperArea.y > (mouseDragArea.y)
						&& PaperKeyboard::keyboardCorner[i].y + FingerKeyboard::selectedPaperArea.y < (mouseDragArea.y + mouseDragArea.height)){
							cornerIndex = i;
							adjustPoint = PaperKeyboard::keyboardCorner[i];
							CurrentMode::state = CATCH_CORNER;
							// cout<<"MESSAGE : Corner selected"<<endl;

							NOTIFYICONDATA nid;    //�������������Ͽ�����
							ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
							nid.cbSize = sizeof(NOTIFYICONDATA);
							nid.hWnd = hWnd;
							nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
							nid.uCallbackMessage = WM_TRAYICON;
							nid.uID = IDH_TRAYICON;
							nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
							_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
							_tcscpy(nid.szInfo, TEXT("���� ���õǾ����ϴ�."));
							nid.dwInfoFlags = NIIF_USER; //�����ܵ� ������
							if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
							{
								::Shell_NotifyIcon(NIM_ADD, &nid);
							}

					}
			}
		}
		break;
	}
}
