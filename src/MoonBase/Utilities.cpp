/**
    @title     MoonBase
    @file      Utilities.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#include "Utilities.h"

//ArduinoJson functions

bool arrayContainsValue(JsonArray array, int value) {
    for (JsonVariant v : array) {
        if (v == value) {
            return true;
        }
    }
    return false;
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
  return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

void extractPath(const char *filepath, char *path) {
    const char *lastSlash = strrchr(filepath, '/');
    if (lastSlash != NULL) {
        size_t pathLength = lastSlash - filepath;
        strncpy(path, filepath, pathLength);
        path[pathLength] = '\0';
    } else {
        // No directory separator found, the entire filepath is the filename
        strcpy(path, "");
    }
}

void walkThroughFiles(File folder, std::function<void(File, File)> fun) {
	folder.rewindDirectory();
	while (true)
	{
		File file = folder.openNextFile();
        if (!file) break;

        fun(folder, file);
        
        if (file.isDirectory()) {
            walkThroughFiles(file, fun);
        }
        file.close();
    }
}

bool copyFile(const char* srcPath, const char* dstPath) {
    File src = ESPFS.open(srcPath, "r");
    if (!src) {
        Serial.printf("Failed to open source file: %s\n", srcPath);
        return false;
    }
    // Check if the destination folders already exist
    String dstDir = String(dstPath).substring(0, String(dstPath).lastIndexOf('/'));
    if (!ESPFS.exists(dstDir.c_str())) {
        if (!ESPFS.mkdir(dstDir.c_str())) {
            Serial.printf("Failed to create destination directory: %s\n", dstDir.c_str());
            src.close();
            return false;
        }
    }

    File dst = ESPFS.open(dstPath, "w");
    if (!dst) {
        Serial.printf("Failed to open destination file: %s\n", dstPath);
        src.close();
        return false;
    }

    uint8_t buf[512];
    size_t n;
    while ((n = src.read(buf, sizeof(buf))) > 0) {
        if (dst.write(buf, n) != n) {
            Serial.println("Write failed!");
            src.close();
            dst.close();
            return false;
        }
    }

    src.close();
    dst.close();
    return true;
}

bool isInPSRAM(void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    #if CONFIG_IDF_TARGET_ESP32
        return (addr >= 0x3F800000 && addr < 0x40000000);
    #elif CONFIG_IDF_TARGET_ESP32S2
        return (addr >= 0x3F500000 && addr < 0x3FF80000);
    #elif CONFIG_IDF_TARGET_ESP32S3
        return (addr >= 0x3C000000 && addr < 0x3E000000);
    #endif
}