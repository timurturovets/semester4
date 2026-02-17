#include <windows.h>
#include "menu.cpp"

int main(int argc, char **argv) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    tasks::menu::start(argc, argv);
}