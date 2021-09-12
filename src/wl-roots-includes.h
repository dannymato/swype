#pragma once
#include <wayland-server.h>
#define static
extern "C" {
    #include <wlr/backend.h>
    #include <wlr/backend/wayland.h>
    #include <wlr/util/log.h>
    #include <wlr/render/wlr_renderer.h>
    #include <wlr/types/wlr_compositor.h>
    #include <wlr/types/wlr_data_device.h>
    #include <wlr/types/wlr_output_layout.h>
    #include <wlr/types/wlr_xdg_shell.h>
    #include <wlr/types/wlr_matrix.h>
    #include <wlr/types/wlr_cursor.h>
    #include <wlr/types/wlr_xcursor_manager.h>
}
#undef static