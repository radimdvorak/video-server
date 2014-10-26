/*
 * TerminateHandler.h
 *
 *  Created on: 29. 6. 2014
 *      Author: dron
 */

#ifndef TERMINATEHANDLER_H_
#define TERMINATEHANDLER_H_

#include <deque>
#ifdef _WIN32
	#include <windows.h>
#endif


template<typename F>
class TerminateEvent;

template<class R>
class TerminateEvent<R (*)()> {
		typedef R (*FuncType)();
		std::deque<FuncType> ls;
	public:
		TerminateEvent<FuncType>& operator+=(FuncType t) {
			ls.push_back(t);
			return *this;
		}

		void operator()() {
			typename std::deque<FuncType>::iterator i = ls.begin();
			typename std::deque<FuncType>::iterator e = ls.end();
			for (; i != e; ++i) {
				(*i)();
			}
		}
};

class TerminateHandler {
	public:
		typedef void (*Functor)();

		static TerminateHandler& instance();

		void operator+=(Functor t);

	private:
		TerminateHandler();

#ifdef _WIN32
		static BOOL ctrlHandler(DWORD fdwCtrlType);

#else
		static void sigHandler(int s);
#endif

	private:
		TerminateEvent<Functor> event;

};


#endif /* TERMINATEHANDLER_H_ */
