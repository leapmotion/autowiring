# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_SIZE_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_SIZE_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_SEQ_SIZE(seq) AUTOBOOST_PP_SEQ_SIZE_I((seq))
#    define AUTOBOOST_PP_SEQ_SIZE_I(par) AUTOBOOST_PP_SEQ_SIZE_II ## par
#    define AUTOBOOST_PP_SEQ_SIZE_II(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_SIZE_, AUTOBOOST_PP_SEQ_SIZE_0 ## seq)
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG() || AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_SEQ_SIZE(seq) AUTOBOOST_PP_SEQ_SIZE_I(seq)
#    define AUTOBOOST_PP_SEQ_SIZE_I(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_SIZE_, AUTOBOOST_PP_SEQ_SIZE_0 seq)
# elif defined(__IBMC__) || defined(__IBMCPP__)
#    define AUTOBOOST_PP_SEQ_SIZE(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_SIZE_, AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_SIZE_0, seq))
# else
#    define AUTOBOOST_PP_SEQ_SIZE(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_SIZE_, AUTOBOOST_PP_SEQ_SIZE_0 seq)
# endif
#
# define AUTOBOOST_PP_SEQ_SIZE_0(_) AUTOBOOST_PP_SEQ_SIZE_1
# define AUTOBOOST_PP_SEQ_SIZE_1(_) AUTOBOOST_PP_SEQ_SIZE_2
# define AUTOBOOST_PP_SEQ_SIZE_2(_) AUTOBOOST_PP_SEQ_SIZE_3
# define AUTOBOOST_PP_SEQ_SIZE_3(_) AUTOBOOST_PP_SEQ_SIZE_4
# define AUTOBOOST_PP_SEQ_SIZE_4(_) AUTOBOOST_PP_SEQ_SIZE_5
# define AUTOBOOST_PP_SEQ_SIZE_5(_) AUTOBOOST_PP_SEQ_SIZE_6
# define AUTOBOOST_PP_SEQ_SIZE_6(_) AUTOBOOST_PP_SEQ_SIZE_7
# define AUTOBOOST_PP_SEQ_SIZE_7(_) AUTOBOOST_PP_SEQ_SIZE_8
# define AUTOBOOST_PP_SEQ_SIZE_8(_) AUTOBOOST_PP_SEQ_SIZE_9
# define AUTOBOOST_PP_SEQ_SIZE_9(_) AUTOBOOST_PP_SEQ_SIZE_10
# define AUTOBOOST_PP_SEQ_SIZE_10(_) AUTOBOOST_PP_SEQ_SIZE_11
# define AUTOBOOST_PP_SEQ_SIZE_11(_) AUTOBOOST_PP_SEQ_SIZE_12
# define AUTOBOOST_PP_SEQ_SIZE_12(_) AUTOBOOST_PP_SEQ_SIZE_13
# define AUTOBOOST_PP_SEQ_SIZE_13(_) AUTOBOOST_PP_SEQ_SIZE_14
# define AUTOBOOST_PP_SEQ_SIZE_14(_) AUTOBOOST_PP_SEQ_SIZE_15
# define AUTOBOOST_PP_SEQ_SIZE_15(_) AUTOBOOST_PP_SEQ_SIZE_16
# define AUTOBOOST_PP_SEQ_SIZE_16(_) AUTOBOOST_PP_SEQ_SIZE_17
# define AUTOBOOST_PP_SEQ_SIZE_17(_) AUTOBOOST_PP_SEQ_SIZE_18
# define AUTOBOOST_PP_SEQ_SIZE_18(_) AUTOBOOST_PP_SEQ_SIZE_19
# define AUTOBOOST_PP_SEQ_SIZE_19(_) AUTOBOOST_PP_SEQ_SIZE_20
# define AUTOBOOST_PP_SEQ_SIZE_20(_) AUTOBOOST_PP_SEQ_SIZE_21
# define AUTOBOOST_PP_SEQ_SIZE_21(_) AUTOBOOST_PP_SEQ_SIZE_22
# define AUTOBOOST_PP_SEQ_SIZE_22(_) AUTOBOOST_PP_SEQ_SIZE_23
# define AUTOBOOST_PP_SEQ_SIZE_23(_) AUTOBOOST_PP_SEQ_SIZE_24
# define AUTOBOOST_PP_SEQ_SIZE_24(_) AUTOBOOST_PP_SEQ_SIZE_25
# define AUTOBOOST_PP_SEQ_SIZE_25(_) AUTOBOOST_PP_SEQ_SIZE_26
# define AUTOBOOST_PP_SEQ_SIZE_26(_) AUTOBOOST_PP_SEQ_SIZE_27
# define AUTOBOOST_PP_SEQ_SIZE_27(_) AUTOBOOST_PP_SEQ_SIZE_28
# define AUTOBOOST_PP_SEQ_SIZE_28(_) AUTOBOOST_PP_SEQ_SIZE_29
# define AUTOBOOST_PP_SEQ_SIZE_29(_) AUTOBOOST_PP_SEQ_SIZE_30
# define AUTOBOOST_PP_SEQ_SIZE_30(_) AUTOBOOST_PP_SEQ_SIZE_31
# define AUTOBOOST_PP_SEQ_SIZE_31(_) AUTOBOOST_PP_SEQ_SIZE_32
# define AUTOBOOST_PP_SEQ_SIZE_32(_) AUTOBOOST_PP_SEQ_SIZE_33
# define AUTOBOOST_PP_SEQ_SIZE_33(_) AUTOBOOST_PP_SEQ_SIZE_34
# define AUTOBOOST_PP_SEQ_SIZE_34(_) AUTOBOOST_PP_SEQ_SIZE_35
# define AUTOBOOST_PP_SEQ_SIZE_35(_) AUTOBOOST_PP_SEQ_SIZE_36
# define AUTOBOOST_PP_SEQ_SIZE_36(_) AUTOBOOST_PP_SEQ_SIZE_37
# define AUTOBOOST_PP_SEQ_SIZE_37(_) AUTOBOOST_PP_SEQ_SIZE_38
# define AUTOBOOST_PP_SEQ_SIZE_38(_) AUTOBOOST_PP_SEQ_SIZE_39
# define AUTOBOOST_PP_SEQ_SIZE_39(_) AUTOBOOST_PP_SEQ_SIZE_40
# define AUTOBOOST_PP_SEQ_SIZE_40(_) AUTOBOOST_PP_SEQ_SIZE_41
# define AUTOBOOST_PP_SEQ_SIZE_41(_) AUTOBOOST_PP_SEQ_SIZE_42
# define AUTOBOOST_PP_SEQ_SIZE_42(_) AUTOBOOST_PP_SEQ_SIZE_43
# define AUTOBOOST_PP_SEQ_SIZE_43(_) AUTOBOOST_PP_SEQ_SIZE_44
# define AUTOBOOST_PP_SEQ_SIZE_44(_) AUTOBOOST_PP_SEQ_SIZE_45
# define AUTOBOOST_PP_SEQ_SIZE_45(_) AUTOBOOST_PP_SEQ_SIZE_46
# define AUTOBOOST_PP_SEQ_SIZE_46(_) AUTOBOOST_PP_SEQ_SIZE_47
# define AUTOBOOST_PP_SEQ_SIZE_47(_) AUTOBOOST_PP_SEQ_SIZE_48
# define AUTOBOOST_PP_SEQ_SIZE_48(_) AUTOBOOST_PP_SEQ_SIZE_49
# define AUTOBOOST_PP_SEQ_SIZE_49(_) AUTOBOOST_PP_SEQ_SIZE_50
# define AUTOBOOST_PP_SEQ_SIZE_50(_) AUTOBOOST_PP_SEQ_SIZE_51
# define AUTOBOOST_PP_SEQ_SIZE_51(_) AUTOBOOST_PP_SEQ_SIZE_52
# define AUTOBOOST_PP_SEQ_SIZE_52(_) AUTOBOOST_PP_SEQ_SIZE_53
# define AUTOBOOST_PP_SEQ_SIZE_53(_) AUTOBOOST_PP_SEQ_SIZE_54
# define AUTOBOOST_PP_SEQ_SIZE_54(_) AUTOBOOST_PP_SEQ_SIZE_55
# define AUTOBOOST_PP_SEQ_SIZE_55(_) AUTOBOOST_PP_SEQ_SIZE_56
# define AUTOBOOST_PP_SEQ_SIZE_56(_) AUTOBOOST_PP_SEQ_SIZE_57
# define AUTOBOOST_PP_SEQ_SIZE_57(_) AUTOBOOST_PP_SEQ_SIZE_58
# define AUTOBOOST_PP_SEQ_SIZE_58(_) AUTOBOOST_PP_SEQ_SIZE_59
# define AUTOBOOST_PP_SEQ_SIZE_59(_) AUTOBOOST_PP_SEQ_SIZE_60
# define AUTOBOOST_PP_SEQ_SIZE_60(_) AUTOBOOST_PP_SEQ_SIZE_61
# define AUTOBOOST_PP_SEQ_SIZE_61(_) AUTOBOOST_PP_SEQ_SIZE_62
# define AUTOBOOST_PP_SEQ_SIZE_62(_) AUTOBOOST_PP_SEQ_SIZE_63
# define AUTOBOOST_PP_SEQ_SIZE_63(_) AUTOBOOST_PP_SEQ_SIZE_64
# define AUTOBOOST_PP_SEQ_SIZE_64(_) AUTOBOOST_PP_SEQ_SIZE_65
# define AUTOBOOST_PP_SEQ_SIZE_65(_) AUTOBOOST_PP_SEQ_SIZE_66
# define AUTOBOOST_PP_SEQ_SIZE_66(_) AUTOBOOST_PP_SEQ_SIZE_67
# define AUTOBOOST_PP_SEQ_SIZE_67(_) AUTOBOOST_PP_SEQ_SIZE_68
# define AUTOBOOST_PP_SEQ_SIZE_68(_) AUTOBOOST_PP_SEQ_SIZE_69
# define AUTOBOOST_PP_SEQ_SIZE_69(_) AUTOBOOST_PP_SEQ_SIZE_70
# define AUTOBOOST_PP_SEQ_SIZE_70(_) AUTOBOOST_PP_SEQ_SIZE_71
# define AUTOBOOST_PP_SEQ_SIZE_71(_) AUTOBOOST_PP_SEQ_SIZE_72
# define AUTOBOOST_PP_SEQ_SIZE_72(_) AUTOBOOST_PP_SEQ_SIZE_73
# define AUTOBOOST_PP_SEQ_SIZE_73(_) AUTOBOOST_PP_SEQ_SIZE_74
# define AUTOBOOST_PP_SEQ_SIZE_74(_) AUTOBOOST_PP_SEQ_SIZE_75
# define AUTOBOOST_PP_SEQ_SIZE_75(_) AUTOBOOST_PP_SEQ_SIZE_76
# define AUTOBOOST_PP_SEQ_SIZE_76(_) AUTOBOOST_PP_SEQ_SIZE_77
# define AUTOBOOST_PP_SEQ_SIZE_77(_) AUTOBOOST_PP_SEQ_SIZE_78
# define AUTOBOOST_PP_SEQ_SIZE_78(_) AUTOBOOST_PP_SEQ_SIZE_79
# define AUTOBOOST_PP_SEQ_SIZE_79(_) AUTOBOOST_PP_SEQ_SIZE_80
# define AUTOBOOST_PP_SEQ_SIZE_80(_) AUTOBOOST_PP_SEQ_SIZE_81
# define AUTOBOOST_PP_SEQ_SIZE_81(_) AUTOBOOST_PP_SEQ_SIZE_82
# define AUTOBOOST_PP_SEQ_SIZE_82(_) AUTOBOOST_PP_SEQ_SIZE_83
# define AUTOBOOST_PP_SEQ_SIZE_83(_) AUTOBOOST_PP_SEQ_SIZE_84
# define AUTOBOOST_PP_SEQ_SIZE_84(_) AUTOBOOST_PP_SEQ_SIZE_85
# define AUTOBOOST_PP_SEQ_SIZE_85(_) AUTOBOOST_PP_SEQ_SIZE_86
# define AUTOBOOST_PP_SEQ_SIZE_86(_) AUTOBOOST_PP_SEQ_SIZE_87
# define AUTOBOOST_PP_SEQ_SIZE_87(_) AUTOBOOST_PP_SEQ_SIZE_88
# define AUTOBOOST_PP_SEQ_SIZE_88(_) AUTOBOOST_PP_SEQ_SIZE_89
# define AUTOBOOST_PP_SEQ_SIZE_89(_) AUTOBOOST_PP_SEQ_SIZE_90
# define AUTOBOOST_PP_SEQ_SIZE_90(_) AUTOBOOST_PP_SEQ_SIZE_91
# define AUTOBOOST_PP_SEQ_SIZE_91(_) AUTOBOOST_PP_SEQ_SIZE_92
# define AUTOBOOST_PP_SEQ_SIZE_92(_) AUTOBOOST_PP_SEQ_SIZE_93
# define AUTOBOOST_PP_SEQ_SIZE_93(_) AUTOBOOST_PP_SEQ_SIZE_94
# define AUTOBOOST_PP_SEQ_SIZE_94(_) AUTOBOOST_PP_SEQ_SIZE_95
# define AUTOBOOST_PP_SEQ_SIZE_95(_) AUTOBOOST_PP_SEQ_SIZE_96
# define AUTOBOOST_PP_SEQ_SIZE_96(_) AUTOBOOST_PP_SEQ_SIZE_97
# define AUTOBOOST_PP_SEQ_SIZE_97(_) AUTOBOOST_PP_SEQ_SIZE_98
# define AUTOBOOST_PP_SEQ_SIZE_98(_) AUTOBOOST_PP_SEQ_SIZE_99
# define AUTOBOOST_PP_SEQ_SIZE_99(_) AUTOBOOST_PP_SEQ_SIZE_100
# define AUTOBOOST_PP_SEQ_SIZE_100(_) AUTOBOOST_PP_SEQ_SIZE_101
# define AUTOBOOST_PP_SEQ_SIZE_101(_) AUTOBOOST_PP_SEQ_SIZE_102
# define AUTOBOOST_PP_SEQ_SIZE_102(_) AUTOBOOST_PP_SEQ_SIZE_103
# define AUTOBOOST_PP_SEQ_SIZE_103(_) AUTOBOOST_PP_SEQ_SIZE_104
# define AUTOBOOST_PP_SEQ_SIZE_104(_) AUTOBOOST_PP_SEQ_SIZE_105
# define AUTOBOOST_PP_SEQ_SIZE_105(_) AUTOBOOST_PP_SEQ_SIZE_106
# define AUTOBOOST_PP_SEQ_SIZE_106(_) AUTOBOOST_PP_SEQ_SIZE_107
# define AUTOBOOST_PP_SEQ_SIZE_107(_) AUTOBOOST_PP_SEQ_SIZE_108
# define AUTOBOOST_PP_SEQ_SIZE_108(_) AUTOBOOST_PP_SEQ_SIZE_109
# define AUTOBOOST_PP_SEQ_SIZE_109(_) AUTOBOOST_PP_SEQ_SIZE_110
# define AUTOBOOST_PP_SEQ_SIZE_110(_) AUTOBOOST_PP_SEQ_SIZE_111
# define AUTOBOOST_PP_SEQ_SIZE_111(_) AUTOBOOST_PP_SEQ_SIZE_112
# define AUTOBOOST_PP_SEQ_SIZE_112(_) AUTOBOOST_PP_SEQ_SIZE_113
# define AUTOBOOST_PP_SEQ_SIZE_113(_) AUTOBOOST_PP_SEQ_SIZE_114
# define AUTOBOOST_PP_SEQ_SIZE_114(_) AUTOBOOST_PP_SEQ_SIZE_115
# define AUTOBOOST_PP_SEQ_SIZE_115(_) AUTOBOOST_PP_SEQ_SIZE_116
# define AUTOBOOST_PP_SEQ_SIZE_116(_) AUTOBOOST_PP_SEQ_SIZE_117
# define AUTOBOOST_PP_SEQ_SIZE_117(_) AUTOBOOST_PP_SEQ_SIZE_118
# define AUTOBOOST_PP_SEQ_SIZE_118(_) AUTOBOOST_PP_SEQ_SIZE_119
# define AUTOBOOST_PP_SEQ_SIZE_119(_) AUTOBOOST_PP_SEQ_SIZE_120
# define AUTOBOOST_PP_SEQ_SIZE_120(_) AUTOBOOST_PP_SEQ_SIZE_121
# define AUTOBOOST_PP_SEQ_SIZE_121(_) AUTOBOOST_PP_SEQ_SIZE_122
# define AUTOBOOST_PP_SEQ_SIZE_122(_) AUTOBOOST_PP_SEQ_SIZE_123
# define AUTOBOOST_PP_SEQ_SIZE_123(_) AUTOBOOST_PP_SEQ_SIZE_124
# define AUTOBOOST_PP_SEQ_SIZE_124(_) AUTOBOOST_PP_SEQ_SIZE_125
# define AUTOBOOST_PP_SEQ_SIZE_125(_) AUTOBOOST_PP_SEQ_SIZE_126
# define AUTOBOOST_PP_SEQ_SIZE_126(_) AUTOBOOST_PP_SEQ_SIZE_127
# define AUTOBOOST_PP_SEQ_SIZE_127(_) AUTOBOOST_PP_SEQ_SIZE_128
# define AUTOBOOST_PP_SEQ_SIZE_128(_) AUTOBOOST_PP_SEQ_SIZE_129
# define AUTOBOOST_PP_SEQ_SIZE_129(_) AUTOBOOST_PP_SEQ_SIZE_130
# define AUTOBOOST_PP_SEQ_SIZE_130(_) AUTOBOOST_PP_SEQ_SIZE_131
# define AUTOBOOST_PP_SEQ_SIZE_131(_) AUTOBOOST_PP_SEQ_SIZE_132
# define AUTOBOOST_PP_SEQ_SIZE_132(_) AUTOBOOST_PP_SEQ_SIZE_133
# define AUTOBOOST_PP_SEQ_SIZE_133(_) AUTOBOOST_PP_SEQ_SIZE_134
# define AUTOBOOST_PP_SEQ_SIZE_134(_) AUTOBOOST_PP_SEQ_SIZE_135
# define AUTOBOOST_PP_SEQ_SIZE_135(_) AUTOBOOST_PP_SEQ_SIZE_136
# define AUTOBOOST_PP_SEQ_SIZE_136(_) AUTOBOOST_PP_SEQ_SIZE_137
# define AUTOBOOST_PP_SEQ_SIZE_137(_) AUTOBOOST_PP_SEQ_SIZE_138
# define AUTOBOOST_PP_SEQ_SIZE_138(_) AUTOBOOST_PP_SEQ_SIZE_139
# define AUTOBOOST_PP_SEQ_SIZE_139(_) AUTOBOOST_PP_SEQ_SIZE_140
# define AUTOBOOST_PP_SEQ_SIZE_140(_) AUTOBOOST_PP_SEQ_SIZE_141
# define AUTOBOOST_PP_SEQ_SIZE_141(_) AUTOBOOST_PP_SEQ_SIZE_142
# define AUTOBOOST_PP_SEQ_SIZE_142(_) AUTOBOOST_PP_SEQ_SIZE_143
# define AUTOBOOST_PP_SEQ_SIZE_143(_) AUTOBOOST_PP_SEQ_SIZE_144
# define AUTOBOOST_PP_SEQ_SIZE_144(_) AUTOBOOST_PP_SEQ_SIZE_145
# define AUTOBOOST_PP_SEQ_SIZE_145(_) AUTOBOOST_PP_SEQ_SIZE_146
# define AUTOBOOST_PP_SEQ_SIZE_146(_) AUTOBOOST_PP_SEQ_SIZE_147
# define AUTOBOOST_PP_SEQ_SIZE_147(_) AUTOBOOST_PP_SEQ_SIZE_148
# define AUTOBOOST_PP_SEQ_SIZE_148(_) AUTOBOOST_PP_SEQ_SIZE_149
# define AUTOBOOST_PP_SEQ_SIZE_149(_) AUTOBOOST_PP_SEQ_SIZE_150
# define AUTOBOOST_PP_SEQ_SIZE_150(_) AUTOBOOST_PP_SEQ_SIZE_151
# define AUTOBOOST_PP_SEQ_SIZE_151(_) AUTOBOOST_PP_SEQ_SIZE_152
# define AUTOBOOST_PP_SEQ_SIZE_152(_) AUTOBOOST_PP_SEQ_SIZE_153
# define AUTOBOOST_PP_SEQ_SIZE_153(_) AUTOBOOST_PP_SEQ_SIZE_154
# define AUTOBOOST_PP_SEQ_SIZE_154(_) AUTOBOOST_PP_SEQ_SIZE_155
# define AUTOBOOST_PP_SEQ_SIZE_155(_) AUTOBOOST_PP_SEQ_SIZE_156
# define AUTOBOOST_PP_SEQ_SIZE_156(_) AUTOBOOST_PP_SEQ_SIZE_157
# define AUTOBOOST_PP_SEQ_SIZE_157(_) AUTOBOOST_PP_SEQ_SIZE_158
# define AUTOBOOST_PP_SEQ_SIZE_158(_) AUTOBOOST_PP_SEQ_SIZE_159
# define AUTOBOOST_PP_SEQ_SIZE_159(_) AUTOBOOST_PP_SEQ_SIZE_160
# define AUTOBOOST_PP_SEQ_SIZE_160(_) AUTOBOOST_PP_SEQ_SIZE_161
# define AUTOBOOST_PP_SEQ_SIZE_161(_) AUTOBOOST_PP_SEQ_SIZE_162
# define AUTOBOOST_PP_SEQ_SIZE_162(_) AUTOBOOST_PP_SEQ_SIZE_163
# define AUTOBOOST_PP_SEQ_SIZE_163(_) AUTOBOOST_PP_SEQ_SIZE_164
# define AUTOBOOST_PP_SEQ_SIZE_164(_) AUTOBOOST_PP_SEQ_SIZE_165
# define AUTOBOOST_PP_SEQ_SIZE_165(_) AUTOBOOST_PP_SEQ_SIZE_166
# define AUTOBOOST_PP_SEQ_SIZE_166(_) AUTOBOOST_PP_SEQ_SIZE_167
# define AUTOBOOST_PP_SEQ_SIZE_167(_) AUTOBOOST_PP_SEQ_SIZE_168
# define AUTOBOOST_PP_SEQ_SIZE_168(_) AUTOBOOST_PP_SEQ_SIZE_169
# define AUTOBOOST_PP_SEQ_SIZE_169(_) AUTOBOOST_PP_SEQ_SIZE_170
# define AUTOBOOST_PP_SEQ_SIZE_170(_) AUTOBOOST_PP_SEQ_SIZE_171
# define AUTOBOOST_PP_SEQ_SIZE_171(_) AUTOBOOST_PP_SEQ_SIZE_172
# define AUTOBOOST_PP_SEQ_SIZE_172(_) AUTOBOOST_PP_SEQ_SIZE_173
# define AUTOBOOST_PP_SEQ_SIZE_173(_) AUTOBOOST_PP_SEQ_SIZE_174
# define AUTOBOOST_PP_SEQ_SIZE_174(_) AUTOBOOST_PP_SEQ_SIZE_175
# define AUTOBOOST_PP_SEQ_SIZE_175(_) AUTOBOOST_PP_SEQ_SIZE_176
# define AUTOBOOST_PP_SEQ_SIZE_176(_) AUTOBOOST_PP_SEQ_SIZE_177
# define AUTOBOOST_PP_SEQ_SIZE_177(_) AUTOBOOST_PP_SEQ_SIZE_178
# define AUTOBOOST_PP_SEQ_SIZE_178(_) AUTOBOOST_PP_SEQ_SIZE_179
# define AUTOBOOST_PP_SEQ_SIZE_179(_) AUTOBOOST_PP_SEQ_SIZE_180
# define AUTOBOOST_PP_SEQ_SIZE_180(_) AUTOBOOST_PP_SEQ_SIZE_181
# define AUTOBOOST_PP_SEQ_SIZE_181(_) AUTOBOOST_PP_SEQ_SIZE_182
# define AUTOBOOST_PP_SEQ_SIZE_182(_) AUTOBOOST_PP_SEQ_SIZE_183
# define AUTOBOOST_PP_SEQ_SIZE_183(_) AUTOBOOST_PP_SEQ_SIZE_184
# define AUTOBOOST_PP_SEQ_SIZE_184(_) AUTOBOOST_PP_SEQ_SIZE_185
# define AUTOBOOST_PP_SEQ_SIZE_185(_) AUTOBOOST_PP_SEQ_SIZE_186
# define AUTOBOOST_PP_SEQ_SIZE_186(_) AUTOBOOST_PP_SEQ_SIZE_187
# define AUTOBOOST_PP_SEQ_SIZE_187(_) AUTOBOOST_PP_SEQ_SIZE_188
# define AUTOBOOST_PP_SEQ_SIZE_188(_) AUTOBOOST_PP_SEQ_SIZE_189
# define AUTOBOOST_PP_SEQ_SIZE_189(_) AUTOBOOST_PP_SEQ_SIZE_190
# define AUTOBOOST_PP_SEQ_SIZE_190(_) AUTOBOOST_PP_SEQ_SIZE_191
# define AUTOBOOST_PP_SEQ_SIZE_191(_) AUTOBOOST_PP_SEQ_SIZE_192
# define AUTOBOOST_PP_SEQ_SIZE_192(_) AUTOBOOST_PP_SEQ_SIZE_193
# define AUTOBOOST_PP_SEQ_SIZE_193(_) AUTOBOOST_PP_SEQ_SIZE_194
# define AUTOBOOST_PP_SEQ_SIZE_194(_) AUTOBOOST_PP_SEQ_SIZE_195
# define AUTOBOOST_PP_SEQ_SIZE_195(_) AUTOBOOST_PP_SEQ_SIZE_196
# define AUTOBOOST_PP_SEQ_SIZE_196(_) AUTOBOOST_PP_SEQ_SIZE_197
# define AUTOBOOST_PP_SEQ_SIZE_197(_) AUTOBOOST_PP_SEQ_SIZE_198
# define AUTOBOOST_PP_SEQ_SIZE_198(_) AUTOBOOST_PP_SEQ_SIZE_199
# define AUTOBOOST_PP_SEQ_SIZE_199(_) AUTOBOOST_PP_SEQ_SIZE_200
# define AUTOBOOST_PP_SEQ_SIZE_200(_) AUTOBOOST_PP_SEQ_SIZE_201
# define AUTOBOOST_PP_SEQ_SIZE_201(_) AUTOBOOST_PP_SEQ_SIZE_202
# define AUTOBOOST_PP_SEQ_SIZE_202(_) AUTOBOOST_PP_SEQ_SIZE_203
# define AUTOBOOST_PP_SEQ_SIZE_203(_) AUTOBOOST_PP_SEQ_SIZE_204
# define AUTOBOOST_PP_SEQ_SIZE_204(_) AUTOBOOST_PP_SEQ_SIZE_205
# define AUTOBOOST_PP_SEQ_SIZE_205(_) AUTOBOOST_PP_SEQ_SIZE_206
# define AUTOBOOST_PP_SEQ_SIZE_206(_) AUTOBOOST_PP_SEQ_SIZE_207
# define AUTOBOOST_PP_SEQ_SIZE_207(_) AUTOBOOST_PP_SEQ_SIZE_208
# define AUTOBOOST_PP_SEQ_SIZE_208(_) AUTOBOOST_PP_SEQ_SIZE_209
# define AUTOBOOST_PP_SEQ_SIZE_209(_) AUTOBOOST_PP_SEQ_SIZE_210
# define AUTOBOOST_PP_SEQ_SIZE_210(_) AUTOBOOST_PP_SEQ_SIZE_211
# define AUTOBOOST_PP_SEQ_SIZE_211(_) AUTOBOOST_PP_SEQ_SIZE_212
# define AUTOBOOST_PP_SEQ_SIZE_212(_) AUTOBOOST_PP_SEQ_SIZE_213
# define AUTOBOOST_PP_SEQ_SIZE_213(_) AUTOBOOST_PP_SEQ_SIZE_214
# define AUTOBOOST_PP_SEQ_SIZE_214(_) AUTOBOOST_PP_SEQ_SIZE_215
# define AUTOBOOST_PP_SEQ_SIZE_215(_) AUTOBOOST_PP_SEQ_SIZE_216
# define AUTOBOOST_PP_SEQ_SIZE_216(_) AUTOBOOST_PP_SEQ_SIZE_217
# define AUTOBOOST_PP_SEQ_SIZE_217(_) AUTOBOOST_PP_SEQ_SIZE_218
# define AUTOBOOST_PP_SEQ_SIZE_218(_) AUTOBOOST_PP_SEQ_SIZE_219
# define AUTOBOOST_PP_SEQ_SIZE_219(_) AUTOBOOST_PP_SEQ_SIZE_220
# define AUTOBOOST_PP_SEQ_SIZE_220(_) AUTOBOOST_PP_SEQ_SIZE_221
# define AUTOBOOST_PP_SEQ_SIZE_221(_) AUTOBOOST_PP_SEQ_SIZE_222
# define AUTOBOOST_PP_SEQ_SIZE_222(_) AUTOBOOST_PP_SEQ_SIZE_223
# define AUTOBOOST_PP_SEQ_SIZE_223(_) AUTOBOOST_PP_SEQ_SIZE_224
# define AUTOBOOST_PP_SEQ_SIZE_224(_) AUTOBOOST_PP_SEQ_SIZE_225
# define AUTOBOOST_PP_SEQ_SIZE_225(_) AUTOBOOST_PP_SEQ_SIZE_226
# define AUTOBOOST_PP_SEQ_SIZE_226(_) AUTOBOOST_PP_SEQ_SIZE_227
# define AUTOBOOST_PP_SEQ_SIZE_227(_) AUTOBOOST_PP_SEQ_SIZE_228
# define AUTOBOOST_PP_SEQ_SIZE_228(_) AUTOBOOST_PP_SEQ_SIZE_229
# define AUTOBOOST_PP_SEQ_SIZE_229(_) AUTOBOOST_PP_SEQ_SIZE_230
# define AUTOBOOST_PP_SEQ_SIZE_230(_) AUTOBOOST_PP_SEQ_SIZE_231
# define AUTOBOOST_PP_SEQ_SIZE_231(_) AUTOBOOST_PP_SEQ_SIZE_232
# define AUTOBOOST_PP_SEQ_SIZE_232(_) AUTOBOOST_PP_SEQ_SIZE_233
# define AUTOBOOST_PP_SEQ_SIZE_233(_) AUTOBOOST_PP_SEQ_SIZE_234
# define AUTOBOOST_PP_SEQ_SIZE_234(_) AUTOBOOST_PP_SEQ_SIZE_235
# define AUTOBOOST_PP_SEQ_SIZE_235(_) AUTOBOOST_PP_SEQ_SIZE_236
# define AUTOBOOST_PP_SEQ_SIZE_236(_) AUTOBOOST_PP_SEQ_SIZE_237
# define AUTOBOOST_PP_SEQ_SIZE_237(_) AUTOBOOST_PP_SEQ_SIZE_238
# define AUTOBOOST_PP_SEQ_SIZE_238(_) AUTOBOOST_PP_SEQ_SIZE_239
# define AUTOBOOST_PP_SEQ_SIZE_239(_) AUTOBOOST_PP_SEQ_SIZE_240
# define AUTOBOOST_PP_SEQ_SIZE_240(_) AUTOBOOST_PP_SEQ_SIZE_241
# define AUTOBOOST_PP_SEQ_SIZE_241(_) AUTOBOOST_PP_SEQ_SIZE_242
# define AUTOBOOST_PP_SEQ_SIZE_242(_) AUTOBOOST_PP_SEQ_SIZE_243
# define AUTOBOOST_PP_SEQ_SIZE_243(_) AUTOBOOST_PP_SEQ_SIZE_244
# define AUTOBOOST_PP_SEQ_SIZE_244(_) AUTOBOOST_PP_SEQ_SIZE_245
# define AUTOBOOST_PP_SEQ_SIZE_245(_) AUTOBOOST_PP_SEQ_SIZE_246
# define AUTOBOOST_PP_SEQ_SIZE_246(_) AUTOBOOST_PP_SEQ_SIZE_247
# define AUTOBOOST_PP_SEQ_SIZE_247(_) AUTOBOOST_PP_SEQ_SIZE_248
# define AUTOBOOST_PP_SEQ_SIZE_248(_) AUTOBOOST_PP_SEQ_SIZE_249
# define AUTOBOOST_PP_SEQ_SIZE_249(_) AUTOBOOST_PP_SEQ_SIZE_250
# define AUTOBOOST_PP_SEQ_SIZE_250(_) AUTOBOOST_PP_SEQ_SIZE_251
# define AUTOBOOST_PP_SEQ_SIZE_251(_) AUTOBOOST_PP_SEQ_SIZE_252
# define AUTOBOOST_PP_SEQ_SIZE_252(_) AUTOBOOST_PP_SEQ_SIZE_253
# define AUTOBOOST_PP_SEQ_SIZE_253(_) AUTOBOOST_PP_SEQ_SIZE_254
# define AUTOBOOST_PP_SEQ_SIZE_254(_) AUTOBOOST_PP_SEQ_SIZE_255
# define AUTOBOOST_PP_SEQ_SIZE_255(_) AUTOBOOST_PP_SEQ_SIZE_256
# define AUTOBOOST_PP_SEQ_SIZE_256(_) AUTOBOOST_PP_SEQ_SIZE_257
#
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_0 0
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_1 1
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_2 2
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_3 3
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_4 4
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_5 5
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_6 6
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_7 7
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_8 8
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_9 9
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_10 10
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_11 11
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_12 12
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_13 13
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_14 14
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_15 15
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_16 16
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_17 17
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_18 18
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_19 19
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_20 20
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_21 21
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_22 22
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_23 23
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_24 24
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_25 25
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_26 26
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_27 27
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_28 28
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_29 29
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_30 30
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_31 31
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_32 32
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_33 33
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_34 34
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_35 35
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_36 36
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_37 37
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_38 38
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_39 39
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_40 40
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_41 41
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_42 42
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_43 43
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_44 44
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_45 45
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_46 46
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_47 47
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_48 48
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_49 49
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_50 50
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_51 51
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_52 52
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_53 53
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_54 54
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_55 55
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_56 56
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_57 57
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_58 58
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_59 59
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_60 60
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_61 61
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_62 62
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_63 63
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_64 64
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_65 65
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_66 66
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_67 67
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_68 68
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_69 69
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_70 70
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_71 71
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_72 72
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_73 73
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_74 74
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_75 75
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_76 76
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_77 77
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_78 78
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_79 79
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_80 80
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_81 81
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_82 82
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_83 83
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_84 84
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_85 85
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_86 86
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_87 87
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_88 88
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_89 89
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_90 90
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_91 91
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_92 92
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_93 93
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_94 94
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_95 95
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_96 96
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_97 97
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_98 98
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_99 99
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_100 100
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_101 101
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_102 102
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_103 103
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_104 104
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_105 105
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_106 106
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_107 107
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_108 108
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_109 109
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_110 110
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_111 111
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_112 112
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_113 113
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_114 114
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_115 115
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_116 116
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_117 117
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_118 118
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_119 119
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_120 120
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_121 121
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_122 122
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_123 123
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_124 124
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_125 125
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_126 126
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_127 127
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_128 128
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_129 129
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_130 130
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_131 131
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_132 132
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_133 133
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_134 134
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_135 135
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_136 136
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_137 137
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_138 138
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_139 139
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_140 140
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_141 141
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_142 142
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_143 143
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_144 144
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_145 145
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_146 146
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_147 147
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_148 148
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_149 149
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_150 150
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_151 151
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_152 152
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_153 153
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_154 154
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_155 155
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_156 156
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_157 157
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_158 158
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_159 159
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_160 160
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_161 161
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_162 162
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_163 163
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_164 164
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_165 165
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_166 166
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_167 167
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_168 168
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_169 169
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_170 170
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_171 171
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_172 172
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_173 173
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_174 174
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_175 175
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_176 176
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_177 177
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_178 178
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_179 179
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_180 180
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_181 181
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_182 182
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_183 183
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_184 184
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_185 185
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_186 186
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_187 187
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_188 188
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_189 189
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_190 190
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_191 191
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_192 192
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_193 193
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_194 194
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_195 195
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_196 196
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_197 197
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_198 198
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_199 199
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_200 200
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_201 201
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_202 202
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_203 203
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_204 204
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_205 205
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_206 206
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_207 207
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_208 208
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_209 209
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_210 210
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_211 211
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_212 212
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_213 213
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_214 214
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_215 215
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_216 216
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_217 217
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_218 218
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_219 219
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_220 220
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_221 221
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_222 222
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_223 223
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_224 224
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_225 225
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_226 226
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_227 227
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_228 228
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_229 229
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_230 230
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_231 231
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_232 232
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_233 233
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_234 234
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_235 235
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_236 236
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_237 237
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_238 238
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_239 239
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_240 240
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_241 241
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_242 242
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_243 243
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_244 244
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_245 245
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_246 246
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_247 247
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_248 248
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_249 249
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_250 250
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_251 251
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_252 252
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_253 253
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_254 254
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_255 255
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_256 256
# define AUTOBOOST_PP_SEQ_SIZE_AUTOBOOST_PP_SEQ_SIZE_257 257
#
# endif
