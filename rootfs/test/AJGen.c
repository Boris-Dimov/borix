#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(WIN32_LEAN_AND_MEAN) || defined(_WIN32)
	#include <windows.h>	//windows.h has to be included here
#endif
#define MAX_STRING_LENGTH 255 //arbitrary, chosen because the maximum filename size in most OSes is 255
#define DIR_LIST_LENGTH 20

_Bool ListCurrentDirectory(char filenameList[][MAX_STRING_LENGTH], const char* fileExtension){	//this function will return true if it succeeds in listing the directory, and false otherwise

	#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) //checking if we are on Linux/UNIX/MacOS

		#include <dirent.h>		//including the header which handles directories
		DIR *currentDirectory = opendir(".");	//pointer to the current directory
		struct dirent *entry;					//pointer to each entry in the directory

		if (currentDirectory == NULL) return 0; //if the pointer to the current directory is a null pointer, we return false to indicate there was a problem with listing the directory

		int flIndex = 0;	//index to an entry in filenameList
		while ((entry = readdir(currentDirectory)) != NULL && flIndex < DIR_LIST_LENGTH)	//as long as there are directory entries AND we haven't filled the filename list yet
		{
			if(strstr(entry->d_name, fileExtension) != NULL)	//if the file has the desired extension
			{
				strcpy(filenameList[flIndex],entry->d_name);	//we add its filename to the list of filenames
				flIndex++;										//and increase the index to the list entries with 1
			}
		}

		return 1;	//we return true to indicate that we have read the directory listing successfully.

	#elif defined(WIN32_LEAN_AND_MEAN) || defined(_WIN32) //checking if we are on Windows

		WIN32_FIND_DATAW FindFileData;	//in Visual Studio, the type needs an additional '_' before the name (_WIN32...)
		HANDLE hFind;
		LPCWSTR placeholder = L"*.*";
		hFind = FindFirstFileW(placeholder, &FindFileData);	//don't ask why this works, it works. It just works.

		if (hFind == INVALID_HANDLE_VALUE) return 0; //if the handle is invalid, return false to indicate a failed reading operation

		wchar_t wFileExtension[MAX_STRING_LENGTH], wFilenameListEntry[MAX_STRING_LENGTH];
		size_t checker = mbstowcs(wFileExtension, fileExtension, MAX_STRING_LENGTH);	//converting the file extension to a wide string
		if (checker < 1)	//checking if the conversion went well, and returning error if not
		{
			FindClose(hFind);
			return 0;
		}

		int flIndex = 0;	//index to an entry in filenameList
		while (FindNextFileW(hFind, &FindFileData) && flIndex < DIR_LIST_LENGTH)	//as long as there are directory entries AND we haven't filled the filename list yet
		{
			if(wcsstr(FindFileData.cFileName, wFileExtension) != NULL)	//if the file has the desired extension
			{
				wcscpy(wFilenameListEntry,FindFileData.cFileName);		//we add its filename to the list of filenames
				checker = WideCharToMultiByte(CP_UTF8, 0, wFilenameListEntry, MAX_STRING_LENGTH, filenameList[flIndex], MAX_STRING_LENGTH, NULL, NULL);
				if (checker < 0)	//checking if the conversion went well, and returning error if not
				{
					FindClose(hFind);
					return 0;
				}
				flIndex++;												//and increase the index to the list entries with 1
			}
		}

		FindClose(hFind);	//cleaning up
		return 1;			//we return true to indicate that we have read the directory listing successfully.

	#else 						//if we cannot recognise the system
		filenameList = NULL;	//may be redundant
		return 0;				//we just return false - e.g. unable to list directory
	#endif
}

_Bool CheckForExistingFile(char* filename){ //A boolean function to make it a little easier to check if a file is present
	FILE* filePointer =  fopen(filename, "r");
	if (filePointer != NULL){
		return 1;
	}
	else{
		return 0;
	}
}

_Bool YesFromKeyboard(void){
	char YorN[2]; //to hold the keyboard input, should be a single char, but the scanf/getchar gets problematic because of the \n character from stdin if only a single char is held
	scanf("%1s", YorN); //grabbing input
	if (YorN[0] == 'y' || YorN[0] == 'Y') return 1;	//return true if the input character is 'y' or 'Y'
	return 0;	//otherwise return false
}

int main(void){
	char albumTitle[MAX_STRING_LENGTH],
		 albumArtist[MAX_STRING_LENGTH],
		 albumYear[5],
		 albumDesc[MAX_STRING_LENGTH],
		 albumCoverFilename[MAX_STRING_LENGTH]; //definitions for the album

/*Setting the console codepages on Windows to UTF-8*/
#if defined(WIN32_LEAN_AND_MEAN) || defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	printf("Album.json Generator v1.0\n\nPlace the executable file in the directory where the audio files are located.\n");
	printf("If you encounter any issues, feel free to report them at the repository: https://github.com/Boris-Dimov/AlbumJSON_Generator\n\n");
	//Just some basic info about the program.

	char FilenameList[DIR_LIST_LENGTH][MAX_STRING_LENGTH];	//the array which will hold the filenames in the directory

	printf("Enter album title: ");
	scanf("%s", albumTitle);
	printf("Enter album artist: ");
	scanf("%s", albumArtist);
	printf("Enter album year: ");
	scanf("%s", albumYear);
	printf("Enter album description (this will show in-game and should be in human-readable format): ");
	scanf("%s", albumDesc);		//prompting the user to enter the corresponding values

	if(ListCurrentDirectory(FilenameList, ".png"))	//calling the function which lists the directory to look for .png files
	{
		if (FilenameList[0][0] == '\0')		//if there are no entries in the array
		{
			printf("No %s files found.\nQuitting\n", ".png");	//we inform the user that no files have been found
			return 0;											//and quit the program.
		}
		puts("Found the following .png files:\n");
		int a;
		for(a = 0; a<DIR_LIST_LENGTH; a++)				//looping through the list of filenames
		{
			if(FilenameList[a][0] == '\0') break;			//if the current entry is empty, we stop the loop
			printf("  [%i]%s\n", a+1,&FilenameList[a][0]);	//otherwise we print the current entry
		}
		printf("\nWhich file to use as an album cover? [1-%i] ",a);
		fgetc(stdin); //to clear the trailing newline from the last scanf()
		int userIndex = 0;
		while(1){   								//loop for getting a valid file index
			char indexString[10];
			fgets(indexString, 9, stdin);			//the user enters the index
			userIndex = atoi(indexString);			//parsing the string
			if(!(userIndex > 0 && userIndex <= a))	//we check the validity of the index
			{
				printf("Invalid index. Try again: ");
				continue;
			}
			printf("Using file '%s'.\n", &FilenameList[userIndex-1][0]);
			strcpy(albumCoverFilename, &FilenameList[userIndex-1][0]);	//assign the chosen filename to the variable
			break;
		}
	}
	else	//if automatic reading fails, we revert to manual
	{
		puts("Unable to automatically list directory. Reverting to manual entry.");
		printf("Enter the filename of the file you wish to use as an album cover (omit .png extension): ");
		while(1)   												//loop for getting a valid filename
		{
			scanf("%250s", albumCoverFilename);    				//the user enters the filename
			strcat(albumCoverFilename, ".png");					//we add the .png extension
			if(!CheckForExistingFile(albumCoverFilename))		//we check if the filename is actually valid and points to a file
			{
				printf("Could not find file: '%s'. Try again: ", albumCoverFilename);
				continue;   									//if it doesn't, we prompt the user to enter a new filename and continue the loop.
			}
			break;
		}
	}

	_Bool 	useDefaultArtist,
			useFilenameAsTitle,
			setAllVolTo100,
			setAllBPMTo0,
			allInCampaign,
			allInChallenges,
			allInSkirmish,
			allInMultiplayer;	//see comment below

	//Set up booleans which indicate whether the default values should be used:
	printf("Use the album artist in all songs? [y/n] ");
	useDefaultArtist = YesFromKeyboard();
	printf("Use the filename as title in all songs? [y/n] ");
	useFilenameAsTitle = YesFromKeyboard();
	printf("Set the volume of all songs to 100? [y/n] ");
	setAllVolTo100 = YesFromKeyboard();
	printf("Set the BPM of all songs to 0? [y/n] ");
	setAllBPMTo0 = YesFromKeyboard();
	printf("Use all songs in campaign? [y/n] ");
	allInCampaign = YesFromKeyboard();
	printf("Use all songs in challenges? [y/n] ");
	allInChallenges = YesFromKeyboard();
	printf("Use all songs in skirmish? [y/n] ");
	allInSkirmish = YesFromKeyboard();
	printf("Use all songs in multiplayer? [y/n] ");
	allInMultiplayer = YesFromKeyboard();

	if(CheckForExistingFile("album.json")){
		printf("An 'album.json' file already exists in this directory. Overwrite? [y/n] ");
		if(!YesFromKeyboard()){
			printf("Operation halted by user.\nQuitting\n");
			return 0;
		}
	}

	FILE* albumJSON = fopen("album.json", "w");
	if(albumJSON != NULL){  //if the file pointer actually points to a file

		//writing out album data:
		fputs("{\n", albumJSON);
		fprintf(albumJSON, "\"title\": \"%s\",\n", albumTitle);
		fprintf(albumJSON, "\"author\": \"%s\",\n", albumArtist);
		fprintf(albumJSON, "\"date\": \"%s\",\n", albumYear);
		fprintf(albumJSON, "\"description\": \"%s\",\n", albumDesc);
		fprintf(albumJSON, "\"album_cover_filename\": \"%s\",\n", albumCoverFilename);
		fprintf(albumJSON, "\"tracks\": [\n");  //all of these are the "header", where the album info resides

		char trackFilename[MAX_STRING_LENGTH],
			 trackTitle[MAX_STRING_LENGTH],
			 trackArtist[MAX_STRING_LENGTH];	//string definitions for the tracks

		int trackVolume = 100, trackBPM;    	//int definitions for the tracks
		int indexOfCurrentTrack = 0;
		_Bool useAutoTrackFinder = 0;

		memset(FilenameList, 0, sizeof FilenameList);	//clear the filename list
		if(ListCurrentDirectory(FilenameList, ".ogg"))	//calling the function which lists the directory to look for .ogg files
		{
			if (FilenameList[0][0] == '\0')		//if there are no entries in the array
			{
				printf("No %s files found.\nQuitting\n", ".ogg");	//we inform the user that no files have been found
				return 0;											//and quit the program.
			}
			puts("Found the following .ogg files:\n");
			int a;
			for(a = 0; a<DIR_LIST_LENGTH; a++)				//looping through the list of filenames
			{
				if(FilenameList[a][0] == '\0') break;			//if the current entry is empty, we stop the loop
				printf("  [%i]%s\n", a+1,&FilenameList[a][0]);	//otherwise we print the current entry
			}
			printf("\nWould you like to automatically add all files to 'album.json'? [y/n] ");
			useAutoTrackFinder = YesFromKeyboard();	//if the user wishes, they can specify that they want all files in the list to be added automatically
		}
		else	//if automatic reading fails, we revert to manual
		{
			puts("Unable to automatically list directory. Reverting to manual entry.");
			useAutoTrackFinder = 0;
		}


		while(1){   //loop which writes info about each individual track

			if(useAutoTrackFinder)	//if automatic addition of files on the list is selected
			{
				memcpy(trackFilename, &FilenameList[indexOfCurrentTrack][0], MAX_STRING_LENGTH);
				indexOfCurrentTrack++;	//advance to the next track on the list
				printf("Now adding track '%s'\n", trackFilename);
			}
			else{
				printf("Enter track filename (omit .ogg extension): ");
				scanf("%250s", trackFilename);					//prompting for track filename
				strcat(trackFilename, ".ogg");					//adding .ogg extension
				if(!CheckForExistingFile(trackFilename)){   	//again, checking if an actual file with that name exists
					printf("Could not find file: '%s'. Try again.\n", trackFilename);
					continue;   //if not, we stop the loop here and continue to the next iteration
				}
			}

			if(useFilenameAsTitle){
				memcpy(trackTitle, trackFilename, strcspn(trackFilename, "."));
			}
			else
			{
				printf("Enter track title: ");
				scanf("%254s", trackTitle);
			}

			if(useDefaultArtist){
				memcpy(trackArtist, albumArtist, MAX_STRING_LENGTH);
			}
			else{
				printf("Enter track artist: ");
				scanf("%254s", trackArtist);
			}

			char trackModes[100] = "["; 	//this will be the text after "default_music_modes". This definition needs to be here because it's the only one that doesn't get cleared automatically after each iteration and so needs to be created blank every time
			int stringIsFull = 0; 			//to determine whether or not there's another value in the square bracket string trackModes

			if(allInCampaign){
				printf("Including track in campaign.\n");   //
				strcat(trackModes, "\"campaign\"");         //
				stringIsFull = 1; 							//if the default bool is set to true, we automatically include the track in campaign.
			}
			else{
				printf("Should the track be played in campaign? [y/n] ");
				if (YesFromKeyboard()){                                     //
					printf("Including track in campaign.\n");               //
					strcat(trackModes, "\"campaign\"");                     //
					stringIsFull = 1;                                       //we prompt the user if he wants to add the track in campaign, and if yes, we add the text "campaign" to trackModes and indicate that there's already something there by setting stringIsFull to 1.
				}
				else{
					printf("Excluding track from campaign.\n");
				}
			}

			if(allInChallenges){	//automatically include track in challenges
				printf("Including track in challenges.\n");
				if (stringIsFull == 1) strcat(trackModes,  ", ");
				strcat(trackModes,  "\"challenge\"");
				stringIsFull = 1;
			}
			else{
				printf("Should the track be played in challenges? [y/n] ");
				if (YesFromKeyboard()){
					printf("Including track in challenges.\n");
					if (stringIsFull == 1) strcat(trackModes,  ", ");   //we check if there's an element in trackModes, and if yes, we add a ", " to the array
					strcat(trackModes,  "\"challenge\"");   			//add the "challenge" indicator
					stringIsFull = 1;   								//and set stringIsFull to 1, if this happens to be the first element.
				}
				else{
					printf("Excluding track from challenges.\n");
				}
			}

			if(allInSkirmish){		//automatically include track in skirmish
				printf("Including track in skirmish.\n");
				if (stringIsFull == 1) strcat(trackModes,  ", ");
				strcat(trackModes,  "\"skirmish\"");
				stringIsFull = 1;
			}
			else{
				printf("Should the track be played in skirmish? [y/n] ");
				if (YesFromKeyboard()){
					printf("Including track in skirmish.\n");
					if (stringIsFull == 1) strcat(trackModes,  ", ");   //same as previous inner if-else
					strcat(trackModes,  "\"skirmish\"");
					stringIsFull = 1;
				}
				else{
					printf("Excluding track from skirmish.\n");
				}
			}

			if(allInMultiplayer){	//automatically include track in multiplayer
				printf("Including track in multiplayer.\n");
				if (stringIsFull == 1) strcat(trackModes,  ", ");
				strcat(trackModes,  "\"multiplayer\"");
			}
			else{
				printf("Should the track be played in multiplayer? [y/n] ");
				if (YesFromKeyboard()){
					printf("Including track in multiplayer.\n");
					if (stringIsFull == 1) strcat(trackModes,  ", ");   //same as previous inner if-else
					strcat(trackModes,  "\"multiplayer\"");
				}
				else{
					printf("Excluding track from multiplayer.\n");
				}
			}

			strcat(trackModes,  "]");   //the closing square bracket on the default_music_modes line

			if(setAllVolTo100){		//if the bool is true, we automatically set the track volume to 100
				printf("Set track volume to 100.\n");
				trackVolume = 100;
			}
			else{					//otherwise we prompt the user to enter the desired volume
				printf("Enter base track volume (0-100): ");
				scanf("%i", &trackVolume);
				if (trackVolume > 100 || trackVolume < 0) trackVolume = 100;
			}

			if(setAllBPMTo0){		//since the BPM value is not used at the moment, we can spare the user some time and automatically set it to 0.
				printf("Set track BPM to 0.\n");
				trackBPM = 100;
			}
			else{
				printf("Enter track BPM (if unknown, type 0): ");
				scanf("%i", &trackBPM);     //prompting for volume and BPM
			}

			fputs("{\n", albumJSON);    //start writing the track segment
			fprintf(albumJSON, "\"filename\": \"%s\",\n", trackFilename);
			fprintf(albumJSON, "\"title\": \"%s\",\n", trackTitle);
			fprintf(albumJSON, "\"author\": \"%s\",\n", trackArtist);
			fprintf(albumJSON, "\"default_music_modes\": %s,\n", trackModes);
			fprintf(albumJSON, "\"base_volume\": %i,\n", trackVolume);
			fprintf(albumJSON, "\"bpm\": %i\n", trackBPM);  //fprintf all properties to the file

			if(useAutoTrackFinder)
			{
				if(FilenameList[indexOfCurrentTrack][0] == '\0')	//if there are no more elements to add
				{
					fputs("}\n", albumJSON);	//add the last closing bracket
					break;						//break the loop
				}
				fputs("},\n", albumJSON);		//add a closing bracket and a comma before continuing the loop
			}
			else
			{
				printf("Added 1 track. Continue? [y/n] ");
				if (YesFromKeyboard()){			//prompt the user if they want to add another file
					printf("\n");
					fputs("},\n", albumJSON);   //if yes, we add a closing bracket and a comma
				}
				else{
					fputs("}\n", albumJSON);    //if no, we just add a closing bracket, send out a little message and break the loop
					printf("\nSequence interrupted by user.\nFinalizing file.\n");
					break;
				}
			}
		}
		printf("Quitting\n");
		fputs("]\n", albumJSON);    //these 2 lines add the necessary closing brackets to the end of the file
		fputs("}", albumJSON);      //
		fclose(albumJSON);  		//flush the filestream and close the file
	}
	else{
		printf("Couldn't create 'album.json' file. Check if you have writing permissions for the directory.\n");    //in case there was a problem with creating an album.json file, we quit
	}

	return 0;
}
