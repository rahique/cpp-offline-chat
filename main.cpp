#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <conio.h>  // for _kbhit(), _getch()
#include <mutex>

using namespace std;

mutex print_mutex;

class ChatUser {
    int userId;
    char* buffer;
    int bufferSize;

public:
    ChatUser(int id) : userId(id), buffer(nullptr), bufferSize(0) {}

    ~ChatUser() {
        if (buffer) delete[] buffer;
    }

    void allocateBuffer(int size) {
        if (buffer) delete[] buffer;
        bufferSize = size;
        buffer = new char[bufferSize];
        memset(buffer, 0, bufferSize);
    }

    void sendMessage(const string& msg) {
        ofstream fout("chat.txt", ios::app);
        if (!fout) {
            cerr << "Error opening file to send message.\n";
            return;
        }
        fout << "User " << userId << ": " << msg << "\n";
        fout.close();
    }

    void readMessages() {
        ifstream fin("chat.txt");
        if (!fin) {
            cerr << "Error opening file to read messages.\n";
            return;
        }

        fin.seekg(0, ios::end);
        int length = fin.tellg();
        fin.seekg(0, ios::beg);

        allocateBuffer(length + 1);
        fin.read(buffer, length);
        buffer[length] = '\0';

        fin.close();

        lock_guard<mutex> lock(print_mutex);
        system("cls");
        cout << "---- Chat History ----\n";
        cout << buffer << "\n";
        cout << "----------------------\n";
        cout << "Enter message (type 'exit' to quit): ";
        cout.flush();
    }
};

void refreshLoop(ChatUser* user, bool* running) {
    while (*running) {
        user->readMessages();
        this_thread::sleep_for(chrono::seconds(5));
    }
}

int main(int argc, char* argv[]) {

    // Create the chat file if it does not exist yet
    ofstream createFile("chat.txt", ios::app);
    createFile.close();

    if (argc < 2) {
        cout << "Usage: chat.exe <userId>\nExample: chat.exe 1\n";
        return 1;
    }
    if (argc < 2) {
        cout << "Usage: chat.exe <userId>\nExample: chat.exe 1\n";
        return 1;
    }

    int userId = atoi(argv[1]);
    if (userId != 1 && userId != 2) {
        cout << "User ID must be 1 or 2\n";
        return 1;
    }

    ChatUser user(userId);
    bool running = true;

    thread refresher(refreshLoop, &user, &running);

    string input;
    while (running) {
        // Check if a key was pressed without blocking
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r') { // Enter key pressed
                if (!input.empty()) {
                    if (input == "exit") {
                        running = false;
                        break;
                    }
                    user.sendMessage(input);
                    input.clear();
                }
            }
            else if (ch == 8) { // Backspace
                if (!input.empty()) {
                    input.pop_back();
                    lock_guard<mutex> lock(print_mutex);
                    cout << "\b \b";
                    cout.flush();
                }
            }
            else {
                input.push_back(ch);
                lock_guard<mutex> lock(print_mutex);
                cout << ch;
                cout.flush();
            }
        }
        else {
            // Sleep a bit to reduce CPU usage
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }

    refresher.join();

    cout << "\nUser " << userId << " exited chat.\n";
    return 0;
}
