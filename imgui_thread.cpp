#include "include/sysmon_and_load.h"

#include "include/imgui_thread.h"

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include <sstream>
#include <string>
#include <vector>
//#include <sys/sysinfo.h>
//#include <sys/statvfs.h>

///////////////////////////////////////////////////////////////////////////////////
int screen_width;
int screen_height;

#include <cstdio>

// Function to send a message from the ImGui thread
void imguiSymonLoadSendMessage(MessageCntrl_s& msgCtrl, MessageImguiSysmonLoad message) {
  std::lock_guard<std::mutex> lock(msgCtrl.mtx);
  msgCtrl.messageQueue.push(message);
  msgCtrl.cv.notify_one();
}

// Function to receive a message from the main thread
MessageImguiSysmonLoad imguiSymonLoadReceiveMessage(MessageCntrl_s& msgCtrl) {
  std::unique_lock<std::mutex> lock(msgCtrl.mtx);
  msgCtrl.cv.wait(lock, [&msgCtrl] { return !msgCtrl.messageQueue.empty(); });
  MessageImguiSysmonLoad message = msgCtrl.messageQueue.front();
  msgCtrl.messageQueue.pop();
  return message;
}


//
// Main code
int imguiSymonLoad(MessageCntrl_s& msgCtl)
{
     glfwSetErrorCallback(glfw_error_callback);
     if (!glfwInit()) return 1;

     glfw_error_callback(-1, "this is a test\n");

     // Create window with graphics context
     char sysmon_version[33];
     sprintf(sysmon_version, "sysmon v%s", VERSION);
     GLFWwindow* window = glfwCreateWindow(XVIEW, YVIEW, sysmon_version, nullptr, nullptr);
     if (window == nullptr)
          return 1;
     glfwMakeContextCurrent(window);
     glfwSwapInterval(1); // Enable vsync

     // Setup Dear ImGui context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

     //ImGui::StyleColorsDark();
     //ImGui::StyleColorsClassic();
     //ImGuiStyle& style = ImGui::GetStyle();
     //style.ScaleAllSizes(2);
     ImGui::StyleColorsLight();
     //ImGuiStyle& style = ImGui::GetStyle();

     // Setup Platform/Renderer backends
     ImGui_ImplGlfw_InitForOpenGL(window, true); // @suppress("Invalid arguments")
     ImGui_ImplOpenGL2_Init(); // @suppress("Invalid arguments")

     K3Buffer* Buffer = new K3Buffer(BUFFER_SIZE);
     K3System* System = new K3System();
     K3Key showin(WIN_MAX);

     bool histogramode = false;
     bool jump = false;
     bool quit = false;
     bool launchImguiDemo = false;
     bool launchLvglDemo = false;
     bool reset = false;

     int procimax = 0;
     int proci;
     int loop = 0;
     int uloop = 0;
     int delay = 1;

     float buftime;
     float font_scale = 1.58;

     const char* status = "unknown";

     ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
     ImGuiWindowFlags controlWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
     
     int allocMBytes = 0;
     int endlessCalcThreads = 0;
     int numPiCalcTasks = 0;
     bool triggerPiTasks = false;
     MessageImguiSysmonLoad message;
     message.quitFlag = false;
     message.switchToImguiDemo = false;

//     ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_SizingFixedFit;
     // Main loop
     while (!glfwWindowShouldClose(window))
     {
          loop++;

          glfwPollEvents();

          // Start the Dear ImGui frame
          ImGui_ImplOpenGL2_NewFrame(); // @suppress("Invalid arguments")
          ImGui_ImplGlfw_NewFrame(); // @suppress("Invalid arguments")
          ImGui::NewFrame();

          io.FontGlobalScale = font_scale;


          if (ImGui::IsKeyPressed(ImGuiKey_A)) showin.flip(WIN_ABOUT);
          // if (ImGui::IsKeyPressed(ImGuiKey_B)) showin.flip(WIN_DEBUG);
          if (ImGui::IsKeyPressed(ImGuiKey_C)) showin.flip(WIN_CONTROL);          
          if (ImGui::IsKeyPressed(ImGuiKey_M)) histogramode = !histogramode;
          if (ImGui::IsKeyPressed(ImGuiKey_L)) showin.flip(WIN_LOAD_TEST);
          if (ImGui::IsKeyPressed(ImGuiKey_T)) showin.flip(WIN_ABOUT_TEST);

          if (ImGui::IsKeyPressed(ImGuiKey_Q)) quit = true;
          if (ImGui::IsKeyPressed(ImGuiKey_I)) launchImguiDemo = true;
          if (ImGui::IsKeyPressed(ImGuiKey_V)) launchLvglDemo = true;

          if (ImGui::IsKeyPressed(ImGuiKey_R)) reset = true;
               
          delay = delay > 0 ? delay : 1;
          jump = loop % delay;
          if (!jump)
          {
               uloop++;
               //System->connect();
               System->get_sysinfo("totalmem", "freemem", "uptime", "procs");
               System->get_statvfs("totalspace", "freespace");
               System->processor("cpunumber");
               // System->connect("cpufreq", "/proc/cpuinfo", "cpu MHz");
               System->connect("procstat", "/proc/stat");
               System->connect("procloadavg", "/proc/loadavg");
               //unsigned int cpufreq0 = System->node_cpufreq_stats(1);
               //glfw_error_callback(cpufreq0, "\n");

               if (uloop < BUFFER_SIZE) status = "load";
               else if (uloop == BUFFER_SIZE) status = "done";
               else status = "run";

               // if (strcmp("done", status) == 0) delay = 6;
               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;

               buftime = float(BUFFER_SIZE) / upfreq;
                              
               proci = System->back("procloadavg", 3);
               procimax = proci > procimax ? proci : procimax;
               
               // Buffer->fill("cpufreq", System->back("cpufreq"));
               Buffer->fill("freemem", 100 * System->back("freemem") / System->back("totalmem"));
               Buffer->fill("procs", System->back("procs"));
               Buffer->fill("upfreq", upfreq);
               Buffer->fill("appfreq", appfreq);
               Buffer->fill("loadavg0", System->back("procloadavg", 0));
               Buffer->fill("loadavg1", System->back("procloadavg", 1));
               Buffer->fill("loadavg2", System->back("procloadavg", 2));
               Buffer->fill("loadavg3", System->back("procloadavg", 3));
          }

          const ImGuiViewport* viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          screen_width = viewport->WorkSize.x;
          screen_height = viewport->WorkSize.y;

          bool  boopen = true;
          ImGui::Begin("main", &boopen, mainWindowFlags);

    // IMGUI_API bool          ::Button(const char* label, const ImVec2& size = ImVec2(0, 0));   // button
    // IMGUI_API bool          SmallButton(const char* label);                                 // button with (FramePadding.y == 0) to easily embed within text

          if (ImGui::Button("[A]BOUT ")) showin.flip(WIN_ABOUT); // showin.show(WIN_ABOUT, winStatAbout);
          ImGui::SameLine();
          // if (ImGui::Button("DE[B]UG ")) showin.flip(WIN_DEBUG); // showin.show(WIN_DEBUG, winStatDebug);
          if (ImGui::Button("[C]ONTROL ")) showin.flip(WIN_CONTROL); // showin.show(WIN_CONTROL, winStatContr);
          ImGui::SameLine();          
          if (ImGui::Button("[M]ODE ")) histogramode = !histogramode;
          ImGui::SameLine();
          if (ImGui::Button("[R]ESET ")) reset = true;
          ImGui::SameLine();
          if (ImGui::Button("[L]OAD TEST ")) showin.flip(WIN_LOAD_TEST); // showin.show(WIN_LOAD_TEST, true);
          ImGui::SameLine();
          if (ImGui::Button("[I]MGUI DEMO ")) launchImguiDemo = true;
          ImGui::SameLine();
          if (ImGui::Button("L[V]GL DEMO ")) launchLvglDemo = true;
          ImGui::SameLine();          
          if (ImGui::Button("[Q]UIT ")) quit = true;


          ImGui::Separator();

          bool histogram = true; 
          bool history = false; 
          if (histogramode)
          {
               draw(Buffer, "procs",     "total processes",   "",    histogram);
               draw(Buffer, "loadavg3",  "running processes", "",    histogram);
               // draw(Buffer, "cpufreq",   "cpu frequence",     "MHz", histogram);
               draw(Buffer, "appfreq",   "imgui frequence",   "Hz",  histogram);
               draw(Buffer, "upfreq",    "app frequence",     "Hz",  histogram);               
               draw(Buffer, "freemem",   "free RAM",       "%",   histogram);
               drawSysSummary(System);
          }
          else
          {
               draw(Buffer, "procs",     "total processes",   "",    history);
               draw(Buffer, "loadavg3",  "running processes", "",    history);
               // draw(Buffer, "cpufreq",   "cpu frequence",     "MHz", history);
               draw(Buffer, "appfreq",   "imgui frequence",   "Hz",  history);
               draw(Buffer, "upfreq",    "app frequence",     "Hz",  history);
               draw(Buffer, "freemem",   "free RAM",       "%",   history);
               drawSysSummary(System);
          }


          ImGui::End();
          

          /*if (showin.status(WIN_DEBUG) && ImGui::Begin("debug", showin.is(WIN_DEBUG), controlWindowFlags))
          {
               ImGui::SeparatorText("debug");
               ImGui::Text("loadavg: %s", System->node("procloadavg")->text);
               ImGui::Text("stat: %s", System->node("procstat")->text);
               ImGui::End();
          }*/

          if (showin.status(WIN_CONTROL) && ImGui::Begin("control", showin.is(WIN_CONTROL), controlWindowFlags))
          {
               if (ImGui::Button("[c]lose")) showin.hide();

               ImGui::SeparatorText("");
                    
               char bufoverlay[33];
               sprintf(bufoverlay, "history %10.0f seconds", buftime);
               ImGui::ProgressBar(buftime / 1111, ImVec2(0.0f, 0.0f), bufoverlay);
               ImGui::SeparatorText("control");
               ImGui::SliderInt("update time", &delay, 1, 20);
               ImGui::Separator();
               ImGui::Separator();
               ImGui::SliderFloat("font scale", &font_scale, 0.5, 5);
               ImGui::Separator();
               ImGui::ShowStyleSelector("color style");
               ImGui::Separator();
               ImGui::End();
          }


          if (showin.status(WIN_LOAD_TEST) && ImGui::Begin("load", showin.is(WIN_LOAD_TEST), controlWindowFlags))
          {
               if (ImGui::Button("[a]bout")) showin.show(WIN_ABOUT_TEST, true);
               ImGui::SameLine();
               if (ImGui::Button("[c]lose")) showin.hide();
               ImGui::SameLine();
               if (ImGui::Button("[r]eset"))
               {
                    Buffer->reset();
                    uloop = 0;
                    delay = 1;
                    reset = false;
               }

               ImGui::SeparatorText("");
                    
               char bufoverlay[33];
               sprintf(bufoverlay, "history %10.0f seconds", buftime);
               ImGui::ProgressBar(buftime / 1111, ImVec2(0.0f, 0.0f), bufoverlay);
               ImGui::SeparatorText("control");
               ImGui::SliderInt("update time", &delay, 1, 100);
               ImGui::Separator();
               ImGui::Separator();
               ImGui::SliderFloat("font scale", &font_scale, 0.5, 5);
               ImGui::Separator();
               ImGui::ShowStyleSelector("color style");
               ImGui::SeparatorText("LOAD TEST");

               ImGui::InputIntPositive("allocMBytes", &allocMBytes, 1, ImGuiInputTextFlags_EnterReturnsTrue);
               ImGui::InputIntPositive("endlessCalcThreads", &endlessCalcThreads, 1, ImGuiInputTextFlags_EnterReturnsTrue);
               ImGui::InputIntPositive("numPiCalcTasks", &numPiCalcTasks, 10, ImGuiInputTextFlags_EnterReturnsTrue);
               
               if (ImGui::Button("triggerPiTasks")) {
                    triggerPiTasks = true;
               }

               message.endlessCalcThreads = (unsigned int) endlessCalcThreads;
               message.allocMbytes = (unsigned int) allocMBytes;
               message.numPiCalcTasks = (unsigned int) numPiCalcTasks;
               message.triggerPiTasks = triggerPiTasks;               
               triggerPiTasks = false;

               imguiSymonLoadSendMessage(msgCtl, message);

               ImGui::End();
          }

          if (showin.status(WIN_ABOUT) && ImGui::Begin("about", showin.is(WIN_ABOUT), controlWindowFlags))
          {
               ImGui::Text("%s", sysmon_version);
               ImGui::SeparatorText("code");
               ImGui::Text("%s", System->author());
               ImGui::SeparatorText("powered by");
               ImGui::Text("ImGui, GLFW, OpenGL, X11");

               ImGui::SeparatorText("copyleft 2023-2024");
               ImGui::Text("triplehelix-consulting.com");
               ImGui::Separator();
               if (ImGui::Button("[a]ll right")) showin.show(WIN_ABOUT, false);
               ImGui::End();
          }


          if (showin.status(WIN_ABOUT_TEST) && ImGui::Begin("about test", showin.is(WIN_ABOUT_TEST), controlWindowFlags))
          {
               ImGui::Text("%s", sysmon_version);
               ImGui::SeparatorText("code");
               ImGui::Text("Atanas Georgiev Rusev");
               ImGui::SeparatorText("powered by");
               ImGui::Text("ImGui");
               ImGui::SameLine();
               ImGui::Text("GLFW");
               ImGui::SameLine();
               ImGui::Text("OpenGL");
               ImGui::SameLine();
               ImGui::Text("X11");
               ImGui::SeparatorText("copyleft 2023-2024");
               ImGui::Text("triplehelix-consulting.com");
               ImGui::Separator();
               if (ImGui::Button("[a]ll right")) showin.show(WIN_ABOUT_TEST, false);
               ImGui::End();
          }


// Rendering
          ImGui::Render();
          int display_w, display_h;
          glfwGetFramebufferSize(window, &display_w, &display_h);
          glViewport(0, 0, display_w, display_h);
          //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
          glClear(GL_COLOR_BUFFER_BIT);

          ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData()); // @suppress("Invalid arguments")

          glfwMakeContextCurrent(window);
          glfwSwapBuffers(window);
          //if (ImGui::End(ImGuiKey_Q)) quit = true;
          //if (ImGui::SmallButton("[q]uit")) quit = true;

          if (quit || launchLvglDemo || launchImguiDemo)
          {
               glfwSetWindowShouldClose(window, 1);
          }
          else if (reset)
          {
               Buffer->reset();
               uloop = 0;
               delay = 1;
               showin.hide();
               reset = false;
          }
          else
          {}
     }

     // Cleanup

     
     message.triggerPiTasks = false;
     
     if (quit) {
          message.quitFlag = true;
     } else {
          if (launchLvglDemo) {
               message.switchToLvglDemo = true;
          } else {
               if (launchImguiDemo) {
                    message.switchToImguiDemo = true;
               } else {
                    // failsafe!!
                    std::cout << "FATAL ERROR : FAILSAFE activated in int imguiSymonLoad(MessageCntrl_s& msgCtl)" << std::endl;
                    message.quitFlag = true;
                    quit = true;
               }
          }
     }   
     
     
     imguiSymonLoadSendMessage(msgCtl, message);

     delete System;
     delete Buffer;

     ImGui_ImplOpenGL2_Shutdown(); // @suppress("Invalid arguments")
     ImGui_ImplGlfw_Shutdown(); // @suppress("Invalid arguments")
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     if (quit) glfwTerminate();

     return 0;
}

static ImVec2 calcGraphSize(void)
{
     int* width = &screen_width;
     int* height = &screen_height;
     ImVec2 p(*width - LEFT_PLUS_RIGHT_HORIZ_MARGIN, *height / (TOTAL_GRAPHS+TOTAL_DATA_ROWS+1));
     return p;
}

static void draw(K3Buffer* objbuf, const char* name,
                 const char* title, const char* siunit,
                 bool mode)
{
     if (mode) spacePlot(objbuf, name, title, siunit);
     else timePlot(objbuf, name, title, siunit);    

}

static void drawSysSummary(K3System* System){

               ImGui::Separator();
               ImGui::Separator();
               // I need: Free storage, uptime
               // System->back("freemem") / 
               // Example usage (assuming a buffer of sufficient size)
               float totMemFloat = (System->back("totalmem"))/(1024*1024);

               static const int STR_SIZE_16 = 16;
               char strToDisplay[STR_SIZE_16];
               int chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.0f", totMemFloat);

               ImVec4 DarkBlue (0.2f, 0.2f, 0.8f, 1.0f);
               ImVec4 DarkRed (0.6f, 0.1f, 0.1f, 1.0f);
               ImVec4 DarkOrange (0.8f, 0.5f, 0.2f, 1.0f);
               ImVec4 DarkGreen (0.1f, 0.5f, 0.1f, 1.0f);

               ImGui::TextColored(DarkBlue, "TOTAL RAM:");ImGui::SameLine();
               ImGui::Button(strToDisplay); ImGui::SameLine();
               ImGui::TextColored(DarkBlue, " MegaBytes; FREE: "); ImGui::SameLine();
               
               float freeMemFloat = (System->back("freemem"))/(1024*1024);

               chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.0f", freeMemFloat);
               ImGui::Button(strToDisplay); ImGui::SameLine();
               ImGui::TextColored(DarkBlue, " IN USE: "); ImGui::SameLine();
               float occupiedMemFloat = totMemFloat - freeMemFloat;
               chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.0f", occupiedMemFloat);
               ImGui::Button(strToDisplay); 

               ImGui::Separator();
               ImGui::Separator();
               float uptime = System->back("uptime");

               chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.0f", uptime);

               ImGui::TextColored(DarkBlue, "UPTIME:");ImGui::SameLine();
               ImGui::Button(strToDisplay); ImGui::SameLine();
               ImGui::TextColored(DarkBlue, " Sec Since Boot; in Minutes:"); ImGui::SameLine();
               uptime /= 60;
               chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.2f", uptime);
               ImGui::Button(strToDisplay);  ImGui::SameLine();
               ImGui::TextColored(DarkBlue, " in Hours:"); ImGui::SameLine();
               uptime /= 60;
               chars_written = snprintf(strToDisplay, STR_SIZE_16, "%.2f", uptime);
               ImGui::Button(strToDisplay); 

               ImGui::Separator();
               // CPU frequency per core
}

static void spacePlot(K3Buffer* objbuf, const char* name,
                   const char* title = "", const char* siunit = "")
{
     std::vector<float> hist(HISTOGRAM_SIZE, 0);
     float hmin, hmax, hmean, hstdev, bmin, bmax, cur;

     objbuf->build(name, &hist, &hmin, &hmax, &hmean, &hstdev, &bmin, &bmax, &cur);
     const char* overlay = objbuf->overtextHist(title, cur, hmean, hstdev, siunit, bmin, bmax);

     ImGui::PlotHistogram("", hist.data(), HISTOGRAM_SIZE, 0, overlay, hmin, hmax, calcGraphSize());
}

static void timePlot(K3Buffer* objbuf, const char* name,
                 const char* title = "", const char* siunit = "")
{
     Feature* ens = objbuf->node(name);
     std::vector<float>* plotBuf = ens->buffer;
     float min = objbuf->min(plotBuf);
     float max = objbuf->max(plotBuf);
     float cur = plotBuf->back();
     size_t size = plotBuf->size();
     float* data = plotBuf->data();
     const char* overlay = objbuf->overtextTime(title, cur, min, max, siunit, ens->mini, ens->maxi);
     
     ImGui::PlotLines("", data, size, 0, overlay, min, max, calcGraphSize());
}

static void glfw_error_callback(int error, const char* description)
{
     printf("glfw %d %s\n", error, description);
}