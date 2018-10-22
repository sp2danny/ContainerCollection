
#include "stdafx.h"

#include <vector>
#include <string>
#include <cctype>
#include <boost/filesystem.hpp>
#include <cassert>
#include <initializer_list>

using namespace boost::filesystem;
using namespace std::literals;

class DiscScanner
{
public:
	DiscScanner() = default;

	void IndexDrive(char drive);

	std::vector<std::string> search_for(std::string);

	void SaveCache( std::string );
	void LoadCache( std::string );

private:

	struct File;
	struct Dir;

	struct File
	{
		std::string name;
		Dir* owner = nullptr;
		unsigned long long gigo_index_begin = 0;
		unsigned long long gigo_index_end   = 0;
		File(std::string name = ""s, Dir* owner = nullptr) : name(name), owner(owner) {}
	};

	struct Dir
	{
		std::string name;
		Dir* owner = nullptr;
		unsigned long long gigo_index_files_begin = 0;
		unsigned long long gigo_index_files_end   = 0;
		unsigned long long gigo_index_dirs_begin  = 0;
		unsigned long long gigo_index_dirs_end    = 0;
		std::vector<Dir> sub_dirs;
		std::vector<File> files;
		Dir(std::string name = ""s, Dir* owner = nullptr) : name(name), owner(owner) {}
	};

	Dir root;
	std::vector<char> gigo_cache;

	void rec_scan_dir(Dir& d);
};

void DiscScanner::IndexDrive(char drive)
{
	gigo_cache.clear();
	root = Dir{};
	root.name = "C:/";
	root.name[0] = std::toupper(drive);

	rec_scan_dir(root);
}

void DiscScanner::rec_scan_dir(Dir& d)
{
	path p = d.name;

	if (!exists(p)) return;

	d.gigo_index_files_begin = gigo_cache.size();

	std::vector<directory_entry> dirs, files;

	for (directory_entry& de : directory_iterator(p))
	{
		if (is_regular_file(p))
			files.push_back(de);
		else if (is_directory(p))
			dirs.push_back(de);
	}

	for(directory_entry& de : files)
	{
		std::string fn = de.path().filename().string();
		d.files.push_back( File{fn,&d} );
		d.files.back().gigo_index_begin = gigo_cache.size();
		for( char c : fn )
			gigo_cache.push_back(c);
		d.files.back().gigo_index_end = gigo_cache.size();
		gigo_cache.push_back(0);
	}

	d.gigo_index_files_end = gigo_cache.size();
	gigo_cache.push_back(0);
	d.gigo_index_dirs_begin = gigo_cache.size();

	for(directory_entry& de : dirs)
	{
		std::string fn = de.path().filename().string();
		d.sub_dirs.push_back( Dir{fn,&d} );
		//d.sub_dirs.back().
		for( char c : fn )
			gigo_cache.push_back(c);
		gigo_cache.push_back(0);
		rec_scan_dir(d.sub_dirs.back());
	}

	d.gigo_index_dirs_end = gigo_cache.size();
	gigo_cache.push_back(0);
}


std::vector<std::string> DiscScanner::search_for(std::string text)
{
	int shifts[256];

	int i, len = text.size();

	for(i=0; i<256; ++i)
		shifts[i] = len;

	for(i=0; i<len; ++i)
	{
		char c = text[i];
		unsigned char uc = static_cast<unsigned char>(c);
		assert( (uc != 0) && "search text cannot contain the null-char" );
		shifts[i] = len-i-1;
	}

	unsigned long long spos = len-1;
	unsigned long long clen = gigo_cache.size();

	std::vector<std::string> ret;

	auto fudp = [&ret,this](unsigned long long p) -> void
	{
	};

	while( spos < clen )
	{
		char c = gigo_cache[i];
		unsigned char uc = static_cast<unsigned char>(c);
		auto sh = shifts[uc];
		if(sh)
		{
			spos += sh;
			continue;
		}
		auto strt = spos-len+1;
		for( auto j = 0ull; j < len; ++j )
		{
			if( gigo_cache[strt+j] != text[j] )
			{
				spos += 1;
				continue;
			}
		}
		fudp(strt);
	}

	return ret;
}


