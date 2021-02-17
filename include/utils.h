#ifndef UTILS_H
#define UTILS_H

#include "http.h"
#include <string>
#include <map>

using namespace std;

void open_file(const char *filename, http &response);
int get_line(int fd, char *buf, int size);
string get_file_type(string filename);
map<string,string> get_conf(const char* filename);

#endif
