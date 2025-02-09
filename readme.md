# PyVgmstream
## Introduction
This python library allows you to use [vgmstream](https://github.com/vgmstream/vgmstream) to convert sounds of your favourite game into playable formats in python and in memory (no need to store them to disk before conversion). 

## Usage
Before using this library, import it, and reads the sound you want to convert to a `bytes` object, whether from a game file or assets bundle.

Note that in many cases, a extension name is needed to help vgmstream figure out which format the sound file is in. I'll use Wwise `wem` for example.
```python
import pyvgmstream

game_sound = open(r"path/to/sound", "rb")
game_sound_bytes = game_sound.read() # game_sound_bytes is a bytes object
```
Currently, I have done 3 functions that you can use to convert sounds:
### `convert` function
This function is the basic function. It takes two parameters:
- `source`: bytes of the game sound, 
- `extension`: the extension of the game sound

returns a `bytes` object of converted sound in `.wav` format.

```python
converted_bytes = pyvgmstream.convert(game_sound_bytes, "wem") # the 2nd param is the extension name of the sound file
```

I recommend that this function is used to convert only one specific sound. For doing batch conversions, see next few functions.

### `convert_and_get_format_id` function
To find the way to decode a sound file, vgmstream tries through 500+ formats to find the one that decodes the file, which is redundant during batch conversions, since sounds in one game are generally in same format.

This function does the above preodure once and return the ID of the format the file is in. So for next conversions, we can simply skip decode tries and use that format directly to convert.

It takes two parameters:
- `source`: bytes of the game sound, 
- `extension`: the extension of the game sound.

It returns a tuple. 
- The first element is a `bytes` object of converted sound in `.wav` format. 
- The second element is an `int` indicating the input's format ID. You can pass it to next function.

```python
converted_bytes, format_id = pyvgmstream.convert_and_get_format_id(game_sound_bytes, "wem")
```

### `convert_with_format_id` function
This function takes the format ID we got in the previous function, and use it decode the sound file you pass to it.

It takes two parameters:
- `source`: bytes of the game sound, 
- `extension`: the extension of the game sound
- `format_id`: format ID we got in the previous function

```python
converted_bytes = pyvgmstream.convert_with_format_id(game_sound_bytes, "wem", format_id)
```
