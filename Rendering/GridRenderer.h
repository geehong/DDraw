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

        float mainStepPx = settings.GetMainGridStepPx();
        float subStepPx = settings.GetSubGridStepPx();

        // 1. Render Sub Grid Lines (fine lines dividing main grid cell, e.g. 0.2 px)
        if (subStepPx >= 3.0f && subStepPx < mainStepPx)
        {
            pBrush->SetColor(D2D1::ColorF(0.85f, 0.88f, 0.92f, 0.6f));
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

        // 2. Render Main Grid Lines (prominent outer boundary, e.g. 1.0 px ~ 1.5 px)
        if (mainStepPx >= 5.0f)
        {
            pBrush->SetColor(D2D1::ColorF(0.20f, 0.25f, 0.35f, 0.95f)); // Distinct dark main grid line
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
