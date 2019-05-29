#include "arducam_mipicamera.h"
#include <linux/v4l2-controls.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOG(fmt, args...) fprintf(stderr, fmt "\n", ##args)

void save_image(CAMERA_INSTANCE *camera_instance, const char *name) {
    //    IMAGE_FORMAT fmt = {IMAGE_ENCODING_RAW_BAYER, 90};
    IMAGE_FORMAT fmt = {IMAGE_ENCODING_JPEG, 90};
    BUFFER *buffer = arducam_capture(camera_instance, &fmt, 3000);
    if (!buffer) {
        LOG("capture timeout.");
        return;
    }
    FILE *file = fopen(name, "wb");
    fwrite(buffer->data, buffer->length, 1, file);
    fclose(file);
    arducam_release_buffer(buffer);
}

int main(int argc, char **argv) {
    CAMERA_INSTANCE camera_instance;

    int width = 0, height = 0;
    int exposure = 10000, gain = 1024;
    int focus = 0;

    int blue = 1660, red = 1660;

    int exposureParam;
    if (sscanf (argv[1], "%i", &exposureParam) != 1) {
        LOG("error - not an integer %d", exposureParam);
    }else 
        exposure = exposureParam;

    int gainParam;
    if (sscanf (argv[2], "%i", &gainParam) != 1) {
        LOG("error - not an integer %d", gainParam);
    }else 
        gain = gainParam;

    int focusParam;
    if (sscanf (argv[3], "%i", &focusParam) != 1) {
        LOG("error - not an integer %d", focusParam);
    }else 
        focus = focusParam;

    int blueParam;
    if (sscanf (argv[4], "%i", &blueParam) != 1) {
        LOG("error - not an integer %d", blueParam);
    }else 
        blue = blueParam;

    int redParam;
    if (sscanf (argv[5], "%i", &redParam) != 1) {
        LOG("error - not an integer %d", redParam);
    }else 
        red = redParam;


    char file_name[100];

    LOG("Open camera...");
    int res = arducam_init_camera(&camera_instance);
    if (res) {
        LOG("init camera status = %d", res);
        return -1;
    }

    width = 4192;
    height = 3120;
    LOG("Setting the resolution...");
    res = arducam_set_resolution(camera_instance, &width, &height);
    if (res) {
        LOG("set resolution status = %d", res);
        return -1;
    } else {
        LOG("Current resolution is %dx%d", width, height);
    }

    LOG("Start preview...");
    PREVIEW_PARAMS preview_params = {
        .fullscreen = 0,             // 0 is use previewRect, non-zero to use full screen
        .opacity = 255,              // Opacity of window - 0 = transparent, 255 = opaque
        .window = {0, 0, 1280, 720}, // Destination rectangle for the preview window.
    };
    
    res = arducam_start_preview(camera_instance, &preview_params);
    if (res) {
        LOG("start preview status = %d", res);
    }

    LOG("Reset the focus...");
    if (arducam_reset_control(camera_instance, V4L2_CID_FOCUS_ABSOLUTE)) {
        LOG("Failed to set focus, the camera may not support this control.");
    }

    ///usleep(1000 * 1000 * 2);


    LOG("Reset the exposure...");
    if (arducam_reset_control(camera_instance, V4L2_CID_EXPOSURE)) {
        LOG("Failed to reset exposure, the camera may not support this control.");
    }

    ///usleep(1000 * 1000 * 2);

    res = arducam_set_control(camera_instance, V4L2_CID_EXPOSURE, exposure);
    if (res) {
        LOG("set exposure status = %d", res);
    } else {
        LOG("Current exposure is %d", exposure);
    }

    ///usleep(1000 * 1000 * 2);


    res = arducam_set_control(camera_instance, V4L2_CID_GAIN, gain);
    if (res) {
        LOG("set gain status = %d", res);
    } else {
        LOG("Current gain is %d", gain);
    }

    ///usleep(1000 * 1000 * 2);


    res = arducam_set_control(camera_instance, V4L2_CID_FOCUS_ABSOLUTE, focus);
    if (res) {
        LOG("set focus status = %d", res);
    } else {
        LOG("Current focus is %d", focus);
    }

    LOG("Reset the blue balance...");
    if (arducam_reset_control(camera_instance, V4L2_CID_BLUE_BALANCE)) {
        LOG("Failed to reset blue balance, the camera may not support this control.");
    }

    res = arducam_set_control(camera_instance, V4L2_CID_BLUE_BALANCE, blue);
    if (res) {
        LOG("set blue status = %d", res);
    } else {
        LOG("Current blue is %d", blue);
    }

    LOG("Reset the red balance...");
    if (arducam_reset_control(camera_instance, V4L2_CID_RED_BALANCE)) {
        LOG("Failed to reset red balance, the camera may not support this control.");
    }

    res = arducam_set_control(camera_instance, V4L2_CID_RED_BALANCE, red);
    if (res) {
        LOG("set red status = %d", res);
    } else {
        LOG("Current red is %d", red);
    }


    ///usleep(1000 * 1000 * 2);


    //LOG("Stop preview...");
    //res = arducam_stop_preview(camera_instance);
    //if (res) {
    //    LOG("stop preview status = %d", res);
    //}


    sprintf(file_name, "%dx%d.jpg", width, height);
    LOG("Capture image %s...", file_name);
    save_image(camera_instance, file_name);

    LOG("Close camera...");
    res = arducam_close_camera(camera_instance);
    if (res) {
        LOG("close camera status = %d", res);
    }

    return 0;
}
