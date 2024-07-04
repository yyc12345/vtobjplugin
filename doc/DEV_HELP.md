# Develop Help

## Encoding System

The core of this dumper, `obj_export.cpp` use different encoding processing strategy with the operating system due to some feature implementations. There are the detail of this core's mechanism.

Virtools use multibyte encoding system related to current system. Stored CKObject name is rely on document's author's encoding(code page).

For model name, we call `CKObject.GetName()` first, then do some operations(name prefix and eliminating non-ASCII char) without considering this name's encoding(`obj_export::GenCKObjectName` done). Then, processed value will be delivered to `obj_export::GenObjMtlName` to generate the name which will be written in file with proper encoding. Also, this value will be transported to `obj_export::OpenObjMtlFile` which convert value into `wchar_t` type and opening corresponding model files via `_wfopen`.

For texture file path, `obj_export::GenCKTextureName` generated the filename of texture file with `wchar_t` type first. Then `GenCKTextureName4File` generated filename used in model files. `CopyTextureFile` also use returned value to saving texture file. The detail is that call `CKTexture.SaveImage()` first and save texture file with proper file suffix(due to Virtools use file extension name to recognize the format of saved files) into temp folder. Then call `MoveFileW` and use `wchar_t` path value to move it into correct path. The reason why use this non-straight method is that `CKTexture.SaveImage()` do not support UTF8 encoding and also do not support `wchar_t` path. So we need a pure ASCII path to prevent any encoding problem.

Custom Encoding configuration only provide a chance to user that allow exporter to read multibyte string with correct encoding, otherwise exporter will assume this multibyte string is based on current Windows code page and process it directly. If Virtools composition contain character which can not be decoded in current Windows code page, exporter will throw unhandled exception. This is the problem of user, not the exporter, because user do not select correct encoding. In code, we assume that user know what they are doing.

## Bump version

* Dialog left-bottom corner version string
* About dialog in `vt_menu.cpp`
* Macro `APP_VERSION` in `config_manager.cpp` if config file structure has been changed when bumping version.
