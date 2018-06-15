#pragma once

#include <map>
#include <unordered_map>

#define OS_WINDOWS

// config
#define PROF_ENABLE // TODO: disable in release
#define PROF_ENABLE_QPC
#define PROF_ENABLE_THREADSAFE
//#define PROF_MAP_IMPL std::map
#define PROF_MAP_IMPL std::unordered_map
#define PROF_LOG(format, ...) UE_LOG(LogRealSense, Warning, format, __VA_ARGS__)

#define STRINGIZE1(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define UNIQ_NAME __FILE__ "_" STRINGIZE1(__LINE__)

#define SAFE_DELETE(ptr) { if (ptr) { delete ptr; ptr = 0; } }

#define NO_COPY(type_)\
	private:\
	type_(const type_&); \
	void operator=(const type_&)

#if defined(PROF_ENABLE_THREADSAFE)
	#include <mutex>
	#define ProfCritSection std::mutex
	#define PROF_SCOPED_LOCK(cs) std::lock_guard<std::mutex> scoped_cs(cs)
	#define PROF_MUTEX_IMPL ProfCritSection m_lock
	#define PROF_LOCK_IMPL PROF_SCOPED_LOCK(m_lock)
#else
	#define PROF_MUTEX_IMPL
	#define PROF_LOCK_IMPL
#endif

#if defined(PROF_ENABLE)
	#define NAMED_PROFILER(sectionName) ScopedProfiler named_profiler(sectionName, _T(UNIQ_NAME))
	#define SCOPED_PROFILER ScopedProfiler scoped_profiler(_T(__FUNCTION__), _T(UNIQ_NAME))
#else
	#define NAMED_PROFILER(sectionName)
	#define SCOPED_PROFILER
#endif

// HiresTimer

class HiresTimer
{
public:
	static int Init();
	static double GetTicks();
};

// ProfilerSection

class ProfilerSection
{
	NO_COPY(ProfilerSection);

	friend class Profiler;
	friend class ScopedProfiler;

private:

	inline ProfilerSection(const TCHAR* name, const TCHAR* uniq) :
		m_name(name), m_uniq(uniq), m_sum(0), m_count(0) {}

	inline void Update(double delta) { m_sum += delta, m_count += 1; }
	inline void ResetCounters() { m_sum = 0, m_count = 0; }

	inline const TCHAR* GetName() const { return m_name; }
	inline const TCHAR* GetUniq() const { return m_uniq; }
	inline double GetSum() const { return m_sum; }
	inline unsigned int GetCount() const { return m_count; }
	inline double GetAvg() const { return (m_count > 0 ? (m_sum / (double)m_count) : 0); }

private:

	const TCHAR* m_name;
	const TCHAR* m_uniq;
	double m_sum;
	unsigned int m_count;
};

// Profiler

class Profiler
{
	NO_COPY(Profiler);

public:

	enum SortMode
	{
		SORT_SUM = 0,
		SORT_AVG
	};

	static void InitInstance();
	static void DestroyInstance();
	static Profiler* GetInstance();

	Profiler() {}
	~Profiler() { Release(); }

	inline ProfilerSection* BeginSection(const TCHAR* name, const TCHAR* uniq);
	inline void EndSection(ProfilerSection* section, double delta);
	inline void ResetCounters();

	void LogSummary(SortMode sortMode = SORT_SUM);
	void Release();

private:

	PROF_MUTEX_IMPL;

	typedef PROF_MAP_IMPL<const TCHAR*, ProfilerSection*> SectionMap;
	SectionMap m_sections;
};

// ScopedProfiler

class ScopedProfiler
{
	NO_COPY(ScopedProfiler);

public:

	inline ScopedProfiler(const TCHAR* name, const TCHAR* uniq)
	{
		m_section = Profiler::GetInstance()->BeginSection(name, uniq);
		m_ticks = HiresTimer::GetTicks();
	}

	inline ~ScopedProfiler()
	{
		const double ticks = HiresTimer::GetTicks();
		const double delta = (ticks - m_ticks);
		Profiler::GetInstance()->EndSection(m_section, delta);
	}

private:

	ProfilerSection* m_section;
	double m_ticks;
};

// ProfilerInstance

class ProfilerInstance
{
public:

	inline ProfilerInstance() { Profiler::InitInstance(); }
	inline ~ProfilerInstance() { Profiler::DestroyInstance(); }
};

// INLINE IMPL

ProfilerSection* Profiler::BeginSection(const TCHAR* name, const TCHAR* uniq)
{
	PROF_LOCK_IMPL;

	ProfilerSection* section;
	SectionMap::iterator iter = m_sections.find(uniq);
	if (iter != m_sections.end())
	{
		section = iter->second;
	}
	else
	{
		section = new ProfilerSection(name, uniq);
		m_sections.insert(std::pair<const TCHAR*, ProfilerSection*>(uniq, section));
	}

	return section;
}

void Profiler::EndSection(ProfilerSection* section, double delta)
{
	PROF_LOCK_IMPL;

	section->Update(delta);
}

void Profiler::ResetCounters()
{
	PROF_LOCK_IMPL;

	for (SectionMap::iterator iter = m_sections.begin(); iter != m_sections.end(); ++iter)
	{
		iter->second->ResetCounters();
	}
}
