#ifndef TYPES_H
#define TYPES_H

struct Point2D
{
    float x;
    float y;

    Point2D(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
};

struct Rect2D
{
    float left;
    float top;
    float right;
    float bottom;

    Rect2D(float _left = 0.0f, float _top = 0.0f, float _right = 0.0f, float _bottom = 0.0f)
        : left(_left), top(_top), right(_right), bottom(_bottom) {}
};

struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a) {}

    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color DarkGreen() { return Color(0.0f, 0.39f, 0.0f, 1.0f); }
    static Color LightSkyBlue(float alpha = 0.4f) { return Color(0.53f, 0.81f, 0.98f, alpha); }
    static Color LightGreen(float alpha = 0.5f) { return Color(0.56f, 0.93f, 0.56f, alpha); }
};

struct Ellipse2D
{
    Point2D center;
    float radiusX;
    float radiusY;

    Ellipse2D(Point2D _center = Point2D(), float _rx = 0.0f, float _ry = 0.0f)
        : center(_center), radiusX(_rx), radiusY(_ry) {}
};

#endif // TYPES_H
