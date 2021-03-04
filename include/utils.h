#ifndef UTILS_H
#define UTILS_H

#include "http.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

void open_file(const char *filename, http &response);
void generic_open(const char *filename, string &data);
int get_line(int fd, char *buf, int size);
string get_file_type(string &filename);
void get_conf(const char *filename, map<string, string> &conf);
char *get_time();
shared_ptr<array<string, 2>> split_in_2(char *s, const char *delim);
shared_ptr<vector<string>> split_path(char* s, const char * delim); // deprecated

shared_ptr<vector<string>> split_string(char* s, const char * delim);
void load_default_conf(map<string, string> &configuration);

#endif
