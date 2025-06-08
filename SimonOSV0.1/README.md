# SimonOS Kernel v0.1

This is a simple hobby operating system kernel written in C.  
It features basic VGA text output, keyboard input handling, and a simple shell.

## Features

- Multiboot compliant header for bootloader compatibility (e.g., GRUB)
- Direct VGA text mode output at memory address `0xB8000`
- Cursor position management via VGA ports
- Keyboard input reading using PC scancodes, with basic ASCII translation
- Simple shell interface supporting commands:
  - `help` — list available commands
  - `echo` — repeat user input
  - `clear` — clear the screen

## Usage

Compile the kernel and load it with a multiboot-compliant bootloader like GRUB.

Once running, the shell prompts with `SimonOS >` allowing you to enter commands.

## Notes

- The keyboard input currently does not handle shift, ctrl, or alt modifiers.
- Only a limited set of ASCII characters is supported.
- The shell and command set are minimal and meant for educational purposes.

## Future improvements

- Implement support for modifier keys (Shift, Ctrl, Alt)
- Add more shell commands (e.g., file system operations)
- Improve input handling and line editing features

---

Feel free to contribute or customize this kernel to your needs!

