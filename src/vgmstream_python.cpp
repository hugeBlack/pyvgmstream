#include <pybind11/pybind11.h>
#include "core/config.h"
#include "core/convert.h"

namespace py = pybind11;

py::bytes pyConvert(const std::string& sourceBytes,const char* inputFileExtension, int fileFormat, int* outputFileFormat, VgmConfig* config) {
    MyFile source, dest;
    int l = sourceBytes.length() + 1;
    source.reset(l);
    sourceBytes.copy(source.buffer, l);
    convert(source, dest, inputFileExtension, fileFormat, config, outputFileFormat);
    py::bytes ans(dest.buffer, dest.current);

    return ans;
}

py::bytes pyConvert(const std::string& sourceBytes,const char* inputFileExtension) {
    VgmConfig c = { 0 };
    int outputFileFormat;
    return pyConvert(sourceBytes, inputFileExtension, -1, &outputFileFormat, &c);
}

py::bytes pyConvertWithFormat(const std::string& sourceBytes, const char* inputFileExtension, int fileFormat) {
    VgmConfig c = { 0 };
    return pyConvert(sourceBytes, inputFileExtension, fileFormat, 0, &c);
}

py::tuple pyConvertAndGetFormat(const std::string& sourceBytes,const char* inputFileExtension) {
    VgmConfig c = { 0 };
    int outputFileFormat;
    py::bytes ans1 = pyConvert(sourceBytes, inputFileExtension, -1, &outputFileFormat, &c);
    
    py::tuple ans(2);
    ans[0] = ans1;
    ans[1] = outputFileFormat;
    return ans;
}




PYBIND11_MODULE(libpyvgmstream, m) {
    py::class_<VgmConfig>(m, "VgmConfig")
        .def(py::init<>())
        .def_readwrite("loop_count", &VgmConfig::loop_count)
        .def_readwrite("fade_time", &VgmConfig::fade_time)
        .def_readwrite("fade_delay", &VgmConfig::fade_delay)
        .def_readwrite("ignore_loop", &VgmConfig::ignore_loop)
        .def_readwrite("play_sdtout", &VgmConfig::play_sdtout)
        .def_readwrite("play_wreckless", &VgmConfig::play_wreckless)
        .def_readwrite("play_forever", &VgmConfig::play_forever)
        .def_readwrite("print_metaonly", &VgmConfig::print_metaonly)
        .def_readwrite("print_adxencd", &VgmConfig::print_adxencd)
        .def_readwrite("print_oggenc", &VgmConfig::print_oggenc)
        .def_readwrite("print_batchvar", &VgmConfig::print_batchvar)
        .def_readwrite("force_loop", &VgmConfig::force_loop)
        .def_readwrite("really_force_loop", &VgmConfig::really_force_loop)
        .def_readwrite("write_lwav", &VgmConfig::write_lwav)
        .def_readwrite("test_reset", &VgmConfig::test_reset)
        .def_readwrite("only_stereo", &VgmConfig::only_stereo)
        .def_readwrite("ignore_fade", &VgmConfig::ignore_fade)
        //.def_readwrite("subsong_index", &VgmConfig::subsong_index)
        //.def_readwrite("subsong_end", &VgmConfig::subsong_end)
        .def_readwrite("tag_filename", &VgmConfig::tag_filename)
        .def_readwrite("show_title", &VgmConfig::show_title)
        .def_readwrite("seek_samples1", &VgmConfig::seek_samples1)
        .def_readwrite("seek_samples2", &VgmConfig::seek_samples2)
        .def_readwrite("decode_only", &VgmConfig::decode_only)
        .def_readwrite("validate_extensions", &VgmConfig::validate_extensions)
        .def_readwrite("downmix_channels", &VgmConfig::downmix_channels);

    m.def("convertAndGetFormat", pyConvertAndGetFormat, "convert sourceBytes to .wav files and get file format ID");
    m.def("convertWithFormat", pyConvertWithFormat, "convert sourceBytes to .wav files with file format");

    m.def("convert", py::overload_cast<const std::string &, const char*>(pyConvert), "convert sourceBytes to .wav files, with default config");
}