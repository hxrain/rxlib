#ifndef _RX_STR_UTIL_FMT_H_
#define _RX_STR_UTIL_FMT_H_
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <float.h>

#include "rx_str_util_std.h"

namespace rx
{
    namespace fmt_imp
    {
        /*
            ����ο� reference https://github.com/mpaland/printf

            �����ĸ�ʽ��ָʾ�ַ����ṹ: %[flags][width][.precision][length]type
            type:����ĸ�ʽ����
                d or i  : ���������������
                u 	    : ����޷���������
                b 	    : �������������
                o 	    : ����˽�������
                x 	    : ���Сд��ʮ����������
                X 	    : �����д��ʮ����������
                f or F 	: �����������(����������������޶�ֵ��Χ,Ҳ����e��E��ʽ���)
                e or E 	: �����ѧ��������������ʽ��
                g or G 	: �����ѧ��������ʽ����ʮ���Ƹ�������,���ֶ��������.
                c 	    : ��������ַ�
                s 	    : ����ַ���
                p 	    : ���ָ���ַ
                % 	    : �������%,���������ַ���ת��
            flags:��ʽ������ķ�����ǰ׺����
                - 	    : ��ָ����ȷ�Χ�������.(Ĭ��Ϊ�Ҷ���)
                + 	    : ǿ����������ֵ���������+��-.(Ĭ��ֻ�и��������-)
                �ո� 	: ǿ����������ֵ��������ӿո�,���û��-��ʱ��.
                # 	    : Ҫ��������ַ�,������Ϊo, b, x or X ��ʱ��,���ַ�Ϊ0, 0b, 0x or 0X.��ֵ0��������ַ�;������Ϊf��F��ʱ��,ǿ��������ĵ�('.')
                0 	    : �����ʱʹ��0�������
            width:��������Ŀ��
                ����     : ָ����С��ȷ�Χ,ָ����������
                *       : ��Ȳ��ڸ�ʽ����������,�����ں��������ݲ�����ǰ��������.
            precision:������ֵľ���
                .����    : �����������(d��i��o��u��x��X),��֪��С���λ��(��Ҫʱ��ǰ�������)������Ϊ0��ֵҲΪ0��ʱ��,������κ��ַ���
                        : �����������(e��E ��f��F),��֪��С����������С��λ����
                        : �����������(g��G),��֪Ҫ����������Чλ����
                        : �����������(s),����Ҫ���������ַ���
                        : ��ĺ���û�����ֵ�ʱ��,���趨����Ϊ0��
                .*      : ���Ȳ��ڸ�ʽ����������,�����ں��������ݲ�����ǰ��������.
            length:���ݳ��Ȼ�����ָʾ��,��Ӧ�������(d��i)��(u��o��x��X)
                Ĭ��  	: int  �� unsigned int
                hh 	    : int8_t �� uint8_t
                h 	    : int16_t �� uint16_t
                l 	    : int32_t �� uin32_t
                ll 	    : int64_t �� uint64_t
                j 	    : intmax_t �� uintmax_t
                z 	    : size_t
                t 	    : ptrdiff_t
        */

        // 'ntoa' ʹ�õ���ʱ�������ߴ�,�����㹻��,��������0.(��ջ����ʱ����)
        #ifndef PRINTF_NTOA_BUFFER_SIZE
            #define PRINTF_NTOA_BUFFER_SIZE    32U
        #endif

        // 'ftoa' ʹ�õ���ʱ�������ߴ�,�����㹻��,��������0.(��ջ����ʱ����)
        #ifndef PRINTF_FTOA_BUFFER_SIZE
            #define PRINTF_FTOA_BUFFER_SIZE    32U
        #endif

        // Ĭ�ϵĸ������������
        #ifndef PRINTF_DEFAULT_FLOAT_PRECISION
            #define PRINTF_DEFAULT_FLOAT_PRECISION  6U
        #endif

        // ������%f���ʱ������ٽ��,����ֵʱ����%e��ʽ���
        #ifndef PRINTF_MAX_FLOAT
            #define PRINTF_MAX_FLOAT            1e9
        #endif

        //---------------------------------------------------------------------
        // �ڲ�ʹ�õĹ������
        const uint32_t FLAGS_ZEROPAD   =(1U <<  0U);        //�Ƿ����0���
        const uint32_t FLAGS_LEFT      =(1U <<  1U);        //�Ƿ������������������
        const uint32_t FLAGS_PLUS      =(1U <<  2U);        //�Ƿ�ǿ���������ǰ�ļӺ�
        const uint32_t FLAGS_SPACE     =(1U <<  3U);        //�Ƿ�ǿ���������ǰ�Ŀո�
        const uint32_t FLAGS_CROWN     =(1U <<  4U);        //�Ƿ��������ǰ�Ĺ��ַ�,OX,0x,0b��
        const uint32_t FLAGS_UPPERCASE =(1U <<  5U);        //������Ƿ�ʹ�ô�д��ĸ
        const uint32_t FLAGS_CHAR      =(1U <<  6U);        //����ַ�
        const uint32_t FLAGS_SHORT     =(1U <<  7U);        //���short����
        const uint32_t FLAGS_LONG      =(1U <<  8U);        //���long����
        const uint32_t FLAGS_LONG_LONG =(1U <<  9U);        //���long long����
        const uint32_t FLAGS_PRECISION =(1U << 10U);        //�Ƿ���ж���ľ���ָʾ
        const uint32_t FLAGS_ADAPT_EXP =(1U << 11U);        //�Ƿ�����Ӧ��ѧ�������򸡵��ʽ

        //---------------------------------------------------------------------
        //�ײ��ַ������,null,ɶҲ�����
        template<class CT>
        class fmt_follower_null
        {
        public:
            typename CT char_t;
            CT* buffer;
            size_t idx;
            size_t maxlen;
            void bind(CT* buff,size_t maxl=(size_t)-1){buffer=buff;idx=0;maxlen=maxl;}
            void operator ()(CT character){}
        };

        //�ײ��ַ������,char,���ַ������stdout
        template<class CT>
        class fmt_follower_char:public fmt_follower_null<CT>
        {
        public:
            void operator ()(CT character)
            {
                if (character)
                    putchar(character);
            }
        };

        //�ײ��ַ������,buff,��¼�ַ���������
        template<class CT>
        class fmt_follower_buff:public fmt_follower_null<CT>
        {
        public:
            void operator ()(CT character)
            {
                if (idx < maxlen)
                    buffer[idx++] = character;
            }
        };

        //---------------------------------------------------------------------
        //�����ַ�������,������󳤶��޶�����.
        template<class CT>
        static inline unsigned int _strnlen_s(const CT* str, size_t maxsize)
        {
            const CT* s;
            for (s = str; *s && maxsize--; ++s);
            return (unsigned int)(s - str);
        }


        //�ж��ַ��Ƿ�Ϊ����(0-9)
        template<class CT>
        static inline bool _is_digit(CT ch)
        {
            return sc<CT>::is_0to9(ch);
        }


        //��ת���ַ����е�0~9�ַ���ɵ���ֵ,���ڴ�����ָʾ�뾫��ָʾ
        template<class CT>
        inline unsigned int _atou(const CT** str)
        {
            unsigned int i = 0U;
            while (_is_digit(**str))
            {
                i = i * 10U + (unsigned int)(*((*str)++) - sc<CT>::zero());
            }
            return i;
        }


        // ��������ַ���,�����������.
        template<class OT>
        inline size_t _out_rev(OT& out,const typename OT::char_t* buf, size_t len, unsigned int width, unsigned int flags)
        {
            const size_t start_idx = out.idx;

            if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD))
            {//���������,Ҳ����0���,���԰�ָ���������󲿵Ŀո�
                for (size_t i = len; i < width; i++)
                    out(' ');
            }

            //�����������
            while (len)
                out(buf[--len]);

            if (flags & FLAGS_LEFT)
            {//����������,����Ҫ���ʣ�ಿ��Ϊ�ո�
                while (out.idx - start_idx < width)
                    out(' ');
            }

            return out.idx;
        }

        //ntoa��ʽ������
        template<class OT>
        inline size_t _ntoa_format(OT& out,const typename OT::char_t* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags)
        {
            if (!(flags & FLAGS_LEFT))
            {//���������,���Խ���0���
                if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
                {
                    width--;
                }
                while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE))
                {
                    buf[len++] = '0';
                }
                while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE))
                {
                    buf[len++] = '0';
                }
            }

            if (flags & FLAGS_CROWN)
            {//Ҫ���������ĸ
                if (!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width)))
                {
                    len--;
                    if (len && (base == 16U))
                        len--;
                }
                if ((base == 16U) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE))
                    buf[len++] = 'x';
                else if ((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE))
                    buf[len++] = 'X';
                else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE))
                    buf[len++] = 'b';
                if (len < PRINTF_NTOA_BUFFER_SIZE)
                    buf[len++] = '0';
            }

            if (len < PRINTF_NTOA_BUFFER_SIZE)
            {//�����������������
                if (negative)
                    buf[len++] = '-';
                else if (flags & FLAGS_PLUS)
                    buf[len++] = '+';  // ignore the space if the '+' exists
                else if (flags & FLAGS_SPACE)
                    buf[len++] = ' ';
            }
            return _out_rev(out, buf, len, width, flags);
        }

        //---------------------------------------------------------------------
        //ntoa ת������,���Դ���longֵҲ������long longֵ
        template<class OT,class LT>
        inline size_t _ntoa(OT& out, LT value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags)
        {
            OT::char_t buf[PRINTF_NTOA_BUFFER_SIZE];
            size_t len = 0U;
            rx_assert(base<=16);

            //����0��Ӧ���й���
            if (!value)
                flags &= ~FLAGS_CROWN;

            // write if precision != 0 and value is != 0
            if (!(flags & FLAGS_PRECISION) || value)
            {//δָ�����Ȼ�ֵ��Ϊ��,����л���ѭ��ȡ��õ����ֶ�Ӧ���ַ�
                const OT::char_t *hex = flags & FLAGS_UPPERCASE ? sc<OT::char_t>::hex_upr() : sc<OT::char_t>::hex_lwr();
                do
                {
                    const int digit = (value % base);
                    buf[len++] = hex[digit];
                    value /= base;
                }
                while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
            }

            return _ntoa_format(out, buf, len, negative, (unsigned int)base, prec, width, flags);
        }

        //---------------------------------------------------------------------
        //�������Ƿ�����ֵ.����ֵ:0����;��������ֵ�Ƿ�,��֪������ƫ����
        template<class OT>
        inline size_t _chk_nan(OT& out, double value, unsigned int width, unsigned int flags)
        {
            // test for special values
            if (value != value)
                return _out_rev(out, "nan", 3, width, flags);
            if (value < -DBL_MAX)
                return _out_rev(out, "fni-", 4, width, flags);
            if (value > DBL_MAX)
            {
                size_t len = (flags & FLAGS_PLUS) ? 4U : 3U;
                return _out_rev(out, (flags & FLAGS_PLUS) ? "fni+" : "fni", len, width, flags);
            }
            return 0;
        }

        //Ԥ������
        template<class OT>
        inline size_t _ftoa(OT& out, double value, unsigned int prec, unsigned int width, unsigned int flags);
        //---------------------------------------------------------------------
        //��������ʽ��Ϊ��ѧ���������
        template<class OT>
        inline size_t _etoa(OT& out, double value, unsigned int prec, unsigned int width, unsigned int flags)
        {
            //�ж�ֵ�Ƿ�Ƿ�
            size_t len=_chk_nan(out,value,width,flags);
            if (len) return len;

            //�ж��Ƿ�Ϊ����,���з�ת����
            const bool negative = value < 0;
            if (negative)
                value = -value;

            //�ж��Ƿ�ʹ��Ĭ�Ͼ���
            if (!(flags & FLAGS_PRECISION))
                prec = PRINTF_DEFAULT_FLOAT_PRECISION;

            // determine the decimal exponent
            // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
            union
            {
                uint64_t U;
                double   F;
            } conv;

            conv.F = value;
            int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;           // effectively log2
            conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U);  // drop the exponent so conv.F is now in [1,2)
            // now approximate log10 from the log2 integer part and an expansion of ln around 1.5
            int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
            // now we want to compute 10^expval but we want to be sure it won't overflow
            exp2 = (int)(expval * 3.321928094887362 + 0.5);
            const double z  = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
            const double z2 = z * z;
            conv.U = (uint64_t)(exp2 + 1023) << 52U;
            // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
            conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
            // correct for rounding errors
            if (value < conv.F)
            {
                expval--;
                conv.F /= 10;
            }

            // the exponent format is "%+03d" and largest value is "307", so set aside 4-5 characters
            unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

            // in "%g" mode, "prec" is the number of *significant figures* not decimals
            if (flags & FLAGS_ADAPT_EXP)
            {
                // do we want to fall-back to "%f" mode?
                if ((value >= 1e-4) && (value < 1e6))
                {
                    if ((int)prec > expval)
                        prec = (unsigned)((int)prec - expval - 1);
                    else
                        prec = 0;

                    flags |= FLAGS_PRECISION;   // make sure _ftoa respects precision
                    // no characters in exponent
                    minwidth = 0U;
                    expval   = 0;
                }
                else
                {
                    // we use one sigfig for the whole part
                    if ((prec > 0) && (flags & FLAGS_PRECISION))
                        --prec;
                }
            }

            // will everything fit?
            unsigned int fwidth = width;
            if (width > minwidth)
            {
                // we didn't fall-back so subtract the characters required for the exponent
                fwidth -= minwidth;
            }
            else
            {
                // not enough characters, so go back to default sizing
                fwidth = 0U;
            }

            // if we're padding on the right, DON'T pad the floating part
            if ((flags & FLAGS_LEFT) && minwidth)
                fwidth = 0U;

            // rescale the float value
            if (expval)
                value /= conv.F;

            // output the floating part
            const size_t start_idx = out.idx;
            _ftoa(out, negative ? -value : value, prec, fwidth, flags & ~FLAGS_ADAPT_EXP);

            // output the exponent part
            if (minwidth)
            {
                // output the exponential symbol
                out((flags & FLAGS_UPPERCASE) ? 'E' : 'e');
                // output the exponent value
                negative = expval < 0;
                _ntoa(out, (negative ? -expval : expval), negative, 10, 0, minwidth-1, FLAGS_ZEROPAD | FLAGS_PLUS);
                // might need to right-pad spaces
                if (flags & FLAGS_LEFT)
                {
                    while (out.idx - start_idx < width)
                        out(' ');
                }
            }
            return out.idx;
        }
        //---------------------------------------------------------------------
        //��ʽ�����������
        template<class OT>
        inline size_t _ftoa(OT& out, double value, unsigned int prec, unsigned int width, unsigned int flags)
        {
            //�ж�ֵ�Ƿ�Ƿ�
            size_t len=_chk_nan(out,value,width,flags);
            if (len) return len;
            typedef typename OT::char_t char_t;
            char_t buf[PRINTF_FTOA_BUFFER_SIZE];
            double diff = 0.0;

            // powers of 10
            static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

            if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT))
            {//�����������޶�ֵ,ֱ�ӷ��ؿ�ѧ����������ʽ
                return _etoa(out, buffer, idx, maxlen, value, prec, width, flags);
            }

            //���и����ж�����ŵ���
            bool negative = false;
            if (value < 0)
            {
                negative = true;
                value = 0 - value;
            }

            // set default precision, if not set explicitly
            if (!(flags & FLAGS_PRECISION))
            {//û�и��������������,��ʹ��Ĭ�Ͼ���
                prec = PRINTF_DEFAULT_FLOAT_PRECISION;
            }
            // limit precision to 9, cause a prec >= 10 can lead to overflow errors
            while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9U))
            {
                buf[len++] = '0';
                prec--;
            }

            int whole = (int)value;
            double tmp = (value - whole) * pow10[prec];
            unsigned long frac = (unsigned long)tmp;
            diff = tmp - frac;

            if (diff > 0.5)
            {
                ++frac;
                // handle rollover, e.g. case 0.99 with prec 1 is 1.0
                if (frac >= pow10[prec])
                {
                    frac = 0;
                    ++whole;
                }
            }
            else if (diff < 0.5)
            {
            }
            else if ((frac == 0U) || (frac & 1U))
            {
                // if halfway, round up if odd OR if last digit is 0
                ++frac;
            }

            if (prec == 0U)
            {
                diff = value - (double)whole;
                if ((!(diff < 0.5) || (diff > 0.5)) && (whole & 1))
                {
                    // exactly 0.5 and ODD, then round up
                    // 1.5 -> 2, but 2.5 -> 2
                    ++whole;
                }
            }
            else
            {
                unsigned int count = prec;
                // now do fractional part, as an unsigned number
                while (len < PRINTF_FTOA_BUFFER_SIZE)
                {
                    --count;
                    buf[len++] = (char_t)(48U + (frac % 10U));
                    if (!(frac /= 10U))
                    {
                        break;
                    }
                }
                // add extra 0s
                while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U))
                    buf[len++] = '0';

                if (len < PRINTF_FTOA_BUFFER_SIZE)
                    buf[len++] = '.';
            }

            // do whole part, number is reversed
            while (len < PRINTF_FTOA_BUFFER_SIZE)
            {
                buf[len++] = (char_t)(48 + (whole % 10));
                if (!(whole /= 10))
                    break;
            }

            // pad leading zeros
            if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD))
            {
                if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
                    width--;

                while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE))
                    buf[len++] = '0';
            }

            if (len < PRINTF_FTOA_BUFFER_SIZE)
            {
                if (negative)
                    buf[len++] = '-';
                else if (flags & FLAGS_PLUS)
                    buf[len++] = '+';  // ignore the space if the '+' exists
                else if (flags & FLAGS_SPACE)
                    buf[len++] = ' ';
            }

            return _out_rev(out, buf, len, width, flags);
        }

        //---------------------------------------------------------------------
        //�ڲ���ʽ������ĺ��ĺ���:outΪ�������;formatΪ��ʽ�����ݴ�;vaΪջ����ָʾ��
        template<class OT>
        inline int fmt_core(OT& out, const typename OT::char_t* format, va_list va)
        {
            typedef typename OT::char_t char_t;
            unsigned int flags, width, precision, n;

            while (*format)
            {//�Ը�ʽ�����������ַ�����
                if (*format != '%')
                {//��ǰ�ַ����������ʽ����ʼ���
                    out(*format);
                    format++;
                    continue;
                }
                else //������ʽ��ʼ�����,׼�����ж�����ദ��
                    format++;

                //�ȷ�����ʽ�����
                flags = 0U;
                do
                {
                    switch (*format)
                    {
                    case '0':
                        flags |= FLAGS_ZEROPAD;
                        format++;
                        n = 1U;
                        break;
                    case '-':
                        flags |= FLAGS_LEFT;
                        format++;
                        n = 1U;
                        break;
                    case '+':
                        flags |= FLAGS_PLUS;
                        format++;
                        n = 1U;
                        break;
                    case ' ':
                        flags |= FLAGS_SPACE;
                        format++;
                        n = 1U;
                        break;
                    case '#':
                        flags |= FLAGS_CROWN;
                        format++;
                        n = 1U;
                        break;
                    default :
                        n = 0U;
                        break;
                    }
                }
                while (n);

                //������ȡ��ǰ��ʽ���������ָʾ
                width = 0U;
                if (_is_digit(*format))
                    width = _atou(&format);
                else if (*format == '*')
                {//���ָʾ��Ҫ�Ӳ����л�ȡ
                    width = (unsigned int)va_arg(va, int);
                    if (width < 0)
                    {//���ָʾС��0,�����������
                        flags |= FLAGS_LEFT;    // reverse padding
                        width = (unsigned int)-width;
                    }
                    format++;
                }

                //������ȡ����
                precision = 0U;
                if (*format == '.')
                {
                    flags |= FLAGS_PRECISION;
                    format++;
                    if (_is_digit(*format))
                        precision = _atou(&format);
                    else if (*format == '*')
                    {//�����ݲ����л�ȡ����,Ҫ�󾫶ȱ������0,���򾫶�Ϊ0
                        const int prec = (int)va_arg(va, int);
                        precision = prec > 0 ? (unsigned int)prec : 0U;
                        format++;
                    }
                }

                //�������ݳ���ָʾ��
                switch (*format)
                {
                case 'l' :
                    flags |= FLAGS_LONG;
                    format++;
                    if (*format == 'l')
                    {
                        flags |= FLAGS_LONG_LONG;
                        format++;
                    }
                    break;
                case 'h' :
                    flags |= FLAGS_SHORT;
                    format++;
                    if (*format == 'h')
                    {
                        flags |= FLAGS_CHAR;
                        format++;
                    }
                    break;
                case 't' :
                    flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                case 'j' :
                    flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                case 'z' :
                    flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                default :
                    break;
                }

                //��ʼ����������ͽ��з��ദ��
                switch (*format)
                {
                case 'd' :
                case 'i' :
                case 'u' :
                case 'x' :
                case 'X' :
                case 'o' :
                case 'b' :
                    {
                        //������������趨���ֵĻ���
                        unsigned int base;
                        if (*format == 'x' || *format == 'X')
                            base = 16U;
                        else if (*format == 'o')
                            base =  8U;
                        else if (*format == 'b')
                            base =  2U;
                        else
                        {
                            base = 10U;
                            flags &= ~FLAGS_CROWN;          //ʮ�������������û�й���
                        }
                        
                        if (*format == 'X')                 //ʮ���������Ҫ����ĸ��д
                            flags |= FLAGS_UPPERCASE;

                        // no plus or space flag for u, x, X, o, b
                        if ((*format != 'i') && (*format != 'd'))
                        {//��ʮ���������,���ܾ��мӼ�������ǰ׺ռλ�ո�
                            flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
                        }

                        // ignore '0' flag when precision is given
                        if (flags & FLAGS_PRECISION)
                            flags &= ~FLAGS_ZEROPAD;

                        // convert the integer
                        if ((*format == 'i') || (*format == 'd'))
                        {//���������ʮ������
                            if (flags & FLAGS_LONG_LONG)
                            {
                                const long long value = va_arg(va, long long);
                                idx = _ntoa(out, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                            else if (flags & FLAGS_LONG)
                            {
                                const long value = va_arg(va, long);
                                idx = _ntoa(out, (unsigned long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                            else
                            {
                                const int value = (flags & FLAGS_CHAR) ? (int8_t)va_arg(va, int) : (flags & FLAGS_SHORT) ? (short int)va_arg(va, int) : va_arg(va, int);
                                idx = _ntoa(out, (unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                        }
                        else
                        {//����޷���ʮ������
                            if (flags & FLAGS_LONG_LONG)
                            {
                                idx = _ntoa(out, va_arg(va, unsigned long long), false, base, precision, width, flags);
                            }
                            else if (flags & FLAGS_LONG)
                            {
                                idx = _ntoa(out, va_arg(va, unsigned long), false, base, precision, width, flags);
                            }
                            else
                            {
                                const unsigned int value = (flags & FLAGS_CHAR) ? (uint8_t)va_arg(va, unsigned int) : (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(va, unsigned int) : va_arg(va, unsigned int);
                                idx = _ntoa(out, value, false, base, precision, width, flags);
                            }
                        }
                        format++;
                        break;
                    }
                case 'f' :
                case 'F' :
                    if (*format == 'F')
                        flags |= FLAGS_UPPERCASE;
                    idx = _ftoa(out, va_arg(va, double), precision, width, flags);
                    format++;
                    break;
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                    if ((*format == 'g')||(*format == 'G'))
                        flags |= FLAGS_ADAPT_EXP;
                    if ((*format == 'E')||(*format == 'G'))
                        flags |= FLAGS_UPPERCASE;
                    idx = _etoa(out, va_arg(va, double), precision, width, flags);
                    format++;
                    break;
                case 'c' :
                    {
                        unsigned int l = 1U;
                        // pre padding
                        if (!(flags & FLAGS_LEFT))
                        {
                            while (l++ < width)
                                out(' ');
                        }
                        // char output
                        out((char_t)va_arg(va, int));
                        // post padding
                        if (flags & FLAGS_LEFT)
                        {
                            while (l++ < width)
                                out(' ');
                        }
                        format++;
                        break;
                    }
                case 's' :
                    {
                        const char_t* p = va_arg(va, char_t*);
                        unsigned int l = _strnlen_s(p, precision ? precision : (size_t)-1);
                        // pre padding
                        if (flags & FLAGS_PRECISION)
                            l = (l < precision ? l : precision);
                        if (!(flags & FLAGS_LEFT))
                        {
                            while (l++ < width)
                                out(' ');
                        }
                        // string output
                        while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--))
                            out(*(p++));

                        // post padding
                        if (flags & FLAGS_LEFT)
                        {
                            while (l++ < width)
                                out(' ');
                        }
                        format++;
                        break;
                    }

                case 'p' :
                    {
                        width = sizeof(void*) * 2U;
                        flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
                        const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
                        if (is_ll)
                            idx = _ntoa(out, (uintptr_t)va_arg(va, void*), false, 16U, precision, width, flags);
                        else
                            idx = _ntoa(out, (unsigned long)((uintptr_t)va_arg(va, void*)), false, 16U, precision, width, flags);
                        format++;
                        break;
                    }

                case '%' :
                    out('%');
                    format++;
                    break;

                default :
                    out(*format);
                    format++;
                    break;
                }
            }

            // termination
            if (out.idx>=out.maxlen)
                out.idx=out.maxlen-1;
            out(0);

            // return written chars without terminating \0
            return out.idx;
        }

        //---------------------------------------------------------------------
        template<class CT>
        inline int vsnprintf(CT* buffer, size_t count, const CT* format, va_list va)
        {
            if (buffer==NULL)
            {
                fmt_follower_null<CT> out;
                return fmt_core(out, format, va);
            }
            else
            {
                fmt_follower_buff<CT> out;
                out.bind(buffer,count);
                return fmt_core(out, format, va);
            }
        }

        template<class CT>
        inline int sprintf(CT* buffer, const CT* format, ...)
        {
            va_list va;
            va_start(va, format);
            int ret=vsnprintf(buffer,-1,format,va);
            va_end(va);
            return ret;
        }

        template<class CT>
        inline int snprintf(CT* buffer, size_t count, const CT* format, ...)
        {
            va_list va;
            va_start(va, format);
            int ret=vsnprintf(buffer,count,format,va);
            va_end(va);
            return ret;
        }

        template<class CT>
        inline int vprintf(const CT* format, va_list va)
        {
            fmt_follower_char<CT> out;
            return fmt_core(out, format, va);
        }

        template<class CT>
        inline int printf(const CT* format, ...)
        {
            va_list va;
            va_start(va, format);
            const int ret = vprintf(format, va);
            va_end(va);
            return ret;
        }
        //---------------------------------------------------------------------
    }
}

#endif