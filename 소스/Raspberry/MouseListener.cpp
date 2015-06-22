#include"Header.h"
Mouse* MouseListener::mouse = new Mouse();
CvRect MouseListener::mouseDragArea;
CvPoint MouseListener::originPoint;
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
				cout<<"MESSAGE : Setting ROI"<<endl;
				CurrentMode::state = SET_CORNER;
			}
			else if(CurrentMode::state == CORNER_CONFIRM){
				CurrentMode::state = CALC_SPOT;
				cout<<"MESSAGE : CORNER CONFIRM"<<endl;
				
			}
			else if(CurrentMode::state  == CATCH_CORNER){
				mouse->setLButtonDown();
				if(mouseDragArea.x < x && mouseDragArea.x + mouseDragArea.width > x){
					if(mouseDragArea.y < y && mouseDragArea.y + mouseDragArea.height > y){
						cout<<"MESSAGE : Corner Position Adjusting"<<endl;
						CurrentMode::state = CORNER_MOVE;
					}
				}
			}
			else if(CurrentMode::state == CATCH_SKIN_COLOR){
				cout<<"MESSAGE : CALCUALATE SKIN COLOR"<<endl;
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
					cout<<"MESSAGE : ROI Confirm"<<endl;
				}
			}
			else if(CurrentMode::state  == CORNER_MOVE){
				mouse->setLButtonUp();
				cout<<"MESSAGE : Corner Confirm"<<endl;
				CurrentMode::state  = CORNER_CONFIRM;
			}
			break;
		case CV_EVENT_RBUTTONDOWN: 
			if(CurrentMode::state == ROI_CONFIRM){
				cout<<"MESSAGE : Reset ROI"<<endl;
				mouseDragArea.width = 0;
				mouseDragArea.height = 0;
				mouseDragArea.x = 0;
				mouseDragArea.y = 0;
				CurrentMode::state = SET_ROI;
			}
			else if(CurrentMode::state == CORNER_CONFIRM){
				cout<<"MESSAGE : Adjust Paper Coner"<<endl;
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
								cout<<"MESSAGE : Corner selected"<<endl;
						}
				}
			}
			break;
	}
}
