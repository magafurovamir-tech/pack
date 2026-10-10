# Pack 📦

A lightweight, dependency-free C command-line utility and interactive shell for archiving directories into a single custom `.pack` binary file and extracting them.

## Features

- **Two Modes of Operation:** Run it directly with command-line arguments or use the built-in interactive shell.
- **Custom Binary Format:** Simple and fast `PACK 1.0` archive format.
- **Colorized Output:** Clear and readable success, warning, and error messages in the terminal.
- **Dependency-Free:** Built using standard C libraries (`stdio.h`, `stdlib.h`, `dirent.h`, `sys/stat.h`, etc.).

## Compilation

To compile the project, you can use `gcc` or any standard C compiler. No external libraries are required.

```bash
gcc main.c -o pack
```

## Usage

You can use `pack` in two ways: via Command-Line Interface (CLI) or through its Interactive Shell.

### 1. Command-Line Interface (CLI)

Pass the commands directly as arguments when running the executable.

**Pack a directory:**
```bash
./pack build <directory_path>
```
*Example: `./pack build ./my_folder` will create a `my_folder.pack` file in the current directory.*

**Unpack an archive:**
```bash
./pack unpack <pack_file_path>
```
*Example: `./pack unpack my_folder.pack` will extract all files into the current working directory.*

### 2. Interactive Shell

If you run the utility without any arguments, it will launch the interactive shell.

```bash
./pack
```

Once inside the shell (`pack> `), you can use the following commands:
- `build <dir_path>` — Pack a target directory.
- `unpack <pack_file_path>` — Extract a `.pack` file.
- `help` — Show available commands.
- `exit` — Exit the interactive shell.

## The `.pack` File Format

The utility uses a simple custom binary format:
1. **Header:** `PACK 1.0\n`
2. **File Entries:** Each file entry consists of metadata and raw binary data:
   - Metadata format: `FILE <filename> SIZE <bytes>:\n`
   - Data: `[Raw binary bytes of the file]`

*(Note: Currently, the utility packs regular files inside the target directory. Subdirectories are not recursively packed in this version).*

## License

This project is dual-licensed under the **Business & Non-Commercial Copyleft Use** License. 

- **Non-Commercial Use:** Free for personal, educational, research, and non-commercial purposes under strict **Copyleft** (Share-Alike) conditions.
- **Commercial Use:** Requires a separate commercial agreement and is strictly prohibited without prior written consent from the Licensor. 

See the `LICENSE` file for the full legal text and details.
