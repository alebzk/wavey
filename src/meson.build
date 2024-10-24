src = files(
  'audio_buffer.cpp',
  'audio_mixer.cpp',
  'audio_system.cpp',
  'file_load_server.cpp',
  'file_notification.cpp',
  'gpu_spectrogram.cpp',
  'gpu_waveform.cpp',
  'low_res_waveform.cpp',
  'low_res_waveform.hpp',
  'main.cpp',
  'primitive_renderer.cpp',
  'renderer.cpp',
  'sample_line_shader.cpp',
  'sample_point_shader.cpp',
  'shader.cpp',
  'spectrogram.cpp',
  'spectrogram_shader.cpp',
  'spectrum_state.cpp',
  'spectrum_window.cpp',
  'state.cpp',
  'wave_shader.cpp',
  'zoom_window.cpp'
  )

cpp_args = []

deps = [gl, glm, sndfile, fftw, openmp, sdl2, imgui, implot]

if host_machine.system() == 'darwin'
  cpp_args += ['-DGL_SILENCE_DEPRECATION']
endif

executable('wavey', src, dependencies : deps, cpp_args : cpp_args,
           install : true)
