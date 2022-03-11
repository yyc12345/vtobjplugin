# Develop Help

## Encoding System

The core of this dumper, `obj_export.cpp` use different encoding system with the operating system due to some feature implementations. There are the detail of this core's mechanism.

Virtools use multi-byte encoding system related to current system. So after executing `CKObject.GetName()`, we should use Win32 function converte it into UTF-8 encoding. Then, in the entire processing of `obj_export.cpp`, we treat all string as UTF-8 style with one exception, the creation of file. For example, creating obj/mtl files and copying texture file. The reason of this decision is that Win32 function do not support UTF8 input and Virtools also do not support UTF8 due to historical reason. The solution is that we should use Win32 function ended with `W` to creating obj/mtl file because obj/mtl file name have potitial to containg non-ASCII character and it's name also can not be decode in current Windows code page. As for the copying of texture file, we call `CKTexture.SaveImage()` first to save it in the path which can be decoded in current Windows code page(Windows temp folder), then call Win32 copy function ended with `W` to copy it into our target path.

Custom Encoding configuration only provide a chance to user that allow exporter to read multi-byte string with correct encoding, otherwise exporter will assume this multi-byte string is based on current Windows code page and process it directly. If Virtools composition contain character which can not be decoded in current Windows code page, exporter will throw unhandled exception. This is the problem of user, not the exporter, because user do not select correct encoding. In code, we assume that user know what they are doing.


## Bump version

* Dialog left-bottom corner version string
* About dialog in `vt_menu.cpp`
* Macro `APP_VERSION` in `config_manager.cpp` if config file structure has been changed when bumping version.
