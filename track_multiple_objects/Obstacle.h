#include <string>
#include <opencv\cv.h>
using namespace std;
using namespace cv;


class Obstacle
{
public:
	Obstacle();
	~Obstacle();
	Obstacle(string name);
	int getxPos();
	void setxPos(int x);

	int getyPos();
	void setyPos(int y);

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar min);
	void setHSVmax(Scalar max);

	string getType() { return type; }
	void setType(string t) { type = t; }

	Scalar getColor() { return Color; }
	void setColor(Scalar c) { Color = c; }



private:
	int xPos, yPos;
	string type;
	Scalar	HSVmin, HSVmax;
	Scalar Color;
};

