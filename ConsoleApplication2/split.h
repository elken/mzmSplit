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
#include <boost/property_tree/ptree.hpp>						// boost xml stuff
#include <boost/property_tree/xml_parser.hpp>					// "
#include <boost/multiprecision/cpp_int.hpp>						// "boost huge numbers

using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;
using namespace boost::multiprecision;

// Fairly obvious offset descriptions
#define BIOS_OFFSET 0x004F4318
#define FRAME_COUNTER_OFFSET 0x004F45F4
#define GAMETIME_OFFSET 0x150
#define MISSILE_OFFSET 0x1532
#define INV_OFFSET 0x153C

struct Segment
{
	int id;
	std::string name;
	std::string pb;
	std::vector<std::string> hist;
};
typedef std::vector<Segment> Run;

class Split
{
public:
	DWORD pid;
	int invAddress = 0;
	int128_t invValue;

	SIZE_T bytesRead = 0;
	DWORD baseAddress;
	HWND window;
	HANDLE phandle;
	std::pair<int, int> invStat;


	unsigned int endian_swap(unsigned const int&);
	std::string hexOutput(int128_t, bool);
	std::string hexOutput(int128_t, SIZE_T);
	std::string getTime();
	Run loadSplits();
	int printSplits(Run);
	int saveSplits(Run);
	int doSplit();

	Split();
	~Split();
};
#endif //MZMSPLIT_H