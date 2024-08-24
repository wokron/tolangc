# 附录 - 开发环境下载及安装

本次实验建议的 C++ 集成开发环境是 Clion2019.3.6，需要配置 clang 10.0.0 编译器。Java 集成开发环境为 IDEA 社区版 2018.3.6，配套的 JDK 版本为 JDK 17。产生的 MIPS 汇编在 Mars 模拟器上运行。

集成开发环境主要是用于调试，尤其是在上机考试时进行现场调试。为了避免大家上机时对机房的集成开发环境不熟悉，建议大家平时也使用推荐的集成开发环境进行开发。

### （1）C++ 开发环境安装

我们的评测机使用的是 clang10.0.0，由于在 windows 上的 clang 没有标准库，所以在 windows 下使用 clion 需要同时下载 mingw64。

机房所用电脑为 windows 系统，需要下载 mingw64 的 8.1.0 版本(可以直接使用课程组提供的[北航网盘链接](https://bhpan.buaa.edu.cn/link/AAD0D50DE017FD4AE7A9E78BA15D2024F4
文件名：mingw64.7z
有效期限：永久有效)，或在[sourceforge](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/)上自行下载安装)。下载完成之后解压，并把其下的 bin 目录加入系统变量 path 中，即可通过命令行查看 gcc 版本。

然后需要下载 clang10.0.0 编译器。我们也提供了云盘链接，大家可以通过[云盘链接](https://bhpan.buaa.edu.cn/link/AA572FC4800FCE4388908B53B73556A93F)下载或者到官网进行下载。

### （2）使用 Clion 进行开发

Clion 可以在[jetbrains官网](https://www.jetbrains.com/clion/download/other.html)下载，选择 Versio 2019.3.6，根据自己的操作系统选择对应的安装包。由于没有社区版可以免费使用，因此需要通过教育邮箱免费申请 Clion 的使用权，教育邮箱使用北航邮箱申请即可，具体操作可以参见[博客](https://blog.csdn.net/engerla/article/details/104972757)。

Clion 下载完成后点击可以参见[这篇博客](https://www.cnblogs.com/INnoVationv2/p/14718371.html)进行环境配置。

需要说明的是由于 clang 默认从 VS 的库中寻找库，此方法需要电脑预先下载了 Visual Studio，如果不想下载 VS 可以直接使用 mingw64 作为编译器。

### （3）Java 开发环境安装

Java 开发环境使用的 JDK 版本为 JDK17，可以到[官网](https://www.oracle.com/java/technologies/downloads/#java17)进行下载下载完成之后配置 Java 的环境变量，通过命令行检查 Java 版本。

### （4）使用 IDEA 进行开发

IDEA 同样也可以到[官网](https://www.jetbrains.com/idea/download/other.html)进行下载，选择 Version 2022.2.2，根据自己的操作系统选择对应的安装包(注册了教育邮箱之后可以使用最终版，但机房中安装的是社区版，建议大家尽量和机房环境保持一致)。在 IDEA 中配置项目的 JDK 可以参考[博客](https://blog.csdn.net/qq_35387940/article/details/104769659)。
