#include"Header.h"
CvRect FingerKeyboard::selectedPaperArea;
CvFileStorage* FingerKeyboard::camCalibrateReader;
CvFileStorage* FingerKeyboard::camCalibrateWriter;
CvMat* FingerKeyboard::cameraMat = cvCreateMat(3, 3, CV_32F);
CvMat* FingerKeyboard::distortionCoeffs = cvCreateMat(1, 4, CV_64F);
bool FingerKeyboard::isCamCalibrate;
IplImage* FingerKeyboard::bgImage;
CvHistogram* FingerKeyboard::pHist;
int FingerKeyboard::captureFrame;
IplImage* FingerKeyboard::buttonImage;
PaperKeyboard* FingerKeyboard::paperKeyboard;
int FingerKeyboard::programSetUp(int camIndex){
	this->camera = new Camera(camIndex, 640, 480);
	if(!this->camera->camCapture)
		return -1;

	//this->camera = new Camera("Test44.avi", 640, 480);
	this->imageProcessor = new ImageProcessor();
	this->mouseListener = new MouseListener();
	this->paperKeyboard = new PaperKeyboard_TypeA();
	for(int i = 0 ; i < 2 ; i++) // Ãß°¡
		userHand[i] = new Hand();

	dstImage = cvCreateImage(this->camera->getResolution(), IPL_DEPTH_8U, 3);
	buttonImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
	bgImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
	captureFrame = 0;
	cvNamedWindow("Program");
	cvSetMouseCallback("Program", this->mouseListener->mouseClickEvent, dstImage);
	
	CurrentMode::state  = SET_ROI;
	
	camCalibrateReader = cvOpenFileStorage("camcalib.xml", 0, CV_STORAGE_READ);
	if(camCalibrateReader){
		cameraMat = (CvMat*)cvReadByName(camCalibrateReader, 0, "cameraMat");
		distortionCoeffs = (CvMat*)cvReadByName(camCalibrateReader, 0, "distortionCoeffs");
		if(cameraMat == NULL || distortionCoeffs == NULL){
			cout<<"false"<<endl;
			isCamCalibrate = false;
		}
		else 
			isCamCalibrate = true;
	}
	return 0;
}


void FingerKeyboard::programRun(){
	IplImage* frame;
	while(1){
		frame = this->camera->getQueryFrame();
		if(!frame)
			continue;
		if(isCamCalibrate){
			cvFlip(frame, dstImage, 1);
	
		}
		else{
			cvFlip(frame, dstImage, 1);
		}
		if(isSettingROI()){
			imageProcessor->paperAreaDraggingImage(dstImage, this->mouseListener);
		}
		//start run
		if(CurrentMode::state ){
			if(CurrentMode::state  == ROI_CONFIRM){
				this->selectedPaperArea = this->mouseListener->getMouseDragArea();
				cvRectangleR(dstImage, this->selectedPaperArea, CV_RGB(51, 153, 255), 2);
			}
			else if(CurrentMode::state == SET_CORNER){
				this->paperKeyboard->setPaperKeyboardCorner(dstImage, this->selectedPaperArea, this->mouseListener);
				
			}
			else if(CurrentMode::state == CORNER_CONFIRM || CurrentMode::state == ADJUST_CORNER){
				cvSetImageROI(dstImage, selectedPaperArea);
				for(int i = 0 ; i < 4 ; i++)
					cvCircle(dstImage, cvPointFrom32f(paperKeyboard->keyboardCorner[i]), 2, CV_RGB(255, 0, 0), 2);
				cvResetImageROI(dstImage);
				if(isAdjustCorner()){
					imageProcessor->paperAreaDraggingImage(dstImage, this->mouseListener);
				}
			}
			else if(CurrentMode::state == CATCH_CORNER){
				cvSetImageROI(dstImage, selectedPaperArea);
				for(int i = 0 ; i  < 4 ; i++)
					cvCircle(dstImage, cvPointFrom32f(paperKeyboard->keyboardCorner[i]), 2, CV_RGB(255,0,0), 2);
				cvResetImageROI(dstImage);
				cvRectangleR(dstImage, this->mouseListener->getMouseDragArea(), CV_RGB(51, 153, 255), 2);
			}
			else if(CurrentMode::state == CORNER_MOVE){
				cvSetImageROI(dstImage, selectedPaperArea);
				for(int i = 0 ; i  < 4 ; i++)
					cvCircle(dstImage, cvPointFrom32f(paperKeyboard->keyboardCorner[i]), 2, CV_RGB(255,0,0), 2);
				cvResetImageROI(dstImage);
			}
			else if(CurrentMode::state == CALC_SPOT){
				paperKeyboard->setKeyButton(dstImage);
				cvCopy(dstImage, bgImage);
				//cvSaveImage("backgroundImage.jpg", dstImage);
				if(isCamCalibrate)
					CurrentMode::state = CATCH_SKIN_COLOR;
				else
					CurrentMode::state = CAM_CALIB;
	
			}
			else if(CurrentMode::state == CAM_CALIB){
				imageProcessor->cameraCalibration(dstImage, this->paperKeyboard);
				CurrentMode::state = CATCH_SKIN_COLOR;
			}
			else if(CurrentMode::state == CATCH_SKIN_COLOR){
				cvCircle(dstImage, cvPoint(2 * 210, 2* 160 -60), 100, CV_RGB(0, 255, 0), 3);
			}
			else if(CurrentMode::state == CALC_HIST){
				cvCvtColor(bgImage, bgImage, CV_BGR2YCrCb);
				
				imageProcessor->createSkinColorHistogram(dstImage);
				CurrentMode::state = HAND_TRACKING;
			}
			else if(CurrentMode::state == HAND_TRACKING){
				imageProcessor->detectHandContour(dstImage, this->paperKeyboard, userHand);
				
			}
		}
		if(CurrentMode::state > ROI_CONFIRM && CurrentMode::state < CATCH_SKIN_COLOR)
			cvRectangleR(dstImage, selectedPaperArea, CV_RGB(0, 230, 0), 2);
		cvShowImage("Program", dstImage);
		if((cvWaitKey(30)) == 27)
			break;
	}

}
bool FingerKeyboard::isAdjustCorner(){
	return CurrentMode::state == ADJUST_CORNER &&
		mouseListener->getMouse()->getRButtonDownState() &&
		mouseListener->isMouseDragSize();
}
bool FingerKeyboard::isSettingROI(){
	return CurrentMode::state  == SET_ROI &&
		mouseListener->getMouse()->getRButtonDownState() &&
		mouseListener->isMouseDragSize();
}
void FingerKeyboard::programExit(){
	delete this->camera;
	delete this->imageProcessor;
	delete this->mouseListener;
	delete paperKeyboard;
	for(int i = 0 ; i < 2 ; i++)
		delete this->userHand[i];

	cvDestroyAllWindows();
	cvReleaseMat(&cameraMat);
	cvReleaseMat(&distortionCoeffs);
	cvReleaseFileStorage(&camCalibrateReader);
	cvReleaseFileStorage(&camCalibrateWriter);
	cvReleaseHist(&pHist);
	cvReleaseImage(&bgImage);
	cvReleaseImage(&dstImage);
	cvReleaseImage(&buttonImage);
}