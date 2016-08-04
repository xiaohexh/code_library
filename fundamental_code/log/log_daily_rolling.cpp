/*
 * g++ -o log_daily_rolling -g log_daily_rolling.cpp 
 */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

static unsigned int maxDaysToKeepDefault = 2;

class DailyRolling
{
public:
	explicit DailyRolling(const std::string &file_name,
		unsigned int maxDaysToKeep = maxDaysToKeepDefault,
		bool append = true,
		mode_t mode = 00644);
	
	void log(const std::string &log_level,
		const std::string &msg);

private:

	void _roll_over();
	void _log(const std::string &msg);
		
	unsigned int _maxDaysToKeep;

	int _fd;
	int _flags;
	mode_t _mode;
	std::string _filename;
	struct tm _logs_time;
};

DailyRolling::DailyRolling(const std::string &filename,
   unsigned int maxDaysToKeep,
   bool append,
   mode_t mode)
{
	_filename = filename;

	_flags = O_CREAT | O_APPEND | O_WRONLY;
	if (!append) {
		_flags |= O_TRUNC;
	}

	_mode = mode;

	_fd = ::open(_filename.c_str(), _flags, _mode);
	assert(_fd > 0);

	struct stat stat_buf;
	int res;
	time_t t;

	/* obtain last modification time */
	res = ::stat(_filename.c_str(), &stat_buf);
	if (res < 0) {
		t = time(NULL);
	} else {
		t = stat_buf.st_mtime;
	}

	localtime_r(&t, &_logs_time);
}

void DailyRolling::log(const std::string &log_level,
	const std::string &msg)
{
	struct tm now;
	time_t t = time(NULL);

	bool timeok = localtime_r(&t, &now) != NULL;
	if (timeok) {
		if ((now.tm_mday != _logs_time.tm_mday) ||
			(now.tm_mon != _logs_time.tm_mon) ||
			(now.tm_year != _logs_time.tm_year)) {
			_roll_over();
			_logs_time = now;
		}
	}

	_log(msg);
}

void DailyRolling::_roll_over()
{
	std::ostringstream filename_s;
	::close(_fd);
	filename_s << _filename << "." << _logs_time.tm_year + 1900 << "-"
				<< std::setfill('0') << std::setw(2) << _logs_time.tm_mon + 1<< "-"
				<< std::setw(2) << _logs_time.tm_mday << std::ends;
	const std::string lastfn = filename_s.str();
	::rename(_filename.c_str(), lastfn.c_str());

	_fd = ::open(_filename.c_str(), _flags, _mode);

	const time_t oldest = time(NULL) - _maxDaysToKeep * 24 * 60 * 60;

#define PATHDELIMITER "/"

	// iterate over files around log file and delete older with same prefix
	const std::string::size_type last_delimiter = _filename.rfind(PATHDELIMITER);
	const std::string dirname((last_delimiter == std::string::npos) ? "." : _filename.substr(0, last_delimiter));
	const std::string filename((last_delimiter == std::string::npos) ? _filename : _filename.substr(last_delimiter + 1, _filename.size() - last_delimiter - 1));

	struct dirent **entries;
	int nentries = scandir(dirname.c_str(), &entries, 0, alphasort);
	if (nentries < 0)
		return;
	for (int i = 0; i < nentries; i++) {
		struct stat stat_buf;
		int res = ::stat(entries[i]->d_name, &stat_buf);
		if ((res == -1) || (!S_ISREG(stat_buf.st_mode))) {
			free(entries[i]);
			continue;
		}

		if (stat_buf.st_mtime < oldest && strstr(entries[i]->d_name, filename.c_str())) {
			const std::string fullname = dirname + PATHDELIMITER + entries[i]->d_name;
			::unlink(fullname.c_str());
			std::cout << " Deleting " << fullname.c_str() << std::endl;
		}
		free(entries[i]);
	}
	free(entries);
}

void DailyRolling::_log(const std::string &msg)
{
	std::string tmp = msg + "\n"; 
	::write(_fd, tmp.c_str(), tmp.size());
}

int main()
{
	DailyRolling dr(std::string("daily_rolling.log"));
	while (1) {
	dr.log("debug", "test log message");
	sleep(3 * 60);
	}
	return 0;
}
