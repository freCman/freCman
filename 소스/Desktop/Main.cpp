#include"Header.h"

int main(){
	FingerKeyboard* fk = new FingerKeyboard();	
	fk->programSetUp();
	fk->programRun();	
	fk->programExit();
	delete fk;
}