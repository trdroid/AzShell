# Video Demos

https://www.youtube.com/watch?v=Y0zxso81Sr0
<br />
https://www.youtube.com/watch?v=Gs-N1FqZpwA

# Snapshots

## Splashscreen of the Shell

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/logo.jpg)

This is my first logo. The logo is displayed upon booting from the disk. It doesn't reflect the progress in loading the kernel. The kernel/$hell is loaded after the progression bar is drawn.

<br />
<br />
<br />
<br />
<br />

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/new_logo.jpg?raw=true)

This is my new logo. I made a little progress by making the progression bar from fake to real in nature. I now have a two stage boot-loader :) The first stage loads the second stage which reads the $hell-kernel sectors into memory and reflects this process of loading using a progression bar.

<br />
<br />
<br />
<br />
<br />

## Commands provided by the Shell

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/list_of_commands.bmp)

A list of commands I have implemented by the end of the project. I have coded the underlying system calls to support the above listed commands. Any application program, like the editor that I created, should be embedded in the shell to make use of the system calls. I do not have the system calls installed as interrupts, like int 0x80 of linux systems, to support application programs.

<br />
<br />

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/command_usage1.jpg?raw=true)

Getting the usage of some commands

<br />
<br />
<br />
<br />
<br />

## File System Commands

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/file_system_commands.bmp)

Getting the usage of the file-system commands

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/file-system_commands_usage.bmp)

Demo of few file-system commands

<br />
<br />
<br />
<br />
<br />

## File System Layout

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/initial-layout.bmp)

A depiction of how my system is organized on the disk. It initially had a single stage boot-loader to display the "Az" logo with a fake progression bar and subsequently load the kernel/shell. Unlike DOS, the kernel/shell is not part of the file system. Consequently my boot-loader is unaware of the file-system format.

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/modified-layout.bmp)

A second stage boot-loader was a natural consequence of making the progression bar real. The kernel/shell doesn't occupy all the sectors reserved for it. As of now, excluding the editor, it is 48 sectors in size. The remaining sectors are for future usage.

<br />
<br />
<br />
<br />
<br />

## Demonstrating that the Shell can load and run a Program

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/tic1.jpg?raw=true)

I made a Tic-Tac-Toe game to demonstrate memory management in a single-tasking system. The $hell/kernel should load the game from the floppy and pass the control to it. After the user desires to quit the game, the control should be returned back to the kernel.

<br />
<br />

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/tic2.jpg?raw=true)

Upon saying 'tictac' in the command line, the system is now able to read & load the game into memory allotted for a single process. The user could play until he/she chooses to quit by pressing 'q'.

<br />
<br />

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/tic3.jpg?raw=true)

The context switching is handled in a manner so as to not to let the loaded process(game) tamper with the context of the $hell-kernel.

<br />
<br />
<br />
<br />
<br />

## First version of the Editor (with no scrolling feature)

![](https://github.com/trdroid/AzShell/blob/main/Snapshots/editor1.jpg?raw=true)

A Simple Editor provided by the shell

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/editor2.bmp)

The File Menu

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/editor3.bmp)

The Save Option

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/editor4.bmp)

On clicking on the Save Option

<br />
<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/editor5.bmp)

The Edit Menu

<br />
<br />
<br />
<br />
<br />

## Second version of the Editor (with scrolling feature)

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/text_editor1.bmp)

Unlike the previous text editor, this editor is tailored to display current cursor location near the bottom left corner. It also effectively handles the scrolling of text when the number of lines exceeds the screen's capacity. Notice the scrolling in the next snapshot.

<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/text_editor2.bmp)

The text in the editor has scrolled up by one line

<br />

![](https://raw.githubusercontent.com/trdroid/AzShell/main/Snapshots/text_editor3.bmp)

The text has scrolled up by many lines. Just like with the normal editors, the unseen text could be retrieved by taking the cursor to the first visible line and repeatedly pressing the 'up' arrow key until all the text is retrieved.
