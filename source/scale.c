static inline void scale3x_8_def_center(unsigned char* dst, const unsigned char* src0, const unsigned char* src1, const unsigned char* src2, unsigned count) {
	/* first pixel */
	if(src0[0] != src2[0] && src1[0] != src1[1]) {
		dst[0] = (src1[0] == src0[0] && src1[0] != src2[0]) || (src1[0] == src2[0] && src1[0] != src0[0]) ? src1[0] : src1[0];
		dst[1] = src1[0];
		dst[2] = (src1[1] == src0[0] && src1[0] != src2[1]) || (src1[1] == src2[0] && src1[0] != src0[1]) ? src1[1] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
	++src0;
	++src1;
	++src2;
	dst += 3;

	/* central pixels */
	count -= 2;
	while(count) {
		if(src0[0] != src2[0] && src1[-1] != src1[1]) {
			dst[0] = (src1[-1] == src0[0] && src1[0] != src2[-1]) || (src1[-1] == src2[0] && src1[0] != src0[-1]) ? src1[-1] : src1[0];
			dst[1] = src1[0];
			dst[2] = (src1[1] == src0[0] && src1[0] != src2[1]) || (src1[1] == src2[0] && src1[0] != src0[1]) ? src1[1] : src1[0];
		} else {
			dst[0] = src1[0];
			dst[1] = src1[0];
			dst[2] = src1[0];
		}

		++src0;
		++src1;
		++src2;
		dst += 3;
		--count;
	}

	/* last pixel */
	if(src0[0] != src2[0] && src1[-1] != src1[0]) {
		dst[0] = (src1[-1] == src0[0] && src1[0] != src2[-1]) || (src1[-1] == src2[0] && src1[0] != src0[-1]) ? src1[-1] : src1[0];
		dst[1] = src1[0];
		dst[2] = (src1[0] == src0[0] && src1[0] != src2[0]) || (src1[0] == src2[0] && src1[0] != src0[0]) ? src1[0] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
}

static inline void scale3x_8_def_border(unsigned char* dst, const unsigned char* src0, const unsigned char* src1, const unsigned char* src2, unsigned count) {

	/* first pixel */
	if(src0[0] != src2[0] && src1[0] != src1[1]) {
		dst[0] = src1[0];
		dst[1] = (src1[0] == src0[0] && src1[0] != src0[1]) || (src1[1] == src0[0] && src1[0] != src0[0]) ? src0[0] : src1[0];
		dst[2] = src1[1] == src0[0] ? src1[1] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
	++src0;
	++src1;
	++src2;
	dst += 3;

	/* central pixels */
	count -= 2;
	while(count) {
		if(src0[0] != src2[0] && src1[-1] != src1[1]) {
			dst[0] = src1[-1] == src0[0] ? src1[-1] : src1[0];
			dst[1] = (src1[-1] == src0[0] && src1[0] != src0[1]) || (src1[1] == src0[0] && src1[0] != src0[-1]) ? src0[0] : src1[0];
			dst[2] = src1[1] == src0[0] ? src1[1] : src1[0];
		} else {
			dst[0] = src1[0];
			dst[1] = src1[0];
			dst[2] = src1[0];
		}

		++src0;
		++src1;
		++src2;
		dst += 3;
		--count;
	}

	/* last pixel */
	if(src0[0] != src2[0] && src1[-1] != src1[0]) {
		dst[0] = src1[-1] == src0[0] ? src1[-1] : src1[0];
		dst[1] = (src1[-1] == src0[0] && src1[0] != src0[0]) || (src1[0] == src0[0] && src1[0] != src0[-1]) ? src0[0] : src1[0];
		dst[2] = src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
}

static inline void scale3x_32_def_border(unsigned* dst, const unsigned* src0, const unsigned* src1, const unsigned* src2, unsigned count) {

	/* first pixel */
	if(src0[0] != src2[0] && src1[0] != src1[1]) {
		dst[0] = src1[0];
		dst[1] = (src1[0] == src0[0] && src1[0] != src0[1]) || (src1[1] == src0[0] && src1[0] != src0[0]) ? src0[0] : src1[0];
		dst[2] = src1[1] == src0[0] ? src1[1] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
	++src0;
	++src1;
	++src2;
	dst += 3;

	/* central pixels */
	count -= 2;
	while(count) {
		if(src0[0] != src2[0] && src1[-1] != src1[1]) {
			dst[0] = src1[-1] == src0[0] ? src1[-1] : src1[0];
			dst[1] = (src1[-1] == src0[0] && src1[0] != src0[1]) || (src1[1] == src0[0] && src1[0] != src0[-1]) ? src0[0] : src1[0];
			dst[2] = src1[1] == src0[0] ? src1[1] : src1[0];
		} else {
			dst[0] = src1[0];
			dst[1] = src1[0];
			dst[2] = src1[0];
		}

		++src0;
		++src1;
		++src2;
		dst += 3;
		--count;
	}

	/* last pixel */
	if(src0[0] != src2[0] && src1[-1] != src1[0]) {
		dst[0] = src1[-1] == src0[0] ? src1[-1] : src1[0];
		dst[1] = (src1[-1] == src0[0] && src1[0] != src0[0]) || (src1[0] == src0[0] && src1[0] != src0[-1]) ? src0[0] : src1[0];
		dst[2] = src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
}

static inline void scale3x_32_def_center(unsigned* dst, const unsigned* src0, const unsigned* src1, const unsigned* src2, unsigned count) {

	/* first pixel */
	if(src0[0] != src2[0] && src1[0] != src1[1]) {
		dst[0] = (src1[0] == src0[0] && src1[0] != src2[0]) || (src1[0] == src2[0] && src1[0] != src0[0]) ? src1[0] : src1[0];
		dst[1] = src1[0];
		dst[2] = (src1[1] == src0[0] && src1[0] != src2[1]) || (src1[1] == src2[0] && src1[0] != src0[1]) ? src1[1] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
	++src0;
	++src1;
	++src2;
	dst += 3;

	/* central pixels */
	count -= 2;
	while(count) {
		if(src0[0] != src2[0] && src1[-1] != src1[1]) {
			dst[0] = (src1[-1] == src0[0] && src1[0] != src2[-1]) || (src1[-1] == src2[0] && src1[0] != src0[-1]) ? src1[-1] : src1[0];
			dst[1] = src1[0];
			dst[2] = (src1[1] == src0[0] && src1[0] != src2[1]) || (src1[1] == src2[0] && src1[0] != src0[1]) ? src1[1] : src1[0];
		} else {
			dst[0] = src1[0];
			dst[1] = src1[0];
			dst[2] = src1[0];
		}

		++src0;
		++src1;
		++src2;
		dst += 3;
		--count;
	}

	/* last pixel */
	if(src0[0] != src2[0] && src1[-1] != src1[0]) {
		dst[0] = (src1[-1] == src0[0] && src1[0] != src2[-1]) || (src1[-1] == src2[0] && src1[0] != src0[-1]) ? src1[-1] : src1[0];
		dst[1] = src1[0];
		dst[2] = (src1[0] == src0[0] && src1[0] != src2[0]) || (src1[0] == src2[0] && src1[0] != src0[0]) ? src1[0] : src1[0];
	} else {
		dst[0] = src1[0];
		dst[1] = src1[0];
		dst[2] = src1[0];
	}
}

/**
* Scale by a factor of 3 a row of pixels of 8 bits.
* The function is implemented in C.
* The pixels over the left and right borders are assumed of the same color of
* the pixels on the border.
* \param src0 Pointer at the first pixel of the previous row.
* \param src1 Pointer at the first pixel of the current row.
* \param src2 Pointer at the first pixel of the next row.
* \param count Length in pixels of the src0, src1 and src2 rows.
* It must be at least 2.
* \param dst0 First destination row, triple length in pixels.
* \param dst1 Second destination row, triple length in pixels.
* \param dst2 Third destination row, triple length in pixels.
*/
void scale3x_8_def(unsigned char* dst0, unsigned char* dst1, unsigned char* dst2, const unsigned char* src0, const unsigned char* src1, const unsigned char* src2, unsigned count) {
	scale3x_8_def_border(dst0, src0, src1, src2, count);
	scale3x_8_def_center(dst1, src0, src1, src2, count);
	scale3x_8_def_border(dst2, src2, src1, src0, count);
}

/**
* Scale by a factor of 3 a row of pixels of 32 bits.
* This function operates like scale3x_8_def() but for 32 bits pixels.
* \param src0 Pointer at the first pixel of the previous row.
* \param src1 Pointer at the first pixel of the current row.
* \param src2 Pointer at the first pixel of the next row.
* \param count Length in pixels of the src0, src1 and src2 rows.
* It must be at least 2.
* \param dst0 First destination row, triple length in pixels.
* \param dst1 Second destination row, triple length in pixels.
* \param dst2 Third destination row, triple length in pixels.
*/
void scale3x_32_def(unsigned* dst0, unsigned* dst1, unsigned* dst2, const unsigned* src0, const unsigned* src1, const unsigned* src2, unsigned count) {
	scale3x_32_def_border(dst0, src0, src1, src2, count);
	scale3x_32_def_center(dst1, src0, src1, src2, count);
	scale3x_32_def_border(dst2, src2, src1, src0, count);
}

static inline void stage_scale3x(void* dst0, void* dst1, void* dst2, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row) {
	switch(pixel) {
	case 1:
		scale3x_8_def(
			(unsigned char*)dst0, (unsigned char*)dst1, (unsigned char*)dst2,
			(unsigned char*)src0, (unsigned char*)src1, (unsigned char*)src2,
			pixel_per_row);
		break;
	case 4:
		scale3x_32_def(
			(unsigned*)dst0, (unsigned*)dst1, (unsigned*)dst2,
			(unsigned*)src0, (unsigned*)src1, (unsigned*)src2,
			pixel_per_row);
		break;
	}
}

/**
* Apply the Scale3x effect on a bitmap.
* The destination bitmap is filled with the scaled version of the source bitmap.
* The source bitmap isn't modified.
* The destination bitmap must be manually allocated before calling the function,
* note that the resulting size is exactly 3x3 times the size of the source bitmap.
* \param void_dst Pointer at the first pixel of the destination bitmap.
* \param dst_slice Size in bytes of a destination bitmap row.
* \param void_src Pointer at the first pixel of the source bitmap.
* \param src_slice Size in bytes of a source bitmap row.
* \param pixel Bytes per pixel of the source and destination bitmap.
* \param width Horizontal size in pixels of the source bitmap.
* \param height Vertical size in pixels of the source bitmap.
*/
void scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height) {
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (const unsigned char*)void_src;
	unsigned count = height;

	stage_scale3x(
		dst + 0 * dst_slice, dst + 1 * dst_slice, dst + 2 * dst_slice,
		src + 0 * src_slice, src + 0 * src_slice, src + 1 * src_slice,
		pixel, width);
	dst += 3 * dst_slice;

	count -= 2;

	while(count) {
		stage_scale3x(
			dst + 0 * dst_slice, dst + 1 * dst_slice, dst + 2 * dst_slice,
			src + 0 * src_slice, src + 1 * src_slice, src + 2 * src_slice,
			pixel, width);
		dst += 3 * dst_slice;
		src += 1 * src_slice;
		--count;
	}

	stage_scale3x(
		dst + 0 * dst_slice, dst + 1 * dst_slice, dst + 2 * dst_slice,
		src + 0 * src_slice, src + 1 * src_slice, src + 1 * src_slice,
		pixel, width);

}