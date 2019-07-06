
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_FONTS_H
#define MYABCS_FONTS_H

#include <wx/font.h>


extern wxFont font_large;
extern wxFont font_small;
extern wxFont font_changelog;

extern void initFonts();

extern void cleanupFonts();


#endif /* MYABCS_FONTS_H */
