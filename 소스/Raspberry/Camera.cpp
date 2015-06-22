#include"Header.h"

void Camera::setResolution(int width, int height){
	this->resolutionWidth = width;
	this->resolutionHeight = height;
	cvSetCaptureProperty(camCapture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty(camCapture, CV_CAP_PROP_FRAME_HEIGHT, height);
}

CvSize Camera::getResolution(){
	return cvSize(this->resolutionWidth, this->resolutionHeight);
}

void Camera::setCamera(int deviceIndex){
	this->camCapture = cvCreateCameraCapture(deviceIndex);
}

Camera::Camera(int deviceIndex, int width, int height){
	setCamera(deviceIndex);
	setResolution(width, height);
	
}
IplImage* Camera::getQueryFrame(){
	return cvQueryFrame(this->camCapture);
}
Camera::~Camera(){
	cout<<"camera Release"<<endl;
	cvReleaseCapture(&camCapture);
}
void Camera::setVideo(char* fileName){
	this->camCapture = cvCaptureFromFile(fileName);

}
Camera::Camera(char* fileName, int width, int height){
	setVideo(fileName);
	setResolution(width, height);

}