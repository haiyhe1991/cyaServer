#ifndef __CYA_MAX_MIN_H__
#define __CYA_MAX_MIN_H__

#define rc_style_min(a,b)	(((a)<(b))?(a):(b))
#define rc_style_max(a,b)	(((b)<(a))?(a):(b))
#define rc_style_abs(v)		((v)<0?-(v):(v))

#if !defined(min)&&!defined(max)
	#define min		rc_style_min
	#define max		rc_style_max
#endif
#if !defined(min)||!defined(max)
	#error min max define error.
#endif
template <typename TYPE>
inline void rose_swap(TYPE&v1, TYPE&v2)
{
	TYPE t = v1;
	v1 = v2;
	v2 = t;
}
#endif