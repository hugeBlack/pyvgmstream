import pyvgmstream.libpyvgmstream as libpyvgmstream

VgmstreamDecodeFunction = libpyvgmstream.VgmStreamFunction

def convert(source: bytes, extension: str) -> bytes:
    return libpyvgmstream.convert(source, extension)

def convert_with_function(source: bytes, extension: str, decode_function: VgmstreamDecodeFunction) -> bytes:
    return libpyvgmstream.convertWithFunction(source, extension, decode_function)

def convert_and_get_decode_function(source: bytes, extension: str) -> tuple[bytes, VgmstreamDecodeFunction]:
    return libpyvgmstream.convertAndGetFunction(source, extension)