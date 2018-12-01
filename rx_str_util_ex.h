#ifndef _RX_STR_UTIL_EX_H_
#define _RX_STR_UTIL_EX_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
    //-----------------------------------------------------
    //ͳһ��װ�˶�ԭʼ�ַ�������ĺ���
    namespace st
    {
        //-------------------------------------------------
        //���ݳ��ȿ����ַ���(Ŀ�껺����������ʧ��)
        //���:dstĿ�껺����;dstMaxSizeĿ����������;SrcԴ��;SrcLenԴ���ĳ���
        //����ֵ:0ʧ��,���������Ŀ�껺��������;����Ϊʵ�ʿ����ĳ���
        template<class CT>
        inline uint32_t strcpy(CT* dst, uint32_t dstMaxSize, const CT* Src, uint32_t SrcLen = 0)
        {
            if (!dstMaxSize)
                return 0;

            dst[0] = 0;
            if (is_empty(Src))
                return 0;

            if (SrcLen == 0)
                SrcLen = strlen(Src);

            if (SrcLen>dstMaxSize - 1)                      //��Ҫ��֤�����������Ҫ��һ��0����������λ��
                return 0;

            strncpy(dst, Src, SrcLen);
            dst[SrcLen] = 0;                                //��Ǵ�������
            return SrcLen;
        }

        //������Ŀ������,��������(�ܷŶ��ٷŶ���)
        //dstĿ�껺����;MaxSizeĿ���������;Srcԭ��
        //����ֵ:ʵ�ʿ����ĳ���
        template<class CT>
        inline uint32_t strcpy2(CT* dst, uint32_t dstMaxSize, const CT* Src, uint32_t SrcLen = 0)
        {
            if (!dstMaxSize)
                return 0;

            dst[0] = 0;
            if (is_empty(Src))
                return 0;

            if (SrcLen == 0)
                SrcLen = strlen(Src);

            if (SrcLen>--dstMaxSize)
                SrcLen = dstMaxSize;	                    //��Ҫ��֤�����������Ҫ��һ��0����������λ��

            strncpy(dst, Src, SrcLen);
            dst[SrcLen] = 0;								//��Ǵ�������
            return SrcLen;
        }
        //-------------------------------------------------
        //��Srcƴ�ӵ�Dst�ĺ���,����Src��DstΪʲô���,ƴ�Ӻ�ķָ���ֻ��һ��SP
        template<class CT>
        inline CT* strcat(CT* Dst, const CT* Src, const CT SP)
        {
            if (is_empty(Dst) || !Src)
                return NULL;

            int DstLen = strlen(Dst);
            if (Dst[DstLen - 1] == SP)
            {
                //Ŀ���Ѿ���SP������
                if (Src[0] == SP)
                    strcpy(&Dst[DstLen], &Src[1]);
                else
                    strcpy(&Dst[DstLen], Src);
            }
            else
            {
                //Ŀ�겻��SP����
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
        //��str1��str2����sp�ָ���ƴ�ӵ�dest��
        //����ֵ:ƴ�Ӻ��ʵ�ʳ���.0Ŀ�껺��������
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
        //β������(ȡsrc�ĺ�nλ)
        template<class CT>
        inline CT*              strrcpy(CT* dst,const CT* src,uint32_t n,uint32_t srclen=0)
        {
            if (!srclen)
                srclen=strlen(src);
            if (n>srclen)
                n=srclen;
            uint32_t P=n-srclen;
            return strncpy(dst,&src[P],n);
        }
        //-------------------------------------------------
        //�ַ���ת��Ϊ��д
        template<class CT>  inline CT *strupr(CT *str)
        {
            for(CT* s=str;*s;++s)
            {
                if(sc<CT>::is_atoz(*s))
                    *s -= sc<CT>::a()-sc<CT>::A();
            }
            return str;
        }
        template<class CT>  inline uint32_t strupr(CT *buff,uint32_t buffsize,const CT *str)
        {
            uint32_t len = 0;
            for (const CT* s = str; *s&&len < buffsize; ++s,++len)
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
        //�ַ���ת��ΪСд
        template<class CT>  inline CT *strlwr(CT *str)
        {
            for(CT* s=str;*s;++s)
            {
                if(sc<CT>::is_AtoZ(*s))
                    *s += sc<CT>::a()-sc<CT>::A();
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
        //�ַ���ת����չ,ֱ�ӿ�����Ŀ�껺����
        template<class CT>
        inline CT *             strupr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strupr(TmpBuf);}
        template<class CT>
        inline CT *             strlwr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strlwr(TmpBuf);}

        //-------------------------------------------------
        //�﷨��,������S�����ж�,����ǿ�ֵ�򷵻ظ�����Ĭ��ֵ
        template<class CT>
        inline const CT* value(const CT* S,const CT* DefValue)
        {
            if (is_empty(S))
                return DefValue;
            return S;
        }
        //-------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ʧ����.
        //����ֵ:0ʧ��;���´�����������
        template<class CT>
        inline uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //����õ�Ŀ���������еĴ�����
            if (SrcLen==0)
                SrcLen=strlen(Src);                         //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1)
                return 0;                                   //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            uint32_t NewLen=SrcLen+DestLen;                 //Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }

        //-------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ֻ��ƴ��һ����.
        //����ֵ:�´�����������
        template<class CT>
        inline uint32_t strcat2(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //����õ�Ŀ���������еĴ�����
            if (SrcLen==0)
                SrcLen=strlen(Src);                         //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen-1;   //����Ŀ���������
            if (SrcLen>DestRemainLen)
                SrcLen=DestRemainLen;
            strncpy(&Dest[DestLen],Src,SrcLen);			    //ƴ�ӿ���
            uint32_t NewLen=SrcLen+DestLen;					//Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }
        //-------------------------------------------------
        //������,��Src1��Src2��ƴװ��Dest��.
        //����ֵ:0ʧ��;����Ϊ�½���ĳ���
        template<class CT>
        inline uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src1,const CT* Src2)
        {
            uint32_t DestLen=strcpy(Dest,DestMaxSize,Src1);
            uint32_t Src2Len=strlen(Src2);                  //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (Src2Len>DestRemainLen-1)
                return 0;                                   //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src2,Src2Len);           //ƴ�ӿ���
            uint32_t NewLen=Src2Len+DestLen;                //Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }
        //-------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ʧ����.
        //����ֵ:0ʧ��;���´�����������
        template<class CT>
        inline uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            if (SrcLen==0)
                SrcLen=strlen(Src);                         //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1)
                return 0;                                   //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            DestLen+=SrcLen;                                //Ŀ�����ڵĳ���
            Dest[DestLen]=0;                                //ȷ��Ŀ����ȷ����
            return DestLen;
        }
        template<class CT>
        inline uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,CT Src)
        {
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (DestRemainLen<2)
                return 0;                                   //�ռ䲻��,����������
            Dest[DestLen]=Src;                              //ƴ�ӿ���
            Dest[++DestLen]=0;                              //ȷ��Ŀ����ȷ����
            return DestLen;
        }
        //-------------------------------------------------
        //ͳ�Ƹ����Ĵ�S��ָ���ַ�Char������
        template<class CT>
        inline const uint32_t count(const CT *S,const CT Char)
        {
            if (is_empty(S))
                return 0;
            uint32_t Ret=0;
            const CT *Pos=strchr(S,Char);
            while(Pos)
            {
                ++Ret;
                Pos=strchr(++Pos,Char);
            }
            return Ret;
        }
        //ͳ�Ƹ����Ĵ�S��ָ���Ӵ�Str������
        template<class CT>
        inline const uint32_t count(const CT *S,const CT* Str)
        {
            if (is_empty(S))
                return 0;
            uint32_t Ret=0;
            const CT *Pos=strstr(S,Str);
            while(Pos)
            {
                ++Ret;
                Pos=strstr(++Pos,Str);
            }
            return Ret;
        }

        //-------------------------------------------------
        //������Str��ָ����ƫ��λ�ú���,��λ����SubStr�Ӵ�
        //����ֵ:-1δ�ҵ�;����Ϊ���ƫ����(�����Str�����������StartIdx)
        template<class CT>
        inline int pos(const CT* Str,const CT *SubStr,uint32_t StartIdx=0)
        {
            if (is_empty(Str)||StartIdx>=strlen(Str))
                return -1;
            const CT* StartStr=&Str[StartIdx];
            const CT* P=strstr(StartStr,SubStr);
            return (P==NULL?-1:P-Str);
        }
        //-------------------------------------------------
        //��src����˳�����A��B��ʼ��λ��,������"abc=[123]456"�в��Ҷ�λ'['��']'���ֵ�λ��
        //���AΪNULL,���srcͷ��ʼ;BΪNULL��src��β
        //����ֵ:<0����;>=0���ҳɹ�,ΪA���ĳ���
        //����:PA��A��λ��ƫ����;PB��B��λ��ƫ����
        template<class CT>
        inline int pos(const CT* src,uint32_t srclen,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (is_empty(src))
                return -1;
            const CT *TP=src;
            uint32_t Alen=0;
            if (!is_empty(A))
            {
                //��A��,��ô�ͼ�¼A���ĳ���,׼������ʹ��;����A����λ��
                Alen=strlen(A);
                TP=strstr(src,A);
                if (TP!=NULL)
                    PA=uint32_t(TP-src);
                else
                    return -2;
            }
            else
                PA=0;

            if (!is_empty(B))
            {
                TP=strstr(TP+Alen,B);                       //��A�����ֵ�λ��֮�����B����λ��
                if (!TP)
                    return -3;
            }
            else
                TP=src+srclen;                              //û��B��,��ô��ָ��src��β�ַ�

            PB=uint32_t(TP-src);                            //�õ�B���ĳ���λ��
            return Alen;
        }

        //�ڵ�ǰ����˳�����A��B��ʼ��λ��,������"abc=[123]456"�в��Ҷ�λ'['��']'���ֵ�λ��
        //���AΪNULL,���srcͷ��ʼ;BΪNULL��src��β
        //����ֵ:true���ҳɹ�,A��B������,����B��A�ĺ���;falseʧ��
        //����:PA��A��λ��ƫ����;PB��B��λ��ƫ����
        template<class CT>
        inline bool pos(const CT* src,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (src==NULL)
                return false;
            return pos(src,strlen(src),A,B,PA,PB)>=0;
        }
        //-------------------------------------------------
        //�ҵ���A����B��֮����ַ���,����в�����A��B(AΪ�����ͷ��ʼ��ȡ,BΪ����β����)
        //����ֵ:0ʧ��;>0Ϊ��ȡ�����ݳ���
        template<class CT>
        inline uint32_t sub(const CT* src,uint32_t srclen,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return 0;
            Result[0]=0;
            uint32_t APos,BPos;
            int Alen=pos(src,srclen,AStr,BStr,APos,BPos);
            if (Alen<0)
                return 0;

            uint32_t RL=BPos-APos-Alen;
            if (!RL)
                return 0;
            return strcpy(Result,ResultMaxSize,&src[APos+Alen],RL);
        }
        template<class CT>
        inline uint32_t sub(const CT* src,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return 0;
            Result[0]=0;
            if (is_empty(src))
                return false;
            return sub(src,strlen(src),AStr,BStr,Result,ResultMaxSize);
        }
        //-------------------------------------------------
        //��src��AStr��������ݷ�����
        template<class CT>
        inline uint32_t sub(const CT* src,const CT* AStr,CT* Result,uint32_t ResultMaxSize) {return sub(src,AStr,NULL,Result,ResultMaxSize);}
        //-------------------------------------------------
        //��ȡStr�д��ײ���SP�ָ���֮ǰ�����ݵ�Result,Str�ڽ�ȡ�������SP֮��
        //����ֵ:<0����;>=0��ȡ�������ݳ���
        template<class CT>
        inline int sub(CT* &Str,const CT SP,CT *Result,uint32_t ResultSize)
        {
            if (!Str)
                return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos)
                return -2;
            uint32_t Len=uint32_t(Pos-Str);
            if (!Len)
            {
                ++Str;
                return 0;
            }

            if (strcpy(Result,ResultSize,Str,Len)==Len)
            {
                Str=Pos+1;
                return Len;
            }
            return -3;
        }
        //-------------------------------------------------
        //��ȡStr�д��ײ���SP�ָ���֮ǰ�����ݲ�ת��������Result,Str�ڽ�ȡ�������SP֮��
        //����ֵ:<0����;>=0��ȡ�������ݳ���
        template<class CT>
        inline int sub(CT* &Str,const CT SP,uint32_t &Result,const uint32_t radix=10)
        {
            if (!Str)
                return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos)
                return -2;
            *Pos=0;

            uint32_t Len=uint32_t(Pos-Str);
            Result=atoul(Str,radix);

            Str=Pos+1;
            return Len;
        }
        //-------------------------------------------------
        //��S��ѡ���һ��Char֮ǰ�Ĵ���Result��
        //����ֵ:NULLʧ��(��S��û��Char);�ǿ�,�ɹ�(S�е�ǰChar��λ��ָ��)
        template<class CT>
        inline const CT* sub(const CT *S,const CT Char,CT* Result)
        {
            const CT* Ret=strchr(S,Char);
            if (!Ret)
                return NULL;
            uint32_t len=Ret-S;
            strncpy(Result,S,len);
            Result[len]=0;
            return Ret;
        }
        //-------------------------------------------------
        //��һ������Str�б���,��To�滻���������е�From�ַ�
        template<class CT>
        inline CT* replace(CT *Str,CT From,CT To)
        {
            if (is_empty(Str))
                return NULL;
            CT* Ret=Str;
            while((Str=strchr(Str,From)))
                *Str++=To;
            return Ret;
        }
        //��Str�е����ݸ��Ƶ�Buf��,�����ַ�From�滻ΪTo
        template<class CT>
        inline CT* replace(const CT *Str,CT* Buf,CT From, CT To)
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
        //��Src���е�����From���滻ΪTo,���������Dst
        //����ֵ:�ɹ�ʱΪDst,ʧ��ʱΪNULL(Ŀ��ռ䲻��)
        template<class CT>
        inline CT* replace(const CT *SrcStr,const CT* From,uint32_t FromLen,const CT* To,uint32_t ToLen,CT* Dst,uint32_t DstSize,uint32_t SrcLen=0)
        {
            const CT* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            if (!SrcLen)
                SrcLen=strlen(Src);
            if (is_empty(To))
                ToLen=0;
            const CT* Pos=strstr(Src,From);
            uint32_t DstLen=0;
            while(Pos)
            {
                uint32_t SegLen=uint32_t(Pos-Src);          //��ǰĿ��ǰ��Ĳ���,��Ҫ������������

                if (DstLen+SegLen>=DstSize)
                    return NULL;                            //Ŀ�껺��������,�˳�
                strncpy(&Dst[DstLen],Src,SegLen);           //����ǰ��֮ǰ�Ĳ��ֿ�����Ŀ�껺����
                DstLen+=SegLen;                             //Ŀ�곤������

                if (DstLen+ToLen>=DstSize)
                    return NULL;

                if (ToLen)
                {
                    strncpy(&Dst[DstLen],To,ToLen);
                    DstLen+=ToLen;
                }

                Src=Pos+FromLen;
                SrcLen-=SegLen+FromLen;
                Pos=strstr(Src,From);
            }
            strncpy(&Dst[DstLen],Src,SrcLen);               //��ʣ�ಿ�ֿ�����Ŀ��
            DstLen+=SrcLen;
            Dst[DstLen++]=0;
            return Dst;
        }
        template<class CT>
        inline CT* replace(const CT *SrcStr,const CT* From,const CT* To,CT* Dst,uint32_t DstSize,uint32_t SrcLen=0)
        {
            const CT* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            uint32_t FromLen=strlen(From);
            uint32_t ToLen=is_empty(To)?0:strlen(To);
            return replace(SrcStr,From,FromLen,To,ToLen,Dst,DstSize,SrcLen);
        }
        //-------------------------------------------------
        //�ж��Ƿ�Ϊʮ���������ַ�
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
        //�ж��Ƿ�Ϊʮ�������ֺ���ĸ
        inline bool             isalnum(char c) { return (c >= '0'&&c <= '9') || (c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z'); }
        inline bool             isalnum(wchar_t c) { return (c >= L'0'&&c <= L'9') || (c >= L'A'&&c <= L'Z') || (c >= L'a'&&c <= L'z'); }
        //-------------------------------------------------
        //�жϸ������ַ����Ƿ�ȫ����Ϊ����(ʮ���ƻ�ʮ����������)
        template<class CT>
        inline bool isnumber(const CT* Str,uint32_t StrLen=0,bool IsHex=false)
        {
            if (is_empty(Str))
                return false;
            if (!StrLen)
                StrLen=strlen(Str);
            CT C;

            for(uint32_t I=0; I<StrLen; I++)                //��ָ�����ȵĴ�����ȫ����������������
            {
                C=Str[I];
                if (C>=sc<CT>::zero()&&C<='9')              //��ʮ��������,ֱ��׼���ж���һ��
                    continue;

                if (!IsHex)
                    return false;                           //����ʮ������ģʽ,�����ֵĴ����־ͷ��ؼ�

                if ((C>=sc<CT>::A()&&C<=sc<CT>::F())||(C>=sc<CT>::a()&&C<=sc<CT>::f()))
                    continue;                               //ʮ������ʱ,��ǰ�ַ��ǺϷ���,����,׼���ж���һ��.
                else
                    return false;                           //����˵����ǰ���ǺϷ���ʮ���������ִ�
            }
            return true;                                    //ȫ�����ַ�����������,˵����ǰ���ǺϷ������ִ�.
        }
    }
}


#endif
