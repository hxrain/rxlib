#ifndef _RX_STR_UTIL_EX_H_
#define _RX_STR_UTIL_EX_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
	//-----------------------------------------------------
	//统一封装了对原始字符串处理的函数
	namespace st
	{
		//-------------------------------------------------
		//根据长度拷贝字符串(目标缓冲区不足则失败)
		//入口:dst目标缓冲区;dstMaxSize目标的最大容量;Src源串;SrcLen源串的长度
		//返回值:0失败,参数错误或目标缓冲区不足;其他为实际拷贝的长度
		template<class CT>
		inline uint32_t strcpy(CT* dst, uint32_t dstMaxSize, const CT* Src, uint32_t SrcLen = 0)
		{
			if (!dstMaxSize || !dst)
				return 0;

			dst[0] = 0;
			if (is_empty(Src))
				return 0;

			if (SrcLen == 0)
				SrcLen = strlen(Src);

			if (SrcLen > dstMaxSize - 1)                    //总要保证缓冲区的最后要有一个0串结束符的位置
				return 0;

			strncpy(dst, Src, SrcLen);
			dst[SrcLen] = 0;                                //标记串结束符
			return SrcLen;
		}

		//不超过目标容量,尽量拷贝(能放多少放多少)
		//dst目标缓冲区;MaxSize目标最大容量;Src原串
		//返回值:实际拷贝的长度
		template<class CT>
		inline uint32_t strcpy2(CT* dst, uint32_t dstMaxSize, const CT* Src, uint32_t SrcLen = 0)
		{
			if (!dstMaxSize || !dst)
				return 0;

			dst[0] = 0;
			if (is_empty(Src))
				return 0;

			if (SrcLen == 0)
				SrcLen = strlen(Src);

			if (SrcLen > --dstMaxSize)
				SrcLen = dstMaxSize;	                    //总要保证缓冲区的最后要有一个0串结束符的位置

			strncpy(dst, Src, SrcLen);
			dst[SrcLen] = 0;								//标记串结束符
			return SrcLen;
		}

		//将src拷贝到dst中,src中的某个字符如果在EndChars中则直接结束
		//返回值:实际拷贝的长度
		template<class CT>
		inline uint32_t strcpy2(CT* dst, uint32_t dstMaxSize, const CT* Src, const CT* EndChars)
		{
			if (!dstMaxSize || !dst)
				return 0;
			dst[0] = 0;

			if (EndChars == NULL)
				EndChars = "";

			if (is_empty(Src))
				return 0;

			uint32_t Len = 0;
			CT c;
			while ((c = Src[Len]))
			{//对源串进行逐一遍历
				if (Len + 1 >= dstMaxSize)
					break;									//如果目标不够了则结束
				if (strchr(EndChars, c))
					break;									//如果遇到结束字符了
				dst[Len++] = c;
			}

			dst[Len] = 0;									//标记串结束符
			return Len;
		}
		//-------------------------------------------------
		//将Src拼接到Dst的后面,不论Src与Dst为什么情况,拼接后的分隔符只有一个SP
		template<class CT>
		inline CT* strcat(CT* Dst, const CT* Src, const CT SP)
		{
			if (is_empty(Dst) || !Src)
				return NULL;

			int DstLen = strlen(Dst);
			if (Dst[DstLen - 1] == SP)
			{
				//目标已经是SP结束了
				if (Src[0] == SP)
					strcpy(&Dst[DstLen], &Src[1]);
				else
					strcpy(&Dst[DstLen], Src);
			}
			else
			{
				//目标不是SP结束
				if (Src[0] == SP)
					strcpy(&Dst[DstLen], Src);
				else
				{
					Dst[DstLen++] = SP;
					strcpy(&Dst[DstLen], Src);
				}
			}
			return Dst;
		}
		//将str1和str2仅用sp分隔后拼接到dest中
		//返回值:拼接后的实际长度.0目标缓冲器不足
		template<class CT>
		inline uint32_t strcpy(CT *Dest, uint32_t DestMaxSize, const CT* Src1, const CT* Src2, const CT SP)
		{
			uint32_t L1 = strcpy(Dest, DestMaxSize, Src1);
			if (!L1)
				return 0;
			uint32_t L2 = strlen(Src2);
			if (L1 + L2 > DestMaxSize)
				return 0;
			return strlen(strcat(Dest, Src2, SP));
		}
		//-------------------------------------------------
		//尾部拷贝(取src的后n位)
		template<class CT>
		inline CT*              strrcpy(CT* dst, const CT* src, uint32_t n, uint32_t srclen = 0)
		{
			if (!srclen)
				srclen = strlen(src);
			if (n > srclen)
				n = srclen;
			uint32_t P = n - srclen;
			return strncpy(dst, &src[P], n);
		}
		//-------------------------------------------------
		//字符串转换为大写
		template<class CT>  inline CT *strupr(CT *str)
		{
			for (CT* s = str; *s; ++s)
			{
				if (sc<CT>::is_atoz(*s))
					*s -= sc<CT>::a() - sc<CT>::A();
			}
			return str;
		}
		template<class CT>  inline uint32_t strupr(CT *buff, uint32_t buffsize, const CT *str)
		{
			uint32_t len = 0;
			for (const CT* s = str; *s&&len < buffsize; ++s, ++len)
			{
				if (sc<CT>::is_atoz(*s))
					buff[len] = *s - (sc<CT>::a() - sc<CT>::A());
				else
					buff[len] = *s;
			}
			buff[len] = 0;
			return len;
		}
		//-------------------------------------------------
		//字符串转换为小写
		template<class CT>  inline CT *strlwr(CT *str)
		{
			for (CT* s = str; *s; ++s)
			{
				if (sc<CT>::is_AtoZ(*s))
					*s += sc<CT>::a() - sc<CT>::A();
			}
			return str;
		}
		template<class CT>  inline uint32_t strlwr(CT *buff, uint32_t buffsize, const CT *str)
		{
			--buffsize;
			uint32_t len = 0;
			for (const CT* s = str; *s&&len < buffsize; ++s, ++len)
			{
				if (sc<CT>::is_AtoZ(*s))
					buff[len] = *s + (sc<CT>::a() - sc<CT>::A());
				else
					buff[len] = *s;
			}
			buff[len] = 0;
			return len;
		}
		//字符串转换扩展,直接拷贝到目标缓冲器
		template<class CT>
		inline CT *             strupr(const CT *s, CT* TmpBuf) { strcpy(TmpBuf, s); return strupr(TmpBuf); }
		template<class CT>
		inline CT *             strlwr(const CT *s, CT* TmpBuf) { strcpy(TmpBuf, s); return strlwr(TmpBuf); }

		//-------------------------------------------------
		//语法糖,对输入S进行判断,如果是空值则返回给定的默认值
		template<class CT>
		inline const CT* value(const CT* S, const CT* DefValue)
		{
			if (is_empty(S))
				return DefValue;
			return S;
		}
		//-------------------------------------------------
		//将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就失败了.
		//返回值:0失败;或新串的完整长度
		template<class CT>
		inline uint32_t strcat(CT *Dest, uint32_t DestMaxSize, const CT* Src, uint32_t SrcLen = 0)
		{
			uint32_t DestLen = strlen(Dest);                //计算得到目标现在已有的串长度
			if (SrcLen == 0)
				SrcLen = strlen(Src);                       //计算得到原串现在已有的长度
			uint32_t DestRemainLen = DestMaxSize - DestLen; //计算目标可用容量
			if (SrcLen > DestRemainLen - 1)
				return 0;									//空间不足,不能连接了
			strncpy(&Dest[DestLen], Src, SrcLen);           //拼接拷贝
			uint32_t NewLen = SrcLen + DestLen;             //目标现在的长度
			Dest[NewLen] = 0;                               //确保目标正确结束
			return NewLen;
		}

		//-------------------------------------------------
		//将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就只能拼接一部分.
		//返回值:新串的完整长度
		template<class CT>
		inline uint32_t strcat2(CT *Dest, uint32_t DestMaxSize, const CT* Src, uint32_t SrcLen = 0)
		{
			uint32_t DestLen = strlen(Dest);                //计算得到目标现在已有的串长度
			if (SrcLen == 0)
				SrcLen = strlen(Src);                       //计算得到原串现在已有的长度
			uint32_t DestRemainLen = DestMaxSize - DestLen - 1;   //计算目标可用容量
			if (SrcLen > DestRemainLen)
				SrcLen = DestRemainLen;
			strncpy(&Dest[DestLen], Src, SrcLen);			//拼接拷贝
			uint32_t NewLen = SrcLen + DestLen;				//目标现在的长度
			Dest[NewLen] = 0;                               //确保目标正确结束
			return NewLen;
		}
		//-------------------------------------------------
		//串连接,将Src1和Src2都拼装到Dest中.
		//返回值:0失败;其他为新结果的长度
		template<class CT>
		inline uint32_t strcat(CT *Dest, uint32_t DestMaxSize, const CT* Src1, const CT* Src2)
		{
			uint32_t DestLen = strcpy(Dest, DestMaxSize, Src1);
			uint32_t Src2Len = strlen(Src2);                //计算得到原串现在已有的长度
			uint32_t DestRemainLen = DestMaxSize - DestLen; //计算目标可用容量
			if (Src2Len > DestRemainLen - 1)
				return 0;                                   //空间不足,不能连接了
			strncpy(&Dest[DestLen], Src2, Src2Len);         //拼接拷贝
			uint32_t NewLen = Src2Len + DestLen;            //目标现在的长度
			Dest[NewLen] = 0;                               //确保目标正确结束
			return NewLen;
		}
		//-------------------------------------------------
		//将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就失败了.
		//返回值:0失败;或新串的完整长度
		template<class CT>
		inline uint32_t strcat(CT *Dest, uint32_t &DestLen, uint32_t DestMaxSize, const CT* Src, uint32_t SrcLen = 0)
		{
			if (SrcLen == 0)
				SrcLen = strlen(Src);                       //计算得到原串现在已有的长度
			uint32_t DestRemainLen = DestMaxSize - DestLen; //计算目标可用容量
			if (SrcLen > DestRemainLen - 1)
				return 0;                                   //空间不足,不能连接了
			strncpy(&Dest[DestLen], Src, SrcLen);           //拼接拷贝
			DestLen += SrcLen;                              //目标现在的长度
			Dest[DestLen] = 0;                              //确保目标正确结束
			return DestLen;
		}
		template<class CT>
		inline uint32_t strcat(CT *Dest, uint32_t &DestLen, uint32_t DestMaxSize, CT Src)
		{
			uint32_t DestRemainLen = DestMaxSize - DestLen; //计算目标可用容量
			if (DestRemainLen < 2)
				return 0;                                   //空间不足,不能连接了
			Dest[DestLen] = Src;                            //拼接拷贝
			Dest[++DestLen] = 0;                            //确保目标正确结束
			return DestLen;
		}
		//-------------------------------------------------
		//统计给定的串S中指定字符Char的数量
		template<class CT>
		inline const uint32_t count(const CT *S, const CT Char)
		{
			if (is_empty(S))
				return 0;
			uint32_t Ret = 0;
			const CT *Pos = strchr(S, Char);
			while (Pos)
			{
				++Ret;
				Pos = strchr(++Pos, Char);
			}
			return Ret;
		}
		//统计给定的串S中指定子串Str的数量
		template<class CT>
		inline const uint32_t count(const CT *S, const CT* Str)
		{
			if (is_empty(S))
				return 0;
			uint32_t Ret = 0;
			const CT *Pos = strstr(S, Str);
			while (Pos)
			{
				++Ret;
				Pos = strstr(++Pos, Str);
			}
			return Ret;
		}

		//-------------------------------------------------
		//尝试在Str串指定的偏移位置后面,定位查找SubStr子串
		//返回值:-1未找到;其他为结果偏移量(相对于Str而不是相对于StartIdx)
		template<class CT>
		inline int pos(const CT* Str, const CT *SubStr, uint32_t StartIdx = 0)
		{
			if (is_empty(Str) || StartIdx >= strlen(Str))
				return -1;
			const CT* StartStr = &Str[StartIdx];
			const CT* P = strstr(StartStr, SubStr);
			return (P == NULL ? -1 : P - Str);
		}
		//-------------------------------------------------
		//在src串中顺序查找A与B开始的位置,比如在"abc=[123]456"中查找定位'['与']'出现的位置
		//如果A为NULL,则从src头开始;B为NULL则到src结尾
		//返回值:<0错误;>=0查找成功,为A串的长度
		//出口:PA是A的位置偏移量;PB是B的位置偏移量
		template<class CT>
		inline int pos(const CT* src, uint32_t srclen, const CT* A, const CT* B, uint32_t &PA, uint32_t &PB)
		{
			if (is_empty(src))
				return -1;
			const CT *TP = src;
			uint32_t Alen = 0;
			if (!is_empty(A))
			{
				//有A串,那么就记录A串的长度,准备返回使用;查找A串的位置
				Alen = strlen(A);
				TP = strstr(src, A);
				if (TP != NULL)
					PA = uint32_t(TP - src);
				else
					return -2;
			}
			else
				PA = 0;

			if (!is_empty(B))
			{
				TP = strstr(TP + Alen, B);                  //在A串出现的位置之后查找B串的位置
				if (!TP)
					return -3;
			}
			else
				TP = src + srclen;                          //没有B串,那么就指向src的尾字符

			PB = uint32_t(TP - src);                        //得到B串的出现位置
			return Alen;
		}

		//在当前串中顺序查找A与B开始的位置,比如在"abc=[123]456"中查找定位'['与']'出现的位置
		//如果A为NULL,则从src头开始;B为NULL则到src结尾
		//返回值:true查找成功,A与B都存在,并且B在A的后面;false失败
		//出口:PA是A的位置偏移量;PB是B的位置偏移量
		template<class CT>
		inline bool pos(const CT* src, const CT* A, const CT* B, uint32_t &PA, uint32_t &PB)
		{
			if (src == NULL)
				return false;
			return pos(src, strlen(src), A, B, PA, PB) >= 0;
		}
		//-------------------------------------------------
		//找到在A串和B串之间的字符串,结果中不包含A与B(A为空则从头开始截取,B为空则到尾结束)
		//返回值:0失败;>0为截取的内容长度
		template<class CT>
		inline uint32_t sub(const CT* src, uint32_t srclen, const CT* AStr, const CT* BStr, CT* Result, uint32_t ResultMaxSize)
		{
			if (Result == NULL)
				return 0;
			Result[0] = 0;
			uint32_t APos, BPos;
			int Alen = pos(src, srclen, AStr, BStr, APos, BPos);
			if (Alen < 0)
				return 0;

			uint32_t RL = BPos - APos - Alen;
			if (!RL)
				return 0;
			return strcpy(Result, ResultMaxSize, &src[APos + Alen], RL);
		}
		template<class CT>
		inline uint32_t sub(const CT* src, const CT* AStr, const CT* BStr, CT* Result, uint32_t ResultMaxSize)
		{
			if (Result == NULL)
				return 0;
			Result[0] = 0;
			if (is_empty(src))
				return false;
			return sub(src, strlen(src), AStr, BStr, Result, ResultMaxSize);
		}
		//-------------------------------------------------
		//将src中AStr后面的内容放入结果
		template<class CT>
		inline uint32_t sub(const CT* src, const CT* AStr, CT* Result, uint32_t ResultMaxSize) { return sub(src, AStr, NULL, Result, ResultMaxSize); }
		//-------------------------------------------------
		//截取Str中从首部到SP分隔符之前的内容到Result,Str在截取后调整到SP之后
		//返回值:<0错误;>=0截取到的内容长度
		template<class CT>
		inline int sub(CT* &Str, const CT SP, CT *Result, uint32_t ResultSize)
		{
			if (!Str)
				return -1;
			CT* Pos = strchr(Str, SP);
			if (!Pos)
				return -2;
			uint32_t Len = uint32_t(Pos - Str);
			if (!Len)
			{
				++Str;
				return 0;
			}

			if (strcpy(Result, ResultSize, Str, Len) == Len)
			{
				Str = Pos + 1;
				return Len;
			}
			return -3;
		}
		//-------------------------------------------------
		//截取Str中从首部到SP分隔符之前的内容并转换到整数Result,Str在截取后调整到SP之后
		//返回值:<0错误;>=0截取到的内容长度
		template<class CT>
		inline int sub(CT* &Str, const CT SP, uint32_t &Result, const uint32_t radix = 10)
		{
			if (!Str)
				return -1;
			CT* Pos = strchr(Str, SP);
			if (!Pos)
				return -2;
			*Pos = 0;

			uint32_t Len = uint32_t(Pos - Str);
			Result = atoul(Str, radix);

			Str = Pos + 1;
			return Len;
		}
		//-------------------------------------------------
		//从S中选择第一个Char之前的串到Result中
		//返回值:NULL失败(在S中没有Char);非空,成功(S中当前Char的位置指针)
		template<class CT>
		inline const CT* sub(const CT *S, const CT Char, CT* Result)
		{
			const CT* Ret = strchr(S, Char);
			if (!Ret)
				return NULL;
			uint32_t len = Ret - S;
			strncpy(Result, S, len);
			Result[len] = 0;
			return Ret;
		}
		//-------------------------------------------------
		//在串str的指定长度len范围内查找指定的字符c
		//返回值:len未找到;其他为c出现的位置
		template<typename CT>
		uint32_t strnchr(const CT *str, uint32_t len, CT c)
		{
			for (uint32_t i = 0; i < len; ++i) if (str[i] == c) return i;
			return len;
		}
		//-------------------------------------------------
		//对一个串进Str行遍历,用To替换掉里面所有的From字符
		template<class CT>
		inline CT* replace(CT *Str, CT From, CT To)
		{
			if (is_empty(Str))
				return NULL;
			CT* Ret = Str;
			while ((Str = strchr(Str, From)))
				*Str++ = To;
			return Ret;
		}
		//将Str中的内容复制到Buf中,并将字符From替换为To
		template<class CT>
		inline CT* replace(const CT *Str, CT* Buf, CT From, CT To)
		{
			if (is_empty(Str))
				return NULL;
			while (*Str)
			{
				if (*Str == From)
					*Buf = To;
				else
					*Buf = *Str;
				++Buf;
				++Str;
			}
			return Buf;
		}
		//将Src串中的所有From都替换为To,将结果放入Dst
		//返回值:成功时为Dst,失败时为NULL(目标空间不足)
		template<class CT>
		inline CT* replace(const CT *SrcStr, const CT* From, uint32_t FromLen, const CT* To, uint32_t ToLen, CT* Dst, uint32_t DstSize, uint32_t SrcLen = 0)
		{
			const CT* Src = SrcStr;
			if (is_empty(Src) || is_empty(From) || !Dst)
				return NULL;
			if (!SrcLen)
				SrcLen = strlen(Src);
			if (is_empty(To))
				ToLen = 0;
			const CT* Pos = strstr(Src, From);
			uint32_t DstLen = 0;
			while (Pos)
			{
				uint32_t SegLen = uint32_t(Pos - Src);      //当前目标前面的部分,需要放入结果缓冲区

				if (DstLen + SegLen >= DstSize)
					return NULL;                            //目标缓冲区不足,退出
				strncpy(&Dst[DstLen], Src, SegLen);         //将当前段之前的部分拷贝到目标缓冲区
				DstLen += SegLen;                           //目标长度增加

				if (DstLen + ToLen >= DstSize)
					return NULL;

				if (ToLen)
				{
					strncpy(&Dst[DstLen], To, ToLen);
					DstLen += ToLen;
				}

				Src = Pos + FromLen;
				SrcLen -= SegLen + FromLen;
				Pos = strstr(Src, From);
			}
			strncpy(&Dst[DstLen], Src, SrcLen);             //将剩余部分拷贝到目标
			DstLen += SrcLen;
			Dst[DstLen++] = 0;
			return Dst;
		}
		template<class CT>
		inline CT* replace(const CT *SrcStr, const CT* From, const CT* To, CT* Dst, uint32_t DstSize, uint32_t SrcLen = 0)
		{
			const CT* Src = SrcStr;
			if (is_empty(Src) || is_empty(From) || !Dst)
				return NULL;
			uint32_t FromLen = strlen(From);
			uint32_t ToLen = is_empty(To) ? 0 : strlen(To);
			return replace(SrcStr, From, FromLen, To, ToLen, Dst, DstSize, SrcLen);
		}
		//-------------------------------------------------
		//判断是否为十进制数字字符
		inline bool             isnumber10(char c) { return c >= '0'&&c <= '9'; }
		inline bool             isnumber10(wchar_t c) { return c >= L'0'&&c <= L'9'; }
		template<class CT>
		inline bool             isnumber10(const CT* s)
		{
			if (s == NULL)
				return false;
			for (; *s; ++s)
				if (!isnumber(*s))
					return false;
			return true;
		}
		//判断是否为十进制数字和字母
		template<class CT>
		inline bool             isalnum(CT c) { return sc<CT>::is_0to9(c) || sc<CT>::is_AtoZ(c) || sc<CT>::is_atoz(c); }
		//-------------------------------------------------
		//判断给定的字符串是否全部都为数字(十进制或十六进制整形)
		template<class CT>
		inline bool isnumber(const CT* Str, uint32_t StrLen = 0, bool IsHex = false)
		{
			if (is_empty(Str))
				return false;
			if (!StrLen)
				StrLen = strlen(Str);
			CT C;

			for (uint32_t I = 0; I < StrLen; I++)           //对指定长度的串进行全遍历或遇到结束符
			{
				C = Str[I];
				if (sc<CT>::is_0to9(C))                     //是十进制数字,直接准备判断下一个
					continue;

				if (!IsHex)
					return false;                           //不是十六进制模式,非数字的串出现就返回假

				if (sc<CT>::is_atof(C) || sc<CT>::is_AtoF(C))
					continue;                               //十六进制时,当前字符是合法的,跳过,准备判断下一个.
				else
					return false;                           //否则说明当前不是合法的十六进制数字串
			}
			return true;                                    //全部的字符都检查完成了,说明当前串是合法的数字串.
		}
		//-------------------------------------------------
		//比较数组buff1和buff2的内容是否相同
		//返回值:buff1<buff2,<0;buff1==buff2,=0;buff1>buff2,>0
		template<class CT>
		inline int memcmpx(const CT *buff1, const CT *buff2, uint32_t size)
		{
			for (uint32_t i = 0; i < size; ++i)
			{
				int r = buff1[i] - buff2[i];
				if (r) return r;
			}
			return 0;
		}
		//-------------------------------------------------
		//在主串haystack中查找子串needle,算法来自 https://github.com/kirabou/fast_strstr 并进行了修正
		//返回值:NULL未找到;其他为子串在主串中的开始指针
		template<class CT>
		inline CT *strstrx(const CT *haystack, const CT *needle)
		{
			if (is_empty(needle))                           //子串为空,返回主串头部
				return (CT *)haystack;

			const CT    needle_first = *needle;				//记录子串的首字符

			//在主串中先查找子串首字符出现的位置
			haystack = strchr(haystack, needle_first);
			if (!haystack)
				return NULL;                                //子串的首字符在主串中都不存在

			const CT   *i_haystack = haystack + 1;			//主串的后续比较点
			const CT   *i_needle = needle + 1;				//子串的后续比较点

			int32_t       sums_diff = 0;					//特征和归零
			bool          identical = true;					//指示当前的子串与主串是否相同,初始的时候标记首字符相同

			while (*i_haystack && *i_needle) {              //对主串与子串的后续部分进行最短的遍历,计算初始特征整数
				sums_diff += *i_haystack;                   //特征和针对主串的后续字母累计求和
				sums_diff -= *i_needle;                     //特征和针对子串的后续字母累计求差
				identical &= *i_haystack++ == *i_needle++;  //逐字符判断后续部分是否相同
			}

			if (*i_needle)
				return NULL;                                //存在剩余的子串,说明主串很短
			else if (identical)
				return (CT *)haystack;						//如果子串恰好与主串的当前位置相同了,直接返回

			uint32_t needle_len_1 = uint32_t(i_needle - needle) - 1; //根据上面的循环,现在可计算得到子串的长度预先计算子串长度减一的值

			//对主串的剩余部分进行逐字符循环判断
			const CT* pos = haystack;
			while (*i_haystack)
			{
				sums_diff -= *pos++;                        //特征和针对主串的开始字母累计求差,与面的过程正好相反
				sums_diff += *i_haystack++;                 //特征和针对主串的子串跨度后的字母累计求和

				if (sums_diff == 0                          //当特征值为0的时候
					&& needle_first == *pos                 //且子串的首字符与当前主串指向相同
					&& memcmpx(pos + 1, needle + 1, needle_len_1) == 0//并且当前主串与子串的后续相同
					)
					return (CT *)pos;						//则当前的主串指向就是目标点
			}

			return NULL;
		}
		//-------------------------------------------------
		//在数组haystack中查找元素needle
		template<class CT>
		inline CT* memmemx(const CT *haystack, uint32_t hsize, const CT needle)
		{
			rx_assert(haystack != NULL);
			for (uint32_t i = 0; i < hsize; ++i)
				if (haystack[i] == needle)
					return (CT*)(haystack + i);
			return NULL;
		}
		//-------------------------------------------------
		//在主数组haystack中查找子数组needle出现的位置
		//返回值:NULL未找到;其他为子串在主串中的开始指针
		template<class CT>
		inline CT *memmemx(const CT *haystack, uint32_t hsize, const CT *needle, uint32_t nsize)
		{
			rx_assert(haystack != NULL);
			if (nsize > hsize) return NULL;                 //子串大于主串,直接返回
			if (needle == NULL || nsize == 0)               //子串为空,返回主串头部
				return (CT *)haystack;

			const CT    needle_first = *needle;				//记录子串的首字符
			const CT   *haystack_begin = haystack;

			//在主串中先查找子串首字符出现的位置
			haystack = memmemx(haystack, hsize, needle_first);
			if (!haystack)
				return NULL;                                //子串的首字符在主串中都不存在

			const CT   *i_haystack = haystack + 1;			//主串的后续比较点
			const CT   *i_needle = needle + 1;				//子串的后续比较点

			int32_t       sums_diff = 0;					//特征和归零
			bool          identical = true;					//指示当前的子串与主串是否相同,初始的时候标记首字符相同

			for (uint32_t i = 1; i < nsize; ++i) {          //对主串与子串的后续部分进行最短的遍历,计算初始特征整数
				sums_diff += *i_haystack;                   //特征和针对主串的后续字母累计求和
				sums_diff -= *i_needle;                     //特征和针对子串的后续字母累计求差
				identical &= *i_haystack++ == *i_needle++;  //逐字符判断后续部分是否相同
			}

			if (identical)
				return (CT *)haystack;						//如果子串恰好与主串的当前位置相同了,直接返回

			uint32_t needle_len_1 = nsize - 1;				//计算子串长度减一的值
			uint32_t remain_hsize = hsize - (uint32_t)(haystack - haystack_begin);

			//对主串的剩余部分进行逐字符循环判断
			const CT* pos = haystack;
			for (uint32_t i = 0; i < remain_hsize; ++i)
			{
				sums_diff -= *pos++;                        //特征和针对主串的开始字母累计求差,与面的过程正好相反
				sums_diff += *i_haystack++;                 //特征和针对主串的子串跨度后的字母累计求和

				if (sums_diff == 0                          //当特征值为0的时候
					&& needle_first == *pos                 //且子串的首字符与当前主串指向相同
					&& memcmpx(pos + 1, needle + 1, needle_len_1) == 0//并且当前主串与子串的后续相同
					)
					return (CT *)pos;						//则当前的主串指向就是目标点
			}

			return NULL;
		}
	}
}


#endif
