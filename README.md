# NKPK-1 Synth Plugin

**Version 1.1.0**

## Overview

The NKPK-1 is a bespoke, polyphonic subtractive synthesizer VST/AU/Standalone plugin designed for instant, tactile control with the Akai MPK Mini. It is crafted for warm, evolving soundscapes, featuring hard-coded knob and pad mappings for a true plug-and-play instrument experience.

## What's New in This Version

This version represents a major step forward, establishing a stable, high-quality audio engine and introducing unique performance features.

* **Complete DSP Core Refactor:** The synthesizer's internal signal path has been rewritten to the professional standard (`Oscillator -> Filter -> Amplifier`) and now uses per-sample processing for a significantly smoother, higher-quality sound.
* **"Kaoss Stick" Implementation (Mode 1):** The X/Y joystick is now a primary performance control for real-time, intuitive manipulation of the filter.
* **Codebase Restructure:** The `SynthVoice` logic has been modularized into its own dedicated files for improved code clarity and maintainability.
* **Bug Fixes & Stability:** Corrected critical bugs related to parameter handling and established a stable build process.

## Key Features

* **Polyphonic Architecture:** Supports up to 8 voices for rich chords and pads.
* **Direct Akai MPK Mini Integration:** No MIDI mapping required in your DAW!
* **8 Knobs (MIDI CC 70-77) for Core Sound Shaping:**
    * **Knob 1 (CC 70):** Oscillator Wave Type (Sine to Saw blend)
    * **Knob 2 (CC 71):** Filter Cutoff (Sets the base cutoff frequency)
    * **Knob 3 (CC 72):** Filter Resonance (Sets the base resonance amount)
    * **Knob 4 (CC 73):** Filter Envelope Amount
    * **Knob 5 (CC 74):** Amplifier Attack Time
    * **Knob 6 (CC 75):** Amplifier Decay Time
    * **Knob 7 (CC 76):** Amplifier Sustain Level
    * **Knob 8 (CC 77):** Amplifier Release Time
* **8 Pads (MIDI Notes 36-43) for On/Off Toggles:**
    * **Pad 1 (Note 36):** Mono/Poly Toggle (Functional)
    * **Pads 2-8:** Control wiring is in place for future DSP features.
* **X/Y Stick ("Kaoss Stick" Mode 1 - Filter Pad):**
    * **X-Axis (Pitch Bend):** Modulates the filter's cutoff frequency for expressive sweeps.
    * **Y-Axis (Modulation CC#1):** Modulates the filter's resonance for added character.
* **Basic Oscillator:** Generates blendable Sine and Sawtooth waveforms.
* **Full ADSR Envelope:** Shapes the volume of each note via the dedicated knobs.
* **State-Variable Filter:** A high-quality filter with its own dedicated ADSR envelope for shaping the timbre.

## Compatibility

* **Framework:** Developed using the modern JUCE Framework.
* **Controller:** Designed specifically for the Akai MPK Mini MIDI controller.
* **Formats:** Functions as a VST3, AU (macOS), and Standalone application.

## Building and Installation

To build this project yourself, you will need a basic C++ development environment set up.

#### Prerequisites:
* JUCE Framework SDK (Version 7 or newer)
* A C++ IDE:
    * **Windows:** Visual Studio 2022 (with "Desktop development with C++" workload)
    * **macOS:** Xcode
* The Projucer application (included with the JUCE SDK)

#### Build Steps:
1.  **Clone the Repository:** Open a terminal or Git client and clone the project to your local machine:
    ```
    git clone [https://github.com/saputo/NKPK-1.git](https://github.com/saputo/NKPK-1.git)
    ```
2.  **Open in Projucer:** Navigate to the cloned project folder and open the `NKPK-1.jucer` file with the Projucer application.
3.  **Save the Project:** In Projucer, simply save the project (File -> Save Project, or the floppy disk icon). This will generate the appropriate IDE project files (e.g., a Visual Studio `.sln` file) inside the `Builds/` directory.
4.  **Build in your IDE:** Open the generated solution/project file in Visual Studio or Xcode. Select the "Release" configuration and build the "StandalonePlugin" target.
5.  **Locate the Plugin:** The compiled plugin files (`.vst3`, `.exe`, `.component`, etc.) will be located in a subfolder within your project's `Builds/` directory (e.g., `Builds/VisualStudio2022/x64/Release/VST3/`).

#### Installation:
* To use the plugin, simply copy the compiled file (e.g., `NKPK-1.vst3`) into your system's appropriate plugin folder.
    * **Windows VST3:** `C:\Program Files\Common Files\VST3`
    * **macOS VST3:** `/Library/Audio/Plug-Ins/VST3`
    * **macOS AU:** `/Library/Audio/Plug-Ins/Components`

## Usage

1.  **Load the Plugin:** Load the NKPK-1 plugin onto an instrument track in your DAW, or run the Standalone application.
2.  **Connect Hardware:** Connect your Akai MPK Mini and ensure its MIDI output is routed to the NKPK-1 plugin track.
3.  **Play:** Play notes on the MPK Mini's keys to generate sound.
4.  **Shape the Sound:** Use the 8 knobs to control the oscillator, filter, and amplifier envelope to shape your sound in real-time.
5.  **Perform:** Use the X/Y joystick to create expressive and dynamic filter sweeps and resonance changes.
6.  **Toggle Modes:** Use the pads to toggle features like Mono/Poly mode.

## Future Roadmap

With a stable engine in place, future development will focus on expanding the sound-generating capabilities and user interface.

* **Sound Engine Enhancements:** The next priorities are implementing the DSP for the remaining toggleable features: Sub-Oscillator, Noise Generator, LFOs (for pitch and filter modulation), Glide/Portamento, the "Warp" effect, and a Chorus module.
* **Advanced Control:** The Filter ADSR parameters will be exposed to the user for full control. The "Kaoss Stick" will be expanded with new modes to control different parameter combinations.
* **Graphical User Interface (GUI):** A full custom GUI will be designed and implemented to provide visual control and feedback for all synth parameters.

## Credits

* Developed by saputo
