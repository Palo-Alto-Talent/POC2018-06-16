#include "PCH.h"
#include <vector>
#include <algorithm>

//=============================================================================
// TIMER
//=============================================================================

#ifdef OS_WINDOWS

#ifdef PROF_ENABLE_QPC
	static double _perfFreqMsecInv = 0;
#else
	#pragma comment(lib, "Winmm.lib")
#endif

int HiresTimer::Init()
{
	#ifdef PROF_ENABLE_QPC
		LARGE_INTEGER perfFreq;
		const BOOL rc = QueryPerformanceFrequency(&perfFreq);
		if (!rc || !perfFreq.QuadPart)
		{
			return -1;
		}

		_perfFreqMsecInv = 1.0 / ((double)perfFreq.QuadPart * 0.001);
	#endif

	return 0;
}

double HiresTimer::GetTicks()
{
	#ifdef PROF_ENABLE_QPC
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return (counter.QuadPart * _perfFreqMsecInv);
	#else
		DWORD t = timeGetTime();
		return (double)t;
	#endif
}

#else // not OS_WINDOWS

#include <sys/time.h>
#include <time.h>

int HiresTimer::Init()
{
	return 0;
}

double HiresTimer::GetTicks()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((ts.tv_sec * 1000.0) + (ts.tv_nsec * 0.000001));
}

#endif

//=============================================================================
// PROFILER
//=============================================================================

static Profiler* _profiler = NULL;

void Profiler::InitInstance()
{
	SAFE_DELETE(_profiler);
	_profiler = new Profiler();
}

void Profiler::DestroyInstance()
{
	SAFE_DELETE(_profiler);
}

Profiler* Profiler::GetInstance()
{
	return _profiler;
}

void Profiler::LogSummary(SortMode sortMode)
{
	std::vector<ProfilerSection*> sections;

	{
		PROF_LOCK_IMPL;

		if (m_sections.empty())
		{
			return;
		}

		sections.reserve(m_sections.size());

		for (SectionMap::iterator iter = m_sections.begin(); iter != m_sections.end(); ++iter)
		{
			sections.push_back(iter->second);
		}
	}

	struct SectionComparatorSum
	{
		inline bool operator() (const ProfilerSection* a, const ProfilerSection* b) const { return a->GetSum() > b->GetSum(); }
	};

	struct SectionComparatorAvg
	{
		inline bool operator() (const ProfilerSection* a, const ProfilerSection* b) const { return a->GetAvg() > b->GetAvg(); }
	};

	if (sortMode == SORT_AVG)
		std::sort(sections.begin(), sections.end(), SectionComparatorAvg());
	else
		std::sort(sections.begin(), sections.end(), SectionComparatorSum());

	PROF_LOG(_T("\n"));
	PROF_LOG(_T("%10s %15s %10s %10s  %-20s\n"), _T("<sum s>"), _T("<sum ms>"), _T("<calls>"), _T("<avg ms>"), _T("<name>"));
	std::vector<ProfilerSection*>::iterator isec;
	for (isec = sections.begin(); isec != sections.end(); ++isec)
	{
		ProfilerSection* section = *isec;
		PROF_LOG(_T("%10.3f %15.3f %10u %10.3f  %-20s\n"), 
			(section->GetSum() * 0.001), section->GetSum(), section->GetCount(), section->GetAvg(), 
			section->GetName()); //, section->GetUniq());
	}
	PROF_LOG(_T("\n"));
}

void Profiler::Release()
{
	PROF_LOCK_IMPL;

	for (SectionMap::iterator iter = m_sections.begin(); iter != m_sections.end(); ++iter)
	{
		delete iter->second;
	}

	m_sections.clear();
}

/*
#include <intrin.h>
inline uint64_t rdtsc() { return __rdtsc(); }

inline uint64_t rdtsc()
{
	unsigned int lo, hi;
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
}
*/
