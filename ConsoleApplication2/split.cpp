// Main cpp file
#include "split.h"

// Endianness can go die in a hole somewhere
unsigned int Split::endian_swap(unsigned const int& x)
{
	return (((x & 0x000000FF) << 24) |
		((x & 0x0000FF00) << 8) |
		((x & 0x00FF0000) >> 8) |
		((x & 0xFF000000) >> 24));
}

// Neat function to format a hex address into bytes for easy viewing.
std::string Split::hexOutput(int128_t x, bool flag)
{
	std::stringstream s;
	s << std::hex << std::setfill('0') <<
		std::setw(2) << ((x >> (flag ? 0x00 : 0x18)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x08 : 0x10)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x10 : 0x08)) & 0xFF) << " " <<
		std::setw(2) << ((x >> (flag ? 0x18 : 0x00)) & 0xFF);
	return s.str();
}

std::string Split::hexOutput(int128_t x, SIZE_T b)
{
	std::stringstream s;
	s << std::setfill('0');
	std::cout << std::hex << "x: " << x << std::endl;

	for (auto i = 0U, j=0U; j < b; i+=8,j++)
	{
		s << std::setw(2) << ((x >> i) & 0xFF) << " ";
	}
	//s << std::setfill('0') << std::setw(2) << (x & 0xFF) << " " << std::setw(2) << ((x >> 8) & 0xFF) << " " << std::setw(2) << ((x >> 16) & 0xFF) << " " << std::setw(3) << ((x >> 24) & 0xFF);
	return s.str();
}
// Decided to merge parseTime & the time acquisition into one function
std::string Split::getTime()
{
	std::uint64_t x = 0;
	ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + GAMETIME_OFFSET), &x, 4, &bytesRead);
	std::stringstream s;
	s << std::setfill('0') << std::setw(2) << (x & 0xFF) << ":" << std::setw(2) << ((x >> 8) & 0xFF) << ":" << std::setw(2) << ((x >> 16) & 0xFF) << "." << std::setw(3) << ((x >> 24) & 0xFF);
	return s.str();
}

Run Split::loadSplits()
{
	std::ifstream splits("splits.xml");

	ptree pt;
	read_xml(splits, pt);

	Run r;

	Segment s;

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

int Split::printSplits(Run s)
{
	for (auto i : s)
	{
		std::cout << i.id << ", " << i.name << ", " << i.pb << std::endl;
	}
	return 0;
}

int Split::saveSplits(Run s)
{
	std::ofstream splits("splits.xml", std::ostream::out);

	ptree tree;
	tree.add("Run.<xmlattr>.version", "1.0");

	std::vector<std::string> names, pb;

	for (auto i : s)
	{
		names.push_back(i.name);
		pb.push_back(i.pb);
	}

	for (auto i = 0U; i < s.size(); i++)
	{
		ptree &segment = tree.add("Run.Segments.Segment", "");
		segment.add("Name", names.at(i));
		segment.add("PB", pb.at(i));
		segment.add("<xmlattr>.id", i);
	}

	write_xml(splits, tree);
	return 0;
}

int Split::doSplit()
{
	/*int count = 0;
	Run r = loadSplits();
	std::vector<std::string> names, pb, cur;
	std::vector<int> bits;

	for (auto i : r)
	{
	names.push_back(i.name);
	pb.push_back(i.pb);
	}

	if (ReadProcessMemory(phandle, (void*)(baseAddress + FRAME_COUNTER_OFFSET), &invAddress, 4, &bytesRead))
	{
	if (ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + MISSILE_OFFSET), &invValue, 4, &bytesRead))
	{
	invStat.first = endian_swap(invValue);
	}
	else
	{
	std::cout << "ROM not loaded. (" << GetLastError() << ")" << std::endl;

	if (bytesRead != 0)
	std::cout << "Bytes read: " << bytesRead << std::endl;
	}
	}
	else
	{
	std::cout << "Failed to load offset. (" << GetLastError() << ")" << std::endl;

	if (bytesRead != 0)
	std::cout << "Bytes read: " << bytesRead << std::endl;
	}

	do
	{
	ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + INV_OFFSET), &invValue, 4, &bytesRead);
	invStat.second = endian_swap(invValue);
	invStat.first &= ~0x00FF00FF;
	invStat.second &= ~0x00FF00FF;

	if (invStat.first < invStat.second)
	{
	std::cout << names.at(count) << " at " << getTime() << std::endl;
	invStat.first = invStat.second;
	r[count].pb = getTime();

	count++;
	}

	} while (count < 2);
	saveSplits(r);*/
	ReadProcessMemory(phandle, (void*)(baseAddress + FRAME_COUNTER_OFFSET), (LPVOID)&invAddress, 4, &bytesRead);
	ReadProcessMemory(phandle, (LPCVOID)((DWORD)invAddress + MISSILE_OFFSET), (LPVOID)&invValue, 1, &bytesRead);
	std::cout << invAddress << std::endl << hexOutput(invValue, bytesRead) << std::endl;
	return 0;
}

Split::Split(){}
Split::~Split(){}

// TODO: Comment main()
int main()
{
	Split s;
	// Find the window handle based on the title
	s.window = FindWindow(NULL, _T("VBA-RR v24 svn480"));

	if (s.window != 0)
	{
		// I honestly don't know what this stuff does but it works so fuck it
		GetWindowThreadProcessId(s.window, &s.pid);
		s.phandle = OpenProcess(PROCESS_VM_READ, 0, s.pid);
		if (!s.phandle)
			std::cout << "Could not get handle! (" << GetLastError() << ")" << std::endl;
		else
			std::cout << "Handle obtained, PID: " << s.pid << std::endl << std::endl;

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
		std::cout << "Unable to find window, try again. (" << GetLastError() << ")" << std::endl;
		return 1;
	}
	return 0;
}