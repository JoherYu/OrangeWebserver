/**
 * @file utils.h
 * @author joher 
 * @brief 工具函数
 * @version 0.1
 * @date 2021-03-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include "http.h"

#include <string>
#include <vector>
#include <map>

#include <memory>

using namespace std;
/**
 * @brief 读取文件内容并存储在response的data变量中
 * @attention 不再建议使用
 * 
 * @param filename 文件路径
 * @param response 传出参数：http_response对象的引用
 */
void open_file(const char *filename, http &response);
/**
 * @brief 读取文件内容并存储在data变量中
 * @todo 读取文件错误时抛出错误
 * 
 * @param filename 文件路径
 * @param data 传出参数
 */
void generic_open(const char *filename, string &data);
//void create_pipes(initializer_list<int*> arrs);
/**
 * @brief 使用pipe函数创建两个管道进行双工通信
 * 
 * @param p_fd parent进程读写管道
 * @param c_fd child进程读写管道。请求方法为GET时不需要此管道，读写端皆设为-1
 * @param method HTTP请求方法
 */
void create_dup_pipe(int *const p_fd, int *const c_fd, const char *const method);
/**
 * @brief 使用管道通信前parent进程进行的准备工作
 * @details 。因不使用c_fd，请求方法为GET时，关闭p_fd写端。因不使用c_fd，不对cfd进行任何处理;
 * 
 * @param p_fd parent进程读写管道。请求方法为GET时，关闭p_fd写端；请求方法为POST时，关闭读端并写入数据
 * @param c_fd child进程读写管道。请求方法为GET时，因不需要此管道，故不对cfd进行任何处理;请求方法为POST时，关闭写端
 * @param method HTTP请求方法
 * @param content 请求方法为POST时需要写入的数据
 */
void p_deal_pipe(int *p_fd, int *c_fd, char *method, char *content);
/**
 * @brief 从fd文件描述符中读取一行数据
 * 
 * @param fd 文件描述符
 * @param buf 传出参数：去读到的数据
 * @param size buf大小
 * @return int 从文件中读取到的字节数
 */
int get_line(int fd, char *buf, int size);
/**
 * @brief 根据文件后缀名取得Mime-Type
 * 
 * @param filename 文件名
 * @return string Mime-Type字符串
 * @retval 不支持的文件类型将返回空字符（""）
 */
string get_file_type(string filename);
/**
 * @brief 获取配置文件信息
 * @todo 重构
 * 
 * @param filename 配置文件路径
 * @param conf 传出参数：配置信息的键值对（键值对都存储为字符串的形式）
 */
void get_conf(const char *filename, map<string, string> &conf);
/**
 * @brief 获取当前时间
 * 
 * @return char* 当前时间的字符串
 */
char *get_time();
/**
 * @brief 根据分隔符将字符串一分为二
 * @details 分割基准为第一个出现的分割符
 * @todo 返回空字符而不是空格字符
 * 
 * @param s 要分割的字符串
 * @param delim 分隔符
 * @return shared_ptr<array<string, 2>> 分割后的两个字符串
 * @retval 若s或delim为空字符串，返回值为空格字符（" "）
 */
shared_ptr<array<string, 2>> split_in_2(char *s, const char *delim);
//shared_ptr<vector<string>> split_path(char *s, const char *delim);
//shared_ptr<vector<string>> split_string(char *s, const char *delim);

/**
 * @brief 在配置文件路径错误或配置文件不存在时生成默认配置
 * @details 不生成目录配置项
 * @todo 核对配置项与可支持配置项是否一致
 * 
 * @param configuration 传出参数：配置信息的键值对（键值对都存储为字符串的形式）
 */
void load_default_conf(map<string, string> &configuration);
/**
 * @brief 检查配置信息中各个目录配置项，若配置项不存在则生成默认配置
 * @todo 完善形参修饰符语义，重构
 * 
 * @param conf 传入传出参数：已经生成的配置信息
 * @param dir 需要检查的目录项名称（键）
 * @param default_val 传入参数：该目录项的默认路径（值）
 */
void check_work_dir(map<string, string> &conf, string dir, string default_val);

#endif
