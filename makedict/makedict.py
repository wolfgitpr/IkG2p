import re

import ToJyutping
import pypinyin
import zhconv

from common import MakeDict


class MakeMandarin(MakeDict):
    def __init__(self, out_path, overwrite_pinyin, add_pinyin, transdict):
        super().__init__(out_path, overwrite_pinyin, add_pinyin, transdict)

    def load_dict(self):
        # 获取词组
        with open("cedict_ts.u8", "r", encoding="utf-8") as f:
            for line in f:
                res = re.search(r"(.*[\u4e00-\u9fa5]) (.*[\u4e00-\u9fa5]) \[([\w :]+)] (\{([\w :]+)})?", line)
                char = res.group(2) if res else None
                if char and not re.search("[0-9A-Za-z·:，]", char):
                    if res.group(3):
                        pinyin = res.group(3).lower().replace("u:", "v")
                        values = ["er5" if i == "r5" else i for i in pinyin.split(" ")]

                        self.phrase_pinyin_dict[char] = " ".join(values)

    def fill_unicode_pinyin(self):
        for i in range(0x4E00, 0x9FFF + 1):
            text = chr(i)
            text = zhconv.convert(text, "zh-cn")
            if self.default_pinyin.get(text) is None:
                pinyin = pypinyin.pinyin(text, style=pypinyin.TONE3)[0][0]
                if re.search(r"([^a-z\d])", pinyin) is None:
                    self.default_pinyin[text] = [pinyin]


class MakeCantonese(MakeDict):
    def __init__(self, out_path, overwrite_pinyin, add_pinyin, transdict):
        super().__init__(out_path, overwrite_pinyin, add_pinyin, transdict)

    def load_dict(self):
        with open("cccedict-canto-readings.txt", "r", encoding="utf-8") as f:
            for line in f:
                res = re.search(r"(.*[\u4e00-\u9fa5]) (.*[\u4e00-\u9fa5]) \[([\w :]+)] (\{([\w :]+)})?", line)
                key = res.group(2) if res else None
                if key and not re.search("[0-9A-Za-z·:，]", key):
                    if res.group(5):
                        pinyin = res.group(5).lower()
                        values = [i for i in pinyin.split(" ")]
                        self.phrase_pinyin_dict[key] = " ".join(values)

    def fill_unicode_pinyin(self):
        for i in range(0x4E00, 0x9FFF + 1):
            text = chr(i)
            text = zhconv.convert(text, "zh-cn")
            if self.default_pinyin.get(text) is None:
                pinyin = ToJyutping.get_jyutping(text)
                if re.search(r"([^a-z\d])", pinyin) is None:
                    self.default_pinyin[text] = [pinyin]


mandarin = True

out_path = "dict/mandarin" if mandarin else "dict/cantonese"
overwrite_pinyin = {
    "儿": "er5", "了": "le5", "呢": "ne5", "曾": "ceng2", "重": "chong2", "地": "de5", "藏": "cang2", "都": "dou1",
    "还": "hai2", "弹": "tan2", "着": "zhe5", "的": "de5", "哦": "o4", "盛": "sheng4", "哟": "you5", "喔": "o1",
    "湮": "yan1", "拓": "ta4", "系": "xi4", "谁": "shei2", "什": "shen2", "么": "me5", "扛": "kang2"
}

extra_pinyin = {"濛": "meng2", "尅": "kei2"}

chinese_transdict = {}
with open("fanjian.txt", "r", encoding="utf-8") as f:
    for line in f:
        k, v = line.strip('\n').split('	')
        chinese_transdict[k] = v

if mandarin:
    MakeMandarin(out_path, overwrite_pinyin, extra_pinyin, chinese_transdict)
else:
    MakeCantonese(out_path, {}, {}, chinese_transdict)
