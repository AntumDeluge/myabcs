#ifndef MYABCS_MAIN_H
#define MYABCS_MAIN_H

#include <wx/app.h>

class App : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

#endif /* MYABCS_MAIN_H */
