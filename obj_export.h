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

enum WrittenFileType {
	WRITTENFILETYPE_OBJMTL,	// write obj/mtl file
	WRITTENFILETYPE_SCRIPT	// write script file
};

enum ModelFileType {
	MODELFILETYPE_OBJ,
	MODELFILETYPE_MTL
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

	void StartFile(FILE** fs, ModelFileType target_fs);
	void NextFile(FILE** fs, CKObject* obj, ModelFileType target_fs);
	void EndFile(FILE** fs);

	void ExportObject(CK3dEntity* obj, int* storedV);
	void ExportMaterial();
	void ExportMaterial(CKMaterial* mtl);


	BOOL ValidateObjectLegal(CK3dEntity* obj);

	BOOL GenCKObjectNameW(CKObject* obj, std::wstring* name);
	void GenCKObjectNameA(CKObject* obj, std::string* name);
	void GenObjMtlName(CKObject* obj, std::string* name, WrittenFileType target_fs);
	FILE* OpenObjMtlFile(CKObject* obj, ModelFileType target_fs);

	BOOL GenCKTextureNameW(CKTexture* obj, std::wstring* name);
	void GenCKTextureNameA(CKTexture* obj, std::string* name);
	void GenCKTextureName4File(CKTexture* obj, std::string* name);
	void CopyTextureFile(CKTexture* texture);

	void RegulateNameA(std::string* str, BOOL eliminate4filebody);
	void RegulateNameW(std::wstring* str, BOOL eliminate4filebody);

	FILE* fObj;
	FILE* fMtl;
	FILE* frepos_3dsmax;
	FILE* frepos_blender;
	std::set<CK_ID>* matList,* txurList;
	ExportConfig* cfg;
	CKContext* ctx;
};

#endif