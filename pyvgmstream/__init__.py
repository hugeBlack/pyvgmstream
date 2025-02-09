import pyvgmstream.libpyvgmstream as libpyvgmstream

def convert(source: bytes, extension: str) -> bytes:
    return libpyvgmstream.convert(source, extension)

def convert_with_format_id(source: bytes, extension: str, format_id: int) -> bytes:
    return libpyvgmstream.convertWithFormat(source, extension, format_id)

def convert_and_get_format_id(source: bytes, extension: str) -> tuple[bytes, int]:
    return libpyvgmstream.convertAndGetFormat(source, extension)