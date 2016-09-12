#include "main.h"
#include "abc.h"

IMPLEMENT_APP(ABCApp)

ABCApp::~ABCApp() {
	std::cout << "Closing SDL audio ..." << std::endl;
	Mix_CloseAudio();
	SDL_Quit();
}

bool ABCApp::OnInit()
{
	std::cout << "Initializing SDL ..." << std::endl;
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cout << "ERROR: Could not initialize SDL" << std::endl;
		return false;
	}

	std::cout << "Initializing SDL_mixer ..." << std::endl;
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
		std::cout << "ERROR: Could not initialize SDL_mixer" << std::endl;
		return false;
	}

    ABCWindow *frame = new ABCWindow(_T(""));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}
