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

#include "../JuceLibraryCode/JuceHeader.h"


namespace jp {
    enum Justification {
        JustificationLeft,
        JustificationRight,
        JustificationCenter
    };
};

class Node {
    int nodeID = -1;
    float xpos = 0;
    float ypos = 0;
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

    void setPos(float x, float y) {xpos = x; ypos = y;}
    float getX() {return xpos;}
    float getY() {return ypos;}
    
    void setColor(float _r, float _g, float _b, float _a) {r = _r; g = _g; b = _b; a = _a;}
    float getRed() {return r;}
    float getGreen() {return g;}
    float getBlue() {return b;}
    float getAlpha() {return a;}

    virtual void update(float dt) {reduceLifetime(dt);}
    virtual void draw(Graphics &g) = 0;
};

class TextNode : public Node {
    std::string text;

    enum jp::Justification justification;
    float size;
    
public:
    TextNode(const std::string &t) : text(t) {}
    
    void setJustification(enum jp::Justification _justification) {justification = _justification;}
    void setSize(float _size) {size = _size;}
    
    virtual void draw(Graphics &g) override;
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
    void removeNode();
    void removeAllNodes();
    
    void setBackground(float r, float g, float b, float a) {backgroundR = r; backgroundG = g; backgroundB = b; backgroundA = a;}

    void update(float dt);
    void draw(Graphics &g);
    
    static TextWorld &sharedTextWorld() {return textWorld;}
};

class LuaProgram {
    static LuaProgram *theProgram;
    
    std::string code;
    
    int errorCount = 0;
    std::string errors;
    
    float colorR, colorG, colorB, colorA;
    float backgroundR, backgroundG, backgroundB, backgroundA;
    float size;
    float posx = 0, posy = 0;
    enum jp::Justification justification;
    
protected:
    void _error(const std::string &msg) {
        errors += msg;
        
        errorCount++;
    }
    
public:
    LuaProgram() {}
    ~LuaProgram();
    
    int getErrorCount() {return errorCount;}
    const std::string &getErrors() {return errors;}
    
    void setCode(const std::string &c) {code = c;}
    std::string execute();
    
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

    static std::wstring convertToProgram(const std::wstring &str);
    
    static LuaProgram *sharedProgram() {return theProgram;}
};
#endif /* Program_hpp */
