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
//void create_pipes(initializer_list<int*> arrs);
void create_dup_pipe(int *const p_fd, int *const c_fd, const char *const method);
void p_deal_pipe(int *p_fd, int *c_fd, char *method, char *content);
int get_line(int fd, char *buf, int size);
string get_file_type(string filename);
void get_conf(const char *filename, map<string, string> &conf);
char *get_time();
shared_ptr<array<string, 2>> split_in_2(char *s, const char *delim);
//shared_ptr<vector<string>> split_path(char *s, const char *delim);

shared_ptr<vector<string>> split_string(char *s, const char *delim);
void load_default_conf(map<string, string> &configuration);
void check_work_dir(map<string, string> &conf, string dir, string default_val);

#endif
