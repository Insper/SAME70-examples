/**
 * \file
 *
 * \brief Virtual file system management.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef __FSDATA_H__
#define __FSDATA_H__

struct fsdata_file {
  const struct fsdata_file *next;
  const unsigned char *name;
  const unsigned char *data;
  const int len;
};

struct fsdata_file_noconst {
  struct fsdata_file *next;
  unsigned char *name;
  unsigned char *data;
  int len;
};

/** FS_ROOT represents the root of the virtual file system. */
#define FS_ROOT file_index_html
#define FS_NUMFILES 3

static const unsigned char HEAD_HTML[] = "<!DOCTYPE html> <html> <body> <img src=\"./img/logo.png\" alt=\"Insper\" style=\"width:304px;height:228px;\"> <h1>Insper: Computacao Embarcada</h1>";
static const unsigned char TAIL_HTML[] = "</body> </html>";

extern const struct fsdata_file file_insper_html[];
extern const struct fsdata_file file_img_logo_png[];
extern const struct fsdata_file file_img_sics_gif[];
extern const struct fsdata_file file_404_html[];
extern const struct fsdata_file file_index_html[];

#endif /* __FSDATA_H__ */
