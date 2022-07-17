//
//  Program.hpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef Program_hpp
#define Program_hpp

#include <string>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

#include "Runner.hpp"

#if TARGET_OS_MAC
#include <lua/lua.hpp>
//#include <CoreGraphics/CoreGraphics.h>
//#include <CoreText/CoreText.h>
#else
#include "lua.hpp"
#endif

#include "../JuceLibraryCode/JuceHeader.h"


namespace jp {
    enum Justification {
        JustificationLeft,
        JustificationRight,
        JustificationCenter
    };
    
    class ProgramRunner;
    class RunContext;
    class VideoExporter;
};

class Node {
    int nodeID = -1;
    float xpos = 0;
    float ypos = 0;
    float angle = 0;
    float r = 0, g = 0, b = 0, a = 1;
    float lifetime = 99999999.0f; // forever

protected:
    void reduceLifetime(float dt) {lifetime -= dt;}
    
public:
    virtual ~Node() {};

    void setID(int _id) {nodeID = _id;}
    int getID() {return nodeID;}

    void setLifetime(float _lifetime) {lifetime = _lifetime;}
    bool isDead() {return lifetime <= 0;}

    void setPosition(float x, float y) {xpos = x; ypos = y;}
    float getX() {return xpos;}
    float getY() {return ypos;}
    
    void setRotation(float _angle) {angle = _angle;}
    float getRotation() {return angle;}
    
    void setColor(float _r, float _g, float _b, float _a) {r = _r; g = _g; b = _b; a = _a;}
    float getRed() {return r;}
    float getGreen() {return g;}
    float getBlue() {return b;}
    float getAlpha() {return a;}

    virtual void update(float dt) {reduceLifetime(dt);}
    virtual void draw(Graphics &g) = 0;
    virtual void drawNative(void *cg) = 0;
};

class TextNode : public Node {
    std::string text;

    enum jp::Justification justification;
    float size;
    
protected:
    void initFont();
    
public:
    TextNode(const std::string &t) : text(t) {}
    
    void setJustification(enum jp::Justification _justification) {justification = _justification;}
    void setSize(float _size) {size = _size;}
    float getSize() {return size;}
    
    const std::string &getText() {return text;}
    
    virtual void draw(Graphics &g) override;
    void drawNative(void *cg) override;
};

class TextWorld {
    static TextWorld textWorld;
    
    std::list<std::shared_ptr<Node>> nodes;
    int nextID = 0;
    std::mutex worldMutex;
    
    float backgroundR, backgroundG, backgroundB, backgroundA;

public:
    ~TextWorld();
    
    void addNode(std::shared_ptr<Node> node);
    void removeNode(int _id);
    void removeAllNodes();
    std::list<std::shared_ptr<Node>> &getNodes() {return nodes;}
    
    void setBackground(float r, float g, float b, float a) {backgroundR = r; backgroundG = g; backgroundB = b; backgroundA = a;}

    void update(float dt);
    void draw(Graphics &g);
    void drawNative(void *cg);

    static TextWorld &sharedTextWorld() {return textWorld;}
};

class LuaProgram :
public juce::ThreadWithProgressWindow
{
    static LuaProgram *theProgram;
 
    lua_State *L = nullptr;
    jp::RunContext *runContext;
    
    std::string code;
    
    std::atomic_int errorCount;
    std::string errors;
    
    float colorR, colorG, colorB, colorA;
    float backgroundR, backgroundG, backgroundB, backgroundA;
    float size;
    float posx = 0, posy = 0;
    float angle = 0;
    enum jp::Justification justification;
    
    // for exporting
    bool exporting = false;
    bool exportingStarted = false;
#if TARGET_OS_MAC
	std::shared_ptr<jp::VideoExporter> exporter;
#endif
    std::string filenameExport;
    float residualWaitTime = 0;
    
    std::list<int> keyPresses;
    std::map<int, int> keyMap;
    
protected:
    void _error(const std::string &msg);
    
    void initState();
    void checkExportStarted();
    
    bool processKeyPresses();
    
public:
    LuaProgram() : juce::ThreadWithProgressWindow("Exporting...", false, true) {}
    ~LuaProgram();
    
    virtual void run() override;
    
    int getErrorCount() {return errorCount;}
    const std::string &getErrors() {return errors;}
    
    void setCode(const std::string &c) {code = c;}
    std::string execute(jp::RunContext *_runContext);
    void exportVideo(const std::string &filename);
    
    void setColor(float r, float g, float b, float a) {colorR = r; colorG = g; colorB = b; colorA = a;}
    void getColor(float &r, float &g, float &b, float &a) {r = colorR; g = colorG; b = colorB; a = colorA;}

    void setBackground(float r, float g, float b, float a) {backgroundR = r; backgroundG = g; backgroundB = b; backgroundA = a;}
    void getBackground(float &r, float &g, float &b, float &a) {r = backgroundR; g = backgroundG; b = backgroundB; a = backgroundA;}

    void setJustification(enum jp::Justification j) {justification = j;}
    enum jp::Justification getJustification() {return justification;}
    
    void setSize(float s) {size = s;}
    float getSize() {return size;}
    
    void setPosition(float x, float y) {posx = x; posy = y;}
    void getPosition(float &x, float &y) {x = posx; y = posy;}

    void setRotation(float _angle) {angle = _angle;}
    float getRotation() {return angle;}
    
    void wait(float time);
    void waitForKey();

    static std::wstring convertToProgram(const std::wstring &code, const std::wstring &str, jp::RunnerType sourceType);
    
    void instructionHook(lua_State *L);
    void changeOutput(const std::string &str);
    void changeShowSize(float width, float height);
    void speak(const std::string &str);
    
    void registerKeyFunction(int keyCode, int funcRef) {keyMap[keyCode] = funcRef;}
    void keyPressed(int keyCode);
    
    bool isExporting() {return exporting;}

    static LuaProgram *sharedProgram() {return theProgram;}
};
#endif /* Program_hpp */
