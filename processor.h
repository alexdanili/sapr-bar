#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <iostream>
#include <math.h>

#include <limits>

class Processor
{
public:

	Processor();
	std::vector<float> nodes;
	std::vector<std::vector<float>> bars;
	std::vector<std::vector<float>> forces;

	std::vector<std::vector<float>> aMat;
	std::vector<float> dMat;
	std::vector<float> uMat;
    std::vector<std::vector<float>> udopMat;
	std::vector<float> sMat;
	std::vector<float> nMat;
	std::vector<float> fMat;
	float n;

	void clear();
    std::vector<double> getInPoint(double barNum, double LBar);
	void setNodes(std::vector<float> _nodes);
	void setBars(std::vector<std::vector<float>> _bars);
	void setForces(std::vector<std::vector<float>> _forces);
	void setFixators(bool l, bool r);

	void calculate();
private:
	bool leftFixator,rightFixator;
	float getForce(float type, float point);
	std::vector<float> kramer(std::vector<std::vector<float>> A);
	std::vector<std::vector<float>> getMat(std::vector<std::vector<float>> A, int _i, int _j);
	float det(std::vector<std::vector<float>> mat);


};

#endif // PROCESSOR_H
