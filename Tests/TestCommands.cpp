#include "TestFramework.h"
#include "../Model/Document.h"
#include "../Shapes/RectShape.h"
#include "../Commands/CommandHistory.h"
#include "../Commands/ConcreteCommands.h"

TEST(CommandHistoryRedoClear) {
    CommandHistory history;
    Document doc;

    auto rect1 = std::make_unique<RectShape>(Rect2D(0, 0, 10, 10));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect1)));
    
    auto rect2 = std::make_unique<RectShape>(Rect2D(10, 10, 20, 20));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect2)));

    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(2));
    ASSERT_TRUE(history.CanUndo());

    history.Undo();
    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(1));
    ASSERT_TRUE(history.CanRedo());

    auto rect3 = std::make_unique<RectShape>(Rect2D(30, 30, 40, 40));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect3)));

    ASSERT_FALSE(history.CanRedo());
    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(2));
}

TEST(CommandHistorySavedStateDirty) {
    CommandHistory history;
    Document doc;

    ASSERT_FALSE(history.IsDirty());

    auto rect = std::make_unique<RectShape>(Rect2D(0, 0, 10, 10));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect)));

    ASSERT_TRUE(history.IsDirty());

    history.MarkSaved();
    ASSERT_FALSE(history.IsDirty());

    history.Undo();
    ASSERT_TRUE(history.IsDirty());

    history.Redo();
    ASSERT_FALSE(history.IsDirty());
}

TEST(MoveAndDeleteZOrderUndoTest) {
    CommandHistory history;
    Document doc;

    auto rect1 = std::make_unique<RectShape>(Rect2D(0, 0, 10, 10));
    auto rect2 = std::make_unique<RectShape>(Rect2D(20, 20, 30, 30));
    auto rect3 = std::make_unique<RectShape>(Rect2D(40, 40, 50, 50));

    IShape* pRect2 = rect2.get();

    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect1)));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect2)));
    history.ExecuteCommand(std::make_unique<AddShapeCommand>(doc, std::move(rect3)));

    ASSERT_EQ(doc.FindShapeIndex(pRect2), static_cast<size_t>(1));

    history.ExecuteCommand(std::make_unique<DeleteShapeCommand>(doc, pRect2));
    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(2));

    history.Undo();
    ASSERT_EQ(doc.GetShapeCount(), static_cast<size_t>(3));
    ASSERT_EQ(doc.FindShapeIndex(pRect2), static_cast<size_t>(1));

    history.ExecuteCommand(std::make_unique<MoveShapeCommand>(pRect2, 5.0f, 10.0f));
    Rect2D bounds = static_cast<RectShape*>(pRect2)->GetRect();
    ASSERT_EQ(bounds.left, 25.0f);
    ASSERT_EQ(bounds.top, 30.0f);

    history.Undo();
    bounds = static_cast<RectShape*>(pRect2)->GetRect();
    ASSERT_EQ(bounds.left, 20.0f);
    ASSERT_EQ(bounds.top, 20.0f);
}
