#ifndef GRID_RENDERER_H
#define GRID_RENDERER_H

#include "RenderContext.h"
#include "../Editing/GridSettings.h"

class GridRenderer
{
public:
    GridRenderer() {}

    void Render(RenderContext& renderContext, const GridSettings& settings, float width, float height)
    {
        if (!settings.IsGridVisible()) return;

        ID2D1HwndRenderTarget* pTarget = renderContext.GetRenderTarget();
        ID2D1SolidColorBrush* pBrush = renderContext.GetSolidBrush();
        if (!pTarget || !pBrush) return;

        float subStepPx = settings.GetSubGridStepPx();
        float mainStepPx = settings.GetMainGridStepPx();

        // 1. Render Sub Grid (smaller scale, thin line width e.g. 0.1 ~ 0.5px)
        if (subStepPx >= 3.0f)
        {
            pBrush->SetColor(D2D1::ColorF(0.90f, 0.90f, 0.90f, 0.7f));
            float subW = settings.GetSubGridLineWidth();

            for (float y = 0.0f; y < height; y += subStepPx)
            {
                pTarget->DrawLine(
                    D2D1::Point2F(0.0f, y),
                    D2D1::Point2F(width, y),
                    pBrush, subW
                );
            }
            for (float x = 0.0f; x < width; x += subStepPx)
            {
                pTarget->DrawLine(
                    D2D1::Point2F(x, 0.0f),
                    D2D1::Point2F(x, height),
                    pBrush, subW
                );
            }
        }

        // 2. Render Main Grid (graph paper concept, thicker/darker line width e.g. 0.5 ~ 1.0px)
        if (mainStepPx >= 5.0f)
        {
            pBrush->SetColor(D2D1::ColorF(0.75f, 0.78f, 0.82f, 0.9f));
            float mainW = settings.GetMainGridLineWidth();

            for (float y = 0.0f; y < height; y += mainStepPx)
            {
                pTarget->DrawLine(
                    D2D1::Point2F(0.0f, y),
                    D2D1::Point2F(width, y),
                    pBrush, mainW
                );
            }
            for (float x = 0.0f; x < width; x += mainStepPx)
            {
                pTarget->DrawLine(
                    D2D1::Point2F(x, 0.0f),
                    D2D1::Point2F(x, height),
                    pBrush, mainW
                );
            }
        }
    }
};

#endif // GRID_RENDERER_H
