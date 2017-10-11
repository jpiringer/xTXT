//
//  UndoableActions.cpp
//  txtgen - App
//
//  Created by joerg piringer on 11.10.17.
//

#include "UndoableActions.hpp"

bool UndoableModifyResultsAction::perform() {
    return true;
}

bool UndoableModifyResultsAction::undo() {
    editor->setText(oldContent);
    return true;
}
