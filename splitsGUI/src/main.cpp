#include "main.h"

IMPLEMENT_APP(Main)

bool Main::OnInit()
{
	mzmSplit* m = new mzmSplit();
	m->Show(true);

	return true;
}