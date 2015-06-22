#include"Header.h"
void Mouse::setLButtonDown(){
	this->bLButtonDown = true;
}
void Mouse::setLButtonUp(){
	this->bLButtonDown = false;
}
void Mouse::setRButtonDown(){
	this->bRButtonDown = true;
}
void Mouse::setRButtonUp(){
	this->bRButtonDown = true;
}
bool Mouse::getLButtonDownState(){
	return this->bLButtonDown;
}
bool Mouse::getRButtonDownState(){
	return this->bRButtonDown;
}
