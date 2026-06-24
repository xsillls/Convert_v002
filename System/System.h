#pragma once
#include <atomic>
#include <string>
#include <oleidl.h> 
#include <vector>
#include <iostream>
#include <fstream>

#include <filesystem>
#include <cstring>



extern std::atomic<float> g_Progress;
extern std::atomic<bool> g_Working;

extern std::atomic<bool> g_Done;


#include "../ImGui/imgui.h"
#include "Header.h"
#include "Converter/Converter.h"
#include "Inverter/Assembler.h"
#include "Inverter/Inverter.h"

extern bool g_IsDraggingFile;
extern bool g_FileDropped;
extern std::string g_FilePath;
extern int Button_Menu;
extern bool Button_Menu_Extract;







IDropTarget* CreateDropTarget();


class File_Entry
{
public:
    std::string path;

    std::atomic<float> progress{ 0.0f };
    bool working = false;
    bool done = false;
    float doneTimer = 0.0f;

    File_Entry(const std::string& p) : path(p) {}


    File_Entry(const File_Entry&) = delete;
    File_Entry& operator=(const File_Entry&) = delete;


    File_Entry(File_Entry&& other) noexcept
        : path(std::move(other.path)),
        progress(other.progress.load()),
        working(other.working),
        done(other.done)
    {
    }

    File_Entry& operator=(File_Entry&& other) noexcept
    {
        path = std::move(other.path);
        progress = other.progress.load();
        working = other.working;
        done = other.done;
        return *this;
    }
};


class File_List
{
    std::vector<File_Entry> files;

public:
	static File_List* Get();
    void Remove(const std::string& path);
    void Add(const std::string& path);
    std::vector<File_Entry>& GetFiles();

    void Update_Gui();
};

class Convert
{
	Convert() = default;

public:
	Convert(const Convert&) = delete;
	Convert& operator=(const Convert&) = delete;

    static Convert& Get();
	

};