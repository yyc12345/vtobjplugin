#include "obj_export.h"
#pragma warning(disable:6387)

void ExportAllWarpper(CKContext* ctx, ExportConfig* cfg) {

}
void ExportGroupWarpper(CKContext* ctx, ExportConfig* cfg) {

}
void ExportObjectWarpper(CKContext* ctx, ExportConfig* cfg) {
	auto matList = new std::set<CK_ID>();
	char* filename = (char*)calloc(1024, sizeof(char));
	assert(filename != NULL);

	//obj
	strcpy(filename, cfg->export_folder);
	strcat(filename, "\\all.obj");
	FILE* fso = fopen(filename, "w");
	assert(fso != NULL);
	if (cfg->export_mtl)
		fputs("mtllib all.mtl\n", fso);
	int storedV = 0;
	ExportObject(fso, (CK3dEntity*)ctx->GetObjectA(cfg->selected_item), cfg, matList, &storedV, filename);
	fclose(fso);

	//mtl
	if (cfg->export_mtl) {
		strcpy(filename, cfg->export_folder);
		strcat(filename, "\\all.mtl");
		FILE* fsm = fopen(filename, "w");
		assert(fsm != NULL);

		CKMaterial* mtl = NULL;
		for (auto it = matList->begin(); it != matList->end(); it++) {
			mtl = (CKMaterial*)ctx->GetObjectA(*it);
			ExportMaterial(fsm, cfg, mtl, filename);
		}
		fclose(fsm);
	}

	free(filename);
	delete matList;
}


void ExportObject(FILE* fs, CK3dEntity* obj, ExportConfig* cfg, std::set<CK_ID>* matList, int* storedV, char* nameingHlp) {
	int voffset = *storedV;
	CKMesh* mesh = obj->GetCurrentMesh();
	int count = mesh->GetVertexCount();
	(*storedV) += count;

	float global_reverse = cfg->right_hand ? -1.0f : 1.0f;

	//v
	VxVector cacheVec1, cacheVec2;
	VxMatrix cacheMat = obj->GetWorldMatrix();
	for (int i = 0; i < count; i++) {
		mesh->GetVertexPosition(i, &cacheVec1);
		if (cfg->omit_transform) {
			fprintf(fs, "v %f %f %f\n", cacheVec1.x, cacheVec1.y, cacheVec1.z * global_reverse);
		} else {
			Vx3DMultiplyMatrixVector(&cacheVec2, cacheMat, &cacheVec1);
			fprintf(fs, "v %f %f %f\n", cacheVec2.x, cacheVec2.y, cacheVec2.z * global_reverse);
		}
	}

	//vt
	float u, v;
	for (int i = 0; i < count; i++) {
		mesh->GetVertexTextureCoordinates(i, &u, &v);
		fprintf(fs, "vt %f %f 0\n", u, v * global_reverse);
	}

	//vn
	for (int i = 0; i < count; i++) {
		mesh->GetVertexNormal(i, &cacheVec1);
		fprintf(fs, "vt %f %f %f\n", cacheVec1.x, cacheVec1.y, cacheVec1.z * global_reverse);
	}

	//g
	GenerateObjName(obj, nameingHlp);
	fprintf(fs, "g %s\n", nameingHlp);

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
				GenerateMtlName(fmtl, nameingHlp);
				fprintf(fs, "usemtl %s\n", nameingHlp);
			} else fputs("usemtl off\n", fs);
		}

		//f
		i1 = (int)fIndices[i * 3] + 1 + voffset;
		i2 = (int)fIndices[i * 3 + 1] + 1 + voffset;
		i3 = (int)fIndices[i * 3 + 2] + 1 + voffset;
		fprintf(fs, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			i1, i1, i1,
			i2, i2, i2,
			i3, i3, i3);
	}

}
void ExportMaterial(FILE* fs, ExportConfig* cfg, CKMaterial* mtl, char* nameingHlp) {
	//basic
	GenerateMtlName(mtl, nameingHlp);
	fprintf(fs, "newmtl %s\n", nameingHlp);
	VxColor col = mtl->GetAmbient();
	fprintf(fs, "Ka %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetDiffuse();
	fprintf(fs, "Kd %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetEmissive();
	fprintf(fs, "Ks %f %f %f\n", col.r, col.g, col.b);

	//texture
	CKTexture* texture = mtl->GetTexture();
	if (texture == NULL) return;
	GenerateTextureName(texture, nameingHlp);
	if (cfg->custom_texture_format) {
		strcat(nameingHlp, ".");
		strcat(nameingHlp, cfg->texture_format);
	}
	fprintf(fs, "map_Kd %s\n", nameingHlp);

	//export texture
	if (!cfg->export_texture) return;
	strinsert(nameingHlp, "\\");
	strinsert(nameingHlp, cfg->export_folder);
	//if (cfg->save_alpha) texture->SaveImageAlpha(nameingHlp, 0);
	//else texture->SaveImage(nameingHlp, 0, !cfg->custom_texture_format);
	texture->SaveImage(nameingHlp, 0, !cfg->custom_texture_format);
	
}


void GenerateObjName(CK3dEntity* obj, char* name) {
	sprintf(name, "obj%d_%s", obj->GetID(), obj->GetName());
	NameUniform(name);
}
void GenerateMtlName(CKMaterial* obj, char* name) {
	sprintf(name, "mtl%d_%s", obj->GetID(), obj->GetName());
	NameUniform(name);
}
void GenerateTextureName(CKTexture* obj, char* name) {
	sprintf(name, "%s", obj->GetName());
	NameUniform(name);
}
void NameUniform(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ')
			str[i] = '_';
	}
}
void strinsert(char* str, const char* insertedStr) {
	int i = strlen(str), j = strlen(insertedStr);
	for (int q = i + j; q >= 0; q--) {
		if (q >= j) str[q] = str[q - j];
		else str[q] = insertedStr[q];
	}
}