#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <iostream>

class Singleton
{
public:
	static Singleton* instance();

	class GC
	{
	public:
		~GC() {
			if (m_ins != NULL) {
				std::cout << "delete m_ins" << std::endl;
				delete m_ins;
			}
		}
	};

private:
	static Singleton* m_ins;
	static GC m_gc;
};

Singleton * Singleton::m_ins = 0; 
Singleton::GC Singleton::m_gc;

Singleton * Singleton::instance()
{
	if(0 == m_ins)
	{
		m_ins = new Singleton;
	}
	return m_ins;
}

#endif

