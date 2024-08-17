//#include "input_box.h"
//
//
//
//
//ChatData chatData; // ���� ���� ����
//
//void gotoxy(int x, int y) {
//    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
//    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
//}
//
//void clearScreen() {
//    COORD topLeft = { 0, 0 };
//    CONSOLE_SCREEN_BUFFER_INFO csbi;
//    DWORD written;
//    DWORD size;
//    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//
//    GetConsoleScreenBufferInfo(hConsole, &csbi);
//    size = csbi.dwSize.X * csbi.dwSize.Y;
//    FillConsoleOutputCharacter(hConsole, ' ', size, topLeft, &written);
//    SetConsoleCursorPosition(hConsole, topLeft);
//}
//
//void displayChat() {
//    gotoxy(0, 0);
//    for (int i = 0; i < chatData.count; i++) {
//        printf("%s\n", chatData.messages[i]);
//    }
//}
//
//DWORD WINAPI InputThread_chat(LPVOID param) {
//    char message[MAX_LENGTH];
//    while (1) {
//        gotoxy(0, CHAT_HEIGHT); // �Է� ��ġ�� Ŀ�� �̵�
//        printf("User: ");
//        fgets(message, MAX_LENGTH, stdin);
//
//        if (strncmp(message, "!q", 2) == 0) break;
//
//        message[strcspn(message, "\n")] = 0; // ���� ���� ����
//        if (chatData.count < MAX_MESSAGES) {
//            strcpy_s(chatData.messages[chatData.count++], MAX_LENGTH, message);
//        }
//        else {
//            memmove(chatData.messages, chatData.messages + 1, (MAX_MESSAGES - 1) * MAX_LENGTH);
//            strcpy_s(chatData.messages[MAX_MESSAGES - 1], MAX_LENGTH, message);
//        }
//
//        clearScreen();
//        displayChat();
//    }
//    return 0;
//}
//
//DWORD WINAPI InputThread_main(LPVOID param) {
//    char message[MAX_LENGTH];
//    while (1) {
//        gotoxy(0, CHAT_HEIGHT); // �Է� ��ġ�� Ŀ�� �̵�
//        printf("User: ");
//        fgets(message, MAX_LENGTH, stdin);
//
//        if (strncmp(message, "!q", 2) == 0) break;
//
//        message[strcspn(message, "\n")] = 0; // ���� ���� ����
//        if (chatData.count < MAX_MESSAGES) {
//            strcpy_s(chatData.messages[chatData.count++], MAX_LENGTH, message);
//        }
//        else {
//            memmove(chatData.messages, chatData.messages + 1, (MAX_MESSAGES - 1) * MAX_LENGTH);
//            strcpy_s(chatData.messages[MAX_MESSAGES - 1], MAX_LENGTH, message);
//        }
//
//        clearScreen();
//        // showMenu();
//    }
//    return 0;
//}
//
//int main() {
//    HANDLE hInputThread;
//    CONSOLE_SCREEN_BUFFER_INFO csbi;
//    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//
//    // �ʱ�ȭ
//    chatData.count = 0;
//
//    // �ܼ� ũ�� ����
//    GetConsoleScreenBufferInfo(hConsole, &csbi);
//    COORD newSize = { csbi.dwSize.X, CHAT_HEIGHT + INPUT_HEIGHT };
//    SetConsoleScreenBufferSize(hConsole, newSize);
//    SMALL_RECT windowSize = { 0, 0, csbi.dwSize.X - 1, CHAT_HEIGHT + INPUT_HEIGHT - 1 };
//    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
//
//    clearScreen();
//
//    // �Է� ������ ����
//    hInputThread = CreateThread(NULL, 0, InputThread_main, NULL, 0, NULL);
//    hInputThread = CreateThread(NULL, 0, InputThread_chat, NULL, 0, NULL);
//
//    // �Է� �����尡 ����� ������ ���
//    WaitForSingleObject(hInputThread, INFINITE);
//
//    // ������ ����
//    CloseHandle(hInputThread);
//
//    return 0;
//}
