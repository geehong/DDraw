#include "TestFramework.h"
#include "../Model/Document.h"
#include "../Serialization/DocumentSerializer.h"
#include <windows.h>

TEST(SerializationRoundTripTest) {
    Document doc;
    doc.AddShape(std::make_unique<RectShape>(Rect2D(10, 20, 100, 200), Color::Red(), 2.5f, true, Color::Blue()));
    doc.AddShape(std::make_unique<EllipseShape>(Ellipse2D(Point2D(150, 150), 30, 40), Color::Black(), 1.0f, false));
    doc.AddShape(std::make_unique<LineShape>(Point2D(0, 0), Point2D(50, 50), Color::Red(), 3.0f));
    doc.AddShape(std::make_unique<TextShape>(L"Test JSON", Point2D(70, 80), L"Arial", 18.0f, Color::Black()));

    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(4));

    std::wstring jsonStr = DocumentSerializer::Serialize(doc);
    ASSERT_TRUE(jsonStr.find(L"\"version\": \"1.0\"") != std::wstring::npos);

    // 1. Atomic Save test
    std::wstring testFile = L"test_document_roundtrip.json";
    ASSERT_TRUE(DocumentSerializer::SaveToFileAtomic(testFile, doc));

    // 2. Load from file (Deserialization into temporary document)
    auto loadedDoc = DocumentSerializer::LoadFromFile(testFile);
    ASSERT_TRUE(loadedDoc != nullptr);
    ASSERT_EQ(loadedDoc->GetShapeCount(), static_cast<size_t>(4));

    // 3. Verify shape types and exact properties of loaded document
    const auto& shapes = loadedDoc->GetShapes();
    ASSERT_EQ(static_cast<int>(shapes[0]->GetType()), static_cast<int>(ShapeType::Rectangle));
    ASSERT_EQ(static_cast<int>(shapes[1]->GetType()), static_cast<int>(ShapeType::Ellipse));
    ASSERT_EQ(static_cast<int>(shapes[2]->GetType()), static_cast<int>(ShapeType::Line));
    ASSERT_EQ(static_cast<int>(shapes[3]->GetType()), static_cast<int>(ShapeType::Text));

    RectShape* pRect = static_cast<RectShape*>(shapes[0].get());
    ASSERT_EQ(pRect->GetRect().left, 10.0f);
    ASSERT_EQ(pRect->GetRect().top, 20.0f);
    ASSERT_EQ(pRect->GetRect().right, 100.0f);
    ASSERT_EQ(pRect->GetRect().bottom, 200.0f);
    ASSERT_TRUE(pRect->IsFilled());
    ASSERT_EQ(pRect->GetStrokeWidth(), 2.5f);

    TextShape* pText = static_cast<TextShape*>(shapes[3].get());
    ASSERT_EQ(pText->GetText(), std::wstring(L"Test JSON"));
    ASSERT_EQ(pText->GetFontFamily(), std::wstring(L"Arial"));
    ASSERT_EQ(pText->GetFontSize(), 18.0f);

    // 4. Verify Escaped Quotes in Text Serialization
    Document docQuotes;
    docQuotes.AddShape(std::make_unique<TextShape>(L"Hello \"World\"!", Point2D(0, 0)));
    std::wstring escapedJson = DocumentSerializer::Serialize(docQuotes);
    auto loadedQuotesDoc = DocumentSerializer::Deserialize(escapedJson);
    ASSERT_TRUE(loadedQuotesDoc != nullptr);
    ASSERT_EQ(loadedQuotesDoc->GetShapeCount(), static_cast<size_t>(1));
    TextShape* pTextEsc = static_cast<TextShape*>(loadedQuotesDoc->GetShapes()[0].get());
    ASSERT_EQ(pTextEsc->GetText(), std::wstring(L"Hello \"World\"!"));

    // 5. Verify Invalid JSON rejection (Safety Check)
    auto invalidDoc1 = DocumentSerializer::Deserialize(L"{ \"invalid\": true }");
    ASSERT_TRUE(invalidDoc1 == nullptr);

    auto invalidDoc2 = DocumentSerializer::Deserialize(L"Not a JSON string at all");
    ASSERT_TRUE(invalidDoc2 == nullptr);

    // Clean up temporary file
    DeleteFile(testFile.c_str());
}
