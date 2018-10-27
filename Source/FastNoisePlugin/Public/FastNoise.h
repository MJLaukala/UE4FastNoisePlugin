// UFastNoise.h
//
// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

// VERSION: 0.4.1

#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "FastNoise.generated.h"

// Uncomment the line below to use doubles throughout UFastNoise instead of floats
//#define FN_USE_DOUBLES

#define FN_CELLULAR_INDEX_MAX 3

UENUM(BlueprintType)
enum class EFNNoiseType : uint8
{
	Value		   UMETA(DisplayName="Value"),
	ValueFractal   UMETA(DisplayName="ValueFractal"),
	Perlin		   UMETA(DisplayName="Perlin"),
	PerlinFractal  UMETA(DisplayName="PerlinFractal"),
	Simplex		   UMETA(DisplayName="Simplex"),
	SimplexFractal UMETA(DisplayName="SimplexFractal"),
	Cellular	   UMETA(DisplayName="Cellular"),
	WhiteNoise	   UMETA(DisplayName="WhiteNoise"),
	Cubic		   UMETA(DisplayName="Cubic"),
	CubicFractal   UMETA(DisplayName="CubicFractal")
};

UENUM(BlueprintType)
enum class EFNInterp : uint8
{
	Linear	UMETA(DisplayName="Linear"),
	Hermite	UMETA(DisplayName="Hermite"),
	Quintic	UMETA(DisplayName="Quintic")
};

UENUM(BlueprintType)
enum class EFNFractalType : uint8
{
	FBM		   UMETA(DisplayName="FBM"),
	Billow	   UMETA(DisplayName="Billow"),
	RigidMulti UMETA(DisplayName="RigidMulti")
};

UENUM(BlueprintType)
enum class EFNCellularDistanceFunction : uint8
{
	Euclidean UMETA(DisplayName="Euclidean"),
	Manhattan UMETA(DisplayName="Manhattan"),
	Natural	  UMETA(DisplayName="Natural")
};

UENUM(BlueprintType)
enum class EFNCellularReturnType : uint8
{
	CellValue	 UMETA(DisplayName="CellValue"),
	NoiseLookup	 UMETA(DisplayName="NoiseLookup"),
	Distance	 UMETA(DisplayName="Distance"),
	Distance2	 UMETA(DisplayName="Distance2"),
	Distance2Add UMETA(DisplayName="Distance2Add"),
	Distance2Sub UMETA(DisplayName="Distance2Sub"),
	Distance2Mul UMETA(DisplayName="Distance2Mul"),
	Distance2Div UMETA(DisplayName="Distance2Div")
};


UCLASS(BlueprintType, meta = (DisplayName = "FastNoise"))
class FASTNOISEPLUGIN_API UFastNoise : public UObject
{
	GENERATED_BODY()
public:
	UFastNoise();

private:
	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "Seed"), BlueprintSetter = SetSeed)
		int32 Seed;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "Frequency"))
		float Frequency;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "Interpolation"))
		EFNInterp Interpolation;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "NoiseType"))
		EFNNoiseType NoiseType;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "FractalOctaves"), BlueprintSetter = SetFractalOctaves)
		int32 FractalOctaves;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "FractalLacunarity"))
		float FractalLacunarity;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "FractalGain"), BlueprintSetter = SetFractalGain)
		float FractalGain;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "FractalType"))
		EFNFractalType FractalType;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularDistanceFunction"))
		EFNCellularDistanceFunction CellularDistanceFunction;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularReturnType"))
		EFNCellularReturnType CellularReturnType;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularDistanceIndex0", ClampMin = "0", ClampMax = "4"))
		int32 CellularDistanceIndex0;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularDistanceIndex1", ClampMin = "0", ClampMax = "4"))
		int32 CellularDistanceIndex1;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularJitter"))
		float CellularJitter;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "GradientPerturbAmp"))
		float GradientPerturbAmp;

	UPROPERTY(EditAnywhere, Category = "NoiseProperties", meta = (DisplayName = "CellularNoiseLookup"))
		UFastNoise* CellularNoiseLookup = nullptr;

#if WITH_EDITORONLY_DATA
public:
	UPROPERTY(VisibleAnywhere)
		UTexture2D* ThumbnailTexture;
	UPROPERTY(EditAnywhere)
		float ThumbnailScale;
	UPROPERTY(EditAnywhere)
		float OffsetX;
	UPROPERTY(EditAnywhere)
		float OffsetY;
#endif

public:

	// Sets seed used for all noise types
	// Default: 1337
	UFUNCTION(BlueprintSetter, Category = "FastNoise")
	void SetSeed(int32 newSeed);

	// Returns seed used for all noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	int32 GetSeed() const { return Seed; }

	// Sets frequency for all noise types
	// Default: 0.01
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetFrequency(float newFrequency) { Frequency = newFrequency; }

	// Returns frequency used for all noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetFrequency() const { return Frequency; }

	// Changes the interpolation method used to smooth between noise values
	// Possible interpolation methods (lowest to highest quality) :
	// - Linear
	// - Hermite
	// - Quintic
	// Used in Value, Perlin Noise and Position Warping
	// Default: Quintic
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetInterp(EFNInterp newInterp) { Interpolation = newInterp; }

	// Returns interpolation method used for supported noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	EFNInterp GetInterp() const { return Interpolation; }

	// Sets noise return type of GetNoise(...)
	// Default: Simplex
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetNoiseType(EFNNoiseType newNoiseType) { NoiseType = newNoiseType; }

	// Returns the noise type used by GetNoise
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	EFNNoiseType GetNoiseType() const { return NoiseType; }

	// Sets octave count for all fractal noise types
	// Default: 3
	UFUNCTION(BlueprintSetter, Category = "FastNoise")
	void SetFractalOctaves(int32 newOctaves) { if (FractalOctaves == newOctaves) return; FractalOctaves = newOctaves; CalculateFractalBounding(); }

	// Returns octave count for all fractal noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	int32 GetFractalOctaves() const { return FractalOctaves; }
	
	// Sets octave lacunarity for all fractal noise types
	// Default: 2.0
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetFractalLacunarity(float newLacunarity) { FractalLacunarity = newLacunarity; }

	// Returns octave lacunarity for all fractal noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetFractalLacunarity() const { return FractalLacunarity; }

	// Sets octave gain for all fractal noise types
	// Default: 0.5
	UFUNCTION(BlueprintSetter, Category = "FastNoise")
	void SetFractalGain(float newGain) { if (FractalGain == newGain) return; FractalGain = newGain; CalculateFractalBounding(); }

	// Returns octave gain for all fractal noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetFractalGain() const { return FractalGain; }

	// Sets method for combining octaves in all fractal noise types
	// Default: FBM
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetFractalType(EFNFractalType newFractalType) { FractalType = newFractalType; }

	// Returns method for combining octaves in all fractal noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	EFNFractalType GetFractalType() const { return FractalType; }

	// Sets distance function used in cellular noise calculations
	// Default: Euclidean
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetCellularDistanceFunction(EFNCellularDistanceFunction newCellularDistanceFunction) { CellularDistanceFunction = newCellularDistanceFunction; }

	// Returns the distance function used in cellular noise calculations
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	EFNCellularDistanceFunction GetCellularDistanceFunction() const { return CellularDistanceFunction; }

	// Sets return type from cellular noise calculations
	// Note: NoiseLookup requires another UFastNoise object be set with SetCellularNoiseLookup() to function
	// Default: CellValue
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetCellularReturnType(EFNCellularReturnType newCellularReturnType) { CellularReturnType = newCellularReturnType; }

	// Returns the return type from cellular noise calculations
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	EFNCellularReturnType GetCellularReturnType() const { return CellularReturnType; }

	// Noise used to calculate a cell value if cellular return type is NoiseLookup
	// The lookup value is acquired through GetNoise() so ensure you SetNoiseType() on the noise lookup, value, Perlin or simplex is recommended
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetCellularNoiseLookup(UFastNoise* newCellularNoiseLookup) { CellularNoiseLookup = newCellularNoiseLookup; }

	// Returns the noise used to calculate a cell value if the cellular return type is NoiseLookup
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	UFastNoise* GetCellularNoiseLookup() const { return CellularNoiseLookup; }

	// Sets the 2 distance indices used for distance2 return types
	// Default: 0, 1
	// Note: index0 should be lower than index1
	// Both indices must be >= 0, index1 must be < 4
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetCellularDistance2Indices(int32 newCellularDistanceIndex0, int32 newCellularDistanceIndex1);

	// Returns the 2 distance indices used for distance2 return types
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void GetCellularDistance2Indices(int32& outCellularDistanceIndex0, int32& outCellularDistanceIndex1) const;

	// Sets the maximum distance a cellular point can move from its grid position
	// Setting this high will make artifacts more common
	// Default: 0.45
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetCellularJitter(float newCellularJitter) { CellularJitter = newCellularJitter; }

	// Returns the maximum distance a cellular point can move from its grid position
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCellularJitter() const { return CellularJitter; }

	// Sets the maximum warp distance from original location when using GradientPerturb{Fractal}(...)
	// Default: 1.0
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void SetGradientPerturbAmp(float newGradientPerturbAmp) { GradientPerturbAmp = newGradientPerturbAmp; }

	// Returns the maximum warp distance from original location when using GradientPerturb{Fractal}(...)
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetGradientPerturbAmp() const { return GradientPerturbAmp; }

	//2D
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetValue2D(float x, float y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetValueFractal2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetPerlin2D(float x, float y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetPerlinFractal2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetSimplex2D(float x, float y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetSimplexFractal2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCellular2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoise2D(float x, float y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoiseInt2D(int32 x, int32 y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCubic2D(float x, float y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCubicFractal2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetNoise2D(float x, float y) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void GradientPerturb2D(float& x, float& y) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void GradientPerturbFractal2D(float& x, float& y) const;

	//3D
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetValue3D(float x, float y, float z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetValueFractal3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetPerlin3D(float x, float y, float z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetPerlinFractal3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetSimplex3D(float x, float y, float z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetSimplexFractal3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCellular3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoise3D(float x, float y, float z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoiseInt3D(int32 x, int32 y, int32 z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCubic3D(float x, float y, float z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetCubicFractal3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetNoise3D(float x, float y, float z) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void GradientPerturb3D(float& x, float& y, float& z) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	void GradientPerturbFractal3D(float& x, float& y, float& z) const;

	//4D
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetSimplex4D(float x, float y, float z, float w) const;

	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoise4D(float x, float y, float z, float w) const;
	UFUNCTION(BlueprintCallable, Category = "FastNoise")
	float GetWhiteNoiseInt4D(int32 x, int32 y, int32 z, int32 w) const;

private:
	uint8 m_perm[512];
	uint8 m_perm12[512];
	float m_fractalBounding;

	void UpdateSeed();
	void CalculateFractalBounding();

	//2D
	float SingleValueFractalFBM(float x, float y) const;
	float SingleValueFractalBillow(float x, float y) const;
	float SingleValueFractalRigidMulti(float x, float y) const;
	float SingleValue(uint8 offset, float x, float y) const;

	float SinglePerlinFractalFBM(float x, float y) const;
	float SinglePerlinFractalBillow(float x, float y) const;
	float SinglePerlinFractalRigidMulti(float x, float y) const;
	float SinglePerlin(uint8 offset, float x, float y) const;

	float SingleSimplexFractalFBM(float x, float y) const;
	float SingleSimplexFractalBillow(float x, float y) const;
	float SingleSimplexFractalRigidMulti(float x, float y) const;
	float SingleSimplexFractalBlend(float x, float y) const;
	float SingleSimplex(uint8 offset, float x, float y) const;

	float SingleCubicFractalFBM(float x, float y) const;
	float SingleCubicFractalBillow(float x, float y) const;
	float SingleCubicFractalRigidMulti(float x, float y) const;
	float SingleCubic(uint8 offset, float x, float y) const;

	float SingleCellular(float x, float y) const;
	float SingleCellular2Edge(float x, float y) const;

	void SingleGradientPerturb(uint8 offset, float warpAmp, float frequency, float& x, float& y) const;

	//3D
	float SingleValueFractalFBM(float x, float y, float z) const;
	float SingleValueFractalBillow(float x, float y, float z) const;
	float SingleValueFractalRigidMulti(float x, float y, float z) const;
	float SingleValue(uint8 offset, float x, float y, float z) const;

	float SinglePerlinFractalFBM(float x, float y, float z) const;
	float SinglePerlinFractalBillow(float x, float y, float z) const;
	float SinglePerlinFractalRigidMulti(float x, float y, float z) const;
	float SinglePerlin(uint8 offset, float x, float y, float z) const;

	float SingleSimplexFractalFBM(float x, float y, float z) const;
	float SingleSimplexFractalBillow(float x, float y, float z) const;
	float SingleSimplexFractalRigidMulti(float x, float y, float z) const;
	float SingleSimplex(uint8 offset, float x, float y, float z) const;

	float SingleCubicFractalFBM(float x, float y, float z) const;
	float SingleCubicFractalBillow(float x, float y, float z) const;
	float SingleCubicFractalRigidMulti(float x, float y, float z) const;
	float SingleCubic(uint8 offset, float x, float y, float z) const;

	float SingleCellular(float x, float y, float z) const;
	float SingleCellular2Edge(float x, float y, float z) const;

	void SingleGradientPerturb(uint8 offset, float warpAmp, float frequency, float& x, float& y, float& z) const;

	//4D
	float SingleSimplex(uint8 offset, float x, float y, float z, float w) const;

	inline uint8 Index2D_12(uint8 offset, int32 x, int32 y) const;
	inline uint8 Index3D_12(uint8 offset, int32 x, int32 y, int32 z) const;
	inline uint8 Index4D_32(uint8 offset, int32 x, int32 y, int32 z, int32 w) const;
	inline uint8 Index2D_256(uint8 offset, int32 x, int32 y) const;
	inline uint8 Index3D_256(uint8 offset, int32 x, int32 y, int32 z) const;
	inline uint8 Index4D_256(uint8 offset, int32 x, int32 y, int32 z, int32 w) const;

	inline float ValCoord2DFast(uint8 offset, int32 x, int32 y) const;
	inline float ValCoord3DFast(uint8 offset, int32 x, int32 y, int32 z) const;
	inline float GradCoord2D(uint8 offset, int32 x, int32 y, float xd, float yd) const;
	inline float GradCoord3D(uint8 offset, int32 x, int32 y, int32 z, float xd, float yd, float zd) const;
	inline float GradCoord4D(uint8 offset, int32 x, int32 y, int32 z, int32 w, float xd, float yd, float zd, float wd) const;
};
