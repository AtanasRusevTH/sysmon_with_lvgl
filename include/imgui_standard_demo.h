
// #pragma once
#ifndef IMGUI_STANDARD_DEMO_H
#define IMGUI_STANDARD_DEMO_H


#include <mutex>
#include <condition_variable>
#include <queue>

// Define a message structure
struct MessageImguiDemo {
  bool quitFlag;
  bool switchToLvglDemo;
  bool switchToSysmonImgui;
};

// Define a message structure
struct MessageCntrlImguiDemo_s {
    // Shared state between threads
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<MessageImguiDemo> messageQueue;
};

#define SWITCH_TO_SYSMON 0
#define SWITCH_TO_LVGL 1


void ImGuiDemoSendMessage(MessageCntrlImguiDemo_s& msgCtrl, MessageImguiDemo message);
MessageImguiDemo MainThreadImguiDemoReceiveMessage(MessageCntrlImguiDemo_s& msgCtrl);
int imguiStandardDemo(MessageCntrlImguiDemo_s& msgCtl);

#endif // IMGUI_STANDARD_DEMO_H