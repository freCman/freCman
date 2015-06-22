#ifndef HEADER_H
#define HEADER_H
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<iostream>
#include<fstream>
#include <signal.h>

extern int sm_fd[2];
extern char sm_buf[40];
extern int _proc_handle; 

using namespace std;

typedef enum {Direction_11, Direction_1, Direction_7, Direction_5};
typedef enum{SET_ROI,ROI_CONFIRM, SET_CORNER, ADJUST_CORNER,
			CATCH_CORNER, CORNER_MOVE, CORNER_CONFIRM, CALC_SPOT, CAM_CALIB,
			CATCH_SKIN_COLOR, CALC_HIST,HAND_TRACKING, OTHER} Status;
typedef enum{KEY_STATE_NONE, KEY_STATE_PRESS, KEY_STATE_PRESS_HOLD, KEY_STATE_RELEASE} KeyState; 

typedef enum{KEY_ID_ESC, KEY_ID_F1, KEY_ID_F2, KEY_ID_F3, KEY_ID_F4, KEY_ID_F5, KEY_ID_F6, KEY_ID_F7, KEY_ID_F8, KEY_ID_F9, KEY_ID_F10, KEY_ID_F11, KEY_ID_F12, KEY_ID_PRTSC, KEY_ID_DELETE, KEY_ID_PO,
			KEY_ID_TILDE, KEY_ID_NUM_1, KEY_ID_NUM_2, KEY_ID_NUM_3, KEY_ID_NUM_4, KEY_ID_NUM_5, KEY_ID_NUM_6, KEY_ID_NUM_7, KEY_ID_NUM_8, KEY_ID_NUM_9, KEY_ID_NUM_0, KEY_ID_NUM_MINUS, KEY_ID_NUM_EQ, KEY_ID_NUM_BACKSP,
			KEY_ID_TAP, KEY_ID_Q, KEY_ID_W, KEY_ID_E, KEY_ID_R, KEY_ID_T, KEY_ID_Y, KEY_ID_U, KEY_ID_I,KEY_ID_O, KEY_ID_P, KEY_ID_OBRACKET, KEY_ID_CBRACKET, KEY_ID_BACKSLASH,
			KEY_ID_CAPS, KEY_ID_A, KEY_ID_S, KEY_ID_D,KEY_ID_F, KEY_ID_G, KEY_ID_H, KEY_ID_J, KEY_ID_K, KEY_ID_L, KEY_ID_SEMI, KEY_ID_QUOTE, KEY_ID_ENTER,
			KEY_ID_LSHIFT, KEY_ID_Z, KEY_ID_X, KEY_ID_C, KEY_ID_V, KEY_ID_B, KEY_ID_N, KEY_ID_M, KEY_ID_COMMA, KEY_ID_DOT, KEY_ID_SLASH, KEY_ID_RSHIFT,
			KEY_ID_CTRL, KEY_ID_NONE, KEY_ID_WINDOW, KEY_ID_ALT, KEY_ID_SPACE, KEY_ID_KOEN, KEY_ID_OTHERCHAR, KEY_ID_DIREC_UPDOWN,KEY_ID_DIREC_LEFT, KEY_ID_DIREC_RIGHT} KeyId;

class Camera{
	//CvCapture* camCapture;
	int resolutionWidth;
	int resolutionHeight;
public:
	CvSize getResolution();
	void setResolution(int width, int height);
	void setCamera(int deviceIndex);
	Camera(int deviceIndex, int width, int height);
	~Camera();
	IplImage* getQueryFrame();
	CvCapture* camCapture; //
	//Video Test
	Camera(char* fileName, int width, int height);
	void setVideo(char* fileName);
};

class Mouse{
	bool bLButtonDown;
	bool bRButtonDown;
public:
	void setLButtonDown();
	void setLButtonUp();
	void setRButtonDown();
	void setRButtonUp();
	bool getLButtonDownState();
	bool getRButtonDownState();
	
};
class KeyButton{
	KeyId  keyId;
	KeyState keyState;
public:
	KeyButton();
	CvRect keyLocation;
	void setKeyLocation(int x, int y, int width, int height);
	void setKeyId(KeyId keyId);
	void keyAction();
	void setPress();
	void setNone();
	void setHold();
	void setRelease();
	KeyId getKeyId();
	KeyState getKeyState();
};
// 추가
class Finger{
public:
	CvPoint currFingerTip;
	Finger* prevFinger;
	
	bool downOrientMotion;
	bool validMotion;
	double motionVector;
	double motionVectorSum;
	bool inButton(KeyButton button);
	bool isMotion();
	void setMotionVector();
	void setValidMotion();
	void initAttribute();
	Finger();
	//~Finger();
	void setPrevFinger();
	void fingerCopy(Finger finger);
	void resetFinger();
	

};
// 추가
class Hand{
public:
	Finger* finger[5];
	CvMemStorage* handStorage;
	CvSeq* handContour;
	CvSeq* hull;
	CvSeq* defect;
	CvConvexityDefect defectArray[100];
	double handArea;
	int detectFingerCount;
	int prevDetectFingerCount;
	
	bool isPressKey(KeyButton button);
	int determinMotion();
	void setFingerMotionVector();
	bool getAllFingerDownMotion();
	void getHandDefect();
	int getDefectTotal();
	void convertArray();
	void arrayMemSet();
	
	void correctPrevFingerTipIndex();
	void setValidMotionVector();
	void setPrevDetectFingerCount();
	void resetFingerAttribute();
	Hand();
	~Hand();
};

class MouseListener{
	static Mouse* mouse;
	static CvRect mouseDragArea;
	static CvPoint originPoint;
public :
	MouseListener();
	~MouseListener();
	Mouse* getMouse();
	CvRect getMouseDragArea();
	bool isMouseDragSize();
	void resetMouseDragArea();
	static void mouseClickEvent(int mEvent, int x, int y, int flags, void* param);
	
};

class PaperKeyboard{
protected:
	int keyButtonCount;
	CvPoint2D32f* keyButtonCorner;
public:
	KeyButton* keyButton;
	static CvPoint2D32f keyboardCorner[4];
	CvPoint2D32f getKeyButtonCorner(int index);
	int getKeyButtonCornerCount();
	void setKeyboardCorner(CvPoint2D32f* corner);
	void setPaperKeyboardCorner(IplImage* srcImage, CvRect paperArea, MouseListener* mouseListener);

	virtual void setKeyButton(IplImage* srcImage)=0;
	//~PaperKeyboard();
};


class PaperKeyboard_TypeA :public PaperKeyboard{
	
public:
	PaperKeyboard_TypeA();
	void setKeyButton(IplImage* srcImage);
	void sortPaperKeyboardCorner();
	void cornerVerification(IplImage* srcImage);
	void initKeyButtonCorner();
	void cornerSortingY(int startIndex, int cornerCount);
	void cornerSortingX(int startIndex, int cornerCount);
	void detectKeyButtonCorner(IplImage* srcImage);
	void setKeyButtonArea(CvPoint2D32f* corners, int startIndex, int keyCount);
	void setDirectionKeyButtonArea(CvPoint2D32f* corners, int startIndex, int next, int index);
	void showKeyButton(IplImage* srcImage);
	
	~PaperKeyboard_TypeA();

};

class ImageProcessor{
	//CvVideoWriter*  videoWriter;
	CvSize size;
	IplImage* transImage;
	IplImage* splitImage;
	IplImage* backProImage;
	IplImage* backSplitImage;
	IplImage* handMasking;
	CvMemStorage* storage;
	CvSeq* firstContour;
	bool inputAvailable;
	double inputStartPoint;
	int pressFinger;
	int pressedKey;
	int holdKey;
public:
	ImageProcessor();
	void paperAreaDraggingImage(IplImage* srcImage, MouseListener* mouseListener);
	void cameraCalibration(IplImage* srcImage, PaperKeyboard* paperKeyboard);
	void createSkinColorHistogram(IplImage* srcImage);
	void detectHandContour(IplImage* srcImage, PaperKeyboard* paperKeyboard, Hand* userHand[]);
	void getHandBinaryImage(IplImage* srcImage);
	void determinSingleHandFingerTip(Hand* userHand);
	void determinDoubleHandFingerTip(Hand* userHand[]);
	void makeMotionGraph(Hand* userHand[]);
	bool keyEvent(CvPoint fingerLocation,int index, PaperKeyboard* paperKeyboard);
	~ImageProcessor();
};

class CurrentMode{
public:
	static Status state;
};

class FingerKeyboard{
	//Camera* camera;
	IplImage* dstImage;
	ImageProcessor* imageProcessor;
	Hand* userHand[2]; //
	bool isAdjustCorner();
	bool isSettingROI();
	
public:
	MouseListener* mouseListener;
	
	Camera* camera;
	static int captureFrame;
	static CvRect selectedPaperArea;
	static PaperKeyboard* paperKeyboard;
	static IplImage* buttonImage;
	static CvFileStorage* camCalibrateReader;
	static CvFileStorage* camCalibrateWriter;
	static CvMat* cameraMat;
	static CvMat* distortionCoeffs;
	static bool isCamCalibrate;
	static CvHistogram* pHist;
	static IplImage* bgImage;
	int programSetUp(int camIndex);
	void programRun();
	void programExit();
};
#endif
