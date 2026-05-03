#pragma once

#include "config.h"

using namespace fs;

uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
uint16_t changeBrightness(uint16_t color, float factor);

int files_number(File path);
int folders_number(File path);

void list_files(File dir, String path, std::vector<String> &paths);
void list_files_non_recursive(File dir, String path, std::vector<String> &paths);

bool read_bmp_dimensions(const char *filename, uint16_t &w, uint16_t &h);

bool file_exists(const char* path);
bool dir_exists(const char* path);
bool write_file(const char* path, const char* content);
bool create_file(const char* path);
bool create_dir(const char* path);
String read_file(const char* path);