#if !defined(_YYCDLL_OBJ_EXPORT_H__IMPORTED_)
#define _YYCDLL_OBJ_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include <string>
#include <set>

#pragma warning(disable:26812)

enum ExportMode {
	EXPORTMODE_OBJECT,
	EXPORTMODE_GROUP,
	EXPORTMODE_ALL
};

enum FileMode {
	FILEMODE_ONEFILE,	//all object in one file
	FILEMODE_MULTIFILE	//one file per object
};

typedef struct {
	ExportMode export_mode;
	CK_ID selected_item;
	FileMode file_mode;
	std::string export_folder;

	BOOL omit_transform;
	BOOL right_hand;
	BOOL name_prefix;
	BOOL use_group_split_object;
	BOOL eliminate_non_ascii;

	BOOL reposition_3dsmax;
	BOOL reposition_blender;

	BOOL export_mtl;
	BOOL export_texture;
	BOOL copy_texture;
	BOOL custom_texture_format;
	std::string texture_format;

	BOOL use_custom_encoding;
	UINT composition_encoding;
	BOOL use_utf8_obj;
	BOOL use_utf8_script;

}ExportConfig;

class obj_export {
	public:
	obj_export();
	~obj_export();

	void Ready4Export(CKContext* ctx, ExportConfig* cfg);
	void ExportAllWarpper();
	void ExportGroupWarpper();
	void ExportObjectWarpper();

	private:
	void StartReposition();
	void NextRepostion(CK3dEntity* obj);
	void EndRepostition();

	void StartFile(FILE** fs, char* suffix);
	void NextFile(FILE** fs, char* name, char* suffix);
	void EndFile(FILE** fs);

	void ExportObject(CK3dEntity* obj, int* storedV);
	void ExportMaterial();
	void ExportMaterial(CKMaterial* mtl);

	BOOL ValidateObjectLegal(CK3dEntity* obj);
	void GenerateObjName(CK3dEntity* obj, BOOL as_utf8, std::string* name);
	void GenerateMtlName(CKMaterial* obj, BOOL as_utf8, std::string* name);
	void GenerateTextureName(CKTexture* obj, BOOL as_utf8, std::string* name);
	void ObjectNameUniform(std::string* str);
	void FileNameUniform(std::string* str);

	FILE* fObj;
	FILE* fMtl;
	FILE* frepos_3dsmax;
	FILE* frepos_blender;
	std::set<CK_ID>* matList,* txurList;
	ExportConfig* cfg;
	CKContext* ctx;
};

#endif