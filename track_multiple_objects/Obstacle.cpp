#include "Obstacle.h"



Obstacle::Obstacle()
{
}

Obstacle::Obstacle(string name) {
	setType(name);

	if (name == "close") {
		//Set values fro close objects
		setHSVmin(Scalar(42, 0, 0));
		setHSVmax(Scalar(256, 186, 119));
		setColor(Scalar(0, 255, 0));
	}
	if (name == "mid") {
		//Set values fro close objects
		setHSVmin(Scalar(7, 202, 0));
		setHSVmax(Scalar(256, 256, 193));
		setColor(Scalar(0, 0, 255));
	}

	if (name == "away") {
		//Set values fro close objects
		setHSVmin(Scalar(0, 16, 46));
		setHSVmax(Scalar(53, 256, 256));
		setColor(Scalar(0, 0, 0));
	}

}


Obstacle::~Obstacle()
{
}

int Obstacle::getxPos() {
	return Obstacle::xPos;
}

void Obstacle::setxPos(int x) {
	Obstacle::xPos = x;
}

int Obstacle::getyPos() {
	return Obstacle::yPos;
}

void Obstacle::setyPos(int y) {
	Obstacle::yPos = y;
}

Scalar Obstacle::getHSVmin() {
	return Obstacle::HSVmin;
}

Scalar Obstacle::getHSVmax() {
	return Obstacle::HSVmax;
}

void Obstacle::setHSVmin(Scalar min) {
	Obstacle::HSVmin = min;
}

void Obstacle::setHSVmax(Scalar max) {
	Obstacle::HSVmax = max;
}