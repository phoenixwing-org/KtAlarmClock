#include <direct.h>
#include <stdio.h>
#include <string>
#define MAX_PATH 256

inline std::string GetProgramDir() {
    char        exeFullPath[ MAX_PATH ]; // Full path
    std::string strPath = "";

    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
    strPath = (std::string)exeFullPath;
    int pos = strPath.find_last_of('\\', strPath.length());
    return strPath.substr(0, pos);
}

inline std::string GetProgramPath() {
    char exeFullPath[ MAX_PATH ]; // Full path
    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
    std::string strPath = exeFullPath;
}
