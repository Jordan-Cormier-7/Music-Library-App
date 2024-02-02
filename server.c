/*********************************************************
Jordan Cormier
Final Project: Concurrent Client Server Music Application
CSCI 3240: Dr. Sainju
server.c file
*********************************************************/

#include "csapp.h"
#include<stdbool.h>
#define MAXUSERNAME 20
#define MAXPASSWORD 20
#define MAXSONG 30
#define MAXARTIST 30
#define MAXGENRE 20
#define MAXLISTNAME 30
#define MAXSONGS 100
#define MAXPLAYLISTS 100
#define MAX_SIZE 1000
#define MAXGENRECOUNT 11

//Mutually exclusive lock to use on critical sections of program
//These locks help eliminate the reader-writer problem of the program
int read_cnt = 0;
sem_t mutex,w; //Locks to prevent race conditions

/*******Data Structure Definitions*******/

//Struct to hold info of a user
struct User{
    char userName[MAXUSERNAME];
    char password[MAXPASSWORD];
};

//Struct to hold info of one song
struct songRecord{
    char songName[MAXSONG];
    char artistName[MAXARTIST];
    char genre[MAXGENRE];
    long long numListens;
    int trackTime;
};

//Struct to hold info of one playlist
struct playList{
    char userName[MAXUSERNAME];
    char playListName[MAXLISTNAME];
    struct songRecord songs[MAXSONGS];
};

/*******End Data Structure Definitions******/



//Function to check if n greater than 0 after read/write
void ErrorCheck(size_t n){
    if(n < 0){
        perror("Error!");
        exit(0);
    }
    return;
}

//Simple function to add an employee to the records.csv file
void AddPlayList(struct playList list,int count){
    FILE *songsFile;
    songsFile = fopen("playLists.csv","a");
    
    if (songsFile == NULL) { //Check to make sure the file exists
        printf("Unable to open the file!\n");
        exit(0);
    }

    //Adding username, playlist name, and list of songs to the playlists file
    fprintf(songsFile,"%c",'\n');
    fprintf(songsFile,"%s,",list.userName);
    fprintf(songsFile,"%s,",list.playListName);
    for(int i=0;i<count;i++){
        if(i == count-1){
            fprintf(songsFile,"%s",list.songs[i].songName);
        }
        else{
            fprintf(songsFile,"%s,",list.songs[i].songName);
        }
    }
    fclose(songsFile);
    return;
}

//Function to search for a song requested by the client
char* searchBySong(struct songRecord songs[],int count,const char* songName){
    //Character array to hold search result
    char* searchResult = malloc(MAX_SIZE*sizeof(char));

    //Variable to indicate if the song has been found
    bool found = false;

    //First clearing the buffer
    bzero(searchResult,MAX_SIZE);

    for(int i=0;i<count;i++){
        //If first and last name matches the given argument, print back to the console the found user
        if(strcmp(songs[i].songName,songName) == 0){
            int minutes = songs[i].trackTime/60; //Number of minutes in the song
            int seconds = songs[i].trackTime%60; //Number of leftover seconds in the song

            //Using sprintf to print the string into the result array
            sprintf(searchResult+strlen(searchResult),"\nSong: %s\nArtist: %s\nGenre: %s\nTrack Time: %d:%02d\nNumber of Listens: %lld\n",songs[i].songName,songs[i].artistName,
            songs[i].genre,minutes,seconds,songs[i].numListens);
            found = true;
        }
    }

    if(!found){
        sprintf(searchResult+strlen(searchResult),"Song not found!!\n");
    }
    return searchResult;
}

//Function to search for an artist requested by the client
char* searchByArtist(struct songRecord songs[],int count,const char* artistName){
    char* searchResult = malloc(MAX_SIZE*sizeof(char));

    //Variable to indicate if the artist has been found
    bool found = false;

    //First clearing the buffer
    bzero(searchResult,MAX_SIZE);

    int artistCount = 1;
    for(int i=0;i<count;i++){
        //If first and last name matches the given argument, print back to the console the found user
        if(strcmp(songs[i].artistName,artistName) == 0){
            int minutes = songs[i].trackTime/60; //Number of minutes in the song
            int seconds = songs[i].trackTime%60; //Number of leftover seconds in the song
            if(artistCount == 1){
                sprintf(searchResult+strlen(searchResult),"Songs by %s:\n",artistName);
            }
            //Using sprintf to print the string into the result array
            sprintf(searchResult+strlen(searchResult),"%s\t Genre: %s\t Track Time: %d:%02d\t Listens: %lld\n" //May need to revise formatting
            ,songs[i].songName,songs[i].genre,minutes,seconds,songs[i].numListens);
            found = true;
            artistCount++;
        }
    }

    if(!found){
        sprintf(searchResult+strlen(searchResult),"Artist not found!!\n");
    }
    return searchResult;
}

//Function to search for a genre requested by the client
char* searchByGenre(struct songRecord songs[],int count,const char* genre){
    char* searchResult = malloc(MAX_SIZE*sizeof(char));

    //Variable to indicate if the genre has been found
    bool found = false;

    //First clearing the buffer
    bzero(searchResult,MAX_SIZE);

    int genreCount = 1;
    for(int i=0;i<count;i++){
        //If first and last name matches the given argument, print back to the console the found user
        if(strcmp(songs[i].genre,genre) == 0){
            int minutes = songs[i].trackTime/60; //Number of minutes in the song
            int seconds = songs[i].trackTime%60; //Number of leftover seconds in the song
            if(genreCount == 1){
                sprintf(searchResult+strlen(searchResult),"Songs with Genre of %s:\n",genre);
            }
            //Using sprintf to print the string into the result array
            sprintf(searchResult+strlen(searchResult),"%s\t Artist: %s\t Track Time: %d:%02d\t Listens: %lld\n" //May need to revise formatting
            ,songs[i].songName,songs[i].artistName,minutes,seconds,songs[i].numListens);
            found = true;
            genreCount++;
        }
    }

    if(!found){
        sprintf(searchResult+strlen(searchResult),"Genre not found!!\n");
    }

    return searchResult;
}


//Main function that gets ready to listen for clients
//Main service function inside main while loop
void serverFunction(int connfd){
    //Main buffer that will be used to read and write data to the client
    char buffer[MAXLINE];
    size_t n; //Amount of bytes in the buffer

    //Resetting the buffer
    bzero(buffer,MAXLINE);
    n = read(connfd,buffer,MAXLINE); //Reading message with choice from the client
    ErrorCheck(n);

    printf("server received %ld bytes message\n", n);
    printf("Message from Client: %s\n",buffer);

    char *token;
    token = strtok(buffer,",");

    //First getting the choice
    char choice[2];
    strcpy(choice,token);
    int numChoice = atoi(choice);
    token = strtok(NULL, ",");

    //Continue to serve the client until the client wants to disconnect 
    while(numChoice != 8){
        switch(numChoice){
            case 1:{ //Search for a song
                struct songRecord song;
                struct songRecord songs[MAX_SIZE]; //Array of structs to hold all songs in the file
                strcpy(song.songName,token);

                P(&w); //Activating lock while accessing file
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }
                char line[MAX_SIZE]; //String to hold one line of the file at a time
                int count = 0; //Keeps track of the amount of songs in the file

                while(fgets(line,sizeof(line),songsFile)){
                    sscanf(line, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[count].songName, songs[count].artistName, songs[count].genre,
                    &songs[count].trackTime,&songs[count].numListens);
                    count++;
                }
                fclose(songsFile);
                V(&w); //Releasing lock after file accessed
                char* searchSongResult = searchBySong(songs,count,song.songName);
                
                //Resetting the buffer to get ready to write to client
                bzero(buffer,MAXLINE);

                //Writing back to client the requested song
                n = write(connfd,searchSongResult,strlen(searchSongResult));
                free(searchSongResult);
                ErrorCheck(n);

                break;
                }
            case 2:{ //Search for an artist
                struct songRecord song;
                struct songRecord songs[MAX_SIZE];
                strcpy(song.artistName,token);

                P(&w);
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }
                char line[MAX_SIZE]; //String to hold one line of the file at a time
                int count = 0; //Keeps track of the amount of songs in the file

                while(fgets(line,sizeof(line),songsFile)){
                    sscanf(line, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[count].songName, songs[count].artistName, songs[count].genre,
                    &songs[count].trackTime,&songs[count].numListens);
                    count++;
                }
                fclose(songsFile);
                V(&w);

                //Locking reader mutex
                P(&mutex);
                read_cnt++;
                if(read_cnt == 1)
                {
                    P(&w);
                }
                V(&mutex);

                char* searchArtistResult = searchByArtist(songs,count,song.artistName);

                //Resetting the buffer to get ready to write to client
                bzero(buffer,MAXLINE);

                //Unlocking reader mutex
                P(&mutex);
                read_cnt--;
                if(read_cnt == 0)
                {
                    V(&w);
                }
                V(&mutex);

                //Write back to the client songs by the requested artist
                n = write(connfd,searchArtistResult,strlen(searchArtistResult));
                free(searchArtistResult);
                ErrorCheck(n);

                break;
                }

            case 3:{ //Search for a genre
                struct songRecord song;
                struct songRecord songs[MAX_SIZE];
                strcpy(song.genre,token);

                P(&w); //Locking access to file
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }
                char line[MAX_SIZE]; //String to hold one line of the file at a time
                int count = 0; //Keeps track of the amount of songs in the file

                while(fgets(line,sizeof(line),songsFile)){
                    sscanf(line, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[count].songName, songs[count].artistName, songs[count].genre,
                    &songs[count].trackTime,&songs[count].numListens);
                    count++;
                }
                fclose(songsFile);
                V(&w); //Unlocking access to file

                //Locking reader mutex
                P(&mutex);
                read_cnt++;
                if(read_cnt == 1)
                {
                    P(&w);
                }
                V(&mutex);

                char* searchGenreResult = searchByGenre(songs,count,song.genre);

                //Resetting the buffer to get ready to write to client
                bzero(buffer,MAXLINE);

                //Unlocking reader mutex
                P(&mutex);
                read_cnt--;
                if(read_cnt == 0)
                {
                    V(&w);
                }
                V(&mutex);

                //Write back to the client songs by the requested artist
                n = write(connfd,searchGenreResult,strlen(searchGenreResult));
                free(searchGenreResult);
                ErrorCheck(n);
                break;
                }

            case 4:{ //Create a playlist
                struct songRecord song; //song variable to hold requested song
                struct songRecord songs[MAX_SIZE]; //Array to hold all song records in songs file
                struct playList newPlayList; //Playlist to add songs to and append to playlists file
                strcpy(newPlayList.userName,token);
                token = strtok(NULL,","); //Getting the playlist name
                strcpy(newPlayList.playListName,token);

                P(&w); //Lock access to file
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }
                char line[MAX_SIZE];
                int count = 0;
                //Reading contents of songs file into songs struct array
                while(fgets(line,sizeof(line),songsFile)){
                    sscanf(line, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[count].songName, songs[count].artistName, songs[count].genre,
                    &songs[count].trackTime,&songs[count].numListens);
                    count++;
                }
                fclose(songsFile); //Close file after all song records read into array of song structs
                V(&w); //Unlock access to file

                bzero(buffer,MAXLINE); //Clearing buffer to read in requested songs to add
                n = read(connfd,buffer,MAXLINE); //Reading in a requested song from the client
                printf("Message from Client: %s\n",buffer);
                ErrorCheck(n);
                char tmp_song[MAXSONG]; //Temp song string
                strcpy(tmp_song,buffer); //Copying sent song to the temporary song string


                int addCount = 0; //Variable to keep track of how many song records are in the new playlist
                const char* Done = "Done"; //Done message 
                //While string not "Done" search for song and send back response
                while(strcmp(Done,tmp_song) != 0){
                    strcpy(song.songName,tmp_song);

                    //Searching for requested song to add to client playlist
                    bool found = false;
                    //char* searchResult = malloc(100*sizeof(char));
                    for(int i=0;i<count;i++){
                        if(strcmp(songs[i].songName,song.songName) == 0){ //If the requested song is in the file, add to the playlist
                            found = true;
                            newPlayList.songs[addCount] = songs[i]; //Adding matched song record to the playlist
                            addCount++;
                            break;
                        }
                    }
                    
                    //Clear buffer before writing back to client
                    bzero(buffer,MAXLINE);
                
                    if(found){
                        //sprintf(searchResult+strlen(searchResult),"Song added to playlist\n");
                        const char* addMessage = "Song added to playlist!\n";
                        n = write(connfd,addMessage,strlen(addMessage));
                    }
                    else{
                        //sprintf(searchResult+strlen(searchResult),"Requested song not found!\n");
                        const char* failMessage = "Requested song not found!\n";
                        n = write(connfd,failMessage,strlen(failMessage));
                    }
                
                    ErrorCheck(n);

                    //Recurring statements 
                    bzero(buffer,MAXLINE); //Clearing buffer to read in requested songs to add
                    n = read(connfd,buffer,MAXLINE); //Reading in a requested song from the client
                    printf("Message from Client: %s\n",buffer);
                    ErrorCheck(n);
                    strcpy(tmp_song,buffer); //Copying sent song to the temporary song string

                }

                //Case if the client is done adding songs to their playlist
                if(strcmp(Done,tmp_song) == 0){
                    //Clear buffer before writing back to client
                    bzero(buffer,MAXLINE);

                    const char* successMessage = "Playlist Created Successfully!\n";
                    n = write(connfd,successMessage,strlen(successMessage));
                    ErrorCheck(n);
                    
                    //Write to playLists file the creator, playlist name,
                    //and list of songs before exiting
                    P(&w); //Locking access to PL file when adding a new PL
                    AddPlayList(newPlayList,addCount);
                    V(&w); //Releasing lock when PL added
                }
                break;
                }

            case 5:{ //Add song to a playlist
                struct playList PlayList; //playList struct to hold owner name and PL name
                char song[MAXSONG]; //song string to hold requested song to add
                strcpy(PlayList.userName,token); //Getting username of owner of PL

                token = strtok(NULL,",");
                strcpy(PlayList.playListName,token); //Getting name of the playlist 

                token = strtok(NULL,",");
                strcpy(song,token); //Getting name of the requested song


                /*****Checking if the requested playlist exists*****/
                //Checking if the playlist and requested song exists
                bool playListFound = false;
                bool songFound = false;
                struct playList PlayLists[MAXPLAYLISTS]; //Array of playList structs to hold each PL in file

                P(&w); //Lock access to file
                FILE *playListsFile = fopen("playLists.csv","r");
                if(playListsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }
                

                char line[MAX_SIZE] = "";
                int playListCount = 0; //The amount of playlists in the PlayLists array
                //Reading contents of songs file into songs struct array
                while(fgets(line,sizeof(line),playListsFile)){
                    
                    char *line_token = strtok(line,","); //Must start a new token for each new line in the file
            
                    strcpy(PlayLists[playListCount].userName,line_token); //Getting owner name of PL
                    line_token = strtok(NULL,",");
                    
                    strcpy(PlayLists[playListCount].playListName,line_token); //Getting name of PL

                    int songCount = 0;
                    line_token = strtok(NULL,",");
                    while(line_token != NULL){
                        strcpy(PlayLists[playListCount].songs[songCount].songName,line_token);
                        line_token = strtok(NULL,",");
                        songCount++;
                    }
                    playListCount++;
                }
                fclose(playListsFile); //Close file after all song records read into array of song structs
                V(&w); //Unlock access to file

                //Check if the client's PL name and requested song are in the PL file
                int targetLine; //Line number to edit in the PL file
                for(targetLine=0;targetLine<playListCount;targetLine++){
                    //If match for both userName and playListName, then playlist found
                    if(strcmp(PlayLists[targetLine].userName,PlayList.userName) == 0 && strcmp(PlayLists[targetLine].playListName,PlayList.playListName) == 0){
                        playListFound = true;
                        break;
                    }
                }
                /*****End check if the playlist is in the playlists file*****/



                /*****Checking if the requested song to add exists*****/

                P(&w); //Lock access to file
                //Opening songs file
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }

                struct songRecord songs[MAXSONGS]; //Array of song structs to hold each record in the file
                char songLine[MAX_SIZE]; //String to hold one line of the songs file
                int numSongs = 0; //Number of songs in the file

                //Reading contents of songs file into songs struct array
                while(fgets(songLine,sizeof(songLine),songsFile)){
                    sscanf(songLine, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[numSongs].songName, songs[numSongs].artistName, songs[numSongs].genre,
                    &songs[numSongs].trackTime,&songs[numSongs].numListens);
                    numSongs++;
                }
                fclose(songsFile); //Close file after all song records read into array of song structs
                V(&w); //Unlock access to file

                //Finding requested song in array of song structs
                for(int i=0;i<numSongs;i++){
                    if(strcmp(song,songs[i].songName) == 0){ //songFound true if requested song matches song in file
                        songFound = true;
                        break;
                    }
                }
                /*****End check for song in the songs.csv file*****/


                /*****Adding song to the select playlist if song and playlist exist*****/
                if(playListFound && songFound){
                    P(&w);
                    FILE *playListsFile = fopen("playLists.csv", "r");
                    FILE *tempFile = fopen("temp.txt", "w");

                    if (playListsFile == NULL || tempFile == NULL) {
                        perror("Error opening files");
                        exit(EXIT_FAILURE);
                    }

                    char line_buffer[MAXLINE];
                    int currentLine = 0;

                    //Copy contents up to the specified line
                    while (fgets(line_buffer, sizeof(line_buffer), playListsFile) != NULL) {
                        if (currentLine == targetLine) { 
                            //Append new data to the line
                            line_buffer[strcspn(line_buffer, "\n")] = 0;
                            //fprintf(tempFile, "%s,%s\n", line_buffer,song);
                            if(targetLine+1 == playListCount){ //Don't write newline if target line not last line
                                fprintf(tempFile,"%s,%s",line_buffer,song);
                            }
                            else{ //Write newline if target line is the last line 
                                fprintf(tempFile,"%s,%s\n",line_buffer,song);
                            }
                            break;  // Stop copying after appending to the specified line
                        }
                        
                        else {
                            //Copy rest of the original content as is
                            fputs(line_buffer, tempFile);
                        }
                        currentLine++;
                    }

                    //Copy the remaining contents
                    while (fgets(buffer, sizeof(buffer), playListsFile) != NULL) {
                        fputs(buffer, tempFile);
                    }

                    //Close both files
                    fclose(playListsFile);
                    fclose(tempFile);
                    
                    //Rename the temporary file to the original file
                    remove("playLists.csv");
                    rename("temp.txt","playLists.csv");

                    V(&w); //Lock after the file removal to prevent data loss from other threads accessing it
                    /*****End append song to select playlist*****/
                    

                    //Sending back success message if the song was added to the PL
                    bzero(buffer,MAXLINE);
                    const char* successMessage = "Song successfully added to playlist!\n";
                    strcat(buffer,successMessage);
                    n = write(connfd,buffer,strlen(buffer)); //Write back a success message to the client if song added
                    ErrorCheck(n);
                }

                else if(playListFound && !songFound){ //Case if the playlist is found, but song not found
                    bzero(buffer,MAXLINE);
                    const char* failMessage = "Playlist found, but song not found!\n";
                    strcat(buffer,failMessage);
                    n = write(connfd,buffer,strlen(buffer));
                    ErrorCheck(n);
                }

                else if(!playListFound && songFound){ //Case if song found and playlist not found
                    bzero(buffer,MAXLINE);
                    const char* failMessage = "Song found, but playlist not found!\n";
                    strcat(buffer,failMessage);
                    n = write(connfd,buffer,strlen(buffer));
                    ErrorCheck(n);
                }

                else{ //Case if playlist not found and song not found
                    bzero(buffer,MAXLINE);
                    const char* failMessage = "Playlist not found and song not found!\n";
                    strcat(buffer,failMessage);
                    n = write(connfd,buffer,strlen(buffer));
                    ErrorCheck(n);
                }

                break;
                }

            case 6:{ //Display playlist
                char userName[MAXUSERNAME];
                char playListName[MAXLISTNAME];

                strcpy(userName,token);

                token = strtok(NULL,",");
                strcpy(playListName,token);

                P(&w); //Locking file
                //Open PL file to read all playlists into PL struct
                FILE *playListsFile = fopen("playLists.csv","r");

                if(playListsFile == NULL){
                    perror("Error opening file!\n");
                    exit(0);
                }

                struct playList PlayLists[MAXPLAYLISTS]; //Array of PL structs to hold each record in the file
                char line[MAX_SIZE] = ""; //String to hold one line of the PL file
                int playListCount = 0; //The amount of playlists in the PlayLists array

                //Reading contents of songs file into songs struct array
                while(fgets(line,sizeof(line),playListsFile)){
                    char *line_token = strtok(line,","); //Must start a new token for each new line in the file
            
                    strcpy(PlayLists[playListCount].userName,line_token); //Getting owner name of PL
                    line_token = strtok(NULL,",");
                    
                    strcpy(PlayLists[playListCount].playListName,line_token); //Getting name of PL

                    int songCount = 0; //Number of songs in each PL
                    line_token = strtok(NULL,",");
                    while(line_token != NULL){
                        strcpy(PlayLists[playListCount].songs[songCount].songName,line_token);
                        line_token = strtok(NULL,",");
                        songCount++;
                    }
                    playListCount++;
                }
                fclose(playListsFile); //Close file after all song records read into array of song structs
                V(&w); //Unlocking file 

                bool playListFound = false;
                //Finding requested song in array of song structs
                int target;
                for(target=0;target<playListCount;target++){
                    //PL found if match between username and PL name
                    if(strcmp(userName,PlayLists[target].userName) == 0 && strcmp(playListName,PlayLists[target].playListName) == 0){
                        playListFound = true;
                        break;
                    }
                }

                bzero(buffer,MAXLINE);
                //Case if the requested PL found
                if(playListFound){
                    printf("Playlist Owner: %s\n",PlayLists[target].userName);
                    printf("Playlist Name: %s\n",PlayLists[target].playListName);

                    //Appending owner of PL and name of PL
                    strcat(buffer,PlayLists[target].userName);
                    strcat(buffer,",");
                    strcat(buffer,PlayLists[target].playListName);
                    strcat(buffer,",");

                    for(int i=0;i<MAXSONGS;i++){
                        //Break out of loop if no more songs in the list of songs
                        if(strcmp(PlayLists[target].songs[i].songName,"") == 0){
                            break; 
                        }
                        printf("%s\n",PlayLists[target].songs[i].songName);
                        strcat(buffer,PlayLists[target].songs[i].songName); //Appending song to buffer
                        strcat(buffer,",");    
                    }

                    n = write(connfd,buffer,strlen(buffer)); //Write to the client the found PL
                    ErrorCheck(n);
                }

                else{
                    const char* failMessage = "Username or playlist name not found!\n";
                    strcat(buffer,failMessage);
                    n = write(connfd,buffer,strlen(buffer));
                }
                break;
                }

            case 7:{ //Display data insights
                //Read in all songs from songs.csv
                //Opening songs file
                P(&w); //Locking file
                FILE *songsFile = fopen("songs.csv","r");
                if(songsFile == NULL){
                    printf("Error on opening file!\n");
                    exit(0);
                }

                struct songRecord songs[MAXSONGS]; //Array of song structs to hold each record in the file
                char songLine[MAX_SIZE]; //String to hold one line of the songs file
                int numSongs = 0; //Number of songs in the file

                //Reading contents of songs file into songs struct array
                while(fgets(songLine,sizeof(songLine),songsFile)){
                    sscanf(songLine, "%[^,],%[^,],%[^,],%d,%lld",
                    songs[numSongs].songName, songs[numSongs].artistName, songs[numSongs].genre,
                    &songs[numSongs].trackTime,&songs[numSongs].numListens);
                    numSongs++;
                }
                fclose(songsFile); //Close file after all song records read into array of song structs
                V(&w); //Unlocking file


                //Algorithm to get top 10 most popular songs (songs with most streams)
                const int popSize = 10;
                struct songRecord popularSongs[10]; //Declare array of top 10 most popular songs
                //Initialize the array of element's numListens to 0
                for (int i = 0; i < popSize; i++) {
                    popularSongs[i].numListens = 0;
                }

                //Finding top 10 most popular songs in songs struct array
                for (int i = 0; i < numSongs; i++) {
                    for (int j = 0; j < popSize; j++) {
                    if (songs[i].numListens > popularSongs[j].numListens) {
                        struct songRecord temp = popularSongs[j];
                        popularSongs[j] = songs[i];
                        songs[i] = temp;
                        }
                    }
                }

                bzero(buffer,MAXLINE);
                //Append to buffer the 10 most popular songs
                for (int i=0;i<popSize;i++) {
                    //printf("%lld\n",popularSongs[i].numListens);
                    sprintf(buffer+strlen(buffer),"(%d) Song: %s\t Artist: %s\t Number of Listens: %lld\n",i+1,popularSongs[i].songName,
                    popularSongs[i].artistName,popularSongs[i].numListens);
                }

                n = write(connfd,buffer,strlen(buffer));
                ErrorCheck(n);
                break;
                }
                
            default:
                break;
        }

        /******************************************************
        Recurring/Update Statements Before Reading Next Choice
        ******************************************************/

        //Resetting the buffer
        bzero(buffer,MAXLINE); 
    
        n = read(connfd, buffer, MAXLINE);
        ErrorCheck(n);
    
        printf("server received %ld bytes message\n", n);
        printf("Message from Client: %s\n",buffer);
    
        token = strtok(buffer,",");

        //Resetting the choice
        choice[0] = '\0';
        choice[1] = '\0';
        strcpy(choice,token);
        numChoice = atoi(choice);
        token = strtok(NULL, ",");

    }

    if(numChoice == 8){
        char closeMessage[MAXLINE] = "Thank you for using Algorhythm! Happy Listening!\n";
        n = write(connfd,closeMessage,strlen(closeMessage));
        ErrorCheck(n);
    }
    return; 
}

//Thread routine definition
void *thread(void *vargp){
    int connfd = *((int*)vargp);
    pthread_detach(pthread_self()); //Keeping thread independent of other threads
    free(vargp);
    serverFunction(connfd); //Server function that interacts with the client
    Close(connfd); //Closing connecion after thread destroyed
    return NULL;
}


//Main function that gets ready to listen for clients
//Main service function inside main while loop
int main(int argc, char *argv[])
{
    /***************************************************************
    Connect to Client Setup
    ***************************************************************/
    //Initializing locks
   sem_init(&mutex,0,1);
   sem_init(&w,0,1);
   int listenfd;
   int *connfd; //file descriptor to communicate with the client, has to be a pointer
   socklen_t clientlen;
   struct sockaddr_storage clientaddr;  /* Enough space for any address */
     
    char client_hostname[MAXLINE], client_port[MAXLINE];
    
    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }

    listenfd = Open_listenfd(argv[1]);

    //Server runs in the infinite loop. 
    //To stop the server process, it needs to be killed using the Ctrl+C key. 
    while (1) {  
    	clientlen = sizeof(struct sockaddr_storage);
    
        // wait for the connection from the client. 
        connfd = malloc(sizeof(int));
    	*connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, 
                                   MAXLINE,client_port, MAXLINE, 0);
        
        pthread_t tid;
        pthread_create(&tid,NULL,thread,connfd);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);

        /***************************************************************
        End Connect to Client Setup
        ***************************************************************/
    }
    //Destroying locks aftr thread has finished executing
    sem_destroy(&mutex);
    sem_destroy(&w);
    return 0;
}