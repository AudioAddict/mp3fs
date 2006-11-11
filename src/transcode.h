/*
  FileTranscoder interface for MP3FS
  
  Copyright (C) David Collett (daveco@users.sourceforge.net)
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <FLAC/export.h>
#if !defined(FLAC_API_VERSION_CURRENT) || FLAC_API_VERSION_CURRENT < 8
#define LEGACY_FLAC
#else
#undef LEGACY_FLAC
#endif

#include <FLAC/metadata.h>
#ifdef LEGACY_FLAC
#include <FLAC/file_decoder.h>
#else
#include <FLAC/stream_decoder.h>
#endif
#include <lame/lame.h>
#include <id3tag.h>

#include "list.h"
#include "class.h"
#include "stringio.h"

#define MP3_QUALITY 5
#define FLAC_BLOCKSIZE 4608
#define BUFSIZE 2 * FLAC_BLOCKSIZE

// a list of currently opened files
struct FileTranscoder filelist;
FILE *logfd;
const char *basepath;
int bitrate;

/** This is used for debugging. */
#ifndef __DEBUG__
#define DEBUG(f, x, ...)
#else
#define DEBUG(f, x, ...) do {				 \
    fprintf(f, "%s:%u ",__FUNCTION__,__LINE__);		 \
    fprintf(f, x, ## __VA_ARGS__);			 \
    fflush(f);						 \
  } while(0)
#endif

CLASS(FileTranscoder, Object)
  StringIO buffer;
  int readptr;
  int framesize;
  int numframes;
  int totalsize;
  char *name;
  char *orig_name;
  struct id3_tag *id3tag;
  char id3v1tag[128];

  lame_global_flags *encoder;
#ifdef LEGACY_FLAC
  FLAC__FileDecoder *decoder;
#else
  FLAC__StreamDecoder *decoder;
#endif
  FLAC__StreamMetadata_StreamInfo info;
  short int lbuf[FLAC_BLOCKSIZE];
  short int rbuf[FLAC_BLOCKSIZE];
  unsigned char mp3buf[BUFSIZE];

  FileTranscoder METHOD(FileTranscoder, Con, char *filename);
  int METHOD(FileTranscoder, Read, char *buff, int offset, int len);
  int METHOD(FileTranscoder, Finish);

  // allow this object to be part of a list
  struct list_head list;
END_CLASS
