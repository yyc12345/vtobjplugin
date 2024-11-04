// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

#pragma region Windows Virtools Stuff
#include <YYCC/WinImportPrefix.hpp>

#include <SDKDDKVer.h>

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//PLUGIN PRECOMPILED HEADER INCLUDED
#include "CKAll.h"
#include "VIControls.h"
#include "CKControlsAll.h"
#include "VEP_ScriptActionMenu.h"
#include "VEP_KeyboardShortcutManager.h"
#include "VEP_All.h"

#include <YYCC/WinImportSuffix.hpp>
#pragma endregion

#pragma region YYCC Stuff
#include <YYCCommonplace.hpp>

#if YYCC_VERCMP_NE(YYCC_VER_MAJOR, YYCC_VER_MINOR, YYCC_VER_PATCH, 1, 3, 0)
#error "The version of provided YYCC library is not matched with we required. Please check your YYCC library installation."
#endif
#pragma endregion

