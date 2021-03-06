#include"Header.h"


//FILE *f = fopen("vector_data.txt","w");

int temp_key = -1;
int temp_index;
bool temp_flag= false;
ImageProcessor::ImageProcessor(){
	size = cvSize(640, 480);
	transImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
	splitImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	backProImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	backSplitImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	handMasking = cvCreateImage(size, IPL_DEPTH_8U, 1);
	storage = cvCreateMemStorage(0);
	firstContour = NULL;
	//	showGraph = cvCreateImage(cvSize(800, 600), IPL_DEPTH_8U, 3);
//	videoWriter = cvCreateVideoWriter("Test.avi", CV_FOURCC('D', 'I', 'V', 'X'), 10.0, size);
	this->inputAvailable = false;

	this->pressedKey = -1;
	this->holdKey = -1;

}
void ImageProcessor::paperAreaDraggingImage(IplImage* srcImage, MouseListener* mouseListener){
	cvSetImageROI(srcImage, mouseListener->getMouseDragArea());
	cvAddS(srcImage, cvScalar(200, 0, 0), srcImage);
	cvRectangleR(srcImage, cvRect(0, 0, mouseListener->getMouseDragArea().width, mouseListener->getMouseDragArea().height), CV_RGB(51, 153, 255),2);
	//cvXorS(srcImage, cvScalarAll(255), srcImage, 0);
	cvResetImageROI(srcImage);
}
void ImageProcessor::cameraCalibration(IplImage* srcImage, PaperKeyboard* paperKeyboard){
	ifstream fin;
	double x, y;
	CvSize size = cvGetSize(srcImage);
	int cornerCount = 304;
	CvMat* mWorld = cvCreateMat(2, cornerCount, CV_32F);
	CvMat* mScreen = cvCreateMat(2, cornerCount, CV_32F);
	CvMat* worldPoint = cvCreateMat(3, cornerCount, CV_32F);
	CvMat* screenPoint = cvCreateMat(2, cornerCount, CV_32F);
	CvMat* mVerifyScreen = cvCreateMat(1, cornerCount, CV_64FC2);
	CvMat* mVerifyWorld = cvCreateMat(1, cornerCount, CV_64FC3);

	CvMat* pointCount = cvCreateMat(1, 1, CV_32S);

	CvMat* rotationVector = cvCreateMat(1, 3, CV_32F);
	CvMat* translationVector = cvCreateMat(1, 3, CV_32F);
	CvMat* rotationMat = cvCreateMat(3, 3, CV_64F);
	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER+ CV_TERMCRIT_EPS, 30, DBL_EPSILON);
	double reError =0;

	IplImage* co = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage* co1 = cvCreateImage(size, IPL_DEPTH_8U, 3);
	fin.open("realworldcorner.txt");
	if(!fin){
		cout<<"File Open Error"<<endl;
		return;
	}

	for(int i = 0 ; i < cornerCount ; i++){
		fin>>x;
		fin>>y;

		cvmSet(mWorld, 0, i, x);
		cvmSet(mWorld, 1, i,  CV_IABS((y-11)));
		cvmSet(mScreen, 0, i, paperKeyboard->getKeyButtonCorner(i).x);
		cvmSet(mScreen, 1, i, paperKeyboard->getKeyButtonCorner(i).y);

		CV_MAT_ELEM(*worldPoint, float, 0, i) = CV_MAT_ELEM(*mWorld, float, 0, i);
		CV_MAT_ELEM(*worldPoint, float, 1, i) = CV_MAT_ELEM(*mWorld, float, 1, i);
		CV_MAT_ELEM(*worldPoint, float, 2, i) = 0.0;

		CV_MAT_ELEM(*screenPoint, float, 0, i) = CV_MAT_ELEM(*mScreen, float, 0, i);
		CV_MAT_ELEM(*screenPoint, float, 1, i) = CV_MAT_ELEM(*mScreen, float, 1, i);
	}

	cvSetReal1D(pointCount, 0, cornerCount);

	reError =  cvCalibrateCamera2(worldPoint, screenPoint, pointCount, size,
		FingerKeyboard::cameraMat,  FingerKeyboard::distortionCoeffs, NULL, NULL, 0,criteria);

	/*if(reError > 2){
	cout<<reError<<endl;
	CurrentMode::state = SET_ROI;
	return;
	}*/
	FingerKeyboard::camCalibrateWriter = cvOpenFileStorage("camcalib.xml", 0, CV_STORAGE_WRITE);
	cvConvertPointsHomogeneous(mWorld, mVerifyWorld);

	cvFindExtrinsicCameraParams2(mWorld, mScreen,  FingerKeyboard::cameraMat,  FingerKeyboard::distortionCoeffs,
		rotationVector, translationVector, 0);

	cvProjectPoints2(mVerifyWorld, rotationVector, translationVector,  FingerKeyboard::cameraMat,  FingerKeyboard::distortionCoeffs, mVerifyScreen);
	cvWrite(FingerKeyboard::camCalibrateWriter, "cameraMat",  FingerKeyboard::cameraMat);
	cvWrite(FingerKeyboard::camCalibrateWriter, "distortionCoeffs",  FingerKeyboard::distortionCoeffs);

	//IplImage* dstImage = cvCreateImage(size, srcImage->depth, srcImage->nChannels);
	//cvUndistort2(srcImage, dstImage, cameraMat, distortionCoeffs);
	//cvShowImage("Dst", dstImage);
	cout<<reError<<endl;
	FingerKeyboard::isCamCalibrate = true;
	CurrentMode::state = OTHER;
	cvReleaseFileStorage(&FingerKeyboard::camCalibrateWriter);
	cvReleaseMat(&mScreen);
	cvReleaseMat(&mVerifyScreen);
	cvReleaseMat(&mWorld);
	cvReleaseMat(&mVerifyWorld);
	cvReleaseMat(&rotationMat);
	cvReleaseMat(&rotationVector);
	cvReleaseMat(&translationVector);
	cvReleaseMat(&pointCount);
	cvReleaseMat(&worldPoint);
	cvReleaseMat(&screenPoint);
}
void ImageProcessor::createSkinColorHistogram(IplImage* srcImage){
	CvRect catchROI = cvRect(420 - 70, 260, 140, 140);

	int histSize = 8;
	float max;
	float valueRange[] = {40, 180};
	float* ranges[] = {valueRange};

	FingerKeyboard::pHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1);

	cvCvtColor(srcImage, this->transImage, CV_BGR2YCrCb);
	cvSplit(transImage, 0, splitImage, 0, 0);
	cvSetImageROI(splitImage, catchROI);
	cvCalcHist(&splitImage, FingerKeyboard::pHist, 0);
	cvGetMinMaxHistValue(FingerKeyboard::pHist, 0, &max, 0, 0);
	cvScale(FingerKeyboard::pHist->bins, FingerKeyboard::pHist->bins, max? 255/max : 0);
	cvResetImageROI(splitImage);

}
void ImageProcessor::getHandBinaryImage(IplImage* srcImage){
	cvCvtColor(srcImage, transImage, CV_BGR2YCrCb);
	cvSplit(transImage, 0, splitImage, 0, 0);

	cvCalcBackProject(&splitImage, backProImage, FingerKeyboard::pHist);
	cvInRangeS(transImage, cvScalar(0, 133, 77), cvScalar(255, 173, 127), handMasking);
	cvSplit(FingerKeyboard::bgImage, 0, backSplitImage, 0, 0);

	cvAbsDiff(backSplitImage, splitImage, splitImage);
	cvThreshold(splitImage, splitImage, 7, 255, CV_THRESH_BINARY);
	
	cvAnd(splitImage, handMasking, splitImage);
	//cvShowImage("split", splitImage);
	//cvShowImage("backPro", backProImage);
	cvAnd(backProImage, splitImage, backProImage);

	cvDilate(backProImage, backProImage, 0, 2);
	cvErode(backProImage, backProImage, 0,6);
}

void ImageProcessor::detectHandContour(IplImage* srcImage, PaperKeyboard* paperKeyboard, Hand* userHand[]){
	int i,j;
	double maxArea,secondArea,area;
	firstContour = NULL;
	getHandBinaryImage(srcImage);
	cvSetImageROI(backProImage, FingerKeyboard::selectedPaperArea);

	cvShowImage("back", backProImage); 
	maxArea = 0;
	int nc = cvFindContours(backProImage, storage, &firstContour, sizeof(CvContour), CV_RETR_TREE);
	if(nc> 0){
		for(CvSeq* c = firstContour ; c != NULL ; c = c->h_next){
			area = cvContourArea(c, CV_WHOLE_SEQ);
			if(area> maxArea){
				maxArea = area;
				userHand[0]->handContour = c;
			}
		}
		secondArea = 0;
		for(CvSeq* c = firstContour ; c != NULL ; c = c->h_next){
			area = cvContourArea(c, CV_WHOLE_SEQ);
			if(area < maxArea && area > secondArea){
				secondArea = area;
				userHand[1]->handContour = c;
			}
		}
		if(maxArea > 50000){
			cout<<"don't overlap two hand"<<endl;

		}
		else if(maxArea < 40000 && maxArea> 3000 && secondArea < 5000){
			userHand[0]->getHandDefect();
			for(;userHand[0]->defect; userHand[0]->defect = userHand[0]->defect->h_next){
				if(userHand[0]->getDefectTotal() == 0)
					continue;
				userHand[0]->convertArray();
				determinSingleHandFingerTip(userHand[0]);
				for(int i =0 ; i < userHand[0]->detectFingerCount ; i++)
					cvCircle(srcImage, userHand[0]->finger[i]->currFingerTip, 3, CV_RGB(255, 0, 0), 2);
			}
		}
		else if(secondArea> 3000 && secondArea < 40000){
			userHand[0]->getHandDefect();
			userHand[1]->getHandDefect();
			for(;userHand[0]->defect || userHand[1]->defect ; userHand[0]->defect = userHand[0]->defect->h_next, userHand[1]->defect = userHand[1]->defect->h_next){
				if(userHand[0]->getDefectTotal() == 0 || userHand[1]->getDefectTotal() == 0)
					continue;
				userHand[0]->convertArray();
				userHand[1]->convertArray();
				determinDoubleHandFingerTip(userHand);
				for(int i =0 ; i < userHand[0]->detectFingerCount ; i++)
					cvCircle(srcImage, userHand[0]->finger[i]->currFingerTip, 3, CV_RGB(255, 0,0 ), 2);
				for(int i =0 ; i < userHand[1]->detectFingerCount ; i++)
					cvCircle(srcImage, userHand[1]->finger[i]->currFingerTip, 3, CV_RGB(0, 0,255 ), 2);
			}
		}
		userHand[0]->arrayMemSet();
		userHand[1]->arrayMemSet();

		//Calculate motion vector and determine user finger motion 
	
		
		userHand[0]->correctPrevFingerTipIndex();
		userHand[1]->correctPrevFingerTipIndex();

		userHand[0]->setValidMotionVector();
		userHand[1]->setValidMotionVector();

		userHand[0]->setFingerMotionVector();
		userHand[1]->setFingerMotionVector();

		if(pressedKey != -1 && paperKeyboard->keyButton[pressedKey].getKeyState() == KEY_STATE_PRESS){
			if(inputAvailable){
				for(i = 0 ; i < 2 ; i++){
					if(userHand[i]->isPressKey(paperKeyboard->keyButton[pressedKey])){
						if(holdKey == -1){
							paperKeyboard->keyButton[pressedKey].setHold();
							this->inputStartPoint = FingerKeyboard::captureFrame +5;
							this->inputAvailable = false;

							holdKey = pressedKey;
						}
						break;
					}
					if(i== 1){
						paperKeyboard->keyButton[pressedKey].setNone();
						paperKeyboard->keyButton[pressedKey].keyAction();
						pressedKey = -1;
					}
				}
			}
			else{
				for(i = 0 ; i < 2 ; i++){
					if(userHand[i]->isPressKey(paperKeyboard->keyButton[pressedKey]))
						break;
					if(i== 1){
						paperKeyboard->keyButton[pressedKey].setNone();
						paperKeyboard->keyButton[pressedKey].keyAction();
						pressedKey = -1;
					}
				}
			}
		}

		if(holdKey != -1 && paperKeyboard->keyButton[holdKey].getKeyState() == KEY_STATE_PRESS_HOLD){
			if(inputAvailable){
				for(i = 0 ; i < 2 ; i++){
					if(userHand[i]->isPressKey(paperKeyboard->keyButton[holdKey])){
						paperKeyboard->keyButton[holdKey].keyAction();
						break;
					}
					else if(i==1)
						paperKeyboard->keyButton[holdKey].setRelease();
				}
			}
			else{
				for(i =0 ; i < 2 ; i++){
					if(userHand[i]->isPressKey(paperKeyboard->keyButton[holdKey]))
						break;
					else if(i == 1)
						paperKeyboard->keyButton[holdKey].setRelease();
				}
			}
		}
		else if(paperKeyboard->keyButton[holdKey].getKeyState() == KEY_STATE_RELEASE){
			paperKeyboard->keyButton[holdKey].keyAction();
			paperKeyboard->keyButton[holdKey].setNone();
			holdKey = -1;
			//pressedKey = -1;
		}
		for(i = 0 ; i < 2 ; i++){
			if(userHand[i]->getAllFingerDownMotion()){
				pressFinger = userHand[i]->determinMotion();
				if(pressFinger != -1 && inputAvailable){
					temp_flag =	keyEvent(userHand[i]->finger[pressFinger]->currFingerTip,(i*5) + pressFinger, paperKeyboard);
					if(temp_flag){
						//this->inputAvailable = false;
						this->inputStartPoint = FingerKeyboard::captureFrame;
					}
				}
			}
		}

	}
	/* record motion vector, motion vector sum, finger tip coordinate(current & previous), orientate, is valid Value, key Button id */
	/*fprintf(f, "%d\n\n", FingerKeyboard::captureFrame);	

	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", i);	

	fprintf(f, "\n");
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->prevFinger->currFingerTip.x);	


	fprintf(f, "\n");
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->prevFinger->currFingerTip.y);	

	fprintf(f, "\n");
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->currFingerTip.x);	


	fprintf(f, "\n");
	for(int i = 0 ; i < 10 ; i++){
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->currFingerTip.y);	
	}

	fprintf(f, "\n   ");	
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6.2f", userHand[i/5]->finger[i%5]->motionVector);

	fprintf(f, "\n   ");	
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6.2f", userHand[i/5]->finger[i%5]->motionVectorSum);

	fprintf(f, "\n");	
	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->downOrientMotion);
	fprintf(f, "\n");	

	for(int i = 0 ; i < 10 ; i++)
		fprintf(f, "%6d", userHand[i/5]->finger[i%5]->validMotion);
	fprintf(f, "\n");	
	for(int i = 0 ; i < 10 ; i++){
		if(temp_flag == true && temp_index == i){
			fprintf(f, "%2c%d)%d", '(',temp_key,1);
		}
		else
			fprintf(f, "%6d", 0);
	}
	fprintf(f, "\n");	
	fprintf(f, "inputAvailable : %d\n", this->inputAvailable);
	fprintf(f, "pressed Key Id : %d\n", this->pressedKey);
	fprintf(f, "pressed Key State : %d\n", paperKeyboard->keyButton[this->pressedKey].getKeyState());
	fprintf(f, "holdKey Id : %d\n", this->holdKey);
	fprintf(f, "holdKey State : %d\n", paperKeyboard->keyButton[this->holdKey].getKeyState());*/
	if(this->inputStartPoint != 0 && this->inputStartPoint + 5< FingerKeyboard::captureFrame)
		inputAvailable = true;	//�Է� ����
	temp_flag= false;

	userHand[0]->setPrevDetectFingerCount();
	userHand[1]->setPrevDetectFingerCount();

	userHand[0]->resetFingerAttribute();
	userHand[1]->resetFingerAttribute();
	FingerKeyboard::captureFrame++;
	cvResetImageROI(backProImage);
	//cvWriteFrame(this->videoWriter, srcImage);
}

void ImageProcessor::determinDoubleHandFingerTip(Hand* userHand[]){
	int k = 0;
	CvConvexityDefect decendingOrder[2][100];
	CvConvexityDefect tmp;
	int total[2];
	total[0] = userHand[0]->getDefectTotal();
	total[1] = userHand[1]->getDefectTotal();
	userHand[0]->detectFingerCount = 0;
	userHand[1]->detectFingerCount = 0;

	for(int i = 0 ; i < 2 ; i++){
		for(int j = 0 ; j < total[i] ; j++){
			if(userHand[i]->defectArray[j].depth > 30)
				decendingOrder[i][userHand[i]->detectFingerCount++] = userHand[i]->defectArray[j];
		}
	}

	for(int k = 0 ; k < 2 ; k++){
		for(int i = 0 ; i < userHand[k]->detectFingerCount ; i++){
			for(int j = 0 ; j < (userHand[k]->detectFingerCount -1) -i ; j++){
				if(decendingOrder[k][j].start->y < decendingOrder[k][j+1].start->y){
					tmp = decendingOrder[k][j];
					decendingOrder[k][j] = decendingOrder[k][j+1];
					decendingOrder[k][j+1] = tmp;
				}	
			}
		}
	}
	//0�� ����		0�� �޼� ����.
	if(userHand[0]->detectFingerCount > 0 && userHand[1]->detectFingerCount > 0){
		if(decendingOrder[0][0].start->x < decendingOrder[1][0].start->x){
			for(int handSide = 0 ; handSide < 2 ; handSide++){
				k = 0;
				if(userHand[handSide]->detectFingerCount >= 4){
					userHand[handSide]->detectFingerCount= 5;
					for(int i = 0 ; i <total[handSide]&& k != 4; i++){

						for(int j = 0 ; j < userHand[handSide]->detectFingerCount ; j++){
							if(userHand[handSide]->defectArray[i].start == decendingOrder[handSide][j].start){
								userHand[handSide]->finger[k]->currFingerTip.x = (userHand[handSide]->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
								userHand[handSide]->finger[k++]->currFingerTip.y = (userHand[handSide]->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;
								if(k == 4){
									userHand[handSide]->finger[k]->currFingerTip.x = (userHand[handSide]->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
									userHand[handSide]->finger[k]->currFingerTip.y = (userHand[handSide]->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
									break;
								}
								break;
							}
						}
					}

				}
				else{
					for(int i = 0 ; i <total[handSide]&& k != userHand[handSide]->detectFingerCount; i++){
						for(int j = 0 ; j < userHand[handSide]->detectFingerCount ; j++){
							if(userHand[handSide]->defectArray[i].start == decendingOrder[handSide][j].start){
								userHand[handSide]->finger[k]->currFingerTip.x = (userHand[handSide]->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
								userHand[handSide]->finger[k++]->currFingerTip.y = (userHand[handSide]->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;
								if(k == userHand[handSide]->detectFingerCount){
									userHand[handSide]->finger[k]->currFingerTip.x = (userHand[handSide]->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
									userHand[handSide]->finger[k]->currFingerTip.y = (userHand[handSide]->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
									break;
								}
								break;
							}
						}
					}
					userHand[handSide]->detectFingerCount++;
				}
			}
		}
		//1�� �޼�
		else{
			for(int handSide = 0 ; handSide < 2 ; handSide++){
				k = 0;
				if(userHand[(handSide+1)%2]->detectFingerCount >= 4){
					userHand[(handSide+1)%2]->detectFingerCount= 5;
					for(int i = 0 ; i <total[(handSide+1)%2]&& k != 4; i++){
						for(int j = 0 ; j < userHand[(handSide+1)%2]->detectFingerCount ; j++){
							if(userHand[(handSide+1)%2]->defectArray[i].start == decendingOrder[(handSide+1)%2][j].start){
								userHand[handSide]->finger[k]->currFingerTip.x = (userHand[(handSide+1)%2]->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
								userHand[handSide]->finger[k++]->currFingerTip.y = (userHand[(handSide+1)%2]->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;

								if(k == 4){
									userHand[handSide]->finger[k]->currFingerTip.x = (userHand[(handSide+1)%2]->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
									userHand[handSide]->finger[k]->currFingerTip.y = (userHand[(handSide+1)%2]->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
									break;
								}
								break;
							}
						}
					}

				}
				else{
					for(int i = 0 ; i <total[(handSide+1)%2]&&k != userHand[handSide]->detectFingerCount; i++){
						for(int j = 0 ; j < userHand[(handSide+1)%2]->detectFingerCount ; j++){
							if(userHand[(handSide+1)%2]->defectArray[i].start == decendingOrder[(handSide+1)%2][j].start){
								userHand[handSide]->finger[k]->currFingerTip.x = (userHand[(handSide+1)%2]->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
								userHand[handSide]->finger[k++]->currFingerTip.y = (userHand[(handSide+1)%2]->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;

								if(k == userHand[(handSide+1)%2]->detectFingerCount){
									userHand[handSide]->finger[k]->currFingerTip.x = (userHand[(handSide+1)%2]->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
									userHand[handSide]->finger[k]->currFingerTip.y = (userHand[(handSide+1)%2]->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
									break;
								}
								break;
							}
						}
					}
					userHand[(handSide+1)%2]->detectFingerCount++;
				}

			}
			int temp;
			temp = userHand[0]->detectFingerCount;
			userHand[0]->detectFingerCount = userHand[1]->detectFingerCount;
			userHand[1]->detectFingerCount = temp;
		}
	}
	memset(decendingOrder[0], 0 , 100);
	memset(decendingOrder[1], 0, 100);
}
void ImageProcessor::determinSingleHandFingerTip(Hand* userHand){
	CvConvexityDefect decendingOrder[100];
	int k = 0;
	int total = userHand->getDefectTotal();
	userHand->detectFingerCount = 0;
	for(int j = 0 ; j < total ; j++)
		if(userHand->defectArray[j].depth > 30)
			decendingOrder[userHand->detectFingerCount++] = userHand->defectArray[j];

	if(userHand->detectFingerCount == 0)
		return ;

	CvConvexityDefect tmp;
	for(int i = 0 ; i < userHand->detectFingerCount ; i++){
		for(int j = 0 ; j < (userHand->detectFingerCount -1) -i ; j++){
			if(decendingOrder[j].start->y < decendingOrder[j+1].start->y){
				tmp = decendingOrder[j];
				decendingOrder[j] = decendingOrder[j+1];
				decendingOrder[j+1] = tmp;
			}	
		}
	}
	k = 0;
	if(userHand->detectFingerCount >= 4){
		userHand->detectFingerCount = 5;
		for(int i = 0 ; i <total &&  k != 4 ; i++){
			for(int j = 0 ; j < userHand->detectFingerCount; j++){
				if(userHand->defectArray[i].start == decendingOrder[j].start){
					userHand->finger[k]->currFingerTip.x = (userHand->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
					userHand->finger[k++]->currFingerTip.y = (userHand->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;
					if(k == 4){
						userHand->finger[k]->currFingerTip.x = (userHand->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
						userHand->finger[k]->currFingerTip.y = (userHand->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
						break;
					}
					break;
				}
			}
		}

	}
	else{
		for(int i = 0 ; i <total && k != userHand->detectFingerCount;i++)
			for(int j = 0 ; j < userHand->detectFingerCount ; j++)
				if(userHand->defectArray[i].start == decendingOrder[j].start){
					userHand->finger[k]->currFingerTip.x = (userHand->defectArray[i].start)->x + FingerKeyboard::selectedPaperArea.x;
					userHand->finger[k++]->currFingerTip.y = (userHand->defectArray[i].start)->y + FingerKeyboard::selectedPaperArea.y;
					if(k == userHand->detectFingerCount){
						userHand->finger[k]->currFingerTip.x = (userHand->defectArray[i].end)->x + FingerKeyboard::selectedPaperArea.x;
						userHand->finger[k]->currFingerTip.y = (userHand->defectArray[i].end)->y + FingerKeyboard::selectedPaperArea.y;
						break;
					}
					break;
				}
				userHand->detectFingerCount++;		
	}
	memset(decendingOrder, 0, 100);
}
bool ImageProcessor::keyEvent(CvPoint fingerLocation, int index, PaperKeyboard* paperKeyboard){
	int t = 1;
	for(int i = 0 ; i < 79 ; i++){
		if(paperKeyboard->keyButton[i].keyLocation.x-t  < fingerLocation.x  && t+paperKeyboard->keyButton[i].keyLocation.x +  paperKeyboard->keyButton[i].keyLocation.width > fingerLocation.x)
			if(paperKeyboard->keyButton[i].keyLocation.y -t < fingerLocation.y &&t+paperKeyboard->keyButton[i].keyLocation.y +  paperKeyboard->keyButton[i].keyLocation.height > fingerLocation.y){
				paperKeyboard->keyButton[i].setPress();
				paperKeyboard->keyButton[i].keyAction();
				pressedKey = paperKeyboard->keyButton[i].getKeyId();
				temp_index = index;
				this->inputAvailable = false;
				temp_key = paperKeyboard->keyButton[i].getKeyId();
				return true;	
			}
	}
	return false;
}

ImageProcessor::~ImageProcessor(){

	//cvReleaseVideoWriter(&videoWriter);
	//fclose(f);
	cvReleaseImage(&transImage);
	cvReleaseImage(&splitImage);
	cvReleaseImage(&backProImage);
	cvReleaseImage(&backSplitImage);
	cvReleaseImage(&handMasking);
	cvReleaseMemStorage(&storage);
}
