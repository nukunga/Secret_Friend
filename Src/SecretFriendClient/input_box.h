#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_MESSAGES 30
#define MAX_LENGTH 1024
#define CHAT_HEIGHT 40 // 채팅 영역의 높이
#define INPUT_HEIGHT 10 // 입력 영역의 높이

typedef struct {
    char messages[MAX_MESSAGES][MAX_LENGTH];
    int count;
} ChatData;

void gotoxy(int x, int y);
void clearScreen();
void displayChat();
DWORD WINAPI InputThread_main(LPVOID param);
DWORD WINAPI InputThread_chat(LPVOID param);