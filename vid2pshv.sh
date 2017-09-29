#!/bin/sh

on_exit() {
  echo Deleting temp files...
  rm -fv /tmp/audiotrack-$$.ogg /tmp/baseline-$$.264
}

echo vid2pshv - PSHV Encoder
echo -----------------------

trap on_exit EXIT INT KILL

echo Starting BSF stream extraction through ffmpeg, please wait...
ffmpeg -hide_banner -i "$1"\
 -r 30000/1001 -vf scale=960:544:force_original_aspect_ratio=decrease -b:v "${VB:-2}M" -maxrate "${VB:-2}M" -bufsize 2M\
 -c:v libx264 -x264opts "aud=1" -crf 18 -profile:v baseline -preset:v veryslow -g 15 -pix_fmt yuv420p\
 /tmp/baseline-$$.264 || exit

echo Starting audio extraction through ffmpeg, please wait...
ffmpeg -hide_banner -i "$1"\
 -acodec: libvorbis -ac "${AC:-2}" -ar "${AR:-48000}" -vn\
 /tmp/audiotrack-$$.ogg || exit

echo Starting video encoding, please wait...
pshv_encoder 29.970 /tmp/audiotrack-$$.ogg /tmp/baseline-$$.264 `basename "$1"`.pshv || exit

echo Video converted successfully, you\'ll find an `basename "$1"`.pshv file.
