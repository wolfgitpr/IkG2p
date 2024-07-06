import os
from collections import Counter

import zhconv


class MakeDict:
    def __init__(self, out_path, overwrite_pinyin, add_pinyin, trans_dict=None):
        if trans_dict is None:
            trans_dict = {}
        self.out_path = out_path
        self.overwrite_pinyin = overwrite_pinyin
        self.add_pinyin = add_pinyin
        self.transdict = trans_dict

        self.phrases_dict_out = {}
        self.phrase_pinyin_dict = {}
        self.pos_dict = {}
        self.phrase_map = {}
        self.default_pinyin = {}
        self.pos_map_dict = {}
        self.map_keys = []
        self.short_phrase_dict = {}
        self.new_phrase_dict = {}

        print(f"make dict -> {self.out_path}")

        os.makedirs(self.out_path, exist_ok=True)
        self.load_dict()
        self.count_phrase()
        self.fill_unicode_pinyin()
        self.add_default_pinyin()
        self.extract_polyphone()
        self.clip_long_phrase()
        self.make_dict()

    def load_dict(self):
        pass

    def count_phrase(self):
        for k, v in self.phrase_pinyin_dict.items():
            values = v.split(" ")
            phrase_size = len(values)
            if len(k) < 10:
                for text, pinyin in zip(k, values):
                    if phrase_size > 1:
                        self.pos_dict.setdefault(text, []).append(phrase_size)
                    self.phrase_map.setdefault(text, []).append(pinyin)
        # 统计频率
        for k, v in self.phrase_map.items():
            value = v
            count = Counter(value)
            sorted_list = sorted(count.items(), key=lambda x: x[1], reverse=True)
            self.phrase_map[k] = [word for word, count in sorted_list]

        # 默认拼音，{k:中文,v:最高频拼音}
        self.default_pinyin = {k: v for k, v in self.phrase_map.items()}
        for k, v in self.overwrite_pinyin.items():
            self.default_pinyin[k] = [v] + [x for x in self.default_pinyin.get(k, []) if x != v]

    # 补充Unicode码中剩余中文:拼音
    def fill_unicode_pinyin(self):
        pass

    def add_default_pinyin(self):
        for k, v in self.add_pinyin.items():
            self.default_pinyin[k] = [v]

    # 提取多音字
    def extract_polyphone(self):
        # 中文:多音字词长list
        self.pos_dict = {k: list(set(v)) for k, v in self.pos_dict.items()}
        for k, v in self.phrase_map.items():
            if len(v) > 1:
                self.pos_map_dict[k] = (self.phrase_map[k], "".join([str(x) for x in self.pos_dict[k]]))
        # 多音字列表
        self.map_keys = self.pos_map_dict.keys()

    # 裁剪长词
    def clip_long_phrase(self):
        # 2字短语
        for k, v in self.phrase_pinyin_dict.items():
            if len(k) == 2:
                self.short_phrase_dict[k] = v

        # 以短词裁剪长词
        for raw_phrase, raw_pinyin in self.phrase_pinyin_dict.items():
            if len(raw_phrase) > 2:
                clip_phrase = raw_phrase
                clip_pinyin = raw_pinyin.split(" ")

                # 长词首尾2个字在短词列表内且读音相同者均删除，重复操作数次
                for _ in range(3):
                    short_phrase = clip_phrase[:2]
                    if short_phrase in self.short_phrase_dict.keys():
                        if self.short_phrase_dict[short_phrase] == " ".join(clip_pinyin[:2]):
                            clip_phrase = clip_phrase[2:]
                            clip_pinyin = clip_pinyin[2:]

                    short_phrase = clip_phrase[-2:]
                    if short_phrase in self.short_phrase_dict.keys() and self.short_phrase_dict[
                        short_phrase] == " ".join(clip_pinyin[-2:]):
                        clip_phrase = clip_phrase[:-2]
                        clip_pinyin = clip_pinyin[:-2]

                # 剩余词语，读音与默认拼音相异者输出
                if len(clip_phrase):
                    sta = False
                    for idx in range(len(clip_phrase)):
                        text = clip_phrase[idx]
                        if self.default_pinyin.get(text) and idx < len(clip_pinyin) and clip_pinyin[idx] != \
                                self.default_pinyin[text][0]:
                            sta = True
                            break
                    if sta:
                        self.phrases_dict_out[raw_phrase] = raw_pinyin

        # 可用默认拼音拼读的2字短词筛除，剩余加入输出
        for k, v in self.short_phrase_dict.items():
            if self.default_pinyin.get(k[0]) and self.default_pinyin.get(k[1]):
                search = f"{self.default_pinyin.get(k[0])[0]} {self.default_pinyin.get(k[1])[0]}"
                if search != v:
                    self.phrases_dict_out[k] = v

    # 输出
    def make_dict(self):
        self.pos_dict.clear()
        with open(f"{self.out_path}/phrases_dict.txt", "w", encoding='utf-8') as f:
            for raw_phrase, raw_pinyin in self.phrases_dict_out.items():
                clip_pinyin = raw_pinyin.split(" ")
                phrase_size = len(raw_phrase)
                if 1 < phrase_size == len(clip_pinyin) <= 4:
                    f.write(f"{raw_phrase}:{raw_pinyin}\n")
                    for i, (text, pinyin) in enumerate(zip(raw_phrase, clip_pinyin)):
                        if text in self.map_keys:
                            self.pos_dict.setdefault(text, []).append(phrase_size)

        with open(f"{self.out_path}/phrases_map.txt", "w", encoding='utf-8') as f:
            for k, v in self.pos_dict.items():
                map_pos = "".join([str(x) for x in list(set(v))])
                f.write(f"{k}:{map_pos}\n")

        with open(f"{self.out_path}/word.txt", "w", encoding='utf-8') as f:
            for k, v in self.default_pinyin.items():
                if len(k) == 1:
                    v_list = " ".join([item for item in v if item])
                    f.write(f"{k}:{v_list}\n")

        with open(f"{self.out_path}/trans_word.txt", "w", encoding='utf-8') as f:
            for k, v in self.default_pinyin.items():
                t_k = zhconv.convert(k, "zh-hant")
                if t_k != k:
                    f.write(f"{t_k}:{k}\n")

            for k, v in self.transdict.items():
                if k != v and k not in self.default_pinyin.keys() and v in self.default_pinyin.keys():
                    f.write(f"{k}:{v}\n")
