#include "config.h"
#define POSIXLY_CORRECT

extern "C"{
#include "src/vgmstream.h"
#include "src/api.h"
#include "src/util.h"
#include "src/util/samples_ops.h"

#include <stdio.h>
}


void copyCfg(VgmConfig* source, cli_config* destination) {
    destination->loop_count = source->loop_count;
    destination->fade_time = source->fade_time;
    destination->fade_delay = source->fade_delay;
    destination->ignore_loop = source->ignore_loop;
    destination->play_sdtout = source->play_sdtout;
    destination->play_wreckless = source->play_wreckless;
    destination->play_forever = source->play_forever;
    destination->print_metaonly = source->print_metaonly;
    destination->print_adxencd = source->print_adxencd;
    destination->print_oggenc = source->print_oggenc;
    destination->print_batchvar = source->print_batchvar;
    destination->force_loop = source->force_loop;
    destination->really_force_loop = source->really_force_loop;
    destination->write_lwav = source->write_lwav;
    destination->test_reset = source->test_reset;
    destination->only_stereo = source->only_stereo;
    destination->ignore_fade = source->ignore_fade;
    //destination->subsong_index = source->subsong_index;
    //destination->subsong_end = source->subsong_end;
    destination->tag_filename = source->tag_filename;
    destination->show_title = source->show_title;
    destination->seek_samples1 = source->seek_samples1;
    destination->seek_samples2 = source->seek_samples2;
    destination->decode_only = source->decode_only;
    destination->validate_extensions = source->validate_extensions;
    destination->downmix_channels = source->downmix_channels;

}

void apply_config(VGMSTREAM* vgmstream, cli_config* cfg) {
    vgmstream_cfg_t vcfg = { 0 };

    /* write loops in the wav, but don't actually loop it */
    if (cfg->write_lwav) {
        vcfg.disable_config_override = 1;
        cfg->ignore_loop = 1;

        if (vgmstream->loop_start_sample < vgmstream->loop_end_sample) {
            cfg->lwav_loop_start = vgmstream->loop_start_sample;
            cfg->lwav_loop_end = vgmstream->loop_end_sample;
            cfg->lwav_loop_end--; /* from spec, +1 is added when reading "smpl" */
        }
        else {
            /* reset for subsongs */
            cfg->lwav_loop_start = 0;
            cfg->lwav_loop_end = 0;
        }
    }
    /* only allowed if manually active */
    if (cfg->play_forever) {
        vcfg.allow_play_forever = 1;
    }

    vcfg.play_forever = cfg->play_forever;
    vcfg.fade_time = cfg->fade_time;
    vcfg.loop_count = cfg->loop_count;
    vcfg.fade_delay = cfg->fade_delay;

    vcfg.ignore_loop = cfg->ignore_loop;
    vcfg.force_loop = cfg->force_loop;
    vcfg.really_force_loop = cfg->really_force_loop;
    vcfg.ignore_fade = cfg->ignore_fade;



    vgmstream_apply_config(vgmstream, &vcfg);
}

MyFile::MyFile() : current(0), size(0) {
    buffer = nullptr;
}

MyFile::~MyFile() {
    if (buffer != nullptr) {
        free(buffer);
    }
}

void MyFile::write(void* src, int unit, int count) {
    int length = unit * count;
    if (!buffer) {
        buffer = (char*)malloc(length * 2);
        if (!buffer) {
            printf("Failed to allocate memory!1");
            return;
        }
        size = length >> 1;
    }

    if (current + length > size) {
        char* newBuf = (char*)malloc((current + length) * 2);
        if (!newBuf) {
            printf("Failed to allocate memory!2");
            return;
        }
        memcpy(newBuf, buffer, current);
        free(buffer);
        buffer = newBuf;
        size = (current + length) * 2;
        printf("Buffer reallocated! This should not happen!");
    }

    memcpy(buffer + current, src, length);
    current += length;

}

void MyFile::reset(int new_size) {
    if (buffer != nullptr) {
        free(buffer);
    }
    buffer = (char*)malloc(new_size);
    current = 0;
    size = new_size;
}