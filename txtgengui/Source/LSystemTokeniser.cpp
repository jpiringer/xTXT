//
//  AutomateTokenizer.cpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "LSystemTokeniser.hpp"

struct LSystemTokeniserFunctions {
    static bool isIdentifierStart (const juce_wchar c) noexcept {
        return c != '=' && c != ';' && c != L'\0';
    }
    
    static bool isIdentifierBody (const juce_wchar c) noexcept {
        return c != '=' && c != ';' && c != L'\0';
    }
    
    static bool isReservedKeyword (String::CharPointerType token, const int tokenLength) noexcept {
       return false;
    }
    
    template <typename Iterator>
    static int parseIdentifier (Iterator& source) noexcept {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier [100];
        String::CharPointerType possible (possibleIdentifier);
        
        while (isIdentifierBody (source.peekNextChar()))
        {
            const juce_wchar c = source.nextChar();
            
            if (tokenLength < 20)
                possible.write (c);
            
            ++tokenLength;
        }
        
        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();
            
            if (isReservedKeyword (String::CharPointerType (possibleIdentifier), tokenLength))
                return CPlusPlusCodeTokeniser::tokenType_keyword;
        }
        
        return CPlusPlusCodeTokeniser::tokenType_identifier;
    }
    
    template <typename Iterator>
    static bool skipNumberSuffix (Iterator& source)
    {
        const juce_wchar c = source.peekNextChar();
        if (c == 'l' || c == 'L' || c == 'u' || c == 'U')
            source.skip();
        
        if (CharacterFunctions::isLetterOrDigit (source.peekNextChar()))
            return false;
        
        return true;
    }
    
    static bool isHexDigit (const juce_wchar c) noexcept
    {
        return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
    }
    
    template <typename Iterator>
    static bool parseHexLiteral (Iterator& source) noexcept
    {
        if (source.peekNextChar() == '-')
            source.skip();
        
        if (source.nextChar() != '0')
            return false;
        
        juce_wchar c = source.nextChar();
        if (c != 'x' && c != 'X')
            return false;
        
        int numDigits = 0;
        while (isHexDigit (source.peekNextChar()))
        {
            ++numDigits;
            source.skip();
        }
        
        if (numDigits == 0)
            return false;
        
        return skipNumberSuffix (source);
    }
    
    static bool isOctalDigit (const juce_wchar c) noexcept
    {
        return c >= '0' && c <= '7';
    }
    
    template <typename Iterator>
    static bool parseOctalLiteral (Iterator& source) noexcept
    {
        if (source.peekNextChar() == '-')
            source.skip();
        
        if (source.nextChar() != '0')
            return false;
        
        if (! isOctalDigit (source.nextChar()))
            return false;
        
        while (isOctalDigit (source.peekNextChar()))
            source.skip();
        
        return skipNumberSuffix (source);
    }
    
    static bool isDecimalDigit (const juce_wchar c) noexcept
    {
        return c >= '0' && c <= '9';
    }
    
    template <typename Iterator>
    static bool parseDecimalLiteral (Iterator& source) noexcept
    {
        if (source.peekNextChar() == '-')
            source.skip();
        
        int numChars = 0;
        while (isDecimalDigit (source.peekNextChar()))
        {
            ++numChars;
            source.skip();
        }
        
        if (numChars == 0)
            return false;
        
        return skipNumberSuffix (source);
    }
    
    template <typename Iterator>
    static bool parseFloatLiteral (Iterator& source) noexcept
    {
        if (source.peekNextChar() == '-')
            source.skip();
        
        int numDigits = 0;
        
        while (isDecimalDigit (source.peekNextChar()))
        {
            source.skip();
            ++numDigits;
        }
        
        const bool hasPoint = (source.peekNextChar() == '.');
        
        if (hasPoint)
        {
            source.skip();
            
            while (isDecimalDigit (source.peekNextChar()))
            {
                source.skip();
                ++numDigits;
            }
        }
        
        if (numDigits == 0)
            return false;
        
        juce_wchar c = source.peekNextChar();
        const bool hasExponent = (c == 'e' || c == 'E');
        
        if (hasExponent)
        {
            source.skip();
            
            c = source.peekNextChar();
            if (c == '+' || c == '-')
                source.skip();
            
            int numExpDigits = 0;
            while (isDecimalDigit (source.peekNextChar()))
            {
                source.skip();
                ++numExpDigits;
            }
            
            if (numExpDigits == 0)
                return false;
        }
        
        c = source.peekNextChar();
        if (c == 'f' || c == 'F')
            source.skip();
        else if (! (hasExponent || hasPoint))
            return false;
        
        return true;
    }
    
    template <typename Iterator>
    static int parseNumber (Iterator& source)
    {
        const Iterator original (source);
        
        if (parseFloatLiteral (source))    return CPlusPlusCodeTokeniser::tokenType_float;
        source = original;
        
        if (parseHexLiteral (source))      return CPlusPlusCodeTokeniser::tokenType_integer;
        source = original;
        
        if (parseOctalLiteral (source))    return CPlusPlusCodeTokeniser::tokenType_integer;
        source = original;
        
        if (parseDecimalLiteral (source))  return CPlusPlusCodeTokeniser::tokenType_integer;
        source = original;
        
        return CPlusPlusCodeTokeniser::tokenType_error;
    }
    
    template <typename Iterator>
    static void skipQuotedString (Iterator& source) noexcept
    {
        const juce_wchar quote = source.nextChar();
        
        for (;;)
        {
            const juce_wchar c = source.nextChar();
            
            if (c == quote || c == 0)
                break;
            
            if (c == '\\')
                source.skip();
        }
    }
    
    template <typename Iterator>
    static void skipComment (Iterator& source) noexcept
    {
        bool lastWasStar = false;
        
        for (;;)
        {
            const juce_wchar c = source.nextChar();
            
            if (c == 0 || (c == '/' && lastWasStar))
                break;
            
            lastWasStar = (c == '*');
        }
    }
    
    template <typename Iterator>
    static void skipPreprocessorLine (Iterator& source) noexcept
    {
        bool lastWasBackslash = false;
        
        for (;;)
        {
            const juce_wchar c = source.peekNextChar();
            
            if (c == '"')
            {
                skipQuotedString (source);
                continue;
            }
            
            if (c == '/')
            {
                Iterator next (source);
                next.skip();
                const juce_wchar c2 = next.peekNextChar();
                
                if (c2 == '/' || c2 == '*')
                    return;
            }
            
            if (c == 0)
                break;
            
            if (c == '\n' || c == '\r')
            {
                source.skipToEndOfLine();
                
                if (lastWasBackslash)
                    skipPreprocessorLine (source);
                
                break;
            }
            
            lastWasBackslash = (c == '\\');
            source.skip();
        }
    }
    
    template <typename Iterator>
    static void skipIfNextCharMatches (Iterator& source, const juce_wchar c) noexcept
    {
        if (source.peekNextChar() == c)
            source.skip();
    }
    
    template <typename Iterator>
    static void skipIfNextCharMatches (Iterator& source, const juce_wchar c1, const juce_wchar c2) noexcept
    {
        const juce_wchar c = source.peekNextChar();
        
        if (c == c1 || c == c2)
            source.skip();
    }
    
    template <typename Iterator>
    static int readNextToken (Iterator& source)
    {
        source.skipWhitespace();
        
        const juce_wchar firstChar = source.peekNextChar();
        
        switch (firstChar)
        {
            case 0:
                break;
                
            case '#':
                source.skipToEndOfLine();
                return LuaTokeniser::tokenType_comment;
            
            case ';':
            case '=':
                source.skip();
                return LSystemTokeniser::tokenType_punctuation;
                
            default:
                if (isIdentifierStart (firstChar))
                    return parseIdentifier (source);
                
                source.skip();
                break;
        }
        
        return LSystemTokeniser::tokenType_error;
    }
    
    /** A class that can be passed to the CppTokeniserFunctions functions in order to
     parse a String.
     */
    struct StringIterator
    {
        StringIterator (const String& s) noexcept            : t (s.getCharPointer()), numChars (0) {}
        StringIterator (String::CharPointerType s) noexcept  : t (s), numChars (0) {}
        
        juce_wchar nextChar() noexcept      { if (isEOF()) return 0; ++numChars; return t.getAndAdvance(); }
        juce_wchar peekNextChar()noexcept   { return *t; }
        void skip() noexcept                { if (! isEOF()) { ++t; ++numChars; } }
        void skipWhitespace() noexcept      { while (t.isWhitespace()) skip(); }
        void skipToEndOfLine() noexcept     { while (*t != '\r' && *t != '\n' && *t != 0) skip(); }
        bool isEOF() const noexcept         { return t.isEmpty(); }
        
        String::CharPointerType t;
        int numChars;
    };
    
    //==============================================================================
    /** Takes a UTF8 string and writes it to a stream using standard C++ escape sequences for any
     non-ascii bytes.
     
     Although not strictly a tokenising function, this is still a function that often comes in
     handy when working with C++ code!
     
     Note that addEscapeChars() is easier to use than this function if you're working with Strings.
     
     @see addEscapeChars
     */
    static void writeEscapeChars (OutputStream& out, const char* utf8, const int numBytesToRead,
                                  const int maxCharsOnLine, const bool breakAtNewLines,
                                  const bool replaceSingleQuotes, const bool allowStringBreaks)
    {
        int charsOnLine = 0;
        bool lastWasHexEscapeCode = false;
        bool trigraphDetected = false;
        
        for (int i = 0; i < numBytesToRead || numBytesToRead < 0; ++i)
        {
            const unsigned char c = (unsigned char) utf8[i];
            bool startNewLine = false;
            
            switch (c)
            {
                    
                case '\t':  out << "\\t";  trigraphDetected = false; lastWasHexEscapeCode = false; charsOnLine += 2; break;
                case '\r':  out << "\\r";  trigraphDetected = false; lastWasHexEscapeCode = false; charsOnLine += 2; break;
                case '\n':  out << "\\n";  trigraphDetected = false; lastWasHexEscapeCode = false; charsOnLine += 2; startNewLine = breakAtNewLines; break;
                case '\\':  out << "\\\\"; trigraphDetected = false; lastWasHexEscapeCode = false; charsOnLine += 2; break;
                case '\"':  out << "\\\""; trigraphDetected = false; lastWasHexEscapeCode = false; charsOnLine += 2; break;
                    
                case '?':
                    if (trigraphDetected)
                    {
                        out << "\\?";
                        charsOnLine++;
                        trigraphDetected = false;
                    }
                    else
                    {
                        out << "?";
                        trigraphDetected = true;
                    }
                    
                    lastWasHexEscapeCode = false;
                    charsOnLine++;
                    break;
                    
                case 0:
                    if (numBytesToRead < 0)
                        return;
                    
                    out << "\\0";
                    lastWasHexEscapeCode = true;
                    trigraphDetected = false;
                    charsOnLine += 2;
                    break;
                    
                case '\'':
                    if (replaceSingleQuotes)
                    {
                        out << "\\\'";
                        lastWasHexEscapeCode = false;
                        trigraphDetected = false;
                        charsOnLine += 2;
                        break;
                    }
                    
                    // deliberate fall-through...
                    
                default:
                    if (c >= 32 && c < 127 && ! (lastWasHexEscapeCode  // (have to avoid following a hex escape sequence with a valid hex digit)
                                                 && CharacterFunctions::getHexDigitValue (c) >= 0))
                    {
                        out << (char) c;
                        lastWasHexEscapeCode = false;
                        trigraphDetected = false;
                        ++charsOnLine;
                    }
                    else if (allowStringBreaks && lastWasHexEscapeCode && c >= 32 && c < 127)
                    {
                        out << "\"\"" << (char) c;
                        lastWasHexEscapeCode = false;
                        trigraphDetected = false;
                        charsOnLine += 3;
                    }
                    else
                    {
                        out << (c < 16 ? "\\x0" : "\\x") << String::toHexString ((int) c);
                        lastWasHexEscapeCode = true;
                        trigraphDetected = false;
                        charsOnLine += 4;
                    }
                    
                    break;
            }
            
            if ((startNewLine || (maxCharsOnLine > 0 && charsOnLine >= maxCharsOnLine))
                && (numBytesToRead < 0 || i < numBytesToRead - 1))
            {
                charsOnLine = 0;
                out << "\"" << newLine << "\"";
                lastWasHexEscapeCode = false;
            }
        }
    }
    
    /** Takes a string and returns a version of it where standard C++ escape sequences have been
     used to replace any non-ascii bytes.
     
     Although not strictly a tokenising function, this is still a function that often comes in
     handy when working with C++ code!
     
     @see writeEscapeChars
     */
    static String addEscapeChars (const String& s)
    {
        MemoryOutputStream mo;
        writeEscapeChars (mo, s.toRawUTF8(), -1, -1, false, true, true);
        return mo.toString();
    }
};

int LSystemTokeniser::readNextToken(CodeDocument::Iterator &source) {
    return LSystemTokeniserFunctions::readNextToken (source);
}

CodeEditorComponent::ColourScheme LSystemTokeniser::getDefaultColourScheme() {
    struct Type {
        const char* name;
        uint32 colour;
    };
    
    const Type types[] = {
        { "Error",              0xffcc0000 },
        { "Comment",            0xffcccccc },
        { "Keyword",            0xff0000cc },
        { "Operator",           0xff00A000 },
        { "Identifier",         0xff000000 },
        { "Integer",            0xff880000 },
        { "Float",              0xff885500 },
        { "String",             0xff990099 },
        { "Bracket",            0xff0000A0 },
        { "Punctuation",        0xff00A000 }
    };
    
    CodeEditorComponent::ColourScheme cs;
    
    for (unsigned int i = 0; i < sizeof (types) / sizeof (types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
        cs.set (types[i].name, Colour (types[i].colour));
    
    return cs;
}
