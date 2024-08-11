#define TEXT0 0x00
#define TEXT1 0x03
#define GRAPHICAL 0x13

#define BCD_TO_BIN(val) ((val & 15) + (val >> 4) * 10)
#define ARR_TO_UCH(val1,val2) ((val1 << 4) + val2)

#define ARRAY_ROWS 20
#define ARRAY_COLS 50

#define BUF_LENGTH 254
#define CMD_HISTORY_LENGTH 5

#define MAX_DIR_ENTRIES 15

void puts(char *);
void putchar(unsigned char);
unsigned char readKey();
void setVideoMode(unsigned char);
void readCommandLine();
void tokenize(char *, char, char [][ARRAY_COLS]);
int strcmp(const char*, const char *);
unsigned int strlen(const char *);
void setCursorPos(unsigned char, unsigned char, unsigned char);
void getCursorPos(unsigned char *, unsigned char *, unsigned char);
void drawWindow(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void printEquipmentList();
void getDate();
void setDate();
void getTime();
void setTime();
void printUnsignedChar(unsigned char);
void printUnsignedInt(unsigned int);
void clearCommandLineArray();
unsigned char validateTime(char *time);
unsigned char validateDate(char *date);
void setSpaceColor(unsigned char);
unsigned char validateSpaceColor(char *);
unsigned char validateSpace(char *);
unsigned char toUpper(unsigned char c);
unsigned char findSubtrahend(char c);
void switchTo(char *pg);
void setCursorSize(unsigned char startScanLine, unsigned char endScanLine);
unsigned char inb(unsigned int port);
void outb(unsigned int port, unsigned char value);
void scrollUp();
unsigned char findWeekDayIndex();
void writeSector(unsigned char *sect, unsigned char numSectors, unsigned char track, unsigned char sector, unsigned char head, unsigned char drive);
void readSector(unsigned char *sect, unsigned char numSectors, unsigned char track, unsigned char sector, unsigned char head, unsigned char drive);
void strcpy(char *dest, char *src);
void makeFileEntry(unsigned int seek);
int returnFATFreeSlot();
void mapSlotToDisk(int slot);
void strcat(char *src, char *toAdd);
void getFATSector(unsigned int sectorNumber);
void fillFileDetails(char *fileName, char *extension, unsigned char attribute, unsigned int startingSector, unsigned long size);
char mkdir(char *name);
char doesFileExist(char *name);
void listDirectory(unsigned int startingSector);
void load(unsigned int startingSector);
//char isFloppyInserted();
void drawEditorLayout();
void putc(char c);


char cmdHistory[CMD_HISTORY_LENGTH][BUF_LENGTH];
unsigned char cmdHistoryIndex;
unsigned char upCount;

char commandLine[BUF_LENGTH];
char commandLineArray[ARRAY_ROWS][ARRAY_COLS];
unsigned char arrayIndex;	//number of tokens
unsigned char cmdIndex;		//end of cmd
unsigned char cmdPointer;

unsigned char commandBeingTyped;

unsigned char day, month, year, century;
unsigned char hours, minutes, seconds;

unsigned char spaceColor[4][2] = { "70", "70", "70", "70"};
unsigned char activeSpace = 0x00;

unsigned char scanCode;

unsigned char spaceLines[4];

char monthName[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
unsigned char monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
unsigned char weekDays[7][4] = {"Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"};

unsigned char cmdBeginsAtRow;

unsigned char shellPromptLength;

unsigned char insertState = 0;
unsigned char isDirectory = 0;

unsigned char far *videoMemory;

char shellPrompt[30];
char prompt[10] = "Az-$hell:";

char processLoaded = 0;
char isExe = 0;

char dateEntities[3][ARRAY_COLS];
char timeEntities[3][ARRAY_COLS];
char dirEntities[10][ARRAY_COLS];

struct FileEntry
{
	char fileName[9];
	char extension[4];
	unsigned char attribute;
	char reserved[5];
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
	unsigned char day;
	unsigned char month;
	unsigned char century;
	unsigned char year;
	unsigned int startingSector;
	unsigned long size;
}file;

struct DiskLocation
{
	unsigned char track;
	unsigned char sector;
	unsigned char head;
}disk, FATSector;

struct PWDDetails
{
	char filePath[MAX_DIR_ENTRIES][9];
	unsigned int startingSector[MAX_DIR_ENTRIES];
	unsigned char sectorIndex;
}pwd, pwdSoFar;

unsigned char sector[512];

unsigned char FAT[512];

void main()
{
	unsigned char row, col;

	setVideoMode(TEXT1);

	pwd.sectorIndex = pwdSoFar.sectorIndex = 0;
	pwd.startingSector[pwd.sectorIndex] = pwdSoFar.startingSector[pwdSoFar.sectorIndex] = 0;

	strcpy(pwd.filePath[pwd.sectorIndex], "/");
	strcpy(pwdSoFar.filePath[pwdSoFar.sectorIndex], "/");

	strcpy(shellPrompt, prompt);
	strcat(shellPrompt, pwd.filePath[pwd.sectorIndex]);
	strcat(shellPrompt, ">");

	shellPromptLength = strlen(shellPrompt);

	asm mov ax, 0x1003
	asm mov bx, 0x0
	asm int 0x10

	while(1)
	{
		getCursorPos(&row, &col, activeSpace);

		cmdBeginsAtRow = row;

		cmdIndex = cmdPointer = 0;

		commandBeingTyped = 1;

		puts(shellPrompt);

		clearCommandLineArray();

		readCommandLine();

		tokenize(commandLine, ' ', commandLineArray);

		if(commandLineArray[0][0] == '\0')
			continue;
		else if(!strcmp(commandLineArray[0], "space"))
		{
			if(commandLineArray[1][0] != '\0')
			{
				if(validateSpace(commandLineArray[1]))
					switchTo(commandLineArray[1]);
				else
				{
					puts("Invalid space entry. Format: space {workspace[1-4]}");
					putchar(0x0d);
				}
			}
			else
			{
				puts("Operating in WorkSpace ");
				putchar(activeSpace + 49);
				putchar(0x0d);
			}
		}
		else if(!strcmp(commandLineArray[0], "reboot"))
		{
			if(commandLineArray[1][0] != '\0')
			{
				puts("Invalid option used with reboot");
				putchar(0x0d);
			}
			else
				asm int 0x19
		}
		else if(!strcmp(commandLineArray[0], "help"))
		{
			if(commandLineArray[1][0] == '\0')
			{
				puts("clear   clears the screen");
				putchar(0x0d);
				puts("date    displays current date");
				putchar(0x0d);
				puts("time    displays current time");
				putchar(0x0d);
				puts("reboot  restarts the system");
				putchar(0x0d);
				puts("help    displays a list of commands");
				putchar(0x0d);
				puts("color   sets console foreground and background colors");
				putchar(0x0d);
				puts("space   shifts to anyone of the four workspaces");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "time"))
			{
				puts("Usage: time {time}");
				putchar(0x0d);
				puts("DEFAULT: displays current time");
				putchar(0x0d);
				puts("time [hr:min:sec]: changes current time to hr:min:sec");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "date"))
			{
				puts("Usage: date {Date}");
				putchar(0x0d);
				puts("DEFAULT: displays current date");
				putchar(0x0d);
				puts("Date [mm-dd-yy]: changes current date to mm-dd-yy");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "help"))
			{
				puts("Usage : help {command}");
				putchar(0x0d);
				puts("DEFAULT: displays a list of commands");
				putchar(0x0d);
				puts("command: displays information on the command");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "space"))
			{
				puts("Usage : space {workspace}");
				putchar(0x0d);
				puts("DEFAULT: displays current workspace");
				putchar(0x0d);
				puts("workspace[1-4]: shifts to anyone of the four workspaces");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "clear"))
			{
				puts("Usage: clear");
				putchar(0x0d);
				puts("Clears the screen");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "reboot"))
			{
				puts("Usage: reboot");
				putchar(0x0d);
				puts("Restarts the system");
				putchar(0x0d);
			}
			else if(!strcmp(commandLineArray[1], "color"))
			{
				puts("Usage: color {attr}");
				putchar(0x0d);
				puts("DEFAULT: no changes to color settings");
				putchar(0x0d);
				puts("attr: Specifies color attribute of console output");
				putchar(0x0d);
				puts("Color attributes are specified by TWO hex digits");
				putchar(0x0d);
				puts("The first corresponds to foreground and second to background");
				putchar(0x0d);
				putchar(0x0d);
				puts("0=BLACK  1=BLUE  2=GREEN  3=AQUA  4=RED  5=PURPLE  6=YELLOW  7=WHITE  8=Gray  ");
				putchar(0x0d);
				putchar(0x0d);
				puts("9=L.BLUE  A=L.GREEN  B=L.AQUA  C=L.RED  D=L.PURPLE  E=L.YELLOW  F=B.WHITE");
				putchar(0x0d);
				putchar(0x0d);
			}
			else
			{
				puts("This command is not recognized by the help utility");
				putchar(0x0d);
			}
		}
		else if(!strcmp(commandLineArray[0], "clear"))
		{
			if(commandLineArray[1][0] != '\0')
			{
				puts("Invalid option used with clear");
				putchar(0x0d);
			}
			else
			{
				unsigned char colorCode;

				colorCode = ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0]));

				drawWindow(0x00, colorCode, 0x0, 0x0, 24, 79);
				setCursorPos(0x0, 0x0, activeSpace);

				spaceLines[activeSpace] = 0;
			}
		}
		else if(!strcmp(commandLineArray[0], "date"))
		{
			if(commandLineArray[1][0] == '\0')
			{
				getDate();
				puts("The current date (mm-dd-yy) is : ");

				printUnsignedChar(month);
				putchar('/');
				printUnsignedChar(day);
				putchar('/');
				printUnsignedInt(BCD_TO_BIN(century) * 100 + BCD_TO_BIN(year));

				puts("  ");

				puts(weekDays[findWeekDayIndex()]);

				puts(" ");

				printUnsignedChar(day);
				putchar('-');
				puts(monthName[BCD_TO_BIN(month) - 1]);
				putchar('-');
				printUnsignedInt(BCD_TO_BIN(century) * 100 + BCD_TO_BIN(year));

				putchar(0x0d);
			}
			else
			{
				if(validateDate(commandLineArray[1]))
					setDate();
				else
				{
					puts("Invalid date entry. Format: mm[1-12]-day[1-31]:year[0001-9999]");
					putchar(0x0d);
				}
			}
		}
		else if(!strcmp(commandLineArray[0], "time"))
		{
			if(commandLineArray[1][0] == '\0')
			{
				getTime();

				puts("CURRENT TIME (24 hr) ");

				printUnsignedChar(hours);
				putchar(':');
				printUnsignedChar(minutes);
				putchar(':');
				printUnsignedChar(seconds);

				puts("  (12 hr) ");

				if(BCD_TO_BIN(hours) == 0)
					printUnsignedChar(0x12);
				else if(BCD_TO_BIN(hours) == 22)
					printUnsignedChar(0x10);
				else if(BCD_TO_BIN(hours) == 23)
					printUnsignedChar(0x11);
				else if(BCD_TO_BIN(hours) >= 13)
					printUnsignedChar((unsigned char)(BCD_TO_BIN(hours) - 12));
				else
					printUnsignedChar(hours);

				putchar(':');
				printUnsignedChar(minutes);
				putchar(':');
				printUnsignedChar(seconds);

				if(BCD_TO_BIN(hours) >= 0 && BCD_TO_BIN(hours) <= 11)
					puts(" AM");
				else
					puts(" PM");

				putchar(0x0d);
			}
			else
			{
				if(validateTime(commandLineArray[1]))
					setTime();
				else
				{
					puts("Invalid time entry. Format: hr[0-23]:min[0-59]:sec[0-59]; No spaces");
					putchar(0x0d);
				}
			}
		}
		else if(!strcmp(commandLineArray[0], "color"))
		{
			if(commandLineArray[1][0] != '\0')
				if(validateSpaceColor(commandLineArray[1]))
					setSpaceColor(activeSpace);
				else
				{
					puts("Invalid option. Format: {fg[0-9 a-f A-F] {bg[0-9 a-f A-F]}}");
					putchar(0x0d);
				}
		}
		else if(!strcmp(commandLineArray[0], "list"))
		{
			puts("Convetional Memory size is ");
			printEquipmentList();
			putchar(0x0d);
		}
		else if(!strcmp(commandLineArray[0], "format"))
		{
			char c;

/*			if(!isFloppyInserted())
			{
				puts("Insert a floppy disk first.");
				putchar(0x0d);
				continue;
			}*/

			if(commandLineArray[1][0] != '\0')
			{
				puts("Invalid option used with format");
				putchar(0x0d);
				continue;
			}

			puts("Are you sure you want to format (y/n):");
			c = readKey();

			if(toUpper(c) == 'Y')
			{
				unsigned int iter;

				putchar(0x0d);
				puts("Formatting...");

				fillFileDetails("/", "", 0x80, 0, 1);

				makeFileEntry(0);

				writeSector(sector, 1, 2, 18, 1, 0);

				for(iter = 0; iter < 512; iter++)
					FAT[iter] = 0;

				writeSector(FAT, 1, 4, 1, 0, 0); //root entry points to this; has nothing to do with FAT; using FAT just because it is all 0's

				FAT[0] = 0xFF;
				FAT[1] = 0xFF;

				writeSector(FAT, 1, 3, 1, 0, 0);

				FAT[0] = 0x00;
				FAT[1] = 0x00;

				for(iter = 2; iter <= 10; iter++)	//clearing FAT
					writeSector(FAT, 1, 3, iter, 0, 0);

				for(iter = 402; iter <= 511; iter++)
					FAT[iter] = 0xFF;

				writeSector(FAT, 1, 3, 11, 0, 0);

				pwd.sectorIndex = 0;

				strcpy(shellPrompt, prompt);
				strcat(shellPrompt, pwd.filePath[pwd.sectorIndex]);
				strcat(shellPrompt, ">");

				shellPromptLength = strlen(shellPrompt);

				putchar(0x0d);
				puts("Disk has been formatted");
			}

			putchar(0x0d);
		}
		else if(!strcmp(commandLineArray[0], "mkdir"))
		{
			char status;
			unsigned char iter = 0;

			if(commandLineArray[1][0] == '\0')
			{
				puts("mkdir: missing operand");
				putchar(0x0d);
				continue;
			}

			while(commandLineArray[++iter][0])
			{
				unsigned char iter1 = 0;

				if(commandLineArray[iter][0] == '/')
				{
					pwdSoFar.sectorIndex = 0;
				}
				else
				{
					unsigned int iter;

					for(iter = 0; iter <= pwd.sectorIndex; iter++)
					{
						strcpy(pwdSoFar.filePath[iter], pwd.filePath[iter]);
						pwdSoFar.startingSector[iter] = pwd.startingSector[iter];
					}
					pwdSoFar.sectorIndex = pwd.sectorIndex;
				}

				tokenize(commandLineArray[iter], '/', dirEntities);

				if(arrayIndex == 0)	//implies no dirEntries
				{
					puts("Cannot create directory '/': File Exists");
					putchar(0x0d);
					continue;
				}

				for(iter1 = 0; iter1 < arrayIndex - 1; iter1++)
				{
					if(!strcmp(dirEntities[iter1], "."))
						continue;
					else if(!strcmp(dirEntities[iter1], ".."))
					{
						if(pwdSoFar.sectorIndex > 0)
							pwdSoFar.sectorIndex--;
					}
					else if(!doesFileExist(dirEntities[iter1]))
					{
						puts("Error: cannot create directory '");
						puts(commandLineArray[iter]);
						puts("': No such file or directory");
						putchar(0x0d);

						break;
					}

					if(!isDirectory)
					{
						puts("Error: cannot create directory '");
						puts(commandLineArray[1]);
						puts("': Not a directory");
						putchar(0x0d);
						break;
					}
				}

				if(iter1 < arrayIndex - 1)
					continue;

				if(!strcmp(dirEntities[iter1], ".") || !strcmp(dirEntities[iter1], ".."))
					status = -1;
				else
					status = mkdir(dirEntities[iter1]);

				if(status == -1)
				{
					puts("Error : cannot create directory ");
					putchar('\'');
					puts(commandLineArray[iter]);
					putchar('\'');
					puts(" : File exists");
					putchar(0x0d);
				}
				else if(status == -2)
				{
					puts("Disk out of memory. Directory creation has failed.");
					putchar(0x0d);
				}
			}
		}
		else if(!strcmp(commandLineArray[0], "cd"))
		{
			if(commandLineArray[1][0] == '\0')
			{
				pwd.sectorIndex = 0;

				strcpy(shellPrompt, prompt);
				strcat(shellPrompt, pwd.filePath[pwd.sectorIndex]);
				strcat(shellPrompt, ">");

				shellPromptLength = strlen(shellPrompt);
			}
			else
			{
				unsigned char iter1 = 0, iter2;

				if(commandLineArray[1][0] == '/')
				{
					pwdSoFar.sectorIndex = 0;
				}
				else
				{
					unsigned char iter;

					for(iter = 0; iter <= pwd.sectorIndex; iter++)
					{
						strcpy(pwdSoFar.filePath[iter], pwd.filePath[iter]);
						pwdSoFar.startingSector[iter] = pwd.startingSector[iter];
					}

					pwdSoFar.sectorIndex = pwd.sectorIndex;
				}

				tokenize(commandLineArray[1], '/', dirEntities);

				if(arrayIndex == 0)	//implies no dirEntries
				{
					pwd.sectorIndex = 0;

					strcpy(shellPrompt, prompt);
					strcat(shellPrompt, pwd.filePath[pwd.sectorIndex]);
					strcat(shellPrompt, ">");

					shellPromptLength = strlen(shellPrompt);

					continue;
				}

				for(iter1 = 0; iter1 < arrayIndex; iter1++)
				{
					if(!strcmp(dirEntities[iter1], "."))
						continue;
					else if(!strcmp(dirEntities[iter1], ".."))
					{
						if(pwdSoFar.sectorIndex > 0)
							pwdSoFar.sectorIndex--;
					}
					else if(!doesFileExist(dirEntities[iter1]))
					{
						putchar('\'');
						puts(commandLineArray[1]);
						puts("': No such file or directory");
						putchar(0x0d);
						break;
					}

					if(!isDirectory)
					{
						putchar('\'');
						puts(commandLineArray[1]);
						puts("': Not a directory");
						putchar(0x0d);
						break;
					}
				}

				if(iter1 < arrayIndex)
					continue;

				for(iter2 = 0; iter2 <= pwdSoFar.sectorIndex; iter2++)
				{
					strcpy(pwd.filePath[iter2], pwdSoFar.filePath[iter2]);
					pwd.startingSector[iter2] = pwdSoFar.startingSector[iter2];
				}

				pwd.sectorIndex = pwdSoFar.sectorIndex;

				strcpy(shellPrompt, prompt);
				strcat(shellPrompt, pwd.filePath[pwd.sectorIndex]);
				strcat(shellPrompt, ">");

				shellPromptLength = strlen(shellPrompt);
			}
		}
		else if(!strcmp(commandLineArray[0], "ls"))
		{
			unsigned char iter = 0;
			unsigned char tempArrayIndex = arrayIndex;

			if(commandLineArray[1][0] == '\0')
			{
				listDirectory(pwd.startingSector[pwd.sectorIndex]);
				continue;
			}

			while(commandLineArray[++iter][0])
			{
				unsigned char iter1 = 0;

				if(commandLineArray[iter][0] == '/')
				{
					pwdSoFar.sectorIndex = 0;
				}
				else
				{
					unsigned char iter;

					for(iter = 0; iter <= pwd.sectorIndex; iter++)
					{
						strcpy(pwdSoFar.filePath[iter], pwd.filePath[iter]);
						pwdSoFar.startingSector[iter] = pwd.startingSector[iter];
					}

					pwdSoFar.sectorIndex = pwd.sectorIndex;
				}

				tokenize(commandLineArray[iter], '/', dirEntities);

				for(iter1 = 0; iter1 < arrayIndex; iter1++)
				{
					if(!strcmp(dirEntities[iter1], "."))
						continue;
					else if(!strcmp(dirEntities[iter1], ".."))
					{
						if(pwdSoFar.sectorIndex > 0)
							pwdSoFar.sectorIndex--;
					}
					else if(!doesFileExist(dirEntities[iter1]))
					{
						puts("Cannot access '");
						puts(commandLineArray[iter]);
						puts("': No such file or directory");
						putchar(0x0d);
						break;
					}
				}

				if(iter1 < arrayIndex)
					continue;

				if(tempArrayIndex > 2)
				{
					puts(commandLineArray[iter]);
					putchar(':');
					putchar(0x0d);
				}

				listDirectory(pwdSoFar.startingSector[pwdSoFar.sectorIndex]);

				if(tempArrayIndex > 2)
					putchar(0x0d);
			}
		}
		else if(!strcmp(commandLineArray[0], "pwd"))
		{
			unsigned char iter;

			if(commandLineArray[1][0] != '\0')
			{
				puts("Invalid option used with pwd");
				putchar(0x0d);
				continue;
			}

			if(pwd.sectorIndex == 0)
				putchar('/');
			else
				for(iter = 1; iter <= pwd.sectorIndex; iter++)
				{
					putchar('/');
					puts(pwd.filePath[iter]);
				}

			putchar(0x0d);
		}
		else if(!strcmp(commandLineArray[0], "rm"))
		{
		}
		else if(!strcmp(commandLineArray[0], "mv"))
		{
		}
		else if(!strcmp(commandLineArray[0], "cp"))
		{
		}
		else if(!strcmp(commandLineArray[0], "ed"))
		{
			char c, row, col;

			asm mov ah, 0x05
			asm mov al, 0x04	// switch to page 4 (0..7)
			asm int 0x10

			drawEditorLayout();

			while(1)
			{
				c = readKey();

				if(scanCode == 0x01)
					break;

				else if(c == 0x0d)
				{
					getCursorPos(&row, &col, 0x04);
					setCursorPos(row + 1, 1, 0x04);
				}

				else
				{
					putc(c);

					getCursorPos(&row, &col, 0x04);

					if((col + 1) % 80 == 0)
						setCursorPos(row + 1, 1, 0x04);
				}
			}

			asm mov ah, 0x05
			asm mov al, activeSpace	// switch to page 4 (0..7)
			asm int 0x10
		}
		else
		{
			unsigned char iter1 = 0, iter2;

			if(commandLineArray[0][0] == '/')
			{
				pwdSoFar.sectorIndex = 0;
			}
			else
			{
				unsigned char iter;

				for(iter = 0; iter <= pwd.sectorIndex; iter++)
				{
					strcpy(pwdSoFar.filePath[iter], pwd.filePath[iter]);
					pwdSoFar.startingSector[iter] = pwd.startingSector[iter];
				}

				pwdSoFar.sectorIndex = pwd.sectorIndex;
			}

			tokenize(commandLineArray[0], '/', dirEntities);

			for(iter1 = 0; iter1 < arrayIndex; iter1++)
			{
				if(!strcmp(dirEntities[iter1], "."))
					continue;
				else if(!strcmp(dirEntities[iter1], ".."))
				{
					if(pwdSoFar.sectorIndex > 0)
						pwdSoFar.sectorIndex--;
				}
				else if(!doesFileExist(dirEntities[iter1]))
				{
					break;
				}
			}

			if(iter1 < arrayIndex)
			{
				puts("Invalid command/operable program. Type 'help' for a list of commands");
				putchar(0x0d);
				continue;
			}

			if(!isDirectory && isExe)
			{
				load(pwdSoFar.startingSector[pwdSoFar.sectorIndex]);

				asm mov al, activeSpace
				asm mov ah, 0x05
				asm int 0x10

				asm mov ch, 0x06
				asm mov cl, 0x07
				asm mov ah, 0x01
				asm int 0x10

				processLoaded = 1;
			}
			else
			{
				puts("Invalid command/operable program. Type 'help' for a list of commands");
				putchar(0x0d);
			}
		}
	}
}

void puts(char *str)
{
	char c;

	char row, col;

	if(activeSpace == 0x00 && processLoaded == 1)
	{
		getCursorPos(&row, &col, 0x00);
		drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(0x00)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(0x00)][1]), spaceColor[BCD_TO_BIN(0x00)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(0x00)][0])), row, 0, 24, 79);
		processLoaded = 0;
	}

	while((c = *str++) != '\0')
	{
		asm mov al, c
		asm mov ah, 0x0e
		asm int 0x10
	}
}

void putchar(unsigned char c)
{
	if(c == 0x0d)
	{
		if(spaceLines[activeSpace] == 24)
		{
//			scrollUp(); //my own scroll up function

			drawWindow(0x01, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), 0, 0, 24, 79);

			setCursorPos(24, 0, activeSpace);
		}
		else
		{
			if(commandBeingTyped)
			{
				cmdPointer = cmdIndex;
				setCursorPos(cmdBeginsAtRow + (shellPromptLength + cmdPointer) / 80, (shellPromptLength + cmdPointer) % 80, activeSpace);
			}

			asm mov ah, 0x0e
			asm mov al, 0x0d //carriage return
			asm int 0x10

			asm mov ah, 0x0e
			asm mov al, 0x0a //next line
			asm int 0x10

			spaceLines[activeSpace]++;
		}
	}
	else
	{
		if(commandBeingTyped)
		{
			if((cmdBeginsAtRow + (cmdIndex - 1 + shellPromptLength)/80 == 24) && ((cmdIndex + shellPromptLength) % 80 == 0))
			{
				cmdBeginsAtRow = cmdBeginsAtRow - 1;

				scrollUp();

				if(cmdPointer == cmdIndex)
					setCursorPos(23, 79, activeSpace);
				else
					setCursorPos(cmdBeginsAtRow + (shellPromptLength + cmdPointer) / 80, (shellPromptLength + cmdPointer) % 80, activeSpace);

				spaceLines[activeSpace]--;
			}

			if(cmdPointer < cmdIndex && insertState == 0)
			{
				unsigned char iter, row, col;

				getCursorPos(&row, &col, activeSpace);

				drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, cmdBeginsAtRow + (cmdIndex + shellPromptLength)/80, 79);

				setCursorPos(cmdBeginsAtRow, 0, activeSpace);

				puts(shellPrompt);

				for(iter = 0; iter < cmdIndex; iter++)
				{
					char c;
					c = commandLine[iter];

					asm mov ah, 0x0e
					asm mov al, c
					asm int 0x10
				}

				if(col == 79)
					setCursorPos(row + 1, 0, activeSpace);
				else
					setCursorPos(row, col + 1, activeSpace);

				cmdPointer++;
				return;
			}
		}
		asm mov ah, 0x0e
		asm mov al, c
		asm int 0x10
	}
}

void setVideoMode(unsigned char c)
{
	asm mov al, c
	asm mov ah, 0x0
	asm int 0x10
}

void readCommandLine()
{
	unsigned char c;

	cmdIndex = 0;

	do
	{
		c = readKey();

		if(scanCode == 0x52)  //insert key
		{
			if(!insertState)
			{
				insertState = 1;
				setCursorSize(0x03, 0x8B);
			}
			else
			{
				insertState = 0;
				setCursorSize(0x0, 0xCE);
			}
		}
		else if(scanCode == 0x50)  //down arrow
		{
			if(upCount > 1)
			{
				char *tempIter = cmdHistory[(cmdHistoryIndex - upCount + 1 + CMD_HISTORY_LENGTH) % CMD_HISTORY_LENGTH];

				if(*tempIter)
				{
					cmdPointer = cmdIndex = 0;

					commandBeingTyped = 1;

					drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, 24, 79);

					setCursorPos(cmdBeginsAtRow, 0, activeSpace);

					puts(shellPrompt);

					while(*tempIter)
					{
						commandLine[cmdIndex] = *tempIter;
						cmdPointer = ++cmdIndex;
						putchar(*tempIter++);
					}

					upCount--;
				}
			}
		}
		else if(scanCode == 0x4B) //left arrow
		{
			if(cmdPointer > 0)
			{
				cmdPointer--;
				setCursorPos(cmdBeginsAtRow + (shellPromptLength + cmdPointer) / 80, (shellPromptLength + cmdPointer) % 80, activeSpace);
			}
		}
		else if(scanCode == 0x48) //up arrow
		{
			if(upCount < 5)
			{
				char *tempIter = cmdHistory[(cmdHistoryIndex - 1 - upCount + CMD_HISTORY_LENGTH) % CMD_HISTORY_LENGTH];

				if(*tempIter)
				{
					cmdPointer = cmdIndex = 0;

					commandBeingTyped = 1;

					drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, 24, 79);

					setCursorPos(cmdBeginsAtRow, 0, activeSpace);

					puts(shellPrompt);

					while(*tempIter)
					{
						commandLine[cmdIndex] = *tempIter;
						cmdPointer = ++cmdIndex;
						putchar(*tempIter++);
					}

					upCount++;
				}
			}
		}
		else if(scanCode == 0x4D) //right arrow
		{
			if(cmdPointer < cmdIndex)
			{
				cmdPointer++;
				setCursorPos(cmdBeginsAtRow + (shellPromptLength + cmdPointer) / 80, (shellPromptLength + cmdPointer) % 80, activeSpace);
			}
		}
		else if(scanCode == 0x01) //Esc key
		{
			drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, cmdBeginsAtRow + (cmdIndex + shellPromptLength)/80, 79);

			setCursorPos(cmdBeginsAtRow, 0, activeSpace);

			commandLine[0] = '\0';

			upCount = 0;
		}
		else if(scanCode == 0x53) //delete key
		{
			if(cmdIndex > 0 && cmdPointer < cmdIndex)
			{
				unsigned int iter;
				unsigned char row, col;

				for(iter = cmdPointer; iter <= cmdIndex; iter++)
				{
					commandLine[iter] = commandLine[iter + 1];
				}

				cmdIndex--;

				getCursorPos(&row, &col, activeSpace);

				drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, cmdBeginsAtRow + (cmdIndex + shellPromptLength)/80, 79);

				setCursorPos(cmdBeginsAtRow, 0, activeSpace);

				puts(shellPrompt);

				for(iter = 0; iter < cmdIndex; iter++)
				{
					char c;
					c = commandLine[iter];

					asm mov ah, 0x0e
					asm mov al, c
					asm int 0x10
				}

				setCursorPos(row, col, activeSpace);
			}
		}
		else if(c == 0x08) //Backspace key
		{
			if(cmdPointer > 0)
			{
				unsigned char row, col;

				unsigned char iter;

				getCursorPos(&row, &col, activeSpace);

				if(col == 0)
				{
					setCursorPos(row - 1, 79, activeSpace);

					asm mov ah, 0x0e
					asm mov al, 32
					asm int 0x10

					setCursorPos(row - 1, 79, activeSpace);
				}
				else
				{
					asm mov ah, 0x0e
					asm mov al, 0x08
					asm int 0x10

					asm mov ah, 0x0e
					asm mov al, 32
					asm int 0x10

					asm mov ah, 0x0e
					asm mov al, 0x08
					asm int 0x10
				}

				if(cmdPointer < cmdIndex)
				{
					for(iter = cmdPointer; iter <= cmdIndex; iter++)
					{
						commandLine[iter - 1] = commandLine[iter];
					}

					getCursorPos(&row, &col, activeSpace);

					drawWindow(0x0, ARR_TO_UCH(spaceColor[BCD_TO_BIN(activeSpace)][1] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][1]), spaceColor[BCD_TO_BIN(activeSpace)][0] - findSubtrahend(spaceColor[BCD_TO_BIN(activeSpace)][0])), cmdBeginsAtRow, 0, cmdBeginsAtRow + (cmdIndex + shellPromptLength)/80, 79);

					setCursorPos(cmdBeginsAtRow, 0, activeSpace);

					puts(shellPrompt);

					for(iter = 0; iter < cmdIndex - 1; iter++)
					{
						char c;
						c = commandLine[iter];

						asm mov ah, 0x0e
						asm mov al, c
						asm int 0x10
					}

					setCursorPos(row, col, activeSpace);
				}

				cmdPointer--;
				cmdIndex--;
			}
		}
		else if(c == 0x09)	//tab key
		{
		}
		else if(c == 0x0d)	//enter key
		{
			unsigned char iter;

			commandLine[cmdIndex] = '\0';

			if(commandLine[0] != '\0' && strcmp(commandLine, cmdHistory[(cmdHistoryIndex + CMD_HISTORY_LENGTH - 1) % CMD_HISTORY_LENGTH]))
			{
				for(iter = 0; iter < cmdIndex; iter++)
					cmdHistory[cmdHistoryIndex][iter] = commandLine[iter];

				cmdHistory[cmdHistoryIndex][iter] = '\0';

				cmdHistoryIndex = (++cmdHistoryIndex) % CMD_HISTORY_LENGTH;
			}

			upCount = 0;
			putchar(0x0d);

			commandBeingTyped = 0;

			break;
		}
		else if(scanCode == 0x47)	//home key
		{
			cmdPointer = 0;
			setCursorPos(cmdBeginsAtRow, shellPromptLength, activeSpace);
		}
		else if(scanCode == 0x4F)	//end key
		{
			cmdPointer = cmdIndex;
			setCursorPos(cmdBeginsAtRow + (shellPromptLength + cmdPointer) / 80, (shellPromptLength + cmdPointer) % 80, activeSpace);
		}
		else
		{
			if(cmdIndex < BUF_LENGTH - 1)
			{
				if(cmdPointer == cmdIndex)
				{
					commandLine[cmdIndex++] = c;
					cmdPointer = cmdIndex;
				}
				else if(insertState)
					commandLine[cmdPointer++] = c;
				else
				{
					int iter;

					for(iter = cmdIndex - 1; iter >= cmdPointer; iter--)
						commandLine[iter + 1] = commandLine[iter];

					commandLine[cmdPointer] = c;
					cmdIndex++;
				}

				putchar(c);
			}
			else if(insertState && cmdPointer < cmdIndex)
			{
				commandLine[cmdPointer++] = c;
				putchar(c);
			}

		}
	}while(c != 0x0d && scanCode != 0x01);

	commandLine[cmdIndex] = '\0';
}

unsigned char readKey()
{
	unsigned char c;

	asm mov ah, 0x0
	asm int 0x16
	asm mov c, al
	asm mov scanCode, ah
//	printUnsignedChar(scanCode);
	printf("%x", c);
	return c;
}

int strcmp(const char *str1, const char *str2)
{
	int diff;

	while(!(diff = *str1 - *str2) && (*str1++ != '\0' && *str2++ != '\0'));

	return diff;
}

unsigned int strlen(const char *str)
{
	unsigned int len = 0;

	while(*str++ != '\0')
		len++;

	return len;
}

void setCursorPos(unsigned char row, unsigned char col, unsigned char page)
{
	asm mov dh, row
	asm mov dl, col
	asm mov bh, page
	asm mov ah, 0x02
	asm int 0x10
}

void drawWindow(unsigned char lines, unsigned char attr, unsigned char row1, unsigned char col1, unsigned char row2, unsigned char col2)
{
		asm mov al, lines
		asm mov ch, row1
		asm mov cl, col1
		asm mov dh, row2
		asm mov dl, col2
		asm mov bh, attr
		asm mov ah, 0x06
		asm int 0x10
}

void getDate()
{
	asm mov ah, 0x04
	asm int 0x1A
	asm mov day, dl
	asm mov month, dh
	asm mov year, cl
	asm mov century, ch
}

void printUnsignedChar(unsigned char ch)	//prints in BCD format
{
	putchar(((ch & 0xF0) >> 4) + 48);
	putchar((ch & 0x0F) + 48);
}

void printUnsignedInt(unsigned int num)		//prints a 4 digit number to console
{
	unsigned int divisor = 1000;

	while(divisor > 0)
	{
		putchar(((unsigned char) (num / divisor)) + 48);
		num %= divisor;
		divisor /= 10;
	}
}

void getTime()
{
	asm mov ah, 0x02
	asm int 0x1A
	asm mov hours, ch
	asm mov minutes, cl
	asm mov seconds, dh
}

void tokenize(char *commandLine, char delim, char commandLineArray[][ARRAY_COLS])
{
	unsigned char character, index = 0;

	unsigned char state = 0;

	char *ptr = commandLine;

	arrayIndex = 0;

	do
	{
		character = *ptr++;

		if(character == delim || character == '\0')
		{
			if(state == 1)
			{
				if(arrayIndex == ARRAY_ROWS) //TOKEN OVERRUN
					break;

				commandLineArray[arrayIndex++][index] = '\0';

				state = 0;
				index = 0;
			}
		}
		else
		{
			if(state == 0)
				state = 1;

			commandLineArray[arrayIndex][index++] = character;
		}
	}while(character != '\0');
}

void clearCommandLineArray()
{
	unsigned char iter;

	for(iter = 0; iter < ARRAY_ROWS; iter++)
		commandLineArray[iter][0] = '\0';
}

unsigned char validateTime(char *time)
{
	unsigned char iter;

	tokenize(time, ':', timeEntities);

	if(arrayIndex != 3)
		return 0;

	for(iter = 0; iter < 3; iter++)
	{
		if(strlen(timeEntities[iter]) > 2 || strlen(timeEntities[iter]) == 0)
			return 0;
		else if(strlen(timeEntities[iter]) < 2)
		{
			timeEntities[iter][2] = timeEntities[iter][1];
			timeEntities[iter][1] = timeEntities[iter][0];
			timeEntities[iter][0] = '0';
		}
	}

	for(iter = 0; iter < 3; iter++)
		if(!((timeEntities[iter][0] >= '0' && timeEntities[iter][0] <= '9') && (timeEntities[iter][1] >= '0' && timeEntities[iter][1] <= '9')))
			return 0;

	hours = ARR_TO_UCH(timeEntities[0][0] - 48, timeEntities[0][1] - 48);

	if(!(BCD_TO_BIN(hours) >= 0 && BCD_TO_BIN(hours) <= 23))
		return 0;

	minutes = ARR_TO_UCH(timeEntities[1][0] - 48, timeEntities[1][1] - 48);

	if(!(BCD_TO_BIN(minutes) >= 0 && BCD_TO_BIN(minutes) <= 59))
		return 0;

	seconds = ARR_TO_UCH(timeEntities[2][0] - 48, timeEntities[2][1] - 48);

	if(!(BCD_TO_BIN(seconds) >= 0 && BCD_TO_BIN(seconds) <= 59))
		return 0;

	return 1;
}

unsigned char validateDate(char *date)
{
	unsigned char iter, m;
	unsigned int y;

	tokenize(date, '-', dateEntities);

	if(arrayIndex != 3)
		return 0;

	//verifying month and day

	for(iter = 0; iter < 2; iter++)
	{
		if(strlen(dateEntities[iter]) > 2 || strlen(dateEntities[iter]) == 0)
			return 0;
		else if(strlen(dateEntities[iter]) < 2)
		{
			dateEntities[iter][2] = dateEntities[iter][1];
			dateEntities[iter][1] = dateEntities[iter][0];
			dateEntities[iter][0] = '0';
		}
	}

	//checking if month and day comprises only of digits
	for(iter = 0; iter < 2; iter++)
		if(!((dateEntities[iter][0] >= '0' && dateEntities[iter][0] <= '9') && (dateEntities[iter][1] >= '0' && dateEntities[iter][1] <= '9')))
			return 0;

	//verifying year
	if(strlen(dateEntities[2]) > 4 || strlen(dateEntities[2]) == 0)
		return 0;
	else if(strlen(dateEntities[2]) < 2)
	{
		dateEntities[iter][2] = dateEntities[iter][1];
		dateEntities[2][1] = dateEntities[2][0];
		dateEntities[2][0] = '0';
	}
	else if(strlen(dateEntities[2]) == 3)
	{
		dateEntities[iter][4] = dateEntities[iter][3];
		dateEntities[iter][3] = dateEntities[iter][2];
		dateEntities[iter][2] = dateEntities[iter][1];
		dateEntities[2][1] = dateEntities[2][0];
		dateEntities[2][0] = '0';
	}
	//verifying if year comprises only of digits
	for(iter = 0; iter < strlen(dateEntities[2]); iter++)
		if(!(dateEntities[2][iter] >= '0' && dateEntities[2][iter] <= '9'))
			return 0;

	getDate();	//to get current century into the variable century in case if user enters just the year

	month = ARR_TO_UCH(dateEntities[0][0] - 48, dateEntities[0][1] - 48);

	if(!(BCD_TO_BIN(month) >= 1 && BCD_TO_BIN(month) <= 12))
		return 0;

	if(strlen(dateEntities[2]) == 2)
		year = ARR_TO_UCH(dateEntities[2][0] - 48, dateEntities[2][1] - 48);
	else
	{
		century = ARR_TO_UCH(dateEntities[2][0] - 48, dateEntities[2][1] - 48);

		year = ARR_TO_UCH(dateEntities[2][2] - 48, dateEntities[2][3] - 48);
	}

	day = ARR_TO_UCH(dateEntities[1][0] - 48, dateEntities[1][1] - 48);

	m = BCD_TO_BIN(month);

	y = BCD_TO_BIN(century) * 100 + BCD_TO_BIN(year);

	if(y == 0 || (y == 1752 && m == 9 && (BCD_TO_BIN(day) >= 3 && BCD_TO_BIN(day) <= 13)))
		return 0;

	if(m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
	{
		if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 31))
			return 0;
	}
	else if(m == 4 || m == 6 || m == 9 || m == 11)
	{
		if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 30))
			return 0;
	}
	else
	{
		if((y % 4 == 0) && (y < 1800))
		{
			if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 29))
				return 0;
		}

		else if((y % 4 == 0))
		{
			if((y % 100 != 0) || ((y % 100 == 0) && (y % 400 == 0)))
			{
				if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 29))
					return 0;
			}
			else
				if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 28))
					return 0;
		}
		else
			if(!(BCD_TO_BIN(day) >= 1 && BCD_TO_BIN(day) <= 28))
				return 0;
	}

	return 1;
}

void setTime()
{
	asm mov ch, hours
	asm mov cl, minutes
	asm mov dh, seconds
	asm mov dl, 0x0
	asm mov ah, 0x03
	asm int 0x1A
}

void setDate()
{
	asm mov dl, day
	asm mov dh, month
	asm mov cl, year
	asm mov ch, century
	asm mov ah, 0x05
	asm int 0x1A
}

unsigned char validateSpaceColor(char *colorCode)
{
	if(strlen(colorCode) > 2)
		return 0;

	if(strlen(colorCode) == 2)
	{
		if(!(((colorCode[0] >= '0' && colorCode[0] <= '9') || (colorCode[0] >= 'a' && colorCode[0] <= 'f') || (colorCode[0] >= 'A' && colorCode[0] <= 'F')) && ((colorCode[1] >= '0' && colorCode[1] <= '9')  ||  (colorCode[1] >= 'a' && colorCode[1] <= 'f') ||  (colorCode[1] >= 'A' && colorCode[1] <= 'F'))))
			return 0;

		if(toUpper(colorCode[0]) != toUpper(colorCode[1]))
		{
			spaceColor[BCD_TO_BIN(activeSpace)][0] = colorCode[0];
			spaceColor[BCD_TO_BIN(activeSpace)][1] = colorCode[1];
		}
	}
	else
		if(!((colorCode[0] >= '0' && colorCode[0] <= '9') || (colorCode[0] >= 'a' && colorCode[0] <= 'f') || (colorCode[0] >= 'A' && colorCode[0] <= 'F')))
			return 0;
		else
			if(toUpper(colorCode[0]) != toUpper(spaceColor[BCD_TO_BIN(activeSpace)][1]))
				spaceColor[BCD_TO_BIN(activeSpace)][0] = colorCode[0];

	return 1;
}

void setSpaceColor(unsigned char wSpace)
{
	if(toUpper(spaceColor[BCD_TO_BIN(activeSpace)][0]) != toUpper(spaceColor[BCD_TO_BIN(activeSpace)][1]))
	{
		unsigned char c = (spaceColor[BCD_TO_BIN(wSpace)][1]-findSubtrahend(spaceColor[BCD_TO_BIN(wSpace)][1])) * 16 + (spaceColor[BCD_TO_BIN(wSpace)][0]-findSubtrahend(spaceColor[BCD_TO_BIN(wSpace)][0]));

		unsigned char iter1, iter2;
		unsigned char rw, cl;

		getCursorPos(&rw, &cl, wSpace);

		for(iter1 = 0; iter1 < 25; iter1++)
		{
			for(iter2 = 0; iter2 < 80; iter2++)
			{
				setCursorPos(iter1, iter2, wSpace);

				asm mov bh, wSpace
				asm mov ah, 0x08
				asm int 0x10

				asm mov bh, wSpace
				asm mov bl, c
				asm mov cx, 1
				asm mov ah, 0x09
				asm int 0x10
			}
		}

		setCursorPos(rw, cl, activeSpace);
	}
}

unsigned char findSubtrahend(char c)
{
	if(c >= '0' && c <= '9')
		return 48;
	else if(c >= 'a' && c <= 'f')
		return 87;

	return 55;
}

unsigned char toUpper(unsigned char c)
{
	return ((c >= 'a' && c <= 'z') ? c - 32 : c);
}

void switchTo(char *pg)
{
	activeSpace = ARR_TO_UCH(pg[0] - findSubtrahend(pg[0]), pg[1] - findSubtrahend(pg[1]));

	asm mov al, activeSpace
	asm mov ah, 0x05
	asm int 0x10
}

void setCursorSize(unsigned char startScanLine, unsigned char endScanLine)
{
	asm mov ch, startScanLine
	asm mov cl, endScanLine
	asm mov ah, 0x01
	asm int 0x10
}

unsigned char inb(unsigned int port)
{
	unsigned char value;

	asm mov dx, port
	asm in al, dx
	asm mov value, al

	return value;
}

void outb(unsigned int port, unsigned char value)
{
	asm mov dx, port
	asm mov al, value
	asm out dx, al
}

void scrollUp()	//discards the first line on the shell
{
	unsigned char row, col;

	videoMemory = (unsigned char far *)(0xB8000000 + 96 * BCD_TO_BIN(activeSpace) + 25 * 80 * 2 * BCD_TO_BIN(activeSpace));

	for(row = 1; row <= 24; row++)
		for(col = 0; col < 80 * 2; col += 2)
			*(videoMemory + (row - 1) * 80 * 2 + col) = *(videoMemory + row * 80 * 2 + col);

	for(col = 0; col < 80 * 2; col += 2)
		*(videoMemory + (row - 1) * 80 * 2 + col) = 0x00; //blanking 24th row
}

unsigned char validateSpace(char *spaceNumber)
{
	if(strlen(spaceNumber) > 1)
		return 0;

	if(!(spaceNumber[0] >= '1' && spaceNumber[0] <= '4'))
		return 0;

	spaceNumber[1] = spaceNumber[0] - 1;
	spaceNumber[0] = '0';

	return 1;
}

unsigned char findWeekDayIndex()
{
	unsigned char index = 0;

	unsigned int yr = BCD_TO_BIN(century) * 100 + BCD_TO_BIN(year);
	unsigned char d = BCD_TO_BIN(day);
	unsigned char m = BCD_TO_BIN(month);
	unsigned int iter;

	monthDays[1] = 28;
	monthDays[8] = 30;

	for(iter = 1; iter < yr; iter++)
	{
		if((iter % 4 == 0) && (iter < 1800))
		{
			if(iter == 1752)
				index = 0;
			index++;
		}

		else
			if((iter % 4 == 0))
				if((iter % 100 != 0) || ((iter % 100 == 0) && (iter % 400 == 0)))
					index++;

		index++;
		index = index % 7;
	}

	if((iter % 4 == 0) && (iter < 1800))
		monthDays[1] = 29;

	else
		if((iter % 4 == 0))
			if((iter % 100 != 0) || ((iter % 100 == 0) && (iter % 400 == 0)))
				monthDays[1] = 29;

	if(yr == 1752)
		monthDays[8] = 19;

	for(iter = 0; iter < m - 1; iter++)
		index = (index + monthDays[iter]) % 7;

	if(yr == 1752)
		if(m - 1 == 8)
			if(d >= 3 && d <= 13)
				return 0xFF; //error
			else if(d >= 14)
				d = d - 11;

	index = (index + d - 1) % 7;

	return index;
}

void getCursorPos(unsigned char *row, unsigned char *col, unsigned char wSpace)
{
	unsigned char r,c;

	asm mov ah, 0x03
	asm mov bh, wSpace

	asm int 0x10

	asm mov r, dh
	asm mov c, dl

	*row = r;
	*col = c;
}

void printEquipmentList()
{
	unsigned int memSize;

	asm int 0x12

	asm mov memSize, ax

	printUnsignedInt(memSize);

	puts(" KB");
}

void writeSector(unsigned char *sect, unsigned char numSectors, unsigned char track, unsigned char sector, unsigned char head, unsigned char drive)
{
	asm mov bx, sect

	asm mov al, numSectors //number of sectors
	asm mov ch, track //track
	asm mov cl, sector //sector
	asm mov dh, head //head
	asm mov dl, drive //drive

	asm mov ah, 0x03
	asm int 0x13
}

void readSector(unsigned char *sect, unsigned char numSectors, unsigned char track, unsigned char sector, unsigned char head, unsigned char drive)
{
	asm mov bx, sect

	asm mov al, numSectors //number of sectors
	asm mov ch, track //track
	asm mov cl, sector //sector
	asm mov dh, head //head
	asm mov dl, drive //drive

	asm mov ah, 0x02
	asm int 0x13
}

void strcpy(char *dest, char *src)
{
	while((*dest++ = *src++) != '\0');
}

void makeFileEntry(unsigned int seek)
{
	unsigned int iter;

	for(iter = 0; iter < 8; iter++)
		if(file.fileName[iter])
			sector[seek + iter] = file.fileName[iter];
		else
			break;

	for(; iter < 8; iter++)
		sector[seek + iter] = 0;

	for(iter = 0; iter < 3; iter++)
		if(file.extension[iter])
			sector[seek + iter + 8] = file.extension[iter];
		else
			break;

	for(; iter < 3; iter++)
		sector[seek + iter + 8] = 0;

	sector[seek + 11] = file.attribute;

	getTime();

	sector[seek + 17] = file.hours;
	sector[seek + 18] = file.minutes;
	sector[seek + 19] = file.seconds;

	getDate();

	sector[seek + 20] = file.day;
	sector[seek + 21] = file.month;
	sector[seek + 22] = file.century;
	sector[seek + 23] = file.year;

	sector[seek + 24] = (unsigned char) file.startingSector >> 8;
	sector[seek + 25] = (unsigned char) file.startingSector;

	sector[seek + 26] = (unsigned char) file.size >> 24;
	sector[seek + 27] = (unsigned char) file.size >> 16;
	sector[seek + 28] = (unsigned char) file.size >> 8;
	sector[seek + 29] = (unsigned char) file.size;
}

int returnFATFreeSlot()
{
	unsigned char iter1;
	unsigned int iter2;

	for(iter1 = 1; iter1 <= 11; iter1++)
	{
		readSector(FAT, 1, 3, iter1, 0, 0);

		for(iter2 = 0; iter2 < 512; iter2 += 2)
			if(FAT[iter2] == 0x00 && FAT[iter2 + 1] == 0x00)
			{
				FAT[iter2] = 0xFF;
				FAT[iter2 + 1] = 0xFF;

				writeSector(FAT, 1, 3, iter1, 0, 0);

				return (iter1 - 1) * 256 + iter2 / 2;
			}
	}

	return -1;
}

void mapSlotToDisk(int slot)
{
	if(slot < 2736)
	{
		disk.head = (((slot / 18) % 2) ? 1 : 0);
		disk.track = slot / 36 + 4;
		disk.sector = slot % 18 + 1;
	}
	else if(slot < 2754)
	{
		disk.head = 1;
		disk.track = 3;
		disk.sector = slot - 2735;
	}
	else
	{
		disk.head = 0;
		disk.track = 0;
		disk.sector = slot - 2753 + 11;
	}
}

void strcat(char *src, char *toAdd)
{
	while(*src++);

	src--;

	while((*src++ = *toAdd++) != '\0');
}

void getFATSector(unsigned int sectorNumber)
{
	FATSector.track = 3;
	FATSector.sector = (sectorNumber / 256) + 1;
	FATSector.head = 0;
}

void fillFileDetails(char *fileName, char *extension, unsigned char attribute, unsigned int startingSector, unsigned long size)
{
	strcpy(file.fileName, fileName);
	strcpy(file.extension, extension);
	file.attribute = attribute;

	getTime();

	file.hours = hours;
	file.minutes = minutes;
	file.seconds = seconds;

	getDate();

	file.day = day;
	file.month = month;
	file.century = century;
	file.year = year;

	file.startingSector = startingSector;
	file.size = size;
}

char mkdir(char *name)
{
	int slotForDirectory;
	unsigned char iter1, iter2;
	unsigned int prevSectNumber, usageCount;
	unsigned int sectorNumber = pwdSoFar.startingSector[pwdSoFar.sectorIndex], FATEntry;
	char fileName[9];

	fileName[8] = '\0';

	do
	{
		mapSlotToDisk(sectorNumber);
		readSector(sector, 1, disk.track, disk.sector, disk.head, 0);

		usageCount = sector[0];
		usageCount = usageCount << 8;
		usageCount = usageCount | sector[1];

		for(iter1 = 0; iter1 < usageCount; iter1++)
		{
			for(iter2 = 0; iter2 < 8; iter2++)
				fileName[iter2] = sector[2 + iter1 * 30 + iter2];

			if(!strcmp(fileName, name))
				return -1;
		}

		getFATSector(sectorNumber);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		prevSectNumber = sectorNumber;

		sectorNumber = FAT[(prevSectNumber % 256) * 2];
		sectorNumber = sectorNumber << 8;
		sectorNumber = sectorNumber | FAT[(prevSectNumber % 256) * 2 + 1];

	}while(sectorNumber != 0xFFFF);

	if((slotForDirectory = returnFATFreeSlot()) == -1)
		return -2;

	if(usageCount == 17)	//implies sector is full
	{
		int freeSlot;

		if((freeSlot = returnFATFreeSlot()) == -1)
		{
			getFATSector(slotForDirectory);
			readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

			FAT[(slotForDirectory % 256) * 2] = 0x00;
			FAT[(slotForDirectory % 256) * 2 + 1] = 0x00;

			writeSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

			return -2;
		}

		getFATSector(prevSectNumber);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		FAT[(prevSectNumber % 256) * 2 + 1] = (unsigned char) freeSlot;
		FAT[(prevSectNumber % 256) * 2] = (unsigned char) (freeSlot >> 8);

		writeSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		getFATSector(freeSlot);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		FAT[(freeSlot % 256) * 2] = 0xFF;
		FAT[(freeSlot % 256) * 2 + 1] = 0xFF;

		writeSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);
		mapSlotToDisk(freeSlot);

		usageCount = 0;
	}

	fillFileDetails(name, "", 0x80, slotForDirectory, 1);
	makeFileEntry(2 + usageCount * 30);

	usageCount = usageCount + 1;

	sector[1] = (unsigned char) usageCount;
	sector[0] = (unsigned char) (usageCount >> 8);

	writeSector(sector, 1, disk.track, disk.sector, disk.head, 0);

	sector[0] = 0;
	sector[1] = 0;

	mapSlotToDisk(slotForDirectory);
	writeSector(sector, 1, disk.track, disk.sector, disk.head, 0);

	return 1;
}

char doesFileExist(char *name)
{
	unsigned int sectorNumber = pwdSoFar.startingSector[pwdSoFar.sectorIndex];
	unsigned char iter1, iter2;
	unsigned int usageCount, prevSectNumber;
	char fileName[9], extension[4];

	fileName[8] = '\0';
	extension[3] = '\0';

	do
	{
		mapSlotToDisk(sectorNumber);
		readSector(sector, 1, disk.track, disk.sector, disk.head, 0);

		usageCount = sector[0];
		usageCount = usageCount << 8;
		usageCount = usageCount | sector[1];

		for(iter1 = 0; iter1 < usageCount; iter1++)
		{
			for(iter2 = 0; iter2 < 8; iter2++)
				fileName[iter2] = sector[2 + iter1 * 30 + iter2];

			if(!strcmp(fileName, name))
			{
				unsigned char attr = sector[2 + iter1 * 30 + 11];

				for(iter2 = 0; iter2 < 3; iter2++)
					extension[iter2] = sector[2 + iter1 * 30 + iter2 + 8];

				if(attr >> 7)
					isDirectory = 1;
				else
					isDirectory = 0;

				if(!strcmp(extension, "exe"))
					isExe = 1;
				else
					isExe = 0;

				pwdSoFar.sectorIndex++;

				strcpy(pwdSoFar.filePath[pwdSoFar.sectorIndex], name);

				pwdSoFar.startingSector[pwdSoFar.sectorIndex] = sector[2 + iter1 * 30 + 24];
				pwdSoFar.startingSector[pwdSoFar.sectorIndex] = pwdSoFar.startingSector[pwdSoFar.sectorIndex] << 8;
				pwdSoFar.startingSector[pwdSoFar.sectorIndex] = pwdSoFar.startingSector[pwdSoFar.sectorIndex] | sector[2 + iter1 * 30 + 25];

				return 1;
			}
		}

		getFATSector(sectorNumber);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		prevSectNumber = sectorNumber;

		sectorNumber = FAT[(prevSectNumber % 256) * 2];
		sectorNumber = sectorNumber << 8;
		sectorNumber = sectorNumber | FAT[(prevSectNumber % 256) * 2 + 1];

	}while(sectorNumber != 0xFFFF);

	return 0;
}

void listDirectory(unsigned int startingSector)
{
	unsigned int sectorNumber = startingSector;
	unsigned char iter1, iter2;
	unsigned int usageCount, prevSectNumber;
	char fileName[9], extension[4];

	fileName[8] = '\0';
	extension[3] = '\0';

	do
	{
		mapSlotToDisk(sectorNumber);
		readSector(sector, 1, disk.track, disk.sector, disk.head, 0);

		usageCount = sector[0];
		usageCount = usageCount << 8;
		usageCount = usageCount | sector[1];

		for(iter1 = 0; iter1 < usageCount; iter1++)
		{
			for(iter2 = 0; iter2 < 8; iter2++)
				fileName[iter2] = sector[2 + iter1 * 30 + iter2];

			printUnsignedChar(sector[2 + iter1 * 30 + 20]);
			putchar('/');
			printUnsignedChar(sector[2 + iter1 * 30 + 21]);
			putchar('/');
			printUnsignedInt(BCD_TO_BIN(sector[2 + iter1 * 30 + 22]) * 100 + BCD_TO_BIN(sector[2 + iter1 * 30 + 23]));

			puts("  ");

			if(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) == 0)
				printUnsignedChar(0x12);
			else if(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) == 22)
				printUnsignedChar(0x10);
			else if(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) == 23)
				printUnsignedChar(0x11);
			else if(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) >= 13)
				printUnsignedChar((unsigned char)(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) - 12));
			else
				printUnsignedChar(sector[2 + iter1 * 30 + 17]);

			putchar(':');
			printUnsignedChar(sector[2 + iter1 * 30 + 18]);
			putchar(':');
			printUnsignedChar(sector[2 + iter1 * 30 + 19]);

			if(BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) >= 0 && BCD_TO_BIN(sector[2 + iter1 * 30 + 17]) <= 11)
				puts(" AM");
			else
				puts(" PM");

			puts("  ");

			if(((unsigned char)(sector[2 + iter1 * 30 + 11] & 0x80)) >> 7)
				puts("<DIR>");
			else
			{
				isDirectory = 0;

				for(iter2 = 0; iter2 < 3; iter2++)
					extension[iter2] = sector[2 + iter1 * 30 + iter2 + 8];

				puts("     ");
			}
			puts("  ");
			puts(fileName);

			if(!isDirectory)
			{
				putchar('.');
				puts(extension);
				isDirectory = 1;
			}

			putchar(0x0d);
		}

		getFATSector(sectorNumber);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		prevSectNumber = sectorNumber;

		sectorNumber = FAT[(prevSectNumber % 256) * 2];
		sectorNumber = sectorNumber << 8;
		sectorNumber = sectorNumber | FAT[(prevSectNumber % 256) * 2 + 1];

	}while(sectorNumber != 0xFFFF);
}

/*char isFloppyInserted()
{
	unsigned char c;

	asm mov dl, 0x00
	asm mov ah, 0x16
	asm int 0x13
	asm mov c, ah

	if(c == 1)
	{
		puts("Removed");
		putchar(0x0d);
		return 0;
	}

	outb(0x70, 0x10);
	c = inb(0x71);

	c = c >> 4;

	c = c & 0xF;

	if(c == 1)
		return 1;
//		puts("360KB 5.25in");
	else if(c == 2)
		return 2;
//		puts("1.2MB 5.25in");
	else if(c == 3)
		return 3;
//		puts("720KB 3.5in");
	else if(c == 4)
		return 4;
//		puts("1.44MB 3.5in");
	else if(c == 5)
		return 5;
//		puts("2.88mb 3.5in");
	else
		return 0;
//		puts("no floppy drive");
}*/

void load(unsigned int startingSector)
{
	unsigned int sectorNumber = startingSector;
	unsigned int off = 0;
	unsigned char temp;
	unsigned int prevSectNumber;

	do
	{
		mapSlotToDisk(sectorNumber);

		asm mov ax, 0x0050
		asm mov es, ax

		asm mov bx, off

		temp = disk.track;
		asm mov ch, temp	//cylinder (0..79)

		temp = disk.sector;
		asm mov cl, temp	//sector number (1..18)

		temp = disk.head;
		asm mov dh, temp	//head number (0..1)

		asm mov dl, 0x00	//drive number (0..3)

		asm mov ah, 0x02  	//indicates read operation
		asm mov al, 1           //number of sectors to be read

		asm int 0x13

		getFATSector(sectorNumber);
		readSector(FAT, 1, FATSector.track, FATSector.sector, FATSector.head, 0);

		prevSectNumber = sectorNumber;

		sectorNumber = FAT[(prevSectNumber % 256) * 2];
		sectorNumber = sectorNumber << 8;
		sectorNumber = sectorNumber | FAT[(prevSectNumber % 256) * 2 + 1];

		off = off + 512;

	}while(sectorNumber != 0xFFFF);

	//jmp 0x0050:0x0000
}

void drawEditorLayout()
{
	char iter, iter1;

	asm mov bh, 0x04
	asm mov bl, 0xf4
	asm mov cx, 80
	asm mov ah, 0x09
	asm int 0x10

	for(iter = 0; iter < 80; iter++)
		putc(' ');

	setCursorPos(0, 1, 0x04);

	putc('F');

	asm mov bh, 0x04  	//color for 'TIC-TAC-TOE'
	asm mov bl, 0xf0
	asm mov cx, 3
	asm mov ah, 0x09
	asm int 0x10

	puts("ile");

	setCursorPos(1, 0, 0x04);
	asm mov bh, 0x04  	//color for 'TIC-TAC-TOE'
	asm mov bl, 0x9f
	asm mov cx, 80 * 23
	asm mov ah, 0x09
	asm int 0x10

	putc(201);

	for(iter = 1; iter < 79; iter++)
		putc(205);

	putc(187);

	for(iter = 2; iter < 23; iter++)
	{
		putc(186);

		for(iter1 = 1; iter1 < 79; iter1++)
			putc(' ');

		putc(186);
	}

	putc(200);

	for(iter = 1; iter < 79; iter++)
		putc(205);

	putc(188);

	asm mov al, ' '
	asm mov bh, 0x04  	//color for 'TIC-TAC-TOE'
	asm mov bl, 0xf0
	asm mov cx, 80
	asm mov ah, 0x09
	asm int 0x10

	setCursorPos(24, 64, 0x04);

	putc(14);
	puts(" Az's Editor ");
	putc(14);

	setCursorPos(2, 1, 0x04);
}

void putc(char c)
{
	asm mov al, c
	asm mov ah, 0x0e
	asm int 0x10
}
