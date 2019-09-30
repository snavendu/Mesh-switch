#include "json.h"

JSON::JSON() {
    cJSON data;
    _data = &data;
}

JSON::JSON(const JSON & prv) {
    _data = cJSON_Duplicate(prv._data, true);
}

JSON::JSON(cJSON * d) {
    _data = d;
}

JSON::JSON(string d) {
    _data = cJSON_CreateString(d.c_str());
}

JSON::~JSON() {
    cJSON_Delete(_data);
}

vector<JSON> JSON::toArray() {
    vector<JSON> items;
    cJSON * item;
    cJSON_ArrayForEach(item, _data) {
        items.push_back(JSON(item));
    }
    return items;
}

JSON JSON::get(string key) {
    return JSON(
        cJSON_GetObjectItemCaseSensitive(_data, key.c_str())
    );
}

JSON & JSON::set(const string & key, JSON & data) {
	cJSON_AddItemToObject(_data, key.c_str(), cJSON_Duplicate(data._data, true));
	return *this;
}

JSON & JSON::set(const string & key, const string & data) {
	cJSON_AddStringToObject(_data, key.c_str(), data.c_str());
	return *this;
}

JSON & JSON::push(JSON & data) {
	cJSON_AddItemToArray(_data, cJSON_Duplicate(data._data, true));
	return *this;
}

JSON & JSON::push(const string & data) {
	cJSON_AddItemToArray(_data, cJSON_CreateString(data.c_str()));
    return *this;
}

int JSON::toNumber() {
    return _data->valueint;
}

double JSON::toDouble() {
    return _data->valuedouble;
}

bool JSON::isNumber() {
    return (_data->type & 0xFF) == cJSON_Number;
}

bool JSON::isString() {
    return (_data->type & 0xFF) == cJSON_String;
}

bool JSON::isObject() {
    return (_data->type & 0xFF) == cJSON_Object;
}

bool JSON::isBool() {
    if (_data == NULL) {
        return false;
    }
    return (_data->type & (cJSON_True | cJSON_False)) != 0;
}

string JSON::toString() {
    return (string)_data->valuestring;
}

string JSON::pretty() {
    char * data = cJSON_Print(_data);
    string ret = data;
    delete(data);
    return ret;
}

JSON JSON::Array() {
    return JSON(cJSON_CreateArray());
}


JSON JSON::Object() {
    return JSON(cJSON_CreateObject());
}

void JSON::each(void(*callback)(string)) {
    cJSON * item;
    cJSON_ArrayForEach(item, _data) {
        callback(item->valuestring);
    }
}

void JSON::each(void(*callback)(JSON)) {
    cJSON * item;
    cJSON_ArrayForEach(item, _data) {
        callback(JSON(item->valuestring));
    }
}

string JSON::json() {
    return cJSON_PrintUnformatted(_data);
}

JSON JSON::parse(const string & str) {
    return JSON(
        cJSON_Parse(str.c_str())
    );
}

string JSON::getString(string key) {
    return cJSON_GetObjectItemCaseSensitive(_data, key.c_str())->valuestring;
}