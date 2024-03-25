#pragma once
extern "C" {
    #include "src/vgmstream.h"
}

typedef struct {
    char** infilenames;
    int infilenames_count;
    const char* infilename;

    const char* outfilename_config;
    const char* outfilename;

    const char* tag_filename;

    int play_forever;
    int play_sdtout;
    int play_wreckless;
    int print_metaonly;

    int print_adxencd;
    int print_oggenc;
    int print_batchvar;
    int write_lwav;
    int only_stereo;
    int subsong_index;
    int subsong_end;

    double loop_count;
    double fade_time;
    double fade_delay;
    int ignore_fade;
    int ignore_loop;
    int force_loop;
    int really_force_loop;

    int validate_extensions;
    int test_reset;
    int seek_samples1;
    int seek_samples2;
    int decode_only;
    int show_title;
    int downmix_channels;

    /* not quite config but eh */
    int lwav_loop_start;
    int lwav_loop_end;
} cli_config;

typedef struct {
    double loop_count;
    double fade_time;
    double fade_delay;
    int ignore_loop;
    int play_sdtout;
    int play_wreckless;
    int play_forever;
    int print_metaonly;
    int print_adxencd;
    int print_oggenc;
    int print_batchvar;
    int force_loop;
    int really_force_loop;
    int write_lwav;
    int test_reset;
    int only_stereo;
    int ignore_fade;
    int subsong_index;
    int subsong_end;
    char* tag_filename;
    int show_title;
    int seek_samples1;
    int seek_samples2;
    int decode_only;
    int validate_extensions;
    int downmix_channels;
} VgmConfig;

class MyFile {
public:
    MyFile();
    ~MyFile();
    void write(void* src, int unit, int count);
    void reset(int new_size);

    char* buffer;
    int current;
    int size;
};

void copyCfg(VgmConfig* source, cli_config* destination);

void apply_config(VGMSTREAM* vgmstream, cli_config* cfg);
