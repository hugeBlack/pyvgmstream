#include "config.h"
extern "C"
{
#include "src/vgmstream.h"
#include "src/api.h"
#include "src/util.h"
#include "src/util/samples_ops.h"
	//todo use <>?
#ifdef HAVE_JSON
#include "jansson/jansson.h"
#endif

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif


#include "version.h"
#ifndef VGMSTREAM_VERSION
#define VGMSTREAM_VERSION "unknown version " __DATE__
#endif
#define APP_NAME  "vgmstream CLI decoder " VGMSTREAM_VERSION
#define APP_INFO  APP_NAME " (" __DATE__ ")"

#include "streamfile_mem.h"

#ifdef __EMSCRIPTEN__
#define SAMPLE_BUFFER_SIZE  1024
#else
#define SAMPLE_BUFFER_SIZE  32768
#endif

#include "src/util/reader_sf.h"
}

int write_file(VGMSTREAM* vgmstream, MyFile& dest, cli_config* cfg);

int convert(MyFile& source, VgmConfig* cfg1, MyFile& dest, const char * inputFileExtension) {

    cli_config c = { 0 };
    copyCfg(cfg1, &c);
    cli_config* cfg = &c;

    VGMSTREAM* vgmstream = NULL;
    char outfilename_temp[PATH_LIMIT];
    int32_t len_samples;


    vgmstream_set_log_stdout(VGM_LOG_LEVEL_ALL);

    /* for plugin testing */
    if (cfg->validate_extensions) {
        int valid;
        vgmstream_ctx_valid_cfg vcfg = { 0 };

        vcfg.skip_standard = 0;
        vcfg.reject_extensionless = 0;
        vcfg.accept_unknown = 0;
        vcfg.accept_common = 0;

        valid = vgmstream_ctx_is_valid(cfg->infilename, &vcfg);
        if (!valid) goto fail;
    }

    /* open streamfile and pass subsong */

    {
        STREAMFILE* sf = get_mem_streamfile(source.buffer, source.size, inputFileExtension);

        if (!sf) {
            fprintf(stderr, "file %s not found\n", cfg->infilename);
            goto fail;
        }

        sf->stream_index = cfg->subsong_index;
        vgmstream = init_vgmstream_from_STREAMFILE(sf);

        close_streamfile(sf);



        if (!vgmstream) {
            fprintf(stderr, "failed opening %s\n", cfg->infilename);
            goto fail;
        }

        /* force load total subsongs if signalled */
        if (cfg->subsong_end == -1) {
            cfg->subsong_end = vgmstream->num_streams;
            close_vgmstream(vgmstream);
            return 1;
        }
    }


    /* modify the VGMSTREAM if needed (before printing file info) */
    apply_config(vgmstream, cfg);

    /* enable after config but before outbuf */
    if (cfg->downmix_channels) {
        vgmstream_mixing_autodownmix(vgmstream, cfg->downmix_channels);
    }
    else if (cfg->only_stereo >= 0) {
        vgmstream_mixing_stereo_only(vgmstream, cfg->only_stereo);
    }
    vgmstream_mixing_enable(vgmstream, SAMPLE_BUFFER_SIZE, NULL, NULL);

    /* get final play config */
    len_samples = vgmstream_get_samples(vgmstream);
    if (len_samples <= 0) {
        fprintf(stderr, "wrong time config\n");
        goto fail;
    }

    /* special values for loop testing */
    if (cfg->seek_samples1 == -2) { /* loop start...end */
        cfg->seek_samples1 = vgmstream->loop_start_sample;
    }
    if (cfg->seek_samples1 == -3) { /* loop end..end */
        cfg->seek_samples1 = vgmstream->loop_end_sample;
    }

    /* would be ignored by seek code though (allowed for seek_samples2 to test this) */
    if (cfg->seek_samples1 < -1 || cfg->seek_samples1 >= len_samples) {
        fprintf(stderr, "wrong seek config\n");
        goto fail;
    }

    if (cfg->play_forever && !vgmstream_get_play_forever(vgmstream)) {
        fprintf(stderr, "file can't be played forever");
        goto fail;
    }



    /* prints */
//#ifdef HAVE_JSON
//    if (!cfg->print_metajson) {
//#endif
//        print_info(vgmstream, cfg);
//        print_tags(cfg);
//        print_title(vgmstream, cfg);
//#ifdef HAVE_JSON
//    }
//    else {
//        print_json_info(vgmstream, cfg);
//        printf("\n");
//    }
//#endif

    /* prints done */
    if (cfg->print_metaonly) {
        close_vgmstream(vgmstream);
        return 1;
    }


    /* main decode */
    {
        write_file(vgmstream, dest, cfg);
    }



    close_vgmstream(vgmstream);
    return 1;

fail:
    close_vgmstream(vgmstream);
    return 0;
}

static void make_smpl_chunk(uint8_t* buf, int32_t loop_start, int32_t loop_end) {
    int i;

    memcpy(buf + 0, "smpl", 0x04); /* header */
    put_s32le(buf + 0x04, 0x3c); /* size */

    for (i = 0; i < 7; i++)
        put_s32le(buf + 0x08 + i * 0x04, 0);

    put_s32le(buf + 0x24, 1);

    for (i = 0; i < 3; i++)
        put_s32le(buf + 0x28 + i * 0x04, 0);

    put_s32le(buf + 0x34, loop_start);
    put_s32le(buf + 0x38, loop_end);
    put_s32le(buf + 0x3C, 0);
    put_s32le(buf + 0x40, 0);
}

static size_t make_wav_header(uint8_t* buf, size_t buf_size, int32_t sample_count, int32_t sample_rate, int channels, int smpl_chunk, int32_t loop_start, int32_t loop_end) {
    size_t data_size, header_size;

    data_size = sample_count * channels * sizeof(sample_t);
    header_size = 0x2c;
    if (smpl_chunk && loop_end)
        header_size += 0x3c + 0x08;

    if (header_size > buf_size)
        goto fail;

    memcpy(buf + 0x00, "RIFF", 0x04); /* RIFF header */
    put_u32le(buf + 0x04, (int32_t)(header_size - 0x08 + data_size)); /* size of RIFF */

    memcpy(buf + 0x08, "WAVE", 4); /* WAVE header */

    memcpy(buf + 0x0c, "fmt ", 0x04); /* WAVE fmt chunk */
    put_s32le(buf + 0x10, 0x10); /* size of WAVE fmt chunk */
    put_s16le(buf + 0x14, 0x0001); /* codec PCM */
    put_s16le(buf + 0x16, channels); /* channel count */
    put_s32le(buf + 0x18, sample_rate); /* sample rate */
    put_s32le(buf + 0x1c, sample_rate * channels * sizeof(sample_t)); /* bytes per second */
    put_s16le(buf + 0x20, (int16_t)(channels * sizeof(sample_t))); /* block align */
    put_s16le(buf + 0x22, sizeof(sample_t) * 8); /* significant bits per sample */

    if (smpl_chunk && loop_end) {
        make_smpl_chunk(buf + 0x24, loop_start, loop_end);
        memcpy(buf + 0x24 + 0x3c + 0x08, "data", 0x04); /* WAVE data chunk */
        put_u32le(buf + 0x28 + 0x3c + 0x08, (int32_t)data_size); /* size of WAVE data chunk */
    }
    else {
        memcpy(buf + 0x24, "data", 0x04); /* WAVE data chunk */
        put_s32le(buf + 0x28, (int32_t)data_size); /* size of WAVE data chunk */
    }

    /* could try to add channel_layout, but would need to write WAVEFORMATEXTENSIBLE (maybe only if arg flag?) */

    return header_size;
fail:
    return 0;
}

int write_file(VGMSTREAM* vgmstream, MyFile& dest, cli_config* cfg) {
    int32_t len_samples;
    sample_t* buf = NULL;
    int i;
    int channels, input_channels;


    channels = vgmstream->channels;
    input_channels = vgmstream->channels;

    vgmstream_mixing_enable(vgmstream, 0, &input_channels, &channels);

    /* last init */
    buf = (sample_t*) malloc(SAMPLE_BUFFER_SIZE * sizeof(sample_t) * input_channels);

    if (!buf) {
        fprintf(stderr, "failed allocating output buffer\n");
        goto fail;
    }

    /* simulate seek */
    len_samples = vgmstream_get_samples(vgmstream);
    if (cfg->seek_samples2 >= 0)
        len_samples -= cfg->seek_samples2;
    else if (cfg->seek_samples1 >= 0)
        len_samples -= cfg->seek_samples1;

    if (cfg->seek_samples1 >= 0)
        seek_vgmstream(vgmstream, cfg->seek_samples1);
    if (cfg->seek_samples2 >= 0)
        seek_vgmstream(vgmstream, cfg->seek_samples2);

    dest.reset(len_samples * sizeof(sample_t) * input_channels + 514);

    /* slap on a .wav header */
    if (!cfg->decode_only) {
        uint8_t wav_buf[0x100];
        size_t bytes_done;

        bytes_done = make_wav_header(wav_buf, 0x100,
            len_samples, vgmstream->sample_rate, channels,
            cfg->write_lwav, cfg->lwav_loop_start, cfg->lwav_loop_end);

        dest.write(wav_buf, sizeof(uint8_t), bytes_done);
    }


    /* decode */
    for (i = 0; i < len_samples; i += SAMPLE_BUFFER_SIZE) {
        int to_get = SAMPLE_BUFFER_SIZE;
        if (i + SAMPLE_BUFFER_SIZE > len_samples)
            to_get = len_samples - i;

        render_vgmstream(buf, to_get, vgmstream);

        if (!cfg->decode_only) {
            swap_samples_le(buf, channels * to_get); /* write PC endian */
            dest.write(buf, sizeof(sample_t) * channels, to_get);
        }
    }

    free(buf);
    return 1;
fail:
    free(buf);
    return 0;
}