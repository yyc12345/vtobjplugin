#include "stdafx.h"
#include "config_manager.h"
#include "buffer_helper.h"
#include <filesystem>

#define APP_VERSION 130

extern PluginInterface* s_Plugininterface;

void config_manager::GetConfigFilePath(char* buffer) {
	std::filesystem::path file(s_Plugininterface->GetVirtoolsDirectory());
	file /= "vtobjplugin.cfg";
	strcpy(buffer, file.string().c_str());
}

void config_manager::LoadConfig(ExportConfig* cfg) {
	GetConfigFilePath(buffer_helper::global_buffer);

	// init necessary variable before goto
	std::string temp_str;
	int temp_int = 0;
	int gotten_version;

	FILE* f = fopen(buffer_helper::global_buffer, "rb");
	if (f == NULL) goto needInitCfg;

	//read config
#define readint(target,conv) fread(&target, sizeof(int), 1, f);
#define readstr(target) fread(&temp_int, sizeof(size_t), 1, f);fread(buffer_helper::global_buffer, sizeof(char), temp_int, f);buffer_helper::global_buffer[temp_int]='\0';target=buffer_helper::global_buffer;

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
	readint(cfg->reposition_3dsmax, BOOL);
	readint(cfg->reposition_blender, BOOL);
	readint(cfg->export_mtl, BOOL);
	readint(cfg->export_texture, BOOL);
	readint(cfg->copy_texture, BOOL);
	readint(cfg->custom_texture_format, BOOL);
	readstr(cfg->texture_format);

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
	cfg->reposition_3dsmax = FALSE;
	cfg->reposition_blender = FALSE;
	cfg->export_mtl = TRUE;
	cfg->export_texture = TRUE;
	cfg->copy_texture = FALSE;
	cfg->custom_texture_format = FALSE;
	cfg->texture_format = "bmp";

	SaveConfig(cfg);
	return;
}

void config_manager::SaveConfig(ExportConfig* cfg) {
	GetConfigFilePath(buffer_helper::global_buffer);

	FILE* f = fopen(buffer_helper::global_buffer, "wb");
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
	writeint(cfg->reposition_3dsmax);
	writeint(cfg->reposition_blender);
	writeint(cfg->export_mtl);
	writeint(cfg->export_texture);
	writeint(cfg->copy_texture);
	writeint(cfg->custom_texture_format);
	writestr(cfg->texture_format.c_str());

#undef writeconstint
#undef writeint
#undef writestr

	fclose(f);
}