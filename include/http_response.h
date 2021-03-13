/**
 * @file http_response.h
 * @author joher 
 * @brief HTTP响应类
 * @version 0.1
 * @date 2021-03-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <array>
#include "http.h"

#include <functional>

using namespace std;
/**
 * @brief HTTP响应类
 * 
 */
class http_response : public http
{

public:
    /**
     * @brief 构造函数
     * 
     * @param protocol 协议版本
     */
    explicit http_response(string protocol);
    /**
     * @brief 构造函数
     * 
     * @param status_code 状态码
     * @param status_descp 描述信息
     */
    http_response(int status_code, string status_descp);
    /**
     * @brief 构造函数
     * 
     * @param status_code 状态码
     * @param status_descp 描述信息
     * @param protocol 协议版本
     */
    http_response(int status_code, string status_descp, string protocol);
    /**
     * @brief 设置响应头Mime-Type
     * 
     * @param type_name Mime-Type名
     */
    void set_content_type(string type_name);
    /**
     * @brief 设置数据实体大小响应头
     * 
     * @param size 数据实体大小
     */
    void set_content_length(long size);
    /**
     * @brief 内联函数：设置状态码
     * 
     * @param status_code 状态码
     */
    void set_status_code(int status_code);
    /**
     * @brief 内联函数：设置状态信息
     * 
     * @param status_descp 状态信息
     */
    void set_status_descp(const string &status_descp);
    /**
     * @brief 内联函数：获取状态码
     * 
     * @return string 状态码
     */
    string get_status_code() override;
    /**
     * @brief 内联函数：获取状态信息
     * 
     * @param status_descp 状态信息
     */
    string get_status_descp() override;
    /**
     * @brief 获取默认（固定）响应头内容
     * 
     * @return string 响应头内容
     */
    string get_fix_headers() override;
    /**
     * @brief 设置请求处理发生错误时的数据实体内容
     * 
     * @param info 实体内容
     * @param type 响应类型判断依据，用于判断生成静态html或动态json
     */
    void set_error_content(const string &info, response_type type);
    /**
     * @brief 处理静态资源
     * 
     * @param filename 静态资源文件路径
     */
    void deal_static_resource(string filename);
    /**
     * @brief 处理动态资源
     * 
     * @param r_fd 管道文件描述符
     */
    void deal_dynamic_resource(int r_fd);
    /**
     * @brief 生成HTTP响应对象
     * 
     * @param filename 资源路径
     * @param protocol 协议版本
     * @param deal_func 请求处理函数（deal_dynamic_event或deal_static_event）
     */
    void make_response(string filename, string protocol, function<void()> deal_func);
    /**
     * @brief 处理动态资源请求
     * @todo 核对fd参数是否被使用
     * @todo 精简形参
     * 
     * @param des_path 动态资源路径
     * @param protocol 协议版本
     * @param fd 
     * @param str_p 元素为请求路径被划分为（组件名+其后路径）的数组,其中，其后路径为传递给组件的参数
     * @param content 请求方法为POST时，需要传递给组件处理的数据
     * @param method 请求方法
     */
    void deal_dynamic_event(string des_path, char *protocol, int fd, array<string, 2> &str_p, char *content, char *method);
    /**
     * @brief 处理静态资源请求
     * 
     * @param filepath 静态资源路径
     * @param protocol 协议版本
     */
    void deal_static_event(string filepath, char *protocol);
    /**
     * @brief 设置请求处理发生错误时的响应信息
     * @todo 更改error_code语义为枚举类型
     * 
     * @param error_code 错误码：进程退出时生成的状态码（可根据需要自行指定）
     * @param status_code 传出参数：状态码
     * @param error_descp 传出参数：响应信息
     */
    static void set_error_info(const int error_code, int &status_code, string &error_descp);

private:
    /**
     * @brief 响应状态码
     * 
     */
    int status_code;
    /**
     * @brief 响应信息
     * 
     */
    string status_descp;
    /**
     * @brief 内置默认（自定义）固定请求头
     * 
     */
    const string fix_headers =
        "Server: myserver\r\n";
};

inline void http_response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

inline void http_response::set_status_descp(const string &status_descp)
{
    this->status_descp = status_descp;
}
inline string http_response::get_status_code()
{
    return to_string(status_code);
}

inline string http_response::get_status_descp()
{
    return status_descp;
}
inline string http_response::get_fix_headers()
{
    return fix_headers;
}

#endif
