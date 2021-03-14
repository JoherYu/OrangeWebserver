OrangeWebserver
==========
这是一个简单的Web服务器示例，主要面向于从C转向C++编程，学习Linux系统编程的初学者。所以代码并不聚焦于性能，而是尽可能多的使用Linux系统API。代码本身并没有进行优化，也可能存在bug，仅仅是个供参考的例子
## 功能特征
+ 整体架构epoll反应堆模型+线程池为主体，采用非阻塞的方式进行事件处理
+ 服务器支持静态资源请求和动态资源请求，动态资源请求示例在components文件夹中
  + 动态资源请求使用多进程处理模型
  + 静态资源支持类型参考[utils](https://github.com/JoherYu/OrangeWebserver/blob/master/utils.cpp)内get_file_type函数
+ 服务器支持GET和POST请求，POST调用示例login，使用[RapidJSON](https://github.com/Tencent/rapidjson)进行JSON数据解析

    
更多文档信息使用[Doxygen](https://www.doxygen.nl/index.html)生成 

    doxygen Doxyfile
    
文档在生成的document文件夹中

## 运行
修改Makefile中programme_name变量可修改程序名

    make            #生成可执行文件
    make components #生成动态资源处理程序
    make allclean   #清除中间生成obj文件
    ./demo          #默认文件名

Debug运行及测试运行（目前还未更新测试）

    make debug=true
    make test=true
    
