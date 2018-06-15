#include "PCH.h"

URealSenseObject::URealSenseObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Context = IRealSensePlugin::Get().GetContext();
}

URealSenseObject::~URealSenseObject()
{
	Context = nullptr;
}
