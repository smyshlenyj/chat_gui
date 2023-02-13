#include <filesystem>
#include "UI.h"
#include "Users.h"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int UI(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Stack - past generation messenger", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	GLFWimage images[1];
	images[0].pixels = stbi_load("ico.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);
#endif

#ifdef __linux__
	std::string home = getenv("HOME");
	std::string path = home + "/.stackmessenger/ico.png";
	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);
#endif

	bool programAlive = true;
	bool showMainMenuWindow = true;
	bool showDemoWindow = false;
	bool showSignUpWindow = false;
	bool showSignInWindow = false;
	bool showUsersWindow = false;
	bool showMessageWindow = false;
	bool signUpModalWindow = false;
	bool signInModalWindow = false;

	bool loggedIn = false;

	static char login[64] = "";
	static char password[64] = "";
	static char userName[64] = "";
	static char message[256] = "";

	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec2 windowSize = ImVec2(400.0f, 220.0f);
	ImVec2 usersWindowSize = ImVec2(500.0f, 720.0f);
	ImVec2 messageWindowSize = ImVec2(500.0f, 720.0f);
	User user;
	User selectedRecepient;
	Users usersDB = Users();
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;

	ImGuiWindowFlags messageWindowFlags = 0;
	messageWindowFlags |= ImGuiWindowFlags_NoCollapse;
	messageWindowFlags |= ImGuiWindowFlags_NoResize;
	messageWindowFlags |= ImGuiWindowFlags_NoMove;

	int avatarImageWidth = 60;
	int avatarImageHeight = 60;
	GLuint avatarImageTexture = 0;

	bool ret = false;
	ret = LoadTextureFromFile("avatar.jpg", &avatarImageTexture, &avatarImageWidth, &avatarImageHeight);
	IM_ASSERT(ret);

	int backgroundImageWidth = 1280;
	int backgroundImageHeight = 720;
	GLuint backgroundTexture = 0;

	ret = LoadTextureFromFile("background.jpg", &backgroundTexture, &backgroundImageWidth, &backgroundImageHeight);
	IM_ASSERT(ret);

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	ImVec2 topLeft = ImVec2(0.0f, 0.0f);
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	// Main loop
	while (!glfwWindowShouldClose(window) && programAlive)
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		auto MAIN_WINDOW = ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar;

		glfwGetWindowSize(window, &backgroundImageWidth, &backgroundImageHeight);
		ImVec2 size(backgroundImageWidth, backgroundImageHeight);
		ImGui::SetNextWindowPos(ImVec2());
		ImGui::SetNextWindowSize(size);
		if (ImGui::Begin("1", nullptr, MAIN_WINDOW))
		{
			ImGui::Image((void*)(intptr_t)backgroundTexture, ImVec2(backgroundImageWidth, backgroundImageHeight));
			ImGui::End();
		}

		if (showMainMenuWindow)
		{
			ImGui::SetNextWindowPos(topLeft, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_None);
			ImGui::Begin("Main menu", NULL, windowFlags); // Create a window called "Hello, world!" and append into it.
			ImGui::Text("Welcome to Stack, past generation messenger!");
			ImGui::Text("");

			if (loggedIn == true)
			{
				ImGui::Text("You are logged in as:");
				ImGui::SameLine();
				ImGui::Text(user.getLogin().c_str());


				if (ImGui::Button("Back to messages"))   // Buttons return true when clicked (most widgets return true when edited/activated)
				{
					showSignInWindow = false;
					showMainMenuWindow = false;
					showUsersWindow = true;
					showMessageWindow = true;
				}

				if (ImGui::Button("Sign out"))   // Buttons return true when clicked (most widgets return true when edited/activated)
				{
					loggedIn = false;
					showSignInWindow = false;
					showMainMenuWindow = true;
					showUsersWindow = false;
					showMessageWindow = false;
				}
			}

			if (loggedIn == false)
			{
				ImGui::Text("Please sign in or sign up:");
				ImGui::Text("");

				if (ImGui::Button("Sign in"))   // Buttons return true when clicked (most widgets return true when edited/activated)
				{
					showSignInWindow = true;
					showMainMenuWindow = false;
				}

				if (ImGui::Button("Sign up"))   // Buttons return true when clicked (most widgets return true when edited/activated)
				{
					showSignUpWindow = true;
					showMainMenuWindow = false;
				}
			}
			ImGui::Text("");

			if (ImGui::Button("Exit"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				programAlive = false;
			//	ImGui::Checkbox("Demo Window", &show_demo_window); 
			ImGui::Text("OS Version: ");
			//ImGui::Text(getOSVersion().c_str());

			ImGui::End();
		}

		if (showSignUpWindow)
		{
			if (signUpModalWindow)
				ImGui::OpenPopup("Warning!");

			// Always center this window when appearin
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Warning!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (strcmp(login, "_all") == 0)
					ImGui::Text("Please don't use \"_all\" as login, it is hardcoded!");
				if (strcmp(password, "") == 0)
					ImGui::Text("Password cannot be empty!");
				if (strcmp(login, "") == 0)
					ImGui::Text("Login cannot be empty!");
				if (strcmp(userName, "") == 0)
					ImGui::Text("User name cannot be empty!");
				if (!usersDB.uniqueLogin(std::string(login)))
					ImGui::Text("User %s already exists!", login);

				ImGui::Separator();

				if (ImGui::Button("I understand", ImVec2(120, 0)))
				{
					signUpModalWindow = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SetItemDefaultFocus();
				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(topLeft, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_None);

			ImGui::Begin("Sign up", NULL, windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("Login:    ");
			ImGui::SameLine();
			ImGui::InputText("##login", login, 64);

			ImGui::Text("Password: ");
			ImGui::SameLine();
			ImGui::InputText("##password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

			ImGui::Text("User name:");
			ImGui::SameLine();
			ImGui::InputText("##userName", userName, 64);

			ImGui::Text("");

			if (ImGui::Button("Create user"))
			{
				if (strcmp(login, "_all") == 0 ||
					strcmp(login, "") == 0 ||
					strcmp(password, "") == 0 ||
					strcmp(userName, "") == 0 ||
					!usersDB.uniqueLogin(std::string(login)))
					signUpModalWindow = true;
				else
				{
					user.setLogin(login);
					user.setPassword(password);
					user.setUserName(userName);
					loggedIn = true;

					usersDB.addUser(user);

					showUsersWindow = true;
					showSignUpWindow = false;

					// Clear the buffer
					strncpy(login, "", 64);
					strncpy(password, "", 64);
					strncpy(userName, "", 64);
				}
			}

			if (ImGui::Button("Exit to main menu"))
			{
				showSignUpWindow = false;
				showMainMenuWindow = true;
			}

			ImGui::End();
		}

		if (showSignInWindow)
		{
			if (signInModalWindow)
				ImGui::OpenPopup("Warning!");

			// Always center this window when appearing
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Warning!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (strcmp(password, "") == 0)
					ImGui::Text("Password cannot be empty!");
				if (strcmp(login, "") == 0)
					ImGui::Text("Login cannot be empty!");
				if (!usersDB.loginAndPasswordMatch(login, password))
					ImGui::Text("Error. No such login + password combination!");

				ImGui::Separator();

				if (ImGui::Button("I understand", ImVec2(120, 0)))
				{
					signInModalWindow = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SetItemDefaultFocus();
				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(topLeft, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_None);

			ImGui::Begin("Sign in", NULL, windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("Login:    ");
			ImGui::SameLine();
			ImGui::InputText("##login", login, 64);

			ImGui::Text("Password: ");
			ImGui::SameLine();
			ImGui::InputText("##password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

			ImGui::Text("");

			if (ImGui::Button("Sign in"))
			{
				if (strcmp(login, "_all") == 0 ||
					strcmp(login, "") == 0 ||
					strcmp(password, "") == 0 ||
					!usersDB.loginAndPasswordMatch(login, password))
					signInModalWindow = true;
				else
				{
					user.setLogin(login);
					user.setPassword(password);
					user.setUserName(usersDB.findUserNameByLogin(login));
					loggedIn = true;
					showSignInWindow = false;
					showUsersWindow = true;

					// Clear the buffer
					strncpy(login, "", 64);
					strncpy(password, "", 64);
				}
			}

			if (ImGui::Button("Exit to main menu"))
			{
				showSignInWindow = false;
				showMainMenuWindow = true;
			}
			ImGui::End();
		}

		if (showUsersWindow)
		{
			ImGui::SetNextWindowSize(usersWindowSize, ImGuiCond_None);
			ImGui::SetNextWindowPos(topLeft, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Users", NULL, windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Registered users:");
			ImGui::Text("");

			Users usersDB = Users();

			ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(22, 21, 23));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(200, 200, 200));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(200, 200, 200));
			if (ImGui::Button("PUBLIC GROUP"))
			{
				User groupUser("_all");
				selectedRecepient = groupUser;
				showMessageWindow = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			int i = 0;
			for (auto element : usersDB.listOfUsers())
			{
				if (user.getLogin() != element.getLogin())
				{
					ImGui::PushID(i);
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(22, 21, 23));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(200, 200, 200));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(200, 200, 200));
					if (ImGui::Button(element.getLogin().c_str()))
					{
						selectedRecepient = element;
						showMessageWindow = true;
					}
					ImGui::PopStyleColor(3);
					ImGui::PopID();
					++i;
				}
			}

			ImGui::Text("");
			if (ImGui::Button("Exit to main menu"))
			{
				//selectedRecepient = User();
				showUsersWindow = false;
				showMessageWindow = false;
				showMainMenuWindow = true;
			}
			ImGui::End();
		}

		if (showMessageWindow)
		{
			ImGui::SetNextWindowSize(usersWindowSize, ImGuiCond_None);
			ImGui::SetNextWindowPos(ImVec2(500.0f, 0.0f), ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Messages", NULL, windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Image((void*)(intptr_t)avatarImageTexture, ImVec2(avatarImageWidth, avatarImageHeight));
			ImGui::SameLine();
			ImGui::Text(selectedRecepient.getLogin().c_str());
			ImGui::Text("");
			ImGui::Separator();
			ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 1.0f, 500), false, windowFlags);

			Chat currentChat = Chat(user.getLogin(), selectedRecepient.getLogin());
			for (auto const& i : currentChat.listOfMessages())
				ImGui::TextWrapped(i.c_str());

			ImGui::EndChild();
			ImGui::Separator();
			ImGui::Text("");
			ImGui::Text("Input your message: ");
			ImGui::SameLine();
			ImGui::InputText("##message", message, 256, ImGuiInputTextFlags_AlwaysOverwrite);
			ImGui::Text("");
			if (ImGui::Button("Send msg"))
			{
				if (strcmp(message, "") != 0)
				{
					Message reply = Message(user.getLogin(), selectedRecepient.getLogin(), message);
					currentChat.sendMessage(reply);

					// Clear the buffer
					strncpy(message, "", 256);
				}
			}
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}