#include"Header.h"
PaperKeyboard_TypeA::PaperKeyboard_TypeA(){
	this->keyButton = new KeyButton[79];
	this->keyButtonCorner = new CvPoint2D32f[316];
	this->keyButtonCount = 79;
	for(int i = 0 ; i < 79 ; i++)
		this->keyButton[i].setKeyId(KeyId(i));
	
}

void PaperKeyboard_TypeA::sortPaperKeyboardCorner(){
	CvPoint2D32f temp;
	for(int i = 0 ; i < 4 ; i++){
		for(int j = 0 ; j < 3 - i ; j++){
			if(keyboardCorner[j].y > keyboardCorner[j+1].y){
				temp = keyboardCorner[j];
				keyboardCorner[j] = keyboardCorner[j+1];
				keyboardCorner[j+1] = temp;
			}
		}
	}
	for(int i =0 ; i < 2 ; i++){
		for(int j = 0 ; j < 1 -i ; i++){
			if(keyboardCorner[j].x > keyboardCorner[j+1].x){
				temp = keyboardCorner[j];
				keyboardCorner[j] = keyboardCorner[j+1];
				keyboardCorner[j+1] = temp;
			}
		}
	}
	for(int i = 0 ; i < 2 ; i++){
		for(int j = 0 ; j < 1 -i ; i++){
			if(keyboardCorner[j+2].x < keyboardCorner[j+3].x){
				temp = keyboardCorner[j+2];
				keyboardCorner[j+2] = keyboardCorner[j+3];
				keyboardCorner[j+3] = temp;
			}
		}
	}
}
void PaperKeyboard_TypeA::cornerSortingX(int startIndex, int cornerCount){
	CvPoint2D32f tmp;
	for(int i = 0  ; i < cornerCount  ; i++){
		for(int j = startIndex ; j < (cornerCount + startIndex -1 ) -i ; j++){
			if(keyButtonCorner[j].x > keyButtonCorner[j+1].x){
				tmp = keyButtonCorner[j];
				keyButtonCorner[j] = keyButtonCorner[j+1];
				keyButtonCorner[j+1] = tmp;
			}	
		}
	}
}
void PaperKeyboard_TypeA::cornerSortingY(int startIndex, int cornerCount){
	CvPoint2D32f tmp;
	for(int i = 0 ; i < cornerCount ; i++){
		for(int j = startIndex ; j < (startIndex + cornerCount -1 ) -i ; j++){
			if(keyButtonCorner[j].y < keyButtonCorner[j+1].y){
				tmp = keyButtonCorner[j];
				keyButtonCorner[j] = keyButtonCorner[j+1];
				keyButtonCorner[j+1] = tmp;
			}	
		}
	}
}
void PaperKeyboard_TypeA::initKeyButtonCorner(){
	cornerSortingY(0,316);

	cornerSortingX(0,32);	cornerSortingX( 32,32);
	cornerSortingX(64,28);	
	cornerSortingX( 92,28);
	cornerSortingX(120,28);	cornerSortingX( 148,28);
	cornerSortingX(176, 26);cornerSortingX( 202, 26);
	cornerSortingX(228,24);	cornerSortingX( 252,24);
	cornerSortingX(276,40);	cornerSortingY(276, 28);			
	cornerSortingX(276,14);cornerSortingX(290, 14);

	cornerSortingY(304, 12);

	cornerSortingX(304, 2);
	cornerSortingX(306, 4);
	cornerSortingX(310, 6);

}
void PaperKeyboard_TypeA::detectKeyButtonCorner(IplImage* srcImage){
	
	CvSize transSize = cvSize(700,600);
	IplImage* eigImage = cvCreateImage(transSize, IPL_DEPTH_8U,1);
	IplImage* tempImage = cvCreateImage(transSize, IPL_DEPTH_8U, 1);
	IplImage* grayImage = cvCreateImage(transSize, IPL_DEPTH_8U, 1);
	IplImage* dstImage = cvCreateImage(transSize, IPL_DEPTH_8U, 1);
	int keyButtonCornerCount = 316;
	cvCopy(srcImage, grayImage);
	//cvShowImage("gray", grayImage);
	cvGoodFeaturesToTrack(grayImage, eigImage, tempImage, keyButtonCorner, &keyButtonCornerCount, 0.03, 7, NULL, 10, 0);
	cvFindCornerSubPix (grayImage, keyButtonCorner, keyButtonCornerCount,cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
	/*for(int i = 0 ; i < 316 ; i++)
		cvCircle(srcImage, cvPointFrom32f(keyButtonCorner[i]), 4, CV_RGB(255, 0, 0)); */
	//cvShowImage("srcsc", srcImage);
	initKeyButtonCorner();
	cvReleaseImage(&eigImage);
	cvReleaseImage(&tempImage);
	cvReleaseImage(&grayImage);
	cvReleaseImage(&dstImage);
}
void PaperKeyboard_TypeA::cornerVerification(IplImage* srcImage){
	CvSize size = cvGetSize(srcImage);
	IplImage* eigImage = cvCreateImage(size, IPL_DEPTH_8U,1);
	IplImage* tempImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* grayImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* veriImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* dstImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* mask2 = cvCreateImage(size, IPL_DEPTH_8U, 1);
	CvRect rect = cvRect(20,200, 700- 40, 400);
	CvPoint2D32f srcQuad[4], dstQuad[4];
	CvMat* warp_matrix = cvCreateMat(3,3, CV_32FC1);
	CvMat* warp_matrix_invert = cvCreateMat(3,3, CV_32FC1);
	CvMat* result = cvCreateMat(3, 1, CV_32FC1);
	CvMat* dst = cvCreateMat(3, 1,CV_32FC1);

	int keyButtonCornerCount = 316;
	
	cvCvtColor(srcImage, grayImage, CV_BGR2GRAY);
	cvSetImageROI(grayImage, rect);
	cvSetImageROI(mask, rect);
	cvSetImageROI(dstImage, rect);
	cvSetImageROI(mask2, rect);
	
	cvInRangeS(grayImage, cvScalar(150, 150, 150), cvScalar(255, 255, 255), mask);
	cvCopy(mask, mask2);
	cvFloodFill(mask, cvPoint(20, 20), cvScalar(0, 0, 0));
	cvXor(mask2, mask, dstImage);
	cvGoodFeaturesToTrack(dstImage, eigImage, tempImage, keyButtonCorner, &keyButtonCornerCount, 0.01, 7, NULL, 7, 0);
	cvFindCornerSubPix (dstImage, keyButtonCorner, keyButtonCornerCount,cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
	cvResetImageROI(dstImage);
	//cvShowImage("dstIMage", dstImage);
	for(int i =0 ; i < 316 ; i++){
		keyButtonCorner[i].x += rect.x;
		keyButtonCorner[i].y += rect.y;
	}
	
	initKeyButtonCorner();
	
	srcQuad[Direction_1].x = keyButtonCorner[315].x+20;
	srcQuad[Direction_1].y = keyButtonCorner[315].y-20;
	srcQuad[Direction_5].x = keyButtonCorner[31].x + 20;
	srcQuad[Direction_5].y = keyButtonCorner[31].y + 20;
	
	srcQuad[Direction_7].x = keyButtonCorner[0].x - 20;
	srcQuad[Direction_7].y = keyButtonCorner[0].y + 20;
	
	srcQuad[Direction_11].x = keyButtonCorner[290].x - 20;
	srcQuad[Direction_11].y = keyButtonCorner[290].y - 20;


	dstQuad[Direction_1].x = 700;
	dstQuad[Direction_1].y = 0;
	dstQuad[Direction_5].x = 700;
	dstQuad[Direction_5].y = 600;
	dstQuad[Direction_7].x = 0;
	dstQuad[Direction_7].y = 600;
	dstQuad[Direction_11].x = 0;
	dstQuad[Direction_11].y = 0;
	cvGetPerspectiveTransform(srcQuad, dstQuad, warp_matrix);
	
	cvWarpPerspective(dstImage, veriImage, warp_matrix);
	detectKeyButtonCorner(veriImage);
	cvInvert(warp_matrix, warp_matrix_invert);
	for(int i = 0 ; i < 316 ; i++){
		cvmSet(dst, 0, 0, keyButtonCorner[i].x);  
		cvmSet(dst, 1, 0, keyButtonCorner[i].y);
		cvmSet(dst, 2, 0, 1);

		cvMatMul(warp_matrix_invert, dst, result);
		float t = cvmGet(result, 2,0);
		keyButtonCorner[i].x = cvmGet(result, 0,0)/t ;
		keyButtonCorner[i].y = cvmGet(result, 1,0)/t ;
	}
	cvResetImageROI(srcImage);
	cvResetImageROI(mask);
	cvReleaseImage(&eigImage);
	cvReleaseImage(&tempImage);
	cvReleaseImage(&grayImage);
	cvReleaseImage(&veriImage);
	cvReleaseImage(&dstImage);
	cvReleaseImage(&mask);
	cvReleaseImage(&mask2);
	cvReleaseMat(&warp_matrix);
	cvReleaseMat(&warp_matrix_invert);
	cvReleaseMat(&result);
	cvReleaseMat(&dst);	
}
void PaperKeyboard_TypeA::setDirectionKeyButtonArea(CvPoint2D32f* corners, int startIndex, int next, int index){
	int cornerCount = next*2;
	int keyWidth, keyHeight, keyX, keyY;
	float width1, width2, height1, height2;
	width1 = corners[startIndex + 1].x - corners[startIndex].x;
	width2 = corners[startIndex +cornerCount+1 ].x - corners[startIndex + cornerCount].x;
	width1 > width2 ? keyWidth = width1 : keyWidth =  width2;

	height1 = corners[startIndex ].y - corners[startIndex+ cornerCount].y;
	height2 = corners[startIndex +1].y - corners[startIndex +cornerCount + 1].y;
	height1 > height2 ? keyHeight = height1 : keyHeight = height2;
	
	keyX = (corners[startIndex + cornerCount].x + corners[startIndex].x)/2;
	keyY = (corners[startIndex + cornerCount].y + corners[startIndex + cornerCount + 1].y)/2;

	keyButton[index].setKeyLocation(keyX, keyY, keyWidth, keyHeight);

}
void PaperKeyboard_TypeA::setKeyButtonArea(CvPoint2D32f* corners, int startIndex, int keyCount){
	int cornerCount = keyCount*2;
	int keyWidth, keyHeight, keyX, keyY;
	float width1, width2, height1, height2;
	int index = 0;
	static int line = 0;

	for(int i =startIndex  ; i < cornerCount + startIndex ; i +=2){
		width1 = corners[i+1].x - corners[i].x;
		width2 = corners[i+cornerCount + 1].x - corners[i + cornerCount].x;
		width1 > width2 ? keyWidth = width1 : keyWidth =  width2;

		height1 = corners[i].y - corners[i + cornerCount].y;
		height2 = corners[i+1].y - corners[i + cornerCount+1].y;
		height1 > height2 ? keyHeight = height1 : keyHeight = height2;

		keyX = (corners[i + cornerCount].x + corners[i].x)/2;
		keyY = (corners[i + cornerCount].y + corners[i + cornerCount + 1].y)/2;

		keyButton[(startIndex/4) + index++].setKeyLocation(keyX, keyY, keyWidth, keyHeight);
	}
	line++;

}

void PaperKeyboard_TypeA::setKeyButton(IplImage* srcImage){
	CvPoint2D32f srcQuad[4], dstQuad[4];
	IplImage* perspectiveTransImage = cvCreateImage(cvSize(700,600), IPL_DEPTH_8U, 3);
	CvMat* warp_matrix = cvCreateMat(3,3, CV_32FC1);
	CvMat* warp_matrix_invert = cvCreateMat(3,3, CV_32FC1);
	CvMat* result = cvCreateMat(3, 1, CV_32FC1);
	CvMat* dst = cvCreateMat(3, 1,CV_32FC1);
	
	//CvPoint2D32f keyButtonCorner[316];

	cvSetImageROI(srcImage, FingerKeyboard::selectedPaperArea);
	sortPaperKeyboardCorner();
	
	srcQuad[Direction_1] = keyboardCorner[1];
	srcQuad[Direction_5] = keyboardCorner[2];
	srcQuad[Direction_7] = keyboardCorner[3];
	srcQuad[Direction_11] = keyboardCorner[0];

	dstQuad[Direction_1].x = 700;
	dstQuad[Direction_1].y = 0;
	dstQuad[Direction_5].x = 700;
	dstQuad[Direction_5].y = 600;
	dstQuad[Direction_7].x = 0;
	dstQuad[Direction_7].y = 600;
	dstQuad[Direction_11].x = 0;
	dstQuad[Direction_11].y = 0;	

	cvGetPerspectiveTransform(srcQuad, dstQuad, warp_matrix);
	cvWarpPerspective(srcImage, perspectiveTransImage, warp_matrix);
	
	cornerVerification(perspectiveTransImage);
	cvInvert(warp_matrix, warp_matrix_invert);
	
	for(int i = 0 ; i < 316 ; i++){
		cvmSet(dst, 0, 0, keyButtonCorner[i].x);  
		cvmSet(dst, 1, 0, keyButtonCorner[i].y);
		cvmSet(dst, 2, 0, 1);

		cvMatMul(warp_matrix_invert, dst, result);
		float t = cvmGet(result, 2,0);
		keyButtonCorner[i].x = cvmGet(result, 0,0)/t + FingerKeyboard::selectedPaperArea.x;
		keyButtonCorner[i].y = cvmGet(result, 1,0)/t  + FingerKeyboard::selectedPaperArea.y;
	}

	setKeyButtonArea(keyButtonCorner, 0, 16);
	setKeyButtonArea(keyButtonCorner, 64, 14);
	setKeyButtonArea(keyButtonCorner, 120, 14);
	setKeyButtonArea(keyButtonCorner, 176, 13);
	setKeyButtonArea(keyButtonCorner, 228, 12);
	setKeyButtonArea(keyButtonCorner, 276, 7);
	setDirectionKeyButtonArea(keyButtonCorner, 306, 2, 77);
	setDirectionKeyButtonArea(keyButtonCorner, 304, 4, 76);
	setDirectionKeyButtonArea(keyButtonCorner, 308, 3, 78);
	
	cvResetImageROI(srcImage);
	
	
	IplImage* buttonShowImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U,3);
	cvCopy(srcImage, buttonShowImage);
	showKeyButton(buttonShowImage); 
	// 키영역 확인 시 나와야 할 부분

	cvReleaseImage(&perspectiveTransImage);
	cvReleaseMat(&warp_matrix);
	cvReleaseMat(&warp_matrix_invert);
	cvReleaseMat(&result);
	cvReleaseMat(&dst);
}
void PaperKeyboard_TypeA::showKeyButton(IplImage* srcImage){
	for(int i =0 ; i < 16 ; i++)
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(255,0,0), 2);
	for(int i =16 ; i < 30 ; i++)
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(255,255,0), 2);
	for(int i =30 ; i < 44 ; i++){
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(0, 255, 255), 2);
	}	
	for(int i =44 ; i < 57 ; i++){
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(0,0,0), 2);
	}
	for(int i =57 ; i < 69 ; i++)
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(170,10,120), 2);
	for(int i =69 ; i < 76 ; i++)
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(255,0,255), 2);
	for(int i =76 ; i < 80 ; i++)
		cvRectangleR(srcImage, keyButton[i].keyLocation, CV_RGB(255,255,255), 2);
	//cvShowImage("button", srcImage); // 화면에 뿌려주는 부분
	cvCopy(srcImage, FingerKeyboard::buttonImage);
	cvShowImage("Button", FingerKeyboard::buttonImage);
	//cvSaveImage("button.jpg", srcImage);
}
PaperKeyboard_TypeA::~PaperKeyboard_TypeA(){
	delete[] keyButton;
	delete[] keyButtonCorner;
}
