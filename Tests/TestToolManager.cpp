#include "TestFramework.h"
#include "../Editing/ToolManager.h"

TEST(ToolManagerInitialState) {
    ToolManager tm;
    ASSERT_EQ(static_cast<int>(tm.GetCurrentTool()), static_cast<int>(ToolType::Select));
}

TEST(ToolManagerSetTool) {
    ToolManager tm;
    tm.SetCurrentTool(ToolType::Rectangle);
    ASSERT_EQ(static_cast<int>(tm.GetCurrentTool()), static_cast<int>(ToolType::Rectangle));
    
    tm.SetCurrentTool(ToolType::Line);
    ASSERT_EQ(static_cast<int>(tm.GetCurrentTool()), static_cast<int>(ToolType::Line));
}
