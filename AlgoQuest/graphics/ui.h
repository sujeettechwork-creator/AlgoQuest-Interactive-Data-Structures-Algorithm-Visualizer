#ifndef ALGOQUEST_UI_H
#define ALGOQUEST_UI_H

#include "raylib.h"

/* Every visualizer draws in a local 950x750 "content" space.
   main.c shifts it right of the sidebar with a Camera2D + mouse offset. */
#define CW 950
#define CH 750

#define C_BG     ((Color){10, 13, 26, 255})
#define C_BG2    ((Color){20, 27, 52, 255})
#define C_PANEL  ((Color){24, 31, 54, 255})
#define C_PANEL2 ((Color){36, 46, 78, 255})
#define C_BORDER ((Color){58, 72, 112, 255})
#define C_CYAN   ((Color){0, 220, 235, 255})
#define C_PURPLE ((Color){157, 110, 255, 255})
#define C_GREEN  ((Color){70, 222, 140, 255})
#define C_RED    ((Color){255, 95, 110, 255})
#define C_AMBER  ((Color){255, 184, 64, 255})
#define C_BLUE   ((Color){70, 140, 255, 255})
#define C_WHITE  ((Color){240, 245, 255, 255})
#define C_MUTED  ((Color){140, 152, 182, 255})

typedef struct
{
    char text[200];
    Color color;
    float age;
} UIStatus;

void UIFrame(void);
float UITime(void);
float UIApproach(float current, float target, float speed);
float UIClampf(float v, float lo, float hi);
Color UILerpColor(Color a, Color b, float t);
Color UITextOn(Color background);

void UIBackground(void);
void UIHeader(const char *title, const char *subtitle, Color accent);
void UICard(Rectangle r, const char *title, Color accent);

/* Immediate-mode widgets: call them from the Draw function. */
int UIButton(Rectangle r, const char *label, Color color);
int UIInput(Rectangle r, char *buf, int cap, int *active,
            const char *placeholder, int lettersOnly);
int UIParseInt(const char *buf, int *out);

void UISetStatus(UIStatus *s, const char *text, Color color);
void UIStatusBar(UIStatus *s, Rectangle r);

void UINode(Vector2 c, float radius, const char *label,
            Color fill, Color border, float glow);
void UIArrow(Vector2 a, Vector2 b, Color color, float thick);
void UICell(Rectangle r, const char *label, Color fill, Color border, int fontSize);

#endif
