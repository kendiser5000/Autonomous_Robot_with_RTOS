/** @file Filter.h
 * @brief Runs on TM4C123, Simple Median Filter Macro
 * UART Command Line Interface
 * @date 2/9/2019
 * @author Sikender Ashraf and Sijin Woo
 */

#ifndef __Filter_H__
#define __Filter_H__ 



/** AddFilter(NAME)
 * @brief Runs on TM4C123, Simple Median Filter Macro
 * UART Command Line Interface
 * @date 2/9/2019
 * @author Sikender Ashraf and Sijin Woo
 */
#define AddFilter(NAME)\
long Filter ## NAME(long y){ \
	static long x ## NAME[2];\
  long result ## NAME;\
  if(y>x ## NAME[0])\
    if(x ## NAME[0]>x ## NAME[1])   result ## NAME=x ## NAME[0];     \
      else\
        if(y>x ## NAME[1]) result ## NAME=x ## NAME[1];   \
        else      result ## NAME=y;   \
  else \
    if(x ## NAME[1]>x ## NAME[0])   result ## NAME=x ## NAME[0];     \
      else\
        if(y>x ## NAME[1]) result ## NAME=y;   \
        else      result ## NAME=x ## NAME[1];   \
	x ## NAME[1] = x ## NAME[0];\
	x ## NAME[0] = y;\
  return(result ## NAME);\
}\





#endif
