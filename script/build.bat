@ECHO OFF
ECHO This script must be executed at the root of project. Press any key to continue...
PAUSE
python "script\setup_env.py" -p "E:\Virtools\Virtools Dev 3.0" -y "J:\YYCCommonplace\bin\msvc_install"
MSBuild src\vtobjplugin.sln -p:Configuration=Release -p:Platform=x86
python -m zipfile -c "E:\Virtools\Virtools Dev 3.0\InterfacePlugins\vtobjplugin_vt3.0.zip" "E:\Virtools\Virtools Dev 3.0\InterfacePlugins\vtobjplugin.dll" "E:\Virtools\Virtools Dev 3.0\InterfacePlugins\vtobjplugin.pdb"
python "script\setup_env.py" -p "E:\Virtools\Virtools Dev 3.5" -y "J:\YYCCommonplace\bin\msvc_install"
MSBuild src\vtobjplugin.sln -p:Configuration=Release -p:Platform=x86
python -m zipfile -c "E:\Virtools\Virtools Dev 3.5\InterfacePlugins\vtobjplugin_vt3.5.zip" "E:\Virtools\Virtools Dev 3.5\InterfacePlugins\vtobjplugin.dll" "E:\Virtools\Virtools Dev 3.5\InterfacePlugins\vtobjplugin.pdb"
python "script\setup_env.py" -p "E:\Virtools\Virtools Dev 4.0" -y "J:\YYCCommonplace\bin\msvc_install"
MSBuild src\vtobjplugin.sln -p:Configuration=Release -p:Platform=x86
python -m zipfile -c "E:\Virtools\Virtools Dev 4.0\InterfacePlugins\vtobjplugin_vt4.0.zip" "E:\Virtools\Virtools Dev 4.0\InterfacePlugins\vtobjplugin.dll" "E:\Virtools\Virtools Dev 4.0\InterfacePlugins\vtobjplugin.pdb"
python "script\setup_env.py" -p "E:\Virtools\Virtools Dev 5.0" -y "J:\YYCCommonplace\bin\msvc_install"
MSBuild src\vtobjplugin.sln -p:Configuration=Release -p:Platform=x86
python -m zipfile -c "E:\Virtools\Virtools Dev 5.0\InterfacePlugins\vtobjplugin_vt5.0.zip" "E:\Virtools\Virtools Dev 5.0\InterfacePlugins\vtobjplugin.dll" "E:\Virtools\Virtools Dev 5.0\InterfacePlugins\vtobjplugin.pdb"
ECHO Build Done.
