#define FS_IMPLEMENTATION
#include "fs.h"

// Window size
#define WIDTH  1600
#define HEIGHT 900

// UI colors
#define AREA_BG    {0.0, 0.0, 0.0, 0.9}
#define WINDOW_BG  {0.0, 0.1, 0.2, 1.0}
#define BOX_NORMAL {0.9, 0.9, 0.9, 1.0}
#define BOX_SELECT {0.6, 0.8, 0.8, 1.0}
#define BOX_TEXT   {0.0, 0.0, 0.0, 1.0}
#define BTN_NORMAL {0.0, 0.7, 0.3, 1.0}
#define BTN_HOVER  {0.1, 0.9, 0.6, 1.0}
#define BTN_TEXT   {0.0, 0.0, 0.0, 1.0}

// Fonts
#define FONT_UI    "C:/Windows/Fonts/segoeui.ttf"
#define FONT_MONO  "C:/Windows/Fonts/consola.ttf"

// Chart parameters
#define MAX_BARS 20            // Max number of bars
#define CHART_H  HEIGHT * 0.55 // Scaling chart height
#define STR_LEN  64
#define BLACK    {0.0, 0.0, 0.0, 1.0}
#define CYAN     {0.1, 0.5, 0.5, 1.0}
#define GREEN    {0.1, 0.5, 0.1, 1.0}
#define RED      {0.5, 0.1, 0.1, 1.0}
#define WHITE    {0.9, 0.9, 0.9, 1.0}

typedef struct
{
    struct
    {
        char item[STR_LEN]; // Item name
        int value;          // Item value
    } elem[MAX_BARS];
    int baseline;        // Baseline y coord
    char title[STR_LEN]; // Chart title
    float abs_h;         // Height of chart
    float bar_w;         // Bar width
    int data_num;        // Number of bars
} ChartData;

void chart_metrics(ChartData *data)
{
    float barmin = 0, barmax = 0, abs_h = 0, prev_val = 0;
    for (int i = 0; i < data->data_num; ++i)
    {
        prev_val += data->elem[i].value;
        if (prev_val < barmin)
        {
            barmin = prev_val;
        }
        else if (prev_val > barmax)
        {
            barmax = prev_val;
        }
    }
    data->abs_h = barmax - barmin;
    data->bar_w = WIDTH * 0.8 / data->data_num;
}

void load_data(ChartData *data)
{
    char line[STR_LEN];
    char *token, *next_token;

    FILE *fp = fopen("chart_data.csv", "r");
    assert(fp);
    // Get title
    fgets(line, STR_LEN, fp);
    token = strtok_s(line, "\n", &next_token);
    strcpy_s(data->title, STR_LEN, token);
    // Get data
    while (fgets(line, STR_LEN, fp) != NULL)
    {
        token = strtok_s(line, ",", &next_token);
        strcpy_s(data->elem[data->data_num].item, STR_LEN, token);
        token = strtok_s(NULL, "\n", &next_token);
        data->elem[data->data_num].value = atof(token);
        data->data_num++;
    }
    fclose(fp);

    chart_metrics(data);
}

void area_value(fs_Context *ctx)
{
    char *text = "Change value of items.\nThe last value (bar) is computed.";
    fs_add_area_text(ctx, text, (vec4)WHITE);
}

void area_baseline_screen_0(fs_Context *ctx)
{
    char *text = "Adjust position of axis X on chart.";
    fs_add_area_text(ctx, text, (vec4)WHITE);
}

void area_baseline_screen_1(fs_Context *ctx)
{
    char *text = "Hit ENTER to update chart.";
    fs_add_area_text(ctx, text, (vec4)WHITE);
}

void save_chart_ppm(fs_Context *ctx) {
    FILE *fp = fopen("chart.ppm", "wb");
    assert(fp);

    unsigned char *buffer = (unsigned char *)calloc(ctx->width * ctx->height * 3, sizeof(unsigned char));
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, ctx->width, ctx->height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    int rowSize = ctx->width * 3;
    unsigned char* tempRow = malloc(rowSize);
    // Flip image - swap row y with row (height - 1 - y)
    for (int y = 0; y < ctx->height / 2; ++y) {
        memcpy(tempRow, buffer + y * rowSize, rowSize);
        memcpy(buffer + y * rowSize, buffer + (ctx->height - 1 - y) * rowSize, rowSize);
        memcpy(buffer + (ctx->height - 1 - y) * rowSize, tempRow, rowSize);
    }
    free(tempRow);

    fprintf(fp, "P6\n"); // P6 format
    fprintf(fp, "%d %d\n", ctx->width, ctx->height); // image size
    fprintf(fp, "%d\n", 255); // rgb component depth = 255
	fwrite(buffer, sizeof(unsigned char), ctx->width * ctx->height * 3, fp);

    fclose(fp);
    free(buffer);
}

void screen_0(fs_Context *ctx, ChartData *data)
{
    // Button
    fs_add_button(ctx, (vec4){100, 50, 320, 80}, "Add item", MEDIUM);
    fs_add_button(ctx, (vec4){520, 50, 320, 80}, "Remove item", MEDIUM);
    fs_add_button(ctx, (vec4){1200, 50, 300, 80}, "Show chart", MEDIUM);

    // Text
    fs_add_text(ctx, (vec2){260, 250}, "Description", SMALL, (vec4)WHITE, ALIGN_CENTER);
    fs_add_text(ctx, (vec2){800, 250}, "Value", SMALL, (vec4)WHITE, ALIGN_CENTER);
    fs_add_text(ctx, (vec2){970, 250}, "Baseline", SMALL, (vec4)WHITE, ALIGN_LEFT);

    // Baseline
    fs_add_inputbox(ctx, (vec4){970, 300, 110, 50}, NUM);

    // Chart data
    for (int i = 0; i < data->data_num; ++i)
    {
        fs_add_inputbox(ctx, (vec4){150, 300 + i * 60, 500, 50}, TXT);
        fs_add_inputbox(ctx, (vec4){750, 300 + i * 60, 120, 50}, NUM);
    }

    // Add hover area
    fs_add_area(ctx, (vec4){750, 200, 150, 50}, area_value);
    fs_add_area(ctx, (vec4){970, 200, 250, 50}, area_baseline_screen_0);
}

void screen_1(fs_Context *ctx, ChartData *data, int flag)
{
    char buf[STR_LEN];
    float x, y, w, h, xpos, ypos = 0;
    vec4 col;
    int i, prev_val = 0;

    // Draw chart
    for (i = 0; i < data->data_num - 1; ++i)
    {
        if (data->elem[i].value < 0)
        {
            memcpy(&col, (vec4)RED, sizeof(vec4));
        }
        else
        {
            memcpy(&col, (vec4)GREEN, sizeof(vec4));
        }
        if (i == 0)
        {
            memcpy(&col, (vec4)CYAN, sizeof(vec4));
        }

        x = WIDTH * 0.1 + i * data->bar_w;
        y = data->baseline + prev_val / data->abs_h * CHART_H;
        w = data->bar_w * 0.9;
        h = -data->elem[i].value / data->abs_h * CHART_H;
        fs_add_rect(ctx, (vec4){x, y, w, h}, col);

        xpos = x + w / 2.0;
        ypos = y + h / 2.0 + ctx->fonts[MONO].line_height / 2.0;
        fs_add_text(ctx, (vec2){xpos, ypos}, data->elem[i].item, MONO, (vec4)WHITE, ALIGN_CENTER);
        sprintf(buf, "%d", data->elem[i].value);
        fs_add_text(ctx, (vec2){xpos, ypos + ctx->fonts[MONO].line_height}, buf, MONO, (vec4)WHITE, ALIGN_CENTER);
        prev_val -= data->elem[i].value;
    }
    // And the last bar
    x = WIDTH * 0.1 + i * data->bar_w;
    w = data->bar_w * 0.9;
    h = prev_val / data->abs_h * CHART_H;
    xpos = x + w / 2.0;
    ypos = data->baseline + h / 2.0 + ctx->fonts[MONO].line_height / 2.0;
    fs_add_rect(ctx, (vec4){x, data->baseline, w, h}, (vec4)CYAN);
    fs_add_text(ctx, (vec2){xpos, ypos}, data->elem[i].item, MONO, (vec4)WHITE, ALIGN_CENTER);
    sprintf(buf, "%d", -prev_val);
    fs_add_text(ctx, (vec2){xpos, ypos + ctx->fonts[MONO].line_height}, buf, MONO, (vec4)WHITE, ALIGN_CENTER);

    // Axis x and y
    fs_add_rect(ctx, (vec4){WIDTH * 0.1, data->baseline, WIDTH * 0.8, 2}, (vec4)WHITE);
    fs_add_rect(ctx, (vec4){WIDTH * 0.1, HEIGHT * 0.1, 2, HEIGHT * 0.8}, (vec4)WHITE);

    // Title
    fs_add_text(ctx, (vec2){WIDTH / 2.0, 70}, data->title, BIG, (vec4)WHITE, ALIGN_CENTER);

    // Add hover area
    fs_add_area(ctx, (vec4){0, 0, 150, 50}, area_baseline_screen_1);

    if (flag)
    {
        fs_add_button(ctx, (vec4){1340, 50, 200, 80}, "BACK", MEDIUM);
        fs_add_inputbox(ctx, (vec4){10, 10, 120, 40}, NUM);
    }
}

void dashboard(fs_Context *ctx, ChartData *data)
{
    if (SCREEN == 0)
    {
        switch (BUTTON_CLICKED)
        {
        case 0: // Button: add item
            if (data->data_num < MAX_BARS - 1)
            {
                data->data_num++;
                fs_clear_screen(ctx);
                screen_0(ctx, data);
            }
            break;
        case 1: // Button: remove item
            if (data->data_num > 3)
            {
                data->data_num--;
                fs_clear_screen(ctx);
                screen_0(ctx, data);
            }
            break;
        case 2: // Button: display and save chart
            // Update chart data
            data->baseline = atof(fs_get_inputbox_content(ctx, 0));
            for (int i = 0; i < data->data_num; ++i)
            {
                strcpy_s(data->elem[i].item, STR_LEN, fs_get_inputbox_content(ctx, i * 2 + 1));
                data->elem[i].value = atof(fs_get_inputbox_content(ctx, i * 2 + 2));
            }
            chart_metrics(data);
            // Initialize screen_1
            fs_change_screen(ctx, 1);
            screen_1(ctx, data, 1);
            char buf[32];
            sprintf(buf, "%d", data->baseline);
            // Update inputbox in screen_1
            fs_set_inputbox_content(ctx, 0, buf);
            break;
        }
    }
    else if (SCREEN == 1)
    {
        if (BUTTON_CLICKED == 0)
        {
            // Save chart without controls
            fs_clear_screen(ctx);
            screen_1(ctx, data, 0);
            fs_render_ui(ctx);
            save_chart_ppm(ctx);
            // Go back
            fs_change_screen(ctx, 0);
            screen_0(ctx, data);
            char buf[32];
            sprintf(buf, "%d", data->baseline);
            fs_set_inputbox_content(ctx, 0, buf);
        }
        if (COMMITED_BOX == 1)
        {
            data->baseline = atof(fs_get_inputbox_content(ctx, 0));
            chart_metrics(data);
            // Initialize sreen 1
            fs_clear_screen(ctx);
            screen_1(ctx, data, 1);
            char buf[32];
            sprintf(buf, "%d", data->baseline);
            fs_set_inputbox_content(ctx, 0, buf);
        }
    }
}

int main(int argc, char **argv)
{
    // Default UI colors
    vec4 fs_colors[] = {
        WINDOW_BG,  // Windows background color
        AREA_BG,    // Hover area background color
        BOX_NORMAL, // Inputbox normal color
        BOX_SELECT, // Inputbox selected color
        BOX_TEXT,   // Inputbox text color
        BTN_NORMAL, // Button normal color
        BTN_HOVER,  // Button hover color
        BTN_TEXT,   // Button text color
    };

    fs_Context *ctx = calloc(1, sizeof(fs_Context));
    fs_init_context(ctx, "Waterfall chart", WIDTH, HEIGHT, fs_colors);

    fs_Fonts fonts[FONTS_NUM] = {
        {   .path = FONT_UI, .size = 35, .gamma = 1.5}, // Medium
        {   .path = FONT_UI, .size = 70, .gamma = 1.5}, // Big
        {   .path = FONT_UI, .size = 25, .gamma = 1.5}, // Small
        { .path = FONT_MONO, .size = 22, .gamma = 1.5}, // Mono
        { .path = FONT_MONO, .size = 22, .gamma = 1.5}, // Inputbox
        { .path = FONT_MONO, .size = 20, .gamma = 1.5}, // Hover
    };

    fs_init_fonts(ctx, fonts);

    ChartData data = {0};
    load_data(&data);
    screen_0(ctx, &data);

    // Load initial data
    fs_set_inputbox_content(ctx, 0, "400");
    for (int i = 0; i < data.data_num; ++i)
    {
        char buf[STR_LEN];
        fs_set_inputbox_content(ctx, i * 2 + 1, data.elem[i].item);
        sprintf(buf, "%d", data.elem[i].value);
        fs_set_inputbox_content(ctx, i * 2 + 2, buf);
    }

    while (!glfwWindowShouldClose(ctx->window))
    {
        dashboard(ctx, &data);
        fs_render_ui(ctx);
    }

    fs_exit(ctx);
    return EXIT_SUCCESS;
}
