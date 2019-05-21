
#include "graph.h"

#include <cstring>
#include <fstream>

/*
struct Image
{
                                                                Image(int, int);
                                                                void
PutPixel(int, int, RGB);
                                                                void
Save(std::ostream&);
                                                                void
Save(std::string fn);
private:
                                                                int idx(int,
int) const;
                                                                int w, h;
                                                                std::vector<RGB>
pixels;
};
*/

Image::Image(int w, int h) : w(w), h(h) {
  pixels.resize(w * h, {255, 255, 255});
}

void Image::PutPixel(int x, int y, RGB pix) {
  if (x < 0)
    return;
  if (x >= w)
    return;
  if (y < 0)
    return;
  if (y >= h)
    return;
  pixels[idx(x, y)] = pix;
}

void Image::Save(std::string fn) {
  std::ofstream ofs(fn, std::ios_base::binary);
  Save(ofs);
}

#pragma pack(push, 1)

struct BITMAP_FILE_HEADER {
  unsigned char magic[2];
  int file_size;
  short reserved1;
  short reserved2;
  int image_data_offset;
};

struct BITMAP_INFO_HDR {
  int hdr_size;
  int img_width;
  int img_height;
  short col_planes;
  short bpp;
  int compr_meth;
  int img_size;
  int hor_dpi;
  int ver_dpi;
  int col_pal_sz;
  int col_pal_important;
};

#pragma pack(pop)

void Image::Save(std::ostream &out) {
  (void)out;
  (void)w;
  (void)h;

  BITMAP_FILE_HEADER bfh;

  int scanlinesize = 3 * w;
  int padbytes = 0;
  while ((scanlinesize % 4) != 0) {
    ++scanlinesize;
    ++padbytes;
  }

  bfh.magic[0] = 'B';
  bfh.magic[1] = 'M';
  bfh.file_size =
      sizeof(BITMAP_FILE_HEADER) + sizeof(BITMAP_INFO_HDR) + scanlinesize * h;
  bfh.reserved1 = 0;
  bfh.reserved2 = 0;
  bfh.image_data_offset = sizeof(BITMAP_FILE_HEADER) + sizeof(BITMAP_INFO_HDR);

  BITMAP_INFO_HDR bih;

  std::memset(&bih, 0, sizeof(BITMAP_INFO_HDR));

  bih.hdr_size = sizeof(BITMAP_INFO_HDR);
  bih.img_width = w;
  bih.img_height = h;
  bih.col_planes = 1;
  bih.bpp = 24;
  bih.compr_meth = 0;
  bih.img_size = scanlinesize * h;
  bih.hor_dpi = 2835;
  bih.ver_dpi = 2835;

  out.write((char *)&bfh, sizeof(BITMAP_FILE_HEADER));
  out.write((char *)&bih, sizeof(BITMAP_INFO_HDR));

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      RGB &pix = pixels[idx(x, y)]; // Pixels(x,y);

      out.write((char *)&pix.r, 1);
      out.write((char *)&pix.g, 1);
      out.write((char *)&pix.b, 1);
    }
    for (int i = 0; i < padbytes; ++i) {
      out.write("", 1);
    }
  }
}

int Image::idx(int x, int y) const { return x + y * w; }

/******************************************************************************/

Plot::Plot() : func(nullptr) {
  xname = "x";
  yname = "y";
  xtype = ytype = atAuto;
}

void Plot::AddPoint(double x, double y) { data.push_back({x, y}); }

void Plot::SetAxisMode(AxisType atx, AxisType aty) {
  xtype = atx;
  ytype = aty;
}
void Plot::SetAxisName(std::string nx, std::string ny) {
  xname = nx;
  yname = ny;
}

void Plot::SetFunction(xyfunc f) { func = f; }

void upd_min_max(double val, double &min, double &max) {
  if (val < min)
    min = val;
  if (val > max)
    max = val;
}

Image Plot::generate(int w, int h) {
  Image img(w, h);

  double minx, maxx, miny, maxy;
  minx = maxx = data[0].x;
  miny = maxy = data[0].y;
  int i, n = (int)data.size();
  for (i = 1; i < n; ++i) {
    upd_min_max(data[i].x, minx, maxx);
    upd_min_max(data[i].y, miny, maxy);
  }
  auto xtrans = [&](double x) -> int {
    int plot_w = w - 20;
    double graph_w = (maxx - minx) + 5;
    double pos_x = (x - minx) / graph_w;
    return 10 + int(pos_x * plot_w);
  };
  auto ytrans = [&](double y) -> int {
    int plot_h = h - 20;
    double graph_h = (maxy - miny) + 5;
    double pos_y = (y - miny) / graph_h;
    return 10 + int(pos_y * plot_h);
  };

  RGB point{0, 0, 0};
  for (auto &&p : data) {
    int x = xtrans(p.x);
    int y = ytrans(p.y);
    img.PutPixel(x, y, point);
  }

  RGB line{128, 128, 255};
  for (int x = 0; x < w; ++x)
    img.PutPixel(x, 10, line);
  for (int y = 0; y < h; ++y)
    img.PutPixel(10, y, line);

  auto xrev = [&](int x) -> double {
    double plot_w = w - 20.0;

    double graph_w = maxx - minx;
    double pos_x = (x - 10.0) / plot_w;
    return (pos_x * graph_w) + minx;
  };

  RGB fit{128, 255, 128};
  if (func)
    for (int x = 10; x < (w - 10); ++x) {
      double xx = xrev(x);
      double yy = func(xx);
      int y = ytrans(yy);
      img.PutPixel(x, y, fit);
    }

  return img;
}

/******************************************************************************/

MultiPlot::MultiPlot() {}

void MultiPlot::AddPoint(RGB pix, double x, double y) {
  data.push_back({pix, x, y});
}

Image MultiPlot::generate(int w, int h) {
  Image img(w, h);

  double minx, maxx, miny, maxy;
  minx = maxx = data[0].x;
  miny = maxy = data[0].y;
  int i, n = (int)data.size();
  for (i = 1; i < n; ++i) {
    upd_min_max(data[i].x, minx, maxx);
    upd_min_max(data[i].y, miny, maxy);
  }
  auto xtrans = [&](double x) -> int {
    int plot_w = w - 20;
    double graph_w = maxx - minx;
    double pos_x = (x - minx) / graph_w;
    return 10 + int(pos_x * plot_w);
  };
  auto ytrans = [&](double y) -> int {
    int plot_h = h - 20;
    double graph_h = maxy - miny;
    double pos_y = (y - miny) / graph_h;
    return 10 + int(pos_y * plot_h);
  };

  for (auto &&p : data) {
    int x = xtrans(p.x);
    int y = ytrans(p.y);
    img.PutPixel(x, y, p.pix);
  }

  RGB line{128, 128, 255};
  for (int x = 0; x < w; ++x)
    img.PutPixel(x, 10, line);
  for (int y = 0; y < h; ++y)
    img.PutPixel(10, y, line);

  return img;
}
