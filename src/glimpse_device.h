/*
 * Copyright (C) 2017 Glimp IP Ltd
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "glimpse_context.h"

enum gm_device_event_type
{
    GM_DEV_EVENT_FRAME_READY
};

struct gm_device_event
{
    enum gm_device_event_type type;

    union {
        struct {
            uint64_t met_requirements;
        } frame_ready;
    };
};


enum gm_device_type {
    GM_DEVICE_KINECT,
    GM_DEVICE_RECORDING,
};

struct gm_device_config {
    enum gm_device_type type;
    union {
        struct {
            int device_number;
        } kinect;
        struct {
            const char *path;
        } recording;
    };
};

#ifdef __cplusplus
extern "C" {
#endif

struct gm_device *
gm_device_open(struct gm_logger *log,
               struct gm_device_config *config,
               char **err);

struct gm_ui_properties *
gm_device_get_ui_properties(struct gm_device *dev);

struct gm_intrinsics *
gm_device_get_depth_intrinsics(struct gm_device *dev);

struct gm_intrinsics *
gm_device_get_video_intrinsics(struct gm_device *dev);

struct gm_extrinsics *
gm_device_get_depth_to_video_extrinsics(struct gm_device *dev);

void
gm_device_set_event_callback(struct gm_device *dev,
                             void (*event_callback)(struct gm_device *dev,
                                                    struct gm_device_event *event,
                                                    void *user_data),
                             void *user_data);

/* It's expected that events aren't synchronously handed within the above
 * event callback considering that it's undefined what thread the callback
 * is invoked on and it's undefined what locks might be held during the
 * invocation whereby the device api may not be reentrant at that point.
 *
 * An event will likely be queued for processing later but when processing
 * is finished then the event structure needs to be freed with this api:
 */
void gm_device_event_free(struct gm_device_event *event);

void gm_device_start(struct gm_device *dev);
// TODO add _stop() api also

void gm_device_close(struct gm_device *dev);

void
gm_device_request_frame(struct gm_device *dev, uint64_t requirements);

struct gm_frame *
gm_device_get_latest_frame(struct gm_device *dev);

#ifdef __cplusplus
}
#endif
