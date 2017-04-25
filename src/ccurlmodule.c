#include <Python.h>
#include <stdint.h>

#define HASH_LENGTH 243
#define NUMBER_OF_ROUNDS 27
#define STATE_LENGTH 3 * HASH_LENGTH

// For consistency with the Curl c library, each trit gets 64 bits.
// In future versions of the software, this will yield significant
// speedups because we can compute multiple hashes concurrently.
typedef int64_t trit_t;

// Copied from https://github.com/iotaledger/ccurl/blob/master/src/lib/Curl.c
#define __TRUTH_TABLE	\
   1,  0, -1, \
   1, -1,  0, \
  -1,  1,  0

static const trit_t TRUTH_TABLE[9] = {__TRUTH_TABLE};

typedef struct {
  PyObject_HEAD
  trit_t _state[STATE_LENGTH];
} Curl;

static PyObject*
Curl_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  Curl *self;

  self = (Curl*)type->tp_alloc(type, 0);
  if (self != NULL) {
    memset(self->_state, 0, STATE_LENGTH * sizeof(trit_t));
  }

  return (PyObject*)self;
}

static void
_Curl_transform(Curl *self)
{
  // Adapted from https://github.com/iotaledger/ccurl/blob/master/src/lib/Curl.c
  trit_t scratchpad[STATE_LENGTH];
  int round, scratchpadIndex=0, scratchpadIndexSave, stateIndex;

  for (round = 0; round < NUMBER_OF_ROUNDS; round++) {
    memcpy(scratchpad, self->_state, STATE_LENGTH * sizeof(trit_t));

    for (stateIndex = 0; stateIndex < STATE_LENGTH; stateIndex++) {
      scratchpadIndexSave = scratchpadIndex;
      scratchpadIndex += (scratchpadIndex < 365 ? 364 : -365);
      self->_state[stateIndex] = TRUTH_TABLE[scratchpad[scratchpadIndexSave ] + scratchpad[scratchpadIndex ] * 3 + 4];
    }
  }
}

static PyObject*
Curl_absorb(Curl *self, PyObject *args, PyObject *kwds)
{
  PyObject *incoming, *incoming_item;
  int i, incoming_count, offset=0;
  trit_t incoming_value;
  trit_t *trits;

  static char *kwlist[] = {"trits", NULL};

  // Extract and validate parameters.
  // Based on https://github.com/alfredch/iotaPy-Extension/blob/master/iotaPy.c
  if (! PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &incoming))
    return NULL;

  if (! PyList_Check(incoming)) {
    PyErr_SetString(PyExc_TypeError, "`trits` argument must be a list.");
    return NULL;
  }

  // Check for invalid values in ``incoming``.
  incoming_count = PyList_Size(incoming);
  trits = (trit_t*)malloc(incoming_count * sizeof(trit_t));

  for (i=0; i < incoming_count; i++) {
    incoming_item = PyList_GetItem(incoming, i);

    if ((incoming_item == NULL) || ! PyLong_Check(incoming_item)) {
      PyErr_Format(PyExc_ValueError, "`trits` argument contains non-numeric value at index %u.", i);
      return NULL;
    }

    incoming_value = (trit_t)PyLong_AsLong(incoming_item);
    if ((incoming_value < -1) || (incoming_value > 1)) {
      PyErr_Format(PyExc_ValueError, "`trits` argument contains value outside range [-1, 1] at index %u.", i);
      return NULL;
    }

    trits[i] = incoming_value;
  }

  // Copy values to state and transform.
  // Adapted from https://github.com/iotaledger/ccurl/blob/master/src/lib/Curl.c
  do {
    memcpy(self->_state, trits+offset, (incoming_count < HASH_LENGTH ? incoming_count : HASH_LENGTH ) * sizeof(trit_t));
    _Curl_transform(self);
    offset += HASH_LENGTH;
  } while ((incoming_count -= HASH_LENGTH) > 0);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
Curl_squeeze(Curl *self, PyObject *args, PyObject *kwds)
{
  PyObject *incoming;
  int i, incoming_count;

  static char *kwlist[] = {"trits", NULL};

  // Extract and validate parameters.
  if (! PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &incoming))
    return NULL;

  if (! PyList_Check(incoming)) {
    PyErr_SetString(PyExc_TypeError, "`trits` argument must be a list.");
    return NULL;
  }

  // Ensure that ``incoming`` can hold at least 1 hash worth of trits.
  for(incoming_count = PyList_Size(incoming); incoming_count < HASH_LENGTH; incoming_count++) {
    PyList_Append(incoming, PyLong_FromLong(0));
  }

  // Adapted from https://github.com/iotaledger/ccurl/blob/master/src/lib/Curl.c
  // Note that squeeze only copies one hash.

  // Can't use ``memcpy`` here because we have to convert ints into ``PyLongObject``.
  // This isn't the slow part of Curl (that honor is reserved for ``_Curl_transform``),
  // so it shouldn't be that big of a problem.
  for (i=0; i < HASH_LENGTH; i++) {
    PyList_SetItem(incoming, i, PyLong_FromLong(self->_state[i]));
  }

  _Curl_transform(self);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
Curl_reset(Curl *self)
{
  memset(self->_state, 0, STATE_LENGTH * sizeof(trit_t));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef Curl_methods[] = {
  {"absorb", (PyCFunction)Curl_absorb, METH_VARARGS|METH_KEYWORDS, "Absorb trits into the sponge."},
  {"reset", (PyCFunction)Curl_reset, METH_NOARGS, "Resets internal state."},
  {"squeeze", (PyCFunction)Curl_squeeze, METH_VARARGS|METH_KEYWORDS, "Squeeze trits from the sponge."},
  {NULL} /* Sentinel */
};

static PyTypeObject ccurl_CurlType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "ccurl.Curl",       /* tp_name */
  sizeof(Curl),       /* tp_basicsize */
  0,                  /* tp_itemsize */
  0,                  /* tp_dealloc */
  0,                  /* tp_print */
  0,                  /* tp_getattr */
  0,                  /* tp_setattr */
  0,                  /* tp_as_async */
  0,                  /* tp_repr */
  0,                  /* tp_as_number */
  0,                  /* tp_as_sequence */
  0,                  /* tp_as_mapping */
  0,                  /* tp_hash  */
  0,                  /* tp_call */
  0,                  /* tp_str */
  0,                  /* tp_getattro */
  0,                  /* tp_setattro */
  0,                  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT, /* tp_flags */
  "Curl",             /* tp_doc */
  0,                  /* tp_traverse */
  0,                  /* tp_clear */
  0,                  /* tp_richcompare */
  0,                  /* tp_weaklistoffset */
  0,                  /* tp_iter */
  0,                  /* tp_iternext */
  Curl_methods,       /* tp_methods */
  0,                  /* tp_members */
  0,                  /* tp_getset */
  0,                  /* tp_base */
  0,                  /* tp_dict */
  0,                  /* tp_descr_get */
  0,                  /* tp_descr_set */
  0,                  /* tp_dictoffset */
  0,                  /* tp_init */
  0,                  /* tp_alloc */
  Curl_new,           /* tp_new */
};

static PyModuleDef ccurlmodule = {
  PyModuleDef_HEAD_INIT,
  "ccurl",
  "C implementation of IOTA curl functionality.",
  -1,
  NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_ccurl(void)
{
  PyObject *m, *hash_length;

  ccurl_CurlType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&ccurl_CurlType) < 0)
    return NULL;

  m = PyModule_Create(&ccurlmodule);
  if (m == NULL)
    return NULL;

  hash_length = Py_BuildValue("i", HASH_LENGTH);
  if (hash_length == NULL)
    return NULL;

  // Define module-level symbols, named for compatibility with
  // pycurl module in PyOTA library.
  Py_INCREF(&ccurl_CurlType);
  PyModule_AddObject(m, "Curl", (PyObject*)&ccurl_CurlType);

  Py_INCREF(hash_length);
  PyModule_AddObject(m, "HASH_LENGTH", hash_length);

  return m;
}

