#include <iostream>
#include <math.h>

class ArduinoController{
    private:
	float prevG = 0;

    public:
	void process( int* data, int gyro ) {
		int leftM = 140;
		int rightM = 140;

		// controls
		float ang = gyro/57.3;
		float dx = cos(ang);
		float dy = sin(ang);
		float M = dy*1+0.7f*(dy-prevG);
		prevG = dy;
		leftM = (int)(30+M*30);
		rightM = (int)(30-M*30);
		// 

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
    void ArduinoController_process(ArduinoController* arc, int* data, int gyro){ arc->process(data,gyro); }
}
