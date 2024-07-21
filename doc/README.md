# vtobjplugin

Virtools Wavefront OBJ Export Plugin.

An interface plugin of Virtools which allows you to export some models as Wavefront OBJ format.

Support Virtools 3.0, 3.5, 4.0 and 5.0

## Features

### Advantages

* Can ignore object transform (i.e. keep the origin of object. better than vt2obj. Ignoring object transform will keep the origin of object but remove translation, rotation and scale of object).
* Support Blender and 3ds Max's reposition script for placing object (Re-place each object with their original transforms in 3D software when you check Ignore Object Transform)
* Better right-hand left-hand coordinate system conversion (From fliping Z axis to swaping YZ axis. This is more suit for Blender and 3ds Max 3d system).
* More material export options.
* Comparing with vt2obj, it support more Virtools versions. It is convenient for Ballance and other games to export 3D objects.
* Support *One File per Object*. It is convenient for exporting object components.

### Shortcomings

* Right-hand left-hand coordinate system conversion only supports swaping YZ axis.
* No smooth group.
* Only can export 3D object. Doesn't support 2D structure such as Line and etc.

## Usage

1. Put downloaded plugin DLL into Virtools's `InterfacePlugins` folder
1. Launch Virtools and open the document which you want to export objects from.
1. Click menu`Wavefront OBJ`-`Export Objects`。
1. Set proper settings in dialog and click OK to export objects.

## Export Settings Manual

### General Settings

* Export Mode: `An Object`, `A Group`, `Selected Objects` and `All`. For `An Object` and `A Group`, you should pick an item in following Combobox to export.
* File Mode: `All Objects in One File` and `One File per Object`. For the first option, file will be named as `all`. For the second option, file name is object name. This option only affect the count of OBJ and MTL files.
* Export Directory：It should be an existing folder. And you should pay attention to that the conflict file will be overwritten directly without any notification.

### Object Settings

* Split Object by: Use `g` (group statement) or `o` (object statement) to split different objects in one OBJ file.
* Ignore Object Transform: Check suggested. This option is only served for the compability with original `vt2obj`.
* Convert to Right Hand Coordinate System: Convert to Blender and 3ds Max's coordinate system.
* Add Name Prefix: Add an unique name prefix to try avoiding possible name conflict.
* Eliminate Non-ASCII Characters: Eliminate all non-ASCII characters in exported files, including object names and texture files. This may be useful in place where OBJ importer couldn't correctly process encoding.

### Reposition Settings

If you haven't check `Ignore Object Transform` and `Convert to Right Hand Coordinate System`, this sector's options are disabled.

Repostion means that restore the original transform of objects, including translation, rotation and scale.

* Generate 3ds Max Reposition Script (3dsmax.ms)
* Generate Blender Reposition Script (blender.py)

### Material Settings

* Export Material: Whether exporting MTL files. If you don't check it, the following options will be disabled.
* Export Texture: Whether including texture in MTL files. If you don't check it, all textures of object' will be removed and only keep its color properties such as ambient, diffuse and etc, and following options will be disabled.
* Copy Texture File: Copy texture to your export folder. If you have existing texture folder, you can uncheck this option to reduce the usage of storages and IO operations.
* Custom Texture Format: If you check this and write a new extension name, all texture will be change to new extension name you given. Please you don't need to enter dot (`.`) at the beginning of this setting.

### Encoding Settings

* Composition Encoding (aka. code page)
  - System: Use system default encoding. Choose this if you want exporter to keep the same behavior with vtobjplugin 1.x version. This option also is suggested to be checked when the Virtools document is not garbled.
  - Custom: Give user a chance to let the exporter read the object names correctly. Virtools uses a system-dependent multibyte encoding. Therefore, when the author of the Virtools document and the reader (you) have different system encodings, and the author uses non-ASCII characters in the document, the document will appear garbled. For example, Ballance uses Western European encoding to encode `A01_Geländer_01`, then under some other encodings, there will be garbled characters. In this case, if you select this option and fill in `28591` ([Windows code page](https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers)), the characters in document can be read correctly and output correct exported files. 
* Use UTF8 in Object and Material File: Use UTF8 encoding instead of system encoding in OBJ and MTL files. This and next setting are typically used along with Custom option of Composition Encoding setting. 
* Use UTF8 in Script File: Use UTF8 encoding in reposition script files.

## Export Import Tips

Since the encoding settings of exporter are complex, and the exported models are mainly used for importing in 3ds Max and Blender, There are export and import tips for 3ds Max and Blender respectively for your reference.

### Export

The settings written in there are based on the default settings (you can click Reset button in dialog to get it). Please keep default settings if the options are not written here unless you know what are you doing.

#### Blender

* Set Composition Encoding if reuqired
* Check `Use UTF8 in Object and Material File`
* Check `Use UTF8 in Script File`
* Check `Generate Blender Reposition Script (blender.py)`

> Blender and Blender's OBJ importer assume that model and script file are written in UTF8 encoding. So the file needs to be output in UTF8 mode, and at the same time there is no need to eliminate non-ASCII characters as Blender can handle them correctly.

#### 3ds Max

* Set Composition Encoding if reuqired
* Check `Use UTF8 in Script File` according to your requirements. View following notes for more detail
* Check `Eliminate Non-ASCII Characters`
* Check `Generate 3ds Max Reposition Script (3dsmax.ms)`

> 3ds Max's default OBJ importer does not recognize non-ASCII characters and will converts them to `_`(underline). Therefore, the generated script does not match the name of the object inside the model file name, and an error occurs when executing the script. So we need to eliminate non-ASCII characters when exporting. At the same time, the MAXScript executor of the old version of 3ds Max can only recognize UTF8 documents with BOM. Any other files with different encoding will let script become garbled. So it needs to use UTF8 to output script files.

> There are 2 different 3ds Max MAXScript executing strategy. If you want to use *MAXScript - Run script* to execute script, you should not check `Use UTF8 in Script File`. However, if you want to use *MAXScript - Open script* to open script file first and use *Tools - Evaluate all* to execute script in opened window, you should check `Use UTF8 in Script File`

### Import

Following tips are suit for OBJ and MTL files exported under suggested export setting (the settings introduced before).

#### Blender

* Choose Y forward, Z up (same as Blender default system)
* Check `Split by Group` or `Split by Object` according to your chosen object split mode.

For reposition script, load script file `blender.py` in Scripting tab and run it.

#### 3ds Max

* Do **NOT** check Flip YZ Axis
* For normals, select Imported from File
* Check Import Material

For reposition, click *MAXScript-Run* script in menu. Select `3dsmax.ms` and run it, or, click *MAXScript - Open script* to open script file first and use *Tools - Evaluate all* to execute script in opened window. This is rely on your chosen solution said in Export Tips section.

## Build

Before building this project, you should prepare essential libraries:

* [YYCC](https://github.com/yyc12345/YYCCommonplace): A homemade library which vastly used by my personal project.
* Virtools SDK: Virtools SDK usually distributed with Virtools self. If your SDK is invididual from Virtools installation, please put it inside Virtools because this project refer it by the root directory of Virtools.

Next, you should have Python 3 and execute command `python "script\setup_env.py" -p <VirtoolsPath> -y <YYCCPath>` at the root direcotry of this project. `<VirtoolsPath>` in command should be replaced by the path to the root directory of Virtools. And `<YYCCPath>` in command should be replaced by the install path of YYCC library (it's CMake install path, not the repository self of YYCC. It means that you should build YYCC library first). This command will configure build environment.

Then, you can open the project in Visual Studio and choose `Release` configuration to build this plugin. The built plugin will be placed in Virtools's `InterfacePlugins` folder automatically.
