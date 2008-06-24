/***************************************************************************
 *
 * iconv.cpp - Win32 implementation of the POSIX iconv facility
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifdef _WIN32

#include <errno.h>
#include <windows.h>


typedef int iconv_t;

iconv_t iconv_open (const char*, const char*);
size_t  iconv (iconv_t, char**, size_t*, char**, size_t*);
int     iconv_close (iconv_t);


iconv_t iconv_open (const char *from_code, const char *to_code)
{
    static const struct {
        int         code;
        const char *name;
    } pages[] = {
        {    37, "EBCDIC-US" },
        {   437, "OEM - United States" },
        {   500, "IBM EBCDIC - International" },
        {   708, "Arabic - ASMO 708" },
        {   709, "Arabic - ASMO 449+, BCON V4" },
        {   710, "Arabic - Transparent Arabic" },
        {   720, "Arabic - Transparent ASMO" },
        {   737, "OEM - Greek (formerly 437G)" },
        {   775, "OEM - Baltic" },
        {   850, "OEM - Multilingual Latin I" },
        {   852, "OEM - Latin II" },
        {   855, "OEM - Cyrillic (primarily Russian)" },
        {   857, "OEM - Turkish" },
        {   858, "OEM - Multlingual Latin I + Euro symbol" },
        {   860, "OEM - Portuguese" },
        {   861, "OEM - Icelandic" },
        {   862, "OEM - Hebrew" },
        {   863, "OEM - Canadian-French" },
        {   864, "OEM - Arabic" },
        {   865, "OEM - Nordic" },
        {   866, "OEM - Russian" },
        {   869, "OEM - Modern Greek" },
        {   870, "IBM EBCDIC - Multilingual/ROECE (Latin-2)" },
        {   874, "ANSI/OEM - Thai (same as 28605, ISO 8859-15)" },
        {   875, "IBM EBCDIC - Modern Greek" },
        {   932, "Shift_JIS" },
        {   936, "ANSI/OEM - Simplified Chinese (PRC, Singapore)" },
        {   949, "ANSI/OEM - Korean (Unified Hangeul Code)" },
        {   950, "ANSI/OEM - Traditional Chinese" },
        {  1026, "IBM EBCDIC - Turkish (Latin-5)" },
        {  1047, "IBM EBCDIC - Latin 1/Open System" },
        {  1140, "EBCDIC-CA-FR@EURO" },
        {  1141, "EBCDIC-AT-DE@EURO" },
        {  1142, "EBCDIC-DK-NO@EURO" },
        {  1143, "EBCDIC-FI-SE@EURO" },
        {  1144, "EBCDIC-IT@EURO" },
        {  1145, "EBCDIC-ES-A@EURO" },
        {  1146, "EBCDIC-UK@EURO" },
        {  1147, "EBCDIC-FR@EURO" },
        {  1148, "IBM EBCDIC - International (500 + Euro symbol)" },
        {  1149, "EBCDIC-IS-FRISS@EURO" },
        {  1200, "UCS-2-LE" },
        {  1201, "UCS-2-BE" },
        {  1251, "ANSI - Cyrillic" },
        {  1252, "ANSI - Latin I" },
        {  1253, "ANSI - Greek" },
        {  1254, "ANSI - Turkish" },
        {  1255, "ANSI - Hebrew" },
        {  1256, "ANSI - Arabic" },
        {  1257, "ANSI - Baltic" },
        {  1258, "ANSI/OEM - Vietnamese" },
        {  1361, "Korean (Johab)" },
        { 10000, "MAC - Roman" },
        { 10001, "MAC-JP" },
        { 10002, "MAC - Traditional Chinese (Big5)" },
        { 10003, "MAC-KR" },
        { 10004, "MAC-AR" },
        { 10005, "MAC - Hebrew" },
        { 10006, "MAC - Greek I" },
        { 10007, "MAC-CYRILLIC" },
        { 10008, "MAC - Simplified Chinese (GB 2312)" },
        { 10010, "MAC - Romania" },
        { 10017, "MAC - Ukraine" },
        { 10021, "MAC - Thai" },
        { 10029, "MAC - Latin II" },
        { 10079, "MAC - Icelandic" },
        { 10081, "MAC - Turkish" },
        { 10082, "MAC - Croatia" },
        { 12000, "UCS-4-LE" },
        { 12001, "UCS-4-BE" },
        { 20000, "CNS - Taiwan" },
        { 20001, "TCA - Taiwan" },
        { 20002, "Eten - Taiwan" },
        { 20003, "IBM5550 - Taiwan" },
        { 20004, "TeleText - Taiwan" },
        { 20005, "Wang - Taiwan" },
        { 20105, "IA5 IRV International Alphabet No. 5 (7-bit)" },
        { 20106, "IA5 German (7-bit)" },
        { 20107, "IA5 Swedish (7-bit)" },
        { 20108, "IA5 Norwegian (7-bit)" },
        { 20127, "ANSI_X3.4-1968" },
        { 20261, "T.61" },
        { 20269, "ISO 6937 Non-Spacing Accent" },
        { 20273, "EBCDIC-DE" },
        { 20277, "EBCDIC-DK-NO" },
        { 20278, "EBCDIC-FI-SE" },
        { 20280, "EBCDIC-IT" },
        { 20284, "EBCDIC-ES-A" },
        { 20285, "EBCDIC-UK" },
        { 20290, "EBCDIC-JP" },
        { 20297, "EBCDIC-FR" },
        { 20420, "EBCDIC-AR" },
        { 20423, "IBM EBCDIC - Greek" },
        { 20424, "IBM EBCDIC - Hebrew" },
        { 20833, "EBCDIC-KR" },
        { 20838, "IBM EBCDIC - Thai" },
        { 20866, "Russian - KOI8-R" },
        { 20871, "IBM EBCDIC - Icelandic" },
        { 20880, "IBM EBCDIC - Cyrillic (Russian)" },
        { 20905, "IBM EBCDIC - Turkish" },
        { 20924, "IBM EBCDIC - Latin-1/Open System (1047 + Euro symbol)" },
        { 20932, "JIS X 0208-1990 & 0121-1990" },
        { 20936, "GB2312" },
        { 21025, "IBM EBCDIC - Cyrillic (Serbian, Bulgarian)" },
        { 21027, "Extended Alpha Lowercase" },
        { 21866, "Ukrainian (KOI8-U)" },
        { 28591, "ISO-8859-1" },
        { 28592, "ISO-8859-2" },
        { 28593, "ISO-8859-3" },
        { 28594, "ISO-8859-4" },
        { 28595, "ISO-8859-5" },
        { 28596, "ISO-8859-6" },
        { 28597, "ISO-8859-7" },
        { 28598, "ISO-8859-8" },
        { 28599, "ISO-8859-95" },
        { 28605, "ISO-8859-15" },
        { 29001, "Europa 3" },
        { 38598, "ISO-8859-8 Hebrew" },
        { 50220, "ISO-2022 Japanese with no halfwidth Katakana" },
        { 50221, "ISO-2022-JP" },
        { 50222, "ISO-2022 Japanese JIS X 0201-1989" },
        { 50225, "ISO-2022-KR" },
        { 50227, "ISO-2022 Simplified Chinese" },
        { 50229, "ISO-2022 Traditional Chinese" },
        { 50930, "Japanese (Katakana) Extended" },
        { 50931, "US/Canada and Japanese" },
        { 50933, "Korean Extended and Korean" },
        { 50935, "Simplified Chinese Extended and Simplified Chinese" },
        { 50936, "Simplified Chinese" },
        { 50937, "US/Canada and Traditional Chinese" },
        { 50939, "Japanese (Latin) Extended and Japanese" },
        { 51932, "EUC-JP" },
        { 51936, "EUC - Simplified Chinese" },
        { 51949, "EUC-KR" },
        { 51950, "EUC - Traditional Chinese" },
        { 52936, "HZ-GB2312 Simplified Chinese" },
        { 54936, "Windows XP: GB18030 Simplified Chinese (4 Byte)" },
        { 57002, "ISCII Devanagari 57003 ISCII Bengali" },
        { 57004, "ISCII Tamil" },
        { 57005, "ISCII Telugu" },
        { 57006, "ISCII Assamese" },
        { 57007, "ISCII Oriya" },
        { 57008, "ISCII Kannada" },
        { 57009, "ISCII Malayalam" },
        { 57010, "ISCII Gujarati" },
        { 57011, "ISCII Punjabi" },
        { 65000, "UTF-7" },
        { 65001, "UTF-8" }
    };

    iconv_t cd = 0;

    for (size_t i = 0; ; ++i) {

        if (i == sizeof pages / sizeof *pages) {
            cd = -1;
            break;
        }

        if (!strcmp (from_code, pages [i].name)) {
            cd |= pages [i].code << 16;

            if (cd & 0x0000ffff)
                break;
        }

        if (!strcmp (to_code, pages [i].name)) {
            cd |= pages [i].code & 0xffff;

            if (cd & 0xffff0000)
                break;
        }
    }

    // validate code pages
    if (   -1 == cd
        || !MultiByteToWideChar (cd & 0x0000ffff, 0, "", 1, 0, 0)
        || !MultiByteToWideChar ((cd >> 16) & 0x0000ffff, 0, "", 1, 0, 0)) {
        // [EINVAL]
        //   The conversion specified by fromcode and tocode
        //   is not supported by the implementation.
        errno = EINVAL;
        cd = -1;
    }

    return cd;
}


size_t iconv (iconv_t cd,
              char **inbuf, size_t *inbytesleft,
              char **outbuf, size_t *outbytesleft)
{
    const int fromcode = (cd >> 16) & 0x0000ffff;
    const int tocode   = cd & 0x0000ffff;

    if (0x0000ffff == fromcode || 0x0000ffff == tocode) {
        errno = EBADF;
        return size_t (-1);
    }

    wchar_t wbuf [256];

    // allocate a sufficient amount of storage to conver the input
    // buffer to wide character assuming, pessimistically, that
    // each byte converts to one wide character
    wchar_t *pwc = *inbytesleft < sizeof wbuf / sizeof *wbuf ?
        wbuf : new wchar_t [*inbytesleft];

    // convert the contents of the narrow input buffer to wide characters
    // `nwout' -- number of wide chars successfully produced by the call
    const int nwout =
        MultiByteToWideChar (fromcode, MB_ERR_INVALID_CHARS,
                             *inbuf, int (*inbytesleft),
                             pwc, int (*inbytesleft));

    if (!nwout) {
        if (pwc != wbuf)
            delete[] pwc;

        const int error = GetLastError ();
        errno = ERROR_INSUFFICIENT_BUFFER == error || !error ? E2BIG : EILSEQ;
        return size_t (-1);
    }

    // convert the contents of wide character buffer into the narrow
    // character buffer
    // `nnout' -- number of narrow chars successfully produced by the
    // call
    const int nnout =
        WideCharToMultiByte (tocode, 0,
                             pwc, nwout,
                             *outbuf, int (*outbytesleft),
                             0, 0);
    if (!nnout || !*outbytesleft) {
        if (pwc != wbuf)
            delete[] pwc;

        const int error = GetLastError ();
        errno = ERROR_INSUFFICIENT_BUFFER == error || !error ? E2BIG : EILSEQ;
        return size_t (-1);
    }

    // compute the number of wide characters consumed by second
    // conversion
    // `nwin' -- number of wide chars consumed by the call above
    const int nwin =
        MultiByteToWideChar (tocode, 0, *outbuf, nnout, 0, 0);

    if (!nwin) {
        if (pwc != wbuf)
            delete[] pwc;

        const int error = GetLastError ();
        errno = ERROR_INSUFFICIENT_BUFFER == error || !error ? E2BIG : EILSEQ;
        return size_t (-1);
    }

    // finally, compute the number of narrow characters in the source
    // encoding corresponding to the number of narrow characters in
    // the destrination encoding
    // `nnin' -- number of narrow chars consumed by the first call
    const int nnin =
        WideCharToMultiByte (fromcode, 0, pwc, nwin, 0, 0, 0, 0);
    if (pwc != wbuf)
        delete[] pwc;

    if (!nnin) {
        const int error = GetLastError ();
        errno = ERROR_INSUFFICIENT_BUFFER == error || !error ? E2BIG : EILSEQ;
        return size_t (-1);
    }

    // advance buffers to the first character to convert
    *inbuf  += nnin;
    *outbuf += nnout;

    // decrement the size of each buffer
    *inbytesleft  -= nnin;
    *outbytesleft -= nnout;

    if (*inbytesleft) {
        errno = E2BIG;
        return size_t (-1);
    }

    return 0;
}


int iconv_close (iconv_t cd)
{
    const int fromcode = (cd >> 16) & 0x0000ffff;
    const int tocode   = cd & 0x0000ffff;

    if (0x0000ffff == fromcode || 0x0000ffff == tocode) {
        errno = EBADF;
        return -1;
    }

    return 0;
}

#endif   // _WIN32
