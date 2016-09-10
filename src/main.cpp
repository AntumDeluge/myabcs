#include "main.h"
#include "abc.h"

IMPLEMENT_APP(App)

bool App::OnInit()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		return false;

	// Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
		return false;

	// Destructor will clean up SDL

    MainWindow *frame = new MainWindow(_T(""));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}
