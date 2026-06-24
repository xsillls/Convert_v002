#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <map>

#include "../Header.h"
#include "Assembler.h"

namespace fs = std::filesystem;


inline int Inverter(const std::string& name)
{
    std::ifstream file(name, std::ios::binary);

    if (!file)
    {
        std::cout << "Ошибка открытия файла\n";
        return 1;
    }

    Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(Header));

    if (!file || memcmp(header.magic, "XXS1", 4) != 0)
    {
        std::cout << "Это не .xxs файл\n";
        return 1;
    }

    std::string name_str(header.name, 64);
    name_str = name_str.c_str();

    std::string ext_str(header.ext, 16);
    ext_str = ext_str.c_str();

    std::cout << "Имя: " << name_str << "\n";
    std::cout << "Расширение: " << ext_str << "\n";
    std::cout << "Размер файла: " << header.file_size << "\n";
    std::cout << "Часть: " << header.part << "/" << header.total_parts << "\n";
    std::cout << "Размер куска: " << header.chunk_size << "\n";

    return 0;
}


inline int CheckAndAssemble(const std::string& start_path, std::atomic<float>& progress)
{
    fs::path base_path(start_path);

  

    if (!fs::exists(base_path))
        return 1;

    if (fs::is_regular_file(base_path))
    {
        base_path = base_path.parent_path();
        if (base_path.empty())
            base_path = ".";
    }

    base_path = fs::absolute(base_path);

    std::vector<PartFile> all_parts;

    // ================= ПОИСК =================
    std::vector<fs::path> files;

    try
    {
        for (auto& entry : fs::recursive_directory_iterator(base_path))
        {
            if (entry.is_regular_file())
                files.push_back(entry.path());
        }
    }
    catch (...)
    {
        return 1;
    }

    int total_files = (int)files.size();
    int processed_files = 0;

    for (auto& path : files)
    {
        processed_files++;

        if (path.extension() == ".xxs")
        {
            std::ifstream file(path, std::ios::binary);
            if (!file) continue;

            PartFile p;
            p.path = path.string();

            file.read(reinterpret_cast<char*>(&p.header), sizeof(Header));

            if (!file || memcmp(p.header.magic, "XXS1", 4) != 0)
                continue;

            all_parts.push_back(p);
        }

       
        float target = ((float)processed_files / (float)total_files) * 0.3f;

        
        progress = progress + (target - progress) * 0.1f;
    }

    if (all_parts.empty())
        return 1;

    // ================= ГРУППИРОВКА =================
    std::map<std::string, std::vector<PartFile>> groups;

    for (auto& p : all_parts)
    {
        std::string name(p.header.name, 64); name = name.c_str();
        std::string ext(p.header.ext, 16);   ext = ext.c_str();

        std::string key = name + "|" + ext + "|" + std::to_string(p.header.file_size);
        groups[key].push_back(p);
    }

    // ================= ОБРАБОТКА =================
    for (auto& [key, parts] : groups)
    {
        uint32_t total = parts[0].header.total_parts;

        std::vector<PartFile> unique_parts(total + 1);
        std::vector<bool> found(total + 1, false);

        for (auto& p : parts)
        {
            uint32_t part = p.header.part;

            if (part == 0 || part > total)
                continue;

            if (!found[part])
            {
                unique_parts[part] = p;
                found[part] = true;
            }
        }

        bool ok = true;

        for (uint32_t i = 1; i <= total; i++)
        {
            if (!found[i])
            {
                ok = false;
                break;
            }
        }

        if (!ok)
            continue;

        // ================= СБОРКА =================
        std::vector<PartFile> final_parts;

        for (uint32_t i = 1; i <= total; i++)
            final_parts.push_back(unique_parts[i]);

        

        int result = Assemble(final_parts, progress);

        progress = 1.0f;
        return result;
    }

    return 1;
}