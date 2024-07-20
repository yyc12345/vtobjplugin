import xml.dom.minidom as minidom

class VsPropsWriter():

    __mMacroList: dict[str, str]

    def __init__(self):
        self.__mMacroList = {}

    def AddMacro(self, key: str, value: str):
        if key in self.__mMacroList:
            raise Exception(f'Duplicated Set Macro "{key}".')
        self.__mMacroList[key] = value

    def Generate(self, filename: str):
        # create some header
        document = minidom.getDOMImplementation().createDocument(None, 'Project', None)
        root: minidom.Element = document.documentElement
        root.setAttribute('ToolsVersion', '4.0')
        root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

        cache = document.createElement('ImportGroup')
        cache.setAttribute('Label', 'PropertySheets')
        root.appendChild(cache)
        
        # write macro
        node_PG = document.createElement('PropertyGroup')    # macro node 1
        node_PG.setAttribute('Label', 'UserMacros')
        root.appendChild(node_PG)

        cache = document.createElement('PropertyGroup') # dummy structure
        root.appendChild(cache)
        cache = document.createElement('ItemDefinitionGroup') #dummy structure
        root.appendChild(cache)

        node_IG = document.createElement('ItemGroup')    # macro node 2
        root.appendChild(node_IG)
        
        for key, value in self.__mMacroList.items():
            # create for PropertyGroup
            node_macro_decl = document.createElement(key)
            if value != '': # check whether data is empty.
                node_macro_decl.appendChild(document.createTextNode(value))
            node_PG.appendChild(node_macro_decl)

            # create for ItemGroup
            node_macro_ref = document.createElement("BuildMacro")
            node_macro_ref.setAttribute('Include', key)
            node_inner_macro_ref = document.createElement('Value')
            node_inner_macro_ref.appendChild(document.createTextNode("$({})".format(key)))
            node_macro_ref.appendChild(node_inner_macro_ref)
            node_IG.appendChild(node_macro_ref)

        # write to file
        with open(filename, 'w', encoding='utf-8') as f:
            document.writexml(f, addindent='\t', newl='\n', encoding='utf-8')
