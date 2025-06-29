#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

class User {
    string username;
    string password;

public:
    User(const string& u, const string& p) : username(u), password(p) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
};

vector<User> loadUsers() {
    vector<User> users;
    ifstream fin("users.txt");
    if (!fin) return users;

    string u, p;
    while (fin >> u >> p) {
        users.emplace_back(u, p);
    }
    return users;
}

void saveUser(const User& user) {
    ofstream fout("users.txt", ios::app);
    fout << user.getUsername() << " " << user.getPassword() << "\n";
}

bool userExists(const vector<User>& users, const string& username) {
    for (const auto& u : users)
        if (u.getUsername() == username) return true;
    return false;
}

bool validateLogin(const vector<User>& users, const string& username, const string& password) {
    for (const auto& u : users)
        if (u.getUsername() == username && u.getPassword() == password)
            return true;
    return false;
}

// Chat log filename: chat_user1_user2.txt (lex order)
string getChatFilename(const string& u1, const string& u2) {
    if (u1 < u2)
        return "chat_" + u1 + "_" + u2 + ".txt";
    else
        return "chat_" + u2 + "_" + u1 + ".txt";
}

void showChat(const string& loggedInUser, const string& chatUser) {
    string filename = getChatFilename(loggedInUser, chatUser);
    ifstream fin(filename);
    cout << "\n--- Chat with " << chatUser << " ---\n";
    if (!fin) {
        cout << "(No chat history)\n";
    } else {
        string line;
        while (getline(fin, line)) {
            cout << line << "\n";
        }
    }
    cout << "------------------------\n";
}

void appendMessage(const string& loggedInUser, const string& chatUser, const string& message) {
    string filename = getChatFilename(loggedInUser, chatUser);
    ofstream fout(filename, ios::app);
    fout << loggedInUser << ": " << message << "\n";
}

int main() {
    cout << "Welcome to Offline Chat!\n";

    vector<User> users = loadUsers();

    while (true) {
        cout << "\n1) Register\n2) Login\n3) Exit\nChoose: ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string username, password;
            cout << "Choose username: ";
            getline(cin, username);
            if (userExists(users, username)) {
                cout << "Username taken. Try again.\n";
                continue;
            }
            cout << "Choose password: ";
            getline(cin, password);
            users.emplace_back(username, password);
            saveUser(users.back());
            cout << "Registered! You can now login.\n";

        } else if (choice == 2) {
            string username, password;
            cout << "Username: ";
            getline(cin, username);
            cout << "Password: ";
            getline(cin, password);

            if (!validateLogin(users, username, password)) {
                cout << "Invalid credentials.\n";
                continue;
            }

            cout << "Logged in as " << username << "\n";

            // Chat session
            while (true) {
                cout << "\nUsers available to chat:\n";
                int idx = 1;
                vector<string> others;
                for (const auto& u : users) {
                    if (u.getUsername() != username) {
                        cout << idx << ") " << u.getUsername() << "\n";
                        others.push_back(u.getUsername());
                        idx++;
                    }
                }
                if (others.empty()) {
                    cout << "(No other users registered yet)\n";
                    break;
                }
                cout << idx << ") Logout\nChoose user to chat: ";
                int chatChoice;
                cin >> chatChoice;
                cin.ignore();

                if (chatChoice == idx) {
                    cout << "Logging out...\n";
                    break;
                }

                if (chatChoice < 1 || chatChoice >= idx) {
                    cout << "Invalid choice.\n";
                    continue;
                }

                string chatUser = others[chatChoice - 1];

                // Chat loop
                while (true) {
                    showChat(username, chatUser);

                    cout << "Enter message (type '/exit' to back): ";
                    string msg;
                    getline(cin, msg);
                    if (msg == "/exit") break;

                    if (!msg.empty()) {
                        appendMessage(username, chatUser, msg);
                    }
                }
            }

        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
