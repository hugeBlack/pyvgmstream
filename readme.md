# PyVgmstream
## Introduction
This python library allows you to use [vgmstream](https://github.com/hugeBlack/vgmstream) to convert sounds of your favourite game into playable formats in python and in memory (no need to store them to disk before conversion). 

I did some slight modifications to vgmstream to meet this project's requierment. For details, see [hugeBlack/vgmstream](https://github.com/hugeBlack/vgmstream)

## Usage
Before using this library, import it, and reads the sound you want to convert to a `bytes` object, whether from a game file or assets bundle.

Note that in many cases, a extension name is needed to help vgmstream figure out which format the sound file is in. I'll use Wwise `wem` for example.
```python
import pyvgmstream

game_sound = open(r"pass/to/sound", "rb")
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

### `convert_and_get_decode_function` function
To find the way to decode a sound file, vgmstream tries through 500+ formats to find the one that decodes the file, which is redundant during batch conversions, since sounds in one game are generally in same format.

This function does the above preodure once and return which decode function it uses. So for next conversions, we can simply skip decode tries and use that function directly to convert.

It takes two parameters:
- `source`: bytes of the game sound, 
- `extension`: the extension of the game sound.

It returns a tuple. 
- The first element is a `bytes` object of converted sound in `.wav` format. 
- The second element is a `VgmstreamDecodeFunction` that you can pass it to next function.

```python
converted_bytes, decode_func = pyvgmstream.convert_and_get_decode_function(game_sound_bytes, "wem")
```

### `convert_with_function` function
This function takes the decode function we got in the previous function, and use it decode the sound file you pass to it.

It takes two parameters:
- `source`: bytes of the game sound, 
- `extension`: the extension of the game sound
- `decode_function`: VgmstreamDecodeFunction we got in the previous function

```python
converted_bytes = pyvgmstream.convert_with_function(game_sound_bytes, "wem", decode_func)
```
