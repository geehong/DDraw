#ifndef DOCUMENT_SERIALIZER_H
#define DOCUMENT_SERIALIZER_H

#include "../Model/Document.h"
#include "../Shapes/RectShape.h"
#include "../Shapes/EllipseShape.h"
#include "../Shapes/LineShape.h"
#include "../Shapes/TextShape.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>

class DocumentSerializer
{
public:
    static std::wstring EscapeJsonString(const std::wstring& str)
    {
        std::wstring res;
        for (wchar_t c : str)
        {
            if (c == L'"') res += L"\\\"";
            else if (c == L'\\') res += L"\\\\";
            else if (c == L'\n') res += L"\\n";
            else if (c == L'\r') res += L"\\r";
            else if (c == L'\t') res += L"\\t";
            else res += c;
        }
        return res;
    }

    static std::wstring Serialize(const Document& doc)
    {
        std::wstringstream ss;
        ss << L"{\n";
        ss << L"  \"version\": \"1.0\",\n";
        ss << L"  \"shapes\": [\n";

        const auto& shapes = doc.GetShapes();
        for (size_t i = 0; i < shapes.size(); ++i)
        {
            const auto& shape = shapes[i];
            if (!shape) continue;

            ss << L"    {\n";
            switch (shape->GetType())
            {
            case ShapeType::Rectangle:
            {
                auto pRect = static_cast<const RectShape*>(shape.get());
                Rect2D r = pRect->GetRect();
                ss << L"      \"type\": \"Rectangle\",\n";
                ss << L"      \"left\": " << r.left << L", \"top\": " << r.top
                   << L", \"right\": " << r.right << L", \"bottom\": " << r.bottom << L",\n";
                break;
            }
            case ShapeType::Ellipse:
            {
                auto pEll = static_cast<const EllipseShape*>(shape.get());
                Ellipse2D e = pEll->GetEllipse();
                ss << L"      \"type\": \"Ellipse\",\n";
                ss << L"      \"centerX\": " << e.center.x << L", \"centerY\": " << e.center.y
                   << L", \"radiusX\": " << e.radiusX << L", \"radiusY\": " << e.radiusY << L",\n";
                break;
            }
            case ShapeType::Line:
            {
                auto pLine = static_cast<const LineShape*>(shape.get());
                Point2D s = pLine->GetStartPoint();
                Point2D e = pLine->GetEndPoint();
                ss << L"      \"type\": \"Line\",\n";
                ss << L"      \"startX\": " << s.x << L", \"startY\": " << s.y
                   << L", \"endX\": " << e.x << L", \"endY\": " << e.y << L",\n";
                break;
            }
            case ShapeType::Text:
            {
                auto pText = static_cast<const TextShape*>(shape.get());
                Point2D pos = pText->GetPosition();
                ss << L"      \"type\": \"Text\",\n";
                ss << L"      \"text\": \"" << EscapeJsonString(pText->GetText()) << L"\",\n";
                ss << L"      \"fontFamily\": \"" << EscapeJsonString(pText->GetFontFamily()) << L"\",\n";
                ss << L"      \"fontSize\": " << pText->GetFontSize() << L",\n";
                ss << L"      \"posX\": " << pos.x << L", \"posY\": " << pos.y << L",\n";
                break;
            }
            default:
                break;
            }

            Color stroke = shape->GetStrokeColor();
            Color fill = shape->GetFillColor();
            ss << L"      \"strokeColor\": [" << stroke.r << L", " << stroke.g << L", " << stroke.b << L", " << stroke.a << L"],\n";
            ss << L"      \"fillColor\": [" << fill.r << L", " << fill.g << L", " << fill.b << L", " << fill.a << L"],\n";
            ss << L"      \"strokeWidth\": " << shape->GetStrokeWidth() << L",\n";
            ss << L"      \"isFilled\": " << (shape->IsFilled() ? L"true" : L"false") << L"\n";

            ss << L"    }" << (i + 1 < shapes.size() ? L"," : L"") << L"\n";
        }

        ss << L"  ]\n";
        ss << L"}\n";
        return ss.str();
    }

    static bool SaveToFileAtomic(const std::wstring& filePath, const Document& doc)
    {
        std::wstring tmpPath = filePath + L".tmp";

        std::wofstream ofs(tmpPath);
        if (!ofs.is_open()) return false;

        ofs << Serialize(doc);
        ofs.close();

        return ReplaceFile(filePath.c_str(), tmpPath.c_str(), NULL, REPLACEFILE_IGNORE_MERGE_ERRORS, NULL, NULL) ||
               MoveFileEx(tmpPath.c_str(), filePath.c_str(), MOVEFILE_REPLACE_EXISTING);
    }

    // --- JSON Deserialize helper ---
    static std::wstring ExtractValue(const std::wstring& json, const std::wstring& key)
    {
        size_t pos = json.find(L"\"" + key + L"\"");
        if (pos == std::wstring::npos) return L"";
        size_t colon = json.find(L":", pos);
        if (colon == std::wstring::npos) return L"";
        size_t start = colon + 1;
        while (start < json.length() && (json[start] == L' ' || json[start] == L'\t' || json[start] == L'\n' || json[start] == L'\r')) start++;
        if (start >= json.length()) return L"";

        if (json[start] == L'"')
        {
            std::wstring res;
            size_t idx = start + 1;
            while (idx < json.length())
            {
                wchar_t c = json[idx];
                if (c == L'\\')
                {
                    if (idx + 1 < json.length())
                    {
                        wchar_t next = json[idx + 1];
                        if (next == L'"') res += L'"';
                        else if (next == L'\\') res += L'\\';
                        else if (next == L'n') res += L'\n';
                        else if (next == L'r') res += L'\r';
                        else if (next == L't') res += L'\t';
                        else res += next;
                        idx += 2;
                        continue;
                    }
                }
                else if (c == L'"')
                {
                    break;
                }
                else
                {
                    res += c;
                }
                idx++;
            }
            return res;
        }
        else
        {
            size_t end = json.find_first_of(L",}\n\r", start);
            if (end == std::wstring::npos) end = json.length();
            return json.substr(start, end - start);
        }
    }

    static Color ParseColor(const std::wstring& json, const std::wstring& key)
    {
        size_t pos = json.find(L"\"" + key + L"\"");
        if (pos == std::wstring::npos) return Color::Black();
        size_t bracket1 = json.find(L"[", pos);
        size_t bracket2 = json.find(L"]", bracket1);
        if (bracket1 == std::wstring::npos || bracket2 == std::wstring::npos) return Color::Black();

        std::wstring arrStr = json.substr(bracket1 + 1, bracket2 - bracket1 - 1);
        std::wstringstream ss(arrStr);
        float r = 0, g = 0, b = 0, a = 1.0f;
        wchar_t comma;
        ss >> r >> comma >> g >> comma >> b >> comma >> a;
        return Color(r, g, b, a);
    }

    // Deserialize JSON string into temporary Document with strict validation
    static std::unique_ptr<Document> Deserialize(const std::wstring& jsonStr)
    {
        // 1. JSON 구조 및 버전 검증
        if (jsonStr.find(L"\"version\"") == std::wstring::npos ||
            jsonStr.find(L"\"shapes\"") == std::wstring::npos)
        {
            return nullptr; // 유효하지 않은 JSON 구조
        }

        std::wstring version = ExtractValue(jsonStr, L"version");
        if (version.empty())
        {
            return nullptr;
        }

        size_t pos = jsonStr.find(L"\"shapes\"");
        size_t arrayStart = jsonStr.find(L"[", pos);
        if (arrayStart == std::wstring::npos) return nullptr;

        auto pDoc = std::make_unique<Document>();
        size_t curr = arrayStart + 1;
        while (curr < jsonStr.length())
        {
            size_t objStart = jsonStr.find(L"{", curr);
            if (objStart == std::wstring::npos) break;
            size_t objEnd = jsonStr.find(L"}", objStart);
            if (objEnd == std::wstring::npos) break;

            std::wstring objJson = jsonStr.substr(objStart, objEnd - objStart + 1);
            std::wstring type = ExtractValue(objJson, L"type");

            Color strokeColor = ParseColor(objJson, L"strokeColor");
            Color fillColor = ParseColor(objJson, L"fillColor");
            float strokeWidth = static_cast<float>(_wtof(ExtractValue(objJson, L"strokeWidth").c_str()));
            bool isFilled = (ExtractValue(objJson, L"isFilled") == L"true");

            if (type == L"Rectangle")
            {
                float left = static_cast<float>(_wtof(ExtractValue(objJson, L"left").c_str()));
                float top = static_cast<float>(_wtof(ExtractValue(objJson, L"top").c_str()));
                float right = static_cast<float>(_wtof(ExtractValue(objJson, L"right").c_str()));
                float bottom = static_cast<float>(_wtof(ExtractValue(objJson, L"bottom").c_str()));

                pDoc->AddShape(std::make_unique<RectShape>(
                    Rect2D(left, top, right, bottom), strokeColor, strokeWidth, isFilled, fillColor
                ));
            }
            else if (type == L"Ellipse")
            {
                float cx = static_cast<float>(_wtof(ExtractValue(objJson, L"centerX").c_str()));
                float cy = static_cast<float>(_wtof(ExtractValue(objJson, L"centerY").c_str()));
                float rx = static_cast<float>(_wtof(ExtractValue(objJson, L"radiusX").c_str()));
                float ry = static_cast<float>(_wtof(ExtractValue(objJson, L"radiusY").c_str()));

                pDoc->AddShape(std::make_unique<EllipseShape>(
                    Ellipse2D(Point2D(cx, cy), rx, ry), strokeColor, strokeWidth, isFilled, fillColor
                ));
            }
            else if (type == L"Line")
            {
                float sx = static_cast<float>(_wtof(ExtractValue(objJson, L"startX").c_str()));
                float sy = static_cast<float>(_wtof(ExtractValue(objJson, L"startY").c_str()));
                float ex = static_cast<float>(_wtof(ExtractValue(objJson, L"endX").c_str()));
                float ey = static_cast<float>(_wtof(ExtractValue(objJson, L"endY").c_str()));

                pDoc->AddShape(std::make_unique<LineShape>(
                    Point2D(sx, sy), Point2D(ex, ey), strokeColor, strokeWidth
                ));
            }
            else if (type == L"Text")
            {
                std::wstring text = ExtractValue(objJson, L"text");
                std::wstring fontFamily = ExtractValue(objJson, L"fontFamily");
                float fontSize = static_cast<float>(_wtof(ExtractValue(objJson, L"fontSize").c_str()));
                float px = static_cast<float>(_wtof(ExtractValue(objJson, L"posX").c_str()));
                float py = static_cast<float>(_wtof(ExtractValue(objJson, L"posY").c_str()));

                pDoc->AddShape(std::make_unique<TextShape>(
                    text, Point2D(px, py), fontFamily, fontSize, strokeColor
                ));
            }

            curr = objEnd + 1;
        }

        return pDoc;
    }

    // Load file into temporary Document first to ensure safety
    static std::unique_ptr<Document> LoadFromFile(const std::wstring& filePath)
    {
        std::wifstream ifs(filePath);
        if (!ifs.is_open()) return nullptr;

        std::wstringstream ss;
        ss << ifs.rdbuf();
        return Deserialize(ss.str());
    }

    static bool LoadFromFile(Document& outDoc, const std::wstring& filePath)
    {
        auto pDoc = LoadFromFile(filePath);
        if (!pDoc) return false;

        outDoc.Clear();
        for (auto& shape : pDoc->GetShapes())
        {
            if (shape)
            {
                outDoc.AddShape(shape->Clone());
            }
        }
        return true;
    }
};

#endif // DOCUMENT_SERIALIZER_H
