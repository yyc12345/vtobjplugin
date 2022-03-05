#include "obj_export.h"
#include "string_helper.h"
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
	txurList = new std::set<CK_ID>();
}
obj_export::~obj_export() {
	delete matList;
	delete txurList;
}

void obj_export::Ready4Export(CKContext* ctx, ExportConfig* cfg) {
	this->ctx = ctx;
	this->cfg = cfg;
	matList->clear();
	txurList->clear();
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
		if (!ValidateObjectLegal(obj)) continue;	//judge whether it is a valid export object

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

		if (!ValidateObjectLegal(obj)) continue;	//judge whether it is a valid export object
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
	if (!ValidateObjectLegal(obj)) return;	//judge whether it is a valid export object

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

		//write bom
		::string_helper::write_utf8bom(frepos_3dsmax);

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
		fputs("# -*- coding: UTF-8 -*-\n", frepos_blender);
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
	std::string obj_name;

	if (frepos_3dsmax != NULL) {
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenerateObjName(obj, cfg->use_utf8_script, &obj_name);
		ObjectNameUniform(&obj_name);
		fprintf(frepos_3dsmax, "tryModify $%s (matrix3 [%f, %f, %f] [%f, %f, %f] [%f, %f, %f] [%f, %f, %f])\n",
			obj_name.c_str(),
			cacheMat[0][0], cacheMat[0][2], cacheMat[0][1],
			cacheMat[2][0], cacheMat[2][2], cacheMat[2][1],
			cacheMat[1][0], cacheMat[1][2], cacheMat[1][1],
			cacheMat[3][0], cacheMat[3][2], cacheMat[3][1]);
	}
	if (frepos_blender != NULL) {
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenerateObjName(obj, cfg->use_utf8_script, &obj_name);
		ObjectNameUniform(&obj_name);
		fprintf(frepos_blender, "tryModify('%s', Matrix(((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))))\n",
			obj_name.c_str(),
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
			std::string filename;
			::string_helper::stdstring_sprintf(&filename, "all.%s", suffix);
			fp /= filename.c_str();

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
		std::string filename;
		::string_helper::stdstring_sprintf(&filename, "%s.%s", name, suffix);
		FileNameUniform(&filename);
		fp /= filename.c_str();

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
	std::string obj_name;
	(*storedV) += count;

	//mtllib
	if (voffset == 0) {
		//the first obj, add mtllib
		std::string mtllib_name;
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			GenerateObjName(obj, FALSE, &mtllib_name);
			FileNameUniform(&mtllib_name);
		} else mtllib_name = "all";

		if (cfg->export_mtl)
			fprintf(fObj, "mtllib %s.mtl\n", mtllib_name.c_str());
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

	//g or o
	GenerateObjName(obj, cfg->use_utf8_obj, &obj_name);
	ObjectNameUniform(&obj_name);
	if (cfg->use_group_split_object)
		fprintf(fObj, "g %s\n", obj_name.c_str());
	else
		fprintf(fObj, "o %s\n", obj_name.c_str());

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
				GenerateMtlName(fmtl, cfg->use_utf8_obj, &obj_name);
				ObjectNameUniform(&obj_name);
				fprintf(fObj, "usemtl %s\n", obj_name.c_str());
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
	std::string mat_name;
	std::string filesystem_filename, fileinternal_filename;

	//basic
	GenerateMtlName(mtl, cfg->use_utf8_obj, &mat_name);
	ObjectNameUniform(&mat_name);
	fprintf(fMtl, "newmtl %s\n", mat_name.c_str());
	VxColor col = mtl->GetAmbient();
	fprintf(fMtl, "Ka %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetDiffuse();
	fprintf(fMtl, "Kd %f %f %f\n", col.r, col.g, col.b);
	col = mtl->GetEmissive();
	fprintf(fMtl, "Ks %f %f %f\n", col.r, col.g, col.b);

	//set up texture
	if (!cfg->export_texture) return;
	CKTexture* texture = mtl->GetTexture();
	//if texture is not existed, next material
	if (texture == NULL) return;
	if (texture->GetSlotFileName(0) == NULL) return;

	GenerateTextureName(texture, cfg->use_utf8_obj, &fileinternal_filename);
	fprintf(fMtl, "map_Kd %s\n", fileinternal_filename.c_str());

	//export texture
	if (!cfg->copy_texture) return;
	if (txurList->find(texture->GetID()) != txurList->end()) return;	// existed item, file has been copied, skip
	txurList->insert(texture->GetID());	// otherwise ass it in set

	if (cfg->use_utf8_obj) {
		// filesystem is not same with internal file string, re-generate it
		GenerateTextureName(texture, FALSE, &filesystem_filename);
	} else {
		// same with internal file string, cpy directly
		filesystem_filename = fileinternal_filename.c_str();
	}
	std::filesystem::path fp(cfg->export_folder);
	fp /= filesystem_filename.c_str();
	texture->SaveImage((CKSTRING)fp.string().c_str(), 0, FALSE);

}


BOOL obj_export::ValidateObjectLegal(CK3dEntity* obj) {
	CKMesh* mesh = obj->GetCurrentMesh();
	if (mesh == NULL) return FALSE;					//no mesh
	if (mesh->GetFaceCount() == 0) return FALSE;	//mo face
	return TRUE;
}
void obj_export::GenerateObjName(CK3dEntity* obj, BOOL as_utf8, std::string* name) {
	std::string naked_name, conv_name;

	naked_name = (obj->GetName() == NULL ? "noname_3d_object" : obj->GetName());
	if (as_utf8) {
		::string_helper::encoding_conv(&naked_name, &conv_name, cfg->composition_encoding, CP_UTF8);
	}

	if (cfg->name_prefix)
		::string_helper::stdstring_sprintf(name, "obj%d_%s", obj->GetID(), conv_name.c_str());
	else
		(*name) = conv_name.c_str();
}
void obj_export::GenerateMtlName(CKMaterial* obj, BOOL as_utf8, std::string* name) {
	std::string naked_name, conv_name;

	naked_name = (obj->GetName() == NULL ? "noname_material" : obj->GetName());
	if (as_utf8) {
		::string_helper::encoding_conv(&naked_name, &conv_name, cfg->composition_encoding, CP_UTF8);
	}

	if (cfg->name_prefix)
		::string_helper::stdstring_sprintf(name, "mtl%d_%s", obj->GetID(), conv_name.c_str());
	else
		(*name) = conv_name.c_str();

}
void obj_export::GenerateTextureName(CKTexture* obj, BOOL as_utf8, std::string* name) {
	std::string file;
	std::filesystem::path filepath;

	file = obj->GetSlotFileName(0);
	if (file.find('\\') != file.npos) {
		filepath = file;
		file = filepath.filename().string();
	}

	// suffix process
	if (cfg->custom_texture_format) {
		file.append(".");
		file.append(cfg->texture_format.c_str());
	}

	(*name) = file.c_str();
}
void obj_export::ObjectNameUniform(std::string* str) {
	for (auto it = str->begin(); it != str->end(); it++) {
		if (*it == '\0') break;

		// check eliminate non-ascii
		if (cfg->eliminate_non_ascii && ((*it) | 0b10000000)) {
			*it = '_';
		}

		if (*it == ' ' || //obj ban
			*it == '\'' || //blender ban
			*it == '$' || //3dsmax ban
			*it == '.' ||
			*it == '"') //my own ban
			*it = '_';
	}
}
void obj_export::FileNameUniform(std::string* str) {
	for (auto it = str->begin(); it != str->end(); it++) {
		if (*it == '\0') break;

		// check eliminate non-ascii
		if (cfg->eliminate_non_ascii && ((*it) | 0b10000000)) {
			*it = '_';
		}

		if (*it == ' ' || //sync with object uniform
			*it == '\'' ||
			*it == '$' ||
			*it == '.' ||
			*it == '\\' || //file system ban
			*it == '/' ||
			*it == '*' ||
			*it == '?' ||
			*it == '"' ||
			*it == '<' ||
			*it == '>' ||
			*it == '|')
			*it = '_';
	}
}
