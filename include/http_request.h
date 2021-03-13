/**
 * @file http_request.h
 * @author joher 
 * @brief HTTP请求类
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include "http.h"

using namespace std;
/**
 * @brief HTTP response类(暂未使用)
 * 
 */
class http_request : public http
{

public:
    /**
     * @brief 构造函数
     * 
     * @param method 请求方法
     * @param url 请求路径
     * @param protocol 协议版本
     */
    http_request(string method, string url, string protocol);

private:
    /**
     * @brief 请求方法
     * 
     */
    string method;
    /**
     * @brief 请求路径
     * 
     */
    string url;
};

#endif
