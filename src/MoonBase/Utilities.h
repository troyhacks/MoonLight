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

#pragma once

#include <Arduino.h>
#include "ArduinoJson.h"
#include <ESPFS.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x) //e.g. for pio.ini settings (see ML_CHIPSET)

#define MB_LOGE(tag, fmt, ...) ESP_LOGE(tag, "[%s:%d] %s: " fmt, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGW(tag, fmt, ...) ESP_LOGW(tag, "[%s:%d] %s: " fmt, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGI(tag, fmt, ...) ESP_LOGI(tag, "[%s:%d] %s: " fmt, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGD(tag, fmt, ...) ESP_LOGD(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGV(tag, fmt, ...) ESP_LOGV(tag, "[%s:%d] %s: " fmt, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define MB_TAG "🌙"
#define ML_TAG "💫"

struct Coord3D {

    uint16_t x;
    uint8_t y;
    uint8_t z;

    // Coord3D() : x(0), y(0), z(0) {} // Default constructor

    Coord3D () {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    
    //x max 2^11 -> 2047, y max 2^8 -> 255, z max 2^5 -> 31
    Coord3D (uint16_t x, uint8_t y = 0, uint8_t z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    //Minus / delta (abs)
    Coord3D operator-(const Coord3D rhs) const {
        return Coord3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Coord3D operator+(const Coord3D rhs) const {
        return Coord3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    Coord3D operator*(const Coord3D rhs) const {
        return Coord3D(x * rhs.x, y * rhs.y, z * rhs.z);
    }
    Coord3D operator/(const Coord3D rhs) const {
        return Coord3D(x / rhs.x, y / rhs.y, z / rhs.z);
    }
    Coord3D operator%(const Coord3D rhs) const {
        return Coord3D(x % rhs.x, y % rhs.y, z % rhs.z);
    }

    //assignments
    // Coord3D operator=(const Coord3D rhs) {
    //     // ppf("Coord3D assign %d,%d,%d\n", rhs.x, rhs.y, rhs.z);
    //     x = rhs.x;
    //     y = rhs.y;
    //     z = rhs.z;
    //     return *this;
    // }
    Coord3D operator+=(const Coord3D rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
      }
    
    Coord3D maximum(const Coord3D rhs) const {
        return Coord3D(max(x, rhs.x), max(y, rhs.y), max(z, rhs.z));
    }

    bool isOutofBounds(const Coord3D rhs) const {
        return x < 0 || y < 0 || z < 0 || x >= rhs.x || y >= rhs.y || z >= rhs.z;
    }
};

//https://arduinojson.org/news/2021/05/04/version-6-18-0/
namespace ArduinoJson {
  template <>
  struct Converter<Coord3D> {
    static bool toJson(const Coord3D& src, JsonVariant dst) {
      // JsonObject obj = dst.to<JsonObject>();
      dst["x"] = src.x;
      dst["y"] = src.y;
      dst["z"] = src.z;
      // ppf("Coord3D toJson %d,%d,%d -> %s\n", src.x, src.y, src.z, dst.as<String>().c_str());
      return true;
    }

    static Coord3D fromJson(JsonVariantConst src) {
      // ppf("Coord3D fromJson %s\n", src.as<String>().c_str());
      return Coord3D{src["x"], src["y"], src["z"]};
    }

    static bool checkJson(JsonVariantConst src) {
      return src["x"].is<uint16_t>() && src["y"].is<uint8_t>() && src["z"].is<uint8_t>();
    }
  };
}

bool arrayContainsValue(JsonArray array, int value);

//convenience function to compare two char strings
static bool equal(const char *a, const char *b) {
    if (a == nullptr || b == nullptr) {
        return false;
    }
    return strcmp(a, b) == 0;
}

static bool contains(const char *a, const char *b) {
    if (a == nullptr || b == nullptr) {
        return false;
    }
    return strnstr(a, b, sizeof(a)) != nullptr;
}


//See https://discord.com/channels/473448917040758787/718943978636050542/1357670679196991629
template <size_t N>
struct Char {
    char s[N] = "";
  
    //assign
    Char& operator=(const char *rhs) {
      strlcpy(s, rhs, sizeof(s));
      return *this;
    }
    Char& operator=(const JsonVariant rhs) {
        if (!rhs.isNull())
            strlcpy(s, rhs.as<String>().c_str(), sizeof(s));
        return *this;
    }
    Char& operator=(const JsonString rhs) {
        if (!rhs.isNull())
            strlcpy(s, rhs.c_str(), sizeof(s));
        return *this;
    }
    
    //concat
    Char& operator+(const char *rhs) {
      strlcat(s, rhs, sizeof(s));
      return *this;
    }
    Char& operator+(String rhs) {
        strlcat(s, rhs.c_str(), sizeof(s));
        return *this;
    }
    Char& operator+=(const char *rhs) {
      strlcat(s, rhs, sizeof(s));
      return *this;
    }
    Char& operator+=(int rhs) { //add integer to string
        char buffer[12]; // enough for 32-bit int
        snprintf(buffer, sizeof(buffer), " %d", rhs);
        strlcat(s, buffer, sizeof(s));
        return *this;
    }
    Char& operator+=(String rhs) {
        strlcat(s, rhs.c_str(), sizeof(s));
        return *this;
      }
  
    //compare
    bool operator==(const char *rhs) {
      return strcmp(s, rhs) == 0;
    }
    bool operator==(Char & rhs) {
        return strcmp(s, rhs.s) == 0;
    }
    bool operator!=(const char *rhs) {
        return strcmp(s, rhs) != 0;
    }
  
    bool contains(const char *rhs) {
      return strnstr(s, rhs, sizeof(s)) != nullptr;
    }

    size_t indexOf(const char * token) {
        return strnstr(s, token, sizeof(s)) - s;
    }

    char operator[](const uint16_t indexV) {
        return s[indexV];
    }
    
    Char<32> substring(uint16_t begin, uint16_t end = sizeof(s)-1) {
        Char<32> sub;
        if (begin >= sizeof(s) || end >= sizeof(s)) 
            sub = "";
        else {
            strlcpy(sub.s, s + begin, end - begin + 1);
        }
        return sub;
    }
  
    size_t length() {
      return strnlen(s, sizeof(s));
    }

    int toInt() {
      return atoi(s);
    }
    float toFloat() {
      return atof(s);
    }
  
    Char& format(const char * format, ...) {
      va_list args;
      va_start(args, format);
      size_t len = vsnprintf(s, sizeof(s), format, args);
      va_end(args);
      return *this;
    }
  
    const char *c_str() {
      return s;
    }

    void split(const char * splitter, std::function<void(const char *)> callback) {
        char *token = strtok(s, splitter);
        while (token != nullptr ) {
            callback(token);
            token = strtok(nullptr, splitter);
        }
    }

    void formatTime(time_t time, const char *format)
    {
        strftime(s, N, format, localtime(&time)); // or gmtime
    }
    
    //for parsing strings
    const char* eat() {
        //todo
        return nullptr;
    }
};

// Example of split:
// Char<32> test;
// test = "one - two - three";
// test.split(" - ", [](const char *token) {
//     MB_LOGV(MB_TAG, "token: %s", token);
// });

//not tested yet
struct CharHeap: public Char<1> {
    char *s = nullptr; //is the superclass char[N] not allocated?
    size_t size = 0; //test how to deal with sizeof... implement overloaded function to return the size ?
  
    CharHeap(size_t size) {
        this->size = size;
        s = new char[size];
        s[0] = '\0';
    }
  
    ~CharHeap() {
        delete[] s;
    }
  
};

float distance(float x1, float y1, float z1, float x2, float y2, float z2);

//file functions

void extractPath(const char *filepath, char *path);

void walkThroughFiles(File folder, std::function<void(File, File)> fun);

bool copyFile(const char* srcPath, const char* dstPath);

bool isInPSRAM(void* ptr);

template<typename T>
struct PSRAMAllocator {
    using value_type = T;
    
    T* allocate(size_t n) {
        T* res = (T*)heap_caps_malloc_prefer(n * sizeof(T), 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
        if (res)
            MB_LOGI(MB_TAG, "Allocated %d x %d bytes in %s", n, sizeof(T), isInPSRAM(res)?"PSRAM":"default");
        else 
            MB_LOGE(MB_TAG, "heap_caps_malloc of %d x %d not succeeded", n, sizeof(T));
        return res;
    }
    
    void deallocate(T* p, size_t n) {
        heap_caps_free(p);
    }
};