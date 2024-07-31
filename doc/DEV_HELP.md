# Developer Help

## Basic Steps

1. First we create an instance of `ExportSettingDialog`, open a dialog and order user input export settings. Dialog read config manager settings when loading, and save settings to config manager after user click OK and input has been validated.
1. Fetch `ExportSetting` from dialog, create `ExportLayoutWeaver` instance and send into it. `ExportLayoutWeaver` will collect all 3D objects, meshes, materials and textures exported and give them an unique name according to `ExportSetting`.
1. Create `ObjExport` instance and fetch collected infomations in `ExportLayoutWeaver` to do read export. `ObjExport` will create essential files and write `ExportLayoutWeaver` collected infomations into it.

## Encoding System

The object name encoding convertion is located in `ExportLayoutWeaver`. Virtools use multibyte encoding system related to current system. Stored CKObject name is rely on document's author's encoding (code page). However, the whole program use UTF8 as default encoding. Thus `ExportLayoutWeaver` will try to convert CKObject name encoded by multibyte encoding system, to UTF8 according to user given encoding strategy.

For texture file path, it is done in both `ExportLayoutWeaver` and `ObjExport`. First `ExportLayoutWeaver` will try get texture slot file name and convert it to UTF8 according to user given encoding. Then it is passed to `ObjExport`. In `ObjExport`, we firstly generate a ASCII-only name first (keep the same extension format). And use `CKTexture.SaveImage()` to save it in Windows Temp directory. The use Win32 function `MoveFileW` to move it from Windows Temp folder to destination directory and rename it to the actual texture name at the same time. The reason why use this non-straight method is that `CKTexture.SaveImage()` do not support UTF8 encoding and also do not support `wchar_t` path. So we need a pure ASCII path to prevent any encoding problem.

## Bump Version

* Dialog left-bottom corner version string (all languages, current is English and Simplified Chinese).
* About dialog string in string table (also for all languages).
* Version info in resource file (also for all languages).
* Version indentifier passed to `YYCC::ConfigManager::CoreManager` constructor in `ConfigManager.cpp` if config file structure has been changed when bumping version.
