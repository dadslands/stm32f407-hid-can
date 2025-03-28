#ifndef __GPIOA_DEFS_H
#define __GPIOA_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Define GPIO ports for STM32F407 if not already defined */
#ifndef GPIOA
#define GPIOA ((GPIO_TypeDef*)0x40020000)
#endif

#ifndef GPIOB
#define GPIOB ((GPIO_TypeDef*)0x40020400)
#endif

#ifndef GPIOC
#define GPIOC ((GPIO_TypeDef*)0x40020800)
#endif

#ifndef GPIOD
#define GPIOD ((GPIO_TypeDef*)0x40020C00)
#endif

#ifndef GPIOE
#define GPIOE ((GPIO_TypeDef*)0x40021000)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GPIOA_DEFS_H */
