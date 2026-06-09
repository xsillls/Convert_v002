#include "Menu.h"
void DrawFileIcon(float size)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("##icon", ImVec2(size, size));

    ImDrawList* draw = ImGui::GetWindowDrawList();

    float w = size;
    float h = size;

    ImU32 bg = IM_COL32(180, 180, 180, 255);
    ImU32 line = IM_COL32(40, 40, 40, 255);

    draw->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), bg, 6.0f);

    float fold = w * 0.3f;

    draw->AddTriangleFilled(
        ImVec2(pos.x + w - fold, pos.y),
        ImVec2(pos.x + w, pos.y),
        ImVec2(pos.x + w, pos.y + fold),
        IM_COL32(100, 100, 100, 255)
    );

    float padding = w * 0.2f;
    float line_w = w - padding * 2;

    float y_start = pos.y + h * 0.35f;
    float spacing = h * 0.12f;

    for (int i = 0; i < 3; i++)
    {
        float y = y_start + i * spacing;

        draw->AddLine(
            ImVec2(pos.x + padding, y),
            ImVec2(pos.x + padding + line_w, y),
            line,
            2.0f
        );
    }
}