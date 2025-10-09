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

#define MB_LOGE(tag, fmt, ...) ESP_LOGE(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGW(tag, fmt, ...) ESP_LOGW(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGI(tag, fmt, ...) ESP_LOGI(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGD(tag, fmt, ...) ESP_LOGD(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MB_LOGV(tag, fmt, ...) ESP_LOGV(tag, "%s (%d) [%s:%d] %s: " fmt, pcTaskGetName(xTaskGetCurrentTaskHandle()), uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()), pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define MB_TAG "🌙"
#define ML_TAG "💫"

struct Coord3D {

    int x;
    int y;
    int z;

    // Coord3D() : x(0), y(0), z(0) {} // Default constructor

    Coord3D() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    
    Coord3D(int x, int y = 0, int z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    //comparisons
    bool operator!=(Coord3D rhs) {
        // ppf("Coord3D compare%d %d %d %d %d %d\n", x, y, z, rhs.x, rhs.y, rhs.z);
        return x != rhs.x || y != rhs.y || z != rhs.z;
        // return !(*this==rhs);
    }
    bool operator==(const Coord3D rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator<(const int rhs) const {
        return x < rhs && y < rhs && z < rhs;
    }


    //Minus / delta (abs)
    Coord3D operator-(const Coord3D rhs) const {
        return Coord3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Coord3D operator-(const int rhs) const {
        return Coord3D(x - rhs, y - rhs, z - rhs);
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
    Coord3D operator/(const int rhs) const {
        return Coord3D(x / rhs, y / rhs, z / rhs);
    }
    Coord3D operator%(const Coord3D rhs) const {
        return Coord3D(x % rhs.x, y % rhs.y, z % rhs.z);
    }

    //assignments
    Coord3D operator=(const Coord3D rhs) {
        // ppf("Coord3D assign %d,%d,%d\n", rhs.x, rhs.y, rhs.z);
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
    Coord3D operator+=(const Coord3D rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    Coord3D operator/=(const Coord3D rhs) {
        if (rhs.x) x /= rhs.x;
        if (rhs.y) y /= rhs.y;
        if (rhs.z) z /= rhs.z;
        return *this;
    }
    
    Coord3D maximum(const Coord3D rhs) const {
        return Coord3D(max(x, rhs.x), max(y, rhs.y), max(z, rhs.z));
    }

    unsigned distanceSquared(const Coord3D rhs) const {
        Coord3D delta = (*this-rhs);
        return (delta.x)*(delta.x) + (delta.y)*(delta.y) + (delta.z)*(delta.z);
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

    void split(const char * splitter, std::function<void(const char *, uint8_t)> callback) {
        char savedS[N];
        strncpy(savedS, s, N); //save s
        char *token = strtok(s, splitter); //eat s
        uint8_t sequence = 0;
        while (token != nullptr ) {
            callback(token, sequence);
            sequence++;
            token = strtok(nullptr, splitter);
        }
        strncpy(s, savedS, N); //restore s
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

//for game of live
uint16_t crc16(const unsigned char* data_p, size_t length);
uint16_t gcd(uint16_t a, uint16_t b);
uint16_t lcm(uint16_t a, uint16_t b);
bool getBitValue(const uint8_t* byteArray, size_t n);
void setBitValue(uint8_t* byteArray, size_t n, bool value);

bool isInPSRAM(void* ptr);

extern int totalAllocatedMB;

//allocate, try PSRAM, else default, use calloc: zero-initialized (all bytes = 0)
template<typename T>
T* allocMB(size_t n, const char *name = nullptr) {
    T* res = (T*)heap_caps_calloc_prefer(n, sizeof(T), 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_INTERNAL); //calloc is malloc + memset(0);
    if (res) {
        totalAllocatedMB += heap_caps_get_allocated_size(res);
        // MB_LOGD(MB_TAG, "Allocated %s: %d x %d bytes in %s s:%d (tot:%d)", name?name:"", n, sizeof(T), isInPSRAM(res)?"PSRAM":"RAM", heap_caps_get_allocated_size(res), totalAllocatedMB);
    }
    else 
        MB_LOGE(MB_TAG, "heap_caps_malloc of %d x %d not succeeded", n, sizeof(T));
    return res;
}

template<typename T>
T* reallocMB(T* p, size_t n, const char *name = nullptr) {
    T* res = (T*)heap_caps_realloc_prefer(p, n, sizeof(T), 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_INTERNAL); //calloc is malloc + memset(0);
    if (res) {
        // MB_LOGD(MB_TAG, "Re-Allocated %s: %d x %d bytes in %s s:%d", name?name:"", n, sizeof(T), isInPSRAM(res)?"PSRAM":"RAM", heap_caps_get_allocated_size(res));
    }
    else 
        MB_LOGE(MB_TAG, "heap_caps_malloc of %d x %d not succeeded", n, sizeof(T));
    return res;
}

//free memory
template<typename T>
void freeMB(T* p, const char *name = nullptr) {
    if (p) {
        totalAllocatedMB -= heap_caps_get_allocated_size(p);
        // MB_LOGD(MB_TAG, "free %s: x x %d bytes in %s, s:%d (tot:%d)", name?name:"", sizeof(T), isInPSRAM(p)?"PSRAM":"RAM", heap_caps_get_allocated_size(p), totalAllocatedMB);
        heap_caps_free(p);
        p = nullptr;
    }
    else
        MB_LOGW(MB_TAG, "Nothing to free: pointer is null");
}

//allocate vector
template<typename T>
struct VectorRAMAllocator {
    using value_type = T;
    
    T* allocate(size_t n) {
        return allocMB<T>(n, "vector");
    }
    
    void deallocate(T* p, size_t n) {
        freeMB(p, "vector");
    }
    T* reallocate(T* p, size_t n)  {
        return reallocMB<T>(p, n, "vector");
    }
};

// https://arduinojson.org/v7/api/jsondocument/
struct JsonRAMAllocator: ArduinoJson::Allocator {
    //(uint8_t*): simulate 1 byte
  void* allocate(size_t n) override {
    return allocMB<uint8_t>(n, "json");
  }
  void deallocate(void* p) override {
    freeMB<uint8_t>((uint8_t*)p, "json");
  }
  void* reallocate(void* p, size_t n) override {
    return reallocMB<uint8_t>((uint8_t*)p, n, "json");
  }
  static Allocator* instance() {
    static JsonRAMAllocator allocator;
    return &allocator;
  }
};

//allocate object
template<typename T, typename... Args>
T* allocMBObject(Args&&... args) {
    void* mem = allocMB<T>(1, "object");
    if (mem) {
        return new(mem) T(std::forward<Args>(args)...);
    } else {
        return nullptr;
    }
}

//free object
template<typename T>
void freeMBObject(T* obj) {
    freeMB(obj, "object");
}

extern std::vector<std::function<void()>, VectorRAMAllocator<std::function<void()>>> runInTask1, runInTask2; // functions to be called in main loopTask (to avoid https to run out of stack space)
extern std::mutex runInTask_mutex; //protect the runInTask vectors

//to use in effect and on display
#if USE_M5UNIFIED
    extern unsigned char moonmanpng[];
    extern unsigned int moonmanpng_len;
#endif