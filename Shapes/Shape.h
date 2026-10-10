#ifndef SHAPE_H
#define SHAPE_H

#include "../Common/Types.h"
#include <memory>

// 도형의 종류
enum class ShapeType
{
    Rectangle,
    Ellipse,
    Line,
    Freehand,
    Text
};

// 모든 도형의 순수 데이터/로직 베이스 클래스 (Direct2D 독립적)
class IShape
{
protected:
    Color m_strokeColor;
    Color m_fillColor;
    float m_strokeWidth;
    bool  m_bFilled;

public:
    IShape(Color strokeColor = Color::Black(),
           float strokeWidth = 2.0f,
           bool bFilled = false,
           Color fillColor = Color::White())
        : m_strokeColor(strokeColor), m_strokeWidth(strokeWidth),
          m_bFilled(bFilled), m_fillColor(fillColor) {}

    virtual ~IShape() {}

    virtual std::unique_ptr<IShape> Clone() const = 0;

    // 히트 테스트 (마우스 클릭 감지)
    virtual bool HitTest(Point2D pt) = 0;

    // 도형 이동 (드래그 시)
    virtual void Move(float dx, float dy) = 0;

    // 도형 크기 조절 (8개 조절점 핸들 드래그 시)
    virtual void Resize(int handleIndex, float dx, float dy) = 0;

    // 도형 외곽 바운딩 박스 반환
    virtual Rect2D GetBoundingBox() const = 0;

    virtual ShapeType GetType() const = 0;

    // 속성 설정 및 반환
    void SetStrokeColor(Color color) { m_strokeColor = color; }
    Color GetStrokeColor() const { return m_strokeColor; }

    void SetFillColor(Color color) { m_fillColor = color; }
    Color GetFillColor() const { return m_fillColor; }

    void SetStrokeWidth(float width) { m_strokeWidth = width; }
    float GetStrokeWidth() const { return m_strokeWidth; }

    void SetFilled(bool filled) { m_bFilled = filled; }
    bool IsFilled() const { return m_bFilled; }
};

#endif // SHAPE_H
