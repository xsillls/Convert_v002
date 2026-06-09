#include "System.h"
#include <thread>
#include <atomic>


std::atomic<float> g_Progress = 0.0f;
std::atomic<bool> g_Working = false;
std::atomic<bool> g_Done = false;

File_List* File_List::Get()
{
    static File_List instance;
    return &instance;
}

void File_List::Remove(const std::string& path)
{
    for (auto it = files.begin(); it != files.end(); )
    {
        if (it->path == path)
            it = files.erase(it);
        else
            ++it;
    }
}

void File_List::Add(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return;

    Header h{};
    file.read(reinterpret_cast<char*>(&h), sizeof(Header));

    if (!file || memcmp(h.magic, "XXS1", 4) != 0)
        return;

    std::string name(h.name, 64); name = name.c_str();
    std::string ext(h.ext, 16);   ext = ext.c_str();

    // 🔥 проверяем уже добавленные
    for (auto& f : files)
    {
        std::ifstream f2(f.path, std::ios::binary);
        if (!f2) continue;

        Header h2{};
        f2.read(reinterpret_cast<char*>(&h2), sizeof(Header));

        if (!f2 || memcmp(h2.magic, "XXS1", 4) != 0)
            continue;

        std::string name2(h2.name, 64); name2 = name2.c_str();
        std::string ext2(h2.ext, 16);   ext2 = ext2.c_str();

        // ✅ ЕСЛИ ЭТО ТОТ ЖЕ ФАЙЛ → НЕ ДОБАВЛЯЕМ
        if (name == name2 && ext == ext2 && h.file_size == h2.file_size)
        {
            return;
        }
    }

    // ✅ если не найден → добавляем
    files.push_back(File_Entry(path));
}

std::vector<File_Entry>& File_List::GetFiles()
{
    return files;
}


std::string Format_text(const std::string& text)
{
    const int MAX = 40;

    if ((int)text.size() <= MAX)
        return text;

    int keep = (MAX - 3) / 2;      // сколько символов оставляем слева и справа

    std::string left = text.substr(0, keep);
    std::string right = text.substr(text.size() - keep, keep);

    return left + "..." + right;
}

void Form_Button(File_Entry& object, int state)
{
    float rowHeight = 40.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));

    ImGui::BeginChild(
        ("row_" + object.path).c_str(),
        ImVec2(0, rowHeight),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );
    {
        ImGui::SetCursorPosX(40);

        float textY = (rowHeight - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(textY);
        ImGui::TextUnformatted(Format_text(object.path).c_str());

        float btnWidth = 120.0f;
        float btnHeight = 32.0f;

        float btnX = ImGui::GetWindowWidth() - btnWidth - 10;
        float btnY = (rowHeight - btnHeight) * 0.5f;

        ImGui::SetCursorPos(ImVec2(btnX, btnY));
        

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));

        bool pressed = ImGui::Button(("##" + object.path).c_str(), ImVec2(btnWidth, btnHeight));

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        float progress = object.progress;

        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetItemRectMin();

        // ===== ПРОГРЕСС =====
        if (object.working)
        {
            draw->AddRectFilled(
                pos,
                ImVec2(pos.x + btnWidth * progress, pos.y + btnHeight),
                IM_COL32(100, 140, 255, 120),
                8.0f
            );
        }

        // ===== ТЕКСТ =====
        if (!object.working && !object.done)
        {
            draw->AddText(ImVec2(pos.x + 35, pos.y + 8),
                IM_COL32(255, 255, 255, 255), "Convert");
        }
        else if (object.working)
        {
            char buf[32];
            sprintf_s(buf, "%d%%", (int)(progress * 100));

            draw->AddText(ImVec2(pos.x + 10, pos.y + 8),
                IM_COL32(255, 255, 255, 255), "Convert");

            draw->AddText(ImVec2(pos.x + btnWidth - 45, pos.y + 8),
                IM_COL32(200, 200, 200, 255), buf);
        }
        else if (object.done)
        {
            draw->AddText(ImVec2(pos.x + 40, pos.y + 8),
                IM_COL32(120, 255, 120, 255), "Done");
        }

        // ===== ЛОГИКА =====
        if (pressed && !object.working)
{
    std::string path = object.path;

    // ✅ СБРОС СРАЗУ (в UI потоке)
    object.progress = 0.0f;
    object.working = true;
    object.done = false;
    object.doneTimer = 0.0f;

    std::thread([path]()
    {
        auto& files = File_List::Get()->GetFiles();

        for (auto& f : files)
        {
            if (f.path == path)
            {
                CheckAndAssemble(path, f.progress);

                f.progress = 1.0f;
                f.working = false;
                f.done = true;
                break;
            }
        }

    }).detach();
}


            // ===== DONE TIMER =====
            if (object.done)
            {
                object.doneTimer += ImGui::GetIO().DeltaTime;

                if (object.doneTimer >= 5.0f)
                {
                    std::string path = object.path; // 💣 сохраняем перед удалением

                    object.done = false;
                    object.progress = 0.0f;
                    object.progress = 0.0f;
                    object.doneTimer = 0.0f;

                    File_List::Get()->Remove(path);
                }
            }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}

void File_List::Update_Gui()
{
    if (files.empty())
    {
        return;
    }
    for (auto& f : files)
    {
        
        Form_Button(f, 1);
    }
}