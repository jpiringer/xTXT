//
//  VideoExporter.cpp
//  xTXT - App
//
//  Created by joerg piringer on 04.12.17.
//

#include "VideoExporter.hpp"

#include <stdlib.h>
#include <string.h>

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>


#define FPS 30
#define SAMPLERATE 44100
#define CHANNEL_COUNT 2

@interface VideoExporterImpl : NSObject {
    AVAssetWriter *videoWriter;
    AVAssetWriterInput *videoWriterInput;
    AVAssetWriterInput *audioWriterInput;
    AVAssetWriterInputPixelBufferAdaptor *adaptor;
    
    id<NSObject> activity;
    
    CIContext *ciContext;
    CGContextRef imageContext;
    
    CMFormatDescriptionRef audioFmtDesc;
    AudioStreamBasicDescription audioFormat;
    
    float *sampleBuffer;
    
    NSString *_path;
    NSString *_parameter;
}

@property (nonatomic, readonly) BOOL hasFrameFunction;
@property (nonatomic, strong) NSString *fileName;
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, readonly) float fps;
@property (nonatomic, readonly) int64_t maxFrames;
@property (nonatomic, assign) float exportScale;
@property (nonatomic, assign) BOOL renderAudio;
@property (nonatomic, assign) BOOL exportsAlpha;
@property (nonatomic, assign) BOOL renderToImage;

- (void)startExportToFile:(NSString *)path width:(int)width height:(int)height;
- (void)stopExport;

@end

@implementation VideoExporterImpl

- (instancetype)initWithPath:(NSString *)path parameter:(NSString *)parameter fps:(float)fps {
    if ((self = [super init])) {
        _path = path;
        _parameter = parameter;
        
        self.fileName = path.lastPathComponent;
        
        _fps = fps;
        
        _exportScale = 1;
        _exportsAlpha = NO;
        
        _renderAudio = NO;
        
        sampleBuffer = nullptr;
        imageContext = nullptr;
        
        if (sampleBuffer != nullptr) {
            delete sampleBuffer;
            sampleBuffer = nullptr;
        }
    }
    
    return self;
}

- (void)dealloc {
    [super dealloc];
    
    if (sampleBuffer != nullptr) {
        delete sampleBuffer;
    }
    if (imageContext != nullptr) {
        CGContextRelease(imageContext);
    }
}

- (void)initAudioFormat {
    audioFmtDesc = nil;
    int nchannels = CHANNEL_COUNT;
    bzero(&audioFormat, sizeof(audioFormat));
    audioFormat.mSampleRate = SAMPLERATE;
    audioFormat.mFormatID   = kAudioFormatLinearPCM;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mChannelsPerFrame = nchannels;
    int bytes_per_sample = sizeof(float);
    audioFormat.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked; //kAudioFormatFlagIsAlignedHigh;
    audioFormat.mBitsPerChannel = bytes_per_sample * 8;
    audioFormat.mBytesPerPacket = bytes_per_sample * nchannels;
    audioFormat.mBytesPerFrame = bytes_per_sample * nchannels;
    
    CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &audioFormat, 0, NULL, 0, NULL, NULL, &audioFmtDesc);
}

- (size_t)frameAudioBufferLength {
    int bytesPerFrame = audioFormat.mBytesPerFrame;
    int n = SAMPLERATE/self.fps;
    size_t buflen = n * bytesPerFrame;
    
    return buflen;
}

- (CMSampleBufferRef)createAudioFrame:(float *)samples time:(CMTime)timeStamp {
    OSStatus status;
    CMBlockBufferRef tempbuf = nullptr;
    CMBlockBufferRef bbuf = nullptr;
    CMSampleBufferRef sbuf = nullptr;
    
    if (samples != nullptr) {
        // Create sample buffer for adding to the audio input.
        status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, samples, self.frameAudioBufferLength, kCFAllocatorNull, NULL, 0, self.frameAudioBufferLength, kCMBlockBufferAssureMemoryNowFlag | kCMBlockBufferAlwaysCopyDataFlag, &tempbuf);
        if (status != noErr) {
            NSLog(@"CMBlockBufferCreateWithMemoryBlock error");
            delete[] samples;
            return nullptr;
        }
        
        // Copy the buffer so that we get a copy of the samples in memory.
        // CMBlockBufferCreateWithMemoryBlock does not actually copy the data!
        //
        status = CMBlockBufferCreateContiguous(kCFAllocatorDefault, tempbuf, kCFAllocatorDefault, NULL, 0, self.frameAudioBufferLength, kCMBlockBufferAssureMemoryNowFlag | kCMBlockBufferAlwaysCopyDataFlag, &bbuf);
        if (status != noErr) {
            NSLog(@"CMBlockBufferCreateContiguous error");
            delete[] samples;
            CFRelease(bbuf);
            return nullptr;
        }
        
        int n = SAMPLERATE/self.fps;
        status = CMAudioSampleBufferCreateWithPacketDescriptions(kCFAllocatorDefault, bbuf, TRUE, 0, NULL, audioFmtDesc, n, timeStamp, NULL, &sbuf);
        if (status != noErr) {
            NSLog(@"CMSampleBufferCreate error");
            return nullptr;
        }
        
        CFRelease(tempbuf);
        CFRelease(bbuf);
        
        return sbuf;
    }
    else {
        return nullptr;
    }
}

- (void)resetFrameAudio {
    size_t n = SAMPLERATE/self.fps*CHANNEL_COUNT;
    
    if (sampleBuffer == nullptr) {
        sampleBuffer = new float[n];
    }
    
    memset(sampleBuffer, 0, self.frameAudioBufferLength);
}

/*- (void)renderFrame:(double)frame useAudio:(BOOL)useAudio context:(CGContextRef)context {
    NSRect pageBounds = getCurrentPageBounds();
    CGContextRef drawingContext;
    float w = pageBounds.size.width;
    float h = pageBounds.size.height;
    
    drawingContext = context;
    
    [self resetFrameAudio];
    
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithCGContext:drawingContext flipped:YES]];
    CGContextSaveGState(drawingContext);
    
    currentScene = self;
    lua_getglobal(L, "frameFunction");
    push(L, (lua_Number)frame+1);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        dispatch_async(dispatch_get_main_queue(), ^{
            NSString *context;
            
            context = [NSString stringWithFormat:@"in file: %@", self.fileName];
            
            [(AppDelegate *)[NSApplication sharedApplication].delegate alertError:[NSString stringWithCString:lua_tostring(L, -1) encoding:NSUTF8StringEncoding]
                                                                   contextMessage:context];
        });
    }
    else {
        if (self.renderAudio && useAudio) {
            if(sampleBuffer != nullptr) {
                CMSampleBufferRef cmSampleBuffer = nullptr;
                CMTime framePosTime = CMTimeMakeWithSeconds(frame/self.fps, 10000);
                
                cmSampleBuffer = [self createAudioFrame:sampleBuffer time:framePosTime];
                
                if (cmSampleBuffer != nullptr) {
                    if (![audioWriterInput appendSampleBuffer:cmSampleBuffer]) {
                        NSLog(@"problem appending audio sample buffer!");
                        NSError *error = [videoWriter error];
                        NSLog(@"failed to append sbuf: %@", error);
                        //currentScene = nil;
                        return;
                    }
                    CFRelease(cmSampleBuffer);
                }
            }
        }
    }
    CGContextRestoreGState(drawingContext);
}*/

- (void)initVideoWriterWithPath:(NSString *)path width:(int)width height:(int)height {
    NSError *error = nil;
    
    if ([[[NSFileManager alloc] init] fileExistsAtPath:path]) {
        [[[NSFileManager alloc] init] removeItemAtPath:path error:&error];
        if (error != nil) {
            NSLog(@"error removing file '%@': %@", path, [error localizedDescription]);
        }
    }
    
    videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:path]
                                            fileType:AVFileTypeQuickTimeMovie
                                               error:&error];
    if (error != nil) {
        NSLog(@"error writing video '%@': %@", path, [error localizedDescription]);
    }
    
    NSSize frameSize = NSMakeSize(width, height);
    
    NSDictionary *videoSettings = @{AVVideoCodecKey: self.exportsAlpha ? AVVideoCodecTypeAppleProRes4444 : AVVideoCodecTypeH264,
    AVVideoWidthKey: @(frameSize.width*self.exportScale),
    AVVideoHeightKey:@(frameSize.height*self.exportScale)
    };
    
    videoWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
    videoWriterInput.expectsMediaDataInRealTime = YES;
    
    if (![videoWriter canAddInput:videoWriterInput]) {
        NSLog(@"cannot add video input to asset writer! %@ file: %@", [videoWriter.error localizedDescription], path);
    }
    [videoWriter addInput:videoWriterInput];
    
    adaptor = [[AVAssetWriterInputPixelBufferAdaptor alloc] initWithAssetWriterInput:videoWriterInput
                                                         sourcePixelBufferAttributes:@{
               (__bridge NSString *)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32ARGB),
               (__bridge NSString *)kCVPixelBufferWidthKey: @(frameSize.width*self.exportScale),
               (__bridge NSString *)kCVPixelBufferHeightKey: @(frameSize.height*self.exportScale),
               (__bridge NSString *)kCVPixelBufferCGImageCompatibilityKey: @YES,
               (__bridge NSString *)kCVPixelBufferCGBitmapContextCompatibilityKey: @YES
               }];
    
    if (self.renderAudio) {
        AudioChannelLayout stereoChannelLayout = {
            .mChannelLayoutTag = kAudioChannelLayoutTag_Stereo,
            .mChannelBitmap = 0,
            .mNumberChannelDescriptions = 0
        };
        NSData *channelLayoutAsData = [NSData dataWithBytes:&stereoChannelLayout length:offsetof(AudioChannelLayout, mChannelDescriptions)];
        
        NSDictionary *audioSettings = @{
        AVNumberOfChannelsKey:@CHANNEL_COUNT,
        AVSampleRateKey:@SAMPLERATE,
        AVFormatIDKey:@(kAudioFormatLinearPCM),
        AVLinearPCMIsBigEndianKey:@NO,
        AVLinearPCMIsNonInterleaved:@NO,
        AVLinearPCMIsFloatKey:@NO,
        AVLinearPCMBitDepthKey:@16,
        AVChannelLayoutKey:channelLayoutAsData
        };
        
        audioWriterInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio
                                                              outputSettings:audioSettings];
        
        audioWriterInput.expectsMediaDataInRealTime = YES;
        if (![videoWriter canAddInput:audioWriterInput]) {
            NSLog(@"cannot add audio input to asset writer! %@ file: %@", [videoWriter.error localizedDescription], path);
        }
        [videoWriter addInput:audioWriterInput];
        
        [self initAudioFormat];
    }
}

- (void)freeMemory {
    videoWriter = nil;
    videoWriterInput = nil;
    CVPixelBufferPoolFlush(adaptor.pixelBufferPool, kCVPixelBufferPoolFlushExcessBuffers);
    CVPixelBufferPoolRelease(adaptor.pixelBufferPool);
    adaptor = nil;
}

- (void)startExportToFile:(NSString *)path width:(int)width height:(int)height {
    
    //[self resetSceneWithPath:_path parameter:_parameter fps:_fps];
    
    [self initVideoWriterWithPath:path width:width height:height];
    
    // start
    activity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityIdleSystemSleepDisabled reason:@"Generating scene"];
    
    videoWriter.movieFragmentInterval = CMTimeMakeWithSeconds(5, 10000);
    [videoWriter startWriting];
    [videoWriter startSessionAtSourceTime:kCMTimeZero];
}

- (void)encode:(int64_t)frameNr drawFunction:(VideoDrawFunction)drawFunction width:(int)width height:(int)height  {
    NSSize frameSize = NSMakeSize(width, height);
    CGColorSpaceRef genericRGB = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CVPixelBufferRef pxbuffer;
    
    CVReturn status = CVPixelBufferPoolCreatePixelBuffer(kCFAllocatorDefault, adaptor.pixelBufferPool, &pxbuffer);
    assert(status == kCVReturnSuccess && pxbuffer != nullptr);
    
    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
    assert(pxdata != nullptr);
    
    CGContextRef exportContext = CGBitmapContextCreate(pxdata, frameSize.width*self.exportScale, frameSize.height*self.exportScale, 8, 4*frameSize.width*self.exportScale, genericRGB, kCGImageAlphaNoneSkipFirst);
    
    @autoreleasepool { // release all the stuff that's allocated during frame generation
        drawFunction(1.f/(float)FPS, exportContext);
    }
    
    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
    while (!videoWriterInput.readyForMoreMediaData) {
        usleep(100);
    }
    
    CMTime presentTime = CMTimeMake(frameNr * 600 / (int64_t)self.fps, 600);
    if (![adaptor appendPixelBuffer:pxbuffer withPresentationTime:presentTime]) {
        NSLog(@"error while appending pixel buffer for frame %lld", frameNr);
    }
    
    CVPixelBufferRelease(pxbuffer);
    CFRelease(exportContext);
}

- (void)stopExport {
    __block BOOL finished = NO;
    
    // stop
    [videoWriterInput markAsFinished];
    if (self.renderAudio) {
        [audioWriterInput markAsFinished];
    }
    [videoWriter finishWritingWithCompletionHandler:^{
     finished = YES;
     }];
    
    [[NSProcessInfo processInfo] endActivity:activity];
    
    [self freeMemory];
}

@end
VideoExporterImpl *impl = nil;

jp::VideoExporter::VideoExporter() {
}

float jp::VideoExporter::encodeFrames(float time, VideoDrawFunction func) {
    float residualTime = time;
    float frameTime = 1.f/(float)FPS;
    
    while (residualTime >= frameTime) {
        // encode
        [impl encode:currentFrameNr drawFunction:func width:width height:height];
        currentFrameNr++;
        residualTime -= frameTime;
    }
    return residualTime;
}

bool jp::VideoExporter::start(const std::string &filename, int w, int h) {
    NSString *path = [NSString stringWithUTF8String:filename.c_str()];
    impl = [[VideoExporterImpl alloc] initWithPath:path parameter:nil fps:FPS];
    
    width = w;
    height = h;
    
    currentFrameNr = 0;
    
    [impl startExportToFile:path width:width height:height];
    
    return true;
}

void jp::VideoExporter::stop() {
    [impl stopExport];
}
