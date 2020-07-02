# FEXP
Fexp is a terminal based file explorer.
![](https://i.imgur.com/P2Okwqo.png)
## Installation
Go to releases and download the [latest](https://github.com/Serj-0/fexp/releases/tag/v0.3.2) fexp. Relocate fexp to /usr/bin or another executable location.
## Usage
All the files and directories in the current path are listed in alphabetical order. Files that you do not have read permissions for or that do not exist are highlighted in red. The current selection is highlighted in blue and written at the bottom of the terminal.

When fexp is ran with no argument, the current directory is used as the entry point. Fexp can be ran with one argument defining a path of entry, e.g.
> fexp /tmp


### Navigation `<- ^ v ->`
Navigation is done with `w a s d` or the `Up Left Down Right` arrow keys.

- `q` to quit.
- `d` or `Right` to enter a selected directory.
- `a` or `Left` to exit the current directory.
- `w` or `Up` to move selection up.
- `s` or `Down` to move selection down.
  - Hold `Shift` when moving up or down to move three spaces.
- `Space` to begin a string command.
- `,` or `.` to select previous or next file.
- `[` or `]` to select previous or next directory.
- `Page Up` or `Page Down` to jump one page up or down the file list.

### String Commands `? $`
When typing in a string command, the closest matching file or directory name will be suggested and can be completed using `Tab`.
 After typing a directory or file path, `Enter` can be pressed to move to that directory or file, e.g.
> /usr/local/bin

Typing `?` and then a terminal command will execute that command, e.g.
> ? touch .newfile

If a direcory or file path is typed before `?` and a command, that path can be used in the command using `$`, e.g.
> /home/me/Pictures ? ls $ > piclist.txt

If no path is typed before `?` then `$` will translate to the currently select file or directory, e.g.
> ? mv $/oldfile /home/me/documents

String command mode can be escaped by pressing `Backtick`.
