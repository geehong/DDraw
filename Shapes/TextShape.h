#ifndef TEXT_SHAPE_H
#define TEXT_SHAPE_H

#include "Shape.h"
#include <string>

class TextShape : public IShape
{
private:
    std::wstring m_text;
    std::wstring m_fontFamily;
    float        m_fontSize;
    Point2D      m_position;

public:
    TextShape(const std::wstring& text,
              Point2D position,
              const std::wstring& fontFamily = L"맑은 고딕",
              float fontSize = 16.0f,
              Color textColor = Color::Black())
        : IShape(textColor, 1.0f, false, textColor),
          m_text(text), m_position(position),
          m_fontFamily(fontFamily), m_fontSize(fontSize) {}

    std::unique_ptr<IShape> Clone() const override
    {
        return std::make_unique<TextShape>(*this);
    }

    bool HitTest(Point2D pt) override
    {
        // 추정 바운딩 박스 (문자 수 기반 간단 계산)
        float width = m_text.length() * m_fontSize * 0.6f;
        float height = m_fontSize * 1.2f;

        return (pt.x >= m_position.x && pt.x <= m_position.x + width &&
                pt.y >= m_position.y && pt.y <= m_position.y + height);
    }

    void Move(float dx, float dy) override
    {
        m_position.x += dx;
        m_position.y += dy;
    }

    void Resize(int handleIndex, float dx, float dy) override
    {
        (void)handleIndex;
        m_position.x += dx;
        m_position.y += dy;
        m_fontSize += (dy + dx) * 0.1f;
        if (m_fontSize < 8.0f) m_fontSize = 8.0f;
    }

    Rect2D GetBoundingBox() const override
    {
        float w = m_text.length() * m_fontSize * 0.6f;
        float h = m_fontSize * 1.2f;
        return Rect2D(m_position.x, m_position.y, m_position.x + w, m_position.y + h);
    }

    ShapeType GetType() const override { return ShapeType::Text; }

    const std::wstring& GetText() const { return m_text; }
    void SetText(const std::wstring& text) { m_text = text; }

    const std::wstring& GetFontFamily() const { return m_fontFamily; }
    void SetFontFamily(const std::wstring& fontFamily) { m_fontFamily = fontFamily; }

    float GetFontSize() const { return m_fontSize; }
    void SetFontSize(float fontSize) { m_fontSize = fontSize; }

    Point2D GetPosition() const { return m_position; }
    void SetPosition(Point2D pos) { m_position = pos; }
};

#endif // TEXT_SHAPE_H
