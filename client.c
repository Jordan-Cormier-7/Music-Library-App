/*********************************************************
Jordan Cormier
Final Project: Concurrent Client Server Music Application
CSCI 3240: Dr. Sainju
client.c file
*********************************************************/

#include "csapp.h"
#include<stdbool.h>
#define MAXUSERNAME 20
#define MAXPASSWORD 20
#define MAXSONG 30
#define MAXARTIST 30
#define MAXGENRE 20
#define MAXLISTNAME 30
#define MAXSONGS 50
#define MAX_SIZE 1000
#define MAXDONE 7

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
    struct songRecord songs[MAXSONGS]; //List of songs (array of structs)
};

//Function to check if n greater than 0 after read/write
void ErrorCheck(size_t n){
    if(n < 0){
        perror("Error!");
        exit(0);
    }
    return;
}

int main(int argc,char *argv[]){
    
    /***************************************************************
    Connect to Server Setup
    ***************************************************************/
    int clientfd;  //file descriptor to communicate with the server
    char *host, *port;
    size_t n; 
    
    //Main buffer to send data back and forth
    char buffer[MAXLINE]; //MAXLINE = 8192 defined in csapp.h
    
    if (argc != 3)
    {
       fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	   exit(0);
    }
    
    host = argv[1]; //First argument which is the host
    port = argv[2]; //Second argument which is the port number

    clientfd = Open_clientfd(host, port);
    
    //Resetting the buffer
    bzero(buffer,MAXLINE);
    /***************************************************************
    End Connect to Server Setup
    ***************************************************************/

    //Opening users file
    FILE *usersFile;
    usersFile = fopen("users.csv","r");
    
    //Terminate program if file not found
    if(usersFile == NULL){
        printf("Unable to open the file.\n");
        exit(0);
    }

    //Read through every user in the users file
    struct User appUser;
    struct User users[MAX_SIZE]; //Array of structs to hold every user 
    char line[MAX_SIZE];
    int count = 0;
    while(fgets(line,sizeof(line),usersFile)){
        sscanf(line,"%[^,],%[^\n]",users[count].userName,users[count].password);
        count++;
    }

    printf("Please enter your Username and Password:\n");
    printf("Username: ");
    fscanf(stdin,"%s",appUser.userName);
    printf("Password: ");
    fscanf(stdin,"%s",appUser.password);
    
    bool found = false; //Variable to check if the appUser is in the users file
    for(int i=0;i<count;i++){
        if(strcmp(users[i].userName,appUser.userName) == 0 && strcmp(users[i].password,appUser.password) == 0){
            found = true;
        }
    }

    //Continuously prompt the user to input a valid username and password no matching records found
    while(!found){
        printf("Incorrect username or password!\n");
        printf("Please enter your Username and Password:\n");
        printf("Username: ");
        fscanf(stdin,"%s",appUser.userName);
        printf("Password: ");
        fscanf(stdin,"%s",appUser.password);

        for(int i=0;i<count;i++){
        if(strcmp(users[i].userName,appUser.userName) == 0 && strcmp(users[i].password,appUser.password) == 0){
            found = true;
            }
        }
    }
    fclose(usersFile);

    //Print welcome message
    printf("Login Success!\n");
    printf("\n");
    printf("    *       *        *****    ******   ******    *      *   *   *   *******   *      *   *        * \n");
    printf("   * *      *        *        *    *   *     *   *      *    * *       *      *      *   *  *   * * \n");
    printf("  *****     *        *  ***   *    *   *  **     ********     *        *      ********   *    *   * \n");
    printf(" *     *    *        *    *   *    *   *  *      *      *     *        *      *      *   *        * \n");
    printf("*       *   ******   ******   ******   *     *   *      *     *        *      *      *   *        * \n\n");

    sleep(2); //Delay to see program backdrop

    //Print main menu options
    printf("Welcome to Algorhythm!\n");
    printf("****MAIN MENU****\n");
    printf("(1) Search Song\n");
    printf("(2) Search Artist\n");
    printf("(3) Search Genre\n");
    printf("(4) Create Playlist\n");
    printf("(5) Add to Playlist\n");
    printf("(6) Display Playlist\n");
    printf("(7) Get Data Insights\n");
    printf("(8) Exit App\n");
    printf("Select an option: ");
    char choice[1];
    fscanf(stdin,"%s",choice);
    while(getchar() != '\n'){ //This line skips the newline character from the choice
                
    }
    printf("\n");
    strncat(buffer,choice,1); //choiceNum appended to buffer

    char newChoice = choice[0]; //Temporary choice variable to avoid frequent memory references

    while(newChoice != '8'){
        char comma = ',';
        strncat(buffer,&comma,1); //comma appended: choiceNum,
        switch(newChoice){
            case '1':{
                struct songRecord song;
                printf("Enter the name of the song: ");
                fgets(song.songName,MAXSONG,stdin);
                song.songName[strlen(song.songName)-1] = '\0'; //Erasing newline character 
                strcat(buffer,song.songName); //Append song name to the buffer

                n = write(clientfd,buffer,strlen(buffer)); //Sending the server the requested song
                ErrorCheck(n);

                bzero(buffer,MAXLINE); //Clearing buffer to read message from the server
                n = read(clientfd,buffer,MAXLINE); //Reading the server's response
                ErrorCheck(n);
                Fputs(buffer,stdout);
                break;
                }
            
            case '2':{
                //printf("Choice 2\n");
                struct songRecord artist;
                printf("Enter the name of the artist: ");
                fgets(artist.artistName,MAXARTIST,stdin);
                artist.artistName[strlen(artist.artistName)-1] = '\0'; //Erasing newline character 
                strcat(buffer,artist.artistName); //Appending artist name to the buffer

                n = write(clientfd,buffer,strlen(buffer)); //Sending the server the requested artist
                ErrorCheck(n);

                bzero(buffer,MAXLINE); //Clearing buffer to read message from the server
                n = read(clientfd,buffer,MAXLINE);
                ErrorCheck(n);
                Fputs(buffer,stdout);
                break;
                }
            //search genre
            case '3':{
                //printf("Choice 3\n");
                struct songRecord Genre;
                printf("Enter the genre: ");
                fgets(Genre.genre,MAXGENRE,stdin);
                Genre.genre[strlen(Genre.genre)-1] = '\0'; //Erasing newline character 
                strcat(buffer,Genre.genre); //Appending genre to the buffer

                n = write(clientfd,buffer,strlen(buffer)); //Sending the server the requested genre
                ErrorCheck(n);

                //Clear buffer to get ready to read the server response
                bzero(buffer,MAXLINE);
                n = read(clientfd,buffer,MAXLINE);
                ErrorCheck(n);
                Fputs(buffer,stdout);
                break;
                }

            //Create playlist
            case '4': {
                struct playList newPlayList; //New playlist object

                //Getting name of new playlist
                printf("Enter name of new playlist: ");
                fgets(newPlayList.playListName,MAXLISTNAME,stdin);
                newPlayList.playListName[strlen(newPlayList.playListName)-1] = '\0'; //Erasing newline character
                strcat(buffer,appUser.userName); //Apending owner name to the buffer
                strncat(buffer,&comma,1);
                strcat(buffer,newPlayList.playListName); //Appending playlist name to the buffer
                
                //Write to server the choice, owner of the playlist, and name of the playlist
                n = write(clientfd,buffer,strlen(buffer));
                ErrorCheck(n);

                bzero(buffer,MAXLINE); //Clear buffer to write to server songs to add to the playlist
                int index=0;
                const char* Done = "Done"; //Done message 
                char song[MAXSONG]; //Temporary string to hold potential song

                //Priming reads
                printf("Enter a song(Enter 'Done' to quit adding songs): ");
                fgets(song,MAXSONG,stdin);
                song[strlen(song)-1] = '\0'; //Erasing newline character 
                
                while(strcmp(Done,song) != 0){ //Continue to prompt user until Done
                    strcpy(newPlayList.songs[index].songName,song); //If user not done, copy song to list of songs
                    song[0] = '\0'; //Clearing temporary song string
                    strcat(buffer,newPlayList.songs[index].songName); //Appending to the buffer a requested song
                    
                    n = write(clientfd,buffer,strlen(buffer)); //Send buffer(song) to check if the song exists
                    ErrorCheck(n);
                    
                    bzero(buffer,MAXLINE);
                    n = read(clientfd,buffer,MAXLINE); //Read back message from server (Song added to playlist OR Song not found in file)
                    ErrorCheck(n);
                    //Print server response to the console
                    Fputs(buffer,stdout);

                    //Recurring statements
                    index++;
                    bzero(buffer,MAXLINE);
                    printf("Enter a song(Enter 'Done' to quit adding songs): ");
                    fgets(song,MAXSONG,stdin);
                    song[strlen(song)-1] = '\0'; //Erasing newline character
                }

                if(strcmp(Done,song) == 0){
                    bzero(buffer,MAXLINE);
                    strcat(buffer,song);
                    n = write(clientfd,buffer,strlen(buffer));
                    ErrorCheck(n);
                    bzero(buffer,MAXLINE);
                    n = read(clientfd,buffer,MAXLINE);
                    ErrorCheck(n);
                    Fputs(buffer,stdout);
                }
                break;
                }
            case '5':{ //Add to playlist 
                //Get playlist and requested song to add from the user
                char playListName[MAXLISTNAME];
                char song[MAXSONG];

                printf("Enter the playlist to add to: ");
                fgets(playListName,MAXLISTNAME,stdin);
                playListName[strlen(playListName)-1] = '\0'; //Erasing newline character

                printf("Enter the song to add: ");
                fgets(song,MAXSONG,stdin);
                song[strlen(song)-1] = '\0'; //Erasing newline character

                //Append to the buffer the owner of the PL, name of the PL, and song to add
                strcat(buffer,appUser.userName);
                strncat(buffer,&comma,1);
                strcat(buffer,playListName);
                strncat(buffer,&comma,1);
                strcat(buffer,song);

                //Write to server the choice, owner of the PL, and name of the PL, and song
                n = write(clientfd,buffer,strlen(buffer));
                ErrorCheck(n);

                bzero(buffer,MAXLINE);
                n = read(clientfd,buffer,MAXLINE);
                ErrorCheck(n);
                Fputs(buffer,stdout);
                
                //Read the server response (Song added to PL OR Song not found)
                break;
                }

            case '6': //Display playlist
                char userName[MAXUSERNAME]; //Owner of PL
                char playListName[MAXLISTNAME]; //Name of the PL

                //Get username
                printf("Enter the owner(username) of the playlist: ");
                fgets(userName,MAXUSERNAME,stdin);
                userName[strcspn(userName, "\n")] = 0; //Erasing newline character

                //Get PL name
                printf("Enter the name of the playlist: ");
                fgets(playListName,MAXLISTNAME,stdin);
                playListName[strcspn(playListName, "\n")] = 0; //Erasing newline characeter

                //Append username and PL name to the buffer
                strcat(buffer,userName);
                strncat(buffer,&comma,1);
                strcat(buffer,playListName);
                
                n = write(clientfd,buffer,strlen(buffer)); //Write to the server username and PL name
                ErrorCheck(n);

                n = read(clientfd,buffer,MAXLINE);
                ErrorCheck(n);

                const char* failMessage = "Username or playlist name not found!\n";
                if(strcmp(buffer,failMessage) == 0){
                    Fputs(buffer,stdout);
                }

                else{
                    struct playList displayList;
                    char *buffer_token = strtok(buffer,",");

                    strcpy(displayList.userName,buffer_token); //Get username
                    buffer_token = strtok(NULL,",");
                    strcpy(displayList.playListName,buffer_token); //Get PL name

                    int songCount = 0; //Number of songs in the PL
                    buffer_token = strtok(NULL,",");
                    while(buffer_token != NULL){ //Get each song in the PL
                        strcpy(displayList.songs[songCount].songName,buffer_token);
                        buffer_token = strtok(NULL,",");
                        songCount++;
                    }
                    printf("\nPlaylist found!\n");
                    printf("Owner: %s\n",displayList.userName);
                    printf("Playlist Name: %s\n",displayList.playListName);

                    for(int i=0;i<songCount;i++){ 
                        printf("(%d): %s\n",i+1,displayList.songs[i].songName);
                    }
                }
                
                break;

            case '7': //Get Data Insights
                n = write(clientfd,buffer,strlen(buffer));
                ErrorCheck(n);

                bzero(buffer,MAXLINE);
                n = read(clientfd,buffer,MAXLINE);
                ErrorCheck(n);

                printf("Top 10 most popular songs between 2010-2023:\n");
                Fputs(buffer,stdout);
                break;

            default:
                printf("Invalid choice\n");
                break;
        }
        bzero(buffer,MAXLINE);
        choice[0] = '\0';
        printf("\n");
        printf("****MAIN MENU****\n");
        printf("(1) Search Song\n");
        printf("(2) Search Artist\n");
        printf("(3) Search Genre\n");
        printf("(4) Create Playlist\n");
        printf("(5) Add to Playlist\n");
        printf("(6) Display Playlist\n");
        printf("(7) Get Data Insights\n");
        printf("(8) Exit App\n");
        printf("Select an option: ");
        fscanf(stdin,"%s",choice);
        while(getchar() != '\n'){ //This line skips the newline character from the choice
                
        }
        strncat(buffer,choice,1); //Adding choice to the buffer
        printf("\n");
        newChoice = choice[0]; 
    }

    //Case if the client wants to disconnect from the server
    if(newChoice == '8'){
        char comma = ',';
        strncat(buffer,&comma,1);

        n = write(clientfd,buffer,strlen(buffer)); //Writing just choice 5 to the server
        ErrorCheck(n);
        
        bzero(buffer,MAXLINE); //Clearing buffer for the server's response
        n = read(clientfd,buffer,MAXLINE); //Reading the server's disconnection message
        ErrorCheck(n);
        printf("Message From Server:\n");
        Fputs(buffer,stdout);
    }
    return 0;
}