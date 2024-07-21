import os

def writeline(f, val: str) -> None:
    f.write(val)
    f.write('\n')

def main() -> None:
    filename: str = os.path.join(os.path.dirname(__file__), 'build.bat')
    with open(filename, 'w') as f: # not utf8 by design because it is BAT
        # build script header
        writeline(f, '@ECHO OFF')
        writeline(f, 'ECHO This script must be executed at the root of project. Press any key to continue...')
        writeline(f, 'PAUSE')

        for ver in ('3.0', '3.5', '4.0', '5.0'):
            vt_path: str = f'E:\Virtools\\Virtools Dev {ver}'
            vt_interface_path: str = f'{vt_path}\InterfacePlugins'
            writeline(f, f'python "script\\setup_env.py" -p "{vt_path}" -y "J:\\YYCCommonplace\\bin\\install"')
            writeline(f, f'MSBuild src\\vtobjplugin.sln -p:Configuration=Release -p:Platform=x86')
            writeline(f, f'python -m zipfile -c "{vt_interface_path}\\vtobjplugin_vt{ver}.zip" "{vt_interface_path}\\vtobjplugin.dll" "{vt_interface_path}\\vtobjplugin.pdb"')

        # build script tail
        writeline(f, 'ECHO Build Done.')

if __name__ == '__main__':
    main()
