#ifndef SYSMON_AND_LOAD_H
#define SYSMON_AND_LOAD_H
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_glfw.h"
#include "../imgui/backends/imgui_impl_opengl2.h"
#include "K3Buffer.h"
#include "K3System.h"
#include "K3Key.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define XVIEW 999
#define YVIEW 666
#define BUFFER_SIZE 600
#define HISTOGRAM_SIZE 100
#define DEBUG true
#define VERSION "0.0.3"
#define WIN_ABOUT 0
#define WIN_DEBUG 1
#define WIN_CONTROL 2
#define WIN_LOAD_TEST 3
#define WIN_ABOUT_TEST 4
#define WIN_MAX 5
#define LEFT_PLUS_RIGHT_HORIZ_MARGIN 16
#define TOTAL_GRAPHS 5
#define TOTAL_DATA_ROWS 2

static void glfw_error_callback(int, const char*);
static ImVec2 calcGraphSize(void);
static void draw(K3Buffer*, const char*, const char*, const char*, bool); /*!< two visual representations are currently implemented; in time and in space (histogram) */
static void drawSysSummary(K3System* System);
static void spacePlot(K3Buffer*, const char*, const char*, const char*); /*!< a feature visualisation in space (histogram)*/
static void timePlot(K3Buffer*, const char*, const char*, const char*); /*!< a feature visualisation in time */

#endif
