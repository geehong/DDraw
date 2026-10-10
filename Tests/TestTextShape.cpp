#include "TestFramework.h"
#include "../Model/Document.h"
#include "../Shapes/TextShape.h"
#include "../Commands/CommandHistory.h"
#include "../Commands/ConcreteCommands.h"

TEST(TextShapeModelAndCommandsTest) {
    Document doc;
    CommandHistory history;

    auto textShape = std::make_unique<TextShape>(
        L"Hello D2D", Point2D(100, 100), L"Arial", 20.0f, Color::Black()
    );
    IShape* pText = textShape.get();

    // 1. Add TextShape via Command
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(textShape)));
    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(1));

    // 2. HitTest
    ASSERT_TRUE(pText->HitTest(Point2D(105, 105)));

    // 3. Move TextShape via Command
    history.ExecuteCommand(std::make_unique<MoveShapeCommand>(pText, 50.0f, 30.0f));
    TextShape* pConcreteText = static_cast<TextShape*>(pText);
    ASSERT_EQ(pConcreteText->GetPosition().x, 150.0f);
    ASSERT_EQ(pConcreteText->GetPosition().y, 130.0f);

    // 4. Undo Move
    history.Undo();
    ASSERT_EQ(pConcreteText->GetPosition().x, 100.0f);
    ASSERT_EQ(pConcreteText->GetPosition().y, 100.0f);

    // 5. Redo Move
    history.Redo();
    ASSERT_EQ(pConcreteText->GetPosition().x, 150.0f);
    ASSERT_EQ(pConcreteText->GetPosition().y, 130.0f);
}
