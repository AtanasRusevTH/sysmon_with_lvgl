
// #pragma once
#ifndef IMGUI_THREAD_H
#define IMGUI_THREAD_H

#include <mutex>
#include <condition_variable>
#include <queue>

// Define a message structure
struct MessageImguiSysmonLoad {
  unsigned int endlessCalcThreads; // amount of special threads dedicated to endless calculations
  unsigned int allocMbytes;        // amount of RAM to be allocated
  unsigned int numPiCalcTasks;     // number of simple PI calculation tasks to be started. Once they finish, they stop.
  unsigned int triggerPiTasks;     // single trigger to calc PI.
  bool quitFlag;
  bool switchToImguiDemo;
  bool switchToLvglDemo;
  bool switchToSysmonImgui;
};

// Define a message structure
struct MessageCntrl_s {
    // Shared state between threads
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<MessageImguiSysmonLoad> messageQueue;
};

void imguiSymonLoadSendMessage(MessageCntrl_s& msgCtrl, MessageImguiSysmonLoad message);
MessageImguiSysmonLoad imguiSymonLoadReceiveMessage(MessageCntrl_s& msgCtrl);
int imguiSymonLoad(MessageCntrl_s& msgCtl);

void LvglSendMessage(MessageCntrl_s& msgCtrl, MessageImguiSysmonLoad message);
MessageImguiSysmonLoad MainThreadLvglReceiveMessage(MessageCntrl_s& msgCtrl);
int lvglTh(MessageCntrl_s& msgCtl);

#endif // #ifndef IMGUI_THREAD_H