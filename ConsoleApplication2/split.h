#ifndef __SPLIT_H__
#define __SPLIT_H__
#include <windows.h>											// Needed for windows stuff
#include <tlhelp32.h>											// "
#include <tchar.h>												// "
#include <cstdio>												// printf and the like
#include <sstream>												// std::stringstream
#include <iostream>												// std::cout, std::endl
#include <fstream>												// std::ostream and the like
#include <vector>												// std::vector
#include <cstdint>												// std::uint64_t
#include <iomanip>												// std::setfill, std::setw
#include <thread>												// std::thread
#include <future>
#include <boost/property_tree/ptree.hpp>						// boost xml stuff
#include <boost/property_tree/xml_parser.hpp>					// "
#include <boost/multiprecision/cpp_int.hpp>						// "boost huge numbers

using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;
using namespace boost::multiprecision;

// Start of GBA BIOS
#define BIOS_OFFSET 0x004F4318

// (0x3000000 in VBA, or the start of IRAM)
#define FRAME_COUNTER_OFFSET 0x004F45F4
#define GAMETIME_OFFSET 0x150
#define MISSILE_OFFSET 0x1532
#define INV_OFFSET 0x153C

// cout shorthand because I'm impossibly lazy
#define output(a) std::cout << a << std::endl

// Debug defines, remove when done

#define COUNT_DEBUG 5
struct Segment
{
	int id;
	std::string name;
	std::string pb;
	std::vector<std::string> hist;
};

class Run
{
public:
	typedef std::vector<Segment> Attempt;
	Attempt* run;
	DWORD pid;
	int invAddress = 0;
	int missilesAddress = 0;
	int128_t invValue;
	int missilesValue = 0;

	SIZE_T bytesRead = 0;
	DWORD baseAddress;
	HWND window;
	HANDLE phandle;
	std::pair<int128_t, int128_t> invStat;
	std::pair<bool, bool> missilesStat;
	bool invS = false;
	std::vector<std::string> names, pb, cur;


	unsigned int endianSwap(unsigned const int&);
	std::string hexOutput(int128_t, bool);
	std::string hexOutput(int128_t, SIZE_T);
	std::string getTime();
	void missilesCheck();
	void invCheck();
	Attempt loadSplits();
	int printSplits();
	int saveSplits();
	int doSplit();
	Segment getSegment(int);

	Run();
	~Run();
};
#endif //MZMSPLIT_H