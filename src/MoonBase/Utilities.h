/**
    @title     MoonBase
    @file      Utilities.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#include <Arduino.h>
#include "ArduinoJson.h"
// struct Coord3D16 {
//     uint16_t x;
//     uint16_t y;
//     uint16_t z;
// };

struct Coord3D {
    int x;
    int y;
    int z;
    //Minus / delta (abs)
    Coord3D operator-(const Coord3D rhs) const {
        return Coord3D{x - rhs.x, y - rhs.y, z - rhs.z};;
    }
    Coord3D operator+(const Coord3D rhs) const {
        return Coord3D{x + rhs.x, y + rhs.y, z + rhs.z};
    }
    Coord3D operator*(const Coord3D rhs) const {
        return Coord3D{x * rhs.x, y * rhs.y, z * rhs.z};
    }
    Coord3D operator/(const Coord3D rhs) const {
        return Coord3D{x / rhs.x, y / rhs.y, z / rhs.z};
    }
    Coord3D operator%(const Coord3D rhs) const {
        return Coord3D{x % rhs.x, y % rhs.y, z % rhs.z};
    }

    Coord3D operator+=(const Coord3D rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
      }
    
    Coord3D maximum(const Coord3D rhs) const {
        return Coord3D{max(x, rhs.x), max(y, rhs.y), max(z, rhs.z)};
    }
};

  
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
//     ESP_LOGD(TAG, "token: %s", token);
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

inline float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
  return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}
