#include "split.h"

void Run::missilesCheck()
{
	ReadProcessMemory(window->phandle, (LPCVOID)((DWORD)window->iramAddr + MISSILE_OFFSET), (LPVOID)&inv->missilesValue, 2, &window->bytesRead);

	if ((inv->missilesValue & ~0xFFFFFF00) != 0)
	{
		inv->hasMissiles = true;
	}
}

void Run::supersCheck()
{
	ReadProcessMemory(window->phandle, (LPCVOID)((DWORD)window->iramAddr + SUPERS_OFFSET), (LPVOID)&inv->missilesValue, 2, &window->bytesRead);

	if ((inv->missilesValue & ~0xFF00FFFF) != 0)
	{
		inv->hasSupers = true;
	}
}

void Run::invCheck()
{
	ReadProcessMemory(window->phandle, (LPCVOID)((DWORD)window->iramAddr + INV_OFFSET), &inv->invValue, 4, &window->bytesRead);
	inv->iNew = (~0xFF00FF00 & inv->invValue);
	inv->iOld &= ~0xFF00FF00;

	if (inv->iOld < inv->iNew)
	{
		inv->hasInv = true;
	}
}

// Decided to merge parseTime & the time acquisition into one function
std::string Run::getTime()
{
	std::uint64_t time = 0;
	ReadProcessMemory(window->phandle, (LPCVOID)((DWORD)window->iramAddr + GAMETIME_OFFSET), &time, 4, &window->bytesRead);
	std::stringstream time_sstr;
	time_sstr << std::setfill('0') << std::setw(2) << (time & 0xFF) << ":" << std::setw(2) << ((time >> 8) & 0xFF) << ":" << std::setw(2) << ((time >> 16) & 0xFF) << "." << std::setw(3) << ((time >> 24) & 0xFF);
	return time_sstr.str();
}

void Run::loadSplits()
{
	std::ifstream splits("splits.xml");

	ptree pt;
	read_xml(splits, pt);

	for (auto i : pt.get_child("Run.Segments"))
	{
		Segment segment;
		segment.id = i.second.get<int>("<xmlattr>.id");
		segment.name = i.second.get<std::string>("Name");
		segment.pb = i.second.get<std::string>("PB");

		attempt->push_back(segment);
	}
}

int Run::printSplits()
{
	for (auto i : *attempt)
	{
		output(i.id << ", " << i.name << ", " << i.pb);
	}
	return 0;
}

int Run::saveSplits()
{
	std::ofstream splits("splits.xml", std::ostream::out);

	ptree tree;
	tree.add("Run.<xmlattr>.version", "1.0");

	std::vector<std::string> names, pb;

	for (auto i : *attempt)
	{
		names.push_back(i.name);
		pb.push_back(i.pb);
	}

	for (auto i = 0U; i < attempt->size(); i++)
	{
		ptree &segment = tree.add("Run.Segments.Segment", "");
		segment.add("Name", names.at(i));
		segment.add("PB", pb.at(i));
		segment.add("<xmlattr>.id", i);
	}

	write_xml(splits, tree);
	return 0;
}

int Run::doSplit()
{
	int count = 0;
	loadSplits();

	for (auto i : *attempt)
	{
		names.push_back(i.name);
		pb.push_back(i.pb);
	}

	if (ReadProcessMemory(window->phandle, (void*)(window->baseAddress + FRAME_COUNTER_OFFSET), &window->iramAddr, 4, &window->bytesRead))
	{
		if (!ReadProcessMemory(window->phandle, (LPCVOID)((DWORD)window->iramAddr + MISSILE_OFFSET), &inv->invValue, 4, &window->bytesRead))
		{
			output("ROM not loaded. (" << GetLastError() << ")");

			if (window->bytesRead != 0)
				output("Bytes read: " << window->bytesRead);

			return 1;
		}
	}
	else
	{
		output("Failed to load offset. (" << GetLastError() << ")");

		if (window->bytesRead != 0)
			output("Bytes read: " << window->bytesRead);

		return 1;
	}

	do
	{
		missilesCheck();
		supersCheck();

		std::thread invT(&Run::invCheck, this);
		invT.join();

		if (inv->hasInv)
		{
			output(count << ": " << names.at(count) << " at " << getTime());
			inv->iOld = inv->iNew;
			inv->hasInv = false;

			attempt->at(count).pb = getTime();
			++count;
		}

		if (inv->mOld && !inv->hasMissiles)
		{
			output(count << ": " << names.at(count) << " at " << getTime());
			inv->hasMissiles = true;
			attempt->at(count).pb = getTime();
			++count;
		}

		else if (inv->mOld && !inv->hasSupers)
		{
			output(count << ": " << names.at(count) << " at " << getTime());
			inv->hasSupers = true;
			attempt->at(count).pb = getTime();
			++count;
		}

	} while (count < attempt->size());

	saveSplits();
	return 0;
}

Run::Run()
{
	attempt = new Attempt();
	inv = new Inv();
	window = new Window();
}

Run::~Run()
{
	delete attempt;
	delete inv;
	delete window;
}

// TODO: Comment main()
int main()
{
	Run *run = new Run();
	// Find the window handle based on the title
	run->window->window = FindWindow(NULL, _T("VBA-RR v24 svn480"));

	if (run->window->window != 0)
	{
		// I honestly don't know what this stuff does but it works so fuck it
		GetWindowThreadProcessId(run->window->window, &run->window->pid);
		run->window->phandle = OpenProcess(PROCESS_VM_READ, 0, run->window->pid);
		if (!run->window->phandle)
			output("Could not get handle! (" << GetLastError() << ")");
		else
			output("Handle obtained, PID: " << run->window->pid);

		// Wiccan magic to find the base address
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, run->window->pid);
		MODULEENTRY32 module;
		module.dwSize = sizeof(MODULEENTRY32);
		Module32First(snapshot, &module);
		run->window->baseAddress = (DWORD)module.modBaseAddr;

		run->doSplit();
	}
	else
	{
		output("Unable to find window, try again. (" << GetLastError() << ")");
		return 1;
	}
	return 0;
}