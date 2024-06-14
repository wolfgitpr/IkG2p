using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
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

            LoadDict(dictDir, "phrases_map.txt", PhrasesMap);
            LoadDict(dictDir, "phrases_dict.txt", PhrasesDict);
            LoadDict(dictDir, "user_dict.txt", PhrasesDict);
            LoadDict(dictDir, "word.txt", WordDict);
            LoadDict(dictDir, "trans_word.txt", TransDict);
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

        public static bool LoadDict(string dictDir, string fileName, Dictionary<string, string> resultMap)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            string resourceName = "IkG2p." + dictDir + "." + fileName;
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            {
                if (stream != null)
                {
                    using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                    {
                        string content = reader.ReadToEnd();
                        string[] lines = content.Split(new[] { Environment.NewLine }, StringSplitOptions.None);

                        foreach (string line in lines)
                        {
                            string trimmedLine = line.Trim();
                            string[] keyValuePair = trimmedLine.Split(':');

                            if (keyValuePair.Length == 2)
                            {
                                string key = keyValuePair[0];
                                string value = keyValuePair[1];
                                resultMap[key] = value;
                            }
                        }

                        return true;
                    }
                }
                else
                {
                    Console.WriteLine($"Resource {fileName} not found.");
                    return false;
                }
            }

        }

        public static bool LoadDict(string dictDir, string fileName, Dictionary<string, List<string>> resultMap)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            string resourceName = "IkG2p." + dictDir + "." + fileName;
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            {
                if (stream != null)
                {
                    using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                    {
                        string content = reader.ReadToEnd();
                        string[] lines = content.Split(new[] { Environment.NewLine }, StringSplitOptions.None);

                        foreach (string line in lines)
                        {
                            string trimmedLine = line.Trim();
                            string[] keyValuePair = trimmedLine.Split(':');

                            if (keyValuePair.Length == 2)
                            {
                                string key = keyValuePair[0];
                                string value = keyValuePair[1];
                                resultMap[key] = value.Split(' ').ToList();
                            }
                        }

                        return true;
                    }
                }
                else
                {
                    Console.WriteLine($"Resource {fileName} not found.");
                    return false;
                }
            }

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

        static List<string> SplitString(string input)
        {
            List<string> res = new List<string>();

            // 正则表达式模式
            string pattern = @"(?![ー゜])([a-zA-Z]+|[+-]|[0-9]|[\u4e00-\u9fa5]|[\u3040-\u309F\u30A0-\u30FF][ャュョゃゅょァィゥェォぁぃぅぇぉ]?)";

            // 使用正则表达式匹配
            MatchCollection matches = Regex.Matches(input, pattern);

            foreach (Match match in matches)
            {
                res.Add(match.Value);
            }

            return res;
        }

        private static List<G2pRes> ResetZH(List<string> input, List<G2pRes> res, List<int> positions)
        {
            List<G2pRes> result = new List<G2pRes>();

            for (int i = 0; i < input.Count(); i++)
            {
                G2pRes g2pRes = new G2pRes(input[i]);
                result.Add(g2pRes);
            }

            for (int i = 0; i < positions.Count(); i++)
            {
                result[positions[i]] = res[i];
            }
            return result;
        }

        private static void AddString(string text, List<string> res)
        {
            var temp = text.Split(' ');
            res.AddRange(temp);
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
            for (int i = 0; i < input.Count; i++)
            {
                string item = input[i];
                if (item == null)
                    continue;

                if (WordDict.ContainsKey(item) || TransDict.ContainsKey(item))
                {
                    res.Add(TradToSim(item));
                    positions.Add(i);
                }

                if (!convertNum)
                    continue;

                if (NumMap.ContainsKey(item))
                {
                    res.Add(item);
                    positions.Add(i);
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
            var cursor = 0;

            while (cursor < inputList.Count)
            {
                var currentChar = inputList[cursor];

                if (!WordDict.ContainsKey(currentChar))
                {
                    G2pRes g2pRes = new G2pRes(currentChar);
                    result.Add(g2pRes);
                    cursor++;
                    continue;
                }

                if (!IsPolyphonic(currentChar))
                {
                    G2pRes g2pRes = new G2pRes(currentChar);
                    g2pRes.candidates = GetDefaultPinyin(currentChar, tone);
                    g2pRes.syllable = g2pRes.candidates == null ? "" : g2pRes.candidates[0];
                    g2pRes.error = false;
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
                            if (PhrasesDict.ContainsKey(subPhrase))
                            {
                                List<string> subRes = PhrasesDict[subPhrase];
                                for (int i = 0; i < subPhrase.Count(); i++)
                                {
                                    var str = inputList[cursor + i];
                                    G2pRes g2pRes = new G2pRes(str, subRes[i]);
                                    g2pRes.candidates = GetDefaultPinyin(str, tone);
                                    g2pRes.error = false;
                                    result.Add(g2pRes);
                                }
                                cursor += length;
                                found = true;
                            }

                            if (cursor >= 1 && !found)
                            {
                                // cursor: 重, subPhrase_1: 语重心长
                                var subPhrase_1 = string.Join("", inputList.GetRange(cursor - 1, length));
                                if (PhrasesDict.ContainsKey(subPhrase_1))
                                {
                                    result.RemoveAt(result.Count - 1);
                                    List<string> subRes = PhrasesDict[subPhrase_1];
                                    for (int i = 0; i < subPhrase_1.Count(); i++)
                                    {
                                        var str = inputList[cursor - 1 + i];
                                        G2pRes g2pRes = new G2pRes(str, subRes[i]);
                                        g2pRes.candidates = GetDefaultPinyin(str, tone);
                                        g2pRes.error = false;
                                        result.Add(g2pRes);
                                    }
                                    cursor += length - 1;
                                    found = true;
                                }
                            }
                        }

                        if (cursor + 1 - length >= 0 && !found && cursor + 1 <= inputList.Count)
                        {
                            // cursor: 好, xSubPhrase: 各有所好
                            var xSubPhrase = string.Join("", inputList.GetRange(cursor + 1 - length, length));
                            if (PhrasesDict.ContainsKey(xSubPhrase))
                            {
                                RemoveElements(result, cursor + 1 - length, length - 1);
                                List<string> subRes = PhrasesDict[xSubPhrase];
                                for (int i = 0; i < xSubPhrase.Count(); i++)
                                {
                                    var str = inputList[cursor + 1 - length + i];
                                    G2pRes g2pRes = new G2pRes(str, subRes[i]);
                                    g2pRes.candidates = GetDefaultPinyin(str, tone);
                                    g2pRes.error = false;
                                    result.Add(g2pRes);
                                }
                                cursor += 1;
                                found = true;
                            }
                        }

                        if (cursor + 2 - length >= 0 && cursor + 2 <= inputList.Count && !found)
                        {
                            // cursor: 好, xSubPhrase: 叶公好龙
                            var xSubPhrase_1 = string.Join("", inputList.GetRange(cursor + 2 - length, length));
                            if (PhrasesDict.ContainsKey(xSubPhrase_1))
                            {
                                RemoveElements(result, cursor + 2 - length, length - 2);
                                List<string> subRes = PhrasesDict[xSubPhrase_1];
                                for (int i = 0; i < xSubPhrase_1.Count(); i++)
                                {
                                    var str = inputList[cursor + 2 - length + i];
                                    G2pRes g2pRes = new G2pRes(str, subRes[i]);
                                    g2pRes.candidates = GetDefaultPinyin(str, tone);
                                    g2pRes.error = false;
                                    result.Add(g2pRes);
                                }
                                cursor += 2;
                                found = true;
                            }
                        }
                    }

                    if (!found)
                    {
                        G2pRes g2pRes = new G2pRes(currentChar);
                        g2pRes.candidates = GetDefaultPinyin(currentChar, tone);
                        g2pRes.syllable = g2pRes.candidates == null ? "" : g2pRes.candidates[0];
                        g2pRes.error = false;
                        result.Add(g2pRes);
                        cursor++;
                    }
                }
            }

            if (!tone)
            {
                for (var i = 0; i < result.Count; i++)
                {
                    if ('0' <= result[i].syllable.Last() && result[i].syllable.Last() <= '9')
                    {
                        G2pRes g2pRes = result[i];
                        g2pRes.syllable = g2pRes.syllable.Remove(g2pRes.syllable.Length - 1);
                        result[i] = g2pRes;
                    }
                }
            }
            return ResetZH(input, result, inputPos);
        }

        bool IsPolyphonic(string text)
        {
            return PhrasesMap.ContainsKey(text);
        }

        string TradToSim(string text)
        {
            return TransDict.ContainsKey(text) ? TransDict[text] : text;
        }

        List<string> GetDefaultPinyin(string text, bool tone)
        {
            var res = WordDict.ContainsKey(text) ? WordDict[text] : null;
            for (var i = 0; i < res.Count; i++)
            {
                if ('0' <= res[i].Last() && res[i].Last() <= '9')
                    res[i] = res[i].Remove(res[i].Length - 1);
            }
            return res;
        }
    }
}