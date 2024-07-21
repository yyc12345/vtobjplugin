import vs_props_writer
import argparse
import os

def main() -> None:
    # define argument parser
    parser = argparse.ArgumentParser(description='Virtools OBJ Exporter Build Environment Maker')
    parser.add_argument(
        '-p', '--virtools-path', required=True, action='store', dest='virtools_path',
        help='''
        The path to the root folder of you picked Virtools version where you can find "Dev.exe".
        '''
    )
    parser.add_argument(
        '-y', '--yycc-path', required=True, action='store', dest='yycc_path',
        help='''
        The path to the built YYCCommonplace library folder.
        See https://github.com/yyc12345/YYCCommonplace .
        '''
    )
    # parse arguments
    args = parser.parse_args()

    # write macros
    writer = vs_props_writer.VsPropsWriter()
    writer.AddMacro('VIRTOOLS_PATH', args.virtools_path)
    writer.AddMacro('YYCC_PATH', args.yycc_path)
    writer.Generate(os.path.join(os.path.dirname(__file__), '../src/vtobjplugin.props'))

if __name__ == '__main__':
    main()
