#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

class TimeStamp 
{
public:
	TimeStamp();
	TimeStamp(unsigned int seconds, unsigned int microSeconds = 0);

	inline int getSeconds() const
	{
		return _seconds;
	}

	inline int getMilliSeconds() const
	{
		return _microSeconds / 1000;
	}

	inline int getMicroSeconds() const
	{
		return _microSeconds;
	}

protected:
	int _seconds;
	int _microSeconds;
};

#endif
