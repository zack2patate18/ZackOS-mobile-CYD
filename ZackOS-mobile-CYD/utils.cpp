#include "utils.h"

using namespace fs;


uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r * 31) / 255;
    uint16_t g6 = (g * 63) / 255;
    uint16_t b5 = (b * 31) / 255;

    return (r5 << 11) | (g6 << 5) | b5;
}

uint8_t r_from565(uint16_t color) { return ((color >> 11) & 0x1F) * 255 / 31; }
uint8_t g_from565(uint16_t color) { return ((color >> 5) & 0x3F) * 255 / 63; }
uint8_t b_from565(uint16_t color) { return (color & 0x1F) * 255 / 31; }

uint16_t changeBrightness(uint16_t color, float factor) {
    uint8_t r = r_from565(color);
    uint8_t g = g_from565(color);
    uint8_t b = b_from565(color);

    r = (r * factor > 255) ? 255 : uint8_t(r * factor);
    g = (g * factor > 255) ? 255 : uint8_t(g * factor);
    b = (b * factor > 255) ? 255 : uint8_t(b * factor);

    return color565(r, g, b);
}

int files_number(File path) {
    int files_count = 0;
    while (true) {
        File entry = path.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            files_count++;
        }
        entry.close();
    }

    return files_count;
}

int folders_number(File path) {
    int folder_count = 0;
    while (true) {
        File entry = path.openNextFile();
        if (!entry) {
            break;
        }

        if (entry.isDirectory()) {
            folder_count++;
        }
        entry.close();

    }

    return folder_count;
}

void list_files(File dir, String path, std::vector<String> &paths) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        String fullPath = path + "/" + entry.name();
        paths.push_back(fullPath);

        if (entry.isDirectory()) {
            list_files(entry, fullPath, paths);
        }
        entry.close();
    }
}

void list_files_non_recursive(File dir, String path, std::vector<String> &paths) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        String fullPath = path + "/" + entry.name();
        paths.push_back(fullPath);
        entry.close();
    }
}

bool read_bmp_dimensions(const char *filename, uint16_t &w, uint16_t &h) {
    File bmpFile = SD.open(filename);
    if (!bmpFile) return false;

    bmpFile.seek(18);
    w = bmpFile.read() | (bmpFile.read() << 8);
    h = bmpFile.read() | (bmpFile.read() << 8);

    bmpFile.close();
    return true;
}

bool file_exists(const char* path) {
    File f = SD.open(path);
    if (!f) return false;
    bool is_file = !f.isDirectory();
    f.close();
    return is_file;
}

bool dir_exists(const char* path) {
    File f = SD.open(path);
    if (!f) return false;
    bool is_dir = f.isDirectory();
    f.close();
    return is_dir;
}

bool write_file(const char* path, const char* content) {
    File f = SD.open(path, FILE_WRITE);
    if (!f) return false;
    f.print(content);
    f.close();
    return true;
}

bool create_file(const char* path) {
    if (file_exists(path)) return true;
    File f = SD.open(path, FILE_WRITE);
    if (!f) return false;
    f.close();
    return true;
}

bool create_dir(const char* path) {
    if (dir_exists(path)) return true;
    return SD.mkdir(path);
}