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
#define SUPERS_OFFSET 0x1534
#define INV_OFFSET 0x153C

// cout shorthand because I'm impossibly lazy
#define output(a) std::cout << a << std::endl

struct Segment
{
	int id;
	std::string name;
	std::string pb;
	std::vector<std::string> hist;
};

struct Inv
{
	bool missiles;
	bool supers;
	bool hasMissiles;
	bool hasSupers;
	bool hasInv;

	int128_t iOld;
	int128_t iNew;

	int mOld;
	int mNew;

	int128_t invValue = 0;
	int missilesValue = 0;
};

struct Window
{
	DWORD pid;
	DWORD baseAddress;
	HWND window;
	HANDLE phandle;
	SIZE_T bytesRead = 0;
	int iramAddr = 0;
};

class Run
{
public:
	typedef std::vector<Segment> Attempt;
	Attempt* attempt;
	Window* window;
	Inv* inv;
	std::vector<std::string> names, pb, cur;

	std::string getTime();
	void supersCheck();
	void missilesCheck();
	void invCheck();
	void loadSplits();
	int printSplits();
	int saveSplits();
	int doSplit();

	Run();
	~Run();
};
#endif //MZMSPLIT_H