#include "obj_export.h"
#include "buffer_helper.h"
#include <filesystem>
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
}
obj_export::~obj_export() {
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
	if (cfg->export_mtl) StartFile(&fMtl, "mtl");
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		if (!JudgeValidObject(obj)) continue;	//judge whether it is a valid export object

		NextFile(&fObj, obj->GetName(), "obj");
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			if (cfg->export_mtl) {
				NextFile(&fMtl, obj->GetName(), "mtl");
				ExportMaterial();
			}
		}
	}

	//if one file, export mtl here to make sure some material can be shared using
	if (cfg->export_mtl) ExportMaterial();

	EndRepostition();
	EndFile(&fObj);
	if (cfg->export_mtl) EndFile(&fMtl);
}
void obj_export::ExportGroupWarpper() {
	XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_3DENTITY, TRUE);
	int count = objArray.Size();

	CK3dEntity* obj = NULL;
	CKGroup* grp = (CKGroup*)ctx->GetObjectA(cfg->selected_item);
	int storedV = 0;
	StartFile(&fObj, "obj");
	if (cfg->export_mtl) StartFile(&fMtl, "mtl");
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		if (!obj->IsInGroup(grp)) continue;

		if (!JudgeValidObject(obj)) continue;	//judge whether it is a valid export object
		NextFile(&fObj, obj->GetName(), "obj");
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			if (cfg->export_mtl) {
				NextFile(&fMtl, obj->GetName(), "mtl");
				ExportMaterial();
			}
		}
	}

	//if one file, export mtl here to make sure some material can be shared using
	if (cfg->export_mtl) ExportMaterial();

	EndRepostition();
	EndFile(&fObj);
	if (cfg->export_mtl) EndFile(&fMtl);
}
void obj_export::ExportObjectWarpper() {
	int storedV = 0;
	CK3dEntity* obj = (CK3dEntity*)ctx->GetObjectA(cfg->selected_item);
	if (!JudgeValidObject(obj)) return;	//judge whether it is a valid export object

	//obj mtl
	StartFile(&fObj, "obj");
	if (cfg->export_mtl) StartFile(&fMtl, "mtl");
	StartReposition();

	NextFile(&fObj, obj->GetName(), "obj");
	ExportObject(obj, &storedV);
	NextRepostion(obj);
	if (cfg->export_mtl) {
		NextFile(&fMtl, obj->GetName(), "mtl");
		ExportMaterial();
	}

	EndRepostition();
	EndFile(&fObj);
	if (cfg->export_mtl) EndFile(&fMtl);

}


void obj_export::StartReposition() {
	if ((!cfg->omit_transform) || (!cfg->right_hand)) return;

	std::filesystem::path filepath;
	if (cfg->reposition_3dsmax) {
		filepath = cfg->export_folder;
		filepath /= "3dsmax.ms";
		frepos_3dsmax = fopen(filepath.string().c_str(), "w");
		if (frepos_3dsmax == NULL) throw std::bad_alloc();

		//write header
		fputs("fn tryModify obj mat = (\n", frepos_3dsmax);
		fputs("    try obj.transform = mat catch ()\n", frepos_3dsmax);
		fputs(")\n", frepos_3dsmax);
	}
	if (cfg->reposition_blender) {
		filepath = cfg->export_folder;
		filepath /= "blender.py";
		frepos_blender = fopen(filepath.string().c_str(), "w");
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
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenerateObjName(obj, buffer_helper::misc_buffer);
		ObjectNameUniform(buffer_helper::misc_buffer);
		fprintf(frepos_3dsmax, "tryModify $%s (matrix3 [%f, %f, %f] [%f, %f, %f] [%f, %f, %f] [%f, %f, %f])\n",
			buffer_helper::misc_buffer,
			cacheMat[0][0], cacheMat[0][2], cacheMat[0][1],
			cacheMat[2][0], cacheMat[2][2], cacheMat[2][1],
			cacheMat[1][0], cacheMat[1][2], cacheMat[1][1],
			cacheMat[3][0], cacheMat[3][2], cacheMat[3][1]);
	}
	if (frepos_blender != NULL) {
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenerateObjName(obj, buffer_helper::misc_buffer);
		ObjectNameUniform(buffer_helper::misc_buffer);
		fprintf(frepos_blender, "tryModify('%s', Matrix(((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))))\n",
			buffer_helper::misc_buffer,
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
			std::filesystem::path fp(cfg->export_folder);
			sprintf(buffer_helper::misc_buffer, "all.%s", suffix);
			fp /= buffer_helper::misc_buffer;

			*fs = fopen(fp.string().c_str(), "w");
			if (*fs == NULL) throw std::bad_alloc();
		}
	}
}
void obj_export::NextFile(FILE** fs, char* name, char* suffix) {
	if (cfg->file_mode == FILEMODE_MULTIFILE) {
		//close file
		if (*fs != NULL) fclose(*fs);

		//open new file
		std::filesystem::path fp(cfg->export_folder);
		strcpy(buffer_helper::misc_buffer, name);
		FileNameUniform(buffer_helper::misc_buffer);
		sprintf(buffer_helper::misc_buffer2, "%s.%s", buffer_helper::misc_buffer, suffix);
		fp /= buffer_helper::misc_buffer2;

		*fs = fopen(fp.string().c_str(), "w");
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
			strcpy(buffer_helper::export_buffer, obj->GetName());
			FileNameUniform(buffer_helper::export_buffer);
		} else strcpy(buffer_helper::export_buffer, "all");
		if (cfg->export_mtl)
			fprintf(fObj, "mtllib %s.mtl\n", buffer_helper::export_buffer);
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
	GenerateObjName(obj, buffer_helper::export_buffer);
	ObjectNameUniform(buffer_helper::export_buffer);
	fprintf(fObj, "g %s\n", buffer_helper::export_buffer);

	//f and usemtl
	count = mesh->GetFaceCount();
#if defined(TARGET_VT35)
	WORD* fIndices = mesh->GetFacesIndices();
#endif
#if defined(TARGET_VT5)
	CKVINDEX* fIndices = mesh->GetFacesIndices();
#endif
	int i1, i2, i3;
	for (int i = 0; i < count; i++) {
		//usemtl
		if (cfg->export_mtl) {
			CKMaterial* fmtl = mesh->GetFaceMaterial(i);
			if (fmtl != NULL) {
				matList->insert(fmtl->GetID());
				GenerateMtlName(fmtl, buffer_helper::export_buffer);
				ObjectNameUniform(buffer_helper::export_buffer);
				fprintf(fObj, "usemtl %s\n", buffer_helper::export_buffer);
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
	GenerateMtlName(mtl, buffer_helper::export_buffer);
	ObjectNameUniform(buffer_helper::export_buffer);
	fprintf(fMtl, "newmtl %s\n", buffer_helper::export_buffer);
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

	if (cfg->custom_texture_format) GenerateTextureName(texture, buffer_helper::export_buffer, cfg->texture_format.c_str());
	else GenerateTextureName(texture, buffer_helper::export_buffer, NULL);

	fprintf(fMtl, "map_Kd %s\n", buffer_helper::export_buffer);

	//export texture
	if (!cfg->copy_texture) return;
	std::filesystem::path fp(cfg->export_folder);
	fp /= buffer_helper::export_buffer;
	strcpy(buffer_helper::export_buffer, fp.string().c_str());
	texture->SaveImage(buffer_helper::export_buffer, 0, FALSE);

}


BOOL obj_export::JudgeValidObject(CK3dEntity* obj) {
	CKMesh* mesh = obj->GetCurrentMesh();
	if (mesh == NULL) return FALSE;					//no mesh
	if (mesh->GetFaceCount() == 0) return FALSE;	//mo face
	return TRUE;
}
void obj_export::GenerateObjName(CK3dEntity* obj, char* name) {
	if (cfg->name_prefix)
		sprintf(name, "obj%d_%s", obj->GetID(), obj->GetName());
	else
		strcpy(name, obj->GetName());
}
void obj_export::GenerateMtlName(CKMaterial* obj, char* name) {
	if (cfg->name_prefix)
		sprintf(name, "mtl%d_%s", obj->GetID(), obj->GetName());
	else
		strcpy(name, obj->GetName());
}
void obj_export::GenerateTextureName(CKTexture* obj, char* name, const char* suffix) {
	std::string file;
	std::filesystem::path filepath;

	file = obj->GetSlotFileName(0);
	if (file.find('\\') != file.npos) {
		filepath = file;
		file = filepath.filename().string();
	}

	if (suffix != NULL) {
		file.append(".");
		file.append(suffix);
	}
	strcpy(name, file.c_str());
}
void obj_export::ObjectNameUniform(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ' || //obj ban
			str[i] == '\'' || //blender ban
			str[i] == '$' || //3dsmax ban
			str[i] == '.' ||
			str[i] == '"') //my own ban
			str[i] = '_';
	}
}
void obj_export::FileNameUniform(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ' || //sync with object uniform
			str[i] == '\'' ||
			str[i] == '$' ||
			str[i] == '.' ||
			str[i] == '\\' || //file system ban
			str[i] == '/' ||
			str[i] == '*' ||
			str[i] == '?' ||
			str[i] == '"' ||
			str[i] == '<' ||
			str[i] == '>' ||
			str[i] == '|')
			str[i] = '_';
	}
}
