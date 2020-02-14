#ifndef _RX_UCS2_MISC_H_
#define _RX_UCS2_MISC_H_

#include "rx_cc_macro.h"
#include "rx_dtl_alg.h"

namespace rx
{
    //定义unicode空白字符
    static const uint32_t ucs_white_spaces[] = {
        0x9, 0xA, 0xB, 0xC, 0xD, 0x20, 0x85, 0xA0, 0x1680, 0x180E, 0x2000,
        0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009,
        0x200A, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000
    };
    static const uint32_t ucs_white_spaces_size=sizeof(ucs_white_spaces)/sizeof(ucs_white_spaces[0]);

    //-----------------------------------------------------
    //判断给定的unicode字符c是否为空白或空格
    inline bool is_ucs_white_spaces(uint32_t c)
    {
        return bisect(ucs_white_spaces,ucs_white_spaces_size,c)<ucs_white_spaces_size;
    }
    //-----------------------------------------------------
    //unicode全角字符转换为ascii字符
    inline uint32_t ucs_sbc_to_ascii(uint32_t code,bool conv_space=true)
    {
        //全角符号范围,包含部分与ascii兼容的全角标点/大小字母/小写字母.
        const uint32_t _SBC_CASE_SIGN_LOW = 0xff01;         //全角标点符号开始,0xff01,L'！'
        const uint32_t _SBC_CASE_SIGN_HIGH = 0xff5e;          //全角标点符号结束,0xff5e,L'～'
        const uint32_t _SBC_CASE_LOWER_DIFF = _SBC_CASE_SIGN_LOW - L'!'; //全角标点符号与半角标点符号间的距离,0xfee0

        if ((_SBC_CASE_SIGN_LOW <= code)&&(code <= _SBC_CASE_SIGN_HIGH))
            return code - _SBC_CASE_LOWER_DIFF;             //全角转半角
        if (conv_space&&is_ucs_white_spaces(code))
            return L' ';                                    //空白转空格
        return code;
    }

    //半角ascii字符转为全角ucs字符
    inline uint32_t ucs_ascii_to_sbc(uint8_t code,bool conv_space=true)
    {
        if (conv_space && code == ' ')
            return 0x3000;

        if (code >= 0x21 && code <= 0x7e)
            return code + 0xfee0;

        return code;
    }

    //判断uc是否为全角字母.返回值:1大写全角字母;2小写全角字母;0不是全角字母
    inline uint32_t is_ucs_sbc_alpha(uint32_t uc)
    {
        if (uc >= 0xff21 && uc <= 0xff3a)
            return 1;                                       //大写

        if (uc >= 0xff41 && uc <= 0xff5a)
            return 2;                                       //小写

        return 0;
    }

    //判断uc是否为全角数字
    inline bool is_ucs_sbc_digit(uint32_t uc)
    {
        if (uc >= 0xff10 && uc <= 0xff19)
            return true;

        return false;
    }

    //判断是否为CJK中日韩越统一表意文字
    inline int is_ucs_cjk(uint32_t uc)
    {
        // 最初期统一汉字
        if (uc >= 0x4e00 && uc <= 0x9fcc)
            return 1;                                       //CJK_UNIFIED_IDEOGRAPHS

        // 扩展 A 区
        if (uc >= 0x3400 && uc <= 0x4db5)
            return 2;                                       //CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A

        // 扩展 B 区
        if (uc >= 0x20000 && uc <= 0x2a6d6)
            return 3;                                       //CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B

        // 扩展 C 区
        if (uc >= 0x2a700 && uc <= 0x2b734)
            return 4;                                       //CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C

        // 扩展 D 区
        if (uc >= 0x2b740 && uc <= 0x2b81f)
            return 5;                                       //CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D

        // 扩展 E 区
        if (uc >= 0x2b820 && uc <= 0x2f7ff)
            return 6;                                       //NULL,保留

        // 台湾兼容汉字
        if (uc >= 0x2f800 && uc <= 0x2fa1d)
            return 7;                                       //CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT

        // 兼容汉字
        if (uc >= 0xfa30 && uc <= 0xfa6d)
            return 10;                                      //属于CJK_COMPATIBILITY_IDEOGRAPHS块

        // 北朝鲜兼容汉字
        if (uc >= 0xfa70 && uc <= 0xfad9)
            return 8;                                       //属于CJK_COMPATIBILITY_IDEOGRAPHS块

        // 兼容汉字
        if (uc >= 0xf900 && uc <= 0xfa2d)
            return 9;                                       //CJK_COMPATIBILITY_IDEOGRAPHS


        return 0;
    }

    //判断uc是否为中文标点
    inline int is_ucs_zh_punct(uint32_t uc)
    {
        if (uc >= 0x3001 && uc <= 0x3002)
            return 1;

        if (uc >= 0x3008 && uc <= 0x300f)
            return 1;

        if (uc >= 0xff01 && uc <= 0xff0f)
            return 1;

        if (uc >= 0xff1a && uc <= 0xff20)
            return 1;

        if (uc >= 0xff3b && uc <= 0xff40)
            return 1;

        if (uc >= 0xff5b && uc <= 0xff5e)
            return 1;

        if (uc >= 0x2012 && uc <= 0x201f)
            return 1;

        if (uc >= 0xfe41 && uc <= 0xfe44)
            return 1;

        if (uc >= 0xfe49 && uc <= 0xfe4f)
            return 1;

        if (uc >= 0x3010 && uc <= 0x3017)
            return 1;

        return 0;
    }

    //判断uc是否为日文平假名
    inline int is_ucs_hiragana(uint32_t uc)
    {
        if (uc >= 0x3040 && uc <= 0x309f)
            return 1;

        return 0;
    }

    //判断uc是否为日文片假名
    inline int is_ucs_katakana(uint32_t uc)
    {
        if (uc >= 0x30a0 && uc <= 0x30ff)
            return 1;

        if (uc >= 0x31f0 && uc <= 0x31ff)
            return 2;

        return 0;
    }

    //判断uc是否为韩文
    inline int is_ucs_korean(uint32_t uc)
    {
        //韩文拼音
        if (uc >= 0xac00 && uc <= 0xd7af)
            return 1;

        //韩文字母
        if (uc >= 0x1100 && uc <= 0x11ff)
            return 2;

        //韩文兼容字母
        if (uc >= 0x3130 && uc <= 0x318f)
            return 3;

        return 0;
    }

    //-----------------------------------------------------
    //unicode字符集代码块条目定义
    typedef struct ucs2_block_item_t
    {
        uint32_t    code_begin;                             //代码块的开始字符集编码
        const char* block_name;                             //代码块的名字
        bool operator==(const ucs2_block_item_t& x)const{return code_begin==x.code_begin;}
        bool operator>(const ucs2_block_item_t& x)const{return code_begin>x.code_begin;}
        bool operator<(const ucs2_block_item_t& x)const{return code_begin<x.code_begin;}
        bool operator<=(const ucs2_block_item_t& x)const{return code_begin<=x.code_begin;}
    }ucs2_block_item_t;

    //unicode代码块范围数组
    static const ucs2_block_item_t ucs2_blocks_range[]={
        {0x0000,NULL},
        {0x0020,"SPACE"},
        {0x0021,"BASIC_PUNCTUATION_A"},
        {0x0030,"DIGIT"},
        {0x003A,"BASIC_PUNCTUATION_B"},
        {0x0041,"BASIC_LATIN_A"},
        {0x005B,"BASIC_PUNCTUATION_C"},
        {0x0061,"BASIC_LATIN_B"},
        {0x007B,"BASIC_PUNCTUATION_D"},
        {0x007F,NULL},
        {0x00A0,"LATIN_1_SUPPLEMENT"},
        {0x00C0,"LATIN_EXTENDED_LETTER"},
        {0x0100,"LATIN_EXTENDED_A"},
        {0x0180,"LATIN_EXTENDED_B"},
        {0x0250,"IPA_EXTENSIONS"},
        {0x02B0,"SPACING_MODIFIER_LETTERS"},
        {0x0300,"COMBINING_DIACRITICAL_MARKS"},
        {0x0370,"GREEK"},
        {0x0400,"CYRILLIC"},
        {0x0500,"CYRILLIC_SUPPLEMENTARY"},
        {0x0530,"ARMENIAN"},
        {0x0590,"HEBREW"},
        {0x0600,"ARABIC"},
        {0x0700,"SYRIAC"},
        {0x0750,"ARABIC_SUPPLEMENT"},
        {0x0780,"THAANA"},
        {0x07C0,"NKO"},
        {0x0800,"SAMARITAN"},
        {0x0840,"MANDAIC"},
        {0x0860,NULL},
        {0x0900,"DEVANAGARI"},
        {0x0980,"BENGALI"},
        {0x0A00,"GURMUKHI"},
        {0x0A80,"GUJARATI"},
        {0x0B00,"ORIYA"},
        {0x0B80,"TAMIL"},
        {0x0C00,"TELUGU"},
        {0x0C80,"KANNADA"},
        {0x0D00,"MALAYALAM"},
        {0x0D80,"SINHALA"},
        {0x0E00,"THAI"},
        {0x0E80,"LAO"},
        {0x0F00,"TIBETAN"},
        {0x1000,"MYANMAR"},
        {0x10A0,"GEORGIAN"},
        {0x1100,"HANGUL_JAMO"},
        {0x1200,"ETHIOPIC"},
        {0x1380,"ETHIOPIC_SUPPLEMENT"},
        {0x13A0,"CHEROKEE"},
        {0x1400,"UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS"},
        {0x1680,"OGHAM"},
        {0x16A0,"RUNIC"},
        {0x1700,"TAGALOG"},
        {0x1720,"HANUNOO"},
        {0x1740,"BUHID"},
        {0x1760,"TAGBANWA"},
        {0x1780,"KHMER"},
        {0x1800,"MONGOLIAN"},
        {0x18B0,"UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED"},
        {0x1900,"LIMBU"},
        {0x1950,"TAI_LE"},
        {0x1980,"NEW_TAI_LUE"},
        {0x19E0,"KHMER_SYMBOLS"},
        {0x1A00,"BUGINESE"},
        {0x1A20,"TAI_THAM"},
        {0x1AB0,NULL},
        {0x1B00,"BALINESE"},
        {0x1B80,"SUNDANESE"},
        {0x1BC0,"BATAK"},
        {0x1C00,"LEPCHA"},
        {0x1C50,"OL_CHIKI"},
        {0x1C80,NULL},
        {0x1CD0,"VEDIC_EXTENSIONS"},
        {0x1D00,"PHONETIC_EXTENSIONS"},
        {0x1D80,"PHONETIC_EXTENSIONS_SUPPLEMENT"},
        {0x1DC0,"COMBINING_DIACRITICAL_MARKS_SUPPLEMENT"},
        {0x1E00,"LATIN_EXTENDED_ADDITIONAL"},
        {0x1F00,"GREEK_EXTENDED"},
        {0x2000,"GENERAL_PUNCTUATION"},
        {0x2070,"SUPERSCRIPTS_AND_SUBSCRIPTS"},
        {0x20A0,"CURRENCY_SYMBOLS"},
        {0x20D0,"COMBINING_MARKS_FOR_SYMBOLS"},
        {0x2100,"LETTERLIKE_SYMBOLS"},
        {0x2150,"NUMBER_FORMS"},
        {0x2190,"ARROWS"},
        {0x2200,"MATHEMATICAL_OPERATORS"},
        {0x2300,"MISCELLANEOUS_TECHNICAL"},
        {0x2400,"CONTROL_PICTURES"},
        {0x2440,"OPTICAL_CHARACTER_RECOGNITION"},
        {0x2460,"ENCLOSED_ALPHANUMERICS"},
        {0x2500,"BOX_DRAWING"},
        {0x2580,"BLOCK_ELEMENTS"},
        {0x25A0,"GEOMETRIC_SHAPES"},
        {0x2600,"MISCELLANEOUS_SYMBOLS"},
        {0x2700,"DINGBATS"},
        {0x27C0,"MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A"},
        {0x27F0,"SUPPLEMENTAL_ARROWS_A"},
        {0x2800,"BRAILLE_PATTERNS"},
        {0x2900,"SUPPLEMENTAL_ARROWS_B"},
        {0x2980,"MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B"},
        {0x2A00,"SUPPLEMENTAL_MATHEMATICAL_OPERATORS"},
        {0x2B00,"MISCELLANEOUS_SYMBOLS_AND_ARROWS"},
        {0x2C00,"GLAGOLITIC"},
        {0x2C60,"LATIN_EXTENDED_C"},
        {0x2C80,"COPTIC"},
        {0x2D00,"GEORGIAN_SUPPLEMENT"},
        {0x2D30,"TIFINAGH"},
        {0x2D80,"ETHIOPIC_EXTENDED_A"},
        {0x2DE0,"CYRILLIC_EXTENDED_A"},
        {0x2E00,"SUPPLEMENTAL_PUNCTUATION"},
        {0x2E80,"CJK_RADICALS_SUPPLEMENT"},
        {0x2F00,"KANGXI_RADICALS"},
        {0x2FE0,NULL},
        {0x2FF0,"IDEOGRAPHIC_DESCRIPTION_CHARACTERS"},
        {0x3000,"CJK_SYMBOLS_AND_PUNCTUATION_A"},
        {0x3041,"HIRAGANA"},
        {0x3097,"CJK_SYMBOLS_AND_PUNCTUATION_B"},
        {0x30A1,"KATAKANA_A"},
        {0x30FB,"CJK_SYMBOLS_AND_PUNCTUATION_C"},
        {0x30FC,"KATAKANA_B"},
        {0x3100,"BOPOMOFO"},
        {0x3130,"HANGUL_COMPATIBILITY_JAMO"},
        {0x3190,"KANBUN"},
        {0x31A0,"BOPOMOFO_EXTENDED"},
        {0x31C0,"CJK_STROKES"},
        {0x31F0,"KATAKANA_PHONETIC_EXTENSIONS"},
        {0x3200,"ENCLOSED_CJK_LETTERS_AND_MONTHS"},
        {0x3300,"CJK_COMPATIBILITY"},
        {0x3400,"CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A"},
        {0x4DC0,"YIJING_HEXAGRAM_SYMBOLS"},
        {0x4E00,"CJK_UNIFIED_IDEOGRAPHS"},
        {0xA000,"YI_SYLLABLES"},
        {0xA490,"YI_RADICALS"},
        {0xA4D0,"LISU"},
        {0xA500,"VAI"},
        {0xA640,"CYRILLIC_EXTENDED_B"},
        {0xA6A0,"BAMUM"},
        {0xA700,"MODIFIER_TONE_LETTERS"},
        {0xA720,"LATIN_EXTENDED_D"},
        {0xA800,"SYLOTI_NAGRI"},
        {0xA830,"COMMON_INDIC_NUMBER_FORMS"},
        {0xA840,"PHAGS_PA"},
        {0xA880,"SAURASHTRA"},
        {0xA8E0,"DEVANAGARI_EXTENDED"},
        {0xA900,"KAYAH_LI"},
        {0xA930,"REJANG"},
        {0xA960,"HANGUL_JAMO_EXTENDED_A"},
        {0xA980,"JAVANESE"},
        {0xA9E0,NULL},
        {0xAA00,"CHAM"},
        {0xAA60,"MYANMAR_EXTENDED_A"},
        {0xAA80,"TAI_VIET"},
        {0xAAE0,NULL},
        {0xAB00,"ETHIOPIC_EXTENDED_A"},
        {0xAB30,NULL},
        {0xABC0,"MEETEI_MAYEK"},
        {0xAC00,"HANGUL_SYLLABLES"},
        {0xD7B0,"HANGUL_JAMO_EXTENDED_B"},
        {0xD800,"HIGH_SURROGATES"},
        {0xDB80,"HIGH_PRIVATE_USE_SURROGATES"},
        {0xDC00,"LOW_SURROGATES"},
        {0xE000,"PRIVATE_USE_AREA"},
        {0xF900,"CJK_COMPATIBILITY_IDEOGRAPHS"},
        {0xFB00,"ALPHABETIC_PRESENTATION_FORMS"},
        {0xFB50,"ARABIC_PRESENTATION_FORMS_A"},
        {0xFE00,"VARIATION_SELECTORS"},
        {0xFE10,"VERTICAL_FORMS"},
        {0xFE20,"COMBINING_HALF_MARKS"},
        {0xFE30,"CJK_COMPATIBILITY_FORMS"},
        {0xFE50,"SMALL_FORM_VARIANTS"},
        {0xFE70,"ARABIC_PRESENTATION_FORMS_B"},
        {0xFF00,"HALFWIDTH_AND_FULLWIDTH_FORMS_A"},
        {0xFF10,"FULLWIDTH_DIGIT"},
        {0xFF1A,"HALFWIDTH_AND_FULLWIDTH_FORMS_B"},
        {0xFF21,"FULLWIDTH_LATIN_A"},
        {0xFF3B,"HALFWIDTH_AND_FULLWIDTH_FORMS_C"},
        {0xFF41,"FULLWIDTH_LATIN_B"},
        {0xFF5B,"HALFWIDTH_AND_FULLWIDTH_FORMS_D"},
        {0xFFF0,"SPECIALS"},
        {0x10000,"LINEAR_B_SYLLABARY"},
        {0x10080,"LINEAR_B_IDEOGRAMS"},
        {0x10100,"AEGEAN_NUMBERS"},
        {0x10140,"ANCIENT_GREEK_NUMBERS"},
        {0x10190,"ANCIENT_SYMBOLS"},
        {0x101D0,"PHAISTOS_DISC"},
        {0x10200,NULL},
        {0x10280,"LYCIAN"},
        {0x102A0,"CARIAN"},
        {0x102E0,NULL},
        {0x10300,"OLD_ITALIC"},
        {0x10330,"GOTHIC"},
        {0x10350,NULL},
        {0x10380,"UGARITIC"},
        {0x103A0,"OLD_PERSIAN"},
        {0x103E0,NULL},
        {0x10400,"DESERET"},
        {0x10450,"SHAVIAN"},
        {0x10480,"OSMANYA"},
        {0x104B0,NULL},
        {0x10800,"CYPRIOT_SYLLABARY"},
        {0x10840,"IMPERIAL_ARAMAIC"},
        {0x10860,NULL},
        {0x10900,"PHOENICIAN"},
        {0x10920,"LYDIAN"},
        {0x10940,NULL},
        {0x10A00,"KHAROSHTHI"},
        {0x10A60,"OLD_SOUTH_ARABIAN"},
        {0x10A80,NULL},
        {0x10B00,"AVESTAN"},
        {0x10B40,"INSCRIPTIONAL_PARTHIAN"},
        {0x10B60,"INSCRIPTIONAL_PAHLAVI"},
        {0x10B80,NULL},
        {0x10C00,"OLD_TURKIC"},
        {0x10C50,NULL},
        {0x10E60,"RUMI_NUMERAL_SYMBOLS"},
        {0x10E80,NULL},
        {0x11000,"BRAHMI"},
        {0x11080,"KAITHI"},
        {0x110D0,NULL},
        {0x12000,"CUNEIFORM"},
        {0x12400,"CUNEIFORM_NUMBERS_AND_PUNCTUATION"},
        {0x12480,NULL},
        {0x13000,"EGYPTIAN_HIEROGLYPHS"},
        {0x13430,NULL},
        {0x16800,"BAMUM_SUPPLEMENT"},
        {0x16A40,NULL},
        {0x1B000,"KANA_SUPPLEMENT"},
        {0x1B100,NULL},
        {0x1D000,"BYZANTINE_MUSICAL_SYMBOLS"},
        {0x1D100,"MUSICAL_SYMBOLS"},
        {0x1D200,"ANCIENT_GREEK_MUSICAL_NOTATION"},
        {0x1D250,NULL},
        {0x1D300,"TAI_XUAN_JING_SYMBOLS"},
        {0x1D360,"COUNTING_ROD_NUMERALS"},
        {0x1D380,NULL},
        {0x1D400,"MATHEMATICAL_ALPHANUMERIC_SYMBOLS"},
        {0x1D800,NULL},
        {0x1F000,"MAHJONG_TILES"},
        {0x1F030,"DOMINO_TILES"},
        {0x1F0A0,"PLAYING_CARDS"},
        {0x1F100,"ENCLOSED_ALPHANUMERIC_SUPPLEMENT"},
        {0x1F200,"ENCLOSED_IDEOGRAPHIC_SUPPLEMENT"},
        {0x1F300,"MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS"},
        {0x1F600,"EMOTICONS"},
        {0x1F650,NULL},
        {0x1F680,"TRANSPORT_AND_MAP_SYMBOLS"},
        {0x1F700,"ALCHEMICAL_SYMBOLS"},
        {0x1F780,NULL},
        {0x20000,"CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B"},
        {0x2A6E0,NULL},
        {0x2A700,"CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C"},
        {0x2B740,"CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D"},
        {0x2B820,NULL},
        {0x2F800,"CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT"},
        {0x2FA20,NULL},
        {0xE0000,"TAGS"},
        {0xE0080,NULL},
        {0xE0100,"VARIATION_SELECTORS_SUPPLEMENT"},
        {0xE01F0,NULL},
        {0xF0000,"SUPPLEMENTARY_PRIVATE_USE_AREA_A"},
        {0x100000,"SUPPLEMENTARY_PRIVATE_USE_AREA_B"},
        {0x10FFFF,NULL}
    };
    //unicode块范围条目数组长度
    const uint32_t ucs2_blocks_range_size=sizeof(ucs2_blocks_range)/sizeof(ucs2_blocks_range[0]);

    //根据给定的unicode编码code,查询得到其所属的unicode块信息的索引
    //返回值:ucs2_blocks_range_size查询失败;其他为ucs2_blocks_range的索引
    inline const uint32_t query_ucs_block(uint32_t code)
    {
        ucs2_block_item_t x;
        x.code_begin=code;
        return bisect_ll(ucs2_blocks_range,ucs2_blocks_range_size,x);
    }

    //根据给定的unicode编码code,查询得到其所属的unicode块信息的准确边缘的索引
    //返回值:ucs2_blocks_range_size查询失败;其他为ucs2_blocks_range的索引
    inline const uint32_t query_ucs_block_edge(uint32_t code)
    {
        ucs2_block_item_t x;
        x.code_begin=code;
        return bisect(ucs2_blocks_range,ucs2_blocks_range_size,x);
    }
}


#endif
