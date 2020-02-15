#include "Widget.h"
#include "types/String.h"

class TextView : public Widget
{
public:
    TextView(u16 x, u16 y, u16 width, u16 height);
    ~TextView() = default;

    void draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const override;

private:
    String m_text;
};