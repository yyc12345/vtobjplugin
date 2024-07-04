# vtobjplugin

[English document](README.md)

Virtools Obj 插件

一个Virtools界面插件，允许你以OBJ格式导出一些模型。

支持Virtools 3.5，4.0和5.0

## 特性

### 优点

* 可以选择忽略世界变换矩阵（较于vt2obj更优，消除世界变换矩阵即消除物体的移动，旋转和缩放变换）。
* 支持Blender和3ds Max的重定位脚本，方便复位（在使用忽略世界变换矩阵后方便在编辑软件中重新放置各类模型）。
* 支持更好的左右手坐标转换（从翻转Z轴改为交换YZ轴，更加符合Blender和3ds Max的轴向）。
* 增加更多材质导出选项。
* 较于vt2obj支持更多Virtools版本，方便导出。
* 支持每个物体一个文件，非常适合素材导出。

### 缺点

* 左右手翻转只能支持YZ轴翻转。
* 平滑组没有考虑。
* 只能导出3D物体，不能导出线等二维结构

## 操作方法

1. 将插件安装到Virtools的`InterfacePlugins`文件夹中。
1. 启动Virtools，加载需要导出的文档。
1. 根据需要，考虑是否需要做额外操作，比如只想导出路面或者某些物体，需要把相关物体归到一个新组中，或者不做任何操作，例如导出单个物体或全部物体。
1. 点击菜单栏Vtobj - Export object。
1. 弹出窗口中配置好各类参数，点击OK即可导出

## 导出选项解释

### General Setting

* Export mode：导出模式，从`An object`（单个物体），`A group`（一个组里的所有物体）和`All`（所有物体）中选择一个。其中前两个选项需要再点击下面的下拉框选择物体或组。
* File mode：文件模式`All objects in one file`（所有物体在一个文件里）和`One file per object`（一个物体一个文件）。前者文件名将命名为`all`，后者文件名为物体名称。此选项只影响obj和mtl文件个数。
* Export folder：导出文件夹，需要是已存在的文件夹。导出中如若碰到同名文件会直接覆盖，需要注意。

### Object Setting

* Split object by: 决定是使用`g`（Group）语句还是`o`（Object）语句来划分物体。
* Omit object transform data：忽略世界变换矩阵，建议勾选。此选项仅仅是为了提供向vt2obj兼容的功能而存在。
* Convert to right-hand coordinate system：转为右手坐标系，特指Blender和3ds Max类型的右手坐标系。
* Add name prefix：增加唯一的名称前缀以规避名称冲突。
* Eliminate non-ASCII characters：消除文本中的所有非ASCII字符，在一些不能够正确处理编码的场合可能会有用。

### Reposition Script

如果`Object Setting`中`Omit object transform data`和`Convert to right-hand coordinate system`中的任意一项不勾选，那么下述两项将不可用。

* Generate 3ds Max script (3dsmax.ms)：生成3ds Max的重定位脚本
* Generate Blender script (blender.py)：生成Blender的重定位脚本

### Material Setting

* Export mtl file：导出mtl文件，如果不勾选，那么下述所有选项将不可用。
* Export texture map in mtl：在mtl中导出贴图文件，如果不需要贴图（即只保留光照等效果），可以不勾选。不勾选此选项下述所有选项将不可用。
* Copy texture file：将导出材质复制到输出目录，如果你已有材质文件列表，可以关闭此选项以节省空间和不必要的IO操作。
* Custom texture map format：自定义材质格式。目前由于SDK限制无法读取原有材质格式，如果不勾选并填入后缀那么保存的材质将不具有任何后缀名，这可能会在Windows 10以前的操作系统上引发错误。

### Encoding

* Composition encoding
  - System：使用系统的编码，选择此项即保持与1.x版本的导出器相同的行为。在文档没有出现乱码的情况下，也可以选择此项。
  - Custom：给予用户一个让导出器正确读取物体名称的机会。Virtools使用依赖于系统的多字节编码。因此当Virtools文档的作者与阅读者（你）的系统编码不相同，且作者恰好在文档中使用了非ASCII字符，那么文档将会出现乱码。例如，Ballance使用西欧编码编码了`A01_Geländer_01`，那么在一些其它编码下则会出现乱码，此时若选择此项并填入`28591`（[Windows代码页](https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers)）则可以正确读取字符并输出正确的文件。
* Use UTF8 in object file：在obj和mtl文件中使用UTF8编码，而不是本机编码。此项与下一项通常与自定义文档编码一起使用。
* Use UTF8 in script file：在脚本文件中使用UTF8编码。

## 导出提示

由于导出选项的编码部分比较复杂，并且导出的模型主要用于3ds Max以及Blender内的导入，因此在此处分别介绍适用于两个软件的导出设置中的编码部分的设置。此处的设置是基于对默认设置（即第一次使用此插件时的设置）的修改，如果没有陈述，则保持默认设置。

### Blender

* 设置Composition encoding，如果需要
* 勾选Use UTF8 in object file
* 勾选Use UTF8 in script file
* 勾选Generate Blender script

> Blender本身和Blender的obj导出插件默认物体文件和脚本文件是以UTF8进行呈现的。因此需要以UTF8模式输出文本，同时不需要消除非ASCII字符，因为Blender可以正确处理这些字符。

### 3ds Max

* 设置Composition encoding，如果需要
* Use UTF8 in script file，请根据情况勾选，详见之后的注释
* 勾选Eliminate non-ASCII characters
* 勾选Generate 3ds Max script

> 3ds Max的默认obj导入器不能识别非ASCII字符，会将其转换为下划线。所以导致生成的脚本和模型文件名内部物体的名称不匹配，进而在执行脚本时出错。因此我们需要在导出时就消除非ASCII字符。同时旧版本的3ds Max的脚本执行器只能识别带有BOM的UTF8文档并正确显示多语言文本，因此需要使用UTF8输出脚本文件。

> 3ds Max的两个MAXScript执行方法具有不同的文件读取策略。如果您想使用 MAXScript-运行脚本 执行脚本，那么您不可以勾选Use UTF8 in script file。如果您想使用先 MAXScript-打开脚本，然后在窗口中使用 工具-计算所有 来运行脚本，则可以勾选这个选项。

## 导入提示

此处就Blender和3ds Max导入做一些提示。此处的设置是以对默认设置（即按照上述导出提示使用此插件时的设置）为前提进行讲述的

### Blender

* 坐标轴选择Y forward, Z up（即与Blender默认坐标轴设置一致）
* 勾选Image search
* Split勾选Group

重定位在Scripting界面加载`blender.py`并执行即可

### 3ds Max

* 不要勾选翻转YZ轴
* 法线从文件导入
* 勾选导入材质

重定位在菜单栏选择 MAXScript-运行脚本，选择`3dsmax.ms`并执行即可，或先 MAXScript-打开脚本，然后在窗口中使用 工具-计算所有。根据您之前在导出提示中做出的选择。

## 编译

此项目有许多配置，分别对应Virtools 3.5，4.0和5.0三个版本以及他们各自的Debug和Release编译模式，需要各自的Virtools SDK才能编译。

如果您需要编译此项目，请先修改`vtobjplugin.props`中的宏定义，将其指向您的Virtools根目录（不加斜杠），然后在Visual Studio中选择合适的配置进行编译。
