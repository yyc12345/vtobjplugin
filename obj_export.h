#if !defined(_YYCDLL_OBJ_EXPORT_H__IMPORTED_)
#define _YYCDLL_OBJ_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include <set>

enum ExportMode {
	EXPORTMODE_OBJECT,
	EXPORTMODE_GROUP,
	EXPORTMODE_ALL
};

typedef struct {
	char export_folder[1024];
	BOOL omit_transform;
	BOOL export_mtl;
	BOOL export_texture;
	BOOL right_hand;
	BOOL custom_texture_format;
	BOOL save_alpha;
	char texture_format[32];
	ExportMode export_mode;
	CK_ID selected_item;
}ExportConfig;

void ExportAllWarpper(CKContext* ctx, ExportConfig* cfg);
void ExportGroupWarpper(CKContext* ctx, ExportConfig* cfg);
void ExportObjectWarpper(CKContext* ctx, ExportConfig* cfg);
void ExportObject(FILE* fs, CK3dEntity* obj, ExportConfig* cfg, std::set<CK_ID>* matList, int* storedV, char* nameingHlp);
void ExportMaterial(FILE* fs, ExportConfig* cfg, CKMaterial* mtl, char* nameingHlp);

void GenerateObjName(CK3dEntity* obj, char* name);
void GenerateMtlName(CKMaterial* obj, char* name);
void GenerateTextureName(CKTexture* obj, char* name);
void NameUniform(char* str);
void strinsert(char* str, const char* insertedStr);

#endif