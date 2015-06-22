CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

Demo : Camera.o CurrentMode.o FingerKeyboard.o Hand.o ImageProcessor.o KeyButton.o Main.o Mouse.o MouseListener.o PaperKeyboard.o PaperKeyboard_TypeA.o Finger.o
	arm-linux-gnueabihf-g++ -o Demo Camera.o CurrentMode.o FingerKeyboard.o Hand.o ImageProcessor.o KeyButton.o Main.o Mouse.o MouseListener.o PaperKeyboard.o PaperKeyboard_TypeA.o Finger.o $(CFLAGS) $(LIBS) -lm

Finger.o : Finger.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm Finger.cpp

Camera.o : Camera.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm Camera.cpp

CurrentMode.o : CurrentMode.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm CurrentMode.cpp

FingerKeyboard.o : FingerKeyboard.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm FingerKeyboard.cpp

Hand.o : Hand.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm Hand.cpp

ImageProcessor.o : ImageProcessor.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm ImageProcessor.cpp

KeyButton.o : KeyButton.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm KeyButton.cpp

Main.o : Main.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm Main.cpp

Mouse.o : Mouse.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm Mouse.cpp

MouseListener.o : MouseListener.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm MouseListener.cpp

PaperKeyboard.o : PaperKeyboard.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm PaperKeyboard.cpp

PaperKeyboard_TypeA.o : PaperKeyboard_TypeA.cpp Header.h
	arm-linux-gnueabihf-g++ -c $(CFLAGS) $(LIBS) -lm PaperKeyboard_TypeA.cpp

