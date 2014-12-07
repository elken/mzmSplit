#include "split.h"

Segment Run::getSegment(int i)
{
	return run->at(i);
}

// Endianness can go die in a hole somewhere
unsigned int Run::endianSwap(unsigned const int& x)
{
	return (((x & 0x000000FF) << 24) |
		((x & 0x0000FF00) << 8) |
		((x & 0x00FF0000) >> 8) |
		((x & 0xFF000000) >> 24));
}

void Run::missilesCheck()
{
		//ReadProcessMemory(phandle, (void*)(baseAddress + FRAME_COUNTER_OFFSET), (LPVOID)&missilesAddress, 4, &bytesRead);
		ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + MISSILE_OFFSET), (LPVOID)&missilesValue, 4, &bytesRead);
		std::string missiles_str = hexOutput(missilesValue, bytesRead);
		
		if ((missilesValue & ~0xFFFFFF00) != 0)
		{
			missilesStat.first = true;
		}
		if ((missilesValue & ~0xFF00FFFF) != 0)
		{
			missilesStat.second = true;
		}
}

void Run::invCheck()
{
	ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + INV_OFFSET), &invValue, 4, &bytesRead);
	invStat.second = endianSwap((int)invValue);
	invStat.first &= ~0x00FF00FF;
	invStat.second &= ~0x00FF00FF;

	if (invStat.first < invStat.second)
	{
		invS =  true;
	}
}

// Neat function to format a hex address into bytes for easy viewing.
std::string Run::hexOutput(int128_t x, bool flag)
{
	std::stringstream s;
	s << std::hex << std::setfill('0') <<
		std::setw(2) << ((x >> (flag ? 0x00 : 0x18)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x08 : 0x10)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x10 : 0x08)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x18 : 0x00)) & 0xFF);
	return s.str();
}

std::string Run::hexOutput(int128_t x, SIZE_T b)
{
	std::stringstream s;
	s << std::setfill('0');

	for (auto i = 0U, j = 0U; j < b; i += 8, j++)
	{
		s << std::setw(2) << ((x >> i) & 0xFF) << " ";
	}
	return s.str();
}

// Decided to merge parseTime & the time acquisition into one function
std::string Run::getTime()
{
	std::uint64_t x = 0;
	ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + GAMETIME_OFFSET), &x, 4, &bytesRead);
	std::stringstream s;
	s << std::setfill('0') << std::setw(2) << (x & 0xFF) << ":" << std::setw(2) << ((x >> 8) & 0xFF) << ":" << std::setw(2) << ((x >> 16) & 0xFF) << "." << std::setw(3) << ((x >> 24) & 0xFF);
	return s.str();
}

Run::Attempt Run::loadSplits()
{
	std::ifstream splits("splits.xml");

	ptree pt;
	read_xml(splits, pt);

	Attempt r;

	for (auto i : pt.get_child("Run.Segments"))
	{
		Segment s;
		s.id = i.second.get<int>("<xmlattr>.id");
		s.name = i.second.get<std::string>("Name");
		s.pb = i.second.get<std::string>("PB");

		r.push_back(s);
	}
	return r;
}

int Run::printSplits()
{
	for (auto i : *run)
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

	for (auto i : *run)
	{
		names.push_back(i.name);
		pb.push_back(i.pb);
	}

	for (auto i = 0U; i < run->size(); i++)
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
	*run = loadSplits();

	for (auto i : *run)
	{
		names.push_back(i.name);
		pb.push_back(i.pb);
	}

	if (ReadProcessMemory(phandle, (void*)(baseAddress + FRAME_COUNTER_OFFSET), &invAddress, 4, &bytesRead))
	{
		if (!ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + MISSILE_OFFSET), &invValue, 4, &bytesRead))
		{
			output("ROM not loaded. (" << GetLastError() << ")");

			if (bytesRead != 0)
				output("Bytes read: " << bytesRead);
		}
	}
	else
	{
		output("Failed to load offset. (" << GetLastError() << ")");

		if (bytesRead != 0)
			output("Bytes read: " << bytesRead);
	}

	do
	{
		std::thread invT(&Run::invCheck, this);
		std::thread misT(&Run::missilesCheck, this);

		invT.join();
		misT.join();

		if (invS)
		{
			output(count << ": " << names.at(count) << " at " << getTime());
			invStat.first = invStat.second;
			invS = false;

			run->at(count).pb = getTime();
			++count;
		}

		if (missilesStat.first || missilesStat.second)
		{
			output(count << ": " << names.at(count) << " at " << getTime());


			run->at(count).pb = getTime();
			++count;
		}
	} 
	while (count < COUNT_DEBUG);

	this->saveSplits();
	return 0;
}

Run::Run()
{
	run = new Attempt();
}

Run::~Run()
{
	delete run;
}

// TODO: Comment main()
int main()
{
	Run s;
	// Find the window handle based on the title
	s.window = FindWindow(NULL, _T("VBA-RR v24 svn480"));

	if (s.window != 0)
	{
		// I honestly don't know what this stuff does but it works so fuck it
		GetWindowThreadProcessId(s.window, &s.pid);
		s.phandle = OpenProcess(PROCESS_VM_READ, 0, s.pid);
		if (!s.phandle)
			output("Could not get handle! (" << GetLastError() << ")");
		else
			output("Handle obtained, PID: " << s.pid);

		// Wiccan magic to find the base address
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, s.pid);
		MODULEENTRY32 module;
		module.dwSize = sizeof(MODULEENTRY32);
		Module32First(snapshot, &module);
		s.baseAddress = (DWORD)module.modBaseAddr;

		s.doSplit();
	}
	else
	{
		output("Unable to find window, try again. (" << GetLastError() << ")");
		return 1;
	}
	return 0;
}