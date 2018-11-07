
#include "graph.h"

#include <fstream>

/*
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
*/

Image::Image(int w, int h)
	: w(w), h(h)
{
	pixels.resize(w*h, {255,255,255});
}

void Image::PutPixel(int x, int y, RGB pix)
{
	pixels[idx(x,y)] = pix;
}

void Image::Save(std::string fn)
{
	std::ofstream ofs(fn, std::ios_base::binary);
	Save(ofs);
}

void Image::Save(std::ostream& out)
{
	(void)out;
	(void)w;
	(void)h;
}

int Image::idx(int x, int y) const
{
	return x + y*w;
}

/******************************************************************************/


Plot::Plot()
	: func(nullptr)
{
	xname = "x";
	yname = "y";
	xtype = ytype = atAuto;
}

void Plot::AddPoint(double x, double y)
{
	data.push_back({x, y});
}

void Plot::SetAxisMode(AxisType atx, AxisType aty)
{
	xtype = atx; ytype = aty;
}
void Plot::SetAxisName(std::string nx, std::string ny)
{
	xname = nx;
	yname = ny;
}

void Plot:: SetFunction(xyfunc f)
{
	func = f;
}

void upd_min_max(double val, double& min, double& max)
{
	if (val<min) min=val;
	if (val>max) max=val;
}

Image Plot::generate(int w, int h)
{
	Image img(w, h);
	
	double minx, maxx, miny, maxy;
	minx = maxx = data[0].x;
	miny = maxy = data[0].y;
	int i, n = (int)data.size();
	for(i=1; i<n; ++i)
	{
		upd_min_max(data[i].x, minx, maxx);
		upd_min_max(data[i].y, miny, maxy);
	}
	auto xtrans = [&](double x) -> int
	{
		int plot_w = w - 20;
		double graph_w = maxx - minx;
		double pos_x = (x - minx) / graph_w;
		return 20 + int(pos_x * plot_w);
	};
	auto ytrans = [&](double y) -> int
	{
		int plot_h = h - 20;
		double graph_h = maxy - miny;
		double pos_y = (y - miny) / graph_h;
		return 20 + int(pos_y * plot_h);
	};
	
	RGB point{0,0,0};
	for (auto&& p : data)
	{
		int x = xtrans(p.x);
		int y = ytrans(p.y);
		img.PutPixel(x,y, point);
	}

	RGB line{128,128,255};
	for (int x=0; x<w; ++x)
		img.PutPixel(x,10, line);
	for (int y=0; y<h; ++y)
		img.PutPixel(10,y, line);
		
	auto xrev = [&](int x) -> double
	{
		double plot_w = w - 20.0;
		double graph_w = maxx - minx;
		double pos_x = (x - 20.0) / plot_w;
		return (pos_x * graph_w) + minx;
	};
	
	RGB fit{128,255,128};
	if (func)
		for (int x = 20; x<w; ++x)
		{
			double xx = xrev(x);
			double yy = func(xx);
			int y = ytrans(yy);
			img.PutPixel(x,y, fit);
		}

	return img;
}


