#ifndef _ProfileMgr_h__
#define _ProfileMgr_h__
#include "../Kernel.h"
#include "IProfileMgr.h"
#include "Tools.h"

class ProfileMgr : public IProfileMgr
{
	struct ProfileConfig 
	{
		bool openMemInfo;
		bool openCpuInfo;
		bool openProfile;
		s64  checkTime;
		s64  lastCheckTime;
	};
public:
	virtual ~ProfileMgr() {};

	virtual bool Ready();
	virtual bool Initialize();
	virtual bool Destroy();
	virtual void Process(s32 tick);
	virtual void AddModuleProfile(core::IModuleProfile *moduleProfile) { _moduleProfiles.push_back(moduleProfile); };

private:
	bool _LoadConfig();

private:
	ProfileConfig	_config;
	std::vector<core::IModuleProfile *> _moduleProfiles;
};

#define PROFILEMGR tools::Singleton<ProfileMgr>::Instance()

#endif
