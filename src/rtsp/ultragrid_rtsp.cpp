/*
 * FILE:    rtsp/ultragrid_rtsp.cpp
 * AUTHORS: David Cassany    <david.cassany@i2cat.net>
 *          Gerard Castillo  <gerard.castillo@i2cat.net>
 *          Martin Pulec     <pulec@cesnet.cz>
 *          Jakub Kováč      <xkovac5@mail.muni.cz>
 *
 * Copyright (c) 2005-2010 Fundació i2CAT, Internet I Innovació Digital a Catalunya
 * Copyright (c) 2010-2023 CESNET, z. s. p. o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *
 *      This product includes software developed by the Fundació i2CAT,
 *      Internet I Innovació Digital a Catalunya. This product also includes
 *      software developed by CESNET z.s.p.o.
 *
 * 4. Neither the name of the University nor of the Institute may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "rtsp/ultragrid_rtsp.hh"
#include "rtsp/UltragridRTSPServer.hh"

ultragrid_rtsp::ultragrid_rtsp(unsigned int rtsp_port, struct module* mod, rtsp_media_type_t media_type, audio_codec_t audio_codec,
        int audio_sample_rate, int audio_channels, int rtp_video_port, int rtp_audio_port)
        : thread_running(false) {
    rtsp_server = std::make_unique<UltragridRTSPServer>(rtsp_port, mod, media_type, audio_codec, audio_sample_rate, audio_channels, rtp_video_port, rtp_audio_port);
}

ultragrid_rtsp::~ultragrid_rtsp() {
    stop_server();
}

int ultragrid_rtsp::start_server() {
    if (thread_running)
        return 1;
    server_stop_flag = 0;

    int ret;
    ret = pthread_create(&server_thread, NULL, ultragrid_rtsp::server_runner, this);
    thread_running = (ret == 0) ? true : false;
    return ret;
}

void ultragrid_rtsp::stop_server() {
    server_stop_flag = 1;

    if (thread_running) {
        pthread_join(server_thread, NULL);
        thread_running = false;
    }
}

void* ultragrid_rtsp::server_runner(void* args) {
    ultragrid_rtsp* server_instance = (ultragrid_rtsp*) args;
    server_instance->rtsp_server->serverRunner(&server_instance->server_stop_flag);
    return NULL;
}