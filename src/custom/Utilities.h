/**
    @title     MoonBase
    @file      Utilities.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilties/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#include <Arduino.h>
#include "ArduinoJson.h"

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
    Char& operator+=(const char *rhs) {
      strlcat(s, rhs, sizeof(s));
      return *this;
    }

    //compare
    bool operator==(const char *rhs) {
      return strcmp(s, rhs) == 0;
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

