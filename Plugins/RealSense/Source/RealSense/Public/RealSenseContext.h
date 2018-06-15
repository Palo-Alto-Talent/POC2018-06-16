#pragma once

#include "RealSenseTypes.h"
#include "RealSenseContext.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class REALSENSE_API URealSenseContext : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseContext();

	struct rs2_context* GetHandle();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	static URealSenseContext* GetRealSense();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	TArray<class URealSenseDevice*> QueryDevices();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	class URealSenseTextureSource* CreateTextureSource();

private:

	friend class FRealSensePlugin;
	void SetHandle(struct rs2_context* Handle);

	struct rs2_context* RsContext = nullptr;
};
