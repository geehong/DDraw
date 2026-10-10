#include "TestFramework.h"
#include "../Model/Document.h"
#include "../Commands/CommandHistory.h"
#include "../Editing/EditorController.h"
#include "../Shapes/RectShape.h"

TEST(EditorControllerMouseCapturePolicy) {
    Document doc;
    CommandHistory history;
    ToolManager toolMgr;
    EditorController controller(doc, history, toolMgr);

    auto rect = std::make_unique<RectShape>(Rect2D(10, 10, 50, 50));
    IShape* pRect = rect.get();
    doc.AddShape(std::move(rect));

    // 1. Mouse Down on shape -> Selection & Dragging start (hWnd == NULL in unit test)
    controller.OnLButtonDown(Point2D(20, 20), NULL);
    ASSERT_TRUE(controller.GetMouseState().bIsDragging);
    ASSERT_TRUE(controller.GetMouseState().bLButtonDown);
    ASSERT_EQ(controller.GetSelectionManager().GetSelectedShape(), pRect);

    // 2. Mouse Move
    controller.OnMouseMove(Point2D(40, 40));

    // 3. WM_CAPTURECHANGED simulation (Lost capture externally)
    // ReleaseCapture should NOT be called again, internal drag state must reset cleanly
    controller.OnCaptureChanged();
    ASSERT_FALSE(controller.GetMouseState().bIsDragging);
    ASSERT_FALSE(controller.GetMouseState().bLButtonDown);

    // 4. ESC Key Cancellation simulation
    controller.OnLButtonDown(Point2D(20, 20), NULL);
    ASSERT_TRUE(controller.GetMouseState().bIsDragging);
    controller.OnEscapeKey(NULL);
    ASSERT_FALSE(controller.GetMouseState().bIsDragging);
    ASSERT_FALSE(controller.GetMouseState().bLButtonDown);
}
