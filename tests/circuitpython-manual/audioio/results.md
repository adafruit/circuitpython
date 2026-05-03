# audioio — `run_serial_tests.py` results

- **Board:** Adafruit Feather STM32F405 Express
- **Suite:** Tests 1–5 automated
- **Result:** all PASS

```
Using port: /dev/cu.usbmodem101

Copying files to board via /Volumes/CIRCUITPY ...
  jeplayer-splash-8000-8bit-mono-unsigned.wav (copied)
  jeplayer-splash-8000-16bit-mono-signed.wav (copied)
  jeplayer-splash-44100-16bit-mono-signed.wav (copied)
  jeplayer-splash-8000-16bit-stereo-signed.wav (copied)
  jeplayer-splash-44100-16bit-stereo-signed.wav (copied)
  wavefile_playback.py (copied)
  wavefile_pause_resume.py (copied)
  single_buffer_loop.py (copied)
  stereo_playback.py (copied)


============================================================
  Test 1 — WAV File Playback (wavefile_playback.py)
============================================================
Output:
    playing jeplayer-splash-44100-16bit-mono-signed.wav
    
    playing jeplayer-splash-44100-16bit-stereo-signed.wav
    
    playing jeplayer-splash-8000-16bit-mono-signed.wav
    
    playing jeplayer-splash-8000-16bit-stereo-signed.wav
    
    playing jeplayer-splash-8000-8bit-mono-unsigned.wav
    
    done
  [PASS] played jeplayer-splash-44100-16bit-mono-signed.wav
  [PASS] played jeplayer-splash-44100-16bit-stereo-signed.wav
  [PASS] played jeplayer-splash-8000-16bit-mono-signed.wav
  [PASS] played jeplayer-splash-8000-16bit-stereo-signed.wav
  [PASS] played jeplayer-splash-8000-8bit-mono-unsigned.wav
  [PASS] No OSError reported during playback
  [PASS] Script completed with 'done'
  [PASS] No exceptions (stderr='')

============================================================
  Test 2 — Pause / Resume (wavefile_pause_resume.py)
============================================================
Output:
    playing with pause/resume: jeplayer-splash-44100-16bit-mono-signed.wav
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
    
    playing with pause/resume: jeplayer-splash-44100-16bit-stereo-signed.wav
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
    
    playing with pause/resume: jeplayer-splash-8000-16bit-mono-signed.wav
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
    
    playing with pause/resume: jeplayer-splash-8000-16bit-stereo-signed.wav
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
    
    playing with pause/resume: jeplayer-splash-8000-8bit-mono-unsigned.wav
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
      paused
      resumed
    
    done
  [PASS] pause/resume header for jeplayer-splash-44100-16bit-mono-signed.wav
  [PASS] pause/resume header for jeplayer-splash-44100-16bit-stereo-signed.wav
  [PASS] pause/resume header for jeplayer-splash-8000-16bit-mono-signed.wav
  [PASS] pause/resume header for jeplayer-splash-8000-16bit-stereo-signed.wav
  [PASS] pause/resume header for jeplayer-splash-8000-8bit-mono-unsigned.wav
  [PASS] At least one 'paused' line printed
  [PASS] At least one 'resumed' line printed
  [PASS] No pause/resume hang timeout
  [PASS] No OSError reported during playback
  [PASS] Script completed with 'done'
  [PASS] No exceptions (stderr='')

============================================================
  Test 3 — Looping Sine Wave (single_buffer_loop.py)
============================================================
Output:
    unsigned 8 bit
    
    signed 8 bit
    
    unsigned 16 bit
    
    signed 16 bit
    
    done
  [PASS] 'unsigned 8 bit' label printed
  [PASS] 'signed 8 bit' label printed
  [PASS] 'unsigned 16 bit' label printed
  [PASS] 'signed 16 bit' label printed
  [PASS] Script completed with 'done'
  [PASS] No exceptions (stderr='')

============================================================
  Test 4 — deinit and Re-init (inline)
============================================================
Output:
    pass
  [PASS] Script printed 'pass'
  [PASS] No exceptions (stderr='')

============================================================
  Test 5 — Stereo Playback (stereo_playback.py)
============================================================
Output:
    channel test: left only
    
    channel test: right only
    
    channel test: both channels
    
    pan sweep: left to right
    
    playing stereo: jeplayer-splash-44100-16bit-stereo-signed.wav
    
    playing stereo: jeplayer-splash-8000-16bit-stereo-signed.wav
    
    done
  [PASS] Left-only channel tone played
  [PASS] Right-only channel tone played
  [PASS] Both-channel tone played
  [PASS] Pan sweep played
  [PASS] 44100 Hz 16-bit stereo WAV played
  [PASS] 8000 Hz 16-bit stereo WAV played
  [PASS] Script completed with 'done'
  [PASS] No exceptions (stderr='')

============================================================
SUMMARY
============================================================
  [PASS] Test 1 — WAV Playback
  [PASS] Test 2 — Pause/Resume
  [PASS] Test 3 — Looping Sine
  [PASS] Test 4 — deinit/Re-init
  [PASS] Test 5 — Stereo Playback

All automated tests passed.
Remaining manual step: audio/oscilloscope verification.
```
