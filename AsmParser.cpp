#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

class AsmParser
{
public:
    explicit AsmParser(std::string line)
        : mLine(line)
    {
        mRegisters.insert("eax");
        mRegisters.insert("ebx");
        mRegisters.insert("ecx");
        mRegisters.insert("edx");
        mRegisters.insert("ebp");
        mRegisters.insert("esp");
        mRegisters.insert("esi");
        mRegisters.insert("edi");

        mPrefixes.insert("lock");
        mPrefixes.insert("rep");
        mPrefixes.insert("repne");
        mPrefixes.insert("repe");
        mPrefixes.insert("repz");

        mMemsizes.insert("byte");
        mMemsizes.insert("word");
        mMemsizes.insert("dword");
        mMemsizes.insert("qword");

        mSegments.insert("fs");
        mSegments.insert("ds");
        mSegments.insert("gs");
        mSegments.insert("es");
        mSegments.insert("ss");
        mSegments.insert("cs");
    }

    void Tokenize()
    {
        mCur.reserve(mLine.length());
        for(size_t i = 0; i < mLine.length(); i++)
        {
            char ch = mLine[i];
            switch(ch)
            {
                case ' ':
                case '\t':
                    appendCurToken();
                    break;
                case ',':
                    appendCurToken();
                    appendToken(Token::Comma, ",");
                    break;
                case ':':
                    appendCurToken();
                    appendToken(Token::Colon, ":");
                    break;
                case '[':
                    appendCurToken();
                    appendToken(Token::Memopen, "[");
                    break;
                case ']':
                    appendCurToken();
                    appendToken(Token::Memclose, "]");
                    break;
                case '+':
                    appendCurToken();
                    appendToken(Token::Plus, "+");
                    break;
                case '-':
                    appendCurToken();
                    appendToken(Token::Minus, "+");
                    break;
                case '*':
                    appendCurToken();
                    appendToken(Token::Mult, "*");
                    break;
                default:
                    mCur.push_back(ch);
                    break;

            }
        }
        appendCurToken();
    }

    void Parse()
    {
        for(size_t i = 0; i < mTokens.size(); i++)
        {
            mTokens[i].Print();
        }
    }

private:
    struct Token
    {
        enum Type
        {
            Unknown = 0,
            Prefix = 1,
            Mnemonic = 2,
            Register = 3,
            Comma = 4, //','
            Memsize = 5,
            Ptr = 6, //'ptr'
            Segment = 7,
            Memopen = 8, //'['
            Memclose = 9, //']'
            Colon = 10, //':'
            Plus = 11, //'+'
            Minus = 12, //'-'
            Mult = 13, //'*'
        };

        Type type;
        std::string data;

        void Print()
        {
            printf("type: %d, data: \"%s\"\n", type, data.c_str());
        }
    };

    void appendCurToken()
    {
        if(mCur.empty())
            return;
        appendToken(getTokenType(mCur), mCur);
        mCur.clear();
    }

    Token::Type getTokenType(const std::string & data)
    {
        if(mSegments.count(data))
            return Token::Segment;
        if(mMemsizes.count(data))
            return Token::Memsize;
        if(mRegisters.count(data))
            return Token::Register;
        if(mPrefixes.count(data))
            return Token::Prefix;
        if(data == "ptr")
            return Token::Ptr;
        if(mTokens.empty() || mTokens[mTokens.size() - 1].type == Token::Prefix)
            return Token::Mnemonic;        
        return Token::Unknown;
    }

    void appendToken(Token::Type type, const std::string & data)
    {
        Token t;
        t.type = type;
        t.data = data;
        mTokens.push_back(t);
    }

    std::string mLine;
    std::string mCur;
    std::vector<Token> mTokens;
    typedef std::unordered_set<std::string> StringSet;
    StringSet mRegisters;
    StringSet mPrefixes;
    StringSet mMemsizes;
    StringSet mSegments;
};

int main()
{
    AsmParser parser("lock repne mov eax, dword ptr ds:[ebx], GetProcAddress, ntdll.NtQueryInformationProcess, dword ptr es:[eax*8+15]");
    parser.Tokenize();
    parser.Parse();
}
