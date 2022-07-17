//
//  UndoableActions.hpp
//  txtgen - App
//
//  Created by joerg piringer on 11.10.17.
//

#ifndef UndoableActions_hpp
#define UndoableActions_hpp

#include "../JuceLibraryCode/JuceHeader.h"

#include <memory>

using namespace juce;

class UndoableModifyResultsAction : public UndoableAction {
private:
    std::string oldContent;
    std::shared_ptr<TextEditor> editor;
public:
    UndoableModifyResultsAction(std::shared_ptr<TextEditor> _editor, const std::string &_oldContent) :
    oldContent(_oldContent), editor(_editor)  {}
    ~UndoableModifyResultsAction() {}
    
    bool perform();
    bool undo();
};

#endif /* UndoableActions_hpp */
