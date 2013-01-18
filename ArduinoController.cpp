#include <iostream>

class ArduinoController{
    public:
	void process( int* data ) {
		int leftM = 200;
		int rightM = -50;
		int leftD = 2;
		int rightD = 2;
		if ( leftM < 0 ) {
			leftM = -leftM;
			leftD = 0;
		}
		if ( rightM < 0 ) {
			rightM = -rightM;
			rightD = 0;
		}
		data[0] = (leftD<<24) | (leftM<<16) | (rightD<<8) | (rightM);
	}
};


extern "C" {
    ArduinoController* ArduinoController_new(){ return new ArduinoController(); }
    void ArduinoController_process(ArduinoController* arc, int* data){ arc->process(data); }
}