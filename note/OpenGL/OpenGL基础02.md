# OpenGL渲染技巧

## 课程目标：

**1.渲染过程产生的问题。**
 **2. 油画渲染。**
 **3. 正面 & 背面剔除。**
 **4. 深度测试。**
 **5. 多边形模型。**
 **6. 多边形偏移。**
 **7. 裁剪。**
 **8. 颜色混合。**

## 在渲染过程中可能产生的问题

**在绘制3D场景的时候，我们需要决定哪些是对观察者可见的，或者哪些部分是对观察者不可见的。对于不可见的部分，应该及早丢弃。例如在一个不透明的墙壁后，就不应该渲染。这种情况叫做“隐藏面消除"
 (Hidden surface elimination).**

![img](https:////upload-images.jianshu.io/upload_images/8804372-90ead7405db5d779.png?imageMogr2/auto-orient/strip|imageView2/2/w/836/format/webp)

隐藏面未消除效果图.png



### 解决方案：油画算法

- 油画算法

  - 先绘制场景中的离观察者比较远的物体，再绘制较近的物体。

  - 例如下面的图例：先绘制红色部分，在绘制黄色部分，最后再绘制灰色部分，即可解决隐藏面消除的问题。

    ![img](https:////upload-images.jianshu.io/upload_images/8804372-64d9e0739d3615e3.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

    油画法.png

- 油画法弊端

  - 使用油画算法，只要将场景按照物理距离观察者的距离远近排序，由远及近绘制即可。那么会出现什么问题？如果三个三角形是叠加的情况，油画算法将无法处理。

    ![img](https:////upload-images.jianshu.io/upload_images/8804372-d7ff7e848913db60.png?imageMogr2/auto-orient/strip|imageView2/2/w/686/format/webp)

    三张图片叠加.png

### 解决方案：正背面剔除(Face Culling)

- 背景
  - 尝试相信一个3D图形，你从任何一个方向去观察，最多可以看到几个面？
  - 答案是最多3面。从一个立方体的任意位置和方向上看，你用过不可能看到多于3个面。
  - 那么思考？我们为何要多余的去绘制那根本看不到的3个面？
  - 如果我们能以某种方式去丢弃这部分数据，OpenGL在渲染得性能上课提高50%。
- 解决问题
  - 如何知道某个面在观察者的视野中不会出现？
  - 任何平面都有2个面，正面/背面。意味着你一个时刻只能看到一面。
  - OpenGL 可以做到检查所有正面朝向观察者的画，并渲染它们。从而丢弃背面朝向的面。这样可以节约片元着色器的性能。
  - 如何告诉OpenGL你绘制的图形，那个面是正面的，那个是背面？
  - 答案：通过分析顶点数据顺序。

### 解决方案：分析顶点顺序

![img](https:////upload-images.jianshu.io/upload_images/8804372-8b1889ec0971a5bd.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

分析顶点顺序.png

- 正面/背面区分
  - 正面：按照逆时针顶点连接顺序的三角形面。
  - 背面：按照顺时针顶点连接顺序的三角形面。

### 解决方案：分析立方体中的正背面

![img](https:////upload-images.jianshu.io/upload_images/8804372-a3fbce30098db52b.png?imageMogr2/auto-orient/strip|imageView2/2/w/1114/format/webp)

分析立方体中的正背面.png

- 分析
  - 左侧三角形顶点顺序为：1 —> 2 —> 3; 右侧三角形的顶点顺序为：1 —> 2 —> 3.
  - 当观察者在右侧时，则右边的三角形方向为逆时针方向则为正面，而左侧的三角形为顺时针则为背面。
  - 当观察者在左侧时，则左边的三角形方向为逆时针方向则为正面，而右侧的三角形为顺时针则为背面。
- 总结
  - 正面和背面是由三角形的顶点定义顺序和观察者方向共同决定的。随着观察者的角度方向的改变，正面背面也会跟着改变。
- 开启表面剔除(默认背面剔除)
   void glEnable(GL_CULL_FACE)
- 关闭表面剔除(默认背面剔除)
   void glDisable(GL_CULL_FACE)
- 用户选择剔除那个面(正面/背面)
   void glCullFace(GLenum modes)
   mode参数为：GL_FRONT,GL_BACK,GL_FRONT_AND_BACK,默认GL_BACK
- 用户指定绕序那个为正面
   void glFrontFace(GLenum mode)
   mode 参数为：GL_CW,GL_CCW,默认值：GL_CCW
- 例如：剔除正面实现(1)
   glCullFace(GL_BACK);
   glFrontFace(GL_CW);
- 例如：剔除正面实现(2)
   glCullFace(GL_FRONT);

## 了解深度

- 什么是深度？
  - 深度其实是该像素点在3D世界中距离摄像机的距离， Z值。
- 什么是深度缓冲区？
  - 深度缓冲区，就是一块内存区域，专门存储着每个像素点(绘制在屏幕上的)深度值。深度值(z值)越大。则离摄像机越远。
- 为什么需要深度缓冲区？
  - 在不使用深度测试的时候，如果我们先绘制一个距离比较近的物体，在绘制距离较远的物体，则距离远的位图因为后绘制，会把距离近的物体覆盖掉。有了深度缓冲区后，绘制物体的顺序就不那么重要了。实际上，只要存在深度缓冲区，OpenGL都会把像素的深度值写入到缓冲区中。除非调用glDepthMask(GL_FALSE).来进制写入。

### 解决方法：z-buffer方法（深度缓冲区Depth-buffer）

- 深度测试
  - 深度缓冲区(DepthBuffer)和颜色缓冲区(ColorBuffer)是对应的。颜色缓冲区存储像素的颜色信息，而深度缓冲区存储像素的深度信息。在决定是否绘制一个物体表面时，首先要将表面的像素的深度值与当前深度缓冲区中的值进行比较。如果大于深度缓冲区的值，则丢弃这部分。否则利用这个像素对应的深度值和颜色值。分别更新深度缓冲区和颜色缓冲区。这个过程为“深度测试”。
- 深度值计算
  - 深度值一般由16位，24位或者32位值表示，通常是24位。位数越高，深度的精度越好。深度值的范围在[0,1]之间，值越小表示越接近观察者，值越大表示越远离观察者。
- 深度缓冲区主要是通过计算深度值来比较大小，在深度缓冲区中包含深度值介于0.0和1.0之间，从观察者看到其内容与场景中所有对象的z值进行比较。这些视图空间中的z值可以在投影平头截体的近平面和远平面之间的任何值。我们因此需要一些方法来转换这些视图空间z值到[0,1]的范围内，下面的（线性）方程把z值换为0.0 和 1.0之间的值；

![img](https:////upload-images.jianshu.io/upload_images/8804372-284200957d84355b.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

深度值计算.png

### 非线性深度缓存

- 方程带内锥截体的深度值z,并将其转换到[0, 1]范围。在下面的图给出 z 值和其相应的深度值的关系：

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-4a4387ff5409790a.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

- 在实践中是可以减少使用这样的线性深度缓冲区。正确的投影特性的非线性深度方程是和1/z成正比的，由于非线性函数和1/z成正比，例如1.0和2.0之间的z值，将变成1.0到0.5之间，这样在z非常小的时候给了我们很高的精度。方程式如下所示：

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-ce0a6423e7c210ca.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

- 要记住的重要一点是在深度缓冲区的值不是线性的屏幕空间（它们在视图空间投影矩阵应用之前是线性）。值为0.5在深度缓冲区并不意味着该对象的z值是投影平头载体的中间；顶点的z值是实际上相当接近平面。你可以看到z值和产生深度缓冲区的值在下图中的非线性关系

![img](https:////upload-images.jianshu.io/upload_images/8804372-4090747144c16d35.png?imageMogr2/auto-orient/strip|imageView2/2/w/1190/format/webp)

### 使用深度测试

- 深度缓冲区，一般由窗口管理系统，GLFW创建。深度值一般由16位，24位，32位值表示。通常是24位。位数越高，深度精度越好。

- 开启深度测试
   glEnable(GL_DEPTH_TEST);

- 在绘制场景前，清除颜色缓存区，深度缓冲
   glClearColor(0.0f,0.0f,0.0f,1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

- 清除深度缓冲区默认值为1.0，表示最大的深度值，深度值范围为(0,1)之间。值越小表示越接近观察者，值越大表示越远离观察者。

- 指定深度测试判断模式
   void glDepthFunc(GLEnum mode);

- 打开/阻断深度缓存区写入
   void glDepthMask(GLBool value);
   value : GL_TURE 开启深度缓冲区写入；GL_FALSE 关闭深度缓冲区写入；

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-9c841960fc4f8e75.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

## 使用正面/背面剔除和深度测试法来解决渲染效率问题。

### ZFighting 闪烁问题的原因

- 因为开启深度测试后，OpenGL就不会再去绘制模型被遮挡的部分。这样实现的显示更加真实。但是由于深度缓冲区精度的限制对于深度相差非常小的情况下。（例如在同一个平面上进行2次绘制），OpenGL就可能出现不能正确判断两者的深度值，会导致深度测试的结果不可预测。显示出来的现象交错闪烁。

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-08c4ad100b5af957.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

### ZFighting 闪烁问题的解决

- 第一步：启用Polygon Offset 方式解决

  - 解决方法：让深度值之间产生间隔。如果2个图形之间有间隔，是不是意味着就不会产生干涉。可以理解为在执行深度测试前将立方体的深度值做一些细微的增加。于是就能将重叠的2个图形深度值之前有所区分。
  - 启用Polygon Offset方式
     glEnable(GL_POLOYGON_OFFSET_FILL)

  参数列表：
   GL_POLYGON_OFFSET_POINT  对应光栅化模式：GL_POINT
   GL_POLYGON_OFFSET_LINE     对应光栅化模式：GL_LINE
   GL_POLYGON_OFFSET_FILL      对应光栅化模式：GL_FILL

- 第二步：指定偏移

  - 通过glPolygonOffset 来指定。glPolygonOffset需要2个参数：factor,units
  - 每个Fragment的深度值都会增减如下所示的偏移量：
     offset = (m * factor) + (r * units)
     m:多边形的深度的斜率的最大值，理解一个多边形越是与裁截平行，m就越接近于0.
     r:能产生于窗口坐标系的深度值中可分辨的差异最小值。r是由具体OpenGL平台指定一个常量。

- 一个大于0的Offset会把模型推到离你（摄像机）更远的位置，相应的一个小于0的Offset会把模型拉近。

- 一般而言，只需要将-1.0和-1这样简单赋值给glPolygonOffset 基本可以满足需求。

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-ba36fe11031b201f.png?imageMogr2/auto-orient/strip|imageView2/2/w/1058/format/webp)

- 第三步：关闭Polygon Offset
   glDisable(GL_POLYGON_OFFSET_FILL)

### ZFighting闪烁问题预防

- 不要将两个物体靠的太近，避免渲染时三角形叠在一起，这种方式要求对场景中插入一个少量的偏移，那么就可能避免ZFighting现象。例如上面的立方体和平面问题中，将平面下移0.001f 就可以解决这个问题。当然手动去插入这个小的偏移是要付出代价的。
- 尽可能将近裁剪面设置的离观察者元一些，上面我们看到，在近裁剪平面附近，深度的精确度是很高的，因此尽可能让近裁剪面元一些的话，会使整个裁剪范围内的精确度搞一些。但是这种方式会使离观察者较近的物体被裁剪掉，因此需要调试好裁剪面参数。
- 使用更高位数的深度缓冲区，通常使用的深度缓冲区是24位的，现在有的一些硬件使用32位的缓冲区，是精确度得到提高。

## 裁剪

在OpenGL中提高渲染得一种方式，只刷新屏幕上发生变化的部分。OpenGL允许将要进行渲染得窗口去指定一个裁剪框。
 基本原理：用于渲染时限制绘制区域，通过此技术可以在屏幕（帧缓冲）指定一个矩形区域。启用裁剪测试之后，不在此矩形区域内的片元被丢弃，只有在此矩形区域内的片元才有可能进入帧缓冲。因此实际达到的效果就是在屏幕上开辟了一个小窗口，可以在其中进行制定内容的绘制。

- 1.开启裁剪测试
   glEnable(GL_SCISSOR_TEST);
- 2.关闭裁剪测试
   glDisable(GL_SCISSOR_TEST);
- 3.指定裁剪窗口
   void glScissor(Glint x, Glint y, GLSize width, GLSize height);
   x,y:指定裁剪框左下角位置；
   width,height:指定裁剪尺寸

## 理解窗口，视口，裁剪区域

- 窗口：就是显示界面

- 视口：就是窗口中用来显示图形的一块矩形区域，它可以和窗口等大，也可以比窗口大或者小。只有绘制在视口区域中的图形才能被显示，如果图形有一部分超出了视口区域，那么那一部分是看不到的。通过glViewport()函数设置。

- 裁剪区域（平行投影）：就是视口矩形区域的最小最大x坐标（left,right）和最小最大y坐标（bottom,top），而不是窗口的最小最大x,y坐标。通过glOrtho()函数设置，这个函数还需要指定最近最远z坐标，形成一个立体的裁剪区域。

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-e7bb4839b8f53dea.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

  ## 混合

OpenGL渲染时会把颜色值存在颜色缓冲区中，每个片段的深度值也是存放在深度缓冲区中。当深度缓冲区被关闭时，新的颜色将简单的覆盖原来的颜色缓冲区的颜色值，当深度缓冲区再次打开时，新的颜色片段只是当它们比原来的值更接近裁截平面才会替换原来的颜色片段。

- 开启颜色混合
   glEnable(GL_BLEND);

### 组合颜色

目标颜色：已经存储在颜色缓存区的颜色值。
 源颜色：作为当前渲染命令结果进入颜色缓存区的颜色值。
 当混合功能被启动时，源颜色和目标颜色的组合方式值混合方程式控制的。在默认情况下，混合方程式如下所示：
 Cf = （Cs *S） + (Cd * D);
 Cf:最终计算参数的颜色
 Cs:源颜色
 Cd:目标颜色
 S:源混合因子
 D:目标混合因子

### 设置混合因子

- 设置混合因子，需要用到glBlendFun函数
   glBlendFunc(GLenum S, GLenum D);

  ![img](https:////upload-images.jianshu.io/upload_images/8804372-152611fc2162c033.png?imageMogr2/auto-orient/strip|imageView2/2/w/1074/format/webp)

  表中的R、G、B、A分别表示红、绿、蓝、alpha。
   表中下标S、D,分别代表源、目标。
   表中C代表常量颜色（默认黑色）。

#### 下面通过一个常见的混合函数组合来说明问题：

glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

如果颜色缓存区已经有一种颜色红色（1.0f ,0.0f ,0.0f ,0.0f）,这个目标颜色cd,如果在这个上面用一种alpha为0.6的蓝色（0.0f, 0.0f, 1.0f, 0.6f）

Cd(目标颜色) = (1.0f ,0.0f ,0.0f ,0.0f);
 Cs(源颜色) = （0.0f, 0.0f, 1.0f, 0.6f）；
 S = 源alpha值 = 0.6f
 D = 1 - 源alpha值 = 0.4f
 方程式 Cf = (Cs * S) + (Cd * D);
 等价于 = （Blue * 0.6f）+ (Red * 0.4f)

### 总结

最终颜色是以原先的红色（目标颜色）与 后来的蓝色（源颜色）进行组合。源颜色的alpha值越高，添加的蓝色颜色成分越高，目标颜色所保留的成分就会越少。混合函数经常用于实现在其他一些不透明的物体前面绘制一个透明物体的效果。

### 改变组合方程式

默认混合方程式：Cf = (Cs * S) + (Sd * D)
 实际上远不止这一种混合方式，我们可以从5个不同的方程式中进行选择： glbBlendEquation(GLenum mode);

![img](https:////upload-images.jianshu.io/upload_images/8804372-e2356aae69c700f9.png?imageMogr2/auto-orient/strip|imageView2/2/w/1102/format/webp)

除了能使用glBlendFunc来设置混合因子，还可以有更灵活的选择。
 void glBlendFuncSeparate(GLenum strRGB, GLenum dstRGB, GLenum strAlpha, GLenum dstAlpha);
 strRGB:源颜色的混合因子
 dstRGB:目标颜色的混合因子
 strAlpha:源颜色的Alpha因子
 dstAlpha:目标颜色的Alpha因子

- glBlendFunc指定源和目标RGBA值的混合函数；但是glBlendFuncSeparate函数则允许为RGB和Alpha成分单独指定混合函数。
- 在混合因子表中 GL_CONSTANT_COLOR, GL_MINUS_CONSTANT_COLOR,GL_CONSTANT_ALPHA,GL_ONE_MINUS_CONSTANT值允许混合方程式中引入一个常量混合颜色。
- 常量混合颜色，默认初始化为黑色（0.0f, 0.0f, 0.0f, 0.0f）,但是还是可以修改这个常量混合颜色。
   void glBlendColor(GLclampf red, GLclampf green, GLclampf blue ,GLclampf alpha);