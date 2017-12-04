//
//  VideoExporter.hpp
//  xTXT - App
//
//  Created by joerg piringer on 04.12.17.
//

#ifndef VideoExporter_hpp
#define VideoExporter_hpp

#include <stdio.h>
#include <string>
#include <functional>

typedef std::function<void(float, void *)> VideoDrawFunction;

namespace jp {
    class VideoExporter {
        int width;
        int height;
        int64_t currentFrameNr = 0;
        
    protected:
        void encodeFrame();
        
    public:
        VideoExporter();
        
        bool start(const std::string &filename, int w, int h);
        void stop();
        
        float encodeFrames(float time, VideoDrawFunction func);
    };
}

#endif /* VideoExporter_hpp */
