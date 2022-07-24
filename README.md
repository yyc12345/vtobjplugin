# vtobjplugin

[中文文档](README_ZH.md)

Virtools Obj Plugin.

An interface plugin of Virtools which allows you to export some models as `.obj` file.

Support Virtools 3.5, 4.0 and 5.0

## Features

### Advantages

* Can omit world matrix (better than vt2obj. Omiting world matrix will omit object's move, rotation and scale).
* Support Blender and 3ds Max's repostion script for placing object (Place each object with the correct status in 3d software when you omit world matrix)
* Better right-hand left-hand coordinate system conversion. (From fliping Z axis to swaping YZ axis. This is more suit for Blender and 3ds Max 3d system)
* More material export options
* Comparing with vt2obj, it support more Virtools versions. It is convenient for Ballance object operation.
* Support *one file per object*. It is convenient for exporting object component.

### Shortcomings

* Right-hand left-hand coordinate system conversion only support swaping YZ axis.
* No smooth group
* Only export 3d object. Don't support 2 dimension structure such as Line and etc.

## Usage

1. Put plugin dll into Virtools's `InterfacePlugins` folder
1. Launch Virtools and open the document which you want to export objects from.
1. According to your need, considering whether you need do some extra operations.
1. Click menu`Vtobj`-`Export object`。
1. Set all options in dialog and click OK to export objects.

## Export config description

### General Setting

* Export mode: `An object`, `A group` and `All`. For `An object` and `A group`, you should pick a item in following Combobox.
* File mode: `All objects in one file` and `One file per object`. For the first option, file will be named as `all`. For the second option, file name is object name. This option only affect obj and mtl files' count.
* Export folder：It should be a existed folder. And you should pay attention to that the conflict file will be overwritten directly.

### Object Setting

* Split object by: Use `g` command(group) or `o` command(object) to split object.
* Omit object transform data: Suggest check. This option is only served for the compability with original `vt2obj`.
* Convert to right-hand coordinate system: Convert to Blender and 3ds Max's coordinate system.
* Add name prefix: Add a unique name prefix to avoid name conflict.
* Eliminate non-ASCII characters: Eliminate non-ASCII characters in exported files. This may be useful in the place where obj importer couldn't process encoding correctly.

### Reposition Script

If you don't check `Omit object transform data` or `Convert to right-hand coordinate system`, this sector's options are disabled.

Repostion means that restore object's original move, rotation and scale.

* Generate 3ds Max script (3dsmax.ms)
* Generate Blender script (blender.py)

### Material Setting

* Export mtl file: If you don't check it, the following options are disabled.
* Export texture map in mtl: If you don't check it, all object's texture will be omitted and only keep light effect, such as reflect rate. And following options are disabled.
* Copy texture file: Copy texture into your export folder. If you have existing texture folder, you can close this option to reduce the usage of storages and reduce useless IO operation.
* Custom texture map format: If you check this and write a new suffix, all texture will be change to new format.

### Encoding

* Composition encoding
  - System: Use system default encoding(aka. code page). Choose this if you want exporter to keep the same behavior with it's 1.x version. This option also can also be selected when the document is not garbled.
  - Custom: Give the user a chance to have the exporter read the object names correctly. Virtools uses a system-dependent multibyte encoding. Therefore, when the author of the Virtools document and the reader (you) have different system encodings, and the author uses non-ASCII characters in the document, the document will appear garbled. For example, Ballance uses Western European encoding to encode `A01_Geländer_01`, then under some other encodings, there will be garbled characters. In this case, if you select this option and fill in `28591` ([Windows code page](https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers)), the characters in document can be read correctly and output correct exported files. 
* Use UTF8 in object file: Use UTF8 encoding instead of local encoding in obj and mtl files. This and the next are typically used with Custom Composition Encoding option. 
* Use UTF8 in script file: Use UTF8 encoding in script files.

## Export tips

Since the export encoding options are more complicated, and the exported models are mainly used for import in 3ds Max and Blender, the export encoding settings applicable to the two softwares are introduced here. The settings here are based on the default settings (the initial settings when you first launch this plugin). Please keep default settings if the options are not written here.

### Blender

* Set Composition encoding, if needed
* Check Use UTF8 in object file
* Check Use UTF8 in script file
* Check Generate Blender script

> Blender and Blender's obj export plugin assume that model and script file are written in UTF8 encoding. So the file needs to be output in UTF8 mode, and at the same time there is no need to eliminate non-ASCII characters as Blender can handle them correctly.

### 3ds Max

* Set Composition encoding, if needed
* Check Use UTF8 in script file according to your requirements. View following notes for more detail
* Check Eliminate non-ASCII characters
* Check Generate 3ds Max script

> 3ds Max's default obj importer does not recognize non-ASCII characters and will converts them to `_`(underline). Therefore, the generated script does not match the name of the object inside the model file name, and an error occurs when executing the script. So we need to eliminate non-ASCII characters when exporting. At the same time, the MAXScript executor of the old version of 3ds Max can only recognize UTF8 documents with BOM. Any other files with different encoding will let script become garbled. So it needs to use UTF8 to output script files.

> There are 2 different 3ds Max MAXScript executing strategy. If you want to use *MAXScript - Run script* to execute script, you should not check *Use UTF8 in script file*. However, if you want to use *MAXScript - Open script* to open script file first and use *Tools - Evaluate all* to execute script in opened window, you should check *Use UTF8 in script file*

## Import tips

There are some tips for importing obj files generated by this plugin in 3ds Max and Blender. Following tips is under suggested export setting (the settings introduced before)

### Blender

* Choose Y forward, Z up (same as Blender default system)
* Check Image search
* Check Group in Split

For reposition, load script file `blender.py` in Scripting tab and run it.

### 3ds Max

* Do not check Flip YZ Axis
* For normals, select imported from file
* Check import material

For reposition, click *MAXScript-Run* script in menu. Select `3dsmax.ms` and run it, or, click *MAXScript - Open script* to open script file first and use *Tools - Evaluate all* to execute script in opened window. This is rely on your chosen solution said in Export Tips section.

## Build

This project has various configurations which corresponding with Virtools 3.5, 4.0 and 5.0. There are also 2 build mode Debug and Release for each individual Virtools versions. It need their corresponding Virtools SDK to build.

You should change some macros located in `vtobjplugin.props` before building this plugin. The macro should be pointed to your Virtools root path(without slash). Then, you can open the project in Visual Studio and choose the proper configuration to build this plugin.
