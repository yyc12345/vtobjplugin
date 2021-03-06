#if !defined(_YYCDLL_OBJ_EXPORT_H__IMPORTED_)
#define _YYCDLL_OBJ_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include <string>
#include <set>

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

	BOOL reposition_3dsmax;
	BOOL reposition_blender;

	BOOL export_mtl;
	BOOL export_texture;
	BOOL copy_texture;
	BOOL custom_texture_format;
	std::string texture_format;

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

	BOOL JudgeValidObject(CK3dEntity* obj);
	void GenerateObjName(CK3dEntity* obj, char* name);
	void GenerateMtlName(CKMaterial* obj, char* name);
	void GenerateTextureName(CKTexture* obj, char* name, const char* suffix);
	void ObjectNameUniform(char* str);
	void FileNameUniform(char* str);

	FILE* fObj;
	FILE* fMtl;
	FILE* frepos_3dsmax;
	FILE* frepos_blender;
	std::set<CK_ID>* matList;
	ExportConfig* cfg;
	CKContext* ctx;
};

#endif