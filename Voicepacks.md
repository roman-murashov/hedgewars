# Voicepacks #
## Introduction ##
Voicepacks are collections of audio files which contain the taunts the hedgehogs say on various events.

## Creating voicepacks ##

A voicepack is a directory in `Data/Sounds/voices` and contains a bunch of audio files which are encoded in the Ogg Vorbis format.

Each audio file represents a single taunt which is played at certain events. Only files with the correct file names will be recognized as taunts, other files will be ignored. For a list of recognized file names, see [Taunts](Taunts.md).

Here is a guide with advice on the process of recording voices itself:
  * [Recording Voices for Hedgewars](http://www.hedgewars.org/node/2132)

## Special voicepacks ##
The voicepack “Default” is the English default voice.

Voicepacks with the name “`Default_<language>`” (where “`<language>`” is a language code) are played by default with respect to the player's language.

If you specify “Default” as voice for a team by using Lua scripting, Hedgewars will play the default voice with respect to the player's language. If no default voice for the local language is found, Hedgewars will use the English Default voicepack instead.

For example, in German Hedgewars, the voice “Default\_de” will be played by default (if it exists).