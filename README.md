---
title: CPP11及以后的常用特性
date: 2020-02-19 17:41:27
tags:
categories:
 - CPP
---

# C++11

## 特性

### Lambda

```c++
[ 捕获 ] ( 形参 ) -> ret { 函数体 }
[ 捕获 ] ( 形参 ) { 函数体 }
[ 捕获 ] { 函数体 }

& 以引用隐式捕获被使用的自动变量
= 以复制隐式捕获被使用的自动变量

[&]{};          // OK：默认以引用捕获
[&, i]{};       // OK：以引用捕获，但 i 以值捕获

[=]{};          // OK：默认以复制捕获
[=, &i]{};      // OK：以复制捕获，但 i 以引用捕获

[this]{}; // 获取this指针, 如果使用了&和=则会默认包括this
```
**使用场景一**
以sort为代表的函数 需要传入函数的函数
```c++
// 原版
bool compare(int &a, int &b)
{
	return a > b;
}

xxx(compare);

// 新版
xxx([](int a, int b){return a > b;});
```
**使用场景二**
```c++
auto add = [](int a, int b){return a + b;};
int bar = add(1, 2);
```

### User-defined literals

```c++
using namespace std::chrono_literals;

std::this_thread::sleep_for(2000ms);

chrono::milliseconds operator"" ms(unsigned long long _Val) noexcept
```

第一次看到上面代码 还疑惑了会 点了下ms发现是重载. 才知道这是c++11的特性...

## 库

### 智能指针 - SharedPtr.cpp

自己简单实现了一下智能指针

Counter作为计数类 当调用`AddRefCopy`时计数值增加`Release`时计数值减少

`Release`计数值减少时判断`shared_count_`如果为0则调用`Dispose`删除掉保存的指针之后接着调用`Destroy`删除掉自己

SharedPtr从`from`拷贝构造时, 内部Counter指针从`from`调用`AddRefCopy`获得 实现拷贝构造计数值增加, 两者共用一个`Counter`

SharedPtr从`from`赋值时, 首先对自己保有的Counter指针调用`Release`减少计数值 然后调用`from`的`AddRefCopy`获得新的`Counter`

SharedPtr析构时调用Counter的`Release`减少计数值


### async

```c++
#include <iostream>
#include <future>
#include <unistd.h>
int Foo()
{
	std::cout << "Foo begin" << std::endl;
	sleep(2);
	std::cout << "Foo end" << std::endl;
	return 1;
}
int main()
{
	auto get_id = std::async(Foo);

	std::cout << "Other work begin" << std::endl;
	sleep(1); // do other works
	std::cout << "Other work end" << std::endl;
	
	get_id.wait();
	std::cout << get_id.get() << std::endl;
	return 0;
}
/*
Other work begin
Foo begin
Other work end
Foo end
1
*/
```

### mutex condition

### std::function 和 std::bind

```c++
##include <functional>
##include <iostream>

void PrintA()
{
    std::cout << "A" << std::endl;
}
void PrintB(int bar)
{
    std::cout << "B" << std::endl;
}
int main()
{
    std::function<void()> FPrintA = PrintA;
    FPrintA();

    std::function<void(int)> FPrintB = PrintB;
    FPrintB(1);

// 感觉配合Lambad 挺不错, 在一个函数中经常使用的功能可以这样定义
    std::function<void()> FLambad = [](){std::cout << "Lambad" << std::endl;};
    FLambad();
	
	// 不过使用auto貌似更简单
	auto ALambad = [](){std::cout << "Lambad" << std::endl;};
    ALambad();
}

$ A
$ B
$ Lambad
```

```c++
##include <functional>
##include <cstdio>

void f(int n1, int n2, int n3, int n4)
{
    printf("f function-->n1: %d, n2: %d, n3: %d, n4: %d\n", n1, n2, n3, n4);
}

void ff(int &n1, int& n2, const int& n3)
{
    printf("before ff function-->n1: %d, n2: %d, n3: %d\n", n1, n2, n3);
    n1 = 11;
    n2 = 22;
    // n3 = 33; 编译错误
    printf("after ff function-->n1: %d, n2: %d, n3: %d\n", n1, n2, n3);
}

int main()
{
    auto f1 = std::bind(f, std::placeholders::_2, std::placeholders::_3, 666, std::placeholders::_1);

    f1(1, 2, 3, 4, 5);
    // infunction-->n1: 2, n2: 3, n3: 666, n4: 1
    // 1 绑定_1  2绑定_2  3绑定_3   // 4 5被忽略
    // 按照 f1的顺序传入参数
    // 所以调用为 f(2, 3, 666, 1);

    int n1 = 1, n2 = 2, n3 = 3;
    auto ff1 = std::bind(ff, n1, std::ref(n2), std::cref(n3));
    n1 = -1;
    n2 = -2;
    n3 = -3;
    printf("before ff1 function-->n1: %d, n2: %d, n3: %d\n", n1, n2, n3);
    ff1(n1, n2, n3);
    printf("after ff1 function-->n1: %d, n2: %d, n3: %d\n", n1, n2, n3);
    // before ff1 function-->n1: -1, n2: -2, n3: -3
    // before ff function-- > n1: 1, n2 : -2, n3 : -3 // 这里说明了值传递 参数是绑定时就决定好了 引用参数还是可以改变的
    // after ff function-- > n1: 11, n2 : 22, n3 : -3
    // after ff1 function-- > n1: -1, n2 : 22, n3 : -3 // 引用传入成功改变, 值传入和const引用传入未变

    // std::ref 按引用传入参数 std::cref按const引用传入参数
}
```

# C++14

## 特性
### 0b010101 二进制表达

```c++
int a = 0b111111111;
```

## 库

# C++17

## 特性

### 结构化绑定

```c++
#include <map>
#include <iostream>
int main()
{
	std::map<int, int> id_map{{1, 2}, {3, 4}};

	for (const auto& id : id_map)
	{
		std::cout << id.first << id.second << std::endl;
	}
	for (const auto&[id1, id2] : id_map)
	{
		std::cout << id1 << id2 << std::endl;;
	}
	return 0;
}

/*
12
34
12
34
*/
```

### 初始化语句不再局限于for语句 if和switch也能使用 while: ???

```c++
int Init()
{
	return 1;
}
int main()
{
	for (int a = Init(); a != 2; a++)
	{
		std::cout << 1 << std::endl;
	}
	if (int a = Init(); a == 1)
	{
		std::cout << 1 << std::endl;
	}
	switch (int a = Init(); a)
	{
	case 1:
		std::cout << 1 << std::endl;
	}
	return 1;
}
```

至于while为什么没有 https://stackoverflow.com/questions/59985550/while-statement-with-initializer

说是C++已经足够复杂了, 每次增加复杂度的时候都要足够的小心, 让这些复杂度增加的合理而自然.

如果对while增加初始化语句, 那么for? ~~干脆删掉while吧~~

## 库

### optional

**optional**

```c++
std::optional<std::string> Foo(bool b)
{
	if (b)
	{
		return "123456";
	}
	else
	{
		return {};
	}
}
int main()
{
	if (!Foo(false).has_value())
	{
		std::cout << "no value" << std::endl; // no value
	}
	std::cout << Foo(false).value_or("(null)"); // (null)
	std::cout << Foo(true).value_or("no value"); // 123456
}
```

### string_view

提供了对`已经被其他持有所有权的字符串`的指针和长度的包装 仅仅提供包装
```c++
int main()
{
	const char* str = "123456789";
	std::string_view message(str, strlen(str));
	std::cout << message;
}
```

# C++20

## 库

### 协程库 - CoRoutine.cpp


协程一大用途就是可以方便的将多个异步调用转换为串行化的同步调用

### format库

https://github.com/fmtlib/fmt

将项目中的include和src拷贝出放入format文件夹添加到项目根目录

format文件夹新增CMakeLists.txt

```cmake
SET(FORMAT_SRC
        src/os.cc
        src/format.cc
        )
add_library(format ${FORMAT_SRC})
```

项目根CMakeLists.txt增加如下三行
```cmake
include_directories(format/include)
add_subdirectory(format)
target_link_libraries(YOUR EXECUTABLE format)
```

即可使用fmt库 相关语法也非常简单
```c++
const char* errnum;
const char* shortmsg;
fmt::format("HTTP/1.0 {} {}\r\n", errnum, shortmsg);
```