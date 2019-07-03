#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <wx/wx.h>

using namespace std;


wxFrame* mwInstance;

void setMainWindowInstance(wxFrame* f) {
	mwInstance = f;
}

wxFrame* getMainWindowInstance() {
	return mwInstance;
}



const int ID_THREADEND = wxNewId();
pthread_t thread_id;

void* threadIt(void* arg);


class MainWindow : public wxFrame {
public:
	MainWindow();
	wxButton* getButton() { return button; }
private:
	void ChangeColor();
	void OnButton(wxCommandEvent& event);
	void OnThreadFinish(wxCommandEvent& event);

	wxPanel* bg;
	wxButton* button;
} *frame;


class App : public wxApp {
public:
	virtual bool OnInit();
};


wxIMPLEMENT_APP(App);


MainWindow::MainWindow() :
		wxFrame(NULL, wxID_ANY, _T("Catch Event from Thread")) {

	bg = new wxPanel(this, wxID_ANY);

	button = new wxButton(bg, wxID_ANY, _T("Enter Thread"));
	button->Connect(wxEVT_BUTTON, wxCommandEventHandler(MainWindow::OnButton), 0, this);
	Connect(wxID_ANY, ID_THREADEND, wxCommandEventHandler(MainWindow::OnThreadFinish), 0, this);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(button, 1, wxEXPAND);

	bg->SetSizer(sizer);
	bg->SetAutoLayout(true);
	bg->Layout();
}

void MainWindow::ChangeColor() {
	wxColour color = button->GetBackgroundColour();

	if (color == wxColour("red")) {
		cout << "Setting color to green ...\n";
		button->SetBackgroundColour("green");
	} else {
		cout << "Setting color to red ...\n";
		button->SetBackgroundColour("red");
	}
}

void MainWindow::OnButton(wxCommandEvent& event) {
	cout << "Button pressed\n";

	pthread_create(&thread_id, NULL, threadIt, NULL);
}

void MainWindow::OnThreadFinish(wxCommandEvent& event) {
	cout << "Thread finished\n";

	ChangeColor();
}


bool App::OnInit() {
	setMainWindowInstance(new MainWindow());
	frame = (MainWindow*) getMainWindowInstance();
	SetTopWindow(frame);
	frame->Show();

	return true;
}


void* threadIt(void* arg) {
	cout << "Entered thread ...\n";

	sleep(3);

	cout << "Exiting thread ...\n";

	wxCommandEvent testEvent(ID_THREADEND, wxID_ANY);
	wxPostEvent(getMainWindowInstance(), testEvent);

	return (void*) 0;
}

