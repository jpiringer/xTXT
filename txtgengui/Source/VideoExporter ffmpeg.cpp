//
//  VideoExporter.cpp
//  xTXT - App
//
//  Created by joerg piringer on 04.12.17.
//

#include "VideoExporter.hpp"

#include <stdlib.h>
#include <string.h>

#define FPS 30

jp::VideoExporter::VideoExporter() {
    av_register_all();
}

void jp::VideoExporter::encodeFrame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt) {
    int ret;
    
    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3" PRId64 "\n", frame->pts);
    
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
    
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }
        
        printf("Write packet %3" PRId64 " (size=%5d)\n", pkt->pts, pkt->size);
        //fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

bool jp::VideoExporter::start(const std::string &filename) {
    //const char *codec_name = "libx264rgb";
    const char *codec_name = "libx264";
    int codec_id = AV_CODEC_ID_H264;
    
    // auto detect the output format from the name. default is mpeg.
    AVOutputFormat *avOutputFormat = av_guess_format(nullptr, filename.c_str(), nullptr);
    if (!avOutputFormat) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avOutputFormat = av_guess_format("mpeg", NULL, NULL);
    }
    if (!avOutputFormat) {
        fprintf(stderr, "Could not find suitable output format\n");
        return false;
    }
    
    /// allocate the output media context, avFormatContext
    formatContext = avformat_alloc_context();
    if (!formatContext) {
        fprintf(stderr, "Memory error\n");
        return false;
    }
    // Set the output format of the newly allocated avFormatContext to our avOutputFormat
    formatContext->oformat = avOutputFormat;
    snprintf(formatContext->filename, sizeof(formatContext->filename),
             "%s", filename.c_str());
    
    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        return false;
    }
    
    videoStream = avformat_new_stream(formatContext, codec);
    if (!videoStream) {
        fprintf(stderr, "Could not alloc stream\n");
        return false;
    }
    if (videoStream->codec == nullptr) {
        fprintf(stderr, "AVStream codec is NULL\n");
        return false;
    }
    videoStream->time_base = (AVRational){1, FPS};
    videoStream->codecpar = 
    
    codecContext = videoStream->codec; /*avcodec_alloc_context3(codec);
    if (!codecContext) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return false;
    }*/
    
    packet = av_packet_alloc();
    if (!packet) {
        fprintf(stderr, "Could not allocate packet\n");
        return false;
    }
    
    /* put sample parameters */
    codecContext->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codecContext->width = 352;
    codecContext->height = 288;
    /* frames per second */
    codecContext->framerate = (AVRational){FPS, 1};
    
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    codecContext->gop_size = 10;
    codecContext->max_b_frames = 1;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    
    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(codecContext->priv_data, "preset", "slow", 0);
    
    /* open it */
    int ret = avcodec_open2(codecContext, codec, nullptr);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        return false;
    }
    
    /*file = fopen(filename.c_str(), "wb");
    if (!file) {
        fprintf(stderr, "Could not open %s\n", filename.c_str());
        return false;
    }*/
    
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return false;
    }
    frame->format = codecContext->pix_fmt;
    frame->width  = codecContext->width;
    frame->height = codecContext->height;
    
    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return false;
    }
    
    // some formats want stream headers to be separate
    if(formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
    
    avformat_init_output(formatContext, nullptr);
    avformat_write_header(formatContext, nullptr);
    
    return true;
}

void jp::VideoExporter::encode() {
    /* encode 1 second of video */
    for (int i = 0; i < 250; i++) {
        fflush(stdout);
        
        /* make sure the frame data is writable */
        int ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        
        /* prepare a dummy image */
        /* Y */
        for (int y = 0; y < codecContext->height; y++) {
            for (int x = 0; x < codecContext->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }
        
        /* Cb and Cr */
        for (int y = 0; y < codecContext->height/2; y++) {
            for (int x = 0; x < codecContext->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
        
        frame->pts = i;
        
        /* encode the image */
        encodeFrame(codecContext, frame, packet);
    }
}

void jp::VideoExporter::stop() {
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    /* flush the encoder */
    encodeFrame(codecContext, nullptr, packet);
    
    av_write_trailer(formatContext);
    
    /* add sequence end code to have a real MPEG file */
    //fwrite(endcode, 1, sizeof(endcode), file);
    //fclose(file);
    
    avcodec_free_context(&codecContext);
    av_frame_free(&frame);
    av_packet_free(&packet);
}
