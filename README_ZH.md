# vtobjplugin

[English document](README.md)

Virtools Obj 插件

一个Virtools界面插件，允许你以OBJ格式导出一些模型。

支持Virtools 3.5和5.0

## 特性

### 优点

* 可以选择忽略世界变换矩阵（较于vt2obj更优，消除世界变换矩阵即消除物体的移动，旋转和缩放变换）。
* 支持Blender和3ds Max的重定位脚本，方便复位（在使用忽略世界变换矩阵后方便在编辑软件中重新放置各类模型）。
* 支持更好的左右手坐标转换（从翻转Z轴改为交换YZ轴，更加符合Blender和3ds Max的轴向）。
* 增加更多材质导出选项。
* 较于vt2obj支持Virtools 3.5，方便导出。
* 支持每个物体一个文件，非常适合素材导出。

### 缺点

* 左右手翻转只能支持YZ轴翻转。
* 为了避免命名重复，物体名会被加上一个固定前缀。
* 使用组来分割物体，并且不提供任何其他选项修改。
* 平滑组没有考虑。
* 只能导出3D物体，不能导出线等二维结构
* 不能探测贴图原始格式并自动以原始格式保存。

## 操作方法

1. 将插件安装到Virtools的InterfacePlugins文件夹中。
1. 启动Virtools，加载需要导出的文档。
1. 根据需要，考虑是否需要做额外操作，比如只想导出路面或者某些物体，需要把相关物体归到一个新组中，或者不做任何操作，例如导出单个物体或全部物体。
1. 点击菜单栏Vtobj-Export object。
1. 弹出窗口中配置好各类参数，点击OK即可导出

## 导出选项解释

### General Setting

* Export mode：导出模式，从`An object`（单个物体），`A group`（一个组里的所有物体）和`All`（所有物体）中选择一个。其中前两个选项需要再点击下面的下拉框选择物体或组。
* File mode：文件模式`All objects in one file`（所有物体在一个文件里）和`One file per object`（一个物体一个文件）。前者文件名将命名为`all`，后者文件名为物体名称。此选项只影响obj和mtl文件个数。
* Export folder：导出文件夹，需要是已存在的文件夹。导出中如若碰到同名文件会直接覆盖，需要注意。

### Object Setting

* Omit object transform data：忽略世界变换矩阵，建议勾选。此选项仅仅是为了提供向vt2obj兼容的功能而存在。
* Convert to right-hand coordinate system：转为右手坐标系，特指Blender和3ds Max类型的右手坐标系。

### Reposition Script

如果`Object Setting`中任意一项不勾选，那么下述两项将不可用。

* Generate 3ds Max script (3dsmax.ms)：生成3ds Max的重定位脚本
* Generate Blender script (blender.py)：生成Blender的重定位脚本

### Material Setting

* Export mtl file：导出mtl文件，如果不勾选，那么下述所有选项将不可用。
* Export texture map in mtl：在mtl中导出贴图文件，如果不需要贴图（即只保留光照等效果），可以不勾选。不勾选此选项下述所有选项将不可用。
* Copy texture file：将导出材质复制到输出目录，如果你已有材质文件列表，可以关闭此选项以节省空间和不必要的IO操作。
* Custom texture map format：自定义材质格式。目前由于SDK限制无法读取原有材质格式，如果不勾选并填入后缀那么保存的材质将不具有任何后缀名，这可能会在Windows 10以前的操作系统上引发错误。

## 导入提示

此处就Blender和3ds Max导入做一些提示

### Blender

* 坐标轴选择Y forward, Z up（即与Blender默认坐标轴设置一致）
* 勾选Image search
* Split勾选Group

重定位在Scripting界面加载`blender.py`并执行即可

### 3ds Max

* 不要勾选翻转YZ轴
* 法线从文件导入
* 勾选导入材质

重定位在菜单栏选择MAXScript-运行脚本，选择`3dsmax.ms`并执行即可

## 编译

此项目有两个配置，分别对应Virtools 3.5和5.0，需要各自的Virtools SDK才能编译运行。

项目编译配置中指定了很多绝对路径，初见者难以修改。

强烈不建议您自行编译，除非您对Virtools非常了解。
