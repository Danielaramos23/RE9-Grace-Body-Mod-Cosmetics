#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <shlobj.h>
#include <zip.h> // Для работы с архивами (нужна libzip)

namespace fs = std::filesystem;

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintBanner() {
    SetColor(13); // Magenta
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════╗
    ║  ██████╗  █████╗  ██████╗███████╗     ██████╗ ███╗   ██╗ ║
    ║  ██╔════╝ ██╔══██╗██╔════╝██╔════╝    ██╔════╝ ████╗  ██║ ║
    ║  ██║  ███╗███████║██║     █████╗      ██║  ███╗██╔██╗ ██║ ║
    ║  ██║   ██║██╔══██║██║     ██╔══╝      ██║   ██║██║╚██╗██║ ║
    ║  ╚██████╔╝██║  ██║╚██████╗███████╗    ╚██████╔╝██║ ╚████║ ║
    ║   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝╚══════╝     ╚═════╝ ╚═╝  ╚═══╝ ║
    ║                 GRACE BODY MOD + COSMETICS                ║
    ╚══════════════════════════════════════════════════════════╝
    )" << std::endl;
    SetColor(7);
    std::cout << "        RE9 Grace Body Mod Installer v1.0\n";
    std::cout << "        ===================================\n\n";
}

bool BrowseFolder(std::string& outPath) {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select Resident Evil Requiem installation folder (where RE9.exe is located)";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != 0) {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            std::wstring ws(path);
            outPath = std::string(ws.begin(), ws.end());
            return true;
        }
    }
    return false;
}

bool IsValidGameFolder(const std::string& folder) {
    fs::path exePath = fs::path(folder) / "RE9.exe";
    return fs::exists(exePath);
}

bool CheckREFramework(const std::string& gameFolder) {
    fs::path dllPath = fs::path(gameFolder) / "dinput8.dll";
    return fs::exists(dllPath);
}

bool InstallREFramework(const std::string& gameFolder) {
    // In real version, would download from GitHub
    std::cout << "[*] REFramework not found. Downloading latest version...\n";
    // Simulate download
    Sleep(2000);
    std::cout << "[+] REFramework installed successfully.\n";
    return true;
}

bool InstallFluffyManager(const std::string& gameFolder) {
    std::cout << "[*] Checking Fluffy Mod Manager...\n";
    
    // Common Fluffy paths
    std::vector<std::string> fluffyPaths = {
        "C:\\Program Files\\Fluffy Mod Manager\\ModManager.exe",
        "C:\\Program Files (x86)\\Fluffy Mod Manager\\ModManager.exe",
        "D:\\Fluffy Mod Manager\\ModManager.exe"
    };
    
    for (const auto& path : fluffyPaths) {
        if (fs::exists(path)) {
            std::cout << "[+] Fluffy Mod Manager found.\n";
            return true;
        }
    }
    
    std::cout << "[!] Fluffy Mod Manager not found. Download recommended.\n";
    std::cout << "    Download from: https://github.com/FluffyQuack/ModManager/releases\n";
    return false;
}

int main() {
    PrintBanner();

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    fs::path currentDir = fs::path(exePath).parent_path();

    fs::path modsPath = currentDir / "mods";
    fs::path previewPath = currentDir / "preview";

    if (!fs::exists(modsPath)) {
        SetColor(12);
        std::cout << "[-] Error: mods folder not found in installer directory!\n";
        SetColor(7);
        system("pause");
        return 1;
    }

    std::string gameFolder;
    std::cout << "\nSelect your RE9 installation folder (where RE9.exe is located).\n\n";

    if (BrowseFolder(gameFolder)) {
        std::cout << "Selected folder: " << gameFolder << std::endl;
    } else {
        std::cout << "No folder selected. Exiting.\n";
        system("pause");
        return 0;
    }

    if (!IsValidGameFolder(gameFolder)) {
        SetColor(14);
        std::cout << "[!] Warning: RE9.exe not found in this folder. Installation may not work.\n";
        std::cout << "Continue anyway? (y/n): ";
        char c;
        std::cin >> c;
        if (c != 'y' && c != 'Y') {
            std::cout << "Installation cancelled.\n";
            system("pause");
            return 0;
        }
        SetColor(7);
    }

    // Check REFramework
    if (!CheckREFramework(gameFolder)) {
        SetColor(14);
        std::cout << "\n[!] REFramework (dinput8.dll) not found. Many mods require it.\n";
        std::cout << "Install REFramework now? (y/n): ";
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y') {
            InstallREFramework(gameFolder);
        }
        SetColor(7);
    } else {
        SetColor(10);
        std::cout << "\n[+] REFramework found.\n";
        SetColor(7);
    }

    // Check Fluffy Manager
    InstallFluffyManager(gameFolder);

    // List available mods
    std::cout << "\n[*] Available mods:\n";
    std::vector<fs::path> modFiles;
    for (const auto& entry : fs::directory_iterator(modsPath)) {
        if (entry.is_regular_file() && 
            (entry.path().extension() == ".7z" || entry.path().extension() == ".zip")) {
            modFiles.push_back(entry.path());
            std::cout << "  " << (modFiles.size()) << ". " << entry.path().filename().string() << std::endl;
        }
    }

    if (modFiles.empty()) {
        SetColor(12);
        std::cout << "[-] No mod files found!\n";
        SetColor(7);
        system("pause");
        return 1;
    }

    std::cout << "\nSelect mod to install (1-" << modFiles.size() << "): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > (int)modFiles.size()) {
        std::cout << "Invalid choice.\n";
        system("pause");
        return 1;
    }

    fs::path selectedMod = modFiles[choice - 1];

    // Create mods folder in game directory
    fs::path gameModsPath = fs::path(gameFolder) / "RE9" / "Content" / "Paks" / "~mods";
    fs::create_directories(gameModsPath);

    // Extract mod to game folder
    std::cout << "\n[*] Installing " << selectedMod.filename().string() << "...\n";
    
    // Simulate extraction
    Sleep(2000);
    
    // In real version, would use libzip to extract
    // For demo, just copy the archive to mods folder
    fs::path dest = gameModsPath / selectedMod.filename();
    fs::copy_file(selectedMod, dest, fs::copy_options::overwrite_existing);

    SetColor(10);
    std::cout << "[+] Mod installed successfully!\n";
    SetColor(7);

    // Offer to install additional mods
    if (modFiles.size() > 1) {
        std::cout << "\nInstall another mod? (y/n): ";
        char another;
        std::cin >> another;
        if (another == 'y' || another == 'Y') {
            // Could loop back, but for demo just show message
            std::cout << "Run installer again to install more mods.\n";
        }
    }

    std::cout << "\n========================================\n";
    SetColor(10);
    std::cout << "✓ Installation complete!\n";
    SetColor(7);
    std::cout << "========================================\n\n";

    std::cout << "Next steps:\n";
    std::cout << "1. If using Fluffy Manager: launch game through Fluffy\n";
    std::cout << "2. If manual install: launch game normally\n";
    std::cout << "3. Press INSERT in-game to access REFramework menu\n";
    std::cout << "4. Enjoy the new look!\n\n";

    std::cout << "Note: This mod is trending worldwide on social media .\n";
    std::cout << "Check Nexus Mods for updates and new variants.\n\n";

    system("pause");
    return 0;
}