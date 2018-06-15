#pragma once

#include "RealSenseObject.h"
#include "RealSenseDevice.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class REALSENSE_API URealSenseDevice : public URealSenseObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseDevice();

	struct rs2_device* GetHandle();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FString GetName();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FString GetSerial();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	TArray<class URealSenseSensor*> QuerySensors();

private:

	friend class URealSenseContext;
	void SetHandle(struct rs2_device* Handle);

	struct rs2_device* RsDevice = nullptr;
};
