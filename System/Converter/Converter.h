#include "../System.h"

namespace fs = std::filesystem; // сокращение для удобства
inline size_t my_min(size_t a, size_t b) // функция-замена std::min
{
    return (a < b) ? a : b; // возвращаем меньшее из двух значений
}



inline int SplitFile(const std::string& path, int chunk_mb) // функция разделения файла
{
    std::ifstream file(path, std::ios::binary); // открываем файл в бинарном режиме
    if (!file) // проверка на ошибку открытия
    {
        std::cout << "Ошибка открытия файла\n"; // сообщение об ошибке
        return 1; // выход
    }

    uint64_t file_size = fs::file_size(path); // получаем размер файла
    uint64_t total_read = 0;

    std::string name = fs::path(path).stem().string(); // имя файла без расширения
    std::string ext = fs::path(path).extension().string(); // расширение файла

    if (!ext.empty() && ext[0] == '.') // если расширение начинается с точки
        ext.erase(0, 1); // удаляем точку

    uint64_t chunk_size = (uint64_t)chunk_mb * 1024 * 1024; // размер чанка в байтах

    uint32_t total_parts = (file_size + chunk_size - 1) / chunk_size; // считаем количество частей (округление вверх)

    fs::path out_dir = name + "_parts"; // имя папки для частей
    fs::create_directory(out_dir); // создаём папку

    char name_buf[64] = {}; // буфер под имя (фиксированный размер)
    char ext_buf[16] = {}; // буфер под расширение

    memcpy(name_buf, name.c_str(), my_min((size_t)63, name.size())); // копируем имя (не больше 63 символов)
    memcpy(ext_buf, ext.c_str(), my_min((size_t)15, ext.size())); // копируем расширение (не больше 15 символов)

    // 🔥 фикс буфер (ключ к норм производительности)
    const size_t BUFFER_SIZE = 1024 * 1024; // размер буфера 1MB
    std::vector<char> buffer(BUFFER_SIZE); // создаём буфер

    for (uint32_t part = 1; part <= total_parts; part++) // цикл по всем частям
    {
        fs::path out_file = out_dir / (name + "_" + std::to_string(part) + ".xxs"); // имя выходного файла

        std::ofstream out(out_file, std::ios::binary); // открываем файл для записи
        if (!out) // проверка
        {
            std::cout << "Ошибка создания файла\n"; // ошибка
            return 1; // выход
        }

        Header header{}; // создаём header (обнуляется)
        memcpy(header.magic, "XXS1", 4); // сигнатура файла
        memcpy(header.name, name_buf, 64); // записываем имя
        memcpy(header.ext, ext_buf, 16); // записываем расширение

        header.file_size = file_size; // общий размер файла
        header.part = part; // номер текущей части
        header.total_parts = total_parts; // общее количество частей

        uint32_t written = 0; // сколько записано в текущую часть

        // 📦 сначала резервируем место под header
        out.seekp(sizeof(Header)); // пропускаем место под header

        while (written < chunk_size && file) // пока не достигли лимита и файл читается
        {
            size_t to_read = my_min(BUFFER_SIZE, chunk_size - written); // сколько читать за раз

            file.read(buffer.data(), to_read); // читаем из исходного файла
            std::streamsize read_bytes = file.gcount(); // сколько реально прочитали

            if (read_bytes <= 0) // если ничего не прочитали
                break; // выходим

            out.write(buffer.data(), read_bytes);
            written += (uint32_t)read_bytes;

            total_read += read_bytes;
            g_Progress = (float)total_read / (float)file_size;
        }

        header.chunk_size = written; // записываем реальный размер чанка

        // 🔥 возвращаемся в начало и пишем header
        out.seekp(0); // идём в начало файла
        out.write(reinterpret_cast<char*>(&header), sizeof(Header)); // записываем header

        std::cout << "[+] " << out_file << "\n"; // выводим имя созданного файла
    }

    std::cout << "Готово ✅\n"; // сообщение о завершении
    return 0; // успех
}