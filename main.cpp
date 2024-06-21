#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

typedef struct AudioFile {
    char filename[100];
    char artist[100];
    int duration;
    struct AudioFile* next;
} AudioFile;

AudioFile* createAudioFile(const char* filename, const char* artist, int duration) {
    AudioFile* newFile = (AudioFile*)malloc(sizeof(AudioFile));
    if (newFile == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newFile->filename, filename);
    strcpy(newFile->artist, artist);
    newFile->duration = duration;
    newFile->next = NULL;
    return newFile;
}

void addAudioFile(AudioFile** playlist, const char* filename, const char* artist, int duration) {
    AudioFile* newFile = createAudioFile(filename, artist, duration);
    if (*playlist == NULL) {
        *playlist = newFile;
    } else {
        AudioFile* current = *playlist;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newFile;
    }
}

void printPlaylist(AudioFile* playlist) {
    AudioFile* current = playlist;
    int count = 1;
    while (current != NULL) {
        printf("%d. Filename: %s, Artist: %s, Duration: %d seconds\n", 
                count++, current->filename, current->artist, current->duration);
        current = current->next;
    }
}

void freePlaylist(AudioFile* playlist) {
    while (playlist != NULL) {
        AudioFile* temp = playlist;
        playlist = playlist->next;
        free(temp);
    }
}

void playAudio(AudioFile* currentSong) {
    printf("Loading audio file: %s\n", currentSong->filename); 
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if(SDL_LoadWAV(currentSong->filename, &wavSpec, &wavStart, &wavLength) == NULL){
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if(deviceId == 0){
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_QueueAudio(deviceId, wavStart, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);

    while (SDL_GetQueuedAudioSize(deviceId) > 0) {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavStart);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    AudioFile* playlist = NULL;

    addAudioFile(&playlist, "song1.wav", "Artist 1", 15);
    addAudioFile(&playlist, "song2.wav", "Artist 2", 233);
    addAudioFile(&playlist, "song3.wav", "Artist 3", 233);


    printf("Playlist:\n");
    printPlaylist(playlist);

    AudioFile* currentSong = playlist;
    playAudio(currentSong);

    char choice;
    do {
        printf("\nEnter 'n' for next song, 'p' for previous song, or 'q' to quit: ");
        scanf("%c", &choice);

        switch(choice) {
            case 'n':
                if (currentSong->next != NULL)
                    currentSong = currentSong->next;
                else
                    printf("End of playlist reached.\n");
                break;
            case 'p':
                
                if (playlist != currentSong) {
                    AudioFile* prevSong = playlist;
                    while (prevSong->next != currentSong)
                        prevSong = prevSong->next;
                    currentSong = prevSong;
                } else {
                    printf("Start of playlist reached.\n");
                }
                break;
            case 'q':
                break;
            default:
                printf("Invalid option.\n");
        }

        if (choice == 'n' || choice == 'p') {
            playAudio(currentSong);
        }

    } while(choice != 'q');

    freePlaylist(playlist);

    return 0;
}
