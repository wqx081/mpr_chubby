#ifndef C_FFMPEG_API_H_
#define C_FFMPEG_API_H_

typedef enum FFmpegStatus {
  STATUS_OK = 0,
  // For option parser
  STATUS_NO_FILE_SPECIFIED,
  STATUS_NO_OUTPUT_FILE,

  STATUS_INVALID_ARGUMENT,
  // For Decoder
  STATUS_DECODER_AUDIO_ERROR,
  STATUS_DECODER_VIDEO_ERROR,

  STATUS_TRANSCODE_ERROR,
  // TODO
} FFmpegStatus;

void InitializeFFmpeg();
FFmpegStatus FFmpegTranscode(int argc, char** argv);

#endif // C_FFMPEG_API_H_
