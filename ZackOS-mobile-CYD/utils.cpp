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