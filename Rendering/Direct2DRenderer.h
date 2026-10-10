#ifndef DIRECT2D_RENDERER_H
#define DIRECT2D_RENDERER_H

#include "RenderContext.h"
#include "../Model/Document.h"
#include "../Shapes/RectShape.h"
#include "../Shapes/EllipseShape.h"
#include "../Shapes/LineShape.h"
#include "../Shapes/TextShape.h"
#include "../Shapes/PolygonShape.h"
#include "../Editing/ToolManager.h"
#include <cmath>
#include <algorithm>
#include <memory>

class Direct2DRenderer
{
public:
    Direct2DRenderer() {}
    ~Direct2DRenderer() {}

    void RenderDocument(RenderContext& renderContext, const Document& document)
    {
        ID2D1HwndRenderTarget* pTarget = renderContext.GetRenderTarget();
        ID2D1SolidColorBrush* pBrush = renderContext.GetSolidBrush();
        IDWriteFactory* pDWriteFactory = renderContext.GetDWriteFactory();
        if (!pTarget || !pBrush) return;

        for (const auto& shape : document.GetShapes())
        {
            if (!shape) continue;

            switch (shape->GetType())
            {
            case ShapeType::Rectangle:
                RenderRect(pTarget, pBrush, static_cast<const RectShape*>(shape.get()));
                break;
            case ShapeType::Ellipse:
                RenderEllipse(pTarget, pBrush, static_cast<const EllipseShape*>(shape.get()));
                break;
            case ShapeType::Line:
                RenderLine(pTarget, pBrush, static_cast<const LineShape*>(shape.get()));
                break;
            case ShapeType::Text:
                RenderText(pTarget, pBrush, pDWriteFactory, static_cast<const TextShape*>(shape.get()));
                break;
            case ShapeType::Freehand:
                RenderPolygon(pTarget, pBrush, static_cast<const PolygonShape*>(shape.get()));
                break;
            default:
                break;
            }
        }
    }

    void RenderSelectionTracker(RenderContext& renderContext, const IShape* pSelectedShape, int activeHandleIndex = -1, int activeVertexIndex = -1)
    {
        if (!pSelectedShape) return;

        ID2D1HwndRenderTarget* pTarget = renderContext.GetRenderTarget();
        ID2D1SolidColorBrush* pBrush = renderContext.GetSolidBrush();
        if (!pTarget || !pBrush) return;

        if (pSelectedShape->GetType() == ShapeType::Freehand)
        {
            const auto* pPoly = static_cast<const PolygonShape*>(pSelectedShape);
            const auto& pts = pPoly->GetPoints();
            for (size_t i = 0; i < pts.size(); ++i)
            {
                D2D1_ELLIPSE nodeMarker = D2D1::Ellipse(D2D1::Point2F(pts[i].x, pts[i].y), 4.5f, 4.5f);
                if (static_cast<int>(i) == activeVertexIndex)
                {
                    pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
                    pTarget->FillEllipse(nodeMarker, pBrush);
                    pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
                    pTarget->DrawEllipse(nodeMarker, pBrush, 1.5f);
                }
                else
                {
                    pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkOrange));
                    pTarget->FillEllipse(nodeMarker, pBrush);
                    pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
                    pTarget->DrawEllipse(nodeMarker, pBrush, 1.0f);
                }
            }
        }

        Rect2D bounds = pSelectedShape->GetBoundingBox();

        float left = (std::min)(bounds.left, bounds.right);
        float right = (std::max)(bounds.left, bounds.right);
        float top = (std::min)(bounds.top, bounds.bottom);
        float bottom = (std::max)(bounds.top, bounds.bottom);

        pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DodgerBlue));
        D2D1_RECT_F outline = D2D1::RectF(left, top, right, bottom);
        pTarget->DrawRectangle(outline, pBrush, 1.0f);

        float midX = (left + right) / 2.0f;
        float midY = (top + bottom) / 2.0f;

        Point2D handles[8] = {
            {left, top}, {midX, top}, {right, top},
            {left, midY},             {right, midY},
            {left, bottom}, {midX, bottom}, {right, bottom}
        };

        const float handleSize = 4.0f;
        for (int i = 0; i < 8; ++i)
        {
            D2D1_RECT_F handleRect = D2D1::RectF(
                handles[i].x - handleSize, handles[i].y - handleSize,
                handles[i].x + handleSize, handles[i].y + handleSize
            );

            if (i == activeHandleIndex)
            {
                pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DodgerBlue));
                pTarget->FillRectangle(handleRect, pBrush);
                pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
                pTarget->DrawRectangle(handleRect, pBrush, 1.0f);
            }
            else
            {
                pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
                pTarget->FillRectangle(handleRect, pBrush);
                pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DodgerBlue));
                pTarget->DrawRectangle(handleRect, pBrush, 1.0f);
            }
        }
    }

private:
    static D2D1_COLOR_F ToD2DColor(Color col)
    {
        return D2D1::ColorF(col.r, col.g, col.b, col.a);
    }

    void RenderRect(ID2D1HwndRenderTarget* pTarget, ID2D1SolidColorBrush* pBrush, const RectShape* pRect)
    {
        Rect2D r = pRect->GetRect();
        D2D1_RECT_F d2dRect = D2D1::RectF(r.left, r.top, r.right, r.bottom);

        if (pRect->IsFilled())
        {
            pBrush->SetColor(ToD2DColor(pRect->GetFillColor()));
            pTarget->FillRectangle(d2dRect, pBrush);
        }
        pBrush->SetColor(ToD2DColor(pRect->GetStrokeColor()));
        pTarget->DrawRectangle(d2dRect, pBrush, pRect->GetStrokeWidth());
    }

    void RenderEllipse(ID2D1HwndRenderTarget* pTarget, ID2D1SolidColorBrush* pBrush, const EllipseShape* pEllipse)
    {
        Ellipse2D e = pEllipse->GetEllipse();
        D2D1_ELLIPSE d2dEllipse;
        d2dEllipse.point = D2D1::Point2F(e.center.x, e.center.y);
        d2dEllipse.radiusX = e.radiusX;
        d2dEllipse.radiusY = e.radiusY;

        if (pEllipse->IsFilled())
        {
            pBrush->SetColor(ToD2DColor(pEllipse->GetFillColor()));
            pTarget->FillEllipse(d2dEllipse, pBrush);
        }
        pBrush->SetColor(ToD2DColor(pEllipse->GetStrokeColor()));
        pTarget->DrawEllipse(d2dEllipse, pBrush, pEllipse->GetStrokeWidth());
    }

    void RenderLine(ID2D1HwndRenderTarget* pTarget, ID2D1SolidColorBrush* pBrush, const LineShape* pLine)
    {
        Point2D s = pLine->GetStartPoint();
        Point2D e = pLine->GetEndPoint();
        D2D1_POINT_2F p1 = D2D1::Point2F(s.x, s.y);
        D2D1_POINT_2F p2 = D2D1::Point2F(e.x, e.y);

        pBrush->SetColor(ToD2DColor(pLine->GetStrokeColor()));
        pTarget->DrawLine(p1, p2, pBrush, pLine->GetStrokeWidth());
    }

    void RenderText(ID2D1HwndRenderTarget* pTarget, ID2D1SolidColorBrush* pBrush, IDWriteFactory* pDWriteFactory, const TextShape* pText)
    {
        if (!pText || !pDWriteFactory) return;

        ComPtr<IDWriteTextFormat> pTextFormat;
        HRESULT hr = pDWriteFactory->CreateTextFormat(
            pText->GetFontFamily().c_str(),
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            pText->GetFontSize(),
            L"ko-kr",
            &pTextFormat
        );

        if (SUCCEEDED(hr))
        {
            Point2D pos = pText->GetPosition();
            D2D1_RECT_F layoutRect = D2D1::RectF(
                pos.x, pos.y,
                pos.x + 2000.0f, pos.y + 1000.0f
            );

            pBrush->SetColor(ToD2DColor(pText->GetStrokeColor()));
            pTarget->DrawText(
                pText->GetText().c_str(),
                static_cast<UINT32>(pText->GetText().length()),
                pTextFormat.Get(),
                layoutRect,
                pBrush
            );
        }
    }

    void RenderPolygon(ID2D1HwndRenderTarget* pTarget, ID2D1SolidColorBrush* pBrush, const PolygonShape* pPoly)
    {
        if (!pTarget || !pBrush || !pPoly) return;
        const auto& pts = pPoly->GetPoints();
        if (pts.size() < 2) return;

        pBrush->SetColor(ToD2DColor(pPoly->GetStrokeColor()));
        for (size_t i = 0; i < pts.size() - 1; ++i)
        {
            pTarget->DrawLine(
                D2D1::Point2F(pts[i].x, pts[i].y),
                D2D1::Point2F(pts[i + 1].x, pts[i + 1].y),
                pBrush, pPoly->GetStrokeWidth()
            );
        }
        if (pts.size() >= 3)
        {
            pTarget->DrawLine(
                D2D1::Point2F(pts.back().x, pts.back().y),
                D2D1::Point2F(pts[0].x, pts[0].y),
                pBrush, pPoly->GetStrokeWidth()
            );
        }
    }

public:
    void RenderPreview(RenderContext& renderContext, ToolType tool, Point2D start, Point2D current, bool bIsDragging, const IShape* selectedShape = nullptr, int activeHandleIndex = -1)
    {
        if (!bIsDragging) return;

        ID2D1HwndRenderTarget* pTarget = renderContext.GetRenderTarget();
        ID2D1SolidColorBrush* pBrush = renderContext.GetSolidBrush();
        if (!pTarget || !pBrush) return;

        pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DodgerBlue));

        if (tool == ToolType::Select && selectedShape)
        {
            float dx = current.x - start.x;
            float dy = current.y - start.y;

            if (activeHandleIndex != -1)
            {
                std::unique_ptr<IShape> tempClone = selectedShape->Clone();
                tempClone->Resize(activeHandleIndex, dx, dy);

                if (tempClone->GetType() == ShapeType::Rectangle)
                {
                    const auto* pRect = static_cast<const RectShape*>(tempClone.get());
                    Rect2D r = pRect->GetRect();
                    D2D1_RECT_F pr = D2D1::RectF(r.left, r.top, r.right, r.bottom);
                    pTarget->DrawRectangle(pr, pBrush, 1.5f);
                }
                else if (tempClone->GetType() == ShapeType::Ellipse)
                {
                    const auto* pEllipse = static_cast<const EllipseShape*>(tempClone.get());
                    Ellipse2D e = pEllipse->GetEllipse();
                    D2D1_ELLIPSE pe = D2D1::Ellipse(D2D1::Point2F(e.center.x, e.center.y), e.radiusX, e.radiusY);
                    pTarget->DrawEllipse(pe, pBrush, 1.5f);
                }
                else if (tempClone->GetType() == ShapeType::Line)
                {
                    const auto* pLine = static_cast<const LineShape*>(tempClone.get());
                    Point2D s = pLine->GetStartPoint();
                    Point2D e = pLine->GetEndPoint();
                    pTarget->DrawLine(D2D1::Point2F(s.x, s.y), D2D1::Point2F(e.x, e.y), pBrush, 1.5f);
                }
                else if (tempClone->GetType() == ShapeType::Freehand)
                {
                    const auto* pPoly = static_cast<const PolygonShape*>(tempClone.get());
                    RenderPolygon(pTarget, pBrush, pPoly);
                }
                else if (tempClone->GetType() == ShapeType::Text)
                {
                    const auto* pText = static_cast<const TextShape*>(tempClone.get());
                    Point2D pos = pText->GetPosition();
                    D2D1_RECT_F pr = D2D1::RectF(pos.x, pos.y, pos.x + 100.0f, pos.y + 30.0f);
                    pTarget->DrawRectangle(pr, pBrush, 1.5f);
                }
            }
            else
            {
                if (selectedShape->GetType() == ShapeType::Rectangle)
                {
                    auto pRect = static_cast<const RectShape*>(selectedShape);
                    Rect2D r = pRect->GetRect();
                    D2D1_RECT_F pr = D2D1::RectF(r.left + dx, r.top + dy, r.right + dx, r.bottom + dy);
                    pTarget->DrawRectangle(pr, pBrush, 1.5f);
                }
                else if (selectedShape->GetType() == ShapeType::Ellipse)
                {
                    auto pEllipse = static_cast<const EllipseShape*>(selectedShape);
                    Ellipse2D e = pEllipse->GetEllipse();
                    D2D1_ELLIPSE pe = D2D1::Ellipse(D2D1::Point2F(e.center.x + dx, e.center.y + dy), e.radiusX, e.radiusY);
                    pTarget->DrawEllipse(pe, pBrush, 1.5f);
                }
                else if (selectedShape->GetType() == ShapeType::Line)
                {
                    auto pLine = static_cast<const LineShape*>(selectedShape);
                    Point2D s = pLine->GetStartPoint();
                    Point2D e = pLine->GetEndPoint();
                    pTarget->DrawLine(D2D1::Point2F(s.x + dx, s.y + dy), D2D1::Point2F(e.x + dx, e.y + dy), pBrush, 1.5f);
                }
                else if (selectedShape->GetType() == ShapeType::Text)
                {
                    auto pText = static_cast<const TextShape*>(selectedShape);
                    Point2D pos = pText->GetPosition();
                    D2D1_RECT_F pr = D2D1::RectF(pos.x + dx, pos.y + dy, pos.x + dx + 100.0f, pos.y + dy + 30.0f);
                    pTarget->DrawRectangle(pr, pBrush, 1.5f);
                }
            }
            return;
        }

        switch (tool)
        {
        case ToolType::Rectangle:
        {
            float left = (std::min)(start.x, current.x);
            float right = (std::max)(start.x, current.x);
            float top = (std::min)(start.y, current.y);
            float bottom = (std::max)(start.y, current.y);

            D2D1_RECT_F r = D2D1::RectF(left, top, right, bottom);
            pTarget->DrawRectangle(r, pBrush, 1.5f);
            break;
        }
        case ToolType::Ellipse:
        {
            float rx = fabsf(current.x - start.x) / 2.0f;
            float ry = fabsf(current.y - start.y) / 2.0f;
            D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F((start.x + current.x) / 2.0f, (start.y + current.y) / 2.0f), rx, ry);
            pTarget->DrawEllipse(e, pBrush, 1.5f);
            break;
        }
        case ToolType::Line:
        {
            pTarget->DrawLine(D2D1::Point2F(start.x, start.y), D2D1::Point2F(current.x, current.y), pBrush, 1.5f);
            break;
        }
        case ToolType::Text:
        {
            D2D1_RECT_F r = D2D1::RectF(start.x, start.y, start.x + 100.0f, start.y + 30.0f);
            pTarget->DrawRectangle(r, pBrush, 1.0f);
            break;
        }
        default:
            break;
        }
    }

    void RenderPolygonPreview(RenderContext& renderContext, const std::vector<Point2D>& points, Point2D currentPos)
    {
        if (points.empty()) return;
        ID2D1HwndRenderTarget* pTarget = renderContext.GetRenderTarget();
        ID2D1SolidColorBrush* pBrush = renderContext.GetSolidBrush();
        if (!pTarget || !pBrush) return;

        pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DodgerBlue));

        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            pTarget->DrawLine(
                D2D1::Point2F(points[i].x, points[i].y),
                D2D1::Point2F(points[i + 1].x, points[i + 1].y),
                pBrush, 1.5f
            );
        }

        pTarget->DrawLine(
            D2D1::Point2F(points.back().x, points.back().y),
            D2D1::Point2F(currentPos.x, currentPos.y),
            pBrush, 1.5f
        );

        for (const auto& pt : points)
        {
            D2D1_ELLIPSE ptMarker = D2D1::Ellipse(D2D1::Point2F(pt.x, pt.y), 3.0f, 3.0f);
            pTarget->FillEllipse(ptMarker, pBrush);
        }
    }
};

#endif // DIRECT2D_RENDERER_H
