# VoiceOver Denoiser (JUCE VST3, Windows)

Production-oriented JUCE VST3 voiceover plugin scaffold focused on **speech transparency first** while reducing:

- Stationary ambient noise (AC hum, hiss, fan/electrical noise)
- Room reverberation / late reflections

The architecture is designed for hybrid AI + DSP processing and real-time DAW operation.

## Implemented architecture

```text
Input -> Mono Analysis Mix -> RNNoise-style pre-clean -> STFT ->
Main Denoise Model (Demucs/DeepFilterNet ONNX slot) ->
Dereverb Model (STFT domain slot) ->
Spectral post-refinement + voice-preserving smoothing -> Output
```

## Key design choices

1. **Voice integrity priority**
   - Conservative spectral gain shaping to avoid over-suppression and metallic artifacts.
   - Voice-preservation and fine-tune controls directly influence post-mask behavior.
2. **Stationary-noise focus**
   - Baseline pre-cleaning is tuned for low-level persistent noise, not transient foreground events.
3. **Latency modes**
   - Low / Balanced / High Quality map to FFT and model tier selection.
4. **Model management**
   - User app-data model directory abstraction (`%APPDATA%/VoiceOverDenoiser/Models`).
   - Hooks are in place for first-launch download + hash verification logic.

## UI controls

### Main
- Noise Reduction Amount
- Reverb Reduction Amount
- Fine Tune

### Advanced
- Noise Profile Bias (stationary vs broadband focus)
- Voice Preservation
- Attack / Release smoothing
- Latency mode (Low/Balanced/High Quality)

## Build (Windows + Visual Studio)

### Prerequisites
- CMake 3.24+
- Visual Studio 2022 (MSVC toolchain)
- Git

### Configure
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

### Build
```powershell
cmake --build build --config Release --target VoiceOverDenoiser_VST3
```

Generated VST3 is copied after build via JUCE plugin settings.

## ONNX / GPU integration notes

This scaffold keeps inference interfaces isolated in `OnnxModelRunner` so you can:

1. Add ONNX Runtime session creation.
2. Register CUDA execution provider when available.
3. Fallback to CPU provider when unavailable.

Build flags:
- `VOICEOVER_WITH_ONNX` (default ON)
- `VOICEOVER_WITH_CUDA` (default OFF)

## Testing

A lightweight smoke test target is included:

```bash
cmake -S . -B build
cmake --build build --target pipeline_smoke_test
ctest --test-dir build --output-on-failure
```

Manual validation checklist in REAPER:

1. Insert plugin on a mono voice track at 48 kHz.
2. Toggle `A/B Compare` between `Processed` and `Dry Reference`.
3. Verify that stationary noise drops while consonants/transients remain intact.
4. Enable anticipative FX + 64/128 sample buffer and monitor dropout/CPU.
5. Sweep latency modes (Low/Balanced/High Quality) and confirm acceptable round-trip behavior.

Sample asset placement guidance: `tests/audio/README.md`.

## Production hardening checklist

- [ ] Replace stub RNNoise approximation with linked RNNoise library.
- [ ] Integrate real Demucs / DeepFilterNet ONNX exports.
- [ ] Integrate real dereverb ONNX model (e.g., encoder-decoder or CMGAN-style).
- [ ] Add async model downloader with signed manifest + SHA256 checks.
- [ ] Add GUI meters: in/out waveform, GR meter, optional spectrogram.
- [ ] Add A/B compare and bypass safety ramp.
- [ ] Add extensive real-time profiling in REAPER at 48 kHz.

## Project layout

- `Source/PluginProcessor.cpp` - parameter + host processing entry.
- `Source/dsp/` - STFT pipeline and post refinement.
- `Source/ml/` - model management and inference-stage abstractions.
- `Source/PluginEditor.cpp` - JUCE GUI.
- `tests/` - smoke test.
