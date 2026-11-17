#include "ui.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int key;
    float hover;
    float press;
} WidgetState;

#define MAX_WIDGETS 192
static WidgetState widgets[MAX_WIDGETS];
static int widgetCount = 0;
static float gTime = 0.0f;

typedef struct
{
    float x, y, speed, radius, phase;
} Particle;

#define PARTICLES 44
static Particle particles[PARTICLES];
static int particlesReady = 0;

void UIFrame(void)
{
    gTime += GetFrameTime();
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

float UITime(void) { return gTime; }

float UIApproach(float current, float target, float speed)
{
    float k = 1.0f - expf(-speed * GetFrameTime());
    return current + (target - current) * k;
}

float UIClampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

Color UILerpColor(Color a, Color b, float t)
{
    t = UIClampf(t, 0.0f, 1.0f);
    return (Color){(unsigned char)(a.r + (b.r - a.r) * t),
                   (unsigned char)(a.g + (b.g - a.g) * t),
                   (unsigned char)(a.b + (b.b - a.b) * t),
                   (unsigned char)(a.a + (b.a - a.a) * t)};
}

Color UITextOn(Color c)
{
    float l = 0.299f * c.r + 0.587f * c.g + 0.114f * c.b;
    return l > 150.0f ? (Color){10, 13, 26, 255} : C_WHITE;
}

static WidgetState *GetWidget(Rectangle r)
{
    int key = (int)r.x * 100003 + (int)r.y * 31 + (int)r.width;

    for (int i = 0; i < widgetCount; i++)
        if (widgets[i].key == key)
            return &widgets[i];

    if (widgetCount < MAX_WIDGETS)
    {
        widgets[widgetCount].key = key;
        widgets[widgetCount].hover = 0.0f;
        widgets[widgetCount].press = 0.0f;
        return &widgets[widgetCount++];
    }

    return &widgets[0];
}

void UIBackground(void)
{
    if (!particlesReady)
    {
        for (int i = 0; i < PARTICLES; i++)
        {
            particles[i].x = (float)GetRandomValue(0, CW);
            particles[i].y = (float)GetRandomValue(0, CH);
            particles[i].speed = 6.0f + (float)GetRandomValue(0, 18);
            particles[i].radius = 1.0f + (float)GetRandomValue(0, 18) / 10.0f;
            particles[i].phase = (float)GetRandomValue(0, 628) / 100.0f;
        }
        particlesReady = 1;
    }

    DrawRectangleGradientV(0, 0, CW, CH, C_BG, C_BG2);

    for (int x = 0; x < CW; x += 48)
        DrawLine(x, 0, x, CH, Fade(C_BORDER, 0.16f));
    for (int y = 0; y < CH; y += 48)
        DrawLine(0, y, CW, y, Fade(C_BORDER, 0.16f));

    for (int i = 0; i < 6; i++)
    {
        DrawCircleV((Vector2){CW - 40.0f, 30.0f}, 300.0f - i * 48.0f, Fade(C_PURPLE, 0.022f));
        DrawCircleV((Vector2){40.0f, CH - 20.0f}, 260.0f - i * 40.0f, Fade(C_CYAN, 0.020f));
    }

    float dt = GetFrameTime();

    for (int i = 0; i < PARTICLES; i++)
    {
        Particle *p = &particles[i];

        p->y -= p->speed * dt;
        if (p->y < -6.0f)
        {
            p->y = CH + 6.0f;
            p->x = (float)GetRandomValue(0, CW);
        }

        float a = 0.25f + 0.2f * sinf(gTime * 1.5f + p->phase);
        DrawCircleV((Vector2){p->x + sinf(gTime * 0.5f + p->phase) * 8.0f, p->y},
                    p->radius, Fade(C_CYAN, a * 0.55f));
    }
}

void UIHeader(const char *title, const char *subtitle, Color accent)
{
    float pulse = 0.5f + 0.5f * sinf(gTime * 2.0f);

    DrawRectangleRounded((Rectangle){28, 22, 6, 48}, 0.5f, 4, accent);
    DrawText(title, 48, 20, 30, C_WHITE);
    DrawText(subtitle, 48, 58, 15, C_MUTED);
    DrawRectangleGradientH(48, 80, 360, 2, accent, Fade(accent, 0.0f));

    const char *hint = "ESC : DASHBOARD";
    DrawText(hint, CW - MeasureText(hint, 12) - 30, 30, 12,
             Fade(C_MUTED, 0.55f + 0.3f * pulse));
}

void UICard(Rectangle r, const char *title, Color accent)
{
    DrawRectangleRounded((Rectangle){r.x + 3, r.y + 5, r.width, r.height}, 0.05f, 8, Fade(BLACK, 0.35f));
    DrawRectangleRounded(r, 0.05f, 8, Fade(C_PANEL, 0.94f));
    DrawRectangleRoundedLinesEx(r, 0.05f, 8, 1.0f, Fade(C_BORDER, 0.9f));

    if (title != NULL && title[0] != '\0')
    {
        DrawRectangle((int)r.x + 16, (int)r.y + 14, 3, 12, accent);
        DrawText(title, (int)r.x + 26, (int)r.y + 13, 13, accent);
    }
}

int UIButton(Rectangle r, const char *label, Color color)
{
    WidgetState *s = GetWidget(r);
    int hover = CheckCollisionPointRec(GetMousePosition(), r);
    int down = hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    s->hover = UIApproach(s->hover, hover ? 1.0f : 0.0f, 16.0f);
    s->press = UIApproach(s->press, down ? 1.0f : 0.0f, 30.0f);

    if (hover)
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

    Rectangle d = r;
    d.y += -s->hover * 2.0f + s->press * 3.0f;

    if (s->hover > 0.02f)
        DrawRectangleRounded((Rectangle){d.x - 4, d.y - 4, d.width + 8, d.height + 8},
                             0.3f, 8, Fade(color, 0.24f * s->hover));

    DrawRectangleRounded((Rectangle){d.x, d.y + 3, d.width, d.height}, 0.25f, 8, Fade(BLACK, 0.35f));
    DrawRectangleRounded(d, 0.25f, 8, ColorBrightness(color, s->hover * 0.12f - s->press * 0.12f));
    DrawRectangleRounded((Rectangle){d.x + 2, d.y + 2, d.width - 4, d.height * 0.45f},
                         0.5f, 8, Fade(WHITE, 0.10f));

    int fs = 16;
    while (fs > 10 && MeasureText(label, fs) > (int)d.width - 12)
        fs--;

    DrawText(label,
             (int)(d.x + (d.width - MeasureText(label, fs)) / 2),
             (int)(d.y + (d.height - fs) / 2), fs, UITextOn(color));

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

int UIInput(Rectangle r, char *buf, int cap, int *active,
            const char *placeholder, int lettersOnly)
{
    WidgetState *s = GetWidget(r);
    int entered = 0;
    int hover = CheckCollisionPointRec(GetMousePosition(), r);

    if (hover)
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        *active = hover;

    int len = (int)strlen(buf);

    if (*active)
    {
        int k = GetCharPressed();

        while (k > 0)
        {
            if (len < cap - 1 && k < 128)
            {
                if (lettersOnly)
                {
                    if (isalpha(k))
                    {
                        buf[len++] = (char)toupper(k);
                        buf[len] = '\0';
                    }
                }
                else if ((k >= '0' && k <= '9') || (k == '-' && len == 0))
                {
                    buf[len++] = (char)k;
                    buf[len] = '\0';
                }
            }
            k = GetCharPressed();
        }

        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && len > 0)
            buf[--len] = '\0';

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            entered = 1;
    }

    s->press = UIApproach(s->press, *active ? 1.0f : 0.0f, 14.0f);

    if (s->press > 0.02f)
        DrawRectangleRounded((Rectangle){r.x - 4, r.y - 4, r.width + 8, r.height + 8},
                             0.3f, 8, Fade(C_CYAN, 0.14f * s->press));

    DrawRectangleRounded(r, 0.2f, 8, C_BG);
    DrawRectangleRoundedLinesEx(r, 0.2f, 8, 1.0f + s->press,
                                UILerpColor(C_BORDER, C_CYAN, s->press));

    int ty = (int)(r.y + (r.height - 20) / 2);

    if (len > 0)
        DrawText(buf, (int)r.x + 14, ty, 20, C_WHITE);
    else
        DrawText(placeholder, (int)r.x + 14, ty + 2, 16, Fade(C_MUTED, 0.8f));

    if (*active && fmodf(gTime, 1.0f) < 0.55f)
        DrawRectangle((int)r.x + 16 + (len > 0 ? MeasureText(buf, 20) : 0),
                      (int)(r.y + r.height * 0.22f), 2, (int)(r.height * 0.56f), C_CYAN);

    return entered;
}

int UIParseInt(const char *buf, int *out)
{
    if (buf[0] == '\0' || (buf[0] == '-' && buf[1] == '\0'))
        return 0;

    *out = atoi(buf);
    return 1;
}

void UISetStatus(UIStatus *s, const char *text, Color color)
{
    snprintf(s->text, sizeof(s->text), "%s", text);
    s->color = color;
    s->age = 0.0f;
}

void UIStatusBar(UIStatus *s, Rectangle r)
{
    s->age += GetFrameTime();

    float a = UIClampf(s->age * 5.0f, 0.0f, 1.0f);
    float flash = UIClampf(1.0f - s->age * 2.0f, 0.0f, 1.0f);

    UICard(r, "STATUS", s->color);

    if (flash > 0.0f)
        DrawRectangleRounded(r, 0.05f, 8, Fade(s->color, 0.12f * flash));

    char line[200];
    snprintf(line, sizeof(line), "%s", s->text);

    int maxW = (int)r.width - 56;
    int len = (int)strlen(line);

    while (len > 4 && MeasureText(line, 18) > maxW)
    {
        len--;
        line[len] = '\0';
        line[len - 1] = '.';
        line[len - 2] = '.';
        line[len - 3] = '.';
    }

    DrawText(line, (int)(r.x + 26 + (1.0f - a) * 14.0f), (int)r.y + 38, 18, Fade(s->color, a));
}

void UINode(Vector2 c, float radius, const char *label,
            Color fill, Color border, float glow)
{
    if (glow > 0.01f)
        for (int i = 5; i >= 1; i--)
            DrawCircleV(c, radius + i * 4.0f, Fade(border, 0.07f * glow));

    DrawCircleV((Vector2){c.x + 2, c.y + 3}, radius, Fade(BLACK, 0.35f));
    DrawCircleV(c, radius, fill);
    DrawCircleV((Vector2){c.x - radius * 0.25f, c.y - radius * 0.3f}, radius * 0.5f, Fade(WHITE, 0.08f));
    DrawRing(c, radius - 2.0f, radius, 0.0f, 360.0f, 40, border);

    int fs = (int)(radius * 0.8f);
    if (fs > 20)
        fs = 20;
    while (fs > 8 && MeasureText(label, fs) > (int)(radius * 1.7f))
        fs--;

    DrawText(label, (int)(c.x - MeasureText(label, fs) / 2.0f), (int)(c.y - fs / 2.0f),
             fs, UITextOn(fill));
}

void UIArrow(Vector2 a, Vector2 b, Color c, float thick)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float len = sqrtf(dx * dx + dy * dy);

    if (len < 1.0f)
        return;

    dx /= len;
    dy /= len;

    float head = 9.0f + thick * 1.5f;
    if (head > len)
        head = len;

    Vector2 base = {b.x - dx * head, b.y - dy * head};
    Vector2 p1 = {base.x - dy * head * 0.5f, base.y + dx * head * 0.5f};
    Vector2 p2 = {base.x + dy * head * 0.5f, base.y - dx * head * 0.5f};

    DrawLineEx(a, base, thick, c);

    /* DrawTriangle needs counter-clockwise order or it is culled. */
    float cross = (p1.x - b.x) * (p2.y - b.y) - (p1.y - b.y) * (p2.x - b.x);

    if (cross > 0.0f)
        DrawTriangle(b, p2, p1, c);
    else
        DrawTriangle(b, p1, p2, c);
}

void UICell(Rectangle r, const char *label, Color fill, Color border, int fontSize)
{
    DrawRectangleRounded(r, 0.14f, 8, fill);
    DrawRectangleRoundedLinesEx(r, 0.14f, 8, 1.5f, border);

    DrawText(label,
             (int)(r.x + (r.width - MeasureText(label, fontSize)) / 2),
             (int)(r.y + (r.height - fontSize) / 2), fontSize, UITextOn(fill));
}
