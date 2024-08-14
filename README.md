# IkG2p

## Intro

IkG2p is a lightweight Chinese/Cantonese to Pinyin library.

Chinese dialects can be used to create their own dictionaries using makedict.

[python_version](https://github.com/wolfgitpr/PyIKG2p)

## Feature

+ Only Unicode within the range of  [ 0x4E00 - 0x9FFF ]  is supported.

+ Segmentation for heteronym words.

+ Support Traditional and Simplified Chinese.

+ Speed is very fast, about 500,000 words/s.

## Usage

c#

Using NuGet to search for IKg2p for installation.

```c#
public struct G2pRes
{
    public string lyric;
    public string syllable;
    public List<string> candidates;
    public bool error;   
}

public List<G2pRes> Convert(List<string> input, bool tone = false, bool convertNum = false)

List<G2pRes> pinyin = ZhG2p.MandarinInstance.Convert(lyric, false, true)
List<G2pRes> jyutping = ZhG2p.CantoneseInstance.Convert(lyric, false, true)
```

c++

```c++
#include <filesystem>

const std::string applicationDirPath = std::filesystem::current_path().string();
IKg2p::setDictionaryPath(applicationDirPath + "\\dict");  // Set dictionary path.

const auto g2p_man = std::make_unique<IKg2p::MandarinG2p>();
const std::string raw = "明月@1几32时有##一";
const std::string tar = "ming yue yi ji san er shi you yi";
const auto g2pRes = g2p_man->hanziToPinyin(raw, false, true, IKg2p::errorType::Ignore);
const std::string res = IKg2p::join(g2p_man->resToStringList(g2pRes), " ");
```

##  Doc

```c++
//  cpp/include/G2pRes.h
struct G2pRes
{
    std::string lyric;                      //  utf-8 std::string
    std::string syllable;                   //  utf-8 std::string
    std::vector<std::string> candidates;    //  Candidate Pronunciation of Polyphonic Characters.
    bool error = true;
};

//  cpp/include/ChineseG2p.h
enum class errorType
{
    Default = 0, Ignore = 1
};

/**
    @param input: raw utf-8 std::string.
    @param tone: Preserve the pinyin tone.
    @param convertNum: Convert numbers to pinyin.
    @param errorType: Ignore words that have failed conversion. Default: Keep content.
    @return std::vector<G2pRes>.
*/
std::vector<G2pRes> hanziToPinyin(const std::string& input, bool tone = true, bool convertNum = true,
                                  errorType error = errorType::Default) const;

/**
    @param input: raw utf-8 std::string vector, each element of the vector is a character.
    ...
    @return std::vector<G2pRes>.
*/
std::vector<G2pRes> hanziToPinyin(const std::vector<std::string>& input, bool tone = true,
                                  bool convertNum = true,
                                  errorType error = errorType::Default) const;

/**
    @param input: std::vector<G2pRes>.
    @return utf-8 std::string vector.
*/
std::vector<std::string> resToStringList(const std::vector<G2pRes>& input);

//  Convert to Simplified Chinese.  utf-8 std::string
std::string tradToSim(const std::string& text) const;

//  Determine if it is a polyphonic character.
bool isPolyphonic(const std::string& text) const;

//  Get a pronunciation list.
std::vector<std::string> getDefaultPinyin(const std::string& text, bool tone = true) const;
```

## Open-source softwares used
+ [zh_CN](https://github.com/ZiQiangWang/zh_CN) 
  The core algorithm source has been further tailored to the dictionary in this project.

+ [opencpop](http://wenet.org.cn/opencpop/) 
  The test data source.

+ [M4Singer](https://github.com/M4Singer/M4Singer)
  The test data source.

+ [cc-edict](https://cc-cedict.org/wiki/) 
  The dictionary source.

+ [pinyin](https://github.com/kfcd/pinyin)
  The fan-jian dictionary source.

## Contributors

+ Sine Striker
