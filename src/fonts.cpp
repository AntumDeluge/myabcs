#include "fonts.h"
#include "res/PixelOperatorMono.ttf.h"

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

// DEBUG:
#include <iostream>
using namespace std;


/* XXX: need wxWidgets v3.1.1 for wxFont::AddPrivateFont
 *
 * Windows can use AddFontResource function for external
 * files & AddFontMemResourceEx for embedded data. XRC
 * may be another option:
 * 		http://students.mimuw.edu.pl/~bbekier/doxygen/out/html/overview_xrc.html
 */


wxFont font_changelog;

void* m_fonthandle;

void initFonts() {
	// FIXME: font resources currently only work on Windows platform
#if defined(WIN32) || defined(WIN64)
	DWORD nFonts;
	m_fonthandle = AddFontMemResourceEx((PVOID) PixelOperatorMono_ttf, sizeof(PixelOperatorMono_ttf), NULL, &nFonts);

	//font_changelog = wxFont("Pixel Operator Mono");
	font_changelog = wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL, false, _T("Pixel Operator Mono"), wxFONTENCODING_DEFAULT);

#endif
}

void cleanupFonts() {
#if defined(WIN32) || defined(WIN64)
	//RemoveFontResource(ttf_pom);
	RemoveFontMemResourceEx(m_fonthandle);
#else
	delete m_fonthandle;
#endif
}
