#ifndef FS_HEADER_
#define FS_HEADER_

#include <assert.h>
#include <stdio.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define NO_SIGNAL         (-1)  // No signal
#define MAX_INSTANCES     8000  // Max glyphs to render
#define MAX_LEN           1023  // Max length of text fields
#define MAX_WIDTH         4096  // Max texture width
#define PADDING           5     // Padding in pixels
#define SCREEN_NUM        6     // Number of screens
#define VEC_INIT_CAP      8     // Initial vector size

#define FONTS_NUM         6     // Number of fonts
#define GLYPHS_NUM        95    // ASCII glyphs (126 - 32 + 1)

#define CLICK_LO          0.02  // Double click LO time
#define CLICK_HI          0.20  // Double click HI time
#define ACC_Y             10.0  // Vertical acceleration
#define LIMIT             5.0f  // Duration of scroll
#define SLOWDOWN          0.98  // Slowdown factor

#define ACTIVE_BOX        ctx->inputbox.boxes[ctx->screen].selected
#define BUTTON_CLICKED    ctx->buttons.clicked
#define COMMITED_BOX      ctx->inputbox.boxes[ctx->screen].commited
#define SCREEN            ctx->screen

enum { TXT, NUM };
typedef enum Align { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT }   Align;
typedef enum { MEDIUM, BIG, SMALL, MONO, BOX, HOVER }          FontType;

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4[4];

typedef struct fs_context fs_Context;
typedef void (*FnPtr)(struct fs_context *); // Function pointer type

typedef struct {
    char path[64];  // Font path
    float size;     // Font size
    float gamma;    // Gamma correction
} fs_Fonts;

typedef struct {
    size_t capacity;
    size_t size;
    size_t item_size;
    void *items;
} fs_Vector;

typedef struct {
    vec4 pos;       // Area background x,y,w,h
    vec4 text_pos;  // Text background position x,y,w,h
    FnPtr func;     // Function call
} fs_Area;

typedef struct {
    fs_Vector *area;
    vec4 col;         // Area background color
    int active;       // Index of active area
    int count;        // Number of areas added to the current screen
} fs_Areas;

typedef struct {
    GLfloat advance_x;
    GLfloat bitmap_width;
    GLfloat bitmap_height;
    GLfloat bitmap_left;
    GLfloat bitmap_top;
    GLfloat offset_x;
    GLfloat offset_y;
} fs_Glyph;

typedef struct {
    fs_Glyph glyphs[GLYPHS_NUM];
    signed long **kerning_table;
    GLfloat gamma;
    GLuint line_height;
    GLuint tex_id;
    GLuint tex_metrics_id;
    GLuint tex_width;
    GLuint tex_height;
} fs_Atlas;

typedef struct {
    char text[MAX_LEN + 1];  // Input text
    vec4 pos;                // Inputbox position: x,y,w,h
    float len_pixel;         // Text pixel length
    int len_char;            // Text char length
    int flag;                // Text or numeric
} fs_Box;

typedef struct {
    fs_Vector *box;
    int selected;       // Index of selected inputbox
    int commited;       // ENTER pressed
    int count;          // Number of inputbox on current screen
} fs_Boxes;

typedef struct {
    fs_Boxes boxes[SCREEN_NUM];
    vec4 bg_col;
    vec4 fg_col;
    vec4 sel_col;
} fs_Inputbox;

typedef struct {
    char text[MAX_LEN + 1]; // Text on button
    vec4 pos;               // Position: x,y,w,h
} fs_Button;

typedef struct {
    fs_Vector *button;
    vec4 normal_col;
    vec4 text_col;
    vec4 hover_col;
    int clicked;       // Index of button clicked
} fs_Buttons;

typedef struct {
    GLuint program;
    GLuint vao;
    GLuint vbo_quad;
    GLuint vbo_instance_data;  // Instance data
    GLuint vbo_instance_color; // Instance color
} fs_Shader;

typedef struct {
    float factor;    // Slow down factor
    float depth;     // Max depth of objects
    float offset;    // Offset y
    float max;       // Max depth of objects
    float speed;     // Slow down speed
    int direction;   // Direction up or down
} fs_Scroll;

typedef struct {
    vec4 pos; // Position: x,y,w,h
    vec4 col;
} fs_Rect;

typedef struct {
    fs_Vector *rect;
} fs_Rects;

typedef struct {
    char text[MAX_LEN + 1]; // Max length of text
    vec2 pos;               // Text position x,y
    vec4 col;               // Text font color - alpha will be computed by fragment shader
} fs_Text;

typedef struct {
    fs_Vector *text;
} fs_Texts;

struct fs_context {
    fs_Atlas fonts[FONTS_NUM];    // Font atlas
    fs_Texts texts[FONTS_NUM];    // Text per font types
    fs_Areas areas;               // Hover areas on current screen
    fs_Buttons buttons;           // Buttons
    fs_Inputbox inputbox;         // Inputboxes
    fs_Rects rects;               // Rectangles
    fs_Scroll scroll;             // Vertical scroll
    fs_Shader area_shader;        // Shader program for hover area
    fs_Shader quad_shader;        // Shader program for rectangles
    fs_Shader text_shader;        // Shader program for text
    GLFWwindow *window;           // GLFW window
    mat4 transform;               // Runtime variable - OpenGl transformation
    float last_click;             // Runtime variable - last click time
    double mx, my;                // Runtime variable - mouse x,y position
    int double_click;             // Runtime variable - double click flag
    int screen;                   // Runtime variable - current screen
    int width, height;            // Runtime variable - window size
};

#ifdef FS_IMPLEMENTATION

inline static void fs_vec2_copy(vec2 dest, vec2 src)
{
    dest[0] = src[0];
    dest[1] = src[1];
}

inline static void fs_vec4_copy(vec4 dest, vec4 src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

inline static void fs_mat4_copy(mat4 dest, mat4 src)
{
    dest[0][0] = src[0][0];
    dest[1][0] = src[1][0];
    dest[0][1] = src[0][1];
    dest[1][1] = src[1][1];
    dest[0][2] = src[0][2];
    dest[1][2] = src[1][2];
    dest[0][3] = src[0][3];
    dest[1][3] = src[1][3];
    dest[2][0] = src[2][0];
    dest[3][0] = src[3][0];
    dest[2][1] = src[2][1];
    dest[3][1] = src[3][1];
    dest[2][2] = src[2][2];
    dest[3][2] = src[3][2];
    dest[2][3] = src[2][3];
    dest[3][3] = src[3][3];
}

inline static void fs_mat4_set_identity(mat4 mat)
{
    mat4 t = {
        { 1.0, 0.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 1.0 }
    };

    fs_mat4_copy(mat, t);
}

inline static void fs_vec4_muladds(vec4 a, float s, vec4 dest)
{
    dest[0] += a[0] * s;
    dest[1] += a[1] * s;
    dest[2] += a[2] * s;
    dest[3] += a[3] * s;
}

static void fs_mat4_translate(mat4 m, vec3 v)
{
    fs_vec4_muladds(m[0], v[0], m[3]);
    fs_vec4_muladds(m[1], v[1], m[3]);
    fs_vec4_muladds(m[2], v[2], m[3]);
}

static fs_Vector *fs_vector_init(size_t data_size, size_t capacity)
{
    fs_Vector *vector = malloc(sizeof(fs_Vector));

    if (vector == NULL) {
        assert(0 && "Failed to initialize vector");
    }
    vector->capacity = capacity;
    vector->size     = 0;

    vector->items = malloc(vector->capacity * data_size);
    if (vector->items == NULL) {
        assert(0 && "Failed to initialize vector");
    }
    vector->item_size = data_size;

    return (vector);
}

static void fs_vector_add(fs_Vector *vec, const void *data)
{
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->items     = realloc(vec->items, vec->capacity * vec->item_size);
        if (vec->items == NULL) {
            assert(0 && "Failed to resize vector");
        }
    }

    memcpy((char *)vec->items + vec->size * vec->item_size, data, vec->item_size);
    vec->size++;
}

inline static void *fs_vector_get(fs_Vector *vector, size_t index)
{
    return ((char *)(vector->items) + vector->item_size * index);
}

inline static void fs_vector_reset(fs_Vector *vector)
{
    memset((char *)(vector->items), 0, vector->size * vector->item_size);
    vector->size = 0;
}

inline static void fs_vector_free(fs_Vector *vector)
{
    if (vector) {
        if (vector->items) {
            free(vector->items);
        }
        free(vector);
    }
}

static float fs_text_width(fs_Atlas *atlas, const char *text)
{
    float         width    = 0;
    unsigned char previous = 32;

    for (const unsigned char *c = (const unsigned char *)text; *c; ++c) {
        if (*c < 32 || *c > 127) {
            continue; // Quick and dirty solution to handle unicode characters
        }
        signed long kerning = atlas->kerning_table[previous][*c];
        width   += atlas->glyphs[*c - 32].advance_x + kerning;
        previous = *c;
    }
    return (width);
}

static void fs_block_width(fs_Atlas *atlas, const char *text, float *width, int *rows)
{
    float row_width = 0;
    char  *p, row_text[MAX_LEN];

    while (1) {
        p = row_text;
        while (*text != '\0' && *text != '\n') {
            *p++ = *text++;
        }
        *p        = '\0';
        row_width = fs_text_width(atlas, row_text);
        if (row_width > *width) {
            *width = row_width;
        }
        *rows += 1;
        if (*text == '\0') {
            break;
        }
        text++;
    }
}

static float fs_text_height(fs_Atlas *atlas, const char *text)
{
    float height = 0;

    for (const unsigned char *c = (const unsigned char *)text; *c; ++c) {
        if (atlas->glyphs[*c - 32].bitmap_height > height) {
            height = atlas->glyphs[*c - 32].bitmap_height;
        }
    }
    return (height);
}

static char *fs_get_inputbox_content(fs_Context *ctx, int index)
{
    if (ctx->inputbox.boxes[ctx->screen].box->size < index) {
        return (NULL);
    }
    fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, index);
    return (box->text);
}

static void fs_set_inputbox_content(fs_Context *ctx, int index, char *text)
{
    if (ctx->inputbox.boxes[ctx->screen].box->size < index) {
        return;
    }
    fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, index);
    memset(box->text, 0, MAX_LEN + 1);
    strncpy(box->text, text, MAX_LEN);
    box->len_char  = strlen(text);
    box->len_pixel = fs_text_width(&ctx->fonts[BOX], text) + PADDING;
}

static void fs_error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}

static void fs_char_callback(GLFWwindow *window, unsigned int codepoint)
{
    fs_Context *ctx = (fs_Context *)glfwGetWindowUserPointer(window);

    if (ctx->inputbox.boxes[ctx->screen].selected == NO_SIGNAL) {
        return;
    }
    fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, ctx->inputbox.boxes[ctx->screen].selected);

    // Clear inputbox if double clicked
    if (ctx->double_click == GLFW_TRUE) {
        memset(box->text, 0, MAX_LEN + 1);
        box->len_char     = 0;
        box->len_pixel    = 0;
        ctx->double_click = GLFW_FALSE;
    }

    // Character check in numeric inputbox
    if (box->flag == NUM && !((codepoint >= '0' && codepoint <= '9') || codepoint == '.' || codepoint == '-')) {
        return;
    }

    // Check string lenght
    if (strlen(box->text) == MAX_LEN - 1) {
        return;
    }

    // Check pixel width
    char        previous = 32;
    signed long kerning  = 0;
    if (box->len_char > 0) {
        previous = box->text[box->len_char - 1];
    }
    float width = ctx->fonts[BOX].glyphs[codepoint - 32].advance_x + ctx->fonts[BOX].kerning_table[previous][codepoint];
    if (box->len_pixel + PADDING + width > box->pos[2]) {
        return;
    }

    // Check if text is valid floating point number in numeric inputbox
    box->text[box->len_char] = codepoint;
    if (strcmp(box->text, "-") != 0 && box->flag == NUM) { // Negative sign is valid
        char   *pEnd;
        double value = strtod(box->text, &pEnd);
        if (pEnd == box->text || *pEnd != '\0') {
            box->text[box->len_char] = '\0';
            return;
        }
    }

    // All good, add character
    box->len_char++;
    box->len_pixel += width;
}

static void fs_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (GLFW_PRESS != action && GLFW_REPEAT != action) {
        return;
    }

    fs_Context *ctx = (fs_Context *)glfwGetWindowUserPointer(window);
    fs_Box     *box = NULL;
    if (ctx->inputbox.boxes[ctx->screen].selected == NO_SIGNAL) {
        box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, 0);
    } else {
        box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, ctx->inputbox.boxes[ctx->screen].selected);
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(ctx->window, GL_TRUE);
        break;

        case GLFW_KEY_BACKSPACE:
            if (ctx->inputbox.boxes[ctx->screen].selected != NO_SIGNAL && box->len_char > 0) {
                if (ctx->double_click == GLFW_TRUE) {
                    memset(box->text, 0, MAX_LEN + 1);
                    box->len_char  = 0;
                    box->len_pixel = PADDING;
                } else {
                    box->len_char--;
                    box->text[box->len_char] = '\0';
                    box->len_pixel           = PADDING + fs_text_width(&ctx->fonts[BOX], box->text);
                }
                ctx->double_click = GLFW_FALSE;
            }
        break;

        case GLFW_KEY_TAB:
            if (mods == GLFW_MOD_SHIFT) {
                if (ctx->inputbox.boxes[ctx->screen].selected > 0) {
                    ctx->inputbox.boxes[ctx->screen].selected--;
                } else {
                    ctx->inputbox.boxes[ctx->screen].selected = ctx->inputbox.boxes[ctx->screen].box->size - 1;
                }
            } else {
                if (ctx->inputbox.boxes[ctx->screen].selected < ctx->inputbox.boxes[ctx->screen].box->size - 1) {
                    ctx->inputbox.boxes[ctx->screen].selected++;
                } else {
                    ctx->inputbox.boxes[ctx->screen].selected = 0;
                }
            }
            ctx->double_click = GLFW_FALSE;
        break;

        case GLFW_KEY_ENTER:
            ctx->inputbox.boxes[ctx->screen].commited = GLFW_TRUE;
        break;
    }

    if ((GLFW_MOD_CONTROL & mods) == 0 || ctx->inputbox.boxes[ctx->screen].selected == NO_SIGNAL) {
        return;
    }

    switch (key) {
        case GLFW_KEY_V:
            // Basic clipboad checks
            const char *cb = glfwGetClipboardString(ctx->window);
            if (cb == NULL || *cb == '\r' || *cb == '\n') {
                return;
            }

            // Remove new lines and subsequent characters
            for (unsigned char *p = (unsigned char *)cb; *p; ++p) {
                if (*p == '\r' || *p == '\n') {
                    *p = '\0';
                    break;
                }
            }

            // Text in clipboard valid for numeric inputbox?
            if (box->flag == NUM) {
                char   *pEnd;
                double value = strtod(cb, &pEnd);
                if (pEnd == cb || *pEnd != '\0') {
                    return;
                }
            }

            // Calculate number of characters to paste
            char buf[MAX_LEN + 1] = { 0 };
            int  copy_len         = 0;

            for (unsigned char *p = (unsigned char *)cb; *p; ++p) {
                buf[copy_len] = *p;
                float width = fs_text_width(&ctx->fonts[BOX], buf) + PADDING;
                if (width > box->pos[2]) {
                    buf[copy_len] = '\0';
                    break;
                }
                ++copy_len;
            }
            ctx->double_click = GLFW_FALSE;
            fs_set_inputbox_content(ctx, ctx->inputbox.boxes[ctx->screen].selected, buf);
        break;

        case GLFW_KEY_C:
            fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, ctx->inputbox.boxes[ctx->screen].selected);
            glfwSetClipboardString(ctx->window, box->text);
        break;
    }
}

static void fs_button_callback(GLFWwindow *window, int btn_m, int action, int mods)
{
    if (btn_m != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    fs_Context *ctx = (fs_Context *)glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS) {
        // Any button clicked?
        float ypos = ctx->my + ctx->scroll.offset / 2.0f;
        for (int i = 0; i < ctx->buttons.button->size; ++i) {
            fs_Button *btn = (fs_Button *)fs_vector_get(ctx->buttons.button, i);
            if (ctx->mx > btn->pos[0] && ctx->mx < btn->pos[0] + btn->pos[2] && ypos > btn->pos[1] && ypos < btn->pos[1] + btn->pos[3]) {
                ctx->buttons.clicked = i;
            }
        }

        // Any inputbox clicked or double-clicked?
        for (int i = 0; i < ctx->inputbox.boxes[ctx->screen].box->size; ++i) {
            fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, i);
            if (ctx->mx > box->pos[0] && ctx->mx < box->pos[0] + box->pos[2] && ypos > box->pos[1] && ypos < box->pos[1] + box->pos[3]) {
                ctx->inputbox.boxes[ctx->screen].selected = i;
                float dt = glfwGetTime() - ctx->last_click;
                if (dt > CLICK_LO && dt < CLICK_HI) {
                    ctx->double_click = GLFW_TRUE;
                } else {
                    ctx->double_click = GLFW_FALSE;
                }
                ctx->last_click = glfwGetTime();
                return;
            }
        }
    }
}

static void fs_scroll_callback(GLFWwindow *window, double offsetX, double offsetY)
{
    fs_Context *ctx = (fs_Context *)glfwGetWindowUserPointer(window);

    ctx->scroll.offset   -= offsetY * ACC_Y;
    ctx->scroll.direction = (offsetY > 0) - (offsetY < 0);
    ctx->scroll.speed     = ACC_Y;
    ctx->scroll.factor    = SLOWDOWN;
}

static void fs_resize_callback(GLFWwindow *window, int width, int height)
{
    fs_Context *ctx = (fs_Context *)glfwGetWindowUserPointer(window);

    glViewport(0, 0, width, height);
    ctx->width  = width;
    ctx->height = height;
}

static void fs_add_area(fs_Context *ctx, vec4 pos, void *callback_fn)
{
    fs_Area area;

    fs_vec4_copy(area.pos, pos);
    area.func = callback_fn;
    fs_vector_add(ctx->areas.area, &area);
}

static void fs_add_rect(fs_Context *ctx, vec4 pos, vec4 col)
{
    fs_Rect rect;

    fs_vec4_copy(rect.pos, pos);
    fs_vec4_copy(rect.col, col);
    fs_vector_add(ctx->rects.rect, &rect);
}

static void fs_add_text(fs_Context *ctx, vec2 pos, char *text, FontType type, vec4 fg_col, Align alignment)
{
    fs_Atlas *atlas = &ctx->fonts[type];
    fs_Text  txt;

    switch (alignment) {
        case ALIGN_CENTER:
            pos[0] -= fs_text_width(atlas, text) / 2.0f;
        break;

        case ALIGN_RIGHT:
            pos[0] -= fs_text_width(atlas, text);
        break;

        default:
        break;
    }

    strncpy(txt.text, text, MAX_LEN);
    fs_vec4_copy(txt.col, fg_col);
    fs_vec2_copy(txt.pos, pos);
    fs_vector_add(ctx->texts[type].text, &txt);
}

static void fs_add_area_text(fs_Context *ctx, char *text, vec4 fg_col)
{
    text[strlen(text)] = '\0';
    float width = 0;
    int   rows  = 3; // Min 1 row and  1 + 1 extra blank row at top and bottom

    fs_Atlas *atlas = &ctx->fonts[HOVER];
    fs_block_width(atlas, text, &width, &rows);

    fs_Area *area  = (fs_Area *)fs_vector_get(ctx->areas.area, ctx->areas.active);
    float   ypos   = ctx->my + ctx->scroll.offset / 2.0f;
    vec4    bg_pos = { ctx->mx, ypos, width + ctx->fonts[HOVER].line_height * 2.0f, rows * ctx->fonts[HOVER].line_height };
    fs_vec4_copy(area->text_pos, bg_pos);
    fs_add_text(ctx, (vec2){ ctx->mx + ctx->fonts[HOVER].line_height, ypos + ctx->fonts[HOVER].line_height * 2.0f }, text, HOVER, fg_col, ALIGN_LEFT);
}

static int fs_check_area(fs_Context *ctx)
{
    float ypos = ctx->my + ctx->scroll.offset / 2.0f;

    for (int i = 0; i < ctx->areas.area->size; ++i) {
        fs_Area *hover = (fs_Area *)fs_vector_get(ctx->areas.area, i);
        if ((ctx->mx > hover->pos[0] && ctx->mx < hover->pos[0] + hover->pos[2] && ypos > hover->pos[1] && ypos < hover->pos[1] + hover->pos[3])) {
            ctx->areas.active = i;
            hover->func(ctx);
            return (1);
        }
    }

    return (0);
}

static void fs_add_button(fs_Context *ctx, vec4 pos, char *text, FontType type)
{
    fs_Button btn;

    strncpy(btn.text, text, MAX_LEN);
    fs_vec4_copy(btn.pos, pos);
    fs_vector_add(ctx->buttons.button, &btn);

    fs_Atlas *atlas = &ctx->fonts[type];
    pos[1] += (pos[3] + fs_text_height(atlas, text)) / 2.0f;
    fs_add_text(ctx, (vec2){ pos[0] + pos[2] / 2.0f, pos[1] }, text, type, ctx->buttons.text_col, ALIGN_CENTER);
}

static void fs_add_inputbox(fs_Context *ctx, vec4 pos, int flag)
{
    // If inputbox is already used on current screen skip and return
    if (ctx->inputbox.boxes[ctx->screen].box->size < ctx->inputbox.boxes[ctx->screen].count) {
        ctx->inputbox.boxes[ctx->screen].box->size++;
        return;
    }

    fs_Box box = { 0 };
    fs_vec4_copy(box.pos, pos);
    box.flag = flag;
    fs_vector_add(ctx->inputbox.boxes[ctx->screen].box, &box);

    // Update y coordinate max depth
    if (pos[1] + pos[3] > ctx->scroll.max) {
        ctx->scroll.max = pos[1] + pos[3];
    }
}

void fs_render_area_background(fs_Context *ctx)
{
    glUseProgram(ctx->area_shader.program);
    glBindVertexArray(ctx->area_shader.vao);

    fs_Area *area = (fs_Area *)fs_vector_get(ctx->areas.area, ctx->areas.active);
    glUniform2f(glGetUniformLocation(ctx->area_shader.program, "p0"), area->text_pos[0], area->text_pos[1] + area->text_pos[3]);
    glUniform2f(glGetUniformLocation(ctx->area_shader.program, "p1"), area->text_pos[0] + area->text_pos[2], area->text_pos[1]);
    glUniform4fv(glGetUniformLocation(ctx->area_shader.program, "color"), 1, (const GLfloat *)ctx->areas.col);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);
}

static void fs_render_rects(fs_Context *ctx)
{
    glUseProgram(ctx->quad_shader.program);
    glBindVertexArray(ctx->quad_shader.vao);

    // Draw rectangles
    for (int i = 0; i < ctx->rects.rect->size; ++i) {
        fs_Rect *rect = (fs_Rect *)fs_vector_get(ctx->rects.rect, i);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p0"), rect->pos[0], rect->pos[1] + rect->pos[3]);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p1"), rect->pos[0] + rect->pos[2], rect->pos[1]);
        glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)rect->col);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Draw buttons
    float ypos = ctx->my + ctx->scroll.offset / 2.0f;
    for (int i = 0; i < ctx->buttons.button->size; ++i) {
        fs_Button *btn = (fs_Button *)fs_vector_get(ctx->buttons.button, i);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p0"), btn->pos[0], btn->pos[1] + btn->pos[3]);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p1"), btn->pos[0] + btn->pos[2], btn->pos[1]);

        if (ctx->mx > btn->pos[0] && ctx->mx < btn->pos[0] + btn->pos[2] && ypos > btn->pos[1] && ypos < btn->pos[1] + btn->pos[3]) {
            glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)ctx->buttons.hover_col);
        } else {
            glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)ctx->buttons.normal_col);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Draw inputboxes
    for (int i = 0; i < ctx->inputbox.boxes[ctx->screen].box->size; ++i) {
        fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, i);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p0"), box->pos[0], box->pos[1] + box->pos[3]);
        glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "p1"), box->pos[0] + box->pos[2], box->pos[1]);

        if (ctx->inputbox.boxes[ctx->screen].selected == i && ctx->double_click == GLFW_TRUE) {
            glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)ctx->inputbox.fg_col);
        } else if (i == ctx->inputbox.boxes[ctx->screen].selected) {
            glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)ctx->inputbox.sel_col);
        } else {
            glUniform4fv(glGetUniformLocation(ctx->quad_shader.program, "color"), 1, (const GLfloat *)ctx->inputbox.bg_col);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void fs_render_text(fs_Context *ctx, FontType type)
{
    glUseProgram(ctx->text_shader.program);
    glBindVertexArray(ctx->text_shader.vao);

    glUniform2f(glGetUniformLocation(ctx->text_shader.program, "res_atlas"), ctx->fonts[type].tex_width, ctx->fonts[type].tex_height);
    glUniform1f(glGetUniformLocation(ctx->text_shader.program, "gamma"), ctx->fonts[type].gamma);
    GLfloat vbi_data[MAX_INSTANCES * 3];
    GLfloat vbi_colors[MAX_INSTANCES][3];
    int     n = 0;

    for (int i = 0; i < ctx->texts[type].text->size; ++i) {
        fs_Text *text = (fs_Text *)fs_vector_get(ctx->texts[type].text, i);
        float   xpos  = text->pos[0] - ctx->width / 2.0f;
        float   ypos  = -text->pos[1] + ctx->height / 2.0f;

        int previous = 0;
        for (const unsigned char *c = (const unsigned char *)text; *c; ++c) {
            if ((*c) == '\n') {
                xpos  = text->pos[0] - ctx->width / 2.0f;
                ypos -= ctx->fonts[type].line_height;
                continue;
            }

            signed long kerning = ctx->fonts[type].kerning_table[previous][*c];
            vbi_data[3 * n + 0] = xpos + kerning;
            vbi_data[3 * n + 1] = ypos;
            vbi_data[3 * n + 2] = *c - 32.0f;
            xpos               += ctx->fonts[type].glyphs[*c - 32].advance_x + kerning;
            previous            = *c;

            memcpy(vbi_colors[n], text->col, 3 * sizeof(float));
            ++n;
        }
    }

    // Instance data
    glBindBuffer(GL_ARRAY_BUFFER, ctx->text_shader.vbo_instance_data);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbi_data), vbi_data, GL_STATIC_DRAW);

    // Instance colors
    glBindBuffer(GL_ARRAY_BUFFER, ctx->text_shader.vbo_instance_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbi_colors), vbi_colors, GL_STATIC_DRAW);

    // Render glyphs
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx->fonts[type].tex_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx->fonts[type].tex_metrics_id);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, n);

    // Render finished
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

static void fs_render_ui(fs_Context *ctx)
{
    glfwGetCursorPos(ctx->window, &ctx->mx, &ctx->my);

    // Calculate srolling offset and speed
    ctx->scroll.offset -= ctx->scroll.direction * ctx->scroll.speed;

    // Scroll only downwards and no scroll if objects fit in window size
    if (ctx->scroll.offset <= 0 || ctx->scroll.max <= ctx->height) {
        ctx->scroll.offset = 0;
    }

    if (ctx->scroll.direction != 0) {
        ctx->scroll.speed *= ctx->scroll.factor; // Slowing down
        if (ctx->scroll.speed < ACC_Y - LIMIT || ctx->scroll.speed > ACC_Y + LIMIT) {
            ctx->scroll.speed = 0; // Stop moving
        }
    }

    fs_mat4_set_identity(ctx->transform);
    fs_mat4_translate(ctx->transform, (vec3){ 0.0f, ctx->scroll.offset / ctx->height, 0.0f });

    glUseProgram(ctx->quad_shader.program);
    glUniform2f(glGetUniformLocation(ctx->quad_shader.program, "res_win"), ctx->width, ctx->height);
    glUniformMatrix4fv(glGetUniformLocation(ctx->quad_shader.program, "transform"), 1, GL_FALSE, (const GLfloat *)ctx->transform);
    glUseProgram(0);

    glUseProgram(ctx->area_shader.program);
    glUniform2f(glGetUniformLocation(ctx->area_shader.program, "res_win"), ctx->width, ctx->height);
    glUniformMatrix4fv(glGetUniformLocation(ctx->area_shader.program, "transform"), 1, GL_FALSE, (const GLfloat *)ctx->transform);
    glUseProgram(0);

    glUseProgram(ctx->text_shader.program);
    glUniform2f(glGetUniformLocation(ctx->text_shader.program, "res_win"), ctx->width, ctx->height);
    glUniformMatrix4fv(glGetUniformLocation(ctx->text_shader.program, "transform"), 1, GL_FALSE, (const GLfloat *)ctx->transform);
    glUseProgram(0);

    // Update inputbox text and configure inputbox text position
    for (int i = 0; i < ctx->inputbox.boxes[ctx->screen].box->size; ++i) {
        fs_Box *box = (fs_Box *)fs_vector_get(ctx->inputbox.boxes[ctx->screen].box, i);
        // Height of text should be fixed to avoid the text jumping up and down
        float ypos = box->pos[1] + (box->pos[3] + ctx->fonts[BOX].glyphs['0' - 32].bitmap_height) / 2.0f;

        if (ctx->inputbox.boxes[ctx->screen].selected == i && ctx->double_click == GLFW_TRUE) {
            fs_add_text(ctx, (vec2){ box->pos[0] + PADDING, ypos }, box->text, BOX, ctx->inputbox.bg_col, ALIGN_LEFT);
        } else {
            fs_add_text(ctx, (vec2){ box->pos[0] + PADDING, ypos }, box->text, BOX, ctx->inputbox.fg_col, ALIGN_LEFT);
        }
    }

    // Clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render rectangles
    fs_render_rects(ctx);

    // Render texts
    for (int i = 0; i < FONTS_NUM - 1; ++i) {
        fs_render_text(ctx, i);
    }
    fs_vector_reset(ctx->texts[BOX].text);

    // Check and render areas
    if (fs_check_area(ctx)) {
        fs_render_area_background(ctx);
        fs_render_text(ctx, HOVER);
        fs_vector_reset(ctx->texts[HOVER].text);
    }

    glfwSwapBuffers(ctx->window);

    // Poll events only if needed
    if (ctx->scroll.speed > 0) {
        glfwPollEvents();
    } else {
        glfwWaitEvents();
    }
}

static void fs_clear_screen(fs_Context *ctx)
{
    // Rectangles and quad shader
    fs_vector_reset(ctx->rects.rect);

    // Buttons
    fs_vector_reset(ctx->buttons.button);
    ctx->buttons.clicked = NO_SIGNAL;

    // Inputboxes
    ctx->inputbox.boxes[ctx->screen].commited  = NO_SIGNAL;
    ctx->inputbox.boxes[ctx->screen].count     = ctx->inputbox.boxes[ctx->screen].box->size;
    ctx->inputbox.boxes[ctx->screen].box->size = 0;

    // Clear texts
    for (int i = 0; i < FONTS_NUM; ++i) {
        fs_vector_reset(ctx->texts[i].text);
    }

    // Clear areas
    fs_vector_reset(ctx->areas.area);
    ctx->areas.active = NO_SIGNAL;
}

static void fs_change_screen(fs_Context *ctx, int scr)
{
    fs_clear_screen(ctx);
    memset(&ctx->scroll, 0, sizeof(fs_Scroll));
    ctx->inputbox.boxes[ctx->screen].selected = NO_SIGNAL;
    ctx->screen                               = scr;
    ctx->scroll.depth                         = ctx->height;
}

void fs_init_font_atlas(fs_Context *ctx, FontType type, const char *font, float size)
{
    FT_Library ft_lib = NULL;
    FT_Face    face   = NULL;

    if (FT_Init_FreeType(&ft_lib) != 0) {
        printf("Error: failed to initialize FreeType library\n");
        exit(1);
    }

    if (FT_New_Face(ft_lib, font, 0, &face) != 0) {
        printf("Error: failed to load font\n");
        exit(1);
    }

    fs_Atlas atlas = { 0 };
    FT_Set_Char_Size(face, 0, size * 64, 144, 144);
    FT_GlyphSlot slot = face->glyph;
    atlas.line_height = face->size->metrics.height >> 6;
    unsigned int roww = 0, rowh = 0;

    // First pass: calculate atlas dimensions
    for (int i = 0; i < GLYPHS_NUM; ++i) {
        if (FT_Load_Char(face, i + 32, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD)) {
            fprintf(stderr, "Error: loading character %c failed\n", i + 32);
            continue;
        }

        unsigned int glyph_width = slot->bitmap.width;
        if (roww + glyph_width + 1 >= MAX_WIDTH) {
            atlas.tex_width   = atlas.tex_width > roww ? atlas.tex_width : roww;
            atlas.tex_height += rowh;
            roww              = 0;
            rowh              = 0;
        }
        roww += glyph_width + 1;
        rowh  = slot->bitmap.rows > rowh ? slot->bitmap.rows : rowh;
    }

    atlas.tex_width   = atlas.tex_width > roww ? atlas.tex_width : roww;
    atlas.tex_height += rowh;

    // Texture unit 0: font atlas. GL_RGB for subpixel rendering.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &atlas.tex_id);
    glBindTexture(GL_TEXTURE_2D, atlas.tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, atlas.tex_width, atlas.tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Paste all glyph bitmaps into the texture
    int ox = 0, oy = 0;
    rowh = 0;

    for (int i = 0; i < GLYPHS_NUM; ++i) {
        if (FT_Load_Char(face, i + 32, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD)) {
            fprintf(stderr, "Error: loading character %c failed\n", i + 32);
            continue;
        }

        unsigned int glyph_width = slot->bitmap.width / 3.0; // Adjust for RGB subpixel data
        if (ox + glyph_width + 1 >= MAX_WIDTH) {
            oy  += rowh;
            rowh = 0;
            ox   = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, glyph_width, slot->bitmap.rows, GL_RGB, GL_UNSIGNED_BYTE, slot->bitmap.buffer);

        atlas.glyphs[i].advance_x     = slot->advance.x >> 6;
        atlas.glyphs[i].bitmap_width  = glyph_width;
        atlas.glyphs[i].bitmap_height = slot->bitmap.rows;
        atlas.glyphs[i].bitmap_left   = slot->bitmap_left;
        atlas.glyphs[i].bitmap_top    = slot->bitmap_top;
        atlas.glyphs[i].offset_x      = ox;
        atlas.glyphs[i].offset_y      = oy;

        rowh = rowh > slot->bitmap.rows ? rowh : slot->bitmap.rows;
        ox  += glyph_width + 1;
    }

    // Texture unit 1: glyph coordinates and metrics
    GLfloat tex_data[GLYPHS_NUM * 4 * 2];
    for (int i = 0; i < GLYPHS_NUM; i++) {
        // The pixel coordinates of the bottom left corner, width and height of each glyph in the atlas
        tex_data[4 * i + 0] = atlas.glyphs[i].offset_x / (float)atlas.tex_width;
        tex_data[4 * i + 1] = atlas.glyphs[i].offset_y / (float)atlas.tex_height;
        tex_data[4 * i + 2] = atlas.glyphs[i].bitmap_width / (float)atlas.tex_width;
        tex_data[4 * i + 3] = atlas.glyphs[i].bitmap_height / (float)atlas.tex_height;
        // Glyph metrics
        tex_data[4 * GLYPHS_NUM + 4 * i + 0] = atlas.glyphs[i].bitmap_left / (float)atlas.tex_width;
        tex_data[4 * GLYPHS_NUM + 4 * i + 1] = atlas.glyphs[i].bitmap_top / (float)atlas.tex_height;
        tex_data[4 * GLYPHS_NUM + 4 * i + 2] = atlas.glyphs[i].bitmap_width / (float)atlas.tex_width;
        tex_data[4 * GLYPHS_NUM + 4 * i + 3] = -atlas.glyphs[i].bitmap_height / (float)atlas.tex_height;
    }

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &atlas.tex_metrics_id);
    glBindTexture(GL_TEXTURE_2D, atlas.tex_metrics_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GLYPHS_NUM, 2, 0, GL_RGBA, GL_FLOAT, tex_data);

    // Save kerning data if available - 0..255 to handle non-ASCII characters in text
    signed long table[256][256] = { 0 };
    if (FT_HAS_KERNING(face)) {
        // Populate kerning pairs
        for (int c1 = 0; c1 <= 255; c1++) {
            for (int c2 = 0; c2 <= 255; c2++) {
                FT_UInt   glyph_index1 = FT_Get_Char_Index(face, c1);
                FT_UInt   glyph_index2 = FT_Get_Char_Index(face, c2);
                FT_Vector kerning;
                // Get kerning value
                if (FT_Get_Kerning(face, glyph_index1, glyph_index2, FT_KERNING_DEFAULT, &kerning) != 0) {
                    kerning.x = 0; // Error getting kerning for 'c1' and 'c2'
                }
                ;
                // Store kerning adjustment (in 26.6 fixed-point format, convert to pixels)
                table[c1][c2] = kerning.x >> 6;
            }
        }
    }

    memcpy(&ctx->fonts[type], &atlas, sizeof(fs_Atlas));

    ctx->fonts[type].kerning_table = malloc(256 * sizeof(signed long *));
    for (int i = 0; i < 256; ++i) {
        ctx->fonts[type].kerning_table[i] = calloc(256, sizeof(signed long));
    }

    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            ctx->fonts[type].kerning_table[i][j] = table[i][j];
        }
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft_lib);
}

static void fs_init_fonts(fs_Context *ctx, fs_Fonts *fonts)
{
    for (FontType type = 0; type < FONTS_NUM; ++type) {
        fs_init_font_atlas(ctx, type, fonts[type].path, fonts[type].size);
        ctx->fonts[type].gamma = fonts[type].gamma;
    }
}

GLuint fs_load_shaders(const char *vertex_shader, const char *fragment_shader)
{
    GLint  result1, result2;
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader_ID, 1, (const char **)&vertex_shader, NULL);
    glCompileShader(vertex_shader_ID);
    glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &result1);

    glShaderSource(fragment_shader_ID, 1, (const char **)&fragment_shader, NULL);
    glCompileShader(fragment_shader_ID);
    glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &result2);

    if (!result1 || !result2) {
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return (0);
    }

    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);
    glGetProgramiv(program_ID, GL_LINK_STATUS, &result1);

    if (result1 == GL_FALSE) {
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return (0);
    }

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);
    return (program_ID);
}

void fs_init_context(fs_Context *ctx, char *title, int width, int height, vec4 colors[])
{
#ifndef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
    assert(0 && "Freetype must be compiled with subpixel rendering.");
#endif

    if (!glfwInit()) {
        assert(0 && "Error: failed to initialize GLFW\n");
    }
    glfwSetErrorCallback(fs_error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ctx->window = glfwCreateWindow(width, height, title, NULL, NULL);
    assert(ctx->window);
    ctx->width  = width;
    ctx->height = height;

    glfwMakeContextCurrent(ctx->window);
    glfwSetWindowUserPointer(ctx->window, ctx);
    glfwSetFramebufferSizeCallback(ctx->window, fs_resize_callback);
    glfwSetKeyCallback(ctx->window, fs_key_callback);
    glfwSetCharCallback(ctx->window, fs_char_callback);
    glfwSetMouseButtonCallback(ctx->window, fs_button_callback);
    glfwSetScrollCallback(ctx->window, fs_scroll_callback);
    glfwSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        assert(0 && "Error: failed to initialize GLAD");
    }

    // GL states
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const char *vertex_quad = "#version 330 core\n"
                              "uniform vec2 p0;\n"
                              "uniform vec2 p1;\n"
                              "uniform vec2 res_win;\n"
                              "uniform mat4 transform;\n"
                              "void main()\n"
                              "{\n"
                              "float x = gl_VertexID & 1;\n"
                              "float y = (gl_VertexID >> 1) & 1;\n"
                              "vec2 ndc_p0 = 2.0 * vec2(p0.x / res_win.x, (res_win.y - p0.y) / res_win.y) - 1.0;\n"
                              "vec2 ndc_p1 = 2.0 * vec2(p1.x / res_win.x, (res_win.y - p1.y) / res_win.y) - 1.0;\n"
                              "gl_Position = transform * vec4(ndc_p0 + vec2(x, y) * (ndc_p1 - ndc_p0), 0.0, 1.0);\n"
                              "}\n";

    const char *fragment_quad = "#version 330 core\n"
                                "uniform vec4 color;\n"
                                "out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                "FragColor = color;\n"
                                "}\n";

    const char *vertex_text = "#version 330 core\n"
                              "layout(location = 0) in vec2 vertexPosition;\n"
                              "layout(location = 1) in vec3 vertexInstance;\n"
                              "layout(location = 2) in vec3 vertexColor;\n"
                              "uniform sampler2D sampler_metrics;\n"
                              "uniform vec2 res_atlas;\n"
                              "uniform vec2 res_win;\n"
                              "uniform float glyphs_num;\n"
                              "uniform mat4 transform;\n"
                              "out vec3 textColor;\n"
                              "out vec2 uv;\n"
                              "void main()\n"
                              "{\n"
                              "vec4 q2 = texture(sampler_metrics, vec2((vertexInstance.z + 0.5) / glyphs_num, 0.75));\n"
                              "q2 *= vec4(res_atlas, res_atlas);\n"
                              "vec2 p = vertexPosition * q2.zw + q2.xy;\n"
                              "p += vertexInstance.xy;\n"
                              "p *= 2.0 / res_win;\n"
                              "gl_Position = transform * vec4(p, 0.0, 1.0);\n"
                              "vec4 q = texture(sampler_metrics, vec2((vertexInstance.z + 0.5) / glyphs_num, 0.0));\n"
                              "uv = q.xy + vertexPosition * q.zw;\n"
                              "textColor = vertexColor;\n"
                              "}\n";

    const char *fragment_text = "#version 330 core\n"
                                "in vec2 uv;\n"
                                "in vec3 textColor;\n"
                                "uniform vec2 res_atlas;\n"
                                "uniform sampler2D sampler_bitmap;\n"
                                "uniform float gamma;\n"
                                "out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                "float subpixel_offset = 1.0 / res_atlas.x / 3.0;\n"
                                "float r = texture(sampler_bitmap, uv + vec2(-subpixel_offset, 0.0)).r;\n"
                                "float g = texture(sampler_bitmap, uv).g;\n"
                                "float b = texture(sampler_bitmap, uv + vec2(subpixel_offset, 0.0)).b;\n"
                                "r = pow(r, 1.0 / gamma);\n"
                                "g = pow(g, 1.0 / gamma);\n"
                                "b = pow(b, 1.0 / gamma);\n"
                                "float alpha = r * 0.3 + g * 0.6 + b * 0.1;\n"
                                "FragColor = vec4(textColor, alpha);\n"
                                "}\n";

    // Rectangle quad shader program
    ctx->quad_shader.program = fs_load_shaders(vertex_quad, fragment_quad);
    assert(ctx->quad_shader.program);
    glGenVertexArrays(1, &ctx->quad_shader.vao);
    glBindVertexArray(ctx->quad_shader.vao);

    // Area quad shader program
    ctx->area_shader.program = fs_load_shaders(vertex_quad, fragment_quad);
    assert(ctx->area_shader.program);
    glGenVertexArrays(1, &ctx->area_shader.vao);
    glBindVertexArray(ctx->area_shader.vao);

    // Text shader program - VAO
    ctx->text_shader.program = fs_load_shaders(vertex_text, fragment_text);
    assert(ctx->text_shader.program);
    glGenVertexArrays(1, &ctx->text_shader.vao);
    glBindVertexArray(ctx->text_shader.vao);

    // Text shader program - VBO for quad
    GLfloat vbo_quad_data[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0 };
    glGenBuffers(1, &ctx->text_shader.vbo_quad);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->text_shader.vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_quad_data), vbo_quad_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribDivisor(0, 0); // Not instanced: resets every instance

    // Text shader program - VBO for glyph data
    glGenBuffers(1, &ctx->text_shader.vbo_instance_data);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->text_shader.vbo_instance_data);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribDivisor(1, 1); // Instanced vertex, advanced every instance

    // Text shader program - VBO for color
    glGenBuffers(1, &ctx->text_shader.vbo_instance_color);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->text_shader.vbo_instance_color);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribDivisor(2, 1); // Instanced vertex, advanced every instance

    // Constant uniforms
    glUseProgram(ctx->text_shader.program);
    glUniform1i(glGetUniformLocation(ctx->text_shader.program, "sampler_bitmap"), 0);
    glUniform1i(glGetUniformLocation(ctx->text_shader.program, "sampler_metrics"), 1);
    glUniform1f(glGetUniformLocation(ctx->text_shader.program, "glyphs_num"), GLYPHS_NUM);
    glUseProgram(0);

    // Set windows background color
    glClearColor(colors[0][0], colors[0][1], colors[0][2], colors[0][3]);

    // Initialize areas
    ctx->areas.area   = fs_vector_init(sizeof(fs_Area), VEC_INIT_CAP);
    ctx->areas.active = NO_SIGNAL;
    fs_vec4_copy(ctx->areas.col, colors[1]);

    // Init and set inputbox defaults
    for (int i = 0; i < SCREEN_NUM; ++i) {
        ctx->inputbox.boxes[i].box      = fs_vector_init(sizeof(fs_Box), VEC_INIT_CAP);
        ctx->inputbox.boxes[i].selected = NO_SIGNAL;
        ctx->inputbox.boxes[i].commited = NO_SIGNAL;
    }
    fs_vec4_copy(ctx->inputbox.bg_col, colors[2]);
    fs_vec4_copy(ctx->inputbox.sel_col, colors[3]);
    fs_vec4_copy(ctx->inputbox.fg_col, colors[4]);

    // Init and set button defaults
    ctx->buttons.button  = fs_vector_init(sizeof(fs_Button), VEC_INIT_CAP);
    ctx->buttons.clicked = NO_SIGNAL;
    fs_vec4_copy(ctx->buttons.normal_col, colors[5]);
    fs_vec4_copy(ctx->buttons.hover_col, colors[6]);
    fs_vec4_copy(ctx->buttons.text_col, colors[7]);

    // Init rectangles
    ctx->rects.rect = fs_vector_init(sizeof(fs_Rect), VEC_INIT_CAP);

    // Init texts
    for (int i = 0; i < FONTS_NUM; ++i) {
        ctx->texts[i].text = fs_vector_init(sizeof(fs_Text), VEC_INIT_CAP);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void fs_exit(fs_Context *ctx)
{
    // Free vectors
    fs_vector_free(ctx->areas.area);
    fs_vector_free(ctx->buttons.button);
    fs_vector_free(ctx->rects.rect);

    for (int i = 0; i < FONTS_NUM; ++i) {
        fs_vector_free(ctx->texts[i].text);

        for (size_t j = 0; j < GLYPHS_NUM; ++j) {
            free(ctx->fonts[i].kerning_table[j]);
        }
        free(ctx->fonts[i].kerning_table);
    }

    for (int i = 0; i < SCREEN_NUM; ++i) {
        fs_vector_free(ctx->inputbox.boxes[i].box);
    }

    // Free shader programs
    glDeleteVertexArrays(1, &ctx->quad_shader.vao);
    glDeleteProgram(ctx->quad_shader.program);

    glDeleteVertexArrays(1, &ctx->area_shader.vao);
    glDeleteProgram(ctx->area_shader.program);

    glDeleteBuffers(1, &ctx->text_shader.vbo_quad);
    glDeleteBuffers(1, &ctx->text_shader.vbo_instance_data);
    glDeleteBuffers(1, &ctx->text_shader.vbo_instance_color);
    glDeleteVertexArrays(1, &ctx->text_shader.vao);
    glDeleteProgram(ctx->text_shader.program);

    glfwTerminate();
    free(ctx);
}

#endif // FS_IMPLEMENTATION

#endif // FS_HEADER_
