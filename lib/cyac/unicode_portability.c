#ifndef __UNICODE_PORTABILITY_H__
#define __UNICODE_PORTABILITY_H__
#include <Python.h>

#if PY_MAJOR_VERSION < 3
extern "C" {
    static int _PyUnicode_ToLowerFull(Py_UCS4 ch, Py_UCS4* res){
        res[0] = _PyUnicode_ToLowercase(ch);
        return 1;
    }
}
#elif PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 13
typedef struct {
    /*
       These are either deltas to the character or offsets in
       _PyUnicode_ExtendedCase.
    */
    const int upper;
    const int lower;
    const int title;
    /* Note if more flag space is needed, decimal and digit could be unified. */
    const unsigned char decimal;
    const unsigned char digit;
    const unsigned short flags;
} _PyUnicode_TypeRecord;

#define EXTENDED_CASE_MASK 0x4000
#include "./unicodetype_db.h"
static const _PyUnicode_TypeRecord *
gettyperecord(Py_UCS4 code)
{
    int index;

    if (code >= 0x110000)
        index = 0;
    else
    {
        index = index1[(code>>SHIFT)];
        index = index2[(index<<SHIFT)+(code&((1<<SHIFT)-1))];
    }

    return &_PyUnicode_TypeRecords[index];
}

static int _PyUnicode_ToLowerFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicode_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->lower & 0xFFFF;
        int n = ctype->lower >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicode_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->lower;
    return 1;
}
#endif
#endif
