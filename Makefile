BUILD_DIR = build
include $(N64_INST)/include/n64.mk

src = main.c
assets_png = $(wildcard assets/*.png)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))

MKSPRITE_FLAGS ?= --compress

all: imagesize.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

#$(BUILD_DIR)/imagesize.dfs: $(assets_conv)
$(BUILD_DIR)/imagesize.elf: $(src:%.c=$(BUILD_DIR)/%.o)

imagesize.z64: N64_ROM_TITLE = "Imagesize Test"
#imagesize.z64: $(BUILD_DIR)/imagesize.dfs

clean:
	rm -rf $(BUILD_DIR) filesystem *.z64

-include $(wildcard $(BUILD_DIR)/*.d))

.PHONY: clean all
