#pragma once
#include "gif_lib.h"

struct GifFrame
{
    int width;
    int height;
    vector<unsigned char> pixels; // RGBA
};

class GifExport
{
private:
    vector<GifFrame> m_Frames;

    int m_FrameWidth = 0;
    int m_FrameHeight = 0;
public:
    GifExport();
    ~GifExport();
};

