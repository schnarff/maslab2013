#include <iostream>
#include <math.h>

class ImageProcessing{
    private:
	int ballInfo[4];
	int minX;
	int minY;
	int maxX;
	int maxY;
	int Nblob;

	const static float camera_height = 8.0f;
	const static float alpha = 320.0f/1.44f;
	const static float beta = 240.0f/1.44f*20;	

    public:
	void classify( int* data ) {
		for ( int i = 0; i < 320*240; i++ ) {
			int p = data[i];
			int b = (p>>16)&0xff;
			int g = (p>>8)&0xff;
			int r = (p)&0xff;

			if ( r > (g+b)*5/8 & r > g*14/10 ) {		//RED
			 	data[i] = 0xff0000ff;
			} else if ( r > 15*b/10 & g > 15*b/10 ) {	//YELLOW
				data[i] = 0xff00ffff;	
			} else if ( g > (r+b)*5/8 ) {			//GREEN
				data[i] = 0xff00ff00;
			} else if ( b > (r+g)*11/16 & r > g ) {		//PURPLE
				data[i] = 0xffff00ff;	
			} else if ( b > (r+g)*6/8 ) {			//BLUE
				data[i] = 0xffff0000;
			} else if ( (r-g)*(r-g)<50*50 & (r-b)*(r-b)<50*50 & (g-b)*(g-b)<50*50 & r+g+b>250 ) {			//WHITE
				data[i] = 0xffffffff;
			} else {
				data[i] = 0xff000000;
			}
		}
	}

	void findWalls( int* data, int* map ) {
		for ( int i= 0; i < 320*240; i++ ) {
			map[i] = 0x00000000;
		}
		for ( int x= 0; x < 320; x+=1 ) {
			int my = 239;
			int bottom = -1;
			int top = -1;
			int consecutive = 0;
			//detect white region bottom
			while ( my>0 ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix==0xffffffff ) {
					consecutive++;
				} else {
					consecutive = 0;
					bottom = my+5;
				}
				if ( consecutive>=5 ) {
					break;
				}
				my--;
			}
			//detect blue region bottom
			while ( my>0 ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix==0xffff0000 )
					break;
				my--;
			}
			//detect blue region top
			consecutive = 0;
			while ( my>0 ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix!=0xffff0000 ) {
					consecutive++;
				} else {
					consecutive = 0;
					top = my+3;
				}
				if ( consecutive>=3 ) {
					break;
				}
				my--;
			}

			//Draw to map
			if ( bottom<0 || top<0 )
				continue;
			int eta = top-bottom-120;
			int xi = x-160;
			float d = beta*6.0f/(eta);
			float Z = sqrt(d*d-camera_height*camera_height);
			float X = Z*(-xi)/alpha;
			int Xcoord = (int)(X+160);
			int Zcoord = (int)(-Z+120);
			int dest = Xcoord+Zcoord*320;
			if ( Xcoord>0 && Xcoord<320 && Zcoord>0 && Zcoord<240 )
				map[dest] = 0xffffffff;
		}
	}

	int findBalls( int* data, int* ballData ) {
		int ballCount = 0;
		for ( int y = 0; y < 240; y+=4 ) {
			for ( int x = 0; x < 320; x+=10 ) {
				int pix = data[x+320*y];
				if ( pix==0xff0000ff | pix==0xff00ff00 ) {
					bool valid = floodFind(x,y,data,pix);
					if ( valid ) {
						int base = ballCount<<2;
						ballData[base] = ballInfo[0];
						ballData[base+1] = ballInfo[1];
						ballData[base+2] = ballInfo[2];
						ballData[base+3] = ballInfo[3];
						ballCount++;
					}
				}
			}
		}
		return ballCount;
	}

	bool floodFind( int x, int y, int* data, int search) {
		minX = 10000;
		minY = 10000;
		maxX = 0;
		maxY = 0;
		Nblob = 0;

		blobFind(x,y,data,search);

		int dx = maxX - minX;
		int dy = maxY - minY;		
		float ratio = (float)(dx)/(float)(dy);
		float fraction = (float)(Nblob)/(float)(dx*dy);

		if ( ratio>0.75 && ratio<1.25 && fraction > 0.65 && fraction < 0.85 && Nblob>=20 ) {
			ballInfo[0] = (minX+maxX)/2;
			ballInfo[1] = (minY+maxY)/2;
			ballInfo[2] = (dx+dy)/2;
			ballInfo[3] = search;
			return true;
		}
		return false;
	}

	void blobFind( int x, int y, int* data, int search ) {
		if ( x<0 | x>=320 | y<0 | y>=240 )
			return;
		int pix = data[x+y*320];
		if ( pix==search ) {
			data[x+y*320] = 0x000000fe;
			Nblob++;
			if ( x < minX ) minX = x;
			if ( y < minY ) minY = y;
			if ( x > maxX ) maxX = x;
			if ( y > maxY ) maxY = y;
			blobFind(x+1,y,data,search);
			blobFind(x-1,y,data,search);
			blobFind(x,y+1,data,search);
			blobFind(x,y-1,data,search);
		}
	}
};


extern "C" {
    ImageProcessing* ImageProcessing_new(){ return new ImageProcessing(); }
    void ImageProcessing_classify(ImageProcessing* imgProc, int* data){ imgProc->classify(data); }
    int ImageProcessing_findBalls(ImageProcessing* imgProc, int* data, int* ballData){ return imgProc->findBalls(data,ballData); }
    void ImageProcessing_findWalls(ImageProcessing* imgProc, int* data, int* map){ return imgProc->findWalls(data,map); }
}
