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
	StartFile(&fObj, MODELFILETYPE_OBJ);
	if (cfg->export_mtl) StartFile(&fMtl, MODELFILETYPE_MTL);
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		if (!ValidateObjectLegal(obj)) continue;	//judge whether it is a valid export object

		NextFile(&fObj, obj, MODELFILETYPE_OBJ);
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			if (cfg->export_mtl) {
				NextFile(&fMtl, obj, MODELFILETYPE_MTL);
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
	StartFile(&fObj, MODELFILETYPE_OBJ);
	if (cfg->export_mtl) StartFile(&fMtl, MODELFILETYPE_MTL);
	StartReposition();

	for (int i = 0; i < count; i++) {
		obj = (CK3dEntity*)objArray[i];
		if (!obj->IsInGroup(grp)) continue;

		if (!ValidateObjectLegal(obj)) continue;	//judge whether it is a valid export object
		NextFile(&fObj, obj, MODELFILETYPE_OBJ);
		ExportObject(obj, &storedV);
		NextRepostion(obj);

		// if multifile, export here for each object
		if (cfg->file_mode == FILEMODE_MULTIFILE) {
			storedV = 0;
			if (cfg->export_mtl) {
				NextFile(&fMtl, obj, MODELFILETYPE_MTL);
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
	StartFile(&fObj, MODELFILETYPE_OBJ);
	if (cfg->export_mtl) StartFile(&fMtl, MODELFILETYPE_MTL);
	StartReposition();

	NextFile(&fObj, obj, MODELFILETYPE_OBJ);
	ExportObject(obj, &storedV);
	NextRepostion(obj);
	if (cfg->export_mtl) {
		NextFile(&fMtl, obj, MODELFILETYPE_MTL);
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
		filepath = cfg->export_folder.c_str();
		filepath /= "3dsmax.ms";
		frepos_3dsmax = fopen(filepath.string().c_str(), "w");
		if (frepos_3dsmax == NULL) throw std::bad_alloc();

		//write bom
		if (cfg->use_utf8_script) {
			::string_helper::write_utf8bom(frepos_3dsmax);
		}

		//write header
		fputs("fn tryModify obj mat = (\n", frepos_3dsmax);
		fputs("    try obj.transform = mat catch ()\n", frepos_3dsmax);
		fputs(")\n", frepos_3dsmax);
	}
	if (cfg->reposition_blender) {
		filepath = cfg->export_folder.c_str();
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
		GenObjMtlName(obj, &obj_name, WRITTENFILETYPE_SCRIPT);
		fprintf(frepos_3dsmax, "tryModify $%s (matrix3 [%f, %f, %f] [%f, %f, %f] [%f, %f, %f] [%f, %f, %f])\n",
			obj_name.c_str(),
			cacheMat[0][0], cacheMat[0][2], cacheMat[0][1],
			cacheMat[2][0], cacheMat[2][2], cacheMat[2][1],
			cacheMat[1][0], cacheMat[1][2], cacheMat[1][1],
			cacheMat[3][0], cacheMat[3][2], cacheMat[3][1]);
	}
	if (frepos_blender != NULL) {
		VxMatrix cacheMat = obj->GetWorldMatrix();
		GenObjMtlName(obj, &obj_name, WRITTENFILETYPE_SCRIPT);
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

void obj_export::StartFile(FILE** fs, ModelFileType target_fs) {
	if (cfg->file_mode == FILEMODE_ONEFILE) {
		if (*fs == NULL) {
			*fs = OpenObjMtlFile(NULL, target_fs);
			if (*fs == NULL) throw std::bad_alloc();
		}
	}
}
void obj_export::NextFile(FILE** fs, CKObject* obj, ModelFileType target_fs) {
	if (cfg->file_mode == FILEMODE_MULTIFILE) {
		//close file
		if (*fs != NULL) fclose(*fs);

		//open new file
		*fs = OpenObjMtlFile(obj, target_fs);
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
		GenObjMtlName(cfg->file_mode == FILEMODE_MULTIFILE ? obj : NULL,
			&mtllib_name, WRITTENFILETYPE_OBJMTL
		);

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
	GenObjMtlName(obj, &obj_name, WRITTENFILETYPE_OBJMTL);
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
	CKMaterial* fmtl = NULL, * prevMtl = NULL;
	for (int i = 0; i < count; i++) {
		//usemtl
		if (cfg->export_mtl) {
			CKMaterial* fmtl = mesh->GetFaceMaterial(i);
			if (fmtl != prevMtl || i == 0) {
				// if current mtl is not same with prev mtl,
				// or exporter are processing first face, 
				// need use `usemtl` command to change used material
				if (fmtl != NULL) {
					matList->insert(fmtl->GetID());
					GenObjMtlName(fmtl, &obj_name, WRITTENFILETYPE_OBJMTL);
					fprintf(fObj, "usemtl %s\n", obj_name.c_str());
				} else fputs("usemtl off\n", fObj);

				prevMtl = fmtl;
			}

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
	std::string filename;

	//basic
	GenObjMtlName(mtl, &mat_name, WRITTENFILETYPE_OBJMTL);
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

	GenCKTextureName4File(texture, &filename);
	fprintf(fMtl, "map_Kd %s\n", filename.c_str());

	//export texture
	if (!cfg->copy_texture) return;
	if (txurList->find(texture->GetID()) != txurList->end()) return;	// existed item, file has been copied, skip
	txurList->insert(texture->GetID());	// otherwise ass it in set

	CopyTextureFile(texture);
}


BOOL obj_export::ValidateObjectLegal(CK3dEntity* obj) {
	CKMesh* mesh = obj->GetCurrentMesh();
	if (mesh == NULL) return FALSE;					//no mesh
	if (mesh->GetFaceCount() == 0) return FALSE;	//mo face
	return TRUE;
}

void obj_export::GenCKObjectName(CKObject* obj, std::string* name) {
	// this function will generate native CKObject name.
	std::string native_name, full_name;

	// obj == NULL mean that File mode is: every objects in on file.
	// return "all" instead
	if (obj == NULL) {
		(*name) = "all";
		return;
	}

	// copy from virtools or generate default name
	if (obj->GetName() != NULL) {
		native_name = obj->GetName();
		RegulateName(&native_name);
	} else {
		::string_helper::stdstring_sprintf(&native_name, "noname_%d", obj->GetID());
	}

	// add name prefix
	if (cfg->name_prefix)
		::string_helper::stdstring_sprintf(name, "obj%d_%s", obj->GetID(), native_name.c_str());
	else
		(*name) = native_name.c_str();
}

void obj_export::GenObjMtlName(CKObject* obj, std::string* name, WrittenFileType target_fs) {
	// this function will generate CKObject name used in file body.
	std::string name_cache;
	GenCKObjectName(obj, &name_cache);

	// conv encoding with fall back
	::string_helper::encoding_conv(&name_cache, name,
		cfg->use_custom_encoding ? cfg->composition_encoding : CP_ACP,
		target_fs == WRITTENFILETYPE_SCRIPT ? (cfg->use_utf8_script ? CP_UTF8 : CP_ACP) :
		(cfg->use_utf8_obj ? CP_UTF8 : CP_ACP));
}

FILE* obj_export::OpenObjMtlFile(CKObject* obj, ModelFileType target_fs) {
	// this function will generate CKObject name used in Win32 function ended with W
	// and execute it.
	std::string mbname;
	std::wstring wsname;
	GenCKObjectName(obj, &mbname);

	std::filesystem::path fp(cfg->export_folder.c_str());
	if (::string_helper::conv_string2wstring(&mbname, &wsname,
		cfg->use_custom_encoding ? cfg->composition_encoding : CP_ACP)) {

		// conv successfully, use W function
		fp /= wsname.c_str();
		fp.replace_extension(target_fs == MODELFILETYPE_OBJ ? L".obj" : L".mtl");

		return _wfopen(fp.wstring().c_str(), L"w");
	} else {
		// conv failed, use A function for falling back
		fp /= mbname.c_str();
		fp.replace_extension(target_fs == MODELFILETYPE_OBJ ? ".obj" : ".mtl");

		return fopen(fp.string().c_str(), "w");
	}
}

void obj_export::GenCKTextureName(CKTexture* obj, std::wstring* name) {
	// this function will generate CKObject name used in file body.
	std::string native_file;
	std::wstring wcfile;
	std::filesystem::path filepath;

	std::wstring real_name;

	// convert file path into utf8 style
	native_file = obj->GetSlotFileName(0);
	if (::string_helper::conv_string2wstring(&native_file, &wcfile,
		cfg->custom_texture_format ? cfg->composition_encoding : CP_ACP)) {
		// init path with wstring
		filepath = wcfile.c_str();
	} else {
		// fall back. init path with string
		filepath = native_file.c_str();
	}

	// suffix process
	if (cfg->custom_texture_format) {
		filepath += ".";
		filepath += cfg->texture_format.c_str();
	}

	(*name) = filepath.filename().wstring().c_str();
	RegulateTextureFilename(name);
}

void obj_export::GenCKTextureName4File(CKTexture* obj, std::string* name) {
	std::wstring wscache;
	GenCKTextureName(obj, &wscache);

	if (!::string_helper::conv_wstring2string(&wscache, name,
		cfg->use_utf8_obj ? CP_UTF8 : CP_ACP)) {

		// failed. fall back to original name
		std::filesystem::path filepath;
		filepath = obj->GetSlotFileName(0);

		if (cfg->custom_texture_format) {
			filepath += ".";
			filepath += cfg->texture_format.c_str();
		}
		(*name) = filepath.filename().string().c_str();
		RegulateTextureFilename(name);
	}
}

void obj_export::CopyTextureFile(CKTexture* texture) {
	// get target file path
	std::wstring wscache, system_temp;
	std::filesystem::path fp(cfg->export_folder.c_str()), fpcache;
	GenCKTextureName(texture, &wscache);
	fp /= wscache.c_str();

	// get cache file path
	system_temp.resize(MAX_PATH);
	GetTempPathW(MAX_PATH, system_temp.data());
	fpcache = system_temp.c_str();
	fpcache /= "fc57c274-7f05-4e89-bbf0-678ad31c6774";	// a random generated GUID for this app
	fpcache.replace_extension(fp.extension().wstring().c_str());

	// export
	texture->SaveImage((CKSTRING)fpcache.string().c_str(), 0, FALSE);
	DeleteFileW(fp.wstring().c_str());
	MoveFileW(fpcache.wstring().c_str(), fp.wstring().c_str());

}

void obj_export::RegulateName(std::string* str) {
	for (auto it = str->begin(); it != str->end(); it++) {
		if (*it == '\0') break;

		// check eliminate non-ascii
		if (cfg->eliminate_non_ascii && ((*it) & 0b10000000u)) {
			*it = '_';
		}

		if (*it == ' ' || //obj ban
			*it == '\'' || //blender ban
			*it == '$' || //3dsmax ban
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

void obj_export::RegulateTextureFilename(std::string* str) {
	for (auto it = str->begin(); it != str->end(); it++) {
		if (*it == '\0') break;

		// check eliminate non-ascii
		if (cfg->eliminate_non_ascii && ((*it) & 0b10000000u)) {
			*it = '_';
		}

		if (*it == ' ' || //obj ban
			//*it == '\'' || //blender ban
			//*it == '$' || //3dsmax ban
			//*it == '.' ||
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
void obj_export::RegulateTextureFilename(std::wstring* str) {
	for (auto it = str->begin(); it != str->end(); it++) {
		if (*it == L'\0') break;

		// check eliminate non-ascii
		if (cfg->eliminate_non_ascii && ((*it) & 0b1111111110000000u)) {
			*it = L'_';
		}

		if (*it == L' ' || //obj ban
			//*it == L'\'' || //blender ban
			//*it == L'$' || //3dsmax ban
			//*it == L'.' ||
			*it == L'\\' || //file system ban
			*it == L'/' ||
			*it == L'*' ||
			*it == L'?' ||
			*it == L'"' ||
			*it == L'<' ||
			*it == L'>' ||
			*it == L'|')
			*it = L'_';
	}
}
