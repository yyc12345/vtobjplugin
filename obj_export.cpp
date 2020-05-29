#include "obj_export.h"
#include <exception>
#pragma warning(disable:6387)

obj_export::obj_export() {
	fObj = NULL;
	fMtl = NULL;
	frepos_3dsmax = NULL;
	frepos_blender = NULL;
	cfg = NULL;
	ctx = NULL;
	matList = new std::set<CK_ID>();
	path_help = (char*)calloc(65526, sizeof(char));
	name_help = (char*)calloc(255, sizeof(char));
	if (path_help == NULL || name_help == NULL)
		throw std::bad_alloc();
}
obj_export::~obj_export() {
	free(path_help);
	free(name_help);
	delete matList;
}

void obj_export::Ready4Export(CKContext* ctx, ExportConfig* cfg) {
	this->ctx = ctx;
	this->cfg = cfg;
	matList->clear();
}
void obj_export::ExportAllWarpper() {
	XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_3DENTITY, TRUE);
	int count = objArray.Size();

	CK3dEntity* obj = NULL;
	int storedV = 0;
	StartFile(&fObj, "obj");
	StartFile(&fMtl, "mtl");
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		NextFile(&fObj, obj->GetName(), "obj");
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			NextFile(&fMtl, obj->GetName(), "mtl");
			ExportMaterial();
		}
	}

	//if one file, export mtl here to make sure some material can be shared using
	ExportMaterial();

	EndRepostition();
	EndFile(&fObj);
	EndFile(&fMtl);
}
void obj_export::ExportGroupWarpper() {
	XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_3DENTITY, TRUE);
	int count = objArray.Size();

	CK3dEntity* obj = NULL;
	CKGroup* grp = (CKGroup*)ctx->GetObjectA(cfg->selected_item);
	int storedV = 0;
	StartFile(&fObj, "obj");
	StartFile(&fMtl, "mtl");
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		if (!obj->IsInGroup(grp)) continue;

		NextFile(&fObj, obj->GetName(), "obj");
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			NextFile(&fMtl, obj->GetName(), "mtl");
			ExportMaterial();
		}
	}

	//if one file, export mtl here to make sure some material can be shared using
	ExportMaterial();

	EndRepostition();
	EndFile(&fObj);
	EndFile(&fMtl);
}
void obj_export::ExportObjectWarpper() {
	//obj mtl
	StartFile(&fObj, "obj");
	StartFile(&fMtl, "mtl");
	StartReposition();

	int storedV = 0;
	CK3dEntity* obj = (CK3dEntity*)ctx->GetObjectA(cfg->selected_item);
	NextFile(&fObj, obj->GetName(), "obj");
	ExportObject(obj, &storedV);
	NextRepostion(obj);
	if (cfg->export_mtl) {
		NextFile(&fMtl, obj->GetName(), "mtl");
		ExportMaterial();
	}

	EndRepostition();
	EndFile(&fObj);
	EndFile(&fMtl);

}


void obj_export::StartReposition() {
	if ((!cfg->omit_transform) || (!cfg->right_hand)) return;

	if (cfg->reposition_3dsmax) {
		strcpy(path_help, cfg->export_folder);
		strcat(path_help, "\\3dsmax.ms");
		frepos_3dsmax = fopen(path_help, "w");
		if (frepos_3dsmax == NULL) throw std::bad_alloc();
	}
	if (cfg->reposition_blender) {
		strcpy(path_help, cfg->export_folder);
		strcat(path_help, "\\blender.py");
		frepos_blender = fopen(path_help, "w");
		if (frepos_blender == NULL) throw std::bad_alloc();

		//write header
		fputs("import bpy\n", frepos_blender);
		fputs("from mathutils import Matrix\n", frepos_blender);
		fputs("def tryModify(obj, mat):\n", frepos_blender);
		fputs("    try:\n", frepos_blender);
		fputs("        bpy.data.objects[obj].matrix_world = mat.transposed()\n", frepos_blender);
		fputs("    except:\n", frepos_blender);
		fputs("        pass\n", frepos_blender);
	}
}
void obj_export::NextRepostion(CK3dEntity* obj) {
	if (frepos_3dsmax != NULL) {
		//todo: finish 3ds max repostion export
	}
	if (frepos_blender != NULL) {
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenerateObjName(obj, name_help);
		fprintf(frepos_blender, "tryModify('%s', Matrix(((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))))\n",
			name_help,
			cacheMat[0][0], cacheMat[0][2], cacheMat[0][1], cacheMat[0][3],
			cacheMat[2][0], cacheMat[2][2], cacheMat[2][1], cacheMat[2][3],
			cacheMat[1][0], cacheMat[1][2], cacheMat[1][1], cacheMat[1][3],
			cacheMat[3][0], cacheMat[3][2], cacheMat[3][1], cacheMat[3][3]);
	}
}
void obj_export::EndRepostition() {
	if (frepos_3dsmax != NULL) {
		fclose(frepos_3dsmax);
		frepos_3dsmax = NULL;
	}

	if (frepos_blender != NULL) {
		fclose(frepos_blender);
		frepos_blender = NULL;
	}
}

void obj_export::StartFile(FILE** fs, char* suffix) {
	if (cfg->file_mode == FILEMODE_ONEFILE) {
		if (*fs == NULL) {
			sprintf(path_help, "%s\\all.%s", cfg->export_folder, suffix);
			*fs = fopen(path_help, "w");
			if (*fs == NULL) throw std::bad_alloc();
		}
	}
}
void obj_export::NextFile(FILE** fs, char* name, char* suffix) {
	if (cfg->file_mode == FILEMODE_MULTIFILE) {
		//close file
		if (*fs != NULL) fclose(*fs);

		//open new file
		strcpy(name_help, name);
		FileNameUniform(name_help);
		sprintf(path_help, "%s\\%s.%s", cfg->export_folder, name_help, suffix);
		*fs = fopen(path_help, "w");
		if (*fs == NULL) throw std::bad_alloc();
	}
}
void obj_export::EndFile(FILE** fs) {
	if (fs != NULL) {
		fclose(*fs);
		*fs = NULL;
	}
}


void obj_export::ExportObject(CK3dEntity* obj, int* storedV) {
	//========================set up stored v and const
	int voffset = *storedV;
	CKMesh* mesh = obj->GetCurrentMesh();
	int count = mesh->GetVertexCount();
	(*storedV) += count;

	//mtllib
	if (voffset == 0) {
		//the first obj, add mtllib
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			strcpy(name_help, obj->GetName());
			FileNameUniform(name_help);
		} else strcpy(name_help, "all");
		if (cfg->export_mtl)
			fprintf(fObj, "mtllib %s.mtl\n", name_help);
	}

#define righthand_pos_converter(condition,y,z) (condition ? z : y), (condition ? y : z)
#define righthand_uv_converter(condition,u,v) u, (condition ? -v : v)

	//v
	VxVector cacheVec1, cacheVec2;
	VxMatrix cacheMat = obj->GetWorldMatrix();
	for (int i = 0; i < count; i++) {
		mesh->GetVertexPosition(i, &cacheVec1);
		if (cfg->omit_transform) {
			fprintf(fObj, "v %f %f %f\n", cacheVec1.x, righthand_pos_converter(cfg->right_hand, cacheVec1.y, cacheVec1.z));
		} else {
			Vx3DMultiplyMatrixVector(&cacheVec2, cacheMat, &cacheVec1);
			fprintf(fObj, "v %f %f %f\n", cacheVec2.x, righthand_pos_converter(cfg->right_hand, cacheVec2.y, cacheVec2.z));
		}
	}

	//vt
	float u, v;
	for (int i = 0; i < count; i++) {
		mesh->GetVertexTextureCoordinates(i, &u, &v);
		fprintf(fObj, "vt %f %f 0\n", righthand_uv_converter(cfg->right_hand, u, v));
	}

	//vn
	for (int i = 0; i < count; i++) {
		mesh->GetVertexNormal(i, &cacheVec1);
		fprintf(fObj, "vn %f %f %f\n", cacheVec1.x, righthand_pos_converter(cfg->right_hand, cacheVec1.y, cacheVec1.z));
	}

	//g
	GenerateObjName(obj, name_help);
	fprintf(fObj, "g %s\n", name_help);

	//f and usemtl
	count = mesh->GetFaceCount();
	WORD* fIndices = mesh->GetFacesIndices();
	int i1, i2, i3;
	for (int i = 0; i < count; i++) {
		//usemtl
		if (cfg->export_mtl) {
			CKMaterial* fmtl = mesh->GetFaceMaterial(i);
			if (fmtl != NULL) {
				matList->insert(fmtl->GetID());
				GenerateMtlName(fmtl, name_help);
				fprintf(fObj, "usemtl %s\n", name_help);
			} else fputs("usemtl off\n", fObj);
		}

		//f
		i1 = (int)fIndices[i * 3] + 1 + voffset;
		i2 = (int)fIndices[i * 3 + 1] + 1 + voffset;
		i3 = (int)fIndices[i * 3 + 2] + 1 + voffset;
		if (cfg->right_hand) {
			fprintf(fObj, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
				i3, i3, i3,
				i2, i2, i2,
				i1, i1, i1);
		} else {
			fprintf(fObj, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
				i1, i1, i1,
				i2, i2, i2,
				i3, i3, i3);
		}

	}

}
void obj_export::ExportMaterial() {
	//save mtl
	CKMaterial* mtl = NULL;
	for (auto it = matList->begin(); it != matList->end(); it++) {
		mtl = (CKMaterial*)ctx->GetObjectA(*it);
		ExportMaterial(mtl);
	}

	matList->clear();
}
void obj_export::ExportMaterial(CKMaterial* mtl) {
	//basic
	GenerateMtlName(mtl, name_help);
	fprintf(fMtl, "newmtl %s\n", name_help);
	VxColor col = mtl->GetAmbient();
	fprintf(fMtl, "Ka %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetDiffuse();
	fprintf(fMtl, "Kd %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetEmissive();
	fprintf(fMtl, "Ks %f %f %f\n", col.r, col.g, col.b);

	//set up texture
	if (!cfg->export_texture) return;
	CKTexture* texture = mtl->GetTexture();
	if (texture == NULL) return;

	if (cfg->custom_texture_format) GenerateTextureName(texture, name_help, cfg->texture_format);
	else GenerateTextureName(texture, name_help, NULL);

	fprintf(fMtl, "map_Kd %s\n", name_help);

	//export texture
	if (!cfg->copy_texture) return;
	sprintf(path_help, "%s\\%s", cfg->export_folder, name_help);
	//if (cfg->save_alpha) texture->SaveImageAlpha(nameingHlp, 0);
	//else texture->SaveImage(nameingHlp, 0, !cfg->custom_texture_format);
	texture->SaveImage(path_help, 0, FALSE);

}


void obj_export::GenerateObjName(CK3dEntity* obj, char* name) {
	sprintf(name, "obj%d_%s", obj->GetID(), obj->GetName());
	ObjectNameUniform(name);
}
void obj_export::GenerateMtlName(CKMaterial* obj, char* name) {
	sprintf(name, "mtl%d_%s", obj->GetID(), obj->GetName());
	ObjectNameUniform(name);
}
void obj_export::GenerateFileName(CKObject* obj, char* name) {
	strcpy(name, obj->GetName());
	FileNameUniform(name);
}
void obj_export::GenerateTextureName(CKTexture* obj, char* name, char* suffix) {
	if (suffix == NULL) sprintf(name, "%s", obj->GetName());
	else sprintf(name, "%s.%s", obj->GetName(), suffix);
	ObjectNameUniform(name);
}
void obj_export::ObjectNameUniform(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ' ||
			str[i] == '\'' ||
			str[i] == '"')
			str[i] = '_';
	}
}
void obj_export::FileNameUniform(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ' ||
			str[i] == '\\' ||
			str[i] == '/' ||
			str[i] == '*' ||
			str[i] == '?' ||
			str[i] == '"' ||
			str[i] == '<' ||
			str[i] == '>' ||
			str[i] == '\'' ||
			str[i] == '|')
			str[i] = '_';
	}
}
void obj_export::strinsert(char* str, const char* insertedStr) {
	int i = strlen(str), j = strlen(insertedStr);
	for (int q = i + j; q >= 0; q--) {
		if (q >= j) str[q] = str[q - j];
		else str[q] = insertedStr[q];
	}
}