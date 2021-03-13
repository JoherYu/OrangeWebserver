/**
 * @file event_exception.h
 * @author joher 
 * @brief event_data相关exception处理
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef EVENT_EXCEPTION_H
#define EVENT_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <cstring>
#include "http.h"

using namespace std;
/**
 * @brief 自定义exception
 * 
 */
class event_exception : public exception
{
public:
    /**
     * @brief 用于传递response信息的构造函数
     * 
     * @param error_code 错误码：进程退出时生成的状态码（可根据需要自行指定），用于http_response::set_error_info
     * @param message 错误信息
     * @param type 响应类型判断依据，用于判断生成静态html或动态json
     */
	event_exception(int error_code, string message, response_type type);
	/**
	 * @brief 仅传递错误信息的构造函数
	 * 
	 * @param message 错误信息
	 */
	explicit event_exception(string message);
	/**
	 * @brief 返回错误信息
	 * 
	 * @return const char* 错误信息
	 */
	virtual const char *what() const throw()
	{
		return message.data();
	};
	/**
	 * @brief 返回错误码
	 * 
	 * @return const int 错误码
	 */
	const int get_error_code() const
	{
		return error_code;
	}
	/**
	 * @brief 返回响应类型判断依据
	 * 
	 * @return const response_type 响应类型
	 */
	const response_type get_type() const
	{
		return type;
	}
	/**
	 * @brief 挂载错误response节点
	 * 
	 * @param fd 生成发送response对象的socket文件描述符
	 * @param error_code 错误码：进程退出时生成的状态码（可根据需要自行指定），用于http_response::set_error_info
	 * @param error_data 响应内容
	 * @param type 响应类型判断依据
	 */
	static void error_mounted(int fd, int error_code, const string &error_data, response_type type);

private:
    /**
     * @brief 错误码：进程退出时生成的状态码（可根据需要自行指定），用于http_response::set_error_info
     * @todo error_code语义
     */
	int error_code;
	/**
	 * @brief 错误信息
	 * 
	 */
	string message;
	/**
	 * @brief 响应文件类型的判断依据
	 * 
	 */
	response_type type;
};

/**
 * @brief 抛出event_exception的模板
 * 
 * @tparam T 若message为int，则根据errno生成message；若message为string，则直接应用message
 * @param error_code 错误码：进程退出时生成的状态码（可根据需要自行指定）
 * @param message 错误信息
 * @param type 响应文件类型的判断依据
 */
template <typename T>
void throw_exception(int error_code, T message, response_type type)
{

	string error_message = (std::is_same<T, int>::value) ? strerror(errno) : message;
	throw event_exception(error_code, error_message, type);
}
/**
 * @brief 抛出仅携带错误信息的event_exception
 * 
 * @param ret_value errno（根据errno生成错误信息）
 */
void throw_exception(int ret_value)
{
	throw event_exception(strerror(errno));
}
#endif
