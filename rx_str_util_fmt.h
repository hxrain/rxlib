#ifndef _RX_STR_UTIL_FMT_H_
#define _RX_STR_UTIL_FMT_H_
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <float.h>

#include "rx_str_util_std.h"

#if (RX_OS==RX_OS_WIN||RX_OS_POSIX)&&!defined(RX_STR_USE_FILE)
    #define RX_STR_USE_FILE 1
#endif

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
                # 	    : Ҫ��������ַ�,������Ϊo, b, x or X ��ʱ��,���ַ�Ϊ0, 0b, 0x or 0X.��ֵ0��������ַ�;������Ϊf��F��ʱ��,ǿ��������ĵ�(sc<char_t>::dot)
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
            #define PRINTF_MAX_FLOAT            1e12
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
            typedef CT char_t;
            CT* buffer;
            size_t idx;
            size_t count;
            size_t maxlen;
            void bind(CT* buff,size_t maxl=(size_t)-1){buffer=buff;idx=0;count=0;maxlen=maxl;}
            void operator ()(CT character){++count;}
            void end(){}
        };

        //�ײ��ַ������,char,���ַ������stdout
        template<class CT>
        class fmt_follower_char:public fmt_follower_null<CT>
        {
            typedef fmt_follower_null<CT> super_t;
        public:
            void operator ()(CT character)
            {
                putchar(character);
                ++super_t::count;
            }
            void end(){}
        };

        //�ײ��ַ������,buff,��¼�ַ���������
        template<class CT>
        class fmt_follower_buff:public fmt_follower_null<CT>
        {
            typedef fmt_follower_null<CT> super_t;
        public:
            void operator ()(CT character)
            {
                if (super_t::idx < super_t::maxlen)
                    super_t::buffer[super_t::idx++] = character;
                ++super_t::count;
            }
            void end()
            {
                size_t last=super_t::maxlen-1;
                if (super_t::idx < last)
                    super_t::buffer[super_t::idx++] = 0;
                else
                    super_t::buffer[last] = 0;
            }
        };

#if RX_STR_USE_FILE
        //�ײ��ַ������,buff,��¼�ַ���������
        template<class CT>
        class fmt_follower_file:public fmt_follower_null<CT>
        {
            FILE *m_fp;
            typedef fmt_follower_null<CT> super_t;
        public:
            fmt_follower_file(FILE *fp):m_fp(fp){}
            void operator ()(CT character)
            {
                if (sizeof(CT)==sizeof(char))
                    fputc(character,m_fp);
                else
                    fputwc(character,m_fp);
                ++super_t::count;
            }
            void end(){}
        };
#endif
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
                i = i * 10U + (unsigned int)(*((*str)++) - sc<CT>::zero);
            }
            return i;
        }


        // ��������ַ���,�����������.
        template<class OT>
        inline size_t _out_rev(OT& out,const typename OT::char_t* buf, size_t len, unsigned int width, unsigned int flags)
        {
            typedef typename OT::char_t char_t;
            const size_t start_idx = out.idx;

            if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD))
            {//���������,Ҳ����0���,���԰�ָ���������󲿵Ŀո�
                for (size_t i = len; i < width; i++)
                    out(sc<char_t>::space);
            }

            //�����������
            while (len)
                out(buf[--len]);

            if (flags & FLAGS_LEFT)
            {//����������,����Ҫ���ʣ�ಿ��Ϊ�ո�
                while (out.idx - start_idx < width)
                    out(sc<char_t>::space);
            }

            return out.idx;
        }

        //ntoa��ʽ������
        template<class OT>
        inline size_t _ntoa_format(OT& out,typename OT::char_t* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags)
        {
            typedef typename OT::char_t char_t;
            if (!(flags & FLAGS_LEFT))
            {//���������,���Խ���0���
                if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
                {
                    width--;
                }
                while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE))
                {
                    buf[len++] = sc<char_t>::zero;
                }
                while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE))
                {
                    buf[len++] = sc<char_t>::zero;
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
                    buf[len++] = sc<char_t>::x;
                else if ((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE))
                    buf[len++] = sc<char_t>::X;
                else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE))
                    buf[len++] = sc<char_t>::b;
                if (len < PRINTF_NTOA_BUFFER_SIZE)
                    buf[len++] = sc<char_t>::zero;
            }

            if (len < PRINTF_NTOA_BUFFER_SIZE)
            {//�����������������
                if (negative)
                    buf[len++] = sc<char_t>::minus;
                else if (flags & FLAGS_PLUS)
                    buf[len++] = sc<char_t>::plus;  // ignore the space if the sc<char_t>::plus exists
                else if (flags & FLAGS_SPACE)
                    buf[len++] = sc<char_t>::space;
            }
            return _out_rev(out, buf, len, width, flags);
        }

        //---------------------------------------------------------------------
        //ntoa ת������,���Դ���longֵҲ������long longֵ
        template<class OT,class LT>
        inline size_t _ntoa(OT& out, LT value, bool negative, unsigned long base=10, unsigned int prec=0, unsigned int width=0, unsigned int flags=0)
        {
            typedef typename OT::char_t char_t;
            char_t buf[PRINTF_NTOA_BUFFER_SIZE];
            size_t len = 0U;
            rx_assert(base<=16);

            //����0��Ӧ���й���
            if (!value)
                flags &= ~FLAGS_CROWN;

            // write if precision != 0 and value is != 0
            if (!(flags & FLAGS_PRECISION) || value)
            {//δָ�����Ȼ�ֵ��Ϊ��,����л���ѭ��ȡ��õ����ֶ�Ӧ���ַ�
                const char_t *hex = flags & FLAGS_UPPERCASE ? sc<char_t>::hex_upr() : sc<char_t>::hex_lwr();
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
            typedef typename OT::char_t char_t;
            // test for special values
            if (value != value)
                return _out_rev(out, sc<char_t>::nan(), 3, width, flags);
            if (value < -DBL_MAX)
                return _out_rev(out, sc<char_t>::fni_minus(), 4, width, flags);
            if (value > DBL_MAX)
            {
                size_t len = (flags & FLAGS_PLUS) ? 4U : 3U;
                return _out_rev(out, (flags & FLAGS_PLUS) ? sc<char_t>::fni_plus() : sc<char_t>::fni(), len, width, flags);
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
            typedef typename OT::char_t char_t;
            //�ж�ֵ�Ƿ�Ƿ�
            size_t len=_chk_nan(out,value,width,flags);
            if (len) return len;

            //�ж��Ƿ�Ϊ����,���з�ת����
            bool negative = value < 0;
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
                out((flags & FLAGS_UPPERCASE) ? sc<char_t>::E : sc<char_t>::e);
                // output the exponent value
                negative = expval < 0;
                _ntoa(out, (negative ? -expval : expval), negative, 10, 0, minwidth-1, FLAGS_ZEROPAD | FLAGS_PLUS);
                // might need to right-pad spaces
                if (flags & FLAGS_LEFT)
                {
                    while (out.idx - start_idx < width)
                        out(sc<char_t>::space);
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
                return _etoa(out, value, prec, width, flags);
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
                buf[len++] = sc<char_t>::zero;
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
                    buf[len++] = sc<char_t>::zero;

                if (len < PRINTF_FTOA_BUFFER_SIZE)
                    buf[len++] = sc<char_t>::dot;
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
                    buf[len++] = sc<char_t>::zero;
            }

            if (len < PRINTF_FTOA_BUFFER_SIZE)
            {
                if (negative)
                    buf[len++] = sc<char_t>::minus;
                else if (flags & FLAGS_PLUS)
                    buf[len++] = sc<char_t>::plus;  // ignore the space if the sc<char_t>::plus exists
                else if (flags & FLAGS_SPACE)
                    buf[len++] = sc<char_t>::space;
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
                if (*format != sc<char_t>::percent)
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
                    case sc<char_t>::zero:
                        flags |= FLAGS_ZEROPAD;
                        format++;
                        n = 1U;
                        break;
                    case sc<char_t>::minus:
                        flags |= FLAGS_LEFT;
                        format++;
                        n = 1U;
                        break;
                    case sc<char_t>::plus:
                        flags |= FLAGS_PLUS;
                        format++;
                        n = 1U;
                        break;
                    case sc<char_t>::space:
                        flags |= FLAGS_SPACE;
                        format++;
                        n = 1U;
                        break;
                    case sc<char_t>::sharp:
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
                else if (*format == sc<char_t>::star)
                {//���ָʾ��Ҫ�Ӳ����л�ȡ
                    width = (unsigned int)va_arg(va, int);
                    if ((int)width < 0)
                    {//���ָʾС��0,�����������
                        flags |= FLAGS_LEFT;    // reverse padding
                        width = (unsigned int)-(int)width;
                    }
                    format++;
                }

                //������ȡ����
                precision = 0U;
                if (*format == sc<char_t>::dot)
                {
                    flags |= FLAGS_PRECISION;
                    format++;
                    if (_is_digit(*format))
                        precision = _atou(&format);
                    else if (*format == sc<char_t>::star)
                    {//�����ݲ����л�ȡ����,Ҫ�󾫶ȱ������0,���򾫶�Ϊ0
                        const int prec = (int)va_arg(va, int);
                        precision = prec > 0 ? (unsigned int)prec : 0U;
                        format++;
                    }
                }

                //�������ݳ���ָʾ��
                switch (*format)
                {
                case sc<char_t>::l :
                    flags |= FLAGS_LONG;
                    format++;
                    if (*format == sc<char_t>::l)
                    {
                        flags |= FLAGS_LONG_LONG;
                        format++;
                    }
                    break;
                case sc<char_t>::h :
                    flags |= FLAGS_SHORT;
                    format++;
                    if (*format == sc<char_t>::h)
                    {
                        flags |= FLAGS_CHAR;
                        format++;
                    }
                    break;
                case sc<char_t>::t :
                    flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                case sc<char_t>::j :
                    flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                case sc<char_t>::z :
                    flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                    format++;
                    break;
                default :
                    break;
                }

                //��ʼ����������ͽ��з��ദ��
                switch (*format)
                {
                case sc<char_t>::d :
                case sc<char_t>::i :
                case sc<char_t>::u :
                case sc<char_t>::x :
                case sc<char_t>::X :
                case sc<char_t>::o :
                case sc<char_t>::b :
                    {
                        //������������趨���ֵĻ���
                        unsigned int base;
                        switch (*format)
                        {
                        case sc<char_t>::x:
                            base = 16U;
                            break;
                        case sc<char_t>::X:
                            base = 16U;
                            flags |= FLAGS_UPPERCASE;       //ʮ���������Ҫ����ĸ��д
                            break;
                        case sc<char_t>::o:
                            base = 8U;
                            break;
                        case sc<char_t>::b:
                            base = 2U;
                            break;
                        default:
                            base = 10U;
                            flags &= ~FLAGS_CROWN;          //ʮ�������������û�й���
                        }

                        // no plus or space flag for u, x, X, o, b
                        if ((*format != sc<char_t>::i) && (*format != sc<char_t>::d))
                        {//��ʮ���������,���ܾ��мӼ�������ǰ׺ռλ�ո�
                            flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
                        }

                        // ignore sc<char_t>::zero flag when precision is given
                        if (flags & FLAGS_PRECISION)
                            flags &= ~FLAGS_ZEROPAD;

                        // convert the integer
                        if ((*format == sc<char_t>::i) || (*format == sc<char_t>::d))
                        {//���������ʮ������
                            if (flags & FLAGS_LONG_LONG)
                            {
                                const long long value = va_arg(va, long long);
                                _ntoa(out, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                            else if (flags & FLAGS_LONG)
                            {
                                const long value = va_arg(va, long);
                                _ntoa(out, (unsigned long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                            else
                            {
                                const int value = (flags & FLAGS_CHAR) ? (int8_t)va_arg(va, int) : (flags & FLAGS_SHORT) ? (short int)va_arg(va, int) : va_arg(va, int);
                                _ntoa(out, (unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                            }
                        }
                        else
                        {//����޷���ʮ������
                            if (flags & FLAGS_LONG_LONG)
                            {
                                _ntoa(out, va_arg(va, unsigned long long), false, base, precision, width, flags);
                            }
                            else if (flags & FLAGS_LONG)
                            {
                                _ntoa(out, va_arg(va, unsigned long), false, base, precision, width, flags);
                            }
                            else
                            {
                                const unsigned int value = (flags & FLAGS_CHAR) ? (uint8_t)va_arg(va, unsigned int) : (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(va, unsigned int) : va_arg(va, unsigned int);
                                _ntoa(out, value, false, base, precision, width, flags);
                            }
                        }
                        format++;
                        break;
                    }
                case sc<char_t>::f :
                case sc<char_t>::F :
                    if (*format == sc<char_t>::F)
                        flags |= FLAGS_UPPERCASE;
                    _ftoa(out, va_arg(va, double), precision, width, flags);
                    format++;
                    break;
                case sc<char_t>::e:
                case sc<char_t>::E:
                case sc<char_t>::g:
                case sc<char_t>::G:
                    if ((*format == sc<char_t>::g)||(*format == sc<char_t>::G))
                        flags |= FLAGS_ADAPT_EXP;
                    if ((*format == sc<char_t>::E)||(*format == sc<char_t>::G))
                        flags |= FLAGS_UPPERCASE;
                    _etoa(out, va_arg(va, double), precision, width, flags);
                    format++;
                    break;
                case sc<char_t>::c :
                    {
                        unsigned int l = 1U;
                        // pre padding
                        if (!(flags & FLAGS_LEFT))
                        {
                            while (l++ < width)
                                out(sc<char_t>::space);
                        }
                        // char output
                        out((char_t)va_arg(va, int));
                        // post padding
                        if (flags & FLAGS_LEFT)
                        {
                            while (l++ < width)
                                out(sc<char_t>::space);
                        }
                        format++;
                        break;
                    }
                case sc<char_t>::s :
                    {
                        const char_t* p = va_arg(va, char_t*);
                        unsigned int l = _strnlen_s(p, precision ? precision : (size_t)-1);
                        // pre padding
                        if (flags & FLAGS_PRECISION)
                            l = (l < precision ? l : precision);
                        if (!(flags & FLAGS_LEFT))
                        {
                            while (l++ < width)
                                out(sc<char_t>::space);
                        }
                        // string output
                        while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--))
                            out(*(p++));

                        // post padding
                        if (flags & FLAGS_LEFT)
                        {
                            while (l++ < width)
                                out(sc<char_t>::space);
                        }
                        format++;
                        break;
                    }

                case sc<char_t>::p :
                    {
                        width = sizeof(void*) * 2U;
                        flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
                        const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
                        if (is_ll)
                            _ntoa(out, (uintptr_t)va_arg(va, void*), false, 16U, precision, width, flags);
                        else
                            _ntoa(out, (unsigned long)((uintptr_t)va_arg(va, void*)), false, 16U, precision, width, flags);
                        format++;
                        break;
                    }

                case sc<char_t>::percent :
                    out(sc<char_t>::percent);
                    format++;
                    break;

                default :
                    out(*format);
                    format++;
                    break;
                }
            }

            //���������
            out.end();
            //������������ճ���(���Ȳ�����������)
            return (int)out.count;
        }
    }
    //-------------------------------------------------------------------------
    namespace st
    {
        //---------------------------------------------------------------------
        //��װ����ת��Ϊ�ַ����ĺ���
        template<class CT,class VT>
        inline char *ftoa(VT value, CT *string,int prec=8,bool exp=false)
        {
            fmt_imp::fmt_follower_buff<CT> out;
            out.bind(string);
            if (exp)
                fmt_imp::_etoa(out,value,prec,0,fmt_imp::FLAGS_PRECISION);
            else
                fmt_imp::_ftoa(out,value,prec,0,fmt_imp::FLAGS_PRECISION);
            out(0);
            return string;
        }
        //---------------------------------------------------------------------
        //��װ����ת��Ϊ�ַ����ĺ���
        template<class CT,class VT>
        inline char *ntoa(VT value, CT *string, int radix=10,bool crown=false)
        {
            fmt_imp::fmt_follower_buff<CT> out;
            out.bind(string);
            if (radix==10)
            {
                bool negative = value<0;
                fmt_imp::_ntoa(out,(negative?0-value:value),negative,radix);
            }
            else
            {
                uint32_t flags=crown?fmt_imp::FLAGS_CROWN:0;
                if (sizeof(value)<=4)
                {
                    uint32_t val=(uint32_t)value;
                    fmt_imp::_ntoa(out,val,false,radix,0,0,flags);
                }
                else
                {
                    uint64_t val=(uint64_t)value;
                    fmt_imp::_ntoa(out,val,false,radix,0,0,flags);
                }
            }
            out(0);
            return string;
        }
        //---------------------------------------------------------------------
        //�﷨��,��Ȼ��ת��Ϊָ���������ִ�
        template<class CT>
        inline char* itoa(int32_t value,CT* string,int radix=10) {return ntoa(value,string,radix);}
        template<class CT>
        inline char* itoa64(int64_t value,CT* string,int radix=10) {return ntoa(value,string,radix);}
        //�﷨��,�޷�������ת��Ϊָ���������ִ�
        template<class CT>
        inline char* utoa(uint32_t value,CT* string,int radix=10) {return ntoa(value,string,radix);}
        template<class CT>
        inline char* utoa64(uint64_t value,CT* string,int radix=10) {return ntoa(value,string,radix);}
        //�﷨��,��Ȼ��ת��Ϊ���й���0x��ʮ���������ִ�
        template<class CT>
        inline char* itox(int32_t value,CT* string) {return ntoa(value,string,16,true);}
        template<class CT>
        inline char* itox64(int64_t value,CT* string) {return ntoa(value,string,16,true);}
        //�﷨��,�޷�������ת��Ϊ���й���0x��ʮ���������ִ�
        template<class CT>
        inline char* utox(uint32_t value,CT* string) {return ntoa(value,string,16,true);}
        template<class CT>
        inline char* utox64(uint64_t value,CT* string) {return ntoa(value,string,16,true);}

        //---------------------------------------------------------------------
        //ͨ�ø�ʽ���������,���������buffer������,���ݳ��Ȳ��ᳬ��count���ַ�
        //����ֵ:<0����;>=0Ϊ������ݳ���,��Ҫ��count���бȽ�
        template<class CT>
        inline int vsnprintf(CT* buffer, const size_t count, const CT* format, va_list va)
        {
            if (buffer==NULL||count==0)
            {
                fmt_imp::fmt_follower_null<CT> out;
                out.bind(NULL);
                return fmt_imp::fmt_core(out, format, va);
            }
            else
            {
                fmt_imp::fmt_follower_buff<CT> out;
                out.bind(buffer,count);
                return fmt_imp::fmt_core(out, format, va);
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

        //����ֵ:<0����;>=0Ϊ������ݳ���,��Ҫ��count���бȽ�
        template<class CT>
        inline int snprintf(CT* buffer,const size_t count, const CT* format, ...)
        {
            va_list va;
            va_start(va, format);
            int ret=vsnprintf(buffer,count,format,va);
            va_end(va);
            return ret;
        }

        template<class CT,class OT>
        inline int format(OT &out,const CT* format, va_list va)
        {
            return fmt_imp::fmt_core(out, format, va);
        }

        template<class CT>
        inline int vprintf(const CT* format, va_list va)
        {
            fmt_imp::fmt_follower_char<CT> out;
            return fmt_imp::fmt_core(out, format, va);
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

#if RX_STR_USE_FILE
        template<class CT>
        inline int vfprintf(FILE *stream, const CT *format,va_list ap)
        {
            fmt_imp::fmt_follower_file<CT> out(stream);
            return fmt_imp::fmt_core(out, format, ap);
        }

        template<class CT>
        inline int fprintf(FILE *stream, const CT *format,...)
        {
            va_list ap;
            va_start(ap, format);
            int ret = vfprintf(stream,format,ap);
            va_end(ap);
            return ret;
        }
#endif
    }
    //-------------------------------------------------------------------------
}

#endif
