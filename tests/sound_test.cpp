#include <iostream>
#include <fstream>
#include <string>
#include <utility> // pair, make_pair
#include <wx/wx.h>
#include <wx/sound.h>
using namespace std;


/*
pair<char*, size_t> loadSound(string filename) {
	fstream wavinput;
	wavinput.open(filename.c_str(), fstream::in|fstream::binary);

	int data_length;
	wavinput.seekg(0, fstream::end);
	data_length = wavinput.tellg();
	wavinput.seekg(0, fstream::beg);

	char data[data_length];
	wavinput.read(data, data_length);

	wavinput.close();

	pair<char*, size_t> ret;
	ret = make_pair(data, data_length);

	return ret;
}
*/

const string filename = "../sound/dog.wav";

class App : public wxApp {
public:
	virtual bool OnInit();
private:
	//wxSound* sound;

	void OnButton(wxCommandEvent& event);
};

bool App::OnInit() {
	// load sound data from memory
	/*
	pair<unsigned char*, size_t> data = loadSound(filename);
	sound(data.second, data.first);
	*/

	// load sound data from file
	//sound = new wxSound(wxString(filename));

	wxFrame* frame = new wxFrame(NULL, wxID_NEW, _T("wxSound Test"));
	frame->Show(true);

	const int ID_PLAY = wxNewId();
	wxButton* play = new wxButton(frame, ID_PLAY, _T("Play Sound"));

	play->Connect(wxEVT_BUTTON, wxCommandEventHandler(App::OnButton), 0, frame);

	SetTopWindow(frame);

	return true;
}

void App::OnButton(wxCommandEvent& event) {
	// load sound data from external file
	//wxSound snd(wxString(filename), false);

	// load sound data from memory
	fstream wavinput;
	wavinput.open(filename.c_str(), fstream::in|fstream::binary);

	int data_length;
	wavinput.seekg(0, fstream::end);
	data_length = wavinput.tellg();
	wavinput.seekg(0, fstream::beg);

	char data[data_length];
	wavinput.read(data, data_length);

	wavinput.close();

	wxSound snd(data_length, data);


	if (snd.IsOk()) {
		cout << "Playing sound ...\n";
		snd.Play();
	} else {
		cout << "Error loading sound\n";
	}
}

int main(int argc, char** argv) {
	App* app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
