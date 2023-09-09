#include <libdragon.h>

volatile bool update = false;
uint32_t H_START = 0;
uint32_t H_END = 0;
uint32_t V_START = 0;
uint32_t V_END = 0;
uint32_t H_VIDEO = 0;
uint32_t V_VIDEO = 0;

void update_vi_regs(void) {
    // Update VI registers under vblank interrupt
    if (update) {
        volatile uint32_t *vi_regs = (volatile uint32_t *)0xA4400000;
        H_VIDEO = vi_regs[9] = (H_START << 16) | (H_END & 0xFFFF);
        V_VIDEO = vi_regs[10] = (V_START << 16) | (V_END & 0xFFFF);
        update = false;
    }
}

int main() {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    register_VI_handler(update_vi_regs);
    controller_init();

    volatile uint32_t *vi_regs = (volatile uint32_t *)0xA4400000;
    H_VIDEO = vi_regs[9];
    V_VIDEO = vi_regs[10];
    H_START = (H_VIDEO >> 16) & 0xFFFF;
    H_END = H_VIDEO & 0xFFFF;
    V_START = (V_VIDEO >> 16) & 0xFFFF;
    V_END = V_VIDEO & 0xFFFF;

    while (1) {
        surface_t *fb = display_get();
        graphics_fill_screen(fb, graphics_make_color(0xA6, 0x80, 0x8C, 255));
        graphics_draw_line(fb, 0, 0, 319, 0, graphics_make_color(0xD6,0xCF,0xCB, 255));
        graphics_draw_line(fb, 0, 239, 319, 239, graphics_make_color(0xD6,0xCF,0xCB, 255));
        graphics_draw_line(fb, 0, 0, 0, 239, graphics_make_color(0xD6,0xCF,0xCB, 255));
        graphics_draw_line(fb, 319, 0, 319, 239, graphics_make_color(0xD6,0xCF,0xCB, 255));

        graphics_set_color(graphics_make_color(0x1F, 0x20, 0x41, 0xFF), 0);

        char buf[64];
        sprintf(buf, "H_VIDEO: %08lx\n", H_VIDEO);
        graphics_draw_text(fb, 100, 100, buf);
        sprintf(buf, "V_VIDEO: %08lx\n", V_VIDEO);
        graphics_draw_text(fb, 100, 110, buf);

        controller_scan();
        struct controller_data keys = get_keys_down();

        disable_interrupts();
        if (keys.c[0].up)      { V_START -= 1; update = true; }
        if (keys.c[0].down)    { V_START += 1; update = true; }
        if (keys.c[0].left)    { H_START -= 1; update = true; }
        if (keys.c[0].right)   { H_START += 1; update = true; }
        if (keys.c[0].C_up)    { V_END -= 1;   update = true; }
        if (keys.c[0].C_down)  { V_END += 1;   update = true; }
        if (keys.c[0].C_left)  { H_END -= 1;   update = true; }
        if (keys.c[0].C_right) { H_END += 1;   update = true; }
        enable_interrupts();
        
        display_show(fb);
    }
}
