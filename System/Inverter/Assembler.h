#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cstring>

#include "../System.h"
#include "../Header.h"

namespace fs = std::filesystem;




inline int Assemble(std::vector<PartFile> parts, std::atomic<float>& progress)
{
    int index = 0;
    int total = parts.size();
    if (parts.empty())
    {
        std::cout << "Нет частей для сборки\n";
        return 1;
    }

    // 🔽 сортировка (обязательно)
    std::sort(parts.begin(), parts.end(),
        [](const PartFile& a, const PartFile& b)
        {
            return a.header.part < b.header.part;
        });

    // имя файла
    std::string name(parts[0].header.name, 64);
    name = name.c_str();

    std::string ext(parts[0].header.ext, 16);
    ext = ext.c_str();

    std::string out_name = name + "." + ext;

    std::ofstream out(out_name, std::ios::binary);
    if (!out)
    {
        std::cout << "Ошибка создания файла\n";
        return 1;
    }

    // 🔥 сборка
    for (const auto& p : parts)
    {
        std::ifstream file(p.path, std::ios::binary);
        if (!file)
        {
            std::cout << "Ошибка открытия: " << p.path << "\n";
            continue;
        }

        Header h;
        file.read(reinterpret_cast<char*>(&h), sizeof(Header));

        if (!file || memcmp(h.magic, "XXS1", 4) != 0)
        {
            std::cout << "Пропуск: " << p.path << "\n";
            continue;
        }

        std::vector<char> buffer(h.chunk_size);
        file.read(buffer.data(), buffer.size());

        if (!file)
        {
            std::cout << "Ошибка чтения: " << p.path << "\n";
            continue;
        }

        out.write(buffer.data(), buffer.size());
        index++;
        progress = (float)index / (float)total;
    }

    std::cout << "Собрано: " << out_name << " ✅\n";
    return 0;
}