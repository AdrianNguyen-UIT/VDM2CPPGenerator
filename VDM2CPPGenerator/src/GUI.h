#pragma once
#include "Log.h"
#include "TextEditor.h"
#include "Timer.h"
class GUI
{
private :
	static ImFont* uiFont;
	static ImFont* editorFont;
	static ImFont* modalMessageFont;
	static ImFont* logFont;
	static std::string errorMessage;
	static TextEditor outputEditor;
	static TextEditor inputEditor;
	static bool isInputWindowFocused;
	static bool isOutputWindowFocused;
	static bool generate;

public:
	GUI() = default;
	GUI(const GUI& p_Input) = delete;
	GUI& operator=(const GUI& p_Input) = delete;

	static void Init(GLFWwindow* window);
	static void Terminate();
	static void Render(const std::function<void()>& OnGUIRender);

	static void ShowGUI(VDMParser::Parser& parser, bool& windowShouldClose);

	static void ShowInputWindow();
	static void ShowOutputWindow();
	static void ShowMainMenuBar(VDMParser::Parser& parser, bool &openLogWindow, bool &openTokenWindow, bool &windowShouldClose);
	static void ShowToolBar(VDMParser::Parser& parser);
	static void ShowBuildWindow(VDMParser::Parser& parser);
	static void ShowLogWindow(bool *open);
	static void ShowTokenWindow(bool *open, VDMParser::Parser& parser);
	static void ShowDockSpace(VDMParser::Parser& parser);

	static void NewInputAndOutput(VDMParser::Parser& parser);
	static void SaveInputFile(VDMParser::Parser& parser);
	static void SaveInputFileAs(VDMParser::Parser& parser);
	static void SaveOutputFileAs(VDMParser::Parser& parser);
	static void OpenInputFile(VDMParser::Parser& parser);
	static void GenerateSolution(VDMParser::Parser& parser, const std::string& solutionName, const std::string& projectName, const std::string& premakeOption);
	static void ShowErrorMessageModal();

	static void Parse(VDMParser::Parser& parser);

	static void Undo();
	static void Redo();
	static void Copy();
	static void Cut();
	static void Paste();
	static void Delete();
	static void SelectAll();
	static bool CanPaste();

private:
	static void HelpMarker(const char* desc);
};

void FindAndReplaceAll(std::string& input, const std::string& search, const std::string& replace);
std::optional<std::string> GetFileName(const std::string& path, char seperator = '/');
std::optional<std::string> GetDirectory(const std::string& path);