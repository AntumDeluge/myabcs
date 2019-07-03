#include <iostream>
#include <string>
#include <wx/wx.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <pthread.h>

using namespace std;

static Mix_Chunk* chunk = NULL;
static int channel;

// main window instance
static wxFrame* frame;

void playSound(const string filename);

class Frame : public wxFrame
{
public:
	Frame(wxString title);
	void OnButton(wxCommandEvent& event);
private:
	void OnSoundOver(wxCommandEvent& event);
	static void* SoundThread(void* arg);
	pthread_t soundThread;
	int rc;
	wxButton* button;
};

Frame::Frame(wxString title) :
	wxFrame(NULL, wxID_NEW, title) {
	button = new wxButton(this, 1001, _T("Airplane"));

	button->Bind(wxEVT_BUTTON, &Frame::OnButton, this);

	Connect(wxID_ANY, 1002, wxCommandEventHandler(Frame::OnSoundOver), 0, this);
}

void Frame::OnButton(wxCommandEvent& event) {
	if (Mix_Playing(channel)) {
		Mix_HaltChannel(channel);
	}

	if (!Mix_Playing(channel)) {
		rc = pthread_create(&soundThread, NULL, SoundThread, this);
	}
}

void Frame::OnSoundOver(wxCommandEvent& event) {
	cout << "Sound finished\n";
}

void* Frame::SoundThread(void* arg) {
	//wxEvtHandler* obj = wxDynamicCast(arg, wxEvtHandler);
	playSound("../sound/airplane.wav");
	wxCommandEvent SoundOverEvent(1002, wxID_ANY);
	/*
	if (obj) {
		wxPostEvent(obj, SoundOverEvent);
	}
	*/
	wxPostEvent(frame, SoundOverEvent);

	return (void*) 0;
}

class App : public wxApp
{
public:
	virtual bool OnInit();
};

void playSound(wxCommandEvent& event);

bool App::OnInit() {
	frame = new Frame(_T("Test"));

	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

static App* app;

int initAudio() {

	// initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		return 1;
	}

	cout << "Initialized SDL audio" << endl;

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 4096) != 0) {
		cerr << "Unable to open audio mixer: " << Mix_GetError() << endl;
		return 1;
	}

	cout << "Opened audio mixer" << endl;
	return 0;
}

void playSound(const string filename) {
	chunk = Mix_LoadWAV(filename.c_str());

	if (chunk == NULL) {
		cerr << "Could not load audio file: " << Mix_GetError() << endl;
	}

	channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel != 0) {
		cerr << "Unable to play file: " << Mix_GetError() << endl;
	}

	cout << "Playing sound: " + filename << "\n";

	while (Mix_Playing(channel) != 0);
}

void playSound(wxCommandEvent& event) {
	playSound("sound/alpha/A.wav");
}

void cleanup() {
	Mix_FreeChunk(chunk);
	Mix_CloseAudio();
	SDL_Quit();
}

int initGUI(int argc, char** argv) {
	app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 1;
}

int main(int argc, char** argv) {
	initAudio();

	initGUI(argc, argv);

	cleanup();
	return 0;
}
