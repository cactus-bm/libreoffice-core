/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: isolang.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: ihi $ $Date: 2006-12-19 18:04:41 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_i18npool.hxx"

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif

#include "i18npool/mslangid.hxx"

// =======================================================================

struct IsoLangEngEntry
{
    LanguageType        mnLang;
    sal_Char            maCountry[3];
};

struct IsoLangNoneStdEntry
{
    LanguageType        mnLang;
    sal_Char            maLangStr[4];
    sal_Char            maCountry[9];
};

struct IsoLangOtherEntry
{
    LanguageType        mnLang;
    const sal_Char*     mpLangStr;
};

// -----------------------------------------------------------------------

// Entries for languages are lower case, for countries upper case,
// as recommended by rfc3066 (obsoletes rfc1766).
// convertIsoNamesToLanguage is case insensitive

// Sortorder: Most used first
// The default entry for every language string has to be first.

static MsLangId::IsoLangEntry const aImplIsoLangEntries[] =
{
    // MS-LANGID codes               ISO639-1/2/3 ISO3166
    { LANGUAGE_ENGLISH,                     "en", ""   },
    { LANGUAGE_ENGLISH_US,                  "en", "US" },
    { LANGUAGE_ENGLISH_UK,                  "en", "GB" },
    { LANGUAGE_ENGLISH_AUS,                 "en", "AU" },
    { LANGUAGE_ENGLISH_CAN,                 "en", "CA" },
    { LANGUAGE_FRENCH,                      "fr", "FR" },
    { LANGUAGE_FRENCH,                      "fr", ""   },
    { LANGUAGE_GERMAN,                      "de", "DE" },
    { LANGUAGE_ITALIAN,                     "it", "IT" },
    { LANGUAGE_DUTCH,                       "nl", "NL" },
    { LANGUAGE_SPANISH,                     "es", "ES" },
    { LANGUAGE_SPANISH,                     "es", ""   },
    { LANGUAGE_SPANISH_MODERN,              "es", "ES" },
    { LANGUAGE_PORTUGUESE,                  "pt", "PT" },
    { LANGUAGE_PORTUGUESE_BRAZILIAN,        "pt", "BR" },
    { LANGUAGE_DANISH,                      "da", "DK" },
    { LANGUAGE_GREEK,                       "el", "GR" },
    { LANGUAGE_CHINESE,                     "zh", ""   },
    { LANGUAGE_CHINESE_TRADITIONAL,         "zh", "TW" },
    { LANGUAGE_CHINESE_SIMPLIFIED,          "zh", "CN" },
    { LANGUAGE_CHINESE_HONGKONG,            "zh", "HK" },
    { LANGUAGE_CHINESE_SINGAPORE,           "zh", "SG" },
    { LANGUAGE_CHINESE_MACAU,               "zh", "MO" },
    { LANGUAGE_ENGLISH_HONG_KONG_SAR,       "en", "HK" },
    { LANGUAGE_JAPANESE,                    "ja", "JP" },
    { LANGUAGE_KOREAN,                      "ko", "KR" },
    { LANGUAGE_KOREAN_JOHAB,                "ko", "KR" },
    { LANGUAGE_USER_KOREAN_NORTH,           "ko", "KP" },
    { LANGUAGE_SWEDISH,                     "sv", "SE" },
    { LANGUAGE_SWEDISH_FINLAND,             "sv", "FI" },
    { LANGUAGE_FINNISH,                     "fi", "FI" },
    { LANGUAGE_RUSSIAN,                     "ru", "RU" },
    { LANGUAGE_TATAR,                       "tt", "RU" },
    { LANGUAGE_ENGLISH_NZ,                  "en", "NZ" },
    { LANGUAGE_ENGLISH_EIRE,                "en", "IE" },
    { LANGUAGE_DUTCH_BELGIAN,               "nl", "BE" },
    { LANGUAGE_FRENCH_BELGIAN,              "fr", "BE" },
    { LANGUAGE_FRENCH_CANADIAN,             "fr", "CA" },
    { LANGUAGE_FRENCH_SWISS,                "fr", "CH" },
    { LANGUAGE_GERMAN_SWISS,                "de", "CH" },
    { LANGUAGE_GERMAN_AUSTRIAN,             "de", "AT" },
    { LANGUAGE_ITALIAN_SWISS,               "it", "CH" },
    { LANGUAGE_ALBANIAN,                    "sq", "AL" },
    { LANGUAGE_ARABIC,                      "ar", ""   },
    { LANGUAGE_ARABIC_SAUDI_ARABIA,         "ar", "SA" },
    { LANGUAGE_ARABIC_EGYPT,                "ar", "EG" },
    { LANGUAGE_ARABIC_UAE,                  "ar", "AE" },
    { LANGUAGE_ARABIC_IRAQ,                 "ar", "IQ" },
    { LANGUAGE_ARABIC_LIBYA,                "ar", "LY" },
    { LANGUAGE_ARABIC_ALGERIA,              "ar", "DZ" },
    { LANGUAGE_ARABIC_MOROCCO,              "ar", "MA" },
    { LANGUAGE_ARABIC_TUNISIA,              "ar", "TN" },
    { LANGUAGE_ARABIC_OMAN,                 "ar", "OM" },
    { LANGUAGE_ARABIC_YEMEN,                "ar", "YE" },
    { LANGUAGE_ARABIC_SYRIA,                "ar", "SY" },
    { LANGUAGE_ARABIC_JORDAN,               "ar", "JO" },
    { LANGUAGE_ARABIC_LEBANON,              "ar", "LB" },
    { LANGUAGE_ARABIC_KUWAIT,               "ar", "KW" },
    { LANGUAGE_ARABIC_BAHRAIN,              "ar", "BH" },
    { LANGUAGE_ARABIC_QATAR,                "ar", "QA" },
    { LANGUAGE_BASQUE,                      "eu", ""   },
    { LANGUAGE_BULGARIAN,                   "bg", "BG" },
    { LANGUAGE_CZECH,                       "cs", "CZ" },
    { LANGUAGE_CZECH,                       "cz", ""   },
    { LANGUAGE_ENGLISH_JAMAICA,             "en", "JM" },
    { LANGUAGE_ENGLISH_CARRIBEAN,           "en", "BS" },   // not 100%, because AG is Bahamas
    { LANGUAGE_ENGLISH_BELIZE,              "en", "BZ" },
    { LANGUAGE_ENGLISH_TRINIDAD,            "en", "TT" },
    { LANGUAGE_ENGLISH_ZIMBABWE,            "en", "ZW" },
    { LANGUAGE_ENGLISH_PHILIPPINES,         "en", "PH" },
    { LANGUAGE_ENGLISH_INDONESIA,           "en", "ID" },
    { LANGUAGE_ESTONIAN,                    "et", "EE" },
    { LANGUAGE_FAEROESE,                    "fo", "FO" },
    { LANGUAGE_FARSI,                       "fa", "IR" },
    { LANGUAGE_FRENCH_LUXEMBOURG,           "fr", "LU" },
    { LANGUAGE_FRENCH_MONACO,               "fr", "MC" },
    { LANGUAGE_GERMAN_LUXEMBOURG,           "de", "LU" },
    { LANGUAGE_GERMAN_LIECHTENSTEIN,        "de", "LI" },
    { LANGUAGE_HEBREW,                      "he", "IL" },   // new: old was "iw"
    { LANGUAGE_HEBREW,                      "iw", "IL" },   // old: new is "he"
    { LANGUAGE_HUNGARIAN,                   "hu", "HU" },
    { LANGUAGE_ICELANDIC,                   "is", "IS" },
    { LANGUAGE_INDONESIAN,                  "id", "ID" },   // new: old was "in"
    { LANGUAGE_INDONESIAN,                  "in", "ID" },   // old: new is "id"
    { LANGUAGE_NORWEGIAN,                   "no", "NO" },
    { LANGUAGE_NORWEGIAN_BOKMAL,            "nb", "NO" },
    { LANGUAGE_NORWEGIAN_NYNORSK,           "nn", "NO" },
    { LANGUAGE_POLISH,                      "pl", "PL" },
    { LANGUAGE_RHAETO_ROMAN,                "rm", ""   },
    { LANGUAGE_ROMANIAN,                    "ro", "RO" },
    { LANGUAGE_ROMANIAN_MOLDOVA,            "ro", "MD" },
    { LANGUAGE_SLOVAK,                      "sk", "SK" },
    { LANGUAGE_SLOVENIAN,                   "sl", "SI" },
    { LANGUAGE_SPANISH_MEXICAN,             "es", "MX" },
    { LANGUAGE_SPANISH_GUATEMALA,           "es", "GT" },
    { LANGUAGE_SPANISH_COSTARICA,           "es", "CR" },
    { LANGUAGE_SPANISH_PANAMA,              "es", "PA" },
    { LANGUAGE_SPANISH_DOMINICAN_REPUBLIC,  "es", "DO" },
    { LANGUAGE_SPANISH_VENEZUELA,           "es", "VE" },
    { LANGUAGE_SPANISH_COLOMBIA,            "es", "CO" },
    { LANGUAGE_SPANISH_PERU,                "es", "PE" },
    { LANGUAGE_SPANISH_ARGENTINA,           "es", "AR" },
    { LANGUAGE_SPANISH_ECUADOR,             "es", "EC" },
    { LANGUAGE_SPANISH_CHILE,               "es", "CL" },
    { LANGUAGE_SPANISH_URUGUAY,             "es", "UY" },
    { LANGUAGE_SPANISH_PARAGUAY,            "es", "PY" },
    { LANGUAGE_SPANISH_BOLIVIA,             "es", "BO" },
    { LANGUAGE_SPANISH_EL_SALVADOR,         "es", "SV" },
    { LANGUAGE_SPANISH_HONDURAS,            "es", "HN" },
    { LANGUAGE_SPANISH_NICARAGUA,           "es", "NI" },
    { LANGUAGE_SPANISH_PUERTO_RICO,         "es", "PR" },
    { LANGUAGE_SPANISH_UNITED_STATES,       "es", "US" },
    { LANGUAGE_SPANISH_LATIN_AMERICA,       "es", ""   },
    { LANGUAGE_TURKISH,                     "tr", "TR" },
    { LANGUAGE_UKRAINIAN,                   "uk", "UA" },
    { LANGUAGE_VIETNAMESE,                  "vi", "VN" },
    { LANGUAGE_LATVIAN,                     "lv", "LV" },
    { LANGUAGE_MACEDONIAN,                  "mk", "MK" },
    { LANGUAGE_MALAY,                       "ms", ""   },
    { LANGUAGE_MALAY_MALAYSIA,              "ms", "MY" },
    { LANGUAGE_MALAY_BRUNEI_DARUSSALAM,     "ms", "BN" },
    { LANGUAGE_ENGLISH_MALAYSIA,            "en", "MY" },
    { LANGUAGE_THAI,                        "th", "TH" },
    { LANGUAGE_LITHUANIAN,                  "lt", "LT" },
    { LANGUAGE_LITHUANIAN_CLASSIC,          "lt", "LT" },
    { LANGUAGE_CROATIAN,                    "hr", "HR" },   // Croatian in Croatia
    { LANGUAGE_CROATIAN_BOSNIA_HERZEGOVINA, "hr", "BA" },
    { LANGUAGE_BOSNIAN_LATIN_BOSNIA_HERZEGOVINA,        "bs", "BA" },
//  { LANGUAGE_BOSNIAN_CYRILLIC_BOSNIA_AND_HERZEGOVINA, "bs", "BA" },   // script codes not supported yet
    { LANGUAGE_SERBIAN_LATIN,               "sh", "YU" },   // Serbian Latin in Serbia and Montenegro (former Yugoslavia); kludge, needed to be sr_Latn_CS instead, script codes not supported yet
    { LANGUAGE_SERBIAN_LATIN_BOSNIA_HERZEGOVINA, "sh", "BA" },
    { LANGUAGE_SERBIAN_CYRILLIC,            "sr", "YU" },   // Serbian Cyrillic in Serbia and Montenegro (former Yugoslavia); kludge, needed to be sr_CS instead, sr_CS not supported by ICU 2.6 (3.4 does)
    { LANGUAGE_SERBIAN_CYRILLIC,            "sr", "CS" },   // alias to be able to integrate localizations, rsc needs it
    { LANGUAGE_SERBIAN_CYRILLIC_BOSNIA_HERZEGOVINA, "sr", "BA" },
    { LANGUAGE_SERBIAN,                     "sr", ""   },   // SERBIAN is only LID, MS-LCID not defined (was dupe of CROATIAN)
    { LANGUAGE_ARMENIAN,                    "hy", "AM" },
    { LANGUAGE_AZERI,                       "az", ""   },
    { LANGUAGE_AZERI_LATIN,                 "az", "AZ" },
//  { LANGUAGE_AZERI_CYRILLIC,              "az", "AZ" },   // script codes not supported yet
    { LANGUAGE_UZBEK,                       "uz", ""   },
    { LANGUAGE_UZBEK_LATIN,                 "uz", "UZ" },
//  { LANGUAGE_UZBEK_CYRILLIC,              "uz", "UZ" },   // script codes not supported yet
    { LANGUAGE_BENGALI_BANGLADESH,          "bn", "BD" },
    { LANGUAGE_BENGALI,                     "bn", "IN" },
    { LANGUAGE_BURMESE,                     "my", "MM" },
    { LANGUAGE_KAZAK,                       "kk", "KZ" },
    { LANGUAGE_ENGLISH_INDIA,               "en", "IN" },
    { LANGUAGE_URDU,                        "ur", ""   },
    { LANGUAGE_URDU_INDIA,                  "ur", "IN" },
    { LANGUAGE_URDU_PAKISTAN,               "ur", "PK" },
    { LANGUAGE_HINDI,                       "hi", "IN" },
    { LANGUAGE_GUJARATI,                    "gu", "IN" },
    { LANGUAGE_KANNADA,                     "kn", "IN" },
    { LANGUAGE_ASSAMESE,                    "as", "IN" },
    { LANGUAGE_KASHMIRI,                    "ks", ""   },
    { LANGUAGE_KASHMIRI_INDIA,              "ks", "IN" },
    { LANGUAGE_MALAYALAM,                   "ml", "IN" },
    { LANGUAGE_MANIPURI,                   "mni", "IN" },
    { LANGUAGE_MARATHI,                     "mr", "IN" },
    { LANGUAGE_KONKANI,                    "kok", "IN" },
    { LANGUAGE_NEPALI,                      "ne", "NP" },
    { LANGUAGE_NEPALI_INDIA,                "ne", "IN" },
    { LANGUAGE_ORIYA,                       "or", "IN" },
    { LANGUAGE_PUNJABI,                     "pa", "IN" },
    { LANGUAGE_SANSKRIT,                    "sa", "IN" },
    { LANGUAGE_SINDHI,                      "sd", "IN" },
    { LANGUAGE_TAMIL,                       "ta", "IN" },
    { LANGUAGE_TELUGU,                      "te", "IN" },
    { LANGUAGE_PUNJABI_PAKISTAN,           "lah", "PK" },   // preferring "lah" over "pa" for Western Punjabi, see http://www.ethnologue.com/show_language.asp?code=PNB
    { LANGUAGE_PUNJABI_PAKISTAN,            "pa", "PK" },
    { LANGUAGE_SINDHI_PAKISTAN,             "sd", "PK" },
    { LANGUAGE_BELARUSIAN,                  "be", "BY" },
    { LANGUAGE_CATALAN,                     "ca", "ES" },   // Spain (default)
    { LANGUAGE_CATALAN,                     "ca", "AD" },   // Andorra
    { LANGUAGE_USER_CATALAN_VALENCIAN,      "ca", "XV" },   // XV: ISO 3166 user-assigned; workaround for UI localization only, do not use in document content!
    { LANGUAGE_FRENCH_CAMEROON,             "fr", "CM" },
    { LANGUAGE_FRENCH_COTE_D_IVOIRE,        "fr", "CI" },
    { LANGUAGE_FRENCH_HAITI,                "fr", "HT" },
    { LANGUAGE_FRENCH_MALI,                 "fr", "ML" },
    { LANGUAGE_FRENCH_SENEGAL,              "fr", "SN" },
    { LANGUAGE_FRENCH_ZAIRE,                "fr", "CD" },   // Democratic Republic Of Congo
    { LANGUAGE_FRENCH_MOROCCO,              "fr", "MA" },
    { LANGUAGE_FRENCH_REUNION,              "fr", "RE" },
    { LANGUAGE_FRENCH_NORTH_AFRICA,         "fr", ""   },
    { LANGUAGE_FRENCH_WEST_INDIES,          "fr", ""   },   // unknown ISO country code
    { LANGUAGE_FRISIAN_NETHERLANDS,         "fy", "NL" },
    { LANGUAGE_GAELIC_IRELAND,              "ga", "IE" },
    { LANGUAGE_GAELIC_SCOTLAND,             "gd", "GB" },
    { LANGUAGE_GALICIAN,                    "gl", "ES" },
    { LANGUAGE_GEORGIAN,                    "ka", "GE" },
    { LANGUAGE_KHMER,                       "km", "KH" },
    { LANGUAGE_KIRGHIZ,                     "ky", "KG" },
    { LANGUAGE_LAO,                         "lo", "LA" },
    { LANGUAGE_MALTESE,                     "mt", "MT" },
    { LANGUAGE_MONGOLIAN,                   "mn", "MN" },   // Cyrillic script
    { LANGUAGE_MONGOLIAN_MONGOLIAN,         "mn", "MN" },
    { LANGUAGE_RUSSIAN_MOLDOVA,             "mo", "MD" },
    { LANGUAGE_SWAHILI,                     "sw", "KE" },
    { LANGUAGE_USER_SWAHILI_TANZANIA,       "sw", "TZ" },
    { LANGUAGE_TAJIK,                       "tg", "TJ" },
    { LANGUAGE_TIBETAN,                     "bo", "CN" },   // CN politically correct?
    { LANGUAGE_DZONGKHA,                    "dz", "BT" },
    { LANGUAGE_TURKMEN,                     "tk", "TM" },
    { LANGUAGE_WELSH,                       "cy", "GB" },
    { LANGUAGE_SESOTHO,                     "st", "ZA" },
    { LANGUAGE_SEPEDI,                     "nso", "ZA" },
    { LANGUAGE_SEPEDI,                      "ns", "ZA" },   // fake "ns" for compatibility with existing OOo1.1.x localization to be able to read those documents
    { LANGUAGE_TSONGA,                      "ts", "ZA" },
    { LANGUAGE_TSWANA,                      "tn", "ZA" },
    { LANGUAGE_ENGLISH_SAFRICA,             "en", "ZA" },
    { LANGUAGE_AFRIKAANS,                   "af", "ZA" },
    { LANGUAGE_VENDA,                       "ve", "ZA" },   // default 639-1
    { LANGUAGE_VENDA,                      "ven", "ZA" },   // 639-2 may have been used temporarily since 2004-07-23
    { LANGUAGE_XHOSA,                       "xh", "ZA" },
    { LANGUAGE_ZULU,                        "zu", "ZA" },
    { LANGUAGE_QUECHUA_BOLIVIA,             "qu", "BO" },
    { LANGUAGE_QUECHUA_ECUADOR,             "qu", "EC" },
    { LANGUAGE_QUECHUA_PERU,                "qu", "PE" },
    { LANGUAGE_PASHTO,                      "ps", "AF" },
    { LANGUAGE_OROMO,                       "om", "ET" },
    { LANGUAGE_DHIVEHI,                     "dv", "MV" },
    { LANGUAGE_UIGHUR_CHINA,                "ug", "CN" },
    { LANGUAGE_TIGRIGNA_ETHIOPIA,           "ti", "ET" },
    { LANGUAGE_TIGRIGNA_ERITREA,            "ti", "ER" },
    { LANGUAGE_AMHARIC_ETHIOPIA,            "am", "ET" },
    { LANGUAGE_GUARANI_PARAGUAY,            "gn", "PY" },
    { LANGUAGE_HAWAIIAN_UNITED_STATES,     "haw", "US" },
    { LANGUAGE_EDO,                        "bin", "NG" },
    { LANGUAGE_FULFULDE_NIGERIA,            "ff", "NG" },
    { LANGUAGE_HAUSA_NIGERIA,               "ha", "NG" },
    { LANGUAGE_IGBO_NIGERIA,                "ig", "NG" },
    { LANGUAGE_KANURI_NIGERIA,              "kr", "NG" },
    { LANGUAGE_YORUBA,                      "yo", "NG" },
    { LANGUAGE_SOMALI,                      "so", "SO" },
    { LANGUAGE_PAPIAMENTU,                 "pap", "AN" },
    { LANGUAGE_ENGLISH_SINGAPORE,           "en", "SG" },
    { LANGUAGE_YIDDISH,                     "yi", "IL" },   // new: old was "ji"
    { LANGUAGE_YIDDISH,                     "ji", "IL" },   // old: new is "yi"
    { LANGUAGE_SYRIAC,                     "syr", "TR" },   // "TR" according to http://www.ethnologue.com/show_language.asp?code=SYC
    { LANGUAGE_SINHALESE_SRI_LANKA,         "si", "LK" },
    { LANGUAGE_CHEROKEE_UNITED_STATES,     "chr", "US" },
    { LANGUAGE_INUKTITUT_LATIN_CANADA,      "iu", "CA" },
//  { LANGUAGE_INUKTITUT_SYLLABICS_CANADA,  "iu", "CA" },   // script codes not supported yet
//  { LANGUAGE_ARABIC_SUDAN,                "ar", "SD" },   // unknown MS-LCID
//  { LANGUAGE_SAMI_LAPPISH,               "smi", ""   },   // unknown ISO code, what's "Lapp"?
    { LANGUAGE_SAMI_INARI,                 "smn", "FI" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_SAMI_LULE_NORWAY,           "smj", "NO" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_SAMI_LULE_SWEDEN,           "smj", "SE" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_SAMI_NORTHERN_FINLAND,       "se", "FI" },
    { LANGUAGE_SAMI_NORTHERN_SWEDEN,        "se", "SE" },
    { LANGUAGE_SAMI_SKOLT,                 "sms", "FI" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_SAMI_SOUTHERN_NORWAY,       "sma", "NO" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_SAMI_SOUTHERN_SWEDEN,       "sma", "SE" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_MAPUDUNGUN_CHILE,           "arn", "CL" },
    { LANGUAGE_CORSICAN_FRANCE,             "co", "FR" },
    { LANGUAGE_ALSATIAN_FRANCE,            "gsw", "FR" },   // in fact 'gsw' is Schwyzerduetsch (Swiss German), which is a dialect of Alemannic German, as is Alsatian. They aren't distinct languages and share this code.
    { LANGUAGE_YAKUT_RUSSIA,               "sah", "RU" },
    { LANGUAGE_MOHAWK_CANADA,              "moh", "CA" },
    { LANGUAGE_BASHKIR_RUSSIA,              "ba", "RU" },
    { LANGUAGE_KICHE_GUATEMALA,            "qut", "GT" },
    { LANGUAGE_DARI_AFGHANISTAN,           "gbz", "AF" },   // ISO/DIS 639-3 (!)
    { LANGUAGE_WOLOF_SENEGAL,               "wo", "SN" },
    { LANGUAGE_FILIPINO,                   "fil", "PH" },
//  { LANGUAGE_IBIBIO_NIGERIA,             "nic", "NG" },   // ISO "nic" is only a collective language code
    { LANGUAGE_YI,                          "ii", "CN" },
//  { LANGUAGE_TAMAZIGHT_LATIN,            "ber", ""   },   // ISO "ber" only collective!
//  { LANGUAGE_TAMAZIGHT_ARABIC,           "ber", ""   },   // ISO "ber" only collective!
    { LANGUAGE_LATIN,                       "la", "VA" },
    { LANGUAGE_OBSOLETE_USER_LATIN,         "la", "VA" },
    { LANGUAGE_USER_ESPERANTO,              "eo", ""   },
    { LANGUAGE_USER_INTERLINGUA,            "ia", ""   },
    { LANGUAGE_MAORI_NEW_ZEALAND,           "mi", "NZ" },
    { LANGUAGE_OBSOLETE_USER_MAORI,         "mi", "NZ" },
    { LANGUAGE_KINYARWANDA_RWANDA,          "rw", "RW" },
    { LANGUAGE_OBSOLETE_USER_KINYARWANDA,   "rw", "RW" },
    { LANGUAGE_UPPER_SORBIAN_GERMANY,      "hsb", "DE" },   // MS maps this to 'wen-DE', which is nonsense. 'wen' is a collective language code, 'WEN' is a SIL code, see http://www.ethnologue.com/14/show_iso639.asp?code=wen and http://www.ethnologue.com/14/show_language.asp?code=WEN
    { LANGUAGE_OBSOLETE_USER_UPPER_SORBIAN,"hsb", "DE" },
    { LANGUAGE_LOWER_SORBIAN_GERMANY,      "dsb", "DE" },   // MS maps this to 'wee-DE', which is nonsense. 'WEE' is a SIL code, see http://www.ethnologue.com/14/show_language.asp?code=WEE
    { LANGUAGE_OBSOLETE_USER_LOWER_SORBIAN,"dsb", "DE" },
    { LANGUAGE_OCCITAN_FRANCE,              "oc", "FR" },
    { LANGUAGE_OBSOLETE_USER_OCCITAN,       "oc", "FR" },
    { LANGUAGE_USER_KURDISH_TURKEY,         "ku", "TR" },
    { LANGUAGE_USER_KURDISH_SYRIA,          "ku", "SY" },
    { LANGUAGE_USER_KURDISH_IRAQ,           "ku", "IQ" },
    { LANGUAGE_USER_KURDISH_IRAN,           "ku", "IR" },
    { LANGUAGE_USER_SARDINIAN,              "sc", "IT" },
    { LANGUAGE_BRETON_FRANCE,               "br", "FR" },
    { LANGUAGE_OBSOLETE_USER_BRETON,        "br", "FR" },
    { LANGUAGE_KALAALLISUT_GREENLAND,       "kl", "GL" },
    { LANGUAGE_OBSOLETE_USER_KALAALLISUT,   "kl", "GL" },
    { LANGUAGE_USER_SWAZI,                  "ss", "ZA" },
    { LANGUAGE_USER_NDEBELE_SOUTH,          "nr", "ZA" },
    { LANGUAGE_USER_TSWANA_BOTSWANA,        "tn", "BW" },
    { LANGUAGE_USER_MOORE,                 "mos", "BF" },
    { LANGUAGE_USER_BAMBARA,                "bm", "ML" },
    { LANGUAGE_USER_AKAN,                   "ak", "GH" },
    { LANGUAGE_LUXEMBOURGISH_LUXEMBOURG,    "lb", "LU" },
    { LANGUAGE_OBSOLETE_USER_LUXEMBOURGISH, "lb", "LU" },
    { LANGUAGE_USER_FRIULIAN,              "fur", "IT" },
    { LANGUAGE_USER_FIJIAN,                 "fj", "FJ" },
    { LANGUAGE_USER_AFRIKAANS_NAMIBIA,      "af", "NA" },
    { LANGUAGE_USER_ENGLISH_NAMIBIA,        "en", "NA" },
    { LANGUAGE_USER_WALLOON,                "wa", "BE" },
    { LANGUAGE_USER_COPTIC,                "cop", "EG" },
    { LANGUAGE_USER_GASCON,                "gsc", "FR" },
    { LANGUAGE_USER_GERMAN_BELGIUM,         "de", "BE" },
    { LANGUAGE_USER_CHUVASH,                "cv", "RU" },
    { LANGUAGE_NONE,                       "zxx", ""   },   // added to ISO 639-2 on 2006-01-11: Used to declare the absence of linguistic information
    { LANGUAGE_DONTKNOW,                    "",   ""   }    // marks end of table
};

// -----------------------------------------------------------------------

// In this table are the countries which should mapped to a specific
// english language
static IsoLangEngEntry const aImplIsoLangEngEntries[] =
{
    { LANGUAGE_ENGLISH_UK,                  "AO" },         // Angola
    { LANGUAGE_ENGLISH_UK,                  "BJ" },         // Benin
    { LANGUAGE_ENGLISH_UK,                  "BW" },         // Botswana
    { LANGUAGE_ENGLISH_UK,                  "BI" },         // Burundi
    { LANGUAGE_ENGLISH_UK,                  "CM" },         // Cameroon
    { LANGUAGE_ENGLISH_UK,                  "GA" },         // Gabon
    { LANGUAGE_ENGLISH_UK,                  "GM" },         // Gambia
    { LANGUAGE_ENGLISH_UK,                  "GH" },         // Ghana
    { LANGUAGE_ENGLISH_UK,                  "GN" },         // Guinea
    { LANGUAGE_ENGLISH_UK,                  "LS" },         // Lesotho
    { LANGUAGE_ENGLISH_UK,                  "MW" },         // Malawi
    { LANGUAGE_ENGLISH_UK,                  "MT" },         // Malta
    { LANGUAGE_ENGLISH_UK,                  "NA" },         // Namibia
    { LANGUAGE_ENGLISH_UK,                  "NG" },         // Nigeria
    { LANGUAGE_ENGLISH_UK,                  "UG" },         // Uganda
    { LANGUAGE_ENGLISH_UK,                  "ZM" },         // Zambia
    { LANGUAGE_ENGLISH_UK,                  "ZW" },         // Zimbabwe
    { LANGUAGE_ENGLISH_UK,                  "SZ" },         // Swaziland
    { LANGUAGE_ENGLISH_UK,                  "NG" },         // Sierra Leone
    { LANGUAGE_ENGLISH_UK,                  "KN" },         // Saint Kitts and Nevis
    { LANGUAGE_ENGLISH_UK,                  "SH" },         // St. Helena
    { LANGUAGE_ENGLISH_UK,                  "IO" },         // British Indian Oceanic Territory
    { LANGUAGE_ENGLISH_UK,                  "FK" },         // Falkland Islands
    { LANGUAGE_ENGLISH_UK,                  "GI" },         // Gibraltar
    { LANGUAGE_ENGLISH_UK,                  "KI" },         // Kiribati
    { LANGUAGE_ENGLISH_UK,                  "VG" },         // Virgin Islands
    { LANGUAGE_ENGLISH_UK,                  "MU" },         // Mauritius
    { LANGUAGE_ENGLISH_UK,                  "FJ" },         // Fiji
    { LANGUAGE_ENGLISH_US,                  "KI" },         // Kiribati
    { LANGUAGE_ENGLISH_US,                  "LR" },         // Liberia
    { LANGUAGE_ENGLISH_US,                  "GU" },         // Guam
    { LANGUAGE_ENGLISH_US,                  "MH" },         // Marshall Islands
    { LANGUAGE_ENGLISH_US,                  "PW" },         // Palau
    { LANGUAGE_ENGLISH_CARRIBEAN,           "AI" },         // Anguilla
    { LANGUAGE_ENGLISH_CARRIBEAN,           "AG" },         // Antigua and Barbuda
    { LANGUAGE_ENGLISH_CARRIBEAN,           "BS" },         // Bahamas
    { LANGUAGE_ENGLISH_CARRIBEAN,           "BB" },         // Barbedos
    { LANGUAGE_ENGLISH_CARRIBEAN,           "BM" },         // Bermuda
    { LANGUAGE_ENGLISH_CARRIBEAN,           "KY" },         // Cayman Islands
    { LANGUAGE_ENGLISH_CARRIBEAN,           "GD" },         // Grenada
    { LANGUAGE_ENGLISH_CARRIBEAN,           "DM" },         // Dominica
    { LANGUAGE_ENGLISH_CARRIBEAN,           "HT" },         // Haiti
    { LANGUAGE_ENGLISH_CARRIBEAN,           "MS" },         // Montserrat
    { LANGUAGE_ENGLISH_CARRIBEAN,           "FM" },         // Micronesia
    { LANGUAGE_ENGLISH_CARRIBEAN,           "VC" },         // St. Vincent / Grenadines
    { LANGUAGE_ENGLISH_CARRIBEAN,           "LC" },         // Saint Lucia
    { LANGUAGE_ENGLISH_CARRIBEAN,           "TC" },         // Turks & Caicos Islands
    { LANGUAGE_ENGLISH_CARRIBEAN,           "GY" },         // Guyana
    { LANGUAGE_ENGLISH_CARRIBEAN,           "TT" },         // Trinidad and Tobago
    { LANGUAGE_ENGLISH_AUS,                 "CX" },         // Christmas Islands
    { LANGUAGE_ENGLISH_AUS,                 "CC" },         // Cocos (Keeling) Islands
    { LANGUAGE_ENGLISH_AUS,                 "NF" },         // Norfolk Island
    { LANGUAGE_ENGLISH_AUS,                 "PG" },         // Papua New Guinea
    { LANGUAGE_ENGLISH_AUS,                 "SB" },         // Solomon Islands
    { LANGUAGE_ENGLISH_AUS,                 "TV" },         // Tuvalu
    { LANGUAGE_ENGLISH_AUS,                 "NR" },         // Nauru
    { LANGUAGE_ENGLISH_NZ,                  "CK" },         // Cook Islands
    { LANGUAGE_ENGLISH_NZ,                  "NU" },         // Niue
    { LANGUAGE_ENGLISH_NZ,                  "TK" },         // Tokelau
    { LANGUAGE_ENGLISH_NZ,                  "TO" },         // Tonga
    { LANGUAGE_DONTKNOW,                    ""   }          // marks end of table
};

// -----------------------------------------------------------------------

static IsoLangNoneStdEntry const aImplIsoNoneStdLangEntries[] =
{
    { LANGUAGE_NORWEGIAN_BOKMAL,            "no", "BOK"      }, // registered subtags for "no" in rfc1766
    { LANGUAGE_NORWEGIAN_NYNORSK,           "no", "NYN"      }, // registered subtags for "no" in rfc1766
    { LANGUAGE_SERBIAN_LATIN,               "sr", "latin"    },
    { LANGUAGE_SERBIAN_CYRILLIC,            "sr", "cyrillic" },
    { LANGUAGE_AZERI_LATIN,                 "az", "latin"    },
    { LANGUAGE_AZERI_CYRILLIC,              "az", "cyrillic" },
    { LANGUAGE_DONTKNOW,                    "",   ""         }  // marks end of table
};

// -----------------------------------------------------------------------

// in this table are only names to find the best language
static IsoLangNoneStdEntry const aImplIsoNoneStdLangEntries2[] =
{
    { LANGUAGE_NORWEGIAN_BOKMAL,            "no", "bokmaal"  },
    { LANGUAGE_NORWEGIAN_BOKMAL,            "no", "bokmal"   },
    { LANGUAGE_NORWEGIAN_NYNORSK,           "no", "nynorsk"  },
    { LANGUAGE_DONTKNOW,                    "",   ""         }  // marks end of table
};

// -----------------------------------------------------------------------

// in this table are only names to find the best language
static IsoLangOtherEntry const aImplOtherEntries[] =
{
    { LANGUAGE_ENGLISH_US,                  "c"              },
    { LANGUAGE_CHINESE,                     "chinese"        },
    { LANGUAGE_GERMAN,                      "german"         },
    { LANGUAGE_JAPANESE,                    "japanese"       },
    { LANGUAGE_KOREAN,                      "korean"         },
    { LANGUAGE_ENGLISH_US,                  "posix"          },
    { LANGUAGE_CHINESE_TRADITIONAL,         "tchinese"       },
    { LANGUAGE_DONTKNOW,                    NULL             }  // marks end of table
};

// =======================================================================

// static
void MsLangId::convertLanguageToIsoNames( LanguageType eLang,
        rtl::OUString& rLangStr, rtl::OUString& rCountry )
{
    if ( eLang == LANGUAGE_SYSTEM )
        eLang = MsLangId::getSystemLanguage();

    // Search for LangID (in this table we find only defined ISO combinations)
    const IsoLangEntry* pEntry = aImplIsoLangEntries;
    do
    {
        if ( pEntry->mnLang == eLang )
        {
            rLangStr = rtl::OUString::createFromAscii( pEntry->maLangStr );
            rCountry = rtl::OUString::createFromAscii( pEntry->maCountry );
            return;
        }
        ++pEntry;
    }
    while ( pEntry->mnLang != LANGUAGE_DONTKNOW );

    // Search for LangID if we didn't find a specific ISO combination.
    // All entries in this table are allowed for mime specifications,
    // but not defined ISO combinations.
    const IsoLangNoneStdEntry* pNoneStdEntry = aImplIsoNoneStdLangEntries;
    do
    {
        if ( pNoneStdEntry->mnLang == eLang )
        {
            rLangStr = rtl::OUString::createFromAscii( pNoneStdEntry->maLangStr );
            rCountry = rtl::OUString::createFromAscii( pNoneStdEntry->maCountry );
            return;
        }
        ++pNoneStdEntry;
    }
    while ( pNoneStdEntry->mnLang != LANGUAGE_DONTKNOW );

    // not found
    rLangStr = rtl::OUString();
    rCountry = rtl::OUString();
}

// -----------------------------------------------------------------------

// static
void MsLangId::convertLanguageToIsoNames( LanguageType eLang,
        rtl::OString& rLangStr, rtl::OString& rCountry )
{
    if ( eLang == LANGUAGE_SYSTEM )
        eLang = MsLangId::getSystemLanguage();

    // Search for LangID (in this table we find only defined ISO combinations)
    const IsoLangEntry* pEntry = aImplIsoLangEntries;
    do
    {
        if ( pEntry->mnLang == eLang )
        {
            rLangStr = rtl::OString( pEntry->maLangStr );
            rCountry = rtl::OString( pEntry->maCountry );
            return;
        }
        ++pEntry;
    }
    while ( pEntry->mnLang != LANGUAGE_DONTKNOW );

    // Search for LangID if we didn't find a specific ISO combination.
    // All entries in this table are allowed for mime specifications,
    // but not defined ISO combinations.
    const IsoLangNoneStdEntry* pNoneStdEntry = aImplIsoNoneStdLangEntries;
    do
    {
        if ( pNoneStdEntry->mnLang == eLang )
        {
            rLangStr = rtl::OString( pNoneStdEntry->maLangStr );
            rCountry = rtl::OString( pNoneStdEntry->maCountry );
            return;
        }
        ++pNoneStdEntry;
    }
    while ( pNoneStdEntry->mnLang != LANGUAGE_DONTKNOW );

    // not found
    rLangStr = rtl::OString();
    rCountry = rtl::OString();
}

// -----------------------------------------------------------------------

// static
rtl::OUString MsLangId::convertLanguageToIsoString( LanguageType eLang,
        sal_Unicode cSep )
{
    rtl::OUString   aLangStr;
    rtl::OUString   aCountry;
    convertLanguageToIsoNames( eLang, aLangStr, aCountry );
    if ( aCountry.getLength() )
    {
        rtl::OUStringBuffer aBuf( aLangStr);
        aBuf.append( cSep );
        aBuf.append( aCountry );
        return aBuf.makeStringAndClear();
    }
    else
        return aLangStr;
}

// -----------------------------------------------------------------------

// static
rtl::OString MsLangId::convertLanguageToIsoByteString( LanguageType eLang,
        sal_Char cSep )
{
    rtl::OString  aLangStr;
    rtl::OString  aCountry;
    convertLanguageToIsoNames( eLang, aLangStr, aCountry );
    if ( aCountry.getLength() )
    {
        rtl::OStringBuffer aBuf( aLangStr);
        aBuf.append( cSep );
        aBuf.append( aCountry );
        return aBuf.makeStringAndClear();
    }
    return aLangStr;
}

// =======================================================================

// static
LanguageType MsLangId::convertIsoNamesToLanguage( const rtl::OUString& rLang,
        const rtl::OUString& rCountry )
{
    // language is lower case in table
    rtl::OUString aLowerLang = rLang.toAsciiLowerCase();
    // country is upper case in table
    rtl::OUString aUpperCountry = rCountry.toAsciiUpperCase();

    //  first look for exact match
    const IsoLangEntry* pFirstLang = NULL;
    const IsoLangEntry* pEntry = aImplIsoLangEntries;
    do
    {
        if ( aLowerLang.equalsAscii( pEntry->maLangStr ) )
        {
            if ( !aUpperCountry.getLength() ||
                 aUpperCountry.equalsAscii( pEntry->maCountry ) )
                return pEntry->mnLang;
            if ( !pFirstLang )
                pFirstLang = pEntry;
            else if ( !*pEntry->maCountry )
                pFirstLang = pEntry;
        }
        ++pEntry;
    }
    while ( pEntry->mnLang != LANGUAGE_DONTKNOW );

    // some eng countries should be mapped to a specific english language
    if ( aLowerLang.equalsAscii( "en" ) )
    {
        const IsoLangEngEntry* pEngEntry = aImplIsoLangEngEntries;
        do
        {
            if ( aUpperCountry.equalsAscii( pEngEntry->maCountry ) )
                return pEngEntry->mnLang;
            ++pEngEntry;
        }
        while ( pEngEntry->mnLang != LANGUAGE_DONTKNOW );
    }

    // test for specific languages which are not used standard ISO 3166 codes
    const IsoLangNoneStdEntry* pNoneStdEntry = aImplIsoNoneStdLangEntries;
    do
    {
        if ( aLowerLang.equalsAscii( pNoneStdEntry->maLangStr ) )
        {
            // The countries in this table are not all in upper case
            if ( aUpperCountry.equalsIgnoreAsciiCaseAscii( pNoneStdEntry->maCountry ) )
                return pNoneStdEntry->mnLang;
        }
        ++pNoneStdEntry;
    }
    while ( pNoneStdEntry->mnLang != LANGUAGE_DONTKNOW );
    pNoneStdEntry = aImplIsoNoneStdLangEntries2;
    do
    {
        if ( aLowerLang.equalsAscii( pNoneStdEntry->maLangStr ) )
        {
            // The countries in this table are not all in upper case
            if ( aUpperCountry.equalsIgnoreAsciiCaseAscii( pNoneStdEntry->maCountry ) )
                return pNoneStdEntry->mnLang;
        }
        ++pNoneStdEntry;
    }
    while ( pNoneStdEntry->mnLang != LANGUAGE_DONTKNOW );

    // If the language is correct, than we return the default language
    if ( pFirstLang )
        return pFirstLang->mnLang;

    //  if only the country is set, look for any entry matching the country
    //  (to allow reading country and language in separate steps, in any order)
    if ( rCountry.getLength() && !rLang.getLength() )
    {
        const IsoLangEntry* pEntry2 = aImplIsoLangEntries;
        do
        {
            if ( aUpperCountry.equalsAscii( pEntry2->maCountry ) )
                return pEntry2->mnLang;
            ++pEntry2;
        }
        while ( pEntry2->mnLang != LANGUAGE_DONTKNOW );

        aLowerLang = aUpperCountry.toAsciiLowerCase();
    }

    // Now look for all other definitions, which are not standard
    const IsoLangOtherEntry* pOtherEntry = aImplOtherEntries;
    do
    {
        if ( aLowerLang.equalsAscii( pOtherEntry->mpLangStr ) )
            return pOtherEntry->mnLang;
        ++pOtherEntry;
    }
    while ( pOtherEntry->mnLang != LANGUAGE_DONTKNOW );

    return LANGUAGE_DONTKNOW;
}

// -----------------------------------------------------------------------

// static
LanguageType MsLangId::convertIsoNamesToLanguage( const rtl::OString& rLang,
        const rtl::OString& rCountry )
{
    rtl::OUString aLang = OStringToOUString( rLang, RTL_TEXTENCODING_ASCII_US);
    rtl::OUString aCountry = OStringToOUString( rCountry, RTL_TEXTENCODING_ASCII_US);
    return convertIsoNamesToLanguage( aLang, aCountry);
}

// -----------------------------------------------------------------------

// static
LanguageType MsLangId::convertIsoStringToLanguage(
        const rtl::OUString& rString, sal_Unicode cSep )
{
    rtl::OUString   aLang;
    rtl::OUString   aCountry;
    sal_Int32  nSepPos = rString.indexOf( cSep );
    if ( nSepPos >= 0 )
    {
        aLang = rString.copy( 0, nSepPos );
        aCountry = rString.copy( nSepPos+1 );
    }
    else
        aLang = rString;

    return convertIsoNamesToLanguage( aLang, aCountry );
}

// -----------------------------------------------------------------------

// static
LanguageType MsLangId::convertIsoByteStringToLanguage(
        const rtl::OString& rString, sal_Char cSep )
{
    rtl::OString  aLang;
    rtl::OString  aCountry;
    sal_Int32  nSepPos = rString.indexOf( cSep );
    if ( nSepPos >= 0 )
    {
        aLang = rString.copy( 0, nSepPos );
        aCountry = rString.copy( nSepPos+1 );
    }
    else
        aLang = rString;

    return convertIsoNamesToLanguage( aLang, aCountry );
}

// -----------------------------------------------------------------------
// convert a unix locale string into LanguageType

// static
LanguageType MsLangId::convertUnxByteStringToLanguage(
        const rtl::OString& rString )
{
    rtl::OString  aLang;
    rtl::OString  aCountry;

    sal_Int32  nLangSepPos    = rString.indexOf( (sal_Char)'_' );
    sal_Int32  nCountrySepPos = rString.indexOf( (sal_Char)'.' );

    if (nCountrySepPos < 0)
        nCountrySepPos = rString.indexOf( (sal_Char)'@' );
    if (nCountrySepPos < 0)
        nCountrySepPos = rString.getLength();

    if (   ((nLangSepPos >= 0) && (nLangSepPos > nCountrySepPos))
        || ((nLangSepPos < 0)) )
    {
        // eg. "el.sun_eu_greek", "tchinese", "es.ISO8859-15"
        aLang    = rString.copy( 0, nCountrySepPos );
    }
    else if ( nLangSepPos >= 0 )
    {
        // well formed iso names like "en_US.UTF-8", "sh_BA.ISO8859-2@bosnia"
        aLang    = rString.copy( 0, nLangSepPos );
        aCountry = rString.copy( nLangSepPos+1, nCountrySepPos - nLangSepPos - 1);
    }

    return convertIsoNamesToLanguage( aLang, aCountry );
}

// -----------------------------------------------------------------------
// pass one IsoLangEntry to the outer world of the resource compiler

// static
const MsLangId::IsoLangEntry* MsLangId::getIsoLangEntry( size_t nIndex )
{
    if (nIndex < sizeof( aImplIsoLangEntries) / sizeof( IsoLangEntry))
        return &aImplIsoLangEntries[ nIndex];
    return 0;
}
