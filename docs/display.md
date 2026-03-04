# display.c

This module provides a high-level interface for controlling a 7-segment display (specifically the TDSR0760). It handles character mapping, segment manipulation, and basic animation support, serving as an abstraction layer over the hardware driver.

## Structures and Enums

`DisplayReturnCode`: standard return codes for display functions. Includes `OK`, `NULL_POINTER`, `INVALID_SEGMENT`, `INVALID_STATUS`, `INVALID_CHARACTER`, and `DRIVER_ERROR`.

`DisplaySegmentStatus`: defines the state of an individual segment: `OFF`, `ON`, or `UNKNOWN`. These map directly to the underlying driver states.

`DisplaySegment`: enum representing the physical segments of the display: `TOP`, `TOP_RIGHT`, `TOP_LEFT`, `MIDDLE`, `BOTTOM`, `BOTTOM_RIGHT`, `BOTTOM_LEFT`, and `DECIMAL_POINT`.

`DisplayCharacterCode`: a bitmask where each bit represents a specific `DisplaySegment`. Pre-defined codes are provided for numbers (0-9), hex digits (A-F), and various ASCII symbols/letters. It is obtained by anding the various segments.

`_Display`: the internal handler structure.

 - `.set`: callback function to set a segment's physical state.

 - `.toggle`: callback function to toggle a segment's physical state.

 - `.tdsr0760`: the low-level driver handler structure.

## Functions

`display_init`: initializes the display module. It requires two callback functions (set and toggle) that link the module to the actual hardware GPIO/control pins. It clears the internal handler and initializes the low-level tdsr0760 driver.

`display_get_code_from_hex_digit`: maps a numeric value (0-15) to its corresponding 7-segment character code (0-9, A-F).

`display_get_code_from_character`: maps an ASCII character to a `DisplayCharacterCode`. It supports numbers, basic symbols (., -, _), and a subset of the alphabet. It includes logic for handling ignore_case and prefer_upcase to choose between capital and lowercase representations where possible (e.g., 'A' vs 'a').

`display_get_segment`: returns the current DisplaySegmentStatus of a specific segment by querying the low-level driver.

`display_set_segment`: updates the status of a specific segment. It updates the driver state and then executes the hardware set callback.

`display_toggle_segment`: toggles the state of a specific segment. It queries the driver for the current state, flips it, and updates the hardware via the set callback.

`display_set_segment_all`: updates all segments at once using a 8-bit bitflag. This is the core function used by higher-level "show character" functions to refresh the entire display.

`display_set_digit`: a wrapper that converts a hex digit to a code and updates the entire display to show that digit.

`display_set_character`: a wrapper that converts an ASCII character to a code (considering case preferences) and updates the entire display.

`display_run_animation`: executes a single frame of a segment-based animation. It calculates which frame to show based on the current system ticks (t), the ticks_per_frame duration, and the total size of the animation array.

`display_run_animation_string`: similar to display_run_animation, but cycles through the characters of a string. Each frame represents one character of the string displayed for the duration of ticks_per_frame.