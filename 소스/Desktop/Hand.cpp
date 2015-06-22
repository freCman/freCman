#include"Header.h"
Hand::Hand(){
	for(int i = 0 ; i < 5 ; i++){
		finger[i] = new Finger();
		finger[i]->setPrevFinger();
	}
	prevDetectFingerCount = 0;
	detectFingerCount = 0;
	handStorage = cvCreateMemStorage(0);
	handContour = NULL;
}
bool Hand::isPressKey(KeyButton button){
	for(int i = 0 ; i < 5; i++)
		if(this->finger[i]->inButton(button))
			return true;
	return false;
}
void Hand::getHandDefect(){
	
	this->handContour = cvApproxPoly(handContour,sizeof(CvContour), handStorage, CV_POLY_APPROX_DP, 1);
	hull = cvConvexHull2(this->handContour, 0, CV_CLOCKWISE, 0);
	defect = cvConvexityDefects(this->handContour,this->hull,this->handStorage );

}
void Hand::setFingerMotionVector(){
	for(int i = 0 ; i < 5 ; i++)
		this->finger[i]->setMotionVector();
}
void Hand::resetFingerAttribute(){
	for(int i =0 ;  i < 5 ; i++)
		this->finger[i]->initAttribute();
}
void Hand::setPrevDetectFingerCount(){
	prevDetectFingerCount = detectFingerCount;

}

void Hand::setValidMotionVector(){
	for(int i = 0 ; i < 5 ; i++)
		finger[i]->setValidMotion();

}
void Hand::correctPrevFingerTipIndex(){
	Finger tempFingerTip[5];
	int x, y;
	for(int i = 0 ; i < 5 ; i++)
		tempFingerTip[i] = *(this->finger[i]->prevFinger);
	for(int i =0 ; i < 5 ; i++){
		for(int j = 0 ; j < 5 ; j++){
			x = CV_IABS(this->finger[i]->currFingerTip.x - tempFingerTip[j].currFingerTip.x);
			y = CV_IABS(this->finger[i]->currFingerTip.y - tempFingerTip[j].currFingerTip.y);
			if(x < 25 && y < 35){
				this->finger[i]->fingerCopy(tempFingerTip[j]);
				break;
			}
			else if(j == 4){
				this->finger[i]->resetFinger();
				/*this->finger[i]->prevFinger->currFingerTip.x = -1;
				this->finger[i]->prevFinger->currFingerTip.y = -1;*/
			}
		}
	}
}
int Hand::determineMotion(){
	double maxMotion = 0;
	int count = 0, index;
	for(int i = 0 ; i < 5 ; i++){
		if(this->finger[i]->motionVectorSum > 50 ){
			count++;
			if(this->finger[i]->motionVectorSum > maxMotion){
				maxMotion = this->finger[i]->motionVectorSum;
				index = i;
			}
		}
	}
	if(count == 1)
		return index;
	else
		return -1;
}
bool Hand::isAllFingerDownMotion(){
	int downCount = 0;
	for(int i = 0 ; i < this->prevDetectFingerCount ; i++)
		if(this->finger[i]->downOrientMotion && this->finger[i]->motionVectorSum > 50)	//motionVector -> motionVectorSum
			downCount++;
	if(downCount > 0 &&downCount >= this->prevDetectFingerCount -1)
		return false;
	return true;
}
void Hand::arrayMemSet(){
	memset(defectArray, 0, 100);
}
void Hand::convertArray(){
	cvCvtSeqToArray(this->defect, this->defectArray, CV_WHOLE_SEQ);

}
int Hand::getDefectTotal(){
	return this->defect->total;
}
Hand::~Hand(){
	cvReleaseMemStorage(&this->handStorage);
	for(int i = 0; i < 5 ; i++){
		delete this->finger[i];
	}
}