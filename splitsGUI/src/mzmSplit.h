#ifndef __MZMSPLIT_H__
#define __MZMSPLIT_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/wx.h>

class mzmSplit : public wxFrame
{
private:

protected:
	wxStatusBar* m_statusBar1;
	wxMenuBar* m_menubar1;
	wxMenu* m_FileMenu;
	wxMenu* m_EditMenu;
	wxMenu* m_HelpMenu;

public:

	mzmSplit();
	~mzmSplit();

	void OnOpen(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnSaveAs(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);

	void OnEditPrefs(wxCommandEvent &event);

protected:
	DECLARE_EVENT_TABLE()
};

#endif //__MZMSPLIT_H__