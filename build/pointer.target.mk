# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := pointer
DEFS_Debug := \
	'-DNODE_GYP_MODULE_NAME=pointer' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-D__STDC_FORMAT_MACROS' \
	'-DBUILDING_NODE_EXTENSION' \
	'-DDEBUG' \
	'-D_DEBUG' \
	'-DV8_ENABLE_CHECKS'

# Flags passed to all source files.
CFLAGS_Debug := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-m64 \
	-g \
	-O0

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-fno-rtti \
	-fno-exceptions \
	-std=gnu++17

INCS_Debug := \
	-I/home/gustav/.cache/node-gyp/17.3.0/include/node \
	-I/home/gustav/.cache/node-gyp/17.3.0/src \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/openssl/config \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/openssl/openssl/include \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/uv/include \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/zlib \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/v8/include \
	-I$(srcdir)/node_modules/nan

DEFS_Release := \
	'-DNODE_GYP_MODULE_NAME=pointer' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-D__STDC_FORMAT_MACROS' \
	'-DBUILDING_NODE_EXTENSION'

# Flags passed to all source files.
CFLAGS_Release := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-m64 \
	-O3 \
	-fno-omit-frame-pointer

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-fno-rtti \
	-fno-exceptions \
	-std=gnu++17

INCS_Release := \
	-I/home/gustav/.cache/node-gyp/17.3.0/include/node \
	-I/home/gustav/.cache/node-gyp/17.3.0/src \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/openssl/config \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/openssl/openssl/include \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/uv/include \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/zlib \
	-I/home/gustav/.cache/node-gyp/17.3.0/deps/v8/include \
	-I$(srcdir)/node_modules/nan

OBJS := \
	$(obj).target/$(TARGET)/pointer.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-pthread \
	-rdynamic \
	-m64

LDFLAGS_Release := \
	-pthread \
	-rdynamic \
	-m64

LIBS := \
	-lX11

$(obj).target/pointer.node: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/pointer.node: LIBS := $(LIBS)
$(obj).target/pointer.node: TOOLSET := $(TOOLSET)
$(obj).target/pointer.node: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,solink_module)

all_deps += $(obj).target/pointer.node
# Add target alias
.PHONY: pointer
pointer: $(builddir)/pointer.node

# Copy this to the executable output path.
$(builddir)/pointer.node: TOOLSET := $(TOOLSET)
$(builddir)/pointer.node: $(obj).target/pointer.node FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/pointer.node
# Short alias for building this executable.
.PHONY: pointer.node
pointer.node: $(obj).target/pointer.node $(builddir)/pointer.node

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/pointer.node

