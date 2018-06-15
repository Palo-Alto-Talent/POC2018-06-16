#pragma once

#include "RealSenseObject.h"
#include "RealSenseTextureSource.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class URealSenseTextureSource : public URealSenseObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseTextureSource();

	void ThreadProc();
	void ProcessFrameset(rs2::frameset& Frameset);

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual void Configure(FString DeviceSerial, ERealSenseStreamType StreamType, int Width, int Height, int Rate);

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual bool Start();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual void Stop();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual void Update();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual UTexture2D* GetTexture();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	virtual bool IsStarted();

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	FString DeviceSerial;

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	ERealSenseStreamType StreamType;

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	int Width = 640;

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	int Height = 480;

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	int Rate = 30;

	UPROPERTY(Category="RealSense", BlueprintReadWrite, EditAnywhere)
	bool UseBackgroundThread = true;

private:

	TUniquePtr<class FDynamicTexture> Texture;
	TUniquePtr<class rs2::pipeline> RsPipeline;

	HANDLE ThreadHandle = nullptr;
	volatile int ThreadFlag = 0;
	int FramesetId = 0;
};
