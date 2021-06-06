#include "pch.h"
#include "GUI.h"
#include "FileDialogs.h"
#include <Windows.h>
#include <ShellAPI.h>
#include "IconsForkAwesome.h"

ImFont* GUI::uiFont;
ImFont* GUI::editorFont;
ImFont* GUI::modalMessageFont;
ImFont* GUI::logFont;
TextEditor GUI::outputEditor;
TextEditor GUI::inputEditor;
std::string GUI::errorMessage;
bool GUI::isInputWindowFocused;
bool GUI::isOutputWindowFocused;
bool GUI::generate;
void GUI::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	uiFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f);
	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
	io.Fonts->AddFontFromFileTTF("resources/fonts/forkawesome-webfont.ttf", 16.0f, &config, icon_ranges);

	editorFont = io.Fonts->AddFontFromFileTTF("resources/fonts/FiraCode-Retina.ttf", 20.0f);
	modalMessageFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 22.0f);
	logFont = io.Fonts->AddFontFromFileTTF("resources/fonts/CascadiaCodePL.ttf", 20.0f);

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.WantCaptureMouse = true;
	io.WantCaptureKeyboard = true;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	style.WindowBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.TabBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;

	style.WindowPadding = ImVec2(12.0f, 8.0f);
	style.FramePadding = ImVec2(12.0f, 4.0f);

	style.ItemSpacing = ImVec2(15.0f, 8.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 7.0f);
	style.ScrollbarSize = 11.0f;
	style.GrabMinSize = 11.0f;

	style.WindowRounding = 10.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 3.0f;
	style.PopupRounding = 3.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 4.0f;

	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.AntiAliasedFill = true;
	style.AntiAliasedLines = true;
	style.Colors[ImGuiCol_WindowBg].w = 0.5f;

	style.CellPadding = ImVec2(10.0f, 10.0f);
	style.DisplaySafeAreaPadding = ImVec2(3.0f, 5.0f);

	TextEditor::LanguageDefinition lang1 = TextEditor::LanguageDefinition::CPlusPlus();
	outputEditor.SetLanguageDefinition(lang1);
	outputEditor.SetImGuiChildIgnored(true);

	TextEditor::LanguageDefinition lang2 = TextEditor::LanguageDefinition::VDM();
	inputEditor.SetLanguageDefinition(lang2);
	inputEditor.SetImGuiChildIgnored(true);

	isInputWindowFocused = false;
	isOutputWindowFocused = false;
	generate = false;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void GUI::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::Render(const std::function<void()>& OnGUIRender)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (!OnGUIRender)
	{
		LOG_ERROR("Application: OnGuiRender is null");
		return;
	}

	OnGUIRender();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void GUI::ShowGUI(VDMParser::Parser& parser, bool& windowShouldClose)
{
	static bool openLogWindow = true;
	static bool openTokenWindow = true;
	ShowMainMenuBar(parser, openLogWindow, openTokenWindow, windowShouldClose);
	ShowDockSpace(parser);
	ShowInputWindow();
	ShowOutputWindow();
	ShowLogWindow(&openLogWindow);
	ShowBuildWindow(parser);
	ShowErrorMessageModal();
	ShowTokenWindow(&openTokenWindow, parser);
}

void GUI::ShowInputWindow()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Input", NULL, window_flags);
	ImGui::PushFont(editorFont);
	inputEditor.Render("InputTextEditor", ImVec2(0,0), true);
	ImGui::PopFont();
	static bool first = true;
	if (!first)
	{
		if (ImGui::IsWindowFocused())
		{
			isInputWindowFocused = true;
		}
		else if (isOutputWindowFocused)
		{
			isInputWindowFocused = false;
		}
	}
	else
	{
		first = false;
	}
	ImGui::End();
}

void GUI::ShowOutputWindow()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::Begin("Output", NULL, window_flags);
	ImGui::PushFont(editorFont);
	outputEditor.Render("OutputTextEditor", ImVec2(0, 0), true);
	ImGui::PopFont();

	static bool first = true;
	if (!first)
	{
		if (ImGui::IsWindowFocused())
		{
			isOutputWindowFocused = true;
		}
		else if (isInputWindowFocused)
		{
			isOutputWindowFocused = false;
		}
	}
	else
	{
		first = false;
	}
	ImGui::End();
}

void GUI::ShowMainMenuBar(VDMParser::Parser& parser, bool& openLogWindow, bool& openTokenWindow, bool& windowShouldClose)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_FILE_TEXT_O"  New", "Ctrl+N"))
			{
				NewInputAndOutput(parser);
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_FOLDER_OPEN_O"  Open", "Ctrl+O"))
			{
				OpenInputFile(parser);
			}
			ImGui::Spacing();
			ImGui::Separator();

			std::string save = ICON_FK_FLOPPY_O"  Save ";
			std::optional<std::string> fileName;
			if (!parser.InputIsEmpty())
			{
				fileName = GetFileName(parser.GetInputPath());
				if (fileName.has_value())
				{
					save += fileName.value();
				}
			}
			else
			{
				save += "Input file";
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(save.c_str(), "Ctrl+S", false, !parser.InputIsEmpty()))
			{ 
				SaveInputFile(parser); 
			}
			ImGui::Spacing();

			save = ICON_FK_CLONE"  Save ";
			if (fileName.has_value())
			{
				save += fileName.value() + " As...";
			}
			else
			{
				save += "Input file As...";
			}
			if (ImGui::MenuItem(save.c_str(), "Ctrl+Shift+S", false, !inputEditor.GetText().empty()))
			{
				SaveInputFileAs(parser);
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_CODE"  Save Output As...", "Ctrl+Alt+S", false, !outputEditor.GetText().empty()))
			{
				SaveOutputFileAs(parser);
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_TIMES"  Exit", "Alt+F4")) 
			{ 
				windowShouldClose = !windowShouldClose; 
			}
			ImGui::Spacing();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::Spacing();
			if (isInputWindowFocused)
			{
				if (ImGui::MenuItem(ICON_FK_UNDO"  Undo", "Ctrl+Z", nullptr, inputEditor.CanUndo()))
				{
					Undo();
				}
				ImGui::Spacing();
				if (ImGui::MenuItem(ICON_FK_REPEAT"  Redo", "Ctrl+Y", nullptr, inputEditor.CanRedo()))
				{
					Redo();
				}
			}
			else if (isOutputWindowFocused)
			{
				if (ImGui::MenuItem(ICON_FK_UNDO"  Undo", "Ctrl+Z", nullptr, outputEditor.CanUndo()))
				{
					Undo();
				}
				ImGui::Spacing();
				if (ImGui::MenuItem(ICON_FK_REPEAT"  Redo", "Ctrl+Y", nullptr, outputEditor.CanRedo()))
				{
					Redo();
				}
			}
			else
			{
				ImGui::MenuItem(ICON_FK_UNDO"  Undo", "Ctrl+Z", nullptr, false);
				ImGui::Spacing();
				ImGui::MenuItem(ICON_FK_REPEAT"  Redo", "Ctrl+Y", nullptr, false);
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			bool enable = isInputWindowFocused || isOutputWindowFocused;
			if (ImGui::MenuItem(ICON_FK_SCISSORS"  Cut", "Ctrl+X", nullptr, enable))
			{
				Cut();
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_FILES_O"  Copy", "Ctrl+C", nullptr, enable))
			{
				Copy();
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_CLIPBOARD"  Paste", "Ctrl+V", nullptr, enable && CanPaste()))
			{
				Paste();
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_ERASER"  Delete", "Del", nullptr, enable))
			{
				Delete();
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_CROSSHAIRS"  Select All", "Ctrl+A", nullptr, enable))
			{
				SelectAll();
			}
			ImGui::Spacing();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_COMMENTING_O"  Log", nullptr, openLogWindow))
			{
				openLogWindow = !openLogWindow;
			}
			ImGui::Spacing();
			if (ImGui::MenuItem(ICON_FK_MATRIX_ORG"  Token", nullptr, openTokenWindow))
			{
				openTokenWindow = !openTokenWindow;
			}
			ImGui::Spacing();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void GUI::ShowToolBar(VDMParser::Parser& parser)
{
	if (ImGui::BeginMenuBar())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.53f, 0.98f, 1.0f));

		if (ImGui::Button(ICON_FK_FILE_TEXT_O))
		{
			NewInputAndOutput(parser);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("New (Ctrl+N)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}


		if (ImGui::Button(ICON_FK_FOLDER_OPEN_O))
		{
			OpenInputFile(parser);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Open Input file (Ctrl+0)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::Separator();
		std::string save = "Save ";
		std::optional<std::string> fileName;
		if (!parser.InputIsEmpty())
		{
			fileName = GetFileName(parser.GetInputPath());
			if (fileName.has_value())
			{
				save += fileName.value() + " (Ctrl+S)";
			}
		}
		else
		{
			save += "Input file (Ctrl+S)";
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_FLOPPY_O))
		{
			SaveInputFile(parser);
		}
		if (parser.InputIsEmpty())
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text(save.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		save = "Save ";
		if (fileName.has_value())
		{
			save += fileName.value() + " As...(Ctrl+Shift+S)";
		}
		else
		{
			save += "Input file As...(Ctrl+Shift+S)";

		}
		if (inputEditor.GetText().empty())
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_CLONE))
		{
			SaveInputFileAs(parser);
		}
		if (inputEditor.GetText().empty())
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text(save.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}


		if (outputEditor.GetText().empty())
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_CODE))
		{
			SaveOutputFileAs(parser);
		}
		if (outputEditor.GetText().empty())
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Save Output file as (Ctrl+Alt+S)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::Separator();


		bool enable = isInputWindowFocused || isOutputWindowFocused;
		if (!enable)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_SCISSORS))
		{
			Cut();
		}
		if (!enable)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Cut (Ctrl+X)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (!enable)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_FILES_O))
		{
			Copy();
		}
		if (!enable)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Copy (Ctrl+C)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (!(enable && CanPaste()))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FK_CLIPBOARD))
		{
			Paste();
		}
		if (!(enable && CanPaste()))
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Paste (Ctrl+V)");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::Separator();


		if (isInputWindowFocused)
		{
			if (!inputEditor.CanUndo())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button(ICON_FK_UNDO))
			{
				Undo();
			}
			if (!inputEditor.CanUndo())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Undo (Ctrl+Z)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}


			if (!inputEditor.CanRedo())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button(ICON_FK_REPEAT))
			{
				Redo();
			}
			if (!inputEditor.CanRedo())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Redo (Ctrl+Y)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		else if (isOutputWindowFocused)
		{
			if (!outputEditor.CanUndo())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button(ICON_FK_UNDO))
			{
				Undo();
			}
			if (!outputEditor.CanUndo())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Undo (Ctrl+Z)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			if (!outputEditor.CanRedo())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button(ICON_FK_REPEAT))
			{
				Redo();
			}
			if (!outputEditor.CanRedo())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Redo (Ctrl+Y)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		else
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button(ICON_FK_UNDO);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Undo (Ctrl+Z)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button(ICON_FK_REPEAT);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Redo (Ctrl+Y)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		ImGui::Separator();

		if (ImGui::Button(ICON_FK_PLAY" Parse"))
		{
			Parse(parser);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Parse Input file");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::Button(ICON_FK_FORWARD" Build Solution"))
		{
			generate = true;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text("Parse Input file");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);


		ImGui::EndMenuBar();
	}
}

void GUI::ShowBuildWindow(VDMParser::Parser& parser)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Build", NULL, window_flags);
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Parse", ImVec2(120, 0)))
	{
		Parse(parser);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	const char* items[] = { "vs2019", "vs2017", "vs2015", "vs2013", "vs2012"};
	static int item_current = 0;
	ImGui::PushItemWidth(200);
	ImGui::Combo("Build Target", &item_current, items, IM_ARRAYSIZE(items));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	HelpMarker("Choose desired Visual Studio's version.");

	static std::string solutionName;
	static std::string projectName;
	ImGui::PushItemWidth(200);
	ImGui::InputText("Solution Name", &solutionName);
	ImGui::PushItemWidth(200);
	ImGui::InputText("Project Name", &projectName);
	if (ImGui::Button("Build Solution", ImVec2(120, 0)) || generate)
	{
		GenerateSolution(parser, solutionName, projectName, items[item_current]);
		generate = false;
	}
	ImGui::End();

}

void GUI::ShowLogWindow(bool* open)
{
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.5f);
	if (*open)
	{
		ImGui::Begin("Log", open);
		static bool errorFilter = true;
		static bool traceFilter = true;
		static bool infoFilter = true;

		if (errorFilter)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.25f, 0.25f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.25f, 0.25f, 0.9f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 0.8f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.25f, 0.25f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.25f, 0.25f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 0.5f));
		}
		if (ImGui::Button(ICON_FK_EXCLAMATION_TRIANGLE"  Error", ImVec2(100.0f, 0.0f)))
		{
			errorFilter = !errorFilter;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		if (traceFilter)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.7f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.7f, 0.7f, 0.9f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 0.8f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.7f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.7f, 0.7f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 0.5f));
		}

		if (ImGui::Button(ICON_FK_PENCIL_SQUARE"  Trace", ImVec2(100.0f, 0.0f)))
		{
			traceFilter = !traceFilter;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();


		if (infoFilter)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.454f, 0.9f, 0.3f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.454f, 0.9f, 0.3f, 0.9f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.454f, 0.9f, 0.3f, 0.8f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.454f, 0.9f, 0.3f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.454f, 0.9f, 0.3f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.454f, 0.9f, 0.3f, 0.5f));
		}
		if (ImGui::Button(ICON_FK_CHECK_CIRCLE"  Info", ImVec2(100.0f, 0.0f)))
		{
			infoFilter = !infoFilter;
		}		
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		if (ImGui::Button(ICON_FK_TRASH"  Clear", ImVec2(100.0f, 0.0f)))
		{
			Log::Clear();
		}

		ImGui::BeginChild("LogChild", ImVec2(0, 0), true);
		for (Message msg : Log::GetMessages())
		{
			ImGui::PushFont(logFont);

			if (msg.GetMessageLevel() == Message::Level::Error && errorFilter)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
				ImGui::TextWrapped(msg.GetMessageString().c_str());
				ImGui::PopStyleColor();
			}
			else if(msg.GetMessageLevel() == Message::Level::Info && infoFilter)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.454f, 0.9f, 0.3f, 1.0f));
				ImGui::TextWrapped(msg.GetMessageString().c_str());
				ImGui::PopStyleColor();
			}
			else if (msg.GetMessageLevel() == Message::Level::Trace && traceFilter)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
				ImGui::TextWrapped(msg.GetMessageString().c_str());
				ImGui::PopStyleColor();
			}
			ImGui::PopFont();
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();

		ImGui::End();
	}
}

void GUI::ShowTokenWindow(bool* open, VDMParser::Parser& parser)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.5f);
	if (*open)
	{
		ImGui::Begin("Token", NULL, window_flags);
		static ImGuiTableFlags flags = ImGuiTableFlags_Borders 
			| ImGuiTableFlags_RowBg 
			| ImGuiTableFlags_Resizable 
			| ImGuiTableFlags_ContextMenuInBody;
		if (ImGui::BeginTable("TokenTable", 4, flags))
		{
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Value");
			ImGui::TableSetupColumn("Line");
			ImGui::TableHeadersRow();

			for (size_t rowIndex = 0; rowIndex < parser.GetTokens().size(); ++rowIndex)
			{
				ImGui::TableNextRow();
				VDMParser::Token token = parser.GetTokens()[rowIndex];
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%i", rowIndex);

				std::string tokenType;
				if (token.GetType() == VDMParser::TokenType::INDENTIFIER)
				{
					tokenType = "Identifier";
				}
				else if (token.GetType() == VDMParser::TokenType::DOUBLE_LITERAL)
				{
					tokenType = "Double literals";
				}
				else if (token.GetType() == VDMParser::TokenType::INTEGER_LITERAL)
				{
					tokenType = "Integer literals";
				}
				else if (token.GetType() == VDMParser::TokenType::OPERATOR)
				{
					tokenType = "Operator";
				}
				else if (token.GetType() == VDMParser::TokenType::STRING_LITERAL)
				{
					tokenType = "String literals";
				}
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", tokenType.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%s", token.GetText().c_str());

				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%i", token.GetLineNumber());
			}


			ImGui::EndTable();
		}
		ImGui::End();
	}
}

void GUI::ShowDockSpace(VDMParser::Parser& parser)
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		static bool first_time = true;
		if (first_time)
		{
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			ImGuiID dock_log = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id);
			ImGuiID dock_input = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3875f, nullptr, &dockspace_id);
			ImGuiID dock_output = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.6323f, nullptr, &dockspace_id);
			ImGuiID dock_build = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.3f, nullptr, &dockspace_id);

			// we now dock our windows into the docking node we made above
			ImGui::DockBuilderDockWindow("Log", dock_log);
			ImGui::DockBuilderDockWindow("Input", dock_input);
			ImGui::DockBuilderDockWindow("Output", dock_output);
			ImGui::DockBuilderDockWindow("Build", dock_build);
			ImGui::DockBuilderDockWindow("Token", dockspace_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}
	}
	ShowToolBar(parser);
	ImGui::End();
}

void GUI::NewInputAndOutput(VDMParser::Parser& parser)
{
	LOG_TRACE("Application: Clearing Input and Output...");
	{
		Timer timer;
		parser.ClearInput();
		parser.ClearOutput();
		inputEditor.SetText("");
		outputEditor.SetText("");
		timer.Stop();
		LOG_INFO("Application: Success");
		timer.Log();
	}
}

void GUI::SaveInputFile(VDMParser::Parser& parser)
{
	if (!parser.InputIsEmpty())
	{
		std::string path = parser.GetInputPath();
		FindAndReplaceAll(path, "/", "\\");
		LOG_TRACE("Application: Saving {0}...", path);

		Timer timer;
		parser.SetInput(inputEditor.GetText());
		parser.SaveInput();
		timer.Stop();
		LOG_INFO("Application: Success");
		timer.Log();
	}
}

void GUI::SaveInputFileAs(VDMParser::Parser& parser)
{
	std::optional<std::string> path = FileDialogs::SaveFile();

	if (path.has_value())
	{
		LOG_TRACE("Application: Saving Input as {0}...", path.value());

		std::ofstream out(path.value());
		if (out.is_open())
		{
			Timer timer;
			out << inputEditor.GetText();
			out.close();

			if (parser.InputIsEmpty())
			{
				std::string pathcpy = path.value();
				std::string find = "\\";
				std::string replace = "/";
				FindAndReplaceAll(pathcpy, find, replace);
				parser.Read(pathcpy);
				inputEditor.SetText(parser.GetSourceCode());
			}

			timer.Stop();
			LOG_INFO("Application: Success");
			timer.Log();
		}
		else
		{
			LOG_ERROR("Application: Fail");
		}
	}
}

void GUI::SaveOutputFileAs(VDMParser::Parser& parser)
{
	if (!parser.OutputIsEmpty())
	{
		std::optional<std::string> path = FileDialogs::SaveFile();

		if (path.has_value())
		{
			LOG_TRACE("Application: Saving Output as {0}...", path.value());

			std::ofstream out(path.value());
			if (out.is_open())
			{
				Timer timer;
				out << outputEditor.GetText();
				out.close();
				timer.Stop();
				LOG_INFO("Application: Success");
				timer.Log();
			}
			else
			{
				LOG_ERROR("Application: Fail");
			}
		}
	}
}

void GUI::OpenInputFile(VDMParser::Parser& parser)
{
	std::optional<std::string> path = FileDialogs::OpenFile();

	if (path.has_value())
	{
		LOG_TRACE("Application: Opening {0}...", path.value());
		std::string pathcpy = path.value();
		std::string find = "\\";
		std::string replace = "/";
		FindAndReplaceAll(pathcpy, find, replace);

		Timer timer;
		parser.Read(pathcpy);
		inputEditor.SetText(parser.GetSourceCode());
		timer.Stop();
		LOG_INFO("Application: Success");
		timer.Log();
	}
	else
	{
		LOG_ERROR("Application: Fail");
	}
}

void GUI::GenerateSolution(VDMParser::Parser& parser, const std::string& solutionName, const std::string& projectName, const std::string& premakeOption)
{
	if (solutionName.empty())
	{
		LOG_ERROR("Application: Solution Name can not be empty.");
		errorMessage = "Solution Name can not be empty.";
		return;
	}

	if (projectName.empty())
	{
		LOG_ERROR("Application: Project Name can not be empty.");
		errorMessage = "Project Name can not be empty.";
		return;
	}

	if (!parser.InputIsEmpty() && !outputEditor.GetText().empty())
	{
		SaveInputFile(parser);

		if (!parser.OutputIsEmpty())
		{
			std::optional<std::string> path = FileDialogs::SaveFile();

			if (path.has_value())
			{
				LOG_TRACE("Application: Saving Output as {0}...", path.value());

				std::ofstream out(path.value());
				if (out.is_open())
				{
					Timer timer;
					out << outputEditor.GetText();
					out.close();
					timer.Stop();
					LOG_INFO("Application: Success");
					timer.Log();
				}
				else
				{
					LOG_ERROR("Application: Fail");
				}

				LOG_TRACE("Application: Generating '{0}' solution which contains '{1}' project for '{2}' at {3}...", solutionName, projectName, premakeOption, path.value());
				std::string directory = GetDirectory(path.value()).value();

				std::string cmd = "/c xcopy /q /e /y /i resources\\premake " + directory;

				LOG_TRACE("Application: Copying premake5 requirements to {0}...", path.value());
				SHELLEXECUTEINFOA ShExecInfo = { 0 };
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShExecInfo.hwnd = NULL;
				ShExecInfo.lpVerb = "open";
				ShExecInfo.lpFile = "cmd";
				ShExecInfo.lpParameters = cmd.data();
				ShExecInfo.lpDirectory = NULL;
				ShExecInfo.nShow = SW_HIDE;
				ShExecInfo.hInstApp = NULL;
				ShellExecuteExA(&ShExecInfo);

				if (ShExecInfo.hProcess != 0)
				{
					Timer timer;
					WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
					timer.Stop();
					LOG_INFO("Application: Success");
					timer.Log();
				}
				else
				{
					LOG_ERROR("Application: Fail");
				}
				CloseHandle(ShExecInfo.hProcess);

				VDMParser::InputReader premakeReader;

				std::string directoryCpy = directory;
				FindAndReplaceAll(directoryCpy, "\\", "/");

				std::string premakePath = directoryCpy + std::string("/premake5.lua");
				std::string genProjectBatPath = directoryCpy + std::string("/GenProject.bat");

				LOG_TRACE("Application: Adjusting the contents of premake5 requirements at {0}...", path.value());
				{
					Timer timer;
					premakeReader.Read(premakePath.c_str());
					FindAndReplaceAll(premakeReader.GetSourceCode(), "{SolutionName}", solutionName);
					FindAndReplaceAll(premakeReader.GetSourceCode(), "{ProjectName}", projectName);
					premakeReader.Save();
					premakeReader.Clear();

					premakeReader.Read(genProjectBatPath.c_str());
					FindAndReplaceAll(premakeReader.GetSourceCode(), "{option}", premakeOption);
					premakeReader.Save();
					premakeReader.Clear();
					timer.Stop();
					LOG_INFO("Application: Success");
					timer.Log();
				}

				FindAndReplaceAll(directoryCpy, "/", "\\");

				LOG_INFO("Application: Execute 'GenProject.bat' file at {0}", path.value());
				ShellExecuteA(NULL, "open", "GenProject.bat", NULL, directoryCpy.data(), SW_HIDE);
			}
			else
			{
				LOG_ERROR("Application: Fail");
			}

		}
		else
		{
			LOG_ERROR("Application: Require parsed Output.");
			errorMessage = "Require parsed Output.";
		}
	}
	else
	{
		LOG_ERROR("Application: Require Input file and parsed Output.");
		errorMessage = "Require Input file and parsed Output.";
	}
}

void GUI::ShowErrorMessageModal()
{
	if (!errorMessage.empty())
	{
		ImGui::OpenPopup("Error!");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Error!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushFont(modalMessageFont);
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMessage.c_str());
			ImGui::PopFont();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 50.0f);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				errorMessage = "";
			}
			ImGui::EndPopup();
		}
	}
}

void GUI::Parse(VDMParser::Parser& parser)
{
	if (!inputEditor.GetText().empty())
	{
		if (!parser.InputIsEmpty())
		{
			SaveInputFile(parser);
		}
		else
		{
			SaveInputFileAs(parser);
		}

		LOG_TRACE("Application: Parsing...");
		Timer timer;
		std::optional<std::string> output = parser.Parse();
		timer.Stop();
		if (!output.has_value())
		{
			LOG_ERROR("Application: Fail");
			for (size_t index = 0; index < VDMParser::ErrorHandler::GetErrors().size(); ++index)
			{
				LOG_ERROR("{0}: {1} Line number {2}.", VDMParser::ErrorHandler::GetErrors()[index].GetModuleName(),
					VDMParser::ErrorHandler::GetErrors()[index].GetMessageLog(),
					std::to_string(VDMParser::ErrorHandler::GetErrors()[index].GetLineNumber()));
			}
			errorMessage = "Parsing Error!";
			VDMParser::ErrorHandler::Clear();
			parser.ClearOutput();
		}
		else
		{
			LOG_INFO("Application: Success");
			timer.Log();
			outputEditor.SetText(output.value());
		}
	}
	else
	{
		LOG_ERROR("Application: Require Input file or contents of Input.");
		errorMessage = "Require Input file or contents of Input.";

	}
}

void GUI::Undo()
{
	if (isInputWindowFocused)
	{
		inputEditor.Undo();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Undo();
	}
}

void GUI::Redo()
{
	if (isInputWindowFocused)
	{
		inputEditor.Redo();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Redo();
	}
}

void GUI::Copy()
{
	if (isInputWindowFocused)
	{
		inputEditor.Copy();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Copy();
	}
}

void GUI::Cut()
{
	if (isInputWindowFocused)
	{
		inputEditor.Cut();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Cut();
	}
}

void GUI::Paste()
{
	if (isInputWindowFocused)
	{
		inputEditor.Paste();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Paste();
	}
}

void GUI::Delete()
{
	if (isInputWindowFocused)
	{
		inputEditor.Delete();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.Delete();
	}
}

void GUI::SelectAll()
{
	if (isInputWindowFocused)
	{
		inputEditor.SelectAll();
	}
	else if (isOutputWindowFocused)
	{
		outputEditor.SelectAll();
	}
}

bool GUI::CanPaste()
{
	auto clipText = ImGui::GetClipboardText();
	if (clipText != nullptr && strlen(clipText) > 0)
	{
		return true;
	}
	return false;
}

void FindAndReplaceAll(std::string& input, const std::string& search, const std::string& replace)
{
	std::size_t foundPos = 0;
	while ((foundPos = input.find(search, foundPos)) != std::string::npos)
	{
		input.replace(foundPos, search.length(), replace);
		foundPos += replace.length();
	}
}

std::optional<std::string> GetFileName(const std::string& path, char seperator)
{
	size_t index = path.rfind(seperator, path.length());
	if (index != std::string::npos)
	{
		return(path.substr(index + 1, path.length() - index));
	}

	return {};
}

std::optional<std::string> GetDirectory(const std::string& path)
{
	char seperator = '\\';

	size_t index = path.rfind(seperator, path.length());
	if (index != std::string::npos)
	{
		return(path.substr(0, index));
	}
	return {};
}

void GUI::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}