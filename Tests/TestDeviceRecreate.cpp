#include "TestFramework.h"
#include "../Rendering/RenderContext.h"

TEST(RenderContextRecreateTest) {
    RenderContext ctx;
    // 1. Initial state (no HWND)
    ASSERT_EQ(ctx.GetRenderTarget(), static_cast<ID2D1HwndRenderTarget*>(nullptr));
    ASSERT_EQ(ctx.GetSolidBrush(), static_cast<ID2D1SolidColorBrush*>(nullptr));

    // 2. Discard device resources safely
    ctx.DiscardDeviceResources();
    ASSERT_EQ(ctx.GetRenderTarget(), static_cast<ID2D1HwndRenderTarget*>(nullptr));
    ASSERT_EQ(ctx.GetSolidBrush(), static_cast<ID2D1SolidColorBrush*>(nullptr));
}
