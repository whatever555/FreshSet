#include "SaturdayDSP.h"

namespace
{
constexpr double kPi = 3.14159265358979323846;

inline double clampd(double v, double lo, double hi)
{
    return std::max(lo, std::min(hi, v));
}
} // namespace

void SaturdayChannelState::reset()
{
    *this = {};
}

void SaturdayEngine::prepare(double sr)
{
    sampleRate = sr;
    updateCoeffs();
    reset();
}

void SaturdayEngine::reset()
{
    ch[0].reset();
    ch[1].reset();
    tapeWowPhase = 0;
    modeCur = modeTgt;
    modeXfade = 1.0;
}

void SaturdayEngine::setMode(int mode)
{
    mode = std::clamp(mode, 0, 2);
    if (mode != modeTgt)
    {
        modeCur = modeTgt;
        modeTgt = mode;
        modeXfade = 0.0;
        ch[0].reset();
        ch[1].reset();
        tapeWowPhase = 0;
    }
}

void SaturdayEngine::updateCoeffs()
{
    const auto coeff = [this](double hz) { return std::exp(-2.0 * kPi * hz / sampleRate); };
    const auto tau = [this](double sec) { return std::exp(-1.0 / (sec * sampleRate)); };

    hpCoeff = coeff(30);
    dcCoeff = coeff(5);
    tiltCoeff = coeff(800);
    tapePreCoeff = coeff(2600);
    tapeLpCoeff = coeff(1750);
    tapeHfCoeff = coeff(580);
    tapeBumpCoeff = coeff(85);
    tapeHystCoeff = coeff(0.75);
    tapeEnvCoeff = tau(0.055);
    tapeEnvAttCoeff = tau(0.004);
    tapeScoopCoeff = coeff(920);
    tapeAzCoeff = coeff(3400);
    tapeWowInc = 2.0 * kPi * 0.32 / sampleRate;
    iconEnvCoeff = tau(0.014);
    iconSplitCoeff = coeff(1500);
    iconSizzleCoeff = coeff(5400);
    iconShineCoeff = coeff(4200);
    iconAirCoeff = coeff(9000);
    tubeBpCoeff = coeff(820);
    tubePresCoeff = coeff(3600);
    tubeBpGain = 0.38;
    meterDecay = tau(0.075);
    gatePkDecay = tau(0.012);
    gateOpenCoeff = tau(0.002);
    gateCloseCoeff = tau(0.045);
}

double SaturdayEngine::tanhSat(double x)
{
    if (x > 8.0) return 1.0;
    if (x < -8.0) return -1.0;
    const double e = std::exp(2.0 * x);
    return (e - 1.0) / (e + 1.0);
}

double SaturdayEngine::softLimit(double x)
{
    if (std::abs(x) <= 1.0) return x;
    return x > 0 ? 1.0 + tanhSat(x - 1.0) * 0.15 : -1.0 - tanhSat(-x - 1.0) * 0.15;
}

double SaturdayEngine::softClip(double x, double k)
{
    return x / (1.0 + std::abs(x) * k);
}

double SaturdayEngine::tubeClip(double x, double g)
{
    if (x >= 0.0)
    {
        const double e = std::exp(-x * g * 1.35);
        return 1.0 - e;
    }
    const double e = std::exp(x * g * 0.95);
    return -(1.0 - e) * 1.15;
}

double SaturdayEngine::linToDb(double x)
{
    return x > 1e-10 ? 20.0 * std::log10(x) : -120.0;
}

double SaturdayEngine::hpf(double x, int channel)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double y = x - s.hpX1 + hpCoeff * s.hpY1;
    s.hpX1 = x;
    s.hpY1 = y;
    return y;
}

double SaturdayEngine::dcBlock(double x, int channel)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double y = x - s.dcX1 + dcCoeff * s.dcY1;
    s.dcX1 = x;
    s.dcY1 = y;
    return y;
}

double SaturdayEngine::tape(double x, double drive, int channel)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double character = std::max(drive, 0.06);
    tapeWowPhase += tapeWowInc;
    const double wow = std::sin(tapeWowPhase) * character * 0.014
                     + std::sin(tapeWowPhase * 4.3) * character * 0.006;

    s.tapePre += (1.0 - tapePreCoeff) * (x - s.tapePre);
    s.tapeBump += (1.0 - tapeBumpCoeff) * (x - s.tapeBump);
    const double level = std::abs(x);
    const double envCoeff = level > s.tapeEnv ? tapeEnvAttCoeff : tapeEnvCoeff;
    s.tapeEnv += (1.0 - envCoeff) * (level - s.tapeEnv);

    const double body = s.tapePre * (0.74 + character * 0.16) + x * (0.26 - character * 0.05);
    double pre = (body + s.tapeBump * (0.72 + character * 0.95)) * (1.0 + character * 3.6);
    pre += s.tapeHyst * (0.48 + character * 0.42 + wow);
    const double gr = 1.0 / (1.0 + s.tapeEnv * s.tapeEnv * character * 3.4);
    pre = pre * gr * (1.0 + character * 0.62);

    const double sq = pre * pre;
    const double g = 0.42 + character * character * 2.9 + character * 1.45;
    const double pos = softClip(std::max(pre, 0.0), g * 0.26);
    const double neg = softClip(std::min(pre, 0.0), g * 0.44) * 1.22;
    double sat = pos + neg;
    sat *= pre > 0 ? 1.12 : 0.78;
    const double crunch = (pre > 0 ? sq : -sq) * character * 0.48;
    sat = sat * (1.0 - character * 0.26) + crunch;

    s.tapeHyst = s.tapeHyst * tapeHystCoeff + sat * (1.0 - tapeHystCoeff);
    s.tapeAz += (1.0 - tapeAzCoeff) * (sat - s.tapeAz);
    sat = sat * (0.8 - character * 0.05) + s.tapeAz * (0.2 + character * 0.1);
    s.tapeScoop += (1.0 - tapeScoopCoeff) * (sat - s.tapeScoop);
    s.tapeHf += (1.0 - tapeHfCoeff) * (sat - s.tapeHf);
    s.tapeLp += (1.0 - tapeLpCoeff) * (sat - s.tapeLp);

    double y = sat - s.tapeScoop * (0.26 + character * 0.2);
    const double hfAmt = 0.86 + character * 0.1;
    y = y * (1.0 - hfAmt) + s.tapeHf * hfAmt;
    y = y * (0.36 + character * 0.07) + s.tapeLp * (0.78 + character * 0.16);
    y += (sat - s.tapeHf) * character * 0.11 * (0.42 + character * 0.38);
    y = softClip(y * (1.0 + character * 0.82), 0.72 + character * 0.62);
    return y * 0.66;
}

double SaturdayEngine::tube(double x, double drive, int channel)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double character = std::max(drive, 0.04);
    const double heat = std::min(1.0, character * character * 6.0 + std::max(0.0, character - 0.2) * 2.6);

    s.tubeBpZ1 += (1.0 - tubeBpCoeff) * (x - s.tubeBpZ1);
    const double mid = s.tubeBpZ1;
    const double clean = x + mid * (0.2 + character * 0.42);
    const double pre = x + mid * (0.6 + character * 0.95) * heat;
    const double preG = 1.0 + character * 5.5 * heat;
    const double g = 2.2 + character * character * 5.5 + character * 2.8;
    const double even = pre * std::abs(pre) * (0.7 + character * 0.85) * heat;
    const double pos = tubeClip(std::max(pre, 0.0) * preG, g * 1.22);
    const double neg = tubeClip(std::min(pre, 0.0) * preG, g * 0.62) * 1.32;
    const double satSat = (pos + neg) * 0.9 + even * 0.95;
    double sat = clean * (1.0 - heat) + satSat * heat;

    s.tubeBp += (1.0 - tubeBpCoeff) * (sat - s.tubeBp);
    s.tubePres += (1.0 - tubePresCoeff) * (sat - s.tubePres);
    const double bp = s.tubeBp;
    const double pres = sat - s.tubePres;
    const double bpAmt = tubeBpGain * (22.0 + character * 28.0) * heat;
    const double presAmt = (0.9 + character * 1.25) * heat;
    return sat + bp * bpAmt + pres * presAmt;
}

double SaturdayEngine::iconic(double x, double drive, int channel)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double character = std::max(drive, 0.08);
    const double grit = std::min(1.0, character * character * 6.0 + std::max(0.0, character - 0.18) * 2.5);

    s.iconEnv += (1.0 - iconEnvCoeff) * (std::abs(x) - s.iconEnv);
    s.iconSplit += (1.0 - iconSplitCoeff) * (x - s.iconSplit);
    s.iconSizzle += (1.0 - iconSizzleCoeff) * (x - s.iconSizzle);
    const double body = s.iconSplit;
    const double pres = s.iconSizzle - s.iconSplit;
    const double airIn = x - s.iconSizzle;
    const double envSq = s.iconEnv * s.iconEnv;

    const double warmSat = softClip((body + body * std::abs(body) * (0.16 + character * 0.28)) * (1.0 + character * 1.8),
                                    0.9 + character * 0.45);
    const double warm = body * (0.62 - character * 0.1) * (1.0 - grit) + warmSat * grit;
    const double presG = (1.5 + character * 3.8) * (0.72 + s.iconEnv * (0.28 + character * 0.38));
    const double glowLin = pres * (0.62 + s.iconEnv * character * 0.72);
    const double glowSat = tanhSat(pres * presG) + pres * std::abs(pres) * (0.14 + character * 0.2) * presG * 0.12;
    const double glow = glowLin * (1.0 - grit) + glowSat * grit;
    const double auraG = (1.6 + character * 4.2) * envSq * (0.35 + character * 0.55);
    const double auraLin = airIn * envSq * (0.28 + character * 0.42);
    const double auraSat = tanhSat(airIn * auraG) + std::sin(airIn * (3.5 + character * 6.5)) * s.iconEnv * character * 0.1;
    const double aura = auraLin * (1.0 - grit) + auraSat * grit;
    double y = warm + glow * (0.78 + character * 0.72) + aura * (0.48 + character * 0.58);

    s.iconShine += (1.0 - iconShineCoeff) * (y - s.iconShine);
    const double hf = y - s.iconShine;
    const double bloom = 1.0 + s.iconEnv * character * 1.45;
    y = s.iconShine + hf * bloom;
    s.iconAir += (1.0 - iconAirCoeff) * (y - s.iconAir);
    const double air = y - s.iconAir;
    y += air * (0.22 + character * 0.38) * (0.55 + envSq * 0.65);
    return y * (0.62 / (1.0 + character * 0.38));
}

double SaturdayEngine::modeProcess(double x, int mode, double drive, int channel)
{
    if (mode < 1) return tape(x, drive, channel);
    if (mode < 2) return tube(x, drive, channel);
    return iconic(x, drive, channel);
}

double SaturdayEngine::oversample(int channel, double inS, double drive, int mode, int osRate)
{
    const int steps = osRate < 2 ? 1 : (osRate < 3 ? 2 : 4);
    auto& s = channel == 0 ? ch[0] : ch[1];
    double acc = 0;
    for (int i = 0; i < steps; ++i)
    {
        const double frac = (i + 1.0) / steps;
        const double interp = s.osLast + frac * (inS - s.osLast);
        acc += modeProcess(interp, mode, drive, channel);
    }
    s.osLast = inS;
    return acc / steps;
}

double SaturdayEngine::modeTilt(double x, int channel, double tone, int mode)
{
    auto& s = channel == 0 ? ch[0] : ch[1];
    s.tiltLp += (1.0 - tiltCoeff) * (x - s.tiltLp);
    const double lp = s.tiltLp;
    const double hp = x - lp;
    double lpAmt = 1, hpAmt = 1;
    if (mode < 1)
    {
        lpAmt = 1 + std::max(0.0, -tone) * 4.8 + std::max(0.0, tone) * 0.32;
        hpAmt = 1 + std::max(0.0, tone) * 0.32 + std::max(0.0, -tone) * 3.4;
    }
    else if (mode < 2)
    {
        lpAmt = 1 + std::max(0.0, -tone) + std::max(0.0, tone) * 1.4;
        hpAmt = 1 + std::max(0.0, tone) * 3.5 + std::max(0.0, -tone) * 0.6;
    }
    else
    {
        lpAmt = 1 + std::max(0.0, -tone) * 0.55 + std::max(0.0, tone) * 0.75;
        hpAmt = 1 + std::max(0.0, tone) * 4.8 + std::max(0.0, -tone) * 0.18;
    }
    return lp * lpAmt + hp * hpAmt;
}

double SaturdayEngine::gateGain(double inDry, int channel, double gateDb)
{
    if (gateDb <= -79.0) return 1.0;
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double level = std::abs(inDry);
    s.gatePk = std::max(level, s.gatePk * gatePkDecay);
    const double pkDb = linToDb(s.gatePk);
    const double closeDb = gateDb - 4.0;
    double tgt = 0;
    if (pkDb > gateDb)
        tgt = std::min(1.0, (pkDb - gateDb) / 3.0);
    else if (pkDb > closeDb)
        tgt = std::max(0.0, (pkDb - closeDb) / 4.0);

    const double coeff = tgt > s.gateEnv ? gateOpenCoeff : gateCloseCoeff;
    s.gateEnv += (1.0 - coeff) * (tgt - s.gateEnv);
    double g = s.gateEnv;
    if (g < 0.015) g = 0;
    else if (g > 0.985) g = 1;
    return g;
}

double SaturdayEngine::processChannel(double inDry, int channel, double drive, double tone,
                                    double mix, int osRate, double gateDb)
{
    const double gateG = gateGain(inDry, channel, gateDb);
    if (gateG < 0.001) return inDry;

    const double inCond = hpf(inDry, channel);
    auto& s = channel == 0 ? ch[0] : ch[1];
    const double osSave = s.osLast;
    const double w0 = oversample(channel, inCond, drive, modeCur, osRate);
    s.osLast = osSave;
    const double w1 = oversample(channel, inCond, drive, modeTgt, osRate);
    double wet = w0 * (1.0 - modeXfade) + w1 * modeXfade;
    wet = modeTilt(wet, channel, tone, modeTgt);
    const double wetMix = mix * gateG;
    const double outWet = inCond * (1.0 - wetMix) + wet * wetMix;
    const double mixComp = 1.0 - wetMix * (1.0 - wetMix) * 0.55;
    double out = outWet * mixComp;
    out = dcBlock(out, channel);
    return softLimit(out);
}

void SaturdayEngine::process(float* left, float* right, int numSamples,
                             float drivePct, float tonePct, float mixPct,
                             int quality, float gateDb)
{
    const double drive = drivePct / 100.0;
    const double tone = tonePct / 100.0;
    const double mix = mixPct / 100.0;
    const int osRate = quality + 1;

    for (int i = 0; i < numSamples; ++i)
    {
        if (modeXfade < 1.0)
        {
            modeXfade += 1.0 / kModeXfadeLen;
            if (modeXfade > 1.0) modeXfade = 1.0;
        }

        const double dryL = left[i];
        const double dryR = right[i];
        ch[0].meterIn = std::max(std::abs(dryL), ch[0].meterIn * meterDecay);
        ch[1].meterIn = std::max(std::abs(dryR), ch[1].meterIn * meterDecay);

        left[i] = static_cast<float>(processChannel(dryL, 0, drive, tone, mix, osRate, gateDb));
        right[i] = static_cast<float>(processChannel(dryR, 1, drive, tone, mix, osRate, gateDb));

        ch[0].meterOut = std::max(static_cast<double>(std::abs(left[i])), ch[0].meterOut * meterDecay);
        ch[1].meterOut = std::max(static_cast<double>(std::abs(right[i])), ch[1].meterOut * meterDecay);
    }
}
