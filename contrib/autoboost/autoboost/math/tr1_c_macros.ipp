// Copyright John Maddock 2008-11.
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_C_MACROS_IPP
#define AUTOBOOST_MATH_C_MACROS_IPP

// C99 Functions:
#ifdef acosh
#undef acosh
#endif
#define acosh autoboost_acosh
#ifdef acoshf
#undef acoshf
#endif
#define acoshf autoboost_acoshf
#ifdef acoshl
#undef acoshl
#endif
#define acoshl autoboost_acoshl

#ifdef asinh
#undef asinh
#endif
#define asinh autoboost_asinh
#ifdef asinhf
#undef asinhf
#endif
#define asinhf autoboost_asinhf
#ifdef asinhl
#undef asinhl
#endif
#define asinhl autoboost_asinhl

#ifdef atanh
#undef atanh
#endif
#define atanh autoboost_atanh
#ifdef atanhf
#undef atanhf
#endif
#define atanhf autoboost_atanhf
#ifdef atanhl
#undef atanhl
#endif
#define atanhl autoboost_atanhl

#ifdef cbrt
#undef cbrt
#endif
#define cbrt autoboost_cbrt
#ifdef cbrtf
#undef cbrtf
#endif
#define cbrtf autoboost_cbrtf
#ifdef cbrtl
#undef cbrtl
#endif
#define cbrtl autoboost_cbrtl

#ifdef copysign
#undef copysign
#endif
#define copysign autoboost_copysign
#ifdef copysignf
#undef copysignf
#endif
#define copysignf autoboost_copysignf
#ifdef copysignl
#undef copysignl
#endif
#define copysignl autoboost_copysignl

#ifdef erf
#undef erf
#endif
#define erf autoboost_erf
#ifdef erff
#undef erff
#endif
#define erff autoboost_erff
#ifdef erfl
#undef erfl
#endif
#define erfl autoboost_erfl

#ifdef erfc
#undef erfc
#endif
#define erfc autoboost_erfc
#ifdef erfcf
#undef erfcf
#endif
#define erfcf autoboost_erfcf
#ifdef erfcl
#undef erfcl
#endif
#define erfcl autoboost_erfcl

#if 0
#ifdef exp2
#undef exp2
#endif
#define exp2 autoboost_exp2
#ifdef exp2f
#undef exp2f
#endif
#define exp2f autoboost_exp2f
#ifdef exp2l
#undef exp2l
#endif
#define exp2l autoboost_exp2l
#endif

#ifdef expm1
#undef expm1
#endif
#define expm1 autoboost_expm1
#ifdef expm1f
#undef expm1f
#endif
#define expm1f autoboost_expm1f
#ifdef expm1l
#undef expm1l
#endif
#define expm1l autoboost_expm1l

#if 0
#ifdef fdim
#undef fdim
#endif
#define fdim autoboost_fdim
#ifdef fdimf
#undef fdimf
#endif
#define fdimf autoboost_fdimf
#ifdef fdiml
#undef fdiml
#endif
#define fdiml autoboost_fdiml
#ifdef acosh
#undef acosh
#endif
#define fma autoboost_fma
#ifdef fmaf
#undef fmaf
#endif
#define fmaf autoboost_fmaf
#ifdef fmal
#undef fmal
#endif
#define fmal autoboost_fmal
#endif

#ifdef fmax
#undef fmax
#endif
#define fmax autoboost_fmax
#ifdef fmaxf
#undef fmaxf
#endif
#define fmaxf autoboost_fmaxf
#ifdef fmaxl
#undef fmaxl
#endif
#define fmaxl autoboost_fmaxl

#ifdef fmin
#undef fmin
#endif
#define fmin autoboost_fmin
#ifdef fminf
#undef fminf
#endif
#define fminf autoboost_fminf
#ifdef fminl
#undef fminl
#endif
#define fminl autoboost_fminl

#ifdef hypot
#undef hypot
#endif
#define hypot autoboost_hypot
#ifdef hypotf
#undef hypotf
#endif
#define hypotf autoboost_hypotf
#ifdef hypotl
#undef hypotl
#endif
#define hypotl autoboost_hypotl

#if 0
#ifdef ilogb
#undef ilogb
#endif
#define ilogb autoboost_ilogb
#ifdef ilogbf
#undef ilogbf
#endif
#define ilogbf autoboost_ilogbf
#ifdef ilogbl
#undef ilogbl
#endif
#define ilogbl autoboost_ilogbl
#endif

#ifdef lgamma
#undef lgamma
#endif
#define lgamma autoboost_lgamma
#ifdef lgammaf
#undef lgammaf
#endif
#define lgammaf autoboost_lgammaf
#ifdef lgammal
#undef lgammal
#endif
#define lgammal autoboost_lgammal

#ifdef AUTOBOOST_HAS_LONG_LONG
#if 0
#ifdef llrint
#undef llrint
#endif
#define llrint autoboost_llrint
#ifdef llrintf
#undef llrintf
#endif
#define llrintf autoboost_llrintf
#ifdef llrintl
#undef llrintl
#endif
#define llrintl autoboost_llrintl
#endif
#ifdef llround
#undef llround
#endif
#define llround autoboost_llround
#ifdef llroundf
#undef llroundf
#endif
#define llroundf autoboost_llroundf
#ifdef llroundl
#undef llroundl
#endif
#define llroundl autoboost_llroundl
#endif

#ifdef log1p
#undef log1p
#endif
#define log1p autoboost_log1p
#ifdef log1pf
#undef log1pf
#endif
#define log1pf autoboost_log1pf
#ifdef log1pl
#undef log1pl
#endif
#define log1pl autoboost_log1pl

#if 0
#ifdef log2
#undef log2
#endif
#define log2 autoboost_log2
#ifdef log2f
#undef log2f
#endif
#define log2f autoboost_log2f
#ifdef log2l
#undef log2l
#endif
#define log2l autoboost_log2l

#ifdef logb
#undef logb
#endif
#define logb autoboost_logb
#ifdef logbf
#undef logbf
#endif
#define logbf autoboost_logbf
#ifdef logbl
#undef logbl
#endif
#define logbl autoboost_logbl

#ifdef lrint
#undef lrint
#endif
#define lrint autoboost_lrint
#ifdef lrintf
#undef lrintf
#endif
#define lrintf autoboost_lrintf
#ifdef lrintl
#undef lrintl
#endif
#define lrintl autoboost_lrintl
#endif

#ifdef lround
#undef lround
#endif
#define lround autoboost_lround
#ifdef lroundf
#undef lroundf
#endif
#define lroundf autoboost_lroundf
#ifdef lroundl
#undef lroundl
#endif
#define lroundl autoboost_lroundl

#if 0
#ifdef nan
#undef nan
#endif
#define nan autoboost_nan
#ifdef nanf
#undef nanf
#endif
#define nanf autoboost_nanf
#ifdef nanl
#undef nanl
#endif
#define nanl autoboost_nanl

#ifdef nearbyint
#undef nearbyint
#endif
#define nearbyint autoboost_nearbyint
#ifdef nearbyintf
#undef nearbyintf
#endif
#define nearbyintf autoboost_nearbyintf
#ifdef nearbyintl
#undef nearbyintl
#endif
#define nearbyintl autoboost_nearbyintl
#endif

#ifdef nextafter
#undef nextafter
#endif
#define nextafter autoboost_nextafter
#ifdef nextafterf
#undef nextafterf
#endif
#define nextafterf autoboost_nextafterf
#ifdef nextafterl
#undef nextafterl
#endif
#define nextafterl autoboost_nextafterl

#ifdef nexttoward
#undef nexttoward
#endif
#define nexttoward autoboost_nexttoward
#ifdef nexttowardf
#undef nexttowardf
#endif
#define nexttowardf autoboost_nexttowardf
#ifdef nexttowardl
#undef nexttowardl
#endif
#define nexttowardl autoboost_nexttowardl

#if 0
#ifdef remainder
#undef remainder
#endif
#define remainder autoboost_remainder
#ifdef remainderf
#undef remainderf
#endif
#define remainderf autoboost_remainderf
#ifdef remainderl
#undef remainderl
#endif
#define remainderl autoboost_remainderl

#ifdef remquo
#undef remquo
#endif
#define remquo autoboost_remquo
#ifdef remquof
#undef remquof
#endif
#define remquof autoboost_remquof
#ifdef remquol
#undef remquol
#endif
#define remquol autoboost_remquol

#ifdef rint
#undef rint
#endif
#define rint autoboost_rint
#ifdef rintf
#undef rintf
#endif
#define rintf autoboost_rintf
#ifdef rintl
#undef rintl
#endif
#define rintl autoboost_rintl
#endif

#ifdef round
#undef round
#endif
#define round autoboost_round
#ifdef roundf
#undef roundf
#endif
#define roundf autoboost_roundf
#ifdef roundl
#undef roundl
#endif
#define roundl autoboost_roundl

#if 0
#ifdef scalbln
#undef scalbln
#endif
#define scalbln autoboost_scalbln
#ifdef scalblnf
#undef scalblnf
#endif
#define scalblnf autoboost_scalblnf
#ifdef scalblnl
#undef scalblnl
#endif
#define scalblnl autoboost_scalblnl

#ifdef scalbn
#undef scalbn
#endif
#define scalbn autoboost_scalbn
#ifdef scalbnf
#undef scalbnf
#endif
#define scalbnf autoboost_scalbnf
#ifdef scalbnl
#undef scalbnl
#endif
#define scalbnl autoboost_scalbnl
#endif

#ifdef tgamma
#undef tgamma
#endif
#define tgamma autoboost_tgamma
#ifdef tgammaf
#undef tgammaf
#endif
#define tgammaf autoboost_tgammaf
#ifdef tgammal
#undef tgammal
#endif
#define tgammal autoboost_tgammal

#ifdef trunc
#undef trunc
#endif
#define trunc autoboost_trunc
#ifdef truncf
#undef truncf
#endif
#define truncf autoboost_truncf
#ifdef truncl
#undef truncl
#endif
#define truncl autoboost_truncl

// [5.2.1.1] associated Laguerre polynomials:
#ifdef assoc_laguerre
#undef assoc_laguerre
#endif
#define assoc_laguerre autoboost_assoc_laguerre
#ifdef assoc_laguerref
#undef assoc_laguerref
#endif
#define assoc_laguerref autoboost_assoc_laguerref
#ifdef assoc_laguerrel
#undef assoc_laguerrel
#endif
#define assoc_laguerrel autoboost_assoc_laguerrel

// [5.2.1.2] associated Legendre functions:
#ifdef assoc_legendre
#undef assoc_legendre
#endif
#define assoc_legendre autoboost_assoc_legendre
#ifdef assoc_legendref
#undef assoc_legendref
#endif
#define assoc_legendref autoboost_assoc_legendref
#ifdef assoc_legendrel
#undef assoc_legendrel
#endif
#define assoc_legendrel autoboost_assoc_legendrel

// [5.2.1.3] beta function:
#ifdef beta
#undef beta
#endif
#define beta autoboost_beta
#ifdef betaf
#undef betaf
#endif
#define betaf autoboost_betaf
#ifdef betal
#undef betal
#endif
#define betal autoboost_betal

// [5.2.1.4] (complete) elliptic integral of the first kind:
#ifdef comp_ellint_1
#undef comp_ellint_1
#endif
#define comp_ellint_1 autoboost_comp_ellint_1
#ifdef comp_ellint_1f
#undef comp_ellint_1f
#endif
#define comp_ellint_1f autoboost_comp_ellint_1f
#ifdef comp_ellint_1l
#undef comp_ellint_1l
#endif
#define comp_ellint_1l autoboost_comp_ellint_1l

// [5.2.1.5] (complete) elliptic integral of the second kind:
#ifdef comp_ellint_2
#undef comp_ellint_2
#endif
#define comp_ellint_2 autoboost_comp_ellint_2
#ifdef comp_ellint_2f
#undef comp_ellint_2f
#endif
#define comp_ellint_2f autoboost_comp_ellint_2f
#ifdef comp_ellint_2l
#undef comp_ellint_2l
#endif
#define comp_ellint_2l autoboost_comp_ellint_2l

// [5.2.1.6] (complete) elliptic integral of the third kind:
#ifdef comp_ellint_3
#undef comp_ellint_3
#endif
#define comp_ellint_3 autoboost_comp_ellint_3
#ifdef comp_ellint_3f
#undef comp_ellint_3f
#endif
#define comp_ellint_3f autoboost_comp_ellint_3f
#ifdef comp_ellint_3l
#undef comp_ellint_3l
#endif
#define comp_ellint_3l autoboost_comp_ellint_3l

#if 0
// [5.2.1.7] confluent hypergeometric functions:
#ifdef conf_hyper
#undef conf_hyper
#endif
#define conf_hyper autoboost_conf_hyper
#ifdef conf_hyperf
#undef conf_hyperf
#endif
#define conf_hyperf autoboost_conf_hyperf
#ifdef conf_hyperl
#undef conf_hyperl
#endif
#define conf_hyperl autoboost_conf_hyperl
#endif

// [5.2.1.8] regular modified cylindrical Bessel functions:
#ifdef cyl_bessel_i
#undef cyl_bessel_i
#endif
#define cyl_bessel_i autoboost_cyl_bessel_i
#ifdef cyl_bessel_if
#undef cyl_bessel_if
#endif
#define cyl_bessel_if autoboost_cyl_bessel_if
#ifdef cyl_bessel_il
#undef cyl_bessel_il
#endif
#define cyl_bessel_il autoboost_cyl_bessel_il

// [5.2.1.9] cylindrical Bessel functions (of the first kind):
#ifdef cyl_bessel_j
#undef cyl_bessel_j
#endif
#define cyl_bessel_j autoboost_cyl_bessel_j
#ifdef cyl_bessel_jf
#undef cyl_bessel_jf
#endif
#define cyl_bessel_jf autoboost_cyl_bessel_jf
#ifdef cyl_bessel_jl
#undef cyl_bessel_jl
#endif
#define cyl_bessel_jl autoboost_cyl_bessel_jl

// [5.2.1.10] irregular modified cylindrical Bessel functions:
#ifdef cyl_bessel_k
#undef cyl_bessel_k
#endif
#define cyl_bessel_k autoboost_cyl_bessel_k
#ifdef cyl_bessel_kf
#undef cyl_bessel_kf
#endif
#define cyl_bessel_kf autoboost_cyl_bessel_kf
#ifdef cyl_bessel_kl
#undef cyl_bessel_kl
#endif
#define cyl_bessel_kl autoboost_cyl_bessel_kl

// [5.2.1.11] cylindrical Neumann functions AUTOBOOST_MATH_C99_THROW_SPEC;
// cylindrical Bessel functions (of the second kind):
#ifdef cyl_neumann
#undef cyl_neumann
#endif
#define cyl_neumann autoboost_cyl_neumann
#ifdef cyl_neumannf
#undef cyl_neumannf
#endif
#define cyl_neumannf autoboost_cyl_neumannf
#ifdef cyl_neumannl
#undef cyl_neumannl
#endif
#define cyl_neumannl autoboost_cyl_neumannl

// [5.2.1.12] (incomplete) elliptic integral of the first kind:
#ifdef ellint_1
#undef ellint_1
#endif
#define ellint_1 autoboost_ellint_1
#ifdef ellint_1f
#undef ellint_1f
#endif
#define ellint_1f autoboost_ellint_1f
#ifdef ellint_1l
#undef ellint_1l
#endif
#define ellint_1l autoboost_ellint_1l

// [5.2.1.13] (incomplete) elliptic integral of the second kind:
#ifdef ellint_2
#undef ellint_2
#endif
#define ellint_2 autoboost_ellint_2
#ifdef ellint_2f
#undef ellint_2f
#endif
#define ellint_2f autoboost_ellint_2f
#ifdef ellint_2l
#undef ellint_2l
#endif
#define ellint_2l autoboost_ellint_2l

// [5.2.1.14] (incomplete) elliptic integral of the third kind:
#ifdef ellint_3
#undef ellint_3
#endif
#define ellint_3 autoboost_ellint_3
#ifdef ellint_3f
#undef ellint_3f
#endif
#define ellint_3f autoboost_ellint_3f
#ifdef ellint_3l
#undef ellint_3l
#endif
#define ellint_3l autoboost_ellint_3l

// [5.2.1.15] exponential integral:
#ifdef expint
#undef expint
#endif
#define expint autoboost_expint
#ifdef expintf
#undef expintf
#endif
#define expintf autoboost_expintf
#ifdef expintl
#undef expintl
#endif
#define expintl autoboost_expintl

// [5.2.1.16] Hermite polynomials:
#ifdef hermite
#undef hermite
#endif
#define hermite autoboost_hermite
#ifdef hermitef
#undef hermitef
#endif
#define hermitef autoboost_hermitef
#ifdef hermitel
#undef hermitel
#endif
#define hermitel autoboost_hermitel

#if 0
// [5.2.1.17] hypergeometric functions:
#ifdef hyperg
#undef hyperg
#endif
#define hyperg autoboost_hyperg
#ifdef hypergf
#undef hypergf
#endif
#define hypergf autoboost_hypergf
#ifdef hypergl
#undef hypergl
#endif
#define hypergl autoboost_hypergl
#endif

// [5.2.1.18] Laguerre polynomials:
#ifdef laguerre
#undef laguerre
#endif
#define laguerre autoboost_laguerre
#ifdef laguerref
#undef laguerref
#endif
#define laguerref autoboost_laguerref
#ifdef laguerrel
#undef laguerrel
#endif
#define laguerrel autoboost_laguerrel

// [5.2.1.19] Legendre polynomials:
#ifdef legendre
#undef legendre
#endif
#define legendre autoboost_legendre
#ifdef legendref
#undef legendref
#endif
#define legendref autoboost_legendref
#ifdef legendrel
#undef legendrel
#endif
#define legendrel autoboost_legendrel

// [5.2.1.20] Riemann zeta function:
#ifdef riemann_zeta
#undef riemann_zeta
#endif
#define riemann_zeta autoboost_riemann_zeta
#ifdef riemann_zetaf
#undef riemann_zetaf
#endif
#define riemann_zetaf autoboost_riemann_zetaf
#ifdef riemann_zetal
#undef riemann_zetal
#endif
#define riemann_zetal autoboost_riemann_zetal

// [5.2.1.21] spherical Bessel functions (of the first kind):
#ifdef sph_bessel
#undef sph_bessel
#endif
#define sph_bessel autoboost_sph_bessel
#ifdef sph_besself
#undef sph_besself
#endif
#define sph_besself autoboost_sph_besself
#ifdef sph_bessell
#undef sph_bessell
#endif
#define sph_bessell autoboost_sph_bessell

// [5.2.1.22] spherical associated Legendre functions:
#ifdef sph_legendre
#undef sph_legendre
#endif
#define sph_legendre autoboost_sph_legendre
#ifdef sph_legendref
#undef sph_legendref
#endif
#define sph_legendref autoboost_sph_legendref
#ifdef sph_legendrel
#undef sph_legendrel
#endif
#define sph_legendrel autoboost_sph_legendrel

// [5.2.1.23] spherical Neumann functions AUTOBOOST_MATH_C99_THROW_SPEC;
// spherical Bessel functions (of the second kind):
#ifdef sph_neumann
#undef sph_neumann
#endif
#define sph_neumann autoboost_sph_neumann
#ifdef sph_neumannf
#undef sph_neumannf
#endif
#define sph_neumannf autoboost_sph_neumannf
#ifdef sph_neumannl
#undef sph_neumannl
#endif
#define sph_neumannl autoboost_sph_neumannl

#endif // AUTOBOOST_MATH_C_MACROS_IPP
