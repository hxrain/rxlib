#ifndef _RX_CT_BITARR_H_
#define _RX_CT_BITARR_H_

#include "rx_cc_macro.h"

//---------------------------------------------------------
//对变量b进行值v操作
#define bits_tst(b,v)       ((b)&(v))                   //获取b&v的结果
#define bits_set(b,v)       ((b)|=(v))                  //给b设置v的值
#define bits_clr(b,v)       ((b)&=~(v))                 //对b清理v的值
//---------------------------------------------------------
//对变量b进行位序操作(位序i从0开始)
#define bit_tst(b,i)        bits_tst(b,(1<<(i)))        //测试,b的第i位是否置位
#define bit_set(b,i)        bits_set(b,(1<<(i)))        //置位,b的第i位
#define bit_clr(b,i)        bits_clr(b,(1<<(i)))        //复位,b的第i位

//---------------------------------------------------------
//获取变量b的指定位序i与位数的值(位序i从0开始,位数方向从高向低计算)
#define bits_get1(b,i)      (((b)&(0x01<<(i)))>>(i))
#define bits_get2(b,i)      (((b)&(0x03<<(i-1)))>>(i-1))
#define bits_get3(b,i)      (((b)&(0x07<<(i-2)))>>(i-2))
#define bits_get4(b,i)      (((b)&(0x0f<<(i-3)))>>(i-3))
#define bits_get5(b,i)      (((b)&(0x1f<<(i-4)))>>(i-4))
#define bits_get6(b,i)      (((b)&(0x3f<<(i-5)))>>(i-5))
#define bits_get7(b,i)      (((b)&(0x7f<<(i-6)))>>(i-6))
#define bits_get8(b,i)      (((b)&(0xff<<(i-7)))>>(i-7))

//---------------------------------------------------------
//覆盖设置变量b的指定位序i与位数的值为v(位序i从0开始,位数方向从高向低计算)
#define bits_put1(b,i,v)    (((b)&=~(0x01<<(i))),(b|=(((v)&0x01)<<(i))))
#define bits_put2(b,i,v)    (((b)&=~(0x03<<(i-1))),(b|=(((v)&0x03)<<(i-1))))
#define bits_put3(b,i,v)    (((b)&=~(0x07<<(i-2))),(b|=(((v)&0x07)<<(i-2))))
#define bits_put4(b,i,v)    (((b)&=~(0x0f<<(i-3))),(b|=(((v)&0x0f)<<(i-3))))
#define bits_put5(b,i,v)    (((b)&=~(0x1f<<(i-4))),(b|=(((v)&0x1f)<<(i-4))))
#define bits_put6(b,i,v)    (((b)&=~(0x3f<<(i-5))),(b|=(((v)&0x3f)<<(i-5))))
#define bits_put7(b,i,v)    (((b)&=~(0x7f<<(i-6))),(b|=(((v)&0x7f)<<(i-6))))
#define bits_put8(b,i,v)    (((b)&=~(0xff<<(i-7))),(b|=(((v)&0xff)<<(i-7))))

//---------------------------------------------------------
class rx_bits_mask_t
{
public:
	//获取指定的低位置位的掩码
	//返回值:0无效
	static const uint32_t lowset(uint32_t idx)
	{
		static  uint32_t masks[] = { 0x00000001,0x00000003,0x00000007,0x0000000f,0x0000001f,0x0000003f,0x0000007f,0x000000ff,
									0x000001ff,0x000003ff,0x000007ff,0x00000fff,0x00001fff,0x00003fff,0x00007fff,0x0000ffff,
									0x0001ffff,0x0003ffff,0x0007ffff,0x000fffff,0x001fffff,0x003fffff,0x007fffff,0x00ffffff,
									0x01ffffff,0x03ffffff,0x07ffffff,0x0fffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff };
		const uint32_t masksize = sizeof(masks) / sizeof(uint32_t);
		if (idx >= masksize)
			return 0;
		return masks[idx];
	}
};

//---------------------------------------------------------
//获取变量b的指定位序i与位数c的值(位序i从0开始,位数方向从高向低计算)
//回值告知是否成功;结果为R
template<class T>
inline bool rx_bits_gets(const T &b, uint32_t i, uint32_t c, T &R)
{
	const uint32_t mbc = sizeof(T) * 8;

	if (i >= mbc || c<1 || c>mbc || c - 1 > i)
		return false;

	uint32_t mask = rx_bits_mask_t::lowset(c - 1);
	if (!mask)
		return false;
	uint32_t offset = i - c + 1;
	R = (b & (mask << offset)) >> offset;
	return true;
}

//---------------------------------------------------------
//覆盖设置变量b的指定位序i与位数c的值为v(位序i从0开始,位数方向从高向低计算)
//回值告知是否成功;结果为R
template<class T>
inline bool rx_bits_puts(const T &b, uint32_t i, uint32_t c, uint32_t v, T &R)
{
	const uint32_t mbc = sizeof(T) * 8;

	if (i >= mbc || c<1 || c>mbc || c - 1 > i)
		return false;

	uint32_t mask = rx_bits_mask_t::lowset(c - 1);
	if (!mask)
		return false;

	uint32_t offset = i - c + 1;
	R = b;
	R &= ~(mask << offset);                             //先清理原位置
	R |= (v & mask) << offset;                          //再覆盖新值

	return true;
}

//--------------------------------------------------------------
//在指定长度len的字节数组bytes中,设置第idx位.字节数组的高地址对应高位序.
static inline bool rx_bits_set(uint32_t idx, uint8_t *bytes, uint32_t len = 8)
{
	uint32_t b_idx = idx >> 3;      //=idx/8
	uint32_t b_offset = idx & 0x07; //=idx%8
	if (b_idx >= len)
		return false;
	bit_set(bytes[b_idx], b_offset);
	return true;
}
//在指定长度len的字节数组bytes中,清0第idx位.字节数组的高地址对应高位序.
static inline bool rx_bits_clr(uint32_t idx, uint8_t *bytes, uint32_t len = 8)
{
	uint32_t b_idx = idx >> 3;      //=idx/8
	uint32_t b_offset = idx & 0x07; //=idx%8
	if (b_idx >= len)
		return false;
	bit_clr(bytes[b_idx], b_offset);
	return true;
}
//在指定长度len的字节数组bytes中(从低至高),获取第idx位(判断其是否置位,从0算起).字节数组的高地址对应高位序.
static inline bool rx_bits_tst(uint32_t idx, const uint8_t *bytes, uint32_t len = 8)
{
	uint32_t b_idx = idx >> 3;      //=idx/8
	uint32_t b_offset = idx & 0x07; //=idx%8
	if (b_idx >= len)
		return false;
	return !!bit_tst(bytes[b_idx], b_offset);
}

//--------------------------------------------------------------
//bits数组功能封装
class rx_bits_array_t
{
	uint32_t         m_bytes_idx;                       //字节索引
	uint8_t          m_bits_offset;                     //字节内比特偏移
	uint8_t          m_order_dir;                       //遍历方向,0为升序.
	uint8_t         *m_bytes;                           //字节数组
	uint32_t         m_bytes_len;                       //字节数组长度
public:
	rx_bits_array_t() :m_bytes(0), m_bytes_len(0) {}
	rx_bits_array_t(uint8_t* data, uint32_t datalen) :m_bytes(data), m_bytes_len(datalen) { begin(true); }
	rx_bits_array_t(uint8_t* data, uint32_t datalen, bool asc) :m_bytes(data), m_bytes_len(datalen) { begin(asc); }
	//-----------------------------------------------------------
	//准备进行顺序操作.从低字节低位序开始访问,升序/从高字节高位序开始访问,降序
	bool begin(bool asc_order = true)
	{
		if (!m_bytes || !m_bytes_len) return false;
		if (asc_order)
		{
			m_order_dir = 0;
			m_bytes_idx = 0;
			m_bits_offset = 0;
		}
		else
		{
			m_order_dir = 1;
			m_bytes_idx = m_bytes_len - 1;
			m_bits_offset = 7;
		}
		return true;
	}
	bool begin(uint8_t* data, uint32_t datalen, bool asc = true)
	{
		m_bytes = data;
		m_bytes_len = datalen;
		return begin(asc);
	}
	//-----------------------------------------------------------
	//移动当前操作点到下一位,准备访问
	//返回值:是否移动成功
	bool next()
	{
		if (m_order_dir)
		{//降序
			if (!m_bytes_idx && !m_bits_offset)
				return false;

			--m_bits_offset;
			if (!m_bits_offset&&m_bytes_idx)
			{
				--m_bytes_idx;
				m_bits_offset = 7;
			}
		}
		else
		{//升序
			if ((m_bytes_idx == m_bytes_len - 1) && m_bits_offset == 7)
				return false;
			++m_bits_offset;
			if (m_bits_offset == 8)
			{
				++m_bytes_idx;
				m_bits_offset = 0;
			}
		}
		return true;
	}
	//-----------------------------------------------------------
	//比特总数
	uint32_t bits() { return (m_bytes_len << 3); }
	//-----------------------------------------------------------
	//得到当前正在操作的bit序号,从0开始.
	uint32_t pos() { return (m_bytes_idx << 3) + m_bits_offset; }
	//调整操作位序点
	bool pos(uint32_t idx)
	{
		if (idx >= bits())
			return false;
		m_bytes_idx = (idx >> 3);
		m_bits_offset = idx & 7;
		return true;
	}
	//-----------------------------------------------------------
	//获取待遍历的剩余比特总数
	uint32_t remain()
	{
		if (m_order_dir)        //降序
			return pos();
		else                    //升序
			return bits() - pos() - 1;
	}
	//-----------------------------------------------------------
	//测试当前位
	bool tst() { return !!bit_tst(m_bytes[m_bytes_idx], m_bits_offset); }
	//-----------------------------------------------------------
	//当前位置位
	void set() { bit_set(m_bytes[m_bytes_idx], m_bits_offset); }
	//-----------------------------------------------------------
	//当前位复位
	void clr() { bit_clr(m_bytes[m_bytes_idx], m_bits_offset); }
	//-----------------------------------------------------------
	//从当前位置获取bitcount比特位的值到value中.
	//升降序决定了value中的开始位置.(升序从低向高;降序从高向低)
	//返回值:告知是否成功
	template<class DT>
	bool fetch(DT &value, uint8_t bitcount)
	{
		value = 0;
		if (bitcount > (sizeof(value) << 3))
			return false;
		if (bitcount > remain())
			return false;

		if (m_order_dir)
		{//降序,填充到value的高处
			for (uint8_t i = 0; i < bitcount; ++i)
			{
				if (tst())
					bit_set(value, (sizeof(DT) << 3) - i - 1);
				next();
			}
		}
		else
		{//升序,填充到value的低处
			for (uint8_t i = 0; i < bitcount; ++i)
			{
				if (tst())
					bit_set(value, i);
				next();
			}
		}

		return true;
	}
	//-----------------------------------------------------------
	//将value中的bitcount比特填充到当前bit流的位置.
	//升降序决定了value中的开始位置.(升序从低向高;降序从高向低)
	template<class DT>
	bool fill(const DT value, uint8_t bitcount)
	{
		if (bitcount > (sizeof(value) << 3))
			return false;
		if (bitcount > remain())
			return false;

		if (m_order_dir)
		{//降序,从value的高位处逐一复制
			for (uint8_t i = 0; i < bitcount; ++i)
			{
				if (bit_tst(value, (sizeof(DT) << 3) - i - 1))
					set();
				else
					clr();
				next();
			}
		}
		else
		{//升序,从value的低位处逐一复制
			for (uint8_t i = 0; i < bitcount; ++i)
			{
				if (bit_tst(value, i))
					set();
				else
					clr();
				next();
			}
		}

		return true;
	}
};

#endif