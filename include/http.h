/**
 * @file http.h
 * @author joher 
 * @brief HTTP类
 * @version 0.1
 * @date 2021-03-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef HTTP_H
#define HTTP_H

#include <string>
using namespace std;
/**
 * @brief HTTP抽象类
 * 
 */
class http
{
public:
    /**
     * @brief 默认的合成构造函数
     * 
     */
    http() = default;
    /**
     * @brief 构造函数
     * 
     * @param protocol 协议版本 
     * @param headers 请求/响应头
     * @param data 请求/响应实体数据
     */
    http(string protocol, string headers, string data);
    /**
     * @brief 设置/添加实体数据
     * 
     * @param data 实体数据
     */
    void add_data(const string &data);
    /**
     * @brief 处理/清空缓冲区请求头信息
     * 
     * @param fd socket文件描述符
     * @return char* 请求携带实体数据时返回实体数据
     * @retval NULL 请求未携带实体数据
     */
    static char *deal_headers(int fd);
    /**
     * @brief 内联函数：获取协议版本
     * 
     * @return string 协议版本
     */
    string get_protocol();
    /**
     * @brief 内联函数：获取头部信息
     * 
     * @return string 头部信息
     */
    string get_headers();
    /**
     * @brief 内联函数：获取实体数据
     * 
     * @return string 实体数据
     */
    string get_data();
    /**
     * @brief 虚函数：获取响应状态码
     * 
     * @return string 响应状态码
     */
    virtual string get_status_code() = 0;
    /**
     * @brief 虚函数：获取响应信息
     * 
     * @return string 相应信息
     */
    virtual string get_status_descp() = 0;
    /**
     * @brief 虚函数：获取默认（固定）响应头内容
     * 
     * @return string 默认（固定）响应头内容
     */
    virtual string get_fix_headers() = 0;

protected:
    /**
     * @brief 协议版本（默认为HTTP/1.1）
     * 
     */
    string protocol = "HTTP/1.1";
    /**
     * @brief 头部信息
     * 
     */
    string headers = "";
    /**
     * @brief 实体数据
     * 
     */
    string data = "";
};

inline string http::get_protocol()
{
    return protocol;
}

inline string http::get_headers()
{
    return headers;
}

inline string http::get_data()
{
    return data;
}
/**
 * @brief 响应类型判断依据
 * @todo 搬移至http_response（暂定）
 * 
 */
enum response_type
{
    Static,
    Dynamic
};

#endif
