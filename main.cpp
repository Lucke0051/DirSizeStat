#include <conio.h>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <map>

using namespace std;
using namespace filesystem;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#define KILO 1000
#define MEGA 1000000
#define GIGA 1000000000

filesystem::path selectedPath;

struct DiveData {
    string filename;
    unsigned long long int size = 0;
};

struct DiveDirectory : DiveData {
    map<string, DiveData> content;
};

string fileSizeInUnits(unsigned long long int fileSize) {
    if (fileSize >= GIGA) {
        return to_string(fileSize / GIGA) + " GB";
    } else if (fileSize >= MEGA) {
        return to_string(fileSize / MEGA) + " MB";
    } else if (fileSize >= KILO) {
        return to_string(fileSize / KILO) + " kB";
    }

    return to_string(fileSize) + " B";
}

DiveDirectory dive(filesystem::path path) {
    DiveDirectory data = {{.filename = path.filename().string()},
                          {}};

    try {
        for (const auto &entry: directory_iterator(path.string(), directory_options::skip_permission_denied)) {
            filesystem::path entryPath = entry.path();
            string filename = entryPath.filename().string();

            if (filename.starts_with("$")) {
                continue;
            }

            if (entry.is_regular_file()) {
                unsigned long long int filesize = entry.file_size();
                data.content[entryPath.filename().string()] = {.filename = filename, .size = filesize};
                data.size += filesize;
            } else if (entry.is_directory()) {
                DiveData diveData = dive(entry.path());
                data.content[entryPath.filename().string()] = diveData;
                data.size += diveData.size;
            }
        }
    } catch (...) {}

    return data;
}

void clearConsole() {
    COORD topLeft = {0, 0};
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);

    FillConsoleOutputCharacterA(
            console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );

    FillConsoleOutputAttribute(
            console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );

    SetConsoleCursorPosition(console, topLeft);
}

void drawConsole(DiveData data) {
    cout << "Size: " << fileSizeInUnits(data.size);
}

int main() {
    short offset = 80;

    filesystem::path initialPath = filesystem::path("C:\\Users\\lukas\\Desktop\\Repositories");
    selectedPath = initialPath;

    cout << "Reading: " << initialPath.string() << endl;

    DiveData data = dive(initialPath);

    clearConsole();
    drawConsole(data);

    return 0;
}
