
#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <functional>

typedef unsigned char UC;

struct RGB
{
	UC r, g, b;
};

struct Image
{
	Image(int, int);
	void PutPixel(int, int, RGB);
	void Save(std::ostream&);
	void Save(std::string fn);
private:
	int idx(int, int) const;
	int w, h;
	std::vector<RGB> pixels;
};

typedef std::function<double(double)> xyfunc;

struct Plot
{
	Plot();
	template<typename It>
	void AddPoints(It, It);
	void AddPoint(double, double);
	enum AxisType { atLin, atLog, atAuto };
	void SetAxisMode(AxisType, AxisType);
	void SetAxisName(std::string, std::string);
	void SetFunction(xyfunc);
	Image generate(int, int);
private:
	std::string xname, yname;
	AxisType xtype, ytype;
	xyfunc func;
	struct Pnt { double x,y; };
	std::vector<Pnt> data;
};


template<typename It>
void Plot::AddPoints(It b, It e)
{
	while (b != e)
	{
		auto [x,y] = *b;
		AddPoint(x, y);
		++b;
	}
}




