//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_SF_TRIGAMMA_HPP
#define AUTOBOOST_MATH_SF_TRIGAMMA_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/tools/rational.hpp>
#include <autoboost/math/tools/series.hpp>
#include <autoboost/math/tools/promotion.hpp>
#include <autoboost/math/policies/error_handling.hpp>
#include <autoboost/math/constants/constants.hpp>
#include <autoboost/mpl/comparison.hpp>
#include <autoboost/math/tools/big_constant.hpp>
#include <autoboost/math/special_functions/polygamma.hpp>

namespace autoboost{
namespace math{
namespace detail{

template<class T, class Policy>
T polygamma_imp(const int n, T x, const Policy &pol);

template <class T, class Policy>
T trigamma_prec(T x, const mpl::int_<53>*, const Policy&)
{
   // Max error in interpolated form: 3.736e-017
   static const T offset = AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.1093254089355469);
   static const T P_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -1.1093280605946045),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -3.8310674472619321),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -3.3703848401898283),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.28080574467981213),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 1.6638069578676164),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.64468386819102836),
   };
   static const T Q_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 3.4535389668541151),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 4.5208926987851437),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.7012734178351534),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.64468798399785611),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -0.20314516859987728e-6),
   };
   // Max error in interpolated form: 1.159e-017
   static const T P_2_4[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -0.13803835004508849e-7),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.50000049158540261),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 1.6077979838469348),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.5645435828098254),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.0534873203680393),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.74566981111565923),
   };
   static const T Q_2_4[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.8822787662376169),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 4.1681660554090917),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 2.7853527819234466),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, 0.74967671848044792),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 53, -0.00057069112416246805),
   };
   // Maximum Deviation Found:                     6.896e-018
   // Expected Error Term :                       -6.895e-018
   // Maximum Relative Change in Control Points :  8.497e-004
   static const T P_4_inf[] = {
      static_cast<T>(0.68947581948701249e-17L),
      static_cast<T>(0.49999999999998975L),
      static_cast<T>(1.0177274392923795L),
      static_cast<T>(2.498208511343429L),
      static_cast<T>(2.1921221359427595L),
      static_cast<T>(1.5897035272532764L),
      static_cast<T>(0.40154388356961734L),
   };
   static const T Q_4_inf[] = {
      static_cast<T>(1.0L),
      static_cast<T>(1.7021215452463932L),
      static_cast<T>(4.4290431747556469L),
      static_cast<T>(2.9745631894384922L),
      static_cast<T>(2.3013614809773616L),
      static_cast<T>(0.28360399799075752L),
      static_cast<T>(0.022892987908906897L),
   };

   if(x <= 2)
   {
      return (offset + autoboost::math::tools::evaluate_polynomial(P_1_2, x) / tools::evaluate_polynomial(Q_1_2, x)) / (x * x);
   }
   else if(x <= 4)
   {
      T y = 1 / x;
      return (1 + tools::evaluate_polynomial(P_2_4, y) / tools::evaluate_polynomial(Q_2_4, y)) / x;
   }
   T y = 1 / x;
   return (1 + tools::evaluate_polynomial(P_4_inf, y) / tools::evaluate_polynomial(Q_4_inf, y)) / x;
}

template <class T, class Policy>
T trigamma_prec(T x, const mpl::int_<64>*, const Policy&)
{
   // Max error in interpolated form: 1.178e-020
   static const T offset_1_2 = AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 2.109325408935546875);
   static const T P_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -1.10932535608960258341),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -4.18793841543017129052),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -4.63865531898487734531),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.919832884430500908047),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.68074038333180423012),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.21172611429185622377),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.259635673503366427284),
   };
   static const T Q_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 3.77521119359546982995),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 5.664338024578956321),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 4.25995134879278028361),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.62956638448940402182),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.259635512844691089868),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.629642219810618032207e-8),
   };
   // Max error in interpolated form: 3.912e-020
   static const T P_2_8[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.387540035162952880976e-11),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.500000000276430504),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 3.21926880986360957306),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 10.2550347708483445775),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 18.9002075150709144043),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 21.0357215832399705625),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 13.4346512182925923978),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 3.98656291026448279118),
   };
   static const T Q_2_8[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 6.10520430478613667724),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 18.475001060603645512),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 31.7087534567758405638),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 31.908814523890465398),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 17.4175479039227084798),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 3.98749106958394941276),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.000115917322224411128566),
   };
   // Maximum Deviation Found:                     2.635e-020
   // Expected Error Term :                        2.635e-020
   // Maximum Relative Change in Control Points :  1.791e-003
   static const T P_8_inf[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.263527875092466899848e-19),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.500000000000000058145),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.0730121433777364138677),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.94505878379957149534),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.0517092358874932620529),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.07995383547483921121),
   };
   static const T Q_8_inf[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.187309046577818095504),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 3.95255391645238842975),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -1.14743283327078949087),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 2.52989799376344914499),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, -0.627414303172402506396),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 64, 0.141554248216425512536),
   };

   if(x <= 2)
   {
      return (offset_1_2 + autoboost::math::tools::evaluate_polynomial(P_1_2, x) / tools::evaluate_polynomial(Q_1_2, x)) / (x * x);
   }
   else if(x <= 8)
   {
      T y = 1 / x;
      return (1 + tools::evaluate_polynomial(P_2_8, y) / tools::evaluate_polynomial(Q_2_8, y)) / x;
   }
   T y = 1 / x;
   return (1 + tools::evaluate_polynomial(P_8_inf, y) / tools::evaluate_polynomial(Q_8_inf, y)) / x;
}

template <class T, class Policy>
T trigamma_prec(T x, const mpl::int_<113>*, const Policy&)
{
   // Max error in interpolated form: 1.916e-035

   static const T P_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.999999999999999082554457936871832533),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -4.71237311120865266379041700054847734),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -7.94125711970499027763789342500817316),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -5.74657746697664735258222071695644535),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.404213349456398905981223965160595687),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 2.47877781178642876561595890095758896),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 2.07714151702455125992166949812126433),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.858877899162360138844032265418028567),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.20499222604410032375789018837922397),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0272103140348194747360175268778415049),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0015764849020876949848954081173520686),
   };
   static const T Q_1_2[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 4.71237311120863419878375031457715223),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 9.58619118655339853449127952145877467),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 11.0940067269829372437561421279054968),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 8.09075424749327792073276309969037885),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 3.87705890159891405185343806884451286),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.22758678701914477836330837816976782),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.249092040606385004109672077814668716),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0295750413900655597027079600025569048),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.00157648490200498142247694709728858139),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.161264050344059471721062360645432809e-14),
   };

   // Max error in interpolated form: 8.958e-035
   static const T P_2_4[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -2.55843734739907925764326773972215085),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -12.2830208240542011967952466273455887),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -23.9195022162767993526575786066414403),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -24.9256431504823483094158828285470862),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -14.7979122765478779075108064826412285),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -4.46654453928610666393276765059122272),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.0191439033405649675717082465687845002),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.515412052554351265708917209749037352),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.195378348786064304378247325360320038),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0334761282624174313035014426794245393),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.002373665205942206348500250056602687),
   };
   static const T Q_2_4[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 4.80098558454419907830670928248659245),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 9.99220727843170133895059300223445265),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 11.8896146167631330735386697123464976),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 8.96613256683809091593793565879092581),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 4.47254136149624110878909334574485751),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.48600982028196527372434773913633152),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.319570735766764237068541501137990078),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0407358345787680953107374215319322066),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.00237366520593271641375755486420859837),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.239554887903526152679337256236302116e-15),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.294749244740618656265237072002026314e-17),
   };

   static const T y_offset_2_4 = AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 3.558437347412109375);

   // Max error in interpolated form: 4.319e-035
   static const T P_4_8[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.166626112697021464248967707021688845e-16),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.499999999999997739552090249208808197),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 6.40270945019053817915772473771553187),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 41.3833374155000608013677627389343329),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 166.803341854562809335667241074035245),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 453.39964786925369319960722793414521),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 851.153712317697055375935433362983944),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1097.70657567285059133109286478004458),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 938.431232478455316020076349367632922),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 487.268001604651932322080970189930074),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 119.953445242335730062471193124820659),
   };
   static const T Q_4_8[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 12.4720855670474488978638945855932398),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 78.6093129753298570701376952709727391),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 307.470246050318322489781182863190127),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 805.140686101151538537565264188630079),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1439.12019760292146454787601409644413),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1735.6105285756048831268586001383127),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1348.32500712856328019355198611280536),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 607.225985860570846699704222144650563),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 119.952317857277045332558673164517227),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.000140165918355036060868680809129436084),
   };

   // Maximum Deviation Found:                     2.867e-035
   // Expected Error Term :                        2.866e-035
   // Maximum Relative Change in Control Points :  2.662e-004
   static const T P_8_16[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.184828315274146610610872315609837439e-19),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.500000000000000004122475157735807738),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 3.02533865247313349284875558880415875),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 13.5995927517457371243039532492642734),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 35.3132224283087906757037999452941588),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 67.1639424550714159157603179911505619),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 83.5767733658513967581959839367419891),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 71.073491212235705900866411319363501),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 35.8621515614725564575893663483998663),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 8.72152231639983491987779743154333318),
   };
   static const T Q_8_16[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 5.71734397161293452310624822415866372),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 25.293404179620438179337103263274815),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 62.2619767967468199111077640625328469),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 113.955048909238993473389714972250235),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 130.807138328938966981862203944329408),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 102.423146902337654110717764213057753),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 44.0424772805245202514468199602123565),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 8.89898032477904072082994913461386099),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -0.0296627336872039988632793863671456398),
   };
   // Maximum Deviation Found:                     1.079e-035
   // Expected Error Term :                       -1.079e-035
   // Maximum Relative Change in Control Points :  7.884e-003
   static const T P_16_inf[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.500000000000000000000000000000087317),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.345625669885456215194494735902663968),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 9.62895499360842232127552650044647769),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 3.5936085382439026269301003761320812),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 49.459599118438883265036646019410669),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 7.77519237321893917784735690560496607),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 74.4536074488178075948642351179304121),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 2.75209340397069050436806159297952699),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 23.9292359711471667884504840186561598),
   };
   static const T Q_16_inf[] = {
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 1.0),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.357918006437579097055656138920742037),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 19.1386039850709849435325005484512944),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 0.874349081464143606016221431763364517),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 98.6516097434855572678195488061432509),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -16.1051972833382893468655223662534306),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 154.316860216253720989145047141653727),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -40.2026880424378986053105969312264534),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 60.1679136674264778074736441126810223),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, -13.3414844622256422644504472438320114),
      AUTOBOOST_MATH_BIG_CONSTANT(T, 113, 2.53795636200649908779512969030363442),
   };

   if(x <= 2)
   {
      return (2 + autoboost::math::tools::evaluate_polynomial(P_1_2, x) / tools::evaluate_polynomial(Q_1_2, x)) / (x * x);
   }
   else if(x <= 4)
   {
      return (y_offset_2_4 + autoboost::math::tools::evaluate_polynomial(P_2_4, x) / tools::evaluate_polynomial(Q_2_4, x)) / (x * x);
   }
   else if(x <= 8)
   {
      T y = 1 / x;
      return (1 + tools::evaluate_polynomial(P_4_8, y) / tools::evaluate_polynomial(Q_4_8, y)) / x;
   }
   else if(x <= 16)
   {
      T y = 1 / x;
      return (1 + tools::evaluate_polynomial(P_8_16, y) / tools::evaluate_polynomial(Q_8_16, y)) / x;
   }
   T y = 1 / x;
   return (1 + tools::evaluate_polynomial(P_16_inf, y) / tools::evaluate_polynomial(Q_16_inf, y)) / x;
}

template <class T, class Tag, class Policy>
T trigamma_imp(T x, const Tag* t, const Policy& pol)
{
   //
   // This handles reflection of negative arguments, and all our
   // error handling, then forwards to the T-specific approximation.
   //
   AUTOBOOST_MATH_STD_USING // ADL of std functions.

   T result = 0;
   //
   // Check for negative arguments and use reflection:
   //
   if(x <= 0)
   {
      // Reflect:
      T z = 1 - x;
      // Argument reduction for tan:
      if(floor(x) == x)
      {
         return policies::raise_pole_error<T>("autoboost::math::trigamma<%1%>(%1%)", 0, (1-x), pol);
      }
      T s = fabs(x) < fabs(z) ? autoboost::math::sin_pi(x, pol) : autoboost::math::sin_pi(z, pol);
      return -trigamma_imp(z, t, pol) + autoboost::math::pow<2>(constants::pi<T>()) / (s * s);
   }
   if(x < 1)
   {
      result = 1 / (x * x);
      x += 1;
   }
   return result + trigamma_prec(x, t, pol);
}

template <class T, class Policy>
T trigamma_imp(T x, const mpl::int_<0>*, const Policy& pol)
{
   return polygamma_imp(1, x, pol);
}
//
// Initializer: ensure all our constants are initialized prior to the first call of main:
//
template <class T, class Policy>
struct trigamma_initializer
{
   struct init
   {
      init()
      {
         typedef typename policies::precision<T, Policy>::type precision_type;
         do_init(mpl::bool_<precision_type::value && (precision_type::value <= 113)>());
      }
      void do_init(const mpl::true_&)
      {
         autoboost::math::trigamma(T(2.5), Policy());
      }
      void do_init(const mpl::false_&){}
      void force_instantiate()const{}
   };
   static const init initializer;
   static void force_instantiate()
   {
      initializer.force_instantiate();
   }
};

template <class T, class Policy>
const typename trigamma_initializer<T, Policy>::init trigamma_initializer<T, Policy>::initializer;

} // namespace detail

template <class T, class Policy>
inline typename tools::promote_args<T>::type
   trigamma(T x, const Policy&)
{
   typedef typename tools::promote_args<T>::type result_type;
   typedef typename policies::evaluation<result_type, Policy>::type value_type;
   typedef typename policies::precision<T, Policy>::type precision_type;
   typedef typename mpl::if_<
      mpl::or_<
         mpl::less_equal<precision_type, mpl::int_<0> >,
         mpl::greater<precision_type, mpl::int_<114> >
      >,
      mpl::int_<0>,
      typename mpl::if_<
         mpl::less<precision_type, mpl::int_<54> >,
         mpl::int_<53>,
         typename mpl::if_<
            mpl::less<precision_type, mpl::int_<65> >,
            mpl::int_<64>,
            mpl::int_<113>
         >::type
      >::type
   >::type tag_type;

   typedef typename policies::normalise<
      Policy,
      policies::promote_float<false>,
      policies::promote_double<false>,
      policies::discrete_quantile<>,
      policies::assert_undefined<> >::type forwarding_policy;

   // Force initialization of constants:
   detail::trigamma_initializer<value_type, forwarding_policy>::force_instantiate();

   return policies::checked_narrowing_cast<result_type, Policy>(detail::trigamma_imp(
      static_cast<value_type>(x),
      static_cast<const tag_type*>(0), forwarding_policy()), "autoboost::math::trigamma<%1%>(%1%)");
}

template <class T>
inline typename tools::promote_args<T>::type
   trigamma(T x)
{
   return trigamma(x, policies::policy<>());
}

} // namespace math
} // namespace autoboost
#endif

