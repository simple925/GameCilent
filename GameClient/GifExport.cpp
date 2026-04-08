#pragma once
#include "pch.h"
#include <giflib/lib/gif_lib.h>

struct GifFrame
{
    int width;
    int height;
    float duration; // 🔥 추가 (초 단위)
    vector<unsigned char> pixels;
};

class GifExport
{
private:
    vector<GifFrame> m_Frames;

    int m_FrameWidth = 0;
    int m_FrameHeight = 0;

public:
    bool LoadGif(const char* path)
    {
        int error = 0;
        GifFileType* gif = DGifOpenFileName(path, &error);
        if (!gif)
            return false;

        if (DGifSlurp(gif) != GIF_OK)
        {
            DGifCloseFile(gif, &error);
            return false;
        }

        m_FrameWidth = gif->SWidth;
        m_FrameHeight = gif->SHeight;

        m_Frames.clear();

        // 🔥 누적 캔버스
        vector<unsigned char> canvas(m_FrameWidth * m_FrameHeight * 4, 0);

        for (int i = 0; i < gif->ImageCount; ++i)
        {
            SavedImage& img = gif->SavedImages[i];
            GifImageDesc& desc = img.ImageDesc;

            ColorMapObject* cmap = desc.ColorMap ? desc.ColorMap : gif->SColorMap;
            if (!cmap)
                continue;

            int transparent = -1;
            int delay = 100; // 기본 100ms

            // 🔥 extension 분석
            for (int j = 0; j < img.ExtensionBlockCount; ++j)
            {
                ExtensionBlock& ext = img.ExtensionBlocks[j];

                if (ext.Function == GRAPHICS_EXT_FUNC_CODE && ext.ByteCount >= 4)
                {
                    // transparency
                    if (ext.Bytes[0] & 0x01)
                        transparent = (unsigned char)ext.Bytes[3];

                    // delay (1/100초)
                    delay = (ext.Bytes[2] << 8) | ext.Bytes[1];
                    delay *= 10; // ms
                }
            }

            GifFrame frame;
            frame.width = m_FrameWidth;
            frame.height = m_FrameHeight;
            frame.duration = delay / 1000.f; // 🔥 초 단위
            frame.pixels = canvas; // 🔥 이전 프레임 복사

            for (int y = 0; y < desc.Height; ++y)
            {
                for (int x = 0; x < desc.Width; ++x)
                {
                    int srcIdx = y * desc.Width + x;
                    int colorIdx = img.RasterBits[srcIdx];

                    if (colorIdx == transparent)
                        continue;

                    GifColorType color = cmap->Colors[colorIdx];

                    int dstX = desc.Left + x;
                    int dstY = desc.Top + y;
                    int dstIdx = (dstY * m_FrameWidth + dstX) * 4;

                    frame.pixels[dstIdx + 0] = color.Red;
                    frame.pixels[dstIdx + 1] = color.Green;
                    frame.pixels[dstIdx + 2] = color.Blue;
                    frame.pixels[dstIdx + 3] = 255;
                }
            }

            canvas = frame.pixels; // 🔥 누적 갱신
            m_Frames.push_back(frame);
        }

        DGifCloseFile(gif, &error);
        return true;
    }

public:
    bool Export(const wstring& outName)
    {
        if (m_Frames.empty())
            return false;

        int frameCount = (int)m_Frames.size();

        int cols = (int)ceil(sqrt(frameCount));
        int rows = (int)ceil((float)frameCount / cols);

        int sheetW = cols * m_FrameWidth;
        int sheetH = rows * m_FrameHeight;

        vector<unsigned char> sheet(sheetW * sheetH * 4, 0);

        for (int i = 0; i < frameCount; ++i)
        {
            int col = i % cols;
            int row = i / cols;

            for (int y = 0; y < m_FrameHeight; ++y)
            {
                for (int x = 0; x < m_FrameWidth; ++x)
                {
                    int dstX = col * m_FrameWidth + x;
                    int dstY = row * m_FrameHeight + y;

                    int dstIdx = (dstY * sheetW + dstX) * 4;
                    int srcIdx = (y * m_FrameWidth + x) * 4;

                    memcpy(&sheet[dstIdx], &m_Frames[i].pixels[srcIdx], 4);
                }
            }
        }

        if (!SavePNG(outName + L".png", sheet.data(), sheetW, sheetH))
            return false;

        if (!SaveFlipbookBinary(outName + L".flipbook", frameCount, cols, rows, sheetW, sheetH))
            return false;

        return true;
    }

private:
    bool SavePNG(const wstring& path, unsigned char* data, int w, int h)
    {
        extern int stbi_write_png(const char*, int, int, int, const void*, int);

        string narrow = WStrToStr(path);

        return stbi_write_png(narrow.c_str(), w, h, 4, data, w * 4) != 0;
    }

    bool SaveFlipbookBinary(const wstring& path, int frameCount, int cols, int rows, int sheetW, int sheetH)
    {
        FILE* f = nullptr;
        _wfopen_s(&f, path.c_str(), L"wb");

        if (!f)
            return false;

        // atlas 경로
        wstring atlas = path;
        size_t pos = atlas.find_last_of(L'.');
        if (pos != wstring::npos)
            atlas = atlas.substr(0, pos);

        atlas += L".png";

        SaveWString(f, atlas); // 🔥 추가

        fwrite(&frameCount, sizeof(int), 1, f);

        for (int i = 0; i < frameCount; ++i)
        {
            int col = i % cols;
            int row = i / cols;

            float u = (float)(col * m_FrameWidth) / sheetW;
            float v = (float)(row * m_FrameHeight) / sheetH;

            float su = (float)m_FrameWidth / sheetW;
            float sv = (float)m_FrameHeight / sheetH;

            float duration = m_Frames[i].duration; // 🔥 핵심

            fwrite(&u, sizeof(float), 1, f);
            fwrite(&v, sizeof(float), 1, f);
            fwrite(&su, sizeof(float), 1, f);
            fwrite(&sv, sizeof(float), 1, f);
            fwrite(&duration, sizeof(float), 1, f); // 🔥 추가
        }

        fclose(f);
        return true;
    }
};