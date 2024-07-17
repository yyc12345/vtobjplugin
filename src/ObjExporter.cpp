#include "ObjExporter.hpp"
#include <cstdarg>
#include <optional>

namespace vtobjplugin {

	class TextFileWriter {
	public:
		enum class Encoding {
			System, UTF8, UTF8WithBOM
		};
	public:
		TextFileWriter() : m_Fs(nullptr), m_Encoding(Encoding::System) {}
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
		TextFileWriter(const TextFileWriter&) = delete;
		TextFileWriter& operator=(const TextFileWriter&) = delete;
		TextFileWriter(TextFileWriter&& rhs) :
			m_Fs(rhs.m_Fs), m_Encoding(m_Encoding) {
			rhs.m_Fs = nullptr;
		}
		TextFileWriter& operator=(TextFileWriter&& rhs) {
			// clean self resource
			if (m_Fs != nullptr)
				std::fclose(m_Fs);
			// assign value
			m_Fs = rhs.m_Fs;
			m_Encoding = rhs.m_Encoding;
			// reset moved object
			rhs.m_Fs = nullptr;
			// return self
			return *this;
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
		GenerateScriptFile();
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
		uint64_t vertex_counter = UINT64_C(0);

		// define some useful macros
#define RIGHTHAND_POS_CONV(is_righthand, x, y, z) (x), ((is_righthand) ? (z) : (y)), ((is_righthand) ? (y) : (z))
#define RIGHTHAND_UV_CONV(is_righthand, u, v) (u), ((is_righthand) ? (-v) : (v))
#define RIGHTHAND_FACE_CONV(is_righthand, p1, p2, p3) \
((is_righthand) ? (p3) : (p1)), ((is_righthand) ? (p3) : (p1)), ((is_righthand) ? (p3) : (p1)), \
(p2), (p2), (p2), \
((is_righthand) ? (p1) : (p3)), ((is_righthand) ? (p1) : (p3)), ((is_righthand) ? (p1) : (p3))

		// iterate object list to export
		for (const auto& obj : file.m_ObjectList) {
			// get mesh
			CKMesh* mesh = obj.first->GetCurrentMesh();
			// get vertex count and face count
			int vertex_count = mesh->GetVertexCount();
			int face_count = mesh->GetFaceCount();

			// write v statement
			VxVector src_pos, transformed_pos;
			VxMatrix world_matrix = obj.first->GetWorldMatrix();
			for (int i = 0; i < vertex_count; ++i) {
				// get source position
				mesh->GetVertexPosition(i, &src_pos);
				// multiply with world matrix if necessary
				if (m_ExportSetting.m_IgnoreTransform) transformed_pos = src_pos;
				else Vx3DMultiplyMatrixVector(&transformed_pos, world_matrix, &src_pos);
				// correct position
				CorrectFloat(transformed_pos);
				// format into file with righthand option
				writer.WriteLine(YYCC_U8("v %f %f %f"),
					RIGHTHAND_POS_CONV(m_ExportSetting.m_ConvertToRightHand, transformed_pos.x, transformed_pos.y, transformed_pos.z)
				);
			}

			// write vt statement
			float uv_u, uv_v;
			for (int i = 0; i < vertex_count; ++i) {
				// get vertex texture
				mesh->GetVertexTextureCoordinates(i, &uv_u, &uv_v);
				// correct uv
				CorrectFloat(uv_u);
				CorrectFloat(uv_v);
				// format it into file with righthand option
				writer.WriteLine(YYCC_U8("vt %f %f 0"),
					RIGHTHAND_UV_CONV(m_ExportSetting.m_ConvertToRightHand, uv_u, uv_v)
				);
				// normalize it and correct position
				transformed_pos.Normalize();
				CorrectFloat(transformed_pos);
				// format into file with righthand option
				writer.WriteLine(YYCC_U8("vn %f %f %f"),
					RIGHTHAND_POS_CONV(m_ExportSetting.m_ConvertToRightHand, transformed_pos.x, transformed_pos.y, transformed_pos.z)
				);
			}

			// write vn statement
			VxVector src_nml, transformed_nml;
			for (int i = 0; i < vertex_count; ++i) {
				// get source normal
				mesh->GetVertexNormal(i, &src_nml);
				// process with world matrix if necessary
				if (m_ExportSetting.m_IgnoreTransform) transformed_nml = src_nml;
				else {
					// todo: finish normal transformation
					// this implementation is wrong
					Vx3DMultiplyMatrixVector(&transformed_pos, world_matrix, &src_pos);
				}
			}

			// write g or o statement to split object
			switch (m_ExportSetting.m_ObjectSplitMode) {
				case DataTypes::ObjectSplitMode::Group:
					writer.CriticalWriteLine(YYCC_U8("g %s"), obj.second.c_str());
					break;
				case DataTypes::ObjectSplitMode::Object:
					writer.CriticalWriteLine(YYCC_U8("o %s"), obj.second.c_str());
					break;
				default:
					throw std::runtime_error("invalid object split mode.");
			}

			// f and usemtl statement.
			CKMaterial* prev_mtl = nullptr;
			auto* face_indices = mesh->GetFacesIndices();
			for (int i = 0; i < face_count; ++i) {
				// usemtl statement
				if (m_ExportSetting.CanExportMaterial()) {
					// if current face material is not equal to previous face material
					// we need to use "usemtl" statement to switch it.
					CKMaterial* this_mtl = mesh->GetFaceMaterial(i);
					if (this_mtl != prev_mtl) {
						// if this face do not have material, we need write usemtl off
						// otherwise try to fetching material name
						if (this_mtl == nullptr) {
							writer.WriteLine(YYCC_U8("usemtl off"));
						} else {
							auto material_finder = file.m_MaterialMap.find(this_mtl);
							if (material_finder != file.m_MaterialMap.end()) {
								writer.CriticalWriteLine(YYCC_U8("usemtl %s"), material_finder->second.c_str());
							}
						}
						// update previous face material as this face material for next face.
						prev_mtl = this_mtl;
					}
				}

				// f statement
				// prepare point index
				uint64_t p1 = static_cast<uint64_t>(face_indices[i * 3]) + UINT64_C(1) + vertex_counter,
					p2 = static_cast<uint64_t>(face_indices[i * 3 + 1]) + UINT64_C(1) + vertex_counter,
					p3 = static_cast<uint64_t>(face_indices[i * 3 + 2]) + UINT64_C(1) + vertex_counter;
				// write into file considering right hand
				writer.WriteLine(
					YYCC_U8("f %" PRIu64 "/%" PRIu64 "/%" PRIu64 " %" PRIu64 "/%" PRIu64 "/%" PRIu64 " %" PRIu64 "/%" PRIu64 "/%" PRIu64 ""),
					RIGHTHAND_FACE_CONV(m_ExportSetting.m_ConvertToRightHand, p1, p2, p3)
				);
			}

			// accumulate vector counter
			vertex_counter += static_cast<uint64_t>(vertex_count);
		}

		//undef useful macros
#undef RIGHTHAND_POS_CONV
#undef RIGHTHAND_UV_CONV
#undef RIGHTHAND_FACE_CONV

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
			auto temp_file = temp_directory / YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("fc57c274-7f05-4e89-bbf0-678ad31c6774")); // a random generated GUID for this app
			temp_file.replace_extension(YYCC::FsPathPatch::FromUTF8Path(texture.second.c_str()).extension());

			// get temp file wchar style and ascii style
			auto u8_temp_file = YYCC::FsPathPatch::ToUTF8Path(temp_file);
			std::string acp_temp_file;
			std::wstring w_temp_file;
			if (!YYCC::EncodingHelper::UTF8ToChar(u8_temp_file, acp_temp_file, CP_ACP) || 
				!YYCC::EncodingHelper::UTF8ToWchar(u8_temp_file, w_temp_file)) {
				m_Reporter.Format(YYCC_U8("Fail to get temporary texture path: \"%s\". Skip texture copy."), u8_temp_file.c_str());
				continue;
			}

			// get destination file path and wchar style
			auto u8_dest_file = GetOtherFilePath(texture.second);
			std::wstring w_dest_file;
			if (!YYCC::EncodingHelper::UTF8ToWchar(u8_dest_file, w_dest_file)) {
				m_Reporter.Format(YYCC_U8("Fail to get texture path: \"%s\". Skip texture copy."), u8_dest_file.c_str());
				continue;
			}

			// save it by virtools function
			texture.first->SaveImage(const_cast<CKSTRING>(acp_temp_file.c_str()));

			// use W style Windows function 
			// to move saved texture to destination.
			DeleteFileW(w_dest_file.c_str());
			MoveFileW(w_temp_file.c_str(), w_dest_file.c_str());
		}
	}

	void ObjExporter::GenerateScriptFile() {
		// ===== prepare text file writer =====

		// prepare optional text writer
		std::optional<TextFileWriter> max_writer, bld_writer;

		// create 3ds max writer if necessary
		if (m_ExportSetting.CanGenerate3dsMaxScript()) {
			// get file path and encoding
			// UTF8 mode in 3ds max script always should has BOM.
			auto filepath = GetOtherFilePath(YYCC_U8("3dsmax.ms"));
			TextFileWriter::Encoding encoding = m_ExportSetting.m_UseUTF8Script ? TextFileWriter::Encoding::UTF8WithBOM : TextFileWriter::Encoding::System;
			// get writer
			max_writer = std::make_optional<TextFileWriter>(filepath.c_str(), encoding);
			if (!max_writer->CanWrite()) {
				m_Reporter.Format(YYCC_U8("Fail to create 3ds Max script file: \"%s\""), filepath.c_str());
				max_writer.reset();
			}
		}

		// create blender writer if necessary
		if (m_ExportSetting.CanGenerateBlenderScript()) {
			// get file path and encoding
			// blender script always use UTF8 encoding
			auto filepath = GetOtherFilePath(YYCC_U8("3dsmax.ms"));
			TextFileWriter::Encoding encoding = TextFileWriter::Encoding::UTF8;
			// get writer
			bld_writer = std::make_optional<TextFileWriter>(filepath.c_str(), encoding);
			if (!bld_writer->CanWrite()) {
				m_Reporter.Format(YYCC_U8("Fail to create Blender script file: \"%s\""), filepath.c_str());
				bld_writer.reset();
			}
		}

		// ===== write script headers =====

		// todo: improve 3ds max script feature that find object by name
		// write 3ds maxscript header
		if (max_writer.has_value()) {
			max_writer->WriteLine(YYCC_U8("fn tryModify obj mat = ("));
			max_writer->WriteLine(YYCC_U8("    try obj.transform = mat catch ()"));
			max_writer->WriteLine(YYCC_U8(")"));
		}

		// write script header
		if (bld_writer.has_value()) {
			max_writer->WriteLine(YYCC_U8("# -*- coding: UTF-8 -*-"));
			max_writer->WriteLine(YYCC_U8("import bpy"));
			max_writer->WriteLine(YYCC_U8("from mathutils import Matrix"));
			max_writer->WriteLine(YYCC_U8("def try_modify(obj, mat):"));
			max_writer->WriteLine(YYCC_U8("    try:"));
			max_writer->WriteLine(YYCC_U8("        bpy.data.objects[obj].matrix_world = mat.transposed()"));
			max_writer->WriteLine(YYCC_U8("    except:"));
			max_writer->WriteLine(YYCC_U8("        pass"));
		}

		// ===== write script body =====

		// write body for each object
		for (const auto& file : m_ExportLayoutWeaver.GetFileList()) {
			for (const auto& obj : file.m_ObjectList) {
				// get world matrix and correct it if possible
				VxMatrix mat = obj.first->GetWorldMatrix();
				CorrectFloat(mat);

				// write to 3ds max script
				if (max_writer.has_value()) {
					max_writer->CriticalWriteLine(YYCC_U8("tryModify $%s (matrix3 [%f, %f, %f] [%f, %f, %f] [%f, %f, %f] [%f, %f, %f])"),
						obj.second.c_str(),
						mat[0][0], mat[0][2], mat[0][1],
						mat[2][0], mat[2][2], mat[2][1],
						mat[1][0], mat[1][2], mat[1][1],
						mat[3][0], mat[3][2], mat[3][1]
					);
				}

				// write to blender script if possible
				if (bld_writer.has_value()) {
					bld_writer->CriticalWriteLine(YYCC_U8("try_modify('%s', Matrix(((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))))"),
						obj.second.c_str(),
						mat[0][0], mat[0][2], mat[0][1], mat[0][3],
						mat[2][0], mat[2][2], mat[2][1], mat[2][3],
						mat[1][0], mat[1][2], mat[1][1], mat[1][3],
						mat[3][0], mat[3][2], mat[3][1], mat[3][3]
					);
				}
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
