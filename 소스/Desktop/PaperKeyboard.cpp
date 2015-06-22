#include"Header.h"

#include "afxwin.h"
#include "stdafx.h"
#include "TrayIconMng.h"
#include "FingerKeyboard.h"
#include "FingerKeyboardDlg.h"

HWND hWnd4;
#define IDH_TRAYICON 0x01

CvPoint2D32f PaperKeyboard::keyboardCorner[4];
void PaperKeyboard::setKeyboardCorner(CvPoint2D32f* corner){
	for(int i = 0 ; i < 4 ; i++)
		this->keyboardCorner[i] = corner[i];
}
int PaperKeyboard::getKeyButtonCornerCount(){
	return this->keyButtonCount;
}
CvPoint2D32f PaperKeyboard::getKeyButtonCorner(int index){
	return this->keyButtonCorner[index];
}
void PaperKeyboard::setPaperKeyboardCorner(IplImage* srcImage, CvRect paperArea, MouseListener* mouseListener){
	CvSize size = cvGetSize(srcImage);
	IplImage* transImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage* splitImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* dstImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* eigImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* tempImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	CvPoint2D32f corner[4];
	int paperCorenrCount = 4;


	cvCvtColor(srcImage, transImage, CV_BGR2HSV);
	cvSplit(transImage, 0, splitImage, 0, 0);

	//cvDilate(splitImage, splitImage, 0, 5);
	cvErode(splitImage, splitImage, 0 ,5);
	cvDilate(splitImage, splitImage, 0, 6);
	cvSmooth(splitImage, dstImage, CV_BLUR, 5);
	//cvShowImage("corner", dstImage);

	cvSetImageROI(dstImage, paperArea);
	cvSetImageROI(srcImage, paperArea);
	//cvShowImage("dst", dstImage);
	cvGoodFeaturesToTrack(dstImage, eigImage, tempImage, corner, &paperCorenrCount, 0.04, 200);//, NULL, 7, 0);
	cvFindCornerSubPix (dstImage, corner, paperCorenrCount,cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
	if(paperCorenrCount != 4){
		// cout<<"MESSAGE : Incorrect Area."<<endl;  

		NOTIFYICONDATA nid;    //아이콘을생성하여설정
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd4;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO | NIF_MESSAGE;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.uID = IDH_TRAYICON;
		nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
		_tcscpy(nid.szInfoTitle, TEXT("FingerKeyboard"));
		_tcscpy(nid.szInfo, TEXT("A4용지 설정이 잘못되었습니다. 다시 설정하여주시기 바랍니다."));
		nid.dwInfoFlags = NIIF_USER; //아이콘도 나오게
		if (!::Shell_NotifyIcon(NIM_MODIFY, &nid))
		{
			::Shell_NotifyIcon(NIM_ADD, &nid);
		}


		CurrentMode::state = SET_ROI;
		mouseListener->resetMouseDragArea();
		cvResetImageROI(dstImage);
		cvResetImageROI(srcImage);
		return;
	}

	setKeyboardCorner(corner);
	cvResetImageROI(dstImage);
	cvResetImageROI(srcImage);


	cvReleaseImage(&dstImage);
	cvReleaseImage(&eigImage);
	cvReleaseImage(&tempImage);
	cvReleaseImage(&transImage);
	cvReleaseImage(&splitImage);
	CurrentMode::state = CORNER_CONFIRM;

}
