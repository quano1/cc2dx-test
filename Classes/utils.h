#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <sstream>

#define LOGD(format, ...) CCLOG("[D]%s(%s:%d)(%s):" format "", __FILE__, __FUNCTION__, __LINE__, ::utils::tid().data(), ##__VA_ARGS__)
#define LOGE(format, ...) CCLOG("[E]%s(%s:%d)(%s):" format "%s", __FILE__, __FUNCTION__, __LINE__, ::utils::tid().data(), ##__VA_ARGS__, strerror(errno))

#define TIMER(ID) ::utils::Timer<std::chrono::duration<double,std::milli>> __timer_##ID(#ID)
#define TRACE() ::utils::Timer<std::chrono::duration<double,std::milli>> __tracer(std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "(" + ::utils::tid() + ")")


namespace utils {

template<typename T>
struct Trait;

template <template <class, class...> class G, class V, class ...Ts>
struct Trait<G<V,Ts...>>
{
    using template_type = V;
};

inline std::string tid()
{
	std::stringstream ss;
	ss << std::this_thread::get_id();
	return ss.str();
}

template <class D=std::chrono::duration<double,std::micro>>
struct Timer
{
using clock = std::chrono::high_resolution_clock;
using D_type = typename Trait<D>::template_type;

Timer() : id_(""), begin_(clock::now()) {}

Timer(std::string id) : id_(id), begin_(clock::now()) {CCLOG(" %s", id.data());}

~Timer()
{
	if(!id_.empty())
		CCLOG(" ~%s: %.3f (ms)", id_.data(), elapse());
}

void reset()
{
	begin_ = clock::now();
}

D_type elapse() const
{
	using namespace std::chrono;
	D_type ret = duration_cast<D>(clock::now() - begin_).count();
	return ret;
}

template <class Duration=D>
Duration duration() const
{
	using namespace std::chrono;
	Duration ret = duration_cast<Duration>(clock::now() - begin_);
	return ret;
}

clock::time_point begin_;
std::string id_;
};
}