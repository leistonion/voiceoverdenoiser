cmake_minimum_required(VERSION 3.24)
project(VoiceOverDenoiser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(VOICEOVER_WITH_ONNX "Enable ONNX Runtime inference backend" ON)
option(VOICEOVER_WITH_CUDA "Enable CUDA execution provider if ONNX runtime supports it" OFF)
option(VOICEOVER_BUILD_TESTS "Build basic pipeline tests" ON)

include(FetchContent)
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.5
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(VoiceOverDenoiser
  COMPANY_NAME "VoiceOver Labs"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_MANUFACTURER_CODE VdLs
  PLUGIN_CODE Vd01
  FORMATS VST3
  PRODUCT_NAME "VoiceOver Denoiser"
)

target_sources(VoiceOverDenoiser PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp
  Source/dsp/DspPipeline.cpp
  Source/dsp/STFTProcessor.cpp
  Source/dsp/PostProcessor.cpp
  Source/ml/ModelManager.cpp
  Source/ml/OnnxModelRunner.cpp
  Source/ml/RnnoiseStage.cpp
  Source/ml/DereverbStage.cpp
)

target_include_directories(VoiceOverDenoiser PRIVATE Source/include)

target_compile_definitions(VoiceOverDenoiser PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  $<$<BOOL:${VOICEOVER_WITH_ONNX}>:VOICEOVER_WITH_ONNX=1>
  $<$<BOOL:${VOICEOVER_WITH_CUDA}>:VOICEOVER_WITH_CUDA=1>
)

target_link_libraries(VoiceOverDenoiser PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_gui_extra
  juce::juce_opengl
)

if(VOICEOVER_BUILD_TESTS)
  enable_testing()
  add_executable(pipeline_smoke_test tests/pipeline_smoke_test.cpp Source/dsp/STFTProcessor.cpp Source/dsp/PostProcessor.cpp)
  target_include_directories(pipeline_smoke_test PRIVATE Source/include)
  target_link_libraries(pipeline_smoke_test PRIVATE juce::juce_dsp)
  add_test(NAME pipeline_smoke_test COMMAND pipeline_smoke_test)
endif()
