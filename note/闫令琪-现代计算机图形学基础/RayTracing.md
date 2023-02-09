---
搬运自知乎，仅用作个人学习使用：https://zhuanlan.zhihu.com/p/128582904
---



# Ray Tracing in One Weekend V3.0中文翻译（下）

![img](.\img\v2-9a35b750d53cf1316439c9ee9c5dce46_180x120.jpg)

## 目录

[1. Overview 概述]
[2. Output a Image 输出你的图像]
[3. The vec3 Class vec3向量类]
[4. Rays, a Simple Camera, and Background 光线, 简单摄像机, 以及背景]
[5. Adding a Sphere 加入球体]
[6. Suface Normals and Multiple Objects 面法相与复数物体]
[7. Antialiasing 反走样]
[8. Diffuse Material 漫反射材质]
[9. Metal 金属材质]
[10. Dielectric 绝缘体材质]
[11. Positionable Camera 可自定义位置的摄像机]
[12. Defocus Blur 散焦模糊]
[13. Where Next? 接下来学什么?]

## 1. 概述

这些年来我(Peter Shirley)开过不少图形学的课。我常常把光线追踪作为课堂上的教学内容。因为对于光线追踪来说, 在不使用任何API的情况下, 你不得不被迫手撸全部的代码, 但你仍然能渲染出炫酷的图片。我决定将我的课堂笔记改写成本教程, 让你能尽可能快的实现一个炫酷的光线追踪器(ray tracer)。这并不是一个功能完备的光线追踪器。但是它却拥用让光线追踪在电影行业成为主流的非直接光照(indirect lighting)。跟随本教程循序渐进, 你的光线追踪器的代码构筑将会变得易于拓展。如果之后你对这方面燃起了兴趣, 你可以将它拓展成一个更加完备的光线追踪器。

当大家提起"光线追踪", 可能指的是很多不同的东西。我对这个词的描述是, 光线追踪就是一个路径追踪器, 事实上大部分情况下这个词都是这个意思。光线追踪器的代码也是十分的简单(写最简单的代码, 让电脑帮我们算去吧!)。当你看到你渲染的图片时, 你一定会感到高兴的。

接下来我会带你一步步的实现这个光线追踪, 并加入一些我的debug建议。最后你会得到一个能渲染出漂亮图片的光线追踪器。我认为你应该能在一个周末的时间内搞定。如果你花的时间比这长, 别担心, 也没太大问题。我使用C++作为本光线追踪器的实现语言。你其实不必, 但我还是推荐你用C++, 因为C++快速, 平台移植性好, 并且大部分的工业级电影和游戏渲染器都是使用C++编写的。注意这里我避免了大部分C++的新特性。但是继承和重载运算符我们保留, 对光线追踪器来说这个太有用了。网上的那些代码不是我提供的, 但是这些代码是真的可以跑的。除了vec3类中的一些简单的操作, 我将所有的代码都公开了。我是"学习编程要亲自动手敲代码"派。但是如果有一份代码摆在我面前, 我可以直接用, 我还是会用的。所以我只在没代码用的时候, 我才奉行我刚刚说的话。好啦, 别提这个了!

我没把上面一段删了, 因为我的态度180°大转变太好玩了。读者们帮我修复了一些次要的编译错误, 这里还是请你亲手来敲一下代码吧!但是你如果你想看看我的代码:[点击这里](https://link.zhihu.com/?target=https%3A//github.com/RayTracing/raytracing.github.io/)

本书假定你有一定的向量运算的知识(比如说点乘和叉乘)。如果你记不太清楚, 回顾一下就行。如果你需要回顾, 或者你是第一次听说这个东西, 你可以看我或者Marschner的图像教材, Foley, Van Dam等也行。或者McGuire的codex。

如果你遇到的麻烦, 或者你弄出了很cooool的东西想要分享给大家看, 请给我发邮件。我的邮箱是ptrshrl@gmail.com

我会维护一个有关本书的博客网站, 网站里有一些拓展阅读和一些链接资源。网址是 [https://in1weekend.blogspot.com](https://link.zhihu.com/?target=https%3A//in1weekend.blogspot.com)

好了不多BB, 让我们开始吧!

## 2. 输出你的图像

当你开始写渲染器的时候, 你首先要有办法看到你渲染的图像。最直接了当的方法就行把图像信息写入文件。问题是, 有那么多图片格式, 而且许多格式都挺复杂的。在一开始, 我常常使用最简单的ppm文件。这里引用Wikipedia上面的简明介绍:

![img](.\img\v2-d23bb56acdadb81f85b89d8fc74f48e1_720w.webp)

我们来写一下输出这种图片格式的C++代码:

```cpp
#include <iostream>

int main() {
    const int image_width = 200;
    const int image_height = 100;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            auto r = double(i) / image_width;
            auto g = double(j) / image_height;
            auto b = 0.2;
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
}
```

代码里有一些我们要注意的事情:

1. 对于像素来说, 每一行是从左往右写入的。
2. 行从上开始往下写入的。
3. 通常我们把RGB通道的值限定在0.0到1.0。我们之后计算颜色值的时候将使用一个动态的范围, 这个范围并不是0到1。但是在使用这段代码输出图像之前, 我们将把颜色映射到0到1。所以这部分输出图像代码不会改变。【译注: 这里挺重要的, 别忘了。忘掉这个在第一本书并不会出现任何问题, 直到第二本书前大半也没问题, 但是一到cornell box与光源的引入, 事情就糟糕了, 不少人都踩进去了,详见[issue-94](https://link.zhihu.com/?target=https%3A//github.com/RayTracing/raytracing.github.io/issues/94)】
4. 下方的红色从左到右由黑边红, 左侧的绿色从上到下由黑到绿。红+绿变黄, 所以我们的右上角应该是黄的。

现在我们要把cout的输出流写入文件中。幸好我们有命令行操作符>来定向输出流。在windows操作系统中差不多这样的:

```bash
build\Release\inOneWeekend.exe > image.ppm
```

在Mac或者Linux操作系统中, 大概是这个样子的

```bash
build/inOneWeekend > image.ppm
```

打开我们输出的文件(我是Mac系统, 我是用ToyViewer打开的, 你可以用你喜欢的任意看图软件来打开。如果你默认的看图软件(比如windows下的图片)不支持ppm格式, 只要Google一下"ppm viewer"装个新的就行。)打开后的结果如下:

![img](.\img\v2-fb2906fa70e6a50c6814f52351d1d8d2_720w.webp)

好耶!这便是图形学中的"hello world"了【吐槽：图形学的hello world不是三角形嘛】。如果你的图像看上去不是这样的, 用文本编辑器打开你的输出文件, 看看里面内容是啥样的。不出意外的话, 正确格式应该是这样的:

```text
P3
200 100
255
0 253 51
1 253 51
2 253 51
3 253 51
5 253 51
6 253 51
7 253 51
8 253 51
```

如果不是这样的, 你可能当中多了些空行或者类似的什么东西, 因此你的看图软件识别不出来。

如果你想生成别的图像格式来代替基础的PPM, 我强烈安利[stb_image.h](https://link.zhihu.com/?target=https%3A//github.com/nothings/stb/blob/master/stb_image.h), 你可以免费在github上获取。

## 2.1. 加入进度提示

在我们往下走之前, 我们先来加个输出的进度提示。对于查看一次长时间渲染的进度来说, 这不失为一种简便的做法。也可以通过这个进度来判断程序是否卡住或者进入一个死循环。

我们的程序将图片信息写入标准输出流(std::cout), 所以我们不能用这个流输出进度。我们换用错误输出流(std::cerr)来输出进度:

```cpp
for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto r = double(i) / image_width;
            auto g = double(j) / image_height;
            auto b = 0.2;
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
std::cerr << "\nDone.\n";
```

## 3. vec3向量类

几乎所有的图形程序都使用类似的类来储存几何向量和颜色。在许多程序中这些向量是四维的(对于位置或者几何向量来说是三维的齐次拓展, 对于颜色来说是RGB加透明通道)。对我们现在这个程序来说, 三维就足够了。我们用一个`vec3`类来储存所有的颜色, 位置, 方向, 位置偏移, 或者别的什么东西。一些人可能不太喜欢这样做, 因为全都用一个类, 没有限制, 写代码的时候难免会犯二, 比如你把颜色和位置加在一起。他们的想法挺好的, 但是我们想在避免明显错误的同时让代码量尽量的精简。所以这里就先一个类吧。【译注: 之后的书中有添加新的color类】

下面是我的`vec3`的头文件:

```cpp
#include <iostream>

class vec3 {
    public:
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t) {
            return *this *= 1/t;
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        void write_color(std::ostream &out) {
            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(255.999 * e[0]) << ' '
                << static_cast<int>(255.999 * e[1]) << ' '
                << static_cast<int>(255.999 * e[2]) << '\n';
        }

    public:
        double e[3];
};
```

我们使用双精度浮点double, 但是有些光线追踪器使用单精度浮点float。这里其实都行, 你喜欢哪个就用那个。头文件的第二部分包括一些向量操作工具函数:

```cpp
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}
```

现在我们可以使用vec3类将我们的main函数改成这样啦:

```cpp
#include "vec3.h"

#include <iostream>

int main() {
    const int image_width = 200;
    const int image_height = 100;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(double(i)/image_width, double(j)/image_height, 0.2);
            color.write_color(std::cout);
        }
    }

    std::cerr << "\nDone.\n";
}
```

## 4. 光线, 简单摄像机, 以及背景

所有的光线追踪器都有个一个ray类, 我们假定光线的公式为 �(�)=�+��→ 。这里的 � 是三维射线上的一个点。 � 是射线的原点, �→ 是射线的方向。类中的变量 � 是一个实数(代码中为double类型)。 �(�) 接受任意的 � 做为变量, 返回射线上的对应点。如果允许 � 取负值你可以得到整条直线。对于一个正数 � , 你只能得到原点前部分 � , 这常常被称为半条直线, 或者说射线。

![img](.\img\v2-0a13240fad9e360645339165054c4e62_720w.webp)

图1:线性插值

我在代码中使用复杂命名, 将函数p(t)扩写为ray::at(t)【吐糟: 你之前版本中那个ray::point_at_parameter(t)才叫复杂】

```cpp
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    public:
        ray() {}
        ray(const vec3& origin, const vec3& direction)
            : orig(origin), dir(direction)
        {}

        vec3 origin() const    { return orig; }
        vec3 direction() const { return dir; }

        vec3 at(double t) const {
            return orig + t*dir;
        }

    public:
        vec3 orig;
        vec3 dir;
};
#endif
```

现在我们再拐回来做我们的光线追踪器。光线追踪器的核心是从像素发射射线, 并计算这些射线得到的颜色。这包括如下的步骤:

1. 将射线从视点转化为像素坐标
2. 计算光线是否与场景中的物体相交
3. 如果有, 计算交点的颜色。在做光线追踪器的初期, 我会先弄个简单摄像机让代码能跑起来。我也会编写一个简单的color(ray)函数来返回背景颜色值(一个简单的渐变色)。

在使用正方形的图像Debug时我时常会遇到问题, 因为我老是把x和y弄反。所以我坚持使用200x100这样长宽不等的图像。我会把视点(或者说摄像机, 如果你认为它是个摄像机的话)放在(0,0,0)。这里y轴向上, x轴向右, 为了准守使用右手系的规范, 摄像机看向的方向为z轴的负方向。我会把发出射线的原点从图像的左下角开始沿着xy方向做增量直至遍历全图。注意我这里并没有将射线的向量设置为单位向量, 因为我认为这样代码会更加简单快捷。

![img](.\img\v2-d8d14f547542744c59ef7db86e510083_720w.webp)

图2:摄像机图例

下面是代码, 射线r现在只是近似的从各个像素的中心射出(现在不必担心精度问题, 因为我们一会儿就会加入抗锯齿):

```cpp
#include "ray.h"

#include <iostream>

vec3 ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

int main() {
    const int image_width = 200;
    const int image_height = 100;

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / image_width;
            auto v = double(j) / image_height;
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            vec3 color = ray_color(r);
            color.write_color(std::cout);
        }
    }

    std::cerr << "\nDone.\n";
}
```

ray_color(ray)函数根据y值将蓝白做了个线性插值的混合, 我们这里把射线做了个单位化, 以保证y的取值范围(-1.0<y<1.0)。因为我们使用y轴做渐变, 所以你可以看到这个蓝白渐变也是竖直的。

我接下来使用了一个标准的小技巧将y的范围从-1.0 ≤ y ≤ 1.0映射到了0 ≤ y ≤ 1.0。这样t=1.0时就是蓝色, 而t=0.0时就是白色。在蓝白之间我想要一个混合效果(blend)。现在我们采用的是线性混合(linear blend)或者说线性插值(liner interpolation)。或者简称其为lerp。一个lerp一般来说会是下面的形式:

blendedValue=(1−t)⋅startValue+t⋅endValue

当t从0到1, 我们会渲染出这样的图像:

![img](.\img\v2-f16c3bad8439858002670f5ee582789e_720w.webp)

一个沿Y轴蓝白渐变的图像

##  5. 加入球体

让我们为我们的光线追踪器加入一个物体吧!人们通常使用的球体, 因为计算射线是否与球体相交是十分简洁明了的。回想一下我们中学时期学过的球体表面方程, 对于一个半径为r的球体来说, 有方程 $x^2+y^2+z^2=R^2$ , 其中(x,y,z)是球面上的点。如果我们想要表示点(x,y,z)在球体的内部, 那便有方程$$x^2+y^2+z^2<R^2$$ , 类似的, 如果要表示球体外部的点, 则有$$x^2+y^2+z^2>R^2$$

如果球体的球心在 $(c_x,c_y,c_z)$ , 那么这个式子就会变得丑陋一些:

$(x−c_x)^2+(y−c_y)^2+(z−c_z)^2=R^2$

在图形学中, 你总希望你方程里面所有东西都是用向量表达的, 这样我们就能用vec3这个类来存储所有的这些xyz相关的东西了。你也许会意识到, 对于到球面上的点 $P=(x,y,z)$ 到球心 $c=(c_x,c_y,c_z)$的距离可以使用向量表示为 $(p−c)$ , 于是就有

$(p−c)⋅(p−c)=(x−c_x)^2+(y−c_y)^2+(z−c_z)^2$

于是我们就能得到球面方程的向量形式:

$(p−c)⋅(p−c)=R^2$

我们可以将其解读为"满足方程上述方程的任意一点p一定位于球面上"。我们还要知道射线$p(t)=a+t\vec{b}$是否与球体相交。如果说它相交了, 那么肯定有一个t使直线上的点p(t)满足球面方程。所以我们先来计算满足条件的任意t值:

$(p(t)−c)⋅(p(t)−c)=R^2$

或者将p(t)展开为射线方程:

$(a+t\vec{b}−c)⋅(a+t\vec{b}−c)=R^2$

好啦, 我们需要的代数部分就到这里。现在我们来展开表达式并移项, 得:

$t^2\vec{b}⋅\vec{b}+2t\vec{b}⋅\vec{(a−c)}+\vec{(a−c)}⋅\vec{(a−c)}−R^2=0$

方程中的向量和半径R都是已知的常量, 唯一的未知数就是t, 并且这个等式是关于t的一个一元二次方程, 就像你在高中数学课上【吐槽: ？？？】学到的那样。你可以用求根公式来判别交点个数, 为正则2个交点, 为负则无交点, 为0则一个交点。在图形学中, 代数与几何往往密切相关, 你看图:

![img](.\img\v2-5a3fbd572f05c511bc2cdf7fcd3df55a_720w.webp)

图3: 射线与球体相交的交点情况

如果我们使用代码来求解, 并使用红色来表示射线击中我们放在(0,0,-1)的小球:

```cpp
bool hit_sphere(const vec3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

vec3 ray_color(const ray& r) {
    if (hit_sphere(vec3(0,0,-1), 0.5, r))
        return vec3(1, 0, 0);
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

我们会得到:

![img](.\img\v2-73a372b979105344547ea89345970093_720w.webp)

一个简单的红色球体

现在我们啥都缺: 例如光照, 反射, 加入更多的物体, 但是我们离成功又近了一步!现在你要注意我们其实求的是直线与球相交的解, t<0的那些情况也计算进去了, 而我们只想要直线中一段射线的解。如果你将你的球心设置在(0,0,1), 你会得到完全相同的结果。这不是一个特性(feature)!【吐槽: 直接说it's a bug嘛】我们会在接下来的章节修复这个bug。

## 6. 面法相与复数物体

为了来给球体着色, 首先我们来定义一下面法相。面法相应该是一种垂直于交点所在平面的三维向量。关于面法相我们存在两个设计抉择。首先是是否将其设计为单位向量, 这样对于着色器来说, 所以我会说"yes!"但是我并没有在代码里这么做, 这部分差异可能会导致一些潜在的bug。所以记住, 这个是个人喜好, 大多数的人喜好使用单位法相。对于球体来说, 朝外的法相是直线与球的交点减去球心:

![img](.\img\v2-009e0ab00e758c137a5a9a6dbc639a8c_720w.webp)

图4: 球体的表面法相

说到底, 其实就是从球心到交点再向外延伸的那个方向。让我们把这部分转变成代码并开始着色。我们暂时还没有光源这样的东西, 所以让我们直接将法相值作为颜色输出吧。对于法相可视化来说, 我们常常将xyz分量的值先映射到0到1的范围(假定 ���� 是一个单位向量, 它的取值范围是-1到1的),再把它赋值给rgb。对于法相来说, 光能判断射线是否与球体相交是不够的, 我们还需求出交点的坐标。在有两个交点的情况下, 我们选取最近的交点smallest(t)。计算与可视化球的法向量的代码如下:

```cpp
//main.cc 球体表面法相
double hit_sphere(const vec3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant) ) / (2.0*a);
    }
}

vec3 ray_color(const ray& r) {
    auto t = hit_sphere(vec3(0,0,-1), 0.5, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0,0,-1));
        return 0.5*vec3(N.x()+1, N.y()+1, N.z()+1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

这会得到下面的结果:

![img](.\img\v2-7b6105ac601399ee8171da1c3df9cf56_720w.webp)

使用法相作为颜色值输出

我们再来回顾上面的直线方程:

```cpp
//main.cc
vec3 oc = r.origin() - center;
auto a = dot(r.direction(), r.direction());
auto b = 2.0 * dot(oc, r.direction());
auto c = dot(oc, oc) - radius*radius;
auto discriminant = b*b - 4*a*c;
```

首先, 回想一下一个向量与自己的点积就是它的长度的平方(都是$x^2+y^2+z^2$ )

其次, 注意其实我们的`b`有一个系数2, 我们设b=2h，有:
$$
\frac{-b\pm\sqrt{b^2-4ac}}{2a}
\\
= \frac{-2h\pm\sqrt{(2h)^2-4ac}}{2a}
\\
= \frac{-2h\pm2\sqrt{h^2-ac}}{2a}
\\
= \frac{-h\pm\sqrt{h^2-ac}}{a}
$$


所以射线与球体求交的代码其实可以简化成下面这样:

```cpp
//main.cc
vec3 oc = r.origin() - center;
auto a = r.direction().length_squared();
auto half_b = dot(oc, r.direction());
auto c = oc.length_squared() - radius*radius;
auto discriminant = half_b*half_b - a*c;

if (discriminant < 0) {
    return -1.0;
} else {
    return (-half_b - sqrt(discriminant) ) / a;
}
```

好啦, 那么怎么在场景中渲染不止一个球呢? 很直接的我们想到使用一个sphere数组, 这里有个很简洁的好方法: 使用一个抽象类, 任何可能与光线求交的东西实现时都继承这个类, 并且让球以及球列表也都继承这个类。我们该给这个类起个什么样的名字呢? 叫它object好像不错但现在我们使用面向对象编程(oop)。suface是时常被翻牌, 但是如果我们想要体积体(volumes)的话就不太适合了。hittable又过于强调了自己的成员函数hit。所以我哪个都不喜欢, 但是总得给它个名字的嘛, 那我就叫它hittable:

hittable类理应有个接受射线为参数的函数, 许多光线追踪器为了便利, 加入了一个区间 ����<�<���� 来判断相交是否有效。对于一开始的光线来说, 这个t值总是正的, 但加入这部分对代码实现的一些细节有着不错的帮助。现在有个设计上的问题:我们是否在每次计算求交的时候都要去计算法相?但其实我们只需要计算离射线原点最近的那个交点的法相就行了, 后面的东西会被遮挡。接下来我会给出我的代码, 并将一些计算的结果存在一个结构体里, 来看, 这就是那个抽象类:

```cpp
//hittable.h
#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

struct hit_record {
    vec3 p;
    vec3 normal;
    double t;
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif
```

这是继承自它的sphere球体类:

```cpp
//sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere: public hittable {
    public:
        sphere() {}
        sphere(vec3 cen, double r) : center(cen), radius(r) {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

    public:
        vec3 center;
        double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
    }
    return false;
}
#endif
```

好了, 让我们来谈谈第二个关于面法相设计上的问题吧， 那就是面法相的朝向问题。对于现在来说, 如果光线从球体外部击中球体, 那么法相也是朝外的, 与射线的方向相反(不是数学意义上的严格相反, 只是大致逆着)。如果光线从内部射向球面时, 此时的面法相依然朝外, 与射线方向相同。相对的, 我们也可以总是让法相向量与射线方向相反, 即射线从外部射向球面, 法向量朝外, 射线从内部射向球面, 法向量向着球心。

![img](.\img\v2-f3be9d1529ba6a7d923f9c4e3533aba3_720w.webp)

图5: 光线可能射入的方向

在我们着色前, 我们需要仔细考虑一下采用上面哪种方式, 这对于双面材质来说至关重要。例如一张双面打印的A4纸, 或者玻璃球这样的同时具有内表面和外表面的物体。

如果我们决定让法相永远朝外, 那在我们就得在射入的时候判断是从表面的哪一侧射入的, 我们可以简单的将光线与法相做点乘来判断。如果法相与光线方向相同, 那就是从内部击中内表面, 如果相反则是从外部击中外表面。【译注: $dot(a,b)=cosθ|a||b|$】

```cpp
if (dot(ray_direction, outward_normal) > 0.0) {
    // ray is inside the sphere
    ...
} else {
    // ray is outside the sphere
    ...
}
```


如果我们永远让法相与入射方向相反, 我们就不用去用点乘来判断射入面是内侧还是外侧了, 但相对的, 我们需要用一个变量储存射入面的信息:

```cpp
bool front_face;
if (dot(ray_direction, outward_normal) > 0.0) {
    // ray is inside the sphere
    normal = -outward_normal;
    front_face = false;
}
else {
    // ray is outside the sphere
    normal = outward_normal;
    front_face = true;
}
```

其实采取哪种策略, 关键在于你想把这部分放在着色阶段还是几何求交的阶段。【译注:反正都要算的, v2.0的时候是在着色阶段判别的, v3.0把它放在了求交阶段】。在本书中我们我们的材质类型会比我们的几何图元类型多, 所以为了有更少的代码量, 我们会在几何部分先判别射入面是内侧还是外侧。这当然也是一种个人喜好。

我们在结构体hit_record中加入front_face变量, 我们接下来还会弄一些动态模糊相关的事情(Book2 chapter1),所以我还会加入一个时间变量:

```cpp
//hittable.h 加入时间与面朝向
ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

struct hit_record {
    vec3 p;
    vec3 normal;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};
#endif
```

接下来我们在求交时加入射入面的判别:

```cpp
//sphere.h 加入射入面判别
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
    }
    return false;
}
```

我们加入存放物体的列表

```cpp
//hittable_list.h
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list: public hittable {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

    public:
        std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

#endif
```

## 6.1. 一些C++的新特性

hittable_list类用了两种C++的特性:vector和shared_ptr, 如果你并不熟悉C++, 你可能会感到有些困惑。

shared_ptr<type>是指向一些已分配内存的类型的指针。每当你将它的值赋值给另一个智能指针时, 物体的引用计数器就会+1。当智能指针离开它所在的生存范围(例如代码块或者函数外), 物体的引用计数器就会-1。一旦引用计数器为0, 即没有任何智能指针指向该物体时, 该物体就会被销毁

一般来说, 智能指针首先由一个刚刚新分配好内存的物体来初始化:

```cpp
shared_ptr<double> double_ptr = make_shared<double>(0.37);
shared_ptr<vec3>   vec3_ptr   = make_shared<vec3>(1.414214, 2.718281, 1.618034);
shared_ptr<sphere> sphere_ptr = make_shared<sphere>(vec3(0,0,0), 1.0);
```

make_shared<thing>(thing_constructor_params ...)为指定的类型分配一段内存, 使用你指定的构造函数与参数来创建这个类, 并返回一个智能指针shared_ptr<thing>

使用C++的auto类型关键字, 可以自动推断make_shared<type>返回的智能指针类型, 于是我们可以把上面的代码简化为:

```cpp
auto double_ptr = make_shared<double>(0.37);
auto vec3_ptr   = make_shared<vec3>(1.414214, 2.718281, 1.618034);
auto sphere_ptr = make_shared<sphere>(vec3(0,0,0), 1.0);
```

我们在代码中使用智能指针的目的是为了能让多个几何图元共享一个实例(举个栗子, 一堆不同球体使用同一个纹理材质), 并且这样内存管理比起普通的指针更加的简单方便。

std::shared_ptr在头文件<memory>中

```cpp
#include<memory>
```

第二个你可能不太熟悉的C++特性是std::vector。这是一个类似数组的结构类型, 可以存储任意指定的类型。在上面的代码中, 我们将hittable类型的智能指针存入vector中, 随着objects.push_back(object)的调用, object被存入vector的末尾, 同时vector的储存空间会自动增加。

std::vector在头文件<vector>中

```cpp
#include<vector>
```

最后, 位于hittable_list.h文件开头部分的using语句告诉编译器, shared_ptr与make_shared是来自std库的。这样我们在使用它们之前就不用每次都加上前缀std::。

## 6.2. 常用的常数与工具

我们需要在头文件中定义一些常用的常数。目前为止我们只需要定义无穷。但是我们先把pi在这里定义好, 之后要用的。对于pi来说并没有什么跨平台的标准定义【译注: 这就是为什么不使用之前版本中M_PI宏定义的原因】, 所以我们自己来定义一下。我们在rtweekend.h中给出了一些未来常用的常数和函数:

```cpp
//rtweekend.h
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


// Usings

using std::shared_ptr;
using std::make_shared;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180;
}

inline double ffmin(double a, double b) { return a <= b ? a : b; }
inline double ffmax(double a, double b) { return a >= b ? a : b; }

// Common Headers

#include "ray.h"
#include "vec3.h"

#endif
```

以及这是更新后的main函数:

```cpp
//main.cc
#include "rtweekend.h"

#include "hittable_list.h"
#include "sphere.h"

#include <iostream>
vec3 ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + vec3(1,1,1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

int main() {
    const int image_width = 200;
    const int image_height = 100;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    hittable_list world;
    world.add(make_shared<sphere>(vec3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(vec3(0,-100.5,-1), 100));

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / image_width;
            auto v = double(j) / image_height;
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);

            vec3 color = ray_color(r, world);

            color.write_color(std::cout);
        }
    }

    std::cerr << "\nDone.\n";
}
```

这样我们就会得到一张使用法向作为球体颜色值的图片。当你想查看模型的特征细节与瑕疵时, 输出面法向作为颜色值不失为一种很好的方法。

![img](.\img\v2-c4e02ebc310fa0ab8102bf334c31a21e_720w.webp)

加上地板, 输出法相

## 7. 反走样(抗锯齿)

真实世界中的摄像机拍摄出来的照片是没有像素状的锯齿的。因为边缘像素是由背景和前景混合而成的。我们也可以在程序中简单的对每个边缘像素多次采样取平均达到类似的效果。我们这里不会使用分层采样。尽管我自己常常在我的程序里使用这种有争议的方法。对某些光线追踪器来说分层采样是很关键的部分, 但是对于我们写的这个小光线追踪器并不会有什么很大的提升, 只会让代码更加丑陋。我们会在这里将摄像机类抽象一下, 以便于后续能有一个更酷的摄像机。

我们还需要一个能够返回真随机数的一个随机数生成器。默认来说这个函数应该返回0≤r<1的随机数。注意这个范围取不到1是很重要的。有时候我们能从这个特性中获得好处。

一个简单的实现方法是, 使用<cstdlib>中的rand()函数。这个函数会返回0到RAND_MAX中的一个任意整数。我们将下面的一小段代码加到rtweekend.h中, 就能得到我们想要的随机函数了:

```cpp
//rtweekend.h
#include <cstdlib>
...

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}
```

传统C++并没有随机数生成器, 但是新版C++中的<random>头实现了这个功能(某些专家觉得这种方法不太完美)。如果你想使用这种方法, 你可以参照下面的代码:

```cpp
//rtweekend.h
#include <functional>
#include <random>

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static std::function<double()> rand_generator =
        std::bind(distribution, generator);
    return rand_generator();
}
```

对于给定的像素, 我们发射多条射线进行多次采样。然后我们对颜色结果求一个平均值:

![img](.\img\v2-e54c77c97131c90f04a27e5a5de7ec55_720w.webp)

图6: 对一个像素进行多次采样

综上, 我们对我们的简单的轴对齐摄像机类进行了一次封装:

```cpp
//camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera() {
            lower_left_corner = vec3(-2.0, -1.0, -1.0);
            horizontal = vec3(4.0, 0.0, 0.0);
            vertical = vec3(0.0, 2.0, 0.0);
            origin = vec3(0.0, 0.0, 0.0);
        }

        ray get_ray(double u, double v) {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
#endif
```

为了对多重采样的颜色值进行计算, 我们升级了vec3::write_color()函数。我们不会在每次发出射线采样时都计算一个0-1之间的颜色值, 而是一次性把所有的颜色都加在一起, 然后最后只需要简单的一除(除以采样点个数)。另外, 我们给头文件rtweekend.h加入了一个新函数clamp(x,min,max), 用来将x限制在[min,max]区间之中:

```cpp
//rtweekend.h
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
```



```cpp
//vec3.h
...
#include "rtweekend.h"
...
void write_color(std::ostream &out, int samples_per_pixel) {
    // Divide the color total by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    auto r = scale * e[0];
    auto g = scale * e[1];
    auto b = scale * e[2];

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
```

main函数也发生了变化:

```cpp
//main.cc
int main() {
    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 100;

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    hittable_list world;
    world.add(make_shared<sphere>(vec3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(vec3(0,-100.5,-1), 100));
    camera cam;
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world);
            }
            color.write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
```

停, 放大放大再放大, 看啊, 每一个像素都是背景和前景的混合:

![img](.\img\v2-807248338e5835236f2c884fa9ed4105_720w.webp)

加入抗锯齿效果后前背景颜色混合的像素

## 8. 漫反射材质

既然我们已经有了物体的类和多重采样, 我们不妨再加入一些逼真的材质吧。我们先从漫反射材质开始。设计上的问题又来了:我们是把材质和物体设计成两个类, 这样就可以将材质赋值给物体类的成员变量, 还是说让它们紧密结合,这对于使用几何信息来生成纹理的程序来说是很便利的 。我们会采取将其分开的做法——实际上大多数的渲染器都是这样做的——但是记得注意的确是有两种设计方法的。

漫反射材质不仅仅接受其周围环境的光线, 还会在散射时使光线变成自己本身的颜色。光线射入漫反射材质后, 其反射方向是随机的。所以如果我们为下面这两个漫发射的球射入三条光线, 光线都会有不同的反射角度:

![img](.\img\v2-f61ad9d024071668f27db1fc248abcf5_720w.webp)

并且大部分的光线都会被吸收, 而不是被反射。表面越暗, 吸收就越有可能发生。我们使用任意的算法生成随机的反射方向, 就能让其看上去像一个粗糙不平的漫反射材质。这里我们采用最简单的算法就能得到一个理想的漫反射表面(其实是懒得写lambertian所以用了一个数学上近似的方法)。

(读者Vassillen Chizhov 提供了这个方法, 虽然并不是精确意义上的lambert。我们会在章节最后提精确的lambertian表达式, 而其并不会很复杂)

好, 现在有两个单位球相切于点p, 这两个球体的球心为$(p+\vec{N})$ 和 $(p−\vec{N})$ ,$\vec{N}$是球体表面的法向量。球心为 $(p-\vec{N})$ 的那个球在表面的内部, 球心为 $(p+\vec{N})$的球在表面的外部。选择和光线原点位于表面同一侧的那个单位球, 并从球中随机选取一点 $s$ , 向量 $(s-p)$ 就是我们要求的反射光线的方向:

![img](.\img\v2-45d8ee02fefda11052daf78be9ace252_720w.webp)

图8：生成一条随机的反射光线

我们需要一个算法来生成球体内的随机点。我们会采用最简单的做法:否定法(rejection method)。首先, 在一个xyz取值范围为-1到+1的单位立方体中选取一个随机点, 如果这个点在球外就重新生成直到该点在球内:

```cpp
//vec3.h
class vec3 {
  public:
    ...
    inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max) {
        return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }
```



```cpp
//vec3.h
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
```

然后使用我们新的生成随机随机反射方向的函数来更新一下我们的ray_color()函数:

```cpp
//main.cc
vec3 ray_color(const ray& r, const hittable& world) {
    hit_record rec;

    if (world.hit(r, 0, infinity, rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

这里还有个潜在的问题: 注意ray_color函数是一个递归函数。那么递归终止的条件是什么呢?当它没有击中任何东西。但是, 在某些条件下, 达到这个终止条件的时间会非常长, 长到足够爆了函数栈【译注:想象一下一条光线在一个镜子材质的密封的盒子(并不吸收光线)中反复折射, 永无尽头】。为了避免这种情况的发生, 我们使用一个变量depth限制递归层数。当递归层数达到限制值时我们终止递归, 返回黑色:【译注: 可以试试返回纯红(1,0,0), 然后渲染一下, 大致看一下是哪里在不停的发生散射】

```cpp
//main.cc
vec3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0,0,0);

    if (world.hit(r, 0, infinity, rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
...
int main() {
    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    ...
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
               color += ray_color(r, world, max_depth);
            }
            color.write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
```

我们会得到:

![img](.\img\v2-01259cbd92f891d2db907a773d51e7aa_720w.webp)

第一次渲染出漫反射材质的球体

注意球下面是有影子的。这个图片非常的暗, 但是我们的球在散射的时候只吸收了一半的能量。如果你看不见这个阴影, 别担心, 我们现在来修复一下。现实世界中的这个球明显是应该更加亮一些的。这是因为所有的看图软件都默认图像已经经过了伽马校正(gamma corrected)。即在图片存入字节之前, 颜色值发生了一次转化。这么做有许多好处, 但这并不是我们这里所讨论的重点。我们使用"gamma 2"空间, 就意味着最终的颜色值要加上指数 1/����� , 在我们的例子里就是 ½, 即开平方根:

```cpp
//vec3.h
void write_color(std::ostream &out, int samples_per_pixel) {
    // Divide the color total by the number of samples and gamma-correct
    // for a gamma value of 2.0.
    auto scale = 1.0 / samples_per_pixel;
    auto r = sqrt(scale * e[0]);
    auto g = sqrt(scale * e[1]);
    auto b = sqrt(scale * e[2]);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
```

好了, 现在看上去更灰了, 如我们所愿:

![img](.\img\v2-f0ab33e768ac857087b5e2547f47b8b3_720w.webp)

伽马校正后的漫反射球体

这里还有个不太重要的潜在bug。有些物体反射的光线会在t=0时再次击中自己。然而由于精度问题, 这个值可能是t=-0.000001或者是t=0.0000000001或者任意接近0的浮点数。所以我们要忽略掉0附近的一部分范围, 防止物体发出的光线再次与自己相交。【译注: 小心自相交问题】



```cpp
//main.cc
if (world.hit(r, 0.001, infinity, rec)) {
```

这样我们就能避免阴影痤疮(shadow ance)的产生。是滴, 这种现象的确是叫这个名字。

拒绝法生成的点是单位球体积内的的随机点, 这样生成的向量大概率上会和法线方向相近, 并且极小概率会沿着入射方向反射回去。这个分布律的表达式有一个 $cos^3(ϕ)$的系数, 其中 $ϕ$ 是反射光线距离法向量的夹角。这样当光线从一个离表面很小的角度射入时, 也会散射到一片很大的区域, 对最终颜色值的影响也会更低。

然而, 事实上的lambertian的分布律并不是这样的, 它的系数是 cos⁡(ϕ) 。真正的lambertian散射后的光线距离法相比较近的概率会更高, 但是分布律会更加均衡。这是因为我们选取的是单位球面上的点。我们可以通过在单位球内选取一个随机点, 然后将其单位化来获得该点。【译注: 然而下面的代码却用了极坐标的形式】

```cpp
//vec3.h
vec3 random_unit_vector() {
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return vec3(r*cos(a), r*sin(a), z);
}
```

![img](.\img\v2-2a37ce5c0bb8e7e3f51cd3f947ce4f8f_720w.webp)

图9: 在球面上生成一个随机的向量

我们使用新函数random_unit_vector()替换现存的random_unit_sphere():

```cpp
//main.cc
vec3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        vec3 target = rec.p + rec.normal + random_unit_vector();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

我们会得到这样的图片, 和之前很相像:

![img](.\img\v2-cc4cdd3d687349fa0d472a701f541cfe_720w.webp)

正确的lambertian球体

我们的场景太简单, 区分这两种方法是比较难的。但你应该能够注意到视觉上的一些差异:

1.阴影部分少了
2.大球和小球都变亮了

这些变化都是由散射光线的单位规整化引起的, 现在更少的光线会朝着发现方向散射。对于漫发射的物体来说, 他们会变得更亮。因为更多光线朝着摄像机反射。对于阴影部分来说, 更少的光线朝上反射, 所以小球下方的大球区域会变得更加明亮。

这本书很长一段时间都采用的是先前的版本, 直到后来有一天大家发现它其实只是理想lambertian漫发射的近似, 其并不正确。这个错误在本书中留存了那么长时间, 主要是因为:

1.概率分布的数学证明算错了
2.视觉上来说, 并不能直接看出 cos⁡(ϕ) 的概率分配是我们所需要的

因为大家日常生活中的物体都是发生了完美的漫反射, 所以我们很难养成对光照下物体是如何表现的视觉直觉。

为了便于大家理解, 简单来说两种方法都选取了一个随机方向的向量, 不过一种是从单位球体内取的, 其长度是随机的, 另一种是从单位球面上取的, 长度固定为单位向量长度。为什么要采取单位球面并不是能很直观的一眼看出。

另一种具有启发性的方法是, 直接从入射点开始选取一个随机的方向, 然后再判断是否在法向量所在的那个半球。在使用lambertian漫发射模型前, 早期的光线追踪论文中大部分使用的都是这个方法:

```cpp
//vec3.h
vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}
```

将我们的新函数套入ray_color()函数:

```cpp
//vec3.h
vec3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        vec3 target = rec.p + random_in_hemisphere(rec.normal);
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

我们会得到如下的图片:

![img](.\img\v2-733c6ea8353952f6f50e33a1990c8a67_720w.webp)

使用半球面向量渲染漫反射球体

我们的场景会随着本书的深入会变得越来越复杂。这里鼓励大家在之后都试一下这几种不同的漫反射渲染法。大多数场景都会有许多的漫反射材质。你可以从中培养出你对这几种方法的敏感程度。

## 9. 金属材质

如果我们想让不同的物体能拥有不同的材质, 我们又面临着一个设计上的抉择。我们可以设计一个宇宙无敌大材质, 这个材质里面有数不胜数的参数和材质类型可供选择。这样其实也不错, 但我们还可以设计并封装一个抽象的材质类。我反正喜欢后面一种, 对于我们的程序来说, 一个材质类应该封装两个功能进去:

1.生成散射后的光线(或者说它吸收了入射光线)
2.如果发生散射, 决定光线会变暗多少(attenuate)

下面来看一下这个抽象类:

```cpp
//material.h
class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const = 0;
};
```

我们在函数中使用hit_record作为传入参数, 就可以不用传入一大堆变量了。当然如果你想传一堆变量进去的话也行。这也是个人喜好。当然物体和材质还要能够联系在一起。在C++中你只要告诉编译器, 我们在`hit_record`里面存了个材质的指针。

```cpp
//hittable.h
#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material;

struct hit_record {
    vec3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;


    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif
```

光线会如何与表面交互是由具体的材质所决定的。hit_record在设计上就是为了把一堆要传的参数给打包在了一起。当光线射入一个表面(比如一个球体), hit_record中的材质指针会被球体的材质指针所赋值, 而球体的材质指针是在main()函数中构造时传入的。当color()函数获取到hit_record时, 他可以找到这个材质的指针, 然后由材质的函数来决定光线是否发生散射, 怎么散射。

所以我们必须在球体的构造函数和变量区域中加入材质指针, 以便之后传给`hit_record`。见下面的代码:

```cpp
class sphere: public hittable {
    public:
        sphere() {}
+        sphere(vec3 cen, double r, shared_ptr<material> m)
+            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

    public:
        vec3 center;
        double radius;
+        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;                
            rec.set_face_normal(r, outward_normal);
+            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}
```

对于我们之前写过的Lambertian(漫反射)材质来说, 这里有两种理解方法, 要么是光线永远发生散射, 每次散射衰减至R, 要么是光线并不衰减, 转而物体吸收(1-R)的光线。你也可以当成是这两种的结合。于是我们可以写出Lambertian的材质类:

```cpp
//material.h
class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            vec3 scatter_direction = rec.normal + random_unit_vector();
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        vec3 albedo;
};
```

注意我们也可以让光线根据一定的概率p发生散射【译注: 若判断没有散射, 光线直接消失】, 并使光线的衰减率(代码中的attenuation)为 $albedo/p$。随你的喜好来。

对于光滑的金属材质来说, 光线是不会像漫反射那样随机散射的, 而是产生反射。关键是:对于一个金属状的镜子, 光线具体是怎么反射的呢?向量数学是我们的好朋友:



![img](.\img\v2-8029e9a049925eeec08d6884d44de012_720w.webp)



反射方向的向量如图所示为 $\vec{V}+2\vec{B}$ , 其中我们规定向量 $\vec{N}$ 是单位向量, 但 $\vec{V}$ 不一定是。向量B的长度应为$\vec{V}.\vec{N}$ , 因为向量 $\vec{V}$ 与向量 $\vec{N}$ 的方向相反, 这里我们需要再加上一个负号, 于是有:

```cpp
//vec3.h
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}
```

金属材质使用上面的公式来计算反射方向:

```cpp
//material.h
class metal : public material {
    public:
        metal(const vec3& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        vec3 albedo;
};
```

我们还需要修改一下color函数:

```cpp
//main.cc
vec3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        vec3 attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return vec3(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
```

现在我们给场景加入一些金属球:

```cpp
//main.cc
int main() {
    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    hittable_list world;

    world.add(make_shared<sphere>(
        vec3(0,0,-1), 0.5, make_shared<lambertian>(vec3(0.7, 0.3, 0.3))));

    world.add(make_shared<sphere>(
        vec3(0,-100.5,-1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));

    world.add(make_shared<sphere>(vec3(1,0,-1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2))));
    world.add(make_shared<sphere>(vec3(-1,0,-1), 0.5, make_shared<metal>(vec3(0.8, 0.8, 0.8))));

    camera cam;
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world, max_depth);
            }
            color.write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
```

我们就能得到这样的图片:

![img](.\img\v2-02f0718b49ea2f298ca637e70f1e2ae4_720w.webp)

闪耀☆金属球

我们还可以给反射方向加入一点点随机性, 只要在算出反射向量后, 在其终点为球心的球内随机选取一个点作为最终的终点:

当然这个球越大, 金属看上去就更加模糊(fuzzy, 或者说粗糙)。所以我们这里引入一个变量来表示模糊的程度(fuzziness)(所以当fuzz=0时不会产生模糊)。如果fuzz, 也就是随机球的半径很大, 光线可能会散射到物体内部去。这时候我们可以认为物体吸收了光线。

```cpp
//material.h
class metal : public material {
    public:
        metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);//dot<0我们认为吸收
        }

    public:
        vec3 albedo;
        double fuzz;
};
```

我们可以将模糊值设置为0.3和1.0, 图片会变成这样:

![img](.\img\v2-e362c4148c81abe70ebefa2d089c3eef_720w.webp)

模糊的金属

## 10. 绝缘体材质

透明的材料, 例如水, 玻璃, 和钻石都是绝缘体。当光线击中这类材料时, 一条光线会分成两条, 一条发生反射, 一条发生折射。我们会采取这样的策略: 每次光线与物体相交时, 要么反射要么折射, 一次只发生一种情况,随机选取。反正最后采样次数多, 会给这些结果取个平均值。

折射部分是最难去debug的部分。我常常一开始让所有的光线只发生折射来调试。在这个项目中, 我加入了两个这样的玻璃球, 并且得到下图(我还没教你怎么弄出这样的玻璃球, 你先往下读, 一会儿你就知道了):

![img](.\img\v2-e61f5ce6422cdb6fd13190900fc30896_720w.webp)

图10-1

这图看上去是对的么? 玻璃球在现实世界中看上去和这差不多。但是, 其实这图不对。玻璃球应该会翻转上下, 也不会有这种奇怪的黑圈。我输出了图片中心的一条光线来debug, 发现它完全错了, 你调试的时候也可以这样来。

折射法则是由Snell法则定义的:

$η⋅sinθ=η^′⋅sinθ^′$

θ与 θ′ 是入射光线与折射光线距离法相的夹角, θ 与 θ′ (读作theta和theta prime)是介质的折射率(规定空气为1.0, 玻璃为1.3-1.7,钻石为2.4), 如图:

![img](.\img\v2-4f08d1085c6bc318a23e135512b6def0_720w.webp)

折射光线示意图

为了解出折射光线的方向, 我们需要解出 sin⁡θ :

$sinθ′=\frac{η}{η′}⋅sinθ$

在折射介质部分有射线光线 R′ 与法向量 N′ , 它们的夹角为 θ′ 。我们可以把光线 R′ 分解成垂直和水平与法向量 N′ 的两个向量:

$R′ = R_∥^′ + R^′_⊥$

如果要解出这两个向量, 有:
$$
R_∥^′ = \frac{η}{η^′}(R+cosθN)
\\
R^′_⊥ = -\sqrt{1-\lvert R_∥^′ \rvert^2N}
$$


你可以自己推导,证明。我们这里先直接拿来当结论用了。这本书有些别的地方也是, 并不需要你完全会证明。【译注: 自己推推也没坏处】

然后我们来解 cos⁡θ , 下面是著名的点乘的公式定义:

$A\cdot B = \lvert A \rvert\lvert B \rvert cosθ$

如果我们将 A 与 B 归一化为单位向量:

$A⋅B=cosθ$

于是我们可以这样表达垂直的那个向量:

$R^′_∥=\frac{η}{η′}(R+(−R⋅N)N)$

根据上述公式, 我们就能写出计算折射光线 R′ 的函数:

```cpp
//vec3.h
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = dot(-uv, n);
    vec3 r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
    return r_out_parallel + r_out_perp;
}
```

一个只会发生折射的绝缘体材质为:

```cpp
//material.h
class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            attenuation = vec3(1.0, 1.0, 1.0);
            double etai_over_etat;
            if (rec.front_face) {
                etai_over_etat = 1.0 / ref_idx;
            } else {
                etai_over_etat = ref_idx;
            }

            vec3 unit_direction = unit_vector(r_in.direction());
            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

        double ref_idx;
};
```

![img](.\img\v2-368925cc729af1d7d580d5b92fc8f2e3_720w.webp)

只发生折射的玻璃材质

现在看上去图好像不太对, 这是因为当光线从高折射律介质射入低折射率介质时, 对于上述的Snell方程可能没有实解【 sin⁡�>1 】。这时候就不会发生折射, 所以就会出现许多小黑点。我们回头看一下snell法则的式子:

$sinθ′=\frac{η}{η′}⋅sinθ$

如果光线从玻璃(η=1.5 )射入空气( η=1.0 )

$sinθ′=\frac{1.5}{1.0}⋅sinθ$

又因为 sin⁡θ′ 是不可能比1大的,所以一旦这种情况发生了:

$\frac{1.5}{1.0}⋅sinθ>1.0$

那就完蛋了, 方程无解了。所以我们认为光线无法发生折射的时候, 他发生了反射:

```cpp
//material.h
if(etai_over_etat * sin_theta > 1.0) {
    // Must Reflect
    ...
}
else {
    // Can Refract
    ...
}
```

这里所有的光线都不发生折射, 转而发生了反射。因为这种情况常常在实心物体的内部发生, 所以我们称这种情况被称为"全内反射"。这也当你浸入水中时, 你发现水与空气的交界处看上去像一面镜子的原因。

我们可以用三角函数解出sin_theta

$sin⁡θ=\sqrt{1−cos^2⁡θ}$

其中的cos_theta为

$cosθ=R⋅N$

```cpp
//material.h
double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
if(etai_over_etat * sin_theta > 1.0) {
    // Must Reflect
    ...
}
else {
    // Can Refract
    ...
}
```

一个在可以偏折的情况下总是偏折, 其余情况发生反射的绝缘体材质为:

```cpp
//material.h
class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            attenuation = vec3(1.0, 1.0, 1.0);
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};
```

这里的光线衰减率为1——就是不衰减, 玻璃表面不吸收光的能量。如果我们使用下面的参数:

```cpp
main.cc
world.add(make_shared<sphere>(
    vec3(0,0,-1), 0.5, make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));

world.add(make_shared<sphere>(
    vec3(0,-100.5,-1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));

world.add(make_shared<sphere>(vec3(1,0,-1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.0)));
world.add(make_shared<sphere>(vec3(-1,0,-1), 0.5, make_shared<dielectric>(1.5)));
```

我们会得到:

![img](.\img\v2-7484d6b5ddb4744f741747ac725e7749_720w.webp)

现实世界中的玻璃, 发生折射的概率会随着入射角而改变——从一个很狭窄的角度去看玻璃窗, 它会变成一面镜子。这个式子又丑又长, 好在我们有个数学上近似的等式, 它是由Christophe Schlick提出的:

```cpp
double schlick(double cosine, double ref_idx) {
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}
```

下面就是我们完整版的玻璃材质:

```cpp
//material.h
class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            attenuation = vec3(1.0, 1.0, 1.0);
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }
            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }
            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};
```

这里有个简单又好用的trick, 如果你将球的半径设为负值, 形状看上去并没什么变化, 但是法相全都翻转到内部去了。所以就可以用这个特性来做出一个通透的玻璃球:【把一个小球套在大球里, 光线发生两次折射, 于是负负得正, 上下不会颠倒】

```cpp
world.add(make_shared<sphere>(vec3(0,0,-1), 0.5, make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));
world.add(make_shared<sphere>(
    vec3(0,-100.5,-1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));
world.add(make_shared<sphere>(vec3(1,0,-1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.3)));
world.add(make_shared<sphere>(vec3(-1,0,-1), 0.5, make_shared<dielectric>(1.5)));
world.add(make_shared<sphere>(vec3(-1,0,-1), -0.45, make_shared<dielectric>(1.5)));
```

就有:

![img](.\img\v2-1c249bd477ba8b2aeb5f49d845486fbc_720w.webp)

一个通透的玻璃球

## 11. 可自定义位置的摄像机

摄像机总是和绝缘体一样难以debug。所以我总是一步步搭建我的摄像机类。首先, 我们使摄像机能调整其视野范围(field of view, fov)。fov是你的视角。因为我们的图片不是方的, 所以垂直和水平的fov值是不同的。我总是使用垂直方向的fov。并且我总是使用角度制来传参, 在构造函数中再将其转化为弧度——这也是我的个人喜好。

首先我让射线从原点射向 z=−1 平面。我们当然也可以让其射向 z=−2 的平面,或者其他的什么值都行, 反正h和这个距离d是成比例的。

![img](.\img\v2-d8f93c297ec72b9e072535b4f6dcb9c8_720w.webp)

摄像机示意图

显然, $ℎ=tan⁡(\frac{θ}{2}) $。我们的摄像机类现在变成:

```cpp
//camera.h
class camera {
    public:
        camera(
            double vfov, // top to bottom, in degrees
            double aspect
        ) {
            origin = vec3(0.0, 0.0, 0.0);

            auto theta = degrees_to_radians(vfov);
            auto half_height = tan(theta/2);
            auto half_width = aspect * half_height;

            lower_left_corner = vec3(-half_width, -half_height, -1.0);

            horizontal = vec3(2*half_width, 0.0, 0.0);
            vertical = vec3(0.0, 2*half_height, 0.0);
        }

        ray get_ray(double u, double v) {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
```

当我们使用一个cam(90, double(image_width)/image_height)的摄像机去拍下面的球:

```cpp
auto R = cos(pi/4);
hittable_list world;
world.add(make_shared<sphere>(vec3(-R,0,-1), R, make_shared<lambertian>(vec3(0, 0, 1))));
world.add(make_shared<sphere>(vec3( R,0,-1), R, make_shared<lambertian>(vec3(1, 0, 0))));
```

我们会得到:

![img](.\img\v2-a5ee444b1b746b41d932c4fc403d6ee3_720w.webp)

一个90°的广角镜头

为了能将我们的摄像机设置在任意位置, 我们先来给这个位置点起个名字。我们管摄像机所在的这个位置叫做 lookfrom, 我们看向的点叫做lookat(如果你不想用世界坐标下的点, 想用向量来表示这个方向的话也完全ok)。

我们还需要一个变量去描述摄像机的倾斜程度, 或者说摄像机绕着轴lookfrom - lookat旋转的角度【想象下图中红色平面绕这个轴旋转】。就好比你站直了, 但是你的头还是可以左右转动。为了去描述这个倾斜程度, 我们需要一个向量来指定摄像机坐标系的正上方方向(up vector)。这里注意:这个向量就在视线方向正交投影过来的那个平面上:

![img](.\img\v2-aade90f2e3cab80e0d37eb824e87d602_720w.webp)

摄像机的视线方向

我们可以使用任意的方向向量, 将其投影到上图的平面中来获得摄像机的up vector。我这里给他起名叫vup向量。经过一系列的点乘操作, 我们会有完整的u,v,w三个向量来描述摄像机的旋向【这里要结合着代码看与下面的图片看】。

![img](.\img\v2-c981ba8338f9688cc36c7f0e4a8861eb_720w.webp)

vup

注意vup, v, w处于同一平面内。和先前我们的摄像机面对着-Z方向一样, 修改后的任意视角摄像机面对着-w方向。记得使用世界坐标系的上方向向量(0,1,0)(不是一定要用这个向量)指定vup。这样会比较方便, 并且你的摄像机镜头会保持水平。如果你想试试那些奇怪的摄像角度, 你可以放心大胆的传入别的值。

```cpp
class camera {
    public:
        camera(
            vec3 lookfrom, vec3 lookat, vec3 vup,
            double vfov, // top to bottom, in degrees
            double aspect
        ) {
            origin = lookfrom;
            vec3 u, v, w;

            auto theta = degrees_to_radians(vfov);
            auto half_height = tan(theta/2);
            auto half_width = aspect * half_height;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            lower_left_corner = origin - half_width*u - half_height*v - w;

            horizontal = 2*half_width*u;
            vertical = 2*half_height*v;
        }

        ray get_ray(double s, double t) {
            return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
        }

    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
```

现在我们就可以改变我们的视角了:

```cpp
//main.cc
const auto aspect_ratio = double(image_width) / image_height;
...
camera cam(vec3(-2,2,1), vec3(0,0,-1), vup, 90, aspect_ratio);
```

我们会得到:



![img](.\img\v2-3a76513e86c48fc03887ee41abb03b53_720w.webp)

从远处看

然后我们在改变一下fov:【这里缩小了fov】

![img](.\img\v2-3b1f9d7c41cfdf7c32c4452f82e80d48_720w.webp)

放大看

## 12. 散焦模糊

终于到了我们最后的特性了: 散焦模糊(defocus blur)。基本上所有的摄影师都它叫景深(depth of field)。所以你和你朋友聊天的时候可别提什么defocus blur啊。

现实世界中的摄像机产生对焦模糊的原因是因为他们需要一个很大的孔, 而不是一个针眼大小的小孔来聚集光线。这会导致所有的东西都被散焦了。但如果我们在孔内加入一块透镜, 在一段距离内的所有物体都会被对焦。你可以这样来想象透镜:所有的光线从同一点分散射出, 击中透镜后又聚焦在图像传感器上的一个点上。

在现实世界中的相机中, 物体在哪里被聚焦是由透镜距离成像平面与聚焦平面这两个平面的距离所决定的。当你改变对焦设置时, 相机中的这个透镜位置就会发生改变(你手机上的摄像头也是这个原理, 只不过透镜不动, 改成了成像传感器动)。快门光圈(aperture)是一个孔, 它控制这块透镜应该多大比较好。如果你需要更多的光线, 你的这个快门光圈就大一点, 景深也会随之加大。对于一个虚拟的摄像机来说, 我们只需要一个传感器就够了。所以我们只需要传入快门光圈的大小就行【即透镜大小】。

现实世界中的摄像机的透镜组是很复杂的。但对于我们写代码来说, 我们只需要模拟上述的顺序: 图像传感器, 透镜, 快门, 然后射出光线, 最后记得翻转图片(进过透镜成像会被上下翻转)。图形学中人们常常使用一块薄片透镜近似模拟:

![img](.\img\v2-08cf2fde8d40da8c84f80f9fec833a0e_720w.webp)

摄像机透镜模型

但是我们根本不用模拟任何摄像机内部的东西, 对于我们渲染摄像机外的物体来说, 这些都没必要。我们只要从一个虚拟的透镜范围中发射光线到我们的摄像机平面就能模拟了,这个透镜与平面的距离成为焦距(focus_dist)

![img](.\img\v2-5d7653d4521c1fff2fde0d4195a0caaa_720w.webp)

摄像机平面

之前我们所有的光线都是从lookfrom发出的, 但现在加入了散焦模糊, 所有光线都从内部的一个虚拟透镜发出, 经过lookfrom点, 这个透镜的半径越大, 图像就越模糊。你可以认为之前的摄像机, 这个半径为0。

```cpp
//vec3.h 从一个单位小圆盘射出光线
vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
```

下面给出完整的camera类

```cpp
class camera {
    public:
        camera(
            vec3 lookfrom, vec3 lookat, vec3 vup,
            double vfov, // top to bottom, in degrees
            double aspect, double aperture, double focus_dist
        ) {
            origin = lookfrom;
            lens_radius = aperture / 2;

            auto theta = degrees_to_radians(vfov);
            auto half_height = tan(theta/2);
            auto half_width = aspect * half_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;

            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }

        ray get_ray(double s, double t) {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset
           );
        }

    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};
```

我们使用一个大大的快门光圈:

```cpp
//main.cc
const auto aspect_ratio = double(image_width) / image_height;
...
vec3 lookfrom(3,3,2);
vec3 lookat(0,0,-1);
vec3 vup(0,1,0);
auto dist_to_focus = (lookfrom-lookat).length();
auto aperture = 2.0;

camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
```

就有:

![img](.\img\v2-8d5daba641a0c74f8af90cd9b47f9e42_720w.webp)

加入景深效果

## 13. 接下来学什么？

首先我们把书的封面图——许多许多的随机球渲染出来:

```cpp
//main.cc
hittable_list random_scene() {
    hittable_list world;

    world.add(make_shared<sphere>(
        vec3(0,-1000,0), 1000, make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<lambertian>(albedo)));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

    world.add(
        make_shared<sphere>(vec3(-4, 1, 0), 1.0, make_shared<lambertian>(vec3(0.4, 0.2, 0.1))));

    world.add(
        make_shared<sphere>(vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return world;
}

int main() {
    ...
    auto world = random_scene();

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    ...
}
```

我们会得到:

![img](.\img\v2-7483e528431ca10622ddd31ce8ebbba9_720w.webp)

最终场景

你可能会发现玻璃球没有阴影, 使得他们看上去像漂浮在空中似得。这不是bug(你在现实世界中很少有机会见到真正的玻璃球, 它们看起来的确就是这样的)。玻璃球下的那个作为地板的大球仍然能被那么多光线击中, 因为光线并不会被玻璃球阻挡，经由玻璃球的折射最终射向天空【the sky is re-ordered rather than blocked. 

现在你拥有一个coooool毙了的光线追踪器了! 那接下来我该何去何从呢?【标*为[下本书](https://link.zhihu.com/?target=https%3A//oxine.github.io/Graphic/Ray-tracing-the-next-week/)中的内容】

1.光照。你可以使用阴影光线来显式实现这部分, 也可以使用产生光线的材质来隐式实现*。

2.偏移散射光线, 然后降低这些光线的权重来消除偏移。这两种都行。硬要说的话, 我偏向后者一点点。【我猜这句话是在说消除自相交所导致的阴影 即Shadow Ance, 如果有人知道这是在说什么请教教我吧！】

3.加入三角形。大部分模型都是三角网格。模型的IO部分是最恶心的, 基本上所有人都不想自己写, 都去找别人的代码用。

4.表面纹理*。这可以让你像贴墙纸一样把图片贴到物体上去。实现起来也很简单。

5.固体纹理*。可以参见Ken Perlin的在线代码, Andrew Kensler的blog中也有关于这部分的信息。

6.体积体(volumes 即雾等)*与其他介质。很Cool, 但是会改变你的代码构筑。我喜欢把体积体也设计成hittable的子类, 根据其密度来随机决定光线是否与其相交。使用这个方法, 你的渲染器甚至不用知道你渲的是体积体就渲出来了。

7.并行优化。使用不同的随机种子, 把你的代码复制上N份跑在N个核心上,然后再求平均值。你可以分层来完成这部分工作, 比如分成N/2对, 每次平均求出N/4的图片, 然后在对这些对之间求平均值。这应该用不了多少代码【[试试CUDA吧](https://link.zhihu.com/?target=https%3A//devblogs.nvidia.com/accelerated-ray-tracing-cuda/)】。

