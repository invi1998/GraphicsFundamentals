*************************************************************************************


# 【windows编程】

## 1.主要的应用程序分类

​	1.控制台程序
​	2.窗口程序
​	3.库程序
​		动态库程序：DLL
​		静态库程序：LIB

## 2.入口函数

​	控制台程序：main
​	窗口程序：	WinMain
​	动态库程序：DllMain
​	静态库程序没有入口函数

## 3.静态库程序没有入口函数，既无法执行，但并不影响编译和链接

## 4.文件存在方式

​	控制台程序、窗口程序 --- EXE文件
​	动态库程序 --- DLL文件
​	静态库程序 --- LIB文件
​	

*************************************************************************************


# 1.编译工具

## 	1.编译器CL.EXE 	将源代码编译成目标代码.obj

## 	2.链接器LINK.EXE 	将目标代码、库链接生成最终文件

​		CL.EXE + LINK.EXE = linux中的gcc	

## 	3.资源编译器RC.EXE (.rc)将资源编译，最终通过链接器存入最终文件

# 2.库和头文件

## 	windows库：

​		kernel32.dll --- 提供了核心的API,例如进程、线程、内存管理等
​		user32.dll   --- 提供了窗口、消息等API
​		gdi32.dll    --- 绘图相关的API
​		路径：C:\windows\system32

## 	头文件：

​		windows.h   所有windows头文件的集合（一般程序开发中，只填写这一个即可）
​		windef.h	windows特有的数据类型
​		winbase.h	kernel32的API
​		winuser.h	user32的API
​		wingdi.h	gdi32的API
​		winnt.h		UNICODE字符集支持 

## 	相关函数：

​	WinMain： 	窗口的入口函数
​	int WINAPI WinMain(
​		HINSTANCE hInstance, 	//当前程序的实例句柄
​		HINSTANCE hPrevInstance,//当前程序前一个实例句柄
​		LPSTR IpCmdLine,		//命令行参数字符串
​		int nCmdShow			//窗口的显示方式
​	);
​	MessageBox：阻塞函数（一直悬停）
​	int MessageBox(
​		HWND hWnd,			//父窗口句柄
​		LPCTSTR IpText,		//显示在消息框中的文字
​		LPCTSTR	IpCaption,	//显示在标题栏中的文字
​		UINT uType			//消息框中的按钮、图标显示类型
​	);//返回点击的按钮ID
​		什么时候阻塞：程序执行到这里即阻塞				
​		什么时候解除阻塞：点击相应的按钮则关闭
​		返回值：接收messagebox的返回值，通过对返回值的判断，作进一步的处理

# 3.编译链接的过程

## 窗口程序：

​	编译环境准备 --- vcvars32.bat
​	编译程序 	 --- CL.EXE  xxx.c -c 
​	链接程序 	 --- LINK.EXE  xxx.obj xxx.lib   (此处执行结束之后，会生成可执行文件xxx.exe)
​	执行 

## 资源程序：

​	编写资源的文件     - .rc资源脚本文件
​	编译rc文件         - RC.EXE
​	将资源连接到程序中 - LINK.EXE

# 4.修改xxx.exe文件的图标

​	必须使用ico格式的图片
​	RC.EXE   small.rc
​	LINK.EXE hello.obj  small.res  user32.lib
​	

*************************************************************************************


# 1.窗口的创建过程

​	定义WinMain函数
​	定义窗口处理函数（自定义，处理消息）
​	注册窗口类（向操作系统写入一些数据）
​	创建窗口（内存中创建窗口）
​	显示窗口（绘制窗口的图像）
​	消息循环（获取/翻译/派发消息）
​	消息处理
​		抓消息：GetMessage
​		翻译消息：TranslateMessage
​		派发消息：DispatchMessage

# 2.创建一个项目的初始工作

​	1.设为启动项
​	2.更改项目的属性   修改为使用多字节的字符集
​	3.添加一个新的文件
3.窗口的打开与关闭与进程的生死无关

*************************************************************************************


## 1.字符编码

​	ASC
​	ASCII
​	DBCS:	 单双字节混合编码（容易造成数据混乱）
​	UNICODE：不存在解析的问题
​		windows:一般是指 UTF-16  所以的字符都是按照两个字节来编码，不管是字符（补0）还是汉字
​		linux：	一般是指 UTF-8

## 2.一种新的数据类型:宽字节字符

​	wchar_t 每个字符占2个字节
​	char	每个字符占1个字节
​	1.wchar_t实际上是unsigned short类型，定义时，需要增加“L”以说明
​	2.需要使用支持wchar_t的函数操作宽字节字符串，例如：
​		wchar_t * pwszText = L"hello wchar";
​		wprintf(L"%s\n",pwszText);
​	3.wcslen求的不是有多少个字节，而是有多少个字符

## 3.TCHAR数据类型

​	#ifdef UNICODE	
​	typedef wchar_t TCHAR;
​		#define __TEXT(quote) L##quote
​	#else
​		typedef char TCHAR;
​		#define __TEXT(quote) quote
​	#endif
​	注意：时刻注意L的存在						

## 4.wprintf对UNICODE字符打印支持不完善

  在windows下使用 WriteConsole API 打印 UNICODE 字符 GetStdHandle
	GetStdHandle用于获取标准句柄（标准输出句柄，标准输入句柄，标准错误句柄）
	标准输出句柄:STD_OUTPUT_HANDLE
	标准输入句柄:STD_INPUT_HANDLE
	标准错误句柄:STD_ERROR_HANDLE

## 5.如果采用的是unicode编码，系统默认添加#define UNICODE

  如果采用的是多字节编码，则不会添加

## 6.系统调用函数的参数类型

​	LPSTR = char* 		LPCTSTR = const char*
​	LPWSTR = wchar_t* 	LPCWSTR = const wchar_t*
​	LPTSTR = TCHAR*		LPCTSTR = const TCHAR*




​		