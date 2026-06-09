#pragma once
#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct Header
{
    char magic[4];      // XXS1

    char name[64];
    char ext[16];

    uint64_t file_size;

    uint32_t part;
    uint32_t total_parts;

    uint32_t chunk_size; // размер данных в этом файле
};
#pragma pack(pop)

// 📦 общая структура для всех файлов
struct PartFile
{
    std::string path;
    Header header;
};