#include "ObjExporter.hpp"
#include <cstdarg>

namespace vtobjplugin {

	class TextFileWriter {
	public:
		enum class Encoding {
			System, UTF8, UTF8WithBOM
		};
	public:
		TextFileWriter(const YYCC::yycc_char8_t* filename, Encoding encoding) :
			m_Fs(nullptr), m_Encoding(encoding) {
			// try to open file
			// we open it in binary because we want to process any possible details in output file.
			m_Fs = YYCC::IOHelper::UTF8FOpen(filename, YYCC_U8("wb"));
			// if encoding is utf8 with BOM, write BOM first
			RawWrite(YYCC_U8("\xef\xbb\xbf"));
		}
		~TextFileWriter() {
			if (m_Fs != nullptr)
				std::fclose(m_Fs);
		}

	public:
		bool CanWrite() const { return m_Fs != nullptr; }
		/**
		 * @brief Write text into file with auto EOL.
		 * @param[in] fmt The format string.
		 * @param[in] ... The argument used by format string.
		*/
		void WriteLine(const YYCC::yycc_char8_t* fmt, ...) {
			if (m_Fs != nullptr) {
				// write formatted string
				va_list argptr;
				va_start(argptr, fmt);
				RawWrite(YYCC::StringHelper::VPrintf(fmt, argptr));
				va_end(argptr);
				// write EOL
				RawWrite(YYCC_U8("\r\n"));
			}
		}
		/**
		 * @brief Write text into file with auto EOL.
		 * @details
		 * Both this function and WriteLine accept UTF8 format string and argument list as parameters.
		 * But this function is different with WriteLine.
		 * WriteLine will write formatted string into file as binary mode directly.
		 * This function will try convert it to system code page first (if encoding is not UTF8),
		 * then write it into file.
		 *
		 * WriteLine output is suit for OBJ statements which can not have non-ASCII, such as "v", "vt", "vn" statements.
		 * This output function is suit for that statement which may contain non-ASCII characters, such as "group", "object", "mtllib" statements.
		 * That's the reason why this function called "Critical".
		 *
		 * The reason why all statements use this function is encoding convertion need consume a bunch of performance.
		 * To improve export experience, there is no need to do encoding convertion for that statements which absolutely do not contain non-ASCII characters.
		 * @param[in] fmt The format string.
		 * @param[in] ... The argument used by format string.
		*/
		void CriticalWriteLine(const YYCC::yycc_char8_t* fmt, ...) {
			if (m_Fs != nullptr) {
				// write formatted string
				va_list argptr;
				va_start(argptr, fmt);
				if (m_Encoding == Encoding::System) {
					// system encoding, do convertion
					auto result = YYCC::EncodingHelper::UTF8ToChar(YYCC::StringHelper::VPrintf(fmt, argptr), CP_ACP);
					RawWrite(YYCC::EncodingHelper::ToUTF8View(result));
				} else {
					// utf8 encoding, write directly.
					RawWrite(YYCC::StringHelper::VPrintf(fmt, argptr));
				}
				va_end(argptr);
				// write EOL
				RawWrite(YYCC_U8("\r\n"));
			}
		}
	private:
		void RawWrite(const YYCC::yycc_u8string_view& strl) {
			if (m_Fs != nullptr)
				std::fwrite(strl.data(), strl.length(), sizeof(YYCC::yycc_u8string_view::value_type), m_Fs);
		}
	private:
		FILE* m_Fs;
		Encoding m_Encoding;
	};

#pragma region Correct Float

	/// Check whether given float is legal.
	/// If it is legal, return self, otherwise return given fallback
#define CORRECT_FLOAT(fval, fallback) ((std::isnormal(fval) || (fval) == 0.0f) ? (fval) : (fallback))

	static void CorrectFloat(float& f) {
		// correction with sign
		f = CORRECT_FLOAT(f, std::signbit(f) ? -1.0f : 1.0f);
	}

	static void CorrectFloat(VxVector& v) {
		// correct vec with its original sign.
		CorrectFloat(v.x);
		CorrectFloat(v.y);
		CorrectFloat(v.z);
	}

	static void CorrectFloat(VxMatrix& mat) {
		// main cross item should be corrected into 1.0f, others will be set as 0.0f.
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				mat[i][j] = CORRECT_FLOAT(mat[i][j], i == j ? 1.0f : 0.0f);
			}
		}
	}

	static void CorrectFloat(VxColor& c) {
		// color should always be corrected into zero.
		c.r = CORRECT_FLOAT(c.r, 0.0f);
		c.g = CORRECT_FLOAT(c.g, 0.0f);
		c.b = CORRECT_FLOAT(c.b, 0.0f);
	}

#undef CORRECT_FLOAT

#pragma endregion

#pragma region Obj Exporter

	ObjExporter::ObjExporter(
		const DataTypes::ExportSetting& export_setting,
		const ExportLayoutWeaver& export_layout_weaver,
		const Utilities::VirtoolsUIReporter reporter) :
		m_ExportSetting(export_setting),
		m_ExportLayoutWeaver(export_layout_weaver),
		m_Reporter(reporter) {}

	ObjExporter::~ObjExporter() {}

	void ObjExporter::DoExport() {
		// get layout data
		const auto& file_list = m_ExportLayoutWeaver.GetFileList();
		const auto& texture_map = m_ExportLayoutWeaver.GetTextureMap();

		// export file list
		for (const auto& file : file_list) {
			// export obj file
			ExportObjFile(file);
			// export mtl file if possible
			ExportMtlFile(file);
		}
		// Copy texture file if possible
		CopyTextureFile();
		// Generate scripts if possible
		Generate3dsMaxScriptFile();
		GenerateBlenderScriptFile();
	}

	void ObjExporter::ExportObjFile(const ExportLayoutWeaver::File_t& file) {
		// get text writer
		auto obj_filepath = GetObjMtlFilePath(file.m_FileName, ObjMtlFile::ObjFile);
		TextFileWriter::Encoding obj_encoding = m_ExportSetting.m_UseUTF8ObjMtl ? TextFileWriter::Encoding::UTF8 : TextFileWriter::Encoding::System;
		TextFileWriter writer(obj_filepath.c_str(), obj_encoding);
		if (!writer.CanWrite()) {
			m_Reporter.Format(YYCC_U8("Fail to create OBJ file: \"%s\""), obj_filepath.c_str());
			return;
		}

		// write mtllib statement if export material
		if (m_ExportSetting.CanExportMaterial()) {
			auto mtl_filepath = GetObjMtlFilePath(file.m_FileName, ObjMtlFile::ObjFile);
			writer.CriticalWriteLine(YYCC_U8("mtllib %s"), mtl_filepath.c_str());
		}

		// initialize vector counter
		uint64_t vector_counter = UINT64_C(0);

	}
	void ObjExporter::ExportMtlFile(const ExportLayoutWeaver::File_t& file) {
		// check whether export material
		if (!m_ExportSetting.CanExportMaterial()) return;

		// get text writer
		auto mtl_filepath = GetObjMtlFilePath(file.m_FileName, ObjMtlFile::ObjFile);
		TextFileWriter::Encoding mtl_encoding = m_ExportSetting.m_UseUTF8ObjMtl ? TextFileWriter::Encoding::UTF8 : TextFileWriter::Encoding::System;
		TextFileWriter writer(mtl_filepath.c_str(), mtl_encoding);
		if (!writer.CanWrite()) {
			m_Reporter.Format(YYCC_U8("Fail to create MTL file: \"%s\""), mtl_filepath.c_str());
			return;
		}

		// iterate material list
		for (const auto& mtl_pair : file.m_MaterialMap) {
			// newmtl statement
			writer.CriticalWriteLine(YYCC_U8("newmtl %s"), mtl_pair.second.c_str());

			// basic color
			VxColor col;
			col = mtl_pair.first->GetAmbient();
			CorrectFloat(col);
			writer.WriteLine(YYCC_U8("Ka %f %f %f"), col.r, col.g, col.b);
			col = mtl_pair.first->GetDiffuse();
			CorrectFloat(col);
			writer.WriteLine(YYCC_U8("Kd %f %f %f"), col.r, col.g, col.b);
			col = mtl_pair.first->GetEmissive();
			CorrectFloat(col);
			writer.WriteLine(YYCC_U8("Ks %f %f %f"), col.r, col.g, col.b);

			// texture
			if (m_ExportSetting.CanExportTexture()) {
				// try to get texture
				CKTexture* texture = mtl_pair.first->GetTexture();
				if (texture == nullptr) continue;

				// find texture name
				const auto& texture_map = m_ExportLayoutWeaver.GetTextureMap();
				auto texture_finder = texture_map.find(texture);
				if (texture_finder != texture_map.end()) {
					// write texture file name if found
					writer.CriticalWriteLine(YYCC_U8("map_Kd %s"), texture_finder->second.c_str());
				}
			}
		}

	}

	void ObjExporter::CopyTextureFile() {
		// check whether copy texture file
		if (!m_ExportSetting.CanCopyTexture()) return;

		// get windows temp directory
		YYCC::yycc_u8string u8_temp_directory;
		if (!YYCC::WinFctHelper::GetTempDirectory(u8_temp_directory)) {
			m_Reporter.Write(YYCC_U8("Fail to get Windows temp path, thus we can not copy texture."));
			return;
		}
		std::filesystem::path temp_directory(YYCC::FsPathPatch::FromUTF8Path(u8_temp_directory.c_str()));

		// iterate texture list
		for (const auto& texture : m_ExportLayoutWeaver.GetTextureMap()) {
			// build temporary file path
			// get texture file name and replace its stem to our GUID
			// because we want to keep its extension 
			// which Virtools rely on it to decide the file type it should output.


		}
	}

	void ObjExporter::Generate3dsMaxScriptFile() {
		// check whether generate script
		if (!m_ExportSetting.CanGenerate3dsMaxScript()) return;

		// get file path and encoding
		// UTF8 mode in 3ds max script always should has BOM.
		auto filepath = GetOtherFilePath(YYCC_U8("3dsmax.ms"));
		TextFileWriter::Encoding encoding = m_ExportSetting.m_UseUTF8Script ? TextFileWriter::Encoding::UTF8WithBOM : TextFileWriter::Encoding::System;
		// get writer
		TextFileWriter writer(filepath.c_str(), encoding);
		if (!writer.CanWrite()) {
			m_Reporter.Format(YYCC_U8("Fail to create 3ds Max script file: \"%s\""), filepath.c_str());
			return;
		}

		// todo: improve 3ds max script feature that find object by name
		// write script header
		writer.WriteLine(YYCC_U8("fn tryModify obj mat = ("));
		writer.WriteLine(YYCC_U8("    try obj.transform = mat catch ()"));
		writer.WriteLine(YYCC_U8(")"));

		// write body for each object
		for (const auto& file : m_ExportLayoutWeaver.GetFileList()) {
			for (const auto& obj : file.m_ObjectList) {
				// get world matrix and correct it if possible
				VxMatrix mat = obj.first->GetWorldMatrix();
				CorrectFloat(mat);

				// write to file
				writer.CriticalWriteLine(YYCC_U8("tryModify $%s (matrix3 [%f, %f, %f] [%f, %f, %f] [%f, %f, %f] [%f, %f, %f])"),
					obj.second.c_str(),
					mat[0][0], mat[0][2], mat[0][1],
					mat[2][0], mat[2][2], mat[2][1],
					mat[1][0], mat[1][2], mat[1][1],
					mat[3][0], mat[3][2], mat[3][1]
				);
			}
		}
	}

	void ObjExporter::GenerateBlenderScriptFile() {
		// check whether generate script
		if (!m_ExportSetting.CanGenerateBlenderScript()) return;

		// get file path and encoding
		// blender script always use UTF8 encoding
		auto filepath = GetOtherFilePath(YYCC_U8("3dsmax.ms"));
		TextFileWriter::Encoding encoding = TextFileWriter::Encoding::UTF8;
		// get writer
		TextFileWriter writer(filepath.c_str(), encoding);
		if (!writer.CanWrite()) {
			m_Reporter.Format(YYCC_U8("Fail to create Blender script file: \"%s\""), filepath.c_str());
			return;
		}

		// write script header
		writer.WriteLine(YYCC_U8("# -*- coding: UTF-8 -*-"));
		writer.WriteLine(YYCC_U8("import bpy"));
		writer.WriteLine(YYCC_U8("from mathutils import Matrix"));
		writer.WriteLine(YYCC_U8("def try_modify(obj, mat):"));
		writer.WriteLine(YYCC_U8("    try:"));
		writer.WriteLine(YYCC_U8("        bpy.data.objects[obj].matrix_world = mat.transposed()"));
		writer.WriteLine(YYCC_U8("    except:"));
		writer.WriteLine(YYCC_U8("        pass"));

		// write body for each object
		for (const auto& file : m_ExportLayoutWeaver.GetFileList()) {
			for (const auto& obj : file.m_ObjectList) {
				// get world matrix and correct it if possible
				VxMatrix mat = obj.first->GetWorldMatrix();
				CorrectFloat(mat);

				// write to file
				writer.CriticalWriteLine(YYCC_U8("try_modify('%s', Matrix(((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))))"),
					obj.second.c_str(),
					mat[0][0], mat[0][2], mat[0][1], mat[0][3],
					mat[2][0], mat[2][2], mat[2][1], mat[2][3],
					mat[1][0], mat[1][2], mat[1][1], mat[1][3],
					mat[3][0], mat[3][2], mat[3][1], mat[3][3]
				);
			}
		}

	}

	YYCC::yycc_u8string ObjExporter::GetObjMtlFilePath(const YYCC::yycc_u8string_view& filename_stem, ObjMtlFile file_type) {
		// get export firectory
		std::filesystem::path export_directory(
			YYCC::FsPathPatch::FromUTF8Path(m_ExportSetting.m_ExportDirectory.c_str())
		);
		// build filename
		YYCC::yycc_u8string u8_filename(filename_stem);
		switch (file_type) {
			case ObjMtlFile::ObjFile:
				u8_filename += YYCC_U8(".obj");
				break;
			case ObjMtlFile::MtlFile:
				u8_filename += YYCC_U8(".mtl");
				break;
			default:
				throw std::runtime_error("invalid obj mtl file type.");
		}
		// get full path output
		return YYCC::FsPathPatch::ToUTF8Path(export_directory / YYCC::FsPathPatch::FromUTF8Path(u8_filename.c_str()));
	}

	YYCC::yycc_u8string ObjExporter::GetOtherFilePath(const YYCC::yycc_u8string_view& filename) {
		// get export firectory
		std::filesystem::path export_directory(
			YYCC::FsPathPatch::FromUTF8Path(m_ExportSetting.m_ExportDirectory.c_str())
		);
		// get full path output
		return YYCC::FsPathPatch::ToUTF8Path(export_directory / YYCC::FsPathPatch::FromUTF8Path(filename.data()));
	}

#pragma endregion

}
