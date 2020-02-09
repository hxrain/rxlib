#ifndef _RX_STR_UTIL_BIN_H_
#define _RX_STR_UTIL_BIN_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
    //-----------------------------------------------------
    //统一封装了对原始字符串处理的函数
    namespace st
    {
        //-----------------------------------------------------
        //将数字转换成十进制字符串:num=数字;result=结果串;C为结果串长度(数字长度不足的前面补零)
        template<class CT>
        inline CT* dec(uint32_t num,CT *result,uint32_t C)
        {
            while (C-->0)
            {
                result[C]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
                num/=10;
            }
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后两位,不足补零
        template<class CT>
        inline CT* dec2(uint32_t num,CT *result)
        {
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后三位,不足补零
        template<class CT>
        inline CT* dec3(uint32_t num,CT *result)
        {
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            return result;
        }

        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后四位,不足补零
        template<class CT>
        inline CT* dec4(uint32_t num,CT *result)
        {
            result[3]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后八位,不足补零
        template<class CT>
        inline CT* dec8(uint32_t num,CT *result)
        {
            result[7]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[6]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[5]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[4]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[3]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero);
            return result;
        }

        //将4bit数字转换成十六进制字符
        template<class CT>
        inline CT hex(uint32_t num)
        {
            rx_assert(num<=0x0F);
            return sc<CT>::hex_upr(num);
        }
        //-----------------------------------------------------
        //将数字转换成十六进制字符串:num=数字;result=结果串;C为结果串长度(数字长度不足的前面补零)
        template<class CT>
        inline CT* hex(uint32_t num,CT *result,uint32_t C)
        {
            while (C-->0)
            {
                result[C]=sc<CT>::hex_upr(num&0xf);
                num>>=4;
            }
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十六进制串,只转换数字的最后两位,不足补零
        template<class CT>
        inline CT* hex2(uint32_t num,CT *result)
        {
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            return result;
        }

        //-----------------------------------------------------
        //将数字num转换为十六进制串,只转换数字的最后四位,不足补零
        template<class CT>
        inline CT* hex4(uint32_t num,CT *result)
        {
            result[3]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[2]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十六进制串,转换数字的后八位,不足补零
        template<class CT>
        inline CT* hex8(uint32_t num,CT *result)
        {
            result[7]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[6]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[5]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[4]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[3]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[2]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            return result;
        }
        //-----------------------------------------------------
        //将二进制数据流转换成十六进制串
        template<class CT>
        inline CT* hex(const uint8_t *bin, uint32_t size,CT *result,bool IsLower=false)
        {
            CT *Map=sc<CT>::hex_upr();
            if (!IsLower)
                Map=sc<CT>::hex_lwr();

            for (; size--; bin++)
            {
                *result++ = Map[bin[0] >> 4];
                *result++ = Map[bin[0] & 0x0f];
            }
            *result = 0;
            return result;
        }
        //将二进制数据转换为16进制字符串,每个字节间使用空格间隔,每隔LineLength长度进行一次换行.
        //返回值:已经处理过的数据数量,与size相同的时候为正常完成.
        template<class CT>
        inline uint32_t hex(const uint8_t* bin, uint32_t size, CT* result, uint32_t result_size, bool IsLower = false, uint32_t LineLength = 32)
        {
            uint32_t RL = 0;
            uint32_t i = 0;
            for (; i < size; ++i)
            {
                if (i)
                {
                    if (i%LineLength == 0)
                    {
                        if (RL + 2 >= result_size)
                            break;
                        result[RL++] = sc<CT>::CR();
                        result[RL++] = sc<CT>::LF();
                    }
                    else
                    {
                        if (RL + 1 >= result_size)
                            break;
                        result[RL++] = sc<CT>::space();
                    }
                }
                if (RL + 2 >= result_size)
                    break;
                hex2(bin[i], &result[RL]);
                RL += 2;
            }
            result[RL] = 0;
            return i;
        }
        //-----------------------------------------------------
        //将给定的字符C(A~F|a~f|0~9)转为对应的数字(字节)
        template<class CT>
        inline uint8_t byte(CT C)
        {
            if (sc<CT>::is_atof(C))
                return C-sc<CT>::a+10;
            else if (sc<CT>::is_AtoF(C))
                return C-sc<CT>::A+10;
            else
                return C-sc<CT>::zero;
        }
        //-----------------------------------------------------
        //将给定的十六进制形式的字符串中的开头的两个字符转换为一个字节长度的数字
        template<class CT>
        inline uint8_t byte(const CT *str)
        {
            uint8_t R=byte<CT>((CT)str[0]);
            R<<=4;
            return R+byte<CT>((CT)str[1]);
        }
        //-----------------------------------------------------
        //将指定长度的十六进制串转换成对应的二进制字节流
        //入口:Str,StrLen待转换的串和长度
        //      Data,DataSize接收字节流的缓冲区和数据长度(调用前告知缓冲区尺寸,调用后为字节流长度)
        //返回值:NULL转换失败;其他为字节流缓冲区指针
        template<class CT>
        inline uint8_t* bin(const CT* Str,uint32_t StrLen,uint8_t* Data,uint32_t &DataSize)
        {
            if (is_empty(Str)||Data==NULL)
                return NULL;
            if (StrLen%2!=0||StrLen/2>DataSize)
                return NULL;
            DataSize=0;
            for(uint32_t i=0; Str[i]&&i<StrLen; i+=2)
                Data[DataSize++]=byte(&Str[i]);
            return Data;
        }
        template<class CT>
        inline uint8_t* bin(const CT* Str,uint32_t StrLen,uint8_t* Data,const uint32_t DataSize)
        {
            uint32_t DS=DataSize;
            return bin(Str,StrLen,Data,DS);
        }
        //-------------------------------------------------
    }
}


#endif
