
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "fonts.h"
#include "res/pixeloperatormono_ttf.h"
#include "res/sniglet_ttf.h"

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif


/* XXX: need wxWidgets v3.1.1 for wxFont::AddPrivateFont
 *
 * Windows can use AddFontResource function for external
 * files & AddFontMemResourceEx for embedded data. XRC
 * may be another option:
 * 		http://students.mimuw.edu.pl/~bbekier/doxygen/out/html/overview_xrc.html
 */


wxFont font_large;
wxFont font_small;
wxFont font_changelog;

void* m_main;
void* m_changelog;

void initFonts() {
	// FIXME: font resources currently only work on Windows platform
#if defined(WIN32) || defined(WIN64)
	DWORD nFonts;
	m_main = AddFontMemResourceEx((PVOID) sniglet_ttf, sizeof(sniglet_ttf), NULL, &nFonts);
	m_changelog = AddFontMemResourceEx((PVOID) pixeloperatormono_ttf, sizeof(pixeloperatormono_ttf), NULL, &nFonts);

	font_large = wxFont(45, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, _T("Sniglet"));
	font_small = wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Sniglet"));
	font_changelog = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Pixel Operator Mono"));
#else
	// use generic fixed-width font
	font_changelog = wxFont(8, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
}

void cleanupFonts() {
#if defined(WIN32) || defined(WIN64)
	RemoveFontMemResourceEx(m_main);
	RemoveFontMemResourceEx(m_changelog);
#else
	delete m_main;
	delete m_changelog;
#endif
}
