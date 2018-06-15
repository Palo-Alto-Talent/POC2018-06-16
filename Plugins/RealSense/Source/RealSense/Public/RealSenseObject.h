#pragma once

#include "RealSenseTypes.h"
#include "RealSenseObject.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class REALSENSE_API URealSenseObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseObject();

	inline class URealSenseContext* GetContext() { return Context; }

protected:

	UPROPERTY()
	class URealSenseContext* Context = nullptr;
};
