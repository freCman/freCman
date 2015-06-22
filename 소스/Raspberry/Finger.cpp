#include"Header.h"
Finger::Finger(){
	this->validMotion = false;
	this->downOrientMotion = false;
	currFingerTip = cvPoint(0, 0);
	//prevFingerTip = cvPoint(0, 0);
	motionVectorSum = 0;
}
void Finger::setPrevFinger(){
	prevFinger = new Finger();
}
void Finger::setValidMotion(){
	double x, y;
	x = CV_IABS(prevFinger->currFingerTip.x - currFingerTip.x);
	y = CV_IABS(prevFinger->currFingerTip.y - currFingerTip.y);
	x < 30 && y < 30 ? this->validMotion = true : this->validMotion = false;

}
bool Finger::isMotion(){
	return this->downOrientMotion && this->motionVector > 15 && this->validMotion;	//motionVector = 35
}
void Finger::initAttribute(){
	prevFinger->currFingerTip.x = currFingerTip.x;
	prevFinger->currFingerTip.y = currFingerTip.y;
	prevFinger->validMotion = this->validMotion;
	prevFinger->downOrientMotion = this->downOrientMotion;
	prevFinger->motionVector = this->motionVector;
	prevFinger->motionVectorSum = this->motionVectorSum;

	this->validMotion = false;
	this->downOrientMotion = false;
	this->currFingerTip.x = -1;
	this->currFingerTip.y = -1;

}
void Finger::resetFinger(){
	prevFinger->currFingerTip.x = -1;
	prevFinger->currFingerTip.y = -1;
	prevFinger->validMotion = false;
	prevFinger->downOrientMotion = false;
	prevFinger->motionVector = 0;
	prevFinger->motionVectorSum = 0;



}
void Finger::fingerCopy(Finger finger){
	this->prevFinger->currFingerTip.x = finger.currFingerTip.x;
	this->prevFinger->currFingerTip.y = finger.currFingerTip.y;
	this->prevFinger->downOrientMotion = finger.downOrientMotion;
	this->prevFinger->validMotion = finger.validMotion;
	this->prevFinger->motionVector = finger.motionVector;
	this->prevFinger->motionVectorSum = finger.motionVectorSum;

}
bool Finger::inButton(KeyButton button){
	int t = 1;
	if(currFingerTip.x > button.keyLocation.x -t && t+currFingerTip.x < button.keyLocation.x + button.keyLocation.width)
		if(currFingerTip.y > button.keyLocation.y -t && t+currFingerTip.y < button.keyLocation.y + button.keyLocation.height)
			return true;
	return false;
}
void Finger::setMotionVector(){
	double x, y;
	x = currFingerTip.x - prevFinger->currFingerTip.x;
	y = currFingerTip.y - prevFinger->currFingerTip.y;
	if(CV_IABS(x) < 10 && CV_IABS(y) < 4){
		this->motionVector = 0;
		this->downOrientMotion = false;
		motionVectorSum = 0;
		return;
	}
	if(y > 0)
		downOrientMotion =true;
	else 
		downOrientMotion = false;
	if(prevFinger->currFingerTip.x == -1)
		this->downOrientMotion = false;
	x *= x;
	x *= 0.2;
	y *= y;
	y *= 20;
	this->motionVector = sqrt(x+y);
	if(motionVector > 150)
		this->motionVector = 0;
	this->isMotion()?motionVectorSum = prevFinger->motionVectorSum + this->motionVector:motionVectorSum = 0;

}
//Finger::~Finger(){
//	delete prevFinger;
//}
