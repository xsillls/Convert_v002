#include <thread>
#include "Menu.h"

bool RoundedButton(const char* id, const char* text, float offset_x = 0.0f)
{
    ImVec2 size(276, 36);
    float rounding = 8.0f;
    ImGui::SetCursorPosY(2);

    ImU32 color = IM_COL32(0, 0, 0, 100);
    ImU32 color_hovered = IM_COL32(10, 10, 10, 100);
    ImU32 color_active = IM_COL32(10, 10, 10, 200);

    ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPosX(cursor.x + offset_x);

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton(id, size);

    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    bool pressed = ImGui::IsItemClicked();

    ImU32 col =
        active ? color_active :
        hovered ? color_hovered :
        color;

    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        col,
        rounding
    );

    ImVec2 ts = ImGui::CalcTextSize(text);
    draw->AddText(
        ImVec2(
            pos.x + (size.x - ts.x) * 0.5f,
            pos.y + (size.y - ts.y) * 0.5f
        ),
        IM_COL32(255, 255, 255, 255),
        text
    );


    ImGui::SetCursorPos(cursor);

    return pressed;
}
std::string ShortenPath(const std::string& path, size_t max_len)
{
    if (path.size() <= max_len)
        return path;

    if (max_len < 5) 
        return "...";

    size_t keep = (max_len - 3) / 2;

    std::string left = path.substr(0, keep);
    std::string right = path.substr(path.size() - keep);

    return left + "..." + right;
}

std::string GetFileSizeFormatted(const std::string& path)
{
    try
    {
        uint64_t size = std::filesystem::file_size(path);

        double mb = size / (1024.0 * 1024.0);
        double gb = mb / 1024.0;

        char buffer[64];

        if (gb >= 1.0)
            sprintf_s(buffer, "%.1f GB", gb);
        else
            sprintf_s(buffer, "%.1f MB", mb);

        return buffer;
    }
    catch (...)
    {
        return "0 MB";
    }
}

uint32_t GetPartsCount(const std::string& path, double part_mb)
{
    try
    {
        uint64_t size = std::filesystem::file_size(path);

        uint64_t part_size = (uint64_t)(part_mb * 1024.0 * 1024.0);

        if (part_size == 0)
            return 0;

        uint32_t parts = size / part_size;

      
        if (size % part_size != 0)
            parts++;

        return parts;
    }
    catch (...)
    {
        return 0;
    }
}

static float g_slider = 0.5f;
static float parts = 100;
void DrawMainUI()
{
    
    size_t pos = g_FilePath.find_last_of("\\/");

    std::string filename = g_FilePath;
    std::string folder = "";

    if (pos != std::string::npos)
    {
        filename = g_FilePath.substr(pos + 1);
        folder = g_FilePath.substr(0, pos);
    }

    ImGui::SetCursorPos(ImVec2(20, 80));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(50/255, 50 / 255, 50 / 255, 0.15f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("##box", ImVec2(278, 110), false);
    ImGui::SetCursorPos(ImVec2(20, 20));
    DrawFileIcon(46.f);
    ImGui::SetCursorPos(ImVec2(80, 20));
    ImGui::Text("%s", filename.c_str());
    ImGui::SetCursorPos(ImVec2(80, 35));
    ImGui::Text("%s", ShortenPath(g_FilePath, 25).c_str());
    ImGui::SetCursorPos(ImVec2(80, 50));
    ImGui::Text("%s", GetFileSizeFormatted(g_FilePath).c_str());
    ImGui::SetCursorPos(ImVec2(30, 70));
    int parts_int = (int)parts;
    ImGui::Text("x%d", GetPartsCount(g_FilePath, parts_int));
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(303, 80));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(50 / 255, 50 / 255, 50 / 255, 0.15f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("##box2", ImVec2(270, 50), false);
    ImGui::SetCursorPos(ImVec2(11, 13));

    if (DrawStyledButton("Telegram", ImVec2(120, 26)))
    {
        parts = 1500;
    }

    ImGui::SameLine();

    if (DrawStyledButton("Discord", ImVec2(120, 26)))
    {
        parts = 8;
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    
    ImGui::SetCursorPos(ImVec2(303, 139));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(50 / 255, 50 / 255, 50 / 255, 0.15f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("##box3", ImVec2(270, 50), false);
    ImGui::SetCursorPos(ImVec2(16, 13));
    
    

    ImGui::SetCursorPos(ImVec2(20, 10));

    DrawStyledSlider("##slider", &parts, 1, 2000, ImVec2(230, 15));
    ImGui::SetCursorPos(ImVec2(30, 30));
    ImGui::Text("%.0f mb", parts);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(395, 280));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(50 / 255, 50 / 255, 50 / 255, 0.15f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("##box4", ImVec2(180, 50), false);



    ImGui::SetCursorPos(ImVec2(10, 10));

    if (DrawStyledButton("Split", ImVec2(160, 30)))
    {
        if (!g_Working)
        {
            std::string path = g_FilePath;
            int chunk = (int)parts;

            g_Working = true;
            g_Progress = 0.0f;
            g_Done = false;

            std::thread([path, chunk]()
                {
                    SplitFile(path, chunk); 

                    g_Working = false;
                    g_Done = true;

                }).detach();
        }
    }
    
    
    
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    
    static float doneTimer = 0.0f;
    ImGui::SetCursorPos(ImVec2(30, 320));

    if (g_Working)
    {
        float progress = g_Progress.load();

        DrawProgressBar("##progress", progress, ImVec2(230, 12));

        doneTimer = 0.0f;
    }
    else if (g_Done)
    {
        doneTimer += ImGui::GetIO().DeltaTime;

        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Done");

        if (doneTimer >= 5.0f)
        {
            g_Done = false;
            g_Progress = 0.0f;
            doneTimer = 0.0f;
        }
    }

}

void Menu()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGui::Begin("Loader##main", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBackground |         
        ImGuiWindowFlags_NoSavedSettings         
    );

    ImDrawList* dl = ImGui::GetWindowDrawList();

  
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::PushID("drag_zone");
    ImGui::InvisibleButton("##drag", ImVec2(500, 20));
    ImGui::PopID();

    static bool dragging = false;
    static POINT drag_offset;

    if (ImGui::IsItemActive())
    {
        POINT mouse;
        GetCursorPos(&mouse);

        if (!dragging)
        {
            dragging = true;

            RECT r;
            GetWindowRect(g_hwnd, &r);

           
            drag_offset.x = mouse.x - r.left;
            drag_offset.y = mouse.y - r.top;
        }

        SetWindowPos(
            g_hwnd,
            nullptr,
            mouse.x - drag_offset.x,
            mouse.y - drag_offset.y,
            0, 0,
            SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }
    else
    {
        dragging = false;
    }


    Uploid_file();

    // ================= UI =================

 
    dl->AddRectFilled(
        ImVec2(0, 30),
        ImVec2(600, 350),
        IM_COL32(20, 20, 20, 160),
        14.0f
    );

 
    dl->AddRectFilled(
        ImVec2(500, 1),
        ImVec2(595, 25),
        IM_COL32(20, 20, 20, 160),
        14.0f
    );

    dl->AddRectFilled(
        ImVec2(5, 1),
        ImVec2(205, 25),
        IM_COL32(20, 20, 20, 160),
        14.0f
    );

    ImGui::SetCursorPos(ImVec2(10, 5));
    ImGui::Text("%s", Logo_Anim("hello").c_str());

    ImGui::SetCursorPos(ImVec2(8, 40));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  

    ImGui::SetCursorPosX(20);
    ImGui::SetCursorPosY(35);
    ImGui::BeginChild(
        "WorkArea_Button",
        ImVec2(562, 40),  
        false,          
        ImGuiWindowFlags_None   
    );
    {
        if (RoundedButton("1", "Compress", 2))
            Button_Menu = 1;
        if (RoundedButton("2", "Extract", 283))
            Button_Menu = 2;
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();

    

    if (Button_Menu == 1)
    {
        ImGui::SetCursorPos(ImVec2(8, 40));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0)); 
        ImGui::SetCursorPosX(20);
        ImGui::SetCursorPosY(50 + 50);
        ImGui::BeginChild(
            "WorkArea",
            ImVec2(562, 150
            ),   
            false,            
            ImGuiWindowFlags_None   
        );
        {
            File_List::Get()->Update_Gui();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }
    
    if (Button_Menu == 2)
    {
        if (!Button_Menu_Extract)
        {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 text_size = ImGui::CalcTextSize("Centered");

            ImGui::SetCursorPos(ImVec2(
                (avail.x - text_size.x) * 0.5f,
                (avail.y - text_size.y) * 0.5f
            ));

            ImGui::Text("Nothing");
            
        }
        else
        {
            DrawMainUI();
        }
    }

    Button_close(dl);

    ImGui::End();
}
