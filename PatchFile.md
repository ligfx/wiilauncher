# GPF File Format #

In order to make a GPF (Gecko Patch File) you will need to use a hex editor. The file should be given named with the game ID and gpf extension and put in a patch folder, that is on the main folder of your SD card. Here is the patch file format:

  1. [byte](1.md) - Number of patches (hex)
  1. [bytes](4.md) - Patch Destination (hex)
  1. [bytes](4.md) - Patch Size (hex)
  1. [X](Patch.md) - Patch data size of Patch Size

## Example ##

  * 02 - Number of Patches
  * 91000000 - Destination Address
  * 00000002 - Patch Size (this example 2 bytes)
  * AABB - The Patch (with the size of the first patch size)
  * 91000004 - Destination Address (for the second patch)
  * 00000002 - Patch Size (for the second patch)
  * CCDD - The Patch (for the second patch)

The format will be just a hex blob like this:

```
029100000000000002AABB9100000200000002CCDD
```

and saved to folder 'patch' on the SD card, with the GameID being the filename with .gpf extension, so for red steel jap, the file name would be REDJ41.gpf placed in folder 'patch'.