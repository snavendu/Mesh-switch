#ifndef JSON_H
#define JSON_H
#include <iostream>
#include <string>
#include <vector>
#include "cJSON.h"
#include "cJSON_Utils.h"

using namespace std;

class JSON {
public:
    JSON();
    JSON(const JSON &);
    JSON(string);
    JSON(cJSON *);
    ~JSON();
    JSON & remove(string);
    JSON get(string);
    JSON & set(const string &, JSON &);
    JSON & set(const string &, const string &);
    JSON & push(JSON &);
    JSON & push(const string &);
    vector<JSON> toArray();
    string pretty();
    string toString();
    string getString(string key);
    int getNumber(string key);
    bool isNumber();
    void each(void(*cb)(JSON));
    void each(void(*cb)(string));
    bool isString();
    bool isObject();
    bool isArray();
    bool isBool();
    int toNumber();
    double toDouble();
    string json();
    static JSON parse(const string & str);
    static JSON Array();
    static JSON Object();
private:
    cJSON * _data = nullptr;
};

#endif
