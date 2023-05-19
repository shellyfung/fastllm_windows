## ChatGLM + fastllm在Windows下编译

ChatGLM：清华开源的大模型

fastllm：c++版本的模型推理框架

贡献者：http://whchen.net/index.php/About.html



### 编译

linux

```shell
mkdir build
cd build
cmake ..
make
```

Windows

假设你安装了Visual Studio 2017

```shell
mkdir build
cd build
cmake ..
open .
```

双击 ChatGLM.sln 然后进行编译

### 运行

模型下载：

https://pan.baidu.com/s/15ZjhxmA9viSrVYhN935lBg

提取码：ah5w

将编译好的exe与模型文件放置同一目录下


