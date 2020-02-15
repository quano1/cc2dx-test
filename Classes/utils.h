#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <sstream>

namespace utils {

inline std::string tid()
{
	std::stringstream ss;
	ss << std::this_thread::get_id();
	return ss.str();
}

// template <typename T=double, class D=std::micro>
template <class D=std::chrono::duration<double,std::micro>>
struct Timer
{
using clock = std::chrono::high_resolution_clock;

Timer() : id_(""), begin_(clock::now()) {}

Timer(std::string id) : id_(id), begin_(clock::now()) {CCLOG("%s", id.data());}

~Timer()
{
	if(!id_.empty())
		CCLOG("~%s: %.3f (ms)", id_.data(), elapse<std::chrono::duration<double,std::milli>>().count());
}

void reset()
{
	begin_ = clock::now();
}

template <class Duration=D>
Duration elapse(bool reset=false)
{
	using namespace std::chrono;
	Duration ret = duration_cast<Duration>(clock::now() - begin_);
	if(reset) this->reset();
	return ret;
}

clock::time_point begin_;
std::string id_;
};
}

#define LOGD(format, ...) CCLOG("[D]%s(%s:%d)(%s):" format "", __FILE__, __FUNCTION__, __LINE__, ::utils::tid().data(), ##__VA_ARGS__)
#define LOGE(format, ...) CCLOG("[E]%s(%s:%d)(%s):" format "%s", __FILE__, __FUNCTION__, __LINE__, ::utils::tid().data(), ##__VA_ARGS__, strerror(errno))

#define TIMER(ID) ::utils::Timer<std::chrono::duration<double,std::milli>> __timer_##ID(#ID)
#define TRACE() ::utils::Timer<std::chrono::duration<double,std::milli>> __tracer(std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "(" + ::utils::tid() + ")")
