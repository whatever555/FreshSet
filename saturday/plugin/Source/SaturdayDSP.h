#pragma once

#include <cmath>
#include <algorithm>

struct SaturdayChannelState
{
    double hpX1 = 0, hpY1 = 0;
    double dcX1 = 0, dcY1 = 0;
    double tiltLp = 0;
    double tapePre = 0, tapeLp = 0, tapeHf = 0, tapeBump = 0;
    double tapeHyst = 0, tapeEnv = 0, tapeScoop = 0, tapeAz = 0;
    double iconEnv = 0, iconSplit = 0, iconSizzle = 0, iconShine = 0, iconAir = 0;
    double tubeBp = 0, tubeBpZ1 = 0, tubePres = 0;
    double osLast = 0;
    double gatePk = 0, gateEnv = 0;
    double meterIn = 0, meterOut = 0;

    void reset();
    void resetProcessing();
};

class SaturdayEngine
{
public:
    static constexpr int kModeXfadeLen = 128;

    void prepare(double sampleRate);
    void reset();
    void resetProcessingStates();
    void setMode(int mode);
    void process(float* left, float* right, int numSamples,
                   float drivePct, float tonePct, float mixPct,
                   int quality, float gateDb);

    float getMeterInL() const { return static_cast<float>(ch[0].meterIn); }
    float getMeterInR() const { return static_cast<float>(ch[1].meterIn); }
    float getMeterOutL() const { return static_cast<float>(ch[0].meterOut); }
    float getMeterOutR() const { return static_cast<float>(ch[1].meterOut); }

private:
    SaturdayChannelState ch[2];
    double sampleRate = 44100.0;
    int modeCur = 0, modeTgt = 0;
    double modeXfade = 1.0;
    float lastDrivePct = -999.f;

    double hpCoeff = 0, dcCoeff = 0, tiltCoeff = 0;
    double tapePreCoeff = 0, tapeLpCoeff = 0, tapeHfCoeff = 0, tapeBumpCoeff = 0;
    double tapeHystCoeff = 0, tapeEnvCoeff = 0, tapeEnvAttCoeff = 0;
    double tapeScoopCoeff = 0, tapeAzCoeff = 0, tapeWowInc = 0, tapeWowPhase = 0;
    double iconEnvCoeff = 0, iconSplitCoeff = 0, iconSizzleCoeff = 0;
    double iconShineCoeff = 0, iconAirCoeff = 0;
    double tubeBpCoeff = 0, tubePresCoeff = 0, tubeBpGain = 0;
    double meterDecay = 0, gatePkDecay = 0, gateOpenCoeff = 0, gateCloseCoeff = 0;

    void updateCoeffs();
    static double tanhSat(double x);
    static double softLimit(double x);
    static double softClip(double x, double k);
    static double tubeClip(double x, double g);
    static double linToDb(double x);

    double hpf(double x, int channel);
    double dcBlock(double x, int channel);
    double tape(double x, double drive, int channel);
    double tube(double x, double drive, int channel);
    double iconic(double x, double drive, int channel);
    double modeProcess(double x, int mode, double drive, int channel);
    double oversample(int channel, double inS, double drive, int mode, int osRate);
    double modeTilt(double x, int channel, double tone, int mode);
    double gateGain(double inDry, int channel, double gateDb);
    double processChannel(double inDry, int channel, double drive, double tone,
                          double mix, int osRate, double gateDb);
};
