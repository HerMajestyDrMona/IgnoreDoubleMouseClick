#include <windows.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
using namespace std;

const wstring ProgramName = L"IgnoreDoubleMouseClick";
const wstring ConfigFile = L"IgnoreDoubleMouseClick.txt";

wstring CurrentExeWorks = L"";
wstring CurrentExeWorksFilenameOnly = L"";
wstring CurrentExeWorksPath = L"";

// Threshold in milliseconds (e.g., 80ms)
long long CLICK_THRESHOLD = 80;

// Global variables to store the time of the last left and right clicks
std::chrono::steady_clock::time_point lastLeftClickTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point lastRightClickTime = std::chrono::steady_clock::now();

// Boolean to control console visibility
bool showConsole = false;
bool MonitorLeftMouseClick = true;
bool MonitorRightMouseClick = true;
bool AutomaticallyRunThisProgramOnStartup = false;

// Function to get current time in milliseconds
long long GetSteadyClockMilliseconds()
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return duration.count();
}

BOOL FileExistsW(LPCWSTR szPath)
{
	/*if (boost::filesystem::exists(szPath)) {
		return true;
	}
	return false;*/

	DWORD dwAttrib = GetFileAttributesW(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL FileExists(LPCTSTR szPath)
{
	/*if (boost::filesystem::exists(szPath)) {
		return true;
	}
	return false;*/

	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void find_and_replace(string& source, string const& find, string const& replace)
{
	//boost::replace_all(source, find, replace);
	for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length() - find.length() + 1;
	}
}

void find_and_replace_ws(wstring& source, wstring const& find, wstring const& replace)
{
	//boost::replace_all(source, find, replace);
	for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length() - find.length() + 1;
	}
}

bool NewIsConfigLineEqualTo(string ConfigLine, string SearchFor, string SearchForValue) {
	string SearchForTmp = SearchFor + "=" + SearchForValue;
	std::size_t sssearch1 = ConfigLine.find(SearchForTmp);
	if (sssearch1 != std::string::npos) {
		return true;
	}
	else {
		SearchForTmp = SearchFor + " = " + SearchForValue;
		sssearch1 = ConfigLine.find(SearchForTmp);
		if (sssearch1 != std::string::npos) {
			return true;
		}
		else {
			SearchForTmp = SearchFor + " =" + SearchForValue;
			sssearch1 = ConfigLine.find(SearchForTmp);
			if (sssearch1 != std::string::npos) {
				return true;
			}
			else {
				SearchForTmp = SearchFor + "= " + SearchForValue;
				sssearch1 = ConfigLine.find(SearchForTmp);
				if (sssearch1 != std::string::npos) {
					return true;
				}
			}
		}
	}
	return false;
}

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

bool is_number_ws(const std::wstring& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

long long int NewConfigGetIntValueAfter(string ConfigLine, string SearchFor) {
	bool GotCorrectString = false;
	string CorrectString = "";
	string SearchForTmp = SearchFor + "=";
	std::size_t sssearch1 = ConfigLine.find(SearchForTmp);
	if (sssearch1 != std::string::npos) {
		CorrectString = ConfigLine.substr(sssearch1 + SearchForTmp.length());
		GotCorrectString = true;
	}
	else {
		SearchForTmp = SearchFor + " = ";
		sssearch1 = ConfigLine.find(SearchForTmp);
		if (sssearch1 != std::string::npos) {
			CorrectString = ConfigLine.substr(sssearch1 + SearchForTmp.length());
			GotCorrectString = true;
		}
		else {
			SearchForTmp = SearchFor + " =";
			sssearch1 = ConfigLine.find(SearchForTmp);
			if (sssearch1 != std::string::npos) {
				CorrectString = ConfigLine.substr(sssearch1 + SearchForTmp.length());
				GotCorrectString = true;
			}
			else {
				SearchForTmp = SearchFor + "= ";
				sssearch1 = ConfigLine.find(SearchForTmp);
				if (sssearch1 != std::string::npos) {
					CorrectString = ConfigLine.substr(sssearch1 + SearchForTmp.length());
					GotCorrectString = true;
				}
			}
		}
	}

	if (GotCorrectString) {
		find_and_replace(CorrectString, "\r\n", "");
		find_and_replace(CorrectString, "\r", "");
		find_and_replace(CorrectString, "\n", "");
		if (CorrectString.length() > 0) {
			if (is_number(CorrectString)) {
				return atoll(CorrectString.c_str());
			}
		}
	}
	return -696969;//Control value
}

void Mona_Load_Configuration() {
	string line = "";
	int rowcnt = 0;
	long long int TmpValueFromNewConfigGetIntFunction = -696969;
	if (FileExists(ConfigFile.c_str())) {
		ifstream settingsfile(ConfigFile, ios::binary);
		if (settingsfile.is_open()) {
			while (!settingsfile.eof()) {
				getline(settingsfile, line);
				++rowcnt;
				if (line.length() > 0) {

					find_and_replace(line, "\r\n", "");
					find_and_replace(line, "\r", "");
					find_and_replace(line, "\n", "");

					if (rowcnt > 1000) {
						//Just in case someone loads a 1000TB file as a config.
						break;
					}


					if (line.length() > 1) {
						string test2 = line.substr(0, 1);
						if (test2 == "/") {
							continue;
						}
						else if (test2 == "#") {
							continue;
						}
						else if (test2 == ";") {
							continue;
						}
						string LastChar = line.substr(line.length() - 1);
						if (LastChar == ";") {
							line = line.substr(0, line.length() - 1);
						}
					}

					if (NewIsConfigLineEqualTo(line, "ShowConsoleWindow", "1") || NewIsConfigLineEqualTo(line, "ShowConsoleWindow", "true")) {
						showConsole = true;
						continue;
					}
					else if (NewIsConfigLineEqualTo(line, "ShowConsoleWindow", "0") || NewIsConfigLineEqualTo(line, "ShowConsoleWindow", "false")) {
						showConsole = false;
						continue;
					}

					if (NewIsConfigLineEqualTo(line, "MonitorLeftMouseClick", "1") || NewIsConfigLineEqualTo(line, "MonitorLeftMouseClick", "true")) {
						MonitorLeftMouseClick = true;
						continue;
					}
					else if (NewIsConfigLineEqualTo(line, "MonitorLeftMouseClick", "0") || NewIsConfigLineEqualTo(line, "MonitorLeftMouseClick", "false")) {
						MonitorLeftMouseClick = false;
						continue;
					}

					if (NewIsConfigLineEqualTo(line, "MonitorRightMouseClick", "1") || NewIsConfigLineEqualTo(line, "MonitorRightMouseClick", "true")) {
						MonitorRightMouseClick = true;
						continue;
					}
					else if (NewIsConfigLineEqualTo(line, "MonitorRightMouseClick", "0") || NewIsConfigLineEqualTo(line, "MonitorRightMouseClick", "false")) {
						MonitorRightMouseClick = false;
						continue;
					}

					if (NewIsConfigLineEqualTo(line, "AutomaticallyRunThisProgramOnStartup", "1") || NewIsConfigLineEqualTo(line, "AutomaticallyRunThisProgramOnStartup", "true")) {
						AutomaticallyRunThisProgramOnStartup = true;
						continue;
					}
					else if (NewIsConfigLineEqualTo(line, "AutomaticallyRunThisProgramOnStartup", "0") || NewIsConfigLineEqualTo(line, "AutomaticallyRunThisProgramOnStartup", "false")) {
						AutomaticallyRunThisProgramOnStartup = false;
						continue;
					}

					TmpValueFromNewConfigGetIntFunction = NewConfigGetIntValueAfter(line, "IgnoreNextMouseClickTimeMilliseconds");
					if (TmpValueFromNewConfigGetIntFunction != -696969) {
						if (TmpValueFromNewConfigGetIntFunction >= 0) {
							if (TmpValueFromNewConfigGetIntFunction < 2001) {
								CLICK_THRESHOLD = static_cast<int>(TmpValueFromNewConfigGetIntFunction);
								continue;
							}
						}
					}

					
				}
			}
		}
	}
}

void ShowConsoleWindow()
{
	AllocConsole(); // Create a new console window
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	std::cout.clear();
	std::cin.clear();
	std::cout << "Console window is now visible." << std::endl;
}

// Function to process mouse clicks
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        auto currentTime = std::chrono::steady_clock::now();
        long long timeDiff = 0;

        if (wParam == WM_LBUTTONDOWN && MonitorLeftMouseClick)
        {
            timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastLeftClickTime).count();
            if (timeDiff < CLICK_THRESHOLD)
            {
                if (showConsole) std::cout << "Left click ignored (too frequent, within " << timeDiff << " ms)" << std::endl;
                return 1; // Block the click
            }
            else
            {
                lastLeftClickTime = currentTime;
                if (showConsole) std::cout << "Left click accepted (after " << timeDiff << " ms)" << std::endl;
            }
        }
        else if (wParam == WM_RBUTTONDOWN && MonitorRightMouseClick)
        {
            timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRightClickTime).count();
            if (timeDiff < CLICK_THRESHOLD)
            {
                if (showConsole) std::cout << "Right click ignored (too frequent, within " << timeDiff << " ms)" << std::endl;
                return 1; // Block the click
            }
            else
            {
                lastRightClickTime = currentTime;
                if (showConsole) std::cout << "Right click accepted (after " << timeDiff << " ms)" << std::endl;
            }
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool RegistryGetStringValue(std::wstring& valueBuf, HKEY MainKey, const std::wstring& regSubKey, const std::wstring& regValue)
{
	size_t bufferSize = 0xFFF; // If too small, will be resized down below.
	//std::wstring valueBuf; // Contiguous buffer since C++11.
	valueBuf.resize(bufferSize);
	auto cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
	auto rc = RegGetValueW(
		MainKey,
		regSubKey.c_str(),
		regValue.c_str(),
		RRF_RT_REG_SZ,
		nullptr,
		static_cast<void*>(valueBuf.data()),
		&cbData
	);
	while (rc == ERROR_MORE_DATA)
	{
		// Get a buffer that is big enough.
		cbData /= sizeof(wchar_t);
		if (cbData > static_cast<DWORD>(bufferSize))
		{
			bufferSize = static_cast<size_t>(cbData);
		}
		else
		{
			bufferSize *= 2;
			cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
		}
		valueBuf.resize(bufferSize);
		rc = RegGetValueW(
			MainKey,
			regSubKey.c_str(),
			regValue.c_str(),
			RRF_RT_REG_SZ,
			nullptr,
			static_cast<void*>(valueBuf.data()),
			&cbData
		);
	}
	if (rc == ERROR_SUCCESS)
	{
		cbData /= sizeof(wchar_t);
		valueBuf.resize(static_cast<size_t>(cbData - 1)); // remove end null character
		//return valueBuf;
		return true;
	}
	else
	{
		//throw std::runtime_error("Windows system error code: " + std::to_string(rc));
	}
	return false;
}

bool RegistrySetStringValue(HKEY MainKey, const std::wstring& regSubKey, const std::wstring& regValue, const std::wstring& stringToSet)
{
	size_t LengthXX = stringToSet.length() * 2;
	auto rc = RegSetKeyValueW(
		MainKey,
		regSubKey.c_str(),
		regValue.c_str(),
		//RRF_RT_REG_SZ,
		REG_SZ,
		stringToSet.c_str(),
		(DWORD)LengthXX
	);

	if (rc == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		//throw std::runtime_error("Windows system error code: " + std::to_string(rc));
	}

	return false;
}

bool RegistryDeleteKeyValue(HKEY MainKey, const std::wstring& regSubKey, const std::wstring& regValue)
{
	auto rc = RegDeleteKeyValueW(
		MainKey,
		regSubKey.c_str(),
		regValue.c_str()
	);

	if (rc == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		//throw std::runtime_error("Windows system error code: " + std::to_string(rc));
	}

	return false;
}

void Check_And_Set_Auto_Program_Startup() {
	wstring QueryKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	wstring QueryValue = L"IgnoreDoubleMouseClick";

	bool FoundInRegistry = false;
	bool CorrectInRegistry = false;

	wstring CurrentExeWorksForRegistry = L"\"";
	CurrentExeWorksForRegistry = CurrentExeWorksForRegistry + CurrentExeWorks + L"\"";


	wstring InAutostart = L"";
	if (RegistryGetStringValue(InAutostart, HKEY_CURRENT_USER, QueryKey, QueryValue)) {
		if (InAutostart.length() > 0) {
			FoundInRegistry = true;
			if (CurrentExeWorksForRegistry == InAutostart) {
				CorrectInRegistry = true;
				if (showConsole) {
					std::wcout << L"The Auto Startup registry key value is correct. No need to update." << std::endl;
				}
			}
			else {
				if (showConsole) {
					std::wcout << L"The existing Auto Startup registry key value: " << InAutostart << L" Length: " << InAutostart.length() << L" does not match the current process: " << CurrentExeWorksForRegistry << L" Length: " << CurrentExeWorksForRegistry.length() << std::endl;
				}
			}
		}
	}
	if (AutomaticallyRunThisProgramOnStartup) {
		if (!FoundInRegistry || !CorrectInRegistry) {
			//Set the key
			if (RegistrySetStringValue(HKEY_CURRENT_USER, QueryKey, QueryValue, CurrentExeWorksForRegistry)) {
				if (showConsole) {
					std::wcout << L"Successfully set the Auto Startup registry key value to: " << CurrentExeWorks << std::endl;
				}
			}
			else {
				if (showConsole) {
					std::wcout << L"Error setting the Auto Startup registry key value to: " << CurrentExeWorks << std::endl;
				}
			}
		}

	}
	else {
		if (FoundInRegistry) {
			//Delete key
			if (RegistryDeleteKeyValue(HKEY_CURRENT_USER, QueryKey, QueryValue)) {
				if (showConsole) {
					std::wcout << L"Successfully removed the Auto Startup registry key value for: " << CurrentExeWorks << std::endl;
				}
			}
			else {
				if (showConsole) {
					std::wcout << L"ERROR: Unable to delete the Auto Startup registry key value for: " << CurrentExeWorks << std::endl;
				}
			}
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	wchar_t result[MAX_PATH];
	CurrentExeWorks = std::wstring(result, GetModuleFileNameW(NULL, result, MAX_PATH));
	std::filesystem::path CurrentPath(CurrentExeWorks);
	CurrentExeWorksFilenameOnly = CurrentPath.filename().wstring();
	//std::wcout << CurrentExeWorksFilenameOnly << endl;

	wstring MutexName = L"Mona" + ProgramName + L"-AlreadyRunning";
	HANDLE handleMutex = CreateMutex(NULL, TRUE, MutexName.c_str());
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		std::wcout << ProgramName << L" is already running. Exiting this instance..." << endl;
		return 1;
	}

	Mona_Load_Configuration();
    // Toggle console window visibility based on the showConsole flag
	if (showConsole) {
		ShowConsoleWindow();
	}

	//Check auto start:
	Check_And_Set_Auto_Program_Startup();

    // Set the mouse hook
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (!mouseHook)
    {
        if (showConsole) std::cerr << "Failed to install mouse hook!" << std::endl;
        return 1;
    }

    if (showConsole) std::cout << "Mouse hook installed. Clicks within " << CLICK_THRESHOLD << " ms will be ignored." << std::endl;

    // Message loop to keep the application running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the mouse hook
    UnhookWindowsHookEx(mouseHook);

    return 0;
}
