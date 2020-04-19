#if !defined(_YYCDLL_OBJ_EXPORT_H__IMPORTED_)
#define _YYCDLL_OBJ_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include <list>

void ExportAllWarpper();
void ExportGroupWarpper();
void ExportObjectWarpper();
void ExportObject(FILE* fs, BOOL omitTransform, std::list<CK_ID>* matList);
void ExportMaterial(FILE* fs, std::list<CK_ID>* matList, BOOL storeTexture, const char* textureSuffix);

#endif