# ABCompare

**ABCompare** is a minimal, fast, Logic-style A/B plugin for comparing your DAW session’s master output against a reference track (MP3, WAV, FLAC, etc.), with instant switching and manual gain matching.  
Built with JUCE 8.0, battle-tested on Logic Pro.

---

## Features

- **A/B Switch** — Instantly toggle between your DAW master and a reference track.
- **Reference File Selector** — Pulldown menu lists all audio files in your `~/Music/ABCompare` folder.
- **Rotary Gain Knob** — Manually adjust reference track volume (-12dB to 0dB).
- **No Audio Degradation** — Pristine bypass when using “A”, bit-perfect reference on “B”.
- **Logic-Like UI** — Minimal, modern, dark blue/gray theme.
- **100% Real-Time Safe** — No disk or file operations in the audio thread; all handled with JUCE’s AsyncUpdater.

---

## How to Use

1. **Put your reference audio files** (MP3, WAV, FLAC, AIFF, OGG) in `~/Music/ABCompare`.
2. **Load ABCompare on your DAW’s master output** (last slot after your mastering chain).
3. **Select a reference track** from the dropdown.
4. **Click the A/B button** to switch between your session (“A”) and the reference (“B”).
5. **Adjust gain** by ear with the knob if needed.

> 💡 *Tip: To refresh the file list, just hit the plugin’s “rescan” (if present), or reload the plugin.*

---

## Building

Requires:
- JUCE 8.0+ (https://juce.com/)
- macOS + Xcode
- C++17 (or later)

Open `ABCompare.jucer` in Projucer, export for Xcode, and build AU/VST3 as usual.

---

## Version History

- **v0.1** — First working version: A/B switching, file select
- **v0.2** — Add rotary gain for reference
- **v0.3** — Stable, pre-GUI-redesign

---

## Roadmap / Ideas

- Visual polish (shadows, gradient, more “Logic” bling)
- Drag-and-drop file loading
- LUFS or auto-leveling (future)
- Bypass/mute indicator

---

## Credits

By [Sašo](https://github.com/SireGrindalot) + ChatGPT-4o  
Built for fun and mix clarity.

---

*“First plugin I ever built, and it’s the one I always wanted!”*
