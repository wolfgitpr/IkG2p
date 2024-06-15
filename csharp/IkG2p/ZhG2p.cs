using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace IKg2p
{
    public struct G2pRes
    {
        public string lyric;
        public string syllable;
        public List<string> candidates;
        public bool error;
        public G2pRes(string lyric)
        {
            this.lyric = lyric;
            this.syllable = lyric;
            candidates = new List<string>();
            error = true;
        }

        public G2pRes(string lyric, string syllable)
        {
            this.lyric = lyric;
            this.syllable = syllable;
            candidates = new List<string>();
            error = true;
        }
    }

    public class ZhG2p
    {
        private static Lazy<ZhG2p> mandarinInstance = new Lazy<ZhG2p>(() => new ZhG2p("mandarin"));
        private static Lazy<ZhG2p> cantoneseInstance = new Lazy<ZhG2p>(() => new ZhG2p("cantonese"));

        public static ZhG2p MandarinInstance => mandarinInstance.Value;
        public static ZhG2p CantoneseInstance => cantoneseInstance.Value;

        private Dictionary<string, string> PhrasesMap = new Dictionary<string, string>();
        private Dictionary<string, string> TransDict = new Dictionary<string, string>();
        private Dictionary<string, List<string>> WordDict = new Dictionary<string, List<string>>();
        private Dictionary<string, List<string>> PhrasesDict = new Dictionary<string, List<string>>();

        public ZhG2p(string language)
        {
            string dictDir;
            if (language == "mandarin")
            {
                dictDir = "dict.mandarin";
            }
            else
            {
                dictDir = "dict.cantonese";
            }

            IkG2p.Util.LoadDict(dictDir, "phrases_map.txt", PhrasesMap);
            IkG2p.Util.LoadDict(dictDir, "phrases_dict.txt", PhrasesDict);
            IkG2p.Util.LoadDict(dictDir, "user_dict.txt", PhrasesDict);
            IkG2p.Util.LoadDict(dictDir, "word.txt", WordDict);
            IkG2p.Util.LoadDict(dictDir, "trans_word.txt", TransDict);
        }

        public static List<string> ToStrList(List<G2pRes> input)
        {
            List<string> res = new List<string>();
            for (int i = 0; i < input.Count; i++)
            {
                res.Add(input[i].error ? input[i].lyric : input[i].syllable);
            }
            return res;
        }

        private static readonly Dictionary<string, string> NumMap = new Dictionary<string, string>
        {
            {"0", "零"},
            {"1", "一"},
            {"2", "二"},
            {"3", "三"},
            {"4", "四"},
            {"5", "五"},
            {"6", "六"},
            {"7", "七"},
            {"8", "八"},
            {"9", "九"}
        };

        public static List<string> SplitString(string input)
        {
            string pattern = @"(?![ー゜])([a-zA-Z]+|[+-]|[0-9]|[\u4e00-\u9fa5]|[\u3040-\u309F\u30A0-\u30FF][ャュョゃゅょァィゥェォぁぃぅぇぉ]?)";
            return Regex.Matches(input, pattern).Cast<Match>().Select(m => m.Value).ToList();
        }

        private static List<G2pRes> ResetZH(List<string> input, List<G2pRes> res, List<int> positions)
        {
            var result = input.Select(i => new G2pRes(i)).ToList();
            for (int i = 0; i < positions.Count; i++)
            {
                result[positions[i]] = res[i];
            }
            return result;
        }

        private static void RemoveElements(List<G2pRes> list, int start, int n)
        {
            if (start >= 0 && start < list.Count && n > 0)
            {
                int countToRemove = Math.Min(n, list.Count - start);
                list.RemoveRange(start, countToRemove);
            }
        }

        private void ZhPosition(List<string> input, List<string> res, List<int> positions, bool convertNum)
        {
            foreach (var (item, index) in input.Select((item, index) => (item, index)))
            {
                if (item == null)
                    continue;

                if (WordDict.ContainsKey(item) || TransDict.ContainsKey(item))
                {
                    res.Add(TradToSim(item));
                    positions.Add(index);
                }

                if (convertNum && NumMap.ContainsKey(item))
                {
                    res.Add(NumMap[item]);
                    positions.Add(index);
                }
            }
        }

        public List<G2pRes> Convert(string input, bool tone = false, bool covertNum = false)
        {
            return Convert(SplitString(input), tone, covertNum);
        }

        public List<G2pRes> Convert(List<string> input, bool tone = false, bool convertNum = false)
        {
            var inputList = new List<string>();
            var inputPos = new List<int>();
            ZhPosition(input, inputList, inputPos, convertNum);
            var result = new List<G2pRes>();
            int cursor = 0;

            while (cursor < inputList.Count)
            {
                var currentChar = inputList[cursor];

                if (!IsHanzi(currentChar))
                {
                    result.Add(new G2pRes(currentChar));
                    cursor++;
                    continue;
                }

                if (!IsPolyphonic(currentChar))
                {
                    var g2pRes = new G2pRes(currentChar)
                    {
                        candidates = GetDefaultPinyin(currentChar, tone),
                        syllable = GetDefaultPinyin(currentChar, tone).FirstOrDefault() ?? "",
                        error = false
                    };
                    result.Add(g2pRes);
                    cursor++;
                }
                else
                {
                    var found = false;
                    for (var length = 4; length >= 2 && !found; length--)
                    {
                        if (cursor + length <= inputList.Count)
                        {
                            // cursor: 地, subPhrase: 地久天长
                            var subPhrase = string.Join("", inputList.GetRange(cursor, length));

                            if (PhrasesDict.TryGetValue(subPhrase, out var subRes))
                            {
                                for (int i = 0; i < subPhrase.Length; i++)
                                {
                                    result.Add(new G2pRes(inputList[cursor + i], subRes[i])
                                    {
                                        candidates = GetDefaultPinyin(inputList[cursor + i], tone),
                                        error = false
                                    });
                                }
                                cursor += length;
                                found = true;
                            }
                        }

                        if (cursor >= 1 && cursor + length <= inputList.Count && !found)
                        {
                            // cursor: 重, subPhrase_1: 语重心长
                            var subPhrase = string.Join("", inputList.GetRange(cursor - 1, length));
                            if (PhrasesDict.TryGetValue(subPhrase, out var subRes))
                            {
                                result.RemoveAt(result.Count - 1);
                                for (int i = 0; i < subPhrase.Length; i++)
                                {
                                    result.Add(new G2pRes(inputList[cursor - 1 + i], subRes[i])
                                    {
                                        candidates = GetDefaultPinyin(inputList[cursor - 1 + i], tone),
                                        error = false
                                    });
                                }
                                cursor += length - 1;
                                found = true;
                            }
                        }

                        if (cursor + 1 - length >= 0 && !found && cursor + 1 <= inputList.Count)
                        {
                            // cursor: 好, xSubPhrase: 各有所好
                            var subPhrase = string.Join("", inputList.GetRange(cursor + 1 - length, length));
                            if (PhrasesDict.TryGetValue(subPhrase, out var subRes))
                            {
                                result.RemoveRange(result.Count - (length - 1), length - 1);
                                for (int i = 0; i < subPhrase.Length; i++)
                                {
                                    result.Add(new G2pRes(inputList[cursor + 1 - length + i], subRes[i])
                                    {
                                        candidates = GetDefaultPinyin(inputList[cursor + 1 - length + i], tone),
                                        error = false
                                    });
                                }
                                cursor += 1;
                                found = true;
                            }
                        }

                        if (cursor + 2 - length >= 0 && cursor + 2 <= inputList.Count && !found)
                        {
                            // cursor: 好, xSubPhrase: 叶公好龙
                            var subPhrase = string.Join("", inputList.GetRange(cursor + 2 - length, length));
                            if (PhrasesDict.TryGetValue(subPhrase, out var subRes))
                            {
                                result.RemoveRange(result.Count - (length - 2), length - 2);
                                for (int i = 0; i < subPhrase.Length; i++)
                                {
                                    result.Add(new G2pRes(inputList[cursor + 2 - length + i], subRes[i])
                                    {
                                        candidates = GetDefaultPinyin(inputList[cursor + 2 - length + i], tone),
                                        error = false
                                    });
                                }
                                cursor += 2;
                                found = true;
                            }
                        }
                    }

                    if (!found)
                    {
                        var g2pRes = new G2pRes(currentChar)
                        {
                            candidates = GetDefaultPinyin(currentChar, tone),
                            syllable = GetDefaultPinyin(currentChar, tone).FirstOrDefault() ?? "",
                            error = false
                        };
                        result.Add(g2pRes);
                        cursor++;
                    }
                }
            }

            if (!tone)
            {
                for (var i = 0; i < result.Count; i++)
                {
                    G2pRes g2pRes = result[i];
                    if (char.IsDigit(g2pRes.syllable.Last()))
                    {
                        g2pRes.syllable = g2pRes.syllable.Remove(g2pRes.syllable.Length - 1);
                        result[i] = g2pRes;
                    }
                }
            }
            return ResetZH(input, result, inputPos);
        }

        public bool IsHanzi(string text)
        {
            return WordDict.ContainsKey(TradToSim(text));
        }

        public bool IsPolyphonic(string text)
        {
            return PhrasesMap.ContainsKey(text);
        }

        public string TradToSim(string text)
        {
            return TransDict.ContainsKey(text) ? TransDict[text] : text;
        }

        public List<string> GetDefaultPinyin(string text, bool tone)
        {
            var res = WordDict.ContainsKey(text) ? WordDict[text] : new List<string> { text };
            for (var i = 0; i < res.Count; i++)
            {
                if (char.IsDigit(res[i].Last()))
                    res[i] = res[i].Remove(res[i].Length - 1);
            }
            return res;
        }
    }
}