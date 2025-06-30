# NKPK-1 Synth

**Version 1.2.0**

## Overview

The NKPK-1 is a bespoke, polyphonic subtractive synthesizer plugin (VST3/AU/Standalone) crafted for instant, tactile control with the Akai MPK Mini. It is designed for warm, evolving soundscapes, featuring a custom graphical interface that mirrors the hardware for a true plug-and-play instrument experience.

![Your Final Screenshot Here](https://i.imgur.com/your-screenshot-url.png)
*(Instructions: Take a screenshot of your final plugin UI and replace the URL above!)*

## Key Features (v1.2.0)

* **Custom Graphical User Interface:** A complete UI designed to visually match the Akai MPK Mini's aesthetic, with custom rotary knobs and interactive pads with the final layout (Pads | Kaoss Pad | Knobs).
* **Performance "Kaoss Pad":** The central X/Y pad provides real-time, intuitive manipulation of the filter, with a visualizer tracking the joystick's position.
    * **X-Axis (Pitch Bend):** Modulates the filter's cutoff frequency.
    * **Y-Axis (Modulation CC#1):** Modulates the filter's resonance.
* **Direct Akai MPK Mini Integration:** No MIDI mapping required in your DAW.
    * **8 Knobs (MIDI CC 70-77):** Control core sound-shaping parameters.
    * **8 Pads (MIDI Notes 36-43):** Toggle synth functions on and off.
* **Polyphonic Architecture:** Supports up to 8 voices for rich chords and pads.
* **Core Sound Engine:**
    * **Oscillator:** Generates blendable Sine and Sawtooth waveforms.
    * **Amplifier:** A full ADSR envelope shapes the volume of each note.
    * **Filter:** A high-quality State-Variable Filter with its own dedicated ADSR envelope.
* **Stable & Modular Codebase:** Refactored for high-quality sound and future expandability.

## Building and Installation

To build this project, you will need a basic C++ development environment.

#### Prerequisites:
* JUCE Framework (v7 or newer)
* A C++ IDE (Visual Studio 2022 on Windows, Xcode on macOS)
* The Projucer application (included with JUCE)

#### Build Steps:
1.  **Clone the Repository:** `git clone https://github.com/saputo/NKPK-1.git`
2.  **Open in Projucer:** Open the `NKPK-1.jucer` file in the project root.
3.  **Save Project:** In Projucer, save the project (File -> Save Project) to generate the IDE solution files in the `Builds/` directory.
4.  **Build in IDE:** Open the generated `.sln` (Visual Studio) or `.xcodeproj` (Xcode) file. Select the "Release" configuration and build the desired target (e.g., "StandalonePlugin", "VST3").

#### Installation:
Copy the compiled plugin file (e.g., `NKPK-1.vst3`) into your system's plugin folder:
* **Windows VST3:** `C:\Program Files\Common Files\VST3`
* **macOS VST3:** `/Library/Audio/Plug-Ins/VST3`
* **macOS AU:** `/Library/Audio/Plug-Ins/Components`

## Usage

1.  Load the NKPK-1 plugin in your DAW or run the Standalone application.
2.  Connect your Akai MPK Mini and route its MIDI output to the plugin.
3.  Play notes, turn the knobs, press the pads, and use the X/Y joystick to shape your sound!

## Roadmap

With a stable interface and core engine, development will now focus on sound design features.

* **Milestone 2: Sound Engine Expansion (Up Next!)**
    * Implement DSP for all toggleable pad functions, starting with the Sub-Oscillator.
* **Milestone 3: Experimental 'Kaoss' Mode**
    * Implement a "Theremin" mode where clicking and dragging on the Kaoss Pad generates a continuous tone.

## Credits
* **Developed by:** saputo
* **AI Assistant:** VST Coder (Gemini)