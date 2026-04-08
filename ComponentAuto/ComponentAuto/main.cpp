#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

static bool ReadAllText(const fs::path& FilePath, std::string& OutText)
{
    std::ifstream In(FilePath, std::ios::binary);
    if (!In.is_open())
    {
        return false;
    }

    OutText.assign((std::istreambuf_iterator<char>(In)), std::istreambuf_iterator<char>());
    return true;
}

static bool WriteAllText(const fs::path& FilePath, const std::string& Text)
{
    std::ofstream Out(FilePath, std::ios::binary | std::ios::trunc);
    if (!Out.is_open())
    {
        return false;
    }

    Out.write(Text.data(), static_cast<std::streamsize>(Text.size()));
    return true;
}

static std::string NormalizeNewlinesToLF(const std::string& Text)
{
    std::string Result;
    Result.reserve(Text.size());

    for (char Ch : Text)
    {
        if (Ch != '\r')
        {
            Result.push_back(Ch);
        }
    }

    return Result;
}

static std::string StripCommentsFromCppForEnumScan(const std::string& Text)
{
    std::string Result;
    Result.reserve(Text.size());

    bool bInLineComment = false;
    bool bInBlockComment = false;

    for (size_t Index = 0; Index < Text.size(); ++Index)
    {
        const char Ch = Text[Index];
        const char Next = (Index + 1 < Text.size()) ? Text[Index + 1] : '\0';

        if (!bInLineComment && !bInBlockComment)
        {
            if (Ch == '/' && Next == '/')
            {
                bInLineComment = true;
                ++Index;
                continue;
            }
            if (Ch == '/' && Next == '*')
            {
                bInBlockComment = true;
                ++Index;
                continue;
            }

            Result.push_back(Ch);
            continue;
        }

        if (bInLineComment)
        {
            if (Ch == '\n')
            {
                bInLineComment = false;
                Result.push_back('\n');
            }
            continue;
        }

        if (bInBlockComment)
        {
            if (Ch == '*' && Next == '/')
            {
                bInBlockComment = false;
                ++Index;
            }
            continue;
        }
    }

    return Result;
}

static bool IsIdentChar(const char Ch)
{
    return (std::isalnum(static_cast<unsigned char>(Ch)) != 0) || (Ch == '_');
}

static bool FindEnumBodyRange(const std::string& Text, const std::string& EnumName, size_t& OutBodyBegin, size_t& OutBodyEnd)
{
    const std::string Needle = "enum class " + EnumName;
    const size_t Found = Text.find(Needle);
    if (Found == std::string::npos)
    {
        return false;
    }

    const size_t OpenBrace = Text.find('{', Found);
    if (OpenBrace == std::string::npos)
    {
        return false;
    }

    int Depth = 0;
    for (size_t Index = OpenBrace; Index < Text.size(); ++Index)
    {
        const char Ch = Text[Index];
        if (Ch == '{')
        {
            ++Depth;
            if (Depth == 1)
            {
                OutBodyBegin = Index + 1;
            }
            continue;
        }
        if (Ch == '}')
        {
            --Depth;
            if (Depth == 0)
            {
                OutBodyEnd = Index;
                return true;
            }
            continue;
        }
    }

    return false;
}

static std::vector<std::string> ParseEnumEnumerators(const std::string& EnumBody)
{
    std::vector<std::string> Enumerators;

    size_t Index = 0;
    while (Index < EnumBody.size())
    {
        while (Index < EnumBody.size() && (std::isspace(static_cast<unsigned char>(EnumBody[Index])) != 0))
        {
            ++Index;
        }
        if (Index >= EnumBody.size())
        {
            break;
        }

        if (!IsIdentChar(EnumBody[Index]) || (std::isdigit(static_cast<unsigned char>(EnumBody[Index])) != 0))
        {
            ++Index;
            continue;
        }

        const size_t Start = Index;
        while (Index < EnumBody.size() && IsIdentChar(EnumBody[Index]))
        {
            ++Index;
        }

        const std::string Name = EnumBody.substr(Start, Index - Start);
        if (!Name.empty())
        {
            Enumerators.push_back(Name);
        }

        while (Index < EnumBody.size() && EnumBody[Index] != ',')
        {
            ++Index;
        }
        if (Index < EnumBody.size() && EnumBody[Index] == ',')
        {
            ++Index;
        }
    }

    return Enumerators;
}

static std::vector<std::string> SplitByUnderscore(const std::string& Text)
{
    std::vector<std::string> Parts;
    std::string Current;

    for (char Ch : Text)
    {
        if (Ch == '_')
        {
            if (!Current.empty())
            {
                Parts.push_back(Current);
                Current.clear();
            }
            continue;
        }
        Current.push_back(Ch);
    }

    if (!Current.empty())
    {
        Parts.push_back(Current);
    }

    return Parts;
}

static std::string ToTitleCaseToken(const std::string& Token)
{
    std::string Result;
    Result.reserve(Token.size());

    size_t Index = 0;
    while (Index < Token.size())
    {
        const bool bIsDigit = (std::isdigit(static_cast<unsigned char>(Token[Index])) != 0);

        const size_t RunStart = Index;
        while (Index < Token.size())
        {
            const bool bCurDigit = (std::isdigit(static_cast<unsigned char>(Token[Index])) != 0);
            if (bCurDigit != bIsDigit)
            {
                break;
            }
            ++Index;
        }

        const std::string Run = Token.substr(RunStart, Index - RunStart);
        if (Run.empty())
        {
            continue;
        }

        if (bIsDigit)
        {
            Result += Run;
        }
        else
        {
            std::string Lower = Run;
            for (char& C : Lower)
            {
                C = static_cast<char>(std::tolower(static_cast<unsigned char>(C)));
            }
            Lower[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(Lower[0])));
            Result += Lower;
        }
    }

    return Result;
}

static std::string MakeComponentClassBaseName(const std::string& EnumeratorName)
{
    static const std::unordered_map<std::string, std::string> ExceptionMap =
    {
        { "MESHRENDER", "MeshRender" },
    };

    auto It = ExceptionMap.find(EnumeratorName);
    if (It != ExceptionMap.end())
    {
        return It->second;
    }

    const std::vector<std::string> Parts = SplitByUnderscore(EnumeratorName);
    if (Parts.empty())
    {
        return std::string();
    }

    std::string Result;
    for (const std::string& Part : Parts)
    {
        Result += ToTitleCaseToken(Part);
    }
    return Result;
}

static std::string MakeComponentHeaderFileNameFromBaseName(const std::string& BaseName)
{
    if (BaseName.empty())
    {
        return std::string();
    }
    return "C" + BaseName + ".h";
}

static bool ExistsHeaderAnywhereUnderProject(const fs::path& ProjectDir, const std::string& HeaderFileName)
{
    std::error_code Ec;
    if (!fs::exists(ProjectDir, Ec))
    {
        return false;
    }

    for (fs::recursive_directory_iterator It(ProjectDir, Ec), End; It != End && !Ec; It.increment(Ec))
    {
        if (It->is_regular_file(Ec))
        {
            if (It->path().filename().string() == HeaderFileName)
            {
                return true;
            }
        }
    }

    return false;
}

static std::unordered_set<std::string> ParseExistingMacroFirstArgs(const std::string& TextIn, const std::string& MacroPrefix)
{
    std::unordered_set<std::string> Existing;

    const std::string Text = NormalizeNewlinesToLF(TextIn);
    size_t SearchPos = 0;

    while (true)
    {
        const size_t Found = Text.find(MacroPrefix, SearchPos);
        if (Found == std::string::npos)
        {
            break;
        }

        const size_t Open = Found + MacroPrefix.size();
        const size_t CommaPos = Text.find(',', Open);
        const size_t ClosePos = Text.find(')', Open);

        size_t EndPos = std::string::npos;
        if (CommaPos != std::string::npos && ClosePos != std::string::npos)
        {
            EndPos = std::min(CommaPos, ClosePos);
        }
        else if (CommaPos != std::string::npos)
        {
            EndPos = CommaPos;
        }
        else
        {
            EndPos = ClosePos;
        }

        if (EndPos == std::string::npos || EndPos <= Open)
        {
            SearchPos = Found + 1;
            continue;
        }

        std::string Name = Text.substr(Open, EndPos - Open);
        Name.erase(std::remove_if(Name.begin(), Name.end(), [](unsigned char C) { return std::isspace(C) != 0; }), Name.end());

        if (!Name.empty())
        {
            Existing.insert(Name);
        }

        SearchPos = Found + 1;
    }

    return Existing;
}

static bool InsertLinesAfterMarkerLine(std::string& InOutText, const std::string& MarkerContains, const std::vector<std::string>& LinesToInsert)
{
    if (LinesToInsert.empty())
    {
        return true;
    }

    const std::string Text = NormalizeNewlinesToLF(InOutText);

    size_t LineStart = 0;
    size_t InsertPos = std::string::npos;

    while (LineStart < Text.size())
    {
        size_t LineEnd = Text.find('\n', LineStart);
        if (LineEnd == std::string::npos)
        {
            LineEnd = Text.size();
        }

        const std::string Line = Text.substr(LineStart, LineEnd - LineStart);
        if (Line.find(MarkerContains) != std::string::npos)
        {
            InsertPos = (LineEnd < Text.size()) ? (LineEnd + 1) : Text.size();
            break;
        }

        LineStart = (LineEnd < Text.size()) ? (LineEnd + 1) : Text.size();
    }

    if (InsertPos == std::string::npos)
    {
        return false;
    }

    std::string InsertBlock;
    for (const std::string& Line : LinesToInsert)
    {
        InsertBlock += Line;
        InsertBlock += "\n";
    }

    std::string NewText = Text;
    NewText.insert(InsertPos, InsertBlock);
    InOutText = NewText;
    return true;
}

static void AppendLinesToFileEnd(std::string& InOutText, const std::vector<std::string>& LinesToAppend)
{
    if (LinesToAppend.empty())
    {
        return;
    }

    std::string Text = NormalizeNewlinesToLF(InOutText);
    if (!Text.empty() && Text.back() != '\n')
    {
        Text.push_back('\n');
    }

    Text.push_back('\n');
    for (const std::string& Line : LinesToAppend)
    {
        Text += Line;
        Text += "\n";
    }

    InOutText = Text;
}

struct FComponentInfo
{
    std::string EnumName;
    std::string BaseName;
    std::string HeaderFileName;
    bool bHeaderExistsOnDisk = false;
};

struct FIncludeLineInfo
{
    size_t LineIndex = 0;
    bool bCommented = false;
};

// components.h를 "정리/동기화"한다.
// - 중복 include 제거(첫 줄만 유지)
// - 파일 존재 여부에 따라 주석/비주석 상태를 맞춤
// - 존재하지 않는 경우에는 라인이 없으면 새로 생성(주석/비주석은 존재 여부에 맞게)
static bool SyncAndDedupComponentsHeader(std::string& InOutComponentsText, const std::vector<FComponentInfo>& Components)
{
    std::string Text = NormalizeNewlinesToLF(InOutComponentsText);

    std::vector<std::string> Lines;
    Lines.reserve(1024);

    size_t LineStart = 0;
    while (LineStart <= Text.size())
    {
        size_t LineEnd = Text.find('\n', LineStart);
        if (LineEnd == std::string::npos)
        {
            LineEnd = Text.size();
        }

        Lines.push_back(Text.substr(LineStart, LineEnd - LineStart));
        if (LineEnd == Text.size())
        {
            break;
        }
        LineStart = LineEnd + 1;
    }

    auto ExtractIncludeFileName = [](const std::string& Line, std::string& OutFileName, size_t& OutNonSpace, bool& OutIsCommented) -> bool
        {
            OutFileName.clear();
            OutNonSpace = 0;
            while (OutNonSpace < Line.size() && (std::isspace(static_cast<unsigned char>(Line[OutNonSpace])) != 0))
            {
                ++OutNonSpace;
            }

            OutIsCommented = false;
            size_t ScanPos = OutNonSpace;

            if (ScanPos + 1 < Line.size() && Line[ScanPos] == '/' && Line[ScanPos + 1] == '/')
            {
                OutIsCommented = true;
                ScanPos += 2;
                if (ScanPos < Line.size() && Line[ScanPos] == ' ')
                {
                    ++ScanPos;
                }
            }

            const size_t IncludePos = Line.find("#include", ScanPos);
            if (IncludePos == std::string::npos)
            {
                return false;
            }

            const size_t QuoteA = Line.find('\"', IncludePos);
            if (QuoteA == std::string::npos)
            {
                return false;
            }

            const size_t QuoteB = Line.find('\"', QuoteA + 1);
            if (QuoteB == std::string::npos || QuoteB <= QuoteA + 1)
            {
                return false;
            }

            const std::string PathInside = Line.substr(QuoteA + 1, QuoteB - QuoteA - 1);
            const size_t LastSlash = PathInside.find_last_of("/\\");
            const std::string FileName = (LastSlash == std::string::npos) ? PathInside : PathInside.substr(LastSlash + 1);
            if (FileName.empty())
            {
                return false;
            }

            OutFileName = FileName;
            return true;
        };

    auto SetLineCommentState = [&](std::string& Line, bool bShouldBeCommented) -> bool
        {
            size_t NonSpace = 0;
            while (NonSpace < Line.size() && (std::isspace(static_cast<unsigned char>(Line[NonSpace])) != 0))
            {
                ++NonSpace;
            }

            const bool bIsCommented = (NonSpace + 1 < Line.size() && Line[NonSpace] == '/' && Line[NonSpace + 1] == '/');

            if (bShouldBeCommented)
            {
                if (!bIsCommented)
                {
                    Line.insert(NonSpace, "// ");
                    return true;
                }
                return false;
            }

            if (bIsCommented)
            {
                Line.erase(NonSpace, 2);
                if (NonSpace < Line.size() && Line[NonSpace] == ' ')
                {
                    Line.erase(NonSpace, 1);
                }
                return true;
            }

            return false;
        };

    // ===== 1) 중복 제거: 동일 파일 include는 첫 번째만 유지 =====
    std::unordered_map<std::string, size_t> FirstIndexByFile;
    std::unordered_set<size_t> DuplicateIndices;

    for (size_t i = 0; i < Lines.size(); ++i)
    {
        std::string FileName;
        size_t NonSpace = 0;
        bool bCommented = false;
        if (!ExtractIncludeFileName(Lines[i], FileName, NonSpace, bCommented))
        {
            continue;
        }

        auto It = FirstIndexByFile.find(FileName);
        if (It == FirstIndexByFile.end())
        {
            FirstIndexByFile.emplace(FileName, i);
        }
        else
        {
            DuplicateIndices.insert(i);
        }
    }

    bool bChanged = false;

    if (!DuplicateIndices.empty())
    {
        std::vector<std::string> NewLines;
        NewLines.reserve(Lines.size());

        for (size_t i = 0; i < Lines.size(); ++i)
        {
            if (DuplicateIndices.find(i) != DuplicateIndices.end())
            {
                bChanged = true;
                continue;
            }
            NewLines.push_back(Lines[i]);
        }

        Lines.swap(NewLines);

        // 인덱스 재구축
        FirstIndexByFile.clear();
        for (size_t i = 0; i < Lines.size(); ++i)
        {
            std::string FileName;
            size_t NonSpace = 0;
            bool bCommented = false;
            if (!ExtractIncludeFileName(Lines[i], FileName, NonSpace, bCommented))
            {
                continue;
            }
            if (FirstIndexByFile.find(FileName) == FirstIndexByFile.end())
            {
                FirstIndexByFile.emplace(FileName, i);
            }
        }
    }

    // ===== 2) enum 컴포넌트 헤더들 동기화 =====
    for (const FComponentInfo& Comp : Components)
    {
        const std::string& HeaderFile = Comp.HeaderFileName;
        const bool bShouldBeActive = Comp.bHeaderExistsOnDisk;

        auto It = FirstIndexByFile.find(HeaderFile);
        if (It != FirstIndexByFile.end())
        {
            const bool bLineChanged = SetLineCommentState(Lines[It->second], !bShouldBeActive);
            if (bLineChanged)
            {
                bChanged = true;
            }
        }
        else
        {
            std::string NewLine;
            if (bShouldBeActive)
            {
                NewLine = "#include \"" + HeaderFile + "\"";
            }
            else
            {
                NewLine = "// #include \"" + HeaderFile + "\"";
            }
            Lines.push_back(NewLine);
            bChanged = true;
        }
    }

    // ===== 3) 항상 맨 마지막: #include "CScript.h" 강제 =====
    const std::string ScriptHeader = "CScript.h";
    const std::string ScriptLine = "#include \"CScript.h\"";

    // 3-1) 만약 CScript.h 라인이 있으면 제거(주석 포함)하고, 마지막에 1번만 추가한다.
    std::vector<std::string> LinesNoScript;
    LinesNoScript.reserve(Lines.size());

    for (size_t i = 0; i < Lines.size(); ++i)
    {
        std::string FileName;
        size_t NonSpace = 0;
        bool bCommented = false;

        if (!ExtractIncludeFileName(Lines[i], FileName, NonSpace, bCommented))
        {
            LinesNoScript.push_back(Lines[i]);
            continue;
        }

        if (FileName == ScriptHeader)
        {
            bChanged = true;
            continue;
        }

        LinesNoScript.push_back(Lines[i]);
    }

    Lines.swap(LinesNoScript);

    // 3-2) 마지막에 활성 include로 1줄 추가
    Lines.push_back(ScriptLine);

    // ===== 재조립 =====
    std::string Rebuilt;
    for (size_t i = 0; i < Lines.size(); ++i)
    {
        Rebuilt += Lines[i];
        if (i + 1 < Lines.size())
        {
            Rebuilt += "\n";
        }
    }

    if (bChanged)
    {
        InOutComponentsText = Rebuilt;
        return true;
    }

    // bChanged가 false였더라도, ScriptLine을 마지막에 추가하면서 이미 바뀌었을 수 있음.
    // (위에서 ScriptLine을 무조건 push_back 했으므로) 내용 비교로 최종 판단.
    if (Rebuilt != NormalizeNewlinesToLF(InOutComponentsText))
    {
        InOutComponentsText = Rebuilt;
        return true;
    }

    return false;
}


int main(int Argc, char** Argv)
{
    bool bDryRun = false;
    for (int Index = 1; Index < Argc; ++Index)
    {
        const std::string Arg = Argv[Index];
        if (Arg == "--dry-run")
        {
            bDryRun = true;
        }
    }

    const fs::path SolutionRoot = fs::current_path();
    const fs::path ProjectDir = SolutionRoot / "GameClient";

    const fs::path EnumPath = ProjectDir / "enum.h";
    const fs::path ComponentsPath = ProjectDir / "components.h";
    const fs::path GameObjectPath = ProjectDir / "GameObject.h";
    const fs::path ComponentHeaderPath = ProjectDir / "Component.h";
    const fs::path ComponentCppPath = ProjectDir / "Component.cpp";

    if (!fs::exists(EnumPath))
    {
        std::cout << "ERROR: enum.h not found: " << EnumPath.string() << "\n";

        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (!fs::exists(ComponentsPath))
    {
        std::cout << "ERROR: components.h not found: " << ComponentsPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (!fs::exists(GameObjectPath))
    {
        std::cout << "ERROR: GameObject.h not found: " << GameObjectPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (!fs::exists(ComponentHeaderPath))
    {
        std::cout << "ERROR: Component.h not found: " << ComponentHeaderPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (!fs::exists(ComponentCppPath))
    {
        std::cout << "ERROR: Component.cpp not found: " << ComponentCppPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }

    // enum 파싱
    std::string EnumTextRaw;
    if (!ReadAllText(EnumPath, EnumTextRaw))
    {
        std::cout << "ERROR: Failed to read: " << EnumPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }

    const std::string EnumText = StripCommentsFromCppForEnumScan(NormalizeNewlinesToLF(EnumTextRaw));

    size_t BodyBegin = 0;
    size_t BodyEnd = 0;
    if (!FindEnumBodyRange(EnumText, "COMPONENT_TYPE", BodyBegin, BodyEnd))
    {
        std::cout << "ERROR: enum class COMPONENT_TYPE block not found in enum.h\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }

    const std::string EnumBody = EnumText.substr(BodyBegin, BodyEnd - BodyBegin);
    const std::vector<std::string> Enumerators = ParseEnumEnumerators(EnumBody);

    std::vector<FComponentInfo> Components;
    Components.reserve(Enumerators.size());

    for (const std::string& EnumName : Enumerators)
    {
        if (EnumName == "END")
        {
            continue;
        }
        if (EnumName == "SCRIPT")
        {
            continue;
        }

        const std::string BaseName = MakeComponentClassBaseName(EnumName);
        if (BaseName.empty())
        {
            continue;
        }

        FComponentInfo Info;
        Info.EnumName = EnumName;
        Info.BaseName = BaseName;
        Info.HeaderFileName = MakeComponentHeaderFileNameFromBaseName(BaseName);
        Info.bHeaderExistsOnDisk = ExistsHeaderAnywhereUnderProject(ProjectDir, Info.HeaderFileName);
        Components.push_back(Info);
    }

    // 정렬은 안정성/가독성용(새로 추가될 때 일관성)
    std::sort(Components.begin(), Components.end(), [](const FComponentInfo& A, const FComponentInfo& B)
        {
            return A.HeaderFileName < B.HeaderFileName;
        });

    // 파일 읽기
    std::string ComponentsTextRaw;
    std::string GameObjectTextRaw;
    std::string ComponentHeaderTextRaw;
    std::string ComponentCppTextRaw;

    if (!ReadAllText(ComponentsPath, ComponentsTextRaw) ||
        !ReadAllText(GameObjectPath, GameObjectTextRaw) ||
        !ReadAllText(ComponentHeaderPath, ComponentHeaderTextRaw) ||
        !ReadAllText(ComponentCppPath, ComponentCppTextRaw))
    {
        std::cout << "ERROR: Failed to read one of target files.\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }

    std::string ComponentsText = NormalizeNewlinesToLF(ComponentsTextRaw);
    std::string GameObjectText = NormalizeNewlinesToLF(GameObjectTextRaw);
    std::string ComponentHeaderText = NormalizeNewlinesToLF(ComponentHeaderTextRaw);
    std::string ComponentCppText = NormalizeNewlinesToLF(ComponentCppTextRaw);

    // components.h: 중복 제거 + 주석 동기화 + 누락 추가
    const bool bComponentsChanged = SyncAndDedupComponentsHeader(ComponentsText, Components);

    // 나머지 3개는 기존 방식 유지(헤더 존재하는 것만)
    const std::unordered_set<std::string> ExistingGameObjectGetters = ParseExistingMacroFirstArgs(GameObjectText, "GET_COMPONENT(");
    const std::unordered_set<std::string> ExistingOtherComponentDecls = ParseExistingMacroFirstArgs(ComponentHeaderText, "GET_OTHER_COMPONENT(");
    const std::unordered_set<std::string> ExistingOtherComponentBodies = ParseExistingMacroFirstArgs(ComponentCppText, "GET_OTHER_COMPONENT_BODY(");

    std::vector<std::string> GameObjectMacroLinesToInsert;
    std::vector<std::string> ComponentHeaderMacroLinesToInsert;
    std::vector<std::string> ComponentCppMacroLinesToAppend;

    for (const FComponentInfo& Info : Components)
    {
        if (!Info.bHeaderExistsOnDisk)
        {
            continue;
        }

        if (ExistingGameObjectGetters.find(Info.BaseName) == ExistingGameObjectGetters.end())
        {
            GameObjectMacroLinesToInsert.push_back("    GET_COMPONENT(" + Info.BaseName + ", " + Info.EnumName + ");");
        }

        if (ExistingOtherComponentDecls.find(Info.BaseName) == ExistingOtherComponentDecls.end())
        {
            ComponentHeaderMacroLinesToInsert.push_back("    GET_OTHER_COMPONENT(" + Info.BaseName + ");");
        }

        if (ExistingOtherComponentBodies.find(Info.BaseName) == ExistingOtherComponentBodies.end())
        {
            ComponentCppMacroLinesToAppend.push_back("GET_OTHER_COMPONENT_BODY(" + Info.BaseName + ");");
        }
    }

    bool bGameObjectChanged = false;
    bool bComponentHeaderChanged = false;
    bool bComponentCppChanged = false;

    if (!GameObjectMacroLinesToInsert.empty())
    {
        const bool bOk = InsertLinesAfterMarkerLine(GameObjectText, "//Component Create Return", GameObjectMacroLinesToInsert);
        if (!bOk)
        {
            std::cout << "ERROR: Marker not found in GameObject.h: \"//Component Create Return\"\n";
            std::string Dummy;
            std::getline(std::cin, Dummy);
            return 1;
        }
        bGameObjectChanged = true;
    }

    if (!ComponentHeaderMacroLinesToInsert.empty())
    {
        const bool bOk = InsertLinesAfterMarkerLine(ComponentHeaderText, "GameObject* GetOwner()", ComponentHeaderMacroLinesToInsert);
        if (!bOk)
        {
            std::cout << "ERROR: Could not find insertion point in Component.h (marker: GameObject* GetOwner())\n";
            std::string Dummy;
            std::getline(std::cin, Dummy);
            return 1;
        }
        bComponentHeaderChanged = true;
    }

    if (!ComponentCppMacroLinesToAppend.empty())
    {
        AppendLinesToFileEnd(ComponentCppText, ComponentCppMacroLinesToAppend);
        bComponentCppChanged = true;
    }

    if (bDryRun)
    {
        std::cout << "DRY RUN:\n";
        std::cout << "  components.h: " << (bComponentsChanged ? "changed" : "no changes") << "\n";
        std::cout << "  GameObject.h: " << (bGameObjectChanged ? "changed" : "no changes") << "\n";
        std::cout << "  Component.h: " << (bComponentHeaderChanged ? "changed" : "no changes") << "\n";
        std::cout << "  Component.cpp: " << (bComponentCppChanged ? "changed" : "no changes") << "\n";
        return 0;
    }

    if (bComponentsChanged && !WriteAllText(ComponentsPath, ComponentsText))
    {
        std::cout << "ERROR: Failed to write: " << ComponentsPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (bGameObjectChanged && !WriteAllText(GameObjectPath, GameObjectText))
    {
        std::cout << "ERROR: Failed to write: " << GameObjectPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (bComponentHeaderChanged && !WriteAllText(ComponentHeaderPath, ComponentHeaderText))
    {
        std::cout << "ERROR: Failed to write: " << ComponentHeaderPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }
    if (bComponentCppChanged && !WriteAllText(ComponentCppPath, ComponentCppText))
    {
        std::cout << "ERROR: Failed to write: " << ComponentCppPath.string() << "\n";
        std::string Dummy;
        std::getline(std::cin, Dummy);
        return 1;
    }

    if (!bComponentsChanged && !bGameObjectChanged && !bComponentHeaderChanged && !bComponentCppChanged)
    {
        std::cout << "OK: No changes.\n";
        return 0;
    }

    std::cout << "OK: Updated";
    if (bComponentsChanged)
    {
        std::cout << " components.h";
    }
    if (bGameObjectChanged)
    {
        std::cout << " GameObject.h";
    }
    if (bComponentHeaderChanged)
    {
        std::cout << " Component.h";
    }
    if (bComponentCppChanged)
    {
        std::cout << " Component.cpp";
    }
    std::cout << "\n";

    return 0;
}
