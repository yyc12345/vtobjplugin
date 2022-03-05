#include "stdafx.h"
#include "config_manager.h"
#include "buffer_helper.h"
#include <filesystem>

#define APP_VERSION 140

extern PluginInterface* s_Plugininterface;

void config_manager::GetConfigFilePath(std::string* buffer) {
	std::filesystem::path file(s_Plugininterface->GetVirtoolsDirectory());
	file /= "vtobjplugin.cfg";
	(*buffer) = file.string().c_str();
}

void config_manager::LoadConfig(ExportConfig* cfg) {
	// init necessary variable before goto
	std::string temp_str, cfg_file_path;
	int temp_int = 0;
	int gotten_version;

	GetConfigFilePath(&cfg_file_path);
	FILE* f = fopen(cfg_file_path.c_str(), "rb");
	if (f == NULL) goto needInitCfg;

	//read config
#define readint(target,conv) fread(&target, sizeof(int), 1, f);
#define readstr(target) fread(&temp_int, sizeof(size_t), 1, f);\
	target.resize(temp_int);\
	fread(target.data(), sizeof(char), temp_int, f);\

	//check version
	readint(gotten_version);
	if (gotten_version != APP_VERSION) {
		fclose(f);
		goto needInitCfg;
	}

	readint(cfg->export_mode, ExportMode);
	cfg->selected_item = 0;
	readint(cfg->file_mode, FileMode);
	readstr(cfg->export_folder);
	readint(cfg->omit_transform, BOOL);
	readint(cfg->right_hand, BOOL);
	readint(cfg->name_prefix, BOOL);
	readint(cfg->use_group_split_object, BOOL);
	readint(cfg->eliminate_non_ascii, BOOL);
	readint(cfg->reposition_3dsmax, BOOL);
	readint(cfg->reposition_blender, BOOL);
	readint(cfg->export_mtl, BOOL);
	readint(cfg->export_texture, BOOL);
	readint(cfg->copy_texture, BOOL);
	readint(cfg->custom_texture_format, BOOL);
	readstr(cfg->texture_format);
	readint(cfg->use_custom_encoding, BOOL);
	readint(cfg->composition_encoding, UINT);
	readint(cfg->use_utf8_obj, BOOL);
	readint(cfg->use_utf8_script, BOOL);

#undef readint
#undef readstr

	fclose(f);
	return;

needInitCfg:
	//no config, set default and save it.
	cfg->export_mode = EXPORTMODE_GROUP;
	cfg->selected_item = 0;
	cfg->file_mode = FILEMODE_ONEFILE;
	cfg->export_folder = "";
	cfg->omit_transform = TRUE;
	cfg->right_hand = TRUE;
	cfg->name_prefix = FALSE;
	cfg->use_group_split_object = TRUE;
	cfg->eliminate_non_ascii = FALSE;
	cfg->reposition_3dsmax = FALSE;
	cfg->reposition_blender = FALSE;
	cfg->export_mtl = TRUE;
	cfg->export_texture = TRUE;
	cfg->copy_texture = FALSE;
	cfg->custom_texture_format = FALSE;
	cfg->texture_format = "bmp";
	cfg->use_custom_encoding = FALSE;
	cfg->composition_encoding = CP_ACP;
	cfg->use_utf8_obj = FALSE;
	cfg->use_utf8_script = FALSE;

	SaveConfig(cfg);
	return;
}

void config_manager::SaveConfig(ExportConfig* cfg) {
	std::string cfg_file_path;
	GetConfigFilePath(&cfg_file_path);
	FILE* f = fopen(cfg_file_path.c_str(), "wb");
	assert(f != NULL);

	const char* temp_str = NULL;
	int temp_int = 0;
#define writeconstint(target) temp_int=target;fwrite(&temp_int, sizeof(int), 1, f);
#define writeint(target) fwrite(&target, sizeof(int), 1, f);
#define writestr(target) temp_str=target;temp_int=strlen(temp_str);fwrite(&temp_int, sizeof(size_t), 1, f);fwrite(temp_str, sizeof(char), temp_int, f);

	writeconstint(APP_VERSION);
	writeint(cfg->export_mode);
	writeint(cfg->file_mode);
	writestr(cfg->export_folder.c_str());
	writeint(cfg->omit_transform);
	writeint(cfg->right_hand);
	writeint(cfg->name_prefix);
	writeint(cfg->use_group_split_object);
	writeint(cfg->eliminate_non_ascii);
	writeint(cfg->reposition_3dsmax);
	writeint(cfg->reposition_blender);
	writeint(cfg->export_mtl);
	writeint(cfg->export_texture);
	writeint(cfg->copy_texture);
	writeint(cfg->custom_texture_format);
	writestr(cfg->texture_format.c_str());
	writeint(cfg->use_custom_encoding);
	writeint(cfg->composition_encoding);
	writeint(cfg->use_utf8_obj);
	writeint(cfg->use_utf8_script);

#undef writeconstint
#undef writeint
#undef writestr

	fclose(f);
}