#include "mzmSplit.h"
BEGIN_EVENT_TABLE(mzmSplit, wxFrame)
EVT_MENU(wxID_OPEN, mzmSplit::OnOpen)
EVT_MENU(wxID_SAVE, mzmSplit::OnSave)
EVT_MENU(wxID_SAVEAS, mzmSplit::OnSaveAs)
EVT_MENU(wxID_EXIT, mzmSplit::OnExit)
EVT_MENU(wxID_ABOUT, mzmSplit::OnAbout)
EVT_MENU(wxID_PREFERENCES, mzmSplit::OnEditPrefs)
END_EVENT_TABLE()

mzmSplit::mzmSplit() : wxFrame(NULL, wxID_ANY, _T("MZM Split"), wxDefaultPosition, wxSize(322, 629))
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	m_statusBar1 = this->CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);
	m_menubar1 = new wxMenuBar(0);
	m_FileMenu = new wxMenu();
	wxMenuItem* m_FileOpen;
	m_FileOpen = new wxMenuItem(m_FileMenu, wxID_OPEN, wxString(wxT("Open")) + wxT('\t') + wxT("CTRL-O"), wxT("Open an XML-encoded split file"), wxITEM_NORMAL);

	m_FileOpen->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));

	m_FileMenu->Append(m_FileOpen);

	wxMenuItem* m_FileSave;
	m_FileSave = new wxMenuItem(m_FileMenu, wxID_SAVE, wxString(wxT("Save")) + wxT('\t') + wxT("CTRL-S"), wxT("Overwrite current splits"), wxITEM_NORMAL);

	m_FileSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));

	m_FileMenu->Append(m_FileSave);

	wxMenuItem* m_FileSaveAs;
	m_FileSaveAs = new wxMenuItem(m_FileMenu, wxID_SAVEAS, wxString(wxT("Save As...")) + wxT('\t') + wxT("CTRL-Shift-S"), wxT("Save splits to new file"), wxITEM_NORMAL);

	m_FileSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));

	m_FileMenu->Append(m_FileSaveAs);

	m_FileMenu->AppendSeparator();

	wxMenuItem* m_FileQuit;
	m_FileQuit = new wxMenuItem(m_FileMenu, wxID_EXIT, wxString(wxT("Quit")) + wxT('\t') + wxT("ALT-F4"), wxT("Quit the program"), wxITEM_NORMAL);

	m_FileQuit->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU));

	m_FileMenu->Append(m_FileQuit);

	m_menubar1->Append(m_FileMenu, wxT("File"));

	m_EditMenu = new wxMenu();
	wxMenuItem* m_EditPrefs;
	m_EditPrefs = new wxMenuItem(m_EditMenu, wxID_PREFERENCES, wxString(wxT("Preferences...")) + wxT('\t') + wxT("ALT-P"), wxT("Edit preferences"), wxITEM_NORMAL);

	m_EditPrefs->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_OTHER));

	m_EditMenu->Append(m_EditPrefs);

	m_menubar1->Append(m_EditMenu, wxT("Edit"));

	m_HelpMenu = new wxMenu();
	wxMenuItem* m_HelpAbout;
	m_HelpAbout = new wxMenuItem(m_HelpMenu, wxID_ABOUT, wxString(wxT("About")) + wxT('\t') + wxT("F1"), wxT("Show information about this program"), wxITEM_NORMAL);

	m_HelpAbout->SetBitmap(wxArtProvider::GetBitmap(wxART_TIP, wxART_MENU));

	m_HelpMenu->Append(m_HelpAbout);

	m_menubar1->Append(m_HelpMenu, wxT("Help"));

	this->SetMenuBar(m_menubar1);

	this->Centre(wxBOTH);
}

void mzmSplit::OnOpen(wxCommandEvent &event)
{
	wxFileDialog *OpenDialog = new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_OPEN);
	if (OpenDialog->ShowModal() == wxID_OK)
	{
		wxString path = OpenDialog->GetPath();
		path.IsAscii() ?
			SetStatusText(_T("Loaded")) :
			SetStatusText(_T("Load Failed"));
	}
	OpenDialog->Close(); // Or OpenDialog->Destroy() ?
}

void mzmSplit::OnSave(wxCommandEvent &event)
{
	wxFileDialog *SaveDialog = new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_SAVE);
	if (SaveDialog->ShowModal() == wxID_OK)
	{
		wxString path = SaveDialog->GetPath();
		path.IsAscii() ?
			SetStatusText(_T("Saved")) :
			SetStatusText(_T("Save Failed"));
	}
	SaveDialog->Close();
}

void mzmSplit::OnSaveAs(wxCommandEvent &event)
{
	wxFileDialog *SaveAsDialog = new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_SAVE);
	if (SaveAsDialog->ShowModal() == wxID_OK)
	{
		wxString path = SaveAsDialog->GetPath();
		path.IsAscii() ?
			SetStatusText(_T("Saved")) :
			SetStatusText(_T("Save Failed"));
	}
	SaveAsDialog->Close();
}

void mzmSplit::OnExit(wxCommandEvent &event)
{
	Close(false);
}

void mzmSplit::OnEditPrefs(wxCommandEvent &event)
{
	wxLogMessage(_T("TODO: Create preferences window"));
}

void mzmSplit::OnAbout(wxCommandEvent &event)
{
	wxLogMessage(_T("TODO: Create an about window"));
}

mzmSplit::~mzmSplit(){}