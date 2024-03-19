#include "ext_io.h"

uint16_t input_buffer[3];
uint16_t output_buffer[2];

//读输入1~16
uint16_t read_input1_16(void)
{
	return *(uint16_t *)(EXT_I1_ADDR);
}

//读输入17~32
uint16_t read_input17_32(void)
{
	return *(uint16_t *)(EXT_I2_ADDR);
}

//读输入33~40
uint16_t read_input33_40(void)
{
	return *(uint16_t *)(EXT_I3_ADDR)&0xFF;
}

//输出1~16
void write_output1_16(uint16_t data)
{
	*(uint16_t *)(EXT_O1_ADDR) = data;
}

//输出17~32
void write_output17_32(uint16_t data)
{
	*(uint16_t *)(EXT_O2_ADDR) = data;
}

//updata所有输入
void updata_all_input(uint16_t* in_buf)
{
	in_buf[0] = read_input1_16();
	in_buf[1] = read_input17_32();
	in_buf[2] = read_input33_40();
}

//updata所有输出
void updata_all_output(uint16_t* out_buf)
{
	write_output1_16(out_buf[0]);
	write_output17_32(out_buf[1]);
}
	
//刷新所有IO
void refresh_all_io(void)
{
	updata_all_input(input_buffer);
	updata_all_output(output_buffer);
}
