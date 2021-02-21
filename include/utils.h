#ifndef UTILS_H
#define UTILS_H

#include "http.h"

#include <string>
#include <map>

using namespace std;

void open_file(const char *filename, http &response);
int get_line(int fd, char *buf, int size);
string get_file_type(string &filename);
void get_conf(const char *filename, map<string, string> &conf);
char *get_time();

void load_default_conf(map<string, string> &configuration);

#endif
