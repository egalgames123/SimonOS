# SimonOS Kernel v0.2

SimonOS is a simple hobby operating system kernel written in C.  
It features VGA text output, keyboard input with modifier key support, a scrolling console, a minimal shell, and an in-memory file system.

## Features

- **Multiboot-compliant** – Compatible with GRUB and other Multiboot bootloaders
- **Direct VGA output** – Text mode output via memory address `0xB8000`
- **Cursor management** – Controlled via VGA ports
- **Keyboard input** – Scancode reading with ASCII translation and modifier key support (Shift, Ctrl, Alt)
- **Scrolling console** – Output scrolls automatically when reaching the bottom
- **Minimal shell with the following commands**:
  - `help` — list available commands
  - `echo` — print user input
  - `clear` — clear the screen
  - `touch <name>` — create a new file in memory
  - `cat <name>` — display file contents
  - `rm <name>` — delete a file
  - `echo <text> > <name>` — write text to a file

## File System (SimonFS)

A simple in-memory file system:

- Manages multiple files stored in RAM
- Each file has a name and content
- Basic file operations (`create`, `read`, `write`, `delete`)

## Usage

Compile the kernel and load it using a Multiboot-compliant bootloader like GRUB.

Once booted, the shell prompts with:

