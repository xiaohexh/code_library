/*
 * g++ -o log_size_rolling -g log_size_rolling.cpp 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#define DEF_MAX_RES_FILES 10

class SizeRolling
{
public:
	SizeRolling(const std::string &file_name,
		size_t max_file_size,
		unsigned int max_backup_index = DEF_MAX_RES_FILES,
		bool append = true,
		mode_t mode = 00644);

	void log(const std::string &log_level,
		const std::string &msg);

private:
	void _roll_over();
	void _log(const std::string &log_level,
		const std::string &msg);

	unsigned int _max_backup_index;
	int _max_backup_index_width;
	size_t _max_file_size;
	int _fd;
	int _flags;
	mode_t _mode;
	std::string _filename;
};

SizeRolling::SizeRolling(const std::string &filename,
        size_t max_file_size,
        unsigned int max_backup_index,
        bool append,
        mode_t mode) :
	_filename(filename),
	_mode(mode),
	_max_backup_index((max_backup_index > 0) ? max_backup_index : 1),
	_max_backup_index_width((max_backup_index > 0) ? log10((float)max_backup_index) + 1 : 1),
	_max_file_size(max_file_size)
{
	_flags = O_CREAT | O_WRONLY | O_APPEND;
	if (!append) {
		_flags |= O_TRUNC;
	}

	_fd = ::open(_filename.c_str(), _flags, _mode);
}

void SizeRolling::_roll_over()
{
	::close(_fd);
	if (_max_backup_index > 0) {
		/* remove largest index log file */
		std::ostringstream filename_s;
		filename_s << _filename << "." << std::setw(_max_backup_index_width) << std::setfill('0') << _max_backup_index << std::ends;
		std::string last_log_filename = filename_s.str();
		::remove(last_log_filename.c_str());

		for (unsigned int i = _max_backup_index; i > 1; i--) {
			filename_s.str(std::string());
			filename_s << _filename << "." << std::setw(_max_backup_index_width) << std::setfill('0') << i - 1 << std::ends;
			::rename(filename_s.str().c_str(), last_log_filename.c_str());
			last_log_filename = filename_s.str();
		}

		::rename(_filename.c_str(), last_log_filename.c_str());
	}
	_fd = ::open(_filename.c_str(), _flags, _mode);
}

void SizeRolling::log(const std::string &log_level,
	const std::string &msg)
{
	_log(log_level, msg);

	off_t offset = ::lseek(_fd, 0, SEEK_END);
	if (offset < 0) {
	} else {
		if (static_cast<size_t>(offset) >= _max_file_size) {
			_roll_over();
		}
	}
}

void SizeRolling::_log(const std::string &log_level,
	const std::string &msg)
{
	std::ostringstream msg_s;
	msg_s << log_level << " " << msg << std::endl;
	::write(_fd, msg_s.str().c_str(), msg_s.str().size());
}

int main()
{
	SizeRolling sr(std::string("./size_rolling.log"), 1000); // max file size: 1K byte
	while (1) {
	sr.log("warn", "test size rolling log");
	}

	return 0;
}
