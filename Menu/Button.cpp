#include "Menu.h"
#include "../ImGui/imgui_internal.h"

void Uploid_file()
{
    //ImGui::Text(std::to_string(g_IsDraggingFile).c_str());
    //ImGui::Text(std::to_string(g_FileDropped).c_str());
    //ImGui::Text(g_FilePath.c_str());
}
bool DrawStyledButton(const char* label, ImVec2 size)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();

    
    ImGui::InvisibleButton((std::string(label) + "##btn").c_str(), size);

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImU32 bg = hovered
        ? IM_COL32(60, 60, 70, 255)
        : IM_COL32(40, 40, 50, 255);

    draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg, 10.0f);

    ImVec2 text_size = ImGui::CalcTextSize(label);

    draw->AddText(
        ImVec2(
            pos.x + (size.x - text_size.x) * 0.5f,
            pos.y + (size.y - text_size.y) * 0.5f
        ),
        IM_COL32(220, 220, 220, 255),
        label
    );

    return clicked;
}
void DrawProgressBar(const char* id, float progress, ImVec2 size)
{
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    
    ImGui::InvisibleButton(id, size);

    
    progress = std::clamp(progress, 0.0f, 1.0f);

    float rounding = size.y * 0.5f;

   
    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(40, 40, 50, 180),
        rounding
    );

  
    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x * progress, pos.y + size.y),
        IM_COL32(100, 140, 255, 255),
        rounding
    );

    
    char buf[32];
    sprintf_s(buf, "%d%%", (int)(progress * 100));

    ImVec2 text_size = ImGui::CalcTextSize(buf);

    draw->AddText(
        ImVec2(
            pos.x + (size.x - text_size.x) * 0.5f,
            pos.y + (size.y - text_size.y) * 0.5f
        ),
        IM_COL32(255, 255, 255, 255),
        buf
    );
}

bool DrawStyledSlider(const char* id, float* value, float min, float max, ImVec2 size)
{
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton(id, size);

    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    float t = (*value - min) / (max - min);
    t = std::clamp(t, 0.0f, 1.0f);

    float rounding = size.y * 0.5f;


    ImU32 bg = IM_COL32(35, 35, 40, 180);
    ImU32 fill = IM_COL32(90, 90, 100, 220);


    ImU32 grab = IM_COL32(140, 140, 150, 255);

    if (hovered)
        grab = IM_COL32(170, 170, 180, 255);

    if (active)
        grab = IM_COL32(200, 200, 210, 255);


    float r = size.y * 0.45f;
    float grab_x = pos.x + t * (size.x - r * 2) + r;

    // ===== фон =====
    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        bg,
        rounding
    );

    // ===== заполнение =====
    draw->AddRectFilled(
        pos,
        ImVec2(grab_x, pos.y + size.y),
        fill,
        rounding
    );

    // ===== кружок (вписан внутрь) =====
    draw->AddCircleFilled(
        ImVec2(grab_x, pos.y + size.y * 0.5f),
        r,
        grab
    );

    // ===== логика =====
    if (active)
    {
        float mouse = ImGui::GetIO().MousePos.x;

        float new_t = (mouse - pos.x - r) / (size.x - r * 2);
        new_t = std::clamp(new_t, 0.0f, 1.0f);

        *value = min + (max - min) * new_t;
        return true;
    }

    return false;
}
void Button_close(ImDrawList* dl)
{
    ImGui::PushID(dl); 

    ImVec2 pos = ImGui::GetWindowPos();

    // ================= MINIMIZE =================
    ImVec2 min_pos = { pos.x + 510.0f, pos.y + 3.0f };

    ImGui::SetCursorScreenPos(min_pos);
    ImGui::InvisibleButton("##minimize", ImVec2(20.0f, 20.0f));

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && g_hwnd)
        ShowWindow(g_hwnd, SW_MINIMIZE);

    bool min_hovered = ImGui::IsItemHovered();
    float min_offset = min_hovered ? -3.0f : 0.0f;

    dl->AddLine(
        { min_pos.x + 4.0f, min_pos.y + 12.0f + min_offset },
        { min_pos.x + 16.0f, min_pos.y + 12.0f + min_offset },
        IM_COL32(220, 220, 220, 255),
        2.0f
    );

    // ================= CLOSE =================
    ImVec2 close_pos = { pos.x + 565.0f, pos.y + 3.0f };

    ImGui::SetCursorScreenPos(close_pos);
    ImGui::InvisibleButton("##close", ImVec2(20.0f, 20.0f));

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        PostQuitMessage(0);

    bool close_hovered = ImGui::IsItemHovered();

    if (!close_hovered)
    {
        dl->AddLine({ close_pos.x + 5, close_pos.y + 5 },
            { close_pos.x + 15, close_pos.y + 15 },
            IM_COL32(220, 60, 60, 255), 2.0f);

        dl->AddLine({ close_pos.x + 15, close_pos.y + 5 },
            { close_pos.x + 5, close_pos.y + 15 },
            IM_COL32(220, 60, 60, 255), 2.0f);
    }
    else
    {
        dl->AddLine({ close_pos.x + 8, close_pos.y + 4 },
            { close_pos.x + 8, close_pos.y + 16 },
            IM_COL32(220, 60, 60, 255), 2.0f);

        dl->AddLine({ close_pos.x + 12, close_pos.y + 4 },
            { close_pos.x + 12, close_pos.y + 16 },
            IM_COL32(220, 60, 60, 255), 2.0f);
    }

    ImGui::PopID();
}
