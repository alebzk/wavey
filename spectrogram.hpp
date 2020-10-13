#ifndef SPECTROGRAM_HPP
#define SPECTROGRAM_HPP

#include <array>
#include <vector>

constexpr int kInputSize = 1024;
constexpr int kInputAdvance = kInputSize / 2;
constexpr int kOutputSize = kInputSize / 2 + 1;

class Spectrogram {
   public:
    Spectrogram(const float* samples, int num_channels, int num_frames);
    int NumChannels() const { return power_spectra.size(); }
    int NumPowerSpectrumPerChannel() const { return power_spectra[0].size(); }
    int Advance() const { return kInputAdvance; }
    int OutputSize() const { return kOutputSize; }
    const float* Data(int channel) const { return power_spectra[channel][0].data(); }

   private:
    std::vector<std::vector<std::array<float, kOutputSize>>> power_spectra;
};

#endif
