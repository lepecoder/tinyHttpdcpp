# TinyHttpdcpp

[Tiny HTTPd](https://sourceforge.net/projects/tinyhttpd/) implemented by C++

虽然说是用C++实现，但很多方法也没有C++风格的库函数，所以，可能就面向对象的思想吧





## 版本

用不同的分支存储不同的版本，

#### v_0.1

最初版本，简单的B/S通信模型，

服务端支持连接多个客户端，但是由于没有设置多线程，只能依次响应请求，每次和客户端通信一次就会关闭。

